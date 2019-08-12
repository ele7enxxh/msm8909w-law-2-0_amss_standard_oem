#ifndef RRCCSPI_H
#define RRCCSPI_H

/*===========================================================================
            R R C    C S P     I N T E R N A L     H E A D E R    F I L E
DESCRIPTION

   This file contains all internal prototypes used by Cell Selection Procedure.

EXTERNALIZED FUNCTIONS



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

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccspi.h_v   1.25   19 Jun 2002 10:01:34   bumatt  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrccspi.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   sn      Changes for LTE U feature
06/24/16   sr      Made changes to not acquire lock for WTOG BPLMN in RRC
03/10/15   sn      Send idle request on receving DL WK indication with no lock
10/31/14   vi      Made changes to allow QTA during IDT for PS domain
10/07/14   sg      Made changes to update the compiler flag name properly
09/29/14   sg      Made changes to check for upper layer band capability
                   also when indicating DBDC capability
09/24/14   sr      Made changes to send a new command to abort WL1 sleep when PLMN LIST REQ is received
09/23/14   sn      Changes for WRRC FR 21174
09/04/14   gv      Added code to support split acq with bplmn
09/04/14   vi      Made changes to reduce curr_scan prints in the log
07/23/14   bc      Made changes to camp on EHPLMN when NAS requested PLMN is not available.
07/08/14   rmn     Made changes so that WRRC will skip redirection attempt on N/W directed barred frequency
06/26/14   sn      WRRC changes for FR20484
06/23/14   rmn     When calling new WRM lock request API, WRRC should indicate lock requested 
                   bands with type 'sys_band_class_e_type'
06/23/14   gv      Made changes to prevent CSP substate from getting reset if there is a pending 
                   MODE CHANGE after WCDMA stack is stopped
06/19/14   rmn     Made changes for DR-DSDS Feature
04/16/14   vi      Fixed compilation errors.
04/07/14   sr      Made changes to allow manual PLMN during WRLF
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/28/14   vi      Made changes to set drx to 320ms if rrc_constant_320ms_drx_nv is set
11/05/13   rmn     Changes for displaying Partial PLMN Search Results during Manual Search
09/02/13   sr      Made changes for BST optimizations
03/17/14   sn      Changes to control ASF timer with a NV 
03/11/14   bc      Made changes to ignore bplmn get_specific_sibs when bplmn is suspended
                   during Manual BPLMN search.
01/03/14   bc      Made Changes to read SIBs across DRX cycles.
11/07/13   bc      Made changes to abort sib reading if we find that the cell is barred 
                   or la is forbidden.
12/24/13   bc      Made changes to not to skip last camped frequency in idle mode once
                   for every raw scan.
12/13/13   sn      Manual PLMN search optimizations
12/13/13   sn      Changes to set UE OOS flag only if S-criteria passes
12/12/13   vi      Added code for FR: Service Search with scan scope ACQ DB only.
09/16/13   sn      G2W TA feature changes
11/18/13   bc      Checked in REL 10 spec crs changes
11/15/13   rmn     Made changes to save BPLMN data when leaving contd. mode scan is triggered due to CN Page Ind in PCH State
10/07/13   rmn     Changes for Feature: RAT Search Order for FDD/TDD LTE
09/13/13   sn      Changes to include ECNO in CGI response to LTE
08/14/13   vg      Made changes to ignore CellUpdateConfirm messages during reselection in a corner case
08/13/13   rmn     Made changes to continue ongoing MPLMN Search in bkgrnd to frgrnd on receiving OOS Ind
08/13/13   sr      FR16437: Reduced W@G timer value to 95
08/13/13   sn      Changes to release after decoding SIBs during BPLMN
08/13/13   rmn     Made changes for requirement to indicate ACQ failure because of band mismatch
07/08/13   sn      Changes for selective band scan for PLMN searches
07/08/13   sn      Changes for LTE CSG feature
06/19/13   sn      Changes to include CSG ID and RSCP in CGI response
06/06/13   sr      Changes to check for S criteria for XTOW reselection after SIB3 is read
05/30/13   gv      Made changes to perform acq db only scan when requested so by higher layers
04/18/13   vg      made changes in RRC to release the lock on urgent access preepmption req
                   from other SUB while waiting for SRB data.
03/25/13   gv      Made changes to stop GTOW redirection when the redirected cell belongs to forbidden LA
12/13/12   gv      Added code to restrict the LTOW redirection search time for a CSFB call
12/07/12   sn      Changes to indicate absence of FP info in ASF srch resp
12/07/12   sn      CSG feature changes
11/26/12   sn      Changes to defer processing RRC_MODE_CHANGE_REQ when wtog abort in progress
11/09/12   gv      Made changes to not scan WCDMA bands when RAT priority list does not contain WCDMA
10/29/12   gv      Mainline the changes under FEATURE_PPAC
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
09/21/12   gv      Made changes to start 1s timer for fast redirection from WtoL before which ACQ is supposed to be successful.
                   If ACQ is successful before expiry of timer, it is restarted with a value of 5s for SIB reading
09/13/12   gv      Added support to send deact request to TDSCDMA while WCDMA is deactivated to facilitate 
                   writing of ACQ DB NV
09/10/12   gv      Mainline the changes under feature FEATURE_WCDMA_BPLMN_SUSPEND_RESUME
08/31/12   gv      Mainline the changes under feature FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL
08/17/12   gv      Added changes to support suspension of intra/inter-W BPLMN search on entering DCH/FACH
                   and resumption of the same on entering PCH/IDLE
07/18/12   sn      Changes to ignore freq redir wait timer when we are in final stages 
                   of camping on redirected frequency
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/12/12   sks     Made changes to not report incorrect RSSI when UE mimics out of service
                   due to TRM resource.
05/01/12   sks     Added changes to indicate NAS that NO SERVICE is because of no
                   resource from TRM.
04/10/12   gv      Fixed compilation issue
04/09/12   gv      Added code to prevent CPU hogging during G2W redirection
04/03/12   sks     For XTOW BPLMN and CGI_REQ, made changes to optimize the retry ACQ duration in the next DRX
                   by saving PSC and PN_POS.
02/09/12   gv      Added code to support inclusion of PLMN ID in IDT message with network sharing
01/12/12   gv      Added code to support FEATURE_PPAC
12/12/11   gv      Added code to support free floating of tasks
11/23/11   gv      Added code to convert detected GSM cells to UARFCN in Band III (DCS)
11/18/11   mn      Made changes to disable reselection to LTE after attach reject from network
                   with cause 14.
10/19/11   gv      Made changes to the variable which saves scanned band mask
09/28/11   gv      Added code to support optimized band scan in WCDMA
09/12/11   gv      Renamed feature flag FEATURE_WCDMA_RRC_EMERG_CALL_SUPPORT
                   to FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL
09/12/11   gv      Added new boolean est_req_for_emerg_call to rrc_csp_int_data_type
                   under feature FEATURE_WCDMA_RRC_EMERG_CALL_SUPPORT
08/23/11   sks     Added interrat abort guard timer of 2sec.
07/08/11   sks     Added a function to init first band to be scanned.
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/28/11   sks     Made changes to return TRUE for can_L1_go_to_sleep(), if CTCH setup is
                   pending at LLC.
03/21/11   rl      Added code to support L2W CGI
03/17/03   sks     Made use of global to start timer RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER.
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/10/11   sks     Made changes to not include pre-redir info in RRC connection request
                   msg following a LTOW redirection.
03/08/11   su      Added code to support enabling and disabling of 
                   FEATURE_UEUT_IGNORE_CELL_BAR_RESERVE_STATUS using NV item.
03/03/11   vg      added code to start 150 sec bplmn gaurd timer if lte is enable
03/02/11   vg      added code to start bplmn guard timer for 3min if UE is
                   dual sim standby enabled.
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   rm      Added code to support BC19 under FEATURE_WCDMA_BC19
01/27/11   rm      Added prorotype for rrc_is_s_criteria_met
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/20/10   sks     Made changes to perform acq retries on LTOW redirected freq.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   rm      Added code to handle CSFB status indication
10/11/10   rm      Added code to support 3GPP CSFB
09/16/10   sks     Added a function to check SIB1 validity by checking the LAC validity.
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM
07/06/10   rm      Added code to put WL1 to sleep if BPLMN is suspended/cleaned up when
                   lock release offset timer is running
07/05/10   rm      Removed duplicate service_req_cause from rrc_csp_int_data_type
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/25/10   rm      Corrected value of WTOW_BPLMN_SEARCH_ADDTNL_OVERHEAD_TIMER_IN_MS to 30ms
06/25/10   rm      Added code to support DUAL SIM BPLMN
06/18/10   rm      Made changes to indicate WL1 if the frequency is reserved for operator use.
06/07/10   rm      Added boolean ctch_drop_pending_at_llc to rrc_csp_int_data_type 
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
05/24/10   sks     Made changes to support R7 spec CR related to HPLMN-VPLMN ping-pong.
05/24/10   as      Made changes to index SIBDB with common PLMN ID for shared cell
03/15/10   rm      Added code to support FEATURE_RAT_PRIORITY_LIST for W<->G IRAT
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/01/10   dm      Added function for sending CPHY_DRX_REQ to L1 for multiple DRX 
                   support feature.
11/02/09   sks     Added code to suspend handling of STOP_MODE_REQ, until current 
                   WTOG process is aborted.
09/22/09   rm      Added code to clear L1 barring info and write ACQ DB to NV on
                   MODE_CHANGE_REQ to LPM 
07/20/09   rm      Added prototype for rrccsp_check_for_eplmn
07/16/09   rm      Ported FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE to COOS
06/15/09   sks     Made changes to support messaging interface with RR for writing 
                   ACQ-DB to NV during power off.
06/03/09   rm      Added new boolean cel_trans_cnf_recvd_at_llc to rrc_csp_int_data_type
                   Added prototype for rrc_csp_cell_trans_cnf_rcvd_at_llc
05/22/09   rm      Fixed Lint errors
05/22/09   rm      Added new boolean ctch_setup_req_pending to rrc_csp_int_data_type
05/20/09   rm      Added new boolean skip_gsm_search to rrc_csp_int_data_type to track if NAS requested RRC
                   to skip GSM search in SERVICE_REQ
05/11/09   rm      Added new boolean cel_resel_ind_rcvd_waiting_trans_cnf to rrc_csp_int_data_type
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
03/28/09   rm      CMI Featurisation
02/02/09   rm      Ported List Frequency Scan (LFS) to conventional OOS
12/29/08   rm      Added support for 3G<->GAN Handover
11/27/08   sk      Changes for BPLMN 320ms
11/21/08   rm      Added code to unbar non-eplmn cells when Equivalent PLMN list changes
11/06/08   sk      Defined RRCTMR_WTOG_BPLMN_ABORT_GUARD_TIMER_IN_MS
11/06/08   rm      Fixed compilation warning
11/04/08   rm      Fixed the merge issue,Added prototype for rrccsp_check_and_send_uoosc_end_ind
10/31/08   sk      Removed GCF specific feature to limit raw scan results to 5
10/10/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
08/01/08   pk      Report Limited_Service or No_Service to UI as soon as first cycle of
                   GSM search is done instead of waiting for whole GSM search to complete
                   when in OOS in RRC connected mode.
07/14/08   pk      Update band preferences given in LIST_REQ to RR while PLMN search is 
                   being done. Restore band preferences after the PLMN search is completed. 
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/20/08   pk      Added support for FEATURE_WCDMA_BAND_PRIORITIZATION.
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
04/11/08   ps      Fixed Compiler warning
03/13/08   vm/pk   Added support for feature FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE.
03/07/08   pk      Added support to maintain scrambling code of the currently camped cell.
                   LLC is required to send correct scr_code to L1 with new DL architecture. 
11/26/07   vm      Changed fn name.
11/21/07   vm      Added support for FEATURE_UEUT_IGNORE_CELL_BAR_RESERVE_STATUS
10/13/07   vm      Added support for feature FEATURE_DISABLE_DEEP_SLEEP. When the feature
                   is defined then no service timer should not be started. So the value of
                   the timer is set to 0 when the feature is defined.                   
10/23/07   pk      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE NETWORK SHARING.
10/12/07   vm      Added #defines for US MCCs
10/08/07   pk      Added prototype for a new function rrc_csp_add_freq_to_available_freq_list().
10/03/07   pk      Removed the stale variable cell_access from rrc_csp_int_data.
09/20/07   vm      Added support for feature FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
09/10/07   pk      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
06/15/07   vm      Added support for FEATURE_CONNECTED_MODE_UMTS_UNIFORM_OOS_HANDLING_UI_UPDATE
06/04/07   pk      Added support for "Partial Roaming Restrictions" under the feature 
                   flag FEATURE SUPPORT FOR PARTIAL ROAMING. For details please see
                   spec CR 151.
05/24/07   pk      Added support for feature FEATURE 900 FREQ SCAN OPTIMIZATION.
05/23/07   pk      Added fix for CR 117463. Now different timers are used for automatic and manual
                   searches under the feature flag FEATURE_MANUAL_SEARCH_FREQ_SCAN_SKIP_TIMER. 
                   Full frequency scans are not done for automatic and manual (both foreground 
                   and background) searches while the respective timers are running.
05/21/07   vm      Added support for the feature FEATURE_RRC_ADDL_CHNL_SCAN_OPTIMIZATION
04/11/07   vm      Added support for feature FEATURE UMTS UNIFORM OOS HANDLING PWR UP ENH
03/16/07   vm      Added fix for CR 109422. Added var plmn_fnd_during_fgnd_svc_srch
                   in rrc_csp_int_data_type structure.
03/04/07   vm      Added support for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2.
11/21/06   vm      Added support for the feature FEATURE UMTS BACKGROUND FULL SERVICE SEARCH.
                   Added new field "service_search" of type "sys_service_search_e_type"
                   in rrc_csp_int_data_type.
11/20/06   vm      Added support for the feature FEATURE_INTER_FREQ_EM_CALL_REDIRECTION.
                   Added prototype of new fn rrccsp_update_acq_type_for_em_call_ifreq_redirection()
                   and added three new variables.
11/16/06   vm      Defined two new constants JAPAN_MCC3 and JAPAN_MCC4.
10/26/06   vm      Added fix for CR 104768. Added new variable bplmn_current_band_initialized.
09/25/06   vm      Added prototype of a new function rrc_convert_band_to_sys_band().
09/25/06   kp      Moved sib bitmask to data.h
09/07/06   vm      Moved a comment inside the feature definition. 
09/05/06   vm      Added support for UOOSC.
08/28/06   vm      Added support for the feature FEATURE_DCH_TO_FACH_PCH_OUT_OF_SERVICE_ENHANCEMENT.
08/25/06   vm      Added support for FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT.
08/23/06   segu    Added the variable curr_lai to store the lai and PLMN information
08/22/06   segu    Added prototype for new function "rrccsp_is_gtow_bplmn_srch_in_progress"
08/09/06   vm      Defined five new constants DEEP_SLEEP_NO_AGC_THRESHOLD_FREQ1,
                   DEEP_SLEEP_NO_AGC_THRESHOLD_FREQ2, DEEP_SLEEP_NO_AGC_THRESHOLD_FREQ3,
                   DEEP_SLEEP_NO_AGC_THRESHOLD_MCC1, and DEEP_SLEEP_NO_AGC_THRESHOLD_MNC1.
                   The constants are defined under the feature name 
                   FEATURE_DEEP_SLEEP_NO_AGC_THRESHOLD_FOR_SPEC_PLMN_FREQ.
07/21/06   vm      Added support for UOOSI.
07/06/06   segu    Added the function prototype rrc_check_if_dsac_is_present()
06/13/06   segu    Added support for DSAC feature implementation
06/08/06   vm      1. Defined two new constants JAPAN_MCC1 and JAPAN_MCC2.
                   2. Added prototype of new function rrccsp_get_curr_type_acq().
05/31/06   segu    Added curr_cell_access_status as member of the structure
                   rrc_csp_int_data_type that stores current cell access status
05/04/06   vm      Added new boolean variable bplmn_dual_mode_enabled to rrc_csp_int_data_type
                   structure. This has been done as part of the support to read dual mode flag
                   from PLMN_LIST_REQ instead of relying on the MCM dual mode flag (which gets 
                   updated whenever service request is received).
05/02/06   sg      Added support for single to dual mode changes
03/30/06   vm      Added support to start the PLMN list full freq scan timer 
                   for 3G to 3G Automatic/Manual PLMN searches also (both 
                   background and foreground). As long as this timer is active 
                   no full scans are done. 
                   Added prototype of a new function
                   rrccsp_check_and_start_plmn_list_full_freq_scan_timer() and changed
                   name of var bplmn_freq_scan_needed to plmn_list_freq_scan_needed.
03/16/06   vm      Changed types of bplmn_req_list_rd_index, bplmn_num_req,
                   bplmn_fnd_list_wr_index, and wtow_bplmn_num_req to uint32 from uint8.
02/16/06   vm      Guard timer has been reduced to 120s from 180s. 
02/14/06   vm      Added fix to save curr_type_acq and type_acq also before
                   starting the BPLMN search. Otherwise, UE may reselect to a
                   non-equivalent PLMN if the frequency corresponding to the PLMN
                   is in the neighbor list. 
                   For this, defined two new variables prev_curr_type_acq and 
                   prev_acq_type.
01/31/06   vm      Replaced SYS_PLMN_LIST_MAX_LENGTH by SYS_PRIORITY_PLMN_LIST_MAX_LENGTH
                   for Automatic BPLMN search requests.
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
01/23/06   kp      Added support for Transaction I.D in BPLMN Abort feature 
12/30/05   sg      Added support for handling BPLMN Abort search from NAS
11/23/05   vm      Added support for feature FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION.
11/16/05   vm      Added more support related to BPLMN features.
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search.
09/28/05   vm      Added support for redirection to another frequency + psc via
                   RRC Connection Setup message. Added new variable redirected_psc
                   in rrc_csp_int_data_type structure.
09/15/05   vm      Moved the declaration of STATIC functions rrccsp_camped_suit_cell_hdlr()
                   and rrccsp_camped_accept_cell_hdlr from rrccspi.h to rrccsp.c to fix
                   compilation errors in other files: rrcccm.c, rrccspdb.c, etc.
09/08/05   siva    Added prototypes of rrccsp_camped_suit_cell_hdlr() and 
                   rrccsp_camped_accept_cell_hdlr()
08/31/05   vm      Added support for scanning frequency in NV. Added two new variables
                   wcdma_dl_freq and wcdma_dl_freq_enabled in rrc_csp_int_data_type
                   structure.
08/16/05   vm      Added support for feature FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION.
                   Added new enum type RRC_CSP_GSM_SCAN for frequency scan and 
                   a new boolean gsm_svc_srch_done in structure rrc_csp_int_data_type.
08/04/05   vm      Added support for scanning again the last camped frequency 
                   during every fine scan when UE goes OOS in Idle mode.
                   Added new variable last_camped_freq_in_idle_mode.
07/15/05   vm      Added support for handling channel config failure in CSP.
                   Added 2 new CSP substates RRCCSP_WT_FOR_CHAN_CFG_CNF and
                   RRCCSP_WT_FOR_CHAN_CFG_CNF_PENDING. Also removed prototype
                   for function rrc_get_csp_currscan() as it is now no longer 
                   used.
07/08/05   vm      FEATURE_OUT_OF_SERVICE_ENHANCEMENT: Added support for OOS in 
                   Connected Mode feature.
                   Added new constant RRCTMR_CONN_MODE_OOS_TIMER_IN_MS; and new variables
                   oos_splmn_srch_tmr_expd, oos_gsm_srch_done, and oos_gsm_num_avail_plmns
                   in rrc_csp_int_data_type structure. Also added prototype for new 
                   function rrc_get_csp_currscan().
07/05/05   vm      Function rrccsp_handle_failure_during_freq_redirection() now does
                   not take any arguments.
06/21/05   vm      Added support for trying acquisition on the best cell in the active set on
                   DCH->Idle transition. Added new frequency scan type RRC_CSP_LAST_ASET_SCAN;
                   and variables aset_scr_code and aset_pn_pos to structure rrc_csp_int_data_type.
06/02/05   vm      Added support for inter-RAT abort handling.
                   (a) Replaced CSP substate name RRCCSP_WT_REDIRECT_ABORT_CNF by
                       a new name RRCCSP_WT_RR_ABORT_CNF as this new common substate
                       will be used for both redirection and reselection abort confirmations.
                   (b) Added new enum type rrc_csp_rsp_to_rr_e_type which will be used
                       to determine if reselection reject should be sent to RR or
                       reselection abort should be sent. This new enum type has been
                       added to rrc_csp_int_data_type.
05/05/05   vm/bu   Added support for T-barred Feature. 
                   (a) Included file rrccsp.h
                   (b) Added a new variable cell_bar_info to rrc_csp_int_data_type structure
                   (c) Added prototype of new function rrc_get_csp_substate()
05/02/05   vm      Added new constant MAX_BPLMN_FREQ_SEL_ATTEMPTS; and new variables
                   bplmn_bgnd_mib_rd_cnt and bplmn_bgnd_mib_rd_freq in rrc_csp_int_data_type
                   structure.
04/14/05   vm      Added prototype of a new function rrccsp_send_cell_update_initiate_req().
                   Additionally, since the feature FEATURE_BARRED_CELL_DURING_CELL_SELECTION 
                   is always defined, un-featurized the code.
04/11/05   bu      Added dchtoidle_flag to rrc_csp_int_data_type.
04/04/05   bu/vm   Added support for inter-RAT redirection feature
                   . Added new scan type RRC_CSP_INTER_RAT_REDIRECTION_SCAN.
                   . Added new CSP substates: RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF,
                     RRCCSP_WT_RESUME_MODE_CHANGE_CNF, RRCCSP_WT_RESPONSE_FROM_RR, and 
                     RRCCSP_WT_REDIRECT_ABORT_CNF.
                   . Replaced boolean variable init_cell_sel_ind_from_cu_during_wtog with
                     init_cell_sel_ind_during_wtog.
                   . Added prototype for new function rrc_csp_cleanup_inter_freq_redirection_state().
03/03/05   vm      Added support for inter-frequency redirection feature. Added
                   new scan type RRC_CSP_INTER_FREQ_REDIRECTION_SCAN; new pending
                   types RRC_FREQ_REDIRECT_WAIT_TIMER_PENDING and RRC_REDIRECT_REQ_PENDING;
                   new variables last_camped_freq, redirected_freq, req_redirect_wait_time,
                   start_acq_on_last_camped_freq, and first_acq_on_redirected_freq in
                   rrc_csp_int_data_type structure; and prototypes for new functions 
                   rrc_csp_start_sel_redirect_freq_in_connecting_state(),
                   rrccsp_handle_freq_redirect_wait_timer_expiry(), 
                   rrc_csp_init_inter_freq_redirection_data(), rrccsp_send_rce_redirect_cnf(),
                   rrccsp_validate_redirection_req(), and
                   rrccsp_handle_failure_during_freq_redirection().
                   Added back protoype for function rrc_csp_init_select_data() as it is now
                   being called outside of rrccsputil.c file (by rrccsp.c).
02/24/05   vm      Added new mask deep_sleep_no_svc_band_scanned_mask in 
                   structure rrc_csp_int_data_type.
02/15/05   vm      Added support for deep sleep. In particular, added new constant 
                   RRC_MAX_FULL_SCANS_IN_DEEP_SLEEP and new variables 
                   full_scan_in_deep_sleep_counter, deep_sleep_acq_db_band_mask, and
                   deep_sleep_band_scanned_mask to rrc_csp_int_data_type. 
02/10/05   sgk     Added SIB_15_4_BIT and SIB_18_BIT for SIB18 support.
01/21/05   vm      Added back timer RRCTMR_DCH_FACH_DUE_TO_CU_CELL_SELECTION_TIMER_IN_MS
                   with new initial value of 180s.
01/13/05   vm      Removed timer RRCTMR_DCH_FACH_DUE_TO_CU_CELL_SELECTION_TIMER_IN_MS.
01/04/05   sk      Added prototype for rrc_csp_send_chan_reconfig(void) function.
12/06/04   vm      Removed declaration of the function rrccsp_send_bplmn_abort_cnf_to_rr( void ).
                   Added it to rrccsp.h.
11/22/04   vm      Added duration RRCTMR_BPLMN_FREQ_SCAN_TIMER_IN_MS for which BPLMN Full Frequency 
                   scan should not be initiated. Added new scan types RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN 
                   and RRC_CSP_GTOW_BPLMN_MANUAL_SCAN. Added new variables to rrc_csp_int_data_type. 
                   Added prototypes for functions rrccsp_update_background_plmn_list, rrc_csp_init_bplmn_data,
                   rrc_csp_start_bplmn_srch_on_rr_req, rrccsp_send_bplmn_abort_cnf_to_rr, 
                   rrccsp_bplmn_update_stored_acq_db_rd_index, rrccsp_send_mode_change_req_to_mcm.
11/22/04   bu      Added RRC_CSP_OUT_OF_SERVICE_AREA_IN_CONNECTING_STATE_SCAN to
                   rrc_csp_scan_e_type. Added prototype for
                   rrc_csp_start_sel_out_of_service_area_in_connecting_state().
11/18/04   vm      Added deep_sleep_no_svc_tmr_expired and deep_sleep_acq_db_plmn_searchloop
                   to rrc_csp_int_data_type.
11/10/04   bu      Added last_camped_freq_in_connected_mode to rrc_csp_int_data_type.
10/21/04   vm      Added declaration of function rrc_csp_release_bch_and_continue_cell_selection(void).
10/12/04   bu      Added addl_chls_scan_needed to rrc_csp_select_ctrl_data_type.
                   Added RRC_CSP_ADDL_CHLS_SCAN to rrc_csp_freq_scan_e_type.
09/24/04   bu      Changed RRC_CSP_MAX_NUM_ACQDB_DEST_FREQ_CYCLES to 5.
09/15/04   bu      Added agc_threshold_cnt to rrc_csp_int_data_type.
09/02/04   bu      Changed the DCH->FACH (due to CU) timer to 40 seconds.
08/31/04   bu      Added wcdma_band_mask instead of booleans to keep track
                   of the frequency bands supported by the UE. Changed
                   current_band and first_band_to_be_scanned to rrc_csp_band_class_types.
                   Added next_band_mask to rrc_csp_select_ctrl_data_type.
08/19/04   sgk     Changed num_freq in raw scan and fine scan in scan_info 
                   structure from int16 to uint16 to remove lint error 
            	     Loss of precision (assignment) (16 bits to 15 bits)
                   in function rrc_csp_update_freq_scan_entry in file 
                   rrccspfscan.c		   
08/13/04   bu      Added WCDMA multiband support. Added fields that indicate whether
                   a particular band is supported or not in rrc_csp_int_data_type.
                   Added current_band and first_band_to_be_scanned in
                   rrc_csp_select_ctrl_data_type.
08/12/04   bu      Defined a 20 seconds timer for DCH->FACH transition due to 
                   RL failure or RLC un-recoverable error. Changed the DCH->FACH
                   (due to reconfiguration message) timer value to 6 seconds.
08/05/04   bu      Added RRC_CSP_INITIATE_CELL_SEL_PENDING to rrc_csp_pending_e_type.
                   Added init_cell_sel_ind_from_cu_during_wtog to rrc_csp_int_data_type.
07/13/04   bu      Added RRCCSP_CELL_SEL_WT_STATE_CHANGE and
                   RRCCSP_CELL_SEL_WT_STATE_CHANGE_PENDING to rrc_csp_substate_e_type.
07/08/04   bu      Added available_freq_list[] and num_available_freq in
                   rrc_csp_select_ctrl_data_type.
06/21/04   bu      Added new CSP states to wait for TRANSITION_CNF for suitable
                   and acceptable cell reselections.
06/14/04   bu      Added ue_in_deep_sleep and acq_db_empty in rrc_csp_int_data_type.
                   Changed FREQ_SCAN_TIMER_IN_MS to 30 mins.
06/01/04   bu      Added band_pref in rrc_csp_int_data_type.
05/21/04   sk      changed timer RRCTMR_FREQ_SCAN_TIMER_IN_MS definition to 30sec.
05/05/04   bu      Defined RRCTMR_FREQ_SCAN_TIMER_IN_MS (default value - 30 mins).
04/02/04   bu      Removed FEATURE definitions from rrc_csp_substate_e_type.
03/22/04   bu      Added scan type CSP_GTOW_CC_ORDER_SCAN for Gtow PCCO.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
03/04/04   bu      Added support for CELL_PCH and URA_PCH states.
02/11/04   bu      Added full_scan_needed and power_up_done to rrc_csp_int_data_type.
02/04/04   bu      Changed the gsm_cell_reselection_ind in rrc_csp_int_data to
                   saved_rrc_cmd. Added RRC_CSP_STATE_CHANGE_IND_PENDING to
                   rrc_csp_pending_e_type.
01/29/04   bu      Added forbidden_lai field to rrc_csp_int_data_type to send
                   in GTOW reselection reject to RR if LAI was found to be forbidden.
12/16/03   bu      Added sys_network_selection_mode_e_type to rrc_csp_int_data_type.
12/16/03   bu      Added sttd_indicator_present & sttd_indicator to rrc_csp_int_data_type.
11/04/03   bu      Changed RRC_CSP_MAX_NUM_ACQDB_DEST_FREQ_CYCLES from 4 to 1.
08/11/03   bu      Added RRC_CSP_REGULAR_CELL_ACCESS_ONLY to
                   rrc_csp_access_control_e_type.
07/16/03   bu      Removed information related to future cell reselection indication
                   from rrc_csp_int_data_type as it is no longer needed.
06/04/03   bu      Added information related to additional cells to be acquired
                   if the strongest cell is barred, to rrc_csp_int_data_type. Removed
                   FEATURE definition around GTOW & WTOG scan types in rrc_csp_scan_e_type.
04/24/03   bu      Changed wcdma_cell_squalmin & wcdma_cell_srxlevmin to int16
                   from uint16 in rrc_csp_int_data_type.
04/22/03   bu      Added RRC_CSP_PLMN_LIST_REQ_RCVD to rrc_csp_mm_primitive_e_type.
                   Added RRC_CSP_PLMN_LIST_REQ_PENDING to rrc_csp_pending_e_type.
                   Added storage of rrc_plmn_list_req_type for later usage in
                   rrc_csp_int_data_type.
04/18/03   bu      Added new state RRCCSP_WT_LOWER_LAYER_DEACT_CNF to
                   rrc_csp_substate_e_type. Added a new scan type
                   RRC_CSP_WCDMA_TO_GSM_CELL_RESELECTION_SCAN.
03/21/03   bu      Removed interrat_any_cell_selection_mode boolean from
                   rrc_csp_int_data_type structure.
02/18/03   vn      Added macros to be used for DCH to FACH cell selection timer,
                   max retries on DCH->FACH directed freq and Acquisition database.
                   Added rrc_csp_pending_e_type to rrc_csp_int_data_type to keep
                   track of any pending commands. Added directed freq and psc to 
                   rrc_csp_int_data_type. Changed names of some CSP substates to make
                   them generic to the pending command.
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to 
                   INTERRAT_HANDOVER_GTOW
02/14/03   bu      Add a new CSP state RRCCSP_CELL_SEL_WT_HANDOVER_ACQ to wait
                   for ACQ_CNF for a GSM to WCDMA handover if
                   FEATURE_INTERRAT_HANDOVER_GTOW_RRC is defined.
01/30/03   vn      Removed compiler flag from around plmn_list in 
                   rrc_csp_int_data_type to fix lint errors.
12/16/02   bu      Defined RRC_MAX_RAW_SCAN_FREQ of 5. This is the maximum
                   number of frequencies that will be scanned during a RAW SCAN.
                   Changes are under feature FEATURE_LIMITED_RAW_SCAN.
12/06/02   bu      Added new CSP states to wait for RRC_ACTIVATION_RSP in
                   suitable and acceptable cell camping for GSM to WCDMA cell
                   reselection. Added function prototype for
                   rrc_csp_send_mm_act_ind.
11/22/02   bu      Added function prototype for rrc_csp_start_sel_for_plmn_list.
11/19/02   bu      Added a new scan type RRC_CSP_PLMN_LIST_SCAN to rrc_csp_scan_e_type
                   for scanning all the frequencies for available PLMNs.
10/11/02   bu      Added support for GSM to WCDMA cell reselection. Added new scan type to
                   rrc_csp_scan_e_type. Added new fields to rrc_csp_int_data_type. Added
                   a new state to wait for STOP_MODE_CHANGE_CNF to rrc_csp_substate_e_type.
                   All changes are under FEATURE_INTERRAT_CELL_RESELECTION_GTOW.
09/30/02   vn      Added a new scan RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN for
                   all cell selection scans when leaving CELL_DCH for CELL_FACH.
                   This could be due to any reason like RL Failure or a directed
                   state transition. Added prototype for functions
                   rrc_csp_send_tr_phy_chl_release and rrc_csp_start_sel_from_dch_to_fach.
09/26/02   bu      Added gsm_cell_reselection_ind in rrc_csp_int_data_type for
                   inter-RAT cell reselection
09/19/02   vn      Added function prototype for rrc_csp_send_l1_drx_req.
09/16/02   bu      Added new CSP states to wait for SUSPEND and RESUME Mode
                   change confirms for Inter-RAT WCDMA to GSM cell reselection while
                   the UE is camped on suitable or acceptable cell. These new
                   states are used only if FEATURE INTERRAT CELL RESELECTION WTOG
                   is defined.
08/02/02   bu      Added new CSP states - RRCCSP_SUIT_CELL_RESELECT_WT_SIB_SRVC_REQ_PENDING,
                   RRCCSP_SUIT_CELL_RESELECT_WT_CAMP_SRVC_REQ_PENDING,
                   RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB_SRVC_REQ_PENDING and
                   RRCCSP_ACCEPT_CELL_RESELECT_WT_CAMP_SRVC_REQ_PENDING to process
                   the SERVICE_REQ in various CSP reselection sub-states.
06/19/02   bu      Removed prototype for rrc_csp_send_mm_act_cnf_cmd() for DUALMODE.
06/14/02   bu      Use mm_as_service_state_T instead of rrc_service_state_e_type
                   for DUALMODE. Added rrc_avail_plmn_list_type to rrc_csp_int_data_type.
                   For DUALMODE use RRC_CSP_SEND_MM_PLMN_LIST_CNF_CMD instead of
                   RRC_CSP_SEND_MM_LAI_LIST_CNF_CMD.
05/16/02   jh      Removed AC info from private data.
05/16/02   bu      Merged the following from cell reselection branch:
           bu      Removed future_cell_acq_entry from rrc_csp_int_data_type.
                   Added future_cell_reselection_ind (rrc_cmd_type) to
                   rrc_csp_int_data_type. Renamed future_cell_reselection to
                   future_cell_reselection_required.
04/15/02   bu      Removed RRCCSP_SUIT_CELL_SEL_WT_NEIGH_S and
                   RRCCSP_SUIT_CELL_SEL_WT_NEIGH_SIB states from rrc_csp_substate_e_type.
                   Removed definitions for rrc_csp_candidate_list_type and
                   rrc_csp_rank_list_type and removed them from rrc_csp_int_data_type.
                   Removed prototypes for rrc_csp_form_cand_list_from_sib11(),
                   rrc_csp_form_interim_cand_list(), rrc_csp_fill_cand_list_s_values(),
                   rrc_csp_form_rank_list_from_cand_list(), rrc_csp_get_next_rank_list_cell(),
                   rrc_csp_get_first_rank_list_cell(), rrc_csp_remove_curr_rank_cell()
                   and rrc_csp_remove_curr_intra_freq_rank_cells().Removed
                   rrc_csp_util_parm_ptr_u_type definition.
12/12/01   bu      Added new CSP states - RRCCSP_SUIT_CELL_RESELECT_WT_CAMP and
                   RRCCSP_ACCEPT_CELL_RESELECT_WT_CAMP. Added a boolean to keep
                   track of any cell reselection indications to be handled in
                   the future and a corresponding rrc_csp_acq_entry for the same.
                   Changed the function prototype for rrc_csp_send_sib_cmd() to
                   include a boolean that indicates whether the cell is a serving
                   cell or not.
11/01/01   vn      Removed macros RRC_CSP_SAME_LAIS and RRC_CSP_NOT_SAME_LAIS
                   as they are not needed with new PLMN matching functions.
10/25/01   vn      Added a new Out of service area scan type applicable when
                   Out of service area happens in Disconnected and Connecting
                   states. Added function prototype for rrc_csp_start_sel_out
                   _of_service_area_in_idle_state().
10/22/01   vn      Support for Out of Service Area - added current scan type
                   and last MM primitive in internal data. Added new CSP
                   substates to process Service request in various substates.
                   Added prototypes for intermediate level functions for starting
                   and continuing cell selection due to various triggers.
                   Removed prototypes for rrc_csp_init_select_data and
                   rrc_csp_start_stored_initial_select_ctrl as they are not
                   directly called from outside rrccsputil.c
10/12/01   vn      Added prototypes of functions that 1.determine which command
                   needs to be sent to MM based on the last CSP event and current
                   service status, 2. find the new CSP event based on the command
                   sent to MM. 3. store data from RRC_SERVICE_REQ command.
09/26/01   vn      Added prototypes for rrc_csp_send_chan_release and
                   rrc_csp_start_sel_leaving_conn_mode.
09/17/01   vn      Added a prototype for rrc_csp_send_mm_lai_list_cnf_cmd that
                   sends a LAI LIST CNF to MM.
08/23/01   vn      Support for frequency scan. Restructured
                   rrc_csp_select_ctrl_data_type into acq db scan info and
                   frequency scan info. Added raw and fine scan info in
                   rrc_csp_select_ctrl_data_type. Added rrc_csp_freq_scan_e_type
                   that lists all types of scans. Updated / Added function interfaces.
08/08/01   vn      Changed rrc_csp_cell_access_e_type to
                   rrc_csp_access_control_e_type to avoid confusion with similar types.
06/22/01   vn      Changed the default UARFCN for the acquisition to 10850 as
                   per the wishes of RF folks.
06/13/01   vn      Added prototype for rrc_csp_send_mm_cmd(). Added a new enum
                   type rrc_csp_last_event_e_type.
06/06/01   vn      Added l1_pccpch_parms_struct_type to rrc_csp_int_data_type.
05/16/01   vn      Added rrc_csp_send_mm_act_cnf_cmd to send a RRC_ACT_CNF
                   with a "NO SERVICE AVAILABLE" status.
03/20/01   vn      Changes to work with rrcmmif.h version 1.14.
                   rrc_csp_int_data_type changed due to changes in RRC_SERVICE_REQ
                   and RRC_SERVICE_CNF command data structures. References to
                   Forbidden RAI list removed from all places. Additional paramters
                   in rrc_csp_send_mm_ser_cnf_cmd to accomodate changes in
                   rrc_service_cnf_type.
03/19/01   vn      Added prototype for rrc_csp_send_llc_cmd.
03/15/01   vn      Changes to acoomodate LED ASN1 header file.
03/02/01   vn      Added a protototype for a utility rrc_csp_form_interim_cand_list
                   to form interim candidate list from Acq entry.
02/08/01   vn      Moved compare_expected_cells from rrccspdb.c
01/26/01   vn      Updated send_l1_cmd() prototype to work with LLC funneling.
01/25/01   vn      Split rrc_csp_stored_initial_select_ctrl to a Start and
                   Continue version for ease with implementation. Added new
                   function prototype for rrc_csp_get_first_rank_list_cell.
01/19/01   vn      Added prototypes for Candidate List, Rank List, Selection
                   Data type and added elements of the above types in Internal
                   Data type. Added prototypes for utilities that form and
                   maintain Candidate List and Rank List, fill Acq entry and
                   send commands to L1, SIB, MM and CCM.
12/21/00   vn      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/


/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "rex.h"
#include "msg.h"
#include "uecomdef.h"

#include "rrcmmif.h"
#include "rrcasn1.h"

#include "rrcdata_v.h"
#include "rrccspdb.h"

#include "l1rrcif.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrccsp.h"
#include "sd.h"

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* Maximum re-direction wait timer value that can be ignored upon expiry */
#define RRC_MAX_NO_ABORT_TIME 1

/* Default Frquency for Initial testing */
#define  DEFAULT_FREQ   10850

/* An Invalid value for frequency */
#define  INVALID_FREQ   0xffff

/* Define a Scrambling Code for Initial testing */
#define  DEFAULT_SCR_CODE  1000

#ifdef FEATURE_EOOS
  #error code not present
#endif

/* This Macro determines if the passed parameters satisfy the Suitable Cell
 * Selection criteria. It returns TRUE or FALSE based on the PLMN status and
 * Cell Status. A cell status of 0 indicates that cell is NOT barred and NOT in forbidden
 * registration area.
 */
#define  RRC_CSP_IN_SUIT_SEL(plmn_status, cell_status)  \
(  \
 (plmn_status == RRC_CSP_PLMN_DIFF_THAN_REQ) ? FALSE : \
   ((cell_status != 0) ? FALSE : TRUE) \
)


/* This macro picks the MM command between an RRC_SERVICE_IND and RRC_SERVICE_CNF
 * based on the last CSP event. If CSP camps on a cell and finds the last event
 * was RRC_CSP_SERVICE_REQ_RCVD, an RRC_SERVICE_CNF is sent to MM.
 * If the last event at the time of camping on a cell is RRC_CSP_CAMPED_ON_CELL_RCVD,
 * it implies the cell was lost after having camped on it last. This time an
 * RRC_SERVICE_IND is to be sent to MM.
 * TBD: A distinction has to be made if the last camped on cell and the newly
 * camped on cell are different. For the time being, RRC_SERVICE_IND is sent
 * irrespective of whether a new cell is selected or the last camped on cell
 * is selected again.
 */
#define   RRC_CSP_SELECT_MM_CMD(last_mm_primitive)    \
(                                                  \
  ((last_mm_primitive) == RRC_CSP_SERVICE_REQ_RCVD) ? RRC_SERVICE_CNF: RRC_SERVICE_IND \
)


/* This defines the max tries to acquire on a cell on a directed frequency */
#define  RRC_CSP_MAX_ACQ_TRIES_ON_DIRECTED_FREQ  6

/* This defines the max number of cycles of tries on destination frequency and
 * acquisition database.
 */
#define  RRC_CSP_MAX_NUM_ACQDB_DEST_FREQ_CYCLES  5

  /* This defines the max time to allow for cell selection 
   * when going from DCH to FACH due to reconfiguration message.
   * As per Spec OOS should be indicated after 4s of being 
   * out of service in CELL_FACH.
   */
  #define  RRCTMR_DCH_FACH_CELL_SELECTION_TIMER_IN_MS  4000UL


  /* This defines the max time to allow for cell selection 
   * when going from FACH to FACH due to reconfiguration message.
   * As per Spec OOS should be indicated after 4s of being 
   * out of service in CELL_FACH.
   */
  #define  RRCTMR_FACH_TO_FACH_CELL_SELECTION_TIMER_IN_MS  4000UL

  /* This defines the max time to allow for cell selection 
   * when going from FACH to PCH/URA_PCH due to reconfiguration message.
   * As per Spec OOS should be indicated after 12s of being 
   * out of service in CELL_FACH.
   */
  #define  RRCTMR_FACH_TO_PCH_CELL_SELECTION_TIMER_IN_MS  12000UL

/* This defines the max time to allow for cell selection
 * when going from DCH to FACH due to RL failure or RLC unrecoverable error
 */
#define  RRCTMR_DCH_FACH_DUE_TO_CU_CELL_SELECTION_TIMER_IN_MS  180000UL


  /* This defines the max time to allow for cell selection 
   * when going from DCH to FACH due to reconfiguration message.
   * As per Spec OOS should be indicated after 12s of being 
   * out of service in CELL_PCH/URA_PCH.
   */
  #define  RRCTMR_DCH_PCH_CELL_SELECTION_TIMER_IN_MS  12000UL

#ifdef FEATURE_EOOS
#error code not present
#endif

#ifdef FEATURE_DISABLE_DEEP_SLEEP 
#define  RRCTMR_DEEP_SLEEP_NO_SVC_TIMER_IN_MS  0
#endif

/* This defines the max time (default - 0 mins) for which BPLMN Frequency scan should not be initiated 
     in case of Manual scan*/
#define  RRCTMR_MANUAL_PLMN_FREQ_SCAN_TIMER_IN_MS  0


/* This defines the max number of attempts for which full frequency scan should be done 
 * after UE enters deep sleep. Full Scan timer is started after these number of attempts
 * are completed.
 */
#define  RRC_MAX_FULL_SCANS_IN_DEEP_SLEEP 3

#define MAX_BPLMN_FREQ_SEL_ATTEMPTS 4

/* This defines the max time (default - 30 seconds) for which Specific 
 * PLMN search should be done on both RATs 
 */
#define  RRCTMR_CONN_MODE_OOS_TIMER_IN_MS  30000

/* Maximum time allowed for completing background PLMN search */
#define  RRCTMR_BPLMN_GUARD_SRCH_TIMER_IN_MS  120000UL

#ifdef FEATURE_DUAL_SIM
#define  RRCTMR_DUAL_SIM_BPLMN_GUARD_SRCH_TIMER_IN_MS  180000UL
#endif

#ifdef FEATURE_WCDMA_TO_LTE  
#define RRCTMR_LTE_BPLMN_GUARD_SRCH_TIMER_IN_MS 150000UL
#endif

/* This is the time substracted from the DRX search timer received from L1
 * to give enough time to L1 to configure PICH on last camped frequency so
 * as to demodulate the PI bits in the paging occasion frame. This overhead
 * is for WTOW BPLMN search
 */
#ifdef FEATURE_DUAL_SIM
/* 70ms for ACQ step and 10ms for PICH setup */
#define WTOW_BPLMN_SEARCH_OVERHEAD_TIMER_WITH_SPLIT_ACQ_IN_MS   80

/* This is the time substracted from the DRX search timer received from GRR/LTE
 * to give enough time to L1 to perform a single ACQ Step(80ms)+W_Start(20ms)+W_Stop(20ms)
 */
#define XTOW_BPLMN_SEARCH_OVERHEAD_TIMER_WITH_SPLIT_ACQ_IN_MS   120

/*Before 60ms of PICH occasion, WRRC should abort SIB reading.
  60ms accounts for BCH Setup/Drop(20ms) + Start/Stop WCDMA(40ms)*/ 
#define XTOW_BPLMN_SIB_DROP_OVERHEAD_TIMER_IN_MS   60

/*Before 30ms of PICH occasion, WRRC should abort SIB reading.
  30ms accounts for BCH Setup+BCH Drop(20ms)+PICH Setup(10ms)*/  
#define WTOW_BPLMN_SIB_DROP_OVERHEAD_TIMER_IN_MS   30
#endif

#define  WTOW_BPLMN_SEARCH_OVERHEAD_TIMER_IN_MS   120

#ifdef FEATURE_DUAL_SIM
/*
  WL1 cannot acquire lock on GO_TO_SLEEP_IND and WL1 not going to sleep.
  So additional time is substracted from DRX timer to make sure WL1 go to sleep and
  acquire lock on wake up
*/
#define  WTOW_BPLMN_SEARCH_ADDTNL_OVERHEAD_TIMER_IN_MS   0
#endif

/* This is the time substracted from the DRX search timer received from L1
 * to give enough time to L1 to configure PICH on last camped frequency so
 * as to demodulate the PI bits in the paging occasion frame. This overhead
 * is for WTOG BPLMN search
 */
#define  WTOG_BPLMN_SEARCH_OVERHEAD_TIMER_IN_MS   95

/* WTOW BPLMN search during the DRX time will be triggered iff the DRX time
 * is at least equal to this value
 */
#define  MIN_WTOW_BPLMN_SRCH_TIMER_IN_MS   300

/* This is the upper bound on DRX search time. If DRX search time
 * received from L1 exceeds this value it implies that there is 
 * some error 
 */
#define  MAX_WTOW_BPLMN_SRCH_TIMER_IN_MS   5120

#ifdef FEATURE_BPLMN_SEARCH_320MS
/* Upto 640ms there is no page miss due to BPLMN searches. Lesser than that
 * there will be page misses if we intend do BPLMN searches. 
 */
#define MIN_WTOW_NORMAL_BPLMN_DRX_CYCLE_LEN 64
#endif

/* Used for mapping GSM ARFCNs to UMTS Channel numbers */
#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
#define GSM_ARFCN_TO_UMTS_CHL_OFFSET_1900_BAND 9139

#define GSM_ARFCN_TO_UMTS_CHL_OFFSET_850_BAND  4218

#define GSM_ARFCN_TO_UMTS_CHL_OFFSET_1800_BAND 639

#define GSM_BASE_BAND_FOR_900_BAND  890

#define GSM_ARFCN_TO_UMTS_CHL_OFFSET_900_BAND  340
#endif

#define INVALID_LIST_TRANS_ID 0xFF

#define JAPAN_MCC1 440
#define JAPAN_MCC2 441
#define JAPAN_MCC3 442
#define JAPAN_MCC4 443

#define USA_MCC_START_RANGE 310
#define USA_MCC_END_RANGE   316
#define USA_MCC1   332

#define MNC1 10


  #define DEEP_SLEEP_NO_AGC_THRESHOLD_FREQ1 4387
  #define DEEP_SLEEP_NO_AGC_THRESHOLD_FREQ2 4412
  #define DEEP_SLEEP_NO_AGC_THRESHOLD_FREQ3 4436
  #define DEEP_SLEEP_NO_AGC_THRESHOLD_MCC1 505
  #define DEEP_SLEEP_NO_AGC_THRESHOLD_MNC1 1

  /* Initial Tmins period during which UE searches for service 
   * continuously without going to deep sleep
   */
  #define  RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER_IN_MS 120000

  /* Determines the duration for which UE should remain in deep
   * sleep
   */
  #define  RRCTMR_CONN_MODE_OOS_DS_TIMER_IN_MS 30000

  /* Determines the time duration for which frequency should be barred due to LA reject 
   * received for the registration from the N/W with the cause "forbidden LAs for roaming"
   */
  #define FORBIDDEN_RESEL_FREQ_BAR_TIMER_IN_SEC 300

  #define  RRCTMR_GTOW_REDIRECTION_WAIT_TIMER_IN_MS 10000

  #define RRCTMR_GTOW_REDIRECTION_LFS_TIMER_IN_MS 2000

#define RRC_MAX_NUM_FREQ 20
/* Maximum number of possible overlapping frequencies is set to Ten */
#define RRC_MAX_OVERLAP_FREQ 10

#define MAX_NUM_OF_WCDMA_BANDS 10

/* WTOG abort is supposed to complete in 2sec*/
#define RRCTMR_WTOG_ABORT_GUARD_TIMER_IN_MS 2000

#define RRC_CSP_MAX_PLMN_SRCH_RATS 3

#ifdef FEATURE_WRLF_SYSTEM_SEL
#define RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS()  (rrc_csp_int_data.ue_in_conn_oos)
#endif

#ifdef FEATURE_LTE_TO_WCDMA
#define RRCCSP_LTOW_MAX_ACQ_TRIES_ON_DIRECTED_FREQ 3
#define RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER_IN_MS 1000
#define RRCTMR_LTE_TO_WCDMA_REDIR_EXT_SRCH_TIMER_IN_MS 5000
#define RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER_FOR_CSFB_IN_MS 25000
#endif

#ifdef FEATURE_FEMTO_CSG
#define RRC_CSG_DISCONN_TRANS_ASF_SRCH_TIMER_IN_MS 10000
#endif
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* This enum lists the possibilities for Cell barred status */
typedef enum
{
  /* This indicates that the cell is not barred and UE may select/reselect
   * in Idle or Connected mode.
   */
  RRC_CSP_CELL_OK,

  /* This indicates that UE shall treat this cell as Barred and CAN NOT
   * select another cell on the same frequency.
   */
  RRC_CSP_CELL_BAR_NO_INTRA_FREQ,

  /* This indicates that UE shall treat this cell as Barred and CAN
   * select another cell on the same frequency.
   */
  RRC_CSP_CELL_BAR_INTRA_FREQ
}rrc_csp_cell_bar_e_type;



/* This enum lists the possibilities for Cell Access restrictions */
typedef enum
{
  /* This indicates all kinds of Cell Access are allowed */
  RRC_CSP_CELL_ACCESS_ALLOWED,
  /* This indicates that cell access is not allowed for any regular call. It
   * is however allowed for emergency calls.
   */
  RRC_CSP_NO_REGULAR_CELL_ACCESS,
  /* This indicates Cell Access is NOT allowed for ANY call (Regular or
   * Emergency.
   */
  RRC_CSP_NO_CELL_ACCESS,
  /* This indicates that cell access is allowed for regular calls but not
   * for emergency calls
   */
  RRC_CSP_REGULAR_CELL_ACCESS_ONLY

}rrc_csp_access_control_e_type;


/* This enum lists the possibilities of LAI forbidden status */
typedef enum
{
  /* This indicates that this is a Forbidden LAI */
  RRC_CSP_FORBID_LAI,
  /* This indicates that LAI is NOT forbidden */
  RRC_CSP_LAI_OK
}rrc_csp_lai_status_e_type;


/* This enumerates all possible primitives exchanged between MM
 * and RRC Cell selection that can have a bearing on the next
 * RRC to MM Primitive.
 */
typedef enum
{
  RRC_CSP_NO_EVENT_RCVD,
  RRC_CSP_ACT_REQ_RCVD,
  RRC_CSP_SRVC_REQ_RCVD,
  RRC_CSP_SRVC_CNF_WITH_NO_SRVC_SENT,
  RRC_CSP_SRVC_CNF_WITH_SRVC_SENT,
  RRC_CSP_ACT_CNF_SENT,
  RRC_CSP_SRVC_IND_WITH_NO_SRVC_SENT,
  RRC_CSP_SRVC_IND_WITH_SRVC_SENT,
  RRC_CSP_PLMN_LIST_REQ_RCVD
}rrc_csp_mm_primitive_e_type;


/* This enumerates the possible frequency scan types */
typedef enum
{
  RRC_CSP_NO_SCAN,
  RRC_CSP_RAW_SCAN,
  RRC_CSP_FINE_SCAN,
  RRC_CSP_ACQ_SCAN,
  RRC_CSP_ADDL_CHLS_SCAN,
  RRC_CSP_LAST_ASET_SCAN,   /* Used for determining if acquisition should be attempted
                              on the best cell in the active set for DCH->Idle transition before
                              trying ACQ DB */
  RRC_CSP_GSM_SCAN,
  RRC_CSP_LIST_SCAN

}rrc_csp_freq_scan_e_type;


/* Enumerates the various scan types */
typedef enum
{
  /* 0: Scan triggered by one of MM commands (Service Req/Act Req etc) */
  RRC_CSP_CELL_SELECTION_SCAN,

  /* 1: Scan triggered by Reselection Indication */
  RRC_CSP_CELL_RESELECTION_SCAN,

  /* 2: Scan triggered by Out of Service Indication */
  RRC_CSP_OUT_OF_SERVICE_AREA_SCAN,

  /* 3: Scan triggered by Out of Service Indication in Idle state */
  RRC_CSP_OUT_OF_SERVICE_AREA_IN_IDLE_STATE_SCAN,

  /* 4: Scan triggered by Out of Service Indication in Idle state */
  RRC_CSP_OUT_OF_SERVICE_AREA_IN_CONNECTING_STATE_SCAN,

  /* 5: Scan triggered when leaving CELL_DCH state for CELL_FACH state */
  RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN,

  /* 6: Scan triggered when leaving CELL_DCH state for CELL_PCH state */
  RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN,

  /* 7: Scan triggered when leaving CELL_DCH state for CELL_PCH state */
  RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN,

  /* 8: Scan triggered on leaving connected mode */
  RRC_CSP_LEAVING_CONN_MODE_SCAN,

  /* 9: Scan triggered by GSM RR */
  RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN,

  /* 10: Scan to GSM triggered by WCDMA */
  RRC_CSP_WCDMA_TO_GSM_CELL_RESELECTION_SCAN,

  /* 11: Scan triggered by PLMN_LIST_REQ */
  RRC_CSP_PLMN_LIST_SCAN,

  /* 12: Scan triggered by GSM RR for Packet Cell Change Order to UTRAN */
  RRC_CSP_GTOW_CC_ORDER_SCAN,

  /* 13: Scan triggered by GSM RR for Automatic Background PLMN search */
  RRC_CSP_GTOW_BPLMN_AUTOMATIC_SCAN,

  /* 14: Scan triggered by GSM RR for Manual PLMN search */
  RRC_CSP_GTOW_BPLMN_MANUAL_SCAN,

  /* 15: Scan triggered by Inter Frequency Redirection request from RCE */
  RRC_CSP_INTER_FREQ_REDIRECTION_SCAN,

  /* 16: Scan triggered by WCDMA RRC for WTOG service redirection */
  RRC_CSP_INTER_RAT_REDIRECTION_SCAN,

  /* 17: Scan triggered by MM for 3G->3G Automatic Background PLMN search */
  RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN,

  /* 18: Scan triggered by MM for 3G->3G Manual PLMN search */
  RRC_CSP_WTOW_BPLMN_MANUAL_SCAN,

  /* 19: Scan triggered by MM for 3G->2G Automatic Background PLMN search */
  RRC_CSP_WTOG_BPLMN_AUTOMATIC_SCAN,

  /* 20: Scan triggered by MM for 3G->2G Manual PLMN search */
  RRC_CSP_WTOG_BPLMN_MANUAL_SCAN,

  /* 21: Scan triggered when leaving CELL_FACH state for CELL_FACH state */
  RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN,

  /* 22: Scan triggered when leaving CELL_FACH state for CELL_PCH state */
  RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN,

  /* 23: Scan triggered when leaving CELL_FACH state for URA_PCH state */
  RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN,

  /* 24: Reselection to GAN Scan*/
  RRC_CSP_WCDMA_TO_GAN_CELL_RESELECTION_SCAN,

  /* 25: Scan triggered when leaving CELL_FACH state for URA_PCH state */
  RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN,

  /*26 Scan triggered for reselection from GAN request*/
  RRC_CSP_GAN_TO_WCDMA_RESELECTION_SCAN,

  /*27 Go to Idle Detached mode scan*/
  RRC_CSP_GO_TO_IDLE_DETACHED_SCAN,

  /*28 GAN ACT REQ Scan*/
  RRC_CSP_GAN_ACT_REQ_SCAN
#ifdef FEATURE_DUAL_SIM
  /*29 UMTS UNLOCK BY Scan */
  ,RRC_CSP_UNLOCK_BY_SCAN
#endif
}rrc_csp_scan_e_type;

/* Enumerates various types of pending commands */
typedef enum
{
  RRC_CSP_SERVICE_REQ_PENDING,
  RRC_CSP_DCH_TO_FACH_TIMER_PENDING,
  RRC_CSP_PLMN_LIST_REQ_PENDING,
  RRC_CSP_STATE_CHANGE_IND_PENDING,
  RRC_CSP_DCH_TO_PCH_TIMER_PENDING,
  RRC_CSP_INITIATE_CELL_SEL_PENDING,
  RRC_BPLMN_SRCH_TIMER_PENDING,
  RRC_INTERRAT_PLMN_SRCH_ABORT_REQ_PENDING,
  RRC_FREQ_REDIRECT_WAIT_TIMER_PENDING,
  RRC_REDIRECT_REQ_PENDING, 
  RRC_CSP_WTOW_BPLMN_DRX_SRCH_TIMER_PENDING,
  RRC_CSP_BPLMN_GUARD_SRCH_TIMER_PENDING,
  RRC_CSP_BPLMN_SRCH_SUSPEND_REQ_PENDING,
  RRC_CSP_BPLMN_SRCH_ABORT_REQ_PENDING,
#ifdef FEATURE_EOOS
  #error code not present
#endif
  RRC_CSP_FACH_TO_FACH_PCH_TIMER_PENDING,
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif
#ifdef FEATURE_DUAL_SIM
  RRC_CSP_PREEMPT_IND_PENDING,
#endif
  RRC_CSP_MODE_CHANGE_REQ_PENDING,

  RRC_CSP_NONE_PENDING
}rrc_csp_pending_e_type;

/* Defines all Substates of Cell Selection Procedure. */

typedef enum{
  /* No cell is selected */
  RRCCSP_NO_CELL_SELECTED,
  /* Waiting for frequency scan */
  RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF,
  /* Waiting for frequency scan
     while a Service Request is pending to be serviced */
  RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF_PENDING,
  /* Waiting for Acq. Confirm */
  RRCCSP_CELL_SEL_WT_ACQ,
  /* Waiting for Acq. Confirm
     while a Service Request is pending to be serviced */
  RRCCSP_CELL_SEL_WT_ACQ_PENDING,
  /* Waiting for SIB Confirm */
  RRCCSP_CELL_SEL_WT_SIB,
  /* Waiting for SIB Confirm */
  RRCCSP_CELL_SEL_WT_SIB_PENDING,
  /* Waiting for State Change Indication to IDLE */
  RRCCSP_CELL_SEL_WT_STATE_CHANGE,
  /* Waiting for State Change Indication while a MM request is pending */
  RRCCSP_CELL_SEL_WT_STATE_CHANGE_PENDING,
  /* Suitable cell selection waiting for Serving cell "S values" */
  RRCCSP_SUIT_CELL_SEL_WT_SERV_S,
  /* Suitable cell selection waiting for Serving cell "S values"
     while a Service Request is pending to be serviced */
  RRCCSP_SUIT_CELL_SEL_WT_SERV_S_PENDING,
  /* Suitable cell selection waiting for Camping Confirm */
  RRCCSP_SUIT_CELL_SEL_WT_CAMP,
  /* Suitable cell selection waiting for Camping Confirm
     while a Service Request is pending to be serviced */
  RRCCSP_SUIT_CELL_SEL_WT_CAMP_PENDING,
  /* Camped on Suitable Cell */
  RRCCSP_CAMPED_SUIT_CELL,
  /* Acceptable cell selection waiting for "S" Value */
  RRCCSP_ACCEPT_CELL_SEL_WT_S,
  /* Acceptable cell selection waiting for "S" Value
     while a Service Request is pending to be serviced */
  RRCCSP_ACCEPT_CELL_SEL_WT_S_PENDING,
  /* Acceptable cell selection waiting for Camping Confirm */
  RRCCSP_ACCEPT_CELL_SEL_WT_CAMP,
  /* Acceptable cell selection waiting for Camping Confirm
     while a Service Request is pending to be serviced */
  RRCCSP_ACCEPT_CELL_SEL_WT_CAMP_PENDING,
  /* Camped on Acceptable Cell */
  RRCCSP_CAMPED_ACCEPT_CELL,
  /* Suitable cell reselection waiting for SIB Confirm */
  RRCCSP_SUIT_CELL_RESELECT_WT_SIB,
  /* Acceptable cell reselection waiting for SIB Confirm */
  RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB,
  /* Suitable cell reacquisition */
  RRCCSP_SUIT_CELL_REACQ,
  /* Acceptable cell reacquisition */
  RRCCSP_ACCEPT_CELL_REACQ,
  /* Suitable cell reselection waiting for Camping Confirm */
  RRCCSP_SUIT_CELL_RESELECT_WT_CAMP,
  /* Acceptable cell reselection waiting for Camping Confirm */
  RRCCSP_ACCEPT_CELL_RESELECT_WT_CAMP,
  /* Suitable cell reselection waiting for SIB Confirm
     while a Service Request is pending to be serviced */
  RRCCSP_SUIT_CELL_RESELECT_WT_SIB_PENDING,
  /* Suitable cell reselection waiting for Camping Confirm
     while a Service Request is pending to be serviced */
  RRCCSP_SUIT_CELL_RESELECT_WT_CAMP_PENDING,
  /* Acceptable cell reselection waiting for SIB Confirm
     while a Service Request is pending to be serviced */
  RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB_PENDING,
  /* Acceptable cell reselection waiting for Camping Confirm
     while a Service Request is pending to be serviced */
  RRCCSP_ACCEPT_CELL_RESELECT_WT_CAMP_PENDING,
  /* Waiting for Suspend Mode Change Confirm */
  RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF,
  /* Waiting for Resume Mode Change Confirm */
  RRCCSP_WT_RESUME_MODE_CHANGE_CNF,
  /* Waiting for Resume Mode Change Confirm in pending */
  RRCCSP_WT_RESUME_MODE_CHANGE_CNF_PENDING,
  /* Waiting for channel config confirm after W resume has succeeded */
  RRCCSP_WT_FOR_CHAN_CFG_CNF,
  /* Waiting for channel config confirm after W resume has succeeded 
   * and RRC_SERVICE_REQ or PLMN_LIST_REQ is received in CSP substate
   * RRCCSP_WT_FOR_CHAN_CFG_CNF */
  RRCCSP_WT_FOR_CHAN_CFG_CNF_PENDING,
  /* CSP waits for RRC_CPHY_CELL_TRANSITION_CNF from L1 */
  RRCCSP_SUIT_CELL_RESELECT_WT_TRANS_CNF,
  RRCCSP_ACCEPT_CELL_RESELECT_WT_TRANS_CNF,
  RRCCSP_WT_STOP_MODE_CHANGE_CNF,
  RRCCSP_SUIT_CELL_WT_ACTIVATION_RSP,
  RRCCSP_ACCEPT_CELL_WT_ACTIVATION_RSP,
  RRCCSP_CELL_SEL_WT_HANDOVER_ACQ,
  /* CSP waits for RRC_LOWER_LAYER_DEACT_CNF from LLC */
  RRCCSP_WT_LOWER_LAYER_DEACT_CNF,

  /* CSP waits for Redirection response from RR */
  RRCCSP_WT_RESPONSE_FROM_RR,
  /* CSP waits for abort cnf from RR for redirection or reselection */
  RRCCSP_WT_RR_ABORT_CNF,

  RRCCSP_WT_IDLE_DETACHED_MODE_CHANGE_CNF,
  /* Maximum number of substates of Cell selection procedure */
  RRCCSP_CSP_MAX_SUBSTATES

}rrc_csp_substate_e_type;


/* Below #defines are for logging purpose only */
#define rrc_csp_substate_e_type_value0 RRCCSP_NO_CELL_SELECTED
/* Waiting for frequency scan */
#define rrc_csp_substate_e_type_value1 RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF
/* Waiting for frequency scan
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value2 RRCCSP_CELL_SEL_WT_FREQ_SCAN_CNF_PENDING
/* Waiting for Acq. Confirm */
#define rrc_csp_substate_e_type_value3 RRCCSP_CELL_SEL_WT_ACQ
/* Waiting for Acq. Confirm
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value4 RRCCSP_CELL_SEL_WT_ACQ_PENDING
/* Waiting for SIB Confirm */
#define rrc_csp_substate_e_type_value5 RRCCSP_CELL_SEL_WT_SIB
/* Waiting for SIB Confirm */
#define rrc_csp_substate_e_type_value6 RRCCSP_CELL_SEL_WT_SIB_PENDING
/* Waiting for State Change Indication to IDLE */
#define rrc_csp_substate_e_type_value7 RRCCSP_CELL_SEL_WT_STATE_CHANGE
/* Waiting for State Change Indication while a MM request is pending */
#define rrc_csp_substate_e_type_value8 RRCCSP_CELL_SEL_WT_STATE_CHANGE_PENDING
/* Suitable cell selection waiting for Serving cell "S values" */
#define rrc_csp_substate_e_type_value9 RRCCSP_SUIT_CELL_SEL_WT_SERV_S
/* Suitable cell selection waiting for Serving cell "S values"
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value10 RRCCSP_SUIT_CELL_SEL_WT_SERV_S_PENDING
/* Suitable cell selection waiting for Camping Confirm */
#define rrc_csp_substate_e_type_value11 RRCCSP_SUIT_CELL_SEL_WT_CAMP
/* Suitable cell selection waiting for Camping Confirm
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value12 RRCCSP_SUIT_CELL_SEL_WT_CAMP_PENDING
/* Camped on Suitable Cell */
#define rrc_csp_substate_e_type_value13 RRCCSP_CAMPED_SUIT_CELL
/* Acceptable cell selection waiting for "S" Value */
#define rrc_csp_substate_e_type_value14 RRCCSP_ACCEPT_CELL_SEL_WT_S
/* Acceptable cell selection waiting for "S" Value
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value15 RRCCSP_ACCEPT_CELL_SEL_WT_S_PENDING
/* Acceptable cell selection waiting for Camping Confirm */
#define rrc_csp_substate_e_type_value16 RRCCSP_ACCEPT_CELL_SEL_WT_CAMP
/* Acceptable cell selection waiting for Camping Confirm
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value17 RRCCSP_ACCEPT_CELL_SEL_WT_CAMP_PENDING
/* Camped on Acceptable Cell */
#define rrc_csp_substate_e_type_value18 RRCCSP_CAMPED_ACCEPT_CELL
/* Suitable cell reselection waiting for SIB Confirm */
#define rrc_csp_substate_e_type_value19 RRCCSP_SUIT_CELL_RESELECT_WT_SIB
/* Acceptable cell reselection waiting for SIB Confirm */
#define rrc_csp_substate_e_type_value20 RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB
/* Suitable cell reacquisition */
#define rrc_csp_substate_e_type_value21 RRCCSP_SUIT_CELL_REACQ
/* Acceptable cell reacquisition */
#define rrc_csp_substate_e_type_value22 RRCCSP_ACCEPT_CELL_REACQ
/* Suitable cell reselection waiting for Camping Confirm */
#define rrc_csp_substate_e_type_value23 RRCCSP_SUIT_CELL_RESELECT_WT_CAMP
/* Acceptable cell reselection waiting for Camping Confirm */
#define rrc_csp_substate_e_type_value24 RRCCSP_ACCEPT_CELL_RESELECT_WT_CAMP
/* Suitable cell reselection waiting for SIB Confirm
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value25 RRCCSP_SUIT_CELL_RESELECT_WT_SIB_PENDING
/* Suitable cell reselection waiting for Camping Confirm
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value26 RRCCSP_SUIT_CELL_RESELECT_WT_CAMP_PENDING
/* Acceptable cell reselection waiting for SIB Confirm
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value27 RRCCSP_ACCEPT_CELL_RESELECT_WT_SIB_PENDING
/* Acceptable cell reselection waiting for Camping Confirm
   while a Service Request is pending to be serviced */
#define rrc_csp_substate_e_type_value28 RRCCSP_ACCEPT_CELL_RESELECT_WT_CAMP_PENDING
/* Waiting for Suspend Mode Change Confirm */
#define rrc_csp_substate_e_type_value29 RRCCSP_WT_SUSPEND_MODE_CHANGE_CNF
/* Waiting for Resume Mode Change Confirm */
#define rrc_csp_substate_e_type_value30 RRCCSP_WT_RESUME_MODE_CHANGE_CNF
/* Waiting for Resume Mode Change Confirm in pending */
#define rrc_csp_substate_e_type_value31 RRCCSP_WT_RESUME_MODE_CHANGE_CNF_PENDING
/* Waiting for channel config confirm after W resume has succeeded */
#define rrc_csp_substate_e_type_value32 RRCCSP_WT_FOR_CHAN_CFG_CNF

#define rrc_csp_substate_e_type_value33 RRCCSP_WT_FOR_CHAN_CFG_CNF_PENDING
/* CSP waits for RRC_CPHY_CELL_TRANSITION_CNF from L1 */
#define rrc_csp_substate_e_type_value34 RRCCSP_SUIT_CELL_RESELECT_WT_TRANS_CNF
#define rrc_csp_substate_e_type_value35 RRCCSP_ACCEPT_CELL_RESELECT_WT_TRANS_CNF
#define rrc_csp_substate_e_type_value36 RRCCSP_WT_STOP_MODE_CHANGE_CNF
#define rrc_csp_substate_e_type_value37 RRCCSP_SUIT_CELL_WT_ACTIVATION_RSP
#define rrc_csp_substate_e_type_value38 RRCCSP_ACCEPT_CELL_WT_ACTIVATION_RSP
#define rrc_csp_substate_e_type_value39 RRCCSP_CELL_SEL_WT_HANDOVER_ACQ
/* CSP waits for RRC_LOWER_LAYER_DEACT_CNF from LLC */
#define rrc_csp_substate_e_type_value40 RRCCSP_WT_LOWER_LAYER_DEACT_CNF

/* CSP waits for Redirection response from RR */
#define rrc_csp_substate_e_type_value41 RRCCSP_WT_RESPONSE_FROM_RR
/* CSP waits for abort cnf from RR for redirection or reselection */
#define rrc_csp_substate_e_type_value42 RRCCSP_WT_RR_ABORT_CNF

#define rrc_csp_substate_e_type_value43 RRCCSP_WT_IDLE_DETACHED_MODE_CHANGE_CNF
/* Maximum number of substates of Cell selection procedure */
#define rrc_csp_substate_e_type_value44 RRCCSP_CSP_MAX_SUBSTATES

/* Used by RRC to send appropriate command to RR in response to 
 * reselections, aborts, HO, and CCO 
 */
typedef enum
{
  /* Used by RRC to send appropriate reselection reject command */
  SEND_GTOW_RESEL_REJECT,

  /* Used by RRC to send appropriate reselection abort confirmation command */
  SEND_GTOW_RESEL_ABORT_CNF

  /* Used by RRC to send appropriate redirection reject command */
  ,SEND_GTOW_REDIRECTION_REJECT

  /* Used by RRC to send appropriate redirection abort confirmation command */
  ,SEND_GTOW_REDIRECTION_ABORT_CNF

}rrc_csp_rsp_to_rr_e_type;


/* Used for initializing the variables associated with 3G->3G,2G background
 * PLMN Search during the search.
 */
typedef enum
{
  /* Automatic PLMN LIST REQ received from NAS          */
  WTOW_SEARCH_START,    

  /* DRX search timer expired or some other procedure
   * wants to become active                             */
  WTOW_SEARCH_SUSPEND,  

  /* Continue the search                                */
  WTOW_SEARCH_RESUME,    

  /* PLMN LIST Search completed                         */
  WTOW_SEARCH_STOP,     

  /* Guard timer expired or state changed to DCH or OOS */
  WTOW_SEARCH_ABORT,
  
  /*Provide Partial PLMN Search Results to NAS*/
  WTOW_SEARCH_PARTIAL
  
} wtow_bplmn_srch_status_type;


/* This is used between CSP functions for passing SIB bit mask */
typedef  uint32  rrc_csp_sib_mask_type;

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)

typedef enum
{
  RRC_CSP_INTERRAT_NONE,
  RRC_CSP_INTERRAT_GSM,
  RRC_CSP_INTERRAT_LTE,
  RRC_CSP_INTERRAT_LTE_CGI,
  RRC_CSP_INTERRAT_MAX
} rrc_csp_interrat_rat_e_type;

#endif

typedef struct
{
  /*Number of RATS*/
  uint8 num_rats;
  sys_radio_access_tech_e_type rat[RRC_CSP_MAX_PLMN_SRCH_RATS];
  int8 curr_rat_srch_idx;
}rrc_csp_plmn_srch_rat_order_type;

#ifdef FEATURE_FEMTO_CSG
typedef struct
{
  rrc_plmn_identity_type  plmn;
  rrc_int_cell_id_type    cell_id;
  l1_meas_csg_list_type   csg_cell_l1_info;
}rrc_csp_csg_cells_info_type;

typedef struct
{
  uint8 num_entries;
  rrc_csp_csg_cells_info_type  csg_cell_info[MAX_ASF_CELLS];
}rrc_csp_temp_csg_cell_list;

typedef struct
{
  uint16             freq;     /*!< Frequency in 100KHZ*/
  uint16             pci;   /*!< Physical Cell Id */    
  uint32             cell_id; /*!< Global cell identity*/
  rrc_plmn_identity_type plmn;          /*!< PLMN */
  uint32 csg_id;
}rrc_csp_fp_macro_info_type;

typedef enum
{
  RRC_CSG_NONE,
  RRC_CSG_WCDMA,
  RRC_CSG_LTE,
  RRC_CSG_GSM
}rrc_csp_csg_rat;

#endif

/* This data structure stores all current global data used internally by Cell
 * Selection. Most of this data comes in commands from MM and from SIBs.
 */
typedef struct
{
  /* Last Cell selection event - determines next MM response */
  /* Note that this is updated at the time of sending an MM command and at
   * the time of receiving a command from MM.
   */
  rrc_csp_mm_primitive_e_type    last_mm_primitive;

  /* The current scan type */
  /* Note that this is updated when a scan is triggered by an event.
   */
  rrc_csp_scan_e_type          curr_scan;

  /* The Home PLMN for this UE */
  /* TBD RRC has to call an MM function that returns the Home PLMN.
   */
  rrc_plmn_identity_type    home_plmn_id;

  /* List of Forbidden LAIs */
  rrc_lai_list_type         forbid_lai_list;

  /* List of Reject LAIs */
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  lai_reject_list_T         lai_reject_list;

#ifdef FEATURE_DUAL_SIM
  boolean                   sm_ok_to_tuneaway;
#endif
#endif

  /* Current PLMN requested by MM in Service Req. message */
  rrc_requested_plmn_type   curr_req_plmn;

  mm_as_service_state_T       curr_service_status;

  /* Current PLMN selected by RRC.
   * It is valid once UE camps on the cell or is unable to camp on any cell.
   */
  rrc_selected_plmn_type    curr_sel_plmn;

  /* Current type of acq.- This indicates the expected acq. type.
   */
  rrc_csp_acq_e_type        curr_type_acq;

  /* Current type of acq. - calculated based on OTA SIBs */
  rrc_csp_acq_e_type        curr_type_acq_ota;

  /* Current Acq entry - stores all data for the current acquisition.
   */
  rrc_csp_acq_entry_type    curr_acq_entry;

  /* New Cell Acq entry - stores all data for the new cell to be reselected.
   * The cell is provided by L1 in the RESELECTION_IND
   */
  rrc_csp_acq_entry_type    new_cell_acq_entry;

  /* This stores parameters for serving SCCPCH */
  l1_pccpch_parms_struct_type   serving_pccpch_parms;

  /* This indicates the type of pending command */
  rrc_csp_pending_e_type   pending_type;
  /* Service Request received and still pending */
  rrc_service_req_type     pending_srvc_req_data;

  rrc_plmn_list_req_type   pending_plmn_list_req_data;

#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif

  /* Avaliable PLMN list */
  rrc_avail_plmn_list_type  plmn_list;

  rrc_cmd_type              saved_rrc_cmd;


  /* Params needed to be set while sending a INTERRAT_RESELECT_REJ to RR or RESEL_FAILED_RSP to LTE RRC */

  /* Cause of InterRAT Reselection Failure */
  rrc_interrat_reject_reason_T   interrat_reselect_failure_reason;

  /* Time ( in Sec ) when the same cell should be considered */
  uint32                         wcdma_cell_time_to_reselect;

  /* Thresholds to be applied to Ec/No measurements */
  int16                         wcdma_cell_squalmin;
  int16                         wcdma_cell_srxlevmin;

  rrc_lai_type                  forbidden_lai;

  boolean                        dest_freq_present;
  rrc_freq_type                  dest_freq;
  boolean                        dest_psc_present;
  rrc_scr_code_type              dest_psc;

  /* PN position of the cell in chipx1 units. */
  uint32                   pn_pos;

  /* Number of other scrambling codes received in ACQ_CNF */
  uint8                    num_scr_code_det;

  l1_acq_scr_code_det      scr_code_det[L1_WCDMA_ACQ_DET_LIST_MAX];

  /* Current Acquisition count */
  uint8                    curr_acq_count;

  l1_wcdma_acq_mode_type   acq_mode;

  l1_wcdma_acq_mech_type   acq_type;

  /* TRUE means "sttd_indicator" value is present.  FALSE otherwise. */
  boolean                 sttd_indicator_present;

  /* If present, TRUE means the cell is using Space Time Transmit Diversity.
   ** FALSE means it is not.  Described in TS 25.331, 10.3.6.78. */
  boolean                 sttd_indicator;

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  /* User band preference */
  sys_band_mask_type       band_pref;

  /* User band preference */
  sys_band_mask_type       prev_band_pref;
  
  /* Indicates if full frequency scan is needed or not */
  boolean                  full_scan_needed;
#ifdef FEATURE_EOOS
#error code not present
#endif
  /* boolean indicating if UE is in deep sleep or not */
  boolean                  ue_in_deep_sleep;

  /* boolean indicating if UE's deep sleep no service timer has expired or not */
  boolean                  deep_sleep_no_svc_tmr_expired;

  /* boolean used for state keeping for multiple calls to rrc_csp_get_match_acq_entry() function */
  boolean                  deep_sleep_acq_db_plmn_searchloop;

  /* if UE is in deep sleep then do full scan till the counter reaches a maximum value */
  uint8                    full_scan_in_deep_sleep_counter;

  /* For WCDMA multiband support, maintain a bit mask of 
   * supported frequency bands present in ACQ DB
   */
  rrc_csp_band_class_type  deep_sleep_acq_db_band_mask;
  
  /* When UE is in deep sleep and full scan timer expires then
   * at least 1 scan should be done on each supported band.
   * Since SD sends different bands as supported in different
   * service requests, this mask maintains a list of bands that
   * have been scanned thus guaranteeing 1 full scan on each supported
   * band on timer expiry.
   */
  rrc_csp_band_class_type  deep_sleep_band_scanned_mask;
  
  /* This mask maintains the bands that have been serviced since
   * the start of No Service timer. The reason for introducing this
   * mask is that RRC may get service request for a band that is different
   * from the ones received earlier when the timer was started/active.
   * Such bands which have not been serviced since the start of no service
   * timer should be serviced and hence the need for this mask.
   */
  rrc_csp_band_class_type  deep_sleep_no_svc_band_scanned_mask;

  /* Variable indicating if ACQ DB is empty - For specific PLMN request this
   * variable is set to TRUE if no matching entry was found. For ANY PLMN it is
   * set to TRUE if there are no entries in the database at all.
   */
  boolean                  acq_db_empty;

  /* boolean indicating if an INITIATE_CELL_SELECTION_IND (to disconnected)
   * while CSP was waiting for SUSPEND_CNF or RESUME_CNF
    * for a WTOInterRAT cell reselection.
   */
  boolean                  init_cell_sel_ind_during_wtointerrat;

  uint8					   agc_threshold_cnt;
  /* For WCDMA multiband support, maintain a bit mask of supported
   * frequency bands
   */
  rrc_csp_band_class_type  wcdma_band_mask;

  rrc_csp_band_class_type  bplmn_band_mask;

  rrc_csp_band_class_type  prev_wcdma_band_mask;

  /* This field is used for additional acquisitions on the last camped frequency
   * in CONNECTED mode during full frequency scan
   */
  rrc_freq_type            last_camped_freq_in_connected_mode;

  /* This field is used for additional acquisitions on the last camped frequency
   * in IDLE mode during full frequency scan
   */
  rrc_freq_type            last_camped_freq_in_idle_mode;

   /*-----------------------------*/
  /* BPLMN data 2G->3G searches  */
  /*-----------------------------*/
  
  /* determines if BPLMN search request is new or continued when bplmn search request
   * is received from RR. Default value TRUE 
   */
  boolean bplmn_new_srch;

  /* determines if BPLMN timer has expired or not. Default value TRUE */
  boolean bplmn_srch_tmr_expired;


  /* PLMN Ids to be searched on 3G with 0th element being
   * the highest priority PLMN.
   * Points to the PLMN that is currently being read from the BPLMN search
   * request list.
   */
   
  uint32    bplmn_req_list_rd_index;

  /* Used to determine if current_band has been initialized or not during BPLMN
   * search. If the current_band has been initialized then it will not be
   * set again when ACQ DB is scanned from the beginning multiple times bcoz
   * of multiple PLMN entries in the HPLMN list. This is done to ensure that
   * full freq scan is started on the most recent "band entry" in the ACQ DB.
   * This variable was added bcoz of CR 104768.
   *
   * FALSE: initial value. implies that current_band has NOT been initialized
   * TRUE:  otherwise. 
   */
  boolean   bplmn_current_band_initialized;

  /* Numbers of plmns requested by RR/NAS to do background search on 
   *   "0"  implies manual plmn search.
   * "> 0"  implies automatic plmn search.
   */
  uint32    bplmn_num_req;

  /* BPLMNs found are added to the list at the index given by the
   * this variable. The list is sent by RRC to RR.
   */
  uint32    bplmn_fnd_list_wr_index;

  /* Determines if acquisition is part of SIB procedure, i.e., intra-freq 
   * cell acquistion. If part of SIB proc then acq db read index is not
   * modified. Default value FALSE;
   */
  boolean bplmn_sib_try_next_best_cell;

  /* PLMNs requested by RR for background search */
  rrc_requested_plmn_type   bplmn_req_list[SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];

  /* RATs associated with the PLMNs in the higher priority PLMN list received from NAS
   * are stored in this array
   */ 
  sys_radio_access_tech_e_type bplmn_rat_req_list[SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];

  /*Upper layers use scan_scope to indicate ACQ_DB only search*/
  sys_scan_scope_e_type srvc_req_scan_scope;

  sys_scan_scope_e_type bplmn_scan_scope;

  /* Confirmation sent by RRC to RR in response to PLMN Search */
  rrc_interrat_plmn_srch_cnf_type rrc_rr_plmn_srch_cnf;

  /* Determines if rrccsp_send_bplmn_search_cnf_to_rr function should
   * be called or rrccsp_send_bplmn_abort_cnf_to_rr should be called.
   * Initial value = TRUE implies function rrccsp_send_bplmn_search_cnf_to_rr
   * should be called.
   */
  boolean bplmn_srch_cnf;

  /* Determines if acquisition on the freq. succeeded or not when waiting
   * for acquisition confirmation and the BPLMN timer expires.
   */
  boolean bplmn_acq_status;

  /* Determines if full freq scan is needed for PLMN srch. Initial value
   * is TRUE.
   */
  boolean plmn_list_freq_scan_needed;

  /* Determines if full freq scan is needed for Manual PLMN srch. Initial value
   * is TRUE.
   */
  boolean manual_plmn_list_freq_scan_needed;


  /* Determines when MIB should be read in foreground. If the value 
   * of the variable reaches (or exceeds) MAX_BPLMN_FREQ_SEL_ATTEMPTS then MIB
   * should be read in foreground else in background.
   */
  uint8   bplmn_bgnd_mib_rd_cnt;

  /* The variable above is incremented over one frequency which is
   * stored in this variable. I.e., if consecutive attempts to read
   * MIB on the frequency stored in this variable fail 
   * MAX_BPLMN_FREQ_SEL_ATTEMPTS number of times then MIB should be 
   * read in foreground.
   */
  rrc_freq_type bplmn_bgnd_mib_rd_freq;
  
  /* Scrambling Code of the Cell */
  /* where last ACQ was successful before returning PLMN list search confirm 
   * to he requesting RAT.
   */
  rrc_scr_code_type bplmn_acq_succ_scr_code;
  rrc_freq_type     bplmn_acq_succ_freq;
  uint32            bplmn_acq_succ_pn_pos;
  
  uint8             bplmn_acq_succ_scr_code_det;
  uint8             bplmn_acq_succ_curr_acq_count;

  uint64 remaining_time_during_bplmn;
  /* ---------------------------------------------------------- */ 
  /* Variables declared for Inter-Frequency Redirection Feature */
  /* ---------------------------------------------------------- */

  /* Frequency camped on when redirect request was received by CSP from RCE */
  rrc_freq_type  last_camped_freq;

  /* Designated frequency in rrc inter-frequency redirect request */
  rrc_freq_type  redirected_freq;

  /* Designated psc in rrc inter-frequency redirect request */
  rrc_scr_code_type redirected_psc;

  /* Wait time in rrc redirect request */
  uint32  freq_redirect_wait_time;

  /* Boolean for wait timer expiry. 
   * TRUE (wait timer expired) & FALSE (otherwise)
   */
  boolean start_acq_on_last_camped_freq;

  /* Determines if acquisition attempt on redirected frequency is the first one.
   * TRUE (first acquisition on redirected frequency) & FALSE (otherwise)
   */
  boolean first_acq_on_redirected_freq;

  /* This is used to inform L1 if the GSM cell is barred, or WCDMA 
   * cell is barred, or WCDMA frequency is barred, or the active
   * set is barred
   */
  rrc_csp_cell_bar_info_e_type cell_bar_info;

  /* Used by CSP to determine if interrat_reselection_rej or
   * interrat_abort_cnf should be sent to GSM-RR on receiving 
   * stop confirmation from MCM 
   */
  rrc_csp_rsp_to_rr_e_type rsp_to_rr;

  /* These two variables contain the scrambling code and pn position of the best cell in the 
   * active set if Connection Release Complete was sent to the network. Otherwise, this variable
   * always contains an invalid value of 8192.
   */
  uint16 aset_scr_code;
  uint32 aset_pn_pos;

  /* OOS Handling in Connected Mode */

  /* Used for 30s specific PLMN search OOS timer. TRUE value implies that 
   * the 30s timer has expired. FALSE value implies that the 30s timer has
   * not expired. Initial value is TRUE.
   */
  boolean oos_splmn_srch_tmr_expd;

  /* Determines if GSM RAT has been searched or not. TRUE value implies that
   * at least 1 search has been completed on GSM. FALSE value implies otherwise.
   * Initial value is FALSE.
   */
  boolean oos_gsm_srch_done;

  /* Contains the number of PLMNs found on GSM RAT when specific PLMN
   * search was requested on GSM. If the variable has a value > 0 and
   * 30s timer has expired, then search is continued on GSM for ANY PLMN
   */
  byte  oos_gsm_num_avail_plmns;

  /* stores current cell access status */
  mm_as_cell_access_status_T curr_cell_access_status;

  /* boolean that says current DSAC info is valid(or) not */
  boolean is_curr_dsac_info_valid;

  /* boolean that says current PPAC info is valid(or) not */
  boolean is_curr_ppac_info_valid;

  /* boolean that says DSAC info is valid(or) not */
  boolean  is_dsac_valid;
  
  /* stores cs cell access based on DSAC info */
  mm_as_cell_access_status_T     curr_cs_cell_access;

  /* stores ps cell access based on DSAC info */
  mm_as_cell_access_status_T     curr_ps_cell_access;
  
  boolean  is_ppac_valid;

/* Stores the curr cell CS reg status */
  boolean                        curr_cell_cs_reg_allowed;

/* Storest the curr cell PS reg status */
  boolean                        curr_cell_ps_reg_allowed;

/* Stores the curr cell paging response status */
  mm_as_page_rsp_e_type          curr_cell_page_rsp_allowed;

  /* ------------------------------------------ */
  /* Optimizing service search on PCS/850 bands */
  /* ------------------------------------------ */
  boolean gsm_svc_srch_done;

  /* ------------------------------------------ */
  /* NV Frequency Scan                          */
  /* ------------------------------------------ */
  /* Read from NV. TRUE implies that frequency in NV
   * should be read */
  boolean       wcdma_dl_freq_enabled;

  /* Read from NV. Frequency on which acquisition should
   * be attempted before doing ACQ-DB/Full-Scan */
  rrc_freq_type wcdma_dl_freq;
  
  /*--------------------------------------*/
  /* BPLMN variables for 3G->3G searches  */
  /*--------------------------------------*/

  /* Determines whether PLMN search should also be done on GSM
   * RAT or not.
   * TRUE: Do PLMN search on GSM RAT also
   * FALSE (default): otherwise.
   */
  boolean  bplmn_dual_mode_enabled;

  /* This flag is set to TRUE by default. It indicates whether
   * search has to be done on WCDMA RAT or not
   */
  boolean  bplmn_wcdma_mode_enabled;

  /* Stores the nw select mode to determine if Automatic PLMN list
   * was sent by NAS or Manual PLMN list request 
   */
  sys_network_selection_mode_e_type bplmn_nw_sel_mode;

  /* stores the status of the BPLMN search variables */
  wtow_bplmn_srch_status_type bplmn_srch_vars_status;

  /* Determines if GUARD timer for 3G->3G BPLMN search has expired (TRUE) or not (FALSE).
   * Initial value is TRUE.
   */
  boolean             bplmn_guard_srch_tmr_expired;

  sys_band_mask_type    bplmn_band_pref;

  /* Used to update GSM with original RAT priority information */
  sys_band_mask_type    rat_pri_band_pref;

  /* Stores the original CSP scan type before changing it to BPLMN search scan type */
  rrc_csp_scan_e_type prev_curr_scan;

  /* Previous PLMN requested by MM in Service Req. message */
  rrc_requested_plmn_type   prev_curr_req_plmn;

  /* Stores the original CSP acq entry type before changing it to BPLMN search acq entry type */
  rrc_csp_acq_entry_type    prev_curr_acq_entry;

  /* Stores the original type of acq before starting the BPLMN search */
  rrc_csp_acq_e_type        prev_curr_type_acq;

  /* Indicates how a PLMN is to be selected - Manual or Automatic. */
  sys_network_selection_mode_e_type prev_network_select_mode;

  /* This variable is set to MM_AS_NO_SERVICE when BPLMN search is 
   * started. Hence the need to save the original value of this var
   */
  mm_as_service_state_T       prev_curr_service_status;

  /* This variable is set to RRC_CSP_NO_EVENT_RCVD when BPLMN search is 
   * started. Hence the need to save the original value of this var
   */
  rrc_csp_mm_primitive_e_type    prev_last_mm_primitive;

  /* Stores the identity of the procedure that sent the suspend request */
  rrc_proc_e_type            bplmn_suspend_proc;

  /* Stores the suspend cause in suspend request */
  rrc_bplmn_suspend_cause_e_type bplmn_suspend_cause;

  /* This variable is used by CSP to determine the exact BPLMN state - active,
   * passive, inactive, even when L1 is a higher priority task and can 
   * pre-empt RRC at any time. This variable allows CSP to determine if CPHY_GO_TO_SLEEP_IND 
   * should be sent to L1 or not; and also allows CSP to inform L1 if it should go to
   * sleep or not when WTOG BPLMN search becomes active. 
   * To achieve the above, this variable is set to TRUE before accessing the critical
   * section; and the value of this variable is checked by L1 before accessing the 
   * critical section. This variable is set to FALSE on exiting the critical section.
   * (It is important to reset the value of the variable on exiting the critical section
   *  as otherwise no BPLMN search will ever be performed).
   */
  boolean bplmn_crit_sec_flag;

#ifdef FEATURE_BPLMN_SEARCH_320MS
  uint16  bplmn_start_sfn;
#endif

  /* This variable stores the # of W PLMNs present in the HPLMN list from NAS
   */
  uint32    wtow_bplmn_num_req;

  /* This variable stores the PLMN ids of the W PLMNs present in the HPLMN list from NAS
   */
  rrc_requested_plmn_type   wtow_bplmn_req_list[SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];

  uint8      bplmn_trans_id;

  /* Stores the UMTS channel numbers which are mapped from GSM ARFCNs */
#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  rr_rrc_gsm_cell_list_type umts_chls_from_gsm;
#endif

#ifdef FEATURE_WCDMA_TO_LTE
  boolean bplmn_lte_mode_enabled;
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type    bplmn_lte_band_pref;
#else
  sys_band_mask_type    bplmn_lte_band_pref;
#endif
#endif
  /* Search timers for background MPLMN Search indicated by NAS*/
  sys_manual_search_timer_value_type    bplmn_search_timer_value;

  boolean  send_partial_results;

#ifdef FEATURE_EOOS
  #error code not present
#endif

  /* For UOOS, this var has been added to inform NAS 
   * (using var ue_oos in RRC_SERVICE_CNF) that service is available. 
   * NAS looks at this var, ue_oos, to determine if limited service request
   * should be sent to RRC or not.
   * For non-UOOS, this var is used to NOT start the 10s No Service
   * timer which (without this fix) gets started when UE finds a PLMN
   * that belongs to forbidden LAI which is NOT correct because UE can
   * get Limited Service on this PLMN.
   */
  boolean    plmn_fnd_during_fgnd_svc_srch;

#ifdef FEATURE_EOOS
  #error code not present
#endif
  
   /* A new variable is added that stores lai and PLMN information
     together */
   inter_task_lai_T curr_lai; 

  /* Used to determine if the initial T mins timer during which UE does 
   * not go to sleep has expired or not. Default value is TRUE.
   */
  boolean oos_initial_no_deep_sleep_tmr_expd;

  /* Used to determine if the 30s duration for which UE should remain in
   * deep sleep has expd or not. Default value is TRUE.
   */
  boolean oos_conn_mode_ds_tmr_expd;


  /* Used to determine if the DCH->FACH/PCH reconfiguration timer has expired
   * or not. Initial value is FALSE.
   */
  boolean            dch_to_fach_pch_tmr_expd;

  /* Stores the destination state as it will be used to configure appropriate
   * channels when UE camps on a suitable cell. The CSP scan type may no longer
   * suffice as it is changed to OOS_Scan from DCH_to_FACH scan for RL_failure
   */
  rrc_state_e_type   dch_tx_dest_state;


  /* Used to determine if the FACH->FACH/PCH reconfiguration timer has expired
   * or not. Initial value is FALSE.
   */
  boolean            fach_to_fach_pch_tmr_expd;

  /* Stores the destination state as it will be used to configure appropriate
   * channels when UE camps on a suitable cell. The CSP scan type may no longer
   * suffice as it is changed to OOS_Scan from FACH_to_FACH/FACH_to_PCH scan
   * after the redirection timer expiry
   */
  rrc_state_e_type   fach_tx_dest_state;



#ifdef FEATURE_INTER_FREQ_EM_CALL_REDIRECTION
  /* TRUE implies that the "type-of-PLMN-selection" variables were updated
   * and need to be reverted back to their original values. The initial
   * value is FALSE
   */
  boolean em_call_acq_type_updated;

  /* Stores the original value of network_select_mode when the above variable
   * em_call_acq_type_updated is set to TRUE
   */
  sys_network_selection_mode_e_type ifreq_orig_nw_select_mode;

  /* Stores the id of the PLMN UE is camped on. This plmn id is used to
   * compare with the PLMN UE eventually camps on to determine if the
   * n/w sel mode needs to be set to Automatic or Limited Routine. If
   * UE camps on the same/equivalent plmn then the n/w sel mode is 
   * set to Automatic, otherwise it is set to Limited Routine.
   */
  rrc_plmn_identity_type ifreq_orig_plmn_id;
#endif

  /* This enum is used by RRC to determine if background search is in progress
   * for "full service search" or "plmn list search". This value is set based
   * on the value received from NAS in PLMN_LIST_REQ.
   * The initial value of this enum is SYS_SERVICE_SEARCH_NONE.
   */
  sys_service_search_e_type service_search_bgnd;

  /* Setting this variable to TRUE means that on a limited service request L1 has to lift freq
    * and PSC barring both which overrides t-bar feature characteristic that  says unbar
    * only the frequency in this case
   */
  boolean bar_for_resel_only;

#ifdef FEATURE_FEMTO_CSG
  /* Set this flag to true while barring/unbarring non-white CSG cells*/
  boolean bar_non_white_csg;
#endif

  boolean non_eq_plmn_barred;

  /* Scratch pad for storing the current (may or may not be camped) cell network 
   * sharing details. Note that this will not give the network sharing details of the 
   * current camped cell. */
  rrc_csp_nw_sharing_entry_type curr_nw_db_entry;

  /* scrambling code */
  uint16 prev_scr_code;

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
  boolean oosc_plmn_found;
  uint8   oosc_l1_rssi;

  /* This variable is used to identify the first cycle of every GSM search in
   * RRC connected mode OOS. UI is updated with service status after every 
   * first cycle of GSM search.
  */
  boolean is_first_gsm_search;
#endif

#ifdef FEATURE_GAN
  #error code not present
#endif

  /* Contains target FDD cell list given by network via GSM-RR */
  rr_rrc_fdd_target_info_type    target_fdd_cell_list;

  /* Used to determine if the GtoW redirection timer has expired
   * or not. Initial value is TRUE.
   */
  boolean    gtow_redirection_timer_expired;

  boolean    gtow_redir_retry_lfs_scan;

  boolean    skip_repeat_scan_for_redir;

  uint8 num_freq_in_list_db;
  uint8 curr_rd_idx;


#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif

  /*
  Stop WCDMA cause indicated by RR in RRC_RR_STOP_WCDMA_REQ
  */
  rrc_rr_stop_wcdma_cause_e_type rrc_rr_stop_wcdma_cause;

  /* To maintain the flexible deep sleep behavior right from the power up and OOS situations  */
  boolean do_lfs_scan;

  /*Boolean to indicate that RRC has received CEL_RESEL_IND that is to be processed when waiting for TRANS CNF*/
  /*Default Value is FALSE*/
  boolean cel_resel_ind_rcvd_waiting_trans_cnf;

  /*Boolean to indicate that LLC received Cel Transition CNF from L1
     Default value is TRUE*/
  boolean cel_trans_cnf_recvd_at_llc;

  /*
    Skip GSM search sent in Service request
    when this is set, do not search G as part of 900/850/PCS Band optimizations
  */
  boolean skip_gsm_search;

  sys_stop_mode_reason_e_type mode_change_reason;

  /* This variable is set when stop WCDMA req at MCM is pending
   * to be handled after W2G procedure abort cnf
   */
  boolean stop_pending_for_wtog_abort;

#ifdef FEATURE_DUAL_SIM
  /*WCDMA is preempted, after IDLE CNF from L1 call the Re ACQ procedure*/
  boolean preemption_handling_in_progress;

  /*If this flag is set to TRUE, RRC is waiting for L1 to release lock.*/
  boolean csp_waiting_for_wl1_to_release_lock;

  /* If this flag is set to true, NAS is to be updated with NO SERVICE with cause
     NO RESOURCE */
  boolean oos_due_to_no_resource;

  /* If this flag is set to TRUE, WL1 should be indicated to perform ACQ splitting */
  boolean shld_acq_be_spltd;

  /*Set to TRUE if RRC receives DL weak indication with no resource*/
  boolean rrc_dl_wk_no_res;
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST
  sys_rat_pri_list_info_s_type rat_pri_list_info;
#endif

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)

  rrc_csp_interrat_rat_e_type interrat_type;

  rrc_csp_interrat_rat_e_type bplmn_interrat_type;

  detected_frequency_list_type detected_frequency_list;

  /* Flag which indicates pre-redirection info has to be included or not in connection request msg. */
  boolean exclude_preredir_info_in_conn_req_msg;
  boolean  cgi_info_included;
#endif

  rrc_csp_plmn_srch_rat_order_type plmn_srch_order;

  rrc_mm_service_req_cause_e_type service_req_cause;

  /* For GTOW PLMN list search, this flag indicates if UE is camped suitably on G */
  boolean gsm_camped_suitably;

  /*Boolean to track if the cell is barred for operator use*/
  boolean cell_reserved_for_operator_use;

#ifdef FEATURE_WRLF_SYSTEM_SEL
  /* Flag used to determine if connected mode OOS search is in progress*/
  boolean ue_in_conn_oos;
  /* PLMN on which NAS went OOS in connected mode.*/
  rrc_plmn_identity_type  rlf_plmn_id;
#ifdef FEATURE_FEMTO_CSG
  /* CSG ID on which NAS went OOS in connected mode */
  uint32  rlf_csg_id;
#endif
#endif
#ifdef FEATURE_3GPP_CSFB
  /*Indicated that SIB cmd is posted to SIB when CSFB is active and SIB has skipped read SIB11/12/11bis/19*/
  boolean sib_read_csfb_active; 
#endif

#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
  boolean waiting_for_activation_rsp;
#endif

  boolean est_req_for_emerg_call;

  boolean allow_lte_resel;
  /* Indicates the presence of primary PLMN ID in RRC dedicated OTA */
  boolean primary_plmn_Id_present;

#ifdef FEATURE_FEMTO_CSG
  boolean csg_asf_srch_in_progress;
  rrc_csp_temp_csg_cell_list  temp_csg_list;
  rrc_csp_fp_macro_info_type fp_macro;
  rrc_csp_fp_macro_info_type lte_macro_asf;
  rrc_csp_csg_rat last_csg_rat;
  /*Indicates whether resel is result of ASF*/
  boolean csg_resel;
#endif
  /*RSCP of the cell. Used in CGI response to LTE*/
  int16 cgi_rscp;

  /*ECIO of the cell. Used in CGI response to LTE*/
  int16 cgi_ecio;
  boolean barred_due_to_band_mismatch;

  boolean continue_cell_selection;

  boolean power_up_service_req;

  /*should the last camped freq in idle mode be skipped now*/
  boolean skip_last_camped_freq_in_idle_mode;

  /* Indicates whether partial results have to be sent due to guard timer expiry or not*/
  boolean send_prtl_results_from_lte_due_to_guard_timer_expiry;

  boolean ignore_csp_change_mode_ind;

  boolean can_ehplmn_be_camped;

}rrc_csp_int_data_type;


/*--------------------------------------*/
/* BPLMN variables for 3G->2G searches  */
/*--------------------------------------*/
/* This data structure stores all global data used internally by Cell
 * Selection procedure related to WTOG BPLMN search. 
 */
typedef struct
{

  /* This variable stores the value returned from GSM-RR. If the value
   * is set to TRUE then it implies that the GSM search is done.
   */
  boolean wtog_srch_done;

  /* This variable stores the value returned from GSM-RR. If the value
   * is set to TRUE then it implies that the HPLMN was found on GSM.
   */
  boolean wtog_hplmn_found;

  /* This variable is used by CSP to determine if WTOG search is currently
   * active or not. The initial value of this variable is set to FALSE
   */
  boolean wtog_bplmn_srch_active;

  /* This variable is used by CSP to determine if WTOG search has already
   * been aborted or not. The initial value of this variable is set to FALSE
   */
  boolean wtog_bplmn_srch_aborted;

/* This variable is used by CSP to determine if WTOG search has already
 * been suspended or not. The initial value of this variable is set to FALSE
 */
  boolean wtog_bplmn_srch_suspended;

  /* This variable stores the timer value that is sent from RRC to RR so that
   * GSM can perform BPLMN search during this time and return control to RRC
   * after the timer expiry. Notice that for WTOG BPLMN search, the DRX timer
   * is started by GSM
   */
  uint32  wtog_rem_drx_tmr_value;

  /* This variable stores the value returned from GSM-RR related to the
   * # of PLMNs found on GSM
   */
  uint32  wtog_gsm_plmns_found;

  /* This variable stores the PLMNs found by GSM during the BPLMN search
   */
  rrc_avail_plmn_list_type  plmn_list;

#ifdef FEATURE_WCDMA_TO_LTE
  /* This variable stores the value returned from LTE-RRC related to the
   * # of PLMNs found on GSM
   */
  uint32  wtol_plmns_found;

  /* This variable stores the PLMNs found by LTE during the BPLMN search
   */
  rrc_avail_plmn_list_type  lte_plmn_list;
#endif
  /* This variable is used to determine if Manual BPLMN search as a whole should 
   * be aborted or restarted after aborting WtoG BPLMN search. Initial
   * value is set to FALSE which implies that Manual BPLMN search should be aborted
   */
  boolean restart_bplmn_search;  

}rrc_csp_wtog_bplmn_int_data_type;


/* This data structure stores the current data associated with
 * Initial and Stored Selection Controller.
 */
typedef struct
{
  /* This is the current PLMN being searched for */
  rrc_requested_plmn_type req_plmn;

  /* This is the current Acquisition type */
  rrc_csp_acq_e_type      acq_type;

  /* This is the original Acquisition type. It is saved before
   * starting the BPLMN search in background
   */
  rrc_csp_acq_e_type      prev_acq_type;

  /* This flag indicates that the Acq. DB is being searched at present */
  boolean                 acq_db_in_use;

  /* This flag indicates that the Destination Freq is being searched at present */
  boolean                 dest_freq_in_use;
  
  /* This count indicates how many times acquisition is to be tried
   * on the destination frequency.
   */
  uint16                  num_tries_on_dest_freq;

  /* Overall count of number of cycles done on destination frequency and 
   * Acquisition database. Note that:
   * 1 count of num_acq_db_and_dest_freq_cycles = 
   *        RRC_CSP_MAX_ACQ_TRIES_ON_DIRECTED_FREQ + 1;
   */    
  uint16                  num_acq_db_and_dest_freq_cycles;
  /* The following data is to be used for acquisition database based scan.
   * This is valid only when acq_db_in_use is TRUE and acq_type is valid.
   */
  struct
  {
    /* This flag indicates that the next search is to begin with the first entry
     * in Acq. DB.
     */
    boolean                 start_with_first;
  }acq_db_scan_info;

  /* The following data is to be used for frequency scan only. This is valid
   * only when acq_db_in_use is FALSE and acq_type is valid.
   */
  struct
  {
    rrc_csp_freq_scan_e_type  type_freq_scan;
    boolean   raw_scan_completed;
    boolean   fine_scan_completed;

#ifdef FEATURE_EOOS
    #error code not present
#else 
    /* List frequency scan data */
    struct
    {
      uint16   num_freq;
      int16   curr_freq_index;
      l1_freq_scan_result_struct  freq_result[L1_MAX_SCAN_FREQ];
      rrc_plmn_identity_type plmn_id[L1_MAX_SCAN_FREQ];
    }list_scan;
#endif 
    /* Raw frequency scan data */
    struct
    {
      uint16  num_freq;
      int16   curr_freq_index;
      l1_freq_scan_result_struct  freq_result[L1_MAX_SCAN_FREQ];
    }raw_scan;

    /* Fine frequency scan data */
    struct
    {
      uint16  num_freq;
      int16   curr_freq_index;
      l1_freq_scan_result_struct  freq_result[L1_MAX_SCAN_FREQ];
    }fine_scan;

  }scan_info;

  /* List of frequencies where service was available */
  rrc_freq_type                   available_freq_list[L1_MAX_SCAN_FREQ];

#ifdef FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
  /* List of bands associated with the frequencies. This band list
   * is then sent to GSM-RR so that it can determine corresponding
   * GSM channels that should be eliminated
   */
  rrc_csp_band_class_type         available_freq_list_band[RRC_MAX_NUM_FREQ];
#endif                                                                       
  
  uint32                          num_available_freq;

  /* Band that was most recently scanned */
  rrc_csp_band_class_type         current_band;

  /* Band that was scanned first as part of the full frequency scan */
  rrc_csp_band_class_type         first_band_to_be_scanned;

  /* Mask to help in determining the next band to be scanned */
  rrc_csp_band_class_type         next_band_mask;

  /* Mask to help in determining the bands which are already scanned */
  rrc_csp_band_class_type         scanned_band_mask;
  rrc_csp_band_class_type         dont_skip_band_mask;
  boolean                         add_barr_freq_to_fine_res;
  uint16                          num_of_barred_freq;
  rrc_freq_type                   barred_freq[RRC_MAX_OVERLAP_FREQ];
  rrc_csp_band_class_type         band_corr_barred_freq[RRC_MAX_OVERLAP_FREQ];

  /* Boolean that indicates if an additional channel scan is
   * required or not for a particular band
   */
  boolean                         addl_chls_scan_needed;

#ifdef FEATURE_EOOS
  #error code not present
#endif

  /* Maintains the list of bands whose additional channels have been
   * scanned during full frequency scan
   */
  rrc_csp_band_class_type         addl_chnl_done_mask;

  rrc_csp_band_class_type         prioritized_bands[MAX_NUM_OF_WCDMA_BANDS];
  uint8         prioritized_bands_read_idx;
  uint8         num_of_prioritized_bands;

}rrc_csp_select_ctrl_data_type;


#ifdef FEATURE_EOOS
#error code not present
#endif



typedef struct 
{
  /* is dsac valid (or) not */
  boolean      valid;
  /* stores cs access control */ 
  rrc_csp_access_control_e_type cs_access_control;
  /* stores ps access control */ 
  rrc_csp_access_control_e_type ps_access_control;
} rrc_dsac_info_type;

typedef struct
{
  boolean                        valid;
  boolean                        cs_reg_allowed;
  boolean                        ps_reg_allowed;
  mm_as_page_rsp_e_type          page_rsp_allowed;
} rrc_ppac_info_type;

/* Stores the DSAC restrictions of each of the shared PLMNs of the current cell */
typedef struct
{
  rrc_plmn_identity_type plmn_id;
  /* is dsac valid (or) not */
  boolean      valid;
  rrc_DomainSpecificAccessRestrictionParam_v670ext dsac_info;
} rrc_dsac_info_for_shared_plmns_type;

/* Stores the PPAC restrictions of each of the shared PLMNs of the current cell */
typedef struct
{
  rrc_plmn_identity_type plmn_id;
  /* is ppac valid (or) not */
  boolean      valid;
  rrc_PagingPermissionWithAccessControlParameters ppac_info;
} rrc_ppac_info_for_shared_plmns_type;

#ifdef FEATURE_3GPP_CSFB
typedef struct
{
  uint8 num_cont;
  uint16 psc[WCDMA_RRC_MAX_UTRA_CONT];
  rrc_System_Information_Container_type cont[WCDMA_RRC_MAX_UTRA_CONT];
}rrc_System_Information_Container_list_type;
#endif
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* RRC CSP Internal data, visible within Cell Selection Procedure */
extern  rrc_csp_int_data_type  rrc_csp_int_data;

#ifdef FEATURE_EOOS
#error code not present
#endif

/* This structure is used by CSP only for maintaining state related to 
 * WTOG BPLMN search
 */ 
extern  rrc_csp_wtog_bplmn_int_data_type  rrc_csp_wtog_bplmn_int_data;

/* This data structure stores the Selection data for Initial and Stored
 * Selection Controller.
 */
extern  rrc_csp_select_ctrl_data_type  rrc_csp_curr_select_data;

extern  rrc_csp_select_ctrl_data_type  prev_rrc_csp_curr_select_data;
extern  boolean bplmn_new_srch;

/* Used for initializing the variables associated with 3G->3G background
 * PLMN Search during the search.
 */
extern  wtow_bplmn_srch_status_type wtow_bplmn_srch_status;

#ifdef FEATURE_3GPP_CSFB
extern rrc_System_Information_Container_list_type rrc_SIB_container;
#endif

typedef enum
{
  CAUSE_NONE,
  CAUSE_QTA,
  CAUSE_TA,
  CAUSE_IMMEDIATE
}rrccsp_preemption_cause_e_type;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */



/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION          RRC_CSP_FIND_CELL_BAR_STATUS

DESCRIPTION       This function is responsible for finding the status of
                  a cell in terms of Cell Barred Status. The
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
                              ,rrc_plmn_identity_type sib_idx_plmn_id
                              );


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
rrc_csp_find_access_control(rrc_csp_access_control_e_type  *access_control,
                            rrc_dsac_info_type *dsac_info,
                            rrc_plmn_identity_type  plmn_id,
                            rrc_int_cell_id_type    cell_id
                            ,rrc_plmn_identity_type sib_idx_plmn_id
                            ,rrc_ppac_info_type *ppac_info
                            );

/*===========================================================================

FUNCTION          RRC_FILL_ACCESS_CLASS_INFO

DESCRIPTION       This function will fill the access class info based on the 
                  access control of the cell
DEPENDENCIES
                  - SIB procedure should have SIB 3 available for this cell.
                  - The UE Access Class should be known.

RETURN VALUE
                  NONE
SIDE EFFECTS
                  None.
===========================================================================*/
void rrc_fill_access_class_info(rrc_int_access_class_barred_list_type rrc_int_access_class_barred_list, 
	                                                   rrc_csp_access_control_e_type *access_control,
	                                                   rrc_plmn_identity_type  plmn_id,
	                                                   rrc_int_cell_id_type    cell_id);

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
void  rrc_csp_init_int_data( void );

/*===========================================================================

FUNCTION          RRC_CSP_CHECK_FOR_LAI_FORBIDDEN

DESCRIPTION       This function is responsible for initializing the internal
                  data structure to default values.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/


boolean  rrccsp_check_for_lai_forbidden(rrc_SysInfoType1 *sib1_ptr,
                                        rrc_MasterInformationBlock *mib_ptr
                                        );

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
  rrc_csp_acq_e_type  *ret_type_acq_ptr,
  rrc_csp_acq_entry_type  *ret_acq_entry_ptr,
  rrc_csp_freq_scan_e_type *ret_type_freq_scan
);

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
  rrc_csp_plmn_status_e_type cell_plmn_status, 
  rrc_csp_plmn_status_e_type cell_plmn_status_wrt_rlf_plmn, 
  rrc_csp_acq_entry_type* acq_entry_ptr,
  rrc_csp_acq_e_type type_acq_expect
);
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
rrc_csp_compare_expected_cell(rrc_csp_plmn_status_e_type cell_plmn_status,
                              rrc_csp_acq_entry_type* acq_entry_ptr,
                              rrc_csp_acq_e_type type_acq_expect);

/*===========================================================================

FUNCTION          RRC_CSP_SEND_L1_CMD

DESCRIPTION       This function is responsible for filling up data structures
                  for all L1 commands sent by CSP. This also allocates cmd
                  buffer and sends the command.
                  Since all L1 cmds go via LLC, the LLC command RRC_LLC_CMD_REQ
                  actaully encapsules the real L1 command. This is the LLC
                  command funneling mechanism.

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
                    rrc_csp_acq_entry_type *acq_entry_ptr);

/*===========================================================================

FUNCTION          RRC_CSP_SEND_L1_DRX_REQ

DESCRIPTION       This function is responsible for sending CPHY_DRX_REQ
                  to L1.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS      None.
===========================================================================*/
void rrc_csp_send_l1_drx_req(void);
/*===========================================================================

FUNCTION          RRC_CSP_SEND_L1_DRX_REQ_FOR_MULTIPLE_DRX

DESCRIPTION       This function is responsible for sending CPHY_DRX_REQ
                  to L1 for multiple DRX support feature.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS      None.
===========================================================================*/
void rrc_csp_send_l1_drx_req_for_multiple_drx(void);

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
                                 rrc_csp_cell_bar_e_type  *cell_bar_status);


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
rrc_csp_send_sib_cmd(rrc_cmd_e_type sib_cmd, uint32 sib_mask,
                     rrc_csp_acq_entry_type *acq_entry_ptr,
                     boolean                for_serving_cell);


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

DEPENDENCIES
                  The calling function MUST decide and screen non-relevant
                  enums in rrc_csp_cell_bar_e_type and
                  rrc_csp_lai_rai_status_e_type.

RETURN VALUE
                  A bit mask that represents Acq. status.

SIDE EFFECTS
                  None
===========================================================================*/
rrc_csp_acq_status_type
rrc_csp_gen_status_mask(rrc_csp_cell_bar_e_type  cell_bar_status,
                        rrc_csp_lai_status_e_type  lai_status);


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
                    rrc_csp_acq_entry_type *acq_entry_ptr);


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
void rrccsp_send_mplmn_partial_report_to_mm(void);
/*===========================================================================

FUNCTION          RRC_CSP_BPLMN_SEND_MM_PLMN_LIST_CNF_CMD

DESCRIPTION       This function is responsible for filling up data structures
                  for PLMN List Confirm command to MM sent by CSP.
                  This also allocates cmd buffer and sends the command.
                  It also frees allocated command and embedded buffers
                  when failure is detected before sending the command.

DEPENDENCIES

RETURN VALUE
                  SUCCESS: Command was successfully sent to MM.
                  FAILURE: Command could not be sent due to any one of multiple
                           reasons.

SIDE EFFECTS
                  For time being it always sends an PLMN_LIST_CNF with the
                  no PLMNs.
===========================================================================*/
void
rrc_csp_bplmn_send_mm_plmn_list_cnf_cmd(wtow_bplmn_srch_status_type status, uint8 trans_id);


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
rrc_csp_send_mm_plmn_list_cnf_cmd(boolean srch_done);

/*===========================================================================

FUNCTION          rrccsp_update_plmn_list

DESCRIPTION       This function reads the SIBs for a cell and checks if it is
                  barred or in the forbidden lai list. If not, it will try to
                  add it to the PLMN list to be returned to MM in the
                  SERVICE_CNF.

DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_update_plmn_list(rrc_plmn_identity_type plmn_id, rrc_int_cell_id_type cell_id);


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
void rrccsp_update_background_plmn_list(rrc_plmn_identity_type plmn_id, rrc_int_cell_id_type cell_id);


/*===========================================================================

FUNCTION          RRC_CSP_SEND_CCM_CMD

DESCRIPTION       This function is responsible for filling up data structures
                  for Cell Change Request sent to CCM.
                  This also allocates cmd buffer and sends the command.

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
                     rrc_new_cell_status_e_type  req_cell_status,
                     rrc_csp_acq_entry_type  *acq_entry_ptr);

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
void rrc_csp_send_l1_idle_req(void);

/*===========================================================================

FUNCTION          RRC_CSP_INIT_FREQ_SCAN_DATA

DESCRIPTION       This function initializes the frequency scan data in
                  Current select data.
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void  rrc_csp_init_freq_scan_data(void);

/*===========================================================================

FUNCTION          RRC_CSP_GET_NEXT_FREQ_SCAN_ENTRY

DESCRIPTION       This function is responsible for determining the next
                  frequency scan type and returning the appropriate
                  data.
DEPENDENCIES
                  None

RETURN VALUE
                  SUCCESS: Able to find a frequency where a Raw or Fine scan
                           or an Acquisition can be tried.
                  FAILURE: All frequencies exhausted.

SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type
rrc_csp_get_next_freq_scan_entry(rrc_freq_type *ret_freq);


/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_FREQ_SCAN_ENTRY

DESCRIPTION       This function is responsible for updating frequency scan
                  data in current selection data and list frequency scan
                  data. Note that only the raw frequency scan list, fine frequency 
                  scan list and list frequency scan list need to be updated.

DEPENDENCIES
                  The L1 command data has to be validated before calling
                  this function.

RETURN VALUE
                  SUCCESS: If received scan type matches with the expected
                           scan type.
                  FAILURE: Discrepency between received and expected scan 
                           types.
SIDE EFFECTS
                  None
===========================================================================*/
uecomdef_status_e_type
rrc_csp_update_freq_scan_entry(l1_freq_scan_cnf_type *freq_scan_cnf);


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
void  rrc_csp_handle_no_available_freq(void);


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
);

/*===========================================================================

FUNCTION          RRC_CSP_SEND_CHAN_RELEASE

DESCRIPTION       This function sends a Channel Config Request to LLC to
                  release all channels.

DEPENDENCIES
                  None
RETURN VALUE
                  None.

SIDE EFFECTS
                  None

===========================================================================*/
void  rrc_csp_send_chan_release(void);

/*===========================================================================

FUNCTION          rrc_csp_send_chan_reconfig

DESCRIPTION       This function sends a Channel Config Request to LLC to
                  release all channels.

DEPENDENCIES
                  None
RETURN VALUE
                  SUCCESS/FAILURE.

SIDE EFFECTS
                  None

===========================================================================*/
uecomdef_status_e_type  rrc_csp_send_chan_reconfig(void);

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
void rrc_csp_send_tr_phy_chl_release(void);

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
);

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
);


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
void  rrc_csp_init_data_from_service_req(rrc_service_req_type  *rrc_service_req);

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
rrc_csp_start_sel_on_mm_req(void);

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
rrc_csp_release_chls_and_start_sel_on_mm_req(void);

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
rrc_csp_continue_cell_selection(void);

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
rrc_csp_release_chls_and_continue_cell_selection(void);

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
rrc_csp_start_sel_out_of_service_area(void);

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
rrc_csp_start_sel_leaving_conn_mode(void);

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
rrc_csp_start_sel_out_of_service_area_in_idle_state(void);

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
rrc_csp_start_sel_out_of_service_area_in_connecting_state(void);


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
rrc_csp_start_sel_from_dch_to_fach(rrc_cmd_type  *rrc_cmd_ptr);

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
rrc_csp_start_sel_from_fach_to_fach(rrc_cmd_type  *rrc_cmd_ptr);

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
rrc_csp_start_sel_from_fach_to_pch(rrc_cmd_type  *rrc_cmd_ptr);

/*===========================================================================

FUNCTION   RRC_CSP_RESTORE_SAVED_BPLMN_DATA

DESCRIPTION
  This function is used to restore the saved 
  data for BPLMN operation

DEPENDENCIES

  None.
 
RETURN VALUE

  None.
 
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_csp_restore_saved_bplmn_data(void);

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
rrccsp_continue_bplmn_in_frgrnd(void);

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
rrc_csp_start_sel_for_plmn_list(void);


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
void rrc_csp_send_mm_act_ind( void );

#endif

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
rrc_csp_start_sel_from_dch_to_pch(rrc_cmd_type  *rrc_cmd_ptr);

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
rrc_csp_substate_e_type rrc_csp_release_bch_and_continue_cell_selection(void);

/*===========================================================================

FUNCTION          rrc_csp_init_bplmn_data

DESCRIPTION       This function initializes RRC data and CSP local data.

DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void  rrc_csp_init_bplmn_data(void);

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
rrc_csp_start_bplmn_srch(void);


/*===========================================================================

FUNCTION          RRCCSP_BPLMN_UPDATE_STORED_ACQ_DB_RD_INDEX

DESCRIPTION       This function is used by CSP to updated 
                  stored_acq_db.curr_rd_index if BPLMN srch timer expires
                  before SIBs could be read.
                  
DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void
rrccsp_bplmn_update_stored_acq_db_rd_index ( void );

/*===========================================================================

FUNCTION          RRCCSP_BPLMN_SET_STORED_ACQ_DB_RD_INDEX

DESCRIPTION       This function is used by CSP to set 
                  stored_acq_db.curr_rd_index if BPLMN srch timer expires
                  before SIBs could be read.

DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void rrccsp_bplmn_set_stored_acq_db_rd_index (int8 );
  
/*===========================================================================
FUNCTION        RRCCSP_SEND_MODE_CHANGE_REQ_TO_MCM

DESCRIPTION
                This function sends a RRC_MODE_CHANGE_REQ (with the desired
                mode) to MCM.

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void 
rrccsp_send_mode_change_req_to_mcm( rrc_mode_e_type rrc_mode);


/*===========================================================================

FUNCTION          RRC_CSP_INIT_SELECT_DATA

DESCRIPTION       This function is responsible for initializing the internal
                  Search data structure to default values based on the passed
                  PLMN Choice type.

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
rrc_csp_init_select_data(rrc_requested_plmn_type  *req_plmn_ptr);


/*===========================================================================

FUNCTION          RRC_CSP_START_SEL_REDIRECT_FREQ_IN_CONNECTING_STATE

DESCRIPTION       This function starts cell selection on the designated
                  frequency received by RRC in inter-frequency redirection
                  request in connecting state.
                  Only transport and physical channels are released before
                  attempting acquisition on the designated frequency.
                  The cell selection is started in the current acquisition 
                  type. The current scan is set to inter frequency 
                  redirection scan.
                  CSP substate is returned as appropriate.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  Next CSP substate.
SIDE EFFECTS
                  None

===========================================================================*/
rrc_csp_substate_e_type
rrc_csp_start_sel_redirect_freq_in_connecting_state(void);


/*===========================================================================

FUNCTION          rrc_csp_send_bch_release

DESCRIPTION       This function sends a Channel Config Request to LLC to 
                  release BCCH/BCH/S_PCCPCH pipe.
           
DEPENDENCIES
                  None.
RETURN VALUE
                  None
SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_send_bch_release(boolean); 

/*===========================================================================

FUNCTION          RRCCSP_HANDLE_FREQ_REDIRECT_WAIT_TIMER_EXPIRY

DESCRIPTION       This function handles expiry of wait timer started due to 
                  inter frequency redirection request received from RCE. 
                  Till the expiry of the wait timer, RRC does not allow
                  cell selections/reselections to the last camped frequency.
                  
                  If timer expires when redirection is in progress, this
                  function determines if UE should continue camping on the 
                  current frequency which would be the case if cell selection
                  is being attempted on the redirected frequency for the first time;
                  or UE should go to pending substate and wait for the appropriate
                  confirmation and on receiving the confirmation start acquisition
                  on the last camped frequency. 
                  
DEPENDENCIES      NONE

RETURN VALUE
                  CSP substate
                  
SIDE EFFECTS

===========================================================================*/
rrc_csp_substate_e_type
rrccsp_handle_freq_redirect_wait_timer_expiry(rrc_csp_substate_e_type curr_substate);

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
void  rrc_csp_init_inter_freq_redirection_data(void);


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
void rrc_csp_cleanup_inter_freq_redirection_state(void);


/*===========================================================================

FUNCTION          RRCCSP_SEND_RCE_REDIRECT_CNF

DESCRIPTION       This function sends REDIRECT_CNF to RCE with appropriate
                  cause for successful or unsuccessful redirection. 
                  The wait timer is stopped only in this function (if it
                  was active).
                                    
DEPENDENCIES      NONE

RETURN VALUE
                  NONE
SIDE EFFECTS

===========================================================================*/
void
rrccsp_send_rce_redirect_cnf(rrc_redirect_status_e_type redirect_status, 
                             rrc_redirect_cause_e_type  redirect_cause);


/*===========================================================================

FUNCTION          RRCCSP_VALIDATE_REDIRECTION_REQ

DESCRIPTION       This function checks whether inter frequency redirection 
                  or inter-RAT redirection request received by CSP has correct 
                  arguments and that the request has been received in correct 
                  RRC state (which is connecting).
                  
                  Additionally, this function sends REDIRECT_CNF to RCE with
                  failure if redirection is not supported or if any of the above
                  checks fail.
                  
DEPENDENCIES      NONE

RETURN VALUE
                  TRUE:  Implies start acq on redirected freq.
                  FALSE: Implies ignore redirection request (because in this 
                         case redirect confirm has already been sent inside this 
                         function).
                  
SIDE EFFECTS

===========================================================================*/
boolean
rrccsp_validate_redirection_req( rrc_cmd_type *cmd_ptr );


/*===========================================================================

FUNCTION          RRCCSP_HANDLE_FAILURE_DURING_FREQ_REDIRECTION

DESCRIPTION       This function handles failure when inter frequency redirection
                  scan is in progress. The failure could be due to acquisition
                  failure, or cell may not be suitable/acceptable, or S-criteria
                  was not met, etc. 
                  
                  Either acquisition is attempted on next frequency or
                  REDIRECT_CNF is returned to RCE with appropriate
                  cause.
                  
DEPENDENCIES      NONE

RETURN VALUE
                  CSP substate which can only be one of the following:
                  RRCCSP_CAMPED_SUIT_CELL, or RRCCSP_CAMPED_ACCEPT_CELL, or
                  RRCCSP_CELL_SEL_WT_ACQ.
                  
SIDE EFFECTS

===========================================================================*/
rrc_csp_substate_e_type 
rrccsp_handle_failure_during_freq_redirection(void);


/*===========================================================================

FUNCTION          RRCCSP_SEND_CELL_UPDATE_INITIATE_REQ

DESCRIPTION       This function sends cell update initiate request to CU procedure
                  when UE goes Out of Service Area in Connected State
                  
                  
DEPENDENCIES      NONE

RETURN VALUE
                  CSP substate
                  
SIDE EFFECTS          
                  NONE
===========================================================================*/
rrc_csp_substate_e_type
rrccsp_send_cell_update_initiate_req(rrc_cell_update_cause_e_type cause);
  

/*===========================================================================

FUNCTION RRC_GET_CSP_SUBSTATE

DESCRIPTION
  This function returns the current substate of CSP.
  
DEPENDENCIES
  None

RETURN VALUE
  Current sub-state of CSP.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrc_csp_substate_e_type  rrc_get_csp_substate( void );


/*===========================================================================

FUNCTION          RRCCSP_UPDATE_BPLMN_SRCH_VARS

DESCRIPTION       This function is used by CSP to update/re-store appropriate 
                  variables during different stages of 3G->3G Background PLMN search.
                  
DEPENDENCIES      NONE

RETURN VALUE
                  NONE
                  
SIDE EFFECTS
                  NONE

===========================================================================*/
void rrccsp_update_bplmn_srch_vars( 
  wtow_bplmn_srch_status_type       status, 
  sys_network_selection_mode_e_type nw_sel_mode);


/*===========================================================================

FUNCTION          RRCCSP_RESET_BPLMN_LIST_SEARCH_STATE

DESCRIPTION       This function sends PLMN_LIST_CNF to NAS (if need be),
                  updates the BPLMN database, and also sends sleep indication
                  to L1 (if need be).
                  
DEPENDENCIES      NONE

RETURN VALUE      NONE
                      
SIDE EFFECTS      NONE

===========================================================================*/
void rrccsp_reset_bplmn_list_search_state(
    wtow_bplmn_srch_status_type status,    
    boolean                     rce_waiting, 
    boolean                     send_sleep_ind);


/*===========================================================================

FUNCTION          RRC_CSP_BPLMN_SRCH_IN_PROGRESS

DESCRIPTION       This function is called by procedures CSP, RCE, SIB, and CU
                  to determine if BPLMN search is in progress.
                  
DEPENDENCIES      NONE

RETURN VALUE      TRUE:  BPLMN search is in progress
                  FALSE: Otherwise
                  
SIDE EFFECTS      NONE
===========================================================================*/
wtow_bplmn_srch_status_type rrc_csp_bplmn_srch_in_progress(void);


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
void rrccsp_cleanup_bplmn_state( void );


/*===========================================================================

FUNCTION          RRCCSP_CLEANUP_AND_RESTART_BPLMN

DESCRIPTION       This function is called when UE transitions from Idle ->
                  non-idle state and BPLMN n/w sel mode is MANUAL. If BPLMN search 
                  was in progress when this function is called then the BPLMN 
                  search is re-started by resetting appropriate variables. Notice
                  that the guard timer is also reset. This function was added to
                  allow UE to continue Manual search across LAU/RAU or MT calls when UE
                  returns to Idle/PCH state.                             
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_cleanup_and_restart_bplmn( void );


/*===========================================================================

FUNCTION   RRCCSP_SEND_BPLMN_SRCH_RESUME_IND

DESCRIPTION

  This function is used by CSP to send itself an internal command
  RRC_BPLMN_SEARCH_IND after L1 calls the function can_l1_go_to_sleep()
  when it determines that BPLMN search can be performed
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrccsp_send_bplmn_srch_resume_ind(void);


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
rrc_csp_substate_e_type rrc_csp_resume_wtog_bplmn_srch(void) ;


/*===========================================================================

FUNCTION        RRCCSP_PLMN_LIST_REQ_IS_VALID

DESCRIPTION
                This function determines if PLMN_LIST_REQ (Automatic and Manual)
                from MM has been received in correct RRC state or not. If  
                PLMN_LIST_REQ is NOT valid then PLMN_LIST_CNF is sent to NAS 
                within this function itself.
                
DEPENDENCIES
                None

RETURN VALUE
                TRUE:  PLMN_LIST_REQ is valid
                FALSE: Otherwise

SIDE EFFECTS
                None

===========================================================================*/
boolean rrccsp_plmn_list_req_is_valid(rrc_plmn_list_req_type  *plmn_list_req_ptr);


/*===========================================================================

FUNCTION        RRCCSP_PLMN_LIST_REQ_WHEN_RESUME_IN_PROG_IS_VALID

DESCRIPTION
                This function determines if PLMN_LIST_REQ (Automatic and Manual)
                from MM has been received in correct RRC state or not. If  
                PLMN_LIST_REQ is NOT valid then PLMN_LIST_CNF is sent to NAS 
                within this function itself.

                This function is different from rrccsp_plmn_list_req_is_valid()
                in the sense that in this function it is not checked if UE is 
                camped or not. The reason being that CCM is always updated with
                cell loss when W is suspended and hence if PLMN List Request 
                is received when W is suspended then Plmn List Request may be
                incorrectly handled if function rrccsp_plmn_list_req_is_valid()
                is called. 

                This function is called only when W is in the process of resuming.
                
DEPENDENCIES
                None

RETURN VALUE
                TRUE:  PLMN_LIST_REQ is valid
                FALSE: Otherwise

SIDE EFFECTS
                None

===========================================================================*/
boolean rrccsp_plmn_list_req_when_resume_in_prog_is_valid(rrc_plmn_list_req_type  *plmn_list_req_ptr);


/*===========================================================================

FUNCTION   RRCCSP_GET_CURR_SCAN

DESCRIPTION

  This function returns the current scan type in CSP
  
  
DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_csp_scan_e_type
 
SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrc_csp_scan_e_type rrccsp_get_curr_scan(void);


/*===========================================================================

FUNCTION   RRCCSP_GET_CURR_SCAN_NO_f3

DESCRIPTION

  This function returns the current scan type in CSP without f3
  
  
DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_csp_scan_e_type
 
SIDE EFFECTS

  None.

===========================================================================*/
rrc_csp_scan_e_type rrccsp_get_curr_scan_no_f3(void);


/*===========================================================================

FUNCTION   RRCCSP_CHECK_IF_3G_BAND_PREF_CHANGED

DESCRIPTION

  This function returns TRUE if both CSP and NAS passed 3G preferences are same
  otherwise returns FALSE  
  
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE/FALSE
 
SIDE EFFECTS

  None.

===========================================================================*/
boolean rrccsp_check_if_3g_band_pref_changed(sys_band_mask_type nas_band_mask);


/*===========================================================================

FUNCTION        RRCCSP_HANDLE_PLMN_LIST_REQ_IN_BGND

DESCRIPTION
                This function processes PLMN_LIST_REQ from MM and determines
                if PLMN list search should be done in background or foreground.
                BPLMN state is updated and appropriate timers are started within
                this function to do PLMN search in background
                
DEPENDENCIES
                None

RETURN VALUE
                TRUE: PLMN_LIST_REQ should be handled in background
                FALSE: PLMN_LIST_REQ should be handled in foreground

SIDE EFFECTS
                None

===========================================================================*/
boolean rrccsp_handle_plmn_list_req_in_bgnd(rrc_plmn_list_req_type  *plmn_list_req_ptr);


/*===========================================================================

FUNCTION        RRCCSP_HANDLE_PASSIVE_BPLMN_SUSPEND_REQ

DESCRIPTION
                This function sends RRC_BPLMN_SUSPEND_CNF to the procedure
                that sent RRC_BPLMN_SUSPEND_REQ and also determines if BPLMN
                search should be suspended or aborted based on the procedure
                that sent the suspend request.
                This function is only called when BPLMN search is passively 
                in progress

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrccsp_handle_passive_bplmn_suspend_req( rrc_cmd_type *cmd_ptr );


/*===========================================================================

FUNCTION   RRC_CSP_SEND_BPLMN_SUSPEND_CNF

DESCRIPTION

  This function is called by CSP and it sends RRC_BPLMN_SUSPEND_CNF to 
  the procedure that sent RRC_BPLMN_SUSPEND_REQ to CSP
  
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.
 
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_csp_send_bplmn_suspend_cnf(rrc_proc_e_type proc, uecomdef_status_e_type status);


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
void  rrc_csp_init_wtog_plmn_srch_int_data( void );


/*===========================================================================

FUNCTION          RRCCSP_CHECK_AND_START_PLMN_LIST_FULL_FREQ_SCAN_TIMER

DESCRIPTION       CSP uses this function to start the PLMN full scan timer.
                  As long as the timer is active, no full scans are done during
                  2G->3G and 3G->3G PLMN searches. 
                  This timer is started when PLMN full (raw/fine) scan search 
                  completes (in background or in foreground). This timer is never stopped. 
                  Additional check has been added for the NV GCF flag. If this flag 
                  is set then the timer is not started. 

DEPENDENCIES      NONE

RETURN VALUE      NONE

SIDE EFFECTS      NONE

===========================================================================*/
void rrccsp_check_and_start_plmn_list_full_freq_scan_timer( void );


/*===========================================================================

FUNCTION   RRCCSP_GET_CURR_TYPE_ACQ

DESCRIPTION

  This function returns the current acq type - RRC_CSP_SPEC_PLMN or
  RRC_CSP_ANY_PLMN      
  
DEPENDENCIES 
  None.
  
RETURN VALUE 

  rrc_csp_acq_e_type: RRC_CSP_SPEC_PLMN or RRC_CSP_ANY_PLMN
 
SIDE EFFECTS

  None.

===========================================================================*/
rrc_csp_acq_e_type rrccsp_get_curr_type_acq(void);

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
);

/*===========================================================================

FUNCTION          RRC_CSP_FIND_PPAC_RESTRICTIONS

DESCRIPTION       This function is responsible for finding the status of
                  Access Control due to Paging Permission Access Class Restrictions.

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
);

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
);

/*====================================================================================

FUNCTION          RRC_CSP_FILL_DSAC_INFO_FOR_SHARED_PLMNS

DESCRIPTION     This function fills the DSAC info for shared PLMNs from SIB3

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
);

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
rrc_csp_substate_e_type  rrccsp_rel_chls_and_go_to_idle(void);

/*===========================================================================

FUNCTION   RRCCSP_IS_GTOW_BPLMN_SRCH_IN_PROGRESS

DESCRIPTION

  This function checks if there is GTOW BPLMN search is in progress (or) not
  
DEPENDENCIES

  None.
 
RETURN VALUE

  return TRUE if GTOW BPLMN search is in progress otherwise
         FALSE
 
SIDE EFFECTS

  None.

===========================================================================*/
boolean rrccsp_is_gtow_bplmn_srch_in_progress(void);


#ifdef FEATURE_EOOS
#error code not present
#endif


/*===========================================================================

FUNCTION          RRC_CONVERT_BAND_TO_SYS_BAND

DESCRIPTION       This function takes the band used internally by RRC and 
                  converts it into sys band which is used by CM/SD.
                  
DEPENDENCIES      None

RETURN VALUE      sys band type used by CM/SD. 

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_e_type rrc_convert_band_to_sys_band(rrc_csp_band_class_type band);


#ifdef FEATURE_INTER_FREQ_EM_CALL_REDIRECTION
/*===========================================================================

FUNCTION          RRCCSP_UPDATE_ACQ_TYPE_FOR_EM_CALL_IFREQ_REDIRECTION

DESCRIPTION       Updates appropriate variables so that cell selection for
                  inter freq redirection is done for any plmn. Currently 
                  this function is called only when Emergency call is made 
                  and it is redirected to another frequency via RRC Connection 
                  Reject message.

DEPENDENCIES      NONE

RETURN VALUE      NONE

SIDE EFFECTS      NONE

===========================================================================*/
void rrccsp_update_acq_type_for_em_call_ifreq_redirection(rrc_csp_acq_e_type type_of_acq);

#endif

#ifdef FEATURE_EOOS
#error code not present
#endif /*FEATURE_EOOS*/


/*===========================================================================

FUNCTION          RRC_CSP_INIT_ADDL_CHNL_SCAN_DATA

DESCRIPTION       This function determines if additional channel scan is needed
                  or not for a particular band
                  
DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_init_addl_chnl_scan_data(void);

/*===========================================================================

FUNCTION          RRCCSP_GET_UARFCN

DESCRIPTION       This function translates barred UARFCN to the one corresponding to the
                  current band
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - if the translated UARFCN is valid
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/


boolean rrccsp_get_uarfcn(rrc_freq_type barred_uarfcn, rrc_csp_band_class_type barred_band, 
                          rrc_csp_band_class_type current_band, rrc_freq_type *uarfcn);

/*===========================================================================

FUNCTION          RRCCSP_GET_UARFCN_UL

DESCRIPTION       This function translates barred UL UARFCN to the one corresponding to the
                  current band
                  
DEPENDENCIES
                  None

RETURN VALUE
                  TRUE - if the translated UARFCN is valid
                  FALSE - otherwise

SIDE EFFECTS
                  None
===========================================================================*/


boolean rrccsp_get_uarfcn_ul(rrc_freq_type barred_uarfcn, rrc_csp_band_class_type barred_band, 
                          rrc_csp_band_class_type current_band, rrc_freq_type *uarfcn);
/*===========================================================================

FUNCTION          RRC_CSP_GET_ADDL_CHANNEL_BAND

DESCRIPTION       This function returns the additional channel scan frequency
                  
DEPENDENCIES      None

RETURN VALUE      returns the additional channel frequency
                  
SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_get_addl_channel_band(rrc_freq_type *ret_freq);


#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION 
/*===========================================================================

FUNCTION          rrccsp_do_gsm_bands_scan_first

DESCRIPTION       Decides to scan the GSM bands

DEPENDENCIES      None

RETURN VALUE      TRUE: GSM Bands should be scanned
                  FALSE: Search should not be redirected to GSM

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_do_gsm_bands_scan_first(void);
#endif /* FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION */

/*===========================================================================

FUNCTION          RRC_CSP_ADD_FREQ_TO_AVAILABLE_FREQ_LIST

DESCRIPTION       Adds a given frequency to avaialble frequency list. All the frequencies which are in the 
                  predefined range of these frequencies are skipped while scanning later.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_add_freq_to_available_freq_list(rrc_freq_type freq);


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
boolean rrc_csp_ignore_barred_rsv_status_due_to_ueut(rrc_plmn_identity_type plmn_id);

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
                                   sys_signal_strength_type sig_st );
#endif

/*===========================================================================

FUNCTION           RRCCSP_PRIORITIZE_WCDMA_BANDS_FOR_FULL_SCAN

DESCRIPTION       Constructs the order in which the WCDMA bands need to be searched while doing
                  full frequency scan.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void 
rrccsp_prioritize_wcdma_bands_for_full_scan(void);

/*===========================================================================

FUNCTION          RRCCSP_IS_BAND_ALREADY_ADDED_TO_PRIORITIZED_LIST

DESCRIPTION       Determines whether a band given is already present in the prioritized band list.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
boolean 
rrccsp_is_band_already_added_to_prioritized_list(
  rrc_csp_band_class_type freq_band,
  uint8 num_of_bands
);


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
);

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
/*===========================================================================

FUNCTION        RRCCSP_CONVERT_ACTIVE_GSM_CHANNELS_TO_UMTS_UARFCNS

DESCRIPTION     This function converts GSM active ARFCNS passed by GSM-RR to equivalent 
                UMTS UARFCNS.

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrccsp_convert_active_gsm_channels_to_umts_uarfcns
(
  rr_rrc_gsm_cell_list_type  gsm_cell_list
);
#endif

#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
#error code not present
#endif

/*===========================================================================

FUNCTION        RRCCSP_SEND_RR_RRC_STOP_GSM_REQ

DESCRIPTION     This function sends RR_RRC_STOP_GSM_REQ to RR

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrccsp_send_rr_rrc_stop_gsm_req
(
  rrc_rr_stop_wcdma_cause_e_type stop_cause
);

/*===========================================================================

FUNCTION        RRCCSP_SEND_RRC_RR_STOP_WCDMA_CNF

DESCRIPTION     This function sends RRC_RR_STOP_WCDMA_CNF to RR

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrccsp_send_rrc_rr_stop_wcdma_cnf
(
  rrc_rr_stop_wcdma_cause_e_type stop_cause
);

#ifdef FEATURE_WCDMA_TO_TDSCDMA
/*===========================================================================

FUNCTION        RRCCSP_SEND_TDSRRC_RRC_STOP_TDS_REQ

DESCRIPTION     This function sends TDSRRC_RRC_STOP_TDS_REQ to TDS RRC

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrccsp_send_tdsrrc_rrc_stop_tds_req
(
  rrc_rr_stop_wcdma_cause_e_type stop_cause
);

/*===========================================================================

FUNCTION        RRCCSP_SEND_RRC_TDSRRC_STOP_WCDMA_CNF

DESCRIPTION     This function sends RRC_TDSRRC_STOP_WCDMA_CNF to TDS RRC

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrccsp_send_rrc_tdsrrc_stop_wcdma_cnf
(
  rrc_rr_stop_wcdma_cause_e_type stop_cause
);
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
);
#endif
#endif

#ifdef FEATURE_GAN_3G
#error code not present
#endif

/*===========================================================================

FUNCTION          RRC_CSP_GET_LIST_DB_MATCH_ACQ_ENTRY

DESCRIPTION    This function will match the frequency and PLMN id of the entry received from L1 as part of the 
                         frequency scan confirmation against the ACQ DB entries. If the match is successful  pointer to the acq list
                         entry will be returned to the called function otherwise NULL will be returned. GSM scan will be initiated if the 
                         List DB exhausts and the criteria for GSM Scan satisifies..

DEPENDENCIES
                  None

RETURN VALUE
                  Pointer to acquisition entry. A NULL pointer means no 
                  acquisition entry was found that fulfilled selection criteria.

SIDE EFFECTS
                  None.

===========================================================================*/
rrc_csp_acq_entry_type*  
rrc_csp_get_list_db_match_acq_entry(rrc_csp_freq_scan_e_type *ret_type_freq_scan );
                                                                              

/*===========================================================================

FUNCTION        RRCCSP_CHECK_FOR_EPLMN

DESCRIPTION     This function checks the network select mode, and accordingly calls the NAS fucntion to check the
                EPLMN list

DEPENDENCIES
                None

RETURN VALUE    If the network select mode is
                >LIMITED     returns TRUE
                >Otherwise   return the return value of sys_eplmn_list_equivalent_plmn

SIDE EFFECTS
                None

===========================================================================*/
boolean rrccsp_check_for_eplmn
(
  sys_plmn_id_s_type nas_plmn_id
);

#ifdef FEATURE_DUAL_SIM


/*===========================================================================

FUNCTION  RRC_CHECK_CAN_PREEMPTION_BE_HANDLED

DESCRIPTION

  This function checks if preemption can be handled.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE If pre-emption can be handled.
  else
  FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_check_can_preemption_be_handled(void);

/*===========================================================================

FUNCTION  rrc_check_can_preemption_be_handled_in_FACH_DCH

DESCRIPTION

  This function checks if preemption can be handled in FACH & DCH states
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE If pre-emption can be handled.
  else
  FALSE.

SIDE EFFECTS

  None.
===========================================================================*/

boolean rrc_check_preemption_can_be_handled_in_fach_dch(void);

/*===========================================================================

FUNCTION  rrc_check__can_rrc_handle_preemption_in_fach_dch

DESCRIPTION

  This function checks if RRC can handle preemption  in FACH & DCH states
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE If pre-emption can be handled.
  else
  FALSE.

SIDE EFFECTS

  None.
===========================================================================*/

boolean rrc_check_can_rrc_handle_preemption_in_fach_dch(rrccsp_preemption_cause_e_type cause_preemption );


/*===========================================================================

FUNCTION  rrc_check_can_l1_handle_preemption_in_fach_dch

DESCRIPTION

  This function checks if L1 can handle preemption  in FACH & DCH states
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE If pre-emption can be handled.
  else
  FALSE.

SIDE EFFECTS

  None.
===========================================================================*/

boolean rrc_check_can_l1_handle_preemption_in_fach_dch(void);

/*===========================================================================

FUNCTION  rrc_check_can_nas_handle_preemption_in_fach_dch

DESCRIPTION

  This function checks if NAS can handle preemption  in FACH & DCH states
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE If pre-emption can be handled.
  else
  FALSE.

SIDE EFFECTS

  None.
===========================================================================*/

boolean rrc_check_can_nas_handle_preemption_in_fach_dch(rrccsp_preemption_cause_e_type cause_preemption );


/*===========================================================================

FUNCTION  rrc_csp_handle_preemption_in_FACH_DCH

DESCRIPTION

  This function determines the next sub state in FACH & DCH states
    
DEPENDENCIES

  None.
 
RETURN VALUE

Next sub state

SIDE EFFECTS

  None.
===========================================================================*/

rrc_csp_substate_e_type rrc_csp_handle_preemption_in_fach_dch(rrc_cmd_type *);
/*===========================================================================

FUNCTION  rrc_csp_initiate_CU_for_immediate_preempt_in_FACH_DCH

DESCRIPTION

  This function iniiates Cell update req in FACH & DCH states
    
DEPENDENCIES

  None.
 
RETURN VALUE

Next sub state

SIDE EFFECTS

  None.
===========================================================================*/

rrc_csp_substate_e_type rrc_csp_initiate_CU_for_immediate_preempt_in_fach_dch(void);


/*===========================================================================

FUNCTION  RRC_HANDLE_PREEMPTION_IND

DESCRIPTION

  This function handles pre-emption indication and aborts the ongoing procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  Cell Selection Procedure substate.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_csp_substate_e_type rrc_handle_preemtpion_ind(  rrc_cmd_type * );

/*===========================================================================

FUNCTION  RRC_SP_HANDLE_NO_RESOURCE_FROM_L1

DESCRIPTION

  This function handles no resource from L1 ,i.e the case when L1 is not able
  to acquire lock. RRC starts out of service search for such cases.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  CSP substate

SIDE EFFECTS

  None.

===========================================================================*/
rrc_csp_substate_e_type rrc_csp_handle_no_resource_from_l1(void);


/*===========================================================================

FUNCTION  RRC_CSP_WAIT_FOR_WRM_LOCK

DESCRIPTION

  This function calls another function to request WRM for lock and takes action
  according to the lock status returned.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  CSP substate.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_csp_substate_e_type rrc_csp_wait_for_wrm_lock(rrc_csp_freq_scan_e_type *type_freq_scan);

/*===========================================================================

FUNCTION  RRC_CSP_CONTINUE_WAIT_FOR_WRM_LOCK

DESCRIPTION

  This function starts cells seletion afresh when RRC is not able to acquire lock
  long time.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  CSP substate

SIDE EFFECTS

  None.

===========================================================================*/
rrc_csp_substate_e_type rrc_csp_continue_wait_for_wrm_lock(rrc_csp_freq_scan_e_type *type_freq_scan);


/*===========================================================================

FUNCTION  RRC_SEND_DS_STATUS_IND_TO_L1

DESCRIPTION

  This function sends dual standby status informed by NAS to L1.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_send_ds_status_ind_to_l1
(
  rrc_cmd_type * cmd_ptr
);

/*====================================================================
FUNCTION: rrc_send_no_resource_available_from_l1_to_csp()

DESCRIPTION:
  This function posts NO_RESOURCE_AVAILABLE_FROM_L1 command to 
  internal command queue.

DEPENDENCIES:
  None

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_send_no_resource_available_from_l1_to_csp(void);


/*===========================================================================

FUNCTION  RRC_CSP_REL_LOCK_AND_CONT_CELL_SEL_ON_PREV_PREEMPTED

DESCRIPTION

  This function is called after L1 has released lock due to preemption. This function then releases
  RRC lock and starts cell selection from the previous state.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  CSP substate

SIDE EFFECTS

  None.

===========================================================================*/
rrc_csp_substate_e_type rrc_csp_rel_lock_and_cont_cell_sel_on_prev_preempted(void);

/*===========================================================================

FUNCTION  RRC_CSP_WAITING_TO_RELEASE_LOCK

DESCRIPTION

  This function is called to check if RRC is waiting for L1 to release lock.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE: RRC is waiting for L1 to release lock.
  else
  FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_csp_waiting_to_release_lock(void);

/*===========================================================================

FUNCTION  RRC_CSP_IS_PREEMPTION_HANDLED

DESCRIPTION

  This function checks the CSP substate and sends RESET_L1_REQ to L1.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  boolean
  Indicates whether preemption handling is taken care or not

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_csp_is_preemption_handled(void);

/*===========================================================================

FUNCTION  RRC_CSP_HANDLE_GTOW_LOCK_FAILURE_OR_PREEMPTION

DESCRIPTION

  This function aborts the ongoing inter-RAT procedure in case of pre-emption
  or abort requests from RR.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  CSP substate.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_csp_substate_e_type rrc_csp_handle_gtow_lock_failure_or_preemption
(
rrc_out_of_lock_wait_e_type wrm_lock_status, 
boolean abort_due_to_preemption
);

/*===========================================================================

FUNCTION        rrc_csp_handle_wtow_bplmn_lock_failure

DESCRIPTION     This function will handle the case of lock not granted for WTOW BPLMN

DEPENDENCIES
                None

RETURN VALUE
                Next CSP substate

SIDE EFFECTS
                None

===========================================================================*/
rrc_csp_substate_e_type rrc_csp_handle_wtow_bplmn_lock_failure
(
  rrc_out_of_lock_wait_e_type wtrm_lock_status /*For now no use of this, can plan to use this later*/
);

/*===========================================================================

FUNCTION          rrc_reset_oos_due_to_no_resource_flag

DESCRIPTION       This function is called to reset the flag which track OOS due to 
                          no resource.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_reset_oos_due_to_no_resource_flag(void);

/*===========================================================================

FUNCTION          rrc_set_oos_due_to_no_resource_flag

DESCRIPTION       This function is called to set the flag which track OOS due to 
                          no resource.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_set_oos_due_to_no_resource_flag(void);

/*===========================================================================

FUNCTION          rrc_get_oos_due_to_no_resource_flag_status

DESCRIPTION       This function is called to get the flag status which tracks OOS due to 
                          no resource.

DEPENDENCIES      None

RETURN VALUE      boolean

SIDE EFFECTS      None
===========================================================================*/
boolean rrc_get_oos_due_to_no_resource_flag_status(void);

/*===========================================================================

FUNCTION          RRC_CONVERT_BAND_TO_SYS_BAND_CLASS

DESCRIPTION       This function takes the band used internally by RRC and 
                  converts it into sys band class is used by TRM
                  
DEPENDENCIES      None

RETURN VALUE      sys band class used by TRM. 

SIDE EFFECTS      None
===========================================================================*/
sys_band_class_e_type rrc_convert_band_to_sys_band_class(rrc_csp_band_class_type band);

#endif
#ifdef FEATURE_WCDMA_DB_DC_HSDPA
/*===========================================================================

FUNCTION          rrc_convert_sys_band_class_to_rrc_band

DESCRIPTION       This function takes the sys band class and 
                  converts it into rrc band
                  
DEPENDENCIES      None

RETURN VALUE      sys band class used by TRM. 

SIDE EFFECTS      None
===========================================================================*/
rrc_csp_band_class_type rrc_convert_sys_band_class_to_rrc_band(sys_band_class_e_type band_class);
#endif
/*===========================================================================

FUNCTION          rrccsp_check_wtog_abort_in_progress

DESCRIPTION       This function checks if W2G procedure abort is in progress.
                  If so, STOP WCDMA should be pended till WTOG search abort is 
                  completed
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
boolean rrccsp_check_wtog_abort_in_progress(
  rrc_stop_cause_e_type stop_cause
);

#ifdef FEATURE_BPLMN_SEARCH_320MS
/*===========================================================================

FUNCTION          RRC_CSP_CHECK_320MS_BPLMN

DESCRIPTION       This functions checks if CSP is required to do 320ms lossy BPLMN

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
boolean rrc_csp_check_320ms_bplmn(void);

/*===========================================================================

FUNCTION          RRC_SUB_SFN

DESCRIPTION       This function subtract two system frame numbers.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
uint16 rrc_sub_sfn(uint16 sfn, uint16 subtract_by);

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
);


#if defined (FEATURE_LTE_TO_WCDMA) || defined (FEATURE_WCDMA_TO_LTE)
#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION          RRCCSP_HANDLE_RESELECTION_REQ_FROM_LTE

DESCRIPTION       This function process the reselection request from LTE

DEPENDENCIES      None

RETURN VALUE      Next CSP Substate

SIDE EFFECTS      None
===========================================================================*/
rrc_csp_substate_e_type rrccsp_handle_reselection_req_from_lte
(
  rrc_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION          RRCCSP_HANDLE_PLMN_SRCH_REQ_FROM_LTE

DESCRIPTION       This function process the plmn search request from LTE

DEPENDENCIES      None

RETURN VALUE      Next CSP Substate

SIDE EFFECTS      None
===========================================================================*/
rrc_csp_substate_e_type rrccsp_handle_plmn_srch_req_from_lte
(
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION          RRCCSP_HANDLE_REDIRECTION_REQ_FROM_LTE

DESCRIPTION       This function process the redirection request from LTE

DEPENDENCIES      None

RETURN VALUE      Next CSP Substate

SIDE EFFECTS      None
===========================================================================*/
rrc_csp_substate_e_type rrccsp_handle_redirection_req_from_lte
(
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION          RRCCSP_HANDLE_CGI_REQ_FROM_LTE

DESCRIPTION       This function process the cgi request from LTE

DEPENDENCIES      None

RETURN VALUE      Next CSP Substate

SIDE EFFECTS      None
===========================================================================*/

rrc_csp_substate_e_type rrccsp_handle_cgi_req_from_lte
(
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION          RRCCSP_CHECK_CGI_SCAN

DESCRIPTION       This function checks whether cgi scan is enabled

DEPENDENCIES      None

RETURN VALUE      Next CSP Substate

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_check_cgi_scan(void);

#endif

#ifdef FEATURE_WCDMA_TO_LTE
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
);

#endif

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
  detected_frequency_list_type * detected_frequency_list
);
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
);
#endif
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
);

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
);

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
);


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
);
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
);


#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION          rrc_csp_handle_bplmn_suspend_lock_rel_offset_tmr_running

DESCRIPTION       This function is called to put WL1 to sleep, stop the 10ms lock 
                  release offset timer and stop BPLMN DRX timer
                  This function is called on handling BPLMN suspend and BPLMN cleanup

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_handle_bplmn_suspend_lock_rel_offset_tmr_running
(
  void
);

/*==========================================================================
FUNCTION rrccsp_populate_req_bands_for_lock_during_cell_sel

DESCRIPTION
  This function is called to populate bands on which lock is required.
  During Cell Selection. based on the scan type, this function will populates band information

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/

void rrccsp_populate_req_bands_for_lock_during_cell_sel(rrc_csp_freq_scan_e_type type_freq_scan, wrm_freq_input_type *lock_req_bands);
#endif
/*===========================================================================

FUNCTION          rrccsp_check_sib1_validity

DESCRIPTION       This function calls NAS API to verify LAC info provided in SIB1

DEPENDENCIES      None

RETURN VALUE      TRUE: LAC info is valid.
                  FALSE: Otherwise. 

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_check_sib1_validity
(
  rrc_SysInfoType1  *sib1
);

#ifdef FEATURE_3GPP_CSFB
/*===========================================================================

FUNCTION          rrccsp_get_idx_sib_cont_list

DESCRIPTION       This function returns idx to the PSC in the SIB container list

DEPENDENCIES      None

RETURN VALUE      Index to the PSC in the SIB container list if present
                          WCDMA_RRC_MAX_UTRA_CONT Otherwise

SIDE EFFECTS      None
===========================================================================*/
uint8  rrccsp_get_idx_sib_cont_list
(
  uint16 psc
);

/*===========================================================================

FUNCTION          rrccsp_free_sib_cont_list

DESCRIPTION       This function will free the memory allocated for SIB cont list. This will be called on 
                         LTOW redirection initialization, successful camping for LTOW redirection, CSP int data init

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_free_sib_cont_list
(
  void
);

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
);
#endif

/*===========================================================================

FUNCTION        RRC_IS_S_CRITERIA_MET

DESCRIPTION
                This function is called during GTOW and WTOW Automatic BPLMN search 
                to determine if the cell satisfies S-criteria before adding the PLMN 
                to the available PLMN list. 

DEPENDENCIES
                None

RETURN VALUE
                TRUE:  S criteria met
                FALSE: S criteria not met

SIDE EFFECTS
                None

===========================================================================*/
boolean rrc_is_s_criteria_met(rrc_plmn_identity_type plmn_id, rrc_int_cell_id_type cell_id);

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
);

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION          rrccsp_reset_flag_for_excluding_pre_redir_info

DESCRIPTION       This function resets the flag to not include pre-redir info in 
                  connection request message.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void rrccsp_reset_flag_for_excluding_pre_redir_info(void);

/*===========================================================================

FUNCTION          rrccsp_exclude_preredirection_info

DESCRIPTION       This function checks if pre-redirection info has to be excluded
                  from RRC connection request message.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
boolean rrccsp_is_preredirection_info_to_be_excluded(void);
#endif

/*===========================================================================

FUNCTION          rrccsp_init_first_band_to_be_scanned

DESCRIPTION       This function will init the first band to be scanned.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_init_first_band_to_be_scanned(void);

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/*===========================================================================

FUNCTION  rrc_sm_tuneaway_allowed

DESCRIPTION
  This function checks if tuneaway is allowed

DEPENDENCIES
  None.

RETURN VALUE
  TRUE If SM allows tuneaway
  else
  FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean rrc_sm_tuneaway_allowed(void);
#endif

/*===========================================================================

FUNCTION        rrc_check_serv_cell_s_crit

DESCRIPTION
                This function is called after SIB3 for the serving cell
                to determine if the cell satisfies S-criteria before proceeding
                with reading of other SIBs

DEPENDENCIES
                None

RETURN VALUE
                TRUE:  S criteria met
                FALSE: S criteria not met

SIDE EFFECTS
                None

===========================================================================*/
boolean rrc_check_serv_cell_s_crit(rrc_SysInfoType3 * sib3_ptr);

/*===========================================================================

FUNCTION          rrc_get_rr_inter_rat_bplmn_prtl_result

DESCRIPTION       WRRC Calls the respective RR API to get Partial PLMN results from RR during ongoing WtoG
                  Manual PLMN Search

DEPENDENCIES
                  None
                  
RETURN VALUE
                  Boolean

SIDE EFFECTS
                  None
===========================================================================*/
boolean rrc_get_rr_inter_rat_bplmn_prtl_result(sys_detailed_plmn_list_s_type *available_plmn_list);


/*===========================================================================

FUNCTION        rrc_csp_return_pending_type

DESCRIPTION
                This function returns the CSP pending command type.

DEPENDENCIES
                None

RETURN VALUE
                CSP Pending type.

SIDE EFFECTS
                None

===========================================================================*/

rrc_csp_pending_e_type rrc_csp_return_pending_type(void);

/*===========================================================================

FUNCTION   RRC_CSP_CLEANUP_NEEDED_BPLMN_DATA

DESCRIPTION

  This function cleans the needed BPLMN data after sending RRC_BPLMN_SUSPEND_REQ to CSP
  to suspend the ongoing Manual or Automatic Background PLMN Search.
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.
 
SIDE EFFECTS

  None.

===========================================================================*/


void rrc_csp_cleanup_needed_bplmn_data(void);

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION          rrccsp_is_csg_white

DESCRIPTION       This function will return lte_resel_allow_status

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_is_csg_white(uint32 csg_id, rrc_plmn_identity_type plmn_id);

/*===========================================================================
FUNCTION          rrccsp_is_csg_white_empty

DESCRIPTION       This function will return lte_resel_allow_status

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_is_csg_white_empty(void);
#endif/*FEATURE_FEMTO_CSG*/

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
boolean rrc_csp_is_suitable_srch_ended(void);

/*==========================================================================
FUNCTION rrccsp_ignore_cu_due_to resel_cell_change_in_progress

DESCRIPTION
  This function is called to know if reselection is in progress to ignore CUC.

Scenario: Cell reslection is triggered during cell_update.
 
          CUC config and new cell config collision causes crash in L1. 
          So it is required to ignore CUC when reselecting to new cell and 
          UE database is getting updated due to that.
          There are two scenario henceforth and according fix added in the function -

          1. CUC is recieved just before CELL_CHANGE_REQ could reach CCM. 
              At this point CCM status still not set to indicate 
              reselection as cell_change_req is not reached to CCM yet.
              Hence This function is written to ignore CUC based on CSP substate indicating 
              reselection in progress. 

          2. CUC is received after CCM status updated to indicate reselection
              This is when cell_change_req already reached CCM. 
              To take care this, new function also has check on existing function 
              rrc_ccm_is_reselection_in_progress as well.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : reselection is in progress
  FALSE: reselection is not in progress 

SIDE EFFECTS
  None
===========================================================================*/
boolean rrccsp_ignore_cu_due_to_resel_cell_change_in_progress(void);

#endif

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_CGI_RSP

DESCRIPTION       This function is responsible for sending
                  WCDMA_RRC_LTE_CGI_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_lte_interrat_cgi_rsp
(
  void
);
#endif

/*===========================================================================

FUNCTION          rrccsp_check_redirection_barred

DESCRIPTION       This function is called to check if redirected freq/PSC is barred or not

DEPENDENCIES      None

RETURN VALUE      Return true if frequency is barred

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_check_redirection_barred(rrc_cmd_type  *rrc_cmd_ptr);

/*===========================================================================

FUNCTION          RRC_CSP_SET_DRX_LENGTH

DESCRIPTION       This function is responsible for setting the DRX length

DEPENDENCIES      None

RETURN VALUE      uint32 -- drx_length

SIDE EFFECTS      None
===========================================================================*/
uint32 rrc_csp_set_drx_length(uint32 drx_len);