/*===========================================================================
                        R R C   C S P   U T I L
DESCRIPTION

   This file contains Utility functions for Cell Selection Procedure. These
   include the following categories -
    - Functions to evaluate cell restrictions.
    - Function to initialize Internal Data.
    - Initial and Stored Cell Selection Controller.
    - Function to initialize Selection Data.
    - Functions to fill Acq Entry from SIB.
    - Functions to send cmds to L1 (only those relevant to CSP).
    - Function to send cmds to SIB (Only those relevant to CSP)
    - Functions to send cmds to MM (Only those relevant to CSP)

   It implements part of the PLMN Search mechanism. The implemented
   blocks in PLMN Search mechanism are marked with a "=======" boundary below.

              CSP PLMN SEARCH MECHANISM
              
       -----------   ============    -------------    ---------
       |         |   | CSP      |    | Acq DB    |    | Acq   |
       |CSP      |   | Initial  |    | Access    |    | DB    |
       |State    |   | And .    |<-->| Functions |<-->|       |
       |Machine  |   | Stored   |    | -PLMN     |    |       |
       |Functions|<->|Selection |    | -Acq type |    |       |
       |         |   |Controller|    -------------    ---------
       |         |   |-PLMN     |    
       |         |   |-PLMN     |    ------------
       -----------   | type     |<-->| CSP      |
            |        ============    | Frequency|
            |           |            | Scan     |
            |           |            | Algorithm| 
       ===========      |            ------------             
       |Internal |   ============       
       |  Data   |   |Selection |
       |         |   |          |
       ===========   |Data      |
                     ============

EXTERNALIZED FUNCTIONS   
  rrc_csp_find_cell_bar_status()
    This function is responsible for finding the status of a cell in terms of
    Cell Barred Status.

  rrc_csp_find_cell_access()
    This function is responsible for finding the status of Access Control due
    to Access Class Restrictions.

  rrc_csp_init_int_data()
    This function is responsible for initializing the internal data structure
    to default values.

  rrc_csp_continue_stored_initial_select_ctrl()
    This function continues with the already started Initial and Stored Cell 
    Selection. It looks for a frequency for a "specific" PLMN or "Any" PLMN.

  rrc_csp_start_cell_selection()
    This function is responsible for starting a New Stored or Initial Cell 
    Selection.

  rrc_csp_init_select_data()
    This function is responsible for initializing the internal Search data 
    structure to default values.

  rrc_csp_compare_expected_cell()
    This function compares the expected acq. type with the actual Acquisition
    status mask and PLMN status mask and declares the actual cell as Better
    than/Same as/Below the expected cell.

  rrc_csp_send_l1_cmd()
    This function is responsible for filling up data structures
    for all L1 commands sent by CSP. This also allocates cmd
    buffer and sends the command.

  rrc_csp_fill_acq_entry_from_sib()
    This function is responsible for filling up all fields of Acq Entry from 
    SIB data and SIB Confirmation command data.

  rrc_csp_send_sib_cmd()
    This function is responsible for allocating and filling up data structures
    for all RRC Internal commands to SIB procedure sent by CSP. It also 
    sends these cmds to SIB.

  rrc_csp_gen_status_mask()
    This function generates an Acq. status mask from Cell Barred status and 
    LAI Status.

  rrc_csp_send_mm_cmd()
    This function is responsible for filling up data structures for all 
    commands to MM. It further enqueues the command for MM.

  rrc_csp_send_mm_act_cnf_cmd()
    This function is responsible for filling up data structures for Activation 
    Confirm command and sending it to MM.

  rrc_csp_send_ccm_cmd()
    This function is responsible for filling up data structures for Cell 
    Change Request and sending it to CCM.

  rrccsp_get_serving_pccpch_parms()
    This function fills up STTD indicator and scrambling code for serving cell 
    PCCPCH.

  rrc_csp_send_next_freq_to_l1()
    This function sends either an Acquisition Request or a 
    Frequency scan request to L1 (via LLC) based on the passed
    type of frequency scan.

  rrc_csp_handle_no_available_freq()
    This function handles failure to find any frequency for acquisition.

  rrc_csp_send_chan_release()
    Sends an RRC_CHANNEL_CONFIG_REQ to LLC to release all channels.

  rrc_csp_start_sel_leaving_conn_mode()
    Starts cell selection on leaving connected mode. It starts with the
    last camped on cell.

  rrc_csp_cmd_to_mm_needed()
    This function determines if there is a need to send a 
    command to MM. If a command is needed to be sent, then
    which command is needed is also determined.

  rrc_csp_mm_primitive_sent()
    This function determines the CSP event based on the MM
    command and service status that was sent to MM. 

  rrc_csp_init_data_from_service_req()
    This function stores the data passed in RRC_SERVICE_REQ.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccsputil.c_v   1.51   19 Jun 2002 11:02:34   bumatt  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrccsputil.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/16   sr      Changes to update cell status properly for network sharing scenarios
07/05/16   sn      Changes for LTE U feature
06/24/16   sr      Made changes to not acquire lock for WTOG BPLMN in RRC
01/12/16   sn      Changes for FR30881
05/11/16   vs      Changes to cleanup unnecessary f3s
09/14/15   sr      Made changes to get default band from RF api for lock request
09/03/15   sas     New argument added in rrccsp_check_plmn_match() to update ACQDB on need purpose.
07/24/15   bc      Made changes to convert background PLMN search to foreground when OOS is received with no lock.
06/25/15   bc      Changes to read SIBs across DRX for CGI scan.
06/12/15   sas     Fetch the sibs using PLMN and cell_id for sending cell_param_req to L1 during cell selection.
04/24/15   sas     Update ACQ-DB when SIB reading is skipped for VPLMN.
03/16/15   sn      Send idle request on receving DL WK indication with no lock
02/26/15   bc      Changes to add all the shared PLMNs of the current camped cell to the list to be
                   sent to NAS during Manual PLMN search.
02/20/15   vi      Made changes to use rrc_send_lte_cmd instead of msgr_send to send as-id info
02/12/15   sa      Made changes to support QTF with JO CRM
02/03/14   vi      Made changes to send req PLMN to LTE if deprio list is valid 
01/28/14   bc      Changes to try ACQ on the frequency provided in connection release during leaving
                   connected mode scan when rplmn info is present in connection release.
12/30/14   sas     Removed redundant code in rrc_csp_init_int_data() for var 'ue_in_conn_oos' reset
12/16/14   bc      Changes to release lock on ACQ failure during GTOW redirection if there are no more frequencies to be scanned 
                   in the list scan frequency results.
12/04/14   sn      Changes to send idle request before PN acq
12/2/14    vi      Added NV to half the DRX length.
10/31/14   bc      Changes to support additional requirements in FR 20971.
11/27/14   sr      NV control for FR23776
11/14/14   sr      Code reduction to remove GET_ALL_SIBS handling
10/22/14   gv      Made changes to have connected mode feature set independent of TA status 
10/17/14   sn      QTA during SIB read changes
10/17/14   sr      CSFB call optimization changes
09/30/14   bc      Made changes to activate gsm bands according to bplmn band preference
                   while converting wtog background plmn search to foreground.
10/09/14   sr      Made changes to acquire lock for GTOW redirection in RRC 
10/06/14   sr      Made changes to not start ASF timer if CSG is not supported
10/03/14   sn      Changes to not query wl1 for leaving connected mode scan trigerred by RCR
                   with redirection information
09/29/14   sg      Made changes to check for upper layer band capability
                   also when indicating DBDC capability
09/12/14   bc      Made changes to buffer the Manual PLMN search request received during state transitions.
09/16/14   rmn     Resolved compilation errors in WRRC for LTE removal flavor
09/08/14   sn      Changes to initialize prioritized band list at the start of full band scan
09/08/14   bc      Made changes to access BPLMN PLMN list array for GTOW BPLMN search while trying to check
                   if the PLMN is already added to the List to be sent to NAS.
09/05/14   vi      Made changes to pass PN pos always for leaving connected mode scan
08/22/14   bc      Made changes to set plmn_fnd_during_fgnd_svc_srch to TRUE when a cell belonging to 
                   Forbidden LA is found.
08/04/14   vi      Made changes to avoid compiler warnings
08/08/14   sr      Made changes to pass the bandclass by derving from UARFCN during acq db scan
08/07/14   sr      Made changes to send dummy service ind and move to idle SIB DB 
08/08/14   vi      Made changes to avoid compiler warnings
07/28/14   bc      Made changes to reduce DCH to FACH cell selection time.
07/23/14   sp      Made changes to remove F3s as part of FR19999
07/23/14   bc      Made changes to camp on EHPLMN when NAS requested PLMN is not available.
07/22/14   sn      Use threshold of -103 for list scans only when G2W TA is active 
07/21/14   sn      f3 reduction changes
07/17/14   vi      Made changes to add WtoW and WtoG Redirection events
07/15/14   sg      Made changes to add initialization of local variables and bound conditions 
                   to access array to avoid warnings and invalid memory access. 
07/14/14   rmn     Made changes to attempt trm_exchange for all XtoW IRAT scenarios 
07/11/14   rmn     Made changes to move rrc_wrm_acquire_lock_on_redirected_freq() under FEATURE_DUAL_SIM
07/04/13   bc      Made changes to not to prematurely end xtow plmn search in DSDS mode.
07/08/14   rmn     Made changes so that WRRC will skip redirection attempt on N/W directed barred frequency
07/03/14   bc      Made changes to use the correct variable to find out the number
                   of PLMNs found till then.
06/27/14   rmn     Made changes to use new enum provided by PM used for advertising 
                   customer requested UE category when in its N/W
06/26/14   sn      WRRC changes for FR20484
06/25/14   gv      Made changes to allow ACQ DB only scan during WRLF
06/23/14   sr      Made changes to unbar frequency info given in IE "redirection info" for 
                   connection reject and connection release OTA
06/23/14   rmn     When calling new WRM lock request API, WRRC should indicate lock requested 
                   bands with type 'sys_band_class_e_type'
06/23/14   gv      Made changes to prevent CSP substate from getting reset if there is a pending 
                   MODE CHANGE after WCDMA stack is stopped
06/19/14   bc      Added new service request cause RRC_MO_CS_ECALL and its handling.
06/19/14   rmn     Made changes for DR-DSDS Feature
06/17/14   rmn     Made changes so that after Cell Reselection completion in PCH State,
                   Lock will only be released if CU is not active
06/17/14   rmn     Made changes to indicate resel fail cause in CPHY_CELL_TRANSITION_REQ
05/26/14   db      Made changes to include the correct diverstiy value when the optiona IE 
                   'Primary PCCPCH info'is omitted
05/20/14   rmn     Made changes to save DS Tuneaway Status Change indication in connected state based on the concurrency NV
05/20/14   rmn     Made changes to initialize CSG ID stored in internal data structure
05/16/14   vi      Made changes to support MOB framework
05/10/14   bc      Made changes to initialize sibproc events when a new service request is 
                   received from NAS.
05/08/14   rmn     WRRC support for new search type SYS_SERVICE_SEARCH_PRIORITY_PLMN
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/07/14   sr      Made changes to allow manual PLMN during WRLF
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
04/01/14   vi       Fixed KW error.
03/28/14   vi      Made changes to set drx to 320ms if rrc_constant_320ms_drx_nv is set
03/14/14   bc      Made changes to send partial plmn results to NAS during manual plmn search
11/05/13   rmn     Changes for displaying Partial PLMN Search Results during Manual Search
09/02/13   sr      Made changes for BST optimizations
03/17/14   vg      Added code to reject and send no_service for full service req if invalid SIM
03/17/14   sn      Changes to control ASF timer with a NV 
03/05/14   bc      Changes to have similar lock mechanism with or without WRLF feature during
                   connected mode OOS.
03/05/14   bc      Made changes to update Scanned band mask properly after completely scanning
                   a band.
02/10/14   bc      Made changes to abort SIB reading for FPLMN during WTOW and GTOW AUTO BPLMN scans.
03/13/14   vi      Stack reduction changes
03/13/14   sn      Check RSSI and update while skipping duplicate NAS PLMN entry
08/13/13   sg      Made changes to include pccpch tx div ind in cell transition req
02/21/14   bc      Made changes to intiliaze previous scanned freq and psc during BPLMN search 
                   at the end of BPLMN search
03/11/14   rmn     Split ACQ for LtoW Redirection
02/20/14   bc      Made changes to abort sib reading after MIB if the found VPLMN is 
                   already added to the list to be sent to NAS.
02/17/14   sr      Made changes to update CCM for serving cell barred cases based on L1 api
10/21/13   rmn     Added function to tell WL1 that BPLMN is active
01/22/14   gv      Report no service to UI only on completing ACQ DB search after entering out of service
01/20/14   sr      Made changes to reduce the TRM priority to BG TRAFFIC after WRLF recovery
01/02/14   rmn     Reduction of unwanted F3's
12/27/13   bc      Made changes to add shared plmns of the serving cell in the list to be sent to NAS
                   during Manual PLMN search.
12/30/13   bc      Made changes to optimize REL 10 Spec CRs code
12/30/13   sn      Send idle request to wl1 before converting background PLMN 
                   search to foreground 
12/30/13   sr      Made changes to modify lock priority with WRLF
11/07/13   bc      Made changes to abort sib reading if we find that the cell is barred 
                   or la is forbidden.
12/13/13   sr      Made changes to release lock while entering WRLF
12/13/13   sn      Manual PLMN search optimizations
12/13/13   sn      Changes to set UE OOS flag only if S-criteria passes
12/12/13   vi      Added code for FR: Service Search with scan scope ACQ DB only.
12/03/13   mn      Moved all declarations to top of the code.
12/03/13   sr      Made changes to reset PPAC and DSAC status
11/26/13   gv      Made changes to trigger FR2LTE when CS call gets released via RRC connection release
11/21/13   rmn     Made changes so that RRC doesn't req WRM functionality in Single Sim mode
09/16/13   sn      G2W TA feature changes
11/18/13   bc      Checked in REL 10 spec crs changes
11/16/13   sr      WRRC support for LWG DSDS
11/15/13   rmn     Made changes to save BPLMN data when leaving contd. mode scan is triggered due to CN Page Ind in PCH State
10/23/13   gv      Made changes to update GSM with correct band preference during foreground search
10/23/13   gv      Made changes to trigger FR2LTE only after a CS RAB gets established and then released
10/11/13   gv      Exposed the API rrccsp_derive_freq_from_uarfcn outside of WRRC
10/08/13   rmn     Made changes so that only saved M-PLMN Search is converted to fgrnd after leaving conn. mode scan is exhausted. 
10/08/13   rmn     Added additional check so that during ACQ DB only Service search, reporting no service occurs only for Service search cases. 
10/07/13   rmn     Changes for Feature: RAT Search Order for FDD/TDD LTE
09/25/13   bc      Added code to support exchange of AS ID when LTE feature is enabled
09/23/13   sn      Changes to pass GCF test case 34.123-1 12.2.2.7e
09/23/13   sn      Derive active band to be sent in service indication to upper layers from 
                   current acq entry
09/13/13   sn      Changes to include ECNO in CGI response to LTE
08/13/13   rmn     Made changes to re-start M-PLMN Search in start phase from bkgrnd to frgrnd on receiving OOS Ind.
08/13/13   rmn     Made changes to continue ongoing MPLMN Search in bkgrnd to frgrnd on receiving OOS Ind
08/13/13   sn      Changes to release after decoding SIBs during BPLMN
08/13/13   sn      Changes to release lock when we decide to stop/delay W2G BPLMN
08/13/13   gv      Added support for FEATURE_TRIPLE_SIM
08/15/13   sn      reset wait for WCDMA L1 release lock flag when BPLMN search is being 
                   suspended or aborted
08/12/13   sn      Restore service search background properly when converting PLMN
                   search from background to foreground
08/14/13   rmn     Made changes for requirement to indicate ACQ failure because of band mismatch
08/07/13   gv      Made changes to skip GSM band search during service scan when NAS indicates that
                   GSM band search is already completed
08/07/13   sn      Changes for selective band scan for PLMN searches
07/22/13   gv      Changes to release lock when we decide to stop/delay W2G BPLMN
07/08/13   sn      Changes for LTE CSG feature
06/25/13   sn      Block IRAT functionality in manual CSG mode
07/03/13   rmn     Made changes to not indicate CCM with cell loss in cases where serving freq is found barred during reselection.
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/21/13   sr      Made changes to abort MPLMN when OOS is received with cause no lock
06/19/13   sn      Changes to include CSG ID and RSCP in CGI response
06/13/13   sn      Force initialize RCE before updating NAS with no service indication
                   in disconnected state
06/06/13   sr      Made changes to abort SIB reading when MIB PLMN doesn't match with the 
                   NAS requested PLMN for XTOW redirection
05/30/13   gv      Made changes to perform acq db only scan when requested so by higher layers
05/29/13   sn      Update LCF with CCM frequency
05/17/13   sn      Changes to skip LAI checks for CSG cell selection 
05/14/13   sr      Hybrid CSG changes
05/08/13   sr      Made changes to reset ASET scr code for DCH to FACH trnasition when connection release is already received
06/05/13   rmn     Made changes to reset curr_scan after leaving conn mode scan fails and  no srvc is indicated to NAS
04/24/13   sn      WRRC side of changes to meet CSG ADAPT TCs
04/18/13   sr      Made changes to not start the RSSI timer when going to wait for lock state on receiving SERVICE_REQ
04/15/13   gv      Made changes to do full band scan when the redirected frequency is not available 
                   during LTOW redirection
04/12/13   gv      Made changes to check for the presence of primary plmn id before rejecting
                   the IDT because of PLMN mismatch
04/03/13   sr      Made changes to remove redundant MSG_HIGHs
04/02/13   gv      Made changes to abort SIB reading if atleast one cell is found in a DRX
                   during ASF search
03/26/13   db     Changes for unification of Cell Transition and CPHY setup handling.
03/25/13   gv      Made changes to stop GTOW redirection when the redirected cell belongs to forbidden LA
03/22/13   mp      Removed tmc.h include file
01/31/13   sn      Changes for BPLMN suspend/resume for WTOW searches
01/31/13   gv      Made changes to stop scanning of the same band twice when the band
                   corresponding to the first matched ACQ DB entry and first band in
                   the band prioritization list are different
01/12/13   gv      Fixed compilation issues with DSDS
01/10/13   sr      Fixed compilation issues
01/03/13   ad      Made changes to reduce the redundant F3s
12/17/12   sr      Added changes to not reset the OOS flag when STOP_WCDMA_MODE_REQ is in queue
                   or MCM substate is in WAIT_STOP_CNF
12/14/12   gv      Added code to pass AS ID in the commands sent to WL1
12/14/12   mp      Fixing KW erros.
12/12/12   sn      Perform LFS for manual PLMN searches when UE in deep sleep
12/11/12   sr      Made changes to bar the frequency/cells when network mode is limited
12/07/12   sn      CSG feature changes
11/30/12   ad      Made changes to remvoe the dependancy on  rfm_band_supports_dual_carrier
11/27/12   gv      Fixed compiler warnings when DSDS is enabled
11/09/12   gv      Made changes to not scan WCDMA bands when RAT priority list does not contain WCDMA
10/29/12   gv      Mainline the changes under FEATURE_PPAC
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/15/12   gv      Made changes to update ppac valid status correctly while sending SERVICE_CNF
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
09/26/12   sr      Fixed compilation errors
09/21/12   ad      Corrected featurization related to FEATURE_LTE_TO_WCDMA
09/20/12   sr      Added changes to reset ue_in_oos_flag before sending SERVICE_IND to MM 
                   during connected mode OOS
09/10/12   gv      Mainline the changes under feature FEATURE_WCDMA_BPLMN_SUSPEND_RESUME
09/03/12   gv      Added few comments
08/31/12   rd      Transfer dedicated priority parameters to/from WCDMA and GERAN
                   during IRAT procedure
08/31/12   gv      Mainline the changes under feature FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL
08/10/12   sn      Changes to correctly determine the BPLMN state while cleaning up
08/17/12   gv      Added changes to support suspension of intra/inter-W BPLMN search on entering DCH/FACH
                   and resumption of the same on entering PCH/IDLE
07/16/12   sn      When BPLMN in progress, NAS requested PLMN will be stored in previous data
08/13/12   gv      Made changes to check for LTE mode enabled status before triggering a WTOL blind redirection after CSFB call
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/12/12   sks     Made changes for WTOW redirection scans to reserve 1st two retries for destination freq + PSC search
                   and the remaining 4 retries for full search on redirected freq.
06/28/12   sks     Made changes to not update UI during connected mode OOS.
06/12/12   sks     Made changes to not report incorrect RSSI when UE mimics out of service
                   due to TRM resource.
05/09/12   gv      Made changes to not send IDLE_REQ to L1 on receiving a new SERVICE_REQ with network select 
                   mode as limited in Idle/PCH states
05/03/12   sks     Made changes for RRC to not update service status to UI during connected mode out of service.
05/03/12   sks     Fixed compilation warning when FEATURE_DUAL_SIM is not defined.
05/02/12   gv      Fixed compilation issue
05/01/12   sks     Added changes to indicate NAS that NO SERVICE is because of no
                   resource from TRM.
04/25/12   as      Made changes to trigger cell update/drx req when UE recived OTA with 
                   redirection info and cell reselection in progress
04/25/12   gv      Merged changes to reset flag do_lfs_scan for connected mode OOS search.
04/25/12   mn      Made changes to request L1 to camp on barred cells only for emergency call
                   service search request from NAS.
04/15/12   vg      fixed compilation errors.
04/13/12   pm      removing FEATURE_BAND_PRIORITY_ORDER_THROUGH_NV feature flag and reverting some #ifdef
                   changes
04/10/12   pm      Added code to support the band priority nv
04/09/12   gv      Added code to prevent CPU hogging during G2W redirection
04/03/12   sks     For XTOW BPLMN and CGI_REQ, made changes to optimize the retry ACQ duration in the next DRX
                   by saving PSC and PN_POS.
03/19/12   sks     Made changes to update UI with limited service during WRLF search.
03/09/12   gv      Provided NV support to enable/disable PPAC functionality
02/17/12   mn      Made changes to abort suspended BPLMN search at the start of OOS search.
02/09/12   gv      Added code to support inclusion of PLMN ID in IDT message with network sharing
01/31/12   gv      Made changes to update CM with RSSI immediately after sending Service Confirm to NAS
                   with Service Available
01/24/12   gv      Made changes to correctly verfiy the number of target frequencies in listdb during LTOW redirection
01/12/12   gv      Added code to support FEATURE_PPAC
01/06/12   gv      Fixed the buffer overflow issue when DSAC parameters for all is used on shared network
12/30/11   gv      Made changes to abort pg1 procedure whenever a new 
                   camping procedure is triggered
12/12/11   mn      Made changes to process ps hard abort request received when waiting for lock
12/12/11   gv      Added code to support free floating of tasks
11/18/11   mn      Made changes to disable reselection to LTE after attach reject from network
                   with cause 14.
11/17/11   ad      Fixed compilation error
11/11/11   mn      Made changes to send CLOSE_SESSION_IND to NAS, in case of WRLF when RRC is 
                   waiting for L2 ACK for PS SCRI during fast redirection to LTE preparation.
10/31/11   sks     Added support for FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB.
10/27/11   gv      Made changes to not move to idle state after finding limited service on HPLMN
                   during connected mode OOS
10/19/11   gv      Fixed compilation errors for BC19
10/19/11   gv      Modified the way to update scanned band mask
09/28/11   gv      Added code to support Optimized Band Scan in WCDMA
09/26/11   mn      Made changes to call the new API to get band capabilities of the UE in case of DSDS
09/17/11   gv      Merged code to send limited srv ind to UI if s criteria met
09/12/11   gv      Renamed feature flag FEATURE_WCDMA_RRC_EMERG_CALL_SUPPORT
                   to FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL
09/12/11   gv      Added code to support FEATURE_WCDMA_RRC_EMERG_CALL_SUPPORT
09/09/11   rl      Fixed KW errors
09/07/11   gv      Modified code to use macro which checks digit by digit when comparing PLMNs
08/26/11   rl      Fixed the KW error
08/12/11   mn      made changes to not to include shared plmns if nas requested plmn is one of them.
08/02/11   gv      Updated the curr scan type after the function call rrc_csp_get_aset_scr_pn()
07/29/11   gv      Added fix to skip prioritizing GSM during service request
                   under the feature flag FEATURE_WCDMA_SKIP_GSM_FOR_SVC_SCAN
07/28/11   gv      Added code to support FEATURE_UIM_DISABLE_GSDI_INTERFACE
07/18/11   sks     Added new search type L1_ABPLMN_SRCH_TYPE to differentiate between Manual 
                   BPLMN and Auto BPLMN search.
07/08/11   sks     Added a function to init first band to be scanned.
07/08/11   gv      Made changes to reset the last mcm activation cause whenever 
                   a fresh acquisition happens
07/06/11   gv      Fixed KW errors
07/04/11   gv      Merged changes to update LTE and GSM mode status from rat priority list
                   on successful cell selection
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/07/11   vg      added code to fix corner case DCH->FACH gaurd timer expiry bplmn scenario
06/17/11   gv      Made changes to perform BPLMN search only for 320ms and above drx cycle.
06/08/11   vg      fixed RRC going in infinite loop issue when NV rrc_ignore_cell_bar_nv is enable.
05/19/11   sks     Made changes to use SI containers for nornal LTOW PS redirection.
05/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.97.50
05/05/11   vg      added channal locking feature support under rrccsp_channel_locking_nv_status
                   condition
05/03/11   rl      Fixed the lint error
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
04/25/11   rl      Fixed the KW error
4/12/11    vg      fixed the issue where cell access bared list was not been read.
04/08/11   sks     Featurised code for checking RF band support for DC.
04/06/11   sks     Added a function to check RF band support for dual carrier.
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/28/11   sks     Made changes to return TRUE for can_L1_go_to_sleep(), if CTCH setup is
                   pending at LLC.
03/21/11   rl      Added code to support L2W CGI
03/28/11   rl      Merged the HS-FACH/PCH changes from mainline
03/21/11   sks     Made changes to parse rat priority list from GSM-RR after GTOW procedures.
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/10/11   sks     Made changes to not include pre-redir info in RRC connection request
                   msg following a LTOW redirection.
03/10/11   sks     Made changes to implement 5s guard timer for LTOW redirection.
03/09/11   su      Fixed compilation warning.
03/08/11   su      Added code to support enabling and disabling of 
                   FEATURE_UEUT_IGNORE_CELL_BAR_RESERVE_STATUS using NV item.
03/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/23/11   sk      added code for BC19 additonal channel scaning
02/17/11   as      Made changes to change lock prioriy properly for dsim tuneaway
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   rm      Added code to support BC19 under FEATURE_WCDMA_BC19
01/27/11   rm      Made changes to check for S-criteria before stopping EOOS search timer
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/13/11   rm      Added code to use correct ACQ entry ptr in rrccsp_fill_in_plmn_lai_domain_ac_info
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/20/10   sks     Made changes to perform acq retries on LTOW redirected freq.
12/09/10   as      Made changes to reset L2 ack info for DSIM Tuneaway
12/08/10   rm      Added code to follow CSFB optimizations for LTOW redirections only
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/06/10   sks     Made changes to remove ACQ-DB bands from prioritized bands
                   list when full scan timer is running.
11/30/10   as      Added changes to update tuneaway status to trm
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/22/10   sks     Added support for handling EM call during WRLF.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   sks     Fixed KW errors.
11/02/10   rm      Added code to handle CSFB status indication
10/27/10   as      Fixed compiler warning
10/26/10   rm      Added code to set num of LTE PLMNs to zero when sending BPLMN SRCH CNF to NAS in ABORT cases
10/25/10   as      Fixed KW errors
10/21/10   rm      Fixed KW errors
10/18/10   rm      Added boundary check when reporting available PLMNs to NAS in LIST_CNF
10/18/10   rm      Added code to support EOOS Events
10/11/10   rm      Added code to support 3GPP CSFB
10/05/10   as      Added code to retain lock after chaning priority to BG_TRAFFIC
                   for FEATURE_DUAL_SIM
09/29/10   as      Fixed KW issues
09/16/10   sks     Added a function to check SIB1 validity by checking the LAC validity.
11/03/10   rl      Fixed review comments
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM
09/09/10   rm      Made changes to hold dedicated priorities during OOS
09/01/10   rm      Fixed compiler warnings
08/31/10   rm      Fixed compiler warnings when FEATURE_DISABLE_LTE_IRAT_BPLMN is defined
08/30/10   rm      Added code to disable WTOL BPLMN under FEATURE_DISABLE_LTE_IRAT_BPLMN
08/30/10   rm      On start BPLMN search if W search completes, made changes to start
                   interrat BPLMN search which is due
08/17/10   sks     Added an API to inform upper layers if PS abort is needed or not
                   in a dual SIM scenario.
08/17/10   rm      Made changes to set maximum interleave freq (max_freq_interlv_ffs)
                   to MAX_LIST_DB_FREQS_INTERLV_WITH_FFS always irrespective of search time
                   for EOOS
08/02/10   sks     Made changes related to FEATURE_WRLF_SYSTEM_SEL.
07/23/10   rm      Fixed compiler error when FEATURE_DUAL_SIM_BPLMN is not defined
07/17/10   rm      Corrected missed merges in reporting LTE PLMNs to NAS
07/16/10   rm      Added  code to send PARM_REQ to WL1 when SIB11/SIB12 changes
07/12/10   rm      Added new line at EOF
07/09/10   rm      Fixed compiler warnings
07/06/10   rm      Added code to put WL1 to sleep if BPLMN is suspended/cleaned up when
                   lock release offset timer is running
07/05/10   rm      Fixed compiler warnings
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/25/10   rm      Added code to support DUAL SIM BPLMN
06/18/10   rm      Added code to reset cell_reserved_for_operator_use when bar request is not
                   sent to WL1
06/18/10   rm      Made changes to indicate WL1 if the frequency is reserved for operator use.
06/17/10   rm      Made changes to do list scan for EOOS new service request
06/14/10   rm      Added code to support BST search on WCDMA
06/14/10   rm      Added code to update UI with limited service and to indicate OOSC end to CM
                   during ANY PLMN OOSC search
06/14/10   rm      Made changes to stop full frequency scan skip timer, on service
                   request for emergency call
06/07/10   sks     Made changes to update UI with limited service if any cell is found
                   during ANY_PLMN out of service search in connected mode.
06/07/10   rm      Added code to let WL1 go to sleep when processing of CTCH drop request from CSP
                   is not complete at LLC
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
05/24/10   sks     Made changes to support R7 spec CR related to HPLMN-VPLMN ping-pong.
05/24/10   as      Made changes to index SIBDB with common PLMN ID for shared cell
05/13/10   as      Fixed KW errors
05/11/10   rm      Made changes to not use LFS for connected mode OOS
05/10/10   sks     Made changes to not change the establishment_cause in case of registration
                   attempt after GTOW redirection success.
05/06/10   rm      Fixed compiler warnings
05/06/10   rm      Fixed compiler warning
05/05/10   sks     Made changes to release channels only after L1 is moved out of PCH_BPLMN state.
04/12/10   rm      Added code to support BC11
03/31/10   rm      Fixed compilation error when FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY is defined
03/29/10   sks     Made changes to acquire lock before waking up L1 during
                   connected mode OOS search.
03/29/10   as      Made changes to set acq_db_empty flag properly for LFS_COOS, so that 850/PCS/900
                   band optimization  can be followed accordingly.
03/16/10   rm      Fixed compilation error
03/15/10   rm      Added code to support FEATURE_RAT_PRIORITY_LIST for W<->G IRAT
03/15/10   gkg     Made changes to put changes for "Multiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
03/11/10   sks     Made changes to perform LFS when cell selection fails on DL NV frequency.
02/01/10   dm      Added function for sending CPHY_DRX_REQ to L1 for multiple DRX support feature.
02/23/10   as      Added code to send correct Tresel value to L1 during state transitions to PCH/FACH
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
02/03/10   dm      Added function to check if reselection data for neighbour cell has been cleared.
01/19/10   sks     Made changes to perform LFS when WTOG OOS redirection fails.
01/18/10   sks     Made changes to not apply AGC threshold for ACQ on LFS scan results on every 12th
                   attempt, in case of COOS.
01/04/10   rm      Added code to reset num_scr_code_det on start of cell selection
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
12/01/09   sks     Fixed lint warnings.
11/30/09   sks     Resetting flag stop_pending_for_wtog_abort in function rrc_csp_init_int_data().
11/02/09   sks     Removed function rrccsp_check_wtog_bplmn_abort_in_progress().
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
10/08/09   sks     Made changes to not reset num_available_freq for each BPLMN DRX search cycle.
09/25/09   sks     Provided API rrccsp_check_initiate_cell_selection_handling()
                   to give info if CSP will handle CELL SELECTION REQ
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
09/22/09   rm      Added code to clear L1 barring info and write ACQ DB to NV on
                   MODE_CHANGE_REQ to LPM 
09/16/09   sks     Removed code so as not to follow LFS for OOS in Idle and Connecting state.
09/15/09   sks     Added code to check for supported bands in function rrccsp_is_current_band().
08/17/09   sks     Added code to stop full scan timer and deep no srvc timer for
                   LIMITED service search, so that deep sleep optimization is not followed.
08/10/09   sks     Added code to skip reading SIBs for a FPLMN cell during connected mode
                   OOS search for spec PLMN.
08/06/09   rm      Ported full frequency scan timer to FEATURE_LFS_COOS
08/03/09   sks     Added code to get G acq channel info and clear it immediately
                   for a RRC_SERVICE_REQ from NAS for COOS scenarios.
08/03/09   sks     Added code to not read SIBs for a FPLMN cell during OOS scan. 
07/27/09   sks     Added code to initialize do_lfs_scan flag in rrc_csp_init_int_data().
07/20/09   rm      Modified the call to funtion rrcrce_force_initialize
07/20/09   sks     Made changes to update UI correctly with service status when leaving 
                   connected mode after being out of service. 
07/20/09   rm      Removed usage of FEATURE_SURF_5200
07/16/09   rm      Ported FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE to COOS
07/13/09   rm      Added code to send SERVICE_IND with NO_SERVICE to NAS when RRC is leaving
                   connected mode OOS and regsistration is in progress at NAS
07/03/09   rm      For 800/850 band in rrccsp_is_current_band, made changes to derive the band_class
                   based on the MIB PLMN_ID
07/02/09   rm      Added code to update current_band with curr_acq_entry band class for LFS_COOS
06/30/09   sks     Made changes to suppress compilation warning.
06/25/09   sks     Ported FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION to conventionel OOS.
06/22/09   rm      Made changes to not reset umts_chls_from_gsm on SERVICE_IND to NAS
06/22/09   sks     Added code to reset available frequency list on every search cycle when
                   following LFS for COOS.
06/03/09   rm      Provided rrc_csp_cell_trans_cnf_rcvd_at_llc to LLC 
05/27/09   rm      Moved the skip GSM search changes under FEATURE_SKIP_GSM_SEARCH
05/22/09   rm      Initialized ctch_setup_req_pending in rrc_csp_init_int_data
05/21/09   rm      Made changes to initialize num_plmns to zero when BPLMN search is restarted
                   in foreground
05/20/09   rm      Added code to skip GSM search when searching for SERVICE_REQ, if NAS indicates
                   GSM search done as TRUE in RRC_SERVICE_REQ
05/15/09   sks     Added code to reset the stored n/w sel mode before starting leaving
                   connected mode scan for cases other than conn mode OOS.
05/14/09   sks     Added code to handle RRC_NW_SEL_MODE_RESET_IND for Manual->Automatic
                   and Automatic->Manual. Also made changes to restore nw sel mode and CSP 
                   substate if they were modified internally during connected mode OOS.
05/13/09   rm      Fixed lint errors when FEATURE NETWORK SHARING is not defined
05/11/09   sks     Made changes in function rrccsp_is_current_band() to validate the band
                   indicated in SIB5 band_indicator with the band corresponding to the curr_freq
                   instead of the current band.
05/11/09   rm      Made changes to send cell capability info whenever RRC sends 
                   a MM cmd.
05/11/09   rm      Added code to initialize cel_resel_ind_rcvd_waiting_trans_cnf
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
05/07/09   rm      Added code to send RR_GAN_MODE_IND after RR_GAN_ACT_IND when starting cell selection
                   on RRC_SERVICE_REQ for Non-UOOS
05/04/09   rm      Made changes to allow only EM calls, when UE Low Access Class is absent
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/14/09   rm      Added code to send activation indication status as MM_AS_GTOW_REDIRECTION
                   when GTOW redirection through connection release is in progress
03/28/09   rm      CMI Featurisation
03/04/09   rm      Made changes to initiate LFS on every new service search request
03/02/09   rm      Report to NAS about RRC connected mode OOS status in case of RL failure
03/02/09   rm      Made changes to initiate LFS on every new service search request
02/27/09   rm      Made changes to index the correct SIBDB when updating the PLMN list for
                   RRC_CSP_PLMN_LIST_SCAN
02/25/09   rm      Made changes to send correct signal_strengths in RRC_INTERRAT_PLMN_SRCH_CNF
02/04/09   kp      Bug fix in function rrccsp_update_wtow_background_plmn_list
                   to call rrc_bplmn_return_sib_for_cell for all BPLMN scans.  
02/02/09   rm      Ported List Frequency Scan (LFS) to conventional OOS
02/02/09   rm      Fixed Lint errors
01/29/09   sks     Made changes to search for both WCDMA amd GSM in every search cycle
                   in connected mode UOOS.
01/19/09   dm      Made changes to support code when feature 
                   FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN is undefined.
01/19/09   sks     Made changes to not update CCM with cell loss when unbarring
                   the cells.
01/16/09   rm      Made changes to set cell_change_type to correct value when sending 
                   RRC_CELL_CHANGE_REQ to CCM
01/09/09   rm      Fixed compilation warning when feature 
                   FEATURE_CONNECTED_MODE_UMTS_UNIFORM_OOS_HANDLING_UI_UPDATE is undefined
01/08/08   sk      Removed sdss_get_supp_gsm_band_pref and defined the fucntionality
                   of it locally.
01/07/08   sks     Fixed compilation error when FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
                   is undefined.
12/29/08   rm      Added support for new Events
12/29/08   rm      Made changes to update aset_scr_code correctly when leaving connected mode
12/26/08   sk      Added rrccsp_check_gsm_bands_supported() to check if GSM band supported
12/18/08   sk      Made changes to set the flag ue_in_deep_sleep according to value 
                   sent by RR.
12/11/08   sks     Fixed compilation warning when 
                   FEATURE_CONNECTED_MODE_UMTS_UNIFORM_OOS_HANDLING is defined. 
12/08/08   sk      Added code to initiate LFS on every new service search request.
12/01/08   sks     Made changes to update UI correctly with service status during 
                   conventional OOS search.
11/27/08   sk      Changes for BPLMN 320ms
11/21/08   rm      Added code to unbar non-eplmn cells when Equivalent PLMN list changes
11/06/08   sk      Pending stop mode cmd till W2G BPLMN is aborted
10/24/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
09/28/08   kp      Added support to call rrcsib_force_sib_event_init() upon initiation of
                   out of service area scan in idle state.
09/22/08   rm      Made changes to send service status to MM with UOOSC indicator as FALSE
                   on reception on STOP WCDMA req from NAS and UOOSC search was in progress
09/01/08   sks     Defined rrccsp_get_serving_frequency, rrccsp_get_neighbor_frequency, 
                   and rrccsp_get_curr_nas_requested_plmn
08/25/08   sks     Report to NAS about RRC connected mode status.
08/01/08   pk      Report Limited_Service or No_Service to UI as soon as first cycle of
                   GSM search is done instead of waiting for whole GSM search to complete
                   when in OOS in RRC connected mode.
07/03/08   pk      Fixed compilation error when both UOOS and GtoW redirection features 
                   are undefined.
06/30/08   ns      Made the necessary changes to enable the functionality of the feature 
                   FEATURE NETWORK SHARING only when the GCF NV is set
06/20/08   pk      Reset oosc_plmn_found only when both W & G searches are completed.
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/20/08   pk      Added support for FEATURE_WCDMA_BAND_PRIORITIZATION.
05/19/08   pk      Inform RR if no WCDMA service is found for a SERVICE_REQ from NAS.
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
04/23/08   pk      Fixed lint error.
03/13/08   vm/pk   Added support for feature FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE.
03/11/08   pk      Added support to report current cell id to NAS through RRC_SERVICE_CNF,
                   RRC_SERVICE_IND and RRC_SERVICE_IND_FROM_OTA.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
03/05/08   pk      Added support to indicate Non HS Capability while sending No SERVICE 
                   to upper layers. 
03/05/08   pk      Removed unnecessary inclusion of rr_general.h
03/05/08   pk      Added support to check for type of frequency scan each time while 
                   checking whether to skip a freq from doing ACQ.
01/10/08   pk      Added an extra protection in RRC to send CPHY_IDLE_REQ to take L1 to
                   IDLE state before sending CELL_BAR_REQ to unbar all the frequencies 
                   barred because of partial roaming restrictions.
01/03/08   pk      During cell reselection, bar a frequency for "reselection only" if none 
                   of the shared PLMNs of the cell are equivalent to the NAS requested PLMN.
12/21/07   vr      Support for FEATURE_MBMS
12/13/07   pk      While reselection if serving frequency is being barred "only for reselection"
                   (due to CR151), then don't update CCM with cell_loss as UE can still be camped 
                   on serving cell.
12/07/07   vm      Added fix for CR 134058. Removed resetting of next_band_mask on sending
                   service indication to NAS. 
11/27/07   vm      Added fix to not check for forbideen LAIs for Out of Service Area Scan.
                   Otherwise we see an issue when FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT is NOT
                   enabled but FEATURE_OUT_OF_SERVICE_ENHANCEMENT is enabled.
11/26/07   vm      Changed fn name.
11/21/07   rc/vm   Added support for FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
11/21/07   vm      Added support for FEATURE_UEUT_IGNORE_CELL_BAR_RESERVE_STATUS
10/26/07   vm      Added support for feature FEATURE_DISABLE_DEEP_SLEEP
10/26/07   vm      Moved some code outside of FEATURE_UMTS_UNIFORM_OOS_HANDLING as
                   part of fix for CR 129966.
10/23/07   pk      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE NETWORK SHARING.
10/19/07   pk      Added check for FACH->FACH/PCH transitions. 
10/12/07   vm      Added support for L1 feature FEATURE_WCDMA_SRCHACQ_RETRY.
10/12/07   vm      Added defn of new fn rrccsp_does_mcc_belong_to_US().
10/09/07   vm      Fixed lint error.
10/08/07   pk      Moved the code which adds a freq to the available freq list to a 
                   new function and also added support to call this new function in case
                   of FPLMN check.
10/05/07   vm      Added check to call function rr_rrc_get_acq_chnl_info() only when Dual 
                   mode is enabled.
10/03/07   pk      Added rrccsp_get_current_plmn_cell_id() which returns the 
                   current PLMN ID and Cell ID
10/03/07   pk      Added support to use the variable curr_cell_access_status to fill in the cell access 
                   status instead of the variable cell_access which is stale. 
09/28/07   vm      Removed GCF flag dependency for UOOSC
09/20/07   vm      Added support for feature FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
09/20/07   pk      Fixed Klocwork warning. 
09/14/07   vm      Added fix for CR 126406. Modified existing function rrccsp_update_acq_db()
                   so that the current camped freq is not overwritten when ACQ DB is 
                   updated with new frequency from RPLMN Freq List.
09/10/07   vm      Added support for feature FEATURE_RRC_DEFAULT_BAND_PRIORITY.
09/10/07   pk      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/06/07   vm      Added fix for CR 125215. When SIB proc calls is_current_band() fn for checking
                   band mismatch then for bands other than 800/850, get_frequency_band() fn is 
                   called.
07/19/07   pk      Added support for not to overwrite DSAC restrictions with normal cell access 
                   class restrictions.
07/12/07   pk      Added support for sending CPHY_CELL_BAR_REQ in limited mode also for the
                   feature FEATURE SUPPORT FOR PARTIAL ROAMING to unbar all the barred frequencies. 
07/11/07   vm      Fixed complilation error.
07/07/07   vm      Removed BSP defined constants for WCDMA Band Boundaries. These have been
                   replaced by WL1 defined constants.
06/28/07   pk      Calculate and report RSSI value for the RPLMN in case of manual PLMN search
                   instead of reporting -95dB.
06/25/07   vm      Added fix for CR 119540. Added support to correctly set/reset current_band 
                   so that the band indicator check by SIB proc is done against the correct
                   current_band value.
06/22/07   vm      Added support for stopping DS tmr after getting the remaining
                   DS timer value
06/15/07   vm      Added support for FEATURE_CONNECTED_MODE_UMTS_UNIFORM_OOS_HANDLING_UI_UPDATE
06/07/07   vm      Added fix for CR 120433. The var rrc_csp_curr_select_data.num_available_freq 
                   is now explicitly set to 0 when List DB scan is started.
06/04/07   pk      Added support for "Partial Roaming Restrictions" under the feature 
                   flag FEATURE SUPPORT FOR PARTIAL ROAMING. For details please see
                   spec CR 151.
05/25/07   vm      Added fix for CR 117262. During UOOS search for OOS in Connected
                   Mode, CSP will now NOT scan ACQ DB RRC_CSP_MAX_NUM_ACQDB_DEST_FREQ_CYCLES
                   times.
                   Also, please notice that the above change is useful when UOOS P2 feature
                   is NOT defined. When UOOS P2 feature is defined, CSP does LIST DB Scan 
                   instead of ACQ DB scan.
05/24/07   pk      Added support for feature FEATURE 900 FREQ SCAN OPTIMIZATION.
05/23/07   pk      Added fix for CR 117463. Now different timers are used for automatic and manual
                   searches under the feature flag FEATURE_MANUAL_SEARCH_FREQ_SCAN_SKIP_TIMER. 
                   Full frequency scans are not done for automatic and manual (both foreground 
                   and background) searches while the respective timers are running.
05/21/07   vm      Added support for the feature FEATURE_RRC_ADDL_CHNL_SCAN_OPTIMIZATION
05/17/07   pk      Added fix for CR 118030. During WtoW BPLMN search report the 
                   RSSI of the tuned frequency to upper layers instead of the 
                   current camped frequency.  
05/14/07   pk      To call rrcrce_force_initialize() in rrc_csp_init_data_from_service_req()
                   and rrc_csp_start_sel_for_plmn_list() as a fix for CR 118382. 
04/30/07   vm      Added fix for CR 115006. Added fix to initialize local var type_freq_scan in fn call 
                   rrc_csp_continue_cell_selection() with a default value RRC_CSP_NO_SCAN.
04/20/07   vm      Added fix for CR 116018. Added call to fn rrc_csp_init_uniform_oos_int_data()
                   when RRC_SERVICE_IND is sent to NAS with service available.
04/17/07   pk      Added support for maintaining intra frequency cell reselection,
                   WTOG cell reselection and OOS statistics which are needed to evaluate 
                   performance in TCXO Shutdown.
04/11/07   vm      Added support for FEATURE UMTS UNIFORM OOS HANDLING PWR UP ENH
03/30/07   vm      Added some debug messages
03/30/07   vm      Added fix for CR 115461. Added support to check the band present
                   in SIB5/6 with supported band group instead of current band for
                   leaving connected mode scan and inter-freq redirection scan.
03/29/07   segu    Added function definition for rrccsp_skip_sibs_for_fplmn()
                   under feature flag FEATURE SKIP SIBS FOR FPLMN
03/27/07   vm      Added fix for CR 114936. The contents of CPHY_CELL_TRANSITION_REQ cmd sent
                   to L1 are now properly initialized, especially req_mask. Junk value of
                   req_mask was leading to NULL ptr access in WL1.
03/16/07   vm      Added fix for CR 109422. During foreground service search 
                   if a cell is found on which UE can camp on then set the 
                   var plmn_fnd_during_fgnd_svc_srch to TRUE. This var serves
                   two purposes:
                   (1) For UOOS, this var has been added to inform NAS 
                   (using var ue_oos in RRC_SERVICE_CNF) that service is available. 
                   NAS looks at this var to determine if limited service request
                   should be sent to RRC or not. Notice that with UOOS, SD always 
                   sends Automatic Service Request and it sends Limited Service Request
                   only when NAS informs SD that Limited Service is available.
                   (2) For non-UOOS, this var is used to NOT start the 10s No Service
                   timer which (without this fix) gets started when UE finds a PLMN
                   that belongs to forbidden LAI which is NOT correct because UE can
                   get Limited Service on this PLMN.
03/07/07   vm      Fixed CR 113511 - This was a corner case of last acquisition on the 
                   last of the supported bands failing at the same time the UOOS Twcdma 
                   timer expires in RRC.  Due to this, in the next awake period the 
                   acquisition database + FFS is found to be empty. Now, UE restarts 
                   the search and starts FFS  search (since ACQ DB is empty). But instead 
                   of returning next CSP substate as RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF, 
                   CSP doesn't update the next_substate and it keeps its default value 
                   of RRCCSP_NO_CELL_SELECTED. And in this substate Tw timer expiry is 
                   NOT handled and UE gets stuck. The fix is to update the CSP substate 
                   in this case to RRCCSP_CELL_SEL_WT_ACQ or RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF 
                   as the case may be.
03/05/07   vm      FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2: agc value is now sent to L1 even when
                   uoos is not being followed - this is because L1 always looks at this value
                   when uoos p2 feature is defined.
03/04/07   vm      Added support for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2.
02/02/07   da      Made change to start 4s + activation time for DCH->FACH timer.
                   Part of fix for CR 109198
12/26/06   pk      Added support to use the default value of the access class barred list 
                   if DSAC restrictions are omitted.
12/22/06   bu      In rrccsp_is_current_band(), if the current CSP scan is DCH->FACH/PCH
                   scan and destination frequency is present, check if the specified band
                   is in the current supported band group instead of just the current
                   band, UE is camped on.
12/05/06   bu      If in rrc_csp_start_sel_on_mm_req(), rrc_csp_start_cell_selection()
                   returns a FAILURE, that indicates that during the previous cell
                   selection, UE had completed acquisition/full scan on all the
                   supported bands and so UE needs to:
                   1) Increment the uoos_num_cycles_completed
                   2) Set the uoos_new_search to TRUE &
                   3) Perform ACQ DB and full scan on the supported bands for the
                   available Twcdma time. 
11/30/06   segu    Do not inform PLMN info to upper layers when RRC is in connected
                   mode.
11/21/06   vm      Added support for the feature FEATURE UMTS BACKGROUND FULL SERVICE SEARCH.
11/20/06   vm      Added support for the feature FEATURE_INTER_FREQ_EM_CALL_REDIRECTION.
11/16/06   vm      Added MCCs 442 and 443 to Japan MCCs and defined two new constants 
                   JAPAN_MCC3 and JAPAN_MCC4.
10/26/06   vm      Added fix for CR 104768 - for BPLMN search do not reset 
                   current_band and first_band_to_be_scanned every time ACQ DB
                   is scanned from the beginning. This is to ensure that 
                   full freq scan is first done on the most recent "band entry" in the
                   ACQ DB.
09/27/06   segu    Restricted the check to only for RRCCSP_SUIT_CELL_RESELECT_WT_SIB                       
09/27/06   segu    In the function rrc_csp_compare_expected_cell(), if the current
                   acq type is SPECIFIC_PLMN,network select mode is manual and 
                   curreent CSP sub state is wating for RRCCSP_SUIT_CELL_RESELECT_WT_SIB
                   (or) RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB(or) G->W Inter RAT reselection 
                   is in progress then return the cell as below expected otherwise as expected
09/26/06   vm      Added support to send the band being scanned, i.e.,
                   rrc_csp_curr_select_data.current_band, information to L1
                   in fine scan request. For details pls see CR 101350.
09/25/06   vm      Added support to send the (correct) band on which UE is camped on 
                   in RRC_SERVICE_CNF/RRC_SERVICE_IND by converting the RRC internal 
                   band type to sys band type (used by SD/CM). Added definition of 
                   a new function rrc_convert_band_to_sys_band().
                   For details pls see CR 100829.
09/25/06   vm      Changed an ERR message to a MSG_HIGH because it can be a genuine
                   code path due to race conditions. For details please see 
                   CR 102193. 
09/15/06   vm      Added support for the feature FEATURE_UI_RF_ENABLED_BANDS_SUPPORT.
                   Added definition of a new function rrccsp_is_band_class_enabled_in_ui_rf().
09/05/06   vm      Added support for UOOSC.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   vm      Added support for the feature FEATURE_DCH_TO_FACH_PCH_OUT_OF_SERVICE_ENHANCEMENT.
08/25/06   vm      Added support for FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT.
08/23/06   segu    a)In the function rrc_csp_compare_expected_cell(), if the current
                     acq type is SPECIFIC_PLMN and network select mode is manual
                     then call nas function to see if the LAI is part of reject list
                     if so return the cell as below expected otherwise as expected
                   b)Using the plmn id and cell_id got as the formal parameters from
                     the function rrc_csp_find_access_control() while calling the 
                     fucntion rrc_fill_access_class_info()
08/09/06   vm      Added support for not applying AGC thresholds for specific
                   PLMNs and frequencies. The changes are under the feature name 
                   FEATURE_DEEP_SLEEP_NO_AGC_THRESHOLD_FOR_SPEC_PLMN_FREQ. Please
                   see CR 99208 for details.
07/21/06   vm      Added support for UOOSI. 
07/06/06   vm      Added support to not populate the available plmn list that is sent
                   to NAS for limited service request. 
07/07/06   segu    In rrc_csp_send_mm_cmd(), corrected with mm_common_buf_ptr 
                   instead of mm_buf_ptr while filling current cell access status
                   in RRC_SERVICE_IND_FROM_OTA_MSG 
06/30/06   segu    service_cnf structure is not correctly filled with gsm map common 
                   information while sending SERIVCE_CNF to mm, removed few -ve indentations 
                   and updated the prints that will say as signal strength >= -95db
06/26/06   segu    Signal strength greater than (or) equal to 95db is considered as
                   SYS_SIGNAL_QUALITY_HIGH instead of usual 75db
06/21/06   segu    Fixed the merged issue where it is not updating the cell access status
                   in CSP internal database
06/15/06   vk      When out of service area is detected, then change the status of
                   variable hsdpa_hsupa_ind to hsdpa not available
06/13/06   segu    Added support for DSAC feature implementation
06/08/06   vm      Added support for new feature FEATURE_MCC_AND_850_800_BAND_COMBINATION.
                   1. Included header file rrcdata.h.
                   2. The function validate_dl_arfcn() used in the file is now called with additional 
                      arguments when the feature is defined. 
                   3. Changes related to setting the variable current_band correctly as it is used by SIB  
                      procedure during the camping process.
                   4. The function rrc_is_arfcn_valid() called by GSM to filter out W UARFCNs, now 
                      takes another argument - which is the PLMN on which the UE is camped on the 
                      GSM side when the function is defined. 
                   5. Added definition of a new function 
                      rrccsp_is_sib_mcc_and_850_800_current_band_comb_valid().
05/31/06   segu    cell access status is updated before sending a command to NAS
                   i.e. either RRC_SERVICE_CNF (or) RRC_SERVICE_IND
05/26/06   vm      Added support to initialize current_band correctly when
                   UE needs to scan the frequency in NV before proceeding
                   to ACQ-DB/Full-scan when RRC_SERVICE_REQ is received by RRC from NAS. 
05/15/06   sgk     Added chenges to support scaling factors for treselection in 
                   SIB3 and 4.
05/08/06   segu    Added support for RSSI optimazation
05/04/06   vm      Added support to read dual mode flag from PLMN_LIST_REQ instead
                   of relying on the MCM dual mode flag (which gets updated whenever
                   service request is received). 
04/24/06   vm      Added definition of new function rrccsp_is_current_band()
                   which is called by SIB procedure for SIB5bis support.
04/21/06   segu    Added support for feature EHPLMN under the compiler flag
                   FEATURE_EQUIVALENT_HPLMN  
04/18/06   vm      Added support to not camp for full service on any of the cells
                   if the frequency is barred. Details in CR 91997.
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
03/30/06   vm      Added support to start the PLMN list full freq scan timer 
                   for 3G to 3G Automatic/Manual PLMN searches also (both 
                   background and foreground). As long as this timer is active 
                   no full scans are done. 
03/15/06   vm      Added fix to not reset next_band_mask when RRC_SERVICE_IND
                   is sent to NAS. For details please see CR 89843. next_band_mask
                   is now also reset in the function rrc_csp_init_select_data().
03/15/06   siva    Fixing the warning for LINT 8.0
03/10/06   sgk     Added code to support sib4 extensions qhyst and t reselection 
                   for PCH and FACH states.  
03/07/06   vm      Added support to populate rrcueci_band_storage_type structure 
                   with band information for BC8 and BC9.
03/03/06   vm      Added support for doing Manual BPLMN search in background
                   when Manual PLMN List Search request is received when CSP
                   CSP scan type is LEAVING_CONN_MODE. Specifically, added support
                   to complete the manual search in foreground if UE is not able
                   to find service when scan type is LEAVING_CONN_MODE.
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/21/06   vm      Added support for 1700_2100 AWS Band (IV) under feature name
                   FEATURE_WCDMA_BC4.
02/17/06   vm      Added support for 1700 band under feature name FEATURE_WCDMA_BC9.
02/16/06   vm      Added the following BPLMN optimization:
                   Doing Manual PLMN search in foreground when UE goes OOS in Idle Mode 
                   during Manual BPLMN Search. Earlier when UE went OOS during Manual 
                   BPLMN search, RRC was aborting the search and returning 0 PLMNs to the 
                   user. Now RRC automatically restarts the search if UE goes OOS in IDLE 
                   mode during Manual BPLMN search and completes it in foreground and returns 
                   the results to the user. 
02/10/06   vm      Added fix for initializing signal_strength to a default value
                   of 75 when RPLMN is added by default to available PLMN list
                   for Manual PLMN search request.
02/09/06   vm      Added fix to properly initialize BPLMN variables when Manual
                   BPLMN search is interrupted by LAU/RAU and the feature 
                   EATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION is enabled.
01/30/06   vm      Added the following BPLMN optimization: always adding serving PLMN 
                   to available PLMN list for Manual PLMN Search. For this a new function
                   has been defined - rrccsp_update_wtow_manual_bplmn_list_with_rplmn().
01/25/06   bd      Lint clean-up
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
01/23/06   kp      Added support for Transaction I.D in BPLMN Abort feature 
01/20/06   vm      Added support for getting GSM arfcns during Manual/Automatic
                   PLMN search so that acq attempt on some of the W channels can
                   be skipped to reduce freq scan times in PCS/850 bands. The 
                   changes are under feature flag FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION.
01/20/06   bu      For now updated the rrccsp_get_supported_band_list() to set the
                   band type for 900 MHz band to rrc_RadioFrequencyBandFDD_spare1. This
                   will need to be updated when the new Rel 6 ASN.1 changes are brought in.
                   All changes are under FEATURE_WCDMA_900.
01/10/06   bu      Corrected the check to determine if a WTOG PLMN search should be
                   initiated or not once a WTOW PLMN search is completed.
01/05/06   vm      Added support for starting the 10s No Service timer after UE
                   enters deep sleep instead of waiting for 12 full scans in deep
                   sleep before starting the 10s No Service timer.
01/05/06   vm      Added support for barring the correct GSM cell when WtoG CCO
                   fails because GSM cell is barred. Now function rrccho_is_wtog_cco_active()
                   is used to determine if CCO is in progress instead of function
                   rrccho_is_wtog_cco_in_progress() (which sets a certain flag due to
                   which UE is taken to idle incorrectly).
01/04/06   vm      Added support for barring the correct GSM cell when WtoG CCO
                   fails because GSM cell is barred.
12/23/05   bu      Added support to fill in the correct signal strength in the available
                   PLMN list by calling l1_get_rssi().
12/21/05   siva    Added support of maximum 40 PLMNs instead of 10 PLMNs in PLMN_LIST_CNF
                   to NAS and also changes for the array bound issue at the time of sending 
                   PLMN_LIST_CNF message to NAS when W+G PLMNs found are more than RRC_MAX_PLMNS
12/15/05   bu      Added support to not read SIB1 and SIB3 during manual PLMN scan. Changes
                   are under FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN.
11/28/05   bu      Added initial support for 900 MHz band under FEATURE_WCDMA_900.
11/23/05   vm      Added support for feature FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION.
                   The list size returned by GSM is initialized to 0.
11/22/05   vm      Added changes related to scanning ALL ACQ DB entries before
                   doing Full Scan. 
11/16/05   vm      Added more support related to BPLMN features.
11/10/05   sgk     Added check for rrc_nv_rel_indicator to be equal to
                   RRC_NV_REL_INDICATOR_REL5 before processing the rel 5 ies. 
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search.
11/01/05   vm      Added required support to try acquisition on the best cell in the active
                   set on Connected Mode -> Idle transition before trying ACQ DB. Earlier this
                   support was present only for DCH->Idle transition and was under a gcf flag.
                   Now the dependence on the GCF flag has also been removed.
10/27/05   bu      Use SIB4, if available, during DCH->FACH and DCH->PCH transitions.
10/26/05   vr      While sending RRC_SERVICE_IND,populate the gsm_nas_common_info
                   if this service_ind is for WTOG Cell Change Order.Typically in 
                   connected mode this information is not included in the service_ind.
                   But for WTOG CCO,NAS relies on this information to detect a change
                   in LAI.
10/21/05   sgk     Added rel5 support for delta qhcs and delta qrxlevmin
                   processing for Sib3 and Sib4.
10/19/05   sgk     Made change to set the s_interat value to 24 when the value
                   received is 10 (actual value 20) which means treat the ie as
                   absent. This will ensure that Squal is always <= SsearchRAT
                   and effectively make the UE behave as if SsearchRAT is absent
                   (min value of Qqualmin is -24).
09/21/05   da      Made change to call func rrc_notify_cell_id_change instead of 
                   rrc_notify_cell_id_change_cb_func to notify serving cell change
09/20/05   da      Calling func rrc_notify_cell_id_change_cb_func() when RRC_SERVICE_IND 
                   or RRC_SERVICE_CNF is sent to MM to notify registered function of 
                   serving cell id.
08/31/05   vm      Added support for scanning the frequency in NV before proceeding
                   to ACQ-DB/Full-scan when RRC_SERVICE_REQ is received by RRC from NAS.
08/24/05   vm      Due to RL failure, max cell selection time for DCH -> FACH 
                   transition is not limited to 3mins when UE is in W only mode. 
08/23/05   vm      Added ANY_PLMN support also in feature FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION.
                   Now for ANY_PLMN search, the flag "is_emergency_call" is set to 
                   TRUE in the interrat redirection request sent to GSM.
08/21/05   bu      Call rr_rrc_set_band_pref() even when UE is in WCDMA only mode.
08/18/05   vm      Modified the changes made in the last check-in.
08/17/05   vm      Added support for not barring a cell if SIM has not been
                   read (if SIM is present).
08/16/05   vm      Added support for feature FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION.
08/05/05   bu      Removed FEATURE_ENHANCED_NW_SELECTION usage as NAS changes are
                   being mainlined.
07/29/05   sgk     Set s_hcsrat_incl = FALSE when the IE is not present.
07/25/05   da      Made change to trigger home zone ind when RRC_SERVICE_IND or
                   RRC_SERVICE_CNF is sent to MM.
07/20/05   vr      Added a new function rrc_csp_send_service_ind() to send 
                   RRC_SERVICE_IND to MM from other procedures.Currently used 
                   by Cell Change Order
07/15/05   vm      Added support for not updating band information for 
                   inter-RAT handover, CCO, and reselections.
                   Function rrccsp_fill_info_for_rr() now takes only 1 argument.
07/08/05   vm      FEATURE_OUT_OF_SERVICE_ENHANCEMENT: Added support for OOS in 
                   Connected Mode feature.
06/25/05   sgk     Added processing of SsearchHCS and Shcs_RAT_incl flag.
06/24/05   vm      Added support for resetting the variable next_band_mask so
                   that UE correctly scans both 1900 and 850 bands during Manual
                   PLMN search (for more details please see CR 67593)
06/23/05   sgk     Included header rrcmeas.h to support MAX_QHCS_RSCP definition.
                   Mapped default QHCS values based on RCSP and ECN0, handled
                   spare values. 
06/21/05   vm      Added required support to try acquisition on the best cell in the active
                   set on DCH->Idle transition before trying ACQ DB. The changes are under a GCF flag.
06/21/05   vm      Added more support for Tbarred feature. Added definition of
                   new function rrc_rr_check_cell_barred() which allows GSM-RR
                   to determine if a particular W cell/frequency is barred or not.
06/06/05   da      Calling function rrc_trigger_home_zone_ind() in func
                   rrc_csp_cmd_to_mm_needed() to indicate the change in cell ID.
06/02/05   vm      Added support for inter-RAT abort handling:
                   initialized new variable rrc_csp_int_data.rsp_to_rr in function
                   rrc_csp_init_int_data().
05/27/05   vm      Added support for 
                   (i)  Sending cell bar request outside of function 
                        rrc_csp_find_cell_bar_status(), 
                   (ii) Sending PSC barred instead of FREQ barred if intra-frequency 
                        neighbor cell's frequency is found to be barred, and
                   (iii)Sending cell bar request after transition failure request is sent
                        to L1 if the neighboring cell is found to be barred
05/17/05   vm      Correctly initialized the variable wcdma_cell_time_to_reselect when
                   cell bar request is sent to L1 for a GSM cell
05/16/05   vm      Added support to send cell bar request with new enum type due to RRC/L1 
                   interface change. As part of the new interface, removed intraf_barred flag
                   and enum cell_type
05/11/05   vm      Removed RRC hack and corresponding GCF flag for starting a smaller T-barred
                   timer because required L1 files have now been checked-in
05/05/05   vm/bu   Added support for T-barred Feature. 
                   (a) Included file rrcrce.h
                   (b) CPHY_CELL_BAR_REQ is sent to L1 with appropriate T-barred timer
                       value and FREQ/PSC barred status in function rrc_csp_find_cell_bar_status()
                   (c) Removed dchtoidle_flag altogether from the file.
                   (d) Added definition of the new function rrccsp_send_cphy_cell_bar_req()
05/02/05   vm      Initialised new variables bplmn_bgnd_mib_rd_cnt and bplmn_bgnd_mib_rd_freq
                   defined in rrc_csp_int_data_type structure. These variables are used to 
                   handle the case where BPLMN search timer is too small to read MIB in background.
04/14/05   vm      Since the feature FEATURE_BARRED_CELL_DURING_CELL_SELECTION is always
                   defined, un-featurized the code.
04/14/05   bu      Added changes for Hard handover search optimizations. Changes are
                   under FEATURE OPTIMIZE SEARCH FOR HHO.
04/11/05   bu      If DCH->IDLE flag is set, ignore the INTRA frequency indicator in SIB3.
                   Set the flag to FALSE while sending SERVICE_CNF/SERVICE_IND with no service.
04/07/05   sgk     Added HCS support.
04/04/05   vm      Added support for inter-RAT redirection feature
                   . Added definition of new function rrc_csp_cleanup_inter_freq_redirection_state().
                   . Included file rr_rrc_if.h for the new interface rr_rrc_set_band_pref()
                     provided by GSM-RR related to frequency band information.
03/31/05   bu      Added definition for rrccsp_is_ue_in_true_limited_service().
03/31/05   bu      Set the wcdma_cell_time_to_reselect appropriately based on t-barred
                   timer value for GTOW reselection/PCCO scan.
03/21/05   ttl     Replace rrc_RadioFrequencyBandFDD_spare6 w/ RRC_FDD1800.
03/04/05   vm      Added support for deep sleep. If service is found when 
                   no service timer is active (which would be the 
                   case if service request is received for a band which has not
                   been scanned since the start of the no service timer, in which
                   case no service timer will be ignored) then stop the 
                   no service timer and reset deep_sleep_no_svc_band_scanned_mask.
                   If atleast one PLMN is found, then do not update the 
                   deep_sleep_no_svc_band_scanned_mask.
                   Do not start the no service timer till at least 
                   RRC_MAX_FULL_SCANS_IN_DEEP_SLEEP number of full scans have been
                   done after UE was put in deep sleep.
03/03/05   vm      Added support for inter-frequency redirection feature. 
                   Added definitions of new functions 
                   rrc_csp_start_sel_redirect_freq_in_connecting_state() and
                   rrc_csp_init_inter_freq_redirection_data().
03/02/05  ttl      Support RRC ASN1 Release 5.
02/24/05   bu      Added change to get the HPLMN by calling reg_sim_hplmn_get()
02/24/05   vm      When no service is returned to MM, then update 
                   deep_sleep_no_svc_band_scanned_mask with the bands scanned in 
                   the last service request. Else, if service is found then
                   reset deep_sleep_no_svc_band_scanned_mask to 0.
02/24/05   bu      In rrc_csp_fill_acq_entry_from_sib() use
                   rrc_get_frequency_band() to set the band_class to be written
                   into the ACQ DB for the acquisition entry. Additionally if
                   FEATURE_WCDMA_MULTIBAND is not enabled, set the band_class to
                   PCS/IMT2K correctly based on whether FEATURE_WCDMA_PCS is
                   defined or not.
02/18/05   bu      If FEATURE_WCDMA_MULTIBAND is not enabled, in
                   rrccsp_is_supported_band(), check for IMT2K/PCS band only
                   based on whether FEATURE_WCDMA_PCS is enabled or not. For
                   all other bands return FALSE.
02/16/05   bu      Added definition for rrc_write_nv_acq_info().
02/16/05   vm      Fixed compilation error due to a piece of code not being
                   under feature (FEATURE_WCDMA_MULTIBAND) definition.
02/15/05   vm      Added support for deep sleep. In particular, setting/resetting of 
                   deep_sleep_acq_db_band_mask, starting and stopping of full frequency scan
                   timer, incrementing full scan counter, and initialization of new band mask.
01/21/05   vm      Added back starting of timer RRCTMR_DCH_FACH_DUE_TO_CU_CELL_SELECTION_TIMER_IN_MS
                   due to RL failure. 
01/17/05   sk      Used new channel config reason for pch/fach reconfiguration due to 
                   sib modification
01/13/05   vm      Removed starting of timer RRCTMR_DCH_FACH_DUE_TO_CU_CELL_SELECTION_TIMER_IN_MS,
                   as related timer T314/T315 is already started in Cell Update procedure.
01/04/05   sk      Added rrc_csp_send_chan_reconfig(void) definition.
12/23/04   bu      Update last_camped_freq_in_connected_mode for OOS in Connecting,
                   OOS in CONNECTED & DCH->FACH/PCH scans.
12/16/04   bu      Reset the next_band_mask to the complete wcdma_band_mask for
                   OOS in connected mode, DCH->FACH/PCH scans whenever we are going
                   to continue to do cell selection till we either find service or
                   the appropriate timers expire and RRC takes UE to idle mode. 
12/03/04   bu      Added definition for rrc_set_ue_in_deep_sleep().
12/02/04   da      Added suport for feature FEATURE_HOME_ZONE_SERVICE
11/22/04   vm      Added support for feature FEATURE_GTOW_BACKGROUND_PLMN_SEARCH.
                   Added initialization of new variables added to rrc_csp_int_data. 
                   Added code for handling new scan types RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN 
                   and RRC_CSP_GTOW_BPLMN_MANUAL_SCAN.
                   Added definition of functions rrc_csp_init_bplmn_data, 
                   rrc_csp_start_bplmn_srch_on_rr_req, rrccsp_update_background_plmn_list,
                   and rrccsp_is_in_bplmn_list.
11/22/04   bu      Added support for physical channel establishment failure
                   handling.
11/18/04   vm      A new timer is started when no service is returned to NAS
                   with empty plmn list when ue is in deep sleep. If a service
                   request is rcvd from NAS before this timer expires then 
                   "no service" is returned immediately.
10/15/04   bu      Fixed the band mask comparison for 850 MHz band.
10/13/04   vm      Fixed lint errors.
10/12/04   bu      Added support to scan additional channels for various WCDMA
                   bands if needed.
10/05/04   bu      Defined rrc_is_arfcn_valid() and rrc_update_band_preference(). These
                   functions will be used by RR to validate the WCDMA ARFCNs and to
                   update the UE's band preference on WCDMA side respectively. 
09/24/04   bu      Set the dest_freq_cycles to RRC_CSP_MAX_NUM_ACQDB_DEST_FREQ_CYCLES
                   for DCH->FACH/PCH scans.
09/22/04   bu      In rrc_csp_release_chls_and_continue_cell_selection(), if UE
                   is in CONNECTED state, since we are releasing the channels which
                   will ensure that CSP will get a STATE_CHANGE_IND (to DISCONNECTED),
                   set the next CSP substate appropriately.
09/21/04   bu      Go to RRCCSP_CELL_SEL_WT_STATE_CHANGE whenever CSP is taking
                   UE from Connected mode to Idle. Added a new function
                   rrccsp_rel_chls_and_go_to_idle().
09/17/04   kc      Added rrccsp_get_supported_band_list to get the list of bands
                   supported by this UE.
09/15/04   bu/sk   Added support to handle Out of service area indication
                   in Connected mode (CELL_FACH/CELL_PCH/URA_PCH).
09/15/04   bu      Threshold acq requests for a specific channel based on AGC.
                   RRC tells L1 to check for AGC 11 out of 12 consecutive service
                   requests on entries in the acquisition database.
09/01/04   bu      Added support for 1800 MHz band.
08/31/04   bu      Added changes to use rrc_csp_band_class_type instead
                   of rrc_csp_band_class_e_type for FEATURE_WCDMA_MULTIBAND.
08/30/04   sgk     fixed lint error Type mismatch (assignment) (enum/enum)
                   in csp_out_cmd_ptr->cmd.rrc_llc_req.l1_req.cell_parm.
                   interrat_info[i].rat_type = rat_list_ptr->value.rat_Identifier;
                   in function rrc_csp_send_l1_cmd by checking if rat type is GSM
                   and assigning the right enum value in other cases the while loop
                   continues.
                   Fixed lint error Type mismatch (assignment) (enum/enum)
                   in rrc_cn_domain_sys_info_ptr->cn_domain_id =  
                   asn1_cn_domain_sys_info_ptr->value.cn_DomainIdentity;
                   in function rrccsp_fill_nas_cn_specific_info caused because 
                   of different enums on LHS and RHS for CN Domain Id by checking for
                   and assigning the right enum values. 
08/26/04   jh      Updated to use new L1 tx_div_ind interface.
08/25/04   bu      Definition for rrccsp_update_acq_db().
08/20/04   bu      Call GSM RR functions to set the band preference anytime a
                   RRC_SERVICE_REQ or RRC_PLMN_LIST_REQ is received.
08/13/04   bu      Added WCDMA multiband support. Defined rrccsp_is_supported_band().
                   Support to scan the next supported band after exhausting one band.
                   Support to set the band class appropriately based on acqusition
                   database entries.
                   Support to set the active band that UE camped on to MM.
                   Defined rrccsp_update_band_preference(). 
08/12/04   bu      If the DCH->FACH transition is due to a reconfiguration message,
                   start a 5 second timer. Otherwise if it is due to CU, start
                   a 20 second timer.
08/11/04   bu      In rrc_csp_cmd_to_mm_needed(), return FALSE, if it is determined
                   that a RRC_SERVICE_IND is to be sent to MM while RRC state is not
                   DISCONNECTED or CONNECTING.
08/09/04   bu      Call rrc_ccm_update_with_cell_loss() whenever initiating
                   cell selection.
08/05/04   bu      Set init_cell_sel_ind_from_cu_during_wtog to FALSE in
                   rrc_csp_init_int_data().
07/03/04   bu      Changes to ignore the INTRA-FREQ cell reselection indicator if
                   UE is camping for emergency services.
07/14/04   bu      In rrccsp_is_state_transition_triggered_by_reconfig_msg(),
                   return FALSE if the transition is triggered by CU procedure.
07/08/04   bu      In rrc_csp_send_next_freq_to_l1(), send an IDLE_REQ to L1 for
                   FINE_SCAN request also. Initialize the num_available_freq variable.
06/25/04   bu      Added MSG_HIGHs in rrccsp_update_plmn_list().
06/15/04   bu      Corrected the check to initialize the dest_freq_in_use and
                   num_tries_on_dest_freq flags in rrc_csp_init_select_data() based on
                   the current scan type.
06/14/04   bu      Added support to store the information regarding when the UE goes
                   into deep sleep and comes out of it. Based on this information a
                   decision is made whether the full frequency scan should be initiated
                   or not. Indicate to L1 in FREQ_SCAN_REQ and ACQ_REQ, if the current
                   scan is for PLMN_LIST search or not.
06/01/04   bu      Added support for band_pref. Added definition for
                   rrccsp_fill_info_for_rr().
05/07/04   bu      In rrccsp_update_plmn_list(), check for requested PLMN only if
                   the current requested NAS PLMN type is not ANY_PLMN.
05/05/04   bu      Added support to start the frequency scan timer when RAW scan
                   is initiated. The full frequency scan is started only if the
                   frequency scan timer has expired. Removed the earlier optimizations
                   which are not needed anymore.
04/20/04   bu      Added SIB4 support - in CONNECTED mode, use SIB4 instead of SIB3.
04/02/04   bu      Added additional support to fill in the forbidden LAC, failure
                   information and feature definitions for GTOW PCCO.
03/16/04   bu      Removed stopping of DCH->FACH or DCH->PCH timers in
                   rrc_csp_release_chls_and_continue_cell_selection().
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
03/13/04   bu      In rrc_csp_release_chls_and_continue_cell_selection if the
                   scan type is RRC_CSP_CELL_RESELECTION_SCAN set it to
                   RRC_CSP_CELL_SELECTION_SCAN.
03/05/04   bu      For ANY_PLMN request removed the check for
                   forbidden registration area.
03/04/04   bu      Added support for CELL_PCH and URA_PCH states.
02/11/04   bu      Don't trigger full frequency scan for a SPECIFIC_PLMN serivce request
                   if at least one match was found in the ACQ_DB.
02/09/04   bu      Fixed singlemode compilation issues.
02/05/04   bu      In rrc_csp_release_chls_and_continue_cell_selection(), removed
                   setting of scan type to RRC_CSP_CELL_SELECTION_SCAN. The scan is
                   set only when RRC gets the MM/L1/RR commands that trigger a change
                   in cell selection scan.
01/29/04   bu      If the LAI is forbidden for GTOW reselection, save the LAI
                   information. This will be sent in the reject to RR. Also
                   set the failure_reason appropriately.
01/21/04   bu      Removed clearing of num forbidden LAIs from rrc_csp_init_int_data().
                   It's done in rrc_init_cell_selection_procedure().
01/06/04   bu      Fix for CR38647. Initialize the cell select quality measure
                   correctly from SIB11/SIB12. 
12/16/03   bu      For manual PLMN selection, don't check for forbidden LAIs
                   while updating the PLMN list. Update CSP internal
                   data with the network_select_mode received in RRC_SERVICE_REQ.
12/16/03   bu      Fill sttd information while sending CPHY_ACQ_REQ to L1.
12/08/03   bu      Fill in the GSM map common information in the SERVICE_IND to MM
                   only if RRC state is not CELL_FACH/URA_PCH/CELL_PCH.
11/07/03   bu      Added a check to avoid acquisition on a frequency which has
                   already been scanned as part of acq database scan.
10/29/03   bu      Changed rrccsp_get_neighbor_pccpch_parms() to fill in
                   the frequency.
10/08/03   bu      For GTOW cell reselection save the Qqualmin and
                   (Qrxlevmin + Pcompensation) information to send back to RR,
                   if the reselection attempt fails due to S criteria not being met.
08/11/03   bu      Added support for the emergency call restriction based on
                   Access Class restrictions in SIB3.
07/16/03   bu      Removed initialization of future cell reselection boolean as it's
                   no longer used.
07/11/03   bu      If the cell is reserved for future extension, treat the cell
                   as barred using the value "not allowed" in the IE "Intra-frequency
                   cell re-selection indicator" in rrc_csp_find_cell_bar_status().
07/01/03   bu      If the cell is not barred but reserved check if UE's Hi Access
                   Class is 11 of 15 and select/re-select this cell if in the
                   home PLMN, otherwise treat the cell as barred.
06/04/03   bu      While sending ACQ_REQ to L1 in rrc_csp_send_next_freq_to_l1()
                   set the ACQ mode correctly. Added support barred cell during
                   cell selection. All the changes are under
                   FEATURE_BARRED_CELL_DURING_CELL_SELECTION.
06/02/03   vn      When leaving Connected mode and on an OutofService area,
                   do not do a full freq scan if UE fails to camp on one
                   of the frequencies in Acq DB.
05/22/03   bu      Set the s_interrat_incl flag to TRUE/FALSE depending on the
                   presence of rrc_fdd_rat_list_present IE in SIB3.
04/30/03   bu      Converted the values for s_intrasrch, s_intersrch, s_interrat
                   s_limit_srchrat, s_hcsrat and q_Hyst_l_S received from SIB3 to
                   their actual value while sending the CPHY_CELL_PARM_REQ to L1.
04/25/03   bu      Filled the cause while sending ACTIVATION_IND to MM.
04/24/03   bu      Removed FEATURE_L1_ACT_TIME usage. Activation time will be
                   initialized in the l1_cmd_hdr_type.
04/17/03   bu      Stop the DCH->FACH timer in 
                   rrc_csp_release_chls_and_continue_cell_selection() if current
                   scan is RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN.
04/10/03   vn      No need to call measurement saving function here as it is
                   moved to LLC. This was needed to take care of issues due 
                   to queueing of RRC_RESET_L1_REQ in LLC.
04/03/03   bu      Changed pri_scr_code to scr_code in l1_acq_cmd_type.
04/01/03   vn      Save IntraFreq Meas Criteria before sending CPHY_IDLE_REQ
                   while in RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN. Invoking of
                   CSP-MEAS-L1 interface function call ensures L1 does not 
                   clear Meas reporting criteria on the next CPHY_IDLE_REQ.
02/28/03   bu      In rrccsp_find_access_control fixed the check for
                   rrc_hi_access_class. It is now checked with RRC_HI_ACCESS_CLASS_ABSENT
                   instead of RRC_HI_ACCESS_CLASS_MAX. In rrc_csp_verify_hi_ac_access
                   the home_plmn_id is set correctly.
02/27/03   vn      Added support for DCH->FACH directed freq and psc. Also
                   implemented timer going DCH->FACH, improvement in cell 
                   selection algorithm to check multiple times for directed
                   frequency and acquisition database before starting full
                   frequency scan. Removed processing for redundant acq types.
                   Removed function: rrc_csp_start_stored_initial_select_ctrl_
                   _in_curr_acq_type, changed name of rrc_csp_start_stored_
                   initial_select_ctrl to rrc_csp_start_cell_selection. Provided
                   actual implementation of rrccsp_is_selected_cell_utran_directed.
                   In rrc_csp_release_chls_and_continue_cell_selection, reset
                   curr_scan to CELL_SELECTION_SCAN.
02/25/03   ram     Even if SIB11 indicates that SIB12 is present, it is not gauranteed
                   to be present in the cell. Hence if SIB12 is not present in the
                   SIB database we should assume it was not present in the cell and use
                   SIB11 instead. Earlier we were ERR_FATALing if SIB12 is not present                   
02/21/03   kc      Corrected Feature Defintion by removing HO feature where not
                   needed
02/18/03   kc      Included header file rrcmcm.h
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to 
                   INTERRAT_HANDOVER_GTOW
02/14/03   bu      Update the activation cause (RRCMCM_INTERRAT_CELL_RESELECTION)
                   in MCM while sending the Activation Indication.Change under 
                   FEATURE_INTERRAT_HANDOVER_GTOW_RRC.
01/31/03   bu      Check available PLMN Id with NAS PLMN Id if the current scan
                   is not RRC_CSP_PLMN_LIST_SCAN in rrccsp_update_plmn_list().
01/31/03   bu      Initialized the cell_change_type to RRC_CHANGE_CELL_SELECTION if
                   the scan type is RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN in
                   rrc_csp_send_ccm_cmd(). An error will be flagged if the scan type
                   is not known.
01/30/03   vn      Removed lint errors.
01/29/03   bu      Added support to default the cell reselection parameters to 0 in
                   case negative values are received. If SIB11/SIB12 is present then
                   fill in the cell select quality measure from it otherwise from
                   SIB3.
01/24/03   xfg     Removed the check for the available PLMN Id with NAS PLMN Id.
12/09/02   bu      Add the PLMN Id to the available plmn list only if the
                   BCCH and NAS requested PLMNs don't match.
11/22/02   bu      Added support for more types in l1_wcdma_acq_mode_type 
                   to support GSM to WCDMA idle mode cell reselection. This
                   structure is used while sending the CPHY_ACQ_REQ. Changes
                   have been made under the approprate feature definitions in
                   rrc_csp_send_l1_cmd().
11/19/02   bu      Added support for manual PLMN selection. Fill the appropriate
                   fields in the PLMN_LIST_CNF and send it to MM. Re-initialize
                   the CSP internal data after sending the confirm to MM. Added
                   rrc_csp_start_sel_for_plmn_list() to do scan all available PLMNs
                   by scanning the entire frequency band. Setting the status
                   field in the PLMN_LIST_CNF to MM_AS_SUCCESS.
11/19/02   bu      Added support to handle the mandatory default parameters related
                   to cell reselection in CELL_PARM_REQ.
11/15/02   bu      Support for L1-RRC interface changes. Use
                   l1_cell_sel_resel_info_struct_type in l1_cell_info_struct_type.
10/30/02   bu      Added support to set num_rats and rat list information in
                   CELL_PARM_REQ.
10/11/02   bu      Added support for GSM to WCDMA cell reselection. Added
                   rrc_csp_send_mm_act_ind() to send the activation indication
                   to MM on a successful cell reselection. All the changes are
                   under FEATURE_INTERRAT_CELL_RESELECTION_GTOW.
10/01/02   vn      Added stubs for rrccsp_is_selected_cell_utran_directed and
                   rrccsp_is_state_transition_triggered_by_reconfig_msg
09/22/02   vn      Added processing to handle cell selection on a state 
                   transition from CELL_DCH to CELL_FACH. Added a new functions
                   rrc_csp_start_sel_cell_dch_to_cell_fach and rrc_csp_send_tr_
                   phy_chl_release. Cleanup of logic for doing indefinite scan -
                   indefinite scan not based on FEATURE_FOREVER_SCAN but on the
                   scan type now.
09/19/02   vn      Moved rrc_csp_send_l1_drx_req from CCM as DRX command will
                   now be sent by CSP.
09/09/02   bu      Removed the intialization of the SIM parameters - lo access
                   class, high access class, imsi, tmsi & ptmsi from
                   rrc_csp_init_int_data().
08/07/02   vn      Work with new Service Request that does not have SIM 
                   parameters except for Forbidden LAI List.
06/19/02   bu      Removed references to RRC_ACT_CNF in rrc_csp_mm_primitive_sent()
                   and rrc_csp_cmd_to_mm_needed() for Dualmode.
06/19/02   bu      Removed rrc_csp_send_mm_act_cnf_cmd() for Dualmode.
06/14/02   vn      Dualmode changes: When a scan is done and if feature 
                   FEATURE_FOREVER_SCAN is not defined, put lower layers to Idle 
                   state. In rrc_csp_continue_stored_initial_select_ctrl, do not
                   change the Current Acquisition Type in Internal Database to
                   ensure an acceptable cell is not selected when specific PLMN
                   is requested by NAS. Fill up available PLMN list in Service 
                   Confirm from internally stored list.
06/14/02   bu      Fixed a merged typo - added #endif after rrccsp_update_plmn_list().
06/14/02   bu      Use mm_as_service_state_T instead of rrc_service_state_e_type
                   and mm_as_cell_access_status_T instead of
                   rrc_cell_access_status_e_type for DUALMODE. Added a utility to
                   add a particular PLMN to the PLMN list to be returned to MM in the
                   SERVICE_CNF if we find a cell which is not barred and is not in
                   the forbidden LAI list.
06/05/02   bu      In rrc_csp_send_sib_cmd() filled the procedure field in 
                   GET_SPECIFIC_SIBS_REQ and GET_ALL_SIBS_REQ.
05/16/02   jh      Changed to use AC info from global data.
05/16/02   bu      Merged the following from cell reselection branch:
           bu      Changed the pri_scr_code field to scr_code in transition req and
                   serving_pccpch_parms.
           bu      Added changes related to CPHY_CELL_PARM_REQ and
                   CPHY_CELL_TRANSITION_REQ. Added a new function 
                   RRCCSP_GET_NEIGHBOR_PCCPCH_PARMS to get the scrambling code of
                   the neighbor cell PCCPCH.
05/13/02   bu      Changed the avail_lai_list to avail_plmn_list in rrc_service_cnf.
                   Added rrc_csp_send_mm_plmn_list_cnf_cmd() to send the confirm
                   to MM. These changes have been added with
                   FEATURE DUALMODE BASELINE tag.
04/15/02   bu      Removed definitions for rrc_csp_form_cand_list_from_sib11(),
                   rrc_csp_form_interim_cand_list(), rrc_csp_fill_cand_list_s_values(),
                   rrc_csp_form_rank_list_from_cand_list(),
                   rrc_csp_get_next_rank_list_cell(), rrc_csp_get_first_rank_list_cell(),
                   rrc_csp_remove_curr_rank_cell(), rrc_csp_get_freq_index() and
                   rrc_csp_remove_curr_intra_freq_rank_cells(). Send the correct value of
                   q_RxlevMin in the CELL_PARM_REQ to L1 in accordance with the specs.
                   Made modifications to use the new CPHY_CELL_SELECTION_REQ command in
                   rrc_csp_send_l1_cmd(). Use acq_entry_ptr instead of
                   rrc_csp_util_parm_ptr_u_type in various functions.
04/08/02   vn      Added additional debug info in rrc_csp_compare_expected_cell
                   and rrc_csp_find_cell_bar_status.
03/13/02   bu      Featurized the changes related to Activation Time in
                   rrc_csp_send_l1_freq_scan_cmd() and rrc_csp_send_l1_cmd().
01/30/02   vn      Include bsp.h in place of bsp_rf_jerry1.h
01/25/02   vn      Took out processing for CPHY_MEASUREMENT_REQ as it is being 
                   done by the Measurement procedure now.
12/21/01   vn      Typecast scr code returned in SIB CNF to rrc_scr_code_type. 
12/12/01   bu      Initialized future_cell_reselection in rrc_csp_init_int_data().
                   Changed rrc_csp_send_sib_cmd() to include a boolean that indicates
                   whether the cell is a serving or not.
12/05/01   kc      More changes for June spec conversion.  Mainly asn changes
12/05/01   kc      Changes for June spec conversion.  Mainly asn changes
10/31/01   vn      Defined and added a call to rrc_csp_bcch_and_nas_lais_match
                   for comparing LAI of the acquired cell with LAIs in forbidden
                   LAI list. Added call to Is_bcch_plmn_an_hplmn to determine if
                   acquired cell belongs to HPLMN. 
10/29/01   vn      Fixed compiler warning.
10/25/01   vn      Added a new function rrc_csp_start_sel_out_of_service_area_
                   in_idle_state to handle Out of Service area scan in
                   Disconnected and Connecting states. All channels and pipes 
                   are released by this function.
10/22/01   vn      Support for Out of Service Area - Added intermediate functions
                   to start cell selection on leaving connected mode, on out of 
                   service area ind, on acting on a new service request. 
                   Utilities are added to continue existing cell selection and to
                   release all cahnnels and continue existing cell selection. 
                   The CSP events are changed to last MM primitives. Last MM 
                   primitives are updated on sending a command to MM. Introduced 
                   current scan type to differentiate between scans due to Out of
                   service area, leaving connected mode, Radio Link failure etc.
                   The cell transition type in CELL_CHANGE_REQ to CCM is set based
                   on the current scan type.
10/12/01   vn      Added utilities 1. to determine which command needs to be sent
                   to MM based on the last CSP event and current service status,
                   2. to find the new CSP event based on the command sent to MM.
                   3. to store data from RRC_SERVICE_REQ command.
09/26/01   vn      Added new function rrc_csp_send_chan_release that sends an
                   RRC_CHANNEL_CONFIG_REQ to release all channels. Added a function
                   rrc_csp_start_sel_leaving_conn_mode to start cell selection on
                   leaving connected mode.
09/17/01   vn      Added a function to send LAI_LIST_CNF for partial LAI_LIST_REQ
                   support. Note that the list has current PLMN if camped on a 
                   cell and has no element if not camped on a cell.
09/12/01   vn      Correctly set new_cell_access field in CELL_CHANGE_REQ to CCM.
08/31/01   vn      Support for frequency scan. rrc_csp_continue_stored_initial_select_ctrl
                   now calls rrc_csp_get_next_freq_scan_entry to get the next 
                   frequency for scanning once acquisition database is exhausted.
                   It further returns the type of frequency scan to the caller.
                   Added new functions rrc_csp_send_next_freq_to_l1 and 
                   rrc_csp_handle_no_available_freq.
08/08/01   vn      Corrected logic in rrc_csp_find_cell_bar_status that handles
                   cell barred indication. Added rrc_csp_find_rrc_cell_access, 
                   that calculates cell access status based on cell restrictions and
                   access control. Calls to rrc_csp_find_rrc_cell_access while
                   sending Cell Change Req, RRC Service Cnf and RRC Service Ind. 
07/06/01   vn      In rrc_csp_send_ccm_cmd, changed to not fill in cell specific
                   fields (PLMN, Cell ID, Scrambling code, Freq) in the command 
                   if this is a case of Cell Loss.
06/19/01   vn      Set the num_cn_domain correctly in RRC_SERVICE_IND and 
                   RRC_SERVICE_CNF going to MM.
06/13/01   vn      Added provision to send RRC_SERVICE_IND. Consolidated all MM
                   command sending functionality in rrc_csp_send_mm_cmd(). Added
                   rrccsp_fill_nas_cn_specific_info and rrccsp_fill_nas_common_info
                   to fill in NAS CN domain specific and common info resp.
06/06/01   vn      Provided function rrccsp_get_serving_pccpch_parms to fill up
                   Serving cell PCCPCH parameters. 
05/24/01   vn      Upgraded to work with L1 VU MSM5200_L1.00.02.94. A fieldname
                   intra_f_dch_rpt_info_incl got changed in l1rrcif.h
05/23/01   vn      Fixed a bug. Returned value for mm_extract_lac_from_sib_info
                   is to be compared as a boolean (TRUE means success).
05/18/01   vn      Initialize PLMN digits after call to mm_extract_lac_from_sib_info.
05/16/01   vn      Added rrc_csp_send_mm_act_cnf_cmd to send a RRC_ACT_CNF 
                   with a "NO SERVICE AVAILABLE" status.
05/09/01   vn      Changed to compile with the new definition of l1_cell_parm_cmd_type.
04/29/01   rj      Fixed a typo.
04/17/01   rj      Modified function rrc_csp_form_rank_list_from_cand_list to 
                   fix the rank list index for the acquired entry.
03/30/01   ram     Included customer.h header file.
03/30/01   rj      Included mmtask.h. Updated LAC field to the latest rrcmmif.h.
                   Modified rrc_csp_send_mm_ser_cnf_cmd function and fixed 
                   compilation errors. Removed MM_UT_READY feature.
03/20/01   vn      Added changes to work with the latest rrcmmif.h version 1.14. 
                   The changes here are due to changes in RRC_SERVICE_REQ, 
                   RRC_SERVICE_CNF and RRC_SERVICE_IND command data structures
                   in rrcmmif.h. References to forbidden RAI lists have also 
                   been removed. Utility to fill all fields of RRC_SERVICE_CNF.
03/19/01   vn      In rrc_csp_send_llc_cmd, don't care for passed NULL pointer.
03/16/01   vn      Changes for simplified cell selection: do not form a 
                   candidate list so that CSP settles straightaway for the
                   original cell. Added a new utility rrc_csp_send_llc_cmd
                   to send non-funneling commands to LLC. Updated to the new
                   l1_meas_sib_parm_struct_type based on 1.14 of l1rrcif.h.
                   Changes to accomodate new LED ASN1 header file. Support
                   for Cell Transition Req and RRC Reset Req.
03/02/01   vn      Added a utility rrc_csp_form_interim_cand_list to form 
                   interim candidate list from Acq entry.
02/13/01   vn      Removed usage of RRC_CSP_SPEC_PLMN_BIT as it was redundant.
                   Fixed SIB11 linked list processing in rrc_csp_send_l1_cmd
                   and rrc_csp_form_cand_list_from_sib11.
                   Fixed bugs during unit testing of rrc_csp_send_l1_cmd.
02/09/01   vn      Bugfix: Updated rrc_csp_verify_hi_ac_access and 
                   rrc_csp_verify_hi_ac_not_barred to shift bitmask.Added and
                   modified debug messages.
02/08/01   vn      Added support for Acceptable Cell Selection. Changed to 
                   search for non-barred, non-forbidden area cells of Any other
                   PLMN if such cells of specific PLMN are not found.
                   Moved compare_expected_cells from rrccspdb.c
01/30/01   vn      Updates to work with LLC command funneling scheme. Changed
                   to release cmd buffers when cmd is not sent (due to any 
                   reason)after a cmd buffer has been allocated.
01/22/01   vn      Added check to verify the presence of Access Class barred 
                   list in SIB3 before accessing it at all places. Split 
                   rrc_csp_stored_initial_select_ctrl to a Start and Continue
                   version for ease with implementation.
01/19/01   vn      Changed to work with new names and fields in l1rrcif.h r1.5
01/12/01   vn      Added utilities for forming and maintaining Candidate List 
                   and Rank List and for sending commands to L1, SIB, MM and
                   CCM. Also added a utility for filling Acq entry from SIB.
12/29/00   vn      Created file.

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
#include "msg.h"
#include "err.h"
#include "uecomdef.h"
#include "task.h"
#include "IxErrno.h"
#include "rrcasn1.h"
#include "rrcmmif.h"
#include "rrcintif.h"
#include "rrccmd_v.h"
#include "l1rrcif.h"
#include "l1task_v.h"

#include "rrccspi.h"
#include "rrcsibdb.h"
#include "rrcsibproc.h"
#include "rrcccm.h"
#include "rrccsp.h"
#include "rrcllc.h"
#include "rrcscmgr.h"
#include "rrcmcm.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrctmr.h"
#include "mm_umts.h"
#include "rrccu.h"
#include "rrccui.h"
#include "rrcueci.h"
#include "rrcrce.h"
#include "rrccho.h"
#include "rrcdata_v.h"
#include "assert.h"
#include "rrcpg1.h"
#include "rrcmisc.h"

#ifdef RF_MULTIMODE
#include "rfm.h"
#else /* RF_MULTIMODE */
#include "rf.h"
#endif /* RF_MULTIMODE */

#include "sys_eplmn_list.h"
#ifdef FEATURE_FEMTO_CSG
#include "sys.h"
#endif
#ifndef TEST_FRAMEWORK
#include "policyman.h"
#endif
#ifdef FEATURE_EQUIVALENT_HPLMN
#include "reg_sim.h"
#endif

#include "rr_rrc_if.h"
#include "reg_sim.h"
#include "rrcmeas.h"
#include "rrcllcoc.h"
#include "rrclogging.h"

#include "rrcasn1Def.h"

#ifdef FEATURE_LTE_TO_WCDMA
#include "wcdma_rrc_api.h"
#endif

#if defined(FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
#include <lte_rrc_ext_api.h>
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
#include "rrcrbe.h"
#endif

#include "rrcdt.h"
#include "rrcnv.h"
#include "rrccmdi.h"

#ifdef FEATURE_WCDMA_BAND_SPEC_DC_HSDPA
#include "rfm_wcdma.h"
#endif

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
#include "rrcfreefloating.h"
#endif

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif
#include "rrcmeasi.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
 
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* This data structure stores the Selection data for Initial and Stored
 * Selection Controller.
 */

extern boolean rrc_bst_band_pref_incl;

rrc_csp_select_ctrl_data_type  rrc_csp_curr_select_data;
extern boolean rrc_ifreq_rdr_in_prgrs;
rrc_csp_select_ctrl_data_type  prev_rrc_csp_curr_select_data;
extern boolean  bplmn_data_saved;

/* Taking a backup of service request payload received from NAS */
rrc_service_req_type rrc_csp_service_req;
boolean rrc_csp_internal_service_req=FALSE;

sys_specialization_t  search_order_specialization = SYS_SPECIAL_NONE;


extern sys_hs_ind_e_type hsdpa_hsupa_ind;

extern RRC_HOME_ZONE_IND_CB_TYPE *rrc_home_zone_ind_cb_func;

/* When GCF flag is enabled then UE should not go into deep sleep
 * when UE is OOS in RRC Connected Mode
 */
extern boolean    nv_gcf_test_flag;


extern rrc_csp_acq_entry_type  list_db[MAX_ACQ_DB_ENTRIES];

#ifdef FEATURE_DUAL_SIM
extern boolean dsim_bplmn_search_complete_w_cycle;
extern rrc_ds_tuneaway_status_change_ind_type ds_tuneaway_status_change_ind;
#endif

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
extern rrcrce_csfb_status_e_type csfb_extended_status;
extern boolean rrc_cs_rab_established;
#endif

#ifdef FEATURE_FEMTO_CSG
extern boolean bplmn_cell_found_in_drx;
#endif

extern uint32 wcdma_rrc_freq_scan_timer_nv;
extern uint32 wcdma_rrc_deep_sleep_no_svc_timer_nv;

extern boolean rrc_rplmn_info_prsnt;

/* Global to hold the last sent service indication data */
rrc_service_ind_type   rrc_service_ind_bkup;
#ifdef FEATURE_LTE_TO_WCDMA
extern rrc_deprio_db_type rrc_deprioritization_db;
#endif

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */



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


#ifdef FEATURE_BPLMN_SEARCH_320MS
/*===========================================================================

FUNCTION          RRC_CSP_CHECK_320MS_BPLMN

DESCRIPTION       This functions checks if CSP is required to do 320ms lossy BPLMN

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
boolean rrc_csp_check_320ms_bplmn(void)
{
  boolean ret_val=FALSE;
  if( (rrc_csp_int_data.bplmn_nw_sel_mode != SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
      ((rrc_get_state() == RRC_STATE_CELL_PCH) || (rrc_get_state() == RRC_STATE_URA_PCH)) && 
      (rrcllc_get_drx_info() == (MIN_WTOW_NORMAL_BPLMN_DRX_CYCLE_LEN/2))
     )
  {
    ret_val=TRUE;
  }
  return ret_val;
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*===========================================================================

FUNCTION          RRC_SUB_SFN

DESCRIPTION       This function subtract two system frame numbers.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
uint16 rrc_sub_sfn(uint16 sfn, uint16 subtract_by)
{
  if (sfn < subtract_by) 
  {
   return (sfn + 4096 - subtract_by);
  }
  else 
  {
    return (sfn - subtract_by);
  }
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#endif


/* -----------------------------------------------------------------------
** Local Defines
** ----------------------------------------------------------------------- */

/* Converts hcs tcr_max_hyst from ASN1 format to L1 format */

#define  RRCCSP_RET_HCS_TCR_MAX_HYST(max_hyst)  \
(  \
 (max_hyst == rrc_T_CRMaxHyst_notUsed) ? L1_T_CR_MAX_HYST_NOT_USED : \
  ((max_hyst == rrc_T_CRMaxHyst_t10) ? L1_T_CR_MAX_HYST_10S : \
  ((max_hyst == rrc_T_CRMaxHyst_t20) ? L1_T_CR_MAX_HYST_20S :\
  ((max_hyst == rrc_T_CRMaxHyst_t30) ? L1_T_CR_MAX_HYST_30S :\
  ((max_hyst == rrc_T_CRMaxHyst_t40) ? L1_T_CR_MAX_HYST_40S :\
  ((max_hyst == rrc_T_CRMaxHyst_t50) ? L1_T_CR_MAX_HYST_50S :\
  (max_hyst == rrc_T_CRMaxHyst_t60) ? L1_T_CR_MAX_HYST_60S : L1_T_CR_MAX_HYST_70S \
  ))))) \
)

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

 /*===========================================================================

FUNCTION          RRC_CSP_BCCH_AND_NAS_LAIS_MATCH

DESCRIPTION       This function is responsible for comparing LAI received
                  on BCCH with one sent by NAS. It declares a match if both 
                  LAC and PLMNs match.
DEPENDENCIES
                  None
RETURN VALUE
                  TRUE: LAIs match
                  FALSE: LAIs don't match
SIDE EFFECTS
                  None
===========================================================================*/
static boolean  rrc_csp_bcch_and_nas_lais_match(rrc_lai_type bcch_lai, rrc_lai_type nas_lai)
{
  uint8  i;

  /* If both the LAC and PLMNs match, declare a match for LAI */
  if(rrc_bcch_and_nas_plmns_match(bcch_lai.plmn, nas_lai.plmn))
  {
    for(i=0; i<RRC_MAX_GSMMAP_LAC_LENGTH; i++)
    {
      if(bcch_lai.lac[i] != nas_lai.lac[i])
      {
        return(FALSE);
      }
    }
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* rrc_csp_bcch_and_nas_lais_match */

/*===========================================================================

FUNCTION          rrccsp_get_supported_band_list

DESCRIPTION       This function checks which bands are supported and sends the
                  populated list to UECI.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
void rrccsp_get_supported_band_list(rrcueci_band_storage_type * ueci_band)
{
#ifndef FEATURE_WCDMA_MULTIBAND
  ueci_band->band_type_present[rrc_fdd1900]=TRUE;
  ueci_band->band_type[ueci_band->num_supported_bands]=(uint32)rrc_RadioFrequencyBandFDD_fdd1900;
  ueci_band->num_supported_bands++;

#else

  if(   (RRC_CSP_IMT2K_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_IMT2K_BAND, FALSE))
    )
  {
    ueci_band->band_type_present[rrc_RadioFrequencyBandFDD_fdd2100]=TRUE;
    ueci_band->band_type[ueci_band->num_supported_bands]=(uint32)rrc_RadioFrequencyBandFDD_fdd2100;
    ueci_band->num_supported_bands++;
  }

  if((RRC_CSP_PCS_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_PCS_BAND, FALSE))
    )
  {
    ueci_band->band_type_present[rrc_RadioFrequencyBandFDD_fdd1900]=TRUE;
    ueci_band->band_type[ueci_band->num_supported_bands]=(uint32)rrc_RadioFrequencyBandFDD_fdd1900;
    ueci_band->num_supported_bands++;
  }

  if((RRC_CSP_WCDMA_850_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_WCDMA_850_BAND, FALSE))
    )
  {
    ueci_band->band_type_present[rrc_RadioFrequencyBandFDD_bandV]=TRUE;
    ueci_band->band_type[ueci_band->num_supported_bands]=(uint32)rrc_RadioFrequencyBandFDD_bandV;
    ueci_band->num_supported_bands++;
  }

  if((RRC_CSP_WCDMA_800_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_WCDMA_800_BAND, FALSE))
    )
  {
    ueci_band->band_type_present[rrc_RadioFrequencyBandFDD_bandVI]=TRUE;
    ueci_band->band_type[ueci_band->num_supported_bands]=(uint32)rrc_RadioFrequencyBandFDD_bandVI;
    ueci_band->num_supported_bands++;
  }


  if((RRC_CSP_WCDMA_1800_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_WCDMA_1800_BAND, FALSE))
    )
  {
    ueci_band->band_type_present[rrc_RadioFrequencyBandFDD_fdd1800]=TRUE;
    ueci_band->band_type[ueci_band->num_supported_bands]=(uint32)rrc_RadioFrequencyBandFDD_fdd1800;
    ueci_band->num_supported_bands++;
  }


  if((RRC_CSP_WCDMA_900_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_WCDMA_900_BAND, FALSE))
    )
  {
    ueci_band->band_type_present_fdd2[rrc_RadioFrequencyBandFDD2_bandVIII]=TRUE;
    ueci_band->band_type_fdd2[ueci_band->num_supported_bands_fdd2]=(uint32)rrc_RadioFrequencyBandFDD2_bandVIII;
    ueci_band->num_supported_bands_fdd2++;
  }

  if((RRC_CSP_WCDMA_1700_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_WCDMA_1700_BAND, FALSE))
    )
  {
    ueci_band->band_type_present_fdd2[rrc_RadioFrequencyBandFDD2_bandIX]=TRUE;
    ueci_band->band_type_fdd2[ueci_band->num_supported_bands_fdd2]=(uint32)rrc_RadioFrequencyBandFDD2_bandIX;
    ueci_band->num_supported_bands_fdd2++;
  }

  if((RRC_CSP_WCDMA_1700_2100_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_WCDMA_1700_2100_BAND, FALSE))
    )
  {
    ueci_band->band_type_present[rrc_RadioFrequencyBandFDD_bandIV]=TRUE;
    ueci_band->band_type[ueci_band->num_supported_bands]=(uint32)rrc_RadioFrequencyBandFDD_bandIV;
    ueci_band->num_supported_bands++;
  }

  if((RRC_CSP_WCDMA_1500_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_WCDMA_1500_BAND, FALSE))
    )
  {
    ueci_band->band_type_present_fdd2[rrc_RadioFrequencyBandFDD2_bandXI]=TRUE;
    ueci_band->band_type_fdd2[ueci_band->num_supported_bands_fdd2]=(uint32)rrc_RadioFrequencyBandFDD2_bandXI;
    ueci_band->num_supported_bands_fdd2++;
  }

  if((RRC_CSP_WCDMA_BC19_BAND & rrc_csp_int_data.wcdma_band_mask)
     || (rrccsp_is_band_class_enabled_in_ui_rf(RRC_CSP_WCDMA_BC19_BAND, FALSE))
    )
  {
    ueci_band->band_type_present_fdd2[rrc_RadioFrequencyBandFDD2_bandXIX]=TRUE;
    ueci_band->band_type_fdd2[ueci_band->num_supported_bands_fdd2]=(uint32)rrc_RadioFrequencyBandFDD2_bandXIX;
    ueci_band->num_supported_bands_fdd2++;
  }

#endif /* FEATURE_WCDMA_MULTIBAND */

} /* rrccsp_get_supported_band_list */

/*===========================================================================

FUNCTION          RRC_IS_BCCH_PLMN_AN_HPLMN_OR_EHPLMN

 DESCRIPTION       This function 
                          If FEATURE_EQUIVALENT_HPLMN is not defined,
                             calls Is_bcch_plmn_an_hplmn 
                           Else
                             calls reg_sim_is_ehplmn
                           When FEATURE_DUAL_SIM id defined, takes care of calling appropriate functions

DEPENDENCIES
                  None

RETURN VALUE
                  TRUE: Access allowed on this cell
                  FALSE: Access NOT allowed on this cell.

SIDE EFFECTS
                  None
===========================================================================*/
static boolean rrc_is_bcch_plmn_an_hplmn_or_ehplmn
(
  rrc_plmn_identity_type plmn_id
)
{
  boolean is_hplmn_or_ehplmn = FALSE;

#ifdef FEATURE_EQUIVALENT_HPLMN
  sys_plmn_id_s_type nas_plmn_id;
#endif

#ifndef FEATURE_EQUIVALENT_HPLMN

#ifdef FEATURE_DUAL_SIM
  is_hplmn_or_ehplmn = mm_per_subs_is_bcch_plmn_an_hplmn(plmn_id,rrc_get_as_id());
#else
  is_hplmn_or_ehplmn = Is_bcch_plmn_an_hplmn(plmn_id);
#endif /*FEATURE_DUAL_SIM*/

#else
  nas_plmn_id = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn_id);

#ifdef FEATURE_DUAL_SIM
  is_hplmn_or_ehplmn = reg_sim_per_subs_is_ehplmn(nas_plmn_id,rrc_get_as_id());
#else
  is_hplmn_or_ehplmn = reg_sim_is_ehplmn(nas_plmn_id);
#endif /*FEATURE_DUAL_SIM*/

#endif

  return is_hplmn_or_ehplmn;
}
/*===========================================================================

FUNCTION          RRC_CSP_VERIFY_HI_AC_ACCESS

DESCRIPTION       This function is responsible for finding if access to cell 
                  is allowed due to one of UE's Hi Access Classes.

DEPENDENCIES
                  None

RETURN VALUE
                  TRUE: Access allowed on this cell
                  FALSE: Access NOT allowed on this cell.

SIDE EFFECTS
                  None
===========================================================================*/
static boolean 
rrc_csp_verify_hi_ac_access (rrc_plmn_identity_type plmn_id,
                             rrc_int_access_class_barred_list_type  ac_barred_list)
{
  rrc_int_access_class_index_e_type i;
  uint16 tmp_shift_bit_mask = RRC_ACCESS_CLASS_11_BIT;

  rrc_plmn_identity_type home_plmn_id;

#ifdef FEATURE_DUAL_SIM
  home_plmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(
                            reg_sim_per_subs_hplmn_get(rrc_get_as_id()));
#else
  home_plmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(reg_sim_hplmn_get());
#endif  


  /* Check the UE Hi Access Class Bitmask to verify access due to any of the
   * UE Hi Access Classes
   */
  for( i = RRC_INT_ACCESS_CLASS_11_INDEX, tmp_shift_bit_mask = 0x0001; 
       i < RRC_INT_ACCESS_CLASS_MAX_INDEX;
       i++, tmp_shift_bit_mask <<= 1 )
  {
    if(( rrc_hi_access_class & tmp_shift_bit_mask ) && 
       ( ac_barred_list.barred[i] == FALSE ))
    {
      /* The Cell is accessible for UE Hi Access Class. Now check if we
       * fulfill rules about Applicability of Access Class in serving network
       * Refer to 22.011, sec. 4.3
       */
      /* Get the home_plmn_id from NAS */ 
      rrc_csp_int_data.home_plmn_id = home_plmn_id;

      if(((i == RRC_INT_ACCESS_CLASS_11_INDEX) ||
          (i == RRC_INT_ACCESS_CLASS_15_INDEX)) && 
          (rrc_is_bcch_plmn_an_hplmn_or_ehplmn(plmn_id)))
      {
        /* We are in Home PLMN and UE Access Class is 11 or 15 - Full Access
         * is allowed.
         */
         WRRC_MSG1_HIGH("In HPLMN UE Hi AC %d  - Access Allowed", i);
         return(TRUE);
      }
      else if ( ((i == RRC_INT_ACCESS_CLASS_12_INDEX) || 
                 (i == RRC_INT_ACCESS_CLASS_13_INDEX) || 
                 (i == RRC_INT_ACCESS_CLASS_14_INDEX)) && 
                (RRC_CSP_SAME_MCC(plmn_id, rrc_csp_int_data.home_plmn_id)))
      {
        /* We are on Access Class 12-14 and are in Home Country. Full Access
         * is allowed 
         */
         MSG_LOW("In Home MCC UE Hi AC %d - Access Allowed", i, 0, 0);
         return(TRUE);
      }
    }
  }

  /* If we are here, it means the UE does not have even ONE hi access class 
   * that is:
   *       - NOT barred in System Info message AND 
   *       - UE satisfies Acceptability of Access Class in Serving network
   * Return a FALSE
   */
  return(FALSE);
}  /* rrc_csp_verify_hi_ac_access  */


/*===========================================================================

FUNCTION          RRC_CSP_FIND_CELL_BAR_STATUS

DESCRIPTION       This function is responsible for finding the status of
                  a cell in terms of Cell Barred status. The
                  Cell Access Restrictions due to Access Control are ignored
                  by this function.

DEPENDENCIES
                  - SIB procedure should have SIB 3 available for this cell.
                  - The UE Access Class should be known.

RETURN VALUE
                  FAILURE: If Cell Barred status couldn't be evaluated.
                  SUCCESS: Cell Barred status could be successfully evaluated.

SIDE EFFECTS
                  None

===========================================================================*/
uecomdef_status_e_type  
rrc_csp_find_cell_bar_status (rrc_plmn_identity_type  plmn_id, 
                              rrc_int_cell_id_type  cell_id,
                              rrc_csp_cell_bar_e_type  *cell_bar
                              ,rrc_plmn_identity_type common_plmn_id
                              )
{

  rrc_SysInfoType3 *sib3_ptr;
  rrc_SysInfoType4 *sib4_ptr;
  rrc_CellAccessRestriction *sib3_4_access_restriction_ptr;
 
  rrc_state_e_type rrc_state = rrc_get_state();
  rrc_csp_substate_e_type curr_csp_substate = rrc_get_csp_substate();  

  rrc_plmn_identity_type sibidx_plmn_id;

  rrc_csp_int_data.wcdma_cell_time_to_reselect = 0;
  *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;

  sibidx_plmn_id = common_plmn_id;

  WRRC_MSG2_HIGH("Sib_idx PLMN: MCC %d-MNC %d",RRC_CSP_GET_INT_MCC(sibidx_plmn_id),RRC_CSP_GET_INT_MNC(sibidx_plmn_id));
  /* Let us first read SIB3 - we should have that SIB for sure */
  /* TBD - Should we pass PLMN and Index to SIB ? */
  if((rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN))
  {
    sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_cell(
                              rrc_SIB3, 
                              sibidx_plmn_id, cell_id);
    if( sib3_ptr == NULL )
    {
      /* SIB3 for this PLMN, CEll ID combination not present */
      ERR("ERR: SIB3 for MCC %d, MNC %d, Cell ID %d not present",
            RRC_CSP_GET_INT_MCC(sibidx_plmn_id), RRC_CSP_GET_INT_MNC(sibidx_plmn_id), cell_id);
      return(FAILURE);
    } /* SIB3 not present */

    if((rrc_state != RRC_STATE_DISCONNECTED) && (rrc_state != RRC_STATE_CONNECTING))
    {
      if(sib3_ptr->sib4indicator == TRUE)
      {
        /* Read SIB4. If it's not there mark and error and continue with SIB3 */
        sib4_ptr = (rrc_SysInfoType4 *) rrc_sibdb_return_sib_for_cell(
                                  rrc_SIB4, 
                                  sibidx_plmn_id, cell_id);
        if( sib4_ptr == NULL )
        {
          /* SIB4 not present */
          WRRC_MSG0_HIGH("SIB4 indicated but not present, using SIB3");
          sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
        }
        else
        {
          sib3_4_access_restriction_ptr = &(sib4_ptr->cellAccessRestriction);
        }
      }
      else
      {
        sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
      }
    }
    else
    {
      sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
    }
  }
  /* WTOW BPLMN search is in progress */
  else
  {
    WRRC_MSG0_HIGH("WTOW: reading SIB3 for cell bar/access restrictions");
    sib3_ptr = (rrc_SysInfoType3 *) rrc_bplmn_return_sib_for_cell(rrc_SIB3);
                              
    if( sib3_ptr == NULL )
    {
      /* SIB3 for this PLMN, CEll ID combination not present */
      ERR_FATAL("SIB3 empty when SIB_CNF returned with SUCCESS", 0, 0, 0);
    } /* SIB3 not present */

    sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
  }

  /* The SIB3 is present - lets disect it */

  if((RRC_CHECK_COMMON_MSG_TYPE2(sib3_4_access_restriction_ptr->cellBarred,rrc_CellBarred,
                                                notBarred )) &&
     ( sib3_4_access_restriction_ptr->cellReservedForOperatorUse == 
                                                     rrc_ReservedIndicator_notReserved ) &&
     ( sib3_4_access_restriction_ptr->cellReservationExtension == 
                                                        rrc_ReservedIndicator_notReserved ))
  {
    /* Cell is NOT barred and NOT Reserved */
    WRRC_MSG0_HIGH("Cell Not Barred, Not reserved");
    *cell_bar = RRC_CSP_CELL_OK;
  } /* If Cell is NOT barred and NOT Reserved */

  else if((RRC_CHECK_COMMON_MSG_TYPE2(sib3_4_access_restriction_ptr->cellBarred,rrc_CellBarred,
                                                      notBarred )) &&
          ( sib3_4_access_restriction_ptr->cellReservedForOperatorUse == 
                                                     rrc_ReservedIndicator_notReserved ) &&
          ( sib3_4_access_restriction_ptr->cellReservationExtension == 
                                                      rrc_ReservedIndicator_reserved ))
  {
#ifdef FEATURE_FEMTO_CSG
    if(RES_FOR_CSG_IN_SIB3(sib3_ptr))
    {
      WRRC_MSG0_HIGH("CSG capable UE, cell reserved for CSG. Consider cell as not barred");
      *cell_bar = RRC_CSP_CELL_OK;
      return(SUCCESS);
    }
#endif
    /* Cell is NOT Barred but RESERVED for SoLSA - Treat it as Barred since the 
     * UE does not have SoLSA support (TBD: May change for UEs supporting 
     * Rel 2000 )
     */
    WRRC_MSG0_HIGH("Cell treated Barred future Reserved");
    rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;

    if((rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ) 
        &&  (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
    {
      /* Cell is Barred. Ignore the Intra-frequency cell re-selection indicator */
      WRRC_MSG0_HIGH("Ignore Intra-freq cell resel ind");
      *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
    }
    else
    {
      /* Cell is Not Barred, but RESERVED for SoLSA (is treated as BARRED) AND
       * "Intra frequency Cell re-selection Indicator" is NOT ALLOWED */ 
      WRRC_MSG0_HIGH("Intra-freq Not Allowed");
      *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
    }
  } /* If Cell is NOT Barred, not reserved for Operator but RESERVED for SoLSA */

  else if ((RRC_CHECK_COMMON_MSG_TYPE2(sib3_4_access_restriction_ptr->cellBarred,rrc_CellBarred,
                                                      notBarred) ) &&
     ( sib3_4_access_restriction_ptr->cellReservedForOperatorUse == 
                                                      rrc_ReservedIndicator_reserved ))
  {
    /* Cell is NOT Barred but RESERVED for Operator */
    WRRC_MSG0_HIGH("Cell reserved for OPerator");

    if(rrc_csp_ignore_barred_rsv_status_due_to_ueut(plmn_id))
    {
      WRRC_MSG0_HIGH("UEUT. Consider cell as NOT rsv for Op");
      *cell_bar = RRC_CSP_CELL_OK;
      return (SUCCESS);
    }

    /* Now check the UE Access Class to determine Cell Status */

   if(( rrc_hi_access_class != RRC_HI_ACCESS_CLASS_ABSENT ) &&
       (((rrc_hi_access_class & RRC_ACCESS_CLASS_11_BIT) ||
         (rrc_hi_access_class & RRC_ACCESS_CLASS_15_BIT)) &&
        (rrc_is_bcch_plmn_an_hplmn_or_ehplmn(plmn_id))))
    {
      /* UEs assigned to Access Class 11 or 15 may select/re-select this cell if in the home PLMN.
       * UEs assigned to an Access Class in the range 0 to 9 and 12 to 14 shall behave as if
       * cell status "barred" is indicated using the value "not allowed" in the
       * IE "Intra-frequency cell re-selection indicator" and the maximum value for Tbarred
       */

       WRRC_MSG0_HIGH("UE High AC present and in HPLMN");
       WRRC_MSG0_HIGH("Cell treated NOT Barred");     
       *cell_bar = RRC_CSP_CELL_OK;
    }
    else
    {
      if(rrc_hi_access_class == RRC_HI_ACCESS_CLASS_ABSENT)
      {
        WRRC_MSG0_HIGH("UE High AC not present");
      }

      WRRC_MSG0_HIGH("Cell treated Barred (Op Reserved)");
      rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;
      rrc_csp_int_data.cell_reserved_for_operator_use = TRUE;
      if((rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ) 
          &&  (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
      {
        /* Cell is Barred. Ignore the Intra-frequency cell re-selection indicator */
        WRRC_MSG0_HIGH("Ignore Intra-freq cell resel ind");
        *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
      }
      else if(((curr_csp_substate == RRCCSP_SUIT_CELL_RESELECT_WT_SIB) 
                 || (curr_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB))
                && (rrc_csp_int_data.new_cell_acq_entry.freq == rrc_csp_int_data.curr_acq_entry.freq))
      {
        /* Intra-frequency neighbor frequency is barred. Barring the frequency */
        WRRC_MSG0_HIGH("Intra-Freq Ngbr Cell Freq barred. Sndng Freq Barred to L1");
        *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
      }
      else
      {
        /* UE has either a High Access Class but is NOT in Home PLMN
         * OR UE does NOT have a High Access Class */
        /* Cell is TREATED AS BARRED for this UE */
        /* Cell is Not Barred, but RESERVED for Operator (is treated as BARRED) 
         * AND "Intra frequency Cell re-selection Indicator" is NOT ALLOWED */ 
        WRRC_MSG0_HIGH("No UE Hi AC or not in HPLMN,Intra-freq Not Allowed");
        *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;       
      }
    }
  } /* If Cell is NOT Barred but RESERVED for Operator */

  else if (RRC_CHECK_COMMON_MSG_TYPE2(sib3_4_access_restriction_ptr->cellBarred,rrc_CellBarred,
                                                      barred ))
  {
    if(rrc_csp_ignore_barred_rsv_status_due_to_ueut(plmn_id))
    {
      WRRC_MSG0_HIGH("UEUT. Consider cell as NOT barred");
      *cell_bar = RRC_CSP_CELL_OK;
      return (SUCCESS);
    }

    switch(sib3_4_access_restriction_ptr->cellBarred.u.barred->t_Barred)
    {
      case rrc_T_Barred_s10:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 10;
        break;
      case rrc_T_Barred_s20:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 20;
        break;
      case rrc_T_Barred_s40:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 40;
        break;
      case rrc_T_Barred_s80:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 80;
        break;
      case rrc_T_Barred_s160:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 160;
        break;
      case rrc_T_Barred_s320:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 320;
        break;
      case rrc_T_Barred_s640:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 640;
        break;
      case rrc_T_Barred_s1280:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;
        break;
      default:
        WRRC_MSG0_HIGH("Unexpected. Setting the time to 1280 secs");
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;
        break;
    }
    
    WRRC_MSG0_HIGH("Cell is indicated as Barred");
    /* Cell is indicated as Barred */
    if( sib3_4_access_restriction_ptr->cellBarred.u.barred->
                      intraFreqCellReselectionInd == rrc_AllowedIndicator_allowed )
    {
      /* Cell is Barred
       * AND "Intra frequency Cell re-selection Indicator" is ALLOWED */ 
      MSG_LOW("Intra-freq Allowed", 0, 0, 0);
      *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
    }
    else
    {
      if((rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ)
         && (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
      {
        /* Cell is Barred. Ignore the Intra-frequency cell re-selection indicator */
        WRRC_MSG0_HIGH("Ignore Intra-freq cell resel ind");
        *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
      }
      else if(CSG_ID_IN_SIB3(sib3_ptr))
      {
          WRRC_MSG0_HIGH("CSG cell ignore intra freq resel not allowed indicator");
          *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
      }
      else
      {
        /* Cell is Barred
         * AND "Intra frequency Cell re-selection Indicator" is NOT ALLOWED */ 
        WRRC_MSG0_HIGH("Intra-freq Not Allowed");
        *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
      }
    }
  } /* Cell is indicated as Barred */
  else
  {
    /* We should not come here */
    WRRC_MSG0_ERROR("ERR: Error in cell bar evaluation logic");
    rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;

    if((rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ) 
        &&  (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
    {
      /* Cell is Barred. Ignore the Intra-frequency cell re-selection indicator */
      WRRC_MSG0_HIGH("Ignore Intra-freq cell resel ind");
      *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
    }
    else if(((curr_csp_substate == RRCCSP_SUIT_CELL_RESELECT_WT_SIB) 
               || (curr_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB))
              && (rrc_csp_int_data.new_cell_acq_entry.freq == rrc_csp_int_data.curr_acq_entry.freq))
    {
      /* Intra-frequency neighbor frequency is barred. Barring the frequency */
      WRRC_MSG0_HIGH("Intra-Freq Ngbr Cell Freq barred. Sndng Freq Barred to L1");
      *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
    }
    else
    {
      *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
    }
  }

  return(SUCCESS);
} /* rrc_csp_find_cell_bar_status() */

/*====================================================================================

FUNCTION          RRC_CHECK_IF_DSAC_IS_PRESENT

DESCRIPTION     This function is to check if DSAC info is present in SIB3 (or) not

DEPENDENCIES

RETURN VALUE
                Returns value TRUE   if DSAC information is present in SIB3
                Returns value FALSE value if DSAC information is not present in SIB3
SIDE EFFECTS
                  None
======================================================================================*/
boolean  rrc_check_if_dsac_is_present(  rrc_SysInfoType3   *sib3_ptr)
{
  if(sib3_ptr->m.v4b0NonCriticalExtensionsPresent)
  {
    if(sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent )
    {
      if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)
      {
        if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
            v5c0NoncriticalExtension.m.v670NonCriticalExtensionPresent )
        {
          if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
              v5c0NoncriticalExtension.v670NonCriticalExtension.
              sysInfoType3_v670ext.m.domainSpecificAccessRestrictionParametersForPLMNOfMIBPresent )
          {
            return TRUE;
          }
        }
      }
    }
  }
return FALSE;
}

/*====================================================================================

FUNCTION          RRC_CHECK_IF_PPAC_IS_PRESENT

DESCRIPTION     This function is to check if PPAC info is present in SIB3 (or) not

DEPENDENCIES

RETURN VALUE
                Returns value TRUE if PPAC information is present in SIB3
                Returns value FALSE value if PPAC information is not present in SIB3
SIDE EFFECTS
                  None
======================================================================================*/
boolean  rrc_check_if_ppac_is_present(  rrc_SysInfoType3   *sib3_ptr)
{
  if(sib3_ptr->m.v4b0NonCriticalExtensionsPresent)
  {
    if(sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent)
    {
      if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)
      {
        if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.
             m.v670NonCriticalExtensionPresent)
        {
          if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.
               v670NonCriticalExtension.m.v770NonCriticalExtensionPresent)
          {
            if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.
                 v670NonCriticalExtension.v770NonCriticalExtension.m.v830NonCriticalExtensionPresent)
            {
              if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.
                   v670NonCriticalExtension.v770NonCriticalExtension.v830NonCriticalExtension.
                   sysInfoType3_v830ext.m.pagingPermissionWithAccessControlParametersForPLMNOfMIBPresent)
              {
                return TRUE;
              }
            }
          }
        }
      }
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION          RRC_CSP_FIND_DSAC_RESTRICTIONS

DESCRIPTION       This function is responsible for finding the status of
                  Access Control due to Domain Specific Access Class Restrictions.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
===========================================================================*/
void rrc_csp_find_dsac_restrictions
(
  rrc_DomainSpecificAccessRestrictionParam_v670ext dsac_local,
  rrc_dsac_info_type* rrc_dsac_info,
  rrc_int_access_class_barred_list_type* rrc_int_access_class_barred_list,
  rrc_plmn_identity_type  plmn_id,
  rrc_int_cell_id_type    cell_id
)
{

  rrc_int_access_class_barred_list_type rrc_dsac_access_class_barred_list;

  /* Set DSAC Info present variable TRUE */
  rrc_dsac_info->valid = TRUE;

  /* If CS/PS Domain access class restriction is present */ 
  if((dsac_local.cSDomainSpecificAccessRestriction.t == 
    T_rrc_DomainSpecificAccessRestriction_v670ext_restriction )
    && (dsac_local.pSDomainSpecificAccessRestriction.t == 
    T_rrc_DomainSpecificAccessRestriction_v670ext_restriction))
  {
    WRRC_MSG0_HIGH("DSAC: Both CS and PS domain restrictions are present ");

    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(dsac_local.cSDomainSpecificAccessRestriction.u.restriction,
        domainSpecficAccessClassBarredList)) &&
       (dsac_local.cSDomainSpecificAccessRestriction.u.restriction->domainSpecficAccessClassBarredList.n != 0))
    {
      WRRC_MSG0_HIGH("NWS: CS DSAC List is present");
      /* Get the CS Domain Specific Access Control Information */
      rrc_translate_access_class_barred_list(&dsac_local.cSDomainSpecificAccessRestriction.u.restriction->domainSpecficAccessClassBarredList, 
                                             &rrc_dsac_access_class_barred_list);
    }
    else
    {
      rrc_dsac_access_class_barred_list = *rrc_int_access_class_barred_list;
    }

    /* Fill the CS domain specific access class information from access class barred list */ 
    rrc_fill_access_class_info(rrc_dsac_access_class_barred_list,
                               &(rrc_dsac_info->cs_access_control),
                               plmn_id,
                               cell_id); 

    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(dsac_local.pSDomainSpecificAccessRestriction.u.restriction,
        domainSpecficAccessClassBarredList)) &&
       (dsac_local.pSDomainSpecificAccessRestriction.u.restriction->domainSpecficAccessClassBarredList.n != 0))
    {
      WRRC_MSG0_HIGH("NWS: PS DSAC List is present");
      /* Get the PS Domain Specific Access Control Information */      
      rrc_translate_access_class_barred_list(&dsac_local.pSDomainSpecificAccessRestriction.u.restriction->domainSpecficAccessClassBarredList,
                                             &rrc_dsac_access_class_barred_list);
    }
    else
    {
      rrc_dsac_access_class_barred_list = *rrc_int_access_class_barred_list;
    }


    /* Fill the PS domain specific access class informattion from access class barred list */ 
    rrc_fill_access_class_info(rrc_dsac_access_class_barred_list,
                               &(rrc_dsac_info->ps_access_control),
                               plmn_id,
                               cell_id); 
  }
  else if(dsac_local.cSDomainSpecificAccessRestriction.t == T_rrc_DomainSpecificAccessRestriction_v670ext_restriction && 
    dsac_local.pSDomainSpecificAccessRestriction.t!= T_rrc_DomainSpecificAccessRestriction_v670ext_restriction)
  {
    WRRC_MSG0_HIGH("DSAC: CS Domain restriction is present but not PS");

    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(dsac_local.cSDomainSpecificAccessRestriction.u.restriction,
        domainSpecficAccessClassBarredList)) &&
       (dsac_local.cSDomainSpecificAccessRestriction.u.restriction->domainSpecficAccessClassBarredList.n != 0))
    {
      WRRC_MSG0_HIGH("NWS: CS DSAC List is present");
      /* Get the CS Domain Specific Access Control Information */
      rrc_translate_access_class_barred_list(&dsac_local.cSDomainSpecificAccessRestriction.u.restriction->domainSpecficAccessClassBarredList, 
                                             &rrc_dsac_access_class_barred_list);
    }
    else
    {
      rrc_dsac_access_class_barred_list = *rrc_int_access_class_barred_list;
    }

    /* Fill the CS domain specific access class information from access class barred list */ 
    rrc_fill_access_class_info(rrc_dsac_access_class_barred_list,
                               &(rrc_dsac_info->cs_access_control),
                               plmn_id,
                               cell_id);
    rrc_dsac_info->ps_access_control = RRC_CSP_CELL_ACCESS_ALLOWED;
  }
  else if(dsac_local.cSDomainSpecificAccessRestriction.t !=
    T_rrc_DomainSpecificAccessRestriction_v670ext_restriction 
    && dsac_local.pSDomainSpecificAccessRestriction.t == 
    T_rrc_DomainSpecificAccessRestriction_v670ext_restriction)
  {
    WRRC_MSG0_HIGH("DSAC: PS Domain restriction is present but not CS");

    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(dsac_local.pSDomainSpecificAccessRestriction.u.restriction,
        domainSpecficAccessClassBarredList)) &&
       (dsac_local.pSDomainSpecificAccessRestriction.u.restriction->domainSpecficAccessClassBarredList.n != 0))
    {
      WRRC_MSG0_HIGH("NWS: PS DSAC List is present");
      /* Get the PS Domain Specific Access Control Information */
      rrc_translate_access_class_barred_list(&dsac_local.pSDomainSpecificAccessRestriction.u.restriction->domainSpecficAccessClassBarredList, 
                                             &rrc_dsac_access_class_barred_list);
    }
    else
    {
      rrc_dsac_access_class_barred_list = *rrc_int_access_class_barred_list;
    }

    /* Fill the PS domain specific access class information from access class barred list */ 
    rrc_fill_access_class_info(rrc_dsac_access_class_barred_list,
                               &(rrc_dsac_info->ps_access_control),
                               plmn_id,
                               cell_id);
    rrc_dsac_info->cs_access_control = RRC_CSP_CELL_ACCESS_ALLOWED;	
  }
  else  /*Both CS/PS Domain DSAC are not chosen hence no CS/PS restrictions apply */
  {
    WRRC_MSG0_HIGH("DSAC: No Domain restrictions are present for CS/PS");
    rrc_dsac_info->cs_access_control = RRC_CSP_CELL_ACCESS_ALLOWED;
    rrc_dsac_info->ps_access_control = RRC_CSP_CELL_ACCESS_ALLOWED;
  }
  if(rrc_lo_access_class == RRC_LO_ACCESS_CLASS_ABSENT)
  {
    WRRC_MSG2_HIGH("Low Access class Absent. Previous DSAC info CS: %d PS: %d",rrc_dsac_info->cs_access_control,rrc_dsac_info->ps_access_control);
    if(rrc_dsac_info->cs_access_control == RRC_CSP_CELL_ACCESS_ALLOWED)
    {
      rrc_dsac_info->cs_access_control = RRC_CSP_NO_REGULAR_CELL_ACCESS;
    }
    else if(rrc_dsac_info->cs_access_control == RRC_CSP_REGULAR_CELL_ACCESS_ONLY)
    {
      rrc_dsac_info->cs_access_control = RRC_CSP_NO_CELL_ACCESS;
    }
    if(rrc_dsac_info->ps_access_control == RRC_CSP_CELL_ACCESS_ALLOWED)
    {
      rrc_dsac_info->ps_access_control = RRC_CSP_NO_REGULAR_CELL_ACCESS;
    }
    else if(rrc_dsac_info->ps_access_control == RRC_CSP_REGULAR_CELL_ACCESS_ONLY)
    {
      rrc_dsac_info->ps_access_control = RRC_CSP_NO_CELL_ACCESS;
    }
    WRRC_MSG2_HIGH("After Updation DSAC info CS: %d PS: %d",rrc_dsac_info->cs_access_control,rrc_dsac_info->ps_access_control);
  }
}

/*===========================================================================

FUNCTION          RRC_CSP_FIND_PPAC_RESTRICTIONS

DESCRIPTION       This function is responsible for finding the status of
                  Access Control due to Paging Permission Access Control Restrictions.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
===========================================================================*/
void rrc_csp_find_ppac_restrictions
(
  rrc_PagingPermissionWithAccessControlParameters ppac_local,
  rrc_ppac_info_type* rrc_ppac_info,
  rrc_int_access_class_barred_list_type* rrc_int_access_class_barred_list,
  rrc_plmn_identity_type  plmn_id,
  rrc_int_cell_id_type    cell_id
)
{
  rrc_int_access_class_barred_list_type rrc_ppac_access_class_barred_list;
  rrc_csp_access_control_e_type access_control;

  /* Set PPAC Info present variable TRUE */
  rrc_ppac_info->valid = TRUE;

  /* Set CS Reg and PS Reg allowed status to default values*/
  rrc_ppac_info->cs_reg_allowed = FALSE;
  rrc_ppac_info->ps_reg_allowed = FALSE;

  /* PPAC parameters for Paging Response */
  if(ppac_local.pagingResponseRestrictionIndicator == rrc_PagingPermissionWithAccessControlParameters_pagingResponseRestrictionIndicator_all)
  {
    rrc_ppac_info->page_rsp_allowed = MM_AS_NONE;
  }
  else if(ppac_local.pagingResponseRestrictionIndicator == rrc_PagingPermissionWithAccessControlParameters_pagingResponseRestrictionIndicator_cS)
  {
    rrc_ppac_info->page_rsp_allowed = MM_AS_PS_ALLOWED;
  }
  else if(ppac_local.pagingResponseRestrictionIndicator == rrc_PagingPermissionWithAccessControlParameters_pagingResponseRestrictionIndicator_pS)
  {
    rrc_ppac_info->page_rsp_allowed = MM_AS_CS_ALLOWED;
  }
  else if(ppac_local.pagingResponseRestrictionIndicator == rrc_PagingPermissionWithAccessControlParameters_pagingResponseRestrictionIndicator_none)
  {
    rrc_ppac_info->page_rsp_allowed = MM_AS_CS_PS_ALLOWED;
  }
  else
  {
    WRRC_MSG0_ERROR("PPAC: Paging Parameters are invalid");
  }

  /* PPAC parameters for Location/Registration */
  if(ppac_local.locationRegistrationRestrictionIndicator == rrc_PagingPermissionWithAccessControlParameters_locationRegistrationRestrictionIndicator_all)
  {
    if(ppac_local.locationRegistration.t == T_rrc_LocationRegistrationParameters_noRestriction)
    {
      rrc_ppac_info->cs_reg_allowed = TRUE;
      rrc_ppac_info->ps_reg_allowed = TRUE;
    }
    else /*Location/Registration Parameters are restricted for CS and PS domain. Look for AC barred list */
    {
      WRRC_MSG0_HIGH("PPAC: Both CS and PS domain restrictions are present ");
      rrc_translate_ppac_access_class_barred_list(ppac_local.locationRegistration.u.restriction, 
                                                  &rrc_ppac_access_class_barred_list);
      rrc_fill_access_class_info(rrc_ppac_access_class_barred_list,
                                 &access_control,
                                 plmn_id,
                                 cell_id);
      if(access_control == RRC_CSP_CELL_ACCESS_ALLOWED)
      {
        rrc_ppac_info->cs_reg_allowed = TRUE;
        rrc_ppac_info->ps_reg_allowed = TRUE;
      }
    }
  }
  else if(ppac_local.locationRegistrationRestrictionIndicator == rrc_PagingPermissionWithAccessControlParameters_locationRegistrationRestrictionIndicator_cS)
  {
    rrc_ppac_info->ps_reg_allowed = TRUE;

    if(ppac_local.locationRegistration.t == T_rrc_LocationRegistrationParameters_noRestriction)
    {
      rrc_ppac_info->cs_reg_allowed = TRUE;
    }
    else /*Location/Registration Parameters are restricted for CS domain. Look for AC barred list */
    {
      WRRC_MSG0_HIGH("PPAC: CS domain restrictions are present ");
      rrc_translate_ppac_access_class_barred_list(ppac_local.locationRegistration.u.restriction, 
                                             &rrc_ppac_access_class_barred_list);
      rrc_fill_access_class_info(rrc_ppac_access_class_barred_list,
                                 &access_control,
                                 plmn_id,
                                 cell_id);
      if(access_control == RRC_CSP_CELL_ACCESS_ALLOWED)
      {
        rrc_ppac_info->cs_reg_allowed = TRUE;
      }
    }
  }
  else if(ppac_local.locationRegistrationRestrictionIndicator == rrc_PagingPermissionWithAccessControlParameters_locationRegistrationRestrictionIndicator_pS)
  {
    rrc_ppac_info->cs_reg_allowed = TRUE;

    if(ppac_local.locationRegistration.t == T_rrc_LocationRegistrationParameters_noRestriction)
    {
      rrc_ppac_info->ps_reg_allowed = TRUE;
    }
    else /*Location/Registration Parameters are restricted for PS domain. Look for AC barred list */
    {
      WRRC_MSG0_HIGH("PPAC: PS domain restrictions are present ");
      rrc_translate_ppac_access_class_barred_list(ppac_local.locationRegistration.u.restriction, 
                                             &rrc_ppac_access_class_barred_list);
      rrc_fill_access_class_info(rrc_ppac_access_class_barred_list,
                                 &access_control,
                                 plmn_id,
                                 cell_id);
      if(access_control == RRC_CSP_CELL_ACCESS_ALLOWED)
      {
        rrc_ppac_info->ps_reg_allowed = TRUE;
      }
    }
  }
  else
  {
    WRRC_MSG0_ERROR("PPAC Location/Registration Parameters are invalid");
  }
}

/*====================================================================================

FUNCTION          RRC_CSP_FILL_DSAC_INFO_FOR_SHARED_PLMNS

DESCRIPTION     This function fills the DSAC info for shared PLMNs from SIB3

DEPENDENCIES

RETURN VALUE
                  None

SIDE EFFECTS
                  None
======================================================================================*/
void rrc_csp_fill_dsac_info_for_shared_plmns
(
  uint8 number_of_plmns,
  rrc_SysInfoType3   *sib3_ptr,
  rrc_dsac_info_for_shared_plmns_type* dsac_info_for_shared_plmns
)
{
  uint16 i = 0;

  for(i = 0; i< (MAX_SHARED_PLMNS-1);i++)
  {
    dsac_info_for_shared_plmns[i].valid = FALSE;
  }

  if(sib3_ptr->m.v4b0NonCriticalExtensionsPresent)
  {
    if(sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent )
    {
      if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)
      {
        if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
            v5c0NoncriticalExtension.m.v670NonCriticalExtensionPresent )
        {
          if((RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
             v5c0NoncriticalExtension.v670NonCriticalExtension.
             sysInfoType3_v670ext,domainSpecificAccessRestictionForSharedNetwork)) &&
             (RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
             v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext,
             domainSpecificAccessRestictionForSharedNetwork)))
          {
            if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
               v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
               domainSpecificAccessRestictionForSharedNetwork.t ==
               T_rrc_DomainSpecificAccessRestrictionForSharedNetwork_v670ext_domainSpecificAccessRestictionList )
            {
              if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                 v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                 domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList,
                 rrc,
                 domainSpecificAccessRestrictionParametersForOperator1)) )
              {
                WRRC_MSG0_HIGH("NWS: DSAC info for operator 1 present in SIB3");
                dsac_info_for_shared_plmns[0].valid = TRUE;
                dsac_info_for_shared_plmns[0].dsac_info = sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                             v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                             domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList->
                             domainSpecificAccessRestrictionParametersForOperator1;
               }
              if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                 v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                 domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList,
                 rrc,
                 domainSpecificAccessRestrictionParametersForOperator2)) )
               {
                 WRRC_MSG0_HIGH("NWS: DSAC info for operator 2 present in SIB3");
                 dsac_info_for_shared_plmns[1].valid = TRUE;
                 dsac_info_for_shared_plmns[1].dsac_info = sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                             v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                             domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList->
                             domainSpecificAccessRestrictionParametersForOperator2;
               }
              if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                 v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                 domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList,
                 rrc,
                 domainSpecificAccessRestrictionParametersForOperator3)) )
               {
                 WRRC_MSG0_HIGH("NWS: DSAC info for operator 3 present in SIB3");
                 dsac_info_for_shared_plmns[2].valid = TRUE;
                 dsac_info_for_shared_plmns[2].dsac_info= sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                             v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                             domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList->
                             domainSpecificAccessRestrictionParametersForOperator3;
               }
              if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                 v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                 domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList,
                 rrc,
                 domainSpecificAccessRestrictionParametersForOperator4)) )
              {
                 WRRC_MSG0_HIGH("NWS: DSAC info for operator 4 present in SIB3");
                 dsac_info_for_shared_plmns[3].valid = TRUE;
                 dsac_info_for_shared_plmns[3].dsac_info= sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                             v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                             domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList->
                             domainSpecificAccessRestrictionParametersForOperator4;
               }
              if((RRC_MSG_COMMON_BITMASK_IE_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                 v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                 domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList,
                 rrc,
                 domainSpecificAccessRestrictionParametersForOperator5)) )
               {
                 WRRC_MSG0_HIGH("NWS: DSAC info for operator 5 present in SIB3");
                 dsac_info_for_shared_plmns[4].valid = TRUE;
                 dsac_info_for_shared_plmns[4].dsac_info= sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                             v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                             domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionList->
                             domainSpecificAccessRestrictionParametersForOperator5;
               }
            }
            else if( sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                    v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.
                    domainSpecificAccessRestictionForSharedNetwork.t ==
                    T_rrc_DomainSpecificAccessRestrictionForSharedNetwork_v670ext_domainSpecificAccessRestictionParametersForAll )
            {
              WRRC_MSG0_HIGH("NWS: DSAC info for all operators present in SIB3");
              for(i = 0; i < (number_of_plmns-1); i++)
              {
                dsac_info_for_shared_plmns[i].valid = TRUE;
                WCDMA_MEMCPY(&(dsac_info_for_shared_plmns[i].dsac_info),
                             sizeof(rrc_DomainSpecificAccessRestrictionParam_v670ext),
                             sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.domainSpecificAccessRestictionForSharedNetwork.u.domainSpecificAccessRestictionParametersForAll,
                             sizeof(rrc_DomainSpecificAccessRestrictionParam_v670ext));
              }
            }
          }
        }
      }
    }
  }
}

/*====================================================================================

FUNCTION          RRC_CSP_FILL_PPAC_INFO_FOR_SHARED_PLMNS

DESCRIPTION     This function fills the PPAC info for shared PLMNs from SIB3

DEPENDENCIES

RETURN VALUE
                  None

SIDE EFFECTS
                  None
======================================================================================*/
void rrc_csp_fill_ppac_info_for_shared_plmns
(
  uint8 number_of_plmns,
  rrc_SysInfoType3   *sib3_ptr,
  rrc_ppac_info_for_shared_plmns_type* ppac_info_for_shared_plmns
)
{

  uint16 i = 0;

  for(i = 0; i< (MAX_SHARED_PLMNS-1);i++)
  {
    ppac_info_for_shared_plmns[i].valid = FALSE;
  }

  if(sib3_ptr->m.v4b0NonCriticalExtensionsPresent)
  {
    if(sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent )
    {
      if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)
      {
        if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.m.v670NonCriticalExtensionPresent)
        {
          if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.
             v830NonCriticalExtension.sysInfoType3_v830ext.m.pagingPermissionWithAccessControlParametersForSharedNetworkPresent)
          {
            if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.
               v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.t 
               == T_rrc_PagingPermissionWithAccessControlForSharedNetwork_pagingPermissionWithAccessControlList)
            {
              if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                     v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                     u.pagingPermissionWithAccessControlList,pagingPermissionWithAccessControlParametersForOperator1))
              {
                WRRC_MSG0_HIGH("NWS: PPAC info for operator 1 present in SIB3");
                ppac_info_for_shared_plmns[0].valid = TRUE;
                ppac_info_for_shared_plmns[0].ppac_info = sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                          v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                         u.pagingPermissionWithAccessControlList->pagingPermissionWithAccessControlParametersForOperator1;
              }
              if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                     v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                     u.pagingPermissionWithAccessControlList,pagingPermissionWithAccessControlParametersForOperator2))
              {
                WRRC_MSG0_HIGH("NWS: PPAC info for operator 2 present in SIB3");
                ppac_info_for_shared_plmns[1].valid = TRUE;
                ppac_info_for_shared_plmns[1].ppac_info = sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                          v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                          u.pagingPermissionWithAccessControlList->pagingPermissionWithAccessControlParametersForOperator2;
              }
              if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                     v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                     u.pagingPermissionWithAccessControlList,pagingPermissionWithAccessControlParametersForOperator3))
              {
                WRRC_MSG0_HIGH("NWS: PPAC info for operator 3 present in SIB3");
                ppac_info_for_shared_plmns[2].valid = TRUE;
                ppac_info_for_shared_plmns[2].ppac_info = sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                          v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                          u.pagingPermissionWithAccessControlList->pagingPermissionWithAccessControlParametersForOperator3;
              }
              if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                     v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                     u.pagingPermissionWithAccessControlList,pagingPermissionWithAccessControlParametersForOperator4))
              {
                WRRC_MSG0_HIGH("NWS: PPAC info for operator 4 present in SIB3");
                ppac_info_for_shared_plmns[3].valid = TRUE;
                ppac_info_for_shared_plmns[3].ppac_info = sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                          v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                          u.pagingPermissionWithAccessControlList->pagingPermissionWithAccessControlParametersForOperator4;
              }
              if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                     v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                     u.pagingPermissionWithAccessControlList,pagingPermissionWithAccessControlParametersForOperator5))
              {
                WRRC_MSG0_HIGH("NWS: PPAC info for operator 5 present in SIB3");
                ppac_info_for_shared_plmns[4].valid = TRUE;
                ppac_info_for_shared_plmns[4].ppac_info = sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.
                                                          v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.
                                                          u.pagingPermissionWithAccessControlList->pagingPermissionWithAccessControlParametersForOperator5;
              }
            }
            else if(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.
                    v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.t 
                    == T_rrc_PagingPermissionWithAccessControlForSharedNetwork_pagingPermissionWithAccessControlForAll)
            {
              WRRC_MSG0_HIGH("NWS: PPAC info for all operators present in SIB3");
              for(i = 0; i < (number_of_plmns-1); i++)
              {
                ppac_info_for_shared_plmns[i].valid = TRUE;
                WCDMA_MEMCPY(&(ppac_info_for_shared_plmns[i].ppac_info),
                             sizeof(rrc_PagingPermissionWithAccessControlParameters),
                             sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForSharedNetwork.u.pagingPermissionWithAccessControlForAll,
                             sizeof(rrc_PagingPermissionWithAccessControlParameters));
              }
            }
          }
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION          RRC_TRANSLATE_ACCESS_CONTROL_TO_ACCESS_CLASS

DESCRIPTION     This function is responsible for conversion of internal cell access control 
                          from CSP to access class in conjunction with RRC<->MM interface

DEPENDENCIES

RETURN VALUE
                  Noe
SIDE EFFECTS
                  None
===========================================================================*/

static void rrc_translate_access_control_to_access_class
(
  rrc_csp_access_control_e_type access_control, 
  mm_as_cell_access_status_T*access_class
)
{
  switch(access_control)
  {
    case RRC_CSP_CELL_ACCESS_ALLOWED:
      *access_class = MM_AS_ACCESS_ALL_CALLS;
      break;

    case RRC_CSP_NO_REGULAR_CELL_ACCESS:
      *access_class = MM_AS_ACCESS_EMERGENCY_ONLY;
      break;

    case RRC_CSP_NO_CELL_ACCESS:
      *access_class = MM_AS_ACCESS_NO_CALLS;
      break;

    case RRC_CSP_REGULAR_CELL_ACCESS_ONLY:
      *access_class = MM_AS_ACCESS_NORMAL_ONLY;
      break;

    default:
      WRRC_MSG1_HIGH("Invalid access control received %d",access_control);
      *access_class = MM_AS_ACCESS_ALL_CALLS;
      break;
  }
}

/*===========================================================================

FUNCTION          RRC_CSP_FIND_ACCESS_CONTROL

DESCRIPTION       This function is responsible for finding the status of
                  Access Control due to Access Class Restrictions.

                  Note that this function doesn't care about Cell Barred status
                  OR Cell Reservations.

DEPENDENCIES
                  - SIB procedure should have SIB 3 available for this cell.
                  - The UE Access Class should be known.

RETURN VALUE
                  SUCCESS: Cell Access restrictions for this cell could be 
                           calculated.
                  FAILURE: Failure to find out Cell Access restrictions due 
                           to multiple reasons.
SIDE EFFECTS
                  None.
===========================================================================*/
uecomdef_status_e_type  
rrc_csp_find_access_control(rrc_csp_access_control_e_type  *access_control, rrc_dsac_info_type *rrc_dsac_info,
                            rrc_plmn_identity_type  plmn_id,
                            rrc_int_cell_id_type    cell_id
                           ,rrc_plmn_identity_type  common_plmn_id
                           ,rrc_ppac_info_type *rrc_ppac_info
                            )
{
  uint16  i;
  boolean look_at_shared_list = FALSE;
  rrc_MasterInformationBlock *mib_ptr;
  rrc_plmn_identity_type mib_PLMN_ID;
  uint8 num_of_shared_plmns;
  rrc_plmn_identity_type shared_plmn_list[MAX_SHARED_PLMNS];
  rrc_SysInfoType3   *sib3_ptr;
  rrc_int_access_class_barred_list_type   rrc_int_access_class_barred_list;
  rrc_SysInfoType4 *sib4_ptr;
  rrc_CellAccessRestriction *sib3_4_access_restriction_ptr;
  rrc_state_e_type rrc_state = rrc_get_state();
  
  rrc_plmn_identity_type sibidx_plmn_id;
  
  *access_control = RRC_CSP_NO_CELL_ACCESS;
  rrc_dsac_info->valid = FALSE;
  rrc_ppac_info->valid = FALSE;

  sibidx_plmn_id = common_plmn_id;


  WRRC_MSG2_HIGH("SIB Index PLMN is MCC%d - MNC%d",RRC_CSP_GET_INT_MCC(sibidx_plmn_id), RRC_CSP_GET_INT_MNC(sibidx_plmn_id));
  rrc_csp_int_data.is_curr_dsac_info_valid = FALSE;
  rrc_csp_int_data.is_curr_ppac_info_valid = FALSE;

  if((rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN))
  {
    /* PLMN and Cell ID of the current camped on cell are known. Get SIB3 */
    sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_cell(
                              rrc_SIB3, 
                              sibidx_plmn_id, cell_id);
    if( sib3_ptr == NULL )
    {
      /* SIB3 for this PLMN, Cell ID combination not present 
       * This should NOT happen 
       */
      ERR("ERR: SIB3 for MCC %d, MNC %d, Cell ID %d not present", 
            RRC_CSP_GET_INT_MCC(sibidx_plmn_id), RRC_CSP_GET_INT_MNC(sibidx_plmn_id), cell_id);
      return(FAILURE);
    }

    if((rrc_state != RRC_STATE_DISCONNECTED) && (rrc_state != RRC_STATE_CONNECTING))
    {
      if(sib3_ptr->sib4indicator == TRUE)
      {
        /* Read SIB4. If it's not there mark and error and continue with SIB3 */
        sib4_ptr = (rrc_SysInfoType4 *) rrc_sibdb_return_sib_for_cell(
                                  rrc_SIB4, 
                                  sibidx_plmn_id, cell_id);
        if( sib4_ptr == NULL )
        {
          /* SIB4 not present */
          WRRC_MSG0_HIGH("SIB4 indicated but not present, using SIB3");
          sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
        }
        else
        {
          sib3_4_access_restriction_ptr = &(sib4_ptr->cellAccessRestriction);
        }
      }
      else
      {
        sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
      }
    }
    else
    {
      sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
    }
  }
  /* WTOW BPLMN search is in progress */
  else
  {
    /* Get SIB3 info */
    sib3_ptr = (rrc_SysInfoType3 *) rrc_bplmn_return_sib_for_cell(rrc_SIB3);

    if( sib3_ptr == NULL )
    {
      /* SIB3 is NULL This should NOT happen */
      ERR_FATAL("SIB3 not read but sib_cnf returned w SUCCESS", 0, 0, 0);
    }
    sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
  }

  /* We ARE camped on a cell and VALID SIBS for that cell exist - proceed further 
   * Check if UE has all access, only emergency access or No access 
   */
  for (i=0; i<(uint16)RRC_INT_ACCESS_CLASS_MAX_INDEX; i++)
  {
    rrc_int_access_class_barred_list.barred[i] = FALSE;
  }

  /* First check if the Access Class Barred List is present or not */
  if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_access_restriction_ptr,
      accessClassBarredList))) 
  {
    /* Access Class Barred List is NOT present */
    /* If UE has a Valid Low Access Class, all calls are allowed.
     */
    MSG_MED("DSAC: Access Class Barred List Not present ",0,0,0);
    if(rrc_lo_access_class != RRC_LO_ACCESS_CLASS_ABSENT)
    {
      MSG_MED("Cell %d Access: All calls", cell_id, 0, 0);
      MSG_LOW("AC bar list absent, valid UE Low AC %d", 
                          rrc_lo_access_class, 0, 0); 
      *access_control = RRC_CSP_CELL_ACCESS_ALLOWED;
    }
    else /* UE does not have a valid Low Access Class Only Emergency calls are allowed */
    {
     /* NOTE: There is no need to check if UE has a Hi Access Class if UE's Low
      * Access Class is absent. All UEs are supposed to have a Low Access Class 
      * An absence of UE Low Access Class may actually be a "No SIM card" case.
      */
      MSG_MED("Cell %d Access: Emergency calls", cell_id, 0, 0);
      MSG_LOW("AC bar list absent, No UE Low AC", 0, 0, 0);
                                                        
      *access_control = RRC_CSP_NO_REGULAR_CELL_ACCESS;
    }
  } /* If Access Class barred List is NOT present */ 
  else  /* Access Class barred list is present */
  {
    /* First store the access class into a local data structure */
    rrc_translate_access_class_barred_list(&sib3_4_access_restriction_ptr->accessClassBarredList,
                                           &rrc_int_access_class_barred_list);
    rrc_fill_access_class_info(rrc_int_access_class_barred_list,
                               access_control,
                               plmn_id,
                               cell_id);
  }

  if((rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN))
  {
    mib_ptr = (rrc_MasterInformationBlock *) rrc_sibdb_return_sib_for_cell(
                                                                          rrc_MIB,
                                                                          sibidx_plmn_id,
                                                                          cell_id
                                                                         );
  }
  else
  {
    mib_ptr = (rrc_MasterInformationBlock *) rrc_bplmn_return_sib_for_cell(
                                                                           rrc_MIB);
  }

  if( mib_ptr == NULL )
  {
    /* SIB3 is NULL This should NOT happen */
    ERR_FATAL("MIB not read but sib_cnf returned w SUCCESS", 0, 0, 0);
  }

  if(rrc_is_nw_shared(mib_ptr))
  {
    rrc_translate_gsm_map_plmn_id(&(mib_ptr->plmn_Type.u.gsm_MAP->plmn_Identity),&mib_PLMN_ID);
    if(!rrccsp_is_mib_plmn_valid(mib_ptr) ||
       !rrc_bcch_plmns_match(mib_PLMN_ID,plmn_id))
    {
      look_at_shared_list = TRUE;
    }
  }

  if(!look_at_shared_list && rrc_check_if_dsac_is_present(sib3_ptr) == TRUE)
  {
    /* First store the access class pointer into a local data structure */
    rrc_DomainSpecificAccessRestrictionParam_v670ext dsac_local = sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.sysInfoType3_v670ext.domainSpecificAccessRestrictionParametersForPLMNOfMIB;
    rrc_csp_int_data.is_curr_dsac_info_valid = TRUE;
    WRRC_MSG0_HIGH("DSAC:Present in SIB3 and is_curr_dsac_info_valid is set to TRUE");

    rrc_csp_find_dsac_restrictions(dsac_local,
                                   rrc_dsac_info,
                                   &rrc_int_access_class_barred_list,
                                   plmn_id,
                                   cell_id
                                   );
  }

  if(!look_at_shared_list && rrc_ppac_support_nv && rrc_check_if_ppac_is_present(sib3_ptr) == TRUE)
  {
    /* First store the access class pointer into a local data structure */
    rrc_PagingPermissionWithAccessControlParameters ppac_local = sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.v830NonCriticalExtension.sysInfoType3_v830ext.pagingPermissionWithAccessControlParametersForPLMNOfMIB;

    rrc_csp_int_data.is_curr_ppac_info_valid = TRUE;
    WRRC_MSG0_HIGH("PPAC:Present in SIB3 and is_curr_ppac_info_valid is set to TRUE");

    rrc_csp_find_ppac_restrictions(ppac_local,
                                   rrc_ppac_info,
                                   &rrc_int_access_class_barred_list,
                                   plmn_id,
                                   cell_id
                                   );
  }

  if(look_at_shared_list)
  {
    uint16 k = 0;
    rrc_dsac_info_for_shared_plmns_type dsac_info_for_shared_plmns[MAX_SHARED_PLMNS-1];
    rrc_DomainSpecificAccessRestrictionParam_v670ext dsac_local;
    num_of_shared_plmns = rrccsp_construct_multiple_plmn_list_from_mib(mib_ptr,shared_plmn_list);

    rrc_csp_fill_dsac_info_for_shared_plmns(num_of_shared_plmns,
                                            sib3_ptr,
                                            dsac_info_for_shared_plmns);

    if(rrccsp_is_mib_plmn_valid(mib_ptr))
    {
      /* Because Mib PLMN will be the first one among the shared PLMN list */
      k++;
    }

    for(i = 0;(i < MAX_SHARED_PLMNS-1) && (i <= num_of_shared_plmns) && (k < MAX_SHARED_PLMNS); i++,k++)
    {
      dsac_info_for_shared_plmns[i].plmn_id = shared_plmn_list[k];
    }

    /* By now dsac_info_for_shared_plmns contains all the shared PLMNs and their dsac restrictions */
    for(i = 0;(i < MAX_SHARED_PLMNS-1) && (i < num_of_shared_plmns); i++)
    {
      if(rrc_bcch_plmns_match(plmn_id, dsac_info_for_shared_plmns[i].plmn_id))
      {
        /* We are sure that this is not a mib common PLMN */
        break;
      }
    }

    if((i == num_of_shared_plmns) || (i == MAX_SHARED_PLMNS-1))
    {
      WRRC_MSG0_ERROR("NWS: Choosen PLMN not present in the MIB of this cell");
      rrc_csp_int_data.is_curr_dsac_info_valid = FALSE;
    }
    else if(dsac_info_for_shared_plmns[i].valid == TRUE)
    {
      dsac_local = dsac_info_for_shared_plmns[i].dsac_info;
      WRRC_MSG0_HIGH("NWS:DSAC Info for chosen PLMN present in SIB 3. Setting DSAC info valid to TRUE");
      rrc_csp_int_data.is_curr_dsac_info_valid = TRUE;
      rrc_csp_find_dsac_restrictions(dsac_local,
                                     rrc_dsac_info,
                                     &rrc_int_access_class_barred_list,
                                     plmn_id,
                                     cell_id
                                     );
    }
    else
    {
      rrc_csp_int_data.is_curr_dsac_info_valid = FALSE;
    }
  }

  if(look_at_shared_list && rrc_ppac_support_nv)
  {
    uint16 k = 0;
    rrc_ppac_info_for_shared_plmns_type ppac_info_for_shared_plmns[MAX_SHARED_PLMNS-1];
    rrc_PagingPermissionWithAccessControlParameters ppac_local;

    rrc_csp_fill_ppac_info_for_shared_plmns(num_of_shared_plmns,
                                            sib3_ptr,
                                            ppac_info_for_shared_plmns);

    if(rrccsp_is_mib_plmn_valid(mib_ptr))
    {
      /* Because Mib PLMN will be the first one among the shared PLMN list */
      k++;
    }

    for(i = 0;(i < MAX_SHARED_PLMNS-1) && (i <= num_of_shared_plmns) && (k < MAX_SHARED_PLMNS); i++,k++)
    {
      ppac_info_for_shared_plmns[i].plmn_id = shared_plmn_list[k];
    }

    /* By now ppac_info_for_shared_plmns contains all the shared PLMNs and their ppac restrictions */
    for(i = 0;(i < MAX_SHARED_PLMNS-1) && (i < num_of_shared_plmns); i++)
    {
      if(rrc_bcch_plmns_match(plmn_id, ppac_info_for_shared_plmns[i].plmn_id))
      {
        /* We are sure that this is not a mib common PLMN */
        break;
      }
    }

    if((i == num_of_shared_plmns) || (i == MAX_SHARED_PLMNS-1))
    {
      WRRC_MSG0_ERROR("NWS: Choosen PLMN not present in the MIB of this cell");
      rrc_csp_int_data.is_curr_ppac_info_valid = FALSE;
    }
    else if(ppac_info_for_shared_plmns[i].valid == TRUE)
    {
      ppac_local = ppac_info_for_shared_plmns[i].ppac_info;
      WRRC_MSG0_HIGH("NWS:PPAC Info for chosen PLMN present in SIB 3. Setting PPAC info valid to TRUE");
      rrc_csp_int_data.is_curr_ppac_info_valid = TRUE;
      rrc_csp_find_ppac_restrictions(ppac_local,
                                     rrc_ppac_info,
                                     &rrc_int_access_class_barred_list,
                                     plmn_id,
                                     cell_id
                                     );
    }
    else
    {
      rrc_csp_int_data.is_curr_ppac_info_valid = FALSE;
    }
  }

  if(rrc_csp_int_data.is_curr_dsac_info_valid == TRUE)
  {
    WRRC_MSG2_HIGH("DSAC: CS cell access: %d, PS cell access: %d",rrc_dsac_info->cs_access_control
                                                           ,rrc_dsac_info->ps_access_control
                                                           );
  }

  if(rrc_csp_int_data.is_curr_ppac_info_valid == TRUE)
  {
    MSG_HIGH("PPAC: CS reg allowed: %d, PS reg allowed: %d, Page rsp allowed: %d",rrc_ppac_info->cs_reg_allowed
                                                                               ,rrc_ppac_info->ps_reg_allowed
                                                                               ,rrc_ppac_info->page_rsp_allowed);
  }
  return (SUCCESS);
} /* rrc_csp_find_access_control */

/*===========================================================================

FUNCTION          RRC_FILL_ACCESS_CLASS_INFO

DESCRIPTION     This function is responsible for filling CS/PS Domain Specific Access Class information from access 
                          class barred list pointer
                          
DEPENDENCIES

RETURN VALUE
                  None
SIDE EFFECTS
                  None
===========================================================================*/
void rrc_fill_access_class_info(rrc_int_access_class_barred_list_type rrc_int_access_class_barred_list, 
                                rrc_csp_access_control_e_type *access_control,
                                rrc_plmn_identity_type  plmn_id,
                                rrc_int_cell_id_type    cell_id)
{  

    /* For UEs with access classes 11 to 15, Emergency calls are not allowed if
     * both "Access Class 10" and relevant Access Class (11 to 15) are barred.
     * Otherwise Emergency Calls are allowed - 25.304 (section 5.3.1.3)
     */

    if(rrc_int_access_class_barred_list.barred[RRC_INT_ACCESS_CLASS_10_INDEX]
            == TRUE)
    {
      if ( (rrc_hi_access_class != RRC_HI_ACCESS_CLASS_ABSENT) && 
           (rrc_csp_verify_hi_ac_access(plmn_id, 
                                          rrc_int_access_class_barred_list)
           == TRUE) )
      {
        /* Since UE's Hi Access Class is not barred and allows access to
         * regular calls we have full access
         */
        WRRC_MSG2_HIGH("Cell %d Access: All calls, AC10 barred but UE Hi AC 0x%x has access", cell_id, rrc_hi_access_class);
        *access_control = RRC_CSP_CELL_ACCESS_ALLOWED;
      }
      else
      {
        /* Since UE's High Access Class is barred or UE has no Hi Access
         * Class Emergency calls are NOT permitted
         */
        /* Check if UE has regular access - Check if UE lo Access Class is NOT barred */
        if ((rrc_lo_access_class != RRC_LO_ACCESS_CLASS_ABSENT) &&
            (rrc_int_access_class_barred_list.barred[rrc_lo_access_class]
                  == FALSE))
        {
          /* The cell is available for regular access but not for emergency calls
           * since the UE Low Access Class is NOT barred.
           */
          WRRC_MSG2_HIGH("Cell %d Access: Regular calls only. UE Low AC %d not barred", cell_id, rrc_lo_access_class);
          *access_control = RRC_CSP_REGULAR_CELL_ACCESS_ONLY;
        }
        else
        {
          WRRC_MSG2_HIGH("Cell %d Access: No Calls. AC10 barred,UE Lo AC 0x%x barred | No Hi AC | No SIM", cell_id, rrc_lo_access_class);
          *access_control = RRC_CSP_NO_CELL_ACCESS;
        }
      }
    } /* If "Access Class 10" bit is set */

    /* Check if UE has full access - Check if UE lo Access Class is NOT barred */
    else if ((rrc_lo_access_class != RRC_LO_ACCESS_CLASS_ABSENT) &&
        (rrc_int_access_class_barred_list.barred[rrc_lo_access_class]
              == FALSE))
    {
      /* The cell is available for full Access since the UE Low Access Class
       * is NOT barred. Since Access Class 0-9 are not barred
       * in Sys Info message and "Access Class 10" Bit is not set, Regular and 
       * emergency calls will be allowed.
       */
      WRRC_MSG2_HIGH("Cell %d Access: All calls. UE Low AC %d not barred", cell_id, rrc_lo_access_class);
      *access_control = RRC_CSP_CELL_ACCESS_ALLOWED;
    }

    /* Check if UE has full access - Check if Access to one of the UE Hi Access 
     * Classes is permitted */
    else if ((rrc_hi_access_class != RRC_HI_ACCESS_CLASS_ABSENT) &&
             (rrc_csp_verify_hi_ac_access(plmn_id, 
                                          rrc_int_access_class_barred_list)
             == TRUE ))
    {
      /* Since Access Class 11-15 are not barred in Sys Info message and
       * "Access Class 10" Bit is Not set, regular and emergency calls
       * are allowed.
       */

      /* We are in Home PLMN and UE Access Class is 11 to 15 - Full Access
       * is allowed.
       */
       WRRC_MSG2_HIGH("Cell %d Access: all calls. UE Hi AC 0x%x has access", cell_id, rrc_hi_access_class);
       *access_control = RRC_CSP_CELL_ACCESS_ALLOWED;
    }

    else
    { 
      /* Access to emergency calls IS permitted since "Access Class 10" bit is 
       * NOT set to rrc_barred
       */
      /* The current camped on Cell is restricted for Regular Call Access */
      WRRC_MSG1_HIGH("Cell %d Access: Emergency calls. AC10 not barred UE Hi and Lo AC: no Access", cell_id);
      *access_control = RRC_CSP_NO_REGULAR_CELL_ACCESS;
    } /* If "Access Class 10" bit is NOT set */
  return;
} /* rrc_fill_access_class_info */


/*===========================================================================

FUNCTION          RRC_CSP_FIND_RRC_CELL_ACCESS

DESCRIPTION       This function is responsible for finding the cell access
                  status. The cell access status is based on the cell 
                  restriction and access control.

DEPENDENCIES

RETURN VALUE
                  Cell Access status as understood by modules outside CSP.
SIDE EFFECTS
                  None
===========================================================================*/
static mm_as_cell_access_status_T rrc_csp_find_rrc_cell_access
(
 rrc_service_info_type *service_info_ptr,
 rrc_dsac_info_type *rrc_dsac_info,           /* DSAC Specific Info*/
 rrc_csp_access_control_e_type access_control,/* Access Control*/
 rrc_csp_acq_status_type  acq_status_mask    /* Acquisition status mask */
)   
{
  mm_as_cell_access_status_T      cell_access_status;
  /* Initialise cell_access_status to supress Lint warning*/
  cell_access_status = MM_AS_ACCESS_NORMAL_ONLY;

  if(acq_status_mask & RRC_CSP_IDLE_CELL_BAR_BIT)
  {
    /* Cell is barred as given by Cell status and cell reservations.
     * Check the cell access as given by Access Control.
     */
    if(rrc_dsac_info->valid != TRUE)
    {
      WRRC_MSG0_HIGH("DSAC: DSAC Info is not present ");
      if(access_control == RRC_CSP_NO_CELL_ACCESS)
      {
        /* Cell is barred AND Access to emergency calls NOT allowed */
        service_info_ptr->cell_access = MM_AS_ACCESS_NO_CALLS;
      }
      else
      {
        /* Cell is barred AND Access to emergency calls are allowed */
        service_info_ptr->cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
      }
    }    
    else
    {
      /* Fill the cs access control information */
      if(rrc_dsac_info->cs_access_control== RRC_CSP_NO_CELL_ACCESS)
      {
        /* Cell is barred AND Access to emergency calls NOT allowed */
        service_info_ptr->cs_cell_access= MM_AS_ACCESS_NO_CALLS;
      }
      else
      {
        /* Cell is barred AND Access to emergency calls are allowed */
        service_info_ptr->cs_cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
      }
      /* Fill the ps access control information */
      if(rrc_dsac_info->ps_access_control== RRC_CSP_NO_CELL_ACCESS)
      {
        /* Cell is barred AND Access to emergency calls NOT allowed */
        service_info_ptr->ps_cell_access= MM_AS_ACCESS_NO_CALLS;
      }
      else
      {
        /* Cell is barred AND Access to emergency calls are allowed */
        service_info_ptr->ps_cell_access= MM_AS_ACCESS_EMERGENCY_ONLY;
      }
      WRRC_MSG2_HIGH("DSAC: CS Cell Access is %d, PS Cell Access is %d", 
      service_info_ptr->cs_cell_access, service_info_ptr->ps_cell_access);
    }
  }
  else
  {

    /* Cell is not barred as given by Cell status and cell reservations.
     * Check the cell access as given by Access Control.
     */
    if(rrc_dsac_info->valid != TRUE)
    {
      MSG_MED("DSAC: DSAC Info is not present ",0,0,0);
      rrc_translate_access_control_to_access_class(access_control, 
                      &(service_info_ptr->cell_access));
    }
    else
    {
      /* Fill the cs access control information */
      rrc_translate_access_control_to_access_class(rrc_dsac_info->cs_access_control,
                       &(service_info_ptr->cs_cell_access));

      /* Fill the ps access control information */
      rrc_translate_access_control_to_access_class(rrc_dsac_info->ps_access_control, 
                       &(service_info_ptr->ps_cell_access));    
      WRRC_MSG2_HIGH("DSAC: CS Cell Access is = %d, PS Cell Access is = %d ",service_info_ptr->cs_cell_access,service_info_ptr->ps_cell_access);

    }

  } /* Cell not barred as given by Cell status */
  return(cell_access_status);
} /* rrc_csp_find_rrc_cell_access */


/*===========================================================================

FUNCTION          RRC_CSP_FIND_FORBID_LAI

DESCRIPTION       This function is responsible for finding the status of
                  the passed LAI. It checks the list of forbidden LAIs
                  to determine the status.

DEPENDENCIES
                  - The Forbidden LAI list in RRC CSP Internal data 
                    should have been initialized.

RETURN VALUE
                  SUCCESS: If LAI status could be successfully evaluated.
                  FAILURE: If LAI status could not be evaluated.
SIDE EFFECTS
                  None
===========================================================================*/
static uecomdef_status_e_type
rrc_csp_find_forbid_lai(rrc_lai_type  *lai, 
                        rrc_csp_lai_status_e_type *lai_status)
{
  uint32 i;

#ifdef FEATURE_FEMTO_CSG
  rrc_csp_substate_e_type csp_substate = rrc_get_csp_substate();
#endif

  *lai_status = RRC_CSP_LAI_OK;

#ifdef FEATURE_FEMTO_CSG
  /*Skip FLAI for cell selection process when in manual CSG mode*/
  /*Spec reference:- 25.304 4.3*/
  MSG_HIGH("CSG_CR: curr scan %d, sub state %d, csg id %d",
            rrc_csp_int_data.curr_scan, csp_substate, rrc_csp_int_data.curr_req_plmn.csg_id);
  if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)&&
     (csp_substate == RRCCSP_CELL_SEL_WT_SIB)&&
     (rrc_csp_int_data.curr_req_plmn.csg_id != SYS_CSG_ID_INVALID))
  {
    WRRC_MSG0_HIGH("CSG: Skip FLAI check");
    return(SUCCESS);
  }

#endif

  /* If LAI is to be checked for forbidden status, compare given LAI with
   * ones in forbidden LAI list in RRC CSP Internal Data 
   */

  for( i = 0; i < rrc_csp_int_data.forbid_lai_list.num_lais; i++ )
  {
    if( rrc_csp_bcch_and_nas_lais_match(*lai, rrc_csp_int_data.forbid_lai_list.lai[i]) )
    {
      if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN) ||
         (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_CC_ORDER_SCAN))
      {
        rrc_csp_int_data.forbidden_lai = rrc_csp_int_data.forbid_lai_list.lai[i];
      }

      *lai_status = RRC_CSP_FORBID_LAI;
      WRRC_MSG0_HIGH("LAI is forbidden");
      break;
    }
  }

  return(SUCCESS);
} /* rrc_csp_find_forbid_lai */

/*===========================================================================

FUNCTION          RRC_CSP_CHECK_FOR_LAI_FORBIDDEN

DESCRIPTION       This function is responsible for finding the status of
                  the passed LAI. It checks the list of forbidden LAIs
                  to determine the status.

DEPENDENCIES
                  - The Forbidden LAI list in RRC CSP Internal data 
                    should have been initialized.

RETURN VALUE
                  SUCCESS: If LAI status could be successfully evaluated.
                  FAILURE: If LAI status could not be evaluated.
SIDE EFFECTS
                  None
===========================================================================*/

boolean rrccsp_check_for_lai_forbidden(rrc_SysInfoType1 *sib1_ptr,
                             rrc_MasterInformationBlock *mib_ptr)
{
  rrc_csp_lai_status_e_type  lai_status;
  rrc_lai_type  lai;
  uint32 i;
  rrc_plmn_identity_type shared_plmn_list[MAX_SHARED_PLMNS];
  uint8 shared_plmn_idx = 0;
  uint8  num_of_shared_plmns = 0;
  boolean nw_shared = FALSE;  
  unsigned char  *data_ptr;
  rrc_gsm_map_nas_sys_info_type   nas_common_info;
  rrc_plmn_identity_type mib_plmn;
  
  rrc_plmn_identity_type mib_plmn_id;
  rrc_translate_gsm_map_plmn_id(&(mib_ptr->plmn_Type.u.gsm_MAP->plmn_Identity),&mib_plmn);
  if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
    nas_common_info.length = (uint32)sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts;
    data_ptr = sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.data;
    for( i = 0; i <sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts; i++)
    {
      nas_common_info.value[i] =  *data_ptr;
      data_ptr ++;
    }
    if(mm_extract_lac_from_sib_info(&lai, &nas_common_info) == FALSE)
    {
      ERR_FATAL("Could not extract LAI from NAS Common sys info", 0, 0, 0);
    }
    lai.plmn = mib_plmn;
    if(rrc_csp_find_forbid_lai(&lai, &lai_status) == FAILURE)
    {
      WRRC_MSG0_HIGH("Failure finding LAI status");
      return FALSE;
    }
    if(lai_status != RRC_CSP_FORBID_LAI)
    {
      return FALSE;
    }
    if(rrc_is_nw_shared(mib_ptr))
    {
      nw_shared = TRUE;
      num_of_shared_plmns = rrccsp_construct_multiple_plmn_list_from_mib(mib_ptr,shared_plmn_list);
    }
    do
    {
      if(nw_shared)
      {
        mib_plmn_id = shared_plmn_list[shared_plmn_idx++];
      }
      else
      {
        mib_plmn_id=mib_plmn;
      }
      lai.plmn = mib_plmn_id;
      if(rrc_csp_find_forbid_lai(&lai, &lai_status)  == FAILURE)
      {
        WRRC_MSG0_HIGH("Failure finding LAI status");
        return FALSE;
      }
      if(lai_status != RRC_CSP_FORBID_LAI)
      {
        return FALSE;
      }
    }while(shared_plmn_idx < num_of_shared_plmns);

    if(lai_status == RRC_CSP_FORBID_LAI)
    {
      WRRC_MSG0_HIGH("LA is forbidden");
      rrc_csp_int_data.bar_for_resel_only = TRUE;
      /* Send Cell bar request to L1 */
      rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_FREQ_BARRED, 
                                    FORBIDDEN_RESEL_FREQ_BAR_TIMER_IN_SEC);
      rrc_csp_int_data.bar_for_resel_only = FALSE;
      if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
      {
        rrc_csp_int_data.plmn_fnd_during_fgnd_svc_srch = TRUE;
      }
      return TRUE;
    }  
  }
  return FALSE;
}
/*===========================================================================

FUNCTION          RRCCSP_REMOVE_ACQDB_BANDS_FROM_PRIORITIZED_LIST

DESCRIPTION       Removes bands corresponding to entries in ACQ-DB from the prioritized band list.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
static void rrccsp_remove_acqdb_bands_from_prioritized_list(void)
{
  int8 idx,idy = 0;

  if(rrc_csp_curr_select_data.num_of_prioritized_bands > MAX_NUM_OF_WCDMA_BANDS)
  {
    WRRC_MSG1_ERROR("ERR: Exceeded the max no of bands %d", rrc_csp_curr_select_data.num_of_prioritized_bands);
    return;
  }
  
  for(idx = (rrc_csp_curr_select_data.num_of_prioritized_bands-1); idx >= 0 ; idx--)
  {
    if(rrc_csp_int_data.deep_sleep_acq_db_band_mask & rrc_csp_curr_select_data.prioritized_bands[idx])
    {
      if(idx != (rrc_csp_curr_select_data.num_of_prioritized_bands-1))
      {
        for(idy = idx+1; (idy < rrc_csp_curr_select_data.num_of_prioritized_bands) && (idy < MAX_NUM_OF_WCDMA_BANDS); idy++)
        {
          rrc_csp_curr_select_data.prioritized_bands[idy -1] = rrc_csp_curr_select_data.prioritized_bands[idy];
        }
      }
      rrc_csp_curr_select_data.num_of_prioritized_bands--;
      MSG_HIGH("WBP: Removing band %d, num_pri_bands: %d, idx: %d", rrc_csp_curr_select_data.prioritized_bands[idx], 
         rrc_csp_curr_select_data.num_of_prioritized_bands, idx);
    }
  }
}

/*===========================================================================

FUNCTION          RRC_CSP_INIT_INT_DATA

DESCRIPTION       This function is responsible for initializing the internal
                  data structure to default values.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void  rrc_csp_init_int_data( void )
{ 
  /* Set the last CSP event to "No Event" */
  rrc_csp_int_data.last_mm_primitive = RRC_CSP_NO_EVENT_RCVD;
  rrc_csp_int_data.curr_scan = RRC_CSP_CELL_SELECTION_SCAN;

  rrc_csp_internal_service_req = FALSE;

  /* Lets presume that MCC=0 and MNC=0 work as Invalid PLMN 
   * TBD: A way to indicate invalid PLMN
   * NOTE: It is presumed that Home PLMN is of GSM MAP type.
   */
  rrc_csp_int_data.home_plmn_id.mcc[0] = 0;
  rrc_csp_int_data.home_plmn_id.mcc[1] = 0;
  rrc_csp_int_data.home_plmn_id.mcc[2] = 1;
  rrc_csp_int_data.home_plmn_id.num_mnc_digits  = 3;
  rrc_csp_int_data.home_plmn_id.mnc[0] = 0;
  rrc_csp_int_data.home_plmn_id.mnc[1] = 0;
  rrc_csp_int_data.home_plmn_id.mnc[2] = 1;


  /* Reset the plmn list and scan type in CSP internal data */
  rrc_csp_int_data.plmn_list.num_plmns = 0;

  rrc_csp_int_data.interrat_reselect_failure_reason = RRC_WCDMA_NONE;
  rrc_csp_int_data.wcdma_cell_squalmin = 0;
  rrc_csp_int_data.wcdma_cell_srxlevmin = 0;

  rrc_csp_int_data.dest_freq_present = FALSE;
  rrc_csp_int_data.dest_psc_present = FALSE;
  rrc_csp_int_data.pending_type = RRC_CSP_NONE_PENDING;

  rrc_csp_int_data.curr_acq_count = 0;

  rrc_csp_int_data.init_cell_sel_ind_during_wtointerrat = FALSE;

  /* BPLMN 2G->3G */
  rrc_csp_int_data.bplmn_new_srch               = TRUE;
  rrc_csp_int_data.bplmn_srch_tmr_expired       = TRUE;

  rrc_csp_int_data.send_partial_results         = FALSE;
  memset(&rrc_csp_int_data.bplmn_search_timer_value,0,sizeof(sys_manual_search_timer_value_type));

  rrc_csp_int_data.bplmn_sib_try_next_best_cell = FALSE;
  rrc_csp_int_data.bplmn_srch_cnf               = TRUE;
  rrc_csp_int_data.bplmn_acq_status             = FALSE;

  rrc_csp_int_data.bplmn_req_list_rd_index  = 0;
  rrc_csp_int_data.bplmn_fnd_list_wr_index  = 0;
  rrc_csp_int_data.bplmn_num_req            = 0;  
  rrc_csp_int_data.bplmn_current_band_initialized = FALSE;

  rrc_csp_int_data.bplmn_bgnd_mib_rd_cnt  = 0;
  rrc_csp_int_data.bplmn_bgnd_mib_rd_freq = 0;
  rrc_csp_int_data.bplmn_srch_vars_status = WTOW_SEARCH_STOP;
  rrc_csp_int_data.bplmn_trans_id= INVALID_LIST_TRANS_ID;
  rrc_csp_int_data.bplmn_dual_mode_enabled = FALSE;
  rrc_csp_int_data.bplmn_wcdma_mode_enabled = TRUE;

  rrc_csp_int_data.service_search_bgnd = SYS_SERVICE_SEARCH_NONE;
  WRRC_MSG0_HIGH("CSG: Resetting service search");
  rrc_csp_int_data.rsp_to_rr = SEND_GTOW_RESEL_REJECT;
  /* OOS Handling in Connected Mode */
#ifdef FEATURE_WRLF_SYSTEM_SEL
  if(!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
  {
  rrc_csp_int_data.oos_splmn_srch_tmr_expd = TRUE;
  }
  rrc_csp_int_data.oos_gsm_srch_done       = FALSE;
  rrc_csp_int_data.oos_gsm_num_avail_plmns = 0;
  /* Initialize the variable with an invalid value */
  rrc_csp_int_data.aset_scr_code = 8192;

#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
  /* Used to maintain the information whether GSM service search
   * has been done or not before doing full search on PCS/850 
   * bands on W */
  rrc_csp_int_data.gsm_svc_srch_done = FALSE;
#endif

  /* Initialize variables associated with WTOG BPLMN search */
  rrc_csp_init_wtog_plmn_srch_int_data();

  /* Initialize GSM list size to 0 */
#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  rrc_csp_int_data.umts_chls_from_gsm.size = 0;
#endif

  /* Initialize Deep Sleep in RRC Connected Mode Variables */
  rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd = TRUE;
  rrc_csp_int_data.oos_conn_mode_ds_tmr_expd          = TRUE;

  rrc_csp_int_data.bar_for_resel_only= FALSE;
  rrc_csp_int_data.non_eq_plmn_barred=FALSE;

#ifdef FEATURE_GAN
  #error code not present
#endif

  rrc_csp_int_data.gtow_redirection_timer_expired = TRUE;
  rrc_csp_int_data.skip_repeat_scan_for_redir = FALSE;
  rrc_csp_int_data.target_fdd_cell_list.num_of_frequencies = 0;
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif
  rrc_csp_int_data.cel_resel_ind_rcvd_waiting_trans_cnf = FALSE;
  rrc_csp_int_data.cel_trans_cnf_recvd_at_llc = TRUE;

  rrc_csp_int_data.skip_gsm_search = FALSE;
  rrc_csp_int_data.est_req_for_emerg_call = FALSE;

  rrc_csp_int_data.do_lfs_scan = FALSE;
  rrc_csp_int_data.mode_change_reason = SYS_STOP_MODE_REASON_MODE_CHANGE;
  rrc_csp_int_data.stop_pending_for_wtog_abort = FALSE;

#ifdef FEATURE_DUAL_SIM
  rrc_csp_int_data.preemption_handling_in_progress = FALSE;
  rrc_csp_int_data.csp_waiting_for_wl1_to_release_lock = FALSE;
  rrc_stop_lock_wait_timer();
  rrc_csp_int_data.shld_acq_be_spltd = FALSE;
#endif
  rrc_csp_int_data.gsm_camped_suitably = FALSE;
  rrc_csp_int_data.cell_reserved_for_operator_use = FALSE;
#if defined(FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  rrc_csp_int_data.interrat_type = RRC_CSP_INTERRAT_NONE;
  rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_NONE;
  rrc_csp_int_data.detected_frequency_list.num_freqs = 0;
  rrc_csp_int_data.exclude_preredir_info_in_conn_req_msg = FALSE;
  rrc_csp_int_data.cgi_info_included = FALSE;
#endif

  rrc_csp_int_data.plmn_srch_order.num_rats = 0;
  rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx = -1;

#ifdef FEATURE_3GPP_CSFB
  rrccsp_free_sib_cont_list();
  rrc_csp_int_data.sib_read_csfb_active = FALSE;
#endif

#ifdef FEATURE_LTE_TO_WCDMA
  rrctmr_stop_timer(RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER);
#endif

#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
  rrc_csp_int_data.waiting_for_activation_rsp = FALSE;
#endif

  rrc_csp_int_data.allow_lte_resel = TRUE;

  rrc_csp_int_data.primary_plmn_Id_present = FALSE;

  rrc_csp_int_data.barred_due_to_band_mismatch = FALSE;

#ifdef FEATURE_FEMTO_CSG
  rrc_csp_int_data.csg_asf_srch_in_progress = FALSE;
  memset(&rrc_csp_int_data.temp_csg_list,0,sizeof(rrc_csp_temp_csg_cell_list));
  rrctmr_stop_timer(RRCTMR_CSG_ASF_SRCH_TIMER);
  rrc_csp_int_data.curr_req_plmn.csg_id = SYS_CSG_ID_INVALID;
#endif

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
  csfb_extended_status = CSFB_INVALID;
  rrc_cs_rab_established = FALSE;
#endif
  rrc_csp_int_data.ignore_csp_change_mode_ind = FALSE;
  rrc_csp_int_data.is_dsac_valid = FALSE;
  rrc_csp_int_data.is_ppac_valid = FALSE;


} /* rrc_csp_init_int_data */

/*===========================================================================

FUNCTION          RRC_CSP_INIT_WTOG_PLMN_SRCH_INT_DATA

DESCRIPTION       This function is responsible for initializing the internal
                  data structure to default values for WTOG PLMN Search.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void  rrc_csp_init_wtog_plmn_srch_int_data( void )
{ 
  rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_aborted = FALSE;
  rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active  = FALSE;
  rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found    = 0;
  rrc_csp_wtog_bplmn_int_data.wtog_hplmn_found        = FALSE;
  rrc_csp_wtog_bplmn_int_data.wtog_rem_drx_tmr_value  = 0;
  rrc_csp_wtog_bplmn_int_data.wtog_srch_done          = FALSE;
  rrc_csp_wtog_bplmn_int_data.restart_bplmn_search    = FALSE;
#ifdef FEATURE_WCDMA_TO_LTE
  rrc_csp_wtog_bplmn_int_data.wtol_plmns_found    = 0;
#endif

  rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_suspended = FALSE;
} /* rrc_csp_init_wtog_plmn_srch_int_data */

#ifdef FEATURE_EOOS
#error code not present
#endif /* FEATURE_EOOS */

/*===========================================================================

FUNCTION          RRCCSP_IS_SUPPORTED_BAND

DESCRIPTION       This function checks whether the passed in band is supported
                  by the UE or not.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
boolean rrccsp_is_supported_band(rrc_csp_band_class_type band_class)
{

#ifndef FEATURE_WCDMA_MULTIBAND
  /* Check if the PCS or IMT2K band should be checked */
   if(band_class & RRC_CSP_PCS_BAND)
   {
     return TRUE;
   }
   else
   {
     return FALSE;
   }
 
#else
  if(band_class & rrc_csp_int_data.wcdma_band_mask)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
#endif /* FEATURE_WCDMA_MULTIBAND */

} /* rrccsp_is_supported_band */


/*===========================================================================

FUNCTION          RRCCSP_IS_CURRENT_BAND

DESCRIPTION       This function checks whether the band passed to the function is 
                  the same as the current band, i.e., the band on which the UE is 
                  camped on or searching for service. This function is currently 
                  called by only SIB procedure for serving cell to determine if the 
                  frequency band indicator in SIB5/SIB5bis/SIB6 is the same as the 
                  band on which UE is camped on or searching for service.
                  
DEPENDENCIES
                  None.
                  
RETURN VALUE
                  TRUE: band passed is the same as the current band.
                  FALSE: otherwise.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
boolean rrccsp_is_current_band(rrc_csp_band_class_type band_class,rrc_plmn_identity_type plmn_id)
{
#ifdef FEATURE_WCDMA_MULTIBAND

  /* Check if band_class belongs to 800 or 850 band */
  if((band_class & RRC_CSP_WCDMA_800_BAND) || (band_class & RRC_CSP_WCDMA_850_BAND))
  {
    if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN)
       || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN)
       || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN)
       || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN)
      )
    {
      if(rrc_csp_int_data.dest_freq_present)
      {
        WRRC_MSG1_HIGH("Dest freq present for %d scan", rrc_csp_int_data.curr_scan);
        return(rrccsp_is_supported_band(band_class));
      }
    }

    /* Current band is correctly initialized during ACQ DB / RAW SCAN. In the scan types
     * below, the acq is attempted on a freq before starting ACQ DB / RAW SCAN hence we
     * need to check for supported band instead of current band
     */
    if(   (rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)
       || (rrc_csp_int_data.curr_scan == RRC_CSP_INTER_FREQ_REDIRECTION_SCAN))
    {
      WRRC_MSG2_HIGH("For curr_scan %d, checking supported_band instead of current_band %d",
                        rrc_csp_int_data.curr_scan, rrc_csp_curr_select_data.current_band);
      return(rrccsp_is_supported_band(band_class));
    }

    if((band_class == rrc_get_frequency_band(rrc_csp_int_data.curr_acq_entry.freq,plmn_id))
         && (rrccsp_is_supported_band(band_class)))
    {
      WRRC_MSG2_HIGH("acq freq %d belongs to the band_class %d", rrc_csp_int_data.curr_acq_entry.freq, 
                                                           band_class);
      return TRUE;
    }
    else
    {
      WRRC_MSG2_HIGH("acq freq %d does NOT belong to the band_class %d", rrc_csp_int_data.curr_acq_entry.freq, 
                                                                   band_class);
      return FALSE;
    } 
  }
  /* band class does not belong to 800 or 850 bands */
  else
  {
    if(band_class == rrc_get_frequency_band(rrc_csp_int_data.curr_acq_entry.freq, 
                                            rrc_csp_int_data.curr_req_plmn.plmn_id))
    {
      WRRC_MSG2_HIGH("acq freq %d belongs to the band_class %d", rrc_csp_int_data.curr_acq_entry.freq, 
                                                           band_class);
      return TRUE;
    }
    else
    {
      WRRC_MSG2_HIGH("acq freq %d does NOT belong to the band_class %d", rrc_csp_int_data.curr_acq_entry.freq, 
                                                                   band_class);
      return FALSE;
    }
  }
#else
  /* Check if the PCS or IMT2K band should be checked */
   if(band_class & RRC_CSP_PCS_BAND)
   {
     WRRC_MSG1_HIGH("band_class %d is the current band",band_class);
     return TRUE;
   }
   else
   {
     WRRC_MSG2_HIGH("band_class %d is NOT the current band %d",band_class,rrc_csp_curr_select_data.current_band);
     return FALSE;
   }
#endif /* FEATURE_WCDMA_MULTIBAND */
} /* rrccsp_is_current_band */


#ifdef FEATURE_WCDMA_MULTIBAND
/*===========================================================================

FUNCTION          RRCCSP_GET_NEXT_BAND_TO_BE_SCANNED

DESCRIPTION       This function returns the next frequency band to be scanned
                  based on the last scanned band.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
static rrc_csp_band_class_type
rrccsp_get_next_band_to_be_scanned( void )
{
  uint16 tmp_shift_bit_mask = RRC_CSP_BAND_MAX >> 1;

  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Bands, First = %d, Current = %d, next = %d, Addl: addl_chnl_done_mask = %d",
           rrc_csp_curr_select_data.first_band_to_be_scanned,
           rrc_csp_curr_select_data.current_band,
           rrc_csp_curr_select_data.next_band_mask,
           rrc_csp_curr_select_data.addl_chnl_done_mask);

  if(   rrc_csp_curr_select_data.next_band_mask 
      & rrc_csp_curr_select_data.first_band_to_be_scanned
      & rrc_csp_curr_select_data.addl_chnl_done_mask
    )
  {
    WRRC_MSG1_HIGH("Addl: need to scan the first band %d still. Also remove it from the next_band_mask", 
                        rrc_csp_curr_select_data.first_band_to_be_scanned);

    /* Update the next_band_mask. Remove first_band_to_be_scanned from it */
    rrc_csp_curr_select_data.next_band_mask &= ~rrc_csp_curr_select_data.first_band_to_be_scanned;

    return rrc_csp_curr_select_data.first_band_to_be_scanned;
  }
  
  /* Update the next_band_mask. Remove first_band_to_be_scanned 
   * from it. 
   */
  rrc_csp_curr_select_data.next_band_mask &= 
    ~rrc_csp_curr_select_data.first_band_to_be_scanned;

  if(rrc_csp_curr_select_data.prioritized_bands_read_idx < rrc_csp_curr_select_data.num_of_prioritized_bands)
  {
    MSG_HIGH("WBP:prioritized_bands_read_idx: %d, num_of_prioritized_bands: %d ,next_band_mask : %d",
                                                                                 rrc_csp_curr_select_data.prioritized_bands_read_idx,
                                                                                 rrc_csp_curr_select_data.num_of_prioritized_bands,
                                  rrc_csp_curr_select_data.next_band_mask);

    tmp_shift_bit_mask = rrc_csp_curr_select_data.prioritized_bands[rrc_csp_curr_select_data.prioritized_bands_read_idx];

    if ((tmp_shift_bit_mask == RRC_CSP_PCS_BAND)
    || (tmp_shift_bit_mask == RRC_CSP_WCDMA_850_BAND)
    || (tmp_shift_bit_mask == RRC_CSP_WCDMA_800_BAND)
    || (tmp_shift_bit_mask == RRC_CSP_WCDMA_1700_2100_BAND)
    || (tmp_shift_bit_mask == RRC_CSP_WCDMA_BC19_BAND)
    )
    {

      if(tmp_shift_bit_mask & rrc_csp_curr_select_data.addl_chnl_done_mask)
      {
        WRRC_MSG1_HIGH("Addl chl scan NOT needed for band %d", tmp_shift_bit_mask);
        rrc_csp_curr_select_data.addl_chls_scan_needed = FALSE;
      }
      else
      {
        rrc_csp_curr_select_data.addl_chls_scan_needed = TRUE;
        rrc_csp_curr_select_data.addl_chnl_done_mask |= tmp_shift_bit_mask;
        WRRC_MSG2_HIGH("Addl chl scan needed for band %d. Added to addl_chnl_done_mask %d", tmp_shift_bit_mask,rrc_csp_curr_select_data.addl_chnl_done_mask);
      } 
    }
    else
    {
      WRRC_MSG1_HIGH("Addl chl scan not needed for band %d", tmp_shift_bit_mask);
      rrc_csp_curr_select_data.addl_chls_scan_needed = FALSE;
    }

    if(rrc_csp_curr_select_data.addl_chls_scan_needed)
    {
      WRRC_MSG1_HIGH("Addl: Do NOT remove the band %d from the band mask", tmp_shift_bit_mask);
      return tmp_shift_bit_mask;
    }
    else
    {
      rrc_csp_curr_select_data.next_band_mask &= ~tmp_shift_bit_mask;

      WRRC_MSG1_HIGH("WBP: Band to be scanned: %d ",tmp_shift_bit_mask);
      rrc_csp_curr_select_data.prioritized_bands_read_idx++;
      return tmp_shift_bit_mask;
    }
  }

  WRRC_MSG1_HIGH("WBP: next_band_mask : %d",rrc_csp_curr_select_data.next_band_mask);
  tmp_shift_bit_mask = RRC_CSP_BAND_MAX >> 1;


  while(tmp_shift_bit_mask)
  {
    if(tmp_shift_bit_mask & rrc_csp_curr_select_data.next_band_mask)
    {
      if ((tmp_shift_bit_mask == RRC_CSP_PCS_BAND)
	  || (tmp_shift_bit_mask == RRC_CSP_WCDMA_850_BAND)
	  || (tmp_shift_bit_mask == RRC_CSP_WCDMA_800_BAND)
	  || (tmp_shift_bit_mask == RRC_CSP_WCDMA_1700_2100_BAND)
	  || (tmp_shift_bit_mask == RRC_CSP_WCDMA_BC19_BAND)
      )
      {
        if(tmp_shift_bit_mask & rrc_csp_curr_select_data.addl_chnl_done_mask)
        {
          WRRC_MSG1_HIGH("Addl chl scan NOT needed for band %d", tmp_shift_bit_mask);
          rrc_csp_curr_select_data.addl_chls_scan_needed = FALSE;
        }
        else
        {
          rrc_csp_curr_select_data.addl_chls_scan_needed = TRUE;
          rrc_csp_curr_select_data.addl_chnl_done_mask |= tmp_shift_bit_mask;
          WRRC_MSG2_HIGH("Addl chl scan needed for band %d. Added to addl_chnl_done_mask = %d", tmp_shift_bit_mask, rrc_csp_curr_select_data.addl_chnl_done_mask);
        } 
      }
      else
      {
        WRRC_MSG1_HIGH("Addl chl scan not needed for band %d", tmp_shift_bit_mask);
        rrc_csp_curr_select_data.addl_chls_scan_needed = FALSE;
      }

      if(rrc_csp_curr_select_data.addl_chls_scan_needed)
      {
        return tmp_shift_bit_mask;
      }
      else
      {
        /* Eliminate the particular band being returned from the
         * next_band_mask in preparation for next time
         */
        rrc_csp_curr_select_data.next_band_mask &= ~tmp_shift_bit_mask;

        return tmp_shift_bit_mask;
      }
    }
    else
    {
      tmp_shift_bit_mask = tmp_shift_bit_mask >> 1;
    }
  }

  return RRC_CSP_BAND_MAX;

} /* rrccsp_get_next_band_to_be_scanned */

#endif /* FEATURE_WCDMA_MULTIBAND */


/*===========================================================================

FUNCTION          RRC_CSP_CONTINUE_INITIAL_STORED_SEL_CNTRL

DESCRIPTION       This function continues with the already started Initial 
                  and Stored Cell Selection
                  It looks for a frequency for a "specific" PLMN or
                  "Any" PLMN. It saves its current search state in Current
                  Selection Controller Data.

DEPENDENCIES
                  This function is closely knit with 
                  rrc_csp_compare_expected_cell() and
                  rrc_csp_get_match_acq_entry().
                  A change in logic in either must be reflected in others.

RETURN VALUE
                  SUCCESS: A frequency has been found where Acquisition 
                           can be tried.
                  FAILURE: No frequency found (all exhausted). Time for a 
                           deep sleep ?

SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type
rrc_csp_continue_stored_initial_select_ctrl
(
/*lint -e715*/
  rrc_csp_acq_e_type       *ret_type_acq_ptr,
  rrc_csp_acq_entry_type   *ret_acq_entry_ptr,
  rrc_csp_freq_scan_e_type *ret_type_freq_scan
)
{
  rrc_plmn_identity_type      req_plmn_id;
  rrc_freq_type               ret_freq;
  rrc_csp_acq_entry_type      tmp_acq_entry;
  rrc_csp_acq_entry_type      *acq_entry_ptr  = NULL;

  /* Indicates when all search is exhausted with no success */
  boolean      all_search_exhausted = FALSE;
#ifndef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
  boolean do_gsm_bands_scan_first = rrccsp_do_gsm_bands_scan_first();
#endif
#endif

#ifdef FEATURE_EOOS
  #error code not present
#endif

  rrclog_wcdma_rrccsp_scan_start_event_type rrccsp_scan_start_evt;
#ifndef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
  if(do_gsm_bands_scan_first)
  {
    if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) && (rrc_csp_int_data.skip_gsm_search))
    {
      do_gsm_bands_scan_first = FALSE;
    }
  }
#endif
#endif

    /* MM/RR has requested to look for specific PLMN */
  req_plmn_id = rrc_csp_curr_select_data.req_plmn.plmn_id;
  
  /* This is done for BPLMN searches. Once ACQ DB is accessed, this variable should
   * be set to FALSE
   */
  rrc_csp_int_data.bplmn_sib_try_next_best_cell = FALSE;

  do  /* Keep looping till an Acq Entry is found */
  {
#ifdef FEATURE_EOOS
    #error code not present
#endif

    /* Proceed based on Current Selection Data */
    if((rrc_csp_curr_select_data.num_tries_on_dest_freq > 0) &&
       (rrc_csp_curr_select_data.dest_freq_in_use == TRUE))
    {
      rrc_csp_curr_select_data.num_tries_on_dest_freq --;
      acq_entry_ptr = &tmp_acq_entry;
      acq_entry_ptr->freq = rrc_csp_int_data.dest_freq;

#ifdef FEATURE_LTE_TO_WCDMA
      if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) &&
          (rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE))
      {
        WRRC_MSG2_HIGH("LTOW: Freq %d try: %d", 
                 rrc_csp_int_data.dest_freq, 
                 (RRCCSP_LTOW_MAX_ACQ_TRIES_ON_DIRECTED_FREQ - rrc_csp_curr_select_data.num_tries_on_dest_freq));
      }
      else
#endif
      {
        WRRC_MSG2_HIGH("Freq %d try: %d", 
                 rrc_csp_int_data.dest_freq, 
                 (RRC_CSP_MAX_ACQ_TRIES_ON_DIRECTED_FREQ - rrc_csp_curr_select_data.num_tries_on_dest_freq));
      }
    }
#ifdef FEATURE_LTE_TO_WCDMA
    /* Perform list scan on the neighbour freq list provided by LTE-RRC */
    else if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) &&
       (rrc_csp_curr_select_data.dest_freq_in_use == TRUE) &&
       (rrc_csp_int_data.num_freq_in_list_db > 0) &&
       (rrc_csp_int_data.curr_rd_idx == 0))
    {
      rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_LIST_SCAN;
      WRRC_MSG1_HIGH("LFS_COOS: set curr frequency scan type =%d",
             rrc_csp_curr_select_data.scan_info.type_freq_scan);
      *ret_type_freq_scan = RRC_CSP_LIST_SCAN;
      return SUCCESS;
    }
#endif
    else if(rrc_nv_channel_lock_item_value !=0)
    {
      WRRC_MSG0_HIGH("NV is already read, ALL SEARCH EXHAUSTED");
      *ret_type_freq_scan = RRC_CSP_NO_SCAN;
      ret_acq_entry_ptr = NULL;
      all_search_exhausted = TRUE;
    }
    else
    {
      switch(rrc_csp_curr_select_data.acq_type)
      {
        case RRC_CSP_SPEC_PLMN:

          /* Check if Acq. database or complete frequency band is being used */
          if(rrc_csp_curr_select_data.acq_db_in_use == TRUE)
          {
            if(rrc_csp_curr_select_data.acq_db_scan_info.start_with_first)
            {
              MSG_LOW("Start: Specific PLMN srch ACQ DB", 0, 0, 0);
              rrc_csp_curr_select_data.acq_db_scan_info.start_with_first = FALSE;

              /* Do not reset num_available_freq to 0 when Automatic BPLMN search is in 
               * progress and the second or later PLMN entry in the NAS HPLMN list is 
               * being matched. 
               */
              if(((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
                  || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))
                 && (rrc_csp_int_data.bplmn_req_list_rd_index == 0))
              {
                rrc_csp_curr_select_data.num_available_freq = 0;
              }
              /* Reset num_available_freq to 0 for all other scans */
              else if((rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
                      && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))
              {
                rrc_csp_curr_select_data.num_available_freq = 0;
              }

              acq_entry_ptr = rrc_csp_get_first_match_acq_entry(req_plmn_id,
                                            rrc_csp_curr_select_data.acq_type);

              if(acq_entry_ptr == NULL)
              { 
                MSG_MED("SPEC_PLMN rrc_csp_int_data.acq_db_empty = %d", rrc_csp_int_data.acq_db_empty,0,0);
#ifdef FEATURE_WCDMA_MULTIBAND
                rrc_csp_curr_select_data.current_band = 
                  rrc_csp_curr_select_data.first_band_to_be_scanned;
#endif
              }
              else
              {
                //EVENT: EVENT_WCDMA_RRCCSP_SCAN_START
                rrccsp_scan_start_evt.scan_type = ACQ_DB_SCAN;
                event_report_payload(EVENT_WCDMA_RRCCSP_SCAN_START, sizeof(rrclog_wcdma_rrccsp_scan_start_event_type),
                               (void *)&rrccsp_scan_start_evt);

#ifdef FEATURE_WCDMA_MULTIBAND
                /* If Automatic BPLMN search is in progress then determine if 
                 * current_band needs to be set or not based on the value of 
                 * bplmn_current_band_initialized. The value stored in current_band
                 * is used to determine which band needs to be scanned first
                 * during full freq scan.
                 * The check below is to avoid updating current_band multiple times
                 * as it may change the order in which full freq scan is done
                 * on supported bands.
                 */
                if((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))
                {
                  if(!rrc_csp_int_data.bplmn_current_band_initialized)
                  {
                    rrc_csp_curr_select_data.current_band = acq_entry_ptr->band_class;
                    rrc_csp_curr_select_data.first_band_to_be_scanned = acq_entry_ptr->band_class;
                    WRRC_MSG1_HIGH("Setting 1st_band and current_band to %d",
                             rrc_csp_curr_select_data.current_band);
                    rrc_csp_int_data.bplmn_current_band_initialized = TRUE;
                  }
                }
                /* Initialize current_band and first_band_to_be_scanned */
                else
                {
                  if(rrc_csp_curr_select_data.num_of_prioritized_bands != 0)
                  {
                    rrc_csp_curr_select_data.current_band = rrc_csp_curr_select_data.first_band_to_be_scanned;
                    WRRC_MSG1_HIGH("Setting current_band to %d",
                             rrc_csp_curr_select_data.current_band);
                  }
                  else
                  {
                    rrc_csp_curr_select_data.current_band = acq_entry_ptr->band_class;
                    rrc_csp_curr_select_data.first_band_to_be_scanned = acq_entry_ptr->band_class;
                    WRRC_MSG1_HIGH("Setting 1st_band and current_band to %d",
                             rrc_csp_curr_select_data.current_band);
                  }
                }
#endif /* FEATURE_WCDMA_MULTIBAND */
                MSG_MED("SPEC_PLMN rrc_csp_int_data.acq_db_empty = %d", rrc_csp_int_data.acq_db_empty,0,0);
              }
            }
            else
            {
              acq_entry_ptr = rrc_csp_get_next_match_acq_entry(req_plmn_id,
                                            rrc_csp_curr_select_data.acq_type);
            }
  
            if(acq_entry_ptr == NULL)
            {
              if(--rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles > 0) 
              {
                /* If still not done with all cycles, start a new one. 
                 * Set the acq_db in use and also reset the count of num_tries_on_dest_freq
                 */
                rrc_csp_curr_select_data.acq_db_in_use = TRUE;
                rrc_csp_curr_select_data.acq_db_scan_info.start_with_first = TRUE;
                
                /* Based on the current scan type, decide if the destination 
                 * frequency (if present) is to be tried again.
                 */
                if((   (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN) 
                    /* For RL Failure the scan is changed to OOS_Scan and hence the need
                     * to add this "if" check.
                     */
                    || ((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) 
                        && (RRC_STATE_CELL_DCH == rrc_get_state()))
                   ) 
                   &&  (rrc_csp_int_data.dest_freq_present))
                {
                  rrc_csp_curr_select_data.num_tries_on_dest_freq = 
                    RRC_CSP_MAX_ACQ_TRIES_ON_DIRECTED_FREQ;
                  rrc_csp_curr_select_data.dest_freq_in_use = TRUE;
                }
                else
                {
                  rrc_csp_curr_select_data.num_tries_on_dest_freq = 0; 
                  rrc_csp_curr_select_data.dest_freq_in_use = FALSE;
                }
                WRRC_MSG0_HIGH("Start next cycle of AcqDB scan");
              }
              else
              {
                /* Since Acq. DB is exhausted for non-barred, non-forbidden area 
                 * cell selection, start with Initial Cell Selection now
                 */
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
               if(rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
               {
#ifdef FEATURE_DUAL_SIM
                 if(!rrc_csp_int_data.oos_due_to_no_resource)
#endif
                 {
                   WRRC_MSG0_HIGH("SVC_ST: Sending UI NO SERVICE in OOSC");
                   rrc_csp_send_svc_status_to_mm(SYS_SRV_STATUS_NO_SRV, SYS_RAT_UMTS_RADIO_ACCESS, 0);
                   rrc_csp_int_data.oosc_plmn_found = FALSE;
                 }
               }
#endif
                WRRC_MSG0_HIGH("Spec PLMN: No cell in Acq DB");
                rrc_csp_curr_select_data.acq_db_in_use = FALSE;
                rrc_csp_curr_select_data.dest_freq_in_use = FALSE;
                
                /* This code is to prevent full band scan during connected mode OOS
                 * This behavior is disabled by defualt at CM level via an NV and 
                 * customer can enable the same based on his requirement
                 */
                if(
#ifdef FEATURE_WRLF_SYSTEM_SEL
                   ((rrc_csp_int_data.srvc_req_scan_scope == SYS_SCAN_SCOPE_ACQ_DB) &&
                    (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) &&
                    RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()) ||
#endif
                   (((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) ||
                     (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)) &&
                    (rrc_csp_int_data.bplmn_scan_scope == SYS_SCAN_SCOPE_ACQ_DB))
                  )
                {
                  WRRC_MSG0_HIGH("Spec PLMN: Acq DB Only Search completed");
                  all_search_exhausted = TRUE;
                }
                else if((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_IN_IDLE_STATE_SCAN) ||
                   (rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
                   #error code not present
#endif
#ifdef FEATURE_FEMTO_CSG
                   ||(rrccsp_asf_actively_in_prog())
#endif
                   )
                {
                  WRRC_MSG1_HIGH("Srch exhausted scantype %d", rrc_csp_int_data.curr_scan);
                  acq_entry_ptr = NULL;
                  all_search_exhausted = TRUE;
                }
#ifndef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
                /* After ACQ DB search, check if search should be done on GSM */
                else if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
                          && (rrcmcm_is_dualmode_enabled())
                            && (!rrc_csp_int_data.gsm_svc_srch_done)
                              && (!rrc_csp_int_data.acq_db_empty)
                                && (!rrc_csp_int_data.ue_in_deep_sleep)
                                  && (do_gsm_bands_scan_first)
#ifdef FEATURE_EOOS
                                  #error code not present
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
                                       && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
                       )
                {
                  WRRC_MSG1_HIGH("PCS/850/900 Acq Db Exhausted. Scantype %d. Srch for service on GSM", 
                                                                  rrc_csp_int_data.curr_scan);
                  *ret_type_freq_scan  = RRC_CSP_GSM_SCAN;
                  acq_entry_ptr        = NULL;
                  all_search_exhausted = TRUE;
                }
#endif
#endif
                else
                {
#ifdef FEATURE_WCDMA_MULTIBAND
                  /* When UE is in deep sleep and full scan timer is running then do full scan for 
                   * only those bands whose frequencies are not present in ACQ DB.
                   */
                  if((rrc_csp_int_data.ue_in_deep_sleep)  &&
                     (!rrc_csp_int_data.full_scan_needed) &&
                     (!rrc_csp_int_data.acq_db_empty)     &&
                     (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN)            &&
                     (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) &&
                     (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)    &&
                     ((rrc_csp_int_data.deep_sleep_acq_db_band_mask ^ rrc_csp_int_data.wcdma_band_mask) != 0) &&
                     (((rrc_csp_int_data.deep_sleep_band_scanned_mask & rrc_csp_int_data.wcdma_band_mask)
                            ^ rrc_csp_int_data.wcdma_band_mask) == 0)
#ifdef FEATURE_EOOS
                     #error code not present
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
                     && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
                     && (rrc_csp_int_data.curr_scan != RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
                    )
                  {
                    rrc_csp_curr_select_data.next_band_mask = 
                             rrc_csp_int_data.deep_sleep_acq_db_band_mask ^ rrc_csp_int_data.wcdma_band_mask;
                    
                    rrc_csp_curr_select_data.first_band_to_be_scanned = 0;

                    /* Remove ACQ-DB bands from the prioritized band list. */
                    if(rrc_csp_curr_select_data.num_of_prioritized_bands != 0)
                    {
                      rrccsp_remove_acqdb_bands_from_prioritized_list();
                      rrc_csp_curr_select_data.prioritized_bands_read_idx = 0;
                    }
                    rrc_csp_curr_select_data.current_band = rrccsp_get_next_band_to_be_scanned();

                    MSG_HIGH("acq_db_mask = %d, next_mask = %d, flag_mask = %d",
                                                        rrc_csp_int_data.deep_sleep_acq_db_band_mask,
                                                        rrc_csp_curr_select_data.next_band_mask,
                                                        rrc_csp_int_data.deep_sleep_band_scanned_mask);
                  } 
#endif
                  /* Go for full freq scan for other scan types */
                  WRRC_MSG0_HIGH("Start: Specific PLMN Freq scan");
                  rrc_csp_init_freq_scan_data();
                  rrc_csp_init_addl_chnl_scan_data();
                }
              } /* Acq DB exhausted - all cycles */
            } /* acq entry ptr is NULL */
#ifdef FEATURE_WCDMA_MULTIBAND
            else
            {
              rrc_csp_curr_select_data.current_band = acq_entry_ptr->band_class;
            }
#endif /* FEATURE_WCDMA_MULTIBAND */
          } /* Acq DB in use */

#ifdef FEATURE_EOOS
          #error code not present
#endif /* FEATURE_EOOS */

          else /* Acq DB is not in use */
          {
            /* Only if the frequency scan timer has expired do we want to do a full
             * frequency scan. Otherwise return NULL.
             */
            /* Get the first or next entry from frequency band (Initial 
             *  Selection) 
             */
            if(rrc_csp_get_next_freq_scan_entry(&ret_freq) == FAILURE)
            {
              /* Since all frequencies have been exhausted for a non-barred,
               * non-forbidden reg. area cell search, we have exhausted ALL 
               * possibilities. Lets return a NULL to calling function so that 
               * it takes necessary action (Like going to Deep Sleep ?)
               */
#ifdef FEATURE_WCDMA_MULTIBAND
              WRRC_MSG1_HIGH("Spec PLMN: No cell in Freq Band %d", 
                         rrc_csp_curr_select_data.current_band);
              rrc_csp_curr_select_data.scanned_band_mask |= rrc_csp_curr_select_data.current_band;
              WRRC_MSG1_HIGH("Scanned_band_mask = %d",rrc_csp_curr_select_data.scanned_band_mask);
              /* Scan the next band if supported */
              if((rrc_csp_curr_select_data.current_band = 
                    rrccsp_get_next_band_to_be_scanned()) != RRC_CSP_BAND_MAX)
              {
                all_search_exhausted = FALSE;
                WRRC_MSG1_HIGH("Start: Any PLMN freq scan on band %d", 
                         rrc_csp_curr_select_data.current_band);
                rrc_csp_init_freq_scan_data();


                #ifdef FEATURE_EOOS
                #error code not present
#endif
              }
              else
              {
                WRRC_MSG0_HIGH("All bands exhausted");
                all_search_exhausted = TRUE;
                acq_entry_ptr = NULL;
              }
#else
              WRRC_MSG0_HIGH("Spec PLMN: No cell in Freq Band");
              acq_entry_ptr = NULL;
              all_search_exhausted = TRUE;
#endif /* FEATURE_WCDMA_MULTIBAND */
            }
            else
            {
              //EVENT: EVENT_WCDMA_RRCCSP_SCAN_START
              if(rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_RAW_SCAN)
              {
                rrccsp_scan_start_evt.scan_type = FULL_FREQ_SCAN;
                event_report_payload(EVENT_WCDMA_RRCCSP_SCAN_START, sizeof(rrclog_wcdma_rrccsp_scan_start_event_type),
                               (void *)&rrccsp_scan_start_evt);
              }
              /* If UE is doing Acquisition scan, check if the particular frequency has already
               * been scanned as part of the initial acquisition database scan
               * If yes, then no need to scan that frequency again
               */
              while((rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_ACQ_SCAN) &&
                    (rrc_csp_should_freq_be_scanned(rrc_csp_int_data.curr_req_plmn, ret_freq) == FALSE))
              {
                WRRC_MSG1_HIGH("Freq %d already scanned",ret_freq);
                if(rrc_csp_get_next_freq_scan_entry(&ret_freq) == FAILURE)
                {
                  WRRC_MSG0_HIGH("Spec PLMN: No cell in Freq Band");
                  acq_entry_ptr = NULL;
                  all_search_exhausted = TRUE;
                  break;
                }
              }

              if(all_search_exhausted == FALSE)
              {
                acq_entry_ptr = &tmp_acq_entry;
                acq_entry_ptr->freq = ret_freq;
                WRRC_MSG2_HIGH("Freq %d scan type %d", ret_freq,
                         rrc_csp_curr_select_data.scan_info.type_freq_scan);
              }
#ifdef FEATURE_WCDMA_MULTIBAND
              else
              {
                rrc_csp_curr_select_data.scanned_band_mask |= rrc_csp_curr_select_data.current_band;
                WRRC_MSG1_HIGH("Scanned_band_mask = %d",rrc_csp_curr_select_data.scanned_band_mask);
                /* Scan the next band if supported */
                if((rrc_csp_curr_select_data.current_band = 
                      rrccsp_get_next_band_to_be_scanned()) != RRC_CSP_BAND_MAX)
                {
                  all_search_exhausted = FALSE;
                  WRRC_MSG1_HIGH("Start: Any PLMN freq scan on band %d", 
                           rrc_csp_curr_select_data.current_band);
                  rrc_csp_init_freq_scan_data();

                  #ifdef FEATURE_EOOS
                  #error code not present
#endif

                }
                else
                {
                  WRRC_MSG0_HIGH("All bands exhausted");
                  all_search_exhausted = TRUE;
                  acq_entry_ptr = NULL;
                }
              }
#endif /* FEATURE_WCDMA_MULTIBAND */
            }
          } /* Acq DB is not in use */
          break;
             
        case RRC_CSP_ANY_PLMN:
          /* Check if Acq. database or complete frequency band is being used */
          if(rrc_csp_curr_select_data.acq_db_in_use == TRUE)
          {
            if(rrc_csp_curr_select_data.acq_db_scan_info.start_with_first)
            {
              MSG_LOW("Start: Any PLMN srch ACQ DB", 0, 0, 0);
              rrc_csp_curr_select_data.acq_db_scan_info.start_with_first = FALSE;
              rrc_csp_curr_select_data.num_available_freq = 0;

              acq_entry_ptr = rrc_csp_get_first_match_acq_entry(req_plmn_id,
                                            rrc_csp_curr_select_data.acq_type);
              if(acq_entry_ptr == NULL)
              { 
                MSG_MED("ANY PLMN rrc_csp_int_data.acq_db_empty = %d", rrc_csp_int_data.acq_db_empty,0,0);
#ifdef FEATURE_WCDMA_MULTIBAND
                rrc_csp_curr_select_data.current_band = 
                  rrc_csp_curr_select_data.first_band_to_be_scanned;
#endif 
              }
              else
              {
                //EVENT: EVENT_WCDMA_RRCCSP_SCAN_START
                rrccsp_scan_start_evt.scan_type = ACQ_DB_SCAN;
                event_report_payload(EVENT_WCDMA_RRCCSP_SCAN_START, sizeof(rrclog_wcdma_rrccsp_scan_start_event_type),
                               (void *)&rrccsp_scan_start_evt);

#ifdef FEATURE_WCDMA_MULTIBAND
                /* If Automatic BPLMN search is in progress then determine if 
                 * current_band needs to be set or not based on the value of 
                 * bplmn_current_band_initialized. The value stored in current_band
                 * is used to determine which band needs to be scanned first
                 * during full freq scan.
                 * The check below is to avoid updating current_band multiple times
                 * as it may change the order in which full freq scan is done
                 * on supported bands.
                 */
                if((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))
                {
                  if(!rrc_csp_int_data.bplmn_current_band_initialized)
                  {
                    rrc_csp_curr_select_data.current_band = acq_entry_ptr->band_class;
                    rrc_csp_curr_select_data.first_band_to_be_scanned = acq_entry_ptr->band_class;
                    WRRC_MSG1_HIGH("Setting 1st_band and current_band to %d",
                             rrc_csp_curr_select_data.current_band);
                    rrc_csp_int_data.bplmn_current_band_initialized = TRUE;
                  }
                }
                /* Initialize current_band and first_band_to_be_scanned */
                else
                {
                  if(rrc_csp_curr_select_data.num_of_prioritized_bands != 0)
                  {
                    rrc_csp_curr_select_data.current_band = rrc_csp_curr_select_data.first_band_to_be_scanned;
                    WRRC_MSG1_HIGH("Setting 1st_band and current_band to %d",
                             rrc_csp_curr_select_data.current_band);
                  }
                  else
                  {
                    rrc_csp_curr_select_data.current_band = acq_entry_ptr->band_class;
                    rrc_csp_curr_select_data.first_band_to_be_scanned = acq_entry_ptr->band_class;
                    WRRC_MSG1_HIGH("Setting 1st_band and current_band to %d",
                             rrc_csp_curr_select_data.current_band);
                  }
                }
#endif /* FEATURE_WCDMA_MULTIBAND */
                MSG_MED("ANY PLMN rrc_csp_int_data.acq_db_empty = %d", rrc_csp_int_data.acq_db_empty,0,0);
              }
            }
            else
            {
              acq_entry_ptr = rrc_csp_get_next_match_acq_entry(req_plmn_id,
                                            rrc_csp_curr_select_data.acq_type);
            }
  
            if(acq_entry_ptr == NULL)
            {
              if(--rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles > 0) 
              {
                /* If still not done with all cycles, start a new one. 
                 * Set the acq_db in use and also reset the count of num_tries_on_dest_freq
                 */
                rrc_csp_curr_select_data.acq_db_in_use = TRUE;
                rrc_csp_curr_select_data.acq_db_scan_info.start_with_first = TRUE;
                
                /* Based on the current scan type, decide if the destination 
                 * frequency (if present) is to be tried again.
                 */
                if((   (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN) 
                    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN) 
                    /* For RL Failure the scan is changed to OOS_Scan and hence the need
                     * to add this "if" check.
                     */
                    || ((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) 
                        && (RRC_STATE_CELL_DCH == rrc_get_state()))
                   )
                   &&  (rrc_csp_int_data.dest_freq_present))
                {
                  rrc_csp_curr_select_data.num_tries_on_dest_freq = 
                    RRC_CSP_MAX_ACQ_TRIES_ON_DIRECTED_FREQ;
                  rrc_csp_curr_select_data.dest_freq_in_use = TRUE;
                }
                else
                {
                  rrc_csp_curr_select_data.num_tries_on_dest_freq = 0;
                  rrc_csp_curr_select_data.dest_freq_in_use = FALSE;
                }
                WRRC_MSG0_HIGH("Start next cycle of AcqDB scan");
              }
              else
              {
                /* Since Acq. DB is exhausted for non-barred, non-forbidden area 
                 * cell selection, start with Initial Cell Selection now
                 */
                WRRC_MSG0_HIGH("Any PLMN: No cell in Acq DB");
                rrc_csp_curr_select_data.acq_db_in_use = FALSE;
                rrc_csp_curr_select_data.dest_freq_in_use = FALSE;
#ifdef FEATURE_WRLF_SYSTEM_SEL
                if((rrc_csp_int_data.srvc_req_scan_scope == SYS_SCAN_SCOPE_ACQ_DB) &&
                    (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) &&
                    (RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()))
                {
                   WRRC_MSG0_HIGH("Any PLMN: Acq DB Only Search completed");
                   all_search_exhausted = TRUE;
                }
                else 
#endif
                if((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_IN_IDLE_STATE_SCAN) ||
                   (rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
                   #error code not present
#endif
                   )
                {
                  WRRC_MSG1_HIGH("Srch exhausted scantype %d", rrc_csp_int_data.curr_scan);
                  acq_entry_ptr = NULL;
                  all_search_exhausted = TRUE;
                }
#ifndef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
                /* After ACQ DB search, check if search should be done on GSM */
                else if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
                          && (rrcmcm_is_dualmode_enabled())
                            && (!rrc_csp_int_data.gsm_svc_srch_done)
                              && (!rrc_csp_int_data.acq_db_empty)
                                && (!rrc_csp_int_data.ue_in_deep_sleep)
                                  && (do_gsm_bands_scan_first)
#ifdef FEATURE_EOOS
                                    #error code not present
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
                                       && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
                       )
                {
                  WRRC_MSG1_HIGH("ANY_PLMN PCS/850/900 Acq Db Exhausted. Scantype %d. Srch for service on GSM", 
                                                                  rrc_csp_int_data.curr_scan);
                  *ret_type_freq_scan  = RRC_CSP_GSM_SCAN;
                  acq_entry_ptr        = NULL;
                  all_search_exhausted = TRUE;
                }
#endif
#endif
                else
                {
#ifdef FEATURE_WCDMA_MULTIBAND
                  /* When UE is in deep sleep and full scan timer is running then do full scan 
                   * for only those bands whose frequencies are not present in ACQ DB
                   */
                  if((rrc_csp_int_data.ue_in_deep_sleep)  &&
                     (!rrc_csp_int_data.full_scan_needed) &&
                     (!rrc_csp_int_data.acq_db_empty)     &&
                     (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN)            &&
                     (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) &&
                     (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)    &&
                     ((rrc_csp_int_data.deep_sleep_acq_db_band_mask ^ rrc_csp_int_data.wcdma_band_mask) != 0) &&
                     (((rrc_csp_int_data.deep_sleep_band_scanned_mask & rrc_csp_int_data.wcdma_band_mask)
                            ^ rrc_csp_int_data.wcdma_band_mask) == 0)

#ifdef FEATURE_EOOS
                     #error code not present
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
                     && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
                     && (rrc_csp_int_data.curr_scan != RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
                    )
                  {
                    rrc_csp_curr_select_data.next_band_mask = 
                             rrc_csp_int_data.deep_sleep_acq_db_band_mask ^ rrc_csp_int_data.wcdma_band_mask;
                    
                    rrc_csp_curr_select_data.first_band_to_be_scanned = 0;

                    /* Remove ACQ-DB bands from the prioritized band list. */
                    if(rrc_csp_curr_select_data.num_of_prioritized_bands != 0)
                    {
                      rrccsp_remove_acqdb_bands_from_prioritized_list();
                      rrc_csp_curr_select_data.prioritized_bands_read_idx = 0;
                    }
                    rrc_csp_curr_select_data.current_band = rrccsp_get_next_band_to_be_scanned();
                    
                    MSG_HIGH("acq_db_mask = %d, next_mask = %d, flag_mask = %d",
                                                        rrc_csp_int_data.deep_sleep_acq_db_band_mask,
                                                        rrc_csp_curr_select_data.next_band_mask,
                                                        rrc_csp_int_data.deep_sleep_band_scanned_mask);
                  } 
#endif
                  WRRC_MSG0_HIGH("Start: Any PLMN freq scan");
                  rrc_csp_init_freq_scan_data();
                  rrc_csp_init_addl_chnl_scan_data();
                }
              }  /* All cycles of Acq DB exhausted */
            } /* Acq entry ptr is NULL */
#ifdef FEATURE_WCDMA_MULTIBAND
            else
            {
              rrc_csp_curr_select_data.current_band = acq_entry_ptr->band_class;
            }
#endif
          }  /* Acq DB is in use */

#ifdef FEATURE_EOOS
          #error code not present
#endif /* FEATURE_EOOS */

          else /* Acq DB is not in use */
          {
            /* Only if the frequency scan timer has expired do we want to do a full
             * frequency scan. Otherwise return NULL.
             */
            /* Get the first or next entry from frequency band (Initial 
             *  Selection) 
             */
            if(rrc_csp_get_next_freq_scan_entry(&ret_freq) == FAILURE)
            {
              /* Since all frequencies have been exhausted for a non-barred,
               * non-forbidden reg. area cell search, we have exhausted ALL 
               * possibilities. Lets return a NULL to calling function so that 
               * it takes necessary action (Like going to Deep Sleep ?)
               */
#ifdef FEATURE_WCDMA_MULTIBAND
              WRRC_MSG1_HIGH("Any PLMN: No cell in Freq Band %d", 
                         rrc_csp_curr_select_data.current_band);
              rrc_csp_curr_select_data.scanned_band_mask |= rrc_csp_curr_select_data.current_band;
              WRRC_MSG1_HIGH("Scanned_band_mask = %d",rrc_csp_curr_select_data.scanned_band_mask);

              /* Scan the next band if supported */
              if((rrc_csp_curr_select_data.current_band = 
                    rrccsp_get_next_band_to_be_scanned()) != RRC_CSP_BAND_MAX)
              {
                all_search_exhausted = FALSE;
                WRRC_MSG1_HIGH("Start: Any PLMN freq scan on band %d", 
                         rrc_csp_curr_select_data.current_band);
                rrc_csp_init_freq_scan_data();


                #ifdef FEATURE_EOOS
                #error code not present
#endif
              }
              else
              {
                WRRC_MSG0_HIGH("All bands exhausted");
                all_search_exhausted = TRUE;
                acq_entry_ptr = NULL;
              }
#else
              WRRC_MSG0_HIGH("Any PLMN: No cell in Freq Band");
              all_search_exhausted = TRUE;
              acq_entry_ptr = NULL;
#endif /* FEATURE_WCDMA_MULTIBAND */
            }
            else
            {
              //EVENT: EVENT_WCDMA_RRCCSP_SCAN_START
              if(rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_RAW_SCAN)
              {
                rrccsp_scan_start_evt.scan_type = FULL_FREQ_SCAN;
                event_report_payload(EVENT_WCDMA_RRCCSP_SCAN_START, sizeof(rrclog_wcdma_rrccsp_scan_start_event_type),
                               (void *)&rrccsp_scan_start_evt);
              }
              /* If UE is doing Acquisition scan, check if the particular frequency has already
               * been scanned as part of the initial acquisition database scan
               * If yes, then no need to scan that frequency again
               */
              while((rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_ACQ_SCAN) &&
                    (rrc_csp_should_freq_be_scanned(rrc_csp_int_data.curr_req_plmn, ret_freq) == FALSE))
              {
                WRRC_MSG1_HIGH("Freq %d already scanned (Acq DB scan)",ret_freq);
                if(rrc_csp_get_next_freq_scan_entry(&ret_freq) == FAILURE)
                {
                  WRRC_MSG0_HIGH("Any PLMN: No cell in Freq Band");
                  acq_entry_ptr = NULL;
                  all_search_exhausted = TRUE;
                  break;
                }
              }

              if(all_search_exhausted == FALSE)
              {
                acq_entry_ptr = &tmp_acq_entry;
                acq_entry_ptr->freq = ret_freq;
                WRRC_MSG2_HIGH("Freq %d scan type %d", ret_freq,
                         rrc_csp_curr_select_data.scan_info.type_freq_scan);
              }
#ifdef FEATURE_WCDMA_MULTIBAND
              else
              {
                rrc_csp_curr_select_data.scanned_band_mask |= rrc_csp_curr_select_data.current_band;
                WRRC_MSG1_HIGH("Scanned_band_mask = %d",rrc_csp_curr_select_data.scanned_band_mask);
                /* Scan the next band if supported */
                if((rrc_csp_curr_select_data.current_band = 
                      rrccsp_get_next_band_to_be_scanned()) != RRC_CSP_BAND_MAX)
                {
                  all_search_exhausted = FALSE;
                  WRRC_MSG1_HIGH("Start: Any PLMN freq scan on band %d", 
                           rrc_csp_curr_select_data.current_band);
                  rrc_csp_init_freq_scan_data();


#ifdef FEATURE_EOOS
                  #error code not present
#endif

                }
                else
                {
                  WRRC_MSG0_HIGH("All bands exhausted");
                }
              }
#endif /* FEATURE_WCDMA_MULTIBAND */
            }
          } /* Acq DB is not in use */
  
          break;
      
        default:
          WRRC_MSG1_ERROR("ERR: Acq type %d not supported, Invalid",
                                      rrc_csp_curr_select_data.acq_type);
          return(FAILURE);
  
      } /* switch(rrc_csp_curr_select_data.acq_type) */
    }
    if(acq_entry_ptr  == NULL)
    {
      /* We exhausted the last acq. type search in either Stored Acq. DataBase 
       * (Stored Cell Selection) or in the frequency band (Initial Cell Selection)
       * Mark Start with First so that next search should start with the first 
       * entry (either in Acq. database or the frequency band)
       */
      rrc_csp_curr_select_data.acq_db_scan_info.start_with_first = TRUE;

      
      /* If BPLMN search is in progress then set/reset acquisition database 
       * access variables appropriately.
       */
      if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
      {
        if((rrc_csp_int_data.bplmn_req_list_rd_index + 1) < rrc_csp_int_data.bplmn_num_req)
        {
          rrc_csp_int_data.curr_req_plmn.plmn_id = 
                 rrc_csp_int_data.bplmn_req_list[++rrc_csp_int_data.bplmn_req_list_rd_index].plmn_id;

          req_plmn_id = rrc_csp_curr_select_data.req_plmn.plmn_id = rrc_csp_int_data.curr_req_plmn.plmn_id;

          WRRC_MSG2_HIGH("BPLMN/WTOW:Srch nxt PLMN in acqDB;bplmn_rd_ix=%d;num_PLMNs_req=%d",
                      rrc_csp_int_data.bplmn_req_list_rd_index,rrc_csp_int_data.bplmn_num_req);

          rrc_csp_curr_select_data.acq_db_in_use                     = TRUE;
          rrc_csp_curr_select_data.acq_db_scan_info.start_with_first = TRUE;
          all_search_exhausted                                       = FALSE;
          rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles   = 1;
        }   
      } /* end RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN */
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
      {
        if((rrc_csp_int_data.bplmn_req_list_rd_index + 1) < rrc_csp_int_data.wtow_bplmn_num_req)
        {
          rrc_csp_int_data.curr_req_plmn.plmn_id = 
               rrc_csp_int_data.wtow_bplmn_req_list[++rrc_csp_int_data.bplmn_req_list_rd_index].plmn_id;

          req_plmn_id = rrc_csp_curr_select_data.req_plmn.plmn_id = rrc_csp_int_data.curr_req_plmn.plmn_id;

          WRRC_MSG2_HIGH("BPLMN/WTOW:Srch nxt PLMN in acqDB;bplmn_rd_ix=%d;num W PLMNs req=%d",
                      rrc_csp_int_data.bplmn_req_list_rd_index,rrc_csp_int_data.wtow_bplmn_num_req);

          rrc_csp_curr_select_data.acq_db_in_use                     = TRUE;
          rrc_csp_curr_select_data.acq_db_scan_info.start_with_first = TRUE;
          all_search_exhausted                                       = FALSE;
          rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles   = 1;
        }   
      }/* end RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN */
    }/* end acq_entry_pt */

  }while((acq_entry_ptr == NULL) && (all_search_exhausted == FALSE));

  if(all_search_exhausted == FALSE)
  {
    if(acq_entry_ptr != NULL)
    {
      *ret_acq_entry_ptr = *acq_entry_ptr;
    }
    else
    {
      WRRC_MSG0_ERROR("ERR: ACQ Ptr should not be NULL");
    }

#ifdef FEATURE_EOOS
    #error code not present
#endif

#ifdef FEATURE_EOOS
    #error code not present
#endif

#ifdef FEATURE_EOOS
    #error code not present
#endif

    if((rrc_csp_curr_select_data.acq_db_in_use) ||
       (rrc_csp_curr_select_data.dest_freq_in_use))
    {
      WRRC_MSG2_HIGH("LFS: Returning type_freq_scan as RRC_CSP_ACQ_SCAN. acq_db_in_use %d, dest_freq_in_use %d",
                rrc_csp_curr_select_data.acq_db_in_use, rrc_csp_curr_select_data.dest_freq_in_use);

      /* If Acquisition Database is in use, the next L1 command has to be an ACQ Req */
      *ret_type_freq_scan =  RRC_CSP_ACQ_SCAN;
    }
    else
    {
      /* The next L1 cmd depends on the current frequency scan type */
      *ret_type_freq_scan =  rrc_csp_curr_select_data.scan_info.type_freq_scan;
    }

  #ifdef FEATURE_WCDMA_MULTIBAND
    /* If UE is in deep sleep and acquisition database is not in use then set 
     * the current_band as scanned
     */
    if(rrc_csp_int_data.ue_in_deep_sleep && !rrc_csp_curr_select_data.acq_db_in_use)
    { 
      rrc_csp_int_data.deep_sleep_band_scanned_mask |= rrc_csp_curr_select_data.current_band;
      WRRC_MSG2_HIGH("deep_sleep_band_scanned_mask = %d and current_band = %d", 
                                            rrc_csp_int_data.deep_sleep_band_scanned_mask,
                                            rrc_csp_curr_select_data.current_band);
    }
  #endif /* FEATURE_WCDMA_MULTIBAND */

    return(SUCCESS);

  } /* End If All Search was NOT exhausted */
  /* Search Exhausted */
  else
  {
    if(((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) ||
        (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)) &&
       (rrc_csp_int_data.bplmn_scan_scope == SYS_SCAN_SCOPE_ACQ_DB))
    {
      WRRC_MSG0_HIGH("BPLMN Search_Type ACQ_DB: Deep Sleep Full Scan Counters/Timers are not impacted");
    }
    else
    {
    /* Increment counter for service search if UE is in deep sleep */
    if((rrc_csp_int_data.ue_in_deep_sleep) &&
       (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN) &&
       (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) &&
       (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN) && 
       (rrc_csp_int_data.full_scan_in_deep_sleep_counter < RRC_MAX_FULL_SCANS_IN_DEEP_SLEEP)
#ifdef FEATURE_EOOS
       #error code not present
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
       && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
       && (rrc_csp_int_data.curr_scan != RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
       )

    {
      rrc_csp_int_data.full_scan_in_deep_sleep_counter ++;

      WRRC_MSG1_HIGH("Deep Sleep full scan counter = %d",
                                      rrc_csp_int_data.full_scan_in_deep_sleep_counter);
    }

    if((rrc_csp_int_data.ue_in_deep_sleep) &&
       (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN) &&
       (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) &&
       (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN) &&
       (rrc_csp_int_data.full_scan_in_deep_sleep_counter >= RRC_MAX_FULL_SCANS_IN_DEEP_SLEEP) &&
       (rrc_csp_int_data.full_scan_needed)
#ifdef FEATURE_EOOS
       #error code not present
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
       && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
       && (rrc_csp_int_data.curr_scan != RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
      )
    {
      /* Start the Frequency Scan Timer and reset the do_freq_scan flag */
      rrctmr_start_timer( RRCTMR_FREQ_SCAN_TIMER, 
                          (unsigned long) wcdma_rrc_freq_scan_timer_nv);
      rrc_csp_int_data.full_scan_needed = FALSE;
      WRRC_MSG0_HIGH("Full scan timer started. UE should NOT do full scan on next wake up for the same band group");
    }
    }

    /* Search did not yield anything and was Exhausted. */
    return(FAILURE);
  }
} /* rrc_csp_continue_stored_initial_select_ctrl */
/*lint +e715*/


/*===========================================================================

FUNCTION          RRC_CSP_INIT_LFS_SCAN

DESCRIPTION       This function initiate datafor starting LFS scan.

DEPENDENCIES
                  None

RETURN VALUE

SIDE EFFECTS
                  None
===========================================================================*/
static void rrc_csp_init_lfs_scan(void)
{
  /* Invalidate the scanned indices- defined for Flexible DS*/
  rrc_csp_curr_select_data.scan_info.list_scan.curr_freq_index = -1;

  rrc_csp_curr_select_data.scan_info.list_scan.num_freq = 0;

#ifdef FEATURE_EOOS
  #error code not present
#endif
  /* Initialize the flexible deep sleep related variables if the UE is in deep sleep and also in case of power up */
  if(((rrc_csp_int_data.ue_in_deep_sleep == TRUE) || 
      (rrc_csp_int_data.do_lfs_scan == TRUE))
#ifdef FEATURE_WRLF_SYSTEM_SEL
      && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
    )
  {
    rrc_csp_curr_select_data.acq_db_in_use = FALSE;
    /* Reset available frequency list for each search cycle */
    rrc_csp_curr_select_data.num_available_freq = 0;

    /* Update all the matching entries LFS table from ACQ DB */
    WRRC_MSG0_HIGH("LFS_COOS: Traverse through ACQ DB and start populating list DB");
    rrccsp_update_match_acq_entries_into_list_db();
    
    /*List Scan DB is not empty, this means ACQ DB search has resulted 
      few successful entries into List Scan Database
    */
    if(rrc_csp_curr_select_data.scan_info.list_scan.num_freq == 0)
    {
      WRRC_MSG0_HIGH("LFS_COOS: No of freqs returned from ACQ DB are 0 during LFS");
    }
  }
}


/*===========================================================================

FUNCTION          RRC_CSP_INIT_SELECT_DATA

DESCRIPTION       This function is responsible for initializing the internal
                  Search data structure to default values based on the passed
                  PLMN t type.

DEPENDENCIES
                  None

RETURN VALUE
                  SUCCESS: Success initializing Select data based on passed
                           PLMN type.
                  FAILURE: Could not initialize Select data for the passed
                           PLMN type.

SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type
rrc_csp_init_select_data(rrc_requested_plmn_type  *req_plmn_ptr)
{

  if(rrc_csp_int_data.bplmn_new_srch)
  {
    if((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN))
    {
      rrc_csp_int_data.bplmn_new_srch = FALSE;
    }
    
#ifdef FEATURE_DUAL_SIM
      rrcwrm_reset_l2_ack_req_mask();
#endif

    if((req_plmn_ptr->plmn_type != RRC_GSM_MAP_SPECIFIC_PLMN_REQ) &&
       (req_plmn_ptr->plmn_type != RRC_GSM_MAP_ANY_PLMN_REQ))
    {
      WRRC_MSG1_ERROR("ERR: Unexpected PLMN type %d", req_plmn_ptr->plmn_type);
      return (FAILURE);
    }

    rrc_csp_curr_select_data.req_plmn = *req_plmn_ptr;
  
    /* 1. Initialize count of AcqDB + DestFReq cycles based on Current Scan */
    /* Set the number of Acq DB + Dest freq cycles to its max value */
  
    if(   (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) 
       || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) 
       || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN)
       || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN)
       || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN)
       || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN)
       /* For RL Failure the scan is changed to OOS_Scan and hence the need
        * to add this "if" check.
        */
       || (   (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) 
           && (RRC_STATE_CELL_DCH == rrc_get_state())
           && (rrc_csp_int_data.dch_tx_dest_state == RRC_STATE_CELL_FACH)
           && (!rrccsp_is_state_transition_triggered_by_reconfig_msg())
          )
      )
    {
      rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles = RRC_CSP_MAX_NUM_ACQDB_DEST_FREQ_CYCLES;
     }
    else
    {
      rrc_csp_curr_select_data.num_acq_db_and_dest_freq_cycles = 1;
    }

    /* 2. Acq DB Initialization */
    /* Start with looking in the Acq. DB */
    rrc_csp_curr_select_data.acq_db_in_use = TRUE;

#ifdef FEATURE_EOOS
    #error code not present
#endif /* FEATURE_EOOS */

#ifdef FEATURE_EOOS
    #error code not present
#endif

    /* Set "Start with First" to TRUE so that the cell search begins with the
     * first entry of Acq. DB or first frequency in Freq Scan.
     */
    rrc_csp_curr_select_data.acq_db_scan_info.start_with_first    = TRUE;

#ifdef FEATURE_EOOS
    #error code not present
#endif

#ifdef FEATURE_WCDMA_MULTIBAND
      /* Reset the next_band_mask appropriately */
      WRRC_MSG0_HIGH("Resetting the next_band_mask");
      rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;

      WRRC_MSG0_HIGH("Resetting the variables for optimized band scan");
      rrc_csp_curr_select_data.scanned_band_mask = 0;
      rrc_csp_curr_select_data.dont_skip_band_mask = 0;
      rrc_csp_curr_select_data.num_of_barred_freq = 0;
      rrc_csp_curr_select_data.add_barr_freq_to_fine_res = FALSE;
      rrc_csp_int_data.barred_due_to_band_mismatch = FALSE;

#endif
      rrc_csp_int_data.primary_plmn_Id_present = FALSE;

     WRRC_MSG1_HIGH("WCDMA Bandclass bitmask : %d",rrc_csp_int_data.wcdma_band_mask);

      rrccsp_prioritize_wcdma_bands_for_full_scan();

      if(rrc_csp_curr_select_data.prioritized_bands_read_idx < rrc_csp_curr_select_data.num_of_prioritized_bands)
      {
        rrc_csp_curr_select_data.first_band_to_be_scanned = rrc_csp_curr_select_data.prioritized_bands[rrc_csp_curr_select_data.prioritized_bands_read_idx];
        MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"WBP:prioritized_bands_read_idx: %d, num_of_prioritized_bands: %d, First Band to be scanned: %d, next_band_mask : %d",
                                             rrc_csp_curr_select_data.prioritized_bands_read_idx,
                                             rrc_csp_curr_select_data.num_of_prioritized_bands,
                                             rrc_csp_curr_select_data.prioritized_bands[rrc_csp_curr_select_data.prioritized_bands_read_idx],
                                             rrc_csp_curr_select_data.next_band_mask);
        rrc_csp_curr_select_data.prioritized_bands_read_idx++;
      }

      /* 3. Freq Scan Initialization */
      /* Initialize the frequency scan data */
      rrc_csp_init_freq_scan_data();

      rrc_csp_init_addl_chnl_scan_data();

      WRRC_MSG0_HIGH("Resetting the addl_chnl_done_mask");
      rrc_csp_curr_select_data.addl_chnl_done_mask = 0;

#ifdef FEATURE_EOOS
      #error code not present
#endif

    /* 4. Number of tries on destination frequency initialization 
     *    based on Current Scan
     */
    if((rrc_csp_int_data.curr_scan != RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) &&
       (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) &&
       (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN)
        && (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN)
        && (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN)
        && (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN)
       #ifdef FEATURE_LTE_TO_WCDMA
        && (!((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) &&
                  (rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE)))
       #endif
      )
    {
      /* There is no destination frequency at this point */
      rrc_csp_curr_select_data.num_tries_on_dest_freq = 0;
      rrc_csp_curr_select_data.dest_freq_in_use = FALSE;
    }
    else
    {
      /* There is a destination frequency at this point */
      if(rrc_csp_int_data.dest_freq_present)
      {
#ifdef FEATURE_LTE_TO_WCDMA
        /* For LTOW redirection set max ACQ attempt counter*/
        if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) &&
           (rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE))
        {
          rrc_csp_curr_select_data.num_tries_on_dest_freq = 
            RRCCSP_LTOW_MAX_ACQ_TRIES_ON_DIRECTED_FREQ;
          WRRC_MSG1_HIGH("LTOW: Setting max ACQ attempt to %d",RRCCSP_LTOW_MAX_ACQ_TRIES_ON_DIRECTED_FREQ);
        }
        else
#endif
        {
          rrc_csp_curr_select_data.num_tries_on_dest_freq = 
          RRC_CSP_MAX_ACQ_TRIES_ON_DIRECTED_FREQ;
        }
        rrc_csp_curr_select_data.dest_freq_in_use = TRUE;
      }
      else
      {
        rrc_csp_curr_select_data.num_tries_on_dest_freq = 0;
        rrc_csp_curr_select_data.dest_freq_in_use = FALSE;
      }
    }
  
    /* 5. Acquisition type initialization based on Requested PLMN type */
    if(req_plmn_ptr->plmn_type == RRC_GSM_MAP_SPECIFIC_PLMN_REQ)
    {
      /* Look for a non-barred, non-forbidden area cell of the 
       * specific PLMN requested 
       */
      rrc_csp_curr_select_data.acq_type = RRC_CSP_SPEC_PLMN;
    }
    else 
    {
      rrc_csp_curr_select_data.acq_type = RRC_CSP_ANY_PLMN;
    }
    WRRC_MSG1_HIGH("Acquisition Type: %d",rrc_csp_curr_select_data.acq_type);

    if(rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
    {
      /*copy the target list to list_db*/
      (void)rrc_csp_lfs_update_list_db_with_target_fdd_list();
    }
    else
    {
      rrc_csp_init_lfs_scan();
    }


  }
  else
  {
    rrc_csp_curr_select_data.req_plmn = *req_plmn_ptr;
  }   

  return(SUCCESS);

} /* rrc_csp_init_select_data */

/*===========================================================================

FUNCTION          rrc_csp_try_cell_acq_on_nv_given_freq

DESCRIPTION       This function will validate the NV freq.
DEPENDENCIES
                  None

RETURN VALUE
SIDE EFFECTS
                  None
===========================================================================*/

boolean rrc_csp_try_cell_acq_on_nv_given_freq
(
    rrc_freq_type wcdma_dl_freq,
    rrc_csp_acq_e_type  *ret_type_acq_ptr,
    rrc_csp_acq_entry_type  *ret_acq_entry_ptr,
    rrc_csp_freq_scan_e_type *ret_type_freq_scan
)
{
  WRRC_MSG1_HIGH("ACQ for NV Freq %d", wcdma_dl_freq);

#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
  if(SUCCESS == validate_dl_arfcn(wcdma_dl_freq, 
                                  rrc_csp_curr_select_data.req_plmn.plmn_id,
                                  W_UARFCN_NOT_FROM_GSM_NBR_LIST
                                  , FALSE
                                 )
    )
#else
  if(SUCCESS == validate_dl_arfcn(wcdma_dl_freq
                                  , FALSE
                                 )
    )
#endif
  {
    ret_acq_entry_ptr->freq = wcdma_dl_freq;
    *ret_type_freq_scan     = RRC_CSP_ACQ_SCAN;

#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
    /* Set the current band appropriately as it is used by SIB procedure 
     * after reading MIB to validate the MCC and Band Combination
     */
    if((rrccsp_get_curr_type_acq() == RRC_CSP_ANY_PLMN)
       && (is_freq_in_overlapping_850_and_800_bands(wcdma_dl_freq)))
    {
      if(rrccsp_is_supported_band(RRC_CSP_WCDMA_850_BAND)
          && rrccsp_is_supported_band(RRC_CSP_WCDMA_800_BAND))
      {
        /* Setting the current_band to RRC_CSP_WCDMA_850_BAND | RRC_CSP_WCDMA_800_BAND
         * or rrc_csp_int_data.wcdma_band_mask will not make any difference because
         * SIB procedure only checks if the band to which the UARFCN belongs to is
         * present in the current_band or not instead of checking whether they are equal.
         */
        rrc_csp_curr_select_data.current_band = rrc_csp_int_data.wcdma_band_mask;
      }
      else if(rrccsp_is_supported_band(RRC_CSP_WCDMA_800_BAND))
      {
        rrc_csp_curr_select_data.current_band = RRC_CSP_WCDMA_800_BAND;
      }
      else
      {
        rrc_csp_curr_select_data.current_band = RRC_CSP_WCDMA_850_BAND;
      }
    }
    /* Set the band based on frequency and MCC combination as in this case
     * MCC is known because NAS sent SPEC_PLMN search request. Notice that
     * MCC information is used only when the frequency belongs to the overlapping
     * region of 850 and 800 bands. 
     */
    else
    {
      rrc_csp_curr_select_data.current_band = rrc_get_frequency_band(
                                                rrc_csp_int_data.curr_acq_entry.freq,
                                                rrc_csp_curr_select_data.req_plmn.plmn_id);
    }
#else
    rrc_csp_curr_select_data.current_band = rrc_get_frequency_band(
                                              rrc_csp_int_data.curr_acq_entry.freq,
                                              rrc_csp_curr_select_data.req_plmn.plmn_id);
#endif

    WRRC_MSG1_HIGH("Setting current_band to %d", rrc_csp_curr_select_data.current_band);

    return (TRUE);
  }
  else
  {
    WRRC_MSG1_ERROR("ERR: freq %d not in valid band group",
                                       wcdma_dl_freq);
    return (FALSE);
  }
}


/*===========================================================================

FUNCTION          rrc_csp_start_cell_selection

DESCRIPTION       This function is responsible for starting a New Stored or
                  Initial Cell Selection. It initializes the Selection Data
                  based on the passed requested PLMN before calling 
                  rrc_csp_continue_stored_initial_select_ctrl which takes off
                  from there.

DEPENDENCIES
                  None

RETURN VALUE
                  SUCCESS: A frequency has been found where Acquisition 
                           can be tried.
                  FAILURE: No frequency found (all exhausted) or passed data not 
                           valid. Time for a deep sleep ?
SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type
rrc_csp_start_cell_selection
(
  rrc_requested_plmn_type  *req_plmn_ptr, 
  rrc_csp_acq_e_type  *ret_type_acq_ptr,
  rrc_csp_acq_entry_type  *ret_acq_entry_ptr,
  rrc_csp_freq_scan_e_type *ret_type_freq_scan
)
{
  uecomdef_status_e_type  ret_status = SUCCESS;
  boolean nv_freq_valid = FALSE;
  uint16 wcdma_dl_nv_freq = 0;

  /* Initialize Selection data so as to start with the correct Acq. type 
   * corresponding to Requested PLMN type. 
   */ 
  if(rrc_csp_init_select_data(req_plmn_ptr) == SUCCESS)
  {
    *ret_type_acq_ptr = rrc_csp_curr_select_data.acq_type;

    rrc_csp_int_data.num_scr_code_det = 0;

    /* Determine if there is a freq in NV that should be
     * scanned first 
     */
    if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
       && rrc_csp_int_data.wcdma_dl_freq_enabled)
    {
      nv_freq_valid = rrc_csp_try_cell_acq_on_nv_given_freq
                      (
                          rrc_csp_int_data.wcdma_dl_freq,
                          ret_type_acq_ptr,
                          ret_acq_entry_ptr,
                          ret_type_freq_scan
                      );
      if(nv_freq_valid == TRUE)
      {
        return SUCCESS;                      
      }
    }
    if(rrc_nv_channel_lock_item_value != 0)
    {

      wcdma_dl_nv_freq = rrc_nv_channel_lock_item_value;
      nv_freq_valid = rrc_csp_try_cell_acq_on_nv_given_freq
                      (
                        (rrc_freq_type)wcdma_dl_nv_freq,
                          ret_type_acq_ptr,
                          ret_acq_entry_ptr,
                          ret_type_freq_scan
                      );
      if(nv_freq_valid == TRUE)
      {
        return SUCCESS;                      
      }
      else
      {
        WRRC_MSG1_HIGH("NV freq not valid %d",wcdma_dl_nv_freq);
        return FAILURE;
      }
    }

    WRRC_MSG1_HIGH("WCDMA Bandclass bitmask : %d",rrc_csp_int_data.wcdma_band_mask);


    /* For transition from Connected Mode to Idle state, attempt acquisition 
     * on the best cell in active set before trying ACQ DB. Do this only
     * if the scr code has a valid value (< 8192). Currently scr code has
     * a valid value only when RCR procedure sends connection release complete
     * to the network.
     * Since PSCs can range from 0 to 512, the scrambling code can also range
     * from 0 to 16*512-1
     */
    if(rrc_csp_int_data.aset_scr_code < 8192)
    {
      if((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) ||
         ((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) &&
          ((!rrc_csp_int_data.dest_freq_present) ||
           (rrc_csp_int_data.dest_freq == rrc_csp_int_data.last_camped_freq_in_connected_mode)) &&
         ((!rrc_csp_int_data.dest_psc_present) ||
         (rrc_csp_int_data.dest_psc == (rrc_csp_int_data.aset_scr_code/16)))))
      {
        WRRC_MSG2_HIGH("Connected Mode ->Idle Tx. Attmpt acq on the best cell (scr %d, pn %d) in active set", 
                                                              rrc_csp_int_data.aset_scr_code, 
                                                              rrc_csp_int_data.aset_pn_pos);

        /* Populate the required variables with the scr code and pn position of the cell */
        rrc_csp_int_data.curr_acq_entry.scr_code  = rrc_csp_int_data.aset_scr_code;
        rrc_csp_int_data.pn_pos                   = rrc_csp_int_data.aset_pn_pos;

        rrc_csp_int_data.acq_mode = L1_WCDMA_FULL;
        rrc_csp_int_data.acq_type = L1_WCDMA_FREQ_SCR_POS;

        *ret_type_freq_scan = RRC_CSP_LAST_ASET_SCAN;
      }
      else
      {
        WRRC_MSG2_HIGH("aset_scr_code(%d) < 8192 when curr_scan(%d) is not LEAVING_CONN_MODE_SCAN",
                                                                            rrc_csp_int_data.aset_scr_code,
                                                                            rrc_csp_int_data.curr_scan);
        /* Since things have been setup, call continue_stored_initial_select_ctrl */
        ret_status = 
        rrc_csp_continue_stored_initial_select_ctrl(ret_type_acq_ptr, 
                                                    ret_acq_entry_ptr,
                                                    ret_type_freq_scan);
      }

      /* Invalidate the scr code */
      rrc_csp_int_data.aset_scr_code = 8192;
    }
    /* aset_scr_code is invalid */
    else
    if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) &&
          (rrc_csp_int_data.num_freq_in_list_db > 0)
      #ifdef FEATURE_LTE_TO_WCDMA
          && (!((rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE) && (rrc_csp_curr_select_data.dest_freq_in_use)))
      #endif
      )
    {
      rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_LIST_SCAN;
      WRRC_MSG1_HIGH("LFS_COOS: set curr frequency scan type =%d",
             rrc_csp_curr_select_data.scan_info.type_freq_scan);                 
      *ret_type_freq_scan = RRC_CSP_LIST_SCAN;
    }
    else
    if(rrc_csp_curr_select_data.scan_info.list_scan.num_freq > 0)
    {
      rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_LIST_SCAN;
      WRRC_MSG1_HIGH("LFS_COOS: set curr frequency scan type =%d",
             rrc_csp_curr_select_data.scan_info.type_freq_scan);                 
      *ret_type_freq_scan = RRC_CSP_LIST_SCAN;
    }
    /* When ACQ DB is empty do not perform full band scan  For connected mode OOS
     * do_lfs_scan will be FALSE
     */
    else
    if( rrc_csp_int_data.do_lfs_scan &&
       (rrc_csp_int_data.srvc_req_scan_scope == SYS_SCAN_SCOPE_ACQ_DB) &&
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
      )
    {
      WRRC_MSG0_HIGH("ACQ DB empty. Return immediate no service");
      rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_NO_SCAN;
      *ret_type_freq_scan = RRC_CSP_NO_SCAN;
      ret_status = FAILURE;
    }
    else
    {
      
      /* Since things have been setup, call continue_stored_initial_select_ctrl */
      ret_status = 
      rrc_csp_continue_stored_initial_select_ctrl(ret_type_acq_ptr, 
                                                  ret_acq_entry_ptr,
                                                  ret_type_freq_scan);
    }

    return(ret_status);
  }
  else
  {
    WRRC_MSG1_ERROR("ERR: Failure initializing Select Data for %d PLMN type",
                                          req_plmn_ptr->plmn_type);
    return(FAILURE);
  }

} /* rrc_csp_start_cell_selection */


/*===========================================================================

FUNCTION          RRCCSP_FILL_IN_PLMN_LAI_DOMAIN_AC_INFO

DESCRIPTION       

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
static uecomdef_status_e_type rrccsp_fill_in_plmn_lai_domain_ac_info(rrc_service_info_type *service_info_ptr)
{
  rrc_csp_access_control_e_type  cell_access_status;
  rrc_dsac_info_type             rrc_dsac_info;
  rrc_SysInfoType1              *sib1_ptr;
  uint32                         num_cn_domains,idx=0;
  rrc_CN_DomainSysInfoList    *   cn_domain_sys_info_list_ptr;
  
  rrc_plmn_identity_type plmn_id;

  rrc_ppac_info_type rrc_ppac_info;

  rrc_csp_acq_entry_type *acq_entry_ptr;
  rrc_csp_substate_e_type curr_csp_substate = rrc_get_csp_substate();  
  
  if((curr_csp_substate== RRCCSP_SUIT_CELL_RESELECT_WT_SIB) ||
     (curr_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB))
  {
    acq_entry_ptr = &rrc_csp_int_data.new_cell_acq_entry;
  }
  else
  {
    acq_entry_ptr = &rrc_csp_int_data.curr_acq_entry;
  }

  /* 1. Get the plmn that will be passed to NAS in a function call to determine
   *    if the PLMN/LA of the cell are forbidden or not.
   */
  service_info_ptr->selected_plmn.plmn_id = rrc_csp_int_data.curr_acq_entry.plmn_id;

  /*
    Get the common plmn id from curr_nw_db_entry
  */
  if(acq_entry_ptr->is_valid_nw_sharing)
  {
    plmn_id = rrc_csp_int_data.curr_nw_db_entry.common_plmn_id;
  }
  else
  {
    plmn_id = acq_entry_ptr->plmn_id;
  }
  WRRC_MSG2_HIGH("SIB Index PLMN is MCC%d - MNC%d",RRC_CSP_GET_INT_MCC(plmn_id), RRC_CSP_GET_INT_MNC(plmn_id));
  /* 2. Get the cell access status of the cell. I.e., fill up service_info structure with 
   *    cell access status
   */
  if(rrc_csp_find_access_control(&cell_access_status, 
                                 &rrc_dsac_info, 
                                 acq_entry_ptr->plmn_id,
                                 acq_entry_ptr->cell_id
                                ,plmn_id
                                ,&rrc_ppac_info
                                 ) == FAILURE)
  {
    ERR_FATAL("Failure finding cell access status", 0, 0, 0); 
  }

  (void) rrc_csp_find_rrc_cell_access(service_info_ptr, 
                                      &rrc_dsac_info, 
                                      cell_access_status, 
                                      acq_entry_ptr->status_mask);


  /* 3. Get the CN domain information and fill up service_info structure with this information 
   */
  sib1_ptr = (rrc_SysInfoType1 *) rrc_sibdb_return_sib_for_cell(
                                            rrc_SIB1,
                                            plmn_id, 
                                            acq_entry_ptr->cell_id);
  if(sib1_ptr == NULL)
  {
    /* SIB1 for this PLMN, CEll ID combination not present */
    ERR("ERR: SIB1 for MCC %d, MNC %d, Cell ID %d not present",
                                            RRC_CSP_GET_INT_MCC(plmn_id), 
                                            RRC_CSP_GET_INT_MNC(plmn_id),
                                            acq_entry_ptr->cell_id);

    return (FAILURE);
  } /* SIB1 not present */


  if(rrccsp_fill_nas_common_info(sib1_ptr, &(service_info_ptr->gsm_map_common_info)) == FAILURE)
  {
    return(FAILURE);
  }

  /* Fill in the CN domain specific NAS system info now */
  num_cn_domains = 0;
  cn_domain_sys_info_list_ptr = &sib1_ptr->cn_DomainSysInfoList; 

  do
  {            
    /* First check if Core network is GSM MAP. */
    if(cn_domain_sys_info_list_ptr->elem[idx].cn_Type.t == T_rrc_CN_DomainSysInfo_cn_Type_gsm_MAP)
    {
      if(rrccsp_fill_nas_cn_specific_info(&cn_domain_sys_info_list_ptr->elem[idx], 
                                          &service_info_ptr->cn_domain_info[num_cn_domains]) == FAILURE)
      {
        return(FAILURE);
      }

      num_cn_domains ++;
    } /* If CN type is GSM MAP */ 

    idx++;

  }while((cn_domain_sys_info_list_ptr->n > idx) && (num_cn_domains < RRC_MAX_CN_DOMAINS));   

  service_info_ptr->num_cn_domains           = num_cn_domains;
  service_info_ptr->gsm_map_common_info_incl = TRUE;
  service_info_ptr->selected_plmn.plmn_type  = RRC_GSM_MAP_PLMN_SEL;
  service_info_ptr->dsac_present             = rrc_dsac_info.valid;

  service_info_ptr->ppac_present             = rrc_ppac_info.valid;
  if(rrc_ppac_info.valid)
  {
    service_info_ptr->cs_reg_allowed          = rrc_ppac_info.cs_reg_allowed;
    service_info_ptr->ps_reg_allowed          = rrc_ppac_info.ps_reg_allowed;
    service_info_ptr->page_rsp_allowed        = rrc_ppac_info.page_rsp_allowed;
  }

  if(!rrc_dsac_info.valid)
  {
    WRRC_MSG1_HIGH("OOS: dsac info invalid. cell access status = %d", cell_access_status);
  }
  else
  {
    WRRC_MSG2_HIGH("OOS: dsac info valid. CS cell ac st = %d, PS cell ac st = %d", 
                     rrc_dsac_info.cs_access_control, rrc_dsac_info.ps_access_control);
  }

  return SUCCESS;
} /* rrccsp_fill_in_plmn_lai_domain_ac_info */

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
 /*===========================================================================

FUNCTION          RRC_CSP_CHECK_LAI_IN_REJECT_LIST

DESCRIPTION       This function is responsible to check if the current LAI 
                  is in the LAI reject list
DEPENDENCIES
                  None
RETURN VALUE
                  TRUE: If current LAI is present in reject list
                  FALSE: Otherwise
SIDE EFFECTS
                  None
===========================================================================*/
static boolean rrc_csp_check_lai_in_reject_list(inter_task_lai_T curr_lai)
{
  int i;

  for(i=0;i<rrc_csp_int_data.lai_reject_list.length;i++)
  {
    if(memcmp(&curr_lai,&rrc_csp_int_data.lai_reject_list.lai[i],sizeof(inter_task_lai_T)) == 0)
    {
      WRRC_MSG0_HIGH("LAI found in reject list");
      return(TRUE);
    }
  }
  return FALSE;
}
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
/*===========================================================================

FUNCTION          RRC_CSP_COMPARE_EXPECTED_CELL_FOR_CONN_OOS

DESCRIPTION       This function compares the expected acq. type with the 
                  actual Acquisition status mask and PLMN status mask and
                  declares the actual cell as Better than/Same as/Below
                  the expected cell, when UE is in connected mode out of service.
           
DEPENDENCIES
                  This function is closely knit with 
                  rrc_csp_continue_stored_initial_select_ctrl() and
                  rrc_csp_get_match_acq_entry().
                  The logic in this function to compare actual and expected 
                  cells is based on the logic in rrc_csp_continue_stored_
                  initial_select_ctrl() to search cells. A change in logic
                  in either must be reflected in others.

RETURN VALUE
                  RRC_CSP_CELL_BETTER_THAN_EXPECTED: Cell exceeds expectations.
                  RRC_CSP_CELL_AS_EXPECTED: Cell meets expectations.
                  RRC_CSP_CELL_BELOW_EXPECTED: Cell is below expectations.

SIDE EFFECTS
                  None.
===========================================================================*/
rrc_csp_cell_actual_vs_expected_e_type 
rrc_csp_compare_expected_cell_for_conn_oos
(
  rrc_csp_plmn_status_e_type cell_plmn_status_wrt_nas_plmn, 
  rrc_csp_plmn_status_e_type cell_plmn_status_wrt_rlf_plmn, 
  rrc_csp_acq_entry_type* acq_entry_ptr,
  rrc_csp_acq_e_type type_acq_expect
)
{
  boolean is_forbidden_lai = FALSE;
  rrc_csp_cell_actual_vs_expected_e_type  
  comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;

  /* Indicates if the cell is barred */
  boolean  cell_barred = FALSE;

  /* Indicates if cell belongs to forbidden reg. area */
  boolean  cell_forbid_reg_area = FALSE;

  rrc_service_info_type service_info;
  /* Indicates whether the service_info is filled in or not. */ 
  boolean service_info_valid = FALSE;

  boolean rrc_plmn_lai_allowed = FALSE;

  boolean is_current_cell_nw_shared = FALSE;


  if((RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()) &&
     (!service_info_valid))
  {
    /* Fill Service Info as it will be passed in a NAS fn call to determine
     * if registration can be performed on the cell or not. 
     */
    if(FAILURE == rrccsp_fill_in_plmn_lai_domain_ac_info(&service_info))
    {
      WRRC_MSG0_ERROR("ERR: OOS: failure returned by the fn rrccsp_fill_in_plmn_lai_domain_ac_info");
      return RRC_CSP_CELL_BELOW_EXPECTED;
    }
    service_info_valid = TRUE;
  } 

  if((rrc_csp_int_data.curr_type_acq == RRC_CSP_ANY_PLMN) &&
     (!service_info_valid))
  {
    /* Fill Service Info as it will be passed in a NAS fn call to determine
     * if registration can be performed on the cell or not. 
     */
    if(FAILURE == rrccsp_fill_in_plmn_lai_domain_ac_info(&service_info))
    {
      WRRC_MSG0_ERROR("ERR: OOS: failure returned by the fn rrccsp_fill_in_plmn_lai_domain_ac_info");
      return RRC_CSP_CELL_BELOW_EXPECTED;
    }
    service_info_valid = TRUE;
  } 

  is_current_cell_nw_shared = rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing;


  if(acq_entry_ptr->status_mask & RRC_CSP_IDLE_CELL_BAR_BIT)
  {
    cell_barred = TRUE;
  }

  if(cell_plmn_status_wrt_nas_plmn != RRC_CSP_PLMN_SAME_AS_REQ)
  {
    WRRC_MSG0_HIGH("Actual: BCCH PLMN diff from NAS requested");
  }

  if(cell_plmn_status_wrt_rlf_plmn != RRC_CSP_PLMN_SAME_AS_REQ)
  {
    WRRC_MSG0_HIGH("Actual: BCCH PLMN diff from RLF PLMN");
  }

  if(acq_entry_ptr->status_mask & RRC_CSP_FORBID_REG_AREA_BIT)
  {
    cell_forbid_reg_area = TRUE;
  }

  if(cell_barred == TRUE)
  {
    WRRC_MSG0_HIGH("Actual: barred cell");
  }

  if(cell_forbid_reg_area == TRUE)
  {
    WRRC_MSG0_HIGH("Actual: Forbid reg area");
  }


  if(!cell_barred)
  {
    /* In case of emergency call service req, camp on any non-barred PLMN. */
    if((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
        && (rrc_csp_int_data.service_req_cause == RRC_MO_CS_CALL))
    {
        /* Cell is Not Barred. Don't care for the PLMN.
         */
        comparison_status = RRC_CSP_CELL_AS_EXPECTED;
        /* For ANY PLMN ideally we can just say comparison status as expected.
         * Added optimization to provide a better among the List of shared PLMNs */

        if((is_current_cell_nw_shared) &&
           (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL) &&
           (!cell_forbid_reg_area))
        {
          /*lint -save -e644*/
#ifdef FEATURE_DUAL_SIM
          rrc_plmn_lai_allowed = mm_rrc_per_subs_plmn_lai_allowed(service_info,rrc_get_as_id());
#else
          rrc_plmn_lai_allowed = mm_rrc_plmn_lai_allowed(service_info);
#endif
          /*lint -restore*/
        }

        if((is_current_cell_nw_shared) &&
           (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL) &&
           ((cell_forbid_reg_area) || (!rrc_plmn_lai_allowed)))
        {
          comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;
          WRRC_MSG0_HIGH("Expected: Same PLMN & non-barred & non Forbid area");
        }
    }
    /* If PLMN is the same as the RLF PLMN*/
    else if(((cell_plmn_status_wrt_rlf_plmn == RRC_CSP_PLMN_SAME_AS_REQ) &&
               (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_AUTOMATIC))
               || ((cell_plmn_status_wrt_nas_plmn == RRC_CSP_PLMN_SAME_AS_REQ) &&
                    (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC))
            )
    {

      /* If PLMN is same or eq to the RLF PLMN, for Manual selction check if LAI is forbidden.*/
      if((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
         ||
        ((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED) 
           && (rrc_csp_int_data.service_req_cause != RRC_MO_CS_CALL))
       )
      {
        if(is_current_cell_nw_shared)
        {
          rrc_csp_int_data.curr_lai.PLMN_id= mmumts_convert_rrc_plmn_id_to_nas_plmn_id(rrc_csp_int_data.curr_acq_entry.plmn_id);
        }
    
            /*Check for the forbidden LAI*/
#ifndef FEATURE_WCDMA_FREE_FLOATING_TASK
    #ifdef FEATURE_DUAL_SIM
        is_forbidden_lai = mm_per_subs_check_lai_in_reject_list(rrc_csp_int_data.curr_lai,rrc_get_as_id());
    #else
        is_forbidden_lai = mm_check_lai_in_reject_list(rrc_csp_int_data.curr_lai);
    #endif
#else
        is_forbidden_lai = rrc_csp_check_lai_in_reject_list(rrc_csp_int_data.curr_lai);
#endif
        if(is_forbidden_lai)
        {
          WRRC_MSG1_HIGH("Cause 15: is_forbidden_lai= %d",is_forbidden_lai);
          comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;
        }
        else
        {
          comparison_status = RRC_CSP_CELL_AS_EXPECTED;
        }
      }
      else
      {
        if(cell_forbid_reg_area == TRUE)
        {
          comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;
          WRRC_MSG0_HIGH("Expected: Same PLMN & non-barred & non Forbid area");
        }
        else
        {
          comparison_status = RRC_CSP_CELL_AS_EXPECTED;
        }
      }
#ifdef FEATURE_FEMTO_CSG
        if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
        {
          if(acq_entry_ptr->csg_id != rrc_csp_int_data.rlf_csg_id)
          {
            comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;                
            WRRC_MSG1_HIGH("CSG: Expected CSG ID: %d", rrc_csp_int_data.rlf_csg_id);
          }
        }
#endif
    }
    /* CSP should proceed with cell selection if
     *     Cell Selection is part of OOS_Scan 
     * and 30s timer has expired 
     * and the PLMN found is NOT ERPLMN (already checked by the status RRC_CSP_PLMN_DIFF_THAN_REQ above) 
     * and the n/w sel mode is NOT Manual (because for Manual mode UE should look for suitable cells only
     *     even after 30s period - details are in the memo)
     * and PLMN and LA are not forbidden
     * and Cell should allow at least normal calls so that registration is guaranteed
     */
    else if((rrc_csp_int_data.oos_splmn_srch_tmr_expd && 
              (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL) &&
              (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
#ifdef FEATURE_FEMTO_CSG
              (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG) &&
#endif
              !cell_forbid_reg_area))
    {
  #ifdef FEATURE_DUAL_SIM
      rrc_plmn_lai_allowed = mm_rrc_per_subs_plmn_lai_allowed(service_info,rrc_get_as_id()); 
  #else
      rrc_plmn_lai_allowed = mm_rrc_plmn_lai_allowed(service_info);
  #endif
      if(rrc_plmn_lai_allowed)
      {
        comparison_status = RRC_CSP_CELL_AS_EXPECTED;
        WRRC_MSG0_HIGH("OOS: Cell as expected. Proceed with camping");
      }
      else
      {
        comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;                
        WRRC_MSG0_HIGH("Expected: non-barred & non Forbid area");
      }
    }
  }
  else /* Cell is barred*/
  {
    comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;                
    WRRC_MSG0_HIGH("Expected: Same PLMN & non-barred & non Forbid area");
  }

  switch(comparison_status)
  {
    case RRC_CSP_CELL_BELOW_EXPECTED:
      MSG_MED("Acquired cell Below Expected", 0, 0, 0);
      break;

    case RRC_CSP_CELL_AS_EXPECTED:
      MSG_MED("Acquired cell as Expected", 0, 0, 0);
      break;

    case RRC_CSP_CELL_BETTER_THAN_EXPECTED:
      MSG_MED("Acquired cell Better than Expected", 0, 0, 0);
      break;

    default:
      WRRC_MSG1_ERROR("ERR: Invalid Cell Comparison Status value: %d", comparison_status);
      break;
  }

  return(comparison_status);
}

#endif

/*===========================================================================

FUNCTION          RRC_CSP_COMPARE_EXPECTED_CELL

DESCRIPTION       This function compares the expected acq. type with the 
                  actual Acquisition status mask and PLMN status mask and
                  declares the actual cell as Better than/Same as/Below
                  the expected cell.
           
DEPENDENCIES
                  This function is closely knit with 
                  rrc_csp_continue_stored_initial_select_ctrl() and
                  rrc_csp_get_match_acq_entry().
                  The logic in this function to compare actual and expected 
                  cells is based on the logic in rrc_csp_continue_stored_
                  initial_select_ctrl() to search cells. A change in logic
                  in either must be reflected in others.

RETURN VALUE
                  RRC_CSP_CELL_BETTER_THAN_EXPECTED: Cell exceeds expectations.
                  RRC_CSP_CELL_AS_EXPECTED: Cell meets expectations.
                  RRC_CSP_CELL_BELOW_EXPECTED: Cell is below expectations.

SIDE EFFECTS
                  None.
===========================================================================*/
rrc_csp_cell_actual_vs_expected_e_type
rrc_csp_compare_expected_cell
(
  rrc_csp_plmn_status_e_type cell_plmn_status, 
  rrc_csp_acq_entry_type* acq_entry_ptr,
  rrc_csp_acq_e_type type_acq_expect
)
{        
  boolean is_forbidden_lai = FALSE;
  rrc_csp_cell_actual_vs_expected_e_type  
  comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;
  rrc_csp_substate_e_type curr_csp_substate = rrc_get_csp_substate();

  /* Indicates if the cell is barred */
  boolean  cell_barred = FALSE;

  /* Indicates if cell belongs to forbidden reg. area */
  boolean  cell_forbid_reg_area = FALSE;

  rrc_service_info_type service_info;
  /* Indicates whether the service_info is filled in or not. */ 
  boolean service_info_valid = FALSE;

  boolean rrc_plmn_lai_allowed = FALSE;

  boolean is_current_cell_nw_shared = FALSE;

  boolean dual_mode_enabled = rrcmcm_is_dualmode_enabled();

  if((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) &&
     (!service_info_valid))
  {
    /* Fill Service Info as it will be passed in a NAS fn call to determine
     * if registration can be performed on the cell or not. 
     */
    if(FAILURE == rrccsp_fill_in_plmn_lai_domain_ac_info(&service_info))
    {
      WRRC_MSG0_ERROR("ERR: OOS: failure returned by the fn rrccsp_fill_in_plmn_lai_domain_ac_info");
      return RRC_CSP_CELL_BELOW_EXPECTED;
    }
    service_info_valid = TRUE;
  } 

  if((rrc_csp_int_data.curr_type_acq == RRC_CSP_ANY_PLMN) &&
     (!service_info_valid))
  {
    /* Fill Service Info as it will be passed in a NAS fn call to determine
     * if registration can be performed on the cell or not. 
     */
    if(FAILURE == rrccsp_fill_in_plmn_lai_domain_ac_info(&service_info))
    {
      WRRC_MSG0_ERROR("ERR: OOS: failure returned by the fn rrccsp_fill_in_plmn_lai_domain_ac_info");
      return RRC_CSP_CELL_BELOW_EXPECTED;
    }
    service_info_valid = TRUE;
  } 

  if((curr_csp_substate == RRCCSP_SUIT_CELL_RESELECT_WT_SIB) ||
     (curr_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB))
  {
    is_current_cell_nw_shared = rrc_csp_int_data.new_cell_acq_entry.is_valid_nw_sharing;
  }
  else
  {
    is_current_cell_nw_shared = rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing;
  }

  if(acq_entry_ptr->status_mask & RRC_CSP_IDLE_CELL_BAR_BIT)
  {
    cell_barred = TRUE;
    WRRC_MSG0_HIGH("Actual: barred cell");
  }

  if(cell_plmn_status != RRC_CSP_PLMN_SAME_AS_REQ)
  {
    rrc_csp_int_data.interrat_reselect_failure_reason = RRC_WCDMA_PLMN_MISMATCH;
    WRRC_MSG0_HIGH("Actual: Diff PLMN");
  }

  if(acq_entry_ptr->status_mask & RRC_CSP_FORBID_REG_AREA_BIT)
  {
    cell_forbid_reg_area = TRUE;
    WRRC_MSG0_HIGH("Actual: Forbid reg area");
    rrc_csp_int_data.interrat_reselect_failure_reason = RRC_WCDMA_FORBIDDEN_LA;
  }

  switch(type_acq_expect)
  {
    case RRC_CSP_SPEC_PLMN:
      /* A non-barred cell belonging to a specific PLMN (matching the requested 
       * PLMN) and NOT in a forbidden reg. area was expected. 
       */
      if((cell_plmn_status == RRC_CSP_PLMN_SAME_AS_REQ) && 
         (cell_barred == FALSE))
      {        
        /* If Network select mode is MANUAL then skip the check
         * for forbidden LAIs
         */
        if(
           (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
           /* If Background Service Request is received from NAS/RR when UE mode
            * is manual then RRC needs to do the same checks as are done when
            * n/w sel mode is explicitly set to SYS_NETWORK_SELECTION_MODE_MANUAL 
            */
#ifdef FEATURE_FEMTO_CSG
           ||(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
#endif
           || 
           (   ((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
                || (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN))
            && (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
            && (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL)
           )
          )
        {
          if((curr_csp_substate ==RRCCSP_SUIT_CELL_RESELECT_WT_SIB) ||             
             (rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN))
          {
            WRRC_MSG2_HIGH("curr CSP sub state =%d, curr CSP scan =%d",
                                         curr_csp_substate ,
                                         rrc_csp_int_data.curr_scan);
            if(cell_forbid_reg_area == TRUE)
            {
              comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;
              WRRC_MSG0_HIGH("Expected: Same PLMN & non-barred & non Forbid area");
            }
            else
            {
              comparison_status = RRC_CSP_CELL_AS_EXPECTED;
            }
          }
          else  /* for rest of the scans */
          {

            if(is_current_cell_nw_shared)
            {
              /*Convert RRC PLMN Id to NAS PLMN Id*/
              if(curr_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB)
              {
                rrc_csp_int_data.curr_lai.PLMN_id= mmumts_convert_rrc_plmn_id_to_nas_plmn_id(rrc_csp_int_data.new_cell_acq_entry.plmn_id);
              }
              else
              {
                rrc_csp_int_data.curr_lai.PLMN_id= mmumts_convert_rrc_plmn_id_to_nas_plmn_id(rrc_csp_int_data.curr_acq_entry.plmn_id);
              }
            }

            /*Check for the forbidden LAI*/
#ifndef FEATURE_WCDMA_FREE_FLOATING_TASK
#ifdef FEATURE_DUAL_SIM
            is_forbidden_lai = mm_per_subs_check_lai_in_reject_list(rrc_csp_int_data.curr_lai,rrc_get_as_id());
#else
            is_forbidden_lai = mm_check_lai_in_reject_list(rrc_csp_int_data.curr_lai);
#endif
#else
            is_forbidden_lai = rrc_csp_check_lai_in_reject_list(rrc_csp_int_data.curr_lai);
#endif
            if(is_forbidden_lai)
            {
              WRRC_MSG1_HIGH("Cause 15: is_forbidden_lai= %d",is_forbidden_lai);
              comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;
#ifdef FEATURE_FEMTO_CSG
              /*Skip LAI reject list check for CSG selection*/
              if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)&&
                 (curr_csp_substate ==RRCCSP_CELL_SEL_WT_SIB)&&
                 (rrc_csp_int_data.curr_req_plmn.csg_id != SYS_CSG_ID_INVALID))
              {
                WRRC_MSG0_HIGH("CSG: Skip LAI reject check");
                comparison_status = RRC_CSP_CELL_AS_EXPECTED;
              }
#endif
            }
            else
            {
              comparison_status = RRC_CSP_CELL_AS_EXPECTED;
            }
          }
        }
        else
        {
          if(cell_forbid_reg_area == TRUE)
          {
            comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;
            WRRC_MSG0_HIGH("Expected: Same PLMN & non-barred & non Forbid area");
          }
          else
          {
            comparison_status = RRC_CSP_CELL_AS_EXPECTED;
          }
        }
#ifdef FEATURE_FEMTO_CSG
        if((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)||
           (((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)||
             (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL))&&
             (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)&&
             (curr_csp_substate ==RRCCSP_CELL_SEL_WT_SIB)&&
             (rrc_csp_int_data.curr_req_plmn.csg_id != SYS_CSG_ID_INVALID)))
        {
          if(rrc_csp_int_data.curr_req_plmn.csg_id != acq_entry_ptr->csg_id)
          {
            comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;                
            WRRC_MSG1_HIGH("CSG: Expected CSG ID: %d", rrc_csp_int_data.curr_req_plmn.csg_id);
          }
        }
#endif
      }
      else if(cell_barred)
      {
        comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;                
        WRRC_MSG0_HIGH("Expected: Same PLMN & non-barred & non Forbid area");
      }
      /* Cell_plmn_status == RRC_CSP_PLMN_DIFF_THAN_REQ */
      else 
      {
        /* CSP should proceed with cell selection if
         *     Cell Selection is part of OOS_Scan 
         * and 30s timer has expired 
         * and atleast 1 WCDMA search is done
         * and the PLMN found is NOT ERPLMN (already checked by the status RRC_CSP_PLMN_DIFF_THAN_REQ above) 
         * and the n/w sel mode is NOT Manual (because for Manual mode UE should look for suitable cells only
         *     even after 30s period - details are in the memo)
         * and PLMN and LA are not forbidden
         * and Cell should allow at least normal calls so that registration is guaranteed
         */
        if(    (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
            && (rrc_csp_int_data.oos_splmn_srch_tmr_expd)
            && (    ((dual_mode_enabled) && (rrc_csp_int_data.oos_gsm_srch_done))
                ||  (!dual_mode_enabled))
            && (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL)
#ifdef FEATURE_FEMTO_CSG
            && (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
#endif
            && (!cell_forbid_reg_area))
        {
          /*lint -save -e644*/
#ifdef FEATURE_DUAL_SIM
          rrc_plmn_lai_allowed = mm_rrc_per_subs_plmn_lai_allowed(service_info,rrc_get_as_id()); 
#else
          rrc_plmn_lai_allowed = mm_rrc_plmn_lai_allowed(service_info);
#endif
          /*lint -restore*/
        }
        if(    (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
            && (rrc_csp_int_data.oos_splmn_srch_tmr_expd)
            && (    ((dual_mode_enabled) && (rrc_csp_int_data.oos_gsm_srch_done))
                ||  (!dual_mode_enabled))
            && (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL)
#ifdef FEATURE_FEMTO_CSG
            && (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
#endif
            && (!cell_forbid_reg_area)
            /*lint -save -e644*/
            && (rrc_plmn_lai_allowed)
            /*lint -restore*/
          )
        {  
          comparison_status = RRC_CSP_CELL_AS_EXPECTED;
          WRRC_MSG0_HIGH("OOS: Cell as expected. Proceed with camping");
        }
        else
        {
          comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;                
          WRRC_MSG0_HIGH("Expected: Same PLMN & non-barred & non Forbid area");
        }

      }
      break;

    case RRC_CSP_ANY_PLMN:
      /* A NON barred cell in a NON forbidden registration area belonging to 
       * Any PLMN was expected.
       */
      if(cell_barred == FALSE)
      {
        /* Cell is Not Barred. Don't care for the PLMN.
         */
        comparison_status = RRC_CSP_CELL_AS_EXPECTED;
        /* For ANY PLMN ideally we can just say comparison status as expected.
         * Added optimization to provide a better among the List of shared PLMNs */

        if((is_current_cell_nw_shared) &&
           (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) &&
           (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN) &&
           (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL) &&
#ifdef FEATURE_FEMTO_CSG
           (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG) &&
#endif
           (!cell_forbid_reg_area))
        {
          /*lint -save -e644*/
#ifdef FEATURE_DUAL_SIM
          rrc_plmn_lai_allowed = mm_rrc_per_subs_plmn_lai_allowed(service_info,rrc_get_as_id());
#else
          rrc_plmn_lai_allowed = mm_rrc_plmn_lai_allowed(service_info);
#endif
          /*lint -restore*/
        }

        if((is_current_cell_nw_shared) &&
           (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) &&
           (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN) &&
           (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL) &&
#ifdef FEATURE_FEMTO_CSG
           (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG) &&
#endif
           ((cell_forbid_reg_area) || (!rrc_plmn_lai_allowed)))
        {
          comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;
          WRRC_MSG0_HIGH("Expected: Same PLMN & non-barred & non Forbid area");
        }
      }
      else
      {
        /* Cell is barred */
        comparison_status = RRC_CSP_CELL_BELOW_EXPECTED;
        WRRC_MSG0_HIGH("Expected: Any PLMN & non-barred");
      }
      break;

    default:
      WRRC_MSG1_HIGH("Invalid expected type %d of Acq.", type_acq_expect); 
      break;

  } /* switch(type_acq_expect) */

  switch(comparison_status)
  {
    case RRC_CSP_CELL_BELOW_EXPECTED:
      MSG_MED("Acquired cell Below Expected", 0, 0, 0);
      break;

    case RRC_CSP_CELL_AS_EXPECTED:
      MSG_MED("Acquired cell as Expected", 0, 0, 0);
      break;

    case RRC_CSP_CELL_BETTER_THAN_EXPECTED:
      MSG_MED("Acquired cell Better than Expected", 0, 0, 0);
      break;

    default:
      WRRC_MSG1_ERROR("ERR: Invalid Cell Comparison Status value: %d", comparison_status);
      break;
  }

  return(comparison_status);

} /* rrc_csp_compare_expected_cell */


/*===========================================================================

FUNCTION          RRC_CSP_GEN_STATUS_MASK

DESCRIPTION       This function generates an Acq. status mask from Cell 
                  Barred status and LAI Status. The Acq. Status mask is
                  understood by Acq. database whereas Cell Barred status and 
                  LAI Status are understood by Cell Selection utility 
                  functions and Cell Selection state machine functions.
                  
                  This utility acts as an interpretor so that the status 
                  returned by Cell selection utilities can be compared with
                  the one returned by Acq. Database access functions.

DEPENDENCIES      None.

RETURN VALUE
                  A bit mask that represents Acq. status.

SIDE EFFECTS
                  None
===========================================================================*/
rrc_csp_acq_status_type 
rrc_csp_gen_status_mask(rrc_csp_cell_bar_e_type  cell_bar_status,
                        rrc_csp_lai_status_e_type  lai_status)
{
    rrc_csp_acq_status_type  acq_status_mask = 0;

    /* Set the Cell bar bit if cell is non-barred */
    if(cell_bar_status != RRC_CSP_CELL_OK)
    {
      acq_status_mask |= RRC_CSP_IDLE_CELL_BAR_BIT;
    }

    /* Set the Reg Area barred bit if the cell belongs to a forbidden LAI */
    if(lai_status != RRC_CSP_LAI_OK)
    {
      acq_status_mask |= RRC_CSP_FORBID_REG_AREA_BIT;
    }

    return(acq_status_mask);
} /* rrc_csp_gen_status_mask */


/*===========================================================================

FUNCTION          RRC_CSP_FILL_ACQ_ENTRY_FROM_SIB

DESCRIPTION       This function is responsible for filling up all fields of
                  Acq Entry from SIB3 data and SIB Confirmation command data.
                  It also fills the Cell Barred status into the passed pointer
                  for usage by the calling function.

DEPENDENCIES
                  SIB Confirmation pointer should point to valid data.
                  SIB3 should have been received for this cell (identified 
                  by PLMN ID, Cell ID combination).

RETURN VALUE
                  SUCCESS: Acq entry successfully filled.
                  FAILURE: Acq. entry could not be filled (multiple reasons)

SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type
rrc_csp_fill_acq_entry_from_sib( rrc_sib_event_cnf_type *sib_cnf, 
                                 rrc_csp_acq_entry_type *acq_entry_ptr,
                                 rrc_csp_cell_bar_e_type  *cell_bar_status)
{
    
  /* Temporary storage for LAI status */
  rrc_csp_lai_status_e_type  lai_status;

  /* This stores the LAI received in SIB1 */
  rrc_lai_type  lai;

  /* This stores the GSM-MAP NAS system info */
  rrc_gsm_map_nas_sys_info_type   nas_common_info;

  /* Pointer to SIB1 */
  rrc_SysInfoType1 *sib1_ptr;

  unsigned char  *data_ptr;
  uint32         i;

  boolean shared_plmn_idx = 0;
  boolean hplmn_cell_bar_status_found = FALSE;
  rrc_csp_cell_bar_e_type hplmn_cell_bar_status = RRC_CSP_CELL_OK;
  boolean vplmn_cell_bar_status_found = FALSE;
  rrc_csp_cell_bar_e_type vplmn_cell_bar_status = RRC_CSP_CELL_OK;


  rrc_plmn_identity_type sib_idx_plmn_id;

  /* Don't go further if one of passed pointers is a NULL */
  if((sib_cnf == NULL) || (acq_entry_ptr == NULL) || (cell_bar_status == NULL))
  {
     WRRC_MSG0_ERROR("ERR: Acq ptr ,SIB Cnf ptr or Cell Bar status ptr is a NULL");
    return(FAILURE);
  }

  /* Fill the tmp_acq_entry with SIB data */
  if(!acq_entry_ptr->is_valid_nw_sharing)
  {
    acq_entry_ptr->plmn_id    = sib_cnf->plmn_id;
  }


  if(acq_entry_ptr->is_valid_nw_sharing)
  {
    sib_idx_plmn_id = rrc_csp_int_data.curr_nw_db_entry.common_plmn_id;
    WRRC_MSG0_HIGH("Use Common PLMN-ID from curr_nw_db_entry");
  }
  else
  {
    sib_idx_plmn_id = acq_entry_ptr->plmn_id;
  }
  acq_entry_ptr->freq       = (rrc_freq_type)sib_cnf->freq;
  acq_entry_ptr->scr_code   = (rrc_scr_code_type)sib_cnf->scrambling_code;
  acq_entry_ptr->cell_id    = sib_cnf->cell_id;

#ifdef FEATURE_WCDMA_MULTIBAND
  acq_entry_ptr->band_class = rrc_get_frequency_band(acq_entry_ptr->freq, acq_entry_ptr->plmn_id);
  WRRC_MSG2_HIGH("Setting band_class %d for freq %d",
             acq_entry_ptr->band_class,acq_entry_ptr->freq);
#else /* FEATURE_WCDMA_MULTIBAND */

   acq_entry_ptr->band_class = RRC_CSP_PCS_BAND;
   WRRC_MSG2_HIGH("Setting band_class %d for freq %d",
              acq_entry_ptr->band_class,acq_entry_ptr->freq);
 
#endif

#ifdef FEATURE_FEMTO_CSG
  acq_entry_ptr->csg_id = sib_cnf->csg_id;

  if(!rrccsp_asf_actively_in_prog())
  {
#endif

  /* Now generate status mask for this cell */
  /* Fill up the NAS system info here */
  /* Let us first read SIB1 - we should have SIB1 for sure */
  /* WTOW BPLMN search in progress. Read SIB1 */
 /* Now generate status mask for this cell */
  /* Fill up the NAS system info here */
  /* Let us first read SIB1 - we should have SIB1 for sure */
  if((rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
		 && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
		 && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
		 && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
		 && (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN))
  {
    sib1_ptr = (rrc_SysInfoType1 *) rrc_sibdb_return_sib_for_cell(
                                              rrc_SIB1,
                                              sib_idx_plmn_id, 
                                              acq_entry_ptr->cell_id);
  }
  /* WTOW BPLMN search in progress. Read SIB1 */
  else
  {
    sib1_ptr = (rrc_SysInfoType1 *) rrc_bplmn_return_sib_for_cell(rrc_SIB1);

  }
  if( sib1_ptr == NULL )
  {
    /* SIB3 for this PLMN, CEll ID combination not present */
    ERR("ERR: SIB1 for MCC %d, MNC %d, Cell ID %d not present",
                      RRC_CSP_GET_INT_MCC(sib_idx_plmn_id), 
                      RRC_CSP_GET_INT_MNC(sib_idx_plmn_id),
                      acq_entry_ptr->cell_id);

    return(FAILURE);
  } /* SIB1 not present */

  /* The SIB1 is present */
  nas_common_info.length = (uint32)sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts;
  /* Check if the RRC MM interface structure array can hold this info */
  if(sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts > RRC_MAX_GSM_INFO_OCTET_COUNT)
  {
    WRRC_MSG1_ERROR("ERR: Not enough space for Common NAS Sys Info (%d bytes)",
                        sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts);
    return(FAILURE);
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
  if(mm_extract_lac_from_sib_info(&lai, &nas_common_info) == FALSE)
  {
    WRRC_MSG0_ERROR("LAC length is less than 2 bytes");
  }
  
  /* Since MM doesn't know the PLMN from NAS system info, fill in the PLMN now */
  lai.plmn = acq_entry_ptr->plmn_id;

  rrc_csp_int_data.curr_lai.location_area_code[0]= lai.lac[0];
  rrc_csp_int_data.curr_lai.location_area_code[1]= lai.lac[1];
  WRRC_MSG2_HIGH("Cause 15: lac[0] = %d, lac[1]=%d",rrc_csp_int_data.curr_lai.location_area_code[0],rrc_csp_int_data.curr_lai.location_area_code[1]);
  /*Convert RRC PLMN Id to NAS PLMN Id*/
  rrc_csp_int_data.curr_lai.PLMN_id= mmumts_convert_rrc_plmn_id_to_nas_plmn_id(lai.plmn);

  if(rrc_csp_find_forbid_lai(&lai, &lai_status) 
     == FAILURE)
  {
    WRRC_MSG0_HIGH("Failure finding LAI status");
    return(FAILURE);
  }

#ifdef FEATURE_FEMTO_CSG
  }
  else
  {
    WRRC_MSG0_HIGH("CSG: Skip forbidden LAI check as SIB1 is not available.");
    lai_status = RRC_CSP_LAI_OK;
  }
#endif

  /* Generate Cell barred status based on SIB info for this cell */
  if(rrc_csp_find_cell_bar_status(acq_entry_ptr->plmn_id, 
                                  acq_entry_ptr->cell_id, 
                                  cell_bar_status
                                  ,sib_idx_plmn_id
                                  ) 
      == FAILURE)
  {
    WRRC_MSG0_HIGH("Failure to fill Cell Barred Status");
    return(FAILURE);
  }

  /* Generate Acq. entry status mask for storage in Acq. database */ 
  acq_entry_ptr->status_mask = rrc_csp_gen_status_mask(*cell_bar_status, 
                                                       lai_status);

  if(acq_entry_ptr->is_valid_nw_sharing)
  {
    rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx++].status_mask = acq_entry_ptr->status_mask;

    if(rrc_is_bcch_plmn_an_hplmn_or_ehplmn(acq_entry_ptr->plmn_id))
    {
      hplmn_cell_bar_status = *cell_bar_status;
      hplmn_cell_bar_status_found = TRUE;
    }
    else
    {
      vplmn_cell_bar_status = *cell_bar_status;
      vplmn_cell_bar_status_found = TRUE;
    }

    MSG_HIGH("NWS: ACQ Status mask : %d for MCC:MNC - %d:%d",
                                                             rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx-1].status_mask,
                                                             RRC_CSP_GET_INT_MCC(rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx-1].plmn_id),
                                                             RRC_CSP_GET_INT_MNC(rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx-1].plmn_id));

    while((shared_plmn_idx < MAX_SHARED_PLMNS) &&(shared_plmn_idx < rrc_csp_int_data.curr_nw_db_entry.num_of_shared_plmns))
    {
      lai.plmn = rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx].plmn_id;

      if(!acq_entry_ptr->is_valid_nw_sharing)
      {
        sib_idx_plmn_id = lai.plmn;
      }

#ifdef FEATURE_FEMTO_CSG
      if(rrccsp_asf_actively_in_prog())
      {
        WRRC_MSG0_HIGH("CSG: Hack forbidden LAI check as SIB1 is not available.");
        lai_status = RRC_CSP_LAI_OK;
      }
      else
#endif
      (void)rrc_csp_find_forbid_lai(&lai, &lai_status);

      /* Below code is added to skip calling rrc_csp_find_cell_bar_status again since we have alredy called it once
       * cell bar status gives different results based on hplmn and non-hplmn cases i.e the reason it
       * has to be called twice in the worst case */

      if(rrc_is_bcch_plmn_an_hplmn_or_ehplmn(lai.plmn))
      {
        if(!hplmn_cell_bar_status_found)
        {
          /* Generate Cell barred status based on SIB info for this cell */
          if(rrc_csp_find_cell_bar_status(lai.plmn, 
                                          acq_entry_ptr->cell_id, 
                                          cell_bar_status
                                          ,sib_idx_plmn_id
                                          ) 
             == FAILURE)
          {
            WRRC_MSG0_HIGH("Failure to fill Cell Barred Status");
            return(FAILURE);
          }
          hplmn_cell_bar_status = *cell_bar_status;
          hplmn_cell_bar_status_found = TRUE;
        }
        else
        {
          *cell_bar_status = hplmn_cell_bar_status;
        }
      }
      else
      {
        if(!vplmn_cell_bar_status_found)
        {
          /* Generate Cell barred status based on SIB info for this cell */
          if(rrc_csp_find_cell_bar_status(lai.plmn, 
                                          acq_entry_ptr->cell_id, 
                                          cell_bar_status
                                          ,sib_idx_plmn_id
                                          ) 
             == FAILURE)
          {
            WRRC_MSG0_HIGH("Failure to fill Cell Barred Status");
            return(FAILURE);
          }
          vplmn_cell_bar_status = *cell_bar_status;
          vplmn_cell_bar_status_found = TRUE;
        }
        else
        {
          *cell_bar_status = vplmn_cell_bar_status;
        }
      }

      rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx++].status_mask 
                                                          = rrc_csp_gen_status_mask(*cell_bar_status, lai_status);

      MSG_HIGH("NWS: ACQ Status mask : %d for MCC:MNC - %d:%d",
                                         rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx-1].status_mask,
                                         RRC_CSP_GET_INT_MCC(rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx-1].plmn_id),
                                         RRC_CSP_GET_INT_MNC(rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx-1].plmn_id));
    }

    /* hplmn_cell_bar_status may be different from vplmn_cell_bar_status*/
    if(hplmn_cell_bar_status_found == TRUE)
    {
      *cell_bar_status = hplmn_cell_bar_status;
    }
  }
  return(SUCCESS);
} /* rrc_csp_fill_acq_entry_from_sib */


/*===========================================================================

FUNCTION          RRC_CSP_SEND_L1_IDLE_REQ

DESCRIPTION       This function sends RRC_RESET_L1_REQ to LLC 
                  which will be forwarded to WL1 in RRC_LLC_CMD_REQ
                  to put WL1 in Idle state.

DEPENDENCIES
                  None.

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_send_l1_idle_req(void)
{

  rrc_cmd_type        *csp_out_cmd_ptr;

  /* Send the command */
  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    /* Populate the cmd data */
    csp_out_cmd_ptr->cmd_hdr.cmd_id = RRC_RESET_L1_REQ;

#ifdef FEATURE_DUAL_SIM
        csp_out_cmd_ptr->cmd.rrc_reset_l1_req.reset_l1_cnf_needed = FALSE;
        if(rrc_csp_int_data.csp_waiting_for_wl1_to_release_lock)
        {
          csp_out_cmd_ptr->cmd.rrc_reset_l1_req.reset_l1_cnf_needed = TRUE;
          WRRC_MSG0_HIGH("DSIM: CSP waiting for RESET_L1_CNF");
        }
#endif
    /* Send the Internal cmd to RRC */
    rrc_put_int_cmd( csp_out_cmd_ptr );
  }
} /* rrc_csp_send_l1_idle_req */


/*===========================================================================

FUNCTION RRCCSP_SEND_CPHY_CELL_BAR_REQ

DESCRIPTION

  This function is called by CSP, SIB, and RCE procedures to send CPHY_CELL_BAR_REQ
  to L1 so that the corresponding cell or frequency or active set can be barred.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrccsp_send_cphy_cell_bar_req 
(
  rrc_csp_cell_bar_info_e_type cell_bar_info,
  uint32 t_barred
)
{
  rrc_csp_acq_entry_type acq_entry = rrc_csp_int_data.curr_acq_entry;
  rrc_csp_substate_e_type curr_csp_substate = rrc_get_csp_substate();
  rrc_csp_int_data.cell_bar_info = cell_bar_info;
  rrc_csp_int_data.wcdma_cell_time_to_reselect = t_barred;

  /* Check if WCDMA cell/frequency needs to be barred.
   * Populate acq_entry with appropriate frequency and scrambling code
   */
  if((rrc_csp_int_data.cell_bar_info != RRC_CSP_GSM_CELL_BARRED)
#ifdef FEATURE_WCDMA_TO_LTE
     &&(rrc_csp_int_data.cell_bar_info != RRC_CSP_LTE_CELL_BARRED)
     &&(rrc_csp_int_data.cell_bar_info != RRC_CSP_LTE_FREQ_BARRED)
#endif
    )
  { 
    /* Determine if the neighboring cell is barred or the serving
     * cell is barred. 
     * Note: We could not use scan type RRC_CSP_CELL_RESELECTION_SCAN
     * because on completion of cell reselection, the scan type is not 
     * reset to RRC_CSP_CELL_SELECTION_SCAN.
     */
    if((curr_csp_substate == RRCCSP_SUIT_CELL_RESELECT_WT_SIB)
       || (curr_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB))
    {
      MSG_HIGH("Cell/Freq Bar Req to be sent during CSP resel state %d for freq %d, scr_code %d", 
                                        curr_csp_substate, rrc_csp_int_data.new_cell_acq_entry.freq, 
                                        rrc_csp_int_data.new_cell_acq_entry.scr_code);
      acq_entry.freq     = rrc_csp_int_data.new_cell_acq_entry.freq;
      acq_entry.scr_code = rrc_csp_int_data.new_cell_acq_entry.scr_code;

      if(!rrccsp_is_inter_frequency_cell_resel_in_progress() 
         && (rrc_csp_int_data.cell_bar_info == RRC_CSP_WCDMA_FREQ_BARRED)
         && !(rrc_csp_int_data.bar_for_resel_only) && 
         /* WL1 api returning TRUE stay dont update cell loss */
         /* L1 api updates TRUE for USET (detected) and FALSE for NCL */
         (wl1_is_serv_freq_considered_barred(acq_entry.freq, acq_entry.scr_code))
        )
      {
        if(rrcrce_is_rce_procedure_inactive() && (rrc_get_state() == RRC_STATE_DISCONNECTED))
        {
          WRRC_MSG0_HIGH("Serving Freq Barred. Update CCM with cell loss");
          rrc_ccm_update_with_cell_loss();
        }
      }	
    }
    else
    {
      MSG_HIGH("Cell/Freq Bar Req to be sent during CSP state %d for freq %d, scr_code %d", 
                                          curr_csp_substate, rrc_csp_int_data.curr_acq_entry.freq, 
                                          rrc_csp_int_data.curr_acq_entry.scr_code);

      acq_entry.freq     = rrc_csp_int_data.curr_acq_entry.freq;
      acq_entry.scr_code = rrc_csp_int_data.curr_acq_entry.scr_code;

      if(rrcrce_is_rce_procedure_inactive() && (rrc_get_state() == RRC_STATE_DISCONNECTED) && (t_barred != 0))
      {
        WRRC_MSG0_HIGH("Serving PSC or Freq Barred. Update CCM with cell loss");
        rrc_ccm_update_with_cell_loss();
      }
    }
  }
  rrc_csp_send_l1_cmd(RRC_PROCEDURE_CSP, CPHY_CELL_BAR_REQ, &acq_entry);
  MSG_MED("L1 cmd 0x%x sent", CPHY_CELL_BAR_REQ, 0, 0);
} /* rrccsp_send_cphy_cell_bar_req */

/*===========================================================================

FUNCTION          RRC_CSP_SEND_L1_FREQ_SCAN_CMD

DESCRIPTION       This function sends a CPHY_FREQ_SCAN_REQ to L1 with a scan
                  type as passed to it in the parameter. 
           
DEPENDENCIES
                  None
RETURN VALUE
                  SUCCESS/FAILURE

SIDE EFFECTS
                  None

===========================================================================*/
static void rrc_csp_send_l1_freq_scan_cmd
(
 rrc_proc_e_type            proc_id,
 rrc_freq_type              freq,
 rrc_csp_freq_scan_e_type   type_freq_scan
)
{
  rrc_cmd_type  *csp_out_cmd_ptr;

  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    l1_req_cmd * csp_cmd_l1_req = NULL;
    RRC_GET_POINTER_L1_REQ_VALUE(csp_out_cmd_ptr->cmd.rrc_llc_req.l1_req,csp_cmd_l1_req);

    /* Initialize the LLC command header first */
    csp_out_cmd_ptr->cmd_hdr.cmd_id= RRC_LLC_CMD_REQ;
#ifdef FEATURE_DUAL_SIM
    csp_out_cmd_ptr->cmd.rrc_llc_req.l1_cmd.sys_modem_as_id = rrc_get_as_id();
#endif
    csp_out_cmd_ptr->cmd.rrc_llc_req.procedure = proc_id;
    csp_out_cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;

    /* Now fill the L1 cmd header */
    csp_out_cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_FREQ_SCAN_REQ; 
    csp_out_cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = 
                                                L1_ACTIVATION_TIME_NONE;

#ifdef FEATURE_EOOS
    #error code not present
#endif /* FEATURE_EOOS */

    /* Populate the cmd data */
    if(type_freq_scan == RRC_CSP_RAW_SCAN)
    {
      csp_cmd_l1_req->freq_scan.step
                                            = L1_FREQ_RAW_SCAN;
      csp_cmd_l1_req->freq_scan.
        u.raw_scan_parm.start_ctr_freq      = freq;


  #ifdef FEATURE_WCDMA_MULTIBAND
      if(rrc_csp_curr_select_data.current_band == RRC_CSP_IMT2K_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_MAX;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_PCS_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_PCS_MAX;
      else if (rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_850_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_850_MAX;
      else if (rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_800_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_800_MAX;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1800_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_1800_MAX;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_900_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_900_MAX;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_1700_MAX;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_2100_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_1700_2100_MAX;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1500_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_1500_MAX;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_BC19_BAND)
        csp_cmd_l1_req->freq_scan.
          u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_EXTENDED_800_MAX;
      else
      {
        WRRC_MSG0_ERROR("ERR: Frequency band not supported");
      }
  #else
      csp_cmd_l1_req->freq_scan.
        u.raw_scan_parm.end_ctr_freq        = WCDMA_FS_RX_CHAN_MAX;

  #endif /* FEATURE_WCDMA_MULTIBAND */


#ifdef FEATURE_EOOS
      #error code not present
#endif
    }
    else if(type_freq_scan == RRC_CSP_FINE_SCAN)
    {
      csp_cmd_l1_req->freq_scan.step = L1_FREQ_FINE_SCAN;
      csp_cmd_l1_req->freq_scan.
        u.fine_scan_parm.ctr_freq      =   freq;

#ifdef FEATURE_WCDMA_MULTIBAND
      /* Send the band information to L1 */
      if(rrc_csp_curr_select_data.current_band == RRC_CSP_IMT2K_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_IMT2K;

      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_PCS_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_PCS;

      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_850_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_WCDMA_850;

      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_800_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_WCDMA_800;

      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1800_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_1800;

      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_900_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_WCDMA_900;

      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_WCDMA_1700;

      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_2100_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_WCDMA_1700_2100;

      else if(rrc_csp_curr_select_data.current_band ==  RRC_CSP_WCDMA_1500_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_1500;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_BC19_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_EXTENDED_800; 
      else
      {
        WRRC_MSG1_ERROR("ERR Invalid current_band %d",
          rrc_csp_curr_select_data.current_band);
      }

#else /* FEATURE_WCDMA_MULTIBAND */


    /* 1900 band */
    csp_cmd_l1_req->freq_scan.band = L1_FREQ_WCDMA_BAND_PCS;


#endif /* FEATURE_WCDMA_MULTIBAND */

#ifdef FEATURE_EOOS
      #error code not present
#endif

    } /* RRC_CSP_FINE_SCAN */

    else if(type_freq_scan == RRC_CSP_LIST_SCAN)
    {
      int i;
      /* Set the step mode for the list frequency scan in the L1 frequency scan command */
      csp_cmd_l1_req->freq_scan.step = L1_FREQ_LIST_SCAN;

      /* Set the agc lfs threshold from the CSP internal data  */
      csp_cmd_l1_req->freq_scan.agc = -106;

#ifdef FEATURE_DUAL_SIM
      /*For G2W TA mode increase threshold to -103dB*/
      if (WRM_G2W_TA_MODE_ACTIVE()) 
      {
        csp_cmd_l1_req->freq_scan.agc = -103;
      }
#endif
      csp_cmd_l1_req->freq_scan.srch= L1_SERVICE_SRCH_TYPE;
      if(rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
      {
        for(i=0;i<rrc_csp_int_data.num_freq_in_list_db;i++)
        {
          /* Configure the frequencies in the list for the list freq scan in the L1 frequency scan command */
          csp_cmd_l1_req->freq_scan.u.list_scan_parm.freq[i] = 
               list_db[i].freq;
          
          /* Set the csp scan type*/
          csp_cmd_l1_req->freq_scan.u.list_scan_parm.plmn_type[i] = 
                (uint8) RRC_CSP_SPEC_PLMN;
          /* Set the number of frequencies in list scan in the L1 frequency scan command */
          csp_cmd_l1_req->freq_scan.num_freq = 
            rrc_csp_int_data.num_freq_in_list_db;
        }
      }
      else
      {
        for(i=0;i<rrc_csp_curr_select_data.scan_info.list_scan.num_freq;i++)
        {
          /* Configure the frequencies in the list for the list freq scan in the L1 frequency scan command */
          csp_cmd_l1_req->freq_scan.u.list_scan_parm.freq[i] = 
               rrc_csp_curr_select_data.scan_info.list_scan.freq_result[i].ctr_freq;
          
          /* Set the csp scan type*/
          csp_cmd_l1_req->freq_scan.u.list_scan_parm.plmn_type[i] = 
                rrc_csp_curr_select_data.scan_info.list_scan.freq_result[i].result_plmn_type;
        }
        /* Set the number of frequencies in list scan in the L1 frequency scan command */
        csp_cmd_l1_req->freq_scan.num_freq = 
          rrc_csp_curr_select_data.scan_info.list_scan.num_freq;
      }
    }  /* Got buffer to send cmd */


#ifdef FEATURE_WCDMA_MULTIBAND
    else if(type_freq_scan == RRC_CSP_ADDL_CHLS_SCAN)
    {
      csp_cmd_l1_req->freq_scan.step = 
        L1_FREQ_ADDITIONAL_CHANNEL_SCAN;

      if(rrc_csp_curr_select_data.current_band == RRC_CSP_PCS_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_PCS;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_850_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_WCDMA_850;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_800_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_WCDMA_800;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_2100_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_WCDMA_1700_2100;
      else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_BC19_BAND)
        csp_cmd_l1_req->freq_scan.band = 
          L1_FREQ_WCDMA_BAND_EXTENDED_800;

      else
      {
        WRRC_MSG1_ERROR("ERR: No addl chls scan required for band %d",
              rrc_csp_curr_select_data.current_band);
      }

#ifdef FEATURE_EOOS
      #error code not present
#endif
    }
#endif /* FEATURE_WCDMA_MUTLIBAND */
    else
    {
      WRRC_MSG1_ERROR("ERR: Freq scan type %d not handled here",type_freq_scan);
    }

    /* Indicate the srch type requested */
    if((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)    || 
       (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN) || 
       (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {
      csp_cmd_l1_req->freq_scan.srch = L1_BPLMN_SRCH_TYPE;
    }
    else if(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
    {
      csp_cmd_l1_req->freq_scan.srch = L1_MPLMN_LIST_SRCH_TYPE;
    }
    else
    {
      csp_cmd_l1_req->freq_scan.srch = L1_SERVICE_SRCH_TYPE;
    }
    /* Send the Internal RRC Command - LLC should directly handle it */
    rrc_put_int_cmd( csp_out_cmd_ptr );
  } /* Got buffer to send cmd */
} /* rrc_csp_send_l1_freq_scan_cmd */

/*===========================================================================

FUNCTION          RRC_CSP_FILL_TCRMAX_PARAMS

DESCRIPTION       This function is responsible for filling up the tcr max pameters
                   for both HCS and non HCS  

DEPENDENCIES
                  The passed pointer should be pointing to a valid data.

RETURN VALUE
                  Function returns filled ncr, tcrmaxhyst and tcrmax.

SIDE EFFECTS
                  None
===========================================================================*/
static void rrc_csp_fill_tcrmax_params
(
  rrc_T_CRMax *t_CR_Max_ptr,
  l1_t_cr_max_enum_type   *t_crmax_ptr,
  uint16      *n_cr_ptr,
  l1_t_cr_max_hyst_enum_type   *t_crmaxhyst_ptr
)
{
  switch(t_CR_Max_ptr->t)
  {
    case T_rrc_T_CRMax_t30:
      
      (*t_crmax_ptr) = L1_T_CR_MAX_30S;

      (*n_cr_ptr) = (uint16) t_CR_Max_ptr->u.t30->n_CR;
    
          
      (*t_crmaxhyst_ptr) = RRCCSP_RET_HCS_TCR_MAX_HYST(t_CR_Max_ptr->u.t30->t_CRMaxHyst);        
                
      break;
    
    case T_rrc_T_CRMax_t60:
      
      (*t_crmax_ptr) =  L1_T_CR_MAX_60S;

      (*n_cr_ptr) = (uint16)  t_CR_Max_ptr->u.t60->n_CR;
      
          
      (*t_crmaxhyst_ptr) = RRCCSP_RET_HCS_TCR_MAX_HYST(t_CR_Max_ptr->u.t60->t_CRMaxHyst);        
              
      break;

    case T_rrc_T_CRMax_t120:
      
      (*t_crmax_ptr) = L1_T_CR_MAX_120S;

      (*n_cr_ptr) = (uint16) t_CR_Max_ptr->u.t120->n_CR;
      
          
      (*t_crmaxhyst_ptr) = RRCCSP_RET_HCS_TCR_MAX_HYST(t_CR_Max_ptr->u.t120->t_CRMaxHyst);        
              
      break;

    case T_rrc_T_CRMax_t180:
      
      (*t_crmax_ptr) = L1_T_CR_MAX_180S;

      (*n_cr_ptr) = (uint16) t_CR_Max_ptr->u.t180->n_CR;
      
          
      (*t_crmaxhyst_ptr) = RRCCSP_RET_HCS_TCR_MAX_HYST(t_CR_Max_ptr->u.t180->t_CRMaxHyst);        
              
      break;

    case T_rrc_T_CRMax_t240:
      
      (*t_crmax_ptr) = L1_T_CR_MAX_240S;

      (*n_cr_ptr) = (uint16)t_CR_Max_ptr->u.t240->n_CR;
      
          
      (*t_crmaxhyst_ptr) = RRCCSP_RET_HCS_TCR_MAX_HYST(t_CR_Max_ptr->u.t240->t_CRMaxHyst);        
              
      break;  

    default:
      /* should not happen */    
      break;
  }
}
/*===========================================================================

FUNCTION          RRC_CSP_SEND_L1_CMD

DESCRIPTION       This function is responsible for filling up data structures
                  for all L1 commands sent by CSP. This also allocates cmd
                  buffer and sends the command.
                  Since all L1 cmds go via LLC, the LLC command RRC_LLC_CMD_REQ
                  actaully encapsules the real L1 command. This is the LLC
                  command funneling mechanism.
                  It frees allocated command and embedded buffers
                  when failure is detected before sending the command.

DEPENDENCIES
                  The passed pointer should be pointing to a valid data.

RETURN VALUE
                  SUCCESS: Command was successfully sent to L1.
                  FAILURE: Command could not be sent due to any one of multiple
                           reasons.

SIDE EFFECTS
                  None
===========================================================================*/
void
rrc_csp_send_l1_cmd(rrc_proc_e_type  proc_id,
                    l1_cmd_enum_type l1_cmd_id,
                    rrc_csp_acq_entry_type  *acq_entry_ptr)
{
  rrc_cmd_type        *csp_out_cmd_ptr;
  rrc_SysInfoType3    *sib3_ptr;
  rrc_SysInfoType4    *sib4_ptr;
  rrc_CellSelectReselectInfoSIB_3_4  *sib3_4_sel_resel_info_ptr;
  rrc_SysInfoType11   *sib11_ptr;
  rrc_SysInfoType12   *sib12_ptr;
  rrc_RAT_FDD_InfoList *rat_list_ptr;
  rrc_state_e_type    rrc_state;
  uint16 i=0;
  rrc_plmn_identity_type  plmn_id;

 rrc_CellSelectReselectInfo_v590ext *sib3_4_sel_resel_ext_ptr = NULL;
 rrc_CellSelectReselectInfoPCHFACH_v5b0ext *sib4_pch_fach_resel_ext_ptr = NULL;
 rrc_CellSelectReselectInfoTreselectionScaling_v5c0ext *sib3_4_sel_resel_v5c0ext_ptr = NULL;

  /* Check validity of Acq Entry pointer */
  if(acq_entry_ptr == NULL)
  {
    ERR_FATAL("ERR: Passed Ptr Union has NULL pointers", 0, 0, 0);
  }

  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    l1_req_cmd * csp_cmd_l1_req = NULL;
    RRC_GET_POINTER_L1_REQ_VALUE(csp_out_cmd_ptr->cmd.rrc_llc_req.l1_req,csp_cmd_l1_req);
    /* Initialize the LLC command header first */
    csp_out_cmd_ptr->cmd_hdr.cmd_id= RRC_LLC_CMD_REQ;
#ifdef FEATURE_DUAL_SIM
    csp_out_cmd_ptr->cmd.rrc_llc_req.l1_cmd.sys_modem_as_id = rrc_get_as_id();
#endif
    csp_out_cmd_ptr->cmd.rrc_llc_req.procedure = proc_id;
    csp_out_cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;

    /* Now fill the L1 cmd header */
    csp_out_cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = l1_cmd_id; 
    csp_out_cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = 
                                                L1_ACTIVATION_TIME_NONE;
   
     /* Fill up the L1 cmd data */
    switch(l1_cmd_id)
    {
      case CPHY_ACQ_REQ:
        /* Populate the cmd data */
        csp_cmd_l1_req->acq.acq_type =
          rrc_csp_int_data.acq_type;
          
        csp_cmd_l1_req->acq.acq_mode =
          rrc_csp_int_data.acq_mode;

        csp_cmd_l1_req->acq.freq     = acq_entry_ptr->freq;

        if((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN) ||
           (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
        {
          csp_cmd_l1_req->acq.srch = L1_BPLMN_SRCH_TYPE;
        }
#ifdef FEATURE_LTE_TO_WCDMA
        else if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN) &&
                (rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE_CGI))
        {
          csp_cmd_l1_req->acq.srch = L1_BPLMN_SRCH_TYPE;
        }
#endif
        else if((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) ||
                (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
#ifdef FEATURE_FEMTO_CSG
             /*Set search type to ABPLMN so that wl1 will consider 6 peaks for service search */
             || ((rrc_get_gcf_flag_status())&&(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN))
#endif
               )
        {
          csp_cmd_l1_req->acq.srch = L1_ABPLMN_SRCH_TYPE;
        }		
        else if(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
        {
          csp_cmd_l1_req->acq.srch = L1_MPLMN_LIST_SRCH_TYPE;
        } 
        else
        {
          csp_cmd_l1_req->acq.srch = L1_SERVICE_SRCH_TYPE;
        } 

        if(((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) ||
            (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN) ||
            (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN) ||
            (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
           ) &&
           (rrc_csp_int_data.bplmn_acq_succ_freq == acq_entry_ptr->freq) &&
           (rrc_csp_int_data.bplmn_acq_succ_scr_code != 8192)
          )
        {
          csp_cmd_l1_req->acq.acq_mode = L1_WCDMA_FULL;
          rrc_csp_int_data.acq_mode = L1_WCDMA_FULL;
          csp_cmd_l1_req->acq.acq_type = L1_WCDMA_FREQ_SCR_POS;
          rrc_csp_int_data.acq_type = L1_WCDMA_FREQ_SCR_POS;
          csp_cmd_l1_req->acq.num_rl = 1;
          csp_cmd_l1_req->acq.scr_code[0] = rrc_csp_int_data.bplmn_acq_succ_scr_code;
          csp_cmd_l1_req->acq.pn_pos = rrc_csp_int_data.bplmn_acq_succ_pn_pos;

          rrc_csp_int_data.bplmn_acq_succ_scr_code = 8192;
          rrc_csp_int_data.bplmn_acq_succ_freq = 0;
          rrc_csp_int_data.bplmn_acq_succ_pn_pos = 0xFFFFFFFF;


          WRRC_MSG2_HIGH("XTOW_BPLMN:Setting scr_code to %d, PN_POS to %d", csp_cmd_l1_req->acq.scr_code[0],
                    csp_cmd_l1_req->acq.pn_pos);
        }
        else 
#ifdef FEATURE_LTE_TO_WCDMA
        if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN) &&
           (rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE_CGI) &&
           (rrc_csp_int_data.bplmn_acq_succ_freq == acq_entry_ptr->freq) &&
           (rrc_csp_int_data.bplmn_acq_succ_scr_code != 8192))
        {
          csp_cmd_l1_req->acq.acq_type = L1_WCDMA_FREQ_SCR_POS;
          rrc_csp_int_data.acq_type = L1_WCDMA_FREQ_SCR_POS;
          csp_cmd_l1_req->acq.num_rl = 1;
          csp_cmd_l1_req->acq.scr_code[0] = rrc_csp_int_data.bplmn_acq_succ_scr_code;
          csp_cmd_l1_req->acq.pn_pos = rrc_csp_int_data.bplmn_acq_succ_pn_pos;
        
          rrc_csp_int_data.bplmn_acq_succ_scr_code = 8192;
          rrc_csp_int_data.bplmn_acq_succ_freq = 0;
          rrc_csp_int_data.bplmn_acq_succ_pn_pos = 0xFFFFFFFF;

          WRRC_MSG2_HIGH("CGI:Setting scr_code to %d, PN_POS to %d", csp_cmd_l1_req->acq.scr_code[0],
                    csp_cmd_l1_req->acq.pn_pos);
        }
        else 
#endif

        if(rrc_csp_int_data.acq_type == L1_WCDMA_FREQ_SCR_POS)
        {
          csp_cmd_l1_req->acq.num_rl = 1;
          csp_cmd_l1_req->acq.scr_code[0] = acq_entry_ptr->scr_code;
          csp_cmd_l1_req->acq.pn_pos = 
            rrc_csp_int_data.pn_pos;
        }
        else if(rrc_csp_int_data.acq_type == L1_WCDMA_FREQ_SCR)
        {
          if(rrc_csp_int_data.acq_mode == L1_WCDMA_INTER_F_REDIRECTION_ACQ)
          {
            csp_cmd_l1_req->acq.num_rl = 1;
            csp_cmd_l1_req->acq.scr_code[0] = 16*rrc_csp_int_data.dest_psc;
            WRRC_MSG1_HIGH("Setting scr_code to %d", csp_cmd_l1_req->acq.scr_code[0]);
          }
          else
          {

            csp_cmd_l1_req->acq.num_rl = 1;
            csp_cmd_l1_req->acq.scr_code[0] = acq_entry_ptr->scr_code;
          }
        }
        else
        {
          /* Do nothing frequency has already been set */
        }

        /* This was added because of a request from carriers - if the PLMNs and the 
         * frequencies match those of the carrier then UE should never apply AGC threshold
         */
        if(     (rrc_csp_int_data.ue_in_deep_sleep)
             && (   (acq_entry_ptr->freq == DEEP_SLEEP_NO_AGC_THRESHOLD_FREQ1)
                 || (acq_entry_ptr->freq == DEEP_SLEEP_NO_AGC_THRESHOLD_FREQ2)
                 || (acq_entry_ptr->freq == DEEP_SLEEP_NO_AGC_THRESHOLD_FREQ3)
                )
             && (   (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
                 || (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
                 #ifdef FEATURE_FEMTO_CSG
                 || (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
	         #endif
                )
             && (RRC_CSP_GET_INT_MCC(rrc_csp_int_data.curr_req_plmn.plmn_id) 
                                                          == DEEP_SLEEP_NO_AGC_THRESHOLD_MCC1)
             && (RRC_CSP_GET_INT_MNC(rrc_csp_int_data.curr_req_plmn.plmn_id) 
                                                          == DEEP_SLEEP_NO_AGC_THRESHOLD_MNC1)
          )
        {
          MSG_HIGH("freq %d. MCC %d, MNC %d. Do NOT apply threshold", 
                                            acq_entry_ptr->freq,
                                            RRC_CSP_GET_INT_MCC(rrc_csp_int_data.curr_req_plmn.plmn_id),
                                            RRC_CSP_GET_INT_MNC(rrc_csp_int_data.curr_req_plmn.plmn_id));

          csp_cmd_l1_req->acq.apply_agc_thresh = FALSE;
        }
        else
        {


#ifdef FEATURE_EOOS
        #error code not present
#endif /* FEATURE_EOOS */

        if((rrc_csp_int_data.ue_in_deep_sleep) 
           && ( (rrc_csp_int_data.agc_threshold_cnt) 
                || ((!rrc_csp_curr_select_data.acq_db_in_use)
                   && (rrc_csp_curr_select_data.scan_info.type_freq_scan != RRC_CSP_LIST_SCAN)
                   )
              )
          )
        {
          csp_cmd_l1_req->acq.apply_agc_thresh = TRUE;
#ifdef FEATURE_EOOS
          #error code not present
#endif
        }
        else
        {
          csp_cmd_l1_req->acq.apply_agc_thresh = FALSE;
        }
     }


        csp_cmd_l1_req->acq.sttd_indicator_present =
          rrc_csp_int_data.sttd_indicator_present;

        csp_cmd_l1_req->acq.sttd_indicator =
          rrc_csp_int_data.sttd_indicator;

        /* If n/w sel mode is SYS_NETWORK_SELECTION_MODE_LIMITED then do not consider 
         * T-barred timer started for a frequency. N/w sel mode as SYS_NETWORK_SELECTION_MODE_LIMITED
         * implies that UE is searching for service for Emergency Call only (or UE does not
         * have a USIM) and will not attempt registration on the camped cell.
         */
        if((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
            && (rrc_csp_int_data.service_req_cause == RRC_MO_CS_CALL))
        {
          csp_cmd_l1_req->acq.unbar_freqs = TRUE;
          WRRC_MSG1_HIGH("Set unbar_freqs=%d",csp_cmd_l1_req->acq.unbar_freqs);
        }
        else
        {
          csp_cmd_l1_req->acq.unbar_freqs = FALSE;
          WRRC_MSG1_HIGH("Set unbar_freqs=%d",csp_cmd_l1_req->acq.unbar_freqs);
        }
#ifdef FEATURE_DUAL_SIM
          csp_cmd_l1_req->acq.do_acq_split = rrc_csp_int_data.shld_acq_be_spltd;
#endif
        break;

      case CPHY_CELL_PARM_REQ:

        rrc_state = rrc_get_state();
        WRRC_MSG1_HIGH("RRC STATE = %d", rrc_state);
        /*  Derive the target state based on the scan, so that correct state specific parameters will be used*/
        if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) ||
            (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN))
        {
          rrc_state = RRC_STATE_CELL_FACH;
        }
        else if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) ||
                  (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN))
        {
          rrc_state = RRC_STATE_CELL_PCH;
        }
        else if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN) ||
                  (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN))
        {
          rrc_state = RRC_STATE_URA_PCH;
        }
        else if (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
        {
          if((rrc_state == RRC_STATE_CELL_DCH) && (rrc_csp_int_data.dch_tx_dest_state != RRC_STATE_MAX))
          {
              rrc_state = rrc_csp_int_data.dch_tx_dest_state;
          }
          else if((rrc_state == RRC_STATE_CELL_FACH) && (rrc_csp_int_data.fach_tx_dest_state != RRC_STATE_MAX))
          {
              rrc_state = rrc_csp_int_data.fach_tx_dest_state;
          }
        }

        WRRC_MSG1_HIGH("RRC Target STATE = %d", rrc_state);
#ifdef FEATURE_FEMTO_CSG
        if(acq_entry_ptr->csg_id != SYS_CSG_ID_INVALID)
        {
          /* Though the cell broadcasts a CSG ID, for a hybrid cell to be considered as a CSG, 
             CSG indicator is set to FALSE and CSG ID should be present in whitelist  */ 
          if((rrc_csp_int_data.curr_acq_entry.cell_is_hybrid) && (!rrccsp_is_csg_white(acq_entry_ptr->csg_id,acq_entry_ptr->plmn_id)))
          {
            csp_cmd_l1_req->cell_parm.is_camped_cell_femto = FALSE;
          }
          else
          {
            csp_cmd_l1_req->cell_parm.is_camped_cell_femto = TRUE;
          }
        }
        else
        {
          csp_cmd_l1_req->cell_parm.is_camped_cell_femto = FALSE;
        }
        if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
        {
          csp_cmd_l1_req->cell_parm.is_manual_csg_mode = TRUE;
        }
        WRRC_MSG2_HIGH("CSG: Femto status in cell parm request to l1 %d manual csg mode: %d",csp_cmd_l1_req->cell_parm.is_camped_cell_femto,csp_cmd_l1_req->cell_parm.is_manual_csg_mode);
#endif
        /* PLMN and Cell ID of the current camped on cell are known. Get SIB3 */
        if(acq_entry_ptr->is_valid_nw_sharing)
        {
          plmn_id = acq_entry_ptr->shared_list_of_plmns.common_plmn_id;
        }
        else
        {
          plmn_id = acq_entry_ptr->plmn_id;
        }
        sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_cell(
                                             rrc_SIB3, 
                                               plmn_id, 
                                               acq_entry_ptr->cell_id);
        sib11_ptr = (rrc_SysInfoType11*) rrc_sibdb_return_sib_for_cell(
                                               rrc_SIB11, 
                                               plmn_id, 
                                             acq_entry_ptr->cell_id);

        sib12_ptr = (rrc_SysInfoType12*) rrc_sibdb_return_sib_for_cell(
                                               rrc_SIB12, 
                                               plmn_id, 
                                               acq_entry_ptr->cell_id);
        if( sib3_ptr == NULL )
        {
          /* SIB3 for this PLMN, Cell ID combination not present. 
           */
          ERR_FATAL("SIB3 for MCC %d, MNC %d, Cell ID %d not present", 
                       RRC_CSP_GET_INT_MCC(plmn_id),
                       RRC_CSP_GET_INT_MNC(plmn_id), 
                       acq_entry_ptr->cell_id);

          /* Call the RRC function to release the Internal Buffer */
          rrc_free_cmd_buf(csp_out_cmd_ptr);
          csp_out_cmd_ptr = NULL;
        }

        if((rrc_state != RRC_STATE_DISCONNECTED) && (rrc_state != RRC_STATE_CONNECTING))
        {
          if(sib3_ptr->sib4indicator == TRUE)
          {
            sib4_ptr = (rrc_SysInfoType4*) rrc_sibdb_return_sib_for_cell(
                                               rrc_SIB4, 
                                               plmn_id, 
                                               acq_entry_ptr->cell_id);
            if(sib4_ptr == NULL)
            {
              WRRC_MSG0_HIGH("SIB4 indicated but not present, using SIB3");
              sib3_4_sel_resel_info_ptr = &(sib3_ptr->cellSelectReselectInfo);
              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
              {            
                if((sib3_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                  (sib3_ptr->v4b0NonCriticalExtensions.m. 
                   v590NonCriticalExtensionPresent) &&
                  (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.sysInfoType3_v590ext.m. 
                   cellSelectReselectInfo_v590extPresent))        
                {
                  sib3_4_sel_resel_ext_ptr = &(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                    sysInfoType3_v590ext.cellSelectReselectInfo_v590ext);
                }

                if((sib3_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                  (sib3_ptr->v4b0NonCriticalExtensions.m.
                   v590NonCriticalExtensionPresent) &&
                   (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.
                   v5c0NoncriticalExtensionPresent) &&
                   (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension
                    .sysInfoType3_v5c0ext.m.
                    cellSelectReselectInfoTreselectionScaling_v5c0extPresent))
                {
                  sib3_4_sel_resel_v5c0ext_ptr = &(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                   v5c0NoncriticalExtension.sysInfoType3_v5c0ext.
                   cellSelectReselectInfoTreselectionScaling_v5c0ext);
                }
              }
            }
            else
            {
              sib3_4_sel_resel_info_ptr = &(sib4_ptr->cellSelectReselectInfo);
              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
              {
                if((sib4_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                  (sib4_ptr->v4b0NonCriticalExtensions.m.
                   v590NonCriticalExtensionPresent) &&
                  (sib4_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.sysInfoType4_v590ext.m.
                   cellSelectReselectInfo_v590extPresent))        
                {
                  sib3_4_sel_resel_ext_ptr = &(sib4_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                    sysInfoType4_v590ext.cellSelectReselectInfo_v590ext);
                }

                if((sib4_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                  (sib4_ptr->v4b0NonCriticalExtensions.m.
                   v590NonCriticalExtensionPresent) &&
                  (sib4_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.
                   v5b0NonCriticalExtensionPresent))
                {
                  if(sib4_ptr->v4b0NonCriticalExtensions.
                   v590NonCriticalExtension.v5b0NonCriticalExtension.sysInfoType4_v5b0ext.m.
                    cellSelectReselectInfoPCHFACH_v5b0extPresent)
                  {
                    sib4_pch_fach_resel_ext_ptr = &(sib4_ptr->v4b0NonCriticalExtensions.
                      v590NonCriticalExtension.v5b0NonCriticalExtension.sysInfoType4_v5b0ext.
                      cellSelectReselectInfoPCHFACH_v5b0ext);
                  }

                  if((sib4_ptr->v4b0NonCriticalExtensions.
                   v590NonCriticalExtension.v5b0NonCriticalExtension.m.v5c0NonCriticalExtensionPresent)
                   && (sib4_ptr->v4b0NonCriticalExtensions.
                   v590NonCriticalExtension.v5b0NonCriticalExtension.v5c0NonCriticalExtension.
                   sysInfoType4_v5c0ext.m.cellSelectReselectInfoTreselectionScaling_v5c0extPresent))
                  {
                    sib3_4_sel_resel_v5c0ext_ptr = &(sib4_ptr->v4b0NonCriticalExtensions.
                     v590NonCriticalExtension.v5b0NonCriticalExtension.v5c0NonCriticalExtension.
                     sysInfoType4_v5c0ext.cellSelectReselectInfoTreselectionScaling_v5c0ext);
                  }
                }
              }
            }
          }
          else
          {
            sib3_4_sel_resel_info_ptr = &(sib3_ptr->cellSelectReselectInfo);
            if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
            {
              if((sib3_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                (sib3_ptr->v4b0NonCriticalExtensions.m. 
                 v590NonCriticalExtensionPresent) &&
                (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.sysInfoType3_v590ext.m.
                 cellSelectReselectInfo_v590extPresent))        
              {
                sib3_4_sel_resel_ext_ptr = &(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                  sysInfoType3_v590ext.cellSelectReselectInfo_v590ext);
              }
              if((sib3_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                  (sib3_ptr->v4b0NonCriticalExtensions.m.
                   v590NonCriticalExtensionPresent) &&
                   (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.
                   v5c0NoncriticalExtensionPresent) &&
                   (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension
                    .sysInfoType3_v5c0ext.m.
                    cellSelectReselectInfoTreselectionScaling_v5c0extPresent))
              {
                sib3_4_sel_resel_v5c0ext_ptr = &(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                   v5c0NoncriticalExtension.sysInfoType3_v5c0ext.
                   cellSelectReselectInfoTreselectionScaling_v5c0ext);
              }
            }
          }
        }
        else
        {
          sib3_4_sel_resel_info_ptr = &(sib3_ptr->cellSelectReselectInfo);
          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
          {
            if((sib3_ptr->m.v4b0NonCriticalExtensionsPresent) &&
              (sib3_ptr->v4b0NonCriticalExtensions.m.
               v590NonCriticalExtensionPresent) &&
              (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.sysInfoType3_v590ext.m.
               cellSelectReselectInfo_v590extPresent))        
            {
              sib3_4_sel_resel_ext_ptr = &(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                sysInfoType3_v590ext.cellSelectReselectInfo_v590ext);
            }
            if((sib3_ptr->m.v4b0NonCriticalExtensionsPresent) &&
                  (sib3_ptr->v4b0NonCriticalExtensions.m. 
                   v590NonCriticalExtensionPresent) &&
                   (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.
                   v5c0NoncriticalExtensionPresent) &&
                   (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension
                    .sysInfoType3_v5c0ext.m.
                    cellSelectReselectInfoTreselectionScaling_v5c0extPresent))
            {
              sib3_4_sel_resel_v5c0ext_ptr = &(sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                   v5c0NoncriticalExtension.sysInfoType3_v5c0ext.
                   cellSelectReselectInfoTreselectionScaling_v5c0ext);
            }
          }
        }

        /* Set Mapping Info to be not present by default */
        csp_cmd_l1_req->cell_parm.map_info_included = FALSE;

        /* Fill in Cell Select Quality Measure specific fields */
        if(sib11_ptr != NULL)
        {
          if(((rrc_state != RRC_STATE_DISCONNECTED) && (rrc_state != RRC_STATE_CONNECTING)) && (sib12_ptr != NULL))
          {
            /* Use cell select quality measure from SIB12 (if present) */
            if(RRC_CHECK_COMMON_MSG_TYPE(
                sib12_ptr->measurementControlSysInfo.use_of_HCS,
                rrc_MeasurementControlSysInfo_use_of_HCS_hcs_not_used))
            {
              switch(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
                       cellSelectQualityMeasure.t)
              {
                case T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_cpich_RSCP:
                  csp_cmd_l1_req->cell_parm.quan_type = L1_RSCP;
                  break;
                case T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_cpich_Ec_N0:
                  csp_cmd_l1_req->cell_parm.quan_type = L1_EC_NO;
                  break;
                default:
                  WRRC_MSG1_ERROR("ERR: Invalid value %d of Cel Sel Qual Meas in SIB", 
                              sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
                              cellSelectQualityMeasure.t);
                  break;
              }
            }
            else if(RRC_CHECK_COMMON_MSG_TYPE(sib12_ptr->measurementControlSysInfo.use_of_HCS,
                rrc_MeasurementControlSysInfo_use_of_HCS_hcs_used))
            {
              switch(sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
                       cellSelectQualityMeasure.t)
              {
                case T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_cpich_RSCP:
                  csp_cmd_l1_req->cell_parm.quan_type = L1_RSCP;
                  break;
                case T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_cpich_Ec_N0:
                  csp_cmd_l1_req->cell_parm.quan_type = L1_EC_NO;
                  break;
                default:
                  WRRC_MSG1_ERROR("Invalid value %d of Cel Sel Qual Meas in SIB", 
                              sib12_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
                              cellSelectQualityMeasure.t);
                  break;
              }
            }
            else
            {
              WRRC_MSG1_ERROR("Invalid value %d of use_of_HCS in SIB12", 
              sib12_ptr->measurementControlSysInfo.use_of_HCS.t);
            }
          }
          else
          {
            /* Use cell select quality measure from SIB11 */
            if(RRC_CHECK_COMMON_MSG_TYPE(sib11_ptr->measurementControlSysInfo.use_of_HCS,
                rrc_MeasurementControlSysInfo_use_of_HCS_hcs_not_used))
            {
              switch(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
                     cellSelectQualityMeasure.t)
              {
                case T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_cpich_RSCP:
                  csp_cmd_l1_req->cell_parm.quan_type = L1_RSCP;
                  break;
                case T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_cpich_Ec_N0:
                  csp_cmd_l1_req->cell_parm.quan_type = L1_EC_NO;
                  break;
                default:
                  WRRC_MSG1_ERROR("Invalid value %d of Cel Sel Qual Meas in SIB", 
                              sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_not_used->
                              cellSelectQualityMeasure.t);
                  break;
              }
            }
            else if(RRC_CHECK_COMMON_MSG_TYPE(sib11_ptr->measurementControlSysInfo.use_of_HCS,
                rrc_MeasurementControlSysInfo_use_of_HCS_hcs_used))
            {
              switch(sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
                     cellSelectQualityMeasure.t)
              {
                case T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_cpich_RSCP:
                  csp_cmd_l1_req->cell_parm.quan_type = L1_RSCP;
                  break;
                case T_rrc_MeasurementControlSysInfo_cellSelectQualityMeasure_cpich_Ec_N0:
                  csp_cmd_l1_req->cell_parm.quan_type = L1_EC_NO;
                  break;
                default:
                  WRRC_MSG1_ERROR("Invalid value %d of Cel Sel Qual Meas in SIB", 
                              sib11_ptr->measurementControlSysInfo.use_of_HCS.u.hcs_used->
                              cellSelectQualityMeasure.t);
                  break;
              }
            }
            else
            {
              WRRC_MSG1_ERROR("Invalid value %d of use_of_HCS in SIB11", 
              sib11_ptr->measurementControlSysInfo.use_of_HCS.t);
            }
          }
        } /* Fill in cell select quality from SIB11/SIB12 */
        else
        { /* Fill in the cell select quality measure from SIB3 */
          switch(sib3_4_sel_resel_info_ptr->cellSelectQualityMeasure.t)
          {
            case T_rrc_CellSelectReselectInfoSIB_3_4_cellSelectQualityMeasure_cpich_Ec_N0:
              csp_cmd_l1_req->cell_parm.quan_type = L1_EC_NO;
              break;
            case T_rrc_CellSelectReselectInfoSIB_3_4_cellSelectQualityMeasure_cpich_RSCP:
              csp_cmd_l1_req->cell_parm.quan_type = L1_RSCP;
              break;
            default:
              WRRC_MSG1_ERROR("Invalid value %d of Cel Sel Qual Meas in SIB", 
                          sib3_4_sel_resel_info_ptr->cellSelectQualityMeasure.t);
              break;
          }
        }

        /* Assign default value of q_Hyst_1_S to q_Hyst_2_S */
        csp_cmd_l1_req->cell_parm.qhyst2_s = 
            (uint16)(sib3_4_sel_resel_info_ptr->q_Hyst_l_S * 2);

        /* Fill in q_HYST_2_S if present in SIB3 */
        if(RRC_CHECK_COMMON_MSG_TYPE(sib3_4_sel_resel_info_ptr->cellSelectQualityMeasure,
             rrc_CellSelectReselectInfoSIB_3_4_cellSelectQualityMeasure_cpich_Ec_N0))
        {
          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_info_ptr->cellSelectQualityMeasure.u.
                     cpich_Ec_N0,q_HYST_2_S))
          {
            csp_cmd_l1_req->cell_parm.qhyst2_s = 
              (uint16)(sib3_4_sel_resel_info_ptr->cellSelectQualityMeasure.u.
              cpich_Ec_N0->q_HYST_2_S * 2);
          }
          if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) && (sib4_pch_fach_resel_ext_ptr != NULL))
          {
            if(((rrc_state == RRC_STATE_CELL_PCH) || (rrc_state == RRC_STATE_URA_PCH)) &&
              (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib4_pch_fach_resel_ext_ptr,q_Hyst_2_S_PCH)))
            {
              csp_cmd_l1_req->cell_parm.qhyst2_s =
                (uint16) sib4_pch_fach_resel_ext_ptr->q_Hyst_2_S_PCH;   
              WRRC_MSG1_HIGH("updated q_Hyst_2_S for PCH = %d",csp_cmd_l1_req->cell_parm.qhyst2_s); 
            }  
            else if((rrc_state == RRC_STATE_CELL_FACH) &&
              (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib4_pch_fach_resel_ext_ptr,q_Hyst_2_S_FACH)))
            {
              csp_cmd_l1_req->cell_parm.qhyst2_s =
                (uint16) sib4_pch_fach_resel_ext_ptr->q_Hyst_2_S_FACH;
              WRRC_MSG1_HIGH("updated q_Hyst_2_S for FACH = %d",csp_cmd_l1_req->cell_parm.qhyst2_s);
            }       
          }          
        }     

        /* Fill in FDD mode specific fields */
        if(RRC_CHECK_COMMON_MSG_TYPE(sib3_4_sel_resel_info_ptr->modeSpecificInfo,
                 rrc_CellSelectReselectInfoSIB_3_4_modeSpecificInfo_fdd))
        {
          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd,
                                        s_Intrasearch))
          {
            csp_cmd_l1_req->cell_parm.s_intrasrch_incl = TRUE;
            if(sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd->s_Intrasearch < 0)
            {
              csp_cmd_l1_req->cell_parm.s_intrasrch = 0;
            }
            else
            {
              csp_cmd_l1_req->cell_parm.s_intrasrch = 
              (int16)(sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd->
                     s_Intrasearch * 2);
            }
          }
          else
          {
            csp_cmd_l1_req->cell_parm.s_intrasrch_incl = FALSE;
          }

          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd,
                                         s_Intersearch))
          {
            csp_cmd_l1_req->cell_parm.s_intersrch_incl = TRUE;
            if(sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd->s_Intersearch < 0)
            {
              csp_cmd_l1_req->cell_parm.s_intersrch = 0;
            }
            else
            {
              csp_cmd_l1_req->cell_parm.s_intersrch = 
              (int16)(sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd->
                      s_Intersearch * 2);
            }
          }
          else
          {
            csp_cmd_l1_req->cell_parm.s_intersrch_incl = FALSE;
          }

          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd,
            s_SearchHCS))
          {
            csp_cmd_l1_req->cell_parm.s_srchhcs_incl = TRUE;
            if(sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd->s_SearchHCS < 0)
            {
              csp_cmd_l1_req->cell_parm.s_srchhcs = 0;
            }
            else
            {
              csp_cmd_l1_req->cell_parm.s_srchhcs = 
                (int16)((sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd->s_SearchHCS * 2) + 1);
            }
          }
          else
          {
            csp_cmd_l1_req->cell_parm.s_srchhcs_incl = FALSE;
          }

          /* Fill the Inter-RAT information here */
          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd,
            rat_List))
          {
            uint32 idx=0;
            csp_cmd_l1_req->cell_parm.s_interrat_incl = TRUE;
            rat_list_ptr = &sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd->rat_List;
            while(rat_list_ptr->n > idx)
            {
              switch(rat_list_ptr->elem[idx].rat_Identifier)
              {
                case rrc_RAT_Identifier_cdma2000: idx++;
                  continue;
                              
                case rrc_RAT_Identifier_gsm:
                  csp_cmd_l1_req->cell_parm.interrat_info[i].rat_type =
                    L1_GSM_SYSTEM;
            
                  if(rat_list_ptr->elem[idx].s_SearchRAT < 0)
                  {
                    csp_cmd_l1_req->cell_parm.interrat_info[i].s_interrat = 0;
                  }
                  else if(rat_list_ptr->elem[idx].s_SearchRAT == 10)
                  {
                    /* actual value is 20 which means treat the ie as absent hence set it to 24 so that
                     * This will ensure that Squal is always <= SsearchRAT and effectively make the UE
                     *  behave as if SsearchRAT is absent (min value of Qqualmin is -24). 
                     */      
                    csp_cmd_l1_req->cell_parm.interrat_info[i].s_interrat = 24; 
                  }
                  else
                  {
                    csp_cmd_l1_req->cell_parm.interrat_info[i].s_interrat =
                      (int16)(rat_list_ptr->elem[idx].s_SearchRAT * 2);
                  }
            
                  if(rat_list_ptr->elem[idx].s_Limit_SearchRAT < 0)
                  {
                    csp_cmd_l1_req->cell_parm.interrat_info[i].s_limit_srchrat = 0;
                  }
                  else
                  {
                    csp_cmd_l1_req->cell_parm.interrat_info[i].s_limit_srchrat =
                      (int16)(rat_list_ptr->elem[idx].s_Limit_SearchRAT * 2);
                  }
            
                  if(RRC_MSG_LIST_BITMASK_IE(rat_list_ptr,rrc_RAT_FDD_Info,s_HCS_RAT))
                  {
                    csp_cmd_l1_req->cell_parm.interrat_info[i].s_hcsrat_incl = TRUE;
                    if(rat_list_ptr->elem[idx].s_HCS_RAT < 0)
                    {
                      csp_cmd_l1_req->cell_parm.interrat_info[i].s_hcsrat = 0;
                    }
                    else
                    {
                      csp_cmd_l1_req->cell_parm.interrat_info[i].s_hcsrat =
                        (int16)((rat_list_ptr->elem[idx].s_HCS_RAT * 2) + 1);
                    }
                  }
                  else 
                  {
                    csp_cmd_l1_req->cell_parm.interrat_info[i].s_hcsrat_incl = FALSE;
                  }
                  idx++;
                  i++;

                  break;

                default: 
                    idx++;
                  break;
              }
            }
          }
          else
          {
            csp_cmd_l1_req->cell_parm.s_interrat_incl = FALSE;
          }

          /* Set the num_rats here */
          csp_cmd_l1_req->cell_parm.num_rats = i;

          csp_cmd_l1_req->cell_parm.q_qualmin = 
            (int16)sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd->q_QualMin;
          csp_cmd_l1_req->cell_parm.q_rxlevmin =
            (int16)((sib3_4_sel_resel_info_ptr->modeSpecificInfo.u.fdd->q_RxlevMin * 2) + 1);
          if((sib3_4_sel_resel_ext_ptr != NULL) && (
            RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_ext_ptr,
            deltaQrxlevmin)))
          {
            MSG_HIGH("Serving Delta-Qrxlev=%d Qrxlev=%d, Sum=%d",
              (sib3_4_sel_resel_ext_ptr->deltaQrxlevmin * 2),
              csp_cmd_l1_req->cell_parm.q_rxlevmin,
              (csp_cmd_l1_req->cell_parm.q_rxlevmin + 
               (sib3_4_sel_resel_ext_ptr->deltaQrxlevmin * 2)));

            /*if ((csp_cmd_l1_req->cell_parm.q_rxlevmin + 
                 (sib3_4_sel_resel_ext_ptr->deltaQrxlevmin * 2)) < -115)
            {*/
              csp_cmd_l1_req->cell_parm.q_rxlevmin +=
                (int16) (sib3_4_sel_resel_ext_ptr->deltaQrxlevmin * 2);          
            /*}*/
          }
        }

        /* Fill in generic fields */
        csp_cmd_l1_req->cell_parm.qhyst1_s =
          (uint16) (sib3_4_sel_resel_info_ptr->q_Hyst_l_S * 2);
        if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) && (sib4_pch_fach_resel_ext_ptr != NULL))
        {
          if(((rrc_state == RRC_STATE_CELL_PCH) || (rrc_state == RRC_STATE_URA_PCH)) &&
            (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib4_pch_fach_resel_ext_ptr,
            q_Hyst_l_S_PCH)))
          {
            csp_cmd_l1_req->cell_parm.qhyst1_s =
              (uint16) sib4_pch_fach_resel_ext_ptr->q_Hyst_l_S_PCH;  
            WRRC_MSG1_HIGH("updated q_Hyst_l_S for PCH = %d",csp_cmd_l1_req->cell_parm.qhyst1_s);      
          }  
          else if((rrc_state == RRC_STATE_CELL_FACH) &&
            (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib4_pch_fach_resel_ext_ptr,
            q_Hyst_l_S_FACH)))
          {
            csp_cmd_l1_req->cell_parm.qhyst1_s =
              (uint16) sib4_pch_fach_resel_ext_ptr->q_Hyst_l_S_FACH;
            WRRC_MSG1_HIGH("updated q_Hyst_l_S for FACH = %d",csp_cmd_l1_req->cell_parm.qhyst1_s);
          }
        } 
        csp_cmd_l1_req->cell_parm.trselc_ms =
          (uint16) sib3_4_sel_resel_info_ptr->t_Reselection_S * 1000;
        WRRC_MSG1_HIGH("t_Reselection_S in ms = %d",csp_cmd_l1_req->cell_parm.trselc_ms);
        if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) && (sib4_pch_fach_resel_ext_ptr != NULL))
        {
          if(((rrc_state == RRC_STATE_CELL_PCH) || (rrc_state == RRC_STATE_URA_PCH)) &&
            (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib4_pch_fach_resel_ext_ptr,
            t_Reselection_S_PCH)))
          {
            csp_cmd_l1_req->cell_parm.trselc_ms =
              (uint16) sib4_pch_fach_resel_ext_ptr->t_Reselection_S_PCH * 1000;   
            WRRC_MSG1_HIGH("updated t_Reselection_S in ms for PCH = %d",csp_cmd_l1_req->cell_parm.trselc_ms);     
          }  
          else if((rrc_state == RRC_STATE_CELL_FACH) &&
            (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib4_pch_fach_resel_ext_ptr,
            t_Reselection_S_FACH)))
          {
            csp_cmd_l1_req->cell_parm.trselc_ms =
              (uint16) ((sib4_pch_fach_resel_ext_ptr->t_Reselection_S_FACH * 0.2) * 1000);
            WRRC_MSG1_HIGH("updated t_Reselection_S in ms for FACH = %d",csp_cmd_l1_req->cell_parm.trselc_ms);
          }
        }
        
        /* HCS Processing Serving cell info assign default values if not present */
        csp_cmd_l1_req->cell_parm.hcs_info_incl = TRUE;
        csp_cmd_l1_req->cell_parm.hcs_info.hcs_prio = 0;
        
        if(csp_cmd_l1_req->cell_parm.quan_type == L1_RSCP)
        {
          csp_cmd_l1_req->cell_parm.hcs_info.q_hcs = 115;
          WRRC_MSG1_HIGH("Initialising serving cell qhcs to %d", csp_cmd_l1_req->cell_parm.hcs_info.q_hcs);
        }
        else
        {
          csp_cmd_l1_req->cell_parm.hcs_info.q_hcs = 24 * 2;
          WRRC_MSG1_HIGH("Initialising serving cell qhcs to %d", csp_cmd_l1_req->cell_parm.hcs_info.q_hcs);
        }
        
        csp_cmd_l1_req->cell_parm.hcs_info.n_cr = 8;
        csp_cmd_l1_req->cell_parm.hcs_info.t_crmax
          = L1_T_CR_MAX_NOT_USED;
        csp_cmd_l1_req->cell_parm.hcs_info.t_crmaxhyst
          = L1_T_CR_MAX_HYST_NOT_USED;        
        
        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_info_ptr,
            hcs_ServingCellInformation))
        {
         
          csp_cmd_l1_req->cell_parm.hcs_info.hcs_prio =
               (uint16)sib3_4_sel_resel_info_ptr->hcs_ServingCellInformation.hcs_PRIO;       
          
         
         
          if(csp_cmd_l1_req->cell_parm.quan_type == L1_RSCP)
          {
            if(sib3_4_sel_resel_info_ptr->hcs_ServingCellInformation.q_HCS < 90)
            {
              csp_cmd_l1_req->cell_parm.hcs_info.q_hcs =
                (uint16) (115 - sib3_4_sel_resel_info_ptr->hcs_ServingCellInformation.q_HCS);
              WRRC_MSG1_HIGH("RSCP serving cell qhcs = %d", csp_cmd_l1_req->cell_parm.hcs_info.q_hcs); 
            }
            else
            {
              csp_cmd_l1_req->cell_parm.hcs_info.q_hcs = MAX_QHCS_RSCP;
              WRRC_MSG1_HIGH("RSCP serving cell qhcs spare value set to %d",csp_cmd_l1_req->cell_parm.hcs_info.q_hcs);
            }
          }
          else
          {
            if(sib3_4_sel_resel_info_ptr->hcs_ServingCellInformation.q_HCS < 49)
            {
              csp_cmd_l1_req->cell_parm.hcs_info.q_hcs =
                (uint16) ((24 - (0.5 * sib3_4_sel_resel_info_ptr->hcs_ServingCellInformation.q_HCS)) * 2);
              WRRC_MSG1_HIGH("ECN0 serving cell qhcs = %d", csp_cmd_l1_req->cell_parm.hcs_info.q_hcs); 
            }
            else
            {
              csp_cmd_l1_req->cell_parm.hcs_info.q_hcs = 0; 
              WRRC_MSG1_HIGH("ECN0 serving cell qhcs spare value set to %d",csp_cmd_l1_req->cell_parm.hcs_info.q_hcs);
            }
          }
          if((csp_cmd_l1_req->cell_parm.quan_type == L1_RSCP) &&
             (sib3_4_sel_resel_ext_ptr != NULL) && 
             (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_ext_ptr,deltaQhcs)))
          {
            MSG_HIGH("Serving Delta-Qhcs=%d, Qhcs=%d, Sum=%d",
              sib3_4_sel_resel_ext_ptr->deltaQhcs,
              csp_cmd_l1_req->cell_parm.hcs_info.q_hcs,
              (csp_cmd_l1_req->cell_parm.hcs_info.q_hcs + 
               (-sib3_4_sel_resel_ext_ptr->deltaQhcs)));
 
           /*if ((csp_cmd_l1_req->cell_parm.hcs_info.q_hcs + 
                (-sib3_4_sel_resel_ext_ptr->deltaQhcs)) > 115)
           {*/
            csp_cmd_l1_req->cell_parm.hcs_info.q_hcs +=
              (uint16) (-sib3_4_sel_resel_ext_ptr->deltaQhcs);          
           /*}*/
          }
       
        
          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(sib3_4_sel_resel_info_ptr->hcs_ServingCellInformation,
             t_CR_Max))
          {
             rrc_csp_fill_tcrmax_params(&sib3_4_sel_resel_info_ptr->hcs_ServingCellInformation.t_CR_Max,
              &csp_cmd_l1_req->cell_parm.hcs_info.t_crmax,
              &csp_cmd_l1_req->cell_parm.hcs_info.n_cr,
              &csp_cmd_l1_req->cell_parm.hcs_info.t_crmaxhyst);
          } 
        }

        csp_cmd_l1_req->cell_parm.max_tx_pwr = 
          (int16)sib3_4_sel_resel_info_ptr->maxAllowedUL_TX_Power;       

        csp_cmd_l1_req->cell_parm.nhcs_n_cr = 8;
        csp_cmd_l1_req->cell_parm.nhcs_t_crmax
          = L1_T_CR_MAX_NOT_USED;
        csp_cmd_l1_req->cell_parm.nhcs_t_crmaxhyst
          = L1_T_CR_MAX_HYST_NOT_USED;
        csp_cmd_l1_req->cell_parm.tresel_scale = 10;
        csp_cmd_l1_req->cell_parm.tresel_interf_scale = 4;
        csp_cmd_l1_req->cell_parm.tresel_irat_scale = 4;

        if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) && (sib3_4_sel_resel_v5c0ext_ptr != NULL))
        {
          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_v5c0ext_ptr,speedDependentScalingFactor))
          {
            csp_cmd_l1_req->cell_parm.tresel_scale = 
              (uint16) sib3_4_sel_resel_v5c0ext_ptr->speedDependentScalingFactor;
          }

          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_v5c0ext_ptr,interFrequencyTreselectionScalingFactor))
          {
            csp_cmd_l1_req->cell_parm.tresel_interf_scale = 
              (uint16) sib3_4_sel_resel_v5c0ext_ptr->interFrequencyTreselectionScalingFactor;
          }

          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_v5c0ext_ptr,interRATTreselectionScalingFactor))
          {
            csp_cmd_l1_req->cell_parm.tresel_irat_scale =
              (uint16) sib3_4_sel_resel_v5c0ext_ptr->interRATTreselectionScalingFactor;
          }

          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sib3_4_sel_resel_v5c0ext_ptr,non_HCS_t_CR_Max))
          {
            rrc_csp_fill_tcrmax_params(&sib3_4_sel_resel_v5c0ext_ptr->non_HCS_t_CR_Max,
             &csp_cmd_l1_req->cell_parm.nhcs_t_crmax,
             &csp_cmd_l1_req->cell_parm.nhcs_n_cr,
             &csp_cmd_l1_req->cell_parm.nhcs_t_crmaxhyst);      
          }
        }
        MSG_HIGH("nhcs_t_crmax = %d, nhcs_n_cr = %d, nhcs_t_crmaxhyst = %d",
            csp_cmd_l1_req->cell_parm.nhcs_t_crmax,
            csp_cmd_l1_req->cell_parm.nhcs_n_cr,
            csp_cmd_l1_req->cell_parm.nhcs_t_crmaxhyst);

        MSG_HIGH("tresel_scale = %d, tresel_interf_scale = %d, tresel_irat_scale = %d",
            csp_cmd_l1_req->cell_parm.tresel_scale,
            csp_cmd_l1_req->cell_parm.tresel_interf_scale,
            csp_cmd_l1_req->cell_parm.tresel_irat_scale); 

        if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN) ||
           (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_CC_ORDER_SCAN))
        {
          rrc_csp_int_data.wcdma_cell_squalmin =
            csp_cmd_l1_req->cell_parm.q_qualmin; 

#ifdef RF_MULTIMODE
          rrc_csp_int_data.wcdma_cell_srxlevmin =
            csp_cmd_l1_req->cell_parm.q_rxlevmin + 
              MAX( (csp_cmd_l1_req->cell_parm.max_tx_pwr -
                      RFUMTS_UE_MAX_TX_POWER), 0 );
#else /* RF_MULTIMODE */
          rrc_csp_int_data.wcdma_cell_srxlevmin =
            csp_cmd_l1_req->cell_parm.q_rxlevmin + 
              MAX( (csp_cmd_l1_req->cell_parm.max_tx_pwr -
                      RF_UE_MAX_TX_POWER), 0 );
#endif /* RF_MULTIMODE */
        }

        break;        


      case CPHY_CELL_SELECTION_REQ:
        csp_cmd_l1_req->cell_sel.freq = 
                            acq_entry_ptr->freq;
        csp_cmd_l1_req->cell_sel.prim_code_inx =
                            (uint16)acq_entry_ptr->scr_code;
        break;

     case CPHY_CELL_TRANSITION_REQ:
       csp_cmd_l1_req->cell_transition.cell_trans_params.select_type = L1_ASET_UPD_CELL_CAMP;
       csp_cmd_l1_req->cell_transition.setup_cmd.req_mask  = 0;

       csp_cmd_l1_req->cell_transition.setup_cmd.freq_incl = TRUE;
       csp_cmd_l1_req->cell_transition.setup_cmd.rf_chan= (uint16)acq_entry_ptr->freq;
       csp_cmd_l1_req->cell_transition.cell_trans_params.scr_code= (uint16)acq_entry_ptr->scr_code;

       csp_cmd_l1_req->cell_transition.setup_cmd.dl_cctrch_info    = NULL;
       csp_cmd_l1_req->cell_transition.setup_cmd.dl_ctfc_info      = NULL;
       csp_cmd_l1_req->cell_transition.setup_cmd.dl_phychan_db = NULL;
       csp_cmd_l1_req->cell_transition.setup_cmd.ul_cctrch_info    = NULL;
       csp_cmd_l1_req->cell_transition.setup_cmd.ul_phychan_db = NULL;
       csp_cmd_l1_req->cell_transition.setup_cmd.tfcs_ptr          = NULL;

       csp_cmd_l1_req->cell_transition.cell_trans_params.resel_fail_cause = L1_RESEL_FAIL_CAUSE_NONE;
       csp_cmd_l1_req->cell_transition.cell_trans_params.tg_freq_excl = FALSE;
       csp_cmd_l1_req->cell_transition.cell_trans_params.tg_cell_excl = FALSE;
       csp_cmd_l1_req->cell_transition.setup_cmd.mac_e_reset_indicator = FALSE;
       csp_cmd_l1_req->cell_transition.setup_cmd.call_type_mask = 0;
       csp_cmd_l1_req->cell_transition.setup_cmd.is_emergency_call = FALSE;
       csp_cmd_l1_req->cell_transition.setup_cmd.transition_to_dch = FALSE;
       csp_cmd_l1_req->cell_transition.setup_cmd.dl_phychan_db = NULL;
       csp_cmd_l1_req->cell_transition.setup_cmd.ul_phychan_db = NULL;
       csp_cmd_l1_req->cell_transition.setup_cmd.e_info_ptr = NULL;
#ifdef FEATURE_WCDMA_HS_RACH
       csp_cmd_l1_req->cell_transition.setup_cmd.hs_rach_e_info_ptr = NULL;
#endif

       csp_cmd_l1_req->cell_transition.setup_cmd.l1_hs_scch_order_action = L1_HS_SCCH_ORDER_ACTION_BACKUP;

       csp_cmd_l1_req->cell_transition.cell_trans_params.pccpch_tx_div_ind = rrcllc_get_correct_pccpch_tx_diversity_from_sibs(rrc_get_state());
       break;

      case CPHY_CELL_BAR_REQ:
        csp_cmd_l1_req->cell_bar_req.t_barred = 
                                            (uint16)rrc_csp_int_data.wcdma_cell_time_to_reselect;

        csp_cmd_l1_req->cell_bar_req.bar_for_resel_only = 
                                                                                  rrc_csp_int_data.bar_for_resel_only;
        csp_cmd_l1_req->cell_bar_req.non_eq_plmn_barred = 
                                                                                  rrc_csp_int_data.non_eq_plmn_barred;

        csp_cmd_l1_req->cell_bar_req.barred_due_to_band_mismatch=
                                                                                  rrc_csp_int_data.barred_due_to_band_mismatch;
        rrc_csp_int_data.barred_due_to_band_mismatch = FALSE;

#ifdef FEATURE_FEMTO_CSG
        csp_cmd_l1_req->cell_bar_req.bar_non_white_csg = rrc_csp_int_data.bar_non_white_csg;
        rrc_csp_int_data.bar_non_white_csg = FALSE;
#endif

        WRRC_MSG2_HIGH("CR151: bar_for_resel_only is %d non-eq %d",
             rrc_csp_int_data.bar_for_resel_only,rrc_csp_int_data.non_eq_plmn_barred);
        rrc_csp_int_data.non_eq_plmn_barred=FALSE;

#ifdef FEATURE_WCDMA_UNBAR_OPRTR_RSVD_FREQ
        if(rrc_csp_int_data.cell_bar_info == RRC_CSP_WCDMA_FREQ_BARRED)
        {
          csp_cmd_l1_req->cell_bar_req.freq_restricted_for_operator_use = rrc_csp_int_data.cell_reserved_for_operator_use;
          WRRC_MSG1_HIGH("Freq restricted for operator use %d ",csp_cmd_l1_req->cell_bar_req.freq_restricted_for_operator_use);
        }
#endif
        rrc_csp_int_data.cell_reserved_for_operator_use = FALSE;
        switch(rrc_csp_int_data.cell_bar_info)
        {
          case RRC_CSP_GSM_CELL_BARRED:
            csp_cmd_l1_req->cell_bar_req.l1_cell_bar_info = 
                                                                        L1_GSM_CELL_BARRED;
            break;

          case RRC_CSP_WCDMA_PSC_BARRED:
            csp_cmd_l1_req->cell_bar_req.l1_cell_bar_info = 
                                                                        L1_WCDMA_PSC_BARRED;
            break;

          case RRC_CSP_WCDMA_FREQ_BARRED:
            csp_cmd_l1_req->cell_bar_req.l1_cell_bar_info = 
                                                                        L1_WCDMA_FREQ_BARRED;
            break;

          case RRC_CSP_WCDMA_ASET_BARRED:
            csp_cmd_l1_req->cell_bar_req.l1_cell_bar_info = 
                                                                        L1_WCDMA_ASET_BARRED;
            break;
#ifdef FEATURE_WCDMA_TO_LTE
          case RRC_CSP_LTE_CELL_BARRED:
            csp_cmd_l1_req->cell_bar_req.l1_cell_bar_info = L1_EUTRA_CELL_BARRED;
            break;

          case RRC_CSP_LTE_FREQ_BARRED:
            csp_cmd_l1_req->cell_bar_req.l1_cell_bar_info = L1_EUTRA_FREQ_BARRED;
            break;
#endif
          case RRC_CSP_WCDMA_FREQ_UNBAR:
            csp_cmd_l1_req->cell_bar_req.l1_cell_bar_info = L1_WCDMA_FREQ_UNBARRED;
            break;

          default:
            WRRC_MSG1_ERROR("ERR Incorrect cell bar info %d", rrc_csp_int_data.cell_bar_info);
            /* We do not want to send wrong cell bar request to L1 */
        }
        
        /* Check if WCDMA cell/frequency needs to be barred */
        if((rrc_csp_int_data.cell_bar_info != RRC_CSP_GSM_CELL_BARRED)
#ifdef FEATURE_WCDMA_TO_LTE
           &&(rrc_csp_int_data.cell_bar_info != RRC_CSP_LTE_CELL_BARRED)
           &&(rrc_csp_int_data.cell_bar_info != RRC_CSP_LTE_FREQ_BARRED)
#endif
          )
        {
          csp_cmd_l1_req->cell_bar_req.id.fdd.freq = 
                                                                (uint16)acq_entry_ptr->freq;
          csp_cmd_l1_req->cell_bar_req.id.fdd.scr_code = 
                                                                (uint16)acq_entry_ptr->scr_code;

          WRRC_MSG2_HIGH("Cell bar req sent to L1. cell_bar_reason: %d, T-barred tmr: %d",
                          csp_cmd_l1_req->cell_bar_req.l1_cell_bar_info, 
                          csp_cmd_l1_req->cell_bar_req.t_barred);

          WRRC_MSG2_HIGH("Cell bar Freq: %d,  Cell bar scr_code: %d",
                          csp_cmd_l1_req->cell_bar_req.id.fdd.freq,
                          csp_cmd_l1_req->cell_bar_req.id.fdd.scr_code);
        }
        /* GSM cell needs to be barred */
        else if (rrc_csp_int_data.cell_bar_info == RRC_CSP_GSM_CELL_BARRED)
        {
          /* Check if GSM cell is being barred due to WTOG Reselection Failure
           * or WTOG Cell Change Order Failure
           */

          /* WTOG Cell Change Order Failed bcoz G cell is barred*/
          if(rrccho_is_wtog_cco_active())
          {
            rrccho_fill_gsm_cell_info_for_cell_bar(
                &(csp_cmd_l1_req->cell_bar_req.id.gsm.bcch_arfcn),
                &(csp_cmd_l1_req->cell_bar_req.id.gsm.bsic.ncc),
                &(csp_cmd_l1_req->cell_bar_req.id.gsm.bsic.bcc));
          }
          /* WTOG Reselection Failed */
          else
          {
            csp_cmd_l1_req->cell_bar_req.id.gsm.bcch_arfcn = 
                    rrc_csp_int_data.saved_rrc_cmd.cmd.cell_rsel_ind.u.gsm_info.cell_info.bcch_arfcn;
            csp_cmd_l1_req->cell_bar_req.id.gsm.bsic.ncc   = 
                    rrc_csp_int_data.saved_rrc_cmd.cmd.cell_rsel_ind.u.gsm_info.cell_info.bsic.ncc;
            csp_cmd_l1_req->cell_bar_req.id.gsm.bsic.bcc   = 
                    rrc_csp_int_data.saved_rrc_cmd.cmd.cell_rsel_ind.u.gsm_info.cell_info.bsic.bcc;
          }
          
          MSG_HIGH("Cell bar req sent to L1. cell_bar_reason: %d, T-barred tmr: %d, bcch_arfcn: %d",
                               rrc_csp_int_data.cell_bar_info, 
                               csp_cmd_l1_req->cell_bar_req.t_barred, 
                               csp_cmd_l1_req->cell_bar_req.id.gsm.bcch_arfcn.num);

          WRRC_MSG2_HIGH("bsic.ncc: %d, bsic.bcc: %d", 
                       csp_cmd_l1_req->cell_bar_req.id.gsm.bsic.ncc, 
                       csp_cmd_l1_req->cell_bar_req.id.gsm.bsic.bcc);
        }
#ifdef FEATURE_WCDMA_TO_LTE
        else
        {
          csp_cmd_l1_req->cell_bar_req.id.eutra.band_width = 
            rrc_csp_int_data.saved_rrc_cmd.cmd.cell_rsel_ind.u.eutra_info.cell_info.band_width;
          csp_cmd_l1_req->cell_bar_req.id.eutra.EARFCN= 
            rrc_csp_int_data.saved_rrc_cmd.cmd.cell_rsel_ind.u.eutra_info.cell_info.EARFCN;
          csp_cmd_l1_req->cell_bar_req.id.eutra.physical_cell_identity= 
            rrc_csp_int_data.saved_rrc_cmd.cmd.cell_rsel_ind.u.eutra_info.cell_info.physical_cell_identity;
          MSG_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "WTOL: Cell bar req sent to L1, cell_bar_reason: %d, T-barred tmr: %d, EARFCN: %d PCI: %d, BW: %d",
            csp_cmd_l1_req->cell_bar_req.l1_cell_bar_info,csp_cmd_l1_req->cell_bar_req.t_barred,
            csp_cmd_l1_req->cell_bar_req.id.eutra.EARFCN, csp_cmd_l1_req->cell_bar_req.id.eutra.physical_cell_identity, 
            csp_cmd_l1_req->cell_bar_req.id.eutra.band_width);
        }
#endif
        break;

	  case CPHY_GO_TO_SLEEP_IND:
        WRRC_MSG0_HIGH("CPHY GO TO SLEEP IND sent to L1"); 
		break;

      default:
        ERR_FATAL("L1 cmd 0x%x not supported",l1_cmd_id, 0, 0 );
        /* Call the RRC function to release the Internal Buffer */
        rrc_free_cmd_buf(csp_out_cmd_ptr);
        csp_out_cmd_ptr = NULL;
    } /* switch(l1_cmd) */

    /* Send the Internal RRC Command - LLC should directly handle it */
    rrc_put_int_cmd( csp_out_cmd_ptr );
  } /* Got buffer to send cmd */
}   /* rrc_csp_send_l1_cmd */

/*===========================================================================

FUNCTION          RRC_CSP_SEND_L1_DRX_REQ

DESCRIPTION       This function is responsible for sending CPHY_DRX_REQ
                  to L1.

DEPENDENCIES
                  

RETURN VALUE
                  None

SIDE EFFECTS      None.
===========================================================================*/
void rrc_csp_send_l1_drx_req(void)
{

  rrc_cmd_type *cmd_ptr;
  uint32 drx_len;

  /* Send DRX command to L1, no confirm to wait for */

  if((cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    l1_req_cmd * csp_cmd_l1_req = NULL;
    RRC_GET_POINTER_L1_REQ_VALUE(cmd_ptr->cmd.rrc_llc_req.l1_req,csp_cmd_l1_req);

    /* Initialize the LLC command header first */
    cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_CMD_REQ;
#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rrc_llc_req.l1_cmd.sys_modem_as_id = rrc_get_as_id();
#endif
    cmd_ptr->cmd.rrc_llc_req.procedure = RRC_PROCEDURE_CSP;
    cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;
  
    /* Now fill the L1 cmd header */
    cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_DRX_REQ;
    cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_NONE;

    /* Populate the cmd data */
    drx_len = rrcllc_get_drx_info(); 

    csp_cmd_l1_req->drx.drx_cycle_len = rrc_csp_set_drx_length(drx_len);
    
    /* Send the Internal RRC command - LLC should directly handle it */
    rrc_put_int_cmd(cmd_ptr);
  }
} /*rrc_csp_send_l1_drx_req */

/*===========================================================================

FUNCTION          RRC_CSP_SEND_L1_DRX_REQ_FOR_MULTIPLE_DRX

DESCRIPTION       This function is responsible for sending CPHY_DRX_REQ
                  to L1 for multiple drx support feature.

DEPENDENCIES
                  

RETURN VALUE
                  SUCCESS: Able to send CPHY_DRX_REQ successfully.
                  FAILURE: Not able to send due to any of the multiple reasons.

SIDE EFFECTS      None.
===========================================================================*/
void rrc_csp_send_l1_drx_req_for_multiple_drx(void)
{

  rrc_cmd_type *cmd_ptr;
  uint32 drx_len;

  /* Send DRX command to L1, no confirm to wait for */

  if((cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    l1_req_cmd * csp_cmd_l1_req = NULL;
    RRC_GET_POINTER_L1_REQ_VALUE(cmd_ptr->cmd.rrc_llc_req.l1_req,csp_cmd_l1_req);

    /* Initialize the LLC command header first */
    cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_CMD_REQ;
#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rrc_llc_req.l1_cmd.sys_modem_as_id = rrc_get_as_id();
#endif
    cmd_ptr->cmd.rrc_llc_req.procedure = RRC_PROCEDURE_CSP;
    cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;
  
    /* Now fill the L1 cmd header */
    cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_DRX_REQ;
    cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_NONE;

    /* Populate the cmd data */
    drx_len=rrcllc_get_drx_info_for_multiple_drx();

    csp_cmd_l1_req->drx.drx_cycle_len =  rrc_csp_set_drx_length(drx_len);
    
    /* Send the Internal RRC command - LLC should directly handle it */
    rrc_put_int_cmd(cmd_ptr);
  }
} /*rrc_csp_send_l1_drx_req_for_multiple_drx*/

/*===========================================================================

FUNCTION          RRC_CSP_SEND_SIB_CMD

DESCRIPTION       This function is responsible for filling up data structures
                  for all RRC Internal commands to SIB procedure sent by CSP.
                  This also allocates cmd buffer and sends the command.
                  TBD: If the data going into commands is conditional on 
                  something, a mechanism will be needed to pass that condition.
                  It could be thru' devicing slightly different names for
                  the same commands with different conditional data.

DEPENDENCIES
                  The pointer to Acquisition Entry should be valid.
RETURN VALUE
                  SUCCESS: RRC Internal command was successfully sent to SIB Proc..
                  FAILURE: Command could not be sent due to any one of multiple
                           reasons.

SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type  
rrc_csp_send_sib_cmd(rrc_cmd_e_type sib_cmd, 
                     rrc_csp_sib_mask_type sib_mask, 
                     rrc_csp_acq_entry_type *acq_entry_ptr,
                     boolean for_serving_cell)
{
  int i;
  rrc_csp_sib_mask_type  gen_mask = MIB_BIT;
  rrc_cmd_type   *csp_out_cmd_ptr;

  /* Check validity of Acq Entry pointer */
  if(acq_entry_ptr == NULL)
  {
    WRRC_MSG0_ERROR("Passed Acq Entry pointer is NULL");
    return(FAILURE);
  }

  /* Send SIB procedure an event to get SIBs */
  csp_out_cmd_ptr = rrc_get_int_cmd_buf();
  /* Populate the cmd data */
  csp_out_cmd_ptr->cmd_hdr.cmd_id = sib_cmd;
  switch(sib_cmd)
  {
      case RRC_GET_SPECIFIC_SIBS_REQ:
        /* Get Frequency and Scrambling Code from CSP Internal Data */
        csp_out_cmd_ptr->cmd.get_specific_sibs_req.freq = acq_entry_ptr->freq;
        csp_out_cmd_ptr->cmd.get_specific_sibs_req.scrambling_code = 
            acq_entry_ptr->scr_code;
        csp_out_cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell = for_serving_cell;
        csp_out_cmd_ptr->cmd.get_specific_sibs_req.procedure = RRC_PROCEDURE_CSP;

        if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
        {
          csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
            RRC_SIB_AUTO_BPLMN_SCAN;
#ifdef FEATURE_FEMTO_CSG
          if(rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_CSG)
          {
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.for_csg_cell = TRUE;
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
              RRC_SIB_AUTO_BPLMN_SCAN;
          }
#endif

        }
        else if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
        {
          csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
            RRC_SIB_MANUAL_BPLMN_SCAN;
#ifdef FEATURE_FEMTO_CSG
          if(rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_CSG)
          {
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.for_csg_cell = TRUE;
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
              RRC_SIB_AUTO_BPLMN_SCAN;
          }
#endif
        }
#ifdef FEATURE_LTE_TO_WCDMA
        else if ( (rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN)
             &&(rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE_CGI ))
        {
          csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
            RRC_SIB_MANUAL_BPLMN_SCAN;
        }         
#endif
        else if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
        {
          csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
            RRC_SIB_WTOW_AUTO_BPLMN_SCAN;
#ifdef FEATURE_FEMTO_CSG
          if(rrc_csp_int_data.csg_asf_srch_in_progress)
          {
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.for_csg_cell = TRUE;
          }
#endif
        }
        else if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)  
        {
          csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
            RRC_SIB_WTOW_MANUAL_BPLMN_SCAN;
#ifdef FEATURE_FEMTO_CSG
          if(rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_CSG)
          {
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.for_csg_cell = TRUE;
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
              RRC_SIB_WTOW_AUTO_BPLMN_SCAN;
          }
#endif
        }
        else if(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
        {
          csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
            RRC_SIB_MANUAL_SCAN;
#ifdef FEATURE_FEMTO_CSG
          if(rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_CSG)
          {
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.for_csg_cell = TRUE;
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = 
              RRC_SIB_AUTO_BPLMN_SCAN;
          }
#endif
        }
        else
        {
          csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type =
            RRC_SIB_NORMAL; 
#ifdef FEATURE_FEMTO_CSG
          if((((rrccsp_get_curr_nas_requested_csg_id() != SYS_CSG_ID_INVALID)&&
             (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)&&(for_serving_cell))||
             ((rrccsp_get_curr_nas_requested_csg_id() != SYS_CSG_ID_INVALID)&&
               rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG))
            )
            {  
              csp_out_cmd_ptr->cmd.get_specific_sibs_req.for_csg_cell = TRUE;
            }
#ifdef FEATURE_LTE_TO_WCDMA
            else if ( (rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN)
                    &&(rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE ))

            {
              csp_out_cmd_ptr->cmd.get_specific_sibs_req.for_csg_cell = rrc_csp_int_data.csg_resel;
            }
#endif
#endif
        }
#ifdef FEATURE_3GPP_CSFB
        if((RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN == rrc_csp_int_data.curr_scan) &&
           (RRC_CSP_INTERRAT_LTE == rrc_csp_int_data.interrat_type))
        {
          uint8 idx;
          if((acq_entry_ptr->freq == rrc_csp_int_data.dest_freq) &&
             (WCDMA_RRC_MAX_UTRA_CONT != (idx = rrccsp_get_idx_sib_cont_list(acq_entry_ptr->scr_code/16))))
          {
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = RRC_SIB_READ_FROM_SIB_CONTAINER;
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.sys_info_cont_ptr = &rrc_SIB_container.cont[idx];
            WRRC_MSG0_HIGH("CSFB: Set scan_type to RRC_SIB_READ_FROM_SIB_CONTAINER");
          }
        }
#endif
        /* Indicate which SIBs are needed */
        for(i = 0; 
            i < RRC_TOT_SIBS; 
            i++, gen_mask <<= 1)
        {
          if(sib_mask & gen_mask)
          {
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_req[i] = TRUE;
          }
          else
          {
            csp_out_cmd_ptr->cmd.get_specific_sibs_req.sib_req[i] = FALSE;
          }
        }
#ifdef FEATURE_3GPP_CSFB
        if((rrc_csfb_call_status) &&
           (RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN == rrc_csp_int_data.curr_scan) && 
           (RRC_CSP_INTERRAT_LTE == rrc_csp_int_data.interrat_type))
        {
          /*CSFB is active when camping on W due to redirection from LTE*/
          WRRC_MSG0_HIGH("CSFB: SIB cmd being sent when CSFB call is up");
          rrc_csp_int_data.sib_read_csfb_active = TRUE;
        }
        else
        {
          rrc_csp_int_data.sib_read_csfb_active = FALSE;
        }
#endif
        break;

      default:
        WRRC_MSG1_ERROR("SIB cmd 0x%x not supported",sib_cmd);
        /* Call the RRC function to release the Internal Buffer */
        rrc_free_cmd_buf(csp_out_cmd_ptr);
        csp_out_cmd_ptr = NULL;
        return(FAILURE);
  }
    /* Send the Internal cmd to RRC */
    rrc_put_int_cmd( csp_out_cmd_ptr );
    return(SUCCESS);
} /* rrc_csp_send_sib_cmd */



uecomdef_status_e_type  
rrccsp_fill_nas_common_info
(
  rrc_SysInfoType1  *sib1_ptr,
  rrc_gsm_map_nas_sys_info_type  *nas_common_info_ptr
)
{
  unsigned char  *data_ptr;
  uint32          i;

  /* The SIB1 is present - lets disect it */
  nas_common_info_ptr->length =
    (uint32)sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts;

  /* Check if the RRC MM interface structure array can hold this info */
  if(sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts > RRC_MAX_GSM_INFO_OCTET_COUNT)
  {
    WRRC_MSG1_ERROR("Not enough space for Common NAS Sys Info (%d bytes)",
                        sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts);
    return(FAILURE);
  }

  /* Fill in the Common NAS system info now */
  data_ptr = sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.data;
  for( i = 0; i < sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts; i ++)
  {
    nas_common_info_ptr->value[i] = *data_ptr;
    data_ptr ++;
  }

  return(SUCCESS);
} /* rrccsp_fill_nas_common_info */


uecomdef_status_e_type  
rrccsp_fill_nas_cn_specific_info
(
  rrc_CN_DomainSysInfo *     asn1_cn_domain_sys_info_ptr,
  rrc_cn_domain_sys_info_type  *rrc_cn_domain_sys_info_ptr
)
{
  unsigned char  *data_ptr;
  uint32          i;

  if(asn1_cn_domain_sys_info_ptr->cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
  {
    rrc_cn_domain_sys_info_ptr->cn_domain_id = RRC_CS_DOMAIN_CN_ID;
  }
  else
  {
    rrc_cn_domain_sys_info_ptr->cn_domain_id = RRC_PS_DOMAIN_CN_ID;
  }

  rrc_cn_domain_sys_info_ptr->cn_domain_specific_info.gsm_map_info.length = 
    asn1_cn_domain_sys_info_ptr->cn_Type.u.gsm_MAP->numocts;

  /* Check if the RRC MM interface structure array can hold this info */
  if(asn1_cn_domain_sys_info_ptr->cn_Type.u.gsm_MAP->numocts > 
     RRC_MAX_GSM_INFO_OCTET_COUNT)
  {
    WRRC_MSG1_ERROR("Not enough space for NAS Sys Info (%d bytes)",
         asn1_cn_domain_sys_info_ptr->cn_Type.u.gsm_MAP->numocts);
    return(FAILURE);
  }

  data_ptr = asn1_cn_domain_sys_info_ptr->cn_Type.u.gsm_MAP->data;

  for( i = 0; i < asn1_cn_domain_sys_info_ptr->cn_Type.u.gsm_MAP->numocts; i ++)
  {
    rrc_cn_domain_sys_info_ptr->cn_domain_specific_info.gsm_map_info.value[i] =
     *data_ptr;   

    data_ptr ++;
  } 

  return(SUCCESS);
} /* rrccsp_fill_nas_cn_specific_info */


/*===========================================================================

FUNCTION          RRC_CSP_SEND_SERVICE_IND_DUMMY

DESCRIPTION       This function sends RRC_SERVICE_IND to MM
                  with the last sent service indication parameters.Currently 
                  used by CHO after a W->G Cell change order failure

DEPENDENCIES
                  None.

RETURN VALUE
                  None.
                  
SIDE EFFECTS
                  None.
===========================================================================*/
void rrc_csp_send_service_ind_dummy(void)
{
  mm_cmd_type *mm_buf_ptr = NULL;

  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_SERVICE_IND);

  WCDMA_MEMCPY(&mm_buf_ptr->cmd.rrc_service_ind, 
                   sizeof(rrc_service_ind_type),
                    &rrc_service_ind_bkup,
                   sizeof(rrc_service_ind_type)); 

  rrc_send_rrc_message_to_mm(mm_buf_ptr);
  
  /* Update the last sent MM command in the int data for bookkeeping */
  rrc_csp_int_data.last_mm_primitive = RRC_CSP_SRVC_IND_WITH_SRVC_SENT;  
}

/*===========================================================================

FUNCTION          RRC_CSP_SEND_SERVICE_IND

DESCRIPTION       This function sends RRC_SERVICE_IND to MM
                  with the current cell's parameters.Currently 
                  used by CHO after a W->G Cell change order failure

DEPENDENCIES
                  None.

RETURN VALUE
                  None.
                  
SIDE EFFECTS
                  None.
===========================================================================*/
void rrc_csp_send_service_ind(void)
{
  /* Send Service Indication to MM */
  rrc_csp_send_mm_cmd(RRC_SERVICE_IND,
                      rrc_csp_int_data.curr_service_status,
                      &rrc_csp_int_data.curr_sel_plmn,
                      &rrc_csp_int_data.curr_acq_entry);

  /* Update the last sent MM command in the int data for bookkeeping */
  rrc_csp_int_data.last_mm_primitive = RRC_CSP_SRVC_IND_WITH_SRVC_SENT;
}

#ifdef FEATURE_WRLF_SYSTEM_SEL
/*===========================================================================

FUNCTION          RRCCSP_CHECK_FOR_STOP_WCDMA_REQ

DESCRIPTION       This function checks if RRC_STOP_WCDMA_MODE_REQ is present in
                  MM command queue and returns TRUE/FALSE accordingly

DEPENDENCIES
                  
RETURN VALUE
                  TRUE: Command is present in MM queue
                  FALSE: Command is not present in MM queue

SIDE EFFECTS
                  None
===========================================================================*/
static boolean rrccsp_check_for_stop_wcdma_req(void)
{
  rrc_cmd_type *mm_cmd_ptr;
  boolean stop_req_sts = FALSE;

  mm_cmd_ptr = (rrc_cmd_type *)q_check (&rrc_mm_cmd_q);
  /*TBD: The case of service_req after the list_req and list_req after service_req without stop in between is also to be taken case*/
  while(mm_cmd_ptr != NULL)
  {
    if(mm_cmd_ptr->cmd_hdr.cmd_id == RRC_STOP_WCDMA_MODE_REQ)
    {
      WRRC_MSG0_HIGH("STOP_WCDMA_MODE_REQ is in MM queue");

      stop_req_sts = TRUE;
      break;
    }
    mm_cmd_ptr = (rrc_cmd_type *)q_next(&rrc_mm_cmd_q,&mm_cmd_ptr->cmd_hdr.link);
  }
  return stop_req_sts;
}

#endif
/*===========================================================================

FUNCTION          RRC_CSP_SEND_MM_CMD

DESCRIPTION       This function is responsible for filling up data structures
                  for all commands to MM sent by CSP.
                  This also allocates cmd buffer and sends the command.
                  It also frees allocated command and embedded buffers
                  when failure is detected before sending the command.
                  It updates the last primtive sent to MM in internal data.

DEPENDENCIES
                  
RETURN VALUE
                  SUCCESS: Command was successfully sent to MM.
                  FAILURE: Command could not be sent due to any one of multiple
                           reasons.

SIDE EFFECTS
                  None
===========================================================================*/
void
rrc_csp_send_mm_cmd(rrc_to_mm_sink_e_type  cmd_id, 
                    mm_as_service_state_T  service_status,
                    rrc_selected_plmn_type *sel_plmn_ptr,
                    rrc_csp_acq_entry_type *acq_entry_ptr)
{
#ifdef FEATURE_WRLF_SYSTEM_SEL
  boolean hack_srvc_avail_for_lim_srvc = FALSE;
#endif 
  rrc_SysInfoType1  *sib1_ptr;
  mm_cmd_type *mm_buf_ptr = NULL;
  uint32 num_cn_domains,idx=0;
  rrc_CN_DomainSysInfoList * cn_domain_sys_info_list_ptr;
  rrc_csp_access_control_e_type   access_control;
  rrc_dsac_info_type rrc_dsac_info;
  rrc_state_e_type rrc_state = rrc_get_state();
  sys_band_mask_type       srv_req_band_pref;
  uint8 rat_idx = 0;

  rrc_ppac_info_type rrc_ppac_info;

#ifdef FEATURE_WCDMA_MULTIBAND
  rrc_csp_band_class_type band_class = rrc_get_frequency_band(acq_entry_ptr->freq, acq_entry_ptr->plmn_id);
  sys_band_mask_e_type sys_band = rrc_convert_band_to_sys_band(band_class);
#endif

  sys_hs_ind_e_type new_hs_ind = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
  rrc_plmn_identity_type common_plmn_id;

  mm_cmd_type  *mm_common_buf_ptr;
  mm_common_buf_ptr = NULL;

  rrc_csp_int_data.do_lfs_scan = FALSE;


  /* Hack */
  if(search_order_specialization  == SYS_SPECIAL_SBM_RAT_SEARCH_ORDER)
  {
#ifdef FEATURE_RAT_PRIORITY_LIST
    srv_req_band_pref = rrc_extract_gw_band_pref_from_service_req(&rrc_csp_service_req.rat_pri_list_info);
#else
    srv_req_band_pref = rrc_csp_service_req.band_pref;
#endif  
    if((cmd_id == RRC_SERVICE_CNF) && (service_status == MM_AS_NO_SERVICE))
    {
      if((rrc_csp_int_data.band_pref == SYS_BAND_MASK_WCDMA_IX_1700) || (rrc_csp_int_data.band_pref == SYS_BAND_MASK_WCDMA_XI_1500))
      {
        sys_band_mask_type band_pref;
  
        if(rrc_csp_int_data.band_pref == SYS_BAND_MASK_WCDMA_XI_1500)
        {
          WRRC_MSG0_HIGH("Search Order Specialization : Band 11 scan is done");
          band_pref = SYS_BAND_MASK_WCDMA_XI_1500;
        }
        else
        {
          WRRC_MSG0_HIGH("Search Order Specialization: Band 9 scan is done");
          band_pref = SYS_BAND_MASK_WCDMA_XI_1500 | SYS_BAND_MASK_WCDMA_IX_1700;
        }
  
        /* If all the bands have already been scanned then report NO service to NAS */

        if(srv_req_band_pref & ~band_pref)
        {
          rrc_cmd_type *rrc_cmd_ptr = NULL;
          WRRC_MSG0_HIGH("Search Order Specialization : Posting internal service request"); 
          rrc_cmd_ptr = rrc_get_mm_cmd_buf();
          rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_SERVICE_REQ;
  
          WCDMA_MEMCPY(&rrc_cmd_ptr->cmd.rrc_service_req, 
                       sizeof(rrc_service_req_type),
                       &rrc_csp_service_req, 
                       sizeof(rrc_service_req_type));
  
#ifdef FEATURE_RAT_PRIORITY_LIST
          for(rat_idx =0;rat_idx < rrc_cmd_ptr->cmd.rrc_service_req.rat_pri_list_info.num_items &&
                       rat_idx< SYS_MAX_ACQ_PREF_RAT_LIST_NUM ; rat_idx ++)
          {
            if(rrc_cmd_ptr->cmd.rrc_service_req.rat_pri_list_info.priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_WCDMA)
            {
#ifdef LTE_BAND_NUM
              rrc_cmd_ptr->cmd.rrc_service_req.rat_pri_list_info.priority_list_info[rat_idx].bst_band_cap.chgwt_band_cap = rrc_cmd_ptr->cmd.rrc_service_req.rat_pri_list_info.priority_list_info[rat_idx].bst_band_cap.chgwt_band_cap & ~band_pref;
#else
              rrc_cmd_ptr->cmd.rrc_service_req.rat_pri_list_info.priority_list_info[rat_idx].bst_band_cap = rrc_cmd_ptr->cmd.rrc_service_req.rat_pri_list_info.priority_list_info[rat_idx].bst_band_cap & ~band_pref;
#endif
            }
          }
#else
          /* Not including the bands which has been scanned already */
          rrc_cmd_ptr->cmd.rrc_service_req.band_pref = rrc_cmd_ptr->cmd.rrc_service_req.band_pref & ~band_pref;
#endif  
          rrc_csp_internal_service_req = TRUE;
          rrc_put_mm_cmd(rrc_cmd_ptr);
  
          if(rrc_csp_int_data.full_scan_in_deep_sleep_counter != 0)
          {
            -- rrc_csp_int_data.full_scan_in_deep_sleep_counter;
          }
          return;
        }
      }
    }
    rrc_csp_internal_service_req = FALSE;
    rrc_csp_int_data.band_pref = srv_req_band_pref;
  }
  if(acq_entry_ptr->is_valid_nw_sharing)
  {
    common_plmn_id = acq_entry_ptr->shared_list_of_plmns.common_plmn_id;
  }
  else
  {
    common_plmn_id = acq_entry_ptr->plmn_id;
  }
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)|| defined (FEATURE_WCDMA_DEDICATED_PRI_INFO)
  if(RRC_SERVICE_CNF == cmd_id)
  {
    if(MM_AS_SERVICE_AVAILABLE == service_status)
    {
      /*UE camped successfull, set the owner to WCDMA if W had valid dedicated priorities*/
      rrcmeas_send_clear_ded_pri_ind_set_owner();
    }
    else if(LTE_RRC_DEDICATED_PRI_OWNER_WCDMA != lte_rrc_dedicated_pri_get_owner())
    {
      rrcmeas_invalidate_dedicated_priority_info();
    }
  }
#endif

#ifdef FEATURE_FEMTO_CSG
  if( (TRUE == is_whitelist_valid) &&
         (MM_AS_SERVICE_AVAILABLE == service_status)
      && (rrc_csp_int_data.curr_type_acq == RRC_CSP_SPEC_PLMN)
      && (rrc_ccm_get_curr_camped_csg_id() == SYS_CSG_ID_INVALID)
      && (rrctmr_get_remaining_time(RRCTMR_CSG_ASF_SRCH_TIMER) == 0)
   )
  {
    if((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)||
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN)||
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN)||
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN)||
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN)
      )
    {
      rrctmr_start_timer(RRCTMR_CSG_ASF_SRCH_TIMER, RRC_CSG_DISCONN_TRANS_ASF_SRCH_TIMER_IN_MS);
    }
    else
    {
      rrctmr_start_timer(RRCTMR_CSG_ASF_SRCH_TIMER, (wcdma_asf_timer_nv)*60000);
    }
  }
#endif

  if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
  {
    /* Get a buffer from MM task */
    mm_common_buf_ptr = mm_rrc_get_cmd_buf(cmd_id);
  }
  else if(cmd_id == RRC_SERVICE_IND || cmd_id == RRC_SERVICE_CNF)
  {
    mm_buf_ptr = mm_rrc_get_cmd_buf(cmd_id);
  }
  else
  {
    WRRC_MSG0_HIGH("It shouldn't come here");
  }   

  MSG_HIGH("curr_scan %d, prev_curr_scan %d, svc_srch_bgnd %d", 
                                           rrc_csp_int_data.curr_scan, 
                                           rrc_csp_int_data.prev_curr_scan, 
                                           rrc_csp_int_data.service_search_bgnd);

  switch(cmd_id)
  {
    case RRC_SERVICE_CNF:
    {
#ifdef FEATURE_DUAL_SIM


        rrc_stop_lock_wait_timer();

       /* Do not release the lock for CSFB and ECALL cases */
       if((service_status == MM_AS_SERVICE_AVAILABLE) && 
           ((rrc_state == RRC_STATE_CELL_FACH) ||
           (rrc_state == RRC_STATE_CELL_DCH) ||
           ((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)&& (rrc_csp_int_data.service_req_cause == RRC_MO_CS_CALL))
#ifdef FEATURE_3GPP_CSFB 
          || (rrc_csfb_call_status)
#endif
         ))
       {
         WRRC_MSG2_HIGH("DSIM: RRC state is %d, MM svc sts %d CSFB call status is set. Do not release the lock",rrc_state,service_status);
       }
       else
       {
        rrc_release_lock();
       }
        mm_buf_ptr->cmd.rrc_service_cnf.as_id = rrc_get_as_id();
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
        /* If there are detected PLMN's during ANY_PLMN search, hack and send service cnf as service_available
                 for updating UI with limited service */
        if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS() && 
           (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) &&
          (rrc_csp_int_data.curr_type_acq == RRC_CSP_ANY_PLMN) &&
          (service_status == MM_AS_SERVICE_AVAILABLE) && (rrc_get_csp_substate() == RRCCSP_CELL_SEL_WT_SIB))
        {
          hack_srvc_avail_for_lim_srvc = TRUE;
          WRRC_MSG0_HIGH("WRLF:Sending SERVICE_CNF as srvc_available to update UI with Lim srvc.");
        }

#ifdef FEATURE_DUAL_SIM
        if(
            (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) && 
            ((rrc_state == RRC_STATE_CELL_DCH) ||(rrc_state == RRC_STATE_CELL_FACH)) &&
            (service_status == MM_AS_SERVICE_AVAILABLE) &&
             wcdma_is_ue_in_dual_standby)
        {
          /*Service is found in DCH/FACH state, increase the priority*/
          trm_reason_enum_t priority;

          if (rrc_ok_for_tune_away())
          {
            priority = TRM_BG_TRAFFIC;
          }
          else
          {
            priority = TRM_ACCESS_URGENT;
          }

          rrc_stop_lock_wait_timer();
          rrc_wrm_change_priority(priority);

          wrm_retain_lock_for_primary();
          rrcwrm_update_tune_away_status(priority);
        }
#endif
#endif
        rrcmcm_reset_last_act_cause();
        /* Fill up header */
        mm_buf_ptr->cmd.rrc_service_cnf.service_status = service_status;
        mm_buf_ptr->cmd.rrc_service_cnf.service_info.selected_plmn = *sel_plmn_ptr;
        mm_buf_ptr->cmd.rrc_service_cnf.service_info.gsm_map_common_info_incl = TRUE;
        mm_buf_ptr->cmd.rrc_service_cnf.status = MM_AS_SUCCESS;

        mm_buf_ptr->cmd.rrc_service_cnf.is_cell_id_valid = FALSE;
#ifdef FEATURE_FEMTO_CSG
        mm_buf_ptr->cmd.rrc_service_cnf.service_info.csg_info.csg_id = SYS_CSG_ID_INVALID;
        mm_buf_ptr->cmd.rrc_service_cnf.service_info.csg_info.hybrid_cell = FALSE;

#endif

        /* if a plmn was found by RRC then set ue_oos (UE is Out of Service) to FALSE 
         * This ue_oos flag is used by NAS to determine if limited service request should
         * be sent to RRC or not.
         */
        mm_buf_ptr->cmd.rrc_service_cnf.ue_oos = !rrc_csp_int_data.plmn_fnd_during_fgnd_svc_srch;
        WRRC_MSG1_HIGH("status of ue_oos %d sent to NAS", mm_buf_ptr->cmd.rrc_service_cnf.ue_oos);

#ifdef FEATURE_WCDMA_MULTIBAND
#ifdef FEATURE_EOOS
      #error code not present
#else
      rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;
#endif /*FEATURE_EOOS*/
#endif /* FEATURE_WCDMA_MULTIBAND */

      if(service_status == MM_AS_NO_SERVICE)
      {
        /* If no PLMN selected, fill in the available PLMN list */
        mm_buf_ptr->cmd.rrc_service_cnf.avail_plmn_list =
          rrc_csp_int_data.plmn_list;

#ifdef FEATURE_WCDMA_MULTIBAND
#ifdef FEATURE_EOOS
        #error code not present
#endif
        {
        /* Reset the current_band */
        rrc_csp_curr_select_data.current_band = RRC_CSP_IMT2K_BAND;
        }
#endif /* FEATURE_WCDMA_MULTIBAND */
  
        /* If ue is in deep sleep and available plmn list is empty then    */
        /* start a no_svc_timer so that if RRC receives a svc req from NAS */ 
        /* before this timer expires then return immediately with no svc   */
        /* - optimization (stand by time improvement)                      */
#ifdef FEATURE_EOOS
        #error code not present
#endif
          /*RRCTMR_DEEP_SLEEP_NO_SVC_TIMER shouldn't be started after performing ACQ DB only service search*/
          if((rrc_csp_int_data.ue_in_deep_sleep) && (rrc_csp_int_data.srvc_req_scan_scope != SYS_SCAN_SCOPE_ACQ_DB))
          {
            if((rrc_csp_int_data.plmn_list.num_plmns==0) && (!rrc_csp_int_data.plmn_fnd_during_fgnd_svc_srch))
            { 
              if(rrc_csp_int_data.deep_sleep_no_svc_tmr_expired)
              {
#ifdef FEATURE_DISABLE_DEEP_SLEEP
                rrctmr_start_timer(  RRCTMR_DEEP_SLEEP_NO_SVC_TIMER, 
                                      RRCTMR_DEEP_SLEEP_NO_SVC_TIMER_IN_MS );
#else
                rrctmr_start_timer( RRCTMR_DEEP_SLEEP_NO_SVC_TIMER, 
                                    (unsigned long) wcdma_rrc_deep_sleep_no_svc_timer_nv );
#endif
                rrc_csp_int_data.deep_sleep_no_svc_tmr_expired = FALSE;
  
#ifdef FEATURE_WCDMA_MULTIBAND
                rrc_csp_int_data.deep_sleep_no_svc_band_scanned_mask |= rrc_csp_int_data.wcdma_band_mask;
#endif
              }
              else
              {
#ifdef FEATURE_WCDMA_MULTIBAND
                rrc_csp_int_data.deep_sleep_no_svc_band_scanned_mask |= rrc_csp_int_data.wcdma_band_mask;
#endif
              }
              WRRC_MSG1_HIGH("Updating no_svc_band_scanned_mask %d",
                                          rrc_csp_int_data.deep_sleep_no_svc_band_scanned_mask);
            }
            else
            {
              WRRC_MSG1_HIGH("At least one PLMN found. Not updating no_svc_band_scanned_mask %d",
                                        rrc_csp_int_data.deep_sleep_no_svc_band_scanned_mask);
            } 
          }
#ifdef FEATURE_EOOS
        #error code not present
#endif


#ifdef FEATURE_EOOS
        #error code not present
#endif /*FEATURE_EOOS*/
      } /* service not available */

      /* Service is available */
      else  
      {

        mm_buf_ptr->cmd.rrc_service_cnf.is_cell_id_valid = TRUE;
        mm_buf_ptr->cmd.rrc_service_cnf.cell_id = acq_entry_ptr->cell_id;
#ifdef FEATURE_FEMTO_CSG
        mm_buf_ptr->cmd.rrc_service_cnf.service_info.csg_info.csg_id = acq_entry_ptr->csg_id;
        mm_buf_ptr->cmd.rrc_service_cnf.service_info.csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;
#endif
        WRRC_MSG1_HIGH("Reporting Cell_ID :%d to NAS",acq_entry_ptr->cell_id);

        if(rrc_csp_find_access_control(&access_control, &rrc_dsac_info,
                                      acq_entry_ptr->plmn_id,
                                      acq_entry_ptr->cell_id
                                      ,common_plmn_id
                                      ,&rrc_ppac_info
                                      )
           == FAILURE)
        {
          ERR_FATAL("Failure finding cell access status", 0, 0, 0); 
        }

        /* TBD: Set the number of PLMNs in available PLMN list to 0 for the time being */
        mm_buf_ptr->cmd.rrc_service_cnf.avail_plmn_list.num_plmns = 0;          

        /* Get the cell access status of the camped on cell from CCM */
        
        (void) rrc_csp_find_rrc_cell_access(&(mm_buf_ptr->cmd.rrc_service_cnf.service_info), &rrc_dsac_info,
                           access_control,acq_entry_ptr->status_mask);
        rrc_csp_int_data.curr_cell_access_status = mm_buf_ptr->cmd.rrc_service_cnf.service_info.cell_access;
        /* Fill CS and PS cell access info when DSAC information is valid*/
        rrc_csp_int_data.is_dsac_valid  = rrc_dsac_info.valid;
        if(rrc_dsac_info.valid == TRUE)
        {
          rrc_csp_int_data.curr_cs_cell_access = mm_buf_ptr->cmd.rrc_service_cnf.service_info.cs_cell_access;
          rrc_csp_int_data.curr_ps_cell_access = mm_buf_ptr->cmd.rrc_service_cnf.service_info.ps_cell_access;
          mm_buf_ptr->cmd.rrc_service_cnf.service_info.dsac_present = rrc_dsac_info.valid;
        }
        else
        {
          rrc_csp_int_data.curr_cell_access_status = mm_buf_ptr->cmd.rrc_service_cnf.service_info.cell_access;
        }

        rrc_csp_int_data.is_ppac_valid = rrc_ppac_info.valid;
        if(rrc_ppac_info.valid == TRUE)
        {
          mm_buf_ptr->cmd.rrc_service_cnf.service_info.ppac_present = rrc_ppac_info.valid;
          rrc_csp_int_data.curr_cell_cs_reg_allowed = mm_buf_ptr->cmd.rrc_service_cnf.service_info.cs_reg_allowed = rrc_ppac_info.cs_reg_allowed;
          rrc_csp_int_data.curr_cell_ps_reg_allowed = mm_buf_ptr->cmd.rrc_service_cnf.service_info.ps_reg_allowed = rrc_ppac_info.ps_reg_allowed;
          rrc_csp_int_data.curr_cell_page_rsp_allowed = mm_buf_ptr->cmd.rrc_service_cnf.service_info.page_rsp_allowed = rrc_ppac_info.page_rsp_allowed;
        }

#ifdef FEATURE_WRLF_SYSTEM_SEL
        /* After WRLF, if UE is continuing call on the same cell or EPLMN cell, RRC to send invalid PLMN to NAS so that
             no LA/RA is attemped by NAS. */
        if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS() && !hack_srvc_avail_for_lim_srvc)
        {
          mm_buf_ptr->cmd.rrc_service_cnf.service_info.selected_plmn.plmn_type = sel_plmn_ptr->plmn_type;
          mm_buf_ptr->cmd.rrc_service_cnf.service_info.selected_plmn.plmn_id.mcc[0]= 0xFF;
          mm_buf_ptr->cmd.rrc_service_cnf.service_info.selected_plmn.plmn_id.mcc[1]= 0xFF;
          mm_buf_ptr->cmd.rrc_service_cnf.service_info.selected_plmn.plmn_id.mcc[2]= 0xFF;
          MSG_HIGH("WRLF: Set mcc[0]=0x%x, mcc[1]=0x%x, mcc[2]=0x%x in RRC_SERVICE_CNF. ",
                         mm_buf_ptr->cmd.rrc_service_cnf.service_info.selected_plmn.plmn_id.mcc[0],
                         mm_buf_ptr->cmd.rrc_service_cnf.service_info.selected_plmn.plmn_id.mcc[1],
                         mm_buf_ptr->cmd.rrc_service_cnf.service_info.selected_plmn.plmn_id.mcc[2]);
 
          mm_buf_ptr->cmd.rrc_service_cnf.service_info.gsm_map_common_info_incl = FALSE;
          mm_buf_ptr->cmd.rrc_service_cnf.service_info.num_cn_domains = 0;
        }
        else
#endif
        {
        /* Fill up the NAS system info here */
        /* Let us first read SIB1 - we should have SIB1 for sure */
        sib1_ptr = (rrc_SysInfoType1 *) rrc_sibdb_return_sib_for_cell(
                                                  rrc_SIB1,
                                                    common_plmn_id, 
                                                  acq_entry_ptr->cell_id);
        if( sib1_ptr == NULL )
        {
          /* SIB3 for this PLMN, CEll ID combination not present */
          ERR_FATAL("SIB1 for MCC %d, MNC %d, Cell ID %d not present",
                        RRC_CSP_GET_INT_MCC(common_plmn_id), 
                        RRC_CSP_GET_INT_MNC(common_plmn_id),
                                         acq_entry_ptr->cell_id);
        } /* SIB1 not present */

        if(rrccsp_fill_nas_common_info(sib1_ptr, 
                                       &mm_buf_ptr->cmd.rrc_service_cnf.
                                         service_info.gsm_map_common_info)
          == FAILURE)
        {
          ERR_FATAL("Not able to send MM command",0,0,0);
        }

        /* Fill in the CN domain specific NAS system info now */
        num_cn_domains = 0;
        cn_domain_sys_info_list_ptr =  &sib1_ptr->cn_DomainSysInfoList; 
        do
        {            
          /* First check if Core network is GSM MAP. */
          if(cn_domain_sys_info_list_ptr->elem[idx].cn_Type.t == 
                                            T_rrc_CN_DomainSysInfo_cn_Type_gsm_MAP)
          {
            if(rrccsp_fill_nas_cn_specific_info(&cn_domain_sys_info_list_ptr->elem[idx], 
                                                &mm_buf_ptr->cmd.rrc_service_cnf.
                                                service_info.cn_domain_info[num_cn_domains])
               == FAILURE)
            {
              ERR_FATAL("Not able to send MM command",0,0,0);
            }
            num_cn_domains ++;
          } /* If CN type is GSM MAP */ 
          idx++;

        }while((cn_domain_sys_info_list_ptr->n > idx) && 
              (num_cn_domains < RRC_MAX_CN_DOMAINS));   
        
        mm_buf_ptr->cmd.rrc_service_cnf.service_info.num_cn_domains = num_cn_domains;
        }
#ifdef FEATURE_WCDMA_MULTIBAND
        /* Set the active band */
        mm_buf_ptr->cmd.rrc_service_cnf.active_band = sys_band;
        rrc_csp_int_data.deep_sleep_band_scanned_mask = 0;

        /* Reset no service band mask to zero since service has been found */
        rrc_csp_int_data.deep_sleep_no_svc_band_scanned_mask = 0;
#endif

        /* Reset the ue_in_deep_sleep variable */
        rrc_csp_int_data.ue_in_deep_sleep = FALSE;
        rrc_csp_int_data.full_scan_in_deep_sleep_counter = 0;
        rrc_csp_int_data.agc_threshold_cnt = 0;

        if(!rrc_csp_int_data.deep_sleep_no_svc_tmr_expired)
        {
          WRRC_MSG0_HIGH("Deep Sleep: Stopping no service timer");
          rrctmr_stop_timer(RRCTMR_DEEP_SLEEP_NO_SVC_TIMER);
          rrc_csp_int_data.deep_sleep_no_svc_tmr_expired = TRUE;
        }

        if(!rrc_csp_int_data.full_scan_needed)
        {
          rrctmr_stop_timer(RRCTMR_FREQ_SCAN_TIMER);
          rrc_csp_int_data.full_scan_needed = TRUE;
        }

#ifdef FEATURE_EOOS
        #error code not present
#endif

#ifdef FEATURE_EOOS
        #error code not present
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
        /* Reset the ue_in_conn_oos flag if UE finds a suitable W cell during OOS search. */
        if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS() && !hack_srvc_avail_for_lim_srvc 
           && (RRCMCM_WAIT_STOP_CNF != rrcmcm_get_substate()) && (FALSE == rrccsp_check_for_stop_wcdma_req()))
        {
          /* Send RAB_EST_IND to NAS if UE continues with the existing connection */
          rrcrbe_send_rabm_est_ind_to_upper_layers();
          WRRC_MSG0_HIGH("WRLF: Resetting ue_in_conn_oos flag");
          rrc_csp_int_data.ue_in_conn_oos = FALSE;

          if(!rrc_csp_int_data.oos_splmn_srch_tmr_expd)
          {
            rrctmr_stop_timer(RRCTMR_CONN_MODE_OOS_TIMER);
            rrc_csp_int_data.oos_splmn_srch_tmr_expd = TRUE;
          }
        }
#endif
      } /* Service is available */
    } /* RRC_SERVICE_CNF */
      break;

    case RRC_SERVICE_IND:
    case RRC_SERVICE_IND_FROM_OTA_MSG:
      {

        if(cmd_id == RRC_SERVICE_IND)
        {
          mm_buf_ptr->cmd.rrc_service_ind.is_cell_id_valid = FALSE;
#ifdef FEATURE_FEMTO_CSG
          mm_buf_ptr->cmd.rrc_service_ind.service_info.csg_info.csg_id = SYS_CSG_ID_INVALID;
          mm_buf_ptr->cmd.rrc_service_ind.service_info.csg_info.hybrid_cell = FALSE; 
#endif
        }
        else
        {
          mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = FALSE;
#ifdef FEATURE_FEMTO_CSG
          mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.csg_info.csg_id = SYS_CSG_ID_INVALID;
          mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.csg_info.hybrid_cell = FALSE;
#endif
        }

#ifdef FEATURE_DUAL_SIM
        if(cmd_id == RRC_SERVICE_IND)
        {
          mm_buf_ptr->cmd.rrc_service_ind.as_id = rrc_get_as_id();
        }
        else
        {
          mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.as_id = rrc_get_as_id();
        }
#endif
        /* Fill up header */
        if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
        {
          mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_status = service_status;
          if(rrc_state == RRC_STATE_DISCONNECTED)
          {
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn = *sel_plmn_ptr;
          }
          else
          {
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_type = sel_plmn_ptr->plmn_type;
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0]= 0xFF;
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1]= 0xFF;
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]= 0xFF;
          }

        }
        else if(cmd_id == RRC_SERVICE_IND)   
        {
           mm_buf_ptr->cmd.rrc_service_ind.service_status = service_status;
           mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn = *sel_plmn_ptr;

           /* NAS expects abort indication before service indication*/
           if((service_status == MM_AS_NO_SERVICE)&&(rrc_get_state() == RRC_STATE_DISCONNECTED))
           {
              rrcrce_force_initialize(FALSE);
           }
        }
        else
        {
           WRRC_MSG0_ERROR("DSAC:Should not come here ");
        }

#ifdef FEATURE_WRLF_SYSTEM_SEL
        /* If UE is in connected mode OOS, stop time only if service is found*/
        if (RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
        {
          if(service_status == MM_AS_SERVICE_AVAILABLE)
          {
            rrctmr_stop_timer(RRCTMR_CONN_MODE_OOS_TIMER);
            rrc_csp_int_data.oos_splmn_srch_tmr_expd = TRUE;
            WRRC_MSG0_HIGH("WRLF: Resetting ue_in_conn_oos flag");
            rrc_csp_int_data.ue_in_conn_oos = FALSE;
          }
        }
#else
        if(!rrc_csp_int_data.oos_splmn_srch_tmr_expd)
        {
          rrctmr_stop_timer(RRCTMR_CONN_MODE_OOS_TIMER);
          rrc_csp_int_data.oos_splmn_srch_tmr_expd = TRUE;
        }
#endif
        else if(!rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd)
        {
          rrctmr_stop_timer(RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER);
          rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd = TRUE;
        }

        if(service_status == MM_AS_SERVICE_AVAILABLE)
        {

          WRRC_MSG1_HIGH("Reporting Cell_ID :%d to NAS",acq_entry_ptr->cell_id);

          if(cmd_id == RRC_SERVICE_IND)
          {
            mm_buf_ptr->cmd.rrc_service_ind.is_cell_id_valid = TRUE;
            mm_buf_ptr->cmd.rrc_service_ind.cell_id = acq_entry_ptr->cell_id;
#ifdef FEATURE_FEMTO_CSG
            mm_buf_ptr->cmd.rrc_service_ind.service_info.csg_info.csg_id = acq_entry_ptr->csg_id;
            mm_buf_ptr->cmd.rrc_service_ind.service_info.csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;
#endif
          }
          else
          {
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = TRUE;
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.cell_id = acq_entry_ptr->cell_id;
#ifdef FEATURE_FEMTO_CSG
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.csg_info.csg_id = acq_entry_ptr->csg_id;
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;
#endif
          }

          if(rrc_csp_find_access_control(&access_control, &rrc_dsac_info,
                                         acq_entry_ptr->plmn_id,
                                         acq_entry_ptr->cell_id
                                        ,common_plmn_id
                                        ,&rrc_ppac_info
                                         )
             == FAILURE)
          {
            ERR_FATAL("Failure finding cell access status", 0, 0, 0); 
          }

        /* Get the cell access status of the camped on cell */
          if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
          {
            rrc_csp_int_data.is_ppac_valid = rrc_ppac_info.valid;
            if(rrc_ppac_info.valid == TRUE)
            {
              rrc_csp_int_data.curr_cell_cs_reg_allowed = mm_common_buf_ptr->cmd.rrc_service_cnf.service_info.cs_reg_allowed = rrc_ppac_info.cs_reg_allowed;
              rrc_csp_int_data.curr_cell_ps_reg_allowed = mm_common_buf_ptr->cmd.rrc_service_cnf.service_info.ps_reg_allowed = rrc_ppac_info.ps_reg_allowed;
              rrc_csp_int_data.curr_cell_page_rsp_allowed = mm_common_buf_ptr->cmd.rrc_service_cnf.service_info.page_rsp_allowed = rrc_ppac_info.page_rsp_allowed;
            }
            (void) rrc_csp_find_rrc_cell_access(&(mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info), &rrc_dsac_info,access_control, acq_entry_ptr->status_mask);
            rrc_csp_int_data.is_dsac_valid  = rrc_dsac_info.valid;
            if(! rrc_csp_int_data.is_dsac_valid)
            {
              rrc_csp_int_data.curr_cell_access_status = mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.cell_access;
            }
            else
            {
              rrc_csp_int_data.curr_cs_cell_access = mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.cs_cell_access;
              rrc_csp_int_data.curr_ps_cell_access = mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.ps_cell_access;
            }
          }
          else if(cmd_id == RRC_SERVICE_IND)
          {
            rrc_csp_int_data.is_ppac_valid = rrc_ppac_info.valid;
            if(rrc_ppac_info.valid == TRUE)
            {
              rrc_csp_int_data.curr_cell_cs_reg_allowed = mm_buf_ptr->cmd.rrc_service_cnf.service_info.cs_reg_allowed = rrc_ppac_info.cs_reg_allowed;
              rrc_csp_int_data.curr_cell_ps_reg_allowed = mm_buf_ptr->cmd.rrc_service_cnf.service_info.ps_reg_allowed = rrc_ppac_info.ps_reg_allowed;
              rrc_csp_int_data.curr_cell_page_rsp_allowed = mm_buf_ptr->cmd.rrc_service_cnf.service_info.page_rsp_allowed = rrc_ppac_info.page_rsp_allowed;
            }

            (void) rrc_csp_find_rrc_cell_access(&(mm_buf_ptr->cmd.rrc_service_ind.service_info), 
                                                                    &rrc_dsac_info,access_control, acq_entry_ptr->status_mask);
            rrc_csp_int_data.is_dsac_valid  = rrc_dsac_info.valid;
            if(!rrc_csp_int_data.is_dsac_valid)
            {
              rrc_csp_int_data.curr_cell_access_status = mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access;
            }
            else
            {
              rrc_csp_int_data.curr_cs_cell_access = mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access;
              rrc_csp_int_data.curr_ps_cell_access = mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access;
            }
          }
          else
          {
            WRRC_MSG0_ERROR("DSAC: Shouldn't come here");
          }

          /* Fill up the NAS system info here */
          /* Let us first read SIB1 - we should have SIB1 for sure */
          sib1_ptr = (rrc_SysInfoType1 *) rrc_sibdb_return_sib_for_cell(
                                                    rrc_SIB1,
                                                    common_plmn_id, 
                                                    acq_entry_ptr->cell_id);
          if( sib1_ptr == NULL )
          {
            /* SIB3 for this PLMN, CEll ID combination not present */
            ERR_FATAL("SIB1 for MCC %d, MNC %d, Cell ID %d not present",
                        RRC_CSP_GET_INT_MCC(common_plmn_id), 
                        RRC_CSP_GET_INT_MNC(common_plmn_id),
                                         acq_entry_ptr->cell_id);
          } /* SIB1 not present */

          /* Fill in the GSM common map information only if state is not CELL_FACH/URA_PCH/CELL_PCH
             and if WTOG CCO is not active.If WTOG CCO is active then the SERVICE_IND should
             include the GSM_MAP_COMMON_INFO even if we are in connected mode.NAS would need this 
             information to get the Location Area info */
          if(((rrc_state == RRC_STATE_CELL_FACH) ||
              (rrc_state == RRC_STATE_CELL_PCH) ||
              (rrc_state == RRC_STATE_URA_PCH)) &&
              (FALSE == rrccho_is_wtog_cco_active()))
          {
            WRRC_MSG1_HIGH("DSAC:Excluding gsm_map_common_info in 0x%x", cmd_id);
            if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
            {
              mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.gsm_map_common_info_incl = FALSE;
            }
            else if( cmd_id == RRC_SERVICE_IND)
            {
              mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = FALSE;
            }
            else
            {
              WRRC_MSG0_HIGH("DSAC:Shoudn't come here ");
            }
          }
          else
          {
            if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
            {
              /* Do not fill nas common info in RRC_SERVICE_IND_FROM_OTA_MSG */
              mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.gsm_map_common_info_incl = FALSE;
            }
            else if(cmd_id == RRC_SERVICE_IND)
            {
              mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = TRUE;
              if(rrccsp_fill_nas_common_info(sib1_ptr,
                                             &mm_buf_ptr->cmd.rrc_service_ind.
                                             service_info.gsm_map_common_info)
                 == FAILURE)
              {
                ERR_FATAL("Not able to send MM command",0,0,0);
              }
            }
            else
            {
              WRRC_MSG0_ERROR("DSAC:Shouldn't come here");
            }
          }
          /* Fill in the CN domain specific NAS system info now */
          num_cn_domains = 0;
          idx=0;
          if(cmd_id != RRC_SERVICE_IND_FROM_OTA_MSG)
          {
            cn_domain_sys_info_list_ptr =  &sib1_ptr->cn_DomainSysInfoList; 
            do
            {
              /* First check if Core network is GSM MAP. */
              if(cn_domain_sys_info_list_ptr->elem[idx].cn_Type.t == 
                                                T_rrc_CN_DomainSysInfo_cn_Type_gsm_MAP)
              {
                if(rrccsp_fill_nas_cn_specific_info(&cn_domain_sys_info_list_ptr->elem[idx], 
                                                    &mm_buf_ptr->cmd.rrc_service_ind.
                                                    service_info.cn_domain_info[num_cn_domains])
                   == FAILURE)
                {
                  ERR_FATAL("Not able to send MM command",0,0,0);
                }
                num_cn_domains ++;
              } /* If CN type is GSM MAP */ 
              idx++;
            }while((cn_domain_sys_info_list_ptr->n > idx) && 
                   (num_cn_domains < RRC_MAX_CN_DOMAINS));  
          }
          else
          {
            WRRC_MSG0_HIGH("DSAC: nas cn spec info is not included RRC_SERVICE_IND_FROM_OTA_MSG");
          }

          if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
          {
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.num_cn_domains = num_cn_domains;
          }
          else if(cmd_id == RRC_SERVICE_IND)
          {
            mm_buf_ptr->cmd.rrc_service_ind.service_info.num_cn_domains = num_cn_domains;
          }
          else
          {
            WRRC_MSG0_ERROR("Shouldn't come here");
          }

  /*Populate DSAC information in mm buffer */
          if(rrc_dsac_info.valid == TRUE)
          {
            if(cmd_id == RRC_SERVICE_IND)
            {
               mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present = rrc_dsac_info.valid;
               rrc_csp_int_data.is_dsac_valid  = rrc_dsac_info.valid;
            }
            else if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
            {
               mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.dsac_present = rrc_dsac_info.valid;
               rrc_csp_int_data.is_dsac_valid  = rrc_dsac_info.valid;
            }
          }
          else
          {
            if(cmd_id == RRC_SERVICE_IND)
            {
              mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present = rrc_dsac_info.valid;
              rrc_csp_int_data.is_dsac_valid  = rrc_dsac_info.valid;
            }
            else if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
            {
              mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.dsac_present = rrc_dsac_info.valid;
              rrc_csp_int_data.is_dsac_valid  = rrc_dsac_info.valid;
            }
          }

          if(rrc_ppac_info.valid == TRUE)
          {
            if(cmd_id == RRC_SERVICE_IND)
            {
              mm_buf_ptr->cmd.rrc_service_ind.service_info.ppac_present = rrc_ppac_info.valid;
            }
            else if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
            {
              mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.ppac_present = rrc_ppac_info.valid;
            }
          }
          else
          {
            if(cmd_id == RRC_SERVICE_IND)
            {
              mm_buf_ptr->cmd.rrc_service_ind.service_info.ppac_present = rrc_ppac_info.valid;
            }
            else if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
            {
              mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.ppac_present = rrc_ppac_info.valid;
            }
          }

#ifdef FEATURE_WCDMA_MULTIBAND
          if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
          {
            /* Set the active band */
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.active_band = sys_band;
            rrc_csp_int_data.deep_sleep_band_scanned_mask = 0;

            /* Reset no service band mask to zero since service has been found */
           rrc_csp_int_data.deep_sleep_no_svc_band_scanned_mask = 0;
          }
          else if(cmd_id == RRC_SERVICE_IND)
          {
            /* Set the active band */
            mm_buf_ptr->cmd.rrc_service_ind.active_band = sys_band;
            rrc_csp_int_data.deep_sleep_band_scanned_mask = 0;

            /* Reset no service band mask to zero since service has been found */
            rrc_csp_int_data.deep_sleep_no_svc_band_scanned_mask = 0;
          }
          else
          {
            WRRC_MSG0_ERROR("DSAC:Shouldn't come here");
          }
#endif

          /* Reset the ue_in_deep_sleep variable */
          rrc_csp_int_data.ue_in_deep_sleep = FALSE;
          rrc_csp_int_data.full_scan_in_deep_sleep_counter = 0;
          rrc_csp_int_data.agc_threshold_cnt = 0;

          rrctmr_stop_timer(RRCTMR_FREQ_SCAN_TIMER);
          rrc_csp_int_data.full_scan_needed = TRUE;

#ifdef FEATURE_EOOS
          #error code not present
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
          if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
          {

            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.no_service_cause = SYS_NO_SVC_CAUSE_NONE;
            mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.suitable_search_time = 0;
          }
          else if(cmd_id == RRC_SERVICE_IND)
          {
            mm_buf_ptr->cmd.rrc_service_ind.no_service_cause = SYS_NO_SVC_CAUSE_NONE;
            mm_buf_ptr->cmd.rrc_service_ind.suitable_search_time = 0;
          }
#endif
        } /* Service is available */
#ifdef FEATURE_WCDMA_MULTIBAND
        else
        {
          /* Reset the current_band */
          rrc_csp_curr_select_data.current_band = RRC_CSP_IMT2K_BAND;
          /* Update UI with connected mode OOS as FALSE before sending SERVICE_IND as
             no service to NAS.
          */
#ifdef FEATURE_OOSC_USER_ACTION
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
          if((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)&&
               (rrc_csp_int_data.prev_curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN))
          {
            WRRC_MSG0_HIGH("SVC_ST: Update UI with connected mode OOS status as FALSE");
            rrc_csp_send_svc_status_to_mm(SYS_SRV_STATUS_NONE, SYS_RAT_UMTS_RADIO_ACCESS, 0);
          }
#endif
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
          if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
          {
            mm_buf_ptr->cmd.rrc_service_ind.no_service_cause = SYS_NO_SVC_CAUSE_RLF;
            mm_buf_ptr->cmd.rrc_service_ind.suitable_search_time = 30000;

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
            /* If UE encounters RLF after "fast return to LTE after CSFB" hack kicks in, send CLOSE_SESSION_IND for CS domain  to NAS 
               so that LTE is also searched as part of OOS search. */
            if(csfb_extended_status == CSFB_CS_RB_RELEASED)
            {
              rrcscr_send_close_session_ind(RRC_CS_DOMAIN_CN_ID,RRC_MAX_SESSION_CLOSE_CAUSE);
            }
#endif
          }
          else
          {
            mm_buf_ptr->cmd.rrc_service_ind.no_service_cause = SYS_NO_SVC_CAUSE_NORMAL;
            mm_buf_ptr->cmd.rrc_service_ind.suitable_search_time = 0;
          }
#endif
#ifdef FEATURE_FEMTO_CSG
          if(cmd_id == RRC_SERVICE_IND)
          {
            rrctmr_stop_timer(RRCTMR_CSG_ASF_SRCH_TIMER);
            WRRC_MSG0_HIGH("CSG: Stopped ASF timer on OOS.");
          }
#endif
        }
#endif
#ifdef FEATURE_DUAL_SIM
/*        if(((cmd_id == RRC_SERVICE_IND) && (rrc_state == RRC_STATE_DISCONNECTED)) ||
            ((cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG) && 
              ((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) || 
                (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) ||
                (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_IN_CONNECTING_STATE_SCAN)) &&
                (service_status == MM_AS_NO_SERVICE)
            )
          )*/
         if((cmd_id == RRC_SERVICE_IND) && (rrc_state == RRC_STATE_DISCONNECTED))
        {
         /* Do not release the lock for CSFB */
         if((service_status == MM_AS_NO_SERVICE) 
#ifdef FEATURE_3GPP_CSFB 
            || (!rrc_csfb_call_status)
#endif
           )
         {
          /*Release the lock in disconnected state, after sending Service IND*/
          rrc_release_lock();
        }
#ifdef FEATURE_3GPP_CSFB 
         else
         {
           WRRC_MSG2_HIGH("DSIM: CSFB/ECALL [%d/%d]. Do not release the lock",rrc_csfb_call_status,rrc_csp_int_data.service_req_cause);
         }
#endif         
        }
        if((rrc_state == RRC_STATE_CELL_PCH || rrc_state == RRC_STATE_URA_PCH)
          &&!(rrccu_check_cu_active_before_lock_rel()))
        {
          rrc_release_lock();
        }
        if((
#ifdef FEATURE_WRLF_SYSTEM_SEL
            (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) || 
            (rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) ||
#endif
            (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)) && 
            ((rrc_state == RRC_STATE_CELL_DCH) ||(rrc_state == RRC_STATE_CELL_FACH)) &&\
            (service_status == MM_AS_SERVICE_AVAILABLE) &&
             wcdma_is_ue_in_dual_standby)
        {
          /*Service is found in DCH/FACH state, increase the priority*/
          trm_reason_enum_t priority;
#ifdef FEATURE_DUAL_SIM
          if (rrc_ok_for_tune_away())
          {
            priority = TRM_BG_TRAFFIC;
          }
          else
#endif
          if(rrcrce_get_rrc_est_cause() == RRC_EST_REGISTRATION)
          {
            priority = TRM_ACCESS;
          }
          else
          {
            priority = TRM_ACCESS_URGENT;
          }

          rrc_stop_lock_wait_timer();

          rrc_wrm_change_priority(priority);
#ifdef FEATURE_DUAL_SIM
          wrm_retain_lock_for_primary();
            rrcwrm_update_tune_away_status(priority);
#endif
        }
        if(((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
#ifdef FEATURE_WRLF_SYSTEM_SEL
            || ((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) && 
                (RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()))
#endif
            ) && (service_status == MM_AS_NO_SERVICE))
        {
          rrc_stop_lock_wait_timer();
          rrc_release_lock();
        }

        if(cmd_id == RRC_SERVICE_IND)
        {
          if((rrc_csp_int_data.oos_due_to_no_resource) && (service_status == MM_AS_NO_SERVICE))
          {
            mm_buf_ptr->cmd.rrc_service_ind.acq_status = SYS_ACQ_STATUS_NO_RESOURCE;
            WRRC_MSG0_HIGH("DSIM: Setting acq_status as SYS_ACQ_STATUS_NO_RESOURCE");
          }
          else
          {
            mm_buf_ptr->cmd.rrc_service_ind.acq_status = SYS_ACQ_STATUS_SUCCESS;
          }
        }
#endif
      } /* RRC_SERVICE_IND */
      break;

    default:
      ERR_FATAL("Invalid cmd ID 0x%x for send_mm_cmd", cmd_id, 0, 0); 

  }  /* switch(cmd_id) */

  /* Taking a backup of service indication information */
  if(cmd_id == RRC_SERVICE_IND)
  {
      WCDMA_MEMCPY(&rrc_service_ind_bkup, 
                   sizeof(rrc_service_ind_type),
                   &mm_buf_ptr->cmd.rrc_service_ind, 
                   sizeof(rrc_service_ind_type));  
  }

  /* Put the command on MM queue */
#ifdef FEATURE_FEMTO_CSG
   WRRC_MSG2_HIGH("CSG: Reporting CSG ID: %d, is_hybrid : %d to NAS",acq_entry_ptr->csg_id,rrc_csp_int_data.curr_acq_entry.cell_is_hybrid);
#endif
   MSG_8(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"DSAC/PPAC:params are DSAC Present, Cell Access, CS Cell Access and PS Cell Access %d %d %d %d PPAC valid, CS reg Allowed, PS reg Allowed, Page rsp %d %d %d %d", 
                                                rrc_csp_int_data.is_dsac_valid,
                                                rrc_csp_int_data.curr_cell_access_status,
                                                rrc_csp_int_data.curr_cs_cell_access,
                                                rrc_csp_int_data.curr_ps_cell_access,
                                                rrc_csp_int_data.is_ppac_valid,
                                                rrc_csp_int_data.curr_cell_cs_reg_allowed,
                                                rrc_csp_int_data.curr_cell_ps_reg_allowed,
                                                rrc_csp_int_data.curr_cell_page_rsp_allowed);

   WRRC_MSG2_HIGH("Dispatching out MM cmd / Sending MM cmd 0x%x in rrc state %d", cmd_id, rrc_state);
   if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG)
   {
     MSG_HIGH(" mcc[0]=0x%x, mcc[1]=0x%x, mcc[2]=0x%x in RRC_SERVICE_IND_FROM_OTA_MSG ",
                       mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0],
                       mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1],
                       mm_common_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]);
     rrc_send_rrc_message_to_mm(mm_common_buf_ptr);
   }
   else if(cmd_id == RRC_SERVICE_IND || cmd_id == RRC_SERVICE_CNF)
   {
     rrc_send_rrc_message_to_mm(mm_buf_ptr);
   }
   else
   {
     rrc_send_rrc_message_to_mm(mm_buf_ptr);       /* More defensive check*/
   }

  if(cmd_id == RRC_SERVICE_CNF)
  {
    if(service_status == MM_AS_SERVICE_AVAILABLE)
    {
      rrctmr_stop_timer(RRCTMR_RSSI_BER_TIMER);
      rrc_rssi_ber_timeout_handler();
      WRRC_MSG0_HIGH("On sending Service Cnf with service available to NAS also post RSSI");
    } 
  }

  if(cmd_id == RRC_SERVICE_IND)
  {
    if((service_status == MM_AS_NO_SERVICE) && (rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN))
    {
      /*Resetting leaving connected mode scan for the case when plmn list request follows no-service in service indication
        Or else, we don't handle the immediate plmn list request properly*/
      rrc_csp_int_data.curr_scan = RRC_CSP_CELL_SELECTION_SCAN;
    }
  } 
  
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    if(cmd_id == RRC_SERVICE_IND_FROM_OTA_MSG || cmd_id == RRC_SERVICE_IND || cmd_id == RRC_SERVICE_CNF)
    {
      if(service_status == MM_AS_SERVICE_AVAILABLE)
      {
        /* Get HS Capability Indication */
        new_hs_ind = rrc_get_3g_cell_capability_indicator();
        if(cmd_id == RRC_SERVICE_CNF)
        {
          hsdpa_hsupa_ind = new_hs_ind;
          rrc_send_3g_cell_capability_information(hsdpa_hsupa_ind);
        }
        else
        {
          if (new_hs_ind == hsdpa_hsupa_ind)
          {
            WRRC_MSG1_HIGH("RRCHS:New cell same HS Cap %d",hsdpa_hsupa_ind);
          }
          else
          {
            hsdpa_hsupa_ind = new_hs_ind;
            WRRC_MSG1_HIGH("RRCHS:Send new HS Cap %d", hsdpa_hsupa_ind);
            rrc_send_3g_cell_capability_information(hsdpa_hsupa_ind);
          }
        }
      }
      else
      {
        WRRC_MSG0_HIGH("RRCHS:OOS detected. Change cell status");
        if(cmd_id == RRC_SERVICE_CNF)
        {
          rrc_send_3g_cell_capability_information(SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL);
        }
        else
        {
          if(hsdpa_hsupa_ind != SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL)
          {
            rrc_send_3g_cell_capability_information(SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL);
          }
        }
        hsdpa_hsupa_ind = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
      }
    }
  }
  /* Now set the Last CSP event */
  rrc_csp_int_data.last_mm_primitive = 
    rrc_csp_mm_primitive_sent(cmd_id, service_status);

  /* Cleanup any stored G channels */
#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  if(cmd_id == RRC_SERVICE_CNF)
  {
    /*Clean up number of channels only if the cmd_id is SERVICE_CNF*/
    rrc_csp_int_data.umts_chls_from_gsm.size = 0;
  }
  else
  {
    /*
      Do not cleanup stored G channels if the cmd_id to NAS is not SERVICE_CNF
      This is to care of scenario like, BPLMN passively in progress while sending service_ind/service_ind_ota
    */
    WRRC_MSG0_HIGH("Do not initialize umts_chls_from_gsm");
  }
#endif

#ifdef FEATURE_DUAL_SIM
  rrc_reset_oos_due_to_no_resource_flag();
#endif

} /* rrc_csp_send_mm_cmd */


/*===========================================================================

FUNCTION          RRC_CSP_SEND_MM_PLMN_LIST_CNF_CMD

DESCRIPTION       This function is responsible for filling up data structures
                  for PLMN List Confirm command to MM sent by CSP.
                  This also allocates cmd buffer and sends the command.
                  It also frees allocated command and embedded buffers
                  when failure is detected before sending the command.

DEPENDENCIES
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  For time being it always sends an PLMN_LIST_CNF with the 
                  no PLMNs.
===========================================================================*/
void
rrc_csp_send_mm_plmn_list_cnf_cmd(boolean srch_done)
{
  mm_cmd_type *mm_buf_ptr;
  uint32 i, j;
  rrc_to_mm_sink_e_type   mm_cmd_id; 

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_PLMN_LIST_CNF);


  if(!srch_done)
  {
    rrc_csp_int_data.plmn_list.num_plmns = rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found = 0;
#ifdef FEATURE_WCDMA_TO_LTE
    rrc_csp_wtog_bplmn_int_data.wtol_plmns_found = 0;
#endif
  }

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.as_id = rrc_get_as_id();
#endif

  /* Fill the PLMN list in the CSP internal data into the one 
   * to be sent in PLMN_LIST_CNF to MM
   */
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns = 
                     rrc_csp_int_data.plmn_list.num_plmns + rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found;

#ifdef FEATURE_WCDMA_TO_LTE
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns += rrc_csp_wtog_bplmn_int_data.wtol_plmns_found;
#endif

  if(mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns > RRC_MAX_PLMNS)
  {
    WRRC_MSG1_ERROR("Number of PLMNS %d > RRC_MAX_PLMNS",mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns);
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns = RRC_MAX_PLMNS;
  }

  WRRC_MSG2_HIGH("W PLMNs = %d, GSM PLMNs = %d", rrc_csp_int_data.plmn_list.num_plmns,
                                           rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found
                                           );

  for(i=0; i < rrc_csp_int_data.plmn_list.num_plmns; i++)
  {
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].plmn = rrc_csp_int_data.plmn_list.plmn[i].plmn;
#ifdef FEATURE_FEMTO_CSG
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.csg_id = rrc_csp_int_data.plmn_list.plmn[i].csg_info.csg_id;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.hybrid_cell = rrc_csp_int_data.plmn_list.plmn[i].csg_info.hybrid_cell;

    WRRC_MSG2_HIGH("CSG CSG ID %d  cell is hybrid : %d",mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.csg_id, rrc_csp_int_data.plmn_list.plmn[i].csg_info.hybrid_cell);
#endif
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].rat = rrc_csp_int_data.plmn_list.plmn[i].rat;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].service_capability = rrc_csp_int_data.plmn_list.plmn[i].service_capability;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_quality = rrc_csp_int_data.plmn_list.plmn[i].signal_quality;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_strength = rrc_csp_int_data.plmn_list.plmn[i].signal_strength;

    WRRC_MSG2_HIGH("Adding WCDMA PLMN %d-%d", RRC_CSP_GET_INT_MCC(rrc_csp_int_data.plmn_list.plmn[i].plmn),
                                        RRC_CSP_GET_INT_MNC(rrc_csp_int_data.plmn_list.plmn[i].plmn));
  }

  for(j=0; j < rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found && i < RRC_MAX_PLMNS; j++, i++)
  {
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].plmn = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].plmn;
#ifdef FEATURE_FEMTO_CSG
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.csg_id= 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].csg_info.csg_id;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.hybrid_cell= 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].csg_info.hybrid_cell;
#endif
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].rat  = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].rat;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].service_capability = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].service_capability;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_quality = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].signal_quality;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_strength = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].signal_strength;

    WRRC_MSG2_HIGH("Adding GSM PLMN %d-%d", RRC_CSP_GET_INT_MCC(rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].plmn),
                                  RRC_CSP_GET_INT_MNC(rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].plmn));
  }

#ifdef FEATURE_WCDMA_TO_LTE
  for(j=0; j < rrc_csp_wtog_bplmn_int_data.wtol_plmns_found && i < RRC_MAX_PLMNS; j++, i++)
  {
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].plmn = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].plmn;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].rat  = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].rat;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].service_capability = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].service_capability;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_quality = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].signal_quality;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_strength = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].signal_strength;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].earfcn = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].earfcn;
#ifdef FEATURE_FEMTO_CSG
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.csg_id= 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].csg_info.csg_id;
    WRRC_MSG1_HIGH("CSG: Adding LTE CSG ID %d",mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.csg_id);
#endif

    WRRC_MSG2_HIGH("Adding LTE PLMN %d-%d", RRC_CSP_GET_INT_MCC(rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].plmn),
                                  RRC_CSP_GET_INT_MNC(rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].plmn));
  }
#endif

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.network_select_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.trans_id = rrc_csp_int_data.bplmn_trans_id;
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.service_search = rrc_csp_int_data.service_search_bgnd;

  if(srch_done)
  {
    /* Set the status field to MM_AS_SUCCESS */
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.status = MM_AS_SUCCESS;
  }
  else
  {
    /* Set the status field to MM_AS_ABORTED */
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.status = MM_AS_ABORTED;
  }

  WRRC_MSG2_HIGH("SSB:: Service Search is %d Trans I.D is %d in BPLMN_LIST_CNF",rrc_csp_int_data.service_search_bgnd, rrc_csp_int_data.bplmn_trans_id);
  rrc_send_rrc_message_to_mm(mm_buf_ptr);

  /* Since UE is OOS, determine if NO SERVICE shd be sent to NAS or not */
  rrc_csp_int_data.curr_service_status     = MM_AS_NO_SERVICE;
  rrc_csp_int_data.curr_sel_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  rrc_csp_int_data.umts_chls_from_gsm.size = 0;
#endif

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  rrc_csp_int_data.detected_frequency_list.num_freqs = 0;
#endif

  if(rrc_csp_cmd_to_mm_needed(&mm_cmd_id,
                              rrc_csp_int_data.last_mm_primitive, 
                              rrc_csp_int_data.curr_service_status)
     == TRUE)
  {
    WRRC_MSG2_HIGH("last_mm_prim = %d. Snd cmd %d to NAS", rrc_csp_int_data.last_mm_primitive, mm_cmd_id);

    rrc_csp_send_mm_cmd(mm_cmd_id,
                        rrc_csp_int_data.curr_service_status, 
                        &rrc_csp_int_data.curr_sel_plmn,
                        &rrc_csp_int_data.curr_acq_entry);
  }
#ifdef FEATURE_DUAL_SIM
  rrc_release_lock();
  rrc_stop_lock_wait_timer();
#endif

} /* rrc_csp_send_mm_plmn_list_cnf_cmd */


/*===========================================================================

FUNCTION          RRC_CSP_SEND_MM_PLMN_LIST_CNF_AS_ABORTED

DESCRIPTION       This function is responsible for filling up data structures
                  for PLMN List Confirm command to MM sent by CSP.
                  This function is called when PLMN_LIST_REQ from NAS needs to
                  be aborted immediately without affecting the ongoing BPLMN
                  search (if it is in progress). The n/w select mode in 
                  PLMN_LIST_CNF is always set to MANUAL in this function when
                  feature FEATURE_WTOW_BACKGROUND_PLMN_SEARCH is enabled.

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_send_mm_plmn_list_cnf_as_aborted(uint8 trans_id)
{
  mm_cmd_type *mm_buf_ptr;

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_PLMN_LIST_CNF);

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns = 0;

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.network_select_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;

  /* Set the status field to MM_AS_SUCCESS */
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.status = MM_AS_ABORTED;

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.trans_id = trans_id;

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.as_id = rrc_get_as_id();
#endif

  WRRC_MSG1_HIGH("Manual PLMN Search Aborted. Trans I.D in BPLMN_LIST_CNF=%d",mm_buf_ptr->cmd.rrc_plmn_list_cnf.trans_id );
  rrc_send_rrc_message_to_mm(mm_buf_ptr);
  
} /* rrc_csp_send_mm_plmn_list_cnf_as_aborted */

/*===========================================================================

FUNCTION          RRC_CSP_SEND_MM_PLMN_LIST_CNF_AS_SUCCESS

DESCRIPTION       

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_send_mm_plmn_list_cnf_as_success(uint8 trans_id)
{
  mm_cmd_type *mm_buf_ptr;

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_PLMN_LIST_CNF);

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns = 0;

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;

  /* Set the status field to MM_AS_SUCCESS */
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.status = MM_AS_SUCCESS;

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.trans_id = trans_id;

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.as_id = rrc_get_as_id();
#endif

  rrc_send_rrc_message_to_mm(mm_buf_ptr);
  
} /* rrc_csp_send_mm_plmn_list_cnf_as_success */


/*===========================================================================

FUNCTION          RRC_CSP_BPLMN_SEND_MM_PLMN_LIST_CNF_CMD

DESCRIPTION       This function is responsible for filling up data structures
                  for PLMN List Confirm command to MM sent by CSP. 
                  This function sends both W and G PLMNs found to NAS.

DEPENDENCIES
                  NONE
                  
RETURN VALUE
                  SUCCESS: Command was successfully sent to MM.
                  FAILURE: Command could not be sent due to any one of multiple
                           reasons.

SIDE EFFECTS
                  NONE
===========================================================================*/
void  
rrc_csp_bplmn_send_mm_plmn_list_cnf_cmd(wtow_bplmn_srch_status_type status, uint8 trans_id)
{
  mm_cmd_type *mm_buf_ptr;
  uint32 i, j;  
  boolean list_cnf_for_new_req = FALSE;

  if (trans_id != rrc_csp_int_data.bplmn_trans_id)
  {
    list_cnf_for_new_req = TRUE;
  }

  if((rrc_csp_int_data.send_partial_results) &&
     ( !list_cnf_for_new_req))
  {
    rrc_csp_int_data.plmn_list.num_plmns = rrc_csp_int_data.bplmn_fnd_list_wr_index;
#ifdef FEATURE_WCDMA_TO_LTE
    WRRC_MSG1_HIGH("MPLMN Opt: Partial Results, total plmns %d", 
              rrc_csp_int_data.plmn_list.num_plmns + rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found 
              + rrc_csp_wtog_bplmn_int_data.wtol_plmns_found);
#else
    WRRC_MSG1_HIGH("MPLMN Opt: Partial Results, total plmns %d", 
              rrc_csp_int_data.plmn_list.num_plmns + rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found );
#endif
    
  }
  else
  {
    if(status == WTOW_SEARCH_STOP)
    {
      rrc_csp_int_data.plmn_list.num_plmns = rrc_csp_int_data.bplmn_fnd_list_wr_index;
    }
    else if(status == WTOW_SEARCH_ABORT)
    {
      rrc_csp_int_data.plmn_list.num_plmns = rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found = 0;
#ifdef FEATURE_WCDMA_TO_LTE
      rrc_csp_wtog_bplmn_int_data.wtol_plmns_found = 0;
#endif
    }
    else
    {
      rrc_csp_int_data.plmn_list.num_plmns = rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found = 0;
      ERR_FATAL("ERR: status = %d", 0, 0, 0);
    }                                            
    WRRC_MSG2_HIGH("status = %d, total plmns %d", status,
                    rrc_csp_int_data.plmn_list.num_plmns + rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found);
  }  

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_PLMN_LIST_CNF);
  
#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.as_id = rrc_get_as_id();
#endif

  /* Store W PLMNs */
  for(i=0; i < rrc_csp_int_data.plmn_list.num_plmns && i < RRC_MAX_PLMNS; i++)
  {
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].plmn = rrc_csp_int_data.plmn_list.plmn[i].plmn;
#ifdef FEATURE_FEMTO_CSG
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.csg_id = rrc_csp_int_data.plmn_list.plmn[i].csg_info.csg_id;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.hybrid_cell = rrc_csp_int_data.plmn_list.plmn[i].csg_info.hybrid_cell;

WRRC_MSG2_HIGH("CSG CSG ID %d  cell is hybrid : %d",mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.csg_id, rrc_csp_int_data.plmn_list.plmn[i].csg_info.hybrid_cell);
#endif
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].rat = rrc_csp_int_data.plmn_list.plmn[i].rat;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].service_capability = rrc_csp_int_data.plmn_list.plmn[i].service_capability;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_quality = rrc_csp_int_data.plmn_list.plmn[i].signal_quality;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_strength = rrc_csp_int_data.plmn_list.plmn[i].signal_strength;

    WRRC_MSG2_HIGH("CSG: Adding WCDMA PLMN %d-%d ", RRC_CSP_GET_INT_MCC(rrc_csp_int_data.plmn_list.plmn[i].plmn),
                                        RRC_CSP_GET_INT_MNC(rrc_csp_int_data.plmn_list.plmn[i].plmn));
  }

  /* Store G PLMNs */
  for(j=0; j < rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found && i < RRC_MAX_PLMNS; j++, i++)
  {
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].plmn = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].plmn;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].rat  = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].rat;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].service_capability = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].service_capability;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_quality = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].signal_quality;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_strength = 
                                            rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].signal_strength;

    WRRC_MSG2_HIGH("Adding GSM PLMN %d-%d", RRC_CSP_GET_INT_MCC(rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].plmn),
                                  RRC_CSP_GET_INT_MNC(rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[j].plmn));
  }

#ifdef FEATURE_WCDMA_TO_LTE
  for(j=0; j < rrc_csp_wtog_bplmn_int_data.wtol_plmns_found && i < RRC_MAX_PLMNS; j++, i++)
  {
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].plmn = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].plmn;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].rat  = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].rat;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].service_capability = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].service_capability;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_quality = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].signal_quality;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].signal_strength = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].signal_strength;
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].earfcn = 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].earfcn;
#ifdef FEATURE_FEMTO_CSG
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.csg_id= 
                                            rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].csg_info.csg_id;
    WRRC_MSG1_HIGH("CSG: Adding LTE CSG ID %d",mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[i].csg_info.csg_id);
#endif

    WRRC_MSG2_HIGH("Adding LTE PLMN %d-%d", RRC_CSP_GET_INT_MCC(rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].plmn),
                                  RRC_CSP_GET_INT_MNC(rrc_csp_wtog_bplmn_int_data.lte_plmn_list.plmn[j].plmn));
  }
#endif

  /* Fill the PLMN list in the CSP internal data into the one 
   * to be sent in PLMN_LIST_CNF to MM
   */
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns = i;

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.network_select_mode = rrc_csp_int_data.bplmn_nw_sel_mode;
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.trans_id = trans_id;
  if(status == WTOW_SEARCH_PARTIAL)
  {
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.status = MM_AS_SEARCH_PARTIAL;
    rrc_csp_int_data.send_partial_results    = FALSE;
  }
  else if(status == WTOW_SEARCH_ABORT)
  {
    if (list_cnf_for_new_req == FALSE)
    {
      memset(&rrc_csp_int_data.bplmn_search_timer_value,0,sizeof(sys_manual_search_timer_value_type));
      rrc_csp_int_data.send_partial_results = FALSE;
      rrctmr_stop_timer(RRCTMR_BPLMN_PARTIAL_REPORT_TIMER);
    } 
    /* Set the status field to MM_AS_ABORTED */
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.status = MM_AS_ABORTED;
  }
  else
  {
    if (list_cnf_for_new_req == FALSE)
    {
      memset(&rrc_csp_int_data.bplmn_search_timer_value,0,sizeof(sys_manual_search_timer_value_type));
      rrc_csp_int_data.send_partial_results = FALSE;
      rrctmr_stop_timer(RRCTMR_BPLMN_PARTIAL_REPORT_TIMER);
    }
    /* Set the status field to MM_AS_SUCCESS */
    mm_buf_ptr->cmd.rrc_plmn_list_cnf.status = MM_AS_SUCCESS;
  }

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.service_search = rrc_csp_int_data.service_search_bgnd;

  if(status != WTOW_SEARCH_PARTIAL)
  {
    /* Clear the backed up BPLMN data*/
    bplmn_data_saved = FALSE;
  }
  MSG_HIGH("n/w sel mode is %d, Trans I.D is %d, svc_srch_bgnd is %d BPLMN_LIST_CNF = %d",rrc_csp_int_data.bplmn_nw_sel_mode, trans_id, rrc_csp_int_data.service_search_bgnd);
  rrc_send_rrc_message_to_mm(mm_buf_ptr);
} /* rrc_csp_bplmn_send_mm_plmn_list_cnf_cmd */

/*===========================================================================

FUNCTION          wrrc_get_inter_rat_bplmn_prtl_result

DESCRIPTION       This API provides partial found PLMN results to RR during ongoing GtoW
                  Manual PLMN Search

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean wrrc_get_inter_rat_bplmn_prtl_result(sys_detailed_plmn_list_s_type *available_plmn_list)
{
  int i;
  boolean status;
  rrc_plmn_identity_type nas_to_rrc_plmn_id;
  /* Perform an atomic operation*/
  WCDMA_RRC_TASK_LOCK();
  for(i=0; i<rrc_csp_int_data.bplmn_fnd_list_wr_index; ++i)
  {
    available_plmn_list->info[i].plmn_service_capability
        = rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].plmn_service_capability;
    available_plmn_list->info[i].plmn
        = rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].plmn; 
    available_plmn_list->info[i].rat
        = rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].rat;
    available_plmn_list->info[i].signal_quality
        = rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].signal_quality;
    available_plmn_list->info[i].signal_strength
        = rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].signal_strength;
    available_plmn_list->info[i].list_category = SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER;
    available_plmn_list->info[i].plmn_forbidden = FALSE;
    nas_to_rrc_plmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(available_plmn_list->info[i].plmn);

    MSG_HIGH("MPLMN Opt: plmns in partial result api PLMN %d-%d", RRC_CSP_GET_INT_MCC(nas_to_rrc_plmn_id),
                                                RRC_CSP_GET_INT_MNC(nas_to_rrc_plmn_id), 0);
  }
  
  available_plmn_list->length = rrc_csp_int_data.bplmn_fnd_list_wr_index;
  WCDMA_RRC_TASK_FREE();
  
  WRRC_MSG1_HIGH("MPLMN Opt: Providing %d Parial Results to RR", available_plmn_list->length);

  if(available_plmn_list->length > 0)  
  {
    status = TRUE;
  }
  else
  {
    status = FALSE;
  }
  return(status);
}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION          wrrc_get_inter_rat_bplmn_prtl_result_sub_id

DESCRIPTION       This API provides partial found PLMN results to RR during ongoing GtoW
                  Manual PLMN Search

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean wrrc_get_inter_rat_bplmn_prtl_result_sub_id(sys_detailed_plmn_list_s_type *available_plmn_list, sys_modem_as_id_e_type as_id)
{
  if (as_id != rrc_get_as_id())
  {
    MSG_ERROR("AS-id mismatch %d, internal %d",as_id, rrc_get_as_id(),0);
  }
  
  return(wrrc_get_inter_rat_bplmn_prtl_result(available_plmn_list));
}
#endif
/*===========================================================================

FUNCTION          rrccsp_send_mplmn_partial_report_to_mm

DESCRIPTION       This function is responsible for filling up data structures
                  for PLMN List Confirm command to MM sent by CSP.
                  This function is called when partial MPLMN results needs to be informed to NAS

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrccsp_send_mplmn_partial_report_to_mm(void)
{
  int i; 
  sys_detailed_plmn_list_s_type available_plmn_list;
  lte_irat_get_plmn_prtl_results_req_s plmn_srch_req;
  available_plmn_list.length=0;

  if(rrc_csp_int_data.bplmn_search_timer_value.MorePLMNTimer > 0)
  {
    rrctmr_start_timer(RRCTMR_BPLMN_PARTIAL_REPORT_TIMER, (unsigned long)(1000*rrc_csp_int_data.bplmn_search_timer_value.MorePLMNTimer));
  }
#ifdef FEATURE_WCDMA_TO_LTE  
  if(rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active
     && rrc_csp_int_data.bplmn_interrat_type == RRC_CSP_INTERRAT_LTE)
  {
    rrc_csp_int_data.send_partial_results = TRUE;
    rrc_send_lte_cmd(&plmn_srch_req.msg_hdr, LTE_RRC_WCDMA_GET_PLMN_PRTL_RESULTS_REQ, sizeof(lte_irat_get_plmn_prtl_results_req_s));
  }
  else 
#endif  
  {
  if(rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active
     && rrc_get_rr_inter_rat_bplmn_prtl_result(&available_plmn_list))
  {
    for(i=0; i < available_plmn_list.length; ++i)
    {
      rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[i].plmn = 
            mmumts_convert_nas_plmn_id_to_rrc_plmn_id(
              available_plmn_list.info[i].plmn);
   
      MSG_HIGH("%d-%d, GSM RAT %d", 
                                 RRC_CSP_GET_INT_MCC(rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[i].plmn),
                                 RRC_CSP_GET_INT_MNC(rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[i].plmn), 
                                 available_plmn_list.info[i].rat);
     
      rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[i].rat = 
              available_plmn_list.info[i].rat;
   
      rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[i].service_capability = 
              available_plmn_list.info[i].plmn_service_capability;
     
      rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[i].signal_quality = 
              available_plmn_list.info[i].signal_quality;
   
      rrc_csp_wtog_bplmn_int_data.plmn_list.plmn[i].signal_strength = 
              available_plmn_list.info[i].signal_strength;
    }
    
    rrc_csp_wtog_bplmn_int_data.wtog_gsm_plmns_found = 
                                available_plmn_list.length;
  }

  rrc_csp_int_data.send_partial_results = TRUE;
  rrc_csp_bplmn_send_mm_plmn_list_cnf_cmd(WTOW_SEARCH_PARTIAL, rrc_csp_int_data.bplmn_trans_id);
  }
} /* rrc_csp_send_mm_plmn_list_cnf_as_aborted */

/*===========================================================================

FUNCTION          RRC_CSP_SEND_CCM_CMD

DESCRIPTION       This function is responsible for filling up data structures
                  for Cell Change Request sent to CCM.
                  This also allocates cmd buffer and sends the command.
                  It also frees allocated command and embedded buffers
                  when failure is detected before sending the command.

DEPENDENCIES
                  
RETURN VALUE
                  SUCCESS: Command was successfully sent to CCM.
                  FAILURE: Command could not be sent due to any one of multiple
                           reasons.

SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type
rrc_csp_send_ccm_cmd(rrc_cmd_e_type  ccm_cmd, 
                     rrc_new_cell_status_e_type  cell_camp_status,
                     rrc_csp_acq_entry_type  *acq_entry_ptr)
{

  rrc_cmd_type   *csp_out_cmd_ptr;
  rrc_csp_access_control_e_type  access_control;
  rrc_dsac_info_type rrc_dsac_info;
  rrc_ppac_info_type rrc_ppac_info;

  //mm_cmd_type *mm_buf_ptr;
  rrc_service_info_type dummy_service_info;
  rrc_plmn_identity_type common_plmn_id;
  /* Check validity of Acq Entry pointer */
  if((cell_camp_status == RRC_NEW_CELL_SELECTED) && (acq_entry_ptr == NULL))
  {
    WRRC_MSG0_ERROR("Passed Acq Entry pointer is NULL");
    return(FAILURE);
  }

  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    csp_out_cmd_ptr->cmd_hdr.cmd_id = ccm_cmd;
    csp_out_cmd_ptr->cmd.cell_change_req.req_proc_id = RRC_PROCEDURE_CSP;
    csp_out_cmd_ptr->cmd.cell_change_req.new_cell_camp_status = cell_camp_status;

    if(cell_camp_status == RRC_NEW_CELL_SELECTED)
    {

  if(acq_entry_ptr->is_valid_nw_sharing)
  {
    common_plmn_id = acq_entry_ptr->shared_list_of_plmns.common_plmn_id;
  }
  else
  {
    common_plmn_id = acq_entry_ptr->plmn_id;
  }
      if(rrc_csp_find_access_control(&access_control, &rrc_dsac_info, 
                                     acq_entry_ptr->plmn_id,
                                     acq_entry_ptr->cell_id
                                     ,common_plmn_id
                                     ,&rrc_ppac_info
                                     )
         == FAILURE)
      {
        ERR_FATAL("Failure finding cell access status", 0, 0, 0); 
      }

      /* Fill in the transition type based on the current scan in CSP internal data */
      if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
#ifdef FEATURE_WRLF_SYSTEM_SEL
          && !RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()
#endif
        )
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_CELL_SELECTION;
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_RESELECTION_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_CELL_RESELECTION;
      }
      /* If curr_scan is OOS_area_scan and current RRC state is DCH then it 
       * implies that the cell selection was triggered due to CELL_DCH ->
       * CELL_FACH/CELL_PCH reconfiguration/RL_falire.
       * Hence based on the destination state, send appropriate cell_change_type
       * to CCM
       */
      else if(((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
#ifdef FEATURE_WRLF_SYSTEM_SEL
                   || RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()
#endif
               )  && (RRC_STATE_CELL_DCH == rrc_get_state()))
      {
        if(rrc_csp_int_data.dch_tx_dest_state == RRC_STATE_CELL_FACH)
        {
          csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                             RRC_CHANGE_CELL_DCH_TO_CELL_FACH;
        }
        else if(rrc_csp_int_data.dch_tx_dest_state == RRC_STATE_CELL_PCH)
        {
          csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                             RRC_CHANGE_CELL_DCH_TO_CELL_PCH;
        }
        else
        {
          csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                             RRC_CHANGE_CELL_DCH_TO_URA_PCH;
        }
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_CELL_FACH_TO_CELL_FACH;
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_CELL_FACH_TO_CELL_PCH;
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_CELL_FACH_TO_URA_PCH;
      }
      /* If curr_scan is OOS_area_scan and current RRC state is DCH then it 
       * implies that the cell selection was triggered due to CELL_DCH ->
       * CELL_FACH/CELL_PCH reconfiguration/RL_falire.
       * Hence based on the destination state, send appropriate cell_change_type
       * to CCM
       */
      else if(((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
#ifdef FEATURE_WRLF_SYSTEM_SEL
          || RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()
#endif
        ) && (RRC_STATE_CELL_FACH == rrc_get_state()))
      {
        if(rrc_csp_int_data.fach_tx_dest_state == RRC_STATE_CELL_FACH)
        {
          csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                             RRC_CHANGE_CELL_FACH_TO_CELL_FACH;
        }
        else if(rrc_csp_int_data.fach_tx_dest_state == RRC_STATE_CELL_PCH)
        {
          csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                             RRC_CHANGE_CELL_FACH_TO_CELL_PCH;
        }
        else if(rrc_csp_int_data.fach_tx_dest_state == RRC_STATE_URA_PCH)
        {
          csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                             RRC_CHANGE_CELL_FACH_TO_URA_PCH;
        }
        else
        {
          csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                             RRC_CHANGE_RETURN_TO_SERVICE_AREA;
        }
      }
      else if((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
#ifdef FEATURE_WRLF_SYSTEM_SEL
          || RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()
#endif
        )
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_RETURN_TO_SERVICE_AREA;
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_CELL_DCH_TO_CELL_FACH;
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_CELL_DCH_TO_CELL_PCH;
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_CELL_DCH_TO_URA_PCH;
      }
      else  if(rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                           RRC_CHANGE_CELL_SELECTION;
      } 
      else  if(rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_IN_IDLE_STATE_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                      RRC_CHANGE_CELL_SELECTION;
      }
#ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type = 
                                            RRC_CHANGE_CELL_SELECTION;
      }
#endif
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type = 
                                            RRC_CHANGE_CELL_SELECTION;
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_CC_ORDER_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type = 
                                            RRC_CHANGE_CELL_SELECTION;
      }
      else  if(rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_IN_CONNECTING_STATE_SCAN)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
                                      RRC_CHANGE_RETURN_TO_SERVICE_AREA;
      }
      else  if(rrc_csp_int_data.curr_scan == RRC_CSP_INTER_FREQ_REDIRECTION_SCAN)
      {

        csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type =
            RRC_CHANGE_RETURN_TO_SERVICE_AREA;
      }
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
      #error code not present
#endif
      else
      {
        WRRC_MSG0_ERROR("ERR Unknown scan type received");
      }

      MSG_HIGH("Scan %d,cell chg type %d, RRC st %d", rrc_csp_int_data.curr_scan,
                  csp_out_cmd_ptr->cmd.cell_change_req.cell_change_type,
                  rrc_get_state());

      csp_out_cmd_ptr->cmd.cell_change_req.cell_id = acq_entry_ptr->cell_id;
#ifdef FEATURE_FEMTO_CSG
      csp_out_cmd_ptr->cmd.cell_change_req.csg_id = acq_entry_ptr->csg_id;
#endif
      csp_out_cmd_ptr->cmd.cell_change_req.plmn_id = acq_entry_ptr->plmn_id;
      csp_out_cmd_ptr->cmd.cell_change_req.freq = acq_entry_ptr->freq;
      csp_out_cmd_ptr->cmd.cell_change_req.scr_code = acq_entry_ptr->scr_code;

      /* Now determine the cell access status for this cell */  
      csp_out_cmd_ptr->cmd.cell_change_req.dsac_present = FALSE;
      csp_out_cmd_ptr->cmd.cell_change_req.is_valid_nw_sharing = acq_entry_ptr->is_valid_nw_sharing;
      csp_out_cmd_ptr->cmd.cell_change_req.common_plmn_id = acq_entry_ptr->shared_list_of_plmns.common_plmn_id;
      (void)rrc_csp_find_rrc_cell_access(&dummy_service_info, &rrc_dsac_info,
                                                          access_control,acq_entry_ptr->status_mask);
      if(rrc_dsac_info.valid != TRUE)
      {
         csp_out_cmd_ptr->cmd.cell_change_req.new_cell_access = dummy_service_info.cell_access;
      }
      else
      {
        csp_out_cmd_ptr->cmd.cell_change_req.dsac_present = TRUE;
        csp_out_cmd_ptr->cmd.cell_change_req.new_cs_cell_access = dummy_service_info.cs_cell_access;
        csp_out_cmd_ptr->cmd.cell_change_req.new_ps_cell_access = dummy_service_info.ps_cell_access;
      }
      csp_out_cmd_ptr->cmd.cell_change_req.ppac_present = rrc_ppac_info.valid;
      if(rrc_ppac_info.valid == TRUE)
      {
        csp_out_cmd_ptr->cmd.cell_change_req.new_cell_cs_reg_allowed = rrc_ppac_info.cs_reg_allowed;
        csp_out_cmd_ptr->cmd.cell_change_req.new_cell_ps_reg_allowed = rrc_ppac_info.ps_reg_allowed;
        csp_out_cmd_ptr->cmd.cell_change_req.new_cell_page_rsp_allowed = rrc_ppac_info.page_rsp_allowed;
      }
    } /* if(cell_camp_status == RRC_NEW_CELL_SELECTED) */
  } /* If able to get int cmd buffer */
  else
  {
    return(FAILURE);
  }

#ifdef FEATURE_FEMTO_CSG
  if((rrccsp_get_curr_nas_requested_csg_id() != SYS_CSG_ID_INVALID)&&
     (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
#ifdef FEATURE_WRLF_SYSTEM_SEL
     &&(!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
    )
    {
       if((acq_entry_ptr->csg_id  != SYS_CSG_ID_INVALID))
       {
         rrccsp_add_macro_fp_info_in_acq_db(&rrc_csp_int_data.curr_acq_entry);
         WRRC_MSG0_HIGH("CSG FP: Adding FP info"); 
       }
    }

/* Macro to femto reselections */
  if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_RESELECTION_SCAN)
  {
    if((rrc_ccm_get_curr_camped_csg_id() == SYS_CSG_ID_INVALID)&&
       (csp_out_cmd_ptr->cmd.cell_change_req.csg_id != SYS_CSG_ID_INVALID))
    {
      if(RRC_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_CSP))
      {
        rrc_csp_int_data.last_csg_rat     = RRC_CSG_WCDMA;
        rrc_csp_int_data.fp_macro.freq    = rrc_ccm_get_curr_camped_freq();
        rrc_csp_int_data.fp_macro.pci     = rrc_ccm_get_curr_camped_psc();
        rrc_csp_int_data.fp_macro.plmn    = rrc_ccm_get_curr_camped_plmn();
        rrc_csp_int_data.fp_macro.cell_id = rrc_ccm_get_curr_camped_cell_id();
      }
      else
      {
        rrc_csp_int_data.last_csg_rat = RRC_CSG_NONE;
      }
      rrccsp_add_macro_fp_info_in_acq_db(&rrc_csp_int_data.new_cell_acq_entry);
      WRRC_MSG0_HIGH("CSG FP: Added FP info for macro to femto reselection case"); 
    }
  }

/* Femto to Macro reselections */
  if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_RESELECTION_SCAN)
  {
    if((rrc_ccm_get_curr_camped_csg_id() != SYS_CSG_ID_INVALID)&&
       (csp_out_cmd_ptr->cmd.cell_change_req.csg_id == SYS_CSG_ID_INVALID))
    {
      if(RRC_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_CSP))
      {
        rrc_csp_int_data.last_csg_rat     = RRC_CSG_WCDMA;
        rrc_csp_int_data.fp_macro.freq    = csp_out_cmd_ptr->cmd.cell_change_req.freq;
        rrc_csp_int_data.fp_macro.pci     = (csp_out_cmd_ptr->cmd.cell_change_req.scr_code)>>4;
        rrc_csp_int_data.fp_macro.plmn    = csp_out_cmd_ptr->cmd.cell_change_req.plmn_id;
        rrc_csp_int_data.fp_macro.cell_id = csp_out_cmd_ptr->cmd.cell_change_req.cell_id;
      }
      else
      {
        rrc_csp_int_data.last_csg_rat = RRC_CSG_NONE;
      }
      rrccsp_add_macro_fp_info_in_acq_db(&rrc_csp_int_data.curr_acq_entry);
      WRRC_MSG0_HIGH("CSG FP: Added FP info for macro to femto reselection case"); 
    }
  }
#ifdef FEATURE_LTE_TO_WCDMA
  /* L Macro to Femto reselections */
  if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN)&&
   (rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE))
  {
    if((csp_out_cmd_ptr->cmd.cell_change_req.csg_id != SYS_CSG_ID_INVALID))
    {
      rrccsp_add_macro_fp_info_in_acq_db(&rrc_csp_int_data.curr_acq_entry);
      WRRC_MSG0_HIGH("CSG FP: Added FP info for macro to femto reselection case"); 
    }
  }
#endif
#endif

  /* Send the Internal cmd to RRC */
  rrc_put_int_cmd( csp_out_cmd_ptr );
  return(SUCCESS);

} /* rrc_csp_send_ccm_cmd */


/*===========================================================================

FUNCTION          RRCCSP_GET_SERVING_PCCPCH_PARMS

DESCRIPTION       This function fills up scrambling code and STTD indicator
                  for serving cell PCCPCH in the passed parameters.
           
DEPENDENCIES
                  The caller has to ensure that Cell selection would have 
                  got valid values for scrambling code and STTD indicator
                  before this function call is made.

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_get_serving_pccpch_parms(uint16 *scr_code_ptr, boolean *sttd_ptr)
{
  *scr_code_ptr = rrc_csp_int_data.serving_pccpch_parms.scr_code;
  *sttd_ptr     = rrc_csp_int_data.serving_pccpch_parms.tx_div_ind;
}

/*===========================================================================

FUNCTION          RRCCSP_GET_CURRENT_PLMN_CELL_ID

DESCRIPTION       This function fills up PLMN & Cell ID.
           
DEPENDENCIES
                  The caller has to ensure that this function will not be called for reselection
                  and also only after CELL_TRANSITION_REQ is sent for cell selection.

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_get_current_plmn_cell_id(rrc_plmn_identity_type *plmn_id, rrc_int_cell_id_type *cell_id)
{
  *plmn_id = rrc_csp_int_data.curr_acq_entry.plmn_id;
  *cell_id = rrc_csp_int_data.curr_acq_entry.cell_id;

  if(rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing)
  {
    *plmn_id = rrc_csp_int_data.curr_acq_entry.shared_list_of_plmns.common_plmn_id;
  }

}


/*===========================================================================

FUNCTION          RRCCSP_GET_NEIGHBOR_PCCPCH_PARMS

DESCRIPTION       This function fills up scrambling code
                  for neighbor cell PCCPCH in the passed parameter.
           
DEPENDENCIES
                  The caller has to ensure that Cell selection would have 
                  got valid value for scrambling code
                  before this function call is made.

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_get_neighbor_pccpch_parms(uint16 *scr_code_ptr, rrc_freq_type *freq_ptr)
{
  *scr_code_ptr = rrc_csp_int_data.new_cell_acq_entry.scr_code;
  *freq_ptr     = rrc_csp_int_data.new_cell_acq_entry.freq;
} /* rrccsp_get_neighbor_pccpch_parms */

/*===========================================================================

FUNCTION          RRC_CSP_HANDLE_NO_AVAILABLE_FREQ

DESCRIPTION       This function handles failure to find any frequency for 
                  acquisition. Based on the current service status an dthe last
                  MM primitive exchanged, it determines if and which command
                  needs to be sent to MM.It then sends the command to MM.
           
DEPENDENCIES
                  None
RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void  rrc_csp_handle_no_available_freq(void)
{
  rrc_to_mm_sink_e_type   mm_cmd_id; 

  if(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
  {
    /* Start the full freq scan timer during which no full scans are done
     * for automatic/manual PLMN searches
     */
    rrccsp_check_and_start_plmn_list_full_freq_scan_timer();

    /* Send the PLMN_LIST_CNF to MM */
    rrc_csp_send_mm_plmn_list_cnf_cmd(TRUE);

    /* Reset the CSP internal data */
    rrc_csp_init_int_data();

#ifdef FEATURE_EOOS
    #error code not present
#endif

    return;
  }

  if(rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
  {
    WRRC_MSG1_HIGH("OOS in Conn Mode: Sending No Svc to MM. PLMNs found %d ", rrc_csp_int_data.plmn_list.num_plmns);
  }                           

  if(rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ)
  {
    WRRC_MSG0_HIGH("No freq. found for ANY PLMN");
  }
  else
  {
    WRRC_MSG2_HIGH("No freq. found for PLMN MCC %d MNC %d", 
               RRC_CSP_GET_INT_MCC(rrc_csp_int_data.curr_req_plmn.plmn_id), 
               RRC_CSP_GET_INT_MNC(rrc_csp_int_data.curr_req_plmn.plmn_id));
  }

  /* Send a Service Confirmation back to MM informing that no PLMN was found */
  rrc_csp_int_data.curr_service_status     = MM_AS_NO_SERVICE;
  rrc_csp_int_data.curr_sel_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

  if(rrc_csp_cmd_to_mm_needed(&mm_cmd_id,
                              rrc_csp_int_data.last_mm_primitive, 
                              rrc_csp_int_data.curr_service_status)
     == TRUE)
  {
    rrc_csp_send_mm_cmd(mm_cmd_id,
                        rrc_csp_int_data.curr_service_status, 
                        &rrc_csp_int_data.curr_sel_plmn,
                        &rrc_csp_int_data.curr_acq_entry);
  }

#ifdef FEATURE_GAN
  #error code not present
#endif /* FEATURE_GAN */

}   /* rrc_csp_handle_no_available_freq */

/*===========================================================================

FUNCTION          RRC_CSP_SEND_CHAN_RELEASE

DESCRIPTION       This function sends a Channel Config Request to LLC to 
                  release all channels.
           
DEPENDENCIES
                  None
RETURN VALUE
                  SUCCESS/FAILURE.

SIDE EFFECTS
                  None

===========================================================================*/
void rrc_csp_send_chan_release(void)
{
  rrc_cmd_type        *csp_out_cmd_ptr;
  /* Start with sending an RRC_CHANNEL_CONFIG_REQ to LLC to release all channels.
   */
  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    /* Populate the cmd data */
    csp_out_cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
    csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_DISCONNECTED;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
    csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 0;
    csp_out_cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_CSP;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = FALSE;
    csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = RRCLLC_CHAN_CFG_REASON_NONE;
    rrc_put_int_cmd( csp_out_cmd_ptr );
  }
} /* rrc_csp_send_chan_release */


/*===========================================================================

FUNCTION          rrc_csp_send_chan_reconfig

DESCRIPTION       This function sends a Channel Config Request to LLC to 
                  reconfig channels.
           
DEPENDENCIES
                  None
RETURN VALUE
                  SUCCESS/FAILURE.

SIDE EFFECTS
                  None

===========================================================================*/
uecomdef_status_e_type  rrc_csp_send_chan_reconfig(void)
{
  rrc_state_e_type rrc_state = rrc_get_state();
  rrc_cmd_type        *csp_out_cmd_ptr;
  /* Start with sending an RRC_CHANNEL_CONFIG_REQ to LLC to release all channels.
   */
  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    /* Populate the cmd data */
    csp_out_cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
    csp_out_cmd_ptr->cmd.chan_config_req.next_state = rrc_state;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
    csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 1;
    /* indicate the logical channel to be reconfigured based on 
    the current substate */
    switch (rrc_state)
    {
      case RRC_STATE_CELL_FACH:
      case RRC_STATE_CONNECTING:
        csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = CCCH_RADIO_BEARER_ID;
        csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = RECONFIG_RB;
        break;

      case RRC_STATE_DISCONNECTED:
      case RRC_STATE_CELL_PCH:
      case RRC_STATE_URA_PCH:
        csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = PCCH_RADIO_BEARER_ID;
        csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = RECONFIG_RB;
        break;

      default:
        WRRC_MSG0_ERROR("Should not come here");
        break;
    }
    csp_out_cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_CSP;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = FALSE;
    csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
      RRCLLC_CHAN_CFG_REASON_RECONFIG_PCH_FACH;
    WRRC_MSG2_HIGH("Sending chan config to reconfig rb:%d rrcst:%d",
             csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id,rrc_state);
    rrc_put_int_cmd( csp_out_cmd_ptr );
    return(SUCCESS);
  }
  else
  {
    return(FAILURE);
  }
} /* rrc_csp_send_chan_reconfig */



/*===========================================================================

FUNCTION          RRCCSP_REL_CHLS_AND_GO_TO_IDLE

DESCRIPTION       This function sends a channel config request to release all
                  channels and take UE to idle. It also checks if the channel
                  release will result in a state change to DISCONNECTED from
                  a non-disconnected state so that CSP can wait for the state
                  change indication before sending NO_SERVICE to MM.
           
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate

SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type  rrccsp_rel_chls_and_go_to_idle(void)
{
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG1_HIGH("UE going from %d to IDLE",rrc_state);
  
  rrccsp_cleanup_bplmn_state();

  /* Release all channels and put lower layers to Idle  */
  rrc_csp_send_chan_release();

  if(rrc_state != RRC_STATE_DISCONNECTED)
  {
    /* Set the CSP substate to RRCCSP_CELL_SEL_WT_STATE_CHANGE, where
     * on getting the STATE_CHANGE_IND (to IDLE), CSP will send
     * NO SERVICE to MM.
     */
    WRRC_MSG0_HIGH("CSP waits for state change ind");
    return(RRCCSP_CELL_SEL_WT_STATE_CHANGE);
  }
  /* if BPLMN search is in progress then send to RR - no plmn found */
  else if( (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) 
           || (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN))
  {
    /* Set data that needs to be reported to RR */
    rrc_csp_int_data.rrc_rr_plmn_srch_cnf.srch_done   = TRUE;
    rrc_csp_int_data.rrc_rr_plmn_srch_cnf.hplmn_found = FALSE;
      
    /* stop WCDMA stack */
    rrccsp_send_mode_change_req_to_mcm(RRC_MODE_INACTIVE);
    return(RRCCSP_WT_STOP_MODE_CHANGE_CNF);
  }
  else
  {
    rrc_csp_handle_no_available_freq();
    return(RRCCSP_NO_CELL_SELECTED);
  }
} /* rrccsp_rel_chls_and_go_to_idle */


/*===========================================================================

FUNCTION          RRC_CSP_SEND_NEXT_FREQ_TO_L1

DESCRIPTION       This function sends either an Acquisition Request or a 
                  Frequency scan request to L1 (via LLC) based on the passed
                  type of frequency scan.
                  The parameters for CPHY_ACQ_REQ and CPHY_FREQ_SCAN_REQ are
                  passed in the acq_entry_ptr.
           
DEPENDENCIES
                  None
RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void  rrc_csp_send_next_freq_to_l1
(
 rrc_csp_acq_entry_type       *acq_entry_ptr,
 rrc_csp_freq_scan_e_type     type_freq_scan
)
{
  if(type_freq_scan != RRC_CSP_NO_SCAN)
  {
    /* Start with sending an RRC_RESET_L1_REQ to L1 before a CPHY_ACQ_REQ or a 
     * CPHY_FREQ_SCAN_REQ for Raw Scan.
     */

    if( ((rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
          && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
        || ((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
            && (RRC_CAMPED_ON != rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_CSP))))
    {
      rrc_csp_send_l1_idle_req();
    }
  }
  else
  {
    ERR_FATAL("type_freq_scan set to NO_SCAN",0,0,0);
  }

  if(type_freq_scan == RRC_CSP_ACQ_SCAN)
  {
#ifdef FEATURE_EOOS
    #error code not present
#endif

    /* Send a new acq_mode to l1 so that l1 can try acquisition on the 
     * freq/psc multiple times for this special case: DCH-->FACH/PCH
     * redirection w/ freq & psc information.
     */
    if((   (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN)
        || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN)
        || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN))
       /* Following check is added to pass GCF TC 34.123 : 8.2.6.8
          Send INTER_F_REDIR for num_tries_on_dest_freq = 5,4, i.e two re-tries 
          These two will be PSC specific search, and the later searches will be full search on the freq */
       && (rrc_csp_curr_select_data.num_tries_on_dest_freq > 3)
       && (rrc_csp_int_data.dest_freq == acq_entry_ptr->freq)
       && (rrc_csp_int_data.dest_freq_present)
       && (rrc_csp_int_data.dest_psc_present)
      )
    {
      rrc_csp_int_data.acq_mode = L1_WCDMA_INTER_F_REDIRECTION_ACQ;
      rrc_csp_int_data.acq_type = L1_WCDMA_FREQ_SCR;

      WRRC_MSG0_HIGH("Setting acq_mode to INTER_F_REDIRECTION_ACQ");
    }                              
    else
    {
      /* Send ACQ_REQ to L1 and indicate to them that they can send a list of scrambling codes in
       * the ACQ_CNF
       */
      rrc_csp_int_data.acq_mode = L1_WCDMA_ACQ_DET_LIST;
      rrc_csp_int_data.acq_type = L1_WCDMA_FREQ_ONLY;
    }

    WRRC_MSG2_HIGH("Sending Acq_mode: %d, acq_type: %d to L1",rrc_csp_int_data.acq_mode,rrc_csp_int_data.acq_type);

    /* Initialize the current acquisition count in CSP internal database */
    rrc_csp_int_data.curr_acq_count = 0;
    rrc_csp_int_data.num_scr_code_det = 0;

    rrc_csp_send_l1_cmd(RRC_PROCEDURE_CSP, CPHY_ACQ_REQ, acq_entry_ptr);
    MSG_MED("L1 cmd 0x%x sent", CPHY_ACQ_REQ, 0, 0);
    }
  else  /* Type of scan - Not Acquisition */
  {
    /* Send a frequency scan request */
    rrc_csp_send_l1_freq_scan_cmd(RRC_PROCEDURE_CSP, acq_entry_ptr->freq, type_freq_scan);
  } /* Type of scan - Not Acquisition */

} /* rrc_csp_send_next_freq_to_l1 */


/*===========================================================================

FUNCTION          rrc_csp_send_tr_phy_chl_release

DESCRIPTION       This function sends a Channel Config Request to LLC to 
                  release Transport and Physical channels.
           
DEPENDENCIES
                  None.
RETURN VALUE
                  None.
SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_send_tr_phy_chl_release(void)
{
  rrc_cmd_type        *csp_out_cmd_ptr;
  /* Start with sending an RRC_CHANNEL_CONFIG_REQ to LLC to release all channels.
   */
  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    /* Populate the cmd data */
    csp_out_cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
    csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_DISCONNECTED;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
    /* No operation on RB */
    csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 0;
    /* Release Physical and Transport Channels */
    csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = TRUE;
    csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = TRUE;

    csp_out_cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_CSP;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = FALSE;

    if((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_IN_CONNECTING_STATE_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_INTER_FREQ_REDIRECTION_SCAN) 
#ifdef FEATURE_WRLF_SYSTEM_SEL
       || ((RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()) &&
            (rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN))
#endif
      )
    {
      csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
        RRCLLC_CHAN_CFG_REASON_OUT_OF_SERVICE;
    }
    else
    {
      csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
        RRCLLC_CHAN_CFG_REASON_NONE;
    }

    rrc_put_int_cmd( csp_out_cmd_ptr );
  }
} /* rrc_csp_send_tr_phy_chl_release */


/*===========================================================================

FUNCTION          rrc_csp_send_bch_release

DESCRIPTION       This function sends a Channel Config Request to LLC to 
                  release BCCH/BCH/S_PCCPCH pipe.
           
DEPENDENCIES
                  None.
RETURN VALUE
                  None.
SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_send_bch_release(boolean cnf_required)
{
  rrc_cmd_type        *csp_out_cmd_ptr;
  /* Start with sending an RRC_CHANNEL_CONFIG_REQ to LLC to release all channels.
   */
  WRRC_MSG0_HIGH("Tearing down BCH.");
  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    /* Populate the cmd data */
    csp_out_cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
    csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_DISCONNECTED;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;

    /* No operation on RB */
    csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 1;
    csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = BCCH_S_RADIO_BEARER_ID;
    csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = RELEASE_RB;

    /* Release Physical and Transport Channels */
    csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
    csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;

    csp_out_cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_CSP;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = cnf_required;

    csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
      RRCLLC_CHAN_CFG_REASON_NONE;

    rrc_put_int_cmd( csp_out_cmd_ptr );
  }
} /* rrc_csp_send_bch_release */

/*===========================================================================

FUNCTION          RRC_CSP_CMD_TO_MM_NEEDED

DESCRIPTION       This function determines if there is a need to send a 
                  command to MM. If a command is needed to be sent, then
                  which command is needed is also determined.
                  The determination is made based on the last primitive
                  exchanged between MM and CSP and the new service status.
           
DEPENDENCIES
                  None
RETURN VALUE
                  TRUE: Command needs to be sent.
                  FALSE: No command needed.

SIDE EFFECTS
                  None

===========================================================================*/
boolean  rrc_csp_cmd_to_mm_needed
( 
  rrc_to_mm_sink_e_type  *mm_cmd_id,
  rrc_csp_mm_primitive_e_type  last_mm_primitive, 
  mm_as_service_state_T      new_service_status
)
{
  boolean  cmd_to_mm = FALSE;
  rrc_state_e_type rrc_state = rrc_get_state();

#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif

  switch(last_mm_primitive)
  {
    case RRC_CSP_ACT_REQ_RCVD:
      cmd_to_mm = TRUE;
      break;

    case RRC_CSP_SRVC_REQ_RCVD:
      cmd_to_mm = TRUE;
      *mm_cmd_id = RRC_SERVICE_CNF;
      break;

    case RRC_CSP_ACT_CNF_SENT:
      cmd_to_mm = TRUE;
      *mm_cmd_id = RRC_SERVICE_IND;
      break;

    case RRC_CSP_SRVC_CNF_WITH_NO_SRVC_SENT:
    case RRC_CSP_SRVC_IND_WITH_NO_SRVC_SENT:
      /* Since a Service Indication with NO_SERVICE was sent before,
       * another Service Indication is sent only when service is available.
       */
      if(new_service_status == MM_AS_SERVICE_AVAILABLE)
      {
        cmd_to_mm = TRUE;
        *mm_cmd_id = RRC_SERVICE_IND;
      }
      break;

    case RRC_CSP_SRVC_CNF_WITH_SRVC_SENT:
    case RRC_CSP_SRVC_IND_WITH_SRVC_SENT:
      cmd_to_mm = TRUE;
      *mm_cmd_id = RRC_SERVICE_IND;
      break;

    case RRC_CSP_PLMN_LIST_REQ_RCVD:
      cmd_to_mm = TRUE;
      *mm_cmd_id = RRC_SERVICE_IND;
      break;

    default:
      *mm_cmd_id = RRC_MM_SINK_CMD_MAX;
      ERR("Invalid event %d when sending MM cmd", last_mm_primitive, 0, 0);
      break;
  }

  /* Don't send the RRC_SERVICE_IND in any state other than DISCONNECTED or
   * CONNECTING
   */
  if(*mm_cmd_id == RRC_SERVICE_IND)
  {
    if((rrc_state != RRC_STATE_DISCONNECTED) && (rrc_state != RRC_STATE_CONNECTING))
    {
      cmd_to_mm = FALSE;
      WRRC_MSG1_HIGH("Svc ind NOT sent to MM in RRC state %d",rrc_state);
    }
    else
    {
      WRRC_MSG0_HIGH("Sending svc ind to MM");
    }
  }

  /* Notify any location information to the registered callback 
   * if UE is not in Cell_DCH State */
  if(*mm_cmd_id == RRC_SERVICE_IND || *mm_cmd_id == RRC_SERVICE_CNF )
  {
    if (rrc_home_zone_ind_cb_func)
    {
      rrc_trigger_home_zone_ind();
    }
  }

  /* Notify any location information to the registered callback 
   * if UE is not in Cell_DCH State */
  if(*mm_cmd_id == RRC_SERVICE_IND || *mm_cmd_id == RRC_SERVICE_CNF )
  {
    /* Notify the serving cell id change indication to the registered callbacks 
     * if UE is not in Cell_DCH State */
    rrc_notify_cell_id_change();
  }
  return(cmd_to_mm);
}   /* rrc_csp_cmd_to_mm_needed */

/*===========================================================================

FUNCTION          rrc_csp_mm_primitive_sent

DESCRIPTION       This function determines the new MM primitive to be sent
                  based on the command for MM and service status.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Last CSP-MM primitive.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_mm_primitive_e_type  rrc_csp_mm_primitive_sent
(
  rrc_to_mm_sink_e_type  last_mm_cmd_id,
  mm_as_service_state_T      last_service_status
)
{
  rrc_csp_mm_primitive_e_type  mm_primitive;

  switch(last_mm_cmd_id)
  {
    case RRC_SERVICE_CNF:
      if(last_service_status == MM_AS_SERVICE_AVAILABLE)
      {
        mm_primitive = RRC_CSP_SRVC_CNF_WITH_SRVC_SENT;
      }
      else
      {
        mm_primitive = RRC_CSP_SRVC_CNF_WITH_NO_SRVC_SENT;
      }
      break;

    case RRC_SERVICE_IND:
      if(last_service_status == MM_AS_SERVICE_AVAILABLE)
      {
        mm_primitive = RRC_CSP_SRVC_IND_WITH_SRVC_SENT;
      }
      else
      {
        mm_primitive = RRC_CSP_SRVC_IND_WITH_NO_SRVC_SENT;
      }
      break;
    case RRC_SERVICE_IND_FROM_OTA_MSG:
      if(last_service_status == MM_AS_SERVICE_AVAILABLE)
      {
        mm_primitive = RRC_CSP_SRVC_IND_WITH_SRVC_SENT;
      }
      else
      {
        mm_primitive = RRC_CSP_SRVC_IND_WITH_NO_SRVC_SENT;
      }
      break;

    default:
      WRRC_MSG1_ERROR("MM cmd %d: No CSP event", last_mm_cmd_id);
      mm_primitive = RRC_CSP_SRVC_IND_WITH_SRVC_SENT;
      break;
  }

  return(mm_primitive);

} /* rrc_csp_mm_primitive_sent */

#ifdef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING

/*===========================================================================

FUNCTION: RRC_CSP_GET_GSM_RR_ARFCNS

DESCRIPTION: This function is called during frequency scanning when UE is out 
             of service. This function in turn calls GSM-RR function to get GSM 
             acquired channels. This information is then used to eliminate WCDMA
             channels to speed-up frequency scanning.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rrc_csp_get_gsm_rr_arfcns(void)
{
  rr_rrc_gsm_cell_list_type gsm_cell_list;

  if(rrcmcm_is_dualmode_enabled())
  {
#ifdef FEATURE_DUAL_SIM
    (void) rr_rrc_get_acq_chnl_info_sub_id(&gsm_cell_list, rrc_get_as_id());
#else
    (void) rr_rrc_get_acq_chnl_info(&gsm_cell_list);
#endif
  }
  else
  {
    gsm_cell_list.size = 0;
  }  

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  /* Map ARFCNs in GSM list to UMTS channels */
  rrccsp_convert_active_gsm_channels_to_umts_uarfcns(gsm_cell_list);
#endif
} /* rrc_csp_get_gsm_rr_arfcns */

#endif /* FEATURE_UMTS_ACQ_CHNL_INFO_SHARING */


/*===========================================================================

FUNCTION          rrc_csp_init_data_from_service_req

DESCRIPTION       This function stores the data passed in RRC_SERVICE_REQ
                  and initializes RRC data and CSP local data with the passed
                  data.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void  rrc_csp_init_data_from_service_req(rrc_service_req_type  *rrc_service_req)
{
  uint32 i;
#ifdef FEATURE_EOOS
  #error code not present
#endif
  sys_specialization_t	specialization;
#ifndef TEST_FRAMEWORK
  policyman_status_t  status = POLICYMAN_STATUS_ERR;
  policyman_item_t	  *pItem;
#endif
  rrc_state_e_type rrc_state = rrc_get_state();
  /* Initilize relevant fields of Internal data here */
  rrc_csp_int_data.last_mm_primitive      = RRC_CSP_SRVC_REQ_RCVD;
  rrc_csp_int_data.curr_req_plmn          = rrc_service_req->requested_plmn;
  rrc_csp_int_data.curr_service_status    = MM_AS_NO_SERVICE;

  rrc_csp_int_data.skip_gsm_search = rrc_service_req->grat_scan_done;
  rrc_csp_int_data.can_ehplmn_be_camped = rrc_service_req->is_ehplmn_to_be_camped;

#ifdef FEATURE_WCDMA_SKIP_GSM_FOR_SVC_SCAN
  /*Skip prioritizing GSM during service request handling*/
  rrc_csp_int_data.skip_gsm_search = TRUE;
#endif

#ifdef FEATURE_FEMTO_CSG
  if(rrc_service_req->requested_plmn.csg_id != SYS_CSG_ID_INVALID)
  {
    rrc_csp_int_data.skip_gsm_search = TRUE;
    /* Unbar all non-white CSG cells */
    if(!rrccsp_is_csg_white(rrc_service_req->requested_plmn.csg_id,rrc_service_req->requested_plmn.plmn_id))
    {
      rrc_csp_int_data.bar_non_white_csg = TRUE;
      rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_FREQ_BARRED,0);
    }
  }
#endif
  WRRC_MSG2_HIGH("SKIP_GSM: in Service Req is %d from NAS %d",rrc_csp_int_data.skip_gsm_search,rrc_service_req->grat_scan_done);

  /* Initialize the num_plmns to zero for every service request */
  rrc_csp_int_data.plmn_list.num_plmns = 0;

  /* Set this var to FALSE. It signifies that RRC has NOT found any PLMN yet */
  rrc_csp_int_data.plmn_fnd_during_fgnd_svc_srch = FALSE;

  /* Initialize the service capability to SYS_SRV_CAPABILITY_NONE for every service request */
  for(i=0; i< RRC_MAX_PLMNS; i++)
  {
    rrc_csp_int_data.plmn_list.plmn[i].service_capability = SYS_SRV_CAPABILITY_NONE;
  }

  rrc_csp_int_data.curr_sel_plmn.plmn_type   = RRC_GSM_MAP_PLMN_SEL;
  rrc_csp_int_data.forbid_lai_list = rrc_service_req->forbid_lai_list;

  rrc_csp_int_data.network_select_mode = rrc_service_req->network_select_mode;

  rrc_csp_int_data.band_pref           = rrc_service_req->band_pref;

#ifdef FEATURE_RAT_PRIORITY_LIST
  rrc_csp_int_data.rat_pri_list_info = rrc_service_req->rat_pri_list_info;
  rrc_csp_int_data.band_pref = rrc_extract_gw_band_pref_from_service_req(&rrc_csp_int_data.rat_pri_list_info);
#endif
  if(rrc_csp_int_data.power_up_service_req)
  {
    // get PM item for Device configuration
#ifndef TEST_FRAMEWORK
    policyman_item_id_t ids[] = {POLICYMAN_ITEM_DEVICE_CONFIGURATION};
    status = policyman_get_items(ids, ARR_SIZE(ids), (policyman_item_t const **) &pItem);	  
  

    // query for specialization with PM item received for Device configuration
    if(status == POLICYMAN_STATUS_SUCCESS)
    {
      policyman_device_config_get_specialization(pItem, &specialization);
    }

    // Release reference count on PM item received for Device configuration irrespective of returned status value
    policyman_item_release(pItem);

    // USE specialization as it contains the correct enum for the sys_specialization_t
    if(status == POLICYMAN_STATUS_SUCCESS)
    {
      search_order_specialization = specialization;
    }
#endif
    WRRC_MSG0_HIGH("Search Order Specialization : Policyman specialization Initialization during Power UP");
    rrc_csp_int_data.power_up_service_req = FALSE;
  } 

/* Hack */
  if(search_order_specialization  == SYS_SPECIAL_SBM_RAT_SEARCH_ORDER)
  {
    WRRC_MSG0_HIGH("Search Order Specialization : Policyman specialization enabled");
    if(!rrc_csp_internal_service_req)
    {
      WRRC_MSG0_HIGH("Search Order Specialization : Backing up Service Request");
      WCDMA_MEMCPY(&rrc_csp_service_req, 
                   sizeof(rrc_service_req_type),
                   rrc_service_req, 
                   sizeof(rrc_service_req_type));
    }
    if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_XI_1500)
    {
      WRRC_MSG0_HIGH("Search Order Specialization : Scan Band 11 first");
      rrc_csp_int_data.band_pref = SYS_BAND_MASK_WCDMA_XI_1500;
    }
    else if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_IX_1700)
    {
      WRRC_MSG0_HIGH("Search Order Specialization : Scan Band 9 ");
      rrc_csp_int_data.band_pref = SYS_BAND_MASK_WCDMA_IX_1700;
    }
  }

  rrc_csp_int_data.srvc_req_scan_scope = rrc_service_req->scan_scope;
  rrc_csp_int_data.service_req_cause = rrc_service_req->service_req_cause;
  MSG_HIGH("WRLF Network Select Mode = %d, Service req cause : %d, Scan Scope : %d",rrc_csp_int_data.network_select_mode, rrc_csp_int_data.service_req_cause, rrc_csp_int_data.srvc_req_scan_scope);

#ifdef FEATURE_WRLF_SYSTEM_SEL
  /* Do not initialize RCE if connected mode OOS search is in progress*/
  if(!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
  {
  /* As we have got a new service request, Initialize RCE just in case it was waiting for UE to camp on,
   * e.g., RRC_EST_REQ. Abort is sent to MM by this function, if needed.
   */
  rrcrce_force_initialize(TRUE);
  }

  /*CR-151*/
  if((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
     (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
  {
    /* Sending IDLE_REQ before BAR_REQ which takes L1 to IDLE. This is needed because 
     * L1 may be in any state when it receives BAR_REQ and BAR_REQ is not handled in all
     * of the L1 states. This is an extra protection in RRC.
     */
    if(!((RRC_STATE_CELL_PCH == rrc_state) || 
         (RRC_STATE_URA_PCH == rrc_state) || 
         (RRC_STATE_DISCONNECTED == rrc_state)))
    {
      rrc_csp_send_l1_idle_req();
    }
    /* Unbar on any cell selection */
    rrc_csp_int_data.bar_for_resel_only = TRUE;
#ifdef FEATURE_FEMTO_CSG
    rrc_csp_int_data.bar_non_white_csg = TRUE;
#endif
    /* Send Cell bar request to L1 to unbar all the frequencies barred due to CR151*/
    rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_FREQ_BARRED,0);
    rrc_csp_int_data.bar_for_resel_only = FALSE;
  }

  /* For emergency call service search:
     - Stop the full scan and deep sleep no service timer.
     - Reset the deep sleep band scanned mask and no service band scanned mask.
     So that for emergency call, UE performs full scan everytime on all bands. 
   * If UE is in deep sleep and full scan timer is still not started with counter value
     less than 3, the counter will get incremented during LIMITED search cycles. As a 
     result the counter may reach max value 3 and start the full scan timer. In this case,
     if the LIMITED search fails UE will not perform full scan for later service requests
     till the full scan timer expires.
   */
  if(((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
     && (rrc_service_req->service_req_cause == RRC_MO_CS_CALL)) ||
     (rrc_service_req->service_req_cause == RRC_MO_CS_ECALL))
  {
    if(!rrc_csp_int_data.full_scan_needed)
    {
      WRRC_MSG0_HIGH("Deep Sleep: Stopping 30mins timer");
      rrctmr_stop_timer(RRCTMR_FREQ_SCAN_TIMER);
      rrc_csp_int_data.full_scan_needed = TRUE;
    }
    if(!rrc_csp_int_data.deep_sleep_no_svc_tmr_expired)
    {
      WRRC_MSG0_HIGH("Deep Sleep: Stopping no service timer");
      rrctmr_stop_timer(RRCTMR_DEEP_SLEEP_NO_SVC_TIMER);
      rrc_csp_int_data.deep_sleep_no_svc_tmr_expired = TRUE;
    }
#ifdef FEATURE_WCDMA_MULTIBAND
    rrc_csp_int_data.deep_sleep_band_scanned_mask = 0;
    rrc_csp_int_data.deep_sleep_no_svc_band_scanned_mask = 0;
#endif
  } 

#ifdef FEATURE_EOOS
  #error code not present
#endif

#ifdef FEATURE_EOOS
  #error code not present
#else
  {
#endif

    rrccsp_update_band_preference();

#ifdef FEATURE_WCDMA_MULTIBAND

    rrc_csp_curr_select_data.prioritized_bands_read_idx = 0;
    rrc_csp_curr_select_data.num_of_prioritized_bands = 0;
    rrc_csp_curr_select_data.first_band_to_be_scanned = RRC_CSP_IMT2K_BAND;

    /* Reset the next_band_mask appropriately */
    WRRC_MSG0_HIGH("Resetting the next_band_mask and addl_chnl_done_mask");
    rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;
    rrc_csp_curr_select_data.addl_chnl_done_mask = 0;

    rrc_csp_curr_select_data.first_band_to_be_scanned = RRC_CSP_BAND_MAX;
    
      if((rrc_csp_curr_select_data.first_band_to_be_scanned =
          rrccsp_get_next_band_to_be_scanned()) == RRC_CSP_BAND_MAX)
      {
        WRRC_MSG0_ERROR("UE doesn't support a valid WCDMA band");
        WRRC_MSG0_HIGH("Defaulting to IMT2k");
        rrc_csp_curr_select_data.first_band_to_be_scanned = RRC_CSP_IMT2K_BAND;
        rrc_csp_curr_select_data.current_band = 
          rrc_csp_curr_select_data.first_band_to_be_scanned;
      }
      else
      {
        rrc_csp_curr_select_data.current_band = 
          rrc_csp_curr_select_data.first_band_to_be_scanned;
        WRRC_MSG1_HIGH("1st band to be scanned = %d",
                   rrc_csp_curr_select_data.first_band_to_be_scanned);
      }

      WRRC_MSG0_HIGH("Resetting the addl_chnl_done_mask");
      rrc_csp_curr_select_data.addl_chnl_done_mask = 0;
      

#endif /* FEATURE_WCDMA_MULTIBAND */

#ifdef FEATURE_EOOS
  #error code not present
#else
  }
#endif

  WRRC_MSG2_HIGH("UE in DS %d, thresh cnt %d", rrc_csp_int_data.ue_in_deep_sleep,
            rrc_csp_int_data.agc_threshold_cnt);

  if (rrc_csp_int_data.ue_in_deep_sleep) 
  {
      // UE is operating in deep sleep mode and the scans performed are for
      // cell selection 11 out of 12 times.

      rrc_csp_int_data.agc_threshold_cnt = 
          ( rrc_csp_int_data.agc_threshold_cnt + 1) % 12; 
  }
  else
  {
      rrc_csp_int_data.agc_threshold_cnt = 0;
  }

#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
  rrc_csp_int_data.gsm_svc_srch_done = FALSE;
#endif

  /* Initialize GSM list size to 0 */
#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  rrc_csp_int_data.umts_chls_from_gsm.size = 0;
#endif

  /* If UE is in deep sleep then store the agc values for LFS and FFS in the current select data of CSP, 
      also store the deep sleep scan type based on the scan type received in the 
      SERVICE_REQ  
  */
  rrc_csp_int_data.do_lfs_scan = TRUE;
#ifdef FEATURE_EOOS
  #error code not present
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
  /* Do not perform LFS scan during connected mode OOS search */
  if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
  {
    rrc_csp_int_data.do_lfs_scan = FALSE;
  }
#endif

    /* Store the NAS requested PLMN Id into CSP current selecte data */
    rrc_csp_curr_select_data.req_plmn.plmn_id = rrc_service_req->requested_plmn.plmn_id;
#ifdef FEATURE_FEMTO_CSG
    rrc_csp_curr_select_data.req_plmn.csg_id = rrc_service_req->requested_plmn.csg_id;
#endif

#ifdef FEATURE_EOOS
  #error code not present
#else /* FEATURE_EOOS */
#ifdef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
  WRRC_MSG0_HIGH("Get GSM Acquired ARFCNs info");
  rrc_csp_get_gsm_rr_arfcns();
  /* Clear RR acq channel db after using it once*/
  WRRC_MSG0_HIGH("Clearing GSM Acquired ARFCNs info after using it once");
  rr_rrc_clear_acq_chnl_info_internal();
#endif
#endif /* FEATURE_EOOS */

#ifdef FEATURE_GAN
  #error code not present
#endif
} /* rrc_csp_init_data_from_service_req */

/*===========================================================================

FUNCTION          rrc_csp_init_bplmn_data

DESCRIPTION       This function stores the data passed in RRC_INTERRAT_PLMN_SRCH_REQ
                  and initializes RRC data and CSP local data.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void  rrc_csp_init_bplmn_data(void)
{
  uint32 i;

  /* Initialize relevant fields of Internal data here */
  if(rrc_csp_int_data.bplmn_new_srch) 
  {
    rrc_csp_int_data.curr_service_status          = MM_AS_NO_SERVICE;
    /* No primitive received from MM */
    rrc_csp_int_data.last_mm_primitive            = RRC_CSP_NO_EVENT_RCVD;  

    /* If 3G->3G BPLMN search is not in progress then initialize
     * 2G->3G BPLMN search data
     */
    if((rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
        && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {
      /* Initialize the service capability to SYS_SRV_CAPABILITY_NONE for every service request */
      for(i = 0; i < SYS_PLMN_LIST_MAX_LENGTH; i++)  
      {
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].plmn_service_capability
                                                        = SYS_SRV_CAPABILITY_NONE;
      }
    }

    rrc_csp_int_data.curr_sel_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

    /* Process the special case when MM does not give UE's Low Access Class */
    if(rrc_lo_access_class == RRC_LO_ACCESS_CLASS_ABSENT)
    {
      //Since UE's Access Class is missing, the Requested PLMN type will be changed
      //to Any PLMN Request if it is not so already. Take care that this is  done 
      //only if Original Requested PLMN type was not ANSI 41.    
      if(rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_SPECIFIC_PLMN_REQ)
      {
        WRRC_MSG0_HIGH("UE Access Class Absent: Change Req PLMN type to ANY PLMN");
        rrc_csp_int_data.curr_req_plmn.plmn_type = RRC_GSM_MAP_ANY_PLMN_REQ;
      }
    }

#ifdef FEATURE_WCDMA_MULTIBAND
    rrc_csp_curr_select_data.prioritized_bands_read_idx = 0;
    rrc_csp_curr_select_data.num_of_prioritized_bands = 0;
    /* Reset the next_band_mask appropriately */
    WRRC_MSG0_HIGH("Resetting the next_band_mask and addl_chnl_done_mask");
    rrc_csp_curr_select_data.next_band_mask           = rrc_csp_int_data.wcdma_band_mask;
    rrc_csp_curr_select_data.addl_chnl_done_mask = 0;
    rrc_csp_curr_select_data.first_band_to_be_scanned = RRC_CSP_BAND_MAX;

      if((rrc_csp_curr_select_data.first_band_to_be_scanned =
          rrccsp_get_next_band_to_be_scanned()) == RRC_CSP_BAND_MAX)
      {
        WRRC_MSG0_ERROR("UE doesn't support a valid WCDMA band. Defaulting to IMT2k");
        rrc_csp_curr_select_data.first_band_to_be_scanned = RRC_CSP_IMT2K_BAND;
        rrc_csp_curr_select_data.current_band = rrc_csp_curr_select_data.first_band_to_be_scanned;
      }
      else
      {
        rrc_csp_curr_select_data.current_band = rrc_csp_curr_select_data.first_band_to_be_scanned;
        WRRC_MSG1_HIGH("1st band to be scanned = %d",rrc_csp_curr_select_data.first_band_to_be_scanned);
      }

      WRRC_MSG0_HIGH("Resetting the addl_chnl_done_mask");
      rrc_csp_curr_select_data.addl_chnl_done_mask = 0;


#endif /* FEATURE_WCDMA_MULTIBAND */
    WRRC_MSG2_HIGH("UE in DS %d, thresh cnt %d", rrc_csp_int_data.ue_in_deep_sleep,rrc_csp_int_data.agc_threshold_cnt);

    if (rrc_csp_int_data.ue_in_deep_sleep) 
    {
      // UE is operating in deep sleep mode and the scans performed are for
      // cell selection 11 out of 12 times.
      rrc_csp_int_data.agc_threshold_cnt = ( rrc_csp_int_data.agc_threshold_cnt + 1) % 12; 
    }
    else
    {
      rrc_csp_int_data.agc_threshold_cnt = 0;
    }
  }
} /* rrc_csp_init_bplmn_data */



/*===========================================================================

FUNCTION          rrc_csp_start_sel_on_mm_req

DESCRIPTION       This function starts a cell selection in response to
                  a newly received service request from MM. The acquisition 
                  type is initialized based on the requested PLMN type. The 
                  current scan is set to Cell selection scan.
                  Based on the returned frequency scan type, a CSP substate
                  is returned.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_on_mm_req(void)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;

#ifndef FEATURE_EOOS
#ifdef FEATURE_GAN
  #error code not present
#endif /* FEATURE_GAN */
#endif

#if defined (FEATURE_LTE_TO_WCDMA) || defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_WCDMA_DEDICATED_PRI_INFO)
  rrcmeas_send_get_dedicated_priority_req();
#endif

  /* Set current scan to "Cell Selection".*/   
  rrc_csp_int_data.curr_scan = RRC_CSP_CELL_SELECTION_SCAN;

#ifdef FEATURE_LTE_TO_WCDMA
  if(rrc_deprioritization_db.is_deprio_db_valid == TRUE)
  {
     /*Send LTE_RRC_DEPRI_REQUESTED_PLMN_IND to LTE if plmn type is Spec PLMN*/
     if(rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_SPECIFIC_PLMN_REQ)
     {
          lte_rrc_depri_requested_plmn_ind_s lte_deprio_plmn_ind;
          lte_deprio_plmn_ind.requested_plmn = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(rrc_csp_int_data.curr_req_plmn.plmn_id);
           /*Send the message to MSGR*/
           /*LTE_RRC_DEPRI_REQUESTED_PLMN_IND*/
          rrc_send_lte_cmd(&lte_deprio_plmn_ind.msg_hdr,LTE_RRC_DEPRI_REQUESTED_PLMN_IND,sizeof(lte_rrc_depri_requested_plmn_ind_s));
          WRRC_MSG0_HIGH("Deprio: LTE_RRC_DEPRI_REQUESTED_PLMN_IND sent to LTE RRC");
     }
  }
#endif


#ifdef FEATURE_WCDMA_MULTIBAND

#ifdef FEATURE_EOOS
  #error code not present
#else
  /* Update GSM with UE's band preference */
  rr_rrc_set_band_pref_internal(rrc_csp_int_data.band_pref);

  /* Reset the next_band_mask appropriately */
  WRRC_MSG0_HIGH("Resetting the next_band_mask and addl_chnl_done_mask");
  rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;

  rrc_csp_curr_select_data.addl_chnl_done_mask = 0;

#endif /* FEATURE_EOOS  */

#endif /* FEATURE_WCDMA_MULTIBAND */

  rrc_csp_curr_select_data.num_available_freq = 0;
  
  if(rrc_csp_start_cell_selection(&rrc_csp_int_data.curr_req_plmn, 
                                  &rrc_csp_int_data.curr_type_acq, 
                                  &rrc_csp_int_data.curr_acq_entry,
                                  &type_freq_scan) 
    == FAILURE)
  {
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
    if(type_freq_scan == RRC_CSP_GSM_SCAN)
    {
      /* Send idle request to L1 followed by suspend request */
      rrc_csp_send_l1_idle_req();
      /* Send change mode request to MCM */
      rrccsp_send_mode_change_req_to_mcm(RRC_MODE_STANDBY);
      return(RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF);
    }
    else
#endif
#ifdef FEATURE_EOOS
    #error code not present
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
    if(((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) ||
                (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN))
              && RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
    {
      rrc_csp_int_data.curr_service_status     = MM_AS_NO_SERVICE;
      rrc_csp_int_data.curr_sel_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

      if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
      {
        rrc_csp_send_mm_cmd(RRC_SERVICE_CNF,
                               rrc_csp_int_data.curr_service_status, 
                               &rrc_csp_int_data.curr_sel_plmn,
                            &rrc_csp_int_data.curr_acq_entry);
      }
      else /* curr_scan = RRC_CSP_LEAVING_CONN_MODE_SCAN*/
      {
        rrc_csp_send_mm_cmd(RRC_SERVICE_IND,
                            rrc_csp_int_data.curr_service_status, 
                            &rrc_csp_int_data.curr_sel_plmn,
                            &rrc_csp_int_data.curr_acq_entry);
        rrc_csp_send_disable_bplmn_ind(TRUE);
      }
      return RRCCSP_NO_CELL_SELECTED;
    }
    else
#endif
    {
      WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");
      return (rrccsp_rel_chls_and_go_to_idle());
    }
  }


  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    /* This is unacceptable since a SUCCESS was returned */
    ERR_FATAL("NO_SCAN returned", 0, 0, 0);
  }

#ifdef FEATURE_DUAL_SIM
  /* Stop RSSI reporting timer when going to wait for lock state, so that RRC does not report invalid values */
  rrctmr_stop_timer(RRCTMR_RSSI_BER_TIMER);
  rrc_csp_int_data.shld_acq_be_spltd = TRUE;
  rrc_start_lock_wait_timer(RRCTMR_DEFAULT_SEARCH_TIME_IN_MS,TRUE);
  next_substate = rrc_csp_wait_for_wrm_lock(&type_freq_scan);
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    return (next_substate);
  }
#endif

  rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

  if(type_freq_scan == RRC_CSP_ACQ_SCAN)
  {
    /* Start waiting for Acquisition Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_ACQ;
  }
  else
  {
    /* Start waiting for Frequency scan Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
  }
  
  return(next_substate);
}

/*===========================================================================

FUNCTION          L1_RRC_IS_BPLMN_ACTIVE

DESCRIPTION       This function is called by L1 during DRX cycle to determine
                  if L1 should call rrc_can_l1_go_to_sleep.

DEPENDENCIES      NONE

RETURN VALUE      
                  TRUE:  BPLMN is in progress
                  FALSE: BPLMN is not in progress
                                  
SIDE EFFECTS

===========================================================================*/
boolean l1_rrc_is_bplmn_active(void)
{
  boolean ret_sts = FALSE;
  
  if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())  
  {
    ret_sts = TRUE;
  }
  return ret_sts;
}

/*===========================================================================

FUNCTION          RRCCSP_CLEANUP_BPLMN_STATE

DESCRIPTION       This function determines if BPLMN search is active or not.
                  If BPLMN search is active then PLMN_LIST_CNF is sent to NAS
                  with status ABORTED and # of PLMNs as 0 and it is also determined 
                  if CPHY_GO_TO_SLEEP_IND should be sent to L1 or not.
                  If WTOG search is in progress (actively or passively) then this
                  function sends INTERRAT_PLMN_SRCH_ABORT_REQ to GSM
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_cleanup_bplmn_state()
{
  
  /* Check if guard timer has expired or not. Notice that we are additionally checking
   * for scan type here to take care of the race condition where in the CSP substate
   * guard timer expired and in the same or its pending substate another PLMN_LIST_REQ
   * was received from NAS. In this case the guard timer would have expired but the
   * previous BPLMN search was in progress. Checking for scan type allows RRC to reset 
   * the BPLMN state appropriately.
   */
#ifdef FEATURE_DUAL_SIM
  rrc_csp_handle_bplmn_suspend_lock_rel_offset_tmr_running();
#endif
  if((rrc_csp_int_data.bplmn_guard_srch_tmr_expired)
       && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
         && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
         && (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN)
         && (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN)
         && (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN)   
         && (rrc_csp_int_data.curr_scan != RRC_CSP_LEAVING_CONN_MODE_SCAN)         
      )
  {
    WRRC_MSG0_HIGH("WTOW: No BPLMN cleanup required");
  }
  /* Determine if Sleep Indication should be sent or not */
  else
  {
    if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
        || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {
      WRRC_MSG0_HIGH("WTOW: Send sleep ind to L1");
      rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_ABORT, TRUE, TRUE);
    }
    else
    {
      rrc_csp_int_data.bplmn_crit_sec_flag = TRUE;

      if(WTOW_SEARCH_RESUME == rrc_csp_bplmn_srch_in_progress())
      {
        WRRC_MSG0_HIGH("WTOW: Send sleep ind to L1");

        if(!rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active)
        {
          rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_ABORT, TRUE, TRUE);
        }
        else
        {
          rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_ABORT, TRUE, FALSE);
        }
      }
      else if(WTOW_SEARCH_SUSPEND == rrc_csp_bplmn_srch_in_progress())
      {
        WRRC_MSG0_HIGH("WTOW: Do NOT send sleep ind to L1");

        if(rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active
           && !(rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_aborted))
        {
          rrccsp_send_irat_bplmn_abort_req();
        }

        rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_ABORT, TRUE, FALSE);
      }
      /* Handle the cases where guard timer expired when BPLMN is passively in progress*/
      else if((rrc_csp_int_data.bplmn_srch_vars_status == WTOW_SEARCH_SUSPEND) || 
              (rrc_csp_int_data.bplmn_srch_vars_status == WTOW_SEARCH_START))
      {
        WRRC_MSG0_HIGH("Do NOT send sleep ind to L1");

        if(rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active
           && !(rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_aborted))
        {
          rrccsp_send_irat_bplmn_abort_req();
        }

        rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_ABORT, TRUE, FALSE);
      }
      /* This statement will be removed */
      else if((rrc_csp_int_data.curr_scan != RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN)
             && (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN)
             && (rrc_csp_int_data.curr_scan != RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN)   
             && (rrc_csp_int_data.curr_scan != RRC_CSP_LEAVING_CONN_MODE_SCAN)         
             )
      {
        ERR_FATAL("WTOW ERR: should never come here", 0, 0, 0);
      }
      else
      {
        WRRC_MSG0_HIGH("WTOW: No BPLMN cleanup required");
      }
      rrc_csp_int_data.bplmn_crit_sec_flag = FALSE;

#ifdef FEATURE_BPLMN_SEARCH_320MS
      rrc_csp_int_data.bplmn_start_sfn=0;
#endif
    }
  }
} /* rrccsp_cleanup_bplmn_state */

/*===========================================================================

FUNCTION          RRCCSP_CLEANUP_AND_RESTART_BPLMN

DESCRIPTION       This function is called when UE transitions from Idle ->
                  non-idle state and BPLMN n/w sel mode is MANUAL. If BPLMN search 
                  was in progress when this function is called then the BPLMN 
                  search is re-started by resetting appropriate variables. Notice
                  that the guard timer is also reset. This function was added to
                  allow UE to continue Manual search across LAU/RAU when UE
                  returns to Idle/PCH state.                             
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_cleanup_and_restart_bplmn()
{

  /* Check if guard timer has expired or not. Notice that we are additionally checking
   * for scan type here to take care of the race condition where in the CSP substate
   * guard timer expired and in the same or its pending substate another PLMN_LIST_REQ
   * was received from NAS. In this case the guard timer would have expired but the
   * previous BPLMN search was in progress. Checking for scan type allows RRC to reset 
   * the BPLMN state appropriately.
   */
  if((rrc_csp_int_data.bplmn_guard_srch_tmr_expired)
       && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
         && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
  {
    WRRC_MSG0_HIGH("WTOW: No BPLMN cleanup required");
  }
  /* Determine if Sleep Indication should be sent or not */
  else
  {
    rrc_csp_int_data.bplmn_crit_sec_flag = TRUE;

    if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
        || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {
      WRRC_MSG0_HIGH("WTOW: Send sleep ind to L1");
      rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_SUSPEND, FALSE, TRUE);
    }
    else
    {
      if(WTOW_SEARCH_RESUME == rrc_csp_bplmn_srch_in_progress())
      {
        WRRC_MSG0_HIGH("WTOW: Send sleep ind to L1");

        if(!rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active)
        {
          rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_SUSPEND, FALSE, TRUE);
        }
        else
        {
          rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_SUSPEND, FALSE, FALSE);
        }
      }
      else if(WTOW_SEARCH_SUSPEND == rrc_csp_bplmn_srch_in_progress())
      {
        WRRC_MSG0_HIGH("WTOW: Do NOT send sleep ind to L1");

        if(rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active)
        {
          if(!(rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_aborted))
          {
            rrccsp_send_irat_bplmn_abort_req();
          }

          /* Even when WTOG BPLMN search is actively in progress, the bplmn_srch_in_progress()
           * function returns status as suspended because the W DRX timer is NOT active for 
           * WTOG search. So even in this case the search needs to be suspended and then
           * re-started */
          rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_SUSPEND, FALSE, FALSE);
        }
      }
      /* This statement will be removed */
      else
      {
        ERR_FATAL("WTOW ERR: should never come here", 0, 0, 0);
      }
    }

    rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_START, FALSE, FALSE);
    rrccsp_generate_bplmn_rat_srch_order_update_curr_srch_rat(rrc_csp_int_data.bplmn_nw_sel_mode);

    rrc_csp_int_data.bplmn_crit_sec_flag = FALSE;

    if(rrc_csp_int_data.bplmn_nw_sel_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
    {
      if(rrc_csp_int_data.bplmn_wcdma_mode_enabled)
      {
        rrccsp_update_wtow_manual_bplmn_list_with_rplmn(rrc_csp_int_data.curr_acq_entry.plmn_id,
                                                        rrc_csp_int_data.curr_acq_entry.cell_id);
      }
    }
  }

  rrc_csp_wtog_bplmn_int_data.restart_bplmn_search = FALSE;
} /* rrccsp_cleanup_and_restart_bplmn */


/*===========================================================================

FUNCTION          rrc_csp_release_chls_and_start_sel_on_mm_req

DESCRIPTION       This function releases all physical, transport and logical
                  channels before starting a cell selection in response to
                  a newly received service request from MM.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_release_chls_and_start_sel_on_mm_req(void)
{
  rrc_csp_substate_e_type   next_substate;

  rrccsp_cleanup_bplmn_state();

/* Abort PG1 whenever we trigger a new camping procedure */
  rrcpg1_check_and_abort_bcch_mod_info_processing();

  /* First call a CCM function to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */
  rrc_csp_send_chan_release();

  /* As all the channels are released, no previous events can be active. Hence init sib proc events */
  rrc_sib_proc_events_init();

  /* Inter Frequency Redirection Scan was aborted. Clean up its state */
  if(rrc_csp_int_data.curr_scan == RRC_CSP_INTER_FREQ_REDIRECTION_SCAN)
  {
    rrc_csp_cleanup_inter_freq_redirection_state();
  }               

  next_substate = rrc_csp_start_sel_on_mm_req();

  return(next_substate);
}

/*===========================================================================

FUNCTION          rrc_csp_start_bplmn_srch

DESCRIPTION       This function starts a BPLMN search in response to
                  a BPLMN search request from RR (for GTOW searches) or 
                  NAS (for WTOW searches). 
                  
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_bplmn_srch(void)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;
  
#ifdef FEATURE_DUAL_SIM
  uint32 lock_wait_timer = 0;
#endif

#ifdef FEATURE_FEMTO_CSG
  bplmn_cell_found_in_drx = FALSE;
#endif

  /* If new BPLMN search then initialise data*/
  if(rrc_csp_int_data.bplmn_new_srch)
  {   
#ifdef FEATURE_WCDMA_MULTIBAND
    /* Reset the next_band_mask appropriately */
    WRRC_MSG0_HIGH("Resetting the next_band_mask");
    rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;
#endif

    WRRC_MSG0_HIGH("Resetting the addl_chnl_done_mask");
    rrc_csp_curr_select_data.addl_chnl_done_mask = 0;

    rrc_csp_curr_select_data.num_available_freq = 0;
  }

  if(rrc_csp_start_cell_selection(&rrc_csp_int_data.curr_req_plmn, 
                                  &rrc_csp_int_data.curr_type_acq, 
                                  &rrc_csp_int_data.curr_acq_entry,
                                  &type_freq_scan) 
     == FAILURE)
  {
    WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");

    if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
       || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {

      /* Start the full freq scan timer during which no full scans are done
       * for automatic/manual PLMN searches
       */
      rrccsp_check_and_start_plmn_list_full_freq_scan_timer();

      if((rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx+1 ) >= rrc_csp_int_data.plmn_srch_order.num_rats) 
      {
        rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_STOP, TRUE, TRUE);
      }
      else
      { 
        rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_SUSPEND, FALSE, TRUE);
        rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx++;
        rrc_csp_init_wtog_plmn_srch_int_data_across_rats();
        rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active = TRUE;
#ifdef FEATURE_WCDMA_TO_LTE
        if(rrc_csp_int_data.plmn_srch_order.rat[rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx] == SYS_RAT_GSM_RADIO_ACCESS)
        {
          rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_GSM;
          WRRC_MSG0_HIGH("WTOG : Do BPLMN Srch on GSM");
        }
        else if (rrc_csp_int_data.plmn_srch_order.rat[rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx] == SYS_RAT_LTE_RADIO_ACCESS)
        {
          rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_LTE;
          WRRC_MSG0_HIGH("WTOL: Do BPLMN Srch  on LTE");
        }
#endif
      }

      if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
      {
        next_substate = RRCCSP_CAMPED_ACCEPT_CELL;
      }
      else
      {
        next_substate = RRCCSP_CAMPED_SUIT_CELL;
      }

    }
    else
    {
      return (rrccsp_rel_chls_and_go_to_idle());
    }

  }
  else  /* Another freq found */
  {
    if(type_freq_scan == RRC_CSP_NO_SCAN)
    {
        /* This is unacceptable since a SUCCESS was returned */
        ERR_FATAL("NO_SCAN returned", 0, 0, 0);
    }

#ifdef FEATURE_DUAL_SIM
    if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
       || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {
      /*use remaining_time WTOW DRX timer - 2msec, so that we no need to come out of lock for internal cmds*/
      lock_wait_timer = rrctmr_get_remaining_time(RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER) - 2;
    }
    else if (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
    {
      lock_wait_timer = RRCTMR_DEFAULT_SEARCH_TIME_IN_MS;
    }
    else
    {
      /*use remaining_time GTOW search timer - 2msec, so that we no need to come out of lock for internal cmds*/
      lock_wait_timer = rrctmr_get_remaining_time(RRCTMR_BPLMN_SRCH_TIMER) - 2;
    }
    rrc_start_lock_wait_timer(lock_wait_timer,TRUE);
    rrc_csp_int_data.shld_acq_be_spltd = TRUE;
    next_substate = rrc_csp_wait_for_wrm_lock(&type_freq_scan);
    if(type_freq_scan == RRC_CSP_NO_SCAN)
    {
      return (next_substate);
    }
#endif

    rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

    if(type_freq_scan == RRC_CSP_ACQ_SCAN)
    {
        /* Start waiting for Acquisition Confirmation */
        next_substate = RRCCSP_CELL_SEL_WT_ACQ;
    }
    else
    {
        /* Start waiting for Frequency scan Confirmation */
        next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
    }
  }  /* Another freq found */

  return(next_substate);
} /* rrc_csp_start_bplmn_srch */


/*===========================================================================

FUNCTION          RRC_CSP_RESUME_WTOG_BPLMN_SRCH

DESCRIPTION       This function starts the WTOG BPLMN search by suspending W
                  and sending INTERRAT_PLMN_SRCH_REQ to GSM
                                               
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type rrc_csp_resume_wtog_bplmn_srch(void)
{
  rrc_csp_substate_e_type next_substate;

  if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
  {
    next_substate = RRCCSP_CAMPED_ACCEPT_CELL;
  }
  else
  {
    next_substate = RRCCSP_CAMPED_SUIT_CELL;
  }

  if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOG_BPLMN_AUTOMATIC_SCAN)
     || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOG_BPLMN_MANUAL_SCAN))
  {
    rrc_csp_wtog_bplmn_int_data.wtog_rem_drx_tmr_value = rrctmr_get_remaining_time(RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER);

    /* Check if enough DRX search time remains to do BPLMN search */
    if(rrc_csp_wtog_bplmn_int_data.wtog_rem_drx_tmr_value < 
                          MIN_WTOW_BPLMN_SRCH_TIMER_IN_MS - WTOG_BPLMN_SEARCH_OVERHEAD_TIMER_IN_MS)
    {
      WRRC_MSG2_HIGH("WTOG: remaining DRX tmr %d < min %d. Wait for Next DRX. Reset BPLMN DB", 
                                  rrc_csp_wtog_bplmn_int_data.wtog_rem_drx_tmr_value,
                                  MIN_WTOW_BPLMN_SRCH_TIMER_IN_MS - WTOG_BPLMN_SEARCH_OVERHEAD_TIMER_IN_MS);
      rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_SUSPEND, FALSE, FALSE);
    }
    else
    {
      WRRC_MSG2_HIGH("WTOG: remaining DRX tmr %d >= min %d. Do GSM Search. Suspend W", 
                                  rrc_csp_wtog_bplmn_int_data.wtog_rem_drx_tmr_value,
                                  MIN_WTOW_BPLMN_SRCH_TIMER_IN_MS - WTOG_BPLMN_SEARCH_OVERHEAD_TIMER_IN_MS);

      /* Send change mode request to MCM to standby for suspending W stack */
      rrccsp_send_mode_change_req_to_mcm(RRC_MODE_STANDBY);
      next_substate = RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF;
    }

    /* Stop the DRX Search Timer as this timer will now be started by GSM */
    rrctmr_stop_timer(RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER);
    rrc_csp_int_data.bplmn_srch_tmr_expired = TRUE;
  }
  /* Do GSM PLMN Search in foreground as UE is OOS */
  else if(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
  {
    WRRC_MSG0_HIGH("WTOG OOS: Do GSM srch in fgnd. Send Suspend Req to MCM");

    /* Send change mode request to MCM to standby for suspending W stack */
    rrccsp_send_mode_change_req_to_mcm(RRC_MODE_STANDBY);
    next_substate = RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF;
  }
  /* Incorrect Scan Type. Print Error Message */
  else
  {
    WRRC_MSG1_ERROR("ERR: Incorrect scan type %d in resume_wtog_bplmn_srch fn call", rrc_csp_int_data.curr_scan);
  } 

  return(next_substate);
} /* rrc_csp_resume_wtog_bplmn_srch */


/*===========================================================================

FUNCTION          RRC_CSP_CONTINUE_OOS_IN_CONNECTED_MODE_SEARCH

DESCRIPTION       This function is called when curr scan is OOS in Connected Mode
                  scan and CSP has completed ACQ DB and Full Frequency Scan.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
static rrc_csp_substate_e_type rrc_csp_continue_oos_in_connected_mode_search(
                              rrc_csp_freq_scan_e_type  *type_freq_scan_ptr)
{
  rrc_csp_substate_e_type next_substate = RRCCSP_NO_CELL_SELECTED;

  /*  --------------------
   *  Dual Mode is Enabled
   *  -------------------- 
   */
  if(rrcmcm_is_dualmode_enabled())
  { 
    WRRC_MSG0_HIGH("Dual Mode is enabled");

    /* If at least 1 PLMN is found after 30s and at least 1 search 
     * has been completed on GSM, then return No Service to MM
     */
    if( (rrc_csp_int_data.oos_splmn_srch_tmr_expd)
          && (rrc_csp_int_data.oos_gsm_srch_done)
            && (rrc_csp_int_data.plmn_list.num_plmns > 0) )
    {
      WRRC_MSG1_HIGH("At least 1 W-PLMN found. #W-PLMNs %d. Return No Svc to NAS", 
                                            rrc_csp_int_data.plmn_list.num_plmns);
      rrc_ccm_update_with_cell_loss();
      return (rrccsp_rel_chls_and_go_to_idle());
    }
    else
    {
      /* Start OOS Service Search on GSM */     
      /* Reset W plmns found to 0 */
      rrc_csp_int_data.plmn_list.num_plmns = 0;

      /* Send idle request to L1 followed by suspend request */
      rrc_csp_send_l1_idle_req();

      WRRC_MSG0_HIGH("Sending Chg Mode Req for StandBy to MCM for OOS Svc Srch on G");
      /* Send change mode request to MCM */
      rrccsp_send_mode_change_req_to_mcm(RRC_MODE_STANDBY);
      return(next_substate = RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF);

    }
  } /* Dual Mode is enabled */


  /*  ------------------------
   *  Dual Mode is NOT Enabled
   *  ------------------------
   */
  else
  {
    /* Dual Mode is not enabled. Continue Searching on WCDMA */
    WRRC_MSG0_HIGH("Dual Mode not enabled");

    /* If at least 1 PLMN is found after 30s then return No Service to MM */
    if((rrc_csp_int_data.oos_splmn_srch_tmr_expd)
           && (rrc_csp_int_data.plmn_list.num_plmns > 0))
    {
      WRRC_MSG1_HIGH("At least 1 W-PLMN found. #W-PLMNs %d. Return No Svc to NAS", 
                                            rrc_csp_int_data.plmn_list.num_plmns);
      rrc_ccm_update_with_cell_loss();
      return (rrccsp_rel_chls_and_go_to_idle());
    }


    /* Determine if UE should be put to deep sleep or not. UE is NOT put into deep sleep
     * if GCF flag is enabled.
     */
    else if(   (rrc_csp_int_data.oos_splmn_srch_tmr_expd) 
            && (rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd)
            && (!nv_gcf_test_flag)
           )
    {
      /* Start sleep timer of 30s */
      WRRC_MSG0_HIGH("OOS: Put UE in Deep Sleep. Start sleep tmr");

      /* Increase the sleep time as UE spends more time in OOS */
      rrctmr_start_timer(RRCTMR_CONN_MODE_OOS_DS_TIMER, 
                         RRCTMR_CONN_MODE_OOS_DS_TIMER_IN_MS);
      rrc_csp_int_data.oos_conn_mode_ds_tmr_expd = FALSE;

      rrccsp_send_conn_mode_oos_ds_req();

#ifdef FEATURE_DUAL_SIM
      rrc_release_lock();
#endif

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
      if(rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
      {
        /* Update UI with No Service if No PLMN was found */
        if(!rrc_csp_int_data.oosc_plmn_found)
        {
          WRRC_MSG0_HIGH("SRV_ST: Update UI w/ No SERVICE b4 putting UE to sleep");
          rrc_csp_send_svc_status_to_mm(SYS_SRV_STATUS_NO_SRV,SYS_RAT_UMTS_RADIO_ACCESS, 0);
        }
        /* Reset the oosc_plmn_found var for each search cycle */
        rrc_csp_int_data.oosc_plmn_found=FALSE;
      }
#endif

      if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
      {
        next_substate = RRCCSP_CAMPED_ACCEPT_CELL;
      }
      else
      {
        next_substate = RRCCSP_CAMPED_SUIT_CELL;
      }

      return(next_substate);
    }

    else
    { 
      /* Reset W plmns found to 0 */
      rrc_csp_int_data.plmn_list.num_plmns = 0;
      /* Update UI with No Service if No PLMN was found */
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
      if(!rrc_csp_int_data.oosc_plmn_found)
      {
        WRRC_MSG0_HIGH("SRV_ST: Update UI w/ No SERVICE");
        rrc_csp_send_svc_status_to_mm(SYS_SRV_STATUS_NO_SRV,SYS_RAT_UMTS_RADIO_ACCESS, 0);
      }
      /* Reset the oosc_plmn_found var for each search cycle */
      rrc_csp_int_data.oosc_plmn_found=FALSE;
#endif

#ifdef FEATURE_WCDMA_MULTIBAND
      /* Reset the next_band_mask appropriately */
      WRRC_MSG0_HIGH("Resetting the next_band_mask");
      rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;
#endif

      WRRC_MSG0_HIGH("Resetting the addl_chnl_done_mask");
      rrc_csp_curr_select_data.addl_chnl_done_mask = 0;


      /* Start again with Stored and Initial cell selection. Initiale Selection
       * data so that Selection Controller starts from the begining.
       */
      if(rrc_csp_start_cell_selection(&rrc_csp_int_data.curr_req_plmn, 
                                      &rrc_csp_int_data.curr_type_acq, 
                                      &rrc_csp_int_data.curr_acq_entry,
                                      type_freq_scan_ptr) 
        == FAILURE)
      {
        WRRC_MSG0_ERROR("ERR No freq by Stored Initial cell selection");
        rrc_ccm_update_with_cell_loss();
        return (rrccsp_rel_chls_and_go_to_idle());
      }
      /* Return added to suppress compilation warning.*/
      return next_substate;
    }

  } /* Dual Mode is NOT enabled */

} /* rrc_csp_continue_oos_in_connected_mode_search */



/*===========================================================================

FUNCTION          rrc_csp_continue_cell_selection

DESCRIPTION       This function continues with the existing cell selection 
                  with the existing acquisition type. It continues from the
                  point in the frequency scan or in acquisition database where
                  we were last.
                  If all frequencies are exhausted, a cell selection is 
                  re-started in an acquisition type based on the requested 
                  PLMN type.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_continue_cell_selection(void)
{
  rrc_csp_freq_scan_e_type  type_freq_scan = RRC_CSP_NO_SCAN;

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;
  rrc_csp_acq_entry_type *temp_acq_entry = NULL;
  boolean continue_cell_selection = TRUE;

  boolean oos_enhancement_in_state_tx = FALSE;

  if(rrc_ifreq_rdr_in_prgrs ==TRUE)
  {
     rrclog_wcdma_to_wcdma_redir_end_event_type wtow_redirct_event;
     wtow_redirct_event.status = WTOW_FAILURE;
     event_report_payload(EVENT_WCDMA_TO_WCDMA_REDIRECTION_END, sizeof(wtow_redirct_event),
                 (void *)&wtow_redirct_event);
     rrc_ifreq_rdr_in_prgrs = FALSE;
  }

  if(  (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN )
    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN  )
    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN   )  
    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN )
    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN  )
    || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN   )  
    )
  {
    oos_enhancement_in_state_tx = TRUE;
  }

#ifdef FEATURE_EOOS
  #error code not present
#endif

  if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) &&
      (rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_LIST_SCAN))
  {
    /*Get the next entry*/
    /*>if valid send the ACQ req to L1*/
    /*>Else*/
    /*>>If the timer didnt expire, continue the List scan*/
    /*>>If timer expired, behave as if search exhausted, initiate mode change request to STOP, wt for mode change cnf*/
    if(((temp_acq_entry = rrc_csp_get_list_db_match_acq_entry(&type_freq_scan))  != NULL))
    {
      rrc_csp_int_data.curr_acq_entry = *temp_acq_entry;
      /*Send acquisition request to L1 */
      type_freq_scan = RRC_CSP_ACQ_SCAN;
#ifdef FEATURE_WCDMA_MULTIBAND
      rrc_csp_curr_select_data.current_band = temp_acq_entry->band_class;
#endif
      continue_cell_selection = FALSE;
    }
    else
    {
      /* During G2W redirection if any of the target cell belongs to forbidden LA/PLMN or is a barred cell
       * abort the redirection and go back to GSM to do suitable cell search on source RAT
       */
      if(rrc_csp_int_data.skip_repeat_scan_for_redir)
      {
        rrc_csp_int_data.rsp_to_rr = SEND_GTOW_REDIRECTION_REJECT;
        rrccsp_send_mode_change_req_to_mcm(RRC_MODE_INACTIVE);
        next_substate = RRCCSP_WT_STOP_MODE_CHANGE_CNF;
        return next_substate;
      }

      /* In the case of G2W redirection target cell list needs to be looked for atleast 10s 
       * where as in the case of L2W redirection there is no such timer
       */
      if((!rrc_csp_int_data.gtow_redirection_timer_expired)
#ifdef FEATURE_LTE_TO_WCDMA
           && (rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_GSM)
#endif
        )
      {
        if(rrc_csp_int_data.gtow_redir_retry_lfs_scan)
        {
          /*copy the target list to list_db*/
          (void)rrc_csp_lfs_update_list_db_with_target_fdd_list();
          rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_LIST_SCAN;
          WRRC_MSG1_HIGH("LFS_COOS: set curr frequency scan type =%d",
                 rrc_csp_curr_select_data.scan_info.type_freq_scan);
          type_freq_scan = RRC_CSP_LIST_SCAN;
          continue_cell_selection = FALSE;
          rrc_csp_int_data.gtow_redir_retry_lfs_scan = FALSE;
        }
        else
        {
          rrctmr_start_timer(RRCTMR_GTOW_REDIRECTION_LFS_TIMER,RRCTMR_GTOW_REDIRECTION_LFS_TIMER_IN_MS);
#ifdef FEATURE_DUAL_SIM
          rrc_release_lock();
#endif
          return RRCCSP_CELL_SEL_WT_ACQ;
        }
      }
      else
      {
#ifdef FEATURE_LTE_TO_WCDMA
        if(rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE)
        {
          WRRC_MSG0_HIGH("Interrat Redirection: Timer expired, Start ACQ DB Scan + FFS.");
          rrc_csp_int_data.do_lfs_scan= FALSE;
          rrc_csp_curr_select_data.acq_db_in_use = TRUE;
          rrc_csp_curr_select_data.acq_db_scan_info.start_with_first = TRUE;
          rrc_csp_init_freq_scan_data();
        }
        else
#endif
        {
          rrctmr_stop_timer(RRCTMR_GTOW_REDIRECTION_LFS_TIMER);
          /* If no cell is found during G2W redirection within 10s of redirection time abort the redirection
           * and go back to GSM to do suitable cell search. GSM spec says suitable cell search needs to be done
           * but doesn't actually specify the RAT on which the search needs to be performed
           */
          WRRC_MSG0_HIGH("Interrat Redirection: Timer expired, Abort redirection and go back to GSM.");
          rrc_csp_int_data.rsp_to_rr = SEND_GTOW_REDIRECTION_REJECT;
          rrccsp_send_mode_change_req_to_mcm(RRC_MODE_INACTIVE);
          next_substate = RRCCSP_WT_STOP_MODE_CHANGE_CNF;
          return next_substate;
        }
      }
    }
  }
  else
  if(rrc_csp_curr_select_data.scan_info.type_freq_scan == RRC_CSP_LIST_SCAN ) 
  {
    if((temp_acq_entry = rrc_csp_get_list_db_match_acq_entry(&type_freq_scan))  != NULL)
    {
      rrc_csp_int_data.curr_acq_entry = *temp_acq_entry;
      /*Send acquisition request to L1 */
      type_freq_scan = RRC_CSP_ACQ_SCAN;
#ifdef FEATURE_WCDMA_MULTIBAND
      rrc_csp_curr_select_data.current_band = temp_acq_entry->band_class;
#endif
      continue_cell_selection = FALSE;
    }
    /* Once ACQ DB search is completed report immediate no service */
    else if((rrc_csp_int_data.srvc_req_scan_scope == SYS_SCAN_SCOPE_ACQ_DB) &&
               (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN))
    {
      rrc_csp_handle_no_available_freq();
      return(RRCCSP_NO_CELL_SELECTED);
    }
#ifndef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
    else if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
               && (rrcmcm_is_dualmode_enabled())
                 && (!rrc_csp_int_data.gsm_svc_srch_done)
                   && (!rrc_csp_int_data.ue_in_deep_sleep)
                     && (rrccsp_do_gsm_bands_scan_first())
                       && (!rrc_csp_int_data.skip_gsm_search)
                         && (!rrc_csp_int_data.acq_db_empty)
             )
    {
      WRRC_MSG1_HIGH("PCS/850/900 Acq Db Exhausted. Scantype %d. Srch for service on GSM", 
                                                                    rrc_csp_int_data.curr_scan);
      type_freq_scan = RRC_CSP_GSM_SCAN;
      /* Send idle request to L1 followed by suspend request */
      rrc_csp_send_l1_idle_req();
    
      /* Send change mode request to MCM */
      rrccsp_send_mode_change_req_to_mcm(RRC_MODE_STANDBY);
      return(next_substate = RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF);
    }
#endif /*FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION*/
#endif
#ifdef FEATURE_WCDMA_MULTIBAND
    else
    {
      /* When UE is in deep sleep and full scan timer is running then do full scan for 
       * only those bands whose frequencies are not present in ACQ DB.
       */
      WRRC_MSG2_HIGH("LFS_COOS deep_sleep_acq_db_band_mask %d deep_sleep_band_scanned_mask %d",
          rrc_csp_int_data.deep_sleep_acq_db_band_mask,rrc_csp_int_data.deep_sleep_band_scanned_mask);
      if((rrc_csp_int_data.ue_in_deep_sleep)  &&
         (!rrc_csp_int_data.full_scan_needed) &&
         (!rrc_csp_int_data.acq_db_empty)     &&
         ((rrc_csp_int_data.deep_sleep_acq_db_band_mask ^ rrc_csp_int_data.wcdma_band_mask) != 0) &&
         (((rrc_csp_int_data.deep_sleep_band_scanned_mask & rrc_csp_int_data.wcdma_band_mask)
                ^ rrc_csp_int_data.wcdma_band_mask) == 0)
        )
      {
        rrc_csp_curr_select_data.next_band_mask = 
                 rrc_csp_int_data.deep_sleep_acq_db_band_mask ^ rrc_csp_int_data.wcdma_band_mask;
        rrc_csp_curr_select_data.first_band_to_be_scanned = 0;

        /* Remove ACQ-DB bands from the prioritized band list. */
        if(rrc_csp_curr_select_data.num_of_prioritized_bands != 0)
        {
          rrccsp_remove_acqdb_bands_from_prioritized_list();
          rrc_csp_curr_select_data.prioritized_bands_read_idx = 0;
        }
        rrc_csp_curr_select_data.current_band = rrccsp_get_next_band_to_be_scanned();
        MSG_HIGH("acq_db_mask = %d, next_mask = %d, flag_mask = %d",
                                            rrc_csp_int_data.deep_sleep_acq_db_band_mask,
                                            rrc_csp_curr_select_data.next_band_mask,
                                            rrc_csp_int_data.deep_sleep_band_scanned_mask);
        rrc_csp_init_freq_scan_data();
        rrc_csp_init_addl_chnl_scan_data();
      }
    }
#endif
  }
  /* On WTOG OOS redirection search failure and UE continuing search on W,LFS needs to be performed. In this case
      freq scan will not be LFS and num_freq>0. num_freq is initialized in rrc_csp_init_select_data().
      Also follow LFS scan if cell selection fails on NV DL freq.*/
  /*  Also perform LFS when UE is in deep sleep for foreground PLMN searches after resuming from G*/
  else if((rrc_csp_curr_select_data.scan_info.list_scan.num_freq > 0) &&
            ((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
               ||(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
               || ((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) && rrc_csp_int_data.wcdma_dl_freq_enabled)))
  {
    rrc_csp_curr_select_data.scan_info.type_freq_scan = RRC_CSP_LIST_SCAN;
    WRRC_MSG1_HIGH("LFS_COOS: set curr frequency scan type =%d",
           rrc_csp_curr_select_data.scan_info.type_freq_scan);                 
    type_freq_scan = RRC_CSP_LIST_SCAN;
    continue_cell_selection = FALSE;
  }

  if((continue_cell_selection) && (rrc_csp_continue_stored_initial_select_ctrl
      (
        &rrc_csp_int_data.curr_type_acq, 
        &rrc_csp_int_data.curr_acq_entry,
        &type_freq_scan
      )   == FAILURE
    ))
  {
    if(rrc_csp_int_data.curr_scan == RRC_CSP_INTER_FREQ_REDIRECTION_SCAN) 
    {
#ifdef FEATURE_WCDMA_MULTIBAND
      /* Reset the next_band_mask appropriately */
      WRRC_MSG0_HIGH("Resetting the next_band_mask");
      rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;
#endif

      WRRC_MSG0_HIGH("Resetting the addl_chnl_done_mask");
      rrc_csp_curr_select_data.addl_chnl_done_mask = 0;


      /* Start again with Stored and Initial cell selection. Initiale Selection
       * data so that Selection Controller starts from the begining.
       */
      if(rrc_csp_start_cell_selection(&rrc_csp_int_data.curr_req_plmn, 
                                      &rrc_csp_int_data.curr_type_acq, 
                                      &rrc_csp_int_data.curr_acq_entry,
                                      &type_freq_scan) 
        == FAILURE)
      {
        WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");

        /* First call a CCM function to indicate Cell Loss - no new cell */
        /* NOTE: The functional I/F has to be used very carefully. */
        rrc_ccm_update_with_cell_loss();
        return (rrccsp_rel_chls_and_go_to_idle());
      }
    }

    /* OOS handling in Connected Mode */
    else if(rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
    {
      next_substate = rrc_csp_continue_oos_in_connected_mode_search(&type_freq_scan);

      if(   (next_substate == RRCCSP_CELL_SEL_WT_STATE_CHANGE)
         || (next_substate == RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF)
         || (next_substate == RRCCSP_CAMPED_SUIT_CELL)
         || (next_substate == RRCCSP_CAMPED_ACCEPT_CELL))
      {
        WRRC_MSG1_HIGH("OOS: next_substate %d", next_substate);
        return next_substate;
      } 
    }

    /* OOS handling in Connected Mode */
    else if(oos_enhancement_in_state_tx)
    {
      /* Dual Mode is enabled */
      if(rrcmcm_is_dualmode_enabled())
      { 
        WRRC_MSG1_HIGH("Dual Mode is enabled. Srch on GSM. curr_scan %d", rrc_csp_int_data.curr_scan);

        /* Start OOS Service Search on GSM */     

        /* Reset W plmns found to 0 */
        rrc_csp_int_data.plmn_list.num_plmns = 0;

        /* Send idle request to L1 followed by suspend request */
        rrc_csp_send_l1_idle_req();
      
        WRRC_MSG0_HIGH("Sending Chg Mode Req for StandBy to MCM for OOS Svc Srch on G");
        /* Send change mode request to MCM */
        rrccsp_send_mode_change_req_to_mcm(RRC_MODE_STANDBY);
        return(next_substate = RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF);    
      }
      /* Dual Mode is not enabled */
      else
      {
        /* Dual Mode is not enabled. Continue Searching on WCDMA */
        WRRC_MSG1_HIGH("Dual Mode not enabled. Restart srch on W. curr_scan %d", rrc_csp_int_data.curr_scan);

        /* Reset W plmns found to 0 */
        rrc_csp_int_data.plmn_list.num_plmns = 0;

#ifdef FEATURE_WCDMA_MULTIBAND
        /* Reset the next_band_mask appropriately */
        WRRC_MSG0_HIGH("Resetting the next_band_mask");
        rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;
#endif

        WRRC_MSG0_HIGH("Resetting the addl_chnl_done_mask");
        rrc_csp_curr_select_data.addl_chnl_done_mask = 0;

        /* Start again with Stored and Initial cell selection. Initiate Selection
         * data so that Selection Controller starts from the begining.
         */
        if(rrc_csp_start_cell_selection(&rrc_csp_int_data.curr_req_plmn, 
                                        &rrc_csp_int_data.curr_type_acq, 
                                        &rrc_csp_int_data.curr_acq_entry,
                                        &type_freq_scan) 
          == FAILURE)
        {
          WRRC_MSG0_ERROR("ERR No freq by Stored Initial cell selection");
          rrc_ccm_update_with_cell_loss();
          return (rrccsp_rel_chls_and_go_to_idle());
        }
      }
    }

#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION     
    else if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
             && (type_freq_scan == RRC_CSP_GSM_SCAN))
    {
      /* Send idle request to L1 followed by suspend request */
      rrc_csp_send_l1_idle_req();
      /* Send change mode request to MCM */
      rrccsp_send_mode_change_req_to_mcm(RRC_MODE_STANDBY);
      return(next_substate = RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF);
    }
#endif

    /* If WtoW BLMN search is in progress then send plmn list confirmation
     * to NAS and sleep indication to L1
     */
    else if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
            || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {
      WRRC_MSG0_HIGH("WTOW: no more freq fnd. W Search completed");

      /* Start the full freq scan timer during which no full scans are done
       * for automatic/manual PLMN searches
       */
      rrccsp_check_and_start_plmn_list_full_freq_scan_timer();

      if((rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx+1 ) >= rrc_csp_int_data.plmn_srch_order.num_rats) 
      {
        rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_STOP, TRUE, TRUE);
      }
      else
      { 
        rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_SUSPEND, FALSE, TRUE);
        rrc_csp_init_wtog_plmn_srch_int_data_across_rats();
        rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active = TRUE;

        rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx++;
#ifdef FEATURE_WCDMA_TO_LTE
        if(rrc_csp_int_data.plmn_srch_order.rat[rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx] == SYS_RAT_GSM_RADIO_ACCESS)
        {
          rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_GSM;
          WRRC_MSG0_HIGH("WTOG : Do BPLMN Srch on GSM");
        }
        else if (rrc_csp_int_data.plmn_srch_order.rat[rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx] == SYS_RAT_LTE_RADIO_ACCESS)
        {
          rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_LTE;
          WRRC_MSG0_HIGH("WTOL: Do BPLMN Srch on LTE");
        }
#endif
      }

      if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
      {
        next_substate = RRCCSP_CAMPED_ACCEPT_CELL;
      }
      else
      {
        next_substate = RRCCSP_CAMPED_SUIT_CELL;
      }
 
      return (next_substate);
    }
    /* Do BPLMN Search on GSM in foreground */
    else if((rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN) 
            && 
            ((rrc_csp_int_data.bplmn_dual_mode_enabled)
#ifdef FEATURE_WCDMA_TO_LTE
             || (rrc_csp_int_data.bplmn_lte_mode_enabled)
#endif
            ))
            
    {
      /* Start the full freq scan timer during which no full scans are done
       * for automatic/manual PLMN searches
       */
      rrccsp_check_and_start_plmn_list_full_freq_scan_timer();

      if((rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx+1 ) >= rrc_csp_int_data.plmn_srch_order.num_rats) 
      {
        /* First call a CCM function to indicate Cell Loss - no new cell */
        /* NOTE: The functional I/F has to be used very carefully. */
        rrc_ccm_update_with_cell_loss();
        return (rrccsp_rel_chls_and_go_to_idle());
      }
      /* Search not yet done on GSM. Do GSM Manual PLMN search */
      else
      {
        /* Send idle request to L1 followed by suspend request */
        rrc_csp_send_l1_idle_req();
        rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx++;
        rrc_csp_init_wtog_plmn_srch_int_data_across_rats();
#ifdef FEATURE_WCDMA_TO_LTE
        if(rrc_csp_int_data.plmn_srch_order.rat[rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx] == SYS_RAT_GSM_RADIO_ACCESS)
        {
          rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_GSM;
          WRRC_MSG0_HIGH("WTOG OOS: Do BPLMN Srch in fgnd on GSM");
        }
        else if (rrc_csp_int_data.plmn_srch_order.rat[rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx] == SYS_RAT_LTE_RADIO_ACCESS)
        {
          rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_LTE;
          WRRC_MSG0_HIGH("WTOL OOS: Do BPLMN Srch in fgnd on LTE");
        }
#endif
        return(rrc_csp_resume_wtog_bplmn_srch());
      }
    }

    else if((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) 
            && (WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
            && (rrc_csp_int_data.bplmn_nw_sel_mode == SYS_NETWORK_SELECTION_MODE_MANUAL))
    {
      WRRC_MSG0_HIGH("WTOW: LeavingConnMode no more freqs. Do Pending Manual Search");
      next_substate = rrccsp_continue_bplmn_in_frgrnd();

      return next_substate;
    } 
#ifdef FEATURE_EOOS
    #error code not present
#endif

    else if(rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
    {
      rrc_csp_int_data.rsp_to_rr = SEND_GTOW_REDIRECTION_REJECT;
      rrccsp_send_mode_change_req_to_mcm(RRC_MODE_INACTIVE);
      return (RRCCSP_WT_STOP_MODE_CHANGE_CNF);
    }
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
    #error code not present
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
    else if(((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) ||
                (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN))
              && RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
    {
      rrc_csp_int_data.curr_service_status     = MM_AS_NO_SERVICE;
      rrc_csp_int_data.curr_sel_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

      if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
      {
        rrc_csp_send_mm_cmd(RRC_SERVICE_CNF,
                            rrc_csp_int_data.curr_service_status, 
                            &rrc_csp_int_data.curr_sel_plmn,
                            &rrc_csp_int_data.curr_acq_entry);
      }
      else /* curr_scan = RRC_CSP_LEAVING_CONN_MODE_SCAN*/
      {
        rrc_csp_send_mm_cmd(RRC_SERVICE_IND,
                            rrc_csp_int_data.curr_service_status, 
                            &rrc_csp_int_data.curr_sel_plmn,
                            &rrc_csp_int_data.curr_acq_entry);
        /* Send disable BPLMN IND to NAS to disable periodic BPLMN searches.*/
        rrc_csp_send_disable_bplmn_ind(TRUE);
      }
      return RRCCSP_NO_CELL_SELECTED;
    }
#endif
    else /* For all other scans... */
    {
      /* First call a CCM function to indicate Cell Loss - no new cell */
      /* NOTE: The functional I/F has to be used very carefully. */
      rrc_ccm_update_with_cell_loss();
      return (rrccsp_rel_chls_and_go_to_idle());
    }
  }  /* No frequency found */

  /* Another freq found */
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    /* This is unacceptable since a SUCCESS was returned */
    ERR_FATAL("NO_SCAN returned", 0, 0, 0);
  }
 
#ifdef FEATURE_DUAL_SIM
  if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) ||
     (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN) ||
     (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) ||
     (!dsim_bplmn_search_complete_w_cycle && 
      ((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)||
       (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)))||
     (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)||
     (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
#ifdef FEATURE_WRLF_SYSTEM_SEL
     || ((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) && 
                (RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()))
#endif
     || ((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) 
#ifdef FEATURE_3GPP_CSFB
        &&(!rrc_csfb_call_status)
#endif
     )
     )
     
  {
    rrc_csp_int_data.shld_acq_be_spltd = TRUE;
    next_substate = rrc_csp_wait_for_wrm_lock(&type_freq_scan);
    if(type_freq_scan == RRC_CSP_NO_SCAN)
    {
      return next_substate;
    }
  }
#endif
 
  rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

  if(type_freq_scan == RRC_CSP_ACQ_SCAN)
  {
    /* Start waiting for Acquisition Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_ACQ;
  }
  else
  {
    /* Start waiting for Frequency scan Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
  }
  
  return(next_substate);
}

/*===========================================================================

FUNCTION          rrc_csp_release_chls_and_continue_cell_selection

DESCRIPTION       This function releases all logical, transport and physical
                  channels before continuing with cell selection.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_release_chls_and_continue_cell_selection(void)
{
  rrc_csp_substate_e_type   next_substate;
  rrc_state_e_type rrc_state = rrc_get_state();

  rrccsp_cleanup_bplmn_state();

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */

  /* Set appropriate current scan here. Note that this is important 
   * since all channels are to be released and UE state is to be changed to Idle. 
   * Any subsequent cell selection will ALWAYS be in Idle mode - the current 
   * scan should not be left with a value that may be incompatible with 
   * Cell selection in Idle disconnected state.
   */
  switch(rrc_csp_int_data.curr_scan)
  {
    case RRC_CSP_CELL_RESELECTION_SCAN:
      WRRC_MSG0_HIGH("Setting scan to OOS in idle");
      rrc_csp_int_data.curr_scan = RRC_CSP_OUT_OF_SERVICE_AREA_IN_IDLE_STATE_SCAN;
      break;

    case RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN:
    case RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN:
    case RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN:
    case RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN:
    case RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN:
    case RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN:
    case RRC_CSP_OUT_OF_SERVICE_AREA_SCAN:
    case RRC_CSP_OUT_OF_SERVICE_AREA_IN_IDLE_STATE_SCAN:
    case RRC_CSP_CELL_SELECTION_SCAN:
    case RRC_CSP_LEAVING_CONN_MODE_SCAN:
    case RRC_CSP_PLMN_LIST_SCAN:
    case RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN:
      WRRC_MSG1_HIGH("Current scan type is %d", rrc_csp_int_data.curr_scan);
      break;

    case RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN:
    case RRC_CSP_GTOW_CC_ORDER_SCAN:
    default:
      WRRC_MSG1_ERROR("Unexpected scan type %d", rrc_csp_int_data.curr_scan);
      break;
  }

  /* First call a CCM function to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

  rrc_csp_send_chan_release();

  if(rrc_state != RRC_STATE_DISCONNECTED)
  {
    /* Set the CSP substate to RRCCSP_CELL_SEL_WT_STATE_CHANGE, where
     * on getting the STATE_CHANGE_IND (to IDLE), CSP will send
     * NO SERVICE to MM.
     */
    WRRC_MSG0_HIGH("CSP waits for state change ind");
    next_substate = RRCCSP_CELL_SEL_WT_STATE_CHANGE;
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
    /*Even for DL weak indication with no resource cases, wait for idle confirmation.
      Redirection scan is added as we take lock for the same in continue cell selection*/
    if((WRM_STATE_IS_IN_LOCK(WCDMA_TRM)|| (rrc_csp_int_data.rrc_dl_wk_no_res)) &&
        ((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) ||
         (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) ||
         (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN) 
#ifdef FEATURE_WRLF_SYSTEM_SEL
          || ((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) &&
              RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
          || ((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) 
#ifdef FEATURE_3GPP_CSFB
              &&(!rrc_csfb_call_status)
#endif
             )
        )
      )
    {
      rrc_csp_int_data.csp_waiting_for_wl1_to_release_lock = TRUE;
      rrc_csp_int_data.rrc_dl_wk_no_res = FALSE;
      rrc_csp_send_l1_idle_req();

      return (rrc_get_csp_substate());
    }
#endif

    next_substate = rrc_csp_continue_cell_selection();
  }

#ifdef FEATURE_DUAL_SIM
  rrc_csp_int_data.rrc_dl_wk_no_res = FALSE;
#endif

  return(next_substate);
}

/*===========================================================================

FUNCTION          rrc_csp_release_bch_and_continue_cell_selection

DESCRIPTION       This function releases BCCH/BCH/S_PCCPCH pipe before 
                  continuing with cell selection.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_release_bch_and_continue_cell_selection(void)
{
  rrc_csp_substate_e_type   next_substate;

  /* First call a CCM function to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */


#ifdef FEATURE_DUAL_SIM
  if(WRM_CLI_STATE_IS_IN_LOCK(WCDMA_TRM, WRM_PRIMARY_CLIENT_RRC) &&
      ((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)||
       (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)||
       (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN) 
#ifdef FEATURE_WRLF_SYSTEM_SEL
        || ((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) &&
             RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
      )
    )
  {
    rrc_csp_int_data.csp_waiting_for_wl1_to_release_lock = TRUE;

    if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)||
       (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {
      rrc_csp_send_bch_release(TRUE);
    }
    else
    {
      rrc_csp_send_bch_release(FALSE);
      rrc_csp_send_l1_idle_req();
    }
    return (rrc_get_csp_substate());
  }
#endif
  rrc_csp_send_bch_release(FALSE);

  next_substate = rrc_csp_continue_cell_selection();

  return(next_substate);
}


/*===========================================================================

FUNCTION          RRC_CSP_START_SEL_REDIRECT_FREQ_IN_CONNECTING_STATE
                  

DESCRIPTION       This function starts cell selection on the designated
                  frequency received by RRC in inter-frequency redirection
                  request in connecting state.
                  Only transport and physical channels are released before
                  attempting acquisition on the designated frequency.
                  Cell selection is started for the current acquisition type.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_redirect_freq_in_connecting_state(void)
{
  rrc_csp_substate_e_type next_substate = RRCCSP_CELL_SEL_WT_ACQ;

  /* First call a CCM function to indicate Cell Loss - no new cell */
  rrc_ccm_update_with_cell_loss();

  /* Release transport and physical channels only.*/
  rrc_csp_send_tr_phy_chl_release();
  WRRC_MSG1_HIGH("Acq req for redirected freq %d sent to L1", 
                              rrc_csp_int_data.curr_acq_entry.freq);

  /* Send redirected frequency to L1 for acquisition */
  rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, RRC_CSP_ACQ_SCAN);

  return(next_substate);
} /* rrc_csp_start_sel_redirect_freq_in_connecting_state */


/*===========================================================================

FUNCTION          rrc_csp_start_sel_out_of_service_area

DESCRIPTION       This function starts a cell selection on an out of service
                  area condition. 
                  The cell selection is started in the current acquisition 
                  type. The current scan is set to Out of Service area scan.
                  Based on the returned frequency scan type, a CSP substate
                  is returned.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_out_of_service_area(void)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;

  rrccsp_cleanup_bplmn_state();

  /* Set current scan to "Out of Service Area" and 
   * start 30s timer for Spec PLMN search
   */   
#ifdef FEATURE_WRLF_SYSTEM_SEL
  rrc_csp_int_data.ue_in_conn_oos = FALSE;
  if(RRC_EST_EMERGENCY_CALL != rrcrce_get_rrc_est_cause())
  {
    rrc_csp_int_data.ue_in_conn_oos = TRUE;
    rrc_csp_int_data.rlf_plmn_id = rrc_csp_int_data.curr_req_plmn.plmn_id;

#ifdef FEATURE_FEMTO_CSG
    rrc_csp_int_data.rlf_csg_id = rrc_csp_int_data.curr_req_plmn.csg_id;
#endif

    /* Perform ACQ-DB search. */
    rrc_csp_int_data.curr_scan = RRC_CSP_LEAVING_CONN_MODE_SCAN;
  }
  else
#endif
  {
    rrc_csp_int_data.curr_scan = RRC_CSP_OUT_OF_SERVICE_AREA_SCAN;
  }



  rrctmr_start_timer(RRCTMR_CONN_MODE_OOS_TIMER, RRCTMR_CONN_MODE_OOS_TIMER_IN_MS);
  rrc_csp_int_data.oos_splmn_srch_tmr_expd = FALSE;
  rrc_csp_int_data.oos_gsm_srch_done       = FALSE;
  rrc_csp_int_data.oos_gsm_num_avail_plmns = 0;
  rrc_csp_int_data.plmn_list.num_plmns     = 0;



#ifdef FEATURE_WRLF_SYSTEM_SEL
  if(RRC_EST_EMERGENCY_CALL == rrcrce_get_rrc_est_cause())
#endif
  {
  #ifdef FEATURE_DISABLE_DEEP_SLEEP 
  /* Do not start the no deep sleep timer when this feature
   * is enabled. Otherwise, if we start the timer then on
   * its expiry, UE is put into deep sleep which we need to
   * avoid.
   * Also, continue setting the var rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd
   * to false so that deep sleep is not triggered.
   */
  #else
  rrctmr_start_timer(RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER, 
                     RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER_IN_MS);
  #endif /* FEATURE_DISABLE_DEEP_SLEEP */

  rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd = FALSE;
  rrc_csp_int_data.oos_conn_mode_ds_tmr_expd          = TRUE;
  }



  /* Save the last camped frequency. This frequency will be specifically
   * scanned during the full frequency scan
   */
  rrc_csp_int_data.last_camped_freq_in_connected_mode = rrc_ccm_get_curr_camped_freq();

  /* First send a Cell Change Request to CCM to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

#ifdef FEATURE_GAN
  #error code not present
#endif

  rrc_csp_int_data.prev_network_select_mode = SYS_NETWORK_SELECTION_MODE_NONE; 

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */
  rrc_csp_send_tr_phy_chl_release();

  if(rrc_csp_start_cell_selection
      (
        &rrc_csp_int_data.curr_req_plmn,
        &rrc_csp_int_data.curr_type_acq, 
        &rrc_csp_int_data.curr_acq_entry,
        &type_freq_scan
      )   == FAILURE
    )
  {
    WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");
    return (rrccsp_rel_chls_and_go_to_idle());
  }

  /* Another freq found */
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    /* This is unacceptable since a SUCCESS was returned */
    ERR_FATAL("NO_SCAN returned", 0, 0, 0);
  }
 
#ifdef FEATURE_DUAL_SIM
  rrc_release_lock();
  rrc_start_lock_wait_timer(RRCTMR_DEFAULT_SEARCH_TIME_IN_MS,TRUE);
  rrc_csp_int_data.shld_acq_be_spltd = TRUE;
  next_substate = rrc_csp_wait_for_wrm_lock(&type_freq_scan);
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    return next_substate;
  }
#endif

  rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

  if(type_freq_scan == RRC_CSP_ACQ_SCAN)
  {
    /* Start waiting for Acquisition Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_ACQ;
  }
  else
  {
    /* Start waiting for Frequency scan Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
  }
  
  return(next_substate);
}

/*===========================================================================

FUNCTION          rrc_csp_start_sel_out_of_service_area_in_idle_state

DESCRIPTION       This function starts a cell selection on an out of service
                  area condition in Idle state.
                  All channels are released. 
                  The cell selection is started in the current acquisition 
                  type. The current scan is set to Out of Service area in
                  Idle state scan.
                  Based on the returned frequency scan type, a CSP substate
                  is returned.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_out_of_service_area_in_idle_state(void)
{
#ifndef FEATURE_EOOS
  rrc_csp_freq_scan_e_type  type_freq_scan;
#endif
  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;
  
  /* Set current scan to "Out of Service Area".*/   
  rrc_csp_int_data.curr_scan = RRC_CSP_OUT_OF_SERVICE_AREA_IN_IDLE_STATE_SCAN;

  /* First call a CCM function to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

#ifdef FEATURE_DUAL_SIM
  /* Release TRM lock if acquired before starting OOS scan */
  rrc_release_lock();
#endif

#ifdef FEATURE_GAN
#error code not present
#endif

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */
  rrc_csp_send_chan_release();

  /* Signal SIB procedure to clear up the active event, if any */
  rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_TO_OOS);

  /* Manual BPLMN search is in progress */
  if((WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
      && (rrc_csp_int_data.bplmn_nw_sel_mode == SYS_NETWORK_SELECTION_MODE_MANUAL))
  {
    next_substate = rrccsp_continue_bplmn_in_frgrnd();
  }
  /* Manual BPLMN search is NOT in progress */
  else
  {
    rrccsp_cleanup_bplmn_state();

#ifdef FEATURE_EOOS
#error code not present
#else
/* FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL */
/* Below if logic is to post an immediate NO SERVICE to NAS which will send
 * a new RRC_SERVICE_REQ with limited camping and with service_req_cause
 * set to RRC_MO_CS_CALL. With this CSP will start looking for a cell which 
 * supports emergency call
 */
  if(rrc_csp_int_data.est_req_for_emerg_call)
  {
    WRRC_MSG0_HIGH("OOS: Send NO SERVICE to NAS Immediately");
    rrc_csp_handle_no_available_freq();
    next_substate = RRCCSP_NO_CELL_SELECTED;
    rrc_csp_int_data.est_req_for_emerg_call = FALSE;
  }
  else
  {
    if(rrc_csp_start_cell_selection
        (
          &rrc_csp_int_data.curr_req_plmn,
          &rrc_csp_int_data.curr_type_acq, 
          &rrc_csp_int_data.curr_acq_entry,
          &type_freq_scan
        )   == FAILURE
      )
    {
      WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");
      return (rrccsp_rel_chls_and_go_to_idle());
    }

    /* Another freq found */
    if(type_freq_scan == RRC_CSP_NO_SCAN)
    {
      /* This is unacceptable since a SUCCESS was returned */
      ERR_FATAL("NO_SCAN returned", 0, 0, 0);
    }

    rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

    if(type_freq_scan == RRC_CSP_ACQ_SCAN)
    {
      /* Start waiting for Acquisition Confirmation */
      next_substate = RRCCSP_CELL_SEL_WT_ACQ;
    }
    else
    {
      /* Start waiting for Frequency scan Confirmation */
      next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
    }
  }

#endif /* FEATURE_EOOS */

  }
  return(next_substate);
}

/*===========================================================================

FUNCTION          rrc_csp_start_sel_out_of_service_area_in_connecting_state

DESCRIPTION       This function starts a cell selection on an out of service
                  area condition in Connecting state.
                  All channels are released. 
                  The cell selection is started in the current acquisition 
                  type. The current scan is set to Out of Service area in
                  Connecting state scan.
                  Based on the returned frequency scan type, a CSP substate
                  is returned.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_out_of_service_area_in_connecting_state(void)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;
  
  rrccsp_cleanup_bplmn_state();

  /* Set current scan to "Out of Service Area".*/   
  rrc_csp_int_data.curr_scan = RRC_CSP_OUT_OF_SERVICE_AREA_IN_CONNECTING_STATE_SCAN;

  /* Save the last camped frequency. This frequency will be specifically
   * scanned during the full frequency scan
   */
  rrc_csp_int_data.last_camped_freq_in_connected_mode = rrc_ccm_get_curr_camped_freq();

  /* First call a CCM function to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

#ifdef FEATURE_GAN
  #error code not present
#endif

  /* Release tr & phy channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */
  rrc_csp_send_tr_phy_chl_release();

  if(rrc_csp_start_cell_selection
      (
        &rrc_csp_int_data.curr_req_plmn,
        &rrc_csp_int_data.curr_type_acq, 
        &rrc_csp_int_data.curr_acq_entry,
        &type_freq_scan
      )   == FAILURE
    )
  {
    WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");
    return (rrccsp_rel_chls_and_go_to_idle());
  }

  /* Another freq found */
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    /* This is unacceptable since a SUCCESS was returned */
    ERR_FATAL("NO_SCAN returned", 0, 0, 0);
  }
 
  rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

  if(type_freq_scan == RRC_CSP_ACQ_SCAN)
  {
    /* Start waiting for Acquisition Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_ACQ;
  }
  else
  {
    /* Start waiting for Frequency scan Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
  }
  
  return(next_substate);
}

/*===========================================================================

FUNCTION          rrc_csp_start_sel_from_dch_to_fach

DESCRIPTION       This function starts a cell selection on a transition from
                  CELL_DCH to CELL_FACH state.                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_from_dch_to_fach(rrc_cmd_type  *rrc_cmd_ptr)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;
  boolean  redirection_barred = FALSE;
  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;
  
  /* Set current scan to "Out of Service Area".*/   
  rrc_csp_int_data.curr_scan = RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN;

  /* Store the destination state as it will be used to configure appropriate
   * channels when UE camps on a suitable cell. The reason that we need to
   * store this information although we can deduce the destination state from
   * the curr_scan is because this curr_scan may change to RRC_CSP_OUT_OF_SERVICE_AREA_SCAN
   * when CU initiates cell selection with cause OOS area indication
   */
  rrc_csp_int_data.dch_tx_dest_state = RRC_STATE_CELL_FACH;

  /* Save the last camped frequency. This frequency will be specifically
   * scanned during the full frequency scan
   */
  rrc_csp_int_data.last_camped_freq_in_connected_mode = rrc_ccm_get_curr_camped_freq();

  redirection_barred = rrccsp_check_redirection_barred(rrc_cmd_ptr);
  /* Initialize destination frequency info if passed in the command */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present &&  !redirection_barred)
  {
    rrc_csp_int_data.dest_freq = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq;
    rrc_csp_int_data.dest_freq_present = TRUE;
  }
  else
  {
    rrc_csp_int_data.dest_freq_present = FALSE;
  }

  /* Initialize destination Pri Scr Code info if passed in the command */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present  && !redirection_barred)
  {
    rrc_csp_int_data.dest_psc = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc;
    rrc_csp_int_data.dest_psc_present = TRUE;
  }
  else
  {
    rrc_csp_int_data.dest_psc_present = FALSE;
  }

  rrc_csp_get_aset_scr_pn();

  /* First send a Cell Change Request to CCM to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

  /* Signal SIB procedure to clear up the active event, if any */
  rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_TO_OOS);

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */
  rrc_csp_send_tr_phy_chl_release();

  /* Here start a timer, at the expiry of which, the RRC state is taken back
   * to Idle. This is the max time for Cell selection during a transition from
   * DCH to FACH.
   */                       
  if(rrccsp_is_state_transition_triggered_by_reconfig_msg())
  {
    WRRC_MSG0_HIGH("OOSC: DCH->FACH due to reconfiguration");
    rrctmr_start_timer(RRCTMR_DCH_FACH_CELL_SELECTION_TIMER, 
                       (RRCTMR_DCH_FACH_CELL_SELECTION_TIMER_IN_MS + rrcllc_get_dch_fach_act_time_in_ms()));

    WRRC_MSG2_HIGH("DCH->FACH timer: started %d ms + act_time %d", RRCTMR_DCH_FACH_CELL_SELECTION_TIMER_IN_MS,
             rrcllc_get_dch_fach_act_time_in_ms());

    /* set the timer as not expired */
    rrc_csp_int_data.dch_to_fach_pch_tmr_expd = FALSE;
  }
  /* Since the DCH->FACH tx is NOT due to reconfiguration, change the
   * type of scan to OOS_Area_Scan which will allow the UE to search 
   * for service as per the feature FEATURE_OUT_OF_SERVICE_ENHANCEMENT
   */
  else
  {
    RRC_DEBUG_INCREMENT_STATS(OOS_IN_CONNECTED_MODE);

#ifdef FEATURE_WRLF_SYSTEM_SEL
    if(RRC_EST_EMERGENCY_CALL != rrcrce_get_rrc_est_cause())
    {
      rrc_csp_int_data.curr_scan = RRC_CSP_LEAVING_CONN_MODE_SCAN;
      rrc_csp_int_data.ue_in_conn_oos = TRUE;
      rrc_csp_int_data.rlf_plmn_id = rrc_csp_int_data.curr_req_plmn.plmn_id;
#ifdef FEATURE_FEMTO_CSG
      rrc_csp_int_data.rlf_csg_id = rrc_csp_int_data.curr_req_plmn.csg_id;
#endif
      WRRC_MSG0_HIGH("WRLF: Changing curr_scan to RRC_CSP_LEAVING_CONN_MODE_SCAN. Start splmn srch 30s tmr");

      /* Abort any suspended BPLMN search at the start of OOS search. */
      rrccsp_cleanup_bplmn_state();
    }
    else
#endif
    {
    rrc_csp_int_data.curr_scan = RRC_CSP_OUT_OF_SERVICE_AREA_SCAN;
      WRRC_MSG0_HIGH("WRLF: Changing curr_scan to RRC_CSP_OUT_OF_SERVICE_AREA_SCAN. Start splmn srch 30s tmr");
    }

#ifdef FEATURE_TRIPLE_SIM
  /*For connected mode oos*/
    rrcwrm_set_trm_extension_flag_for_alternate_page(TRUE);
#endif

    rrctmr_start_timer(RRCTMR_CONN_MODE_OOS_TIMER, RRCTMR_CONN_MODE_OOS_TIMER_IN_MS);
    rrc_csp_int_data.oos_splmn_srch_tmr_expd = FALSE;
    rrc_csp_int_data.oos_gsm_srch_done       = FALSE;
    rrc_csp_int_data.oos_gsm_num_avail_plmns = 0;
    rrc_csp_int_data.plmn_list.num_plmns     = 0;

    /* Reset aset_scr_code while handling leaving connected mode scan to 
     *  prevent ASET scan to handle corner case scenarios such as receiving 
     * connection release before OOS */
    rrc_csp_int_data.aset_scr_code = 8192;

#ifdef FEATURE_WRLF_SYSTEM_SEL
    if(RRC_EST_EMERGENCY_CALL == rrcrce_get_rrc_est_cause())
#endif
    {
    #ifdef FEATURE_DISABLE_DEEP_SLEEP 
    /* Do not start the no deep sleep timer when this feature
     * is enabled. Otherwise, if we start the timer then on
     * its expiry, UE is put into deep sleep which we need to
     * avoid.
     * Also, continue setting the var rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd
     * to false so that deep sleep is not triggered.
     */
    #else

    rrctmr_start_timer(RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER, 
                       RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER_IN_MS);

    #endif /* FEATURE_DISABLE_DEEP_SLEEP */

    rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd = FALSE;
    rrc_csp_int_data.oos_conn_mode_ds_tmr_expd          = TRUE;

    }
    rrc_csp_int_data.prev_network_select_mode = SYS_NETWORK_SELECTION_MODE_NONE;
  }


  if(rrc_csp_start_cell_selection
      (
        &rrc_csp_int_data.curr_req_plmn,
        &rrc_csp_int_data.curr_type_acq, 
        &rrc_csp_int_data.curr_acq_entry,
        &type_freq_scan
      )   == FAILURE
    )
  {
    WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");
    return (rrccsp_rel_chls_and_go_to_idle());
  }

  /* Another freq found */
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    /* This is unacceptable since a SUCCESS was returned */
    ERR_FATAL("NO_SCAN returned", 0, 0, 0);
  }
 
#ifdef FEATURE_DUAL_SIM
  if((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) 
#ifdef FEATURE_WRLF_SYSTEM_SEL
     || (rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)
#endif
     )
  {
    rrc_release_lock();
    rrc_start_lock_wait_timer(RRCTMR_DEFAULT_SEARCH_TIME_IN_MS,TRUE);
    rrc_csp_int_data.shld_acq_be_spltd = TRUE;
    next_substate = rrc_csp_wait_for_wrm_lock(&type_freq_scan);
    if(type_freq_scan == RRC_CSP_NO_SCAN)
    {
      return next_substate;
    }
  }
  else if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) &&
          (rrc_csp_int_data.dest_freq_present))
  {
    rrc_wrm_acquire_lock_on_redirected_freq(rrc_csp_int_data.dest_freq,TRM_BG_TRAFFIC);
  }
#endif
  if(type_freq_scan == RRC_CSP_LAST_ASET_SCAN)
  {
    /* Send CPHY_ACQ_REQ to L1 and wait for the CNF */
    rrc_csp_send_l1_cmd(RRC_PROCEDURE_CSP, CPHY_ACQ_REQ, &rrc_csp_int_data.curr_acq_entry);
    MSG_HIGH("ACQ_REQ for the best cell in active set. Freq %d, SCR = %d, PN = %d",
                                                    rrc_csp_int_data.curr_acq_entry.freq,
                                                    rrc_csp_int_data.curr_acq_entry.scr_code,
                                                    rrc_csp_int_data.pn_pos);

    /* Start waiting for Acquisition Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_ACQ;
  }
  else
  {
    rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

    if(type_freq_scan == RRC_CSP_ACQ_SCAN)
    {
      /* Start waiting for Acquisition Confirmation */
      next_substate = RRCCSP_CELL_SEL_WT_ACQ;
    }
    else
    {
      /* Start waiting for Frequency scan Confirmation */
      next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
    }
  }
  
  return(next_substate);
} /* rrc_csp_start_sel_from_dch_to_fach */


/*===========================================================================

FUNCTION          rrc_csp_start_sel_from_dch_to_pch

DESCRIPTION       This function starts a cell selection on a transition from
                  CELL_DCH to CELL_PCH or URA_PCH state.                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_from_dch_to_pch(rrc_cmd_type  *rrc_cmd_ptr)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;
  boolean  redirection_barred = FALSE;

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;
  
  /* Set current scan to CELL_PCH or URA_PCH scan */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state == RRC_STATE_CELL_PCH)
  {
    rrc_csp_int_data.curr_scan = RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN;

    /* Store the destination state as it will be used to configure appropriate
     * channels when UE camps on a suitable cell. The reason that we need to
     * store this information although we can deduce the destination state from
     * the curr_scan is because this curr_scan may change to RRC_CSP_OUT_OF_SERVICE_AREA_SCAN
     * when CU initiates cell selection with cause OOS area indication
     */
    rrc_csp_int_data.dch_tx_dest_state = RRC_STATE_CELL_PCH;
  }
  else if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state == RRC_STATE_URA_PCH)
  {
    rrc_csp_int_data.curr_scan = RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN;

    /* Store the destination state as it will be used to configure appropriate
     * channels when UE camps on a suitable cell. The reason that we need to
     * store this information although we can deduce the destination state from
     * the curr_scan is because this curr_scan may change to RRC_CSP_OUT_OF_SERVICE_AREA_SCAN
     * when CU initiates cell selection with cause OOS area indication
     */
    rrc_csp_int_data.dch_tx_dest_state = RRC_STATE_URA_PCH;
  }
  else
  {
    WRRC_MSG1_ERROR("Dest state %d not processed here",rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state);
    return next_substate;
  }

  /* Save the last camped frequency. This frequency will be specifically
   * scanned during the full frequency scan
   */
  rrc_csp_int_data.last_camped_freq_in_connected_mode = rrc_ccm_get_curr_camped_freq();

  redirection_barred = rrccsp_check_redirection_barred(rrc_cmd_ptr);

  /* Initialize destination frequency info if passed in the command */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present && !redirection_barred)
  {
    rrc_csp_int_data.dest_freq = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq;
    rrc_csp_int_data.dest_freq_present = TRUE;
  }
  else
  {
    rrc_csp_int_data.dest_freq_present = FALSE;
  }

  /* Initialize destination Pri Scr Code info if passed in the command */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present && !redirection_barred)
  {
    rrc_csp_int_data.dest_psc = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc;
    rrc_csp_int_data.dest_psc_present = TRUE;
  }
  else
  {
    rrc_csp_int_data.dest_psc_present = FALSE;
  }

  /* First send a Cell Change Request to CCM to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

  /* Signal SIB procedure to clear up the active event, if any */
  rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_TO_OOS);

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */
  rrc_csp_send_tr_phy_chl_release();

  if(rrc_csp_start_cell_selection
      (
        &rrc_csp_int_data.curr_req_plmn,
        &rrc_csp_int_data.curr_type_acq, 
        &rrc_csp_int_data.curr_acq_entry,
        &type_freq_scan
      )   == FAILURE
    )
  {
    WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");
    return (rrccsp_rel_chls_and_go_to_idle());
  }

  /* Another freq found */
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    /* This is unacceptable since a SUCCESS was returned */
    ERR_FATAL("NO_SCAN returned", 0, 0, 0);
  }
 
  /* Here start a timer, at the expiry of which, the RRC state is taken back
   * to Idle. This is the max time for Cell selection during a transition from
   * DCH to CELL_PCH or URA_PCH.
   */
  rrctmr_start_timer( RRCTMR_DCH_PCH_CELL_SELECTION_TIMER, 
                      RRCTMR_DCH_PCH_CELL_SELECTION_TIMER_IN_MS );

  /* set the timer as not expired */
  rrc_csp_int_data.dch_to_fach_pch_tmr_expd = FALSE;
  
#ifdef FEATURE_DUAL_SIM
  if(((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) ||
           (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN))&&
          (rrc_csp_int_data.dest_freq_present))
  {
    rrc_wrm_acquire_lock_on_redirected_freq(rrc_csp_int_data.dest_freq,TRM_BG_TRAFFIC);
  }
#endif

  rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

  if(type_freq_scan == RRC_CSP_ACQ_SCAN)
  {
    /* Start waiting for Acquisition Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_ACQ;
  }
  else
  {
    /* Start waiting for Frequency scan Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
  }
  
  return(next_substate);
} /* rrc_csp_start_sel_from_dch_to_pch */

/*===========================================================================

FUNCTION          RRC_CSP_START_SEL_FROM_FACH_TO_FACH

DESCRIPTION       This function starts a cell selection on a transition from
                  CELL_FACH to CELL_FACH state with frequency redirection info.
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_from_fach_to_fach(rrc_cmd_type  *rrc_cmd_ptr)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;
  boolean  redirection_barred = FALSE;

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;
  
  rrc_csp_int_data.curr_scan = RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN;

  /* Store the destination state as it will be used to configure appropriate
   * channels when UE camps on a suitable cell. The reason that we need to
   * store this information although we can deduce the destination state from
   * the curr_scan is because this curr_scan may change to RRC_CSP_OUT_OF_SERVICE_AREA_SCAN
   * when CU initiates cell selection with cause OOS area indication
   */
  rrc_csp_int_data.fach_tx_dest_state = RRC_STATE_CELL_FACH;

  /* Save the last camped frequency. This frequency will be specifically
   * scanned during the full frequency scan
   */
  rrc_csp_int_data.last_camped_freq_in_connected_mode = rrc_ccm_get_curr_camped_freq();

  redirection_barred = rrccsp_check_redirection_barred(rrc_cmd_ptr);
  /* Initialize destination frequency info if passed in the command */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present && !redirection_barred)
  {
    rrc_csp_int_data.dest_freq = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq;
    rrc_csp_int_data.dest_freq_present = TRUE;
  }
  else
  {
    rrc_csp_int_data.dest_freq_present = FALSE;
  }

  /* Initialize destination Pri Scr Code info if passed in the command */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present && !redirection_barred)
  {
    rrc_csp_int_data.dest_psc = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc;
    rrc_csp_int_data.dest_psc_present = TRUE;
  }
  else
  {
    rrc_csp_int_data.dest_psc_present = FALSE;
  }

  /* First send a Cell Change Request to CCM to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

  /* Signal SIB procedure to clear up the active event, if any */
  rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_TO_OOS);

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */
  rrc_csp_send_tr_phy_chl_release();

  /* Here start a timer, at the expiry of which, the RRC state is taken back
   * to Idle. This is the max time for Cell selection during a transition from
   * FACH to FACH.
   */
  WRRC_MSG0_HIGH("FACH->FACH due to reconfiguration");
  rrctmr_start_timer(RRCTMR_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER, 
                     RRCTMR_FACH_TO_FACH_CELL_SELECTION_TIMER_IN_MS);

  WRRC_MSG1_HIGH("FACH->FACH timer: started %d ms", RRCTMR_FACH_TO_FACH_CELL_SELECTION_TIMER_IN_MS);

  /* set the timer as not expired */
  rrc_csp_int_data.fach_to_fach_pch_tmr_expd = FALSE;

  if(rrc_csp_start_cell_selection
      (
        &rrc_csp_int_data.curr_req_plmn,
        &rrc_csp_int_data.curr_type_acq, 
        &rrc_csp_int_data.curr_acq_entry,
        &type_freq_scan
      )   == FAILURE
    )
  {
    WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");
    return (rrccsp_rel_chls_and_go_to_idle());
  }

  /* Another freq found */
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    /* This is unacceptable since a SUCCESS was returned */
    ERR_FATAL("NO_SCAN returned", 0, 0, 0);
  }

#ifdef FEATURE_DUAL_SIM 
  if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN) &&
     (rrc_csp_int_data.dest_freq_present))
  {
    rrc_wrm_acquire_lock_on_redirected_freq(rrc_csp_int_data.dest_freq,TRM_BG_TRAFFIC);
  }
 #endif
 
  rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

  if(type_freq_scan == RRC_CSP_ACQ_SCAN)
  {
    /* Start waiting for Acquisition Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_ACQ;
  }
  else
  {
    /* Start waiting for Frequency scan Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
  }
  
  return(next_substate);
} /* rrc_csp_start_sel_from_fach_to_fach */

/*===========================================================================

FUNCTION          RRC_CSP_START_SEL_FROM_FACH_TO_PCH

DESCRIPTION       This function starts a cell selection on a transition from
                  CELL_FACH to CELL_PCH or URA_PCH state with frequency redirection info.
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_from_fach_to_pch(rrc_cmd_type  *rrc_cmd_ptr)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;
  boolean  redirection_barred = FALSE;

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;

  /* Set current scan to CELL_PCH or URA_PCH scan */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state == RRC_STATE_CELL_PCH)
  {
    rrc_csp_int_data.curr_scan = RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN;
  }
  else if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state == RRC_STATE_URA_PCH)
  {
    rrc_csp_int_data.curr_scan = RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN;
  }
  else
  {
    WRRC_MSG1_ERROR("Dest state %d not processed here",rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state);
    return next_substate;
  }

  /* Store the destination state as it will be used to configure appropriate
   * channels when UE camps on a suitable cell. The reason that we need to
   * store this information although we can deduce the destination state from
   * the curr_scan is because this curr_scan may change to RRC_CSP_OUT_OF_SERVICE_AREA_SCAN
   * when CU initiates cell selection with cause OOS area indication
   */
  rrc_csp_int_data.fach_tx_dest_state = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state;

  /* Save the last camped frequency. This frequency will be specifically
   * scanned during the full frequency scan
   */
  rrc_csp_int_data.last_camped_freq_in_connected_mode = rrc_ccm_get_curr_camped_freq();

  redirection_barred = rrccsp_check_redirection_barred(rrc_cmd_ptr);

  /* Initialize destination frequency info if passed in the command */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present &&  !redirection_barred)
  {
    rrc_csp_int_data.dest_freq = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq;
    rrc_csp_int_data.dest_freq_present = TRUE;
  }
  else
  {
    rrc_csp_int_data.dest_freq_present = FALSE;
  }

  /* Initialize destination Pri Scr Code info if passed in the command */
  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present &&  !redirection_barred)
  {
    rrc_csp_int_data.dest_psc = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc;
    rrc_csp_int_data.dest_psc_present = TRUE;
  }
  else
  {
    rrc_csp_int_data.dest_psc_present = FALSE;
  }

  /* First send a Cell Change Request to CCM to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

  /* Signal SIB procedure to clear up the active event, if any */
  rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_TO_OOS);

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */
  rrc_csp_send_tr_phy_chl_release();

  /* Here start a timer, at the expiry of which, the RRC state is taken back
   * to Idle. This is the max time for Cell selection during a transition from
   * FACH to FACH.
   */
  WRRC_MSG0_HIGH("FACH->PCH/URA_PCH due to reconfiguration");
  rrctmr_start_timer(RRCTMR_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER, 
                     RRCTMR_FACH_TO_PCH_CELL_SELECTION_TIMER_IN_MS);

  WRRC_MSG1_HIGH("FACH->PCH/URA_PCH timer: started %d ms", RRCTMR_FACH_TO_PCH_CELL_SELECTION_TIMER_IN_MS);

  /* set the timer as not expired */
  rrc_csp_int_data.fach_to_fach_pch_tmr_expd = FALSE;

  if(rrc_csp_start_cell_selection
      (
        &rrc_csp_int_data.curr_req_plmn,
        &rrc_csp_int_data.curr_type_acq, 
        &rrc_csp_int_data.curr_acq_entry,
        &type_freq_scan
      )   == FAILURE
    )
  {
    WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");
    return (rrccsp_rel_chls_and_go_to_idle());
  }

  /* Another freq found */
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    /* This is unacceptable since a SUCCESS was returned */
    ERR_FATAL("NO_SCAN returned", 0, 0, 0);
  }

#ifdef FEATURE_DUAL_SIM
  else if(((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN) ||
           (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN))&&
          (rrc_csp_int_data.dest_freq_present))
  {
    rrc_wrm_acquire_lock_on_redirected_freq(rrc_csp_int_data.dest_freq,TRM_BG_TRAFFIC);
  }
#endif
 
  rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

  if(type_freq_scan == RRC_CSP_ACQ_SCAN)
  {
    /* Start waiting for Acquisition Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_ACQ;
  }
  else
  {
    /* Start waiting for Frequency scan Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
  }
  
  return(next_substate);
} /* rrc_csp_start_sel_from_fach_to_pch */


/*===========================================================================

FUNCTION          rrc_csp_start_sel_leaving_conn_mode

DESCRIPTION       This function starts a cell selection on leaving connected
                  mode.
                  The cell selection is started in the current acquisition 
                  type. The current scan is set to leaving Connected mode scan.
                  Based on the returned frequency scan type, a CSP substate
                  is returned.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_leaving_conn_mode(void)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;

  boolean mm_registration_in_progress = FALSE;

  boolean bplmn_srch_actively_in_progress = FALSE;

  WRRC_MSG1_HIGH("Leaving conn mode. curr_scan %d", rrc_csp_int_data.curr_scan);

  /* Trigger FR2LTE only if CS RAB has been established and then released when UE moves
   * from LTE to WCDMA due to CSFB call
   */
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
  if(((csfb_extended_status == CSFB_CS_RB_RELEASED) || 
      ((csfb_extended_status == CSFB_STARTED) && rrc_cs_rab_established))
      && rrcmcm_is_lte_mode_enabled()
#ifdef FEATURE_WRLF_SYSTEM_SEL
      && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
     )
  {
    if(rrccho_is_wtog_cco_active())
    {
      WRRC_MSG0_HIGH("WTOG CCO Active. Do not trigger WTOL redirection.");
      return next_substate;
    }

    WRRC_MSG0_HIGH("CSFB_HACK: Suspend WCDMA and start WTOL blind redirection.");
    rrc_csp_int_data.saved_rrc_cmd.cmd.redirect_req.proc_id = RRC_PROCEDURE_CSP;
    rrc_csp_int_data.interrat_type = RRC_CSP_INTERRAT_LTE;

    rrc_csp_int_data.curr_scan = RRC_CSP_INTER_RAT_REDIRECTION_SCAN;
    /* Send change mode request to MCM */
    rrccsp_send_mode_change_req_to_mcm(RRC_MODE_STANDBY);
    next_substate = RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF;
    return next_substate;
  }
#endif

  /* Invalidate the fach_tx_dest_state */
  rrc_csp_int_data.fach_tx_dest_state = RRC_STATE_MAX;

#ifdef FEATURE_DUAL_SIM
  /* Release TRM lock if acquired before starting leaving connected mode scan */
  rrc_release_lock();
  rrcwrm_reset_l2_ack_req_mask();
#endif
  /* Inter Frequency Redirection Scan was aborted. Clean up its state */
  if(rrc_csp_int_data.curr_scan == RRC_CSP_INTER_FREQ_REDIRECTION_SCAN)
  {
    rrc_csp_cleanup_inter_freq_redirection_state();
  }
  
  MSG_HIGH("Leaving Conn mode. nw_sel_mode %d, rrc_state %d, srch_st %d", 
                                      rrc_csp_int_data.bplmn_nw_sel_mode,
                                      rrc_get_state(), rrc_csp_int_data.bplmn_srch_vars_status);

  /* If manual plmn search is not in progress or 
   * manual search is in progress but search status is not "START"
   * then cleanup bplmn state.
   */
  if(   (rrc_csp_int_data.bplmn_nw_sel_mode      != SYS_NETWORK_SELECTION_MODE_MANUAL)
     || (rrc_csp_int_data.bplmn_srch_vars_status != WTOW_SEARCH_START))
  {
    /* If BPLMN srch is actively in progress, release channels and send IDLE_REQ to L1 only after L1 is put to sleep */
    if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN) ||
        (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
    {
      bplmn_srch_actively_in_progress = TRUE;
    }

    if(bplmn_srch_actively_in_progress)
    {
      rrc_csp_send_chan_release();
    }
  }

  if(!rrc_csp_int_data.bplmn_guard_srch_tmr_expired && (!bplmn_data_saved) && (rrc_csp_int_data.bplmn_srch_vars_status != WTOW_SEARCH_START))
  {
    WRRC_MSG0_HIGH("Save the BPLMN data");
    /* Save BPLMN data here and call a specialized clean up function which is going to clean the needed variables for BPLMN */
    prev_rrc_csp_curr_select_data = rrc_csp_curr_select_data;
    bplmn_data_saved = TRUE;
    bplmn_new_srch = rrc_csp_int_data.bplmn_new_srch;
    rrc_csp_cleanup_needed_bplmn_data();
  }

#ifdef FEATURE_WRLF_SYSTEM_SEL
  if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS() && 
      ( rrc_csp_int_data.prev_network_select_mode == SYS_NETWORK_SELECTION_MODE_NONE))
  {
    /* Set the CSP substate to RRCCSP_CELL_SEL_WT_STATE_CHANGE, where
     * on getting the STATE_CHANGE_IND (to IDLE), CSP will send
     * NO SERVICE to MM.
     */
    next_substate = RRCCSP_CELL_SEL_WT_STATE_CHANGE;
    return next_substate;
  }
#endif


  if(((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
#ifdef FEATURE_WRLF_SYSTEM_SEL
       || (RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
      ) && (!rrc_csp_int_data.oos_splmn_srch_tmr_expd))
  {
    rrctmr_stop_timer(RRCTMR_CONN_MODE_OOS_TIMER);
    rrc_csp_int_data.oos_splmn_srch_tmr_expd = TRUE;
  }

  if((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
      && (!rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd))
  {
    rrctmr_stop_timer(RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER);
    rrc_csp_int_data.oos_initial_no_deep_sleep_tmr_expd = TRUE;
  }

  if(rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
  {
    /*Call the NAS API*/
#ifdef FEATURE_DUAL_SIM
     mm_registration_in_progress = mm_per_subs_rrc_registration_in_progress(rrc_get_as_id());
#else
     mm_registration_in_progress = mm_rrc_registration_in_progress();
#endif
    if(mm_registration_in_progress)
    {
      WRRC_MSG0_HIGH("MM Reg is in progress, RRC to send NO_SERVICE to NAS");
#ifdef FEATURE_OOSC_USER_ACTION
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
      rrc_csp_send_svc_status_to_mm(SYS_SRV_STATUS_NONE, SYS_RAT_UMTS_RADIO_ACCESS, 0);
#endif
#endif /*FEATURE_OOSC_USER_ACTION*/
      rrc_ccm_update_with_cell_loss();

      /* Set the CSP substate to RRCCSP_CELL_SEL_WT_STATE_CHANGE, where
       * on getting the STATE_CHANGE_IND (to IDLE), CSP will send
       * NO SERVICE to MM.
       */
      next_substate = rrccsp_rel_chls_and_go_to_idle();;
      return next_substate;
    }
  }

  if((rrc_csp_int_data.curr_scan != RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
#ifdef FEATURE_WRLF_SYSTEM_SEL
    && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
    )
  {
    rrc_csp_int_data.prev_network_select_mode = SYS_NETWORK_SELECTION_MODE_NONE;
  }

#ifdef FEATURE_WRLF_SYSTEM_SEL
  /* Reset the ue_in_conn_oos flag if UE finds a suitable W cell during OOS search. */
  if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
  {
    WRRC_MSG0_HIGH("WRLF: Resetting ue_in_conn_oos flag");
    rrc_csp_int_data.ue_in_conn_oos = FALSE;
  }
#endif

  /* Saving curr_scan before starting leaving connected mode scan */
  rrc_csp_int_data.prev_curr_scan = rrc_csp_int_data.curr_scan;
  /* Set current scan to Leaving Connected Mode Scan.*/   

  /*If aset_scr_code is already set, set it to the latest value 
    Do not set aset value for redirection scan. initiated by RCR*/
  if((!rrc_ifreq_rdr_in_prgrs) && (!rrc_rplmn_info_prsnt)) 
  {
    rrc_csp_get_aset_scr_pn();
  }

  rrc_rplmn_info_prsnt = FALSE;

  rrc_csp_int_data.curr_scan = RRC_CSP_LEAVING_CONN_MODE_SCAN;

  /* First call a CCM function to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

  if(rrc_csp_start_cell_selection
      (
        &rrc_csp_int_data.curr_req_plmn,
        &rrc_csp_int_data.curr_type_acq, 
        &rrc_csp_int_data.curr_acq_entry,
        &type_freq_scan
      )   == FAILURE
    )
  {
    return (rrccsp_rel_chls_and_go_to_idle());
  }

  /* Another freq found */
  if(type_freq_scan == RRC_CSP_NO_SCAN)
  {
    /* This is unacceptable since a SUCCESS was returned */
    ERR_FATAL("NO_SCAN returned", 0, 0, 0);
  }
  else if(type_freq_scan == RRC_CSP_LAST_ASET_SCAN)
  {
    /*Send CPHY_ACQ_REQ to L1 and wait for the CNF */
    /*Send idle request before ACQ, there are cases where LLC ignores channel release*/
    rrc_csp_send_l1_idle_req();
    rrc_csp_send_l1_cmd(RRC_PROCEDURE_CSP, CPHY_ACQ_REQ, &rrc_csp_int_data.curr_acq_entry);
    MSG_HIGH("ACQ_REQ for the best cell in active set. Freq %d, SCR = %d, PN = %d",
                                                  rrc_csp_int_data.curr_acq_entry.freq,
                                                  rrc_csp_int_data.curr_acq_entry.scr_code,
                                                  rrc_csp_int_data.pn_pos);

    /* Start waiting for Acquisition Confirmation */
    next_substate = RRCCSP_CELL_SEL_WT_ACQ;
  }
  else
  {
    rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);
    if(type_freq_scan == RRC_CSP_ACQ_SCAN)
    {
      /* Start waiting for Acquisition Confirmation */
      next_substate = RRCCSP_CELL_SEL_WT_ACQ;
    }
    else
    {
      /* Start waiting for Frequency scan Confirmation */
      next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
    }
  }

  return(next_substate);
}

/*===========================================================================

FUNCTION          rrc_csp_start_sel_for_plmn_list

DESCRIPTION       This function starts scanning for all the available PLMNs
                  Based on the returned frequency scan type, a CSP substate
                  is returned.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_for_plmn_list(void)
{
  rrc_csp_freq_scan_e_type  type_freq_scan;
  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;

  /* Inter Frequency Redirection Scan was aborted. Clean up its state */
  if(rrc_csp_int_data.curr_scan == RRC_CSP_INTER_FREQ_REDIRECTION_SCAN)
  {
    rrc_csp_cleanup_inter_freq_redirection_state();
  }
  
  /*Stop the timer in case background search is converted to foreground search*/
  memset(&rrc_csp_int_data.bplmn_search_timer_value,0,sizeof(sys_manual_search_timer_value_type));
  rrc_csp_int_data.send_partial_results = FALSE;
  rrctmr_stop_timer(RRCTMR_BPLMN_PARTIAL_REPORT_TIMER);
  
  /* Set current scan to "PLMN List Scan".*/   
  rrc_csp_int_data.curr_scan = RRC_CSP_PLMN_LIST_SCAN;

  if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
  {
    WRRC_MSG1_HIGH("BST band pref incl %d",rrc_bst_band_pref_incl);
    if(rrc_bst_band_pref_incl)
    {
      /* Update GSM with UE's band preference */
      rr_rrc_set_band_pref_internal(rrc_csp_int_data.rat_pri_band_pref);

      /* Update GSM with BST band preference */
      rr_rrc_set_bst_band_pref_internal(rrc_csp_int_data.bplmn_band_pref);  
    }
    else
    {
      /* Update GSM with UE's band preference */
      rr_rrc_set_band_pref_internal(rrc_csp_int_data.bplmn_band_pref);
  
      /* Update GSM with BST band preference */
      rr_rrc_set_bst_band_pref_internal(0);
    }
  }
  else
  {
    /* Update GSM with UE's band preference */
    rr_rrc_set_band_pref_internal(rrc_csp_int_data.bplmn_band_pref);
  }
  
#ifdef FEATURE_WCDMA_MULTIBAND
  rrc_csp_curr_select_data.prioritized_bands_read_idx = 0;
  rrc_csp_curr_select_data.num_of_prioritized_bands = 0;
  /* Reset the next_band_mask appropriately */
  WRRC_MSG0_HIGH("Resetting the next_band_mask and addl_chnl_done_mask");
  rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;
  rrc_csp_curr_select_data.addl_chnl_done_mask = 0;

  rrc_csp_curr_select_data.first_band_to_be_scanned = RRC_CSP_BAND_MAX;

    if((rrc_csp_curr_select_data.first_band_to_be_scanned =
        rrccsp_get_next_band_to_be_scanned()) == RRC_CSP_BAND_MAX)
    {
      WRRC_MSG0_ERROR("UE doesn't support a valid WCDMA band. Defaulting to IMT2k");
      rrc_csp_curr_select_data.first_band_to_be_scanned = RRC_CSP_IMT2K_BAND;
      rrc_csp_curr_select_data.current_band =
        rrc_csp_curr_select_data.first_band_to_be_scanned;
    }
    else
    {
      rrc_csp_curr_select_data.current_band =
        rrc_csp_curr_select_data.first_band_to_be_scanned;
      WRRC_MSG1_HIGH("1st band to be scanned = %d",
                 rrc_csp_curr_select_data.first_band_to_be_scanned);
    }

    /* Reset the next_band_mask */
    rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;
    WRRC_MSG1_HIGH("Resetting the addl_chnl_done_mask. next_band_mask reset to %d", rrc_csp_curr_select_data.next_band_mask);

    rrc_csp_curr_select_data.addl_chnl_done_mask = 0;
    
#endif

  rrc_csp_curr_select_data.num_available_freq = 0;

  /* Now set the requested PLMN in Internal data to "Any PLMN".
   * This implies that the actual PLMN value is not valid.
   */
  rrc_csp_int_data.curr_req_plmn.plmn_type = RRC_GSM_MAP_ANY_PLMN_REQ;

  /* Initialize RCE just in case it was waiting for UE to camp on,
   * e.g., RRC_EST_REQ. Abort is sent to MM by this function, if needed.
   */
  rrcrce_force_initialize(TRUE);

  /* Initialize variables associated with WTOG BPLMN search */
  rrc_csp_init_wtog_plmn_srch_int_data();

#ifdef FEATURE_EOOS
  #error code not present
#endif

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  rrc_csp_int_data.umts_chls_from_gsm.size = 0;
#endif

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  rrc_csp_int_data.detected_frequency_list.num_freqs = 0;
#endif

  /* First call a CCM function to indicate Cell Loss - no new cell */
  /* NOTE: The functional I/F has to be used very carefully. */
  rrc_ccm_update_with_cell_loss();

  /* Release all channels and put lower layers to Idle before sending another 
   * Acquisition Req or a Frequency Scan Req.
   */
  rrc_csp_send_chan_release();

  WRRC_MSG2_HIGH("WTOW: dual_mode_enabled = %d, wcdma_mode_enabled = %d in PLMN_LIST_REQ", 
                          rrc_csp_int_data.bplmn_dual_mode_enabled,rrc_csp_int_data.bplmn_wcdma_mode_enabled);

  rrccsp_generate_rat_order_for_manual_plmn_srch();

  rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx = 0;

  if(rrc_csp_int_data.plmn_srch_order.rat[0] != SYS_RAT_UMTS_RADIO_ACCESS)
  {
    /* Send idle request to L1 followed by suspend request */
    rrc_csp_send_l1_idle_req();

#ifdef FEATURE_WCDMA_TO_LTE
    if(rrc_csp_int_data.plmn_srch_order.rat[0] == SYS_RAT_GSM_RADIO_ACCESS)
    {
      rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_GSM;
    }
    else if (rrc_csp_int_data.plmn_srch_order.rat[0] == SYS_RAT_LTE_RADIO_ACCESS)
    {
      rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_LTE;
    }
#endif
    return(rrc_csp_resume_wtog_bplmn_srch());
  }
  else
  {
    WRRC_MSG0_HIGH("WTOG OOS: Do Manual PLMN Srch in fgnd on W first");

    if(rrc_csp_start_cell_selection
        (
          &rrc_csp_int_data.curr_req_plmn,
          &rrc_csp_int_data.curr_type_acq, 
          &rrc_csp_int_data.curr_acq_entry,
          &type_freq_scan
        )   == FAILURE
      )
    {
      WRRC_MSG0_HIGH("No freq by Stored Initial cell selection");
      return (rrccsp_rel_chls_and_go_to_idle());
    }

    /* Another freq found */
    if(type_freq_scan == RRC_CSP_NO_SCAN)
    {
      /* This is unacceptable since a SUCCESS was returned */
      ERR_FATAL("NO_SCAN returned", 0, 0, 0);
    }

#ifdef FEATURE_DUAL_SIM
    rrc_start_lock_wait_timer(RRCTMR_DEFAULT_SEARCH_TIME_IN_MS,TRUE);
    rrc_csp_int_data.shld_acq_be_spltd = TRUE;
    if((rrc_csp_wait_for_wrm_lock(&type_freq_scan) == RRCCSP_NO_CELL_SELECTED) && (type_freq_scan == RRC_CSP_NO_SCAN))
    {
      return (RRCCSP_NO_CELL_SELECTED);
    }
#endif
 
    rrc_csp_send_next_freq_to_l1(&rrc_csp_int_data.curr_acq_entry, type_freq_scan);

    if(type_freq_scan == RRC_CSP_ACQ_SCAN)
    {
      /* Start waiting for Acquisition Confirmation */
      next_substate = RRCCSP_CELL_SEL_WT_ACQ;
    }
    else
    {
      /* Start waiting for Frequency scan Confirmation */
      next_substate = RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF;
    }                     
    return(next_substate);
  }

} /* rrc_csp_start_sel_for_plmn_list */


/*===========================================================================

FUNCTION          rrccsp_update_plmn_list

DESCRIPTION       This function reads the SIBs for a cell and checks if it is
                  barred or in the forbidden lai list. If not, it will try to
                  add the PLMN to the PLMN list to be returned to MM in the
                  SERVICE_CNF.
                  
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_update_plmn_list(rrc_plmn_identity_type plmn_id, rrc_int_cell_id_type cell_id)
{
  /* Temporary storage for LAI status */
  rrc_csp_lai_status_e_type  lai_status;

  /* This stores the LAI received in SIB1 */
  rrc_lai_type  lai;

  /* This stores the GSM-MAP NAS system info */
  rrc_gsm_map_nas_sys_info_type   nas_common_info;

  /* Pointer to SIB1 */
  rrc_SysInfoType1 *sib1_ptr = NULL;

  unsigned char  *data_ptr;
  uint32         i,idx=0;
  uint32         num_plmns;

  /* Temporary storage for Cell Barred Status */
  rrc_csp_cell_bar_e_type  cell_bar_status;

  /* Pointer & counter to loop through the CN System Information List */
  rrc_CN_DomainSysInfoList * cn_ptr = NULL;
  uint8 cn_cnt                    = 0;
#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
  #error code not present
#else
  l1_rssi_info_struct_type l1_rssi;
#endif
  sys_signal_strength_type signal_strength;
  boolean plmn_found = FALSE;
  boolean get_sib1_ptr = TRUE;


  uint32 shared_plmn_idx = 0;
  boolean is_current_cell_nw_shared = rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing;
  boolean found_shared_plmn = FALSE;

#ifdef FEATURE_EOOS
  #error code not present
#endif

  rrc_plmn_identity_type common_plmn_id;
#ifdef FEATURE_FEMTO_CSG
  if(rrccsp_asf_actively_in_prog())
  {
    WRRC_MSG0_HIGH("CSG: For ASF no need to update plmn list");
    return;
  }
#endif
  if(rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing)
  {
    common_plmn_id = rrc_csp_int_data.curr_nw_db_entry.common_plmn_id;
  }
  else
  {
    common_plmn_id = plmn_id;
  }
#ifndef FEATURE_WCDMA_FREE_FLOATING_TASK
#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
  #error code not present
#else
  l1_get_rssi(&l1_rssi);
  signal_strength = l1_rssi.rssi_value;
#endif
#else
#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
  #error code not present
#else
  rrcff_l1_get_rssi(&l1_rssi);
  signal_strength = l1_rssi.rssi_value;
#endif
#endif

  WRRC_MSG1_HIGH("RSSI Signal Strength is %d",signal_strength);

  /* This check has been added for cr109422. During foreground
   * service search if a cell is found on which UE can camp on
   * then set the var plmn_fnd_during_fgnd_svc_srch to TRUE.
   * This var has been added to inform NAS (using var ue_oos 
   * in RRC_SERVICE_CNF) that service is available. NAS looks
   * at this var to determine if limited service request should
   * be sent to RRC or not. Notice that with UOOS, SD always sends
   * Automatic Service Request and it sends Limited Service Request
   * only when NAS informs SD that Limited Service is available
   */
  if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
  {
#ifdef FEATURE_EOOS
    #error code not present
#endif
    {
      rrc_csp_int_data.continue_cell_selection = FALSE;
      if(!rrc_is_s_criteria_met(common_plmn_id,cell_id))
      {
        rrc_csp_int_data.continue_cell_selection = TRUE;
        /*S-Criteria failure, neither report the PLMN nor set the flag plmn_fnd_during_fgnd_svc_srch to TRUE*/
        WRRC_MSG0_HIGH("S criteria failed, do not add PLMN to available list");
        return;
      }
      rrc_csp_int_data.plmn_fnd_during_fgnd_svc_srch = TRUE;
#ifdef FEATURE_EOOS
      #error code not present
#endif
    }
  }

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
  if((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) ||
      ((rrc_csp_int_data.curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN) && 
       (rrc_csp_int_data.prev_curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)))
    {
    if((!rrc_csp_int_data.oosc_plmn_found) && (rrc_is_s_criteria_met(plmn_id,cell_id)))
      {
        rrc_csp_int_data.oosc_plmn_found = TRUE;
      rrc_csp_int_data.oosc_l1_rssi    = (uint8)signal_strength;
  
        WRRC_MSG0_HIGH("SVC_ST: Imm update UI w/ at least limited svc");
      rrc_csp_send_svc_status_to_mm(SYS_SRV_STATUS_LIMITED, SYS_RAT_UMTS_RADIO_ACCESS,signal_strength);
      }
      else
      {
        WRRC_MSG0_HIGH("SVC_ST: do NOT update UI w/ lim svc status. UI already updated");
      }
    }
#endif

  /* If scan type is not PLMN_LIST_SCAN and PLMN type is not ANY_PLMN,
   * do not add the PLMN to the list if it's the requested PLMN
   */
  if((rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN) &&
      (rrc_csp_int_data.curr_req_plmn.plmn_type != RRC_GSM_MAP_ANY_PLMN_REQ))
  {
    /*feature code clean up for FEATURE NETWORK SHARING*/
  }

  /* If scan type is not PLMN_LIST_SCAN and PLMN type is ANY_PLMN,
   * then do not add the PLMN to the available PLMN list as RRC 
   * should just camp on this PLMN if it is "acceptable" and there 
   * is no need to report it to NAS.
   */
  else if((rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN) &&
          (rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ))
  {
    WRRC_MSG0_HIGH("Any PLMN Srch. Don't add PLMN to available plmn list");
    return;
  }


  /* If scan type is OOS_Scan then the available plmn list should not be populated.
   * This is because during OOS search, RRC will camp on the PLMN autonomously
   * and send service indication to NAS with service available instead of
   * reporting the PLMNs to NAS and triggering PLMN selection via NAS. For details 
   * please see the memo on the above feature
   */
  if(rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
  {
    WRRC_MSG0_HIGH("OOS: Do NOT populate avail_plmn_list for OOS_Scan");
    return;
  }

#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
  if(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
  {
    get_sib1_ptr = FALSE;
  }
#endif

  if(get_sib1_ptr == TRUE)
  {
    /* Now generate status mask for this cell */
    /* Fill up the NAS system info here */
    /* Let us first read SIB1 - we should have SIB1 for sure */
    if(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
    {
      sib1_ptr = (rrc_SysInfoType1 *) rrc_bplmn_return_sib_for_cell(
                                             rrc_SIB1);
    }
    else
    {
      
      sib1_ptr = (rrc_SysInfoType1 *) rrc_sibdb_return_sib_for_cell(
                                              rrc_SIB1,
                                              common_plmn_id, 
                                              cell_id);
    }

    if( sib1_ptr == NULL )
    {
      /* SIB3 for this PLMN, CEll ID combination not present */
      ERR("SIB1 for MCC %d, MNC %d, Cell ID %d not present",
                        RRC_CSP_GET_INT_MCC(common_plmn_id), 
                        RRC_CSP_GET_INT_MNC(common_plmn_id),
                        cell_id);
  
      return;
    } /* SIB1 not present */
  
    /* The SIB1 is present */
    nas_common_info.length = (uint32)sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts;
  
    /* Check if the RRC MM interface structure array can hold this info */
    if(sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts > RRC_MAX_GSM_INFO_OCTET_COUNT)
    {
      WRRC_MSG1_ERROR("Not enough space for Common NAS Sys Info (%d bytes)",
                          sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts);
      return;
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
    if(mm_extract_lac_from_sib_info(&lai, &nas_common_info) == FALSE)
    {
      WRRC_MSG0_ERROR("LAC length is less than 2 bytes");
      return;
    }
  }

  if((rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN) &&
      (rrc_csp_int_data.curr_req_plmn.plmn_type != RRC_GSM_MAP_ANY_PLMN_REQ)
       && is_current_cell_nw_shared)
  {
    for(i=0; i< rrc_csp_int_data.curr_nw_db_entry.num_of_shared_plmns; i++)
    {
      plmn_id = rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[i].plmn_id;
      if( rrc_bcch_and_nas_plmns_match(
            plmn_id, rrc_csp_int_data.curr_req_plmn.plmn_id))
      {
        WRRC_MSG0_HIGH("requested PLMN is in Shared list");
        found_shared_plmn = TRUE;
        break;
      }
    }
  }
  if(!found_shared_plmn)
  {
    do
    {
      if((is_current_cell_nw_shared) && (shared_plmn_idx < rrc_csp_int_data.curr_nw_db_entry.num_of_shared_plmns))
      {
        plmn_id = rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx++].plmn_id;
      }
  
      /* If scan type is not PLMN_LIST_SCAN and PLMN type is not ANY_PLMN,
       * do not add the PLMN to the list if it's the requested PLMN
       */
      if((rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN) &&
          (rrc_csp_int_data.curr_req_plmn.plmn_type != RRC_GSM_MAP_ANY_PLMN_REQ))
      {
        if( rrc_bcch_and_nas_plmns_match(
              plmn_id, rrc_csp_int_data.curr_req_plmn.plmn_id))
        {
          WRRC_MSG0_HIGH("PLMN is the requested PLMN");
          continue;
        }
      }
  
  
      /* If the we already have reached the MAX_PLMNS no need to go any further */
      if(rrc_csp_int_data.plmn_list.num_plmns < RRC_MAX_PLMNS)
      {
        /* Initialize num_plmns to the number of PLMNs in the CSP internal data */
        num_plmns = rrc_csp_int_data.plmn_list.num_plmns;
  
#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
  
        if(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
        {
          WRRC_MSG1_HIGH("WTOW: SIB1/3 not read. Setting defaults. Scan = %d",rrc_csp_int_data.curr_scan);
          /* Check if the PLMN is already present in the PLMN list
           * If the PLMN is not present in the PLMN list, add it
           */

          for(i=0; (i< num_plmns) && !plmn_found; i++)
          {
            /* Check if the PLMN is already in the list */
            if( RRC_CSP_SAME_MCC(plmn_id, rrc_csp_int_data.plmn_list.plmn[i].plmn) &&
                RRC_CSP_SAME_MNC(plmn_id, rrc_csp_int_data.plmn_list.plmn[i].plmn)
              )
            {
#ifdef FEATURE_FEMTO_CSG
              if((rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_CSG)||
                 (rrc_csp_int_data.curr_acq_entry.csg_id == rrc_csp_int_data.plmn_list.plmn[i].csg_info.csg_id))
#endif
              {
                WRRC_MSG2_HIGH("PLMN %d-%d already in the PLMN list", RRC_CSP_GET_INT_MCC(plmn_id),
                                                RRC_CSP_GET_INT_MNC(plmn_id));
                plmn_found = TRUE;

                if(signal_strength < rrc_csp_int_data.plmn_list.plmn[i].signal_strength)
                {
                  rrc_csp_int_data.plmn_list.plmn[i].signal_strength = signal_strength;

                  if(rrc_csp_int_data.plmn_list.plmn[i].signal_strength <= 95)
                  {
                    rrc_csp_int_data.plmn_list.plmn[i].signal_quality = SYS_SIGNAL_QUALITY_HIGH;
                  }
                }
              }
            }
          }
    
          if(plmn_found)
          {
            plmn_found = FALSE;
            continue;

          }
    
          rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                   = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
    
          WRRC_MSG2_HIGH("Adding PLMN %d-%d", RRC_CSP_GET_INT_MCC(plmn_id),
                                                  RRC_CSP_GET_INT_MNC(plmn_id));
          rrc_csp_int_data.plmn_list.plmn[num_plmns].plmn = plmn_id;
#ifdef FEATURE_FEMTO_CSG
          rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.csg_id = rrc_csp_int_data.curr_acq_entry.csg_id;
          rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;

#endif
          rrc_csp_int_data.plmn_list.plmn[num_plmns].rat 
                                                           = SYS_RAT_UMTS_RADIO_ACCESS;
          rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength = signal_strength;
          if(rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength <= 95)
          {
            rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality= SYS_SIGNAL_QUALITY_HIGH;
          }
          else
          {
            rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality 
                                                           = SYS_SIGNAL_QUALITY_LOW;
          }
  
          rrc_csp_int_data.plmn_list.num_plmns++;
  
          continue;

        }
#endif

        /* Since MM doesn't know the PLMN from NAS system info, fill in the PLMN now */
        lai.plmn = plmn_id;
      
        /* If it's a Manual PLMN selection scan don't check for
         * forbidden LAIs
         */
        if(
             (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN)
         
          && (rrc_csp_int_data.curr_scan != RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
     
          )
        {
          if(rrc_csp_find_forbid_lai(&lai, &lai_status) 
             == FAILURE)
          {
            WRRC_MSG0_HIGH("Failure finding LAI status");
            return;
          }
    
          /* Check if the cell belongs to a forbidden LAI */
          if(lai_status != RRC_CSP_LAI_OK)
          {
            MSG_MED("Cell belongs to a forbidden LAI",0,0,0);
            continue;

          }
        }
    
        /* Generate Cell barred status based on SIB info for this cell */
        if(!rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing)
        {
          common_plmn_id = plmn_id;
        }

        if(rrc_csp_find_cell_bar_status(plmn_id, 
                                        cell_id, 
                                        &cell_bar_status
                                        ,common_plmn_id
                                      ) 
            == FAILURE)
        {
          WRRC_MSG0_HIGH("Failure to fill Cell Barred Status");
          return;
        }
      
        if(cell_bar_status != RRC_CSP_CELL_OK)
        {
          MSG_MED("Cell is barred",0,0,0);
          continue;

        }
    
        /* Check if the PLMN is already present in the PLMN list
         * If the PLMN is not present in the PLMN list, add it
         */
    
        for(i=0; (i< num_plmns) && !plmn_found; i++)
        {
          /* Check if the PLMN is already in the list */
          if( RRC_CSP_SAME_MCC(plmn_id, rrc_csp_int_data.plmn_list.plmn[i].plmn) &&
              RRC_CSP_SAME_MNC(plmn_id, rrc_csp_int_data.plmn_list.plmn[i].plmn)
            )
          {
#ifdef FEATURE_FEMTO_CSG
              if((rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_CSG)||
                 (rrc_csp_int_data.curr_acq_entry.csg_id == rrc_csp_int_data.plmn_list.plmn[i].csg_info.csg_id))
#endif
              {
                WRRC_MSG2_HIGH("PLMN %d-%d already in the PLMN list", RRC_CSP_GET_INT_MCC(plmn_id),
                                                RRC_CSP_GET_INT_MNC(plmn_id));
                plmn_found = TRUE;

                if(signal_strength < rrc_csp_int_data.plmn_list.plmn[i].signal_strength)
                {
                  rrc_csp_int_data.plmn_list.plmn[i].signal_strength = signal_strength;

                  if(rrc_csp_int_data.plmn_list.plmn[i].signal_strength <= 95)
                  {
                    rrc_csp_int_data.plmn_list.plmn[i].signal_quality= SYS_SIGNAL_QUALITY_HIGH;
                  }
                }
              }
           }
        }
    
        if(plmn_found)
        {
          plmn_found = FALSE;
          continue;

        }
  
        ASSERT(sib1_ptr != NULL);
    
        /* Get the CN Domain identities from the CN Domain Sys Info List */
        cn_ptr = &sib1_ptr->cn_DomainSysInfoList;
  
        cn_cnt = 0;
        idx=0;
        while((cn_ptr->n > idx) && (cn_cnt < UE_MAX_CN_DOMAINS))
        {
          switch(cn_ptr->elem[idx].cn_DomainIdentity)
          {
            case rrc_CN_DomainIdentity_cs_domain:
              if(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                   == SYS_SRV_CAPABILITY_PS_SERVICE_ONLY)
              {
                rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                         = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
              }
              else
              {
                rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                         = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
              }
              break;
      
            case rrc_CN_DomainIdentity_ps_domain:
              if(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                   == SYS_SRV_CAPABILITY_CS_SERVICE_ONLY)
              {
                rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                         = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
              }
              else
              {
                rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                         = SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
              }
              break;
            
            default:
              WRRC_MSG1_HIGH("Invalid CN 0x%x for Service Capability", cn_ptr->elem[idx].cn_DomainIdentity);
              break;
          } /* end CN Identity switch */
      
          /* If both CS and PS services are supported break out of the loop */
          if(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability 
               == SYS_SRV_CAPABILITY_CS_PS_SERVICE)
          {
             break;
          }
          /* Next CN Info */
          idx++;
          cn_cnt++;
        } /* end while Core Network Info */
    
        /* If there was no CN domain information set the service capability to SYS_SRV_CAPABILITY_NONE */
        switch(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability)
        {
          case SYS_SRV_CAPABILITY_PS_SERVICE_ONLY:
          case SYS_SRV_CAPABILITY_CS_SERVICE_ONLY:
          case SYS_SRV_CAPABILITY_CS_PS_SERVICE:
            break;
          default:
            WRRC_MSG0_HIGH("No service capability for the PLMN");
            rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                                         = SYS_SRV_CAPABILITY_NONE;
        }
      
        WRRC_MSG2_HIGH("Adding PLMN %d-%d", RRC_CSP_GET_INT_MCC(plmn_id),
                                                RRC_CSP_GET_INT_MNC(plmn_id));
        rrc_csp_int_data.plmn_list.plmn[num_plmns].plmn = plmn_id;
#ifdef FEATURE_FEMTO_CSG
        rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.csg_id = rrc_csp_int_data.curr_acq_entry.csg_id;
        rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;
#endif
        rrc_csp_int_data.plmn_list.plmn[num_plmns].rat 
                                                         = SYS_RAT_UMTS_RADIO_ACCESS;
        rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength = signal_strength;
        if(rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength <= 95)
        {
          rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality = SYS_SIGNAL_QUALITY_HIGH;
        }
        else
        {
          rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality 
                                                        = SYS_SIGNAL_QUALITY_LOW;
        }
    
        rrc_csp_int_data.plmn_list.num_plmns++;
      }
      else
      {
        WRRC_MSG0_HIGH("Reached the max number of PLMNs");
        return;
      }
    }while((is_current_cell_nw_shared) && (shared_plmn_idx < rrc_csp_int_data.curr_nw_db_entry.num_of_shared_plmns));
  }
  return;

} /* rrccsp_update_plmn_list */


/*===========================================================================

FUNCTION          rrccsp_update_background_plmn_list

DESCRIPTION       This function reads the SIBs for a cell and checks if it is
                  barred or in the forbidden lai list. If not, it will try to
                  add it to the background PLMN list to be returned to RR in the
                  RRC_INTERRAT_PLMN_SRCH_CNF or RRC_INTERRAT_PLMN_SRCH_ABORT_CNF.
                  
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_update_background_plmn_list(rrc_plmn_identity_type plmn_id, rrc_int_cell_id_type cell_id)
{
  /* Temporary storage for LAI status */
  rrc_csp_lai_status_e_type  lai_status;

  /* This stores the LAI received in SIB1 */
  rrc_lai_type  lai;

  /* This stores the GSM-MAP NAS system info */
  rrc_gsm_map_nas_sys_info_type   nas_common_info;

  rrc_plmn_identity_type nas_to_rrc_plmn_id;

  /* Pointer to SIB1 */
  rrc_SysInfoType1 *sib1_ptr = NULL;

  unsigned char  *data_ptr;
  uint32         i,idx=0;
  uint32         num_plmns;

  /* Temporary storage for Cell Barred Status */
  rrc_csp_cell_bar_e_type  cell_bar_status;

  /* Pointer & counter to loop through the CN System Information List */
  rrc_CN_DomainSysInfoList* cn_ptr = NULL;
  uint8 cn_cnt                    = 0;

#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
  #error code not present
#else
  l1_rssi_info_struct_type l1_rssi;
#endif

  sys_signal_strength_type signal_strength;
  boolean plmn_found = FALSE;
  boolean get_sib1_ptr = TRUE;

  uint32 shared_plmn_idx = 0;
  boolean is_current_cell_nw_shared = rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing;
#ifdef FEATURE_FEMTO_CSG
  if(rrccsp_asf_actively_in_prog())
  {
    WRRC_MSG0_HIGH("CSG: For ASF no need to update plmn list");
    return;
  }
#endif

#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
  if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
  {
    get_sib1_ptr = FALSE;
  }
#endif

  if(get_sib1_ptr == TRUE)
  {
    /* Now generate status mask for this cell */
    /* Fill up the NAS system info here */
    /* Let us first read SIB1 - we should have SIB1 for sure */
    sib1_ptr = (rrc_SysInfoType1 *) rrc_bplmn_return_sib_for_cell(rrc_SIB1);
    if( sib1_ptr == NULL )
    {
      // SIB3 for this PLMN, CEll ID combination not present 
      ERR("SIB1 for MCC %d, MNC %d, Cell ID %d not present",
                        RRC_CSP_GET_INT_MCC(plmn_id), 
                        RRC_CSP_GET_INT_MNC(plmn_id),
                        cell_id);
      return;
    } // SIB1 not present 

    /* The SIB1 is present */
    nas_common_info.length = (uint32)sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts;

    /* Check if the RRC MM interface structure array can hold this info */
    if(sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts > RRC_MAX_GSM_INFO_OCTET_COUNT)
    {
      WRRC_MSG1_ERROR("Not enough space for Common NAS Sys Info (%d bytes)",
                          sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts);
      return;
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
    if(mm_extract_lac_from_sib_info(&lai, &nas_common_info) == FALSE)
    {
      WRRC_MSG0_ERROR("LAC length is less than 2 bytes");
      return;
    }
  }

#ifndef FEATURE_WCDMA_FREE_FLOATING_TASK
#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
  #error code not present
#else
  memset(&l1_rssi, 0, sizeof(l1_rssi_info_struct_type));
  l1_get_rssi(&l1_rssi);
  signal_strength = l1_rssi.rssi_value;
#endif
#else
#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
  #error code not present
#else
  memset(&l1_rssi, 0, sizeof(l1_rssi_info_struct_type));
  rrcff_l1_get_rssi(&l1_rssi);
  signal_strength = l1_rssi.rssi_value;
#endif
#endif

  WRRC_MSG1_HIGH("RSSI Signal Strength is %d",signal_strength);

  do
  {
    if( (is_current_cell_nw_shared) && 
        (shared_plmn_idx < rrc_csp_int_data.curr_nw_db_entry.num_of_shared_plmns) &&
        (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) )
    {
      plmn_id = rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx++].plmn_id;
    }

    /* If the we already have reached the MAX_PLMNS no need to go any further */
    if(rrc_csp_int_data.bplmn_fnd_list_wr_index < SYS_PLMN_LIST_MAX_LENGTH)
    {
      /* Initialize num_plmns to the number of PLMNs in the CSP internal data */
      num_plmns = rrc_csp_int_data.bplmn_fnd_list_wr_index;

#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
      if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
      {
        WRRC_MSG1_HIGH("GTOW BPLMN:ManualScan. SIB1/3 not read. Setting defaults. Scan = %d",rrc_csp_int_data.curr_scan);
        /* Check if the PLMN is already present in the PLMN list
         * If the PLMN is not present in the PLMN list, add it
         */
  
        for(i = 0; (i < num_plmns) && !plmn_found; i++)
        {
          nas_to_rrc_plmn_id 
              = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(
                   rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].plmn);
  
          /* Check if the PLMN is already in the list */
          if( RRC_CSP_SAME_MCC(nas_to_rrc_plmn_id, plmn_id) &&
              RRC_CSP_SAME_MNC(nas_to_rrc_plmn_id, plmn_id)
            )
          {
#ifdef FEATURE_FEMTO_CSG
            if((rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_CSG)||
               (rrc_csp_int_data.curr_acq_entry.csg_id == rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].csg_info.csg_id))
#endif
            {
              WRRC_MSG2_HIGH("BPLMN: PLMN %d-%d already in the PLMN list", 
                           RRC_CSP_GET_INT_MCC(plmn_id),
                           RRC_CSP_GET_INT_MNC(plmn_id));
              plmn_found = TRUE;

              if(signal_strength < rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].signal_strength)
              {
                rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].signal_strength = signal_strength;

                if(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].signal_strength <= 95)
                {
                  rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].signal_quality= SYS_SIGNAL_QUALITY_HIGH;
                }
              }
            }
          }
        } /* end for loop */

        if(plmn_found)
        {
          plmn_found = FALSE;
          continue;

        }
  
        WRRC_MSG2_HIGH("GTOW BPLMN:ManualScan. PLMN matched. Adding PLMN %d-%d to found list", 
                  RRC_CSP_GET_INT_MCC(plmn_id), RRC_CSP_GET_INT_MNC(plmn_id));

        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability
                              = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn 
                              = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn_id);
#ifdef FEATURE_FEMTO_CSG	
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].csg_info.csg_id = rrc_csp_int_data.curr_acq_entry.csg_id;
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;  
#endif
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].rat 
                              = SYS_RAT_UMTS_RADIO_ACCESS;
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_strength = signal_strength;
        if(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_strength  <= 95)
        {
          rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_quality
                                                          = SYS_SIGNAL_QUALITY_HIGH;
        }
        else
        {
          rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_quality 
                                                         = SYS_SIGNAL_QUALITY_LOW;
        }

        rrc_csp_int_data.bplmn_fnd_list_wr_index++;                              
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.length 
                              = rrc_csp_int_data.bplmn_fnd_list_wr_index;
        continue;
  
      }
#endif
  
      /* Since MM doesn't know the PLMN from NAS system info, fill in the PLMN now */
      lai.plmn = plmn_id;
    
      /* If it's a Manual PLMN selection scan don't check for
       * forbidden LAIs
       */
  
      if(rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
      {
        if(rrc_csp_find_forbid_lai(&lai, &lai_status) 
           == FAILURE)
        {
          WRRC_MSG0_HIGH("Failure finding LAI status");
          return;
        }
  
        // Check if the cell belongs to a forbidden LAI 
        if(lai_status != RRC_CSP_LAI_OK)
        {
          MSG_MED("Cell belongs to a forbidden LAI",0,0,0);
          return;
        }
      }
    
      /* Generate Cell barred status based on SIB info for this cell */
      if(rrc_csp_find_cell_bar_status(plmn_id, 
                                      cell_id, 
                                      &cell_bar_status
                                      ,plmn_id
                                      ) 
          == FAILURE)
      {
        WRRC_MSG0_HIGH("Failure to fill Cell Barred Status");
        return;
      }
  
      if(cell_bar_status != RRC_CSP_CELL_OK)
      {
        MSG_MED("Cell is barred",0,0,0);
        continue;

      }
    
      /* Check if the PLMN is already present in the PLMN list
       * If the PLMN is not present in the PLMN list, add it
       */
    
      for(i = 0; (i < num_plmns) && !plmn_found; i++)
      {
        nas_to_rrc_plmn_id 
            = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(
                 rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].plmn);
  
        /* Check if the PLMN is already in the list */
        if( RRC_CSP_SAME_MCC(nas_to_rrc_plmn_id, plmn_id) &&
            RRC_CSP_SAME_MNC(nas_to_rrc_plmn_id, plmn_id)
          )
        {
#ifdef FEATURE_FEMTO_CSG
          if((rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_CSG)||
             (rrc_csp_int_data.curr_acq_entry.csg_id == rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].csg_info.csg_id))
#endif
          {
            WRRC_MSG2_HIGH("BPLMN: PLMN %d-%d already in the PLMN list", 
                         RRC_CSP_GET_INT_MCC(plmn_id),
                         RRC_CSP_GET_INT_MNC(plmn_id));
            plmn_found = TRUE;
          }
        }
      } /* end for loop */
  
      if(plmn_found)
      {
        plmn_found = FALSE;
        continue;

      }
  
      ASSERT(sib1_ptr != NULL);
  
      /* Get the CN Domain identities from the CN Domain Sys Info List */
      cn_ptr = &sib1_ptr->cn_DomainSysInfoList;
  
      cn_cnt = 0;
      idx=0;
      while((cn_ptr->n > idx) && (cn_cnt < UE_MAX_CN_DOMAINS))
      {
        switch(cn_ptr->elem[idx].cn_DomainIdentity)
        {
        case rrc_CN_DomainIdentity_cs_domain:
            
            if(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability
                 == SYS_SRV_CAPABILITY_PS_SERVICE_ONLY)
            {
              rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability
                                       = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
            }
            else
            {
              rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability
                                       = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
            }
            break;
    
          case rrc_CN_DomainIdentity_ps_domain:
            if(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability
                 == SYS_SRV_CAPABILITY_CS_SERVICE_ONLY)
            {
              rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability
                                       = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
            }
            else
            {
              rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability
                                       = SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
            }
            break;
          
          default:
            WRRC_MSG1_HIGH("Invalid CN 0x%x for Service Capability", cn_ptr->elem[idx].cn_DomainIdentity);
            break;
        } /* end CN Identity switch */
    
        /* If both CS and PS services are supported break out of the loop */
        if(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability
             == SYS_SRV_CAPABILITY_CS_PS_SERVICE)
        {
          break;
        }
        /* Next CN Info */
        
        idx++;
        cn_cnt++;
      } /* end while Core Network Info */
      
      /* If there was no CN domain information set the service capability to SYS_SRV_CAPABILITY_NONE */
      switch(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability)
      {
        case SYS_SRV_CAPABILITY_PS_SERVICE_ONLY:
        case SYS_SRV_CAPABILITY_CS_SERVICE_ONLY:
        case SYS_SRV_CAPABILITY_CS_PS_SERVICE:
          break;
        default:
          WRRC_MSG0_HIGH("No service capability for the PLMN");
          rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn_service_capability
                                                       = SYS_SRV_CAPABILITY_NONE;
      }

      WRRC_MSG2_HIGH("Adding PLMN %d-%d", RRC_CSP_GET_INT_MCC(plmn_id),
                                              RRC_CSP_GET_INT_MNC(plmn_id));
  
      if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
      {
        WRRC_MSG0_HIGH("BPLMN:ManualScan. PLMN matched. Adding PLMN to found list");
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn 
            = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn_id);
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].rat 
                                                       = SYS_RAT_UMTS_RADIO_ACCESS;
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_strength = signal_strength;
        if(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_strength <= 95)
        {
          rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_quality 
                                                         = SYS_SIGNAL_QUALITY_HIGH;
        }
        else
        {
          rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_quality 
                                                         = SYS_SIGNAL_QUALITY_LOW;
        }

        rrc_csp_int_data.bplmn_fnd_list_wr_index++;                              
        rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.length = rrc_csp_int_data.bplmn_fnd_list_wr_index;
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
      {
        for(i = 0; i < rrc_csp_int_data.bplmn_num_req; ++i)
        {
          if(rrc_bcch_and_nas_plmns_match(rrc_csp_int_data.curr_acq_entry.plmn_id,
                                              rrc_csp_int_data.bplmn_req_list[i].plmn_id)
             /* For background service search, the PLMNs should be reported to NAS even
              * if they do not match with those sent by NAS in the PLMN_LIST_REQ.
              * Hence the check here for the enum service_search_bgnd.
              */
             || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC)
             || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL)
             || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_PRIORITY_PLMN)
                /* The new search type is for a customer requirement where if the UE is camped on HPLMN,
                 * NAS has to periodically trigger Automatic search(SYS_SERVICE_SEARCH_PRIORITY_PLMN) for 
                 * searching another customer's PLMN*/
#ifdef FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN
             || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_NONE)
#endif
            )
          {
            WRRC_MSG0_HIGH("BPLMN:AutoScan. PLMN matched. Adding PLMN to found list");
            rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].plmn 
                                                       = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn_id);
#ifdef FEATURE_FEMTO_CSG
	    rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].csg_info.csg_id = rrc_csp_int_data.curr_acq_entry.csg_id;
            rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;
#endif
            rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].rat 
                                                       = SYS_RAT_UMTS_RADIO_ACCESS;
            rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_strength = signal_strength;
            if(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_strength <= 95)
            {
              rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_quality 
                                                             = SYS_SIGNAL_QUALITY_HIGH;
            }
            else
            {
              rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[num_plmns].signal_quality 
                                                             = SYS_SIGNAL_QUALITY_LOW;
            }
  
            rrc_csp_int_data.bplmn_fnd_list_wr_index++;                              
            rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.length 
                                                       = rrc_csp_int_data.bplmn_fnd_list_wr_index;
            return;
          }
        }/* end for loop */   
      }/* end RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN */     
      else
      {
        WRRC_MSG0_ERROR("update_bplmn_list fn clld whn NOT BPLMN_AUTO/MANUAL SCAN");
      }
    }
    else
    {
      WRRC_MSG0_HIGH("Reached the max number of PLMNs");
      return;
    }
  }while( (is_current_cell_nw_shared) && 
          (shared_plmn_idx < rrc_csp_int_data.curr_nw_db_entry.num_of_shared_plmns) &&
          (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) );

  return;
} /* rrccsp_update_background_plmn_list */

/*===========================================================================

FUNCTION          RRCCSP_UPDATE_WTOW_BACKGROUND_PLMN_LIST

DESCRIPTION       This function reads the SIBs for a cell and checks if it is
                  barred or in the forbidden lai list. If not, it will try to
                  add it to the WTOW background PLMN list to be returned to MM in the
                  RRC_PLMN_LIST_CNF.
                  
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_update_wtow_background_plmn_list(
  rrc_plmn_identity_type plmn_id, rrc_int_cell_id_type cell_id)
{
  /* Temporary storage for LAI status */
  rrc_csp_lai_status_e_type  lai_status;

  /* This stores the LAI received in SIB1 */
  rrc_lai_type  lai;

  /* This stores the GSM-MAP NAS system info */
  rrc_gsm_map_nas_sys_info_type   nas_common_info;

  /* Pointer to SIB1 */
  rrc_SysInfoType1 *sib1_ptr = NULL;

  unsigned char  *data_ptr;
  uint32         i,idx=0;
  uint32         num_plmns;

  /* Temporary storage for Cell Barred Status */
  rrc_csp_cell_bar_e_type  cell_bar_status;

  /* Pointer & counter to loop through the CN System Information List */
  rrc_CN_DomainSysInfoList* cn_ptr = NULL;
  uint8 cn_cnt                    = 0;
  l1_rssi_info_struct_type l1_rssi;
  sys_signal_strength_type signal_strength;
  boolean plmn_found = FALSE;
  boolean get_sib1_ptr = TRUE;

  uint32 shared_plmn_idx = 0;
  boolean is_current_cell_nw_shared = rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing;

#ifdef FEATURE_FEMTO_CSG
  /* DUring internal ASF search no list is sent to NAS */
  if(rrc_csp_int_data.csg_asf_srch_in_progress &&
     (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))
  {
    WRRC_MSG0_HIGH("CSG: No need to add entry to plmn_list for ASF search. ");
    return;
  }
#endif

#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
  if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
  {
    get_sib1_ptr = FALSE;
  }
#endif

  if(get_sib1_ptr == TRUE)
  {
    /* Now generate status mask for this cell */
    /* Fill up the NAS system info here */
    /* Let us first read SIB1 - we should have SIB1 for sure */
    if((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
        || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
        || (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
        || (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
        || (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN))
    {
      sib1_ptr = (rrc_SysInfoType1 *) rrc_bplmn_return_sib_for_cell(rrc_SIB1);
    }

    if( sib1_ptr == NULL )
    {
      // SIB3 for this PLMN, CEll ID combination not present 
      ERR("SIB1 for MCC %d, MNC %d, Cell ID %d not present",
                        RRC_CSP_GET_INT_MCC(plmn_id), 
                        RRC_CSP_GET_INT_MNC(plmn_id),
                        cell_id);
  
      return;
    } // SIB1 not present 
  
    /* The SIB1 is present */
    nas_common_info.length = (uint32)sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts;
  
    /* Check if the RRC MM interface structure array can hold this info */
    if(sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts > RRC_MAX_GSM_INFO_OCTET_COUNT)
    {
      WRRC_MSG1_ERROR("Not enough space for Common NAS Sys Info (%d bytes)",
                          sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts);
      return;
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
    if(mm_extract_lac_from_sib_info(&lai, &nas_common_info) == FALSE)
    {
      WRRC_MSG0_ERROR("LAC length is less than 2 bytes");
      return;
    }
  }

  l1_get_rssi_during_wtow_bplmn(&l1_rssi);
  signal_strength = l1_rssi.rssi_value;
  WRRC_MSG1_HIGH("RSSI Signal Strength is %d",signal_strength);

  do
  {
    if( (is_current_cell_nw_shared) &&
        (shared_plmn_idx < rrc_csp_int_data.curr_nw_db_entry.num_of_shared_plmns) &&
        (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN) )
    {
      plmn_id = rrc_csp_int_data.curr_nw_db_entry.shared_plmns_access_info[shared_plmn_idx++].plmn_id;
    }

    /* If the we already have reached the MAX_PLMNS no need to go any further */
    if(rrc_csp_int_data.bplmn_fnd_list_wr_index < SYS_PLMN_LIST_MAX_LENGTH)
    {
      /* Initialize num_plmns to the number of PLMNs in the CSP internal data */
      num_plmns = rrc_csp_int_data.bplmn_fnd_list_wr_index;
  
#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
      if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
      {
        WRRC_MSG1_HIGH("WTOW: SIB1/3 not read. Setting defaults. Scan = %d",rrc_csp_int_data.curr_scan);
        /* Check if the PLMN is already present in the PLMN list
         * If the PLMN is not present in the PLMN list, add it
         */
  
        for(i=0; (i< num_plmns) && !plmn_found; i++)
        {
          /* Check if the PLMN is already in the list */
          if( RRC_CSP_SAME_MCC(plmn_id, rrc_csp_int_data.plmn_list.plmn[i].plmn) &&
              RRC_CSP_SAME_MNC(plmn_id, rrc_csp_int_data.plmn_list.plmn[i].plmn)
            )
          {
#ifdef FEATURE_FEMTO_CSG
            if((rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_CSG)||
               (rrc_csp_int_data.curr_acq_entry.csg_id == rrc_csp_int_data.plmn_list.plmn[i].csg_info.csg_id))
#endif
            {
              WRRC_MSG2_HIGH("PLMN %d-%d already in the PLMN list", RRC_CSP_GET_INT_MCC(plmn_id),
                                                  RRC_CSP_GET_INT_MNC(plmn_id));
              plmn_found = TRUE;

              if(signal_strength < rrc_csp_int_data.plmn_list.plmn[i].signal_strength)
              {
                rrc_csp_int_data.plmn_list.plmn[i].signal_strength = signal_strength;

                if(rrc_csp_int_data.plmn_list.plmn[i].signal_strength <= 95)
                {
                  rrc_csp_int_data.plmn_list.plmn[i].signal_quality= SYS_SIGNAL_QUALITY_HIGH;
                }
              }
            }
          }
        }
  
        if(plmn_found)
        {
          plmn_found = FALSE;
          continue;

        }
        WRRC_MSG2_HIGH("BPLMN:ManualScan. PLMN matched. Adding PLMN %d-%d to found list", 
                  RRC_CSP_GET_INT_MCC(plmn_id), RRC_CSP_GET_INT_MNC(plmn_id));
  
        rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                                                   = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
        rrc_csp_int_data.plmn_list.plmn[num_plmns].plmn            = plmn_id;
#ifdef FEATURE_FEMTO_CSG
        rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.csg_id = rrc_csp_int_data.curr_acq_entry.csg_id;
        rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;
#endif
        rrc_csp_int_data.plmn_list.plmn[num_plmns].rat             = SYS_RAT_UMTS_RADIO_ACCESS; 
        rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength = signal_strength;
        if(rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength <= 95)
        {
          rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality 
                                                         = SYS_SIGNAL_QUALITY_HIGH;
        }
        else
        {
          rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality 
                                                         = SYS_SIGNAL_QUALITY_LOW;
        }
  
        rrc_csp_int_data.bplmn_fnd_list_wr_index++;   
  
        continue;
  
      }
#endif
  
      /* Since MM doesn't know the PLMN from NAS system info, fill in the PLMN now */
      lai.plmn = plmn_id;
    
      /* If it's a Manual PLMN selection scan don't check for
       * forbidden LAIs
       */
  
      if(rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
      {
        if(rrc_csp_find_forbid_lai(&lai, &lai_status) 
           == FAILURE)
        {
          WRRC_MSG0_HIGH("Failure finding LAI status");
          return;
        }
  
        // Check if the cell belongs to a forbidden LAI 
        if(lai_status != RRC_CSP_LAI_OK)
        {
          MSG_MED("Cell belongs to a forbidden LAI",0,0,0);
          return;
        }
      }
    
      /* Generate Cell barred status based on SIB info for this cell */
      if(rrc_csp_find_cell_bar_status(plmn_id, 
                                      cell_id, 
                                      &cell_bar_status
                                      ,plmn_id
                                      ) 
          == FAILURE)
      {
        WRRC_MSG0_HIGH("Failure to fill Cell Barred Status");
        return;
      }
  
      if(cell_bar_status != RRC_CSP_CELL_OK)
      {
         MSG_MED("Cell is barred",0,0,0);
        continue;
      }
    
      /* Check if the PLMN is already present in the PLMN list
       * If the PLMN is not present in the PLMN list, add it
       */
    
      for(i = 0; (i < num_plmns) && !plmn_found; i++)
      {
        /* Check if the PLMN is already in the list */
        if( RRC_CSP_SAME_MCC(plmn_id, rrc_csp_int_data.plmn_list.plmn[i].plmn) &&
            RRC_CSP_SAME_MNC(plmn_id, rrc_csp_int_data.plmn_list.plmn[i].plmn)
          ) 
        {
#ifdef FEATURE_FEMTO_CSG
          if((rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_CSG)||
             (rrc_csp_int_data.curr_acq_entry.csg_id == rrc_csp_int_data.plmn_list.plmn[i].csg_info.csg_id))
#endif
          {
            WRRC_MSG2_HIGH("BPLMN: PLMN %d-%d already in the PLMN list", 
                         RRC_CSP_GET_INT_MCC(plmn_id),
                         RRC_CSP_GET_INT_MNC(plmn_id));
            plmn_found = TRUE;

            if(signal_strength < rrc_csp_int_data.plmn_list.plmn[i].signal_strength)
            {
              rrc_csp_int_data.plmn_list.plmn[i].signal_strength = signal_strength;

              if(rrc_csp_int_data.plmn_list.plmn[i].signal_strength <= 95)
              {
                rrc_csp_int_data.plmn_list.plmn[i].signal_quality= SYS_SIGNAL_QUALITY_HIGH;
              }
            }
          }
        }
      } /* end for loop */
  
      if(plmn_found)
      {
        plmn_found = FALSE;
        continue;

      }
  
      ASSERT(sib1_ptr != NULL);
  
      /* Get the CN Domain identities from the CN Domain Sys Info List */
      cn_ptr = &sib1_ptr->cn_DomainSysInfoList;
  
      cn_cnt = 0;
      idx=0;
      while((cn_ptr->n > idx) && (cn_cnt < UE_MAX_CN_DOMAINS))
      {
        switch(cn_ptr->elem[idx].cn_DomainIdentity)
        {
        case rrc_CN_DomainIdentity_cs_domain:
            
            if(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                 == SYS_SRV_CAPABILITY_PS_SERVICE_ONLY)
            {
              rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                       = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
            }
            else
            {
              rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                       = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
            }
            break;
    
          case rrc_CN_DomainIdentity_ps_domain:
            if(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                 == SYS_SRV_CAPABILITY_CS_SERVICE_ONLY)
            {
              rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                       = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
            }
            else
            {
              rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                       = SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
            }
            break;
          
          default:
            WRRC_MSG1_HIGH("Invalid CN 0x%x for Service Capability", cn_ptr->elem[idx].cn_DomainIdentity);
            break;
        } /* end CN Identity switch */
    
        /* If both CS and PS services are supported break out of the loop */
        if(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
             == SYS_SRV_CAPABILITY_CS_PS_SERVICE)
        {
          break;
        }
        /* Next CN Info */
        
        idx++;
        cn_cnt++;
      } /* end while Core Network Info */
      
      /* If there was no CN domain information set the service capability to SYS_SRV_CAPABILITY_NONE */
      switch(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability)
      {
        case SYS_SRV_CAPABILITY_PS_SERVICE_ONLY:
        case SYS_SRV_CAPABILITY_CS_SERVICE_ONLY:
        case SYS_SRV_CAPABILITY_CS_PS_SERVICE:
          break;
        default:
          WRRC_MSG0_HIGH("No service capability for the PLMN");
          rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                                       = SYS_SRV_CAPABILITY_NONE;
      }
      
      if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
      {
        WRRC_MSG2_HIGH("BPLMN:ManualScan. PLMN matched. Adding PLMN %d-%d to found list", 
                  RRC_CSP_GET_INT_MCC(plmn_id), RRC_CSP_GET_INT_MNC(plmn_id));
        rrc_csp_int_data.plmn_list.plmn[num_plmns].plmn = plmn_id;
#ifdef FEATURE_FEMTO_CSG
        rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.csg_id = rrc_csp_int_data.curr_acq_entry.csg_id;
        rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;
#endif
        rrc_csp_int_data.plmn_list.plmn[num_plmns].rat = SYS_RAT_UMTS_RADIO_ACCESS; 
        rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength = signal_strength;
        if(rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength <= 95)
        {
          rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality = SYS_SIGNAL_QUALITY_HIGH;
        }
        else
        {
          rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality 
                                                         = SYS_SIGNAL_QUALITY_LOW;
        }
  
        rrc_csp_int_data.bplmn_fnd_list_wr_index++;                              
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
      {
        for(i = 0; i < rrc_csp_int_data.wtow_bplmn_num_req; ++i)
        {
          if(rrc_bcch_and_nas_plmns_match(rrc_csp_int_data.curr_acq_entry.plmn_id,
                                              rrc_csp_int_data.wtow_bplmn_req_list[i].plmn_id)
             /* For background service search, the PLMNs should be reported to NAS even
              * if they do not match with those sent by NAS in the PLMN_LIST_REQ.
              * Hence the check here for the enum service_search_bgnd.
              */
             || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC)
             || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL)
             || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_PRIORITY_PLMN)
                /* The new search type is for a customer requirement where if the UE is camped on HPLMN,
                 * NAS has to periodically trigger Automatic search(SYS_SERVICE_SEARCH_PRIORITY_PLMN) for 
                 * searching another customer's PLMN*/
#ifdef FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN
             || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_NONE)
#endif
            )
          {
            WRRC_MSG2_HIGH("WTOW: AutoScan. PLMN matched. Adding PLMN %d-%d to found list", 
                      RRC_CSP_GET_INT_MCC(plmn_id), RRC_CSP_GET_INT_MNC(plmn_id));
            rrc_csp_int_data.plmn_list.plmn[num_plmns].plmn = plmn_id;
            rrc_csp_int_data.plmn_list.plmn[num_plmns].rat = SYS_RAT_UMTS_RADIO_ACCESS; 
            rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength = signal_strength;
            if(rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength <= 95)
            {
              rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality = SYS_SIGNAL_QUALITY_HIGH;
            }
            else
            {
              rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality 
                                                             = SYS_SIGNAL_QUALITY_LOW;
            }
  
            rrc_csp_int_data.bplmn_fnd_list_wr_index++;                              
            return;
          }
        }/* end for loop */   
      }/* end RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN */     
      else
      {
        WRRC_MSG0_ERROR("ERR: update_wtow_background_plmn_list fn called whn NOT BPLMN_AUTO/MANUAL SCAN");
      }
    }
    else
    {
      WRRC_MSG0_ERROR("ERR: Reached the max number of PLMNs");
      return;
    }
  }while( (is_current_cell_nw_shared) &&
          (shared_plmn_idx < rrc_csp_int_data.curr_nw_db_entry.num_of_shared_plmns) &&
          (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN) );
  return;
} /* rrccsp_update_wtow_background_plmn_list */


/*===========================================================================

FUNCTION          RRCCSP_UPDATE_WTOW_MANUAL_BPLMN_LIST_WITH_RPLMN

DESCRIPTION       This function updates the WTOW Available PLMN list with the 
                  PLMN on which UE is camped on. This function is called only
                  when MANUAL PLMN_LIST_REQ is received from NAS and UE is 
                  in service.
                                    
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_update_wtow_manual_bplmn_list_with_rplmn(rrc_plmn_identity_type plmn_id, 
                                                     rrc_int_cell_id_type cell_id)
{
  uint32         num_plmns;
  uint32         i;

#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
  #error code not present
#else
  l1_rssi_info_struct_type l1_rssi;
#endif

  sys_signal_strength_type signal_strength;

  rrc_csp_band_class_type csp_band_class = rrc_get_frequency_band(rrc_ccm_get_curr_camped_freq(),
                                                                  rrc_ccm_get_curr_camped_plmn());

#ifdef FEATURE_FEMTO_CSG
/* For manual CSG searches add current camped PLMN info, only if serving cell is a CSG*/
  if((rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_CSG)&&(rrc_ccm_get_curr_camped_csg_id() == SYS_CSG_ID_INVALID))
  {
    WRRC_MSG0_HIGH("CSG: Camped on a macro cell do not add current rgstrd PLMN info");
    return;
  }
#endif

  /*Do not add current camped information if camped frequency is in not present in BPLMN band preference*/
  if (!(rrc_csp_int_data.bplmn_band_mask & csp_band_class ))
  {
    WRRC_MSG0_HIGH("Current camped frequency is not present in bplmn band preference");
    return;
  }
  /* If we already have reached the MAX_PLMNS no need to go any further */
  if(rrc_csp_int_data.bplmn_fnd_list_wr_index < SYS_PLMN_LIST_MAX_LENGTH)
  {
    /* Initialize num_plmns to the number of PLMNs in the CSP internal data */
    num_plmns = rrc_csp_int_data.bplmn_fnd_list_wr_index;
    
#ifdef FEATURE_FEMTO_CSG
  rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.csg_id = rrc_csp_int_data.curr_acq_entry.csg_id;
  rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;
#endif

#ifndef FEATURE_WCDMA_FREE_FLOATING_TASK
#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
    #error code not present
#else
    l1_get_rssi(&l1_rssi);
    signal_strength = l1_rssi.rssi_value;
#endif
#else
#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
  #error code not present
#else
  rrcff_l1_get_rssi(&l1_rssi);
  signal_strength = l1_rssi.rssi_value;
#endif
#endif

  WRRC_MSG1_HIGH("RSSI Signal Strength is %d",signal_strength);

#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN

    if(num_plmns > 0)
    {
      WRRC_MSG1_ERROR("WTOW ERR: num_plmns %d has a value > 0", num_plmns);
      return;
    }

    WRRC_MSG2_HIGH("Adding RPLMN %d-%d", RRC_CSP_GET_INT_MCC(plmn_id),
                                            RRC_CSP_GET_INT_MNC(plmn_id));

    rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                                               = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
    rrc_csp_int_data.plmn_list.plmn[num_plmns].plmn            = plmn_id;
    rrc_csp_int_data.plmn_list.plmn[num_plmns].rat             = SYS_RAT_UMTS_RADIO_ACCESS; 

    rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength = signal_strength;
    if(rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength <= 95)
    {
      rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality = SYS_SIGNAL_QUALITY_HIGH;
    }
    else
    {
      rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality = SYS_SIGNAL_QUALITY_LOW;
    }
    rrc_csp_int_data.bplmn_fnd_list_wr_index++;   
    /* if there are any shared plmns, add them to the manual bplmn list */
    if(rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing)
    {
      uint32 ref_index = num_plmns;
      num_plmns++;
      for(i=0; (rrc_csp_int_data.bplmn_fnd_list_wr_index < SYS_PLMN_LIST_MAX_LENGTH) &&
          ((i < rrc_csp_int_data.curr_acq_entry.shared_list_of_plmns.num_of_shared_plmns) && (i< MAX_SHARED_PLMNS)) ; i++)
      {
        if((!RRC_CSP_SAME_MCC(plmn_id, rrc_csp_int_data.curr_acq_entry.shared_list_of_plmns.shared_plmns_access_info[i].plmn_id)) || 
           (!RRC_CSP_SAME_MNC(plmn_id, rrc_csp_int_data.curr_acq_entry.shared_list_of_plmns.shared_plmns_access_info[i].plmn_id)))
        {
#ifdef FEATURE_FEMTO_CSG
         rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.csg_id = rrc_csp_int_data.curr_acq_entry.csg_id;
         rrc_csp_int_data.plmn_list.plmn[num_plmns].csg_info.hybrid_cell = rrc_csp_int_data.curr_acq_entry.cell_is_hybrid;
#endif
          rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
          rrc_csp_int_data.plmn_list.plmn[num_plmns].plmn            = rrc_csp_int_data.curr_acq_entry.shared_list_of_plmns.shared_plmns_access_info[i].plmn_id;
          rrc_csp_int_data.plmn_list.plmn[num_plmns].rat             = SYS_RAT_UMTS_RADIO_ACCESS;
          rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength = signal_strength;
          rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality = rrc_csp_int_data.plmn_list.plmn[ref_index].signal_quality;
          rrc_csp_int_data.bplmn_fnd_list_wr_index++;
          num_plmns++;
        }
      }
    }
    return;
 
#else
   {
     /* This stores the LAI received in SIB1 */
     rrc_lai_type  lai;
   
     /* This stores the GSM-MAP NAS system info */
     rrc_gsm_map_nas_sys_info_type   nas_common_info;
     
     /* Pointer to SIB1 */
     rrc_SysInfoType1 *sib1_ptr;
     
   
     unsigned char  *data_ptr;
     uint32         i,idx=0;
   
     /* Temporary storage for Cell Barred Status */
     rrc_csp_cell_bar_e_type  cell_bar_status;
   
     /* Pointer & counter to loop through the CN System Information List */
     rrc_CN_DomainSysInfoList * cn_ptr = NULL;
     uint8 cn_cnt                    = 0;
       /* Now generate status mask for this cell */
       /* Fill up the NAS system info here */
       /* Let us first read SIB1 - we should have SIB1 for sure */
     sib1_ptr = (rrc_SysInfoType1 *) rrc_bplmn_return_sib_for_cell(
                                                rrc_SIB1);

       
       if( sib1_ptr == NULL )
       {
         // SIB3 for this PLMN, CEll ID combination not present 
         ERR("SIB1 for MCC %d, MNC %d, Cell ID %d not present",
                           RRC_CSP_GET_INT_MCC(plmn_id), 
                           RRC_CSP_GET_INT_MNC(plmn_id),
                           cell_id);
     
         return;
       } // SIB1 not present 
     
       /* The SIB1 is present */
       nas_common_info.length = (uint32)sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts;
     
       /* Check if the RRC MM interface structure array can hold this info */
       if(sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts > RRC_MAX_GSM_INFO_OCTET_COUNT)
       {
         WRRC_MSG1_ERROR("Not enough space for Common NAS Sys Info (%d bytes)",
                             sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts);
         return;
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
     
       if(mm_extract_lac_from_sib_info(&lai, &nas_common_info) == FALSE)
       {
         WRRC_MSG0_ERROR("LAC length is less than 2 bytes");
         return;
       }
       
       
       /* Since MM doesn't know the PLMN from NAS system info, fill in the PLMN now */
       lai.plmn = plmn_id;
     
       /* Generate Cell barred status based on SIB info for this cell */
       if(rrc_csp_find_cell_bar_status(plmn_id, 
                                       cell_id, 
                                       &cell_bar_status
                                      ,plmn_id
                                       ) 
           == FAILURE)
       {
         WRRC_MSG0_HIGH("Failure to fill Cell Barred Status");
         return;
       }
   
       if(cell_bar_status != RRC_CSP_CELL_OK)
       {
          MSG_MED("Cell is barred",0,0,0);
          return;
       }
     
       /* Get the CN Domain identities from the CN Domain Sys Info List */
       cn_ptr = &sib1_ptr->cn_DomainSysInfoList;
       idx=0;
       while((cn_ptr->n > idx) && (cn_cnt < UE_MAX_CN_DOMAINS))
       {
         switch(cn_ptr->elem[idx].cn_DomainIdentity)
         {
           case rrc_CN_DomainIdentity_cs_domain:
   
             if(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                  == SYS_SRV_CAPABILITY_PS_SERVICE_ONLY)
             {
               rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                        = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
             }
             else
             {
               rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                        = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
             }
             break;
   
           case rrc_CN_DomainIdentity_ps_domain:
             if(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                  == SYS_SRV_CAPABILITY_CS_SERVICE_ONLY)
             {
               rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                        = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
             }
             else
             {
               rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                        = SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
             }
             break;
   
           default:
             WRRC_MSG1_HIGH("Invalid CN 0x%x for Service Capability", cn_ptr->elem[idx].cn_DomainIdentity);
             break;
         } /* end CN Identity switch */
   
         /* If both CS and PS services are supported break out of the loop */
         if(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
              == SYS_SRV_CAPABILITY_CS_PS_SERVICE)
         {
           break;
         }
         /* Next CN Info */
   
         idx++;
         cn_cnt++;
       } /* end while Core Network Info */
   
       /* If there was no CN domain information set the service capability to SYS_SRV_CAPABILITY_NONE */
       switch(rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability)
       {
         case SYS_SRV_CAPABILITY_PS_SERVICE_ONLY:
         case SYS_SRV_CAPABILITY_CS_SERVICE_ONLY:
         case SYS_SRV_CAPABILITY_CS_PS_SERVICE:
           break;
         default:
           WRRC_MSG0_HIGH("No service capability for the PLMN");
           rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability
                                                        = SYS_SRV_CAPABILITY_NONE;
       }
   
       WRRC_MSG2_HIGH("Adding RPLMN %d-%d", RRC_CSP_GET_INT_MCC(plmn_id),
                                               RRC_CSP_GET_INT_MNC(plmn_id));
       rrc_csp_int_data.plmn_list.plmn[num_plmns].plmn = plmn_id;
       rrc_csp_int_data.plmn_list.plmn[num_plmns].rat  = SYS_RAT_UMTS_RADIO_ACCESS; 

       rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength = signal_strength;
       if(rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength <= 95)
       {
         rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality = SYS_SIGNAL_QUALITY_HIGH;
       }
       else
       {
         rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality = SYS_SIGNAL_QUALITY_LOW;
       }
       rrc_csp_int_data.bplmn_fnd_list_wr_index++;    
       /* if there are any shared plmns, add them to the manual bplmn list */
       if(rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing)
       {
         uint32 ref_index = num_plmns;
         num_plmns++;
         for(i=0; rrc_csp_int_data.bplmn_fnd_list_wr_index < SYS_PLMN_LIST_MAX_LENGTH &&
             i < rrc_csp_int_data.curr_acq_entry.shared_list_of_plmns.num_of_shared_plmns ; i++)
         {
           if((!RRC_CSP_SAME_MCC(plmn_id, rrc_csp_int_data.curr_acq_entry.shared_list_of_plmns.shared_plmns_access_info[i].plmn_id)) || 
              (!RRC_CSP_SAME_MNC(plmn_id, rrc_csp_int_data.curr_acq_entry.shared_list_of_plmns.shared_plmns_access_info[i].plmn_id)))
           {
             rrc_csp_int_data.plmn_list.plmn[num_plmns].service_capability = rrc_csp_int_data.plmn_list.plmn[ref_index].service_capability;
             rrc_csp_int_data.plmn_list.plmn[num_plmns].plmn            = rrc_csp_int_data.curr_acq_entry.shared_list_of_plmns.shared_plmns_access_info[i].plmn_id;
             rrc_csp_int_data.plmn_list.plmn[num_plmns].rat             = SYS_RAT_UMTS_RADIO_ACCESS;
             rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_strength = signal_strength;
             rrc_csp_int_data.plmn_list.plmn[num_plmns].signal_quality = rrc_csp_int_data.plmn_list.plmn[ref_index].signal_quality;
             rrc_csp_int_data.bplmn_fnd_list_wr_index++;
             num_plmns++;
           }
         }
       }                                        
     }
#endif 
  }  
  else
  {
    WRRC_MSG0_HIGH("Reached the max number of PLMNs");
  } 
  return;
} /* rrccsp_update_wtow_manual_bplmn_list_with_rplmn */


/*===========================================================================

FUNCTION          rrccsp_continue_bplmn_in_frgrnd

DESCRIPTION       This function is used to continue ongoing MPLMN Search in background 
                  to foreground.
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP Substate
SIDE EFFECTS
                  None

===========================================================================*/



rrc_csp_substate_e_type
rrccsp_continue_bplmn_in_frgrnd(void)
{

  rrc_csp_substate_e_type   next_substate = RRCCSP_NO_CELL_SELECTED;
  uint8 trans_id;
  sys_service_search_e_type srvc_srch_bgnd;

  /* Restore BPLMN Context*/
  if(rrc_csp_int_data.bplmn_srch_vars_status == WTOW_SEARCH_SUSPEND)
  {
    if (bplmn_data_saved)
    {
      /* If BPLMN data is saved restore the needy ones and reset backed up data*/
      bplmn_data_saved = FALSE; 
      /* Resetting the backed up data */
      rrc_csp_restore_saved_bplmn_data();
    }

    rrc_csp_int_data.bplmn_acq_succ_scr_code = 8192;
    rrc_csp_int_data.bplmn_acq_succ_freq = 0;
    rrc_csp_int_data.bplmn_acq_succ_pn_pos = 0xFFFFFFFF;
    rrc_csp_int_data.bplmn_current_band_initialized = FALSE;
    rrc_csp_int_data.bplmn_sib_try_next_best_cell = FALSE;
    rrc_csp_int_data.bplmn_acq_status             = FALSE;
    rrc_csp_int_data.bplmn_bgnd_mib_rd_cnt  = 0;
    rrc_csp_int_data.bplmn_bgnd_mib_rd_freq = 0;
    rrc_csp_int_data.bplmn_suspend_cause = BPLMN_SUSPEND_REASON_NONE;  
    
    if(!rrc_csp_int_data.bplmn_guard_srch_tmr_expired)
    {
      rrctmr_stop_timer(RRCTMR_BPLMN_GUARD_SRCH_TIMER);
      rrc_csp_int_data.bplmn_guard_srch_tmr_expired = TRUE;
    }

    rrcrce_force_initialize(TRUE);

    rrc_csp_int_data.curr_scan = RRC_CSP_PLMN_LIST_SCAN;
    rrc_csp_int_data.curr_req_plmn.plmn_type = RRC_GSM_MAP_ANY_PLMN_REQ;
    rrc_csp_curr_select_data.acq_type = RRC_CSP_ANY_PLMN;  
    rrc_csp_int_data.plmn_list.num_plmns = rrc_csp_int_data.bplmn_fnd_list_wr_index;

    WRRC_MSG1_HIGH("WTOG: Updating GSM with band preferences for PLMN search,BST band pref incl %d",rrc_bst_band_pref_incl);
    if(rrc_bst_band_pref_incl)
    {
      /* Update GSM with UE's band preference */
      rr_rrc_set_band_pref_internal(rrc_csp_int_data.rat_pri_band_pref);

      /* Update GSM with BST band preference */
      rr_rrc_set_bst_band_pref_internal(rrc_csp_int_data.bplmn_band_pref);
    }
    else
    {
      /* Update GSM with UE's band preference */
      rr_rrc_set_band_pref_internal(rrc_csp_int_data.bplmn_band_pref);

      /* Update GSM with BST band preference */
      rr_rrc_set_bst_band_pref_internal(0);
    }

    if(!rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active)
    {
      WRRC_MSG0_HIGH("Continue BPLMN search on W in foreground");
      next_substate = rrc_csp_start_bplmn_srch();
    }
    /* WTOG BPLMN search is in progress */
    else
    {
      WRRC_MSG0_HIGH("Continue BPLMN search on G in foreground");
      /* Send idle request to L1 followed by suspend request */
      rrc_csp_send_l1_idle_req();
      next_substate = rrc_csp_resume_wtog_bplmn_srch();
    }
  }
  else
  {
    WRRC_MSG0_HIGH("Restart PLMN Search in Foreground");
    rrc_csp_int_data.plmn_list.num_plmns = 0;
    trans_id = rrc_csp_int_data.bplmn_trans_id;
    srvc_srch_bgnd = rrc_csp_int_data.service_search_bgnd;
    rrccsp_reset_bplmn_list_search_state(WTOW_SEARCH_STOP, FALSE, FALSE);
    rrc_csp_int_data.bplmn_trans_id= trans_id;
    rrc_csp_int_data.service_search_bgnd = srvc_srch_bgnd;
    next_substate = rrc_csp_start_sel_for_plmn_list();
  }
   
  return(next_substate);
  
}

/*===========================================================================

FUNCTION          rrccsp_is_state_transition_triggered_by_reconfig_msg

DESCRIPTION       This function returns a TRUE if the state transition was
                  trigerred by a Reconfig message and a FALSE when it was
                  done by UE on its own (e.g. due to Radio Link Failure)                  
DEPENDENCIES
                  None
RETURN VALUE
                  Boolean
SIDE EFFECTS
                  None

===========================================================================*/
boolean  rrccsp_is_state_transition_triggered_by_reconfig_msg( void )
{
  boolean         trg_frm_reconfig_msg = TRUE;
  rrc_proc_e_type proc;
  rrcllc_oc_process_state_e_type process_status;

  /* Return FALSE if the state transition was triggered by CU procedure */
  if (OC_SET_FOR_DCH_FACH_TRANS == 
      rrcllc_get_ordered_config_state_and_proc( &proc,
                                                &process_status
                                              ) )
  {
    if (proc == RRC_PROCEDURE_CU)
    {
      trg_frm_reconfig_msg = FALSE;   
    }
  }
  return(trg_frm_reconfig_msg);
}

/*===========================================================================

FUNCTION          rrccsp_is_selected_cell_utran_directed

DESCRIPTION       This function returns a TRUE if the selected cell is the
                  same as directed by UTRAN and a FALSE when it is
                  different or when the cell to be camped is not indicated
                  by UTRAN.
DEPENDENCIES
                  This function should be called ONLY IF a call to
                  rrccsp_is_state_transition_triggered_by_reconfig_msg returns
                  a TRUE.
RETURN VALUE
                  Boolean
SIDE EFFECTS
                  None

===========================================================================*/
boolean  rrccsp_is_selected_cell_utran_directed(void)
{
  /* If both destination frequency and destination PSC are present and
   * the acquired cell is on the same freauency and PSC, return a TRUE.
   * In all other cases, return a FALSE.
   */
  if(rrc_csp_int_data.dest_freq_present)
  {
    if(rrc_ccm_get_curr_camped_freq() == rrc_csp_int_data.dest_freq)
    {
      if(rrc_csp_int_data.dest_psc_present)
      {
        if(rrc_ccm_get_curr_camped_psc() == rrc_csp_int_data.dest_psc)
        {
          return(TRUE);
        }
      }
    }
  }

  return(FALSE);
} /* rrccsp_is_selected_cell_utran_directed */

#ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW
/*===========================================================================

FUNCTION          RRC_CSP_SEND_MM_ACT_IND

DESCRIPTION       This function is responsible for filling up data structures
                  for Activation Indication to MM sent by CSP.
                  This also allocates cmd buffer and sends the command.
                  It also frees allocated command and embedded buffers
                  when failure is detected before sending the command.

DEPENDENCIES
                  
RETURN VALUE
                  SUCCESS: Command was successfully sent to MM.
                  FAILURE: Command could not be sent due to any one of multiple
                           reasons.

SIDE EFFECTS
                  
===========================================================================*/
void  
rrc_csp_send_mm_act_ind( void )
{
  mm_cmd_type *mm_buf_ptr;
#ifdef FEATURE_FEMTO_CSG
  lte_rrc_wcdma_resel_rsp_s resel_rsp;
#endif

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_ACTIVATION_IND);

  if(rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
  {
#if defined (FEATURE_LTE_TO_WCDMA) || defined (FEATURE_WCDMA_TO_LTE)
    if(rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE)
    {
      mm_buf_ptr->cmd.rrc_activation_ind.status = MM_AS_INTERRAT_REDIRECTION;
    }
    else
#endif
    {
      mm_buf_ptr->cmd.rrc_activation_ind.status = MM_AS_GTOW_REDIRECTION;
    }
    /*update the cause for activation indication*/
    rrcmcm_update_wcdma_activation_cause(RRCMCM_INTERRAT_CELL_REDIRECTION);
  }
  else
  {
    mm_buf_ptr->cmd.rrc_activation_ind.status = MM_AS_INTERRAT_RESELECTION;
    /*update the cause for activation indication*/
    rrcmcm_update_wcdma_activation_cause(RRCMCM_INTERRAT_CELL_RESELECTION);

#ifdef FEATURE_FEMTO_CSG
    rrc_csp_int_data.csg_resel = FALSE;
    /*Send WCDMA_RRC_LTE_RESEL_RSP only for L CSG to W macro reselections */
#ifdef FEATURE_LTE_TO_WCDMA
    if((rrc_ccm_get_curr_camped_csg_id() == SYS_CSG_ID_INVALID)&&
       (rrc_csp_int_data.last_csg_rat == RRC_CSG_NONE)&&
       (rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE))
    {
       memset(&resel_rsp,0,sizeof(lte_rrc_wcdma_resel_rsp_s));

       resel_rsp.w_cell_info.frequency      = rrc_ccm_get_curr_camped_freq();
       resel_rsp.w_cell_info.phy_cell_id    = rrc_ccm_get_curr_camped_psc();
       resel_rsp.w_cell_info.cell_identity  = rrc_ccm_get_curr_camped_cell_id();
       resel_rsp.w_cell_info.plmn           = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(rrc_ccm_get_curr_camped_plmn());
       resel_rsp.w_cell_info.csg_id         = rrc_ccm_get_curr_camped_csg_id();

       /*fp_macro is updated with LTE macro when handling resel req from LTE*/
       resel_rsp.lte_cell_info.frequency      = rrc_csp_int_data.fp_macro.freq;
       resel_rsp.lte_cell_info.phy_cell_id            = rrc_csp_int_data.fp_macro.pci;
       resel_rsp.lte_cell_info.cell_identity        = rrc_csp_int_data.fp_macro.cell_id;
       resel_rsp.lte_cell_info.plmn           = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(rrc_csp_int_data.fp_macro.plmn);
       resel_rsp.lte_cell_info.csg_id         = rrc_csp_int_data.fp_macro.csg_id;


       /*LTE_RRC_WCDMA_RESEL_REQ*/
       rrc_send_lte_cmd(&resel_rsp.msg_hdr,WCDMA_RRC_LTE_RESEL_RSP,sizeof(lte_rrc_wcdma_resel_rsp_s));
       WRRC_MSG0_HIGH("WTOL: WCDMA_RRC_LTE_RESEL_RSP sent to LTE RRC");

    }
#endif
#endif
  }

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_activation_ind.as_id = rrc_get_as_id();
#endif

  /* Put the command on MM queue */
  WRRC_MSG1_HIGH("Dispatching out MM cmd RRC_ACTIVATION_IND / Sending MM cmd 0x%x", RRC_ACTIVATION_IND);
  rrc_send_rrc_message_to_mm(mm_buf_ptr);

} /* rrc_csp_send_mm_act_ind */

#endif


/*===========================================================================

FUNCTION          RRCCSP_FILL_INFO_FOR_RR

DESCRIPTION       This function is responsible for filling the band preference
                  and the network select mode to be sent to RR.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
void rrccsp_fill_info_for_rr(sys_network_selection_mode_e_type *network_select_mode)
{
  *network_select_mode = rrc_csp_int_data.network_select_mode;
} /* rrccsp_fill_info_for_rr */


/*===========================================================================

FUNCTION          RRCCSP_SET_UE_IN_DEEP_SLEEP

DESCRIPTION       This function is responsible for setting the deep sleep variable
                  when UE is going to deep sleep.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
void rrccsp_set_ue_in_deep_sleep(void)
{
  rrc_csp_int_data.ue_in_deep_sleep = TRUE;

} /* rrccsp_set_ue_in_deep_sleep */


/*===========================================================================

FUNCTION          RRCCSP_UPDATE_BAND_PREFERENCE

DESCRIPTION       This function updates the information related to supported
                  frequency bands based on the band_pref mask.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
void rrccsp_update_band_preference(void)
{
  rrc_csp_int_data.wcdma_band_mask = 0;
  rrc_csp_int_data.deep_sleep_acq_db_band_mask = 0;

  if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_I_IMT_2000)
  {
    WRRC_MSG0_HIGH("IMT2k band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_IMT2K_BAND;
  }

  if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_II_PCS_1900)
  {
    WRRC_MSG0_HIGH("1900 band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_PCS_BAND;
  }

  if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_V_850)
  {
    WRRC_MSG0_HIGH("850 band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_WCDMA_850_BAND;
  }

  if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_VI_800)
  {
    WRRC_MSG0_HIGH("800 MHz band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_WCDMA_800_BAND;
  }

  if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_III_1700)
  {
    WRRC_MSG0_HIGH("1800 MHz band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_WCDMA_1800_BAND;
  }

  if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_VIII_900)
  {
    WRRC_MSG0_HIGH("900 MHz band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_WCDMA_900_BAND;
  }

  if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_IX_1700)
  {
    WRRC_MSG0_HIGH("1700 MHz band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_WCDMA_1700_BAND;
  }

  if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_IV_1700)
  {
    WRRC_MSG0_HIGH("1700_2100 (AWS) MHz band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_WCDMA_1700_2100_BAND;
  }

  if(rrc_csp_int_data.band_pref & SYS_BAND_MASK_WCDMA_XI_1500)
  {
    WRRC_MSG0_HIGH("1500 MHz band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_WCDMA_1500_BAND;
  }

  if(rrc_csp_int_data.band_pref &SYS_BAND_MASK_WCDMA_XIX_850)
  {
    WRRC_MSG0_HIGH("BC19: band supported by the UE");
    rrc_csp_int_data.wcdma_band_mask = rrc_csp_int_data.wcdma_band_mask |
                                       RRC_CSP_WCDMA_BC19_BAND;
  }

  WRRC_MSG1_HIGH("WCDMA Bandclass bitmask : %d",rrc_csp_int_data.wcdma_band_mask);


#ifndef FEATURE_MCC_AND_850_800_BAND_COMBINATION
  /* Now update the band class for all acq db entries as some
   * entries may have mask set to indicate that they support both
   * 800 as well as 850 MHz bands.
   */
  rrccsp_update_band_class_in_acq_db();
#endif

}

/*===========================================================================

FUNCTION          RRCCSP_IS_BAND_CLASS_ENABLED_IN_UI_RF

DESCRIPTION       This function is used to check if the band_class is enabled in
                  both the UI and RF.
                  The band_class passed to the function is added to the wcdma_band_mask
                  (CSP uses this band_mask to determine what all bands it should search
                  for when UE goes OOS), if the second argument passed to this function
                  is set to TRUE. E.g., when this function is called from inside the 
                  function validate_fdd_frequency()/validate_dl_arfcn() then the value 
                  of this argument is TRUE. This is because the function 
                  validate_fdd_frequency() is called when the frequency is received from 
                  UTRAN in SIBs or redirection or reconfig messages, etc. and for all these 
                  cases the design is that the frequency will be added to the wcdma_band_mask. 
                  Now, when this function is called from inside the function 
                  rrccsp_get_supported_band_list() then the value of the argument is always 
                  FALSE as the function rrccsp_get_supported_band_list() is called by UECI 
                  procedure only to check which bands are supported by the UE - UI && RF settings.
DEPENDENCIES
                  
RETURN VALUE
                  TRUE: band_class is enabled in UI and RF.
                  FALSE: Otherwise. 

SIDE EFFECTS
                  None.
                  
===========================================================================*/
boolean rrccsp_is_band_class_enabled_in_ui_rf(rrc_csp_band_class_type band_class,
                                              boolean update_wcdma_band_mask)
{
  boolean status = FALSE;

  sd_ss_band_pref_e_type ui_rf_band_capability;

  /* Get band capability of the UE from SD. The mask returned indicates the bands the UE
   * is capable of supporting, and which are selected via the UI. 
   */
  #ifdef FEATURE_DUAL_SIM
  ui_rf_band_capability = (sd_ss_band_pref_e_type) sd_misc_get_subs_supp_band_pref(rrc_get_as_id());
  #else
  ui_rf_band_capability = (sd_ss_band_pref_e_type) sd_misc_get_supp_band_pref();
  #endif


  if(band_class == RRC_CSP_PCS_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_II_PCS_1900)
    {
      WRRC_MSG0_HIGH("UI_RF: 1900 band supported by the UI/RF");

      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_PCS_BAND;
      }

      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 1900 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }
  
  else if(band_class == RRC_CSP_IMT2K_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_I_IMT_2000)
    {
      WRRC_MSG0_HIGH("UI_RF: IMT2k band supported by the UI/RF");
      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_IMT2K_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: IMT2k band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }
  
  else if(band_class == RRC_CSP_WCDMA_800_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_VI_800)
    {
      WRRC_MSG0_HIGH("UI_RF: 800 band supported by the UI/RF");
      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_800_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 800 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }
  
  else if(band_class == RRC_CSP_WCDMA_850_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_V_850)
    {
      WRRC_MSG0_HIGH("UI_RF: 850 band supported by the UI/RF");

      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_850_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 850 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }

  else if(band_class == (RRC_CSP_WCDMA_800_BAND | RRC_CSP_WCDMA_850_BAND))
  {
    /* Check if 800 band is enabled in the UI/RF */
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_VI_800)
    {
      WRRC_MSG0_HIGH("UI_RF: 800 band supported by the UI/RF");
      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_800_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 800 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }

    /* Check if 850 band is enabled in the UI/RF */
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_V_850)
    {
      WRRC_MSG0_HIGH("UI_RF: 850 band supported by the UI/RF");
  
      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_850_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 850 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }

  else if(band_class == RRC_CSP_WCDMA_1800_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_III_1700)
    {
      WRRC_MSG0_HIGH("UI_RF: 1800 band supported by the UI/RF");

      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_1800_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 1800 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }
  
  else if(band_class == RRC_CSP_WCDMA_900_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_VIII_900)
    {
      WRRC_MSG0_HIGH("UI_RF: 900 band supported by the UI/RF");

      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_900_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 900 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }
  
  else if(band_class == RRC_CSP_WCDMA_1700_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_IX_1700)
    {
      WRRC_MSG0_HIGH("UI_RF: 1700 band supported by the UI/RF");

      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_1700_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 1700 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }
  
  else if(band_class == RRC_CSP_WCDMA_1700_2100_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_IV_1700)
    {
      WRRC_MSG0_HIGH("UI_RF: 1700_2100 band supported by the UI/RF");

      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_1700_2100_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 1700_2100 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }
  
  else if(band_class == RRC_CSP_WCDMA_1500_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_XI_1500)
    {
      WRRC_MSG0_HIGH("UI_RF: 1500 band supported by the UI/RF");
      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_1500_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: 1500 band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }

  else if(band_class == RRC_CSP_WCDMA_BC19_BAND)
  {
    if(ui_rf_band_capability & SD_SS_BAND_PREF_WCDMA_XIX_850)
    {
      WRRC_MSG0_HIGH("UI_RF: BC19: band supported by the UI/RF");

      if(update_wcdma_band_mask)
      {
        WRRC_MSG1_HIGH("UI_RF: Add it to the wcdma_band_mask %d", rrc_csp_int_data.wcdma_band_mask);
        rrc_csp_int_data.wcdma_band_mask |= RRC_CSP_WCDMA_BC19_BAND;
      } 
      status = TRUE;
    }
    else
    {
      WRRC_MSG1_HIGH("UI_RF: BC19: band NOT supported by the UI/RF. wcdma_band_mask %d", 
                                            rrc_csp_int_data.wcdma_band_mask);
    }
  }

  else
  {
    WRRC_MSG1_ERROR("UI_RF: incorrect band_class %d passed", band_class);
  }


  if(update_wcdma_band_mask && status)
  {
#ifndef FEATURE_MCC_AND_850_800_BAND_COMBINATION
  /* Now update the band class for all acq db entries as some
   * entries may have mask set to indicate that they support both
   * 800 as well as 850 MHz bands.
   */
  rrccsp_update_band_class_in_acq_db();
#endif
  }

  return status;
} /* rrccsp_is_band_class_enabled_in_ui_rf */

#ifdef FEATURE_DISCONNECT_ON_UNSUPPORTED_IRAT_REDIRECTION

/*===========================================================================

FUNCTION          RRCCSP_CHECK_GSM_BANDS_SUPPORTED

DESCRIPTION       This function check if UE can support GSM bands or not.

DEPENDENCIES
                  
RETURN VALUE
                  TRUE: If UE can support GSM.
                  FALSE: Otherwise. 

SIDE EFFECTS
                  None.
                  
===========================================================================*/

boolean rrccsp_check_gsm_bands_supported(void)
{
  sd_ss_band_pref_e_type band_capability;
  
  #ifdef FEATURE_DUAL_SIM
  band_capability = sd_misc_get_subs_supp_band_pref(rrc_get_as_id());
  #else
  band_capability = sd_misc_get_supp_band_pref();
  #endif
  WRRC_MSG1_HIGH("band capability %x", band_capability);
  if (((band_capability) & 
           ( SD_SS_BAND_PREF_GSM_450 |
             SD_SS_BAND_PREF_GSM_480 |
             SD_SS_BAND_PREF_GSM_750 |
             SD_SS_BAND_PREF_GSM_850 |
             SD_SS_BAND_PREF_GSM_EGSM_900 |
             SD_SS_BAND_PREF_GSM_PGSM_900 |
             SD_SS_BAND_PREF_GSM_RGSM_900 |
             SD_SS_BAND_PREF_GSM_DCS_1800 |
             SD_SS_BAND_PREF_GSM_PCS_1900)
       ) != 0)
  {
    return TRUE;
  }
  else 
  {
    return FALSE;
  }
}
#endif

/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_ACQ_DB

DESCRIPTION       This function adds an acquisition entry based on the network
                  direction (e.g. RRC connection release message) to the 
                  acquisition list. While doing so, it also updates the 
                  acquisition database pointers so that the 1st pointer points
                  to this most recent acquisition entry.
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                  Success/Failure. Failure will be an extra-ordinary event and
                  could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS
                  None.

===========================================================================*/
uecomdef_status_e_type  rrccsp_update_acq_db(rrc_freq_type freq)
{
  /* copy curr_acq_entry into a temp entry so that the plmn_id, etc. are the same */
  rrc_csp_acq_entry_type temp_acq_entry = rrc_csp_int_data.curr_acq_entry;

  temp_acq_entry.is_valid_nw_sharing = FALSE;

  /* just update the frequency information. rest of the information remains the same */
  temp_acq_entry.freq = freq;

  if(rrc_csp_add_acq_entry(&temp_acq_entry) == SUCCESS)
  {
    WRRC_MSG1_HIGH("Freq %d added to ACQ database",freq);
    return SUCCESS;
  }
  else
  {
    return FAILURE;
  }
} /* rrccsp_update_acq_db */


/*===========================================================================

FUNCTION RRC_UPDATE_BAND_PREFERENCE

DESCRIPTION

  This function is called by RR to update the band preference
  in RRC.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
===========================================================================*/
void rrc_update_band_preference
(
  sys_band_mask_type band_pref_mask  /* band mask */
)
{
  rrc_csp_int_data.band_pref = band_pref_mask;
  rrccsp_update_band_preference();

} /* rrc_update_band_preference */

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION RRC_UPDATE_BAND_PREFERENCE_SUB_ID

DESCRIPTION

  This function is called by RR to update the band preference
  in RRC when DSDS is enabled
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
===========================================================================*/
void rrc_update_band_preference_sub_id
(
  sys_band_mask_type band_pref_mask,  /* band mask */
  sys_modem_as_id_e_type  as_id
)
{
  if (as_id != rrc_get_as_id())
  {
    WRRC_MSG2_ERROR("AS-id mismatch %d, internal %d",as_id, rrc_get_as_id());
  }
  rrc_update_band_preference(band_pref_mask);
} /* rrc_update_band_preference */
#endif

#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
/*===========================================================================

FUNCTION RRC_IS_ARFCN_VALID

DESCRIPTION

  This function is called by RR to validate the WCDMA ARFCN it
  reads from the SIs. RRC validates the range and band of the ARFCN.
  RR also passes the PLMN_ID of the PLMN the UE is camped on. This 
  information is used only when the feature FEATURE_MCC_AND_850_800_BAND_COMBINATION
  is defined
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If ARFCN is valid,
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_is_arfcn_valid
(
  uint32 UARFCN,
  PLMN_id_T rr_plmn_id
) 
{
  if(validate_dl_arfcn(UARFCN, 
                       mmumts_convert_nas_plmn_id_to_rrc_plmn_id(rr_plmn_id), 
                       W_UARFCN_FROM_GSM_NBR_LIST
                       , TRUE
                      )
     == SUCCESS)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* rrc_is_arfcn_valid */

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION RRC_IS_ARFCN_VALID_SUB_ID

DESCRIPTION

  This function is called by RR to validate the WCDMA ARFCN it
  reads from the SIs when DSDS is enabled. RRC validates the range and band of the ARFCN.
  RR also passes the PLMN_ID of the PLMN the UE is camped on. This 
  information is used only when the feature FEATURE_MCC_AND_850_800_BAND_COMBINATION
  is defined
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If ARFCN is valid,
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/

boolean rrc_is_arfcn_valid_sub_id
(
  uint32 UARFCN,
  PLMN_id_T rr_plmn_id,
  sys_modem_as_id_e_type as_id
) 
{
  if (as_id != rrc_get_as_id())
  {
    WRRC_MSG2_ERROR("AS-id mismatch %d, internal %d",as_id, rrc_get_as_id());
  }
  return (rrc_is_arfcn_valid(UARFCN, rr_plmn_id));
}
#endif
#else
/*===========================================================================

FUNCTION RRC_IS_ARFCN_VALID

DESCRIPTION

  This function is called by RR to validate the WCDMA ARFCN it
  reads from the SIs. RRC validates the range and band of the ARFCN.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If ARFCN is valid,
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_is_arfcn_valid
(
  uint32 UARFCN
) 
{
  if(validate_dl_arfcn(UARFCN
                       , TRUE
                      )
     == SUCCESS)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* rrc_is_arfcn_valid */
#endif


/*===========================================================================

FUNCTION RRC_SET_UE_IN_DEEP_SLEEP

DESCRIPTION

  This function is called by RR to inform RRC that UE is going to DEEP SLEEP
  state.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_set_ue_in_deep_sleep( boolean value )
{
  WRRC_MSG1_HIGH("Deep sleep indicated by RR %d",value);
  rrc_csp_int_data.ue_in_deep_sleep = value;

} /* rrc_set_ue_in_deep_sleep */

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION RRC_SET_UE_IN_DEEP_SLEEP_SUB_ID

DESCRIPTION

  This function is called by RR to inform RRC that UE is going to DEEP SLEEP
  state when DSDS is enabled
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_set_ue_in_deep_sleep_sub_id( boolean value,sys_modem_as_id_e_type as_id)
{

  if (as_id != rrc_get_as_id())
  {
    WRRC_MSG2_ERROR("AS-id mismatch %d, internal %d",as_id, rrc_get_as_id());
  }
  rrc_set_ue_in_deep_sleep(value);
}
#endif

/*===========================================================================

FUNCTION RRC_WRITE_NV_ACQ_INFO

DESCRIPTION

  This function is called by RR when it gets RR_DEACT_REQ from MM,
  so that RRC can write the acquisition database information to NV.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_write_nv_acq_info( void )
{
  WRRC_MSG0_HIGH("Powerdown indicated by RR");
  rrc_csp_write_nv_acq_info();

} /* rrc_write_nv_acq_info */


/*===========================================================================

FUNCTION RRCCSP_IS_IN_BPLMN_LIST

DESCRIPTION

  This function is called by SIB procedure once it reads the MIB to check if it
  should proceed further and read SIB1 and SIB3 for that cell. If any of the requested 
  PLMNs are in the bcch shared plmn list then SIB should proceed with reading SIBS.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If PLMN is in the BPLMN list
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrccsp_is_in_bplmn_list (rrc_plmn_identity_type plmn_id
                                 , rrc_MasterInformationBlock *mib_ptr
                                 ) 
{
  uint32                    i;
  rrc_csp_acq_entry_type    *bplmn_acq_entry = rrc_malloc(sizeof(rrc_csp_acq_entry_type));
  boolean                   plmns_shared = FALSE;

  rrc_csp_nw_sharing_entry_type bplmn_nw_db_entry;
  bplmn_nw_db_entry.num_of_shared_plmns = 0;

  MSG_HIGH("BPLMN: SIB querying if MCC %d, MNC %d on freq %d is in BPLMN list",
                            RRC_CSP_GET_INT_MCC(plmn_id), RRC_CSP_GET_INT_MNC(plmn_id), 
                            rrc_csp_int_data.curr_acq_entry.freq);

  /* First add this frequency to Available PLMN List to eliminate scanning of 
   * other frequencies during full frequency scan 
   */
  rrc_csp_add_freq_to_available_freq_list(rrc_csp_int_data.curr_acq_entry.freq);

  if(   (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
     || (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
    )
  {

    /* For background service search, the PLMNs should be reported to NAS even
     * if they do not match with those sent by NAS in the PLMN_LIST_REQ.
     * Hence the check here for the enum service_search_bgnd.
     */
    if(    (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC)
        || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL)
        || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_PRIORITY_PLMN)
            /* The new search type is for a customer requirement where if the UE is camped on HPLMN,
             * NAS has to periodically trigger Automatic search(SYS_SERVICE_SEARCH_PRIORITY_PLMN) for 
             * searching another customer's PLMN*/
#ifdef FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN
        || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_NONE)
#endif
      )
    {
      WRRC_MSG0_HIGH("SSB: Read SIBs for ALL PLMNs");
      rrc_free(bplmn_acq_entry);
      return TRUE;
    }

    if(rrc_is_nw_shared(mib_ptr))
    {
      plmns_shared = TRUE;
      bplmn_acq_entry->is_valid_nw_sharing = TRUE;
      rrccsp_find_shared_plmns(
                               mib_ptr,
                               plmn_id,
                               &bplmn_nw_db_entry
                               );
    }
    else
    {
      bplmn_acq_entry->is_valid_nw_sharing = FALSE;
    }


    /* optimization: for Manual PLMN Search, Read SIBs only if the MIB-PLMN matches with
     * requested one.
     */              
    if(plmns_shared == FALSE)
    {
      if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
      {
        for(i = 0; i < rrc_csp_int_data.bplmn_num_req; ++i)
        {
          if(rrc_bcch_and_nas_plmns_match(plmn_id, rrc_csp_int_data.bplmn_req_list[i].plmn_id))
          {
            WRRC_MSG0_HIGH("BPLMN:SIB proc -> plmn is in BPLMN list");
            rrc_free(bplmn_acq_entry);
            return TRUE;
          }
        }   
      }
      else if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
      {
        for(i = 0; i < rrc_csp_int_data.wtow_bplmn_num_req; ++i)
        {
          if(rrc_bcch_and_nas_plmns_match(plmn_id, rrc_csp_int_data.wtow_bplmn_req_list[i].plmn_id))
          {
            WRRC_MSG0_HIGH("BPLMN:SIB proc -> plmn is in BPLMN list");
            rrc_free(bplmn_acq_entry);
            return TRUE;
          }
        }   
      }
    }
    else 
    {
      if(rrccsp_is_any_of_bcch_plmns_in_bplmn_list(bplmn_nw_db_entry))
      {
        rrc_free(bplmn_acq_entry);
        return TRUE;
      }
    }
  }
  else
  {
#ifdef FEATURE_FEMTO_CSG
    if(rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_CSG)
    {
#endif
    WRRC_MSG0_ERROR("BPLMN ERR:SIB proc shd NOT have called this function");
    rrc_free(bplmn_acq_entry);
    return FALSE;
#ifdef FEATURE_FEMTO_CSG
    }
    else
    {
      WRRC_MSG0_HIGH("CSG: SIB thinks that auto BPLMN in progress for CSG searches. return true");
      rrc_free(bplmn_acq_entry);
      return TRUE;
    }
#endif
  }

  WRRC_MSG0_HIGH("BPLMN/WTOW:SIB proc -> plmn is NOT in BPLMN list");
  WRRC_MSG0_HIGH("BPLMN/WTOW:Add the freq/plmn to ACQ DB and avail_plmn_list");

  /* Add the PLMN to ACQ DB */
  if(bplmn_acq_entry->is_valid_nw_sharing)
  {
    bplmn_acq_entry->plmn_id = bplmn_nw_db_entry.shared_plmns_access_info[0].plmn_id;
  }
  else 
  {
    bplmn_acq_entry->plmn_id      = plmn_id;
  }


  bplmn_acq_entry->status_mask  = 0;
  bplmn_acq_entry->freq         = rrc_csp_int_data.curr_acq_entry.freq;
  bplmn_acq_entry->scr_code     = rrc_csp_int_data.curr_acq_entry.scr_code;

#ifdef FEATURE_WCDMA_MULTIBAND
  bplmn_acq_entry->band_class  = rrc_get_frequency_band(
                                                  rrc_csp_int_data.curr_acq_entry.freq,
                                                  plmn_id);
  WRRC_MSG2_HIGH("band_class %d for freq %d", bplmn_acq_entry->band_class,
                                        bplmn_acq_entry->freq);

#else
   bplmn_acq_entry.band_class = RRC_CSP_PCS_BAND;
   WRRC_MSG2_HIGH("band_class %d for freq %d", bplmn_acq_entry->band_class,
                                         bplmn_acq_entry->freq);
#endif

#ifdef FEATURE_FEMTO_CSG
/* Not yet read SIB3*/
  bplmn_acq_entry->csg_id = SYS_CSG_ID_INVALID;
#endif
  rrccsp_add_acq_db_and_nw_db_entries(bplmn_acq_entry,&bplmn_nw_db_entry);
  WRRC_MSG1_HIGH("BPLMN: Freq %d added to ACQ database",rrc_csp_int_data.curr_acq_entry.freq);
  rrc_free(bplmn_acq_entry);
  return FALSE;
} /* rrccsp_is_in_bplmn_list */


/*===========================================================================

FUNCTION     RRCCSP_IS_SIB_MCC_AND_850_800_CURRENT_BAND_COMB_VALID

DESCRIPTION

  This function is called by SIB procedure during cell selection once it reads the 
  MIB, to check if it should proceed further and read remaining SIBs for that cell.
  E.g., if the MCC in the MIB is Japan and the frequency is in the 800 band then the current
  band should be 800 otherwise the combination is invalid and UE should try the next
  cell/frequency. Similarly, if the MCC in the MIB is NOT Japan and the frequency is in the 
  800 band then the current band should be 850 otherwise the combination is invalid 
  and UE should try the next cell/frequency; etc.
  
DEPENDENCIES

  This function should only be called when the 850 or 800 band is supported, and the 
  UARFCN is in the overlapping region of 850 and 800 bands. 

RETURN VALUE

  TRUE - MCC, Frequency and Current Band combination is valid 
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrccsp_is_sib_mcc_and_850_800_current_band_comb_valid(rrc_plmn_identity_type plmn_id) 
{
  MSG_HIGH("MCC_850_800: plmn_mcc=%d, current_band=%d, current_freq=%d",
                                     RRC_CSP_GET_INT_MCC(plmn_id),
                                     rrc_csp_curr_select_data.current_band, 
                                     rrc_csp_int_data.curr_acq_entry.freq);

  /* If either 850 or 800 band or both are in the current_band then if MCC in the MIB is
   * Japan but 800 band is not in the current_band then the combination is invalid. Similarly, if
   * the MCC in the MIB is NOT Japan but 850 is not in the current_band then also the combination
   * is invalid. 
   */
  if((((RRC_CSP_GET_INT_MCC(plmn_id) == JAPAN_MCC1)
       || (RRC_CSP_GET_INT_MCC(plmn_id) == JAPAN_MCC2)
       || (RRC_CSP_GET_INT_MCC(plmn_id) == JAPAN_MCC3)
       || (RRC_CSP_GET_INT_MCC(plmn_id) == JAPAN_MCC4))
       && (!rrccsp_is_current_band(RRC_CSP_WCDMA_800_BAND,plmn_id)))
     ||(((RRC_CSP_GET_INT_MCC(plmn_id) != JAPAN_MCC1)
       && (RRC_CSP_GET_INT_MCC(plmn_id) != JAPAN_MCC2)
       && (RRC_CSP_GET_INT_MCC(plmn_id) != JAPAN_MCC3)
       && (RRC_CSP_GET_INT_MCC(plmn_id) != JAPAN_MCC4))
       && (!rrccsp_is_current_band(RRC_CSP_WCDMA_850_BAND,plmn_id))))
  {
    WRRC_MSG1_HIGH("MCC_850_800: MCC %d and DL UARFCN Combination is INVALID", 
                                            RRC_CSP_GET_INT_MCC(plmn_id));
    return FALSE;
  }
  /* Combination is valid */
  else
  {
    WRRC_MSG0_HIGH("MCC_850_800: MCC and DL UARFCN Combination is Valid");
    return TRUE;
  }
} /* rrccsp_is_sib_mcc_and_850_800_current_band_comb_valid */


/*===========================================================================

FUNCTION          rrc_csp_init_inter_freq_redirection_data

DESCRIPTION       This function initializes appropriate data before attempting
                  acquisition on redirected frequency.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void rrc_csp_init_inter_freq_redirection_data(void)
{       
  /* Save the last camped frequency. This frequency will be specifically
   * scanned during the full frequency scan
   */
  rrc_csp_int_data.last_camped_freq_in_connected_mode = rrc_ccm_get_curr_camped_freq();

  /* Save camped frequency, in case redirection to new frequency fails */
  rrc_csp_int_data.last_camped_freq              = rrc_csp_int_data.curr_acq_entry.freq;
  rrc_csp_int_data.start_acq_on_last_camped_freq = FALSE;
  rrc_csp_int_data.first_acq_on_redirected_freq  = TRUE;

  rrc_csp_int_data.curr_scan = RRC_CSP_INTER_FREQ_REDIRECTION_SCAN;

#ifdef FEATURE_INTER_FREQ_EM_CALL_REDIRECTION
  /* Initialize the variable to FALSE. This variable is set to TRUE only
   * when type-of-plmn-selection variables are updated for an em call */
  rrc_csp_int_data.em_call_acq_type_updated = FALSE;

  /* Search for ANY_PLMN if an emergency call is redirected */
  rrccsp_update_acq_type_for_em_call_ifreq_redirection(RRC_CSP_ANY_PLMN);
#endif

  return;
} /* rrc_csp_init_inter_freq_redirection_data */


/*===========================================================================

FUNCTION          rrccsp_is_ue_in_true_limited_service

DESCRIPTION       This function checks the network select mode and returns TRUE/FALSE
                  to the calling procedure.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
boolean rrccsp_is_ue_in_true_limited_service(void)
{
  if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}



/*===========================================================================

FUNCTION          rrc_csp_cleanup_inter_freq_redirection_state

DESCRIPTION       This function cleans up appropriate data on completing or
                  aborting inter frequency redirection scan.
                                               
DEPENDENCIES
                  None
RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void rrc_csp_cleanup_inter_freq_redirection_state(void)
{   
  /* Stop the wait timer if it was active */
  if(!rrc_csp_int_data.start_acq_on_last_camped_freq)
  {
    rrctmr_stop_timer(RRCTMR_FREQ_REDIRECT_WAIT_TIMER);
    rrc_csp_int_data.start_acq_on_last_camped_freq = TRUE;
  }

#ifdef FEATURE_INTER_FREQ_EM_CALL_REDIRECTION
  /* Reset the variables to their original state if they were
   * updated because of an emergency call
   */
  rrccsp_update_acq_type_for_em_call_ifreq_redirection(RRC_CSP_SPEC_PLMN);
#endif

  return;
} /* rrc_csp_cleanup_inter_freq_redirection_state */


/*===========================================================================

FUNCTION: RRC_RR_CHECK_CELL_BARRED

DESCRIPTION:
  This function is called by RR to check if a particular WCDMA cell is 
  barred or not

DEPENDENCIES:
  NONE

RETURN VALUE:
  WCDMA cell is not barred, or only the PSC is barred, or the frequency
  is barred

SIDE EFFECTS:
  NONE
====================================================================*/
rrc_rr_cell_bar_info_e_type rrc_rr_check_cell_barred (uint16 freq, uint16 scr_code)
{
  rrc_rr_cell_bar_info_e_type cell_bar_info = WCDMA_NOT_BARRED;

  switch(wl1_check_cell_barred(freq, scr_code))
  {
    case L1_WCDMA_NOT_BARRED:
      WRRC_MSG2_HIGH("Informing RR: freq %d, scr_code %d NOT barred", freq, scr_code);
      cell_bar_info = WCDMA_NOT_BARRED;
      break;

    case L1_WCDMA_PSC_BARRED:
      WRRC_MSG2_HIGH("Informing RR: freq %d, scr_code %d PSC barred", freq, scr_code);
      cell_bar_info = WCDMA_PSC_BARRED;
      break;

    case L1_WCDMA_FREQ_BARRED:
      WRRC_MSG2_HIGH("Informing RR: freq %d, scr_code %d FREQ barred", freq, scr_code);
      cell_bar_info = WCDMA_FREQ_BARRED;
      break;

    default:
      WRRC_MSG2_ERROR("ERR l1_cell_bar_info incorrect. freq %d, scr_code %d NOT barred", freq, scr_code);
      cell_bar_info = WCDMA_NOT_BARRED;
      break;
  }
  return(cell_bar_info);
} /* rrc_rr_check_cell_barred */

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION: RRC_RR_CHECK_CELL_BARRED_SUB_ID

DESCRIPTION:
  This function is called by RR to check if a particular WCDMA cell is 
  barred or not when DSDS is enabled

DEPENDENCIES:
  NONE

RETURN VALUE:
  WCDMA cell is not barred, or only the PSC is barred, or the frequency
  is barred

SIDE EFFECTS:
  NONE
====================================================================*/
rrc_rr_cell_bar_info_e_type rrc_rr_check_cell_barred_sub_id
(
  uint16 freq, uint16 scr_code, sys_modem_as_id_e_type as_id
)
{
  if (as_id != rrc_get_as_id())
  {
    WRRC_MSG2_ERROR("AS-id mismatch %d, internal %d",as_id, rrc_get_as_id());
  }
  return (rrc_rr_check_cell_barred( freq, scr_code));
}
#endif

/*===========================================================================

FUNCTION          rrc_csp_send_cell_param_req_for_pch_fach

DESCRIPTION       This function sends the cell param request to Layer1 if on  
                  transition from FACH to PCH or PCH to FACH, SIb4 is present
                  in the db and has reselection extensions t_reselection or
                  q_hyst.
                                               
DEPENDENCIES
                  None
RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void rrc_csp_send_cell_param_req_for_pch_fach(void)
{   
  rrc_SysInfoType3    *sib3_ptr = NULL;
  rrc_SysInfoType4    *sib4_ptr = NULL;
  rrc_plmn_identity_type common_plmn_id;
  if(rrc_csp_int_data.curr_acq_entry.is_valid_nw_sharing)
  {
    common_plmn_id = rrc_csp_int_data.curr_nw_db_entry.common_plmn_id;
  }
  else
  {
    common_plmn_id = rrc_csp_int_data.curr_acq_entry.plmn_id;
  }
  WRRC_MSG2_HIGH("Sib_idx PLMN: MCC %d-MNC %d",RRC_CSP_GET_INT_MCC(common_plmn_id),RRC_CSP_GET_INT_MNC(common_plmn_id));
  /* PLMN and Cell ID of the current camped on cell are known. Get SIB3 */
  sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_cell(
                                        rrc_SIB3, 
                                        common_plmn_id, 
                                        rrc_csp_int_data.curr_acq_entry.cell_id);
  if( sib3_ptr == NULL )
  {
    /* SIB3 for this PLMN, Cell ID combination not present. 
     */
    MSG_HIGH("SIB3 for MCC %d, MNC %d, Cell ID %d not present", 
              RRC_CSP_GET_INT_MCC(rrc_csp_int_data.curr_acq_entry.plmn_id),
               RRC_CSP_GET_INT_MNC(rrc_csp_int_data.curr_acq_entry.plmn_id), 
               rrc_csp_int_data.curr_acq_entry.cell_id);
    return;
  }

  if(sib3_ptr->sib4indicator == TRUE)
  {
     sib4_ptr = (rrc_SysInfoType4*) rrc_sibdb_return_sib_for_srv_cell(
                                    rrc_SIB4); 
  }
  if((sib4_ptr != NULL) && (sib4_ptr->m.v4b0NonCriticalExtensionsPresent) &&
    (sib4_ptr->v4b0NonCriticalExtensions.m.
    v590NonCriticalExtensionPresent) &&
    (sib4_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.
    v5b0NonCriticalExtensionPresent) && (sib4_ptr->v4b0NonCriticalExtensions.
    v590NonCriticalExtension.v5b0NonCriticalExtension.sysInfoType4_v5b0ext.m.
    cellSelectReselectInfoPCHFACH_v5b0extPresent))
  {
    /* Send CPHY_CELL_PARM_REQ to L1 */
    rrc_csp_send_l1_cmd(RRC_PROCEDURE_CSP, CPHY_CELL_PARM_REQ,
                        &rrc_csp_int_data.curr_acq_entry);
    WRRC_MSG0_HIGH("CELL PARAM REQ for sib4 PCH/FACH info sent");
  }
  return;
} /*rrc_csp_send_cell_param_req_for_pch_fach */



/*====================================================================
FUNCTION: RRCCSP_SEND_CONN_MODE_OOS_DS_REQ

DESCRIPTION:
  This function constructs the RRC_CONN_MODE_DEEP_SLEEP_REQ primitive and sends it
  to LLC. This function is called when UE needs to be put in deep sleep in
  RRC Connected Mode.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
====================================================================*/
void rrccsp_send_conn_mode_oos_ds_req (void)
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CONN_MODE_DEEP_SLEEP_REQ;

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG0_HIGH("OOS: RRC_CONN_MODE_DEEP_SLEEP_REQ sent to LLC");

} /* end rrccsp_send_conn_mode_oos_ds_req() */


/*====================================================================
FUNCTION: RRCCSP_SEND_CONN_MODE_OOS_DS_WAKEUP_REQ

DESCRIPTION:
  This function constructs the RRCCSP_SEND_CONN_MODE_OOS_DS_WAKEUP_REQ primitive and sends it
  to LLC. This function is called when UE needs to be woken up from deep sleep in
  RRC Connected Mode.

DEPENDENCIES:
  None

RETURN VALUE:
  uecomdef_status_e_type.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrccsp_send_conn_mode_oos_ds_wakeup_req (void)
{
  rrc_cmd_type *cmd_ptr;

#ifdef FEATURE_DUAL_SIM
  wrm_freq_input_type lock_req_bands;
  lock_req_bands.num_bands = 1;
  lock_req_bands.bands[0].band = rrc_return_default_band_lock_req();  
#endif

#ifndef FEATURE_TRIPLE_SIM
#ifdef FEATURE_DUAL_SIM
  if(rrc_block_wait_to_acquire_lock_for_time(300, TRM_ACQUISITION, lock_req_bands) == FAILURE)
  {
    return FAILURE;
  }
#endif
#else
  if(rrc_block_wait_to_acquire_lock_for_time(200, TRM_ACQUISITION, lock_req_bands, FALSE ) == FAILURE)
  {
    return FAILURE;
  }
#endif

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CONN_MODE_DEEP_SLEEP_WAKEUP_REQ;

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG0_HIGH("OOS: RRC_CONN_MODE_DEEP_SLEEP_WAKEUP_REQ sent to LLC");

  return SUCCESS;
} /* end rrccsp_send_conn_mode_oos_ds_wakeup_req() */


/*===========================================================================

FUNCTION          RRC_CONVERT_BAND_TO_SYS_BAND

DESCRIPTION       This function takes the band used internally by RRC and 
                  converts it into sys band which is used by CM/SD.
                  
DEPENDENCIES      None

RETURN VALUE      sys band type used by CM/SD. 

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_e_type rrc_convert_band_to_sys_band(rrc_csp_band_class_type band)
{
  sys_band_mask_e_type ret_sys_band = SYS_BAND_MASK_WCDMA_I_IMT_2000;
  
  if(band == RRC_CSP_PCS_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_II_PCS_1900;
    
  else if(band == RRC_CSP_IMT2K_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_I_IMT_2000;
    
  else if(band == RRC_CSP_WCDMA_800_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_VI_800;
    
  else if(band == RRC_CSP_WCDMA_850_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_V_850;
    
  else if(band == RRC_CSP_WCDMA_1800_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_III_1700;
    
  else if(band == RRC_CSP_WCDMA_900_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_VIII_900;
    
  else if(band == RRC_CSP_WCDMA_1700_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_IX_1700;
    
  else if(band == RRC_CSP_WCDMA_1700_2100_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_IV_1700;
    
  else if(band == RRC_CSP_WCDMA_1500_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_XI_1500;

  else if(band == RRC_CSP_WCDMA_BC19_BAND)
    ret_sys_band = SYS_BAND_MASK_WCDMA_XIX_850;
  else
  {
    WRRC_MSG1_ERROR("Incorrect band %d passed", band);
  }
  
  WRRC_MSG2_HIGH("band %d, sys_band %d", band, ret_sys_band);
  return(ret_sys_band);
} /* rrc_convert_band_to_sys_band */

/*===========================================================================

FUNCTION          RRC_CONVERT_BAND_TO_SYS_BAND_CLASS

DESCRIPTION       This function takes the band used internally by RRC and 
                  converts it into sys band class is used by TRM
                  
DEPENDENCIES      None

RETURN VALUE      sys band class used by TRM. 

SIDE EFFECTS      None
===========================================================================*/
sys_band_class_e_type rrc_convert_band_to_sys_band_class(rrc_csp_band_class_type band)
{
  sys_band_class_e_type ret_sys_band = SYS_BAND_WCDMA_I_IMT_2000;
  
  if(band == RRC_CSP_PCS_BAND)
    ret_sys_band = SYS_BAND_WCDMA_II_PCS_1900;
    
  else if(band == RRC_CSP_IMT2K_BAND)
    ret_sys_band = SYS_BAND_WCDMA_I_IMT_2000;
    
  else if(band == RRC_CSP_WCDMA_800_BAND)
    ret_sys_band = SYS_BAND_WCDMA_VI_800;
    
  else if(band == RRC_CSP_WCDMA_850_BAND)
    ret_sys_band = SYS_BAND_WCDMA_V_850;
    
  else if(band == RRC_CSP_WCDMA_1800_BAND)
    ret_sys_band = SYS_BAND_WCDMA_III_1700;
    
  else if(band == RRC_CSP_WCDMA_900_BAND)
    ret_sys_band = SYS_BAND_WCDMA_VIII_900;
    
  else if(band == RRC_CSP_WCDMA_1700_BAND)
    ret_sys_band = SYS_BAND_WCDMA_IX_1700;
    
  else if(band == RRC_CSP_WCDMA_1700_2100_BAND)
    ret_sys_band = SYS_BAND_WCDMA_IV_1700;
    
  else if(band == RRC_CSP_WCDMA_1500_BAND)
    ret_sys_band = SYS_BAND_WCDMA_XI_1500;

  else if(band == RRC_CSP_WCDMA_BC19_BAND)
    ret_sys_band = SYS_BAND_WCDMA_XIX_850;
  else
  {
    WRRC_MSG1_ERROR("Incorrect band %d passed", band);
  }
  
  WRRC_MSG2_HIGH("band %d, sys_band %d", band, ret_sys_band);
  return(ret_sys_band);
} /* rrc_convert_band_to_sys_band */

#ifdef FEATURE_WCDMA_DB_DC_HSDPA
/*===========================================================================

FUNCTION          rrc_convert_sys_band_class_to_rrc_band

DESCRIPTION       This function takes the band used internally by RRC and 
                  converts it into sys band class is used by TRM
                  
DEPENDENCIES      None

RETURN VALUE      sys band class used by TRM. 

SIDE EFFECTS      None
===========================================================================*/
rrc_csp_band_class_type rrc_convert_sys_band_class_to_rrc_band(sys_band_class_e_type band_class)
{
  rrc_csp_band_class_type ret_rrc_band = RRC_CSP_BAND_MAX;

  if(band_class == SYS_BAND_WCDMA_II_PCS_1900)
    ret_rrc_band = RRC_CSP_PCS_BAND;
  
  else if(band_class == SYS_BAND_WCDMA_I_IMT_2000)
    ret_rrc_band = RRC_CSP_IMT2K_BAND;

  else if(band_class == SYS_BAND_WCDMA_VI_800)
    ret_rrc_band = RRC_CSP_WCDMA_800_BAND;

  else if(band_class == SYS_BAND_WCDMA_V_850)
    ret_rrc_band = RRC_CSP_WCDMA_850_BAND;

  else if(band_class == SYS_BAND_WCDMA_III_1700)
    ret_rrc_band = RRC_CSP_WCDMA_1800_BAND;

  else if(band_class == SYS_BAND_WCDMA_VIII_900)
    ret_rrc_band = RRC_CSP_WCDMA_900_BAND;

  else if(band_class == SYS_BAND_WCDMA_IX_1700)
    ret_rrc_band = RRC_CSP_WCDMA_1700_BAND;

  else if(band_class == SYS_BAND_WCDMA_IV_1700)
    ret_rrc_band = RRC_CSP_WCDMA_1700_2100_BAND;

  else if(band_class == SYS_BAND_WCDMA_XI_1500)
    ret_rrc_band = RRC_CSP_WCDMA_1500_BAND;

  else if(band_class == SYS_BAND_WCDMA_XIX_850)
    ret_rrc_band = RRC_CSP_WCDMA_BC19_BAND;
  else
  {
    WRRC_MSG1_ERROR("Incorrect band class %d passed", band_class);
  }
  
  return(ret_rrc_band);
} /* rrc_convert_band_to_sys_band */
#endif
/*====================================================================
FUNCTION: RRCCSP_SKIP_SIBS_FOR_FPLMN

DESCRIPTION:
         This function checks whether SIB procedure needs to read SIBs or not after reading MIB.
         This function will be used only by SIB module to avoid extra time consumed during SIB
         reading if (all shared) PLMN(s) (are) is forbidden.
         CSP will check for forbidden status of the PLMN only when n/w sel mode is 
         automatic and acq_type is SPECIFIC and Cell SELECTION is in progress.

DEPENDENCIES:
         None

RETURN VALUE:
         TRUE: skip reading SIBS if the passed PLMN is FPLMN
         FALSE: otherwise

SIDE EFFECTS:
         None.
====================================================================*/
boolean rrccsp_skip_sibs_for_fplmn(
  rrc_plmn_identity_type plmn_id
  , rrc_MasterInformationBlock *mib_ptr
  )
{
  boolean is_valid_nw_sharing = FALSE;

  boolean is_plmn_forbidden = FALSE;

#ifdef FEATURE_EOOS
  #error code not present
#endif

  rrc_csp_nw_sharing_entry_type  *fplmn_nw_db_entry = rrc_malloc(sizeof(rrc_csp_nw_sharing_entry_type));
  uint8 shared_plmn_idx = 0;
  fplmn_nw_db_entry->num_of_shared_plmns = 0;

  MSG_HIGH("FPLMN: curr_type_acq=%d, curr_scan=%d, n/w_sel_mode=%d",
                                                rrc_csp_int_data.curr_type_acq,
                                                rrc_csp_int_data.curr_scan,
                                                rrc_csp_int_data.network_select_mode);

  /* First add this frequency to Available PLMN List to eliminate scanning of 
   * other frequencies during full frequency scan 
   */
  rrc_csp_add_freq_to_available_freq_list(rrc_csp_int_data.curr_acq_entry.freq);

  if(((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
       (rrc_csp_int_data.curr_type_acq == RRC_CSP_SPEC_PLMN) &&
       (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN))
     ||(((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
     ||(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN))
     &&(rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_BACKGROUND_MANUAL))
     /* Skip reading SIBs for FPLMN during connected mode OOS search for SPEC PLMN search. */
     ||((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) &&
          (rrc_csp_int_data.curr_type_acq == RRC_CSP_SPEC_PLMN))
     )
  {

    if(rrc_is_nw_shared(mib_ptr))
    {
      is_valid_nw_sharing = TRUE;
      rrccsp_find_shared_plmns(
                               mib_ptr,
                               plmn_id,
                               fplmn_nw_db_entry
                               );
    }
    else
    {
      is_valid_nw_sharing = FALSE;
    }

    do
    {
      if((is_valid_nw_sharing) && (shared_plmn_idx < fplmn_nw_db_entry->num_of_shared_plmns) &&
         (shared_plmn_idx < MAX_SHARED_PLMNS))
      {
        plmn_id = fplmn_nw_db_entry->shared_plmns_access_info[shared_plmn_idx++].plmn_id;
      }

#ifdef FEATURE_DUAL_SIM
      is_plmn_forbidden = reg_sim_per_subs_plmn_forbidden(mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn_id),
                                      rrc_get_as_id());
#else
      is_plmn_forbidden = reg_sim_plmn_forbidden(mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn_id));
#endif

      if(is_plmn_forbidden == FALSE)
      {
        WRRC_MSG0_HIGH("FPLMN: BCCH PLMN is not forbidden ");
        rrc_free(fplmn_nw_db_entry);
        return FALSE;
      }
    }while((is_valid_nw_sharing) && (shared_plmn_idx < fplmn_nw_db_entry->num_of_shared_plmns));

    /* This check has been added for cr109422. During foreground
    * service search if a cell is found on which UE can camp on
    * then set the var plmn_fnd_during_fgnd_svc_srch to TRUE.
    * This var has been added to inform NAS (using var ue_oos 
    * in RRC_SERVICE_CNF) that service is available. NAS looks
    * at this var to determine if limited service request should
    * be sent to RRC or not. Notice that with UOOS, SD always sends
    * Automatic Service Request and it sends Limited Service Request
    * only when NAS informs SD that Limited Service is available
    */
    if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
    {
      rrc_csp_int_data.plmn_fnd_during_fgnd_svc_srch = TRUE;
#ifdef FEATURE_EOOS
      #error code not present
#endif
    }

    /* Add the PLMN to ACQ DB */
    /* Avaialable PLMNs are forbidden. Add into ACQ DB */
    rrccsp_add_plmn_acq_db_and_nw_db_entries(plmn_id, fplmn_nw_db_entry, is_valid_nw_sharing);
    WRRC_MSG0_HIGH("FPLMN: BCCH PLMN is forbidden PLMN");
    rrc_free(fplmn_nw_db_entry);
    return TRUE;
  }
  rrc_free(fplmn_nw_db_entry);
  return FALSE;
}

/*====================================================================
FUNCTION: RRCCSP_SKIP_SIBS_FOR_VPLMN

DESCRIPTION:
         This function checks if the PLMN is a visitor PLMN and if it is already 
         present in the list to be sent to NAS.
DEPENDENCIES:
         None

RETURN VALUE:
         TRUE: skip reading SIBS if the passed PLMN is VPLMN and if the PLMN is 
               already added in the list to be sent to NAS.
         FALSE: otherwise

SIDE EFFECTS:
         None.
====================================================================*/
boolean rrccsp_skip_sibs_for_vplmn(
  rrc_plmn_identity_type plmn_id
  , rrc_MasterInformationBlock *mib_ptr
  )
{
  boolean is_valid_nw_sharing = FALSE;
  uint32 i;
  rrc_plmn_identity_type nas_to_rrc_plmn_id;
  boolean ret_val = FALSE;
  rrc_csp_nw_sharing_entry_type  *vplmn_nw_db_entry = rrc_malloc(sizeof(rrc_csp_nw_sharing_entry_type));
  uint8 shared_plmn_idx = 0;
  uint32 num_plmns = 0;
  vplmn_nw_db_entry->num_of_shared_plmns = 0;
  /* We should skip SIB reading only for cells whose PLMNs are not NAS Requested PLMNs/EPLMNs
     and whose PLMNs are already added to the list to be sent to NAS */
  if(((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
      (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)) &&
      (rrc_csp_int_data.curr_type_acq == RRC_CSP_SPEC_PLMN) &&
      (rrccsp_check_plmn_match(plmn_id,mib_ptr, FALSE) == FALSE) &&
      (rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_CSG))
  {
    if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
    {
      num_plmns = rrc_csp_int_data.plmn_list.num_plmns;
    }
    else if((rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN) ||
            (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))
    {
      num_plmns = rrc_csp_int_data.bplmn_fnd_list_wr_index;
    }
    if(rrc_is_nw_shared(mib_ptr))
    {
      is_valid_nw_sharing = TRUE;
      rrccsp_find_shared_plmns(
                               mib_ptr,
                               plmn_id,
                               vplmn_nw_db_entry
                               );
    }
    else
    {
      is_valid_nw_sharing = FALSE;
    }
    do
    {
      if((is_valid_nw_sharing) && (shared_plmn_idx < vplmn_nw_db_entry->num_of_shared_plmns) &&
         (shared_plmn_idx < MAX_SHARED_PLMNS))
      {
        plmn_id = vplmn_nw_db_entry->shared_plmns_access_info[shared_plmn_idx++].plmn_id;
      }
      for(i=0; i<num_plmns; i++)
      {
        if((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN) ||
           (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN))
        {
          if(rrc_csp_int_data.plmn_list.plmn[i].rat == SYS_RAT_UMTS_RADIO_ACCESS)
          {
            if( RRC_CSP_GET_INT_MCC(plmn_id) ==
                  RRC_CSP_GET_INT_MCC(rrc_csp_int_data.plmn_list.plmn[i].plmn) &&
                    RRC_CSP_GET_INT_MNC(plmn_id)
                      == RRC_CSP_GET_INT_MNC(rrc_csp_int_data.plmn_list.plmn[i].plmn))
            {
              ret_val = TRUE;
              break;
            }
            else
            {
              ret_val = FALSE;
            }
          }
        }
        else if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
        {
          if(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].rat == SYS_RAT_UMTS_RADIO_ACCESS)
          {
            nas_to_rrc_plmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(rrc_csp_int_data.rrc_rr_plmn_srch_cnf.available_plmn_list.info[i].plmn);
            if( RRC_CSP_GET_INT_MCC(plmn_id) ==
                RRC_CSP_GET_INT_MCC(nas_to_rrc_plmn_id) &&
                  RRC_CSP_GET_INT_MNC(plmn_id)
                    == RRC_CSP_GET_INT_MNC(nas_to_rrc_plmn_id))
            {
              ret_val = TRUE;
              break;
            }
            else
            {
              ret_val = FALSE;
            }
          }
        }
      }
      if(FALSE == ret_val)
      {
        rrc_free(vplmn_nw_db_entry);
        return ret_val;
      }
    }while((is_valid_nw_sharing) && (shared_plmn_idx < vplmn_nw_db_entry->num_of_shared_plmns));
  }

  if(TRUE == ret_val)
  { /* Add the PLMN to ACQ DB */
    rrccsp_add_plmn_acq_db_and_nw_db_entries(plmn_id, vplmn_nw_db_entry, is_valid_nw_sharing);
  }
  rrc_free(vplmn_nw_db_entry);
  return ret_val;
}
#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION 
/*===========================================================================

FUNCTION          rrccsp_do_gsm_bands_scan_first

DESCRIPTION       Decides to scan the GSM bands

DEPENDENCIES      None

RETURN VALUE      TRUE: GSM Bands should be scanned
                            FALSE: Seacrh should not be redirected to GSM

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_do_gsm_bands_scan_first(void)
{
  if(rrccsp_is_supported_band(RRC_CSP_PCS_BAND) 
     || rrccsp_is_supported_band(RRC_CSP_WCDMA_850_BAND)
     || rrccsp_is_supported_band(RRC_CSP_WCDMA_900_BAND)
    )
  {
    return TRUE;
  }
  return FALSE;
}
#endif /* FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION */

/*===========================================================================

FUNCTION: rrc_rr_unbar_flai_cells

DESCRIPTION:
           This function is called by RR upon the initiation of any cell selection, to unbar all the barred frequencies 
           because of partial roaming restrictions

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
=============================================================================*/
void rrc_rr_unbar_flai_cells(void)
{
  /*CR-151*/
  /* Unbar on any cell selection */
  rrc_csp_int_data.bar_for_resel_only = TRUE;
  /* Send Cell bar request to L1 to unbar all the frequencies barred due to CR151*/
  rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_FREQ_BARRED,0);
  rrc_csp_int_data.bar_for_resel_only = FALSE;
}


#ifdef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING

/*===========================================================================

FUNCTION: RR_GET_RRC_ACQ_CHNL_INFO

DESCRIPTION:
          This functin is called by GSM-RR to get WCDMA channel info. GSM-RR
          uses WCDMA channel info to eliminate GSM channels for faster scanning

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rr_get_rrc_acq_chnl_info(rrc_rr_wcdma_acq_chnl_info_type *acq_chnl_info)
{
  uint32 i = 0;

  WRRC_MSG0_HIGH("Fn rr_get_rrc_acq_chnl_info() called by GSM-RR");

  acq_chnl_info->num_chnls = rrc_csp_curr_select_data.num_available_freq;

  for(i = 0; i < rrc_csp_curr_select_data.num_available_freq; ++i)
  {
    acq_chnl_info->chnl_info[i].freq = rrc_csp_curr_select_data.available_freq_list[i];
    acq_chnl_info->chnl_info[i].band = rrc_convert_band_to_sys_band(
                                        rrc_csp_curr_select_data.available_freq_list_band[i]);

    MSG_HIGH("[%d] Freq %d & Band %d added to acq chnl info", i,
                                                              acq_chnl_info->chnl_info[i].freq, 
                                                              rrc_csp_curr_select_data.available_freq_list_band[i]);
  }

  return;
}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION: RR_GET_RRC_ACQ_CHNL_INFO_SUB_ID

DESCRIPTION:
          This function is called by GSM-RR to get WCDMA channel info when DSDS is enabled. GSM-RR
          uses WCDMA channel info to eliminate GSM channels for faster scanning

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rr_get_rrc_acq_chnl_info_sub_id(rrc_rr_wcdma_acq_chnl_info_type *acq_chnl_info,sys_modem_as_id_e_type as_id)
{
  if (as_id != rrc_get_as_id())
  {
    WRRC_MSG2_ERROR("AS-id mismatch %d, internal %d",as_id, rrc_get_as_id());
  }

  rr_get_rrc_acq_chnl_info(acq_chnl_info);
}
#endif

/*===========================================================================

FUNCTION: RRC_CLEAR_ACQ_CHNL_INFO

DESCRIPTION:
          This function is called by both RRC and GSM-RR to clear
          WCDMA Acq channel info so that no stale information is used
          later on during frequency scanning. This function is called 
          when UE camps or RRC/RR receive service request, plmn_list_req, etc.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rrc_clear_acq_chnl_info(void)
{
  WRRC_MSG0_HIGH("Fn rrc_clear_acq_chnl_info() called. Setting num_avail_freq to 0");
  rrc_csp_curr_select_data.num_available_freq = 0;                    
} 

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION: RRC_CLEAR_ACQ_CHNL_INFO_SUB_ID

DESCRIPTION:
          This function is called by both RRC and GSM-RR to clear
          WCDMA Acq channel info so that no stale information is used
          later on during frequency scanning. This function is called 
          when UE camps or RRC/RR receive service request, plmn_list_req, etc.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rrc_clear_acq_chnl_info_sub_id(sys_modem_as_id_e_type as_id)
{
  if (as_id != rrc_get_as_id())
 {
   WRRC_MSG2_ERROR("AS-id mismatch %d, internal %d",as_id, rrc_get_as_id());
  }

  rrc_clear_acq_chnl_info();
}
#endif
#endif /* FEATURE_UMTS_ACQ_CHNL_INFO_SHARING */


/*===========================================================================

FUNCTION          RRCCSP_IS_PRIMARY_PLMN_ID_PRESENT

DESCRIPTION       Used to find out if the primary plmn id is received in RRC dedicated OTA
                  
DEPENDENCIES      None

RETURN VALUE      TRUE: If primary plmn id is present
                  FALSE: Otherwise

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_is_primary_plmn_id_present(void)
{
  boolean status = FALSE;
  status = rrc_csp_int_data.primary_plmn_Id_present;
  return status;
}

/*===========================================================================

FUNCTION          RRCCSP_IS_CAMPED_CELL_NETWORK_SHARED

DESCRIPTION       Used to find out whether the current camped cell is a part of shared network
                  
DEPENDENCIES      None

RETURN VALUE      TRUE: Camped cell is a part of shared network
                  FALSE: Camped cell belongs to a dedicated network

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_is_camped_cell_network_shared()
{
  boolean status = FALSE;
  rrc_MasterInformationBlock *mib;
  mib = (rrc_MasterInformationBlock*)rrc_sibdb_return_sib_for_srv_cell(rrc_MIB);

  if(rrc_csp_int_data.primary_plmn_Id_present && (rrc_get_state() == RRC_STATE_CELL_DCH))
  {
    WRRC_MSG1_HIGH("Include primary plmnid in initial direct transfer msg = %d", rrc_csp_int_data.primary_plmn_Id_present);
    status = TRUE;
    return status;
  }
  if(mib != NULL)
  {
    if(rrc_is_nw_shared(mib))
    {
      status = TRUE;
    }
    else
    {
      status = FALSE;
    }
  }
  else
  {
    WRRC_MSG0_HIGH("NWS: MIB ptr is NULL for camped cell");
    status = FALSE;
  }
  return status;
}

/*===========================================================================

FUNCTION          RRCCSP_IS_MIB_PLMN_VALID

DESCRIPTION       If network is shared finds out the validity of the common PLMN given in the mib.
                  
DEPENDENCIES      None

RETURN VALUE      TRUE: common PLMN is valid.
                  FALSE: common PLMN is not valid and therefore can not be considered for PLMN selection

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_is_mib_plmn_valid(rrc_MasterInformationBlock *mib_ptr)
{
  boolean mibPLMN_Identity_valid = FALSE;

  if(rrc_is_nw_shared(mib_ptr))
  {
    mibPLMN_Identity_valid = mib_ptr->v690NonCriticalExtensions.masterInformationBlock_v690ext.multiplePLMN_List.mibPLMN_Identity;
  }
  else
  {
    mibPLMN_Identity_valid = TRUE;
  }

  return mibPLMN_Identity_valid;
}

/*===========================================================================

FUNCTION          RRCCSP_CONSTRUCT_MULTIPLE_PLMN_LIST_FROM_MIB

DESCRIPTION       Constructs the shared PLMN list given the Mib
                  
DEPENDENCIES      None

RETURN VALUE      number of shared PLMNs

SIDE EFFECTS      None
===========================================================================*/
uint8 rrccsp_construct_multiple_plmn_list_from_mib(
                                                   rrc_MasterInformationBlock *mib_ptr,
                                                   rrc_plmn_identity_type *shared_PLMN_list)
{
  rrc_plmn_identity_type mib_PLMN_ID;
  rrc_MultiplePLMN_List_r6 multiple_PLMN_List;
  rrc_plmn_identity_type plmn_id;
  boolean is_mcc_valid;
  byte mcc_to_be_used[RRC_MAX_MCC_DIGITS];
  uint8 number_of_PLMNs = 0;
  int i;
  uint32 n_value =0, n_index = 0;

  rrc_translate_gsm_map_plmn_id(&(mib_ptr->plmn_Type.u.gsm_MAP->plmn_Identity),&mib_PLMN_ID);

  if(rrccsp_is_mib_plmn_valid(mib_ptr))
  {
    shared_PLMN_list[number_of_PLMNs++] = mib_PLMN_ID;
  }

  for(i = 0; i < RRC_MAX_MCC_DIGITS; i++)
  {
    mcc_to_be_used[i] = mib_PLMN_ID.mcc[i];
  }

  multiple_PLMN_List = mib_ptr->v690NonCriticalExtensions.masterInformationBlock_v690ext.multiplePLMN_List;

  n_value = multiple_PLMN_List.multiplePLMNs.n;

  while((n_value != 0) && (number_of_PLMNs < MAX_SHARED_PLMNS))
  {
    rrc_translate_gsm_map_plmn_id_with_optional_mcc(&is_mcc_valid, &(multiple_PLMN_List.multiplePLMNs.elem[n_index]), &plmn_id);

    if(!is_mcc_valid)
    {
      for(i = 0; i < RRC_MAX_MCC_DIGITS; i++)
      {
        plmn_id.mcc[i] = mcc_to_be_used[i];
      }
    }
    else
    {
      for(i = 0; i < RRC_MAX_MCC_DIGITS; i++)
      {
        mcc_to_be_used[i] = plmn_id.mcc[i];
      }
    }

    shared_PLMN_list[number_of_PLMNs++] = plmn_id;
    n_value--;
    n_index++;
  }
  return number_of_PLMNs;
}

/*===========================================================================

FUNCTION          RRCCSP_PRINT_SHARED_PLMNS

DESCRIPTION       prints the list of shared PLMNs
                  
DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
static void rrccsp_print_shared_PLMNs(int number_of_PLMNs, rrc_plmn_identity_type *shared_PLMN_list)
{
  int     i;

  WRRC_MSG1_HIGH("NWS: Number of Shared PLMNs: %d ",number_of_PLMNs);
  WRRC_MSG0_HIGH("NWS: Shared PLMN List before Prioritizing ");
  WRRC_MSG0_HIGH("******************************************************************");
  for(i =0 ; i<number_of_PLMNs; i++)
  {
    WRRC_MSG2_HIGH(" MCC:MNC - %d:%d",RRC_CSP_GET_INT_MCC(shared_PLMN_list[i]),
                                                      RRC_CSP_GET_INT_MNC(shared_PLMN_list[i])
                                                      );
  }
  WRRC_MSG0_HIGH("******************************************************************");
}

/*===========================================================================

FUNCTION          RRCCSP_PRIORITIZE_THE_LIST_OF_SHARED_PLMNS

DESCRIPTION       Used to prioritize a PLMN among the available PLMNs based on the current scan type so that
                  this PLMN is considered first for PLMN selection if it is part of the list of shared PLMNs.
                  
DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_prioritize_the_list_of_shared_plmns(void)
{
  int i =0 ;
  rrc_csp_shared_plmns_access_info_type temp_plmn_access_info;
  rrc_plmn_identity_type plmn_tobe_prioritized;
  boolean ehplmn_present = FALSE;
  rrc_csp_substate_e_type curr_csp_substate = rrc_get_csp_substate();
  rrc_csp_nw_sharing_entry_type *bcch_plmn_list = &rrc_csp_int_data.curr_nw_db_entry;

  if( (rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN) ||
       (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN) )
  {
    WRRC_MSG1_HIGH("NWS: No Need to prioritize. curr_scan : %d",rrc_csp_int_data.curr_scan);
    return;
  }

  if( rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
  {
    plmn_tobe_prioritized = rrc_csp_int_data.wtow_bplmn_req_list[0].plmn_id;
  }
  else if( rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
  {
    plmn_tobe_prioritized = rrc_csp_int_data.bplmn_req_list[0].plmn_id;
  }
  else
  {
    plmn_tobe_prioritized = rrc_csp_int_data.curr_req_plmn.plmn_id;
  }

  MSG_HIGH("NWS: Current Scan: %d, PLMN tobe prioritized: MCC:MNC - %d:%d",
                                                                      rrc_csp_int_data.curr_scan,
                                                                      RRC_CSP_GET_INT_MCC(plmn_tobe_prioritized),
                                                                      RRC_CSP_GET_INT_MNC(plmn_tobe_prioritized));

  /* Prioritizing to keep the best PLMN in index 0 */
  if((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
#ifdef FEATURE_FEMTO_CSG
     (rrc_csp_int_data.curr_req_plmn.csg_id == SYS_CSG_ID_INVALID) &&
#endif
     (rrc_csp_int_data.can_ehplmn_be_camped) &&
     (curr_csp_substate == RRCCSP_CELL_SEL_WT_SIB) &&
     (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN))
  {
    for(i = 0; i < bcch_plmn_list->num_of_shared_plmns; i++)
    {
      if(rrc_is_bcch_plmn_an_hplmn_or_ehplmn(bcch_plmn_list->shared_plmns_access_info[i].plmn_id))
      {
        /* Move the HPLMN/EHPLMN to the top of the PLMN list */

        WRRC_MSG2_HIGH("NWS: Prioritizing the PLMN MCC:MNC - %d:%d",
                                                         RRC_CSP_GET_INT_MCC(bcch_plmn_list->shared_plmns_access_info[i].plmn_id),
                                                         RRC_CSP_GET_INT_MNC(bcch_plmn_list->shared_plmns_access_info[i].plmn_id));

        temp_plmn_access_info = bcch_plmn_list->shared_plmns_access_info[0];
        bcch_plmn_list->shared_plmns_access_info[0] = bcch_plmn_list->shared_plmns_access_info[i];
        bcch_plmn_list->shared_plmns_access_info[i] = temp_plmn_access_info;
        ehplmn_present = TRUE;
        break;
      }
    }
  }

  for(i = 0; i < bcch_plmn_list->num_of_shared_plmns; i++)
  {
    if(rrc_bcch_and_nas_plmns_match(bcch_plmn_list->shared_plmns_access_info[i].plmn_id,
                                                               plmn_tobe_prioritized))
    {
      /* Move it to the begining */

      WRRC_MSG2_HIGH("NWS: Prioritizing the PLMN MCC:MNC - %d:%d",
                                                       RRC_CSP_GET_INT_MCC(bcch_plmn_list->shared_plmns_access_info[i].plmn_id),
                                                       RRC_CSP_GET_INT_MNC(bcch_plmn_list->shared_plmns_access_info[i].plmn_id));
      if((ehplmn_present) && (i == 0))
      {
        /* If EHPLMN/HPLMN is already moved to the top of the PLMN list and if EHPLMN/HPLMN is the NAS requested PLMN, then dont change the list */
        break;
      }
      else if(ehplmn_present)
      {
        /* If EHPLMN/HPLMN is moved to the top of the PLMN list and if a NAS requested PLMN is also found, move NAS requested PLMN to the second place in the list*/
        temp_plmn_access_info = bcch_plmn_list->shared_plmns_access_info[1];
        bcch_plmn_list->shared_plmns_access_info[1] = bcch_plmn_list->shared_plmns_access_info[i];
        bcch_plmn_list->shared_plmns_access_info[i] = temp_plmn_access_info;
      }
      else
      {
        /* If EHPLMN/HPLMN is not present in the PLMN list and if a NAS requested PLMN is found, move NAS requested PLMN to the top of the list*/
        temp_plmn_access_info = bcch_plmn_list->shared_plmns_access_info[0];
        bcch_plmn_list->shared_plmns_access_info[0] = bcch_plmn_list->shared_plmns_access_info[i];
        bcch_plmn_list->shared_plmns_access_info[i] = temp_plmn_access_info;
      }
      break;
    }
  }

  WRRC_MSG1_HIGH("NWS: Number of Shared PLMNs: %d ",bcch_plmn_list->num_of_shared_plmns);
  WRRC_MSG0_HIGH("NWS: Shared PLMN List after Prioritizing ");
  WRRC_MSG0_HIGH("******************************************************************");
  for(i =0 ; i<bcch_plmn_list->num_of_shared_plmns; i++)
  {
    WRRC_MSG2_HIGH(" MCC:MNC - %d:%d",
                                RRC_CSP_GET_INT_MCC(bcch_plmn_list->shared_plmns_access_info[i].plmn_id),
                                RRC_CSP_GET_INT_MNC(bcch_plmn_list->shared_plmns_access_info[i].plmn_id));
  }
  WRRC_MSG0_HIGH("******************************************************************");

  return;
}

/*===========================================================================

FUNCTION          RRCCSP_FIND_SHARED_PLMNS

DESCRIPTION       Finds out the list of shared PLMNs and fills in the nw_db_entry_ptr.
                  
DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_find_shared_plmns(
                              rrc_MasterInformationBlock* mib_ptr,
                              rrc_plmn_identity_type plmn_id,
                              rrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
                              )
{
  rrc_plmn_identity_type shared_plmn_list[MAX_SHARED_PLMNS];
  uint8 num_of_shared_plmns;
  uint8 i;

  nw_db_entry_ptr->common_plmn_id = plmn_id;
  num_of_shared_plmns = rrccsp_construct_multiple_plmn_list_from_mib(
                                                                     mib_ptr,
                                                                     shared_plmn_list);

  rrccsp_print_shared_PLMNs(num_of_shared_plmns,shared_plmn_list);

  for(i = 0; (i < MAX_SHARED_PLMNS) && (i<num_of_shared_plmns); i++)
  {
    nw_db_entry_ptr->shared_plmns_access_info[i].plmn_id= shared_plmn_list[i];
  }
  nw_db_entry_ptr->num_of_shared_plmns = num_of_shared_plmns;
}


/*===========================================================================

FUNCTION          RRCCSP_GET_CELL_PLMN_STATUS

DESCRIPTION       This function compares the requested PLMN with the currently
                  acquired cells PLMN.
                  
DEPENDENCIES      None

RETURN VALUE      RRC_CSP_PLMN_SAME_AS_REQ
                  RRC_CSP_PLMN_DIFF_THAN_REQ

SIDE EFFECTS      None
===========================================================================*/
static rrc_csp_plmn_status_e_type rrccsp_get_cell_plmn_status
(
  rrc_csp_acq_entry_type* acq_entry_ptr,
  rrc_plmn_identity_type reference_plmn
)
{
      rrc_csp_plmn_status_e_type  cell_plmn_status;
#ifdef FEATURE_FEMTO_CSG
      rrc_csp_substate_e_type curr_csp_substate = rrc_get_csp_substate();
#endif

      if(rrc_bcch_and_nas_plmns_match(acq_entry_ptr->plmn_id, reference_plmn))
      {
        WRRC_MSG0_HIGH("PLMN is the same as requested BCCH and NAS PLMN match");
        cell_plmn_status = RRC_CSP_PLMN_SAME_AS_REQ;
      }
#ifdef FEATURE_FEMTO_CSG
      else if((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)||
               (((rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)||
                 (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL))&&
                 (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)&&
                 (curr_csp_substate == RRCCSP_CELL_SEL_WT_SIB)&&
                 (rrc_csp_int_data.curr_req_plmn.csg_id != SYS_CSG_ID_INVALID)
#ifdef FEATURE_WRLF_SYSTEM_SEL
                 &&(!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
                 ))
      {
        WRRC_MSG0_HIGH("CSG: Skip equivalent PLMNs check");
        cell_plmn_status = RRC_CSP_PLMN_DIFF_THAN_REQ;
      }
#endif
      else
      {
        if((rrccsp_check_for_eplmn(
                  mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                    reference_plmn))) == TRUE)
        {
          WRRC_MSG0_HIGH("NAS PLMN is in the eq PLMN list");
          if((rrccsp_check_for_eplmn(
                    mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                     acq_entry_ptr->plmn_id))) == TRUE)
          {
            WRRC_MSG0_HIGH("BCCH PLMN is in the eq PLMN list");
            cell_plmn_status = RRC_CSP_PLMN_SAME_AS_REQ;
          }
          else
          {
            WRRC_MSG0_HIGH("BCCH PLMN not in eq PLMN list");
            cell_plmn_status = RRC_CSP_PLMN_DIFF_THAN_REQ;
          }
        }
        else
        {
          WRRC_MSG0_HIGH("NAS PLMN not in eq PLMN list");
          cell_plmn_status = RRC_CSP_PLMN_DIFF_THAN_REQ;
        }
      }
      if((cell_plmn_status == RRC_CSP_PLMN_DIFF_THAN_REQ) &&
         (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
#ifdef FEATURE_FEMTO_CSG
         (rrc_csp_int_data.curr_req_plmn.csg_id == SYS_CSG_ID_INVALID) &&
#endif
         (rrc_csp_int_data.can_ehplmn_be_camped) &&
         (curr_csp_substate == RRCCSP_CELL_SEL_WT_SIB) &&
         (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN))
      {
        if(rrc_is_bcch_plmn_an_hplmn_or_ehplmn(acq_entry_ptr->plmn_id))
        {
          WRRC_MSG0_HIGH("BCCH PLMN is in the HPLMN/EHPLMN list ");
          cell_plmn_status = RRC_CSP_PLMN_SAME_AS_REQ;
        }
      }
      return cell_plmn_status;
}

/*===========================================================================

FUNCTION RRCCSP_CHECK_PLMN_MATCH

DESCRIPTION

  This function checks the PLMN ID given as an arguement is 
  matching with the NAS requested PLMN or equivalent PLMN.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  boolean

SIDE EFFECTS
===========================================================================*/
boolean rrccsp_check_plmn_match( rrc_plmn_identity_type plmn_id
  , rrc_MasterInformationBlock *mib_ptr, boolean update_acq_db)
{
   rrc_csp_plmn_status_e_type cell_plmn_status;
    boolean plmns_match = FALSE;
    rrc_csp_acq_entry_type    *acq_entry =  rrc_malloc(sizeof(rrc_csp_acq_entry_type));

    rrc_csp_nw_sharing_entry_type  *nw_db_entry = rrc_malloc(sizeof(rrc_csp_nw_sharing_entry_type));
    uint8 shared_plmn_idx = 0; 
    nw_db_entry->num_of_shared_plmns = 0;
    cell_plmn_status = RRC_CSP_PLMN_DIFF_THAN_REQ;

    if(rrc_is_nw_shared(mib_ptr))
    {
      acq_entry->is_valid_nw_sharing = TRUE;
      rrccsp_find_shared_plmns(
                               mib_ptr,
                               plmn_id,
                               nw_db_entry
                               );
    }
    else
    {
      acq_entry->is_valid_nw_sharing = FALSE;
    }

  
    do
    {
      if((acq_entry->is_valid_nw_sharing) && (shared_plmn_idx < nw_db_entry->num_of_shared_plmns) &&
         (shared_plmn_idx < MAX_SHARED_PLMNS))
      {
        acq_entry->plmn_id = nw_db_entry->shared_plmns_access_info[shared_plmn_idx++].plmn_id;
      }
      else
      {
        acq_entry->plmn_id = plmn_id;
      }

      if(rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ)
      {
        /* For ANY PLMN Request, we don't care what PLMN is selected */
        cell_plmn_status = RRC_CSP_PLMN_SAME_AS_REQ;
        plmns_match = TRUE;
      }
      else/* PLMN request for a specific PLMN */
      {
        cell_plmn_status = rrccsp_get_cell_plmn_status(acq_entry, rrc_csp_int_data.curr_req_plmn.plmn_id);
        if(cell_plmn_status == RRC_CSP_PLMN_SAME_AS_REQ)
        {
          plmns_match = TRUE;
        }
  
      } /* end PLMN request for a specific PLMN */
  
    }while((acq_entry->is_valid_nw_sharing) && 
           (shared_plmn_idx < nw_db_entry->num_of_shared_plmns) &&
           (plmns_match == FALSE));

  if((FALSE == plmns_match) && (update_acq_db))
  {
    rrccsp_add_plmn_acq_db_and_nw_db_entries(acq_entry->plmn_id, nw_db_entry, acq_entry->is_valid_nw_sharing);
  }
  rrc_free(acq_entry);
  rrc_free(nw_db_entry);
  return plmns_match;
}


/*===========================================================================

FUNCTION          RRCCSP_RESET_NUM_TRIES_DEST_FREQ
DESCRIPTION       Resets the num tries to be done om destination frequency
                  which is received in the case of redirection
                  
DEPENDENCIES      None

RETURN VALUE      void
                 

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_reset_num_tries_dest_freq()
{
  /* Check if dest freq in use and reset it */    
  if(rrc_csp_curr_select_data.dest_freq_in_use)
  {
    rrc_csp_curr_select_data.num_tries_on_dest_freq = 0;
  }
}

/*===========================================================================

FUNCTION          RRCCSP_GET_CELL_STATUS

DESCRIPTION       Finds out the suitability of the cell for camping based on the cell bar status, requesed PLMN etc.
                  
DEPENDENCIES      None

RETURN VALUE      
                  RRC_CSP_CELL_BETTER_THAN_EXPECTED: Cell exceeds expectations.
                  RRC_CSP_CELL_AS_EXPECTED: Cell meets expectations.
                  RRC_CSP_CELL_BELOW_EXPECTED: Cell is below expectations.

SIDE EFFECTS      None
===========================================================================*/
rrc_csp_cell_actual_vs_expected_e_type rrccsp_get_cell_status(
                                                              rrc_csp_plmn_status_e_type* cell_plmn_status,
                                                              rrc_csp_acq_entry_type* acq_entry_ptr
                                                              ,rrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
                                                              )
  {
  rrc_csp_cell_actual_vs_expected_e_type cell_status = RRC_CSP_CELL_BELOW_EXPECTED;

  boolean plmns_match = FALSE;

#ifdef FEATURE_WRLF_SYSTEM_SEL
  rrc_csp_plmn_status_e_type cell_plmn_status_wrt_rlf_plmn = RRC_CSP_PLMN_DIFF_THAN_REQ;
#endif

  uint8 shared_plmn_idx = 0;

  do
  {
    if(acq_entry_ptr->is_valid_nw_sharing)
    {
      acq_entry_ptr->plmn_id = nw_db_entry_ptr->shared_plmns_access_info[shared_plmn_idx].plmn_id;
      acq_entry_ptr->status_mask = nw_db_entry_ptr->shared_plmns_access_info[shared_plmn_idx++].status_mask;
    }

    /* Now decide if the cell is good for camping */

    /* Generate PLMN status based on received and requested PLMN IDs */
    if((rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ)
#ifdef FEATURE_WRLF_SYSTEM_SEL
       && !RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()
#endif
      )
    {
      /* For ANY PLMN Request, we don't care what PLMN is selected */
      *cell_plmn_status = RRC_CSP_PLMN_SAME_AS_REQ;
      plmns_match = TRUE;
    }
    else  /* PLMN request for a specific PLMN */
    {
      *cell_plmn_status = rrccsp_get_cell_plmn_status(acq_entry_ptr, rrc_csp_int_data.curr_req_plmn.plmn_id);
      if(*cell_plmn_status == RRC_CSP_PLMN_SAME_AS_REQ)
      {
        plmns_match = TRUE;
      }
      else
      {
        plmns_match = FALSE;
      }

#ifdef FEATURE_WRLF_SYSTEM_SEL
      if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
        {
        cell_plmn_status_wrt_rlf_plmn = rrccsp_get_cell_plmn_status(acq_entry_ptr, rrc_csp_int_data.rlf_plmn_id);
        }
#endif
    } /* end PLMN request for a specific PLMN */

    /* Compare the cell based on status mask formed from SIBs received for this cell */

    /* Decide if this cell is good enough - If actual cell status is lower than the one
      * expected by Selection Controller, this cell is not selected.
      */
#ifdef FEATURE_WRLF_SYSTEM_SEL
    if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
    {
      cell_status = rrc_csp_compare_expected_cell_for_conn_oos(*cell_plmn_status,
                                           cell_plmn_status_wrt_rlf_plmn,
                                           acq_entry_ptr,
                                           rrc_csp_int_data.curr_type_acq);
    }
    else
#endif
    {
    cell_status = rrc_csp_compare_expected_cell(*cell_plmn_status,
                                         acq_entry_ptr,
                                         rrc_csp_int_data.curr_type_acq);
    }

  }while((acq_entry_ptr->is_valid_nw_sharing) && 
         (shared_plmn_idx < nw_db_entry_ptr->num_of_shared_plmns) &&
         (cell_status == RRC_CSP_CELL_BELOW_EXPECTED));

  rrc_csp_int_data.bar_for_resel_only = !plmns_match;
  rrc_csp_int_data.non_eq_plmn_barred=!plmns_match;

  return cell_status;
}

/*===========================================================================

FUNCTION          RRCCSP_GET_CELL_STATUS_DURING_BPLMN_SEARCH

DESCRIPTION       Finds out the suitability of the cell during bplmn search based on the 
                  cell bar status, list of requesed PLMNs etc.
                  
DEPENDENCIES      None

RETURN VALUE      
                  RRC_CSP_CELL_BETTER_THAN_EXPECTED: Cell exceeds expectations.
                  RRC_CSP_CELL_AS_EXPECTED: Cell meets expectations.
                  RRC_CSP_CELL_BELOW_EXPECTED: Cell is below expectations.

SIDE EFFECTS      None
===========================================================================*/
rrc_csp_cell_actual_vs_expected_e_type rrccsp_get_cell_status_during_bplmn_search(
                                                                                  rrc_csp_acq_entry_type* acq_entry_ptr
                                                                                  ,rrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
                                                                                  )
{
  /* Temporary storage for PLMN status */
  rrc_csp_plmn_status_e_type  cell_plmn_status;
  rrc_csp_cell_actual_vs_expected_e_type cell_status = RRC_CSP_CELL_BELOW_EXPECTED;
  rrc_plmn_identity_type  hplmn_id;
  boolean wtow_search = FALSE;

#ifdef FEATURE_EQUIVALENT_HPLMN
  boolean is_hplmn_or_ehplmn_found = FALSE;
  uint32 j = 0;
#endif

  uint8 shared_plmn_idx = 0;

  if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
  {
    hplmn_id = rrc_csp_int_data.bplmn_req_list[0].plmn_id;
    wtow_search = FALSE;
  }
  else if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
  {
    hplmn_id = rrc_csp_int_data.wtow_bplmn_req_list[0].plmn_id;
    wtow_search = TRUE;
  }
  else
  {
    WRRC_MSG1_HIGH(" Wrong call to this function for scan type %d",rrc_csp_int_data.curr_scan);
    return cell_status;
  }

  do
  {
    if(acq_entry_ptr->is_valid_nw_sharing)
    {
      acq_entry_ptr->plmn_id = nw_db_entry_ptr->shared_plmns_access_info[shared_plmn_idx].plmn_id;
      acq_entry_ptr->status_mask = nw_db_entry_ptr->shared_plmns_access_info[shared_plmn_idx++].status_mask;
    }

    /* NAS requirement is that when Background Service Request is sent by NAS
      * when UE mode is Manual then RRC should populate the available PLMN list
      * only if the PLMNs match. 
      * If UE is in Automatic Mode, then filtering of PLMNs will be done by NAS
      * and RRC should report ALL available PLMNs.
      */
    if(rrc_csp_int_data.service_search_bgnd != SYS_SERVICE_SEARCH_BACKGROUND_MANUAL)
    {
      if(!wtow_search)
      {
        rrccsp_update_background_plmn_list(acq_entry_ptr->plmn_id, acq_entry_ptr->cell_id);
      }
      else
      {
        rrccsp_update_wtow_background_plmn_list(acq_entry_ptr->plmn_id, acq_entry_ptr->cell_id);
      }
    }

    /* Generate PLMN status based on received and requested PLMN IDs */
    if(rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ)
    {
      /* For ANY PLMN Request, we don't care what PLMN is selected */
      cell_plmn_status = RRC_CSP_PLMN_SAME_AS_REQ;
    }
    else  /* match bcch and nas plmns */
    {
      if(rrc_bcch_and_nas_plmns_match(acq_entry_ptr->plmn_id, hplmn_id))
      {
        WRRC_MSG0_HIGH("PLMN is the same as requested BCCH and NAS PLMN match");
        cell_plmn_status = RRC_CSP_PLMN_SAME_AS_REQ;
      }
      else
      {
        if(   (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC)
            || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL)
            || (rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_PRIORITY_PLMN)
                /* The new search type is for a customer requirement where if the UE is camped on HPLMN,
                 * NAS has to periodically trigger Automatic search(SYS_SERVICE_SEARCH_PRIORITY_PLMN) for 
                 * searching another customer's PLMN*/
           )
        {
          WRRC_MSG0_HIGH("SSB: Checking for EPLMNs");

          if((rrccsp_check_for_eplmn(
                      mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                       rrc_csp_int_data.curr_req_plmn.plmn_id))) == TRUE)
          {
            WRRC_MSG0_HIGH("NAS PLMN is in the eq PLMN list");
            if((rrccsp_check_for_eplmn(
                        mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                        acq_entry_ptr->plmn_id))) == TRUE)
            {
              WRRC_MSG0_HIGH("BCCH PLMN is in the eq PLMN list");
              cell_plmn_status = RRC_CSP_PLMN_SAME_AS_REQ;
            }
            else
            {
              WRRC_MSG0_HIGH("BCCH PLMN not in eq PLMN list");
              cell_plmn_status = RRC_CSP_PLMN_DIFF_THAN_REQ;
            }
          }
          else
          {
            WRRC_MSG0_HIGH("NAS PLMN not in eq PLMN list");
            cell_plmn_status = RRC_CSP_PLMN_DIFF_THAN_REQ;
          }
        }
        else
        {
#ifdef FEATURE_EQUIVALENT_HPLMN
          /* Initialise the is_hplmn_or_ehplmn_found to FALSE */
          is_hplmn_or_ehplmn_found = FALSE;

          /* Convert the current plmn_id to nas plmn id before using NAS function */

          if(rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
          {
            /* Loop through until HPLMN/EHPLMN is found */
            for(j=1; j<rrc_csp_int_data.bplmn_num_req;j++)
            {
              if(rrc_bcch_and_nas_plmns_match(acq_entry_ptr->plmn_id,
                                      rrc_csp_int_data.bplmn_req_list[j].plmn_id))
              {
                if(rrc_is_bcch_plmn_an_hplmn_or_ehplmn(acq_entry_ptr->plmn_id))
                {
                  is_hplmn_or_ehplmn_found = TRUE;
                  break;
                }
              }
            }
          }
          else if(rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
          {
            /* Loop through until HPLMN/EHPLMN is found */
            for(j=1; j<rrc_csp_int_data.wtow_bplmn_num_req;j++)
            {
              if(rrc_bcch_and_nas_plmns_match(acq_entry_ptr->plmn_id,
                                   rrc_csp_int_data.wtow_bplmn_req_list[j].plmn_id))
              {
                if(rrc_is_bcch_plmn_an_hplmn_or_ehplmn(acq_entry_ptr->plmn_id))
                {
                   is_hplmn_or_ehplmn_found = TRUE;
                   break;
                }
              }
            }
          }

          /* HPLMN/EHPLMN is found */
          if(is_hplmn_or_ehplmn_found)
          {
            WRRC_MSG0_HIGH("EHPLMN: HPLMN/EHPLMN is found ");
            cell_plmn_status = RRC_CSP_PLMN_SAME_AS_REQ;
          }
          else  /* HPLMN/EHPLMN is not found */
          {
            WRRC_MSG0_HIGH("EHPLMN: HPLMN/EHPLMN is not found ");
            cell_plmn_status = RRC_CSP_PLMN_DIFF_THAN_REQ;
          }
#else
          cell_plmn_status = RRC_CSP_PLMN_DIFF_THAN_REQ;
#endif
        }
      } /* end eplmn */
    } /* end match bcch and nas plmns */

    /* Compare the cell based on status mask formed from SIBs received for this cell */

    /* Decide if this cell is good enough - If actual cell status is lower than the one
      * expected by Selection Controller, this cell is not selected.
      */
    cell_status = rrc_csp_compare_expected_cell(cell_plmn_status,
                                       acq_entry_ptr,
                                       rrc_csp_int_data.curr_type_acq);

  }
  while((acq_entry_ptr->is_valid_nw_sharing) && 
         (shared_plmn_idx < nw_db_entry_ptr->num_of_shared_plmns) &&
         (cell_status == RRC_CSP_CELL_BELOW_EXPECTED));

  return cell_status;
}

/*===========================================================================

FUNCTION          RRC_CSP_ADD_FREQ_TO_AVAILABLE_FREQ_LIST

DESCRIPTION       Adds a given frequency to avaialble frequency list. All the frequencies which are in the 
                  predefined range of these frequencies are skipped while scanning later.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_add_freq_to_available_freq_list(rrc_freq_type freq)
{
  uint32 i;
  /* Don't allow duplicate frequencies in available frequencies list */
  boolean  freq_in_list = FALSE;
  rrc_csp_band_class_type curr_band = 0;
  rrc_plmn_identity_type plmn_id;  

#if defined(FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  detected_frequency_type detected_wcdma_frequency;
#endif

  /* Avoid duplicate entry of the frequency in the list */
  for(i=0; i<rrc_csp_curr_select_data.num_available_freq; i++)
  {
      if(freq == rrc_csp_curr_select_data.available_freq_list[i])
      {
          freq_in_list = TRUE;
          break;
      }
  }

  if(!freq_in_list)
  {
    if(rrc_csp_curr_select_data.num_available_freq < RRC_MAX_NUM_FREQ - 1)
    {
      #ifdef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
      /* store band information as well. As this band information is now sent to GSM-RR so that 
       * GSM-RR can accordingly eliminate GSM channels
       */
      rrc_csp_curr_select_data.available_freq_list_band[rrc_csp_curr_select_data.num_available_freq] =
                                                               rrc_csp_curr_select_data.current_band;
      #endif

      rrc_csp_curr_select_data.available_freq_list[rrc_csp_curr_select_data.num_available_freq++] =
        freq;

      WRRC_MSG2_HIGH("frequency %d band %d added to available PLMN list", freq, 
                                                                    rrc_csp_curr_select_data.current_band
                                                                    );
    }
    else
    {
      WRRC_MSG1_ERROR("ERR: Max Avail Freq List reached %d. Resetting num_available_freq to 0",
                                    rrc_csp_curr_select_data.num_available_freq);
      rrc_csp_curr_select_data.num_available_freq = 0;

      #ifdef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
      /* store band information as well. As this band information is now sent to GSM-RR so that 
       * GSM-RR can accordingly eliminate GSM channels
       */
      rrc_csp_curr_select_data.available_freq_list_band[rrc_csp_curr_select_data.num_available_freq] =
                                                                 rrc_csp_curr_select_data.current_band;
      #endif

      rrc_csp_curr_select_data.available_freq_list[rrc_csp_curr_select_data.num_available_freq++] =
        freq;

      WRRC_MSG2_HIGH("frequency %d band %d added to available PLMN list", freq, 
                                                                    rrc_csp_curr_select_data.current_band
                                                                    );
    }
  }
  else
  {
    WRRC_MSG1_HIGH("freq %d already in list. Not adding",freq);
  }

#if defined(FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  /* During acq db scan, current band will not be updated in curr select data 
     Derive the absolute frequency by passing the band class from UARFCN */
  
  if(rrc_csp_curr_select_data.acq_db_in_use)
  {
    memset(plmn_id.mcc, 0, RRC_MAX_MCC_DIGITS * sizeof(byte));
    curr_band = rrc_get_frequency_band(freq,plmn_id);
  }
  else
  {
    curr_band = rrc_csp_curr_select_data.current_band;
  }

  /*First convet the UARFCN to frequency */
  detected_wcdma_frequency.frequency = rrccsp_derive_freq_from_uarfcn(freq,curr_band);
  detected_wcdma_frequency.bandwidth = 50;/*50*100KHz*/
  detected_wcdma_frequency.rat = SYS_RAT_UMTS_RADIO_ACCESS;
  rrc_csp_add_frequency_to_detected_cell_list(&detected_wcdma_frequency);
#endif

}

/*===========================================================================

FUNCTION          RRCCSP_DOES_MCC_BELONG_TO_US

DESCRIPTION       This function returns TRUE if the passed PLMN belongs to US
                  otherwise it returns FALSE.
                                    
DEPENDENCIES      None

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_does_mcc_belong_to_US(rrc_plmn_identity_type plmn_id)
{
  if(((RRC_CSP_GET_INT_MCC(plmn_id) >= USA_MCC_START_RANGE)
        && (RRC_CSP_GET_INT_MCC(plmn_id) <= USA_MCC_END_RANGE))
     || (RRC_CSP_GET_INT_MCC(plmn_id) == USA_MCC1))
  {
    WRRC_MSG1_HIGH("MCC %d belongs to US", RRC_CSP_GET_INT_MCC(plmn_id));
    return TRUE;
  } 
  else
  {
    WRRC_MSG1_HIGH("MCC %d does NOT belong to US", RRC_CSP_GET_INT_MCC(plmn_id));
    return FALSE;
  }
} /* rrccsp_does_mcc_belong_to_US */


/*===========================================================================
FUNCTION: RRC_CSP_IGNORE_BARRED_RSV_STATUS_DUE_TO_UEUT

DESCRIPTION: This function is called to check if UE is under test. The conditions
          for the "under test" are specified by the carrier. If the default 
          conditions are met then UE camps on the cell even if the cell is barred
          or reserved for operator use.

DEPENDENCIES: NONE

RETURN VALUE: TRUE: UE is under test. So ignore barred/reserve status 
              FALSE: either UE is not under test or the conditions are not met. 

SIDE EFFECTS: NONE
=============================================================================*/
#if !defined (FEATURE_UIM_DISABLE_GSDI_INTERFACE) && !defined (FEATURE_DUAL_SIM)
boolean rrc_csp_ignore_barred_rsv_status_due_to_ueut(rrc_plmn_identity_type plmn_id)
{
  rrc_plmn_identity_type hplmn_id;
  byte buffer[GSDI_MAX_DATA_BLOCK_LEN];
  gsdi_card_mode_capability_T card_mode;
  gsdi_returns_T mode_read_status = gsdi_nas_get_card_mode(&card_mode);
  gsdi_returns_T sim_read_status;

  if(rrc_ignore_cell_bar_nv == FALSE)
  {
    WRRC_MSG1_HIGH(" Ignore Cell Bar Staus is %d so return",rrc_ignore_cell_bar_nv);
    return FALSE;
  }
#ifdef FEATURE_DUAL_SIM
  hplmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(
                            reg_sim_per_subs_hplmn_get(rrc_get_as_id()));
#else
  hplmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(reg_sim_hplmn_get());
#endif

  WRRC_MSG2_HIGH("UEUT: card_mode = %d, mode_read_st = %d", card_mode, mode_read_status);

  if(mode_read_status == GSDI_SUCCESS)
  {
    if(card_mode == GSDI_CARD_USIM)
    {
      sim_read_status  = gsdi_sim_read(UIM_USIM_AD, GSDI_NO_INDEX, 0, NULL, 0, buffer, 0, 0, NULL, 1, NULL);
    }
    else if(card_mode == GSDI_CARD_GSM)
    {
      sim_read_status  = gsdi_sim_read(UIM_GSM_AD, GSDI_NO_INDEX, 0, NULL, 0, buffer, 0, 0, NULL, 1, NULL);
    }
    else
    {
      WRRC_MSG0_ERROR("UEUT: CARD MODE NOT HANDLED");
      return (FALSE);
    }
  }
  else
  {
    WRRC_MSG0_ERROR("UEUT: card mode reading failed. Return");
    return (FALSE);
  }

  WRRC_MSG2_HIGH("sim_read_status = %d, 1st byte: buffer[0] = %d", sim_read_status, buffer[0]);
  WRRC_MSG2_HIGH("HPLMN MCC-MNC: %d-%d", RRC_CSP_GET_INT_MCC(hplmn_id), RRC_CSP_GET_INT_MNC(hplmn_id));
  WRRC_MSG2_HIGH("PLMN  MCC-MNC: %d-%d", RRC_CSP_GET_INT_MCC(plmn_id), RRC_CSP_GET_INT_MNC(plmn_id));

  if(
     (
         (RRC_CSP_GET_INT_MCC(hplmn_id) == JAPAN_MCC1) 
      && (RRC_CSP_GET_INT_MNC(hplmn_id) == MNC1)      
     )
     &&
     (
         (RRC_CSP_GET_INT_MCC(plmn_id) == JAPAN_MCC1) 
      && (RRC_CSP_GET_INT_MNC(plmn_id) == MNC1)      
     )
    )
  {
    WRRC_MSG0_HIGH("UEUT inside the check");

    if(   (sim_read_status == GSDI_SUCCESS)
       && (buffer[0] == 4)
      )
    {
      WRRC_MSG0_HIGH("DCM UEUT: ALL conditions met. Stay on the cell");
      return (TRUE);      
    }
  }

  WRRC_MSG0_HIGH("UEUT: Cond NOT meet. Return FALSE");
  return (FALSE);
}

#else

boolean rrc_csp_ignore_barred_rsv_status_due_to_ueut(rrc_plmn_identity_type plmn_id)
{
  rrc_plmn_identity_type hplmn_id;
  
  if(rrc_ignore_cell_bar_nv == FALSE)
  {
    WRRC_MSG1_HIGH(" Ignore Cell Bar Staus is %d so return",rrc_ignore_cell_bar_nv);
    return FALSE;
  }
  
#ifdef FEATURE_DUAL_SIM
  hplmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(
                            reg_sim_per_subs_hplmn_get(rrc_get_as_id()));
#else
  hplmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(reg_sim_hplmn_get());
#endif

  if(rrc_rcvd_admin_data != 4)
  {
    WRRC_MSG1_HIGH("Received admin data in SIM Insterted Req is %d",rrc_rcvd_admin_data);
    return FALSE;
  }

  WRRC_MSG2_HIGH("HPLMN MCC-MNC: %d-%d", RRC_CSP_GET_INT_MCC(hplmn_id), RRC_CSP_GET_INT_MNC(hplmn_id));
  WRRC_MSG2_HIGH("PLMN  MCC-MNC: %d-%d", RRC_CSP_GET_INT_MCC(plmn_id), RRC_CSP_GET_INT_MNC(plmn_id));

  if(
     (
         (RRC_CSP_GET_INT_MCC(hplmn_id) == JAPAN_MCC1) 
      && (RRC_CSP_GET_INT_MNC(hplmn_id) == MNC1)      
     )
     &&
     (
         (RRC_CSP_GET_INT_MCC(plmn_id) == JAPAN_MCC1) 
      && (RRC_CSP_GET_INT_MNC(plmn_id) == MNC1)      
     )
    )
  {
    WRRC_MSG0_HIGH("UEUT inside the check");
    WRRC_MSG0_HIGH("DCM UEUT: ALL conditions met. Stay on the cell");
    return (TRUE);      
  }

  WRRC_MSG0_HIGH("UEUT: Cond NOT meet. Return FALSE");
  return (FALSE);
}

#endif


#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
/*===========================================================================

FUNCTION: RRC_CSP_SEND_SVC_STATUS_TO_MM

DESCRIPTION:

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rrc_csp_send_svc_status_to_mm(sys_srv_status_e_type svc_st, 
                                   sys_radio_access_tech_e_type rat, 
                                   sys_signal_strength_type sig_st )
{
#ifndef FEATURE_WRLF_SYSTEM_SEL

    mm_cmd_type *mm_buf_ptr;

    if((rrc_csp_int_data.oos_due_to_no_resource == TRUE)&&(svc_st == SYS_SRV_STATUS_NO_SRV))
    {
       return;
    }
    /* Get a buffer from MM task */
    mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_CONNECTED_MODE_SERVICE_STATUS_IND);

#ifdef FEATURE_DUAL_SIM
    mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.as_id = rrc_get_as_id();
#endif

    mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.service_status = svc_st;
    mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.selected_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;
#ifdef  FEATURE_OOSC_USER_ACTION
    mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.is_connected_mode_oos = TRUE;
    if(svc_st == SYS_SRV_STATUS_NONE || svc_st == SYS_SRV_STATUS_SRV)
    {
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.is_connected_mode_oos = FALSE;
    }
    if(svc_st == SYS_SRV_STATUS_NONE)
    {
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.service_status = SYS_SRV_STATUS_NO_SRV;
    }
    WRRC_MSG1_HIGH("OOSC:Connected mode OOS is sent as %d to MM/ Dispatching out MM cmd RRC_CONNECTED_MODE_SERVICE_STATUS_IND",
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.is_connected_mode_oos);
#endif /*FEATURE_OOSC_USER_ACTION*/

    if(mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.service_status == SYS_SRV_STATUS_NO_SRV)
    {
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.selected_plmn.plmn_id.mcc[0] = 0x0F;
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.selected_plmn.plmn_id.mcc[1] = 0x0F;
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.selected_plmn.plmn_id.mcc[2] = 0x0F;
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.selected_plmn.plmn_id.mnc[0] = 0x0F;
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.selected_plmn.plmn_id.mnc[1] = 0x0F;
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.selected_plmn.plmn_id.mnc[2] = 0x0F;
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.selected_plmn.plmn_id.num_mnc_digits = 3;
    }
    else
    {
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.selected_plmn.plmn_id = 
                                                        rrc_csp_int_data.curr_acq_entry.plmn_id;
    }
  
    mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.rat                   = rat; 

    mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.signal_strength       = sig_st;
    if(sig_st <= 95)
    {
      WRRC_MSG1_HIGH("signal strength (%d) >= -95db is considered as SYS_SIGNAL_QUALITY_HIGH",sig_st);
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.signal_quality = SYS_SIGNAL_QUALITY_HIGH;
    }
    else
    {
      WRRC_MSG1_HIGH("signal strength (%d) < -95db is considered as SYS_SIGNAL_QUALITY_LOW",sig_st);
      mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.signal_quality = SYS_SIGNAL_QUALITY_LOW;
    }


    MSG_HIGH("SVC_ST: Svc st %d, plmn mcc %d, mnc %d",mm_buf_ptr->cmd.rrc_conn_mode_service_status_ind.service_status,
                                           RRC_CSP_GET_INT_MCC(rrc_csp_int_data.curr_acq_entry.plmn_id), 
                                           RRC_CSP_GET_INT_MNC(rrc_csp_int_data.curr_acq_entry.plmn_id));

    rrc_send_rrc_message_to_mm(mm_buf_ptr);

#else
  /* With WRLF feature, NAS will take care of updating UI during connected mode OOS */
  return;

#endif

} /* rrc_csp_send_service_status_to_cm */
#endif /* FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE */

/*===========================================================================

FUNCTION          RRCCSP_IS_BAND_ALREADY_ADDED_TO_PRIORITIZED_LIST

DESCRIPTION       Determines whether a band given is already present in the prioritized band list.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
boolean 
rrccsp_is_band_already_added_to_prioritized_list(rrc_csp_band_class_type freq_band, uint8 num_of_bands)
{
  uint8 i;

  for(i = 0; i < num_of_bands; i++)
  {
    if(freq_band == rrc_csp_curr_select_data.prioritized_bands[i])
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*===========================================================================

FUNCTION          RRCCSP_GET_SERVING_FREQUENCY

DESCRIPTION       This function gets the frequency of the serving cell 

DEPENDENCIES
                  None.

RETURN VALUE
                  Serving cell's frequency.

SIDE EFFECTS      None.
===========================================================================*/
rrc_freq_type rrccsp_get_serving_frequency(void)
{
  return rrc_csp_int_data.curr_acq_entry.freq;
}

/*===========================================================================

FUNCTION          RRCCSP_GET_NEIGHBOR_FREQUENCY

DESCRIPTION       This function gets the frequency of the neighbor cell UE is
                  reselecting to
DEPENDENCIES
                  None.

RETURN VALUE
                  Neighbor frequency.

SIDE EFFECTS      None.
===========================================================================*/
rrc_freq_type rrccsp_get_neighbor_frequency(void)
{
  return rrc_csp_int_data.new_cell_acq_entry.freq;
}

/*===========================================================================

FUNCTION          RRCCSP_GET_CURR_NAS_REQUESTED_PLMN

DESCRIPTION       This function returns the specific PLMN on which NAS has 
                  requested RRC to camp.
DEPENDENCIES
                  None.

RETURN VALUE
                  NAS requested PLMN.

SIDE EFFECTS      None.
===========================================================================*/
rrc_plmn_identity_type rrccsp_get_curr_nas_requested_plmn(void)
{
  if(WTOW_SEARCH_RESUME == rrc_csp_bplmn_srch_in_progress())
  {
    return rrc_csp_int_data.prev_curr_req_plmn.plmn_id;
  }
  else
  {
    return rrc_csp_int_data.curr_req_plmn.plmn_id;
  }
}

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION          RRCCSP_GET_CURR_NAS_REQUESTED_PLMN

DESCRIPTION       This function returns the specific PLMN on which NAS has 
                  requested RRC to camp.
DEPENDENCIES
                  None.

RETURN VALUE
                  NAS requested PLMN.

SIDE EFFECTS      None.
===========================================================================*/
rrc_int_csg_id_type rrccsp_get_curr_nas_requested_csg_id(void)
{
  return rrc_csp_int_data.curr_req_plmn.csg_id;
}
#endif

#ifdef FEATURE_OOSC_USER_ACTION
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
/*===========================================================================

FUNCTION        RRCCSP_CHECK_AND_SEND_UOOSC_END_IND

DESCRIPTION     This function is called when RRC receives STOP WCDMA request from NAS
                This function checks if UOOSC search was in progress and sends 
                RRC_CONNECTED_MODE_SERVICE_STATUS_IND with is_connected_mode_oos as FALSE

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrccsp_check_and_send_uoosc_end_ind
(
  void
)
{
  rrc_state_e_type curr_rrc_state = rrc_get_state();
  if((rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN) &&
    ((curr_rrc_state == RRC_STATE_CELL_DCH)||
     (curr_rrc_state == RRC_STATE_CELL_FACH)||
     (curr_rrc_state == RRC_STATE_CELL_PCH)||
     (curr_rrc_state == RRC_STATE_URA_PCH)))
  {
    /*UOOSC scan was in progress, Send end of UOOSC*/
    rrc_csp_send_svc_status_to_mm(SYS_SRV_STATUS_NONE, SYS_RAT_UMTS_RADIO_ACCESS, 0);
  }
}
#endif
#endif

/*===========================================================================

FUNCTION          RRC_CSP_CELL_TRANS_CNF_RCVD_AT_LLC

DESCRIPTION       This function is called by LLC on reception of cell_trans_cnf from L1

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_cell_trans_cnf_rcvd_at_llc
(
  void
)
{
  WRRC_MSG0_HIGH("LLC received Cell transition CNF");
  rrc_csp_int_data.cel_trans_cnf_recvd_at_llc = TRUE;
}

/*===========================================================================

FUNCTION          RRCCSP_CHECK_INITIATE_CELL_SELECTION_HANDLING

DESCRIPTION       This function is called to check if CSP is in proper substate to handle 
                         RRC_INITIATE_CELL_SELECTION_IND to transition to DCH or FACH.

DEPENDENCIES      None

RETURN VALUE      TRUE/FALSE

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_check_initiate_cell_selection_handling
(
  rrcrb_directed_cell_info_type *directed_cell_info
)
{
  rrc_csp_substate_e_type  curr_substate = rrc_get_csp_substate();
  if((curr_substate == RRCCSP_CAMPED_SUIT_CELL)
    || (curr_substate == RRCCSP_CAMPED_ACCEPT_CELL))
  {
    return TRUE;
  }

  if (NULL != directed_cell_info)
  {
    if(!((rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) 
     || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) 
     || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN)
#ifdef FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
     || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN)
     || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN)
     || (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN)
#endif
     ))
    {
      if (directed_cell_info->freq_info_present)
      {
         rrc_csp_int_data.dest_freq_present = directed_cell_info->freq_info_present;
         rrc_csp_int_data.dest_freq         = directed_cell_info->uarfcn_dl;
      }
      if(directed_cell_info->pscr_present)
      {
         rrc_csp_int_data.dest_psc_present = directed_cell_info->pscr_present;
         rrc_csp_int_data.dest_psc         = directed_cell_info->pscr;
      }
	    MSG_HIGH("Redirection info frequency %d ,PSC present %d and PSC is %d ",rrc_csp_int_data.dest_freq,
                                  rrc_csp_int_data.dest_psc_present,rrc_csp_int_data.dest_psc);
    }
    else
    {
      WRRC_MSG1_ERROR("Redirection cell info is received while the Current CSP scan is %d",rrc_csp_int_data.curr_scan);
    }
  }
  WRRC_MSG1_HIGH("CSP not in a state to accept cell selection req csp state %d ",curr_substate);
  return FALSE;
}

/*===========================================================================

FUNCTION          RRC_CSP_IS_RESELECTION_CLEARED

DESCRIPTION       This function is called on reception of Chan config req for N-BCCH 
               to check if DL WEAK IND was received for this cell and reselection data has been cleared.

DEPENDENCIES      None

RETURN VALUE      TRUE : if reselection is cleared.
                  FLASE: otherwise

SIDE EFFECTS      None
===========================================================================*/
boolean rrc_csp_is_reselection_cleared
(
  void
)
{
  boolean resel_cleared = FALSE;
  rrc_csp_substate_e_type curr_csp_substate = rrc_get_csp_substate();

  if((curr_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_TRANS_CNF) ||
     (curr_csp_substate == RRCCSP_SUIT_CELL_RESELECT_WT_TRANS_CNF))
  {
    WRRC_MSG0_HIGH("Reselection cancelled and CSP is waiting for transition CNF");
    resel_cleared = TRUE;
  }
  return resel_cleared;
}

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/*===========================================================================

FUNCTION          RRCCSP_CHECK_IF_CAMPED_NORMALLY

DESCRIPTION       This function checks if UE is camped normally, 

DEPENDENCIES      None

RETURN VALUE      TRUE if camped normally
                          FALSE if camped on any cell

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_check_if_camped_normally
(
  void
)
{
  boolean camped_normally = FALSE;
  if(rrc_csp_bplmn_srch_in_progress() == WTOW_SEARCH_RESUME)
  {
    if(rrc_csp_int_data.prev_curr_req_plmn.plmn_type == RRC_GSM_MAP_SPECIFIC_PLMN_REQ)
    {
      camped_normally = TRUE;
    }
  }
  else
  {
    if(rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_SPECIFIC_PLMN_REQ)
    {
      camped_normally = TRUE;
    }
  }
  return (camped_normally);
}
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST
/*===========================================================================

FUNCTION          RRC_EXTRACT_GW_BAND_PREF_FROM_RAT_PRI_LIST

DESCRIPTION       This function extracts the GW band pref from RAT PRI List

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_type rrc_extract_gw_band_pref_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list
)
{
  uint8 rat_idx = 0;
  sys_band_mask_type gw_band_pref = 0;
  for(rat_idx =0;rat_idx < rat_pri_list->num_items &&
               rat_idx< SYS_MAX_ACQ_PREF_RAT_LIST_NUM ; rat_idx ++)
  {
    if((rat_pri_list->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_WCDMA)
       || (rat_pri_list->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_GSM))
    {
#ifdef LTE_BAND_NUM
      gw_band_pref |= rat_pri_list->priority_list_info[rat_idx].band_cap.chgwt_band_cap;
#else
      gw_band_pref |= rat_pri_list->priority_list_info[rat_idx].band_cap;
#endif
    }
  }
  return gw_band_pref;
}

/*===========================================================================

FUNCTION          RRC_EXTRACT_GW_BAND_PREF_FROM_SERVICE_REQ

DESCRIPTION       This function extracts the GW band pref from RAT PRI List of service_req

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_type rrc_extract_gw_band_pref_from_service_req
(
  sys_rat_pri_list_info_s_type *rat_pri_list
)
{
  uint8 rat_idx = 0;
  sys_band_mask_type w_band_pref, g_band_pref,wcdma_complete_band_mask;
  w_band_pref = g_band_pref = 0;

  wcdma_complete_band_mask = rrccsp_get_sys_wcdma_band_mask();

  for(rat_idx =0;rat_idx < rat_pri_list->num_items &&
               rat_idx< SYS_MAX_ACQ_PREF_RAT_LIST_NUM ; rat_idx ++)
  {
    if(rat_pri_list->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_WCDMA)
    {
      if(!rat_pri_list->priority_list_info[rat_idx].bst_rat_acq_required)
      {
        WRRC_MSG0_ERROR("BST: WCDMA RAT ACQ is indicated as not required ");
      }
#ifdef LTE_BAND_NUM
      w_band_pref = rat_pri_list->priority_list_info[rat_idx].bst_band_cap.chgwt_band_cap;
#else
      w_band_pref = rat_pri_list->priority_list_info[rat_idx].bst_band_cap;
#endif
    }
    if((rat_pri_list->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_GSM) &&
        (rat_pri_list->priority_list_info[rat_idx].bst_rat_acq_required))
    {
#ifdef LTE_BAND_NUM
      g_band_pref = rat_pri_list->priority_list_info[rat_idx].bst_band_cap.chgwt_band_cap;
#else
      g_band_pref = rat_pri_list->priority_list_info[rat_idx].bst_band_cap;
#endif
    }
  }

  w_band_pref &= wcdma_complete_band_mask;
  g_band_pref &= ~wcdma_complete_band_mask;

  return (w_band_pref | g_band_pref);
}

/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_GW_BAND_PREF_AFTER_SUCC_CAMPING

DESCRIPTION       This function updates mode and band preference after successful camping
                         This is called by CCM on successful cell change

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_update_mode_band_pref_after_succ_camping
(
  void
)
{
    WRRC_MSG0_HIGH("BST: Update mode and band pref from non BST");
    rrc_csp_int_data.band_pref           = rrc_extract_gw_band_pref_from_rat_pri_list(&rrc_csp_int_data.rat_pri_list_info);
    rrccsp_update_band_preference();
    rr_rrc_set_band_pref_internal(rrc_csp_int_data.band_pref);
    rrcmcm_update_mode_capability(&rrc_csp_int_data.rat_pri_list_info);
}

/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_RAT_PRI_LIST_INFO

DESCRIPTION       This function updates rat_pri_list_info in CSP INT DATA

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_update_rat_pri_list_info
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info
)
{
  WRRC_MSG0_HIGH("Updating the RAT PRI LIST info");
  rrc_csp_int_data.rat_pri_list_info = *rat_pri_list_info;
  rrcmcm_update_mode_capability(&rrc_csp_int_data.rat_pri_list_info);
  rrc_csp_int_data.band_pref = rrc_extract_gw_band_pref_from_rat_pri_list(&rrc_csp_int_data.rat_pri_list_info);
  rrccsp_update_band_preference();
}
/*===========================================================================

FUNCTION          RRC_CSP_GET_RAT_PRI_LIST_INFO

DESCRIPTION       This function fills the arguments with rat_pri_list_info in CSP INT DATA

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_get_rat_pri_list_info
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info
)
{
  *rat_pri_list_info = rrc_csp_int_data.rat_pri_list_info;
}
#endif


#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION          RRC_EXTRACT_LTE_BAND_PREF_FROM_RAT_PRI_LIST

DESCRIPTION       This function extracts the LTE band pref from RAT PRI List

DEPENDENCIES      None

RETURN VALUE      LTE band pref

SIDE EFFECTS      None
===========================================================================*/
#ifdef LTE_BAND_NUM
sys_lte_band_mask_e_type rrc_extract_lte_band_pref_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list
)
{
  uint8 rat_idx = 0;
  sys_lte_band_mask_e_type lte_band_pref;
  memset(&lte_band_pref,0,sizeof(sys_lte_band_mask_e_type));
  for(rat_idx =0;rat_idx < rat_pri_list->num_items &&
               rat_idx< SYS_MAX_ACQ_PREF_RAT_LIST_NUM ; rat_idx ++)
  {
    if(rat_pri_list->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      SYS_LTE_BAND_MASK_ADD_MASK(&lte_band_pref,&(rat_pri_list->priority_list_info[rat_idx].band_cap.lte_band_cap));
      break;
    }
  }
  return lte_band_pref;
}
#else
sys_band_mask_type rrc_extract_lte_band_pref_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list
)
{
  uint8 rat_idx = 0;
  sys_band_mask_type lte_band_pref = 0;
  for(rat_idx =0;rat_idx < rat_pri_list->num_items &&
               rat_idx< SYS_MAX_ACQ_PREF_RAT_LIST_NUM ; rat_idx ++)
  {
    if(rat_pri_list->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      lte_band_pref |= rat_pri_list->priority_list_info[rat_idx].band_cap;
      break;
    }
  }
  return lte_band_pref;
}
#endif


/*===========================================================================

FUNCTION          RRCCSP_UPDATE_BPLMN_LTE_INFO_FROM_RAT_PRI_LIST

DESCRIPTION       This function updates the LTE BPLMN variables in rrc_csp_int_data from rat pri list

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_update_bplmn_lte_info_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list
)
{
  rrc_csp_int_data.bplmn_lte_mode_enabled = rrcmcm_is_mode_enabled_in_rat_pri_list(rat_pri_list,SYS_SYS_MODE_LTE);
  rrc_csp_int_data.bplmn_lte_band_pref = rrc_extract_lte_band_pref_from_rat_pri_list(rat_pri_list);
}
#ifdef LTE_BAND_NUM
/*===========================================================================

FUNCTION          rrccsp_check_if_lte_band_pref_is_set

DESCRIPTION       This function checks if LTE band pref is set

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
boolean rrccsp_check_if_lte_band_pref_is_set(sys_lte_band_mask_e_type lte_band_mask)
{
   if(!SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_band_mask))
   {
     return TRUE;
   }
   return FALSE;
}
#endif

/*===========================================================================

FUNCTION          rrccsp_get_lte_band_pref

DESCRIPTION       This function returns LTE band preference

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
#ifdef LTE_BAND_NUM
sys_lte_band_mask_e_type rrccsp_get_lte_band_pref(void)
#else
sys_band_mask_type rrccsp_get_lte_band_pref(void)
#endif

{
  return (rrc_extract_lte_band_pref_from_rat_pri_list(&rrc_csp_int_data.rat_pri_list_info));
}

/*===========================================================================

FUNCTION          rrccsp_get_wg_band_pref

DESCRIPTION       This function populates WCDMA & GSM band preference

DEPENDENCIES
                  None.

RETURN VALUE
                  None

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_get_wg_band_pref
(
  sys_band_mask_type *wcdma_band_pref,
  sys_band_mask_type *gsm_band_pref
)
{
  sys_band_mask_type wcdma_complete_band_mask = 0;
  sys_band_mask_type gw_band_pref = rrc_extract_gw_band_pref_from_rat_pri_list(&rrc_csp_int_data.rat_pri_list_info);

  wcdma_complete_band_mask = rrccsp_get_sys_wcdma_band_mask();

  *wcdma_band_pref = gw_band_pref & wcdma_complete_band_mask ;
  *gsm_band_pref = gw_band_pref & ~wcdma_complete_band_mask ;
}

#endif

/*===========================================================================

FUNCTION          rrc_csp_add_frequency_to_detected_cell_list

DESCRIPTION       This function adds WCDMA frequency to detected frequency list

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrc_csp_add_frequency_to_detected_cell_list
(
  detected_frequency_type *detected_frequency
)
{
  boolean frequency_found = FALSE;
  uint8 idx = 0;
  for(idx =0;idx<rrc_csp_int_data.detected_frequency_list.num_freqs && idx < LTE_IRAT_MAX_DETECTED_FREQ; idx++)
  {
    if((rrc_csp_int_data.detected_frequency_list.freq[idx].rat == detected_frequency->rat) &&
        (rrc_csp_int_data.detected_frequency_list.freq[idx].frequency == detected_frequency->frequency))
    {
      frequency_found = TRUE;
      break;
    }
  }
  if((!frequency_found) && (rrc_csp_int_data.detected_frequency_list.num_freqs < LTE_IRAT_MAX_DETECTED_FREQ))
  {
    idx= rrc_csp_int_data.detected_frequency_list.num_freqs;
    rrc_csp_int_data.detected_frequency_list.freq[idx] = *detected_frequency;
    rrc_csp_int_data.detected_frequency_list.num_freqs++;
    WRRC_MSG2_HIGH("Added frequency %d @ idx %d",detected_frequency->frequency,idx);
  }
}

/*===========================================================================

FUNCTION          RRCCSP_UPDATE_IRAT_DETECTED_FREQUENCIES

DESCRIPTION       This function updates the detected frequencies in rrc_csp_int_data with detected frequency list
                          received from other RAT

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_update_irat_detected_frequencies
(
  detected_frequency_list_type* detected_frequency_list
)
{
  uint16 idx=0;
  for(idx =0 ;idx < detected_frequency_list->num_freqs;idx++)
  {
    rrc_csp_add_frequency_to_detected_cell_list(&detected_frequency_list->freq[idx]);
  }
  return;
}

#endif

/*===========================================================================

FUNCTION          RRCCSP_GENERATE_RAT_ORDER_FOR_AUTO_PLMN_SRCH

DESCRIPTION       This function generates the RAT search order for Automatic PLMN search

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_generate_rat_order_for_auto_plmn_srch
(
  void
)
{
  uint16 rat_idx;
  boolean wcdma_rat_added, gsm_rat_added;

#if defined FEATURE_WCDMA_TO_LTE 
  boolean lte_rat_added = FALSE;
#endif
  rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx = -1;
  rrc_csp_int_data.plmn_srch_order.num_rats = 0;
  wcdma_rat_added = gsm_rat_added = FALSE;

  for(rat_idx=0;rat_idx< rrc_csp_int_data.bplmn_num_req &&rat_idx < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH  &&
               rrc_csp_int_data.plmn_srch_order.num_rats < RRC_CSP_MAX_PLMN_SRCH_RATS; rat_idx++)
  {
    if((rrc_csp_int_data.bplmn_rat_req_list[rat_idx] == SYS_RAT_UMTS_RADIO_ACCESS) && !wcdma_rat_added)
    {
      rrc_csp_int_data.plmn_srch_order.rat[rrc_csp_int_data.plmn_srch_order.num_rats++] = SYS_RAT_UMTS_RADIO_ACCESS;
      wcdma_rat_added = TRUE;
    }
    else if((rrc_csp_int_data.bplmn_rat_req_list[rat_idx] == SYS_RAT_GSM_RADIO_ACCESS) 
               && rrc_csp_int_data.bplmn_dual_mode_enabled && !gsm_rat_added)
    {
      rrc_csp_int_data.plmn_srch_order.rat[rrc_csp_int_data.plmn_srch_order.num_rats++] = SYS_RAT_GSM_RADIO_ACCESS;
      gsm_rat_added = TRUE;
    }
#if defined FEATURE_WCDMA_TO_LTE
    else if((rrc_csp_int_data.bplmn_rat_req_list[rat_idx] == SYS_RAT_LTE_RADIO_ACCESS) 
               && rrc_csp_int_data.bplmn_lte_mode_enabled && !lte_rat_added)
    {
      rrc_csp_int_data.plmn_srch_order.rat[rrc_csp_int_data.plmn_srch_order.num_rats++] = SYS_RAT_LTE_RADIO_ACCESS;
      lte_rat_added = TRUE;
    }
#endif
  }
  rrc_csp_print_rat_search_order();
}

/*===========================================================================

FUNCTION          RRCCSP_GENERATE_RAT_ORDER_FOR_MANUAL_PLMN_SRCH

DESCRIPTION       This function generates the RAT search order for Manual PLMN search

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_generate_rat_order_for_manual_plmn_srch
(
  void
)
{
  boolean do_gsm_bands_scan_first = FALSE;
  uint8 num_rats;
  rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx = -1;
  num_rats = rrc_csp_int_data.plmn_srch_order.num_rats = 0;

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  do_gsm_bands_scan_first = rrccsp_do_gsm_bands_scan_first();
#endif

  if(rrc_csp_int_data.bplmn_dual_mode_enabled && do_gsm_bands_scan_first)
  {
    rrc_csp_int_data.plmn_srch_order.rat[num_rats] = SYS_RAT_GSM_RADIO_ACCESS;
    num_rats ++;
#if defined FEATURE_WCDMA_TO_LTE
    if(rrc_csp_int_data.bplmn_lte_mode_enabled)
    {
      rrc_csp_int_data.plmn_srch_order.rat[num_rats] = SYS_RAT_LTE_RADIO_ACCESS;
      num_rats ++;
    }
#endif
    if(rrc_csp_int_data.bplmn_wcdma_mode_enabled)
    {
      rrc_csp_int_data.plmn_srch_order.rat[num_rats] = SYS_RAT_UMTS_RADIO_ACCESS;
      num_rats ++;
    }
  }

  else if(rrc_csp_int_data.bplmn_dual_mode_enabled)
  {
#if defined FEATURE_WCDMA_TO_LTE
    if(rrc_csp_int_data.bplmn_lte_mode_enabled)
    {
      rrc_csp_int_data.plmn_srch_order.rat[num_rats] = SYS_RAT_LTE_RADIO_ACCESS;
      num_rats ++;
    }
#endif
    if(rrc_csp_int_data.bplmn_wcdma_mode_enabled)
    {
      rrc_csp_int_data.plmn_srch_order.rat[num_rats] = SYS_RAT_UMTS_RADIO_ACCESS;
      num_rats ++;
    }
    rrc_csp_int_data.plmn_srch_order.rat[num_rats] = SYS_RAT_GSM_RADIO_ACCESS;
    num_rats ++;
  }
  else
  {
    if(rrc_csp_int_data.bplmn_wcdma_mode_enabled)
    {
      rrc_csp_int_data.plmn_srch_order.rat[num_rats] = SYS_RAT_UMTS_RADIO_ACCESS;
      num_rats ++;
    }
#if defined FEATURE_WCDMA_TO_LTE
    if(rrc_csp_int_data.bplmn_lte_mode_enabled)
    {
      rrc_csp_int_data.plmn_srch_order.rat[num_rats] = SYS_RAT_LTE_RADIO_ACCESS;
      num_rats ++;
    }
#endif
  }

  rrc_csp_int_data.plmn_srch_order.num_rats = num_rats;

  rrc_csp_print_rat_search_order();

}

/*===========================================================================

FUNCTION          RRCCSP_GENERATE_BPLMN_RAT_SRCH_ORDER_UPDATE_CURR_SRCH_RAT

DESCRIPTION       This function generates the RAT search order for either Manual of Automatic BPLMN search
                         based on n/W sel mode passeds
                         After generation of RAT order for search, will update the curr srch idx to first RAT

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_generate_bplmn_rat_srch_order_update_curr_srch_rat
(
  sys_network_selection_mode_e_type nw_sel_mode
)
{
  if(nw_sel_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
  {
     rrccsp_generate_rat_order_for_manual_plmn_srch();
  }
  else
  {
     rrccsp_generate_rat_order_for_auto_plmn_srch();
  }
  if(rrc_csp_int_data.plmn_srch_order.num_rats !=0)
  {
    rrc_csp_int_data.plmn_srch_order.curr_rat_srch_idx = 0;
    if(rrc_csp_int_data.plmn_srch_order.rat[0] == SYS_RAT_UMTS_RADIO_ACCESS)
    {
      WRRC_MSG0_HIGH("Do WTOW search first");
      rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active = FALSE;
    }
#ifdef FEATURE_WCDMA_TO_LTE
    else if(rrc_csp_int_data.plmn_srch_order.rat[0] == SYS_RAT_LTE_RADIO_ACCESS)
    {
      WRRC_MSG0_HIGH("Do WTOL search first");
      rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active = TRUE;
      rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_LTE;
    }
#endif
    else
    {
      WRRC_MSG0_HIGH("Do WTOG search first");
      rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active = TRUE;
#ifdef FEATURE_WCDMA_TO_LTE
      rrc_csp_int_data.bplmn_interrat_type = RRC_CSP_INTERRAT_GSM;
#endif
    }
  }
}
/*===========================================================================

FUNCTION          RRC_CSP_PRINT_RAT_SEARCH_ORDER

DESCRIPTION       This function prints the RAT search order for PLMN_LIST_REQ

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_print_rat_search_order
(
  void
)
{
  uint8 rat_idx;
  WRRC_MSG0_HIGH("RAT Search order for PLMN List Req");
  for(rat_idx =0; rat_idx< rrc_csp_int_data.plmn_srch_order.num_rats && rat_idx < RRC_CSP_MAX_PLMN_SRCH_RATS; rat_idx++)
  {
    if(rrc_csp_int_data.plmn_srch_order.rat[rat_idx] == SYS_RAT_GSM_RADIO_ACCESS)
    {
      WRRC_MSG1_HIGH(">%d   ---GSM RAT------",rat_idx);
    }
    else if (rrc_csp_int_data.plmn_srch_order.rat[rat_idx] == SYS_RAT_UMTS_RADIO_ACCESS)
    {
      WRRC_MSG1_HIGH(">%d   ---UMTS RAT------",rat_idx);
    }
#ifdef FEATURE_WCDMA_TO_LTE
    else if (rrc_csp_int_data.plmn_srch_order.rat[rat_idx] == SYS_RAT_LTE_RADIO_ACCESS)
    {
      WRRC_MSG1_HIGH(">%d   ---LTE RAT------",rat_idx);
    }
#endif
  }
}


/*===========================================================================

FUNCTION          RRC_CSP_INIT_WTOG_PLMN_SRCH_INT_DATA_ACROSS_RATS

DESCRIPTION       This function is responsible for initializing the internal data structure to default values for WTOInterrat search
                         without initalzing num of plmns

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void  rrc_csp_init_wtog_plmn_srch_int_data_across_rats
(
  void
)
{
  rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_aborted = FALSE;
  rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active  = FALSE;
  rrc_csp_wtog_bplmn_int_data.wtog_hplmn_found        = FALSE;
  rrc_csp_wtog_bplmn_int_data.wtog_rem_drx_tmr_value  = 0;
  rrc_csp_wtog_bplmn_int_data.wtog_srch_done          = FALSE;
  rrc_csp_wtog_bplmn_int_data.restart_bplmn_search    = FALSE;

  rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_suspended = FALSE;
}



/*===========================================================================

FUNCTION          WCDMA_RRC_GET_BAND_FROM_DL_ARFCN

DESCRIPTION       This function is returns W band corresponding to DL UARFCN and PLMN ID

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
#ifdef FEATURE_LTE_TO_WCDMA
sys_band_mask_type wcdma_rrc_get_band_from_dl_uarfcn
(
  uint16 uarfcn,
  sys_plmn_id_s_type camped_plmn_id,
  boolean *is_band_valid
)
{
  rrc_csp_band_class_type csp_band_class = 
    rrc_get_frequency_band(
      (rrc_freq_type)uarfcn,
      mmumts_convert_nas_plmn_id_to_rrc_plmn_id(camped_plmn_id)
    );

  if(RRC_CSP_BAND_MAX != csp_band_class )
  {
    *is_band_valid = TRUE;
    return (rrc_convert_band_to_sys_band(csp_band_class));
  }

  /*Return any value as this is not going to be used*/
  *is_band_valid = FALSE;
  return SYS_BAND_MASK_WCDMA_I_IMT_2000;
}
#endif
/*===========================================================================

FUNCTION          WCDMA_CONVERT_UARFCN

DESCRIPTION       This function converts the UARFCN to a UARFCN corresponding
                  to the given band.

DEPENDENCIES
                  None

RETURN VALUE
                  TRUE if the conversion is succesful
                  FALSE if the conversion is not succesful

SIDE EFFECTS
                  None
===========================================================================*/
boolean wcdma_convert_uarfcn
(
  uint16 source_uarfcn,
  sys_sband_e_type band_num,
  uint16 *target_uarfcn,
  sys_plmn_id_s_type camped_plmn_id
)
{
  rrc_csp_band_class_type source_band_class = rrc_get_frequency_band((rrc_freq_type)source_uarfcn,
                                              mmumts_convert_nas_plmn_id_to_rrc_plmn_id(camped_plmn_id));

  rrc_csp_band_class_type target_band_class = RRC_CSP_BAND_MAX;
  
  if((band_num) == SYS_SBAND_WCDMA_I_IMT_2000)
  {
    target_band_class = RRC_CSP_IMT2K_BAND;
  }
  else if((band_num) == SYS_SBAND_WCDMA_II_PCS_1900)
  {
    target_band_class = RRC_CSP_PCS_BAND;
  }
  else if((band_num) == SYS_SBAND_WCDMA_V_850)
  {
    target_band_class = RRC_CSP_WCDMA_850_BAND;
  }
  else if((band_num) == SYS_SBAND_WCDMA_VI_800)
  {
    target_band_class = RRC_CSP_WCDMA_800_BAND;
  }
  else if((band_num) == SYS_SBAND_WCDMA_III_1700)
  {
    target_band_class = RRC_CSP_WCDMA_1800_BAND;
  }
  else if((band_num) == SYS_SBAND_WCDMA_VIII_900)
  {
    target_band_class = RRC_CSP_WCDMA_900_BAND;
  }
  else if((band_num) == SYS_SBAND_WCDMA_IX_1700)
  {
    target_band_class = RRC_CSP_WCDMA_1700_BAND;
  }
  else if((band_num) == SYS_SBAND_WCDMA_IV_1700)
  {
    target_band_class = RRC_CSP_WCDMA_1700_2100_BAND;
  }
  else if((band_num)== SYS_SBAND_WCDMA_XI_1500)
  {
    target_band_class = RRC_CSP_WCDMA_1500_BAND;
  }
  else if((band_num) == SYS_SBAND_WCDMA_XIX_850)
  {
    target_band_class = RRC_CSP_WCDMA_BC19_BAND;
  }
  else 
  {
    WRRC_MSG1_ERROR("band %d not supported", band_num);
    return FALSE;
  }
  if(rrccsp_get_uarfcn(source_uarfcn, source_band_class, target_band_class, target_uarfcn) == TRUE)
  {
    return TRUE;
  } 
  return FALSE; 
}
/*===========================================================================

FUNCTION          WCDMA_DERIVE_FREQ_FROM_UARFCN

DESCRIPTION       This function converts the given UARFCN to absolute frequency

DEPENDENCIES
                  None

RETURN VALUE
                  E_SUCCESS if the conversion is succesful
                  E_FAILURE if the conversion is not succesful

SIDE EFFECTS
                  None
===========================================================================*/
errno_enum_type wcdma_derive_freq_from_uarfcn
(
  uint16 uarfcn,
  sys_plmn_id_s_type camped_plmn_id,
  uint16 *freq
)
{
  rrc_csp_band_class_type csp_band_class = rrc_get_frequency_band((rrc_freq_type)uarfcn,
                                              mmumts_convert_nas_plmn_id_to_rrc_plmn_id(camped_plmn_id));
  if(RRC_CSP_BAND_MAX != csp_band_class )
  {
    *freq = rrccsp_derive_freq_from_uarfcn(uarfcn,csp_band_class);
    return E_SUCCESS;
  }
  else
  {
    return E_FAILURE;
  }
}
#ifdef FEATURE_WRLF_SYSTEM_SEL

/*===========================================================================

FUNCTION          rrc_csp_is_suitable_srch_ended

DESCRIPTION       This function checks if duitable search has ended

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
boolean rrc_csp_is_suitable_srch_ended
(
  void
)
{
  boolean status;
  if(rrc_csp_int_data.oos_splmn_srch_tmr_expd)
  {
    status=TRUE;
  }
  else
  {
    status=FALSE; 
  }
  return status;
}

/*===========================================================================

FUNCTION          RRC_CSP_CHECK_N_SEND_SUITABLE_SRCH_END_IND

DESCRIPTION       This function checks and sends suitable_srch_end_ind
                  to NAS.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_check_n_send_suitable_srch_end_ind
(
  void
)
{
  mm_cmd_type *mm_buf_ptr;

  if(!rrc_csp_int_data.oos_splmn_srch_tmr_expd)
  {
    /* Get a buffer from MM task */
    mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_SUITABLE_SEARCH_END_IND);
#ifdef FEATURE_DUAL_SIM
    mm_buf_ptr->cmd.rrc_suitable_search_end_ind.as_id = rrc_get_as_id();
#endif
    /* Put the command on MM queue */
    WRRC_MSG0_HIGH("WRLF: Sending MM RRC_SUITABLE_SEARCH_END_IND / Dispatching out MM cmd");
    rrc_send_rrc_message_to_mm(mm_buf_ptr);
  }
}
/*===========================================================================

FUNCTION          RRC_CSP_SEND_DISABLE_BPLMN_IND

DESCRIPTION       This function sends disable_bplmn_ind to NAS.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_send_disable_bplmn_ind(boolean status)
{
  mm_cmd_type *mm_buf_ptr;

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_DISABLE_BPLMN_IND);
  mm_buf_ptr->cmd.rrc_disable_bplmn_ind.disable_status = status;
#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_disable_bplmn_ind.as_id = rrc_get_as_id();
#endif 
  /* Put the command on MM queue */
  WRRC_MSG0_HIGH("WRLF: Sending MM RRC_DISABLE_BPLMN_IND / Dispatching out MM cmd");
  rrc_send_rrc_message_to_mm(mm_buf_ptr);
}

#endif

#ifdef FEATURE_DUAL_SIM

/*===========================================================================

FUNCTION          rrc_csp_handle_bplmn_suspend_lock_rel_offset_tmr_running

DESCRIPTION       This function is called to put WL1 to sleep, stop the 10ms lock release offset timer and
                         stop BPLMN DRX timer
                         This function is called on handling BPLMN suspend and BPLMN cleanup

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_handle_bplmn_suspend_lock_rel_offset_tmr_running
(
  void
)
{
  if(0 != rrctmr_get_remaining_time(RRCTMR_DSIM_BPLMN_LOCK_REL_OFFSET_TIMER))
  {
    rrctmr_stop_timer(RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER);
    rrc_csp_int_data.bplmn_srch_tmr_expired = TRUE;
    rrc_csp_send_l1_cmd(RRC_PROCEDURE_CSP, CPHY_GO_TO_SLEEP_IND, 
                                     &rrc_csp_int_data.curr_acq_entry);
    rrctmr_stop_timer(RRCTMR_DSIM_BPLMN_LOCK_REL_OFFSET_TIMER);
  }
}

/*===========================================================================

FUNCTION          rrc_is_ps_abort_required

DESCRIPTION       This function is to be called by CM/SD to check if PS abort can be attemped on W
                         or G, for CS call on the other RAT.
                         1. In PCH/Connected Mode OOS states do not allow PS Abort
                         2. In FACH/DCH state allow PS abort.
                         3. If W is INACTIVE or PS domain in W is not active, then this API returns TRUE to 
                             allow PS abort on G.

DEPENDENCIES      None

RETURN VALUE      TRUE: To allow PS abort. and FALSE otherwise.

SIDE EFFECTS      None
===========================================================================*/
boolean rrc_is_ps_abort_required(void)
{
  rrc_state_e_type rrc_state = rrc_get_state();

  if((RRC_MODE_ACTIVE == rrcmcm_get_rrc_mode())
      && rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID)
      && ((rrc_state == RRC_STATE_CELL_PCH) || (rrc_state == RRC_STATE_URA_PCH) ||
            (rrc_csp_int_data.curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN))
    )
  {
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION          rrc_reset_oos_due_to_no_resource_flag

DESCRIPTION       This function is called to reset the flag which track OOS due to 
                          no resource.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_reset_oos_due_to_no_resource_flag(void)
{
  if(rrc_csp_int_data.oos_due_to_no_resource)
  {
    WRRC_MSG0_HIGH("DSIM: resetting flag oos_due_to_no_resource.");
    rrc_csp_int_data.oos_due_to_no_resource = FALSE;

    rrctmr_start_timer( RRCTMR_RSSI_BER_TIMER,
                       RRCTMR_RSSI_BER_REPORT_TIME );
  }
}

/*===========================================================================

FUNCTION          rrc_set_oos_due_to_no_resource_flag

DESCRIPTION       This function is called to set the flag which track OOS due to 
                          no resource.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_set_oos_due_to_no_resource_flag(void)
{
  WRRC_MSG0_HIGH("DSIM: setting flag oos_due_to_no_resource.");
  rrc_csp_int_data.oos_due_to_no_resource = TRUE;

  rrctmr_stop_timer(RRCTMR_RSSI_BER_TIMER);
}

/*===========================================================================

FUNCTION          rrc_get_oos_due_to_no_resource_flag_status

DESCRIPTION       This function is called to get the flag status which tracks OOS due to 
                          no resource.

DEPENDENCIES      None

RETURN VALUE      boolean

SIDE EFFECTS      None
===========================================================================*/
boolean rrc_get_oos_due_to_no_resource_flag_status(void)
{
  WRRC_MSG1_HIGH("DSIM: oos_due_to_no_resource.: %d", rrc_csp_int_data.oos_due_to_no_resource);
  return (rrc_csp_int_data.oos_due_to_no_resource);
}
#endif

/*===========================================================================

FUNCTION          rrccsp_check_sib1_validity

DESCRIPTION       This function calls NAS API to verify LAC info provided in SIB1

DEPENDENCIES      None

RETURN VALUE      TRUE: LAC info is valid.
                  FALSE: Otherwise. 

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_check_sib1_validity( rrc_SysInfoType1  *sib1)
{
  /* This stores the GSM-MAP NAS system info */
  rrc_gsm_map_nas_sys_info_type   nas_common_info;
  /* This stores the LAI received in SIB1 */
  rrc_lai_type  lai;

  unsigned char  *data_ptr;
  uint32         i;

  if( sib1 == NULL )
  {
    return FALSE;
  } /* SIB1 not present */

  /* The SIB1 is present */
  nas_common_info.length = (uint32)sib1->cn_CommonGSM_MAP_NAS_SysInfo.numocts;
  /* Check if the RRC MM interface structure array can hold this info */
  if(sib1->cn_CommonGSM_MAP_NAS_SysInfo.numocts > RRC_MAX_GSM_INFO_OCTET_COUNT)
  {
    WRRC_MSG1_ERROR("ERR: Not enough space for Common NAS Sys Info (%d bytes)", 
                        sib1->cn_CommonGSM_MAP_NAS_SysInfo.numocts);
    return FALSE;
  }

  /* Fill in the Common NAS system info now */
  data_ptr = sib1->cn_CommonGSM_MAP_NAS_SysInfo.data;
  for( i = 0; i < sib1->cn_CommonGSM_MAP_NAS_SysInfo.numocts; i ++)
  {
    nas_common_info.value[i] =  *data_ptr;
    data_ptr ++;
  }

  /* Call NAS API to check if LAC info provided in SIB1 is correct. */
  return (mm_extract_lac_from_sib_info(&lai, &nas_common_info));
}

#ifdef FEATURE_3GPP_CSFB
/*===========================================================================

FUNCTION          rrccsp_post_update_sibs_ind

DESCRIPTION       This function will post update SIBS IND to SIB with cause received in arguments

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_post_update_sibs_ind
(
  rrcsib_update_sibs_cause_e_type cause
)
{
  rrc_cmd_type *rrc_cmd_ptr;
  /* Build RRC_UPDATE_SIBS_IND command to post to SIB */
  /* Allocates the buffer to RRC Internal command */
  if( (rrc_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {     
    rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_UPDATE_SIBS_IND;
    rrc_cmd_ptr->cmd.update_sibs_ind.procedure = RRC_PROCEDURE_CSP;
    rrc_cmd_ptr->cmd.update_sibs_ind.cause = cause;
    /*Lets default the value TAG*/
    rrc_cmd_ptr->cmd.update_sibs_ind.mib_value_tag = 1;
    rrc_cmd_ptr->cmd.update_sibs_ind.real_sib_mod = TRUE;
    rrc_put_int_cmd(rrc_cmd_ptr);
    WRRC_MSG0_HIGH("CSFB: Sent RRC_UPDATE_SIBS_IND");
  }
}
#endif

/*===========================================================================

FUNCTION          rrccsp_get_sys_wcdma_band_mask

DESCRIPTION       This function will return supported WCDMA band mask

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_type rrccsp_get_sys_wcdma_band_mask
(
  void
)
{
  /*Define a local band mask */
  sys_band_mask_type local_band_mask= (SYS_BAND_MASK_WCDMA_I_IMT_2000 | SYS_BAND_MASK_WCDMA_II_PCS_1900 |
                                       SYS_BAND_MASK_WCDMA_III_1700 |SYS_BAND_MASK_WCDMA_IV_1700 |
                                       SYS_BAND_MASK_WCDMA_V_850 |SYS_BAND_MASK_WCDMA_VI_800 |
                                       SYS_BAND_MASK_WCDMA_VII_2600 |SYS_BAND_MASK_WCDMA_VIII_900|
                                       SYS_BAND_MASK_WCDMA_IX_1700
                                       |SYS_BAND_MASK_WCDMA_XI_1500
                                       | SYS_BAND_MASK_WCDMA_XIX_850
                                       );

  return local_band_mask;
}
/*===========================================================================

FUNCTION          rrccsp_channel_locking_nv_status

DESCRIPTION       This function will return TRUE if nv is non zero otherwise false

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/


boolean rrccsp_channel_locking_nv_status(void)
{
  if(rrc_nv_channel_lock_item_value !=0)
  {
    WRRC_MSG1_HIGH("NV LOCKING STATUS %d",rrc_nv_channel_lock_item_value);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION          rrccsp_get_locking_nv_value

DESCRIPTION       This function will return NV value

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

uint16 rrccsp_get_locking_nv_value(void)
{
  return rrc_nv_channel_lock_item_value;
}

/*===========================================================================

FUNCTION          rrccsp_init_first_band_to_be_scanned

DESCRIPTION       This function will init the first band to be scanned.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_init_first_band_to_be_scanned(void)
{
#ifdef FEATURE_WCDMA_MULTIBAND
  
  rrc_csp_curr_select_data.first_band_to_be_scanned = RRC_CSP_IMT2K_BAND;
  
  /* Reset the next_band_mask appropriately */
  WRRC_MSG0_HIGH("Resetting the next_band_mask");
  rrc_csp_curr_select_data.next_band_mask = rrc_csp_int_data.wcdma_band_mask;
  
  WRRC_MSG0_HIGH("Resetting the addl_chnl_done_mask");
  rrc_csp_curr_select_data.addl_chnl_done_mask = 0;
  
  
  rrc_csp_curr_select_data.first_band_to_be_scanned = RRC_CSP_BAND_MAX;

    if((rrc_csp_curr_select_data.first_band_to_be_scanned =
      rrccsp_get_next_band_to_be_scanned()) != RRC_CSP_BAND_MAX)
    {
      rrc_csp_curr_select_data.current_band = 
        rrc_csp_curr_select_data.first_band_to_be_scanned;
      WRRC_MSG1_HIGH("1st band to be scanned = %d",
        rrc_csp_curr_select_data.first_band_to_be_scanned);
    }
  
    WRRC_MSG0_HIGH("Resetting the addl_chnl_done_mask");
    rrc_csp_curr_select_data.addl_chnl_done_mask = 0;

  
#endif /* FEATURE_WCDMA_MULTIBAND */
}
/*===========================================================================

FUNCTION          rrccsp_set_est_req_for_emerg_call

DESCRIPTION       This function sets establishment request flag for emergency call

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

void rrccsp_set_est_req_for_emerg_call(void)
{
  rrc_csp_int_data.est_req_for_emerg_call = TRUE;
}

/*===========================================================================

FUNCTION rrc_csp_save_barred_freq

DESCRIPTION 
    This function adds barred frequency to the barred list
DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/

void rrc_csp_save_barred_freq()
{

  if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
  {
    if(rrc_csp_curr_select_data.num_of_barred_freq < RRC_MAX_OVERLAP_FREQ)
    {
      rrc_csp_curr_select_data.barred_freq[rrc_csp_curr_select_data.num_of_barred_freq] = rrc_csp_int_data.curr_acq_entry.freq;
      rrc_csp_curr_select_data.band_corr_barred_freq[rrc_csp_curr_select_data.num_of_barred_freq++] = rrc_csp_curr_select_data.current_band;
      MSG_HIGH("Adding WCDMA ARFCN freq %d from band %d. Total no of frequencies not to be skipped is %d",
         rrc_csp_int_data.curr_acq_entry.freq,rrc_csp_curr_select_data.current_band,rrc_csp_curr_select_data.num_of_barred_freq);
    }

    if(rrc_csp_curr_select_data.current_band == RRC_CSP_IMT2K_BAND)
      rrc_csp_curr_select_data.dont_skip_band_mask |= RRC_CSP_WCDMA_1700_2100_BAND;

    else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_2100_BAND)
      rrc_csp_curr_select_data.dont_skip_band_mask |= RRC_CSP_IMT2K_BAND;
   
    else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1800_BAND)
      rrc_csp_curr_select_data.dont_skip_band_mask |= RRC_CSP_WCDMA_1700_BAND;

    else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_1700_BAND)
      rrc_csp_curr_select_data.dont_skip_band_mask |= RRC_CSP_WCDMA_1800_BAND;

    else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_850_BAND)
      rrc_csp_curr_select_data.dont_skip_band_mask |= (RRC_CSP_WCDMA_BC19_BAND | RRC_CSP_WCDMA_800_BAND);

    else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_800_BAND)
      rrc_csp_curr_select_data.dont_skip_band_mask |= (RRC_CSP_WCDMA_850_BAND | RRC_CSP_WCDMA_BC19_BAND);
    
    else if(rrc_csp_curr_select_data.current_band == RRC_CSP_WCDMA_BC19_BAND)
      rrc_csp_curr_select_data.dont_skip_band_mask |= (RRC_CSP_WCDMA_850_BAND | RRC_CSP_WCDMA_800_BAND);

    else
      WRRC_MSG0_ERROR("Unexpected. Frequency barred in an non-overlapping band");

  }
}

/*===========================================================================

FUNCTION          rrccsp_get_lte_resel_allow_status

DESCRIPTION       This function will return lte_resel_allow_status

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_get_lte_resel_allow_status(void)
{
  return rrc_csp_int_data.allow_lte_resel;
}

/*===========================================================================

FUNCTION          rrccsp_check_if_em_plmn

DESCRIPTION       This function will return true if UE is currently camped on EM NW
                  This is needed as HSPA+ (Category 14) should be indicated in case 
                  of EM network.
DEPENDENCIES      None

RETURN VALUE      Boolean

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_check_if_em_plmn(void)
{
  int i,num_of_em_plmn;

  rrc_plmn_identity_type em_plmn[]= {
                                      { {4,4,0},2,{0,0,0xF} },
                                    };  
  num_of_em_plmn=1; /*Value should be set based on the number of EM network PLMNs present in array above */

  if((search_order_specialization  == SYS_SPECIAL_SBM_RAT_SEARCH_ORDER) || (search_order_specialization  == SYS_SPECIAL_SBM_PERIODIC_SEARCH))
  {  
    WRRC_MSG2_HIGH("Current camped PLMN MCC %d MNC %d ", 
               RRC_CSP_GET_INT_MCC(rrc_csp_int_data.curr_sel_plmn.plmn_id),RRC_CSP_GET_INT_MNC(rrc_csp_int_data.curr_sel_plmn.plmn_id));

    for(i=0;i<num_of_em_plmn;i++ )
    {
      WRRC_MSG2_HIGH("EM PLMN MCC %d MNC %d ",
                RRC_CSP_GET_INT_MCC(em_plmn[i]), RRC_CSP_GET_INT_MNC(em_plmn[i]));

      if ((RRC_CSP_SAME_MCC(rrc_csp_int_data.curr_sel_plmn.plmn_id, em_plmn[i]))
      && (RRC_CSP_SAME_MNC(rrc_csp_int_data.curr_sel_plmn.plmn_id, em_plmn[i])))
      {
        WRRC_MSG0_HIGH("Search Order Specialization: EM PLMN");
        return TRUE;
      }  
    }
  }
  return FALSE;

}



#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION          rrccsp_is_csg_white

DESCRIPTION       This function will return lte_resel_allow_status

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_is_csg_white(uint32 csg_id, rrc_plmn_identity_type plmn_id)
{
  uint32 i=0;

  for(i=0;i<rrc_csg_white_list.length;i++)
  {
    if((csg_id == rrc_csg_white_list.csg_list[i].csg_id)&&(sys_plmn_match(mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn_id),rrc_csg_white_list.csg_list[i].plmn_id)))
      return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION          rrccsp_asf_actively_in_prog

DESCRIPTION       This function will return TRUE if ASF is actively 
                  in progress

DEPENDENCIES      None

RETURN VALUE      bool

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_asf_actively_in_prog(void)
{

  return (rrc_csp_int_data.csg_asf_srch_in_progress && 
           ((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
             || (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
             ||(rrc_csp_int_data.curr_scan == RRC_CSP_WTOG_BPLMN_AUTOMATIC_SCAN)));
}

/*===========================================================================

FUNCTION          rrccsp_csg_in_whitelist

DESCRIPTION       This function will return CSG whitelist query based on 
                  current scan 

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_csg_in_whitelist(uint32 csg_id, rrc_plmn_identity_type plmn_id)
{
  /* If Manual CSG search in progress always return true so that SIB event
     doesnt get aborted*/
  if((rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_CSG)&&
      ((rrc_csp_int_data.curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)||
       (rrc_csp_int_data.curr_scan == RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)))
  {
    return TRUE;
  }

  if((rrccsp_get_curr_nas_requested_csg_id() != SYS_CSG_ID_INVALID)&&
     (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)&&
     (rrc_get_csp_substate() == RRCCSP_CELL_SEL_WT_SIB)
#ifdef FEATURE_WRLF_SYSTEM_SEL
     &&(!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
    )
  { /* It is expected that NAS updates whitelist after CSG service request if reg is successful */
    return TRUE;
  }
  /* Return TRUE for limited service search*/
  if(rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ)
  {
    return TRUE;
  }

  if(rrccsp_is_csg_white(csg_id,plmn_id))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION          rrccsp_is_csg_white_empty

DESCRIPTION       This function will return lte_resel_allow_status

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_is_csg_white_empty(void)
{
  return (rrc_csg_white_list.length);
}
/*===========================================================================

FUNCTION          RRCCSP_CSG_SET_FP_INFO

DESCRIPTION       This function updates the FP vars with current camped macro
                  upon W manual CSG search trigger
DEPENDENCIES
                  None.

RETURN VALUE
                  NAS requested PLMN.

SIDE EFFECTS      None.
===========================================================================*/

void rrccsp_csg_set_fp_info()
{
  if(rrc_csp_int_data.service_search_bgnd == SYS_SERVICE_SEARCH_CSG)
  {
    if(RRC_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_CSP))
    {
      rrc_csp_int_data.last_csg_rat     = RRC_CSG_WCDMA;
      rrc_csp_int_data.fp_macro.freq    = rrc_ccm_get_curr_camped_freq();
      rrc_csp_int_data.fp_macro.pci     = rrc_ccm_get_curr_camped_psc();
      rrc_csp_int_data.fp_macro.plmn    = rrc_ccm_get_curr_camped_plmn();
      rrc_csp_int_data.fp_macro.cell_id = rrc_ccm_get_curr_camped_cell_id();
    }
    else
    {
      rrc_csp_int_data.last_csg_rat = RRC_CSG_NONE;
    }
  }
}

#endif
/*===========================================================================

FUNCTION          rrc_check_cell_bar_status_for_reselection

DESCRIPTION       This function will check the cell bar status for teh cell.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
static void rrccsp_check_cell_bar_status(rrc_SysInfoType3 *sib3_ptr, 
      rrc_plmn_identity_type  plmn_id, rrc_csp_cell_bar_e_type  *cell_bar)
{
  rrc_CellAccessRestriction *sib3_4_access_restriction_ptr;
  rrc_csp_substate_e_type curr_csp_substate = rrc_get_csp_substate();  

  if( sib3_ptr == NULL )
  {
    /* SIB3 for this PLMN, CEll ID combination not present */
    ERR_FATAL("CSG: SIB3 empty.", 0, 0, 0);
  } /* SIB3 not present */
  if((rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_MANUAL_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_PLMN_LIST_SCAN))
  {

    if((rrc_get_state() != RRC_STATE_DISCONNECTED) && (rrc_get_state() != RRC_STATE_CONNECTING))
    {
      if(sib3_ptr->sib4indicator == TRUE)
      {
        WRRC_MSG0_HIGH("CSG: We might not have read SIB4 now. Check for cell bar status at CSP");
        WRRC_MSG0_HIGH("CSG: This scenario can be handled here in future");
        *cell_bar = RRC_CSP_CELL_OK;
        return;
      }
      else
      {
        sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
      }
    }
    else
    {
      sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
    }
  }
  else
  {
    sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
  }

  /* The SIB3 is present - lets disect it */

  if((RRC_CHECK_COMMON_MSG_TYPE2(sib3_4_access_restriction_ptr->cellBarred,rrc_CellBarred,
                                                notBarred )) &&
     ( sib3_4_access_restriction_ptr->cellReservedForOperatorUse == 
                                                     rrc_ReservedIndicator_notReserved ) &&
     ( sib3_4_access_restriction_ptr->cellReservationExtension == 
                                                        rrc_ReservedIndicator_notReserved ))
  {
    /* Cell is NOT barred and NOT Reserved */
    WRRC_MSG0_HIGH("Cell Not Barred, Not reserved");
    *cell_bar = RRC_CSP_CELL_OK;
  } /* If Cell is NOT barred and NOT Reserved */

  else if((RRC_CHECK_COMMON_MSG_TYPE2(sib3_4_access_restriction_ptr->cellBarred,rrc_CellBarred,
                                                      notBarred )) &&
          ( sib3_4_access_restriction_ptr->cellReservedForOperatorUse == 
                                                     rrc_ReservedIndicator_notReserved ) &&
          ( sib3_4_access_restriction_ptr->cellReservationExtension == 
                                                      rrc_ReservedIndicator_reserved ))
  {
#ifdef FEATURE_FEMTO_CSG
    if(RES_FOR_CSG_IN_SIB3(sib3_ptr))
    {
      WRRC_MSG0_HIGH("CSG capable UE, cell reserved for CSG. Consider cell as not barred");
      *cell_bar = RRC_CSP_CELL_OK;
      return;
    }
#endif
    /* Cell is NOT Barred but RESERVED for SoLSA - Treat it as Barred since the 
     * UE does not have SoLSA support (TBD: May change for UEs supporting 
     * Rel 2000 )
     */
    WRRC_MSG0_HIGH("Cell treated Barred future Reserved");
    rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;

    if((rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ) 
        &&  (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
    {
      /* Cell is Barred. Ignore the Intra-frequency cell re-selection indicator */
      WRRC_MSG0_HIGH("Ignore Intra-freq cell resel ind");
      *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
    }
    else
    {
      /* Cell is Not Barred, but RESERVED for SoLSA (is treated as BARRED) AND
       * "Intra frequency Cell re-selection Indicator" is NOT ALLOWED */ 
      WRRC_MSG0_HIGH("Intra-freq Not Allowed");
      *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
    }
  } /* If Cell is NOT Barred, not reserved for Operator but RESERVED for SoLSA */

  else if ((RRC_CHECK_COMMON_MSG_TYPE2(sib3_4_access_restriction_ptr->cellBarred,rrc_CellBarred,
                                                      notBarred) ) &&
     ( sib3_4_access_restriction_ptr->cellReservedForOperatorUse == 
                                                      rrc_ReservedIndicator_reserved ))
  {
    /* Cell is NOT Barred but RESERVED for Operator */
    WRRC_MSG0_HIGH("Cell reserved for OPerator");

    if(rrc_csp_ignore_barred_rsv_status_due_to_ueut(plmn_id))
    {
      WRRC_MSG0_HIGH("UEUT. Consider cell as NOT rsv for Op");
      *cell_bar = RRC_CSP_CELL_OK;
      return;
    }

    /* Now check the UE Access Class to determine Cell Status */

   if(( rrc_hi_access_class != RRC_HI_ACCESS_CLASS_ABSENT ) &&
       (((rrc_hi_access_class & RRC_ACCESS_CLASS_11_BIT) ||
         (rrc_hi_access_class & RRC_ACCESS_CLASS_15_BIT)) &&
        (rrc_is_bcch_plmn_an_hplmn_or_ehplmn(plmn_id))))
    {
      /* UEs assigned to Access Class 11 or 15 may select/re-select this cell if in the home PLMN.
       * UEs assigned to an Access Class in the range 0 to 9 and 12 to 14 shall behave as if
       * cell status "barred" is indicated using the value "not allowed" in the
       * IE "Intra-frequency cell re-selection indicator" and the maximum value for Tbarred
       */

       WRRC_MSG0_HIGH("UE High AC present and in HPLMN");
       WRRC_MSG0_HIGH("Cell treated NOT Barred");     
       *cell_bar = RRC_CSP_CELL_OK;
    }
    else
    {
      if(rrc_hi_access_class == RRC_HI_ACCESS_CLASS_ABSENT)
      {
        WRRC_MSG0_HIGH("UE High AC not present");
      }

      WRRC_MSG0_HIGH("Cell treated Barred (Op Reserved)");
      rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;
      rrc_csp_int_data.cell_reserved_for_operator_use = TRUE;
      if((rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ) 
          &&  (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
      {
        /* Cell is Barred. Ignore the Intra-frequency cell re-selection indicator */
        WRRC_MSG0_HIGH("Ignore Intra-freq cell resel ind");
        *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
      }
      else if(((curr_csp_substate == RRCCSP_SUIT_CELL_RESELECT_WT_SIB) 
                 || (curr_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB))
                && (rrc_csp_int_data.new_cell_acq_entry.freq == rrc_csp_int_data.curr_acq_entry.freq))
      {
        /* Intra-frequency neighbor frequency is barred. Barring the frequency */
        WRRC_MSG0_HIGH("Intra-Freq Ngbr Cell Freq barred. Sndng Freq Barred to L1");
        *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
      }
      else
      {
        /* UE has either a High Access Class but is NOT in Home PLMN
         * OR UE does NOT have a High Access Class */
        /* Cell is TREATED AS BARRED for this UE */
        /* Cell is Not Barred, but RESERVED for Operator (is treated as BARRED) 
         * AND "Intra frequency Cell re-selection Indicator" is NOT ALLOWED */ 
        WRRC_MSG0_HIGH("No UE Hi AC or not in HPLMN,Intra-freq Not Allowed");
        *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;       
      }
    }
  } /* If Cell is NOT Barred but RESERVED for Operator */

  else if (RRC_CHECK_COMMON_MSG_TYPE2(sib3_4_access_restriction_ptr->cellBarred,rrc_CellBarred,
                                                      barred ))
  {
    if(rrc_csp_ignore_barred_rsv_status_due_to_ueut(plmn_id))
    {
      WRRC_MSG0_HIGH("UEUT. Consider cell as NOT barred");
      *cell_bar = RRC_CSP_CELL_OK;
      return;
    }

    switch(sib3_4_access_restriction_ptr->cellBarred.u.barred->t_Barred)
    {
      case rrc_T_Barred_s10:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 10;
        break;
      case rrc_T_Barred_s20:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 20;
        break;
      case rrc_T_Barred_s40:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 40;
        break;
      case rrc_T_Barred_s80:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 80;
        break;
      case rrc_T_Barred_s160:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 160;
        break;
      case rrc_T_Barred_s320:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 320;
        break;
      case rrc_T_Barred_s640:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 640;
        break;
      case rrc_T_Barred_s1280:
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;
        break;
      default:
        WRRC_MSG0_HIGH("Unexpected. Setting the time to 1280 secs");
        rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;
        break;
    }
    
    WRRC_MSG0_HIGH("Cell is indicated as Barred");
    /* Cell is indicated as Barred */
    if( sib3_4_access_restriction_ptr->cellBarred.u.barred->
                      intraFreqCellReselectionInd == rrc_AllowedIndicator_allowed )
    {
      /* Cell is Barred
       * AND "Intra frequency Cell re-selection Indicator" is ALLOWED */ 
      MSG_LOW("Intra-freq Allowed", 0, 0, 0);
      *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
    }
    else
    {
      if((rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ)
         && (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
      {
        /* Cell is Barred. Ignore the Intra-frequency cell re-selection indicator */
        WRRC_MSG0_HIGH("Ignore Intra-freq cell resel ind");
        *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
      }
      else if(CSG_ID_IN_SIB3(sib3_ptr))
      {
          WRRC_MSG0_HIGH("CSG cell ignore intra freq resel not allowed indicator");
          *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
      }
      else
      {
        /* Cell is Barred
         * AND "Intra frequency Cell re-selection Indicator" is NOT ALLOWED */ 
        WRRC_MSG0_HIGH("Intra-freq Not Allowed");
        *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
      }
    }
  } /* Cell is indicated as Barred */
  else
  {
    /* We should not come here */
    WRRC_MSG0_ERROR("ERR: Error in cell bar evaluation logic");
    rrc_csp_int_data.wcdma_cell_time_to_reselect = 1280;

    if((rrc_csp_int_data.curr_req_plmn.plmn_type == RRC_GSM_MAP_ANY_PLMN_REQ) 
        &&  (rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
    {
      /* Cell is Barred. Ignore the Intra-frequency cell re-selection indicator */
      WRRC_MSG0_HIGH("Ignore Intra-freq cell resel ind");
      *cell_bar = RRC_CSP_CELL_BAR_INTRA_FREQ;
    }
    else if(((curr_csp_substate == RRCCSP_SUIT_CELL_RESELECT_WT_SIB) 
               || (curr_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB))
              && (rrc_csp_int_data.new_cell_acq_entry.freq == rrc_csp_int_data.curr_acq_entry.freq))
    {
      /* Intra-frequency neighbor frequency is barred. Barring the frequency */
      WRRC_MSG0_HIGH("Intra-Freq Ngbr Cell Freq barred. Sndng Freq Barred to L1");
      *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
    }
    else
    {
      *cell_bar = RRC_CSP_CELL_BAR_NO_INTRA_FREQ;
    }
  }
}
/*===========================================================================

FUNCTION          rrccsp_initial_suitability_check

DESCRIPTION       This function will check initial suitability of the cell

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

void rrccsp_initial_suitability_check(rrc_MasterInformationBlock *mib_ptr, rrc_SysInfoType3 *sib3_ptr,
                                      rrc_plmn_identity_type mib_plmn_id, 
                                      #ifdef FEATURE_FEMTO_CSG
                                      uint32 csg_id,
                                      #endif
                                      rrc_sib_status_e_type *status)
{

//  rrc_csp_plmn_status_e_type  plmn_status;
  rrc_plmn_identity_type shared_plmn_list[MAX_SHARED_PLMNS];
  rrc_csp_cell_bar_e_type  cell_bar_status;
#ifdef FEATURE_FEMTO_CSG
  boolean is_csg_white;
  uint8 i;
  rrc_csp_substate_e_type rrc_csp_substate;
  rrclog_wcdma_csg_cell_detected_event_type csg_cell_detected;
#endif
 uint8 shared_plmn_idx = 0, num_of_shared_plmns = 0;
  boolean nw_shared = FALSE;

//  plmn_same_or_eq = FALSE;
  

  boolean hplmn_cell_bar_status_found = FALSE;
  rrc_csp_cell_bar_e_type hplmn_cell_bar_status = RRC_CSP_CELL_OK;
  boolean vplmn_cell_bar_status_found = FALSE;
  rrc_csp_cell_bar_e_type vplmn_cell_bar_status = RRC_CSP_CELL_OK;
  *status = SIB_EVENT_SUCCEEDED;
#ifdef FEATURE_FEMTO_CSG
  rrc_csp_substate = rrc_get_csp_substate();
  



  is_csg_white = rrccsp_csg_in_whitelist(csg_id, mib_plmn_id);
  if(rrccsp_asf_actively_in_prog())
  {
    rrccsp_prune_acqdb_after_SIB(csg_id,is_csg_white);
  }

  /* Check CSG ID for white list. */
  if(csg_id != SYS_CSG_ID_INVALID)
  {
    for(i=0; i<3; i++)
    {
      csg_cell_detected.mcc[i] = mib_plmn_id.mcc[i];
      csg_cell_detected.mnc[i] = mib_plmn_id.mnc[i];
    }
    if((rrc_csp_substate == RRCCSP_SUIT_CELL_RESELECT_WT_SIB)||
       (rrc_csp_substate == RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB))
    {
      csg_cell_detected.freq = rrc_csp_int_data.new_cell_acq_entry.freq;
      csg_cell_detected.psc = rrc_csp_int_data.new_cell_acq_entry.scr_code;
    }
    else
    {
      csg_cell_detected.freq = rrc_csp_int_data.curr_acq_entry.freq;
      csg_cell_detected.psc = rrc_csp_int_data.curr_acq_entry.scr_code;
    }
    csg_cell_detected.csg_id = csg_id;
    event_report_payload(EVENT_WCDMA_RRC_CSG_DETECTED, sizeof(rrclog_wcdma_csg_cell_detected_event_type),
                   (void*)&csg_cell_detected);

    if((!is_csg_white) && (CSG_IND_IN_MIB(mib_ptr)))
    {
      if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_SELECTION_SCAN)
      {
        if(rrc_is_s_criteria_met(mib_plmn_id,rrc_translate_cell_id(&sib3_ptr->cellIdentity)))
        {
          rrc_csp_int_data.plmn_fnd_during_fgnd_svc_srch = TRUE;
        }
      }

      WRRC_MSG0_HIGH("CSG: CSG cell not in white list. Barring cell for 1280sec");
      rrc_csp_int_data.bar_non_white_csg = TRUE;
      rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_PSC_BARRED, 1280);

      *status = SIB_EVENT_FAILED_CSG_NW;
      return;
    }
    /* For ASF search consider hybrid cells if CSG ID is present in whitelist else fail the SIB event */
    else if((rrccsp_asf_actively_in_prog()) && (!is_csg_white) && (!CSG_IND_IN_MIB(mib_ptr)) 
        && (!((rrc_ccm_get_curr_camped_freq() == rrccsp_get_serving_frequency()))))
    {
      WRRC_MSG0_HIGH("CSG: CSG cell is not hybrid but a macro...bailing out for ASF search");    
      *status = SIB_EVENT_FAILED_OTHER;
      return;
    }
  }
#endif
  if(rrc_is_nw_shared(mib_ptr))
  {
    nw_shared = TRUE;
    num_of_shared_plmns = rrccsp_construct_multiple_plmn_list_from_mib(
                                                                       mib_ptr,
                                                                       shared_plmn_list);
  }

/*
  do
  {
      if(nw_shared)
      {
        mib_plmn_id = shared_plmn_list[shared_plmn_idx++];
      }

      if(rrc_bcch_and_nas_plmns_match(mib_plmn_id, rrc_csp_int_data.curr_req_plmn.plmn_id))
      {
        WRRC_MSG0_HIGH("PLMN is the same as requested BCCH and NAS PLMN match");
        plmn_same_or_eq = TRUE;
        break;
      }
      else
      {
        if((rrccsp_check_for_eplmn(
                  mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                    rrc_csp_int_data.curr_req_plmn.plmn_id))) == TRUE)
        {
          WRRC_MSG0_HIGH("NAS PLMN is in the eq PLMN list");
          if((rrccsp_check_for_eplmn(
                    mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                     mib_plmn_id))) == TRUE)
          {
            WRRC_MSG0_HIGH("BCCH PLMN is in the eq PLMN list");
            plmn_same_or_eq = TRUE;
            break;
          }
          else
          {
            WRRC_MSG0_HIGH("BCCH PLMN not in eq PLMN list");
          }
        }
        else
        {
          WRRC_MSG0_HIGH("NAS PLMN not in eq PLMN list");
        }
      }

  }while(shared_plmn_idx < num_of_shared_plmns)

  if(!plmn_same_or_eq)
  {
    return FALSE;
  }
  */

  /* CHECK CELL BAR STATUS */

  do
  {
    if(nw_shared)
    {
      mib_plmn_id = shared_plmn_list[shared_plmn_idx++];
    }

    if(rrc_is_bcch_plmn_an_hplmn_or_ehplmn(mib_plmn_id))
    {
      if(!hplmn_cell_bar_status_found)
      {
        rrccsp_check_cell_bar_status( sib3_ptr, mib_plmn_id, &cell_bar_status);
        hplmn_cell_bar_status = cell_bar_status;
        hplmn_cell_bar_status_found = TRUE;
      }
      else
      {
        cell_bar_status = hplmn_cell_bar_status;
      }
    }
    else
    {
      if(!vplmn_cell_bar_status_found)
      {
        rrccsp_check_cell_bar_status( sib3_ptr, mib_plmn_id, &cell_bar_status);
        vplmn_cell_bar_status = cell_bar_status;
        vplmn_cell_bar_status_found = TRUE;
      }
      else
      {
        cell_bar_status = vplmn_cell_bar_status;
      }
    }

  }while(shared_plmn_idx < num_of_shared_plmns);

  if(hplmn_cell_bar_status_found == TRUE)
  {
    cell_bar_status = hplmn_cell_bar_status;
  }

 
    if(cell_bar_status == RRC_CSP_CELL_BAR_INTRA_FREQ)
    {
      WRRC_MSG0_HIGH("CSG: Cell barred");
      /* Send Cell bar request to L1 */
      rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_PSC_BARRED, 
                                            rrc_csp_int_data.wcdma_cell_time_to_reselect);
      *status = SIB_EVENT_FAILED_CELL_BAR;
    }
    else if(cell_bar_status == RRC_CSP_CELL_BAR_NO_INTRA_FREQ)
    {
      WRRC_MSG0_HIGH("CSG: Freq barred");
      /* Send Cell bar request to L1 */
      rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_FREQ_BARRED, 
                                            rrc_csp_int_data.wcdma_cell_time_to_reselect);
      *status = SIB_EVENT_FAILED_FREQ_BAR;
    }

}



#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION          rrccsp_reset_wait_l1_rel_lock

DESCRIPTION       To reset csp_waiting_for_wl1_to_release_lock flag

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

void rrccsp_reset_wait_l1_rel_lock()
{
  rrc_csp_int_data.csp_waiting_for_wl1_to_release_lock = FALSE;
}
#endif
/*===========================================================================

FUNCTION          RRCCSP_CONVERT_RCSP_MEAS_TO_REPORT

DESCRIPTION       Converts measured RSCP value to reported value. 
                  Spec 25.133 section 9.1.1.3 CPICH RSCP measurement 
                  report mapping

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/


int16 rrccsp_convert_rcsp_meas_to_report(int16 rscp)
{
  if(rscp < -120)
  {
    return -5;
  }
  else if(rscp >= -25)
  {
    return 91;
  }
  else
  {
    return (rscp+116);
  }

}

/*===========================================================================

FUNCTION          RRCCSP_CONVERT_ECIO_MEAS_TO_REPORT

DESCRIPTION       Converts measured ECIO value to reported value. 
                  Spec 25.133 section 9.1.2.3 CPICH ECIO 
                  measurement report mapping.At wl1 mapping is as follows
                  ecio_2x = -1  --> -0.5dB
                  ecio_2x = -2 -->    -1dB

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

uint8 rrccsp_convert_ecio_meas_to_report(int16 ecio_2x)
{
  if(ecio_2x >= 0)
  {
    return 49;
  }
  else if(ecio_2x < -48)
  {
    return 0;
  }
  else
  {
    return (49 + ecio_2x);
  }
}
/*===========================================================================

FUNCTION          RRC_CSP_SET_DRX_LENGTH

DESCRIPTION       This function is responsible for setting the DRX length

DEPENDENCIES      None

RETURN VALUE      uint32 -- drx_length

SIDE EFFECTS      None
===========================================================================*/
uint32 rrc_csp_set_drx_length(uint32 drx_len)
{

  if(rrc_constant_320ms_drx_nv == TRUE)
  {
     WRRC_MSG0_HIGH("rrc_constant_320ms_drx_nv is SET");
     if(drx_len % 32 == 0 )
     {
           /* If drx_len is multiple of 320 then set drx_len as 320*/
           WRRC_MSG1_HIGH("drx %d is mutliple of 32.. Setting drx to 32 ",drx_len);
           drx_len = 32;
   
     }
  }
  else if(rrc_half_drx_len_nv == TRUE)
  {
     rrc_state_e_type  rrc_state =  rrc_get_state();
     if((rrc_state == RRC_STATE_DISCONNECTED) && (drx_len > 32))
     {
       drx_len = drx_len/2;
       WRRC_MSG2_HIGH("rrc_half_drx_len_nv is SET..Old DRX %d New DRX length is %d",2*drx_len,drx_len);
     }
  }
  return drx_len;

}

/*===========================================================================

FUNCTION          rrccsp_check_redirection_barred

DESCRIPTION       This function is called to check if redirected freq/PSC is barred or not

DEPENDENCIES      None

RETURN VALUE      Return true if frequency is barred

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_check_redirection_barred(rrc_cmd_type  *rrc_cmd_ptr)
{
  l1_cell_bar_info_e_type l1_bar_status;
  uint16 wcdma_cell_time_to_reselect;
  boolean is_non_white_csg;
  boolean is_bar_resel;
  l1_cell_bar_info_e_type bar_type;
  rrc_freq_type  dest_freq;
  rrc_scr_code_type   dest_scr_code = 8192;
  boolean status = FALSE;

  if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present)
  {
    dest_freq = rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq;
    if(rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present)
    {
      bar_type = L1_WCDMA_PSC_BARRED;
      dest_scr_code = 16*rrc_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc;
    }
    else
    {
      bar_type = L1_WCDMA_FREQ_BARRED;
    }

    l1_bar_status = wl1_check_cell_barred_and_get_bar_time (dest_freq, dest_scr_code, &wcdma_cell_time_to_reselect, &is_non_white_csg,&is_bar_resel);
    
    MSG_HIGH("Checking if redirection to barred Freq/PSC: bar_type %d, l1_bar_status %d., dest_scr_code %d",bar_type, l1_bar_status,dest_scr_code);
    
    if((l1_bar_status == L1_WCDMA_FREQ_BARRED)  ||
       (bar_type == l1_bar_status))
    {
      WRRC_MSG0_HIGH("Skipping redirection attempt as Freq/PSC is barred ");
      status = TRUE;
    }
  }
  return status;
}

#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
/*===========================================================================

FUNCTION          l1_rrc_check_sfn_det_acq

DESCRIPTION       This function is called by WL1 to check if SFN detection needs to be
                  performed during ACQ or not

DEPENDENCIES      None

RETURN VALUE      True/FALSE

SIDE EFFECTS      None
===========================================================================*/
boolean l1_rrc_check_sfn_det_acq(uint16 freq, uint16 psc)
{
  uint8 idx = 0;
  
  if(
      (WCDMA_RRC_IDLE_FEATURES(RRC_CSFB_CALL_TIMELINE_OPT)) &&
      (rrc_csfb_call_status) &&
      (rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)  &&
      (rrc_csp_int_data.interrat_type == RRC_CSP_INTERRAT_LTE) &&
      (rrc_csp_int_data.dest_freq_present) &&
      (rrc_csp_int_data.dest_freq == freq) 
    )  
  {
    for(idx; idx < rrc_SIB_container.num_cont; idx ++)
    {
      if(psc == rrc_SIB_container.psc[idx])
      {
        return TRUE;
      }
    }
  }
  return FALSE;  
}
#endif