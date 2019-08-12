/*===========================================================================
               D O W N L I N K   R A D I O   L I N K   C O N T R O L

DESCRIPTION
  This file provides the functions to process RLC Downlink logical channels.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  List any initialization and sequencing that is required for this module.

Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlcdl.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who     what, where, why
--------  ---     -----------------------------------------------------------
05/10/16  as      Remove excessive F3s
03/28/16  kc      Adding changes for WM register/deregister confirmation
10/12/15  scv     Prevent taking DACC Mutex after EUL ISR Mutex in Task context
10/12/15  scv     Changes for DSM Multiple DeQ/EnQ
07/16/15  scv     Check for NON-NULL dl_reseq_q pointer before dereferencing
06/09/15  aa      Replaced RLC UM/AM/TM config log packets with new packets
04/29/15  scv     RLC_ENH: Avoid RESET due to MaxDAT while higher priority LC is receiving DL traffic
02/26/15  kv      If SDU's pending return value TRUE to RRC
01/22/15  kv      Fix to process Poll when status report and status prohibit are true for particular rlc-id
10/17/14  ts      Fix to discard DL PDUs received while in the Reset Pending State, to avoid potential ciphering mismatch right after the recovery
10/10/14  kv      Fix to prevent race condition between l2_dl task and rrc task
09/12/14  aa      Fix to avoid deadlock between EUL ISR datapath mutex and RLC FC mutex
07/26/14  rkmk    Reduce image size by using right macro depending upon number of parameters
05/06/14  geg     Use safer versions of memcpy() and memmove()
03/26/14  kv      F3's to indicate lo,hi,dne WM counts for UL and DL channels during LC registration
                  and current_count and highest_count values
01/30/14  kv      0x421E/0x4220 Logging Fixes for 3C and WM debug prints
01/16/14  scv     Fix Compiler Warnings
01/13/14  kv      Fixed KW error  
01/09/14  scv     Code changes for UL Compression Feature
12/09/13  as      Code change to EDRX full sleep if there pending partials at MAC. 
11/08/13  as      Added code change not to disable RACH if there is any pending data in DL 
                  that needs to be received which in turn would not trigger EDRX full sleep.
11/20/13  kv      Moved mask for MSG_4to9 from MSG_SSID_DIAG to MSG_SSID_WCDMA_RLC
05/08/13  scv     Fix L2 compilation errors
05/03/13  as      Change to discard DL PDUs if PDU health passed by decoder is false
02/01/13  ts      Added UE log mask check in R99 code path
01/21/13  as      Added code check to enqueue DL data to Watermark queue only 
                  if its length is less than 1503 bytes.
01/17/13  as      Removed unwanted F3's.
01/15/13  as      Fix to reset UL state to DATA_TRANSFER_READY from RESET_PENDING 
                  during one sided DL re-establishment procedure. 
01/04/13  kc      ZI reduction related changes
12/20/12  kc      Fix for KW error
08/17/12  as      Fix to delay tune-away if there is any pending DL AM SRB PDU
                  to be received.
08/29/12  grk     Fix to synchronize DSM app_field updation between MAC and RLC
07/13/12  grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
08/07/12  grk     Added mutex protection to avoid WM deregistration while DL
                  TM PDUs are being processed.
07/24/12  geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12  zr      Adding feature definitions for Dime
06/18/12  mrg     L2 - MAC-I and HS-RACH code changes
04/26/12  grk     Mainlined code under FEATURE_INACTIVITY_HANDLING.
04/09/12  grk     Fix to handle DL only reconfiguration correctly.
02/08/12  grk     Fix to initialize DL base timer if FEATURE_WCDMA_RLC_ZI_REDUCTION_PHASE3
                  is defined.
02/02/12  Ksr     Mutex lock and unlock sequences are updated in RLC to ensure it will not block ISR for a long time
02/16/12  Ksr     Added support for Trigger Based Rach Procedure
02/01/12  grk     Feature cleanup
01/23/12   as     Support for Free Floating Task changes
12/06/10  grk     Added periodic PS<->RLC DL WM traces at RLC
11/23/11  grk     Fix to initiate new RESET procedure, for lower RESET HFN, 
                  only after earlier UL RESET procedure is completed.
10/11/11  grk     Fix to move PDU discarding(when WM is not registered) for TM
                  PDUs from ISR to task context.
11/07/11  grk     Fix to process UL only re-establishment correctly 
11/02/11  grk     Fix to update ciphering flags correctly for PDUs not to be 
                  deciphered by default.
10/18/11  grk     Moved log_status check for 0x4168 log packet from header decode
                  to config.
10/27/11  as      Restricting the RLC fixes provided to generate periodic status report if
                  there is hole detected and no TSP is configured(CR#275752) to SRB only.
10/10/11  grk     Fix to process UL only re-establishment correctly
07/18/11  grk     Fix to avoid infinite loop on W UL during RESET procedure.
08/26/11  grk     Fix for KW errors
04/25/11  grk     Fix to skip updating count_c/hfn when old ciphering configuration
                  is used.
03/09/11  ssg     MAC-ehs support on Apollo.
03/08/11  kvk     Added support to generate periodic status report if there is hole detected
                  and no TSP is configured
03/02/11  kvk     Added support for PDU/SDU print and also Invalid SDU detection >1500
01/31/11  pj      Added proper Featurization
01/17/11  vp      CMI Phase-4 clean-up of header files
01/12/11  grk     Compilation fixes for non-L2 opt.
12/07/10  pj      1. Added changes to populate rlc_sn along with hfn for rrc
                  2. Added debug print to log nchan during cipher config
11/30/10  pj      L2 mutex cleanup
11/26/10  pj      Added mutex protection changes for non-Q6 targets
10/18/10  ssg     Added changes for new WCDMA-L2 interface.
11/12/10  pj      moved "REX_ISR_LOCK" code under FEATURE_Q6_MT
11/10/10  grk     Added fix to properly check DL-SDU status
10/27/10  grk     Added API to check DL SDU status.
10/20/10  av      Fixing compilation warnings
10/19/10  kvk     Added support to flush rlc_tsn_info_q when HS is stopped
09/30/10  pj      removed return statement from error loggign function
09/20/10  pj      Added support to log erroneous conditions
09/19/10  pj      Added fix to use proper flag, while capturing DL ciphering log-packet
09/19/10  av      HS_FACH and Enhanced 0x4220 log changes
09/15/10  pj      1. Added fix to use proper index while accessing downlink_table
                  2. Added fix to count active DL_AM channels, after 
                     current released channel is set to NULL_state
                  3. Added F3s for dumping debug prints of base timer
08/12/10  grk     Fix to use future ciphering activation for new ciphering 
                  config activation. 
07/07/10  pj      Added changes to use a global base timer for status processing
05/18/10  pj      Added ZI reduction changes to reduce memory used in NAK pool 
05/25/10  pj      Added debug code to print nchan info during WM registration
05/14/10  pj      Fixed KW/lint warning
05/10/10  pj      Added support for DL Datapath Log packet
22/04/10  pj      Modified cipher function to accept DL-cipher info ptr
04/13/10  grk     changes for ZI memory reduction.
04/12/10  grk     Fixed compilation error on silver MOB.
                  1. Removed test header inclusion in source files.
                  2. Stubbed L2 function calls.
03/30/10  grk     Changes for RLC QTF.
03/18/10  kvk     Supressed redudants F3
02/09/10  grk     Fixed KW warnings.
12/23/09  pj      Moved DSM-trace related code before TSNs are deliverd to RLC (from MAC-HS)
12/07/09  grk     Changes to return valid deciphering parameters for PDUs 
                  received before receiving RESET_ACK (in case DL is in 
                  RESET_PENDING due to UE initiated RESET) and drop PDUs received 
                  after receiving RESET_ACK till DL comes out of RESET_PENDING.
11/30/09  grk     Added fix to use correct LSN in case NAK LIST is split across
                  TTIs
11/24/09  pj      Added changes to process resetting status timer 
10/08/09  pj      Added changes to to yield L2-DL task (out of PDU-processing) 
11/05/09  grk     Fixed compilation error/warnings for genesis.
10/27/09  av      Fixing mob compile warnings 
10/21/09  grk     1.Added new DL state variable to keep track of SN till which
                    DL status array is updated dl_vr_h_status. 
                  2.Fix to use vr_r as last NAK SN in case dl_vr_h_status is less 
                    than vr_r.
10/15/09  grk     Fix to send status for PDUs only after corresponding NAK 
                  status is updated.
09/17/09  pj      1. Added changes to left-shift header bytes by 8-bits for logging UM-PDUs
                  2. Added changes to avoid logging UM-PDUs if 0x4332 log-packet is disabled
10/07/09  av      Multi-client flow control algorithm 
10/06/09  grk     Fixed lint warnings.
10/01/09  grk     Fixed KW warnings.
09/29/09  grk     Fixed KW warnings.
09/09/09  grk     Fix to populate app_ptr for DUP DSM items.
09/07/09  kvk     Added support for TSN tracing
08/25/09  prk     Changes to free the TSN if the num_complete_pdus is set to zero for
                  mac-hs as well
08/13/09  grk     Changes for bitmap based status tracking.
08/12/09  pj      1. Added code-changes to avoid discarding TM-PDUs in interrupt-context.
                  2. Reverted the code changes to discount number of pdus-processed
                     from total-rcvd-pdus
07/22/09  pj      1. Added protective code to discard PDUs rcvd on a unestablished 
                     TM logical-channel
                  2. Added code-changes to discount number of pdus-processed from total-rcvd-pdus
07/22/09  pj      Added code-changes to avoid appending partial-dsm-chians (within dl_pdu_info) 
                  to partial_sdu_optr, if DISCARD-FLAG is TRUE in UM-mode
07/30/09  grk     Added debug code.
07/13/09  grk     Cleaned up F3s in DL deciphering path.
07/06/09  av      R99 & L2 Mutex cleanup
07/03/09  grk     Modified cipher algo checks in accordance with new 
                  ciphering algo enum (added as part of snow3g support).
07/01/09  rm      Fixed Lint error.
07/01/09  grk     Lint warning fixes.
06/25/09  kvk     Fixed PDU header decoding if no cipherign is configured and muliple logical channels 
                  mapped to same Trch
06/25/09  grk     Added support for future ciphering activation, when SMC is
                  is received with activation sequence number less than vr_h.
06/12/09  av      Added feature to genrate RLC event during DL config
06/03/09  kvk     Refined logic for PDU/SDU trace dump
06/02/09  kvk     Added PDU and SDU dumping functions
05/29/09  ssg     Added support for SNOW_3G algo.
05/27/09  grk     R99/EUL mutex changes to backup the mutex(R99/EUL)
                  being locked and unlock the same.
05/13/09  ssg     Added fix to prepare NAKS when POLL SUFI is received.
05/15/09  grk     Added changes for R99 mutex.
05/12/09  rm      Fixed Lint Errors
05/11/09  ssg     Updated Copyright Information
04/29/09  av      Fixed compiler warnings in L2 
02/25/09  pj      Added debug prints to dump dsm contents (associated with PDU/ SDU-chain being freed) 
                  under RLC_DBG_REASSEMBLY_1 flag
04/09/09  kvk     Added support for backup and restore FC params for RLC reset params
04/02/09  kvk     Diacrd the last PDU(p-t-c) if the tb_id in the TSN structure is set to 0xFF
02/25/08  pj      Added requried changes to calculate report_vr_h value while 
                  copying NAKs to TX-NAK queue 
03/13/09  grk     Off-target lint warning fixes.
03/12/09  grk     Added fix to ignore RLC AM PDUs while in RESET_PENDING state.
03/11/09  pj      Added code changes to have UL/DL-task & EUL-ISR concurrency 
                  protection under light-weight mutex
01/30/09  grk     Added NULL checks for RLC timers (creation/usage).
01/28/09  grk     Merged missed out fixes from taxis to mainline.
01/22/09  grk     Merged L2 mutex changes from taxis to mainline.
16/01/08  grk     Cleanup for F3s under FEATURE_WCDMA_RLC_PROFILING flag.
01/09/09  kvk     Moved F3s under debug varibale
12/31/08  ssg     Moved F3s under debug variable.
12/18/08  ksr     L2 Mutex changes to avoid concurrency issues
12/10/08  pj      Added fix to consider BMC mode, while calculating hdr length
12/10/08  pj      Added support for processing BMC mode PDUs in Data path opt code
12/05/08  grk     compilation fix for 8200 (rel7).
11/22/08  kvk     Now sharing Mutex with L1 for all L2 to avoid memory concurrency issues
11/21/08  kvk     Replaced INTLOCK/INTFREE with REX_ISR_LOCK/REX_ISR_UNLOCK to avoid timer cb
                  avoid memory concurreny between timer task and WCDMA L2 DL task
10/15/08  kvk     INTLOCK/INTFREE used to avoid memory concurrency between timer cb ISR and
                  L2 DL task
10/23/08  pj      Removed "DL WM != NULL" check before processing TM PDUs in task context
10/23/08  pj      Added definition for rlci_chk_dl_reset_pending() function 
10/17/08  grk     Merged taxis changes to mainline.
10/15/08  ksr     clear the num log packets for every TTI
10/03/08  pj      Added fix to log UM PDUs, only if dsm_extract() is successful (LINT WRN)
09/11/08  pj      Added the debug message under FEATURE_RLC_REPORT_VR_H_ACK_SN flag
09/10/08  ssg     Added F3 to print rxd control PDU contents.
09/02/08  cnp     Replaced intlock/free with wcdma_intlock/free.
          cnp     Replaced tasklock/free with wcdma_tasklock/free.
          cnp     Replaced intlock/free_sav with wcdma_intlock/free_sav.
08/26/08  pj      Added fix to discard all the Data Pdus received, 
                  when a LC is in RESET pending state
08/26/08  pj      Added debug code for logging ciphering info within rlc_debug_var                  
08/22/08  ssg     Added debug F3s(w.r.t. ACK not sending)
08/21/08  pj      Added fix to free resquence queue element during re-establishment in 
                  RLC_MULTIPLE_OPERATIONS case
07/21/08  ssg     Added debug code to DSM memory leaking under RLC_DBG_DSM_MEM_LEAK.
07/21/08  ssg     Added fix to store un released MBMS channel back during 
                  CLRC_RELEASE_ALL_REQ.
06/27/08  pj      Added Debug-F3 under rlc_debug_var flag
06/20/08  ssg     Proper featurization of FEATURE_MODEM_MBMS during CLRC_RELEASE_ALL_REQ
06/12/08  ssg     Added fix to release MBMS channels by CLRC_RELEASE_ALL_REQ.
06/11/08  pj      Added fix for mis-alignment access issue
06/09/08  pj      Added fix for mis-alignment access issue
05/30/08  ssg     Added fix to not send a STATUS report for MAC-hs reset 
                  when RLC Reset procedure is ongoing.
05/21/08  ssg     Added fix to set report_vr_h to vr_h after completing copy 
                  of NAKs from DL nak_list_q to UL tx_nak_q.
05/19/08  kvk     1)RLC discards the PDU in case if the RLC LC is not configured and 
                    MAC delivers the PDU on that particular LC (HS and Non HS)
                  2)rlc_dl_decode_pdu_hdr will now return boolean status
05/16/08  ssg     Added functionality for DL stop, DL continue request.
05/06/08  ssg     Removal of temporary AR_FIX code.
04/25/08  ksr     app_field is already discounted while putting in PDU into SDU pointer. 
                  While discarding SDU, discard mechanism is updated to discard only 
                  when app_field is 0
          ksr     Added F3 msgs under global debug flag
03/28/08  pj      Added error handling  to discard DSM items of PDUs for which 
                  RLC entity is not yet configured in DL
03/27/08  vg      Added fix for report_vr_h handling
03/27/08  pj      Converted ERR_FATAL to MSG_ERROR in function rlc_dl_decode_pdu_hdr
03/24/08  pj      Added support for data path optimizations under 
                  FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
          vg      1. Fixed issues during DL integration testing
          vg      2. a. Added array bound check and corrected logging of DL AM PDU in 
                        rlc_enh_logging_data_handler()
                     b. Added Error Fatal to rlc_discard_dsm_item() for integration phase
                     c. Call RLCI_GET_AM_LOGGING_CONFIG() with correct RLC id
          vg      3. Updated rlc_enh_calc_rlc_hdr_length() to return whether a given lc_id
                     is tm_mode or not
          ar      4. a. Added global variable rlci_log_mask for storing AM and UM log info
                     b. Added support for UM logging under feature FEATURE_RLC_QXDM_LOGGING
                     c. Moved MSG_HIGHs under feature FEATURE_WCDMA_RLC_PROFILING
                     d. Added function call rlci_enh_dl_log_info_handler() in function
                        rlc_enh_dl_pdu_data_handler()
          vg      5. fixed 2 GCF failure issues.
                      a. added "missing pdu reporting" support
                      b. fixed AM/UM pdu validation check coding bug. 
          vg      6. fixed ciphering logging issue
          ssg     7. Added fix for pdu_size field of DL PDU log packet.
02/01/08  pj      Changed FEATURE_MBMS flag to FEATURE_MODEM_MBMS.
12/17/07  ssg     Added rlci_establish_dl_um(), rlci_modify_dl_um() function
                  calls.
12/10/07  vg      initialised config_conf result to SUCCESS when no bitmap is set 
                  under FEATURE_MULTIPLE_RLC_OPERATIONS              
12/06/07  vg      added support for multiple RRC operations handling at the
                  time of channel config req under feature flag 
                  FEATURE_MULTIPLE_RLC_OPERATIONS.
11/30/07  ssg     Added mbms support under FEATURE_MBMS.
10/08/07  vg      Added support for multiple PDUs in a TTI on CTCH logical channel 
                  under the feature FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT.
05/15/07  vg      removed MSG_HIGH
01/11/07  vg      removed a extra variable from UM logging code.
01/04/07  ssg     Moving downink cipher PDU logging from interrupt context to
                  task context.
                  Added rlc_dl_log_cipher_pkt(), rlc_dl_set_cipher_logging()
                  function definitions.
12/06/06  ttl     Return config rx win size in rlci_get_rx_win_size().
11/23/06  vg      added UM DL/UL logging support under feature FEATURE_UM_PDU_LOGGING
11/21/06  vg      added support for two pending cipher config for AM SRBs/RBs
11/10/06  vg      added code for WM clenup handling for PDCP.
10/26/06  ttl     Increase internal rx q threshold.
10/06/06  ssg     Added function definition for rlci_dl_change_state().
                  Fixed lint error.
09/22/06  ssg     Added support for more data pdu logging on user plane
                  downlink logical channels.
09/22/06  ssg     Modified rlci_get_dl_hfn() to return HFN=0 for LC' which
                  have pending ciphering info.
09/11/06  ttl     Add support to overwrite the HFN being using.
09/06/06  ttl     Correct print out messages.
08/23/06  ssg     Added handling for one side re-estbalishemnt under the
                  feature FEATURE_RLC_ONE_SIDE_RE_ESTABLISHMENT.
06/28/06  ttl     Add HFN query support.
06/02/06  ttl     Check the ciphering activation time of the SRB2 before adding
                  another pending config record.
04/28/06  ttl     Moved Release 5 specified code under FEATURE_REL5.
04/12/06  ttl     Directly write PDU logging data into logging buffer w/o
                  calling the dsm_extract().
03/14/06  ttl     Lint fixes.
03/03/06  ttl     Perform status report only on AM LC as request by MAC when
                  it is in HS RESET.
02/07/06  ttl     Changes Internal Q size from 512 to 768 for HSDPA.
12/09/05  pdv     Code Optimization.
12/02/05  pdv     Handling corner cases of CR79364
11/11/05  pdv     Fix for CR79364
11/08/05  aw      Replaced the FEATURE_HSDPA with the check for release 5
                  indicator from L2.
11/07/05  aw      Added a new function rlci_get_dl_count_c to get count_c
                  values for all UM and AM channels under feature
                  FEATURE_INACTIVITY_HANDLING.
06/13/05  mv      Made changes in the establish and modify procedure for UM
                  mode to handle the LI size directly under FEATURE_HSDPA.
05/31/05  mv      Replaced AM_DATA_START_LC_ID with
                  RLC_DL_AM_USR_PLANE_START_LC_ID.
05/03/05  mv      Added the support for aborting the new ciphering config
                  and reverting it to the old one when requested by RRC.
04/05/05  aw      Removed Log_on_demand feature.
03/09/05  mv      Added the support for BMC under the feature
                  FEATURE_UMTS_BMC.
03/08/05  mv      Fixed a lint error.
03/08/05  mv      Added the storing of li size passed directly by RRC for
                  UM mode in the control block for establish and modify
                  procedures under FEATURE_HSDPA.
03/07/05  mv/aw   Merged the HSDPA changes to mainline
02/09/05  aw      Removed get16 function and added as a macro in rlcdl.h.
12/20/04  mv      Fixed the bug where the ciphering config was being
                  over-written by the new one even though the act_sn for the
                  new one was not reached.
09/09/04  mv      Added selective Ciphering logging for user plane data
                  channels.
09/07/04  mv      Added a check so that the internal RLC buffer does not
                  hold more than UE_MAX_DL_RLC_PDUS at a given time
                  for a particular logical channel.
08/19/04  mv      Separated the RLC PDU logging for the user plane and
                  signaling plane logical channels.
08/20/04  mv      Changed some log packet names.
06/24/04  mv      Changed some messages to print the name of arguments along
                  with their values.
06/02/04  mv      Fixed lint errors and warnings.
04/16/04  mv      Fixed the bug of not freeing the data in the UM downlink
                  control block when the UM channel in released.
04/05/04  mv      Changed an ERR message to MSG HIGH.
02/09/03  ttl     Supported suspend/resume AM timer for Cell_PCH.
01/20/03  mv      Replaced the FEATURE_RLC_ENHANCED_CB with the
                  FEATURE_RLC_SDU_RCVD_ENHANCED_CB.
12/29/03  vsk     Replace FEATURE_MULTIPLE_PDP with FEATURE_RLC_ENHANCED_CB
12/06/03  ttl     Added PDU ciphering parameters logging.
11/14/03  ttl     Added ciphering debugging message for UM.
10/28/03  ttl     Added cipher algorithm indication operation..
                  Added re-establishment for UM.
10/14/03  mv      If the window size configured by peer side is greater
                  than MAX_RX_WINDOW_SIZE, send a WINDOW SUFI with window
                  size set to MAX_RX_WINDOW_SIZE after a reset procedure.
10/07/03  ttl     Modified ciphering code to handle two copies of new ciphering
                  config, one copy of current using and two copies of old
                  ciphering config.
09/22/03  mv      Added a third parameter to the rlc_post_rx_func_ptr()
                  to support multiple PDP contexts under the feature
                  FEATURE_MULTIPLE_PDP.
09/04/03  ttl     Delay setup AM timer until the AM entity start to tx PDUs.
08/20/03  ttl     Merged branch code into tip.
08/11/03  ttl     Fixed the problem of two pending ciphering config for RB2.
08/01/03  mv      Added support for RLC re-establish procedure for AM.
07/23/03  ttl     Fixed a bug of not using correct cipher key id.
06/18/03  ttl     Set cipher flag to be FALSE when the channel does not exist.
                  Removed unreferenced variables.
05/16/03  ttl     Replace divide w/ bit shifting.
                  Added PDU trashing test under FEATURE_RLC_TESTING.
03/18/03  ttl     Reset new_cipher_on when the UM channel is config'd.
03/09/03  ttl     Fixed a bug on tracing the end of TTI.
                  Changed WM unregistered message level.
                  Added checking on the obtianing of ctrl_blk pointer when
                  call rlci_dl_get_ctl_blk_ptr().
02/14/03  ttl     Fixed Ciphering reconfig problem when there is no data
                  Tx or Rx in the channel.
01/14/03  ttl     Increasing the internal receiving queue size to 72.
12/10/02  ttl     Fixed a bug in log_on_demand.
                  Added rlc_dl_am_backup_timer() for backup timer.
12/09/02  ttl     Code clean up.
11/20/02  ttl     Gave the item limitation to the internal receiving queue.
11/13/02  ttl     Removed dupliacted tot_num_pdu_byte_rxd for AM statistics logging.
                  Added a RESET complete msg.
11/05/02  ttl     Added logging for ciphering config.
                  Fixed the bug of referencing un-initialized value in ciphering
                  config.
                  Fixed the bug of not accepting of re-config of ciphering
                  parameters when in CIPHER_MOD mode.
10/30/02  ttl     Added Cipher restart feature.
10/25/02  ttl     Added FEATURE_DATA_MM to exclude the get16 if not defined.
10/06/02  ttl     Added an internal PDUs receving queue rlci_rcvd_q to replace
                  the structure pointer dl_data_ptr shared by MAC and RLC.
                  Adjust frequent cases for user plant data calls.
09/17/02  ttl     Assign the dl_data_ptr when RLC wnats to process incoming PDUs
                  in L2 DL task context.
09/16/02  ttl     Reset cipher_mode and cipher_all in UM cipher config.
                  Added DSM tracer.
                  Fixed the bug of not distiguishing the AM data PDUs and control
                  PDUs in rlc_dl_decode_pdu_hdr().
09/05/02  ttl     Added Ciphering feature.
09/04/02  ttl     Fixed the bug of accessing dl_data_ptr when it is NULL.
08/21/02  ttl     Fixed the bug of overwritten the memory when PDU logging
                  is turned on.
08/16/02  ttl     Fixed the compiling errors when FEATURE_RLC_QXDM_LOGGING is off.
08/01/02  ttl     Added rlci_get_dl_sdu_byte_cnt() to give number of byte count of
                  received SDUs.
07/31/02  ttl     Removed rlc_id/Time sig mapping table.
                  Rewrote 10ms base timer, removed timer sig, using timer
                  callback function instead.
                  Removed rlci_ul_am_get_timer_sig(), rlci_ul_am_timer_sig_to_rlc_id(),
                  rlci_ul_am_release_timer_sig().
06/12/02  ttl     Added DL AM PDU logging.
05/17/02  ttl     Added code to handle STOP/CONTINUE in Config_Req from RRC for
                  AM and UM..
                  Changed some messages contents.
05/14/02  ttl     Naming changed for wcdma_l2_dl_tcb.
05/13/02  ttl     Corrected lc_state.
                  Added rlci_get_rx_win_size() to return rx window size.
05/08/02  ttl     Give the number of SDUs enqueued into WM when rlc_post_rx_func_ptr()
                  is called.
04/24/02  ttl     Rewrote the statistic log code.
04/19/02  ttl     Reset dl_wm_ptr and rlc_post_rx_func_ptr for TM and UM at
                  the call establishment..
04/10/02  ttl     Fixed the bug of setting timer sigs for Downlink timer.
04/03/02  ttl     Added Log_on_demand feature.
                  Changed UE_MAX_UL_LOGICAL_CHANNEL to UE_MAX_DL_LOGICAL_CHANNEL.
03/15/02  sk      Added support for qxdm logging of AM statistics. Defined and set
                  timer in rlc_dl_init(). Added function send_dl_am_stat_packet()
                  to send the Am statistics to qxdm for logging.
02/13/02  ttl     Added service de-registration function.
                  Changed loop counters to uint16.
01/15/02  ttl     Changed dl_data_ptr's message level to MSG_MED.
01/07/02  ttl     reset global variable dl_data_ptr.
11/09/01  sk      Added functions : rlci_dl_am_get_timer_sig,
                  rlci_dl_am_release_timer_sig(), rlci_dl_get_ctl_blk_ptr(),
                  rlci_dl_am_timer_sig_to_rlc_id(). Added code for initialising
                  timer_blk in rlc_dl_init().Added 10ms signalling control block.
11/04/01  ttl     Remove application/voice service callback function feature.
10/26/01  ttl     Added application/voice service callback function feature.
                  Moved TM PDUs processing back to L2 context.
                  Revised messages contents.
10/12/01  ttl     Moved TM PDUs processing into interrupt context, but leaving
                  the Upper layer informing in L2 context.
09/06/01  rc      Fixed comment and added check for logical channel id
                  exceeding UE_MAX_DL_LOGICAL_CHANNEL.
09/06/01  sk      Code added to free the dsm packets in error states.
08/30/01  rc      Added Watermark Check.Cleaned up code.Renamed tm_log_ptr,
                  um_log_ptr, am_log_ptr and log_state as dl_tm_log_ptr,
                  dl_um_log_ptr,dl_am_log_ptr and dl_log_state respectively.
08/28/01  rc      Renamed hfn_cnf as dl_hfn_cnf.
08/27/01  rc      Made structure definitions global to the file.
08/20/01  rc      Added support for QXDM logging.
08/10/01  rc      Changed some messages to fix the formatting problem in
                  F3 messages.
06/28/01  rc      Changed MSG levels. Added check for logical channel id
                  being less than UE_MAX_DL_LOGICAL_CHANNEL.
06/18/01  rc      Added code to support Downlink Processing in Transparent
                  Mode in both Task and  Interrupt Context.
06/08/01  rc      Removed Release all Confirmation to Uplink.Confirmation
                  of Release All is sent to RRC.
06/07/01  rc      Added support for Releasing all active channels on
                  the Uplink and sending a confirmation back to Uplink
                  after releasing all channels.
06/07/01  rc      Removed Temporary fix for VST problem.
06/06/01  rc      Changed Messages in TM and UM Config to display both
                  Channel ID and Table Index for each logical channel.
                  Added a temporary fix for a VST problem.
05/29/01  ttl     Changed naming convention from nchan to nentity for
                  am_config_req.
                  Fixed RESET state transitions.
05/21/01  rc      Cleaned up code and added more comments.
05/02/01  ttl     Moved act_incl and act_time out of the chan_info structure
                  to action structure for TM/UM/AM.
                  Added rlci_dl_proc_reset_msg().
04/10/01  rc      Fixed warnings found while compiling for ARM.
04/09/01  rc      Fixed bug where rlc did not assign a value to the number
                  of channels in crlc_dl_*m_config_cnf.
04/03/01  rc      Changed commands from RRC_CRLC_CONFIG_*M_DL_CNF to
                  RRC_CRLC_DL_*M_CONFIG_CNF. Fixed bug in rlc_dl_decode_pdu_hdr()
                  where "=" was being used instead of "==" in the if statement.
                  Changed messages in crlc_dl_*m_config_handler() to display
                  channel id instead of table index.
03/26/01  ttl     Fixed bugs in rlc_dl_pdu_data_handler() due to the confusing
                  of name convention of index and i.
                  Added rlc_register_am_sdu_discard_cb_req().
03/15/01  rc      Fixed bug in rlc_dl_pdu_data_handler() where the data was
                  being accessed from the wrong index.
03/01/01  rc      Removed rlcrrctask.h.
02/28/01  ttl/rc  Added code to send confirms back to RRC after a config.
                  Introduced the mapping table for control block id and
                  channel id to support Acknowledged mode.
01/31/01  rc      Changed name of function rlc_reassemble_dl_tm_pdus() and
                  rlci_reassemble_dl_um_pdus() to
                  rlci_dl_reassemble_tm_pdus() and
                  rlci_dl_reassemble_um_pdus() respectively.
01/08/01  rc      Cleaned up code and fixed some bugs. Modified code to
                  use the data types defined in uecomdef.h
11/22/00  rc      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
**                          Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "task.h"
#include "l2dltask.h"
#include "l2ultask.h"
#include "rlcdl.h"
#include "rlci.h"
#include "rlcdltm.h"
#include "rlcdlum.h"
#include "rlcdlam.h"
#include "bit.h"
#include "rrccmd_v.h"
#include "rlcul.h"
#include "l1macdata.h"

#ifdef FEATURE_RLC_QXDM_LOGGING
 #include "rlcdllog.h"
#endif

#ifdef FEATURE_RLC_QXDM_LOGGING
#include "event.h"
#include "event_defs.h"
#endif

#include "rlcfc.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif
/* -----------------------------------------------------------------------
** Variable Definitions
** ----------------------------------------------------------------------- */
const char rlc_entry[20]="RLC DL ENTRY", rlc_exit[20]= "RLC DL EXIT", rlc_dup[20]= "RLC DL DUP";

uint8 wm_trace_timer[UE_MAX_DL_LOGICAL_CHANNEL] = {0};

#ifdef FEATURE_RLC_QXDM_LOGGING
rex_timer_type dl_tmr_log_blk;    /* Rex timer used for qxdm loggin  */
#endif

rlci_downlink_table_type downlink_table;
q_type rlci_rcvd_q;

  #define MAX_PDUS_IN_Q 1550

uint8 dl_ctl_blk_id_tbl[UE_MAX_DL_LOGICAL_CHANNEL];

rex_timer_type        rlc_dl_am_base_tmr;
                              /* This is base 10ms timer for RLC AM channels */
                              /* at expiry, periodic status/ control related */
                              /* procedures are triggered*/
boolean               rlc_dl_am_base_tmr_start = FALSE;

rlc_dl_hfn_cnf_type dl_hfn_cnf;        /* HFN confirmation               */

#ifdef FEATURE_WCDMA_DL_DATAPATH_LOG_PKT
  uint16 rlc_global_sub_fn;       /* holds SUB_FN value in received TSN*/
#endif 

#ifdef FEATURE_RLC_QXDM_LOGGING
  rlc_dl_tm_config_type dl_tm_log_ptr;
  rlc_dl_um_config_type dl_um_log_ptr;
  rlc_am_config_req_type dl_am_log_ptr;
  rlc_dl_log_state_type dl_log_state;

  boolean rlci_dl_pdu_cipher_log_on = FALSE;
  rlc_dl_pdu_cipher_info_type rlci_dl_pdu_cipher_log;

#endif

/* Variable to store log status information  */
/*  RLCI_UM_USR_PLANE_LOG_MASK : 0x01
 *  RLCI_AM_USR_PLANE_LOG_MASK : 0x02
 *  RLCI_AM_SIG_PLANE_LOG_MASK : 0x04 */
  uint32  rlci_log_mask = 0x0;

/* Q of TSN info structures */
/* MAC delivers the tsn structure to RLC in interrupt context 
 * This TSN will be processed in RLC DL task context and 
 * releases (puts) them back in Free Q (call provided by MAC-HS)*/
  q_type rlc_tsn_info_q;
  /*--------------------------------------------------------------------------
 *  Mutex to protect the header read ISR and 
 *  MAC DL recfg flushing the partial/complete PDUs 
 ---------------------------------------------------------------------------*/
 rex_crit_sect_type rlc_tsn_mutex;

   /*--------------------------------------------------------------------------
 *  Mutex to protect downlink table when
 *  l2 dl task is released and rrc task check SRB BO
 ---------------------------------------------------------------------------*/
 rex_crit_sect_type rlc_channel_release_mutex;

/* Q of RLC ENH DATA structures */
/* MAC will deliver Non-HS data to RLC using this data structure
 * MAC will enqueue them into rlc_enh_data_q and post a signal to RLC
 * to act in TASK context. After handling, the entry will be returned
 * to rlc_enh_free_data_q */
rlc_enh_data_ind_type *rlc_enh_data = NULL;

q_type rlc_enh_data_q;
q_type rlc_enh_free_data_q;

#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  boolean ctch_first_pdu = TRUE;
#endif

#if ((defined FEATURE_DUAL_SIM_WCDMA_TUNEAWAY) || (defined FEATURE_WCDMA_HS_FACH_DRX))
/* Holds the AM SDU Chain during reassembly */
extern rlci_dl_temp_pdu_info_type dl_am_pdu_info;
extern rlci_dl_temp_pdu_info_type dl_um_pdu_info;
#endif
extern rex_crit_sect_type rlc_dl_mutex;
extern rex_crit_sect_type rlc_dl_data_process_mutex;

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
extern rex_crit_sect_type rlc_fc_sem;
#endif

rex_crit_sect_type rlc_dl_tm_mutex;
rex_crit_sect_type rlc_mac_dl_pdu_mutex;
boolean dl_pdu_decode_result = TRUE;

uint16  time_since_last_dl_pdu_rcvd;
dl_priority_type dl_priority_stats[UE_MAX_AM_ENTITY];
extern uint8 get_lc_priority ( rlc_lc_id_type rlc_id );

/* =======================================================================
**                            Functions
** ======================================================================= */
extern void mac_hs_dl_discard_tsn(l1_dec_hs_tsn_info_type *int_tsn_ptr);

rlci_dl_ctrl_blk_type *rlci_get_dl_ctrl_blk_ptr(uint8 ctrl_index)
{
  return (&(downlink_table.ctrl_blk[ctrl_index]));
}

/*===========================================================================
FUNCTION update_dl_priority_stats

DESCRIPTION  Checks if any Logical Channel is currently monitoring to see
             if higher priority traffic is being received in DL. If yes, then
             we reset 'time_since_higher_priority_rx' for that LC.
             
      NOTE   LC Priority is highest at 0, and lowest at 7
===========================================================================*/

void update_dl_priority_stats ( uint8 rlc_id )
{
  uint8 lc_priority, am_chan_idx; 
  dl_priority_type *dl_stats_table_ptr; 
  
  /* Get the priority for the LC on which data is currently received */
  lc_priority = get_lc_priority(rlc_id);
  
  if( lc_priority != 0xFF) 
  {
    for ( am_chan_idx = 0 ; am_chan_idx < UE_MAX_AM_ENTITY ; am_chan_idx++)
    {
      dl_stats_table_ptr = &dl_priority_stats[am_chan_idx]; 
      
      if ( (dl_stats_table_ptr->check_priority) &&
           (dl_stats_table_ptr->lc_priority > lc_priority) )
      {
        /* MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RST_OPT: Higher Priority %d for LC %d, time_since_higher_priority_rx %d",
                                                     lc_priority,
                                                     rlc_id, dl_stats_table_ptr->time_since_higher_priority_rx ); */
        
        dl_stats_table_ptr->time_since_higher_priority_rx = 0;
      }
    }
  }
}


/*===========================================================================
FUNCTION: RLC_DL_ALLOCATE_MEM_ZI_OPT

DESCRIPTION:
  This function dynamically allocates memory to RLC-DL data structures
===========================================================================*/
void rlc_dl_allocate_mem_ZI_opt(void)
{
  if(rlc_enh_data == NULL)
  {
    rlc_enh_data = (rlc_enh_data_ind_type *)modem_mem_alloc(RLC_MAX_TTI_DATA_DL * sizeof(rlc_enh_data_ind_type), 
                                                      MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    if(rlc_enh_data != NULL)
    {
      MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Allocated memory for rlc_enh_data");
    }
    else
    {
      ERR_FATAL("RLC_ERR: Cannot allocate memory for rlc_enh_data",0,0,0); 
    }
  }
  else
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "rlc_enh_data dynamic allocation already done");
  }
}

/*===========================================================================
FUNCTION: RLC_DL_DEALLOCATE_MEM_ZI_OPT

DESCRIPTION:
  This function frees RLC-DL dynamic allocated data structures
===========================================================================*/
void rlc_dl_deallocate_mem_ZI_opt(void)
{
  if(rlc_enh_data != NULL)
  {
    modem_mem_free(rlc_enh_data, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Freeing memory for rlc_enh_data");
    rlc_enh_data = NULL;     
  }
}
/*===========================================================================

FUNCTION DL_ASSIGN_CTL_BLK

DESCRIPTION
  Assign Downlink control block for an RLC entity.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 dl_assign_ctl_blk(void)
{
  uint8 ret_val = INIT_CTL_BLK_ID;
  int i;

  for (i=0; i<UE_MAX_DL_RLC_ENTITY; i++)
  {
    if (downlink_table.ctrl_blk[i].lc_state == DL_NULL_STATE)
    {
      ret_val = (uint8)i;
      break;
    }
  }
  if (i == UE_MAX_DL_RLC_ENTITY)
  {
    ERR("Too many RLC entities", 0, 0, 0);
  }
  return ret_val;

} /* dl_assign_ctl_blk() */


/*===========================================================================

FUNCTION RLC_DL_INIT

DESCRIPTION
  Initializes the RLC Downlink Logical channel table.

DEPENDENCIES
  This function should be called at task start up.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_init(void)
{
  int i;


  MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "In RLC Downlink Initialization");

  rlc_dl_allocate_mem_ZI_opt();
  
  /*-------------------------------------------------------------------------
    Initialize the downlink table
  -------------------------------------------------------------------------*/
  (void)memset( &downlink_table, 0, sizeof( downlink_table));
  /*-------------------------------------------------------------------------
    Initialize the mapping table for control block id and channel id.
  -------------------------------------------------------------------------*/
  for (i=0; i<UE_MAX_DL_LOGICAL_CHANNEL; i++)
  {
    dl_ctl_blk_id_tbl[i] = INIT_CTL_BLK_ID;
  }

  /*-------------------------------------------------------------------------
    Init internal receiving queue.
  -------------------------------------------------------------------------*/
  (void)q_init(&rlci_rcvd_q);

  (void)q_init(&rlc_tsn_info_q);
  (void)q_init(&rlc_enh_data_q);
  (void) q_init(&rlc_enh_free_data_q);
  /*Initialize the mutex*/
  rex_init_crit_sect(&rlc_tsn_mutex);
  for (i = 0; i < RLC_MAX_TTI_DATA_DL; i++)
  {
    (void) q_link(&rlc_enh_data[i], &rlc_enh_data[i].link);
    q_put(&rlc_enh_free_data_q, &rlc_enh_data[i].link);
  }

  /*-------------------------------------------------------------------------
    Initialize DL mutex.
  -------------------------------------------------------------------------*/
  rex_init_crit_sect(&rlc_dl_mutex);
  rex_init_crit_sect(&rlc_dl_data_process_mutex);

  rex_init_crit_sect(&rlc_dl_tm_mutex);
  rex_init_crit_sect(&rlc_mac_dl_pdu_mutex);
  rex_init_crit_sect(&rlc_channel_release_mutex);

#ifndef FEATURE_WCDMA_RLC_ZI_REDUCTION_PHASE3  
  /*-------------------------------------------------------------------------
    Perform the AM initialization.
  -------------------------------------------------------------------------*/
  rlci_dl_am_init();
#else
#ifdef FEATURE_WCDMA_RLC_TIMER_OPT

  /*-------------------------------------------------------------------------
    Initialize base RLC AM timer, the timer will be started when first 
    AM PDU (on any channel) is ready to be Txd
  -------------------------------------------------------------------------*/
//  rex_def_timer(&rlc_dl_am_base_tmr,&wcdma_l2_dl_tcb, RLC_DL_AM_BASE_TIMER_SIG);
  rex_def_timer_ex(&rlc_dl_am_base_tmr, rlci_am_base_timer_cb, RLC_DL_AM_BASE_TIMER_SIG);
  rlc_dl_am_base_tmr_start = FALSE;
#endif
#endif
  
  /* Initialize Globals used for Reset Optimization */
  time_since_last_dl_pdu_rcvd = DL_PDU_TIMER_INACTIVE;
  memset(dl_priority_stats, 0, (sizeof(dl_priority_type)*UE_MAX_AM_ENTITY) );
  
#ifdef FEATURE_RLC_QXDM_LOGGING
  /*---------------------------------------------------------------------
  Define the log timer here and Set the log timer
  ----------------------------------------------------------------------*/
  rex_def_timer(&dl_tmr_log_blk, wcdma_l2_dl_get_tcb(), RLC_DL_AM_LOG_TIMER_EXP_SIG);

  (void) rex_set_timer(&dl_tmr_log_blk,(uint32)RLC_AM_LOG_TIMER_BASE);

  memset(&rlci_dl_pdu_cipher_log, 0, sizeof(rlci_dl_pdu_cipher_log));

#ifdef FEATURE_RLC_ERROR_LOG_SUBMIT
  rlc_debug_var |= RLC_DBG_ERR_LOG_SUBMIT;
#endif

#endif

} /* rlc_dl_init() */


/*===========================================================================

FUNCTION CRLC_DL_CIPHER_CONFIG_HANDLER

DESCRIPTION
  Configures ciphering on one or more Unacknowledged or Acknowledged
  Mode Channels.This function configures the ciphering parameters that are
  recevied from RRC. The ciphering parameters are updated only if the the
  channel is in DL_DATA_TRANSFER_READY STATE.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_cipher_config_handler
(
  rlc_dl_cipher_config_type *cipher_ptr   /* Ptr to Downlink Cipher Config */
)
{
  rlci_dl_ctrl_blk_type *ctl_blk = NULL;
  uint16 index;
  int i;

  MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC_DBG: recevied DL cipher config for nchan %d ",
                                             cipher_ptr->nchan);

  /*-------------------------------------------------------------------------
    For each channel indicated in the Ciphering Config Request
  -------------------------------------------------------------------------*/
  for (i = 0; i < cipher_ptr->nchan; i++)
  {
    /*-----------------------------------------------------------------------
      Get the control block id
    -----------------------------------------------------------------------*/
    index = dl_ctl_blk_id_tbl[cipher_ptr->ciphering[i].lc_id];
    ctl_blk = &downlink_table.ctrl_blk[index];
    /*-----------------------------------------------------------------------
      If RLC downlink is in Data Transfer Ready state, copy details
     to the downlink table entry.
    -----------------------------------------------------------------------*/
    /*lint -e655 */
    if (index != INIT_CTL_BLK_ID && ctl_blk->lc_state & DL_DATA_TRANSFER_READY)
    /*lint +e655 */
    {
      /*---------------------------------------------------------------------
        Copy the ciphering configuration details to the downlink_table entry.
      ---------------------------------------------------------------------*/
      if (cipher_ptr->ciphering[i].cipher_mode)
      {
        MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "DL cipher config, LC %d, hfn %x, act_seq %x",
                                                   cipher_ptr->ciphering[i].lc_id,
                                                   cipher_ptr->ciphering[i].hfn,
                                      cipher_ptr->ciphering[i].ciph_act_sn);

        /*-------------------------------------------------------------------
          Backup the old ciphering configuration as RRC might abort the
          new configuration if a cell update is triggered.
        -------------------------------------------------------------------*/
        ctl_blk->cipher_backup = ctl_blk->cipher;

        /*---------------------------------------------------------------------
          UNACKNOWLEDGED Mode cipher config.
        ---------------------------------------------------------------------*/
        if (ctl_blk->lc_mode == UE_MODE_UNACKNOWLEDGED)
        {
          switch (ctl_blk->cipher.cipher_mode)
          {
            /*---------------------------------------------------------------
             There is no PDU retx in UM. We don't need to remember the
             current cipher_key and hfn when the cipher_mode moves from
             CIPHER_MOD to CIPHER_ON.
            ---------------------------------------------------------------*/
            case NO_CIPHER:
            case CIPHER_ON:
              /*-------------------------------------------------------------
                If the channel is directly config to CIPHER_ON mode before,
                but the vr_h has not reached the act_sn, store the old
                and the new ciphering configurations.
              -------------------------------------------------------------*/
              if (RLCI_SEQ_GT(cipher_ptr->ciphering[i].ciph_act_sn,
                               ctl_blk->ctrl.um_ctrl.vr_ur, RLC_UM_MODULUS))
              {
                ctl_blk->cipher.cipher_mode =
                  (ctl_blk->cipher.cipher_mode == NO_CIPHER) ?
                  CIPHER_WAIT : CIPHER_MOD;

                ctl_blk->cipher.cipher_key_id_new = cipher_ptr->ciphering[i].cipher_key_id;
                ctl_blk->cipher.cipher_algo_new = cipher_ptr->ciphering[i].ciph_algo;
                ctl_blk->cipher.hfn_new = cipher_ptr->ciphering[i].hfn;
#ifdef FEATURE_UM_FUTURE_CIPHERING_ACTIVATION
                ctl_blk->cipher.wrap_valid_new = FALSE;
#endif
              }
              else
              {
#ifdef FEATURE_UM_FUTURE_CIPHERING_ACTIVATION
                if (cipher_ptr->ciphering[i].ciph_act_sn != ctl_blk->ctrl.um_ctrl.vr_ur)
                {

                  ctl_blk->cipher.cipher_mode =
                    (ctl_blk->cipher.cipher_mode == NO_CIPHER) ?
                    CIPHER_WAIT : CIPHER_MOD;

                  ctl_blk->cipher.cipher_key_id_new = cipher_ptr->ciphering[i].cipher_key_id;
                  ctl_blk->cipher.cipher_algo_new = cipher_ptr->ciphering[i].ciph_algo;
                  ctl_blk->cipher.hfn_new = cipher_ptr->ciphering[i].hfn;
                  ctl_blk->cipher.wrap_valid_new = TRUE;
                  MSG_HIGH(
                    "Future ciphering - DL Cipher config, LC %d no_cipher/cipher_on act_sn_old %d vr_ur %d",
                    ctl_blk->ctrl.um_ctrl.rlc_id, ctl_blk->cipher.act_sn, 
                    ctl_blk->ctrl.um_ctrl.vr_ur);
                }
                else
#endif
                {  
                  ctl_blk->cipher.cipher_mode = CIPHER_ON;
                  //ctl_blk->cipher.new_cipher_on = TRUE;
                  ctl_blk->cipher.cipher_key_id = cipher_ptr->ciphering[i].cipher_key_id;
                  ctl_blk->cipher.cipher_algo = cipher_ptr->ciphering[i].ciph_algo;
                  ctl_blk->cipher.hfn = cipher_ptr->ciphering[i].hfn;
                  ctl_blk->cipher.count = (cipher_ptr->ciphering[i].hfn << 7) |
                                        ctl_blk->ctrl.um_ctrl.vr_ur;

                  MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Cipher ON DL UM, LC %d, Key_id %d, count %x",
                          cipher_ptr->ciphering[i].lc_id,
                                                             ctl_blk->cipher.cipher_key_id,
                                                             ctl_blk->cipher.count);
                }
              }
              ctl_blk->cipher.act_sn_old = ctl_blk->cipher.act_sn;
              ctl_blk->cipher.act_sn= cipher_ptr->ciphering[i].ciph_act_sn;
              break;

            case CIPHER_WAIT:
            case CIPHER_MOD:
              ctl_blk->cipher.cipher_key_id_new = cipher_ptr->ciphering[i].cipher_key_id;
              ctl_blk->cipher.cipher_algo_new = cipher_ptr->ciphering[i].ciph_algo;
              ctl_blk->cipher.hfn_new = cipher_ptr->ciphering[i].hfn;
              break;

            default:
              MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "Invalid Ciphering State");
          }
        }
        /*---------------------------------------------------------------------
          ACKNOWLEDGED Mode cipher config.
        ---------------------------------------------------------------------*/
        else if (ctl_blk->lc_mode == UE_MODE_ACKNOWLEDGED)
        {
          switch (ctl_blk->cipher.cipher_mode)
          {
            case NO_CIPHER:
            case CIPHER_ON:

              /*-----------------------------------------------------------
                If the new activation time has not been reached, store the
                new ciphering parameters.
              -------------------------------------------------------------*/
              if (RLCI_SEQ_GT(cipher_ptr->ciphering[i].ciph_act_sn,
                    ctl_blk->ctrl.am_ctrl.dl_state_var.vr_h, RLC_AM_MODULUS))
              {
                ctl_blk->cipher.cipher_mode =
                  (ctl_blk->cipher.cipher_mode == NO_CIPHER) ?
                  CIPHER_WAIT : CIPHER_MOD;

                ctl_blk->cipher.cipher_key_id_new = cipher_ptr->ciphering[i].cipher_key_id;
                ctl_blk->cipher.cipher_algo_new = cipher_ptr->ciphering[i].ciph_algo;
                ctl_blk->cipher.hfn_new = cipher_ptr->ciphering[i].hfn;
                ctl_blk->cipher.act_sn_new = cipher_ptr->ciphering[i].ciph_act_sn;
              }
              /*-------------------------------------------------------------
                Reached the new activation time.
              -------------------------------------------------------------*/
              else
              {

                /*-------------------------------------------------------------
                  Cipher is on when received this new cipher config.
                -------------------------------------------------------------*/
                if (ctl_blk->cipher.cipher_mode == CIPHER_ON)
                {
                  /*-------------------------------------------------------------
                    Old config is active.
                  -------------------------------------------------------------*/
                  if (ctl_blk->cipher.act_old_cfg)
                  {
                    ctl_blk->cipher.act_old_cfg1 = TRUE;
                    ctl_blk->cipher.cipher_key_id_old1 = ctl_blk->cipher.cipher_key_id_old;
                    ctl_blk->cipher.cipher_algo_old1 = ctl_blk->cipher.cipher_algo_old;
                    ctl_blk->cipher.act_sn_old1 = ctl_blk->cipher.act_sn_old;
                    ctl_blk->cipher.count_old1 = ctl_blk->cipher.count_old;
                  }
                  /*-------------------------------------------------------------
                    No old config is active.
                  -------------------------------------------------------------*/
                  else
                  {
                    ctl_blk->cipher.act_old_cfg = TRUE;
                  }
                  /*-------------------------------------------------------------
                    Back up the current cipher config into old cipher config
                    for retx using.
                  -------------------------------------------------------------*/
                  ctl_blk->cipher.cipher_key_id_old = ctl_blk->cipher.cipher_key_id;
                  ctl_blk->cipher.cipher_algo_old = ctl_blk->cipher.cipher_algo;
                  ctl_blk->cipher.act_sn_old = ctl_blk->cipher.act_sn;
                  /*-----------------------------------------------------------------
                    Since the count_c always move 1 ahead for the next outgoing PDU,
                    We substract the current count_c by 1 to reflect the previous
                    outgoing PDU.
                  -----------------------------------------------------------------*/
                  ctl_blk->cipher.count_old = ctl_blk->cipher.count - 1;
                }
                /*-------------------------------------------------------------
                  No Cipher when received this new cipher config.
                -------------------------------------------------------------*/
                else
                {
                  ctl_blk->cipher.cipher_mode = CIPHER_ON;
                }

                //ctl_blk->cipher.new_cipher_on = TRUE;
                /*-------------------------------------------------------------
                  Update current active cipher config.
                -------------------------------------------------------------*/
                ctl_blk->cipher.cipher_key_id = cipher_ptr->ciphering[i].cipher_key_id;
                ctl_blk->cipher.cipher_algo = cipher_ptr->ciphering[i].ciph_algo;
                ctl_blk->cipher.hfn = cipher_ptr->ciphering[i].hfn;
                ctl_blk->cipher.count = (cipher_ptr->ciphering[i].hfn << 12) |
                                          ctl_blk->ctrl.am_ctrl.dl_state_var.vr_h;
                ctl_blk->cipher.act_sn = cipher_ptr->ciphering[i].ciph_act_sn;
                MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Cipher ON DL AM, LC %d, Key_id %d, count %x",
                          cipher_ptr->ciphering[i].lc_id,
                                                           ctl_blk->cipher.cipher_key_id,
                                                           ctl_blk->cipher.count);
              }

              break;

            case CIPHER_WAIT:
            case CIPHER_MOD:
              if (ctl_blk->cipher.act_sn_new != cipher_ptr->ciphering[i].ciph_act_sn)
              {
                ctl_blk->cipher.act_sn_new1 = cipher_ptr->ciphering[i].ciph_act_sn;
                ctl_blk->cipher.cipher_key_id_new1 = cipher_ptr->ciphering[i].cipher_key_id;
                ctl_blk->cipher.cipher_algo_new1 = cipher_ptr->ciphering[i].ciph_algo;
                ctl_blk->cipher.hfn_new1 = cipher_ptr->ciphering[i].hfn;
                ctl_blk->cipher.two_pending = TRUE;
              }
              else
              {
               /*lint -e64 -e641 -e655 */
                ctl_blk->cipher.cipher_key_id_new = cipher_ptr->ciphering[i].cipher_key_id;
                ctl_blk->cipher.cipher_algo_new = cipher_ptr->ciphering[i].ciph_algo;
                /*lint +e64 +e641 +e655 */
                ctl_blk->cipher.hfn_new = cipher_ptr->ciphering[i].hfn;
              }
              break;

            default:
              MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "Invalid Ciphering State");
          }
        }
        ctl_blk->cipher.rb_id = cipher_ptr->ciphering[i].rb_id;
      }
      else
      {
        ctl_blk->cipher.cipher_mode = NO_CIPHER;
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL Cipher config, LC %d cipher OFF",
                                                   cipher_ptr->ciphering[i].lc_id);
      }
    }
    else
    {
      ERR("DL Cipher config,LC %d not up", cipher_ptr->ciphering[i].lc_id,0,0);
    }
  }

#ifdef FEATURE_RLC_QXDM_LOGGING
  if (log_status(WCDMA_DL_RLC_V1_CIPH_CFG_LOG_PACKET))
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "LOG DL Cipher config");
    rlc_dl_tx_cipher_log_packet(cipher_ptr);
  }
  else
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "NO LOG DL Cipher config");
  }
#endif

} /* crlc_dl_cipher_config_handler() */
/*===========================================================================

FUNCTION CRLC_DL_HFN_CNF

DESCRIPTION
 This function sends the hyperframe number(s) of the requested channel(s)
 to RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_hfn_cnf
(
  rlc_dl_hfn_cnf_type *hfn_ptr             /* Ptr to HFN CNF info.         */
)
{
  rrc_cmd_type *rrc_rlc_cmd_ptr;

  rrc_rlc_cmd_ptr = rrc_get_rlc_cmd_buf();
  if (rrc_rlc_cmd_ptr != NULL)
  {
    rrc_rlc_cmd_ptr->cmd_hdr.cmd_id = RRC_CRLC_DL_HFN_CNF;
    rrc_rlc_cmd_ptr->cmd.crlc_dl_hfn_cnf = *hfn_ptr;
    rrc_put_rlc_cmd(rrc_rlc_cmd_ptr);
  }
  else
  {
    ERR_FATAL("Could not get command buf",0,0,0);
  }
} /* crlc_dl_hfn_cnf() */

/*===========================================================================

FUNCTION CRLC_DL_HFN_HANDLER

DESCRIPTION
 This function handles the request for the Hyperframe number by RRC.
 RRC can request the hyperframe number for all Unacknowledged or
 Acknowledged mode channels or for a specific channel.
 the Hyperframe number is incremented everytime the sequence number for
 a channel rolls over.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_hfn_handler
(
  rlc_dl_hfn_type *hfn_ptr                 /* Ptr to HFN info              */
)
{
  uint16 index;                             /* Downlink Table Index         */
  uint16 count;                             /* Number of Active Channels    */
  uint8 lc_id = 0;                         /* Logical Channel ID           */
  uint8 i;
  uint8 array_len = 0;
  /*------------------------------------------------------------------------
    Initialize the number of channels and index to 0.
  ------------------------------------------------------------------------*/
  dl_hfn_cnf.nchan = 0;
  index = 0;
  /*-------------------------------------------------------------------------
    If the HFN for all unacknowledged and acknowledged is requested,
    get the HFN for all the active unacknowledged and acknowledged mode
    channels from the downlink table and call the function crlc_dl_hfn_cnf()
    to send the information to RRC.
  -------------------------------------------------------------------------*/
  if (hfn_ptr->all_or_one == RLC_ALL)
  {
    count = downlink_table.active_ch;
    array_len = sizeof(dl_hfn_cnf.hfn_info)/sizeof(rlc_dl_hfn_info_type);

    while ((count > 0) && (dl_hfn_cnf.nchan < array_len))
    {
      if (downlink_table.ctrl_blk[index].lc_state != DL_NULL_STATE)
      {
        count--;
        if (downlink_table.ctrl_blk[index].lc_mode != UE_MODE_TRANSPARENT)
        {
          for (i=0; i<UE_MAX_DL_LOGICAL_CHANNEL; i++)
          {
            if (dl_ctl_blk_id_tbl[i] == index)
            {
              lc_id = i;
              break;
            }
          }
          dl_hfn_cnf.hfn_info[dl_hfn_cnf.nchan].lc_id = lc_id;
          dl_hfn_cnf.hfn_info[dl_hfn_cnf.nchan].hfn  =
                      downlink_table.ctrl_blk[index].cipher.hfn;
          dl_hfn_cnf.nchan++;
        }
      }
      index++;
    }
    crlc_dl_hfn_cnf(&dl_hfn_cnf);
  }
  else
  {
    /*-----------------------------------------------------------------------
      If the HFN for a specific logical channel is requested,get the HFN for
      the channel, if it is active, and call the function crlc_dl_hfn_cnf()
      to send the information to RRC.
    -----------------------------------------------------------------------*/
    index = dl_ctl_blk_id_tbl[hfn_ptr->lc_id];
    if (index != INIT_CTL_BLK_ID &&
        downlink_table.ctrl_blk[index].lc_state != DL_NULL_STATE)
    {
      dl_hfn_cnf.hfn_info[dl_hfn_cnf.nchan].lc_id = hfn_ptr->lc_id;
      dl_hfn_cnf.hfn_info[dl_hfn_cnf.nchan].hfn =
                            downlink_table.ctrl_blk[index].cipher.hfn;
      dl_hfn_cnf.nchan++;
      crlc_dl_hfn_cnf(&dl_hfn_cnf);
    }
    else
    {
      ERR("Logical Channel %d does not exist",index,0,0);
    }
  }
} /* crlc_dl_hfn_handler() */


/*===========================================================================

FUNCTION CRLC_DL_TM_CONFIG_HANDLER

DESCRIPTION
  This function handles the configuration of Transparent Mode Logical
  channels. This function is called when RRC sends a CRLC_DL_TM_CONFIG_REQ
  to RLC. If a new channel is to be established, the function checks if
  the channels exists. If it already exists, and ERR message is dispalyed.
  If the channel does not exist, the downlink table entry for that channel
  is updated and the state is set to DL_DATA_TRANSFER_READY. If a channel
  is to be released,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the channel is set to the NULL
  state and the watermark is set to NULL. If the parameters of a channel
  are to be modified ,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the downlink table entry for the
  channel is updated with the configuration received from RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_tm_config_handler
(
  rlc_dl_tm_config_type *tm_ptr /* Pointer to TM config info               */
)
{
  rrc_cmd_type *rrc_rlc_cmd_ptr; /* Command buffer to store confirm        */
                                 /* Information                            */
  uint8 index;
  int i;
  rlc_channel_action_cnf_e_type action;
#ifdef FEATURE_RLC_QXDM_LOGGING
  uint8 j = 0;
  uint8 k = 0;
#endif
  /*-------------------------------------------------------------------------
    Get a command buffer to store the confirmation information to send to
    RRC.
  -------------------------------------------------------------------------*/
  rrc_rlc_cmd_ptr = rrc_get_rlc_cmd_buf();
  if (rrc_rlc_cmd_ptr == NULL)
  {
    ERR_FATAL("Could not get command buf",0,0,0);
    /*lint -e527 */
    return;
    /*lint +e527 */
  }
  rrc_rlc_cmd_ptr->cmd_hdr.cmd_id = RRC_CRLC_DL_TM_CONFIG_CNF;
  rrc_rlc_cmd_ptr->cmd.crlc_dl_tm_config_cnf.nchan = tm_ptr->nchan;

  L2_ACQ_DL_LOCK();

  for (i = 0; i < tm_ptr->nchan; i++)
  {
    rrc_rlc_cmd_ptr->cmd.crlc_dl_tm_config_cnf.tm_config_result[i].lc_id =
                                                  tm_ptr->chan_info[i].lc_id;
    if (tm_ptr->chan_info[i].lc_id >= UE_MAX_DL_LOGICAL_CHANNEL)
    {
      ERR("Invalid DL TM LC Id %d",tm_ptr->chan_info[i].lc_id,0,0);
      rrc_rlc_cmd_ptr->cmd.crlc_dl_tm_config_cnf.
                                  tm_config_result[i].result = FAILURE;
    }
    else
    {
      rrc_rlc_cmd_ptr->cmd.crlc_ul_tm_config_cnf.
                                          tm_config_result[i].result = SUCCESS;
      if(tm_ptr->dl_tm_act[i].action == RLC_MODIFY)
      {
        while(tm_ptr->dl_tm_act[i].dl_cfg_mask != 0)
        {
          action = rlci_get_config_action(&(tm_ptr->dl_tm_act[i].dl_cfg_mask));
          switch(action)
          {
            case RLCI_MODIFY:
              /*-------------------------------------------------------------------
                Check if Channel is in NULL state. If it is not, modify
                the channel parameters and send a SUCCESS indication in the
                confirm message to RRC.
              -------------------------------------------------------------------*/
              if ((index = dl_ctl_blk_id_tbl[tm_ptr->chan_info[i].lc_id])
                                                          != INIT_CTL_BLK_ID)
              {
                MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "DL TM Modify LC %d, Index %d",
                                                           tm_ptr->chan_info[i].lc_id,
                                                           index);
                downlink_table.ctrl_blk[index].lc_type =
                                            tm_ptr->chan_info[i].lc_type;
                downlink_table.ctrl_blk[index].ctrl.tm_ctrl.seg_ind =
                                            tm_ptr->chan_info[i].seg_ind;
                downlink_table.ctrl_blk[index].ctrl.tm_ctrl.act_incl =
                                            tm_ptr->dl_tm_act[i].act_incl;
                downlink_table.ctrl_blk[index].ctrl.tm_ctrl.act_time =
                                            tm_ptr->dl_tm_act[i].act_time;
                rrc_rlc_cmd_ptr->cmd.crlc_ul_tm_config_cnf.
                                            tm_config_result[i].result = SUCCESS;
    #ifdef FEATURE_RLC_QXDM_LOGGING
                if (j < UE_MAX_DL_TM_CHANNEL)
                {
                  dl_tm_log_ptr.chan_info[j] = tm_ptr->chan_info[i];
                  j++;
                }
    #endif
              }
              else
              {
                /*-----------------------------------------------------------------
                  If the channel does not exist, send a FAILURE indication in the
                  confirm message to RRC.
                -----------------------------------------------------------------*/
                rrc_rlc_cmd_ptr->cmd.crlc_ul_tm_config_cnf.
                                            tm_config_result[i].result = FAILURE;
                ERR("DL LC %d not in right State - Cannot Mod",
                                            tm_ptr->chan_info[i].lc_id,0,0);
              }
              break;
            default:
              /*-------------------------------------------------------------------
                If the action is Invalid, send a FAILURE indication in the confirm
                to RRC.
              -------------------------------------------------------------------*/
              rrc_rlc_cmd_ptr->cmd.crlc_ul_tm_config_cnf.
                                   tm_config_result[i].result = FAILURE;
    
              ERR("Invalid Action %d for channel id %d",tm_ptr->dl_tm_act[i].action,tm_ptr->chan_info[i].lc_id,0);
  
          }
        }
      }
      else
      {
        switch(tm_ptr->dl_tm_act[i].action)
        {
          case RLC_ESTABLISH:
          /*---------------------------------------------------------------------
            Check if the channel exists. If it does not, get an index
            into the downlink table and store all the information for the
            channel. Send a SUCCESS indication in the confirm message to RRC
          ---------------------------------------------------------------------*/
            if ((dl_ctl_blk_id_tbl[tm_ptr->chan_info[i].lc_id] ==
                                                  INIT_CTL_BLK_ID) &&
                ((index = dl_assign_ctl_blk()) != INIT_CTL_BLK_ID))
            {
              MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL TM Establish LC %d, Index %d",
                                                         tm_ptr->chan_info[i].lc_id,
                                                         index);
              dl_ctl_blk_id_tbl[tm_ptr->chan_info[i].lc_id] = index;
              downlink_table.ctrl_blk[index].lc_mode = UE_MODE_TRANSPARENT;
              downlink_table.ctrl_blk[index].lc_type =
                                          tm_ptr->chan_info[i].lc_type;
              downlink_table.ctrl_blk[index].ctrl.tm_ctrl.seg_ind =
                                          tm_ptr->chan_info[i].seg_ind;
              downlink_table.ctrl_blk[index].ctrl.tm_ctrl.act_incl =
                                          tm_ptr->dl_tm_act[i].act_incl;
              downlink_table.ctrl_blk[index].ctrl.tm_ctrl.act_time =
                                          tm_ptr->dl_tm_act[i].act_time;
              downlink_table.ctrl_blk[index].lc_state = DL_DATA_TRANSFER_READY;
              downlink_table.ctrl_blk[index].dl_wm_ptr = NULL;
              downlink_table.ctrl_blk[index].rlc_post_rx_func_ptr = NULL;
              downlink_table.active_ch++;
              rrc_rlc_cmd_ptr->cmd.crlc_ul_tm_config_cnf.
                                          tm_config_result[i].result = SUCCESS;
  #ifdef FEATURE_RLC_QXDM_LOGGING
              dl_tm_log_ptr.chan_info[j] = tm_ptr->chan_info[i];
              j++;
              dl_log_state.dl_state[k].rlc_id = tm_ptr->chan_info[i].lc_id;
              dl_log_state.dl_state[k].lc_state = TM_DL_DATA_TRANSFER_READY;
              k++;
  #endif
            }
            else
            {
              /*-----------------------------------------------------------------
                If the channel exists, send a FAILURE indication in the confirm
                message to RRC.
              -----------------------------------------------------------------*/
              rrc_rlc_cmd_ptr->cmd.crlc_ul_tm_config_cnf.
                                          tm_config_result[i].result = FAILURE;
              ERR("DL LC %d Exists, Cannot Establish",
                                          tm_ptr->chan_info[i].lc_id,0,0);
            }
            break;
          case RLC_RELEASE:
            /*-------------------------------------------------------------------
              Check if the channel is in NULL state. If it is not, release
              the channel and send a SUCCESS indication in the confirm message
              to RRC.
            -------------------------------------------------------------------*/
            if ((index = dl_ctl_blk_id_tbl[tm_ptr->chan_info[i].lc_id])
                                                        != INIT_CTL_BLK_ID)
            {
              MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL TM Release LC %d, Index %d",
                                                         tm_ptr->chan_info[i].lc_id,
                                                         index);
              downlink_table.ctrl_blk[index].lc_state = DL_NULL_STATE;
              downlink_table.active_ch--;
              dl_ctl_blk_id_tbl[tm_ptr->chan_info[i].lc_id] = INIT_CTL_BLK_ID;
              rrc_rlc_cmd_ptr->cmd.crlc_ul_tm_config_cnf.
                                          tm_config_result[i].result = SUCCESS;
  
  #ifdef FEATURE_RLC_QXDM_LOGGING
              dl_log_state.dl_state[k].rlc_id = tm_ptr->chan_info[i].lc_id;
              dl_log_state.dl_state[k].lc_state = RLC_DL_NULL_STATE;
              k++;
  #endif
  
            }
            else
            {
              /*-----------------------------------------------------------------
                If the channel does not exist, send a FAILURE indication in the
                confirm message to RRC.
              -----------------------------------------------------------------*/
              rrc_rlc_cmd_ptr->cmd.crlc_ul_tm_config_cnf.
                                          tm_config_result[i].result = FAILURE;
              ERR("DL LC %d doesn't exist - Cannot Release",
                                          tm_ptr->chan_info[i].lc_id,0,0);
            }
            break;
          default:
            /*-------------------------------------------------------------------
              If the action is Invalid, send a FAILURE indication in the confirm
              to RRC.
            -------------------------------------------------------------------*/
            rrc_rlc_cmd_ptr->cmd.crlc_ul_tm_config_cnf.
                                 tm_config_result[i].result = FAILURE;
  
            ERR("Invalid Action %d for channel id %d",tm_ptr->dl_tm_act[i].action,tm_ptr->chan_info[i].lc_id,0);
        }
      }
    }
  }
  
  L2_ACQ_DL_UNLOCK();

  /*-------------------------------------------------------------------------
    Send confirmation to RRC.
  -------------------------------------------------------------------------*/
  rrc_put_rlc_cmd(rrc_rlc_cmd_ptr);

#ifdef FEATURE_RLC_QXDM_LOGGING
  if (j > 0)
  {
    dl_tm_log_ptr.nchan = j;
    rlc_dl_send_tm_log_packet(&dl_tm_log_ptr);
  }
  if (k > 0)
  {
    dl_log_state.nchan = k;
    rlc_dl_send_state_log_packet(&dl_log_state);
  }
#endif

} /* crlc_dl_tm_config_handler() */


/*===========================================================================

FUNCTION CRLC_DL_UM_CONFIG_HANDLER

DESCRIPTION
  This function handles the configuration of Unacknowledged Mode Logical
  channels. This function is called when RRC sends a CRLC_DL_UM_CONFIG_REQ
  to RLC. If a new channel is to be established, the function checks if
  the channels exists. If it already exists, and ERR message is dispalyed.
  If the channel does not exist, the downlink table entry for that channel
  is updated and the state is set to DL_DATA_TRANSFER_READY. If a channel
  is to be released,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the channel is set to the NULL
  state and the watermark is set to NULL. If the parameters of a channel
  are to be modified ,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the downlink table entry for the
  channel is updated with the configuration received from RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_um_config_handler
(
  rlc_dl_um_config_type *um_ptr  /* Pointer to UM config info              */
)
{
  rrc_cmd_type *rrc_rlc_cmd_ptr; /* Command buffer to store confirm        */
  uint8 index;
  int i;
  rlc_channel_action_cnf_e_type action;
#ifdef FEATURE_RLC_QXDM_LOGGING
  uint8 j = 0;
  uint8 k = 0;
  boolean est_or_reest[UE_MAX_DL_UM_CHANNEL];
#endif

  /*-------------------------------------------------------------------------
    Get a command buffer to store the confirmation information to send to
    RRC.
  -------------------------------------------------------------------------*/
  rrc_rlc_cmd_ptr = rrc_get_rlc_cmd_buf();
  if (rrc_rlc_cmd_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: Could not get command buf");
    return;
  }
  rrc_rlc_cmd_ptr->cmd_hdr.cmd_id = RRC_CRLC_DL_UM_CONFIG_CNF;
  rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.nchan = um_ptr->nchan;

  L2_ACQ_DL_LOCK();

  for (i = 0; i < um_ptr->nchan; i++)
  {
    rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.um_config_result[i].lc_id =
                                                  um_ptr->chan_info[i].lc_id;
    if (um_ptr->chan_info[i].lc_id >= UE_MAX_DL_LOGICAL_CHANNEL)
    {
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: Invalid DL UM LC Id %d",
                                                  um_ptr->chan_info[i].lc_id);
      rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                  um_config_result[i].result = FAILURE;
    }
    else
    {
      switch(um_ptr->dl_um_act[i].action)
      {
        case RLC_ESTABLISH:
          /*-------------------------------------------------------------------
            Make sure the channel does not exist. If it does not, get an index
            into the downlink table and store all the information for the
            channel. Send a SUCCESS indication in the confirm message to RRC
          -------------------------------------------------------------------*/
          if ((dl_ctl_blk_id_tbl[um_ptr->chan_info[i].lc_id] == INIT_CTL_BLK_ID) &&
             ((index = dl_assign_ctl_blk()) != INIT_CTL_BLK_ID))
          {
            dl_ctl_blk_id_tbl[um_ptr->chan_info[i].lc_id] = index;

            rlci_establish_dl_um(&downlink_table.ctrl_blk[index],
                                 &um_ptr->chan_info[i],
                                 &um_ptr->dl_um_act[i]);

            rlc_enh_establish_dl_um(&downlink_table.ctrl_blk[index]);
            rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                 um_config_result[i].result = SUCCESS;
            downlink_table.active_ch++;
#ifdef FEATURE_RLC_QXDM_LOGGING
            rlci_generate_dl_config_event(&downlink_table.ctrl_blk[index],
                                          um_ptr->chan_info[i].rlc_size);
            dl_um_log_ptr.chan_info[j] = um_ptr->chan_info[i];
	    est_or_reest[j] = TRUE;
            j++;
            dl_log_state.dl_state[k].rlc_id = um_ptr->chan_info[i].lc_id;
            dl_log_state.dl_state[k].lc_state = UM_DL_DATA_TRANSFER_READY;
            k++;
#endif
          }
          else
          {
            /*-----------------------------------------------------------------
              If the channel exists, send a FAILURE indication in the confirm
              message to RRC.
            -----------------------------------------------------------------*/
            rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                        um_config_result[i].result = FAILURE;
            MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: DL LC %d in use - No Establish",
                                                        um_ptr->chan_info[i].lc_id);
          }
          break;
        case RLC_RELEASE:
          /*-------------------------------------------------------------------
            Make sure the channel is not in NULL state. If it is not, release
            the channel and send a SUCCESS indication in the confirm message
            to RRC.
          -------------------------------------------------------------------*/
          L2_MUTEX_LOCK(&rlc_channel_release_mutex);
          if ((index = dl_ctl_blk_id_tbl[um_ptr->chan_info[i].lc_id]) != INIT_CTL_BLK_ID)
          {
            MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL UM Release LC %d, Index %d",
                                                       um_ptr->chan_info[i].lc_id,
                                                       index);

            rlci_release_dl_um(&(downlink_table.ctrl_blk[index]));

            dl_ctl_blk_id_tbl[um_ptr->chan_info[i].lc_id] = INIT_CTL_BLK_ID;
            rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                        um_config_result[i].result = SUCCESS;
            downlink_table.active_ch--;
#ifdef FEATURE_RLC_QXDM_LOGGING
            dl_log_state.dl_state[k].rlc_id = um_ptr->chan_info[i].lc_id;
            dl_log_state.dl_state[k].lc_state = RLC_DL_NULL_STATE;
            k++;
#endif
          }
          else
          {
            /*-----------------------------------------------------------------
              If the channel does not exist, send a FAILURE indication in the
              confirm message to RRC.
            -----------------------------------------------------------------*/
            rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                 um_config_result[i].result = FAILURE;
            MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: DL LC %d Not config'd - No Release",
                                                        um_ptr->chan_info[i].lc_id);
          }
          L2_MUTEX_UNLOCK(&rlc_channel_release_mutex);
          break;

        case RLC_MODIFY:
          rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                                um_config_result[i].result = SUCCESS;

          if(um_ptr->dl_um_act[i].action == RLC_MODIFY)
          {
            while(um_ptr->dl_um_act[i].dl_cfg_mask != 0)
            {
              action =rlci_get_config_action(&(um_ptr->dl_um_act[i].dl_cfg_mask));
              switch(action)
              {
                case RLCI_MODIFY:
                  /*-------------------------------------------------------------------
                    Make sure the channel is not in NULL state. If it is not, Modify
                    the channel parameters and send a SUCCESS indication in the
                    confirm message to RRC.
                  -------------------------------------------------------------------*/
                  if ((index = dl_ctl_blk_id_tbl[um_ptr->chan_info[i].lc_id]) != INIT_CTL_BLK_ID)
                  {
                    MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL UM Modify LC %d, Index %d",
                                                               um_ptr->chan_info[i].lc_id,
                                                               index);

                    rlci_modify_dl_um(&downlink_table.ctrl_blk[index],
                                      &um_ptr->chan_info[i],
                                      &um_ptr->dl_um_act[i]);
                    
                    rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                                um_config_result[i].result = SUCCESS;
        #ifdef FEATURE_RLC_QXDM_LOGGING
                    rlci_generate_dl_config_event(&downlink_table.ctrl_blk[index],
                                                  um_ptr->chan_info[i].rlc_size);
                    if (j < UE_MAX_DL_UM_CHANNEL)
                    {
                      dl_um_log_ptr.chan_info[j] = um_ptr->chan_info[i];
		      est_or_reest[j] = FALSE;
                      j++;
                    }
        #endif
                  }
                  else
                  {
                    /*-----------------------------------------------------------------
                      If the channel does not exist, send a FAILURE indication in the
                      confirm message to RRC.
                    -----------------------------------------------------------------*/
                    rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                                um_config_result[i].result = FAILURE;
                    ERR("DL LC %d not config'd -- No Mod",
                                                um_ptr->chan_info[i].lc_id,0,0);
                  }
                  break;
        
                case RLCI_STOP:
                  if ((index = dl_ctl_blk_id_tbl[um_ptr->chan_info[i].lc_id]) != INIT_CTL_BLK_ID)
                  {
                    /* Enter into Stop state. */
                    /*lint -e641 -e655 -e64 */
                    downlink_table.ctrl_blk[index].lc_state |= DL_STOP;
                    downlink_table.ctrl_blk[index].lc_state &= (~DL_DATA_TRANSFER_READY);
                    rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                         um_config_result[i].result = SUCCESS;
                    /*lint +e641 +e655 +e64 */
                    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL UM LC %d - Stop",
                                                               um_ptr->chan_info[i].lc_id);
                  }
                  else
                  {
                    rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                         um_config_result[i].result = FAILURE;
                    ERR("DL UM LC %d not Config'd -- No Stop", um_ptr->chan_info[i].lc_id,0,0);
                  }
                  break;
        
                case RLCI_CONTINUE:
                  if ((index = dl_ctl_blk_id_tbl[um_ptr->chan_info[i].lc_id]) != INIT_CTL_BLK_ID)
                  {
                    /* Remove Stop state. Enter into the existing states before entering  */
                    /* into the Stop State.                                               */
                    /*lint -e64 -e641 -e655 */
                    downlink_table.ctrl_blk[index].lc_state &= (~DL_STOP);
                    downlink_table.ctrl_blk[index].lc_state |= DL_DATA_TRANSFER_READY;
                    rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                         um_config_result[i].result = SUCCESS;
                    /*lint +e64 +e641 +e655 */
                    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL UM LC %d - Continue",
                                                               um_ptr->chan_info[i].lc_id);
                  }
                  else
                  {
                    rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                                um_config_result[i].result = FAILURE;
                    ERR("DL UM LC %d not Config'd -- No Continue", um_ptr->chan_info[i].lc_id,0,0);
                  }
                  break;
      
                case RLCI_RE_ESTABLISH:
                  if ((index = dl_ctl_blk_id_tbl[um_ptr->chan_info[i].lc_id]) != INIT_CTL_BLK_ID)
                  {
                    rlci_re_establish_dl_um(&downlink_table.ctrl_blk[index], &um_ptr->chan_info[i]);
                    rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                         um_config_result[i].result = SUCCESS;
        #ifdef FEATURE_RLC_QXDM_LOGGING
                    rlci_generate_dl_config_event(&downlink_table.ctrl_blk[index],
                                                  um_ptr->chan_info[i].rlc_size);
                    if (j < UE_MAX_DL_UM_CHANNEL)
                    {
                      dl_um_log_ptr.chan_info[j] = um_ptr->chan_info[i];
		      est_or_reest[j] = TRUE;
                      j++;
                    }
        #endif
                  }
                  else
                  {
                    /*-----------------------------------------------------------------
                      If the channel does not exist, send a FAILURE indication in the
                      confirm message to RRC.
                    -----------------------------------------------------------------*/
                    rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                                um_config_result[i].result = FAILURE;
                    ERR("DL LC %d not config'd -- No Re-establish",
                                                um_ptr->chan_info[i].lc_id,0,0);
                  }
                  break;
      
                default:
                  /*-------------------------------------------------------------------
                    If the action is Invalid, send a FAILURE indication in the confirm
                    to RRC.
                  -------------------------------------------------------------------*/
                  rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                                       um_config_result[i].result = FAILURE;
                  ERR("Invalid Action %d for channel %d",
                      um_ptr->dl_um_act[i].action,
                      um_ptr->chan_info[i].lc_id,0);
      
              }
            }
          }
          break;
        default:
          /*-------------------------------------------------------------------
            If the action is Invalid, send a FAILURE indication in the confirm
            to RRC.
          -------------------------------------------------------------------*/
          rrc_rlc_cmd_ptr->cmd.crlc_dl_um_config_cnf.
                               um_config_result[i].result = FAILURE;
          ERR("Invalid Action %d for channel %d",
              um_ptr->dl_um_act[i].action,
              um_ptr->chan_info[i].lc_id,0);
      }
    }
  }

  L2_ACQ_DL_UNLOCK();
  rrc_put_rlc_cmd(rrc_rlc_cmd_ptr);
#ifdef FEATURE_RLC_QXDM_LOGGING
  if (j > 0)
  {
    dl_um_log_ptr.nchan = j;
    rlc_dl_send_um_log_packet(&dl_um_log_ptr, est_or_reest);
  }
  if (k > 0)
  {
    dl_log_state.nchan = k;
    rlc_dl_send_state_log_packet(&dl_log_state);
  }
#endif

} /* crlc_dl_um_config_handler() */


/*===========================================================================

FUNCTION CRLC_DL_AM_CONFIG_HANDLER

DESCRIPTION
  This function handles the configuration of an Acknowledged Mode entity.
  it is called when RRC sends a CRLC_DL_AM_CONFIG_REQ to RLC. If a new
  entity is to be established, the function checks if the channels exists.
  If it already exists, an ERR message is dispalyed. If the entity does not
  exist, an downlink table entry for this entity is updated and the state is
  set to DL_DATA_TRANSFER_READY.
  If an entity is to be released,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the channel is set to the NULL state
  and the watermark is set to NULL. If the parameters of a channel
  are to be modified ,the function checks if the channel is in the
  DL_DATA_TRANSFER_READY state. If it is, the downlink table entry for the
  channel is updated with the configuration received from RRC.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_am_config_handler
(
  rlci_dl_am_config_req_type *dl_am_config_req_ptr
)
{
  rlc_am_config_req_type *am_config_req_ptr;
  rlc_am_common_config_type *common_ptr;
  wcdma_l2_ul_cmd_type *ul_cmd_ptr;
  uint16 i;
  uint32 act_dl_am_chnl = 0, index = 0;
  uint8 ctl_blk_id;
  boolean result = FALSE;
  rlc_channel_action_cnf_e_type action;

#ifdef FEATURE_RLC_QXDM_LOGGING
  uint8 j = 0;
  uint8 k = 0;
  boolean est_or_reest[UE_MAX_AM_ENTITY];
#endif

  /*-------------------------------------------------------------------------
    Post RLCI_DL_CONFIG_CNF to the Uplink for the config completion
  -------------------------------------------------------------------------*/
  am_config_req_ptr = &(dl_am_config_req_ptr->am_config_req);
  ul_cmd_ptr = wcdma_l2_ul_get_cmd_buf();
  if (ul_cmd_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: Could not get command buf");
    return;
  }
  ul_cmd_ptr->cmd_hdr.cmd_id = RLCI_DL_CONFIG_CNF;
  ul_cmd_ptr->cmd_data.dl_config_cnf.nchan = am_config_req_ptr->nentity;
 
  for (i=0; i < am_config_req_ptr->nentity; i++)
  {
    common_ptr = &(am_config_req_ptr->am_info[i]->common_config);
    ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].ul_data_id =
                                          common_ptr->ul_data_id;
    if(am_config_req_ptr->am_act[i].action == RLC_ESTABLISH)
    {
      ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_ESTABLISH;
    }
    else if(am_config_req_ptr->am_act[i].action == RLC_RELEASE)
    {
      ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_RELEASE;
    }
    else if(am_config_req_ptr->am_act[i].action == RLC_MODIFY)
    {
      action = rlci_get_config_action(&(am_config_req_ptr->am_act[i].ul_cfg_mask));

      if ((RLCI_RE_ESTABLISH == action) || 
          (((am_config_req_ptr->am_act[i].dl_cfg_mask) & RLC_RE_ESTABLISH_CFG) == RLC_RE_ESTABLISH_CFG))
      {
        ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_RE_ESTABLISH;
      }
      else if(RLCI_MODIFY == action)
      {
        ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_MODIFY;
      }
      else
      {
        ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_NO_ACTION;
      }
    }
    ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].rb_id = common_ptr->rb_id;

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
    if (rlc_debug_var & RLC_DBG_CORRUPTION)
    {
      MSG_9(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH,
         "Padding_DBG: DL lc_id %d, action %d, dl_cfg_mask %d, \
          ul_cfg_mask %d, ul_rlc_size %d, dl_rlc_size %d",
          common_ptr->dl_data_id, am_config_req_ptr->am_act[i].action, 
          am_config_req_ptr->am_act[i].dl_cfg_mask, 
          am_config_req_ptr->am_act[i].ul_cfg_mask, 
          am_config_req_ptr->am_info[i]->rlc_ul_am_config.ul_rlc_size, 
          am_config_req_ptr->am_info[i]->rlc_dl_am_config.dl_rlc_size, 0, 0, 0);
    }
#endif
    if ((dl_am_config_req_ptr->result[i] == SUCCESS) &&
        (common_ptr->dl_data_id < UE_MAX_DL_LOGICAL_CHANNEL))
    {
      switch(am_config_req_ptr->am_act[i].action)
      {
        case RLC_ESTABLISH:
          if ((dl_ctl_blk_id_tbl[common_ptr->dl_data_id] == INIT_CTL_BLK_ID) &&
              ((ctl_blk_id = dl_assign_ctl_blk()) != INIT_CTL_BLK_ID))
          {
            dl_ctl_blk_id_tbl[common_ptr->dl_data_id] = ctl_blk_id;
            dl_ctl_blk_id_tbl[common_ptr->dl_control_id] = ctl_blk_id;
            downlink_table.active_ch +=
              rlci_establish_dl_am(&downlink_table.ctrl_blk[ctl_blk_id],
                                   am_config_req_ptr->am_info[i]
#ifdef FEATURE_HSDPA_MAC_EHS
                                   , &result
#endif 
               );          
            result = rlci_enh_establish_dl_am(&downlink_table.ctrl_blk[ctl_blk_id],
                                              common_ptr->dl_data_id);
            
            if (result == FALSE)
            {
              ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
              break;
            }

            ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = SUCCESS;
            MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "DL AM Establish:Data ID = %d, index = %d",
                                                       common_ptr->dl_data_id,
                                                       ctl_blk_id);
            /*---------------------------------------------------------------------
              Delay timer setting until this RLC's first received PDU.
            ----------------------------------------------------------------------*/
            downlink_table.ctrl_blk[ctl_blk_id].ctrl.am_ctrl.dl_state_var.fst_rx_pdu = TRUE;          
            downlink_table.ctrl_blk[ctl_blk_id].ctrl.am_ctrl.dl_tmr_sts = RLC_AM_TIMER_UNDEF_STATUS;

#ifdef FEATURE_RLC_QXDM_LOGGING
            rlci_generate_dl_config_event(&downlink_table.ctrl_blk[ctl_blk_id],
                                          0);
            /*---------------------------------------------------------------------
              Update the state information for the particular AM entity.
            ----------------------------------------------------------------------*/
            if (j < UE_MAX_AM_ENTITY)
            {
              dl_am_log_ptr.am_info[j] = am_config_req_ptr->am_info[i];
	      est_or_reest[j] = TRUE;
              j++;
            }
            if (k < UE_MAX_DL_LOGICAL_CHANNEL)
            {
              dl_log_state.dl_state[k].rlc_id = common_ptr->dl_data_id;
              dl_log_state.dl_state[k].lc_state = AM_DL_DATA_TRANSFER_READY;
              k++;
            }
#endif
          }
          else
          {
            ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
            MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: DL AM LC %d in use, No Establish ",
                                                        common_ptr->dl_data_id);
          }


          break;

        case RLC_RELEASE:
          L2_MUTEX_LOCK(&rlc_channel_release_mutex);
          if ((ctl_blk_id = dl_ctl_blk_id_tbl[common_ptr->dl_data_id]) != INIT_CTL_BLK_ID)
          {
            downlink_table.active_ch -=
              rlci_release_dl_am(&downlink_table.ctrl_blk[ctl_blk_id]);
              
            dl_ctl_blk_id_tbl[common_ptr->dl_data_id] = INIT_CTL_BLK_ID;
            dl_ctl_blk_id_tbl[common_ptr->dl_control_id] = INIT_CTL_BLK_ID;

            for (index = 0, act_dl_am_chnl = 0; index < UE_MAX_DL_LOGICAL_CHANNEL; index++)
            {
              ctl_blk_id = dl_ctl_blk_id_tbl[index];
              if ((INIT_CTL_BLK_ID != ctl_blk_id) && 
                  ((DL_NULL_STATE != downlink_table.ctrl_blk[ctl_blk_id].lc_state) && 
                   (UE_MODE_ACKNOWLEDGED == downlink_table.ctrl_blk[ctl_blk_id].lc_mode)))
              {
                act_dl_am_chnl++;
                break;
              }
            }

            if ((0 == act_dl_am_chnl) && (TRUE == rlc_dl_am_base_tmr_start))
            {
              /* Since all AM channels are released, stop base dl am timer */
              (void) rex_clr_timer(&rlc_dl_am_base_tmr);
              rlc_dl_am_base_tmr_start = FALSE;
              
              MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC_TMR_DBG: Stopping DL base timer, active_dl_chan %d",
                                                         downlink_table.active_ch);
            }
          
            MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL AM LC %d - Release, acitve_dl_channels %d",
                                                       common_ptr->dl_data_id,
                                                       downlink_table.active_ch);
            ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = SUCCESS;
#ifdef FEATURE_RLC_QXDM_LOGGING
            if (k < UE_MAX_DL_LOGICAL_CHANNEL)
            {
              dl_log_state.dl_state[k].rlc_id = common_ptr->dl_data_id;
              dl_log_state.dl_state[k].lc_state = RLC_DL_NULL_STATE;
              k++;
            }
#endif
          }
          else
          {
            ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
            MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: DL AM LC %d not config'd, No Release",
                                                        common_ptr->dl_data_id);
          }
          L2_MUTEX_UNLOCK(&rlc_channel_release_mutex);
          break;

      case RLC_MODIFY:

        ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = SUCCESS;
        if(am_config_req_ptr->am_act[i].action == RLC_MODIFY)
        {
          while(am_config_req_ptr->am_act[i].dl_cfg_mask != 0)
          {
            action = rlci_get_config_action(&(am_config_req_ptr->am_act[i].dl_cfg_mask));
            switch(action)
            {
              case RLCI_MODIFY :
                //ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_MODIFY;
                //if (RLCI_RE_ESTABLISH == rlci_get_config_action(&(am_config_req_ptr->am_act[i].ul_cfg_mask)))
                //{
                //  ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_RE_ESTABLISH;
                //}

                if ((ctl_blk_id = dl_ctl_blk_id_tbl[common_ptr->dl_data_id]) != INIT_CTL_BLK_ID)
                {
                  rlci_modify_dl_am(&downlink_table.ctrl_blk[ctl_blk_id],
                                    am_config_req_ptr->am_info[i]);
                  MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "DL AM LC %d - Modify",
                                                             common_ptr->dl_data_id);
                  ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = SUCCESS;
      
  #ifdef FEATURE_RLC_QXDM_LOGGING
                  rlci_generate_dl_config_event(&downlink_table.ctrl_blk[ctl_blk_id],
                                                0);
                  if (j < UE_MAX_AM_ENTITY)
                  {
                    dl_am_log_ptr.am_info[j] = am_config_req_ptr->am_info[i];
		    est_or_reest[j] = FALSE;
                    j++;
                  }
  #endif
                }
                else
                {
                  ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
                  ERR("DL LC %d not config'd, No Modi", common_ptr->dl_data_id, 0, 0);
                }
                break;
    
              case RLCI_STOP:
                //ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_STOP;
              
                if ((ctl_blk_id = dl_ctl_blk_id_tbl[common_ptr->dl_data_id]) != INIT_CTL_BLK_ID)
                {
                  /* Enter into Stop state. */
                  /*lint -e64 -e641 -e655 */
                  downlink_table.ctrl_blk[ctl_blk_id].lc_state |= DL_STOP;
                  downlink_table.ctrl_blk[ctl_blk_id].lc_state &= (~DL_DATA_TRANSFER_READY);
                  /*lint +e64 +e641 +e655 */
                  MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL AM LC %d - Stop",
                                                             common_ptr->dl_data_id);
                  ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = SUCCESS;
                }
                else
                {
                  ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
                  ERR("DL AM LC %d not config'd, No Stop", common_ptr->dl_data_id, 0, 0);
                }
                break;
    
              case RLCI_CONTINUE:
                //ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_CONTINUE;
              
                if ((ctl_blk_id = dl_ctl_blk_id_tbl[common_ptr->dl_data_id]) != INIT_CTL_BLK_ID)
                {
                  /* Remove Stop state. Go back to the original states before */
                  /* it entered into the Stop State.                          */
                  /*lint -e64 -e641 -e655 */
                  downlink_table.ctrl_blk[ctl_blk_id].lc_state &= (~DL_STOP);
                  downlink_table.ctrl_blk[ctl_blk_id].lc_state |= DL_DATA_TRANSFER_READY;
                  /*lint +e64 +e641 +e655 */
                  MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL AM LC %d - Continue",
                                                             common_ptr->dl_data_id);
                  ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = SUCCESS;
                }
                else
                {
                  ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
                  ERR("DL AM LC %d not config'd, No Stop", common_ptr->dl_data_id, 0, 0);
                }
                break;
    
              case RLCI_RE_ESTABLISH:
                //ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].action = RLCI_RE_ESTABLISH;
              
                if((ctl_blk_id = dl_ctl_blk_id_tbl[common_ptr->dl_data_id])
                          != INIT_CTL_BLK_ID)
                {
                  rlci_re_establish_dl_am(&downlink_table.ctrl_blk[ctl_blk_id],
                                           am_config_req_ptr->am_info[i]);
                  MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Re-establish DL AM LC %d",
                                                             common_ptr->dl_data_id);
      
                  rlc_enh_free_reseq_q(&downlink_table.ctrl_blk[ctl_blk_id], RLC_RE_ASSEMBLY_Q_SIZE, 0);
                  ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = SUCCESS;
      
  #ifdef FEATURE_RLC_QXDM_LOGGING
                  rlci_generate_dl_config_event(&downlink_table.ctrl_blk[ctl_blk_id],
                                                0);
                  /*--------------------------------------------------------------
                    Update the state information for the particular AM entity.
                  --------------------------------------------------------------*/
                  if (j < UE_MAX_AM_ENTITY)
                  {
                    dl_am_log_ptr.am_info[j] = am_config_req_ptr->am_info[i];
		    est_or_reest[j] = TRUE;
                    j++;
                  }
  #endif
                }
                else
                {
                  ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
                  ERR("DL LC %d not config'd, Unable to Re-establish",
                                  common_ptr->dl_data_id, 0, 0);
                }
                break;
    
              default:
                ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
                ERR("DL AM %d, Invalid Action", common_ptr->dl_data_id, 0, 0);
    
            }
          }
        }
        break;

      default:
          ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
          MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: DL AM %d, Invalid Action",
                                                      common_ptr->dl_data_id);
      }
    }
    else
    {
      ul_cmd_ptr->cmd_data.dl_config_cnf.am_cnf[i].result = FAILURE;
    }
  }


//  wcdma_l2_ul_put_cmd(ul_cmd_ptr);

  rlci_dl_config_cnf_handler(&(ul_cmd_ptr->cmd_data.dl_config_cnf));
  l2_free_cmd_buf(ul_cmd_ptr);


#ifdef FEATURE_RLC_QXDM_LOGGING
  if (j > 0)
  {
    dl_am_log_ptr.nentity = j;
    rlc_dl_send_am_log_packet(&dl_am_log_ptr, est_or_reest);
  }
  if (k > 0)
  {
    dl_log_state.nchan = k;
    rlc_dl_send_state_log_packet(&dl_log_state);
  }
#endif

} /* crlc_dl_am_config_handler() */

/*===========================================================================

FUNCTION RLC_DL_REGISTER_SRVC_HANDLER

DESCRIPTION
  This function handles the registration of the watermark and the callback
  function for the logical channels on the downlink.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_register_srvc_handler
(
  rlc_dl_register_srvc_type *reg_ptr      /* Ptr to WM registration Info   */
)
{
  uint16 index;
  int i;

  L2_ACQ_DL_LOCK();

  /*-------------------------------------------------------------------------
    Register the Downlink Watermark and Callback Function (if any) for
    all the channels indicated in the registration Pointer.
  -------------------------------------------------------------------------*/
  for (i = 0; i < reg_ptr->nchan; i++)
  {
	
    index = dl_ctl_blk_id_tbl[reg_ptr->rlc_data[i].lc_id];
    if ((reg_ptr->rlc_data[i].lc_id < UE_MAX_DL_LOGICAL_CHANNEL) &&
        (index != INIT_CTL_BLK_ID) &&
        (downlink_table.ctrl_blk[index].lc_state != DL_NULL_STATE))
    {
      /* Task or Interrupt Context */
      downlink_table.ctrl_blk[index].context = reg_ptr->rlc_data[i].context;
      if (reg_ptr->rlc_data[i].dl_wm_ptr != NULL)
      {
        downlink_table.ctrl_blk[index].dl_wm_ptr =
                                            reg_ptr->rlc_data[i].dl_wm_ptr;
        MSG_9(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH,
		       "RLC_WM:DL: RLC-id - %d, Lo_Wm_Cnt - %d, Hi_Wm_Cnt - %d, Do_Not_Exceed - %d, context %d Funct Ptr 0x%x",
                          reg_ptr->rlc_data[i].lc_id,
                          downlink_table.ctrl_blk[index].dl_wm_ptr->lo_watermark,
                          downlink_table.ctrl_blk[index].dl_wm_ptr->hi_watermark,
                          downlink_table.ctrl_blk[index].dl_wm_ptr->dont_exceed_cnt,
                          reg_ptr->rlc_data[i].context, reg_ptr->rlc_data[i].rlc_post_rx_proc_func_ptr,0, 0, 0);						  
      }
      else
      {
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: WM ptr NULL, LC %d",
                                                    reg_ptr->rlc_data[i].lc_id);
      }

      if (reg_ptr->rlc_data[i].rlc_post_rx_proc_func_ptr != NULL)
      {
        downlink_table.ctrl_blk[index].rlc_post_rx_func_ptr =
                      reg_ptr->rlc_data[i].rlc_post_rx_proc_func_ptr;
        downlink_table.ctrl_blk[index].rlc_post_rx_func_ptr_para =
                      reg_ptr->rlc_data[i].rlc_post_rx_func_ptr_para;
      }
    }
    else
    {
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: LC %d not config yet",
                                                  reg_ptr->rlc_data[i].lc_id);
    }
  }

  L2_ACQ_DL_UNLOCK();
  
} /* rlc_dl_register_srvc_handler() */


/*===========================================================================

FUNCTION RLC_DL_DEREGISTER_SRVC_HANDLER

DESCRIPTION
  This function handles the de-registration of the watermark and the callback
  function for the logical channels on the downlink.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_deregister_srvc_handler
(
  rlc_dl_deregister_srvc_type *de_reg_ptr      /* Ptr to WM registration Info   */
)
{
  uint16 index;
  int i;

  L2_ACQ_DL_LOCK();

  L2_MUTEX_LOCK(&rlc_dl_tm_mutex);

  /*-------------------------------------------------------------------------
    Register the Downlink Watermark and Callback Function (if any) for
    all the channels indicated in the registration Pointer.
  -------------------------------------------------------------------------*/
  for (i = 0; i < de_reg_ptr->nchan; i++)
  {
    index = dl_ctl_blk_id_tbl[de_reg_ptr->rlc_id[i]];
    if ((de_reg_ptr->rlc_id[i] < UE_MAX_DL_LOGICAL_CHANNEL) &&
        (index != INIT_CTL_BLK_ID))
    {
      /* Task or Interrupt Context */
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DeRegister DL WM, LC %d",
                                                 de_reg_ptr->rlc_id[i]);
      downlink_table.ctrl_blk[index].dl_wm_ptr = NULL;

      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "DeRegister Rx CB fcn, LC %d",
                                                de_reg_ptr->rlc_id[i]);
      downlink_table.ctrl_blk[index].rlc_post_rx_func_ptr = NULL;
    }
    else
    {
      ERR("LC %d not config yet",de_reg_ptr->rlc_id[i], 0, 0);
    }
  }
  
  L2_MUTEX_UNLOCK(&rlc_dl_tm_mutex);

  L2_ACQ_DL_UNLOCK();

} /* rlc_dl_deregister_srvc_handler() */


/*===========================================================================

FUNCTION RLC_REGISTER_AM_SDU_DISCARD_CB_REQ

DESCRIPTION
  Register AM SDU discard callback function.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_register_am_sdu_discard_cb_req
(
  rlc_register_am_sdu_discard_callback_type *dl_sdu_discard_cb
)
{
  uint16 index;
  int i;

  L2_ACQ_DL_LOCK();

  for (i = 0; i < dl_sdu_discard_cb->nchan; i++)
  {
    index = dl_ctl_blk_id_tbl[dl_sdu_discard_cb->sdu_discard_cb[i].lc_id];
    if ((dl_sdu_discard_cb->sdu_discard_cb[i].lc_id <
         UE_MAX_DL_LOGICAL_CHANNEL) &&
        (index != INIT_CTL_BLK_ID) &&
        (downlink_table.ctrl_blk[index].lc_state != DL_NULL_STATE))
    {
      if (dl_sdu_discard_cb->sdu_discard_cb[i].rlc_am_sdu_discard_cb != NULL)
      {
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Register DL WM, LC %d", i);
        downlink_table.ctrl_blk[index].ctrl.am_ctrl.sdu_discard_cb =
                  dl_sdu_discard_cb->sdu_discard_cb[i].rlc_am_sdu_discard_cb;
      }
      else
      {
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: SDU discard cb fcn ptr NULL, LC %d",
                                                    i);
      }
    }
    else
    {
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: LC %d not config yet",
                                                  dl_sdu_discard_cb->sdu_discard_cb[i].lc_id);
    }
  }

  L2_ACQ_DL_UNLOCK();

}/* rlc_register_am_sdu_discard_cb_req() */

/*===========================================================================

FUNCTION RLC_DL_DECODE_PDU_HDR

DESCRIPTION
 This function is called by MAC to decode the RLC header of the received PDU.
 For Unacknowledged Mode PDUs, the first byte of the PDU is extracted
 and the new count is determined with the extracted byte. For Acknowledged
 Mode PDUs, the first 2 bytes of the PDU are extracted and the new count
 is determined with the extracted bytes. If ciphering is turned on, the
 structure is updated with the ciphering parameters.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_dl_decode_pdu_hdr
(
   rlc_dl_tb_hdr_info_type *hdr_ptr,
   rlc_dl_tb_cipher_info_type *cipher_ptr
)
{
  rlci_dl_ctrl_blk_type *ctl_blk = NULL;
  uint16 rlc_pdu_header = 0, rx_sn = 0, cmp_sn;
  uint8  ctl_index;
/*Status flag to return the PDU decode status*/
  boolean status = TRUE;
#ifdef FEATURE_RLC_QXDM_LOGGING
  int cipher_idx;
#endif //FEATURE_RLC_QXDM_LOGGING
  boolean old_config = FALSE;

#ifdef FEATURE_RLC_QXDM_LOGGING
  //rlci_dl_pdu_cipher_log_on = log_status(WCDMA_DL_RLC_V1_PDU_CIPHER_LOG_PACKET);
#endif //FEATURE_RLC_QXDM_LOGGING

  /*---------------------------------------------------------------------
    Init cipher flag to be OFF.
  ---------------------------------------------------------------------*/
  cipher_ptr->cipher_flag = OFF;
  cipher_ptr->ciphering_algo = UE_WCDMA_CIPHER_ALGO_NONE; 
  cipher_ptr->cipher_offset = 0;
  hdr_ptr->discard_l2_pdu = FALSE;
  /* Get the new ctrl Index and ctrl blk ptr */
  ctl_index = RLCI_GET_CTRL_INDEX(hdr_ptr->rlc_id);
  if ( ctl_index == INIT_CTL_BLK_ID )
  {
    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: rlc_id %d is not yet configured",
                                                hdr_ptr->rlc_id);
    status = FALSE;
    return(status);
  }
  /*Get the control block ptr*/
  ctl_blk = RLCI_GET_CTRL_BLK_PTR(ctl_index);
  if ( ctl_blk->lc_state == DL_NULL_STATE )
  {
    MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: lc_state %d is NULL for rlc_id %d",
                                                ctl_blk->lc_state,
                                                hdr_ptr->rlc_id);
    status = FALSE;
    return(status);
  }

  if (dl_ctl_blk_id_tbl[hdr_ptr->rlc_id] != INIT_CTL_BLK_ID &&
      ctl_blk->lc_state != DL_NULL_STATE)
  {
    if (ctl_blk->lc_mode != UE_MODE_TRANSPARENT)
    {
      if (rlc_debug_var & RLC_DBG_DL_CIPHERING)
      {
        MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "B4 cipher %x %x %x",
                                                   rlc_copy_to_word(hdr_ptr->hdr_ptr),
                  rlc_copy_to_word(hdr_ptr->hdr_ptr + 4),
                  rlc_copy_to_word(hdr_ptr->hdr_ptr + 8));
      }

      /*---------------------------------------------------------------------
        It's in UNACKNOWLEDGED mode. Get the 1st byte of the PDU to obtain
        the SN and form count_c.
      ---------------------------------------------------------------------*/
      if (ctl_blk->lc_mode == UE_MODE_UNACKNOWLEDGED)
      {
        rlc_pdu_header = b_unpackb(hdr_ptr->hdr_ptr, hdr_ptr->rlc_pdu_start_offset, 8);
        rx_sn = (rlc_pdu_header >> 1) & 0x007F;

        /*---------------------------------------------------------------------
          Yes, we need to decipher the incoming pdu.
        ---------------------------------------------------------------------*/
        cipher_ptr->cipher_offset = hdr_ptr->rlc_pdu_start_offset + 8;
        if (rlci_dl_um_cipher_pdu(ctl_blk, cipher_ptr, rx_sn))
        {
          cipher_ptr->count_c = rlci_dl_um_get_count_c(ctl_blk, rx_sn);
          /*-------------------------------------------------------------------
            De-ciphering this incoming PDU, since ciphering algorithm is UEA_1
          -------------------------------------------------------------------*/
          if ((ctl_blk->cipher.cipher_algo == UE_WCDMA_CIPHER_ALGO_UEA1)
#ifdef FEATURE_WCDMA_REL7
               || (ctl_blk->cipher.cipher_algo == UE_WCDMA_CIPHER_ALGO_UEA2)
#endif
             )
          {
            cipher_ptr->cipher_flag = ON;
            cipher_ptr->ciphering_algo = ctl_blk->cipher.cipher_algo;
            cipher_ptr->cipher_key_id = ctl_blk->cipher.cipher_key_id;
            cipher_ptr->rb_id = ctl_blk->cipher.rb_id;
            if (rlc_debug_var & RLC_DBG_DL_CIPHERING)
            {
              MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Cipher Key %x, cnt_c %x, offset %x",
                                                         cipher_ptr->cipher_key_id,
                                                         cipher_ptr->count_c,
                                             cipher_ptr->cipher_offset);
            }
          }
          ctl_blk->cipher.new_cipher_on = FALSE;
        }
      }
      /*---------------------------------------------------------------------
        If in ACKNOWLEDGED mode. Get the first 2 bytes of the PDU to obtain
        the SN and form count_c.
      ---------------------------------------------------------------------*/
      else if (ctl_blk->lc_mode == UE_MODE_ACKNOWLEDGED)
      {
        rlc_pdu_header = b_unpackw(hdr_ptr->hdr_ptr, hdr_ptr->rlc_pdu_start_offset, 16);
        /*---------------------------------------------------------------------
          Ok, it is a data PDU.
        ---------------------------------------------------------------------*/
        if (rlc_pdu_header & 0x8000)
        {
          /* Drop data PDUs if RESET_ACK is recived but RLC is still in 
             RESET_PENDING state*/
          if (ctl_blk->lc_state & DL_RESET_PENDING)
          {
            /* Set the discard_pdu_rst_pending to TRUE, to drop the PDUs that are received while DL is in RESET Pending*/
            hdr_ptr->discard_l2_pdu = (ctl_blk->cipher.cipher_mode != NO_CIPHER) ? TRUE : FALSE;

            if (rlc_debug_var & RLC_DBG_DL_CIPHERING)
            {
              MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "NA: LC %d is in RESET PENDING - DATA PDU Hdr: 0x%x Payload: 0x%x in this TTI",
                                                          hdr_ptr->rlc_id,
                                                          rlc_pdu_header,
                              b_unpackw(hdr_ptr->hdr_ptr, hdr_ptr->rlc_pdu_start_offset+16, 32));
            }
            return TRUE;
          }
          rx_sn = (rlc_pdu_header >> 3) & 0x0FFF ;
          /* set cipher_offset ir-respective of cipher flag to pass on
             header number of bits to L1 */
          cipher_ptr->cipher_offset = hdr_ptr->rlc_pdu_start_offset + 16;

          /*---------------------------------------------------------------------
            Yes, we need to decipher the incoming pdu.
          ---------------------------------------------------------------------*/
          if (ctl_blk->cipher.cipher_mode != NO_CIPHER &&
              rlci_dl_am_cipher_pdu(ctl_blk, rx_sn))
          {
            /*---------------------------------------------------------------------
              Find the ciphering starting.
            ---------------------------------------------------------------------*/
            if (ctl_blk->cipher.act_old_cfg1)
            {
              cmp_sn = ctl_blk->cipher.act_sn_old1;
            }
            else if (ctl_blk->cipher.act_old_cfg)
            {
              cmp_sn = ctl_blk->cipher.act_sn_old;
            }
            else
            {
              cmp_sn = ctl_blk->cipher.act_sn;
            }

            /*---------------------------------------------------------------------
            This rx'd PDU needs to be de-ciphering.
            ---------------------------------------------------------------------*/
            if (ctl_blk->cipher.cipher_all ||
                !(RLCI_SEQ_GT(cmp_sn, rx_sn, RLC_AM_MODULUS)))
            {
              ctl_blk->cipher.new_cipher_on = FALSE;
              cipher_ptr->count_c = rlci_dl_am_get_count_c(ctl_blk, rx_sn,
                                                      &(cipher_ptr->cipher_key_id),
                                                      &cipher_ptr->cipher_flag, &old_config);
              cipher_ptr->rb_id = ctl_blk->cipher.rb_id;
              cipher_ptr->ciphering_algo = ctl_blk->cipher.cipher_algo;

              if (rlc_debug_var & RLC_DBG_DL_CIPHERING)
              {
                MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Cipher Key %x, cnt_c %x, offset %x",
                                                           cipher_ptr->cipher_key_id,
                                                           cipher_ptr->count_c,
                                 cipher_ptr->cipher_offset);
              }
            }
          } /* end of Need to cipher pdu */
        } /* end of data PDU */
        else /* its a control PDU */
        {
          /* set cipher_offset ir-respective of cipher flag to pass on
             header number of bits to L1 - there is no AM header for 
             control PDU hence dont add anything to rlc_pdu_start_offset */
          cipher_ptr->cipher_offset = hdr_ptr->rlc_pdu_start_offset;
        }
      } /* AM & UM  */

#ifdef FEATURE_RLC_QXDM_LOGGING
      /*---------------------------------------------------------------------
        Ciphering parameters logging is done for SRBs and selectively for
        user plane data channels.
      ---------------------------------------------------------------------*/
      if (rlci_dl_pdu_cipher_log_on && !rlci_dl_pdu_cipher_log.cipher_pdu_logging &&
          (ON == cipher_ptr->cipher_flag) &&
          ((hdr_ptr->rlc_id < RLC_DL_AM_USR_PLANE_START_LC_ID) || (rx_sn %100 == 0) ||
           (rx_sn < 20)))
      {
        /*---------------------------------------------------------------------
          Skip the status PDU received, since status PDU is not ciphered.
        ---------------------------------------------------------------------*/
        if (ctl_blk->lc_mode == UE_MODE_ACKNOWLEDGED &&
            (rlc_pdu_header & 0x8000) == 0)
        {
          return status;
        }

        if (rlci_dl_pdu_cipher_log.num_pdu < MAX_CIPHER_PDU)
        {
          cipher_idx = rlci_dl_pdu_cipher_log.num_pdu;
          rlci_dl_pdu_cipher_log.cipher_log_data[cipher_idx].log_dl_rlc_id= hdr_ptr->rlc_id;
          rlci_dl_pdu_cipher_log.cipher_log_data[cipher_idx].cipher_key_idx =
                                                            cipher_ptr->cipher_key_id;
          rlci_dl_pdu_cipher_log.cipher_log_data[cipher_idx].cipher_algo=
                                                          ctl_blk->cipher.cipher_algo;
          rlci_dl_pdu_cipher_log.cipher_log_data[cipher_idx].count_c= cipher_ptr->count_c;
          rlci_dl_pdu_cipher_log.cipher_log_data[cipher_idx].b4_cipher =
                      b_unpackd(hdr_ptr->hdr_ptr, hdr_ptr->rlc_pdu_start_offset, 32);
          /*------------------------------------------------------------------
            After filling in the PDU cipher parameters, increase the index
            to the next entry.
          --------------------------------------------------------------------*/
          rlci_dl_pdu_cipher_log.num_pdu ++;
        }
      }
#endif //FEATURE_RLC_QXDM_LOGGING
    } /* NON TM */
  }
  else
  {
    /*Set the status flag to FALSE in case the LC ID is not configured*/
    status = FALSE;
    MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: RLC Channel %d not exist, %d",
                                                hdr_ptr->rlc_id,
                                                ctl_blk->cipher.cipher_mode);
  }
  return status;
}/* rlc_dl_decode_pdu_hdr() */


/*===========================================================================

FUNCTION GET_AM_CTL_RLK

DESCRIPTION
  This function return the pointer of control block for an AM.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the AM control block.

SIDE EFFECTS
  None
===========================================================================*/

rlci_dl_ctrl_blk_type *get_am_ctl_blk
(
  uint8 lc_id
)
{
  return (&(downlink_table.ctrl_blk[dl_ctl_blk_id_tbl[lc_id]]));
} /* get_am_ctl_blk */

/*===========================================================================

FUNCTION RLCi_CHK_DL_RESET_PENDING

DESCRIPTION
  This function checks if the DL is in RESET pending state.

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating, RESET pending or not.

SIDE EFFECTS
  None
===========================================================================*/

boolean rlci_chk_dl_reset_pending
(
  rlc_lc_id_type lc_id
)
{
  rlci_dl_ctrl_blk_type *dl_ctrl_blk_ptr = get_am_ctl_blk(lc_id);
  boolean               ret_val = FALSE;
  
  /*lint -save -e655 */
  if ((dl_ctrl_blk_ptr != NULL) && 
      (dl_ctrl_blk_ptr->lc_state & (rlci_dl_state_e_type)DL_RESET_PENDING))
  {
    ret_val = TRUE;
  }
  /*lint -restore */
  return ret_val;
} /* rlci_chk_dl_reset_pending */

/*===========================================================================

FUNCTION RLCI_DL_PROC_RESET_MSG

DESCRIPTION
  Process the RLCI_UL_RESET_DONE and RLCI_DL_RESET_REQ messages from
  UL AM RLC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_proc_reset_msg
(
  wcdma_l2_dl_cmd_enum_type cmd_id,
  rlc_lc_id_type dl_rlc_data_id
)
{
  uint8 ctl_id;
  rlci_dl_ctrl_blk_type *ctrl_blk;
  rex_crit_sect_type *datapath_mutex = NULL;
  ctl_id = dl_ctl_blk_id_tbl[dl_rlc_data_id];

  ctrl_blk = &downlink_table.ctrl_blk[ctl_id];
  MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "Received Reset related msg id:%d",
                                            cmd_id);


  L2_ACQ_DL_LOCK();

  if (cmd_id == RLCI_UL_RESET_DONE)
  {
    uint16 rlc_win_size = 0;
    rlci_ul_ctrl_blk_type *ul_ctrl_blk_ptr =  NULL;

    RLC_ACQ_FC_LOCK();
    datapath_mutex = L2_ACQ_UL_LW_LOCK(ctrl_blk->ctrl.am_ctrl.am_common->ul_data_id);

    ul_ctrl_blk_ptr =  rlci_get_ctl_blk_ptr( ctrl_blk->ctrl.am_ctrl.am_common->ul_data_id);
    /* Complete the RESET procedure. Return back to the original state. */
    /*lint -e64 -e641 */
    downlink_table.ctrl_blk[ctl_id].lc_state &= (~DL_RESET_PENDING);
    /*lint +e64 +e641 */
    if ((rlci_dl_fc_status())&&  (ul_ctrl_blk_ptr != NULL)
          && (ul_ctrl_blk_ptr->ctrl.am_ctrl.ul_state.dl_cpu_base_fc_f)
        /*Change the peer win to the WIN SIZE triggered by FC before RESET*/
        )
    {
      if ( ctrl_blk->ctrl.am_ctrl.dl_ctl_cnfg->rlc_info.rx_window_size > MAX_RX_WINDOW_SIZE)
      {
        ctrl_blk->ctrl.am_ctrl.dl_state_var.rx_win = MAX_RX_WINDOW_SIZE;
      }
      else
      {
        ctrl_blk->ctrl.am_ctrl.dl_state_var.rx_win =
                ctrl_blk->ctrl.am_ctrl.dl_ctl_cnfg->rlc_info.rx_window_size;
      }
      rlc_dl_multi_client_fc_modify_max_flow
        (ctrl_blk->ctrl.am_ctrl.am_common->ul_data_id,
         ctrl_blk->ctrl.am_ctrl.dl_state_var.rx_win,
         FALSE);
                          
      rlc_win_size = rlc_dl_multi_client_fc_get_win_size
        (ctrl_blk->ctrl.am_ctrl.am_common->ul_data_id);
      if(rlc_win_size < ctrl_blk->ctrl.am_ctrl.dl_state_var.rx_win)
      {
        rlci_change_peer_tx_win(ctrl_blk->ctrl.am_ctrl.am_common->ul_data_id,
                      rlc_win_size);
      }
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Sending  win SUFI with win size %d configured by FC before RESET",
                                                 rlc_win_size);
    }
    else
    {
      /*----------------------------------------------------------------------
       Set the receiver's window size. If the window size is greater than
       the maximum RX_Window size that UE can handle, send a window sufi
       indicating that the receiver's window size is MAX_RX_WINDOW_SIZE
      ----------------------------------------------------------------------*/
      if(ctrl_blk->ctrl.am_ctrl.dl_ctl_cnfg->rlc_info.rx_window_size > MAX_RX_WINDOW_SIZE)
      {
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "Max window size on DL = %d",
                                                  MAX_RX_WINDOW_SIZE);
        ctrl_blk->ctrl.am_ctrl.dl_state_var.rx_win = MAX_RX_WINDOW_SIZE;

        rlc_dl_multi_client_fc_modify_max_flow
          (ctrl_blk->ctrl.am_ctrl.am_common->ul_data_id,
           ctrl_blk->ctrl.am_ctrl.dl_state_var.rx_win,
           FALSE);
        rlci_change_peer_tx_win(ctrl_blk->ctrl.am_ctrl.am_common->ul_data_id,
                                rlc_dl_multi_client_fc_get_win_size
                                (ctrl_blk->ctrl.am_ctrl.am_common->ul_data_id));
      }
      else
      {
        ctrl_blk->ctrl.am_ctrl.dl_state_var.rx_win =
                 ctrl_blk->ctrl.am_ctrl.dl_ctl_cnfg->rlc_info.rx_window_size;
        rlc_dl_multi_client_fc_modify_max_flow
          (ctrl_blk->ctrl.am_ctrl.am_common->ul_data_id,
           ctrl_blk->ctrl.am_ctrl.dl_state_var.rx_win,
           TRUE);

      }
    }
    ctrl_blk->ctrl.am_ctrl.dl_state_var.vr_mr =
                                ctrl_blk->ctrl.am_ctrl.dl_state_var.rx_win;
    L2_ACQ_UL_LW_UNLOCK(datapath_mutex);
    RLC_ACQ_FC_UNLOCK();

    if (rlci_ul_fc_status())
    {
      if (ul_ctrl_blk_ptr 
         && ul_ctrl_blk_ptr->ctrl.am_ctrl.ul_state.ul_cpu_base_fc_f
      )
      {
         /*After RESET is done configure TX WIN to previous size*/
         rlci_ul_fc_calc_new_tx_win_size(ul_ctrl_blk_ptr);
      }
    }
    rlci_initiate_reset_for_lower_dl_hfn(dl_rlc_data_id);

#ifdef FEATURE_RLC_QXDM_LOGGING
    dl_log_state.dl_state[0].rlc_id = dl_rlc_data_id;
    dl_log_state.dl_state[0].lc_state = AM_DL_DATA_TRANSFER_READY;
    dl_log_state.nchan = 1;
    rlc_dl_send_state_log_packet(&dl_log_state);
#endif

  }
  else if (cmd_id == RLCI_DL_RESET_REQ)
  {
    /* Rx RESET request from the peer side via the DL AM RLC. */
    rlci_reset_dl_am(&downlink_table.ctrl_blk[ctl_id], FALSE);
  }
  else
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Unknown DL RLC reset msg");
  }
  
  L2_ACQ_DL_UNLOCK();
  
} /* rlci_dl_proc_reset_msg() */


/*===========================================================================

FUNCTION CRLC_DL_RELEASE_ALL_HANDLER

DESCRIPTION
  Releases all downlink Channels and sends a confirmation to the uplink
  after release.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_release_all_handler
(
  void
)
{
  uint8 ctrl_blk_id, num_ch_released;       /* Index into the Downlink Table       */
  rrc_cmd_type *rrc_rlc_cmd_ptr;            /* Command Buffer for sending Release  */
                                            /* All confirm to RRC                  */
  uint8 i = 0;
  uint32 act_dl_am_chnl = 0, index = 0;

#ifdef FEATURE_RLC_QXDM_LOGGING
  uint8 j = 0;
#endif
  num_ch_released = 0;

  /*-------------------------------------------------------------------------
    Get command Buffer for posting Release ALL confirm Confirm to RRC.
  -------------------------------------------------------------------------*/
  rrc_rlc_cmd_ptr = rrc_get_rlc_cmd_buf();
  if (rrc_rlc_cmd_ptr == NULL)
  {
   ERR_FATAL("Could not get cmd buffer",0,0,0);
   /*lint -e527 */
   return;
   /*lint +e527 */
  }
  rrc_rlc_cmd_ptr->cmd_hdr.cmd_id = RRC_CRLC_RELEASE_ALL_CNF;
  MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "Release ALL Downlink Channels, No.of DL-LC %d",
                                            downlink_table.active_ch);

  L2_ACQ_DL_LOCK();

  /*-------------------------------------------------------------------------
    Release all active Downlink Channels.
  -------------------------------------------------------------------------*/
  L2_MUTEX_LOCK(&rlc_channel_release_mutex);
  while (downlink_table.active_ch > 0)
  {
    /*-----------------------------------------------------------------------
      Check if the channel is in NULL state. If it is not, release
      the channel.
    -----------------------------------------------------------------------*/
    if ((ctrl_blk_id = dl_ctl_blk_id_tbl[i]) != INIT_CTL_BLK_ID)
    {
      num_ch_released++;
      if (downlink_table.ctrl_blk[ctrl_blk_id].lc_mode == UE_MODE_ACKNOWLEDGED)
      {
        downlink_table.active_ch -=
               rlci_release_dl_am(&downlink_table.ctrl_blk[ctrl_blk_id]);

        downlink_table.ctrl_blk[ctrl_blk_id].lc_state = DL_NULL_STATE;
        dl_ctl_blk_id_tbl[i] = INIT_CTL_BLK_ID;
      }
      else
      {
        downlink_table.active_ch--;

        if((downlink_table.ctrl_blk[ctrl_blk_id].lc_mode == UE_MODE_UNACKNOWLEDGED) 
#ifdef FEATURE_UMTS_BMC
           ||(downlink_table.ctrl_blk[ctrl_blk_id].lc_mode == UE_MODE_DL_CTCH)
#endif
          )
        {
          {
            rlci_release_dl_um(&(downlink_table.ctrl_blk[ctrl_blk_id]));
            dl_ctl_blk_id_tbl[i] = INIT_CTL_BLK_ID;                
          }
        }
        else
        {
          downlink_table.ctrl_blk[ctrl_blk_id].lc_state = DL_NULL_STATE;
          dl_ctl_blk_id_tbl[i] = INIT_CTL_BLK_ID;
        }
      }

#ifdef FEATURE_RLC_QXDM_LOGGING
      if(j < UE_MAX_DL_LOGICAL_CHANNEL)
      {
        dl_log_state.dl_state[j].rlc_id = i;
        dl_log_state.dl_state[j].lc_state = RLC_DL_NULL_STATE;
        j++;
      }
#endif
    }

    i++;
  }

  MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Num DL LC released %d, No.of active DL-LC %d",
                                     num_ch_released, downlink_table.active_ch);
  for (index = 0, act_dl_am_chnl = 0; index < UE_MAX_DL_LOGICAL_CHANNEL; index++)
  {
    ctrl_blk_id = dl_ctl_blk_id_tbl[index];
    if ((INIT_CTL_BLK_ID != ctrl_blk_id) && 
        ((DL_NULL_STATE != downlink_table.ctrl_blk[ctrl_blk_id].lc_state) && 
         (UE_MODE_ACKNOWLEDGED == downlink_table.ctrl_blk[ctrl_blk_id].lc_mode)))
    {
      act_dl_am_chnl++;
      break;
    }
  }

    if ((0 == act_dl_am_chnl) && (TRUE == rlc_dl_am_base_tmr_start))
    {
      /* Since all AM channels are released, stop base dl am timer */
      (void) rex_clr_timer(&rlc_dl_am_base_tmr);
      rlc_dl_am_base_tmr_start = FALSE;
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC_TMR_DBG: Stopping DL base timer, active_dl_chan %d",
                                                 downlink_table.active_ch);

    }

  L2_MUTEX_UNLOCK(&rlc_channel_release_mutex);
   L2_ACQ_DL_UNLOCK();

  rrc_put_rlc_cmd(rrc_rlc_cmd_ptr);

#ifdef FEATURE_RLC_QXDM_LOGGING
  if (j > 0)
  {
    dl_log_state.nchan = j;
    rlc_dl_send_state_log_packet(&dl_log_state);
  }
#endif

} /* crlc_dl_release_all_handler() */



/*===========================================================================

FUNCTION RLCI_DL_GET_CTL_BLK_PTR

DESCRIPTION
  Builds uplink PDUs for transmission in a TTI.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
rlci_dl_ctrl_blk_type *rlci_dl_get_ctl_blk_ptr
(
  uint8 ch_id
)
{
  uint8 ctl_blk_id;
  rlci_dl_ctrl_blk_type *ctl_blk_ptr = NULL;

  if (ch_id < UE_MAX_DL_LOGICAL_CHANNEL &&
      (ctl_blk_id = dl_ctl_blk_id_tbl[ch_id]) != INIT_CTL_BLK_ID)
  {
    ctl_blk_ptr = &downlink_table.ctrl_blk[ctl_blk_id];
  }

  return (ctl_blk_ptr);

} /* rlci_dl_get_ctl_blk_ptr() */

/*===========================================================================

FUNCTION RLCI_GET_RX_WIN_SIZE

DESCRIPTION
  Obtain the rx_win_size.

DEPENDENCIES
  None.

RETURN VALUE
  rx_win_size.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_get_rx_win_size (rlc_lc_id_type rlc_id)
{
  rlci_dl_ctrl_blk_type *ctl_blk_ptr;
  uint16 rx_win = 0;

  if ((ctl_blk_ptr = rlci_dl_get_ctl_blk_ptr(rlc_id)) != NULL)
  {
    rx_win = MIN(MAX_RX_WINDOW_SIZE,
      ctl_blk_ptr->ctrl.am_ctrl.dl_ctl_cnfg->rlc_info.rx_window_size);
  }

  return (rx_win);
} /* rlci_get_rx_win_size() */


/*===========================================================================

FUNCTION RLCI_GET_DL_SDU_BYTE_CNT

DESCRIPTION

  Obtains the number of data bytes received from the the network.

DEPENDENCIES
  None.

RETURN VALUE
  The byte count of received SDUs.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 rlci_get_dl_sdu_byte_cnt (rlc_lc_id_type rlc_id)
{
#ifdef FEATURE_RLC_QXDM_LOGGING
  rlci_dl_ctrl_blk_type *ctl_blk_ptr;

  if ((ctl_blk_ptr = rlci_dl_get_ctl_blk_ptr(rlc_id)) != NULL)
  {
    return (ctl_blk_ptr->ctrl.am_ctrl.dl_chan_am_stat.tot_num_sdu_byte_rxd);
  }
  else
  {
    return 0;
  }

#else
  return 0;
#endif

} /* rlci_get_dl_sdu_byte_cnt() */


#ifdef FEATURE_RLC_QXDM_LOGGING

/*===========================================================================

FUNCTION RLCI_DL_AM_HANDLE_QXDM_LOG_TIMER

DESCRIPTION

This function is called when the qxdm log timer expires. This functionthen
prepares to send the packet to qxdm for logging purposes. RLC_AM_LOG_TIMER_BASE
is currently set to 3seconds.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void rlci_dl_am_handle_qxdm_log_timer
(
  uint32 dl_log_timer_sig
  /*lint -esym(715,dl_log_timer_sig) */
)
{
  /* Set this log timer tick again */
  (void) rex_set_timer(&dl_tmr_log_blk, (uint32)RLC_AM_LOG_TIMER_BASE);

  /*---------------------------------------------------------------------
    Send the log packet to the qxdm for logging purposes every 3seconds.
  ---------------------------------------------------------------------*/
  if (log_status(WCDMA_DL_RLC_AM_STAT_LOG_PACKET))
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_MED, "Sending log packet");
    rlc_dl_tx_am_statistic_pkt(&downlink_table);
  }
} /* rlci_dl_am_handle_qxdm_log_timer() */

#endif // FEATURE_RLC_QXDM_LOGGING

/*===========================================================================

FUNCTION RLCI_GET_DL_HFN

DESCRIPTION
   This function is called by RLC-UL to get latest HFN's for all established
   RLC-DL UM and AM radio-bearers.For cipher pending logical channels a value
   of zero is retuned. RLC populates the * to struct rlc_hfn_req_type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  If HFN's for all UM and AM rb's are populated.
  FALSE:  If no UM and AM rb's are present.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_get_dl_hfn
(
  rlc_hfn_req_type* hfn_req             /*ptr to hfn request type          */
)
{
  uint16  count;                        /* Number of active Channels       */
  uint16  index = 0;
  uint8 array_len = 0;
  /*------------------------------------------------------------------------
    Get the number of active channels from the uplink table.
  ------------------------------------------------------------------------*/
  count = downlink_table.active_ch;
  hfn_req->dl_nchan = 0;
  array_len = sizeof(hfn_req->hfn_per_rb_dl)/sizeof(rlc_hfn_type);
  L2_ACQ_DL_LOCK(); 
  while((count > 0) && (index < UE_MAX_DL_RLC_ENTITY) && (hfn_req->dl_nchan < array_len))
  {
    if (downlink_table.ctrl_blk[index].lc_state != DL_NULL_STATE)
    {
      count--;
      if (downlink_table.ctrl_blk[index].lc_mode != UE_MODE_TRANSPARENT &&
          downlink_table.ctrl_blk[index].cipher.cipher_mode != NO_CIPHER)
      {
        if (downlink_table.ctrl_blk[index].lc_mode == UE_MODE_ACKNOWLEDGED)
        {
          /*-----------------------------------------------------------------
            Remove the LS 12 bits as of SN.
          -------------------------------------------------------------------*/
          if (downlink_table.ctrl_blk[index].cipher.cipher_mode == CIPHER_ON)
          {
            hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].hfn_valid = TRUE;
          }
          else
          {
            hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].hfn_valid = FALSE;
          }
          
          hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].hfn =
                      downlink_table.ctrl_blk[index].cipher.count >> 12;

          hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].rlc_sn =
            downlink_table.ctrl_blk[index].ctrl.am_ctrl.dl_state_var.vr_h;
          
          hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].rlc_id =
            downlink_table.ctrl_blk[index].ctrl.am_ctrl.am_common->dl_data_id;
        }
        else
        {
          /*----------------------------------------------------------------
            Remove the LS 7 bits as of SN.
          ------------------------------------------------------------------*/
          if (downlink_table.ctrl_blk[index].cipher.cipher_mode == CIPHER_ON)
          {
            hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].hfn_valid = TRUE;
          }
          else
          {
            hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].hfn_valid = FALSE;
          }

          hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].hfn  =
                    downlink_table.ctrl_blk[index].cipher.count >> 7;

          hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].rlc_sn =
                          downlink_table.ctrl_blk[index].ctrl.um_ctrl.vr_ur;

          hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].rlc_id =
                          downlink_table.ctrl_blk[index].ctrl.um_ctrl.rlc_id;
        }

        MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC_DBG: RRC req DL Cipher LC %d, rlc_sn %d hfn 0x%x",
                          hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].rlc_id, 
                          hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].rlc_sn,
                          hfn_req->hfn_per_rb_dl[hfn_req->dl_nchan].hfn);

        hfn_req->dl_nchan ++;
      }

    }
    index++;
  }
  L2_ACQ_DL_UNLOCK();

  return TRUE;

} /* rlci_get_dl_hfn() */

/*===========================================================================

FUNCTION RLCI_GET_DL_COUNT_C

DESCRIPTION
   This function is called by RLC UL to get latest COUNT_C's for all established
   RLC UM and AM radio-bearers.  RLC populates the * to struct
   rlc_count_c_req_type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  If COUNT_C's for all UM and AM rb's are populated.
  FALSE:  If no UM and AM rb's are present.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlci_get_dl_count_c
(
  rlc_count_c_req_type* count_c_req             /*ptr to hfn request type          */
)
{
  uint16  count;                        /* Number of active Channels       */
  uint16  index = 0;

  /*------------------------------------------------------------------------
    Get the number of active channels from the uplink table.
  ------------------------------------------------------------------------*/
  count = downlink_table.active_ch;
  count_c_req->dl_nchan = 0;

  while(count > 0 && index < UE_MAX_DL_RLC_ENTITY)
  {
    if (downlink_table.ctrl_blk[index].lc_state != DL_NULL_STATE)
    {
      count--;
      if ((downlink_table.ctrl_blk[index].lc_mode != UE_MODE_TRANSPARENT) &&
          (downlink_table.ctrl_blk[index].cipher.cipher_mode != NO_CIPHER) &&
          (count_c_req->dl_nchan < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY)))
      {
        if (downlink_table.ctrl_blk[index].lc_mode == UE_MODE_UNACKNOWLEDGED)
        {
          count_c_req->count_c_per_rb_dl[count_c_req->dl_nchan].count_c =
            downlink_table.ctrl_blk[index].cipher.count;
          count_c_req->count_c_per_rb_dl[count_c_req->dl_nchan].rlc_id =
            downlink_table.ctrl_blk[index].ctrl.um_ctrl.rlc_id;
        }
        else
        {
          count_c_req->count_c_per_rb_dl[count_c_req->dl_nchan].count_c =
            downlink_table.ctrl_blk[index].cipher.count;
          count_c_req->count_c_per_rb_dl[count_c_req->dl_nchan].rlc_id =
            downlink_table.ctrl_blk[index].ctrl.am_ctrl.am_common->dl_data_id;
        }
        count_c_req->dl_nchan ++;
      }
    }
    index++;
  }

  return TRUE;

} /* rlci_get_dl_count_c() */

/*===========================================================================

FUNCTION  RLC_DL_AM_BACKUP_TIMER

DESCRIPTION
  This function check the backup timer count. If it is greater than the
  hard-coded value (5). Start the 10ms timer for the AM channels.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_am_backup_timer(void)
{
  uint16  i = 0;
  uint16  active_ch;

  active_ch = downlink_table.active_ch;
  //MSG_HIGH("backup_timer %d", active_ch, 0, 0);
  while(active_ch > 0 && i < UE_MAX_DL_RLC_ENTITY)
  {
    if (downlink_table.ctrl_blk[i].lc_state != DL_NULL_STATE)
    {
      /* Ok, this is a AM channel.  */
      if (downlink_table.ctrl_blk[i].lc_mode == UE_MODE_ACKNOWLEDGED &&
          (downlink_table.ctrl_blk[i].lc_state == DL_DATA_TRANSFER_READY ||
           downlink_table.ctrl_blk[i].lc_state == DL_RESET_PENDING))
      {
        /*---------------------------------------------------------------------
          Do not resume the timer before it actually starts.
        ----------------------------------------------------------------------*/
        if(!downlink_table.ctrl_blk[i].ctrl.am_ctrl.dl_state_var.fst_rx_pdu &&
           ++downlink_table.ctrl_blk[i].ctrl.am_ctrl.
           dl_state_var.backup_timer_cnt > RLC_BACKUP_TIMER_CNT)
        {
          downlink_table.ctrl_blk[i].ctrl.am_ctrl.dl_state_var.backup_timer_cnt = 0;

          downlink_table.ctrl_blk[i].ctrl.am_ctrl.dl_tmr_sts = RLC_AM_TIMER_SUSPEND;
          MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "Set timer from backup, LC %d",
                                                    downlink_table.ctrl_blk[i].ctrl.am_ctrl.am_common->dl_data_id);
        }
      }
      active_ch --;
    }
    i++;
  }
} /* rlc_dl_am_backup_timer() */

/*===========================================================================

FUNCTION  RLCI_SUSPEND_DL_AM_TIMER

DESCRIPTION
  It suspends the DL AM 20ms timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_suspend_dl_am_timer
(
  byte lc_id
)
{

  L2_ACQ_DL_LOCK();
  downlink_table.ctrl_blk[dl_ctl_blk_id_tbl[lc_id]].ctrl.am_ctrl.dl_tmr_sts = RLC_AM_TIMER_STOP;
  L2_ACQ_DL_UNLOCK();

} /* rlci_suspend_dl_am_timer() */


/*===========================================================================

FUNCTION  RLCI_RESUME_DL_AM_TIMER

DESCRIPTION
  It resumes the DL AM 20ms timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_resume_dl_am_timer
(
 byte lc_id
)
{

  L2_ACQ_DL_LOCK();
  downlink_table.ctrl_blk[dl_ctl_blk_id_tbl[lc_id]].ctrl.am_ctrl.dl_tmr_sts = RLC_AM_TIMER_RUNNING;
  L2_ACQ_DL_UNLOCK();
} /* rlci_resume_dl_am_timer() */

/*===========================================================================

FUNCTION RLC_PROCESS_MAC_HS_RESET_REQ

DESCRIPTION
  This function handles the MAC HS Reset command from MAC. RLC sends
  a status report on receiving this command even if any status prohibit
  timer is on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_process_mac_hs_reset_req
(
  mac_hs_dl_reset_req_type  *mac_hs_reset_req
)
{

  rlci_dl_ctrl_blk_type *dl_ctl_blk = NULL;
  uint8 i =0;

  /*-------------------------------------------------------------------------
    Send a status report on all AM logical channels which are in DL_DATA_TRANSFER_READY
    state, that have been indicated in the MAC HS reset command.
  -------------------------------------------------------------------------*/
  L2_ACQ_DL_LOCK();

  for(i=0; i<mac_hs_reset_req->nchan; i++)
  {
    dl_ctl_blk = rlci_dl_get_ctl_blk_ptr(mac_hs_reset_req->rlc_id_list[i]);
    /*lint -save -e655 */
    if((dl_ctl_blk == NULL) ||
       (dl_ctl_blk->lc_mode < UE_MODE_ACKNOWLEDGED) ||
       (dl_ctl_blk->lc_state != DL_DATA_TRANSFER_READY))
    {
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "No status report after HS reset, LC:%d",
                                                  mac_hs_reset_req->rlc_id_list[i]);
      continue;
    }
    /*lint -restore */

    rlci_rx_status_changed(dl_ctl_blk->ctrl.am_ctrl.am_common->ul_control_id,
                         FALSE);
    dl_ctl_blk->ctrl.am_ctrl.dl_status_opt_bitmap.next_sn_valid = FALSE;
    rlci_set_ul_num_sn_li(dl_ctl_blk->ctrl.am_ctrl.am_common->ul_data_id,
            dl_ctl_blk->ctrl.am_ctrl.num_sn_li);
    rlci_rx_status_changed(dl_ctl_blk->ctrl.am_ctrl.am_common->ul_control_id,
                           TRUE);

    /*---------------------------------------------------------------------
      If Status prohibit timer is active for the channel, re-start the
      timer.
    ---------------------------------------------------------------------*/
    if(dl_ctl_blk->ctrl.am_ctrl.dl_state_var.status_prohibit)
    {
      dl_ctl_blk->ctrl.am_ctrl.dl_state_var.status_prohibit_cnt = 0;
    }
  }
  
  L2_ACQ_DL_UNLOCK();
}

/*===========================================================================

FUNCTION CRLC_DL_ABORT_CIPHER_CONFIG

DESCRIPTION
  This function reverts back to the old ciphering configuration if an abort
  is received from RRC while applying the new configuration due to triggering
  of cell update in RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_abort_cipher_config
(
  rlc_abort_cipher_config_type *abort_cipher_info

)
{
  int i = 0;
  uint16 index = 0;                         /* Index to the control block  */
  rlci_dl_ctrl_blk_type *ctl_blk = NULL;    /* Pointer to the control block*/

  L2_ACQ_DL_LOCK();

  /*-------------------------------------------------------------------------
    For all the logical channels specified, revert back to the old ciphering
    configuration.
  -------------------------------------------------------------------------*/
  for(i=0;i<abort_cipher_info->nchan;i++)
  {
    /*-----------------------------------------------------------------------
      Get the index to the control block from the logical channel id
      and the control block.
    -----------------------------------------------------------------------*/
    index = dl_ctl_blk_id_tbl[abort_cipher_info->lc_id[i]];
    ctl_blk = &downlink_table.ctrl_blk[index];

    MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Aborting DL Cipher Config for RbId: %d,LC: %d",
                                               ctl_blk->cipher.rb_id,
                                               abort_cipher_info->lc_id[i]);

    /*-----------------------------------------------------------------------
      Revert to the old ciphering configuration.
    -----------------------------------------------------------------------*/
    ctl_blk->cipher = ctl_blk->cipher_backup;
  }

  L2_ACQ_DL_UNLOCK();

}

/*===========================================================================

FUNCTION RLCI_GET_DL_LATEST_PENDING_HFN

DESCRIPTION
   This function is called by RRC to get latest pending HFN for indicated
   logical channel ID via RLC UL.
   RLC DL populates the info to the struct rlc_pending_hfn_req_type.
   If there is a pending configuration for UL alone then
   dl_pending_hfn_present flag will be set to FALSE and vice-versa.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE:  If either dl_pending_hfn_present or ul_pending_hfn_present
                    flag is set to TRUE then function will return SUCCESS.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_get_dl_latest_pending_hfn
(
  rlc_lc_id_type rlc_id,                /* queried RLC logical channel ID */
  rlc_pending_hfn_req_type *pending_hfn
)
{
  rlci_dl_ctrl_blk_type *dl_ctrl_blk = NULL;
  uint16 ctl_blk_id;

  if (rlc_id < UE_MAX_DL_LOGICAL_CHANNEL &&
      (ctl_blk_id = (uint16)dl_ctl_blk_id_tbl[rlc_id]) != INIT_CTL_BLK_ID)
  {
    dl_ctrl_blk = &downlink_table.ctrl_blk[ctl_blk_id];

    if (dl_ctrl_blk->cipher.cipher_mode == CIPHER_WAIT ||
        dl_ctrl_blk->cipher.cipher_mode == CIPHER_MOD)
    {
      pending_hfn->dl_pending_hfn = (dl_ctrl_blk->cipher.two_pending) ?
        dl_ctrl_blk->cipher.hfn_new1 : dl_ctrl_blk->cipher.hfn_new;
      pending_hfn->dl_pending_hfn_present = TRUE;
    }
  }
  else
  {
    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "Invalid RLC DL LC, %d",
                                               rlc_id);
  }
}

/*===========================================================================

FUNCTION RLCI_DL_OVERWRITE_HFN

DESCRIPTION
   This function is called by RRC to overwrite the HFN of RLC currently using
   thru UL.

   This function will be called after RRC re-establish RLC to overwrite the
   new HFN that RRC just comes up before the traffic can be resumed.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE:  Done successful / no such LC or no ciphering at all for
                    this LC.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rlci_dl_overwrite_hfn
(
  rlc_lc_id_type rlc_id,                /* queried RLC logical channel ID */
  uint32 hfn
)
{
  rlci_dl_ctrl_blk_type *dl_ctrl_blk = NULL;
  uint16 ctl_blk_id;
  uecomdef_status_e_type dl_ret = FAILURE;

  L2_ACQ_DL_LOCK();
  
  /* Perform DL AM overwrite */
  if (rlc_id < UE_MAX_DL_LOGICAL_CHANNEL &&
      (ctl_blk_id = (uint16)dl_ctl_blk_id_tbl[rlc_id]) != INIT_CTL_BLK_ID)
  {
    dl_ctrl_blk = &downlink_table.ctrl_blk[ctl_blk_id];

    if (dl_ctrl_blk->lc_mode == UE_MODE_ACKNOWLEDGED &&
        dl_ctrl_blk->cipher.cipher_mode == CIPHER_ON)
    {
      dl_ctrl_blk->cipher.hfn = hfn;
      dl_ctrl_blk->cipher.count = hfn << 12;
      dl_ret = SUCCESS;
    }
  }

  L2_ACQ_DL_UNLOCK();
  
  return (dl_ret);
}

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION RLC_DL_LOG_CIPHER_PKT

DESCRIPTION
This function logs the dowlink PDU ciphering parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_log_cipher_pkt
(
  void
)
{
  if(rlci_dl_pdu_cipher_log.num_pdu)
  {
    /*-----------------------------------------------------------------------
      Lock the cipher pdu log info updation by setting the flag.
    -----------------------------------------------------------------------*/
    rlci_dl_pdu_cipher_log.cipher_pdu_logging = TRUE;

    /*-----------------------------------------------------------------------
      The logging data is filled in at the time before a PDU is decoded.
    -----------------------------------------------------------------------*/
    rlc_dl_tx_pdu_cipher_log_pkt(&rlci_dl_pdu_cipher_log);

    /*-----------------------------------------------------------------------
      Reset num_pdu after sending logging data to DIAG.
    -----------------------------------------------------------------------*/
    rlci_dl_pdu_cipher_log.num_pdu = 0;

    /*-----------------------------------------------------------------------
      Unlock cipher pdu log info updation.
    -----------------------------------------------------------------------*/
    rlci_dl_pdu_cipher_log.cipher_pdu_logging = FALSE;
  }
} /* rlc_dl_process_log_cipher_pkt() */

/*===========================================================================

FUNCTION RLC_DL_SET_CIPHER_LOGGING

DESCRIPTION
This function sets signal to log downlink cipher PDU packets in task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rlc_dl_set_cipher_logging
(
  void
)
{
  if(rlci_dl_pdu_cipher_log.num_pdu)
  {
    /*-----------------------------------------------------------------------
      Set the RLC_DL_LOG_CIPHER_PKT_SIG rex signal, if ciher PDUs 
      needs to be logged
    -----------------------------------------------------------------------*/
    (void)rex_set_sigs(wcdma_l2_dl_get_tcb(), RLC_DL_LOG_CIPHER_PKT_SIG);
  }
} /* rlc_dl_set_cipher_logging() */

#endif /* FEATURE_RLC_QXDM_LOGGING */

/*===========================================================================

FUNCTION RLCI_DL_CHANGE_STATE

DESCRIPTION
   This function will change the downlink RLC from DL_RESET_PENDING state to
   its previous state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_change_state
(
  rlc_lc_id_type lc_id
)
{
  /*lint -e64 -e641 */
  (downlink_table.ctrl_blk[dl_ctl_blk_id_tbl[lc_id]].lc_state) &= (~DL_RESET_PENDING);
  /*lint +e64 +e641 */
}

/*===========================================================================

FUNCTION CRLC_DL_WM_CLEANUP_CMD_HANDLER

DESCRIPTION
   this function cleanup the PDCP-RLC WM by a call back function.

DEPENDENCIES
  None.

RETURN VALUE
 none

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_wm_clean_up_cmd_handler (rlci_cleanup_wm_cmd_type *cleanup_wm_cmd)
{
  uint16 ctl_blk_id;

  if (cleanup_wm_cmd->lc_id < UE_MAX_DL_LOGICAL_CHANNEL)
  {
    if ((ctl_blk_id = (uint16)dl_ctl_blk_id_tbl[cleanup_wm_cmd->lc_id]) != INIT_CTL_BLK_ID)
    {
      if(downlink_table.ctrl_blk[ctl_blk_id].dl_wm_ptr == NULL)
      {
        cleanup_wm_cmd->cleanup_wm_cb(cleanup_wm_cmd->cleanup_wm_para);
      }
    }
    else
    {
      cleanup_wm_cmd->cleanup_wm_cb(cleanup_wm_cmd->cleanup_wm_para);
    }
  }
}
/*===========================================================================

FUNCTION CRLC_DL_PERFORM_STOP_FUNCTIONALITY

DESCRIPTION

   Stops the DL reception on one or more logical channels.

DEPENDENCIES
  None.

RETURN VALUE
 none

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_perform_stop_functionality
(
  rlc_dl_stop_request_type *rrcsmc_dl_stop_req
)
{

  uint8   index, lc_id, i;

  L2_ACQ_DL_LOCK();

  if(rrcsmc_dl_stop_req->nchan < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY))
  {
    for (i = 0; i < rrcsmc_dl_stop_req->nchan; i++)
    {
      /*-----------------------------------------------------------------------
        Get the index of the downlink table.
      -----------------------------------------------------------------------*/
      lc_id = rrcsmc_dl_stop_req->lc_id[i];
      index = dl_ctl_blk_id_tbl[lc_id];
      
      if (index != INIT_CTL_BLK_ID)
      {
        /*lint -e64 -e641 -e655 */
        downlink_table.ctrl_blk[index].lc_state |= DL_STOP;
        downlink_table.ctrl_blk[index].lc_state &= (~DL_DATA_TRANSFER_READY);
        /*lint +e64 +e641 +e655 */
        
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL  LC %d - Stop", lc_id);
      }
      else
      {
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "DL LC %d not config'd, No Stop",
                                                    lc_id);
      }
    }
  }
  else
  {
    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: Num Chan %d provided by RRC out of bounds",
                                                rrcsmc_dl_stop_req->nchan);
  }
  
  L2_ACQ_DL_UNLOCK();
  
} /* crlc_dl_perform_stop_functionality */

/*===========================================================================

FUNCTION CRLC_DL_CONTINUE_HANDLER

DESCRIPTION
  Continue transmission on one or more downlink unacknowledged or acknowledged
  mode channels.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void crlc_dl_continue_handler
(
  rlc_dl_continue_type *continue_ptr      /* Ptr to continue Info                */
)
{
  uint8 index, lc_id ,i;

  L2_ACQ_DL_LOCK();
  
  for (i = 0; i < continue_ptr->nchan; i++)
  {
    lc_id = continue_ptr->channel[i];
    index = dl_ctl_blk_id_tbl[lc_id];

    /*lint -save -e64 -e641 -e655 */
    if (index != INIT_CTL_BLK_ID &&
        (downlink_table.ctrl_blk[index].lc_state & DL_STOP))
    /*lint -restore */
    {
      /*lint -save -e64 -e641 -e655 */
      downlink_table.ctrl_blk[index].lc_state &= (~DL_STOP);
      downlink_table.ctrl_blk[index].lc_state |= DL_DATA_TRANSFER_READY;
      /*lint -restore */
      
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL LC %d Continue", lc_id);
    }
    else
    {
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL LC %d not in Stop State",
                                                 lc_id);
    }
  }
  
  L2_ACQ_DL_UNLOCK();
  
}/* crlc_dl_continue_handler() */

#ifdef FEATURE_RLC_QXDM_LOGGING

#define RLC_DEBUG_PDU_DUMP_WORDS 500
#define RLC_DEBUG_PDU_DUMP_BYTES 128

boolean rlc_debug_pdu_dump = FALSE;
uint32 rlc_debug_pdu_dump_array[RLC_DEBUG_PDU_DUMP_WORDS];
uint32 rlc_debug_pdu_size_bits;

void rlc_debug_print_data(uint32 num_valid_words, uint32 *debug_array)
{
  uint32 log_offset;
  
  log_offset = 0;
  while (num_valid_words > 0)
  {
    MSG_8(MSG_SSID_DFLT, MSG_LEGACY_FATAL, 
          "RLC_PDU_DUMP: 0x%4x 0x%4x 0x%4x 0x%4x 0x%4x 0x%4x 0x%4x 0x%4x", 
          debug_array[log_offset+0], debug_array[log_offset+1], debug_array[log_offset+2], 
          debug_array[log_offset+3], debug_array[log_offset+4], debug_array[log_offset+5], 
          debug_array[log_offset+6], debug_array[log_offset+7]);
    
    /* Loop through logging, if something still pending to be printed */
    num_valid_words = ((num_valid_words > 8) ? (num_valid_words - 8) : 0);
    log_offset += 8;
  }
}

void rlc_enh_logging_data_dump(rlci_dl_ctrl_blk_type *ctrl_blk_ptr, complete_pdu_type *complete_pdu_ptr)
{
  uint8 temp_data[4], rlc_debug_pdu_bytes[RLC_DEBUG_PDU_DUMP_BYTES];
  uint16 pdu_size_in_bits;
  uint32 temp_word, pdu_offset, array_offset, num_words, num_bytes, bytes_read, bytes_to_read, bytes_written;
  uint16 dcbit;

  temp_word = pdu_size_in_bits = 0;
  if (ctrl_blk_ptr->lc_mode == UE_MODE_ACKNOWLEDGED)
  {
    dcbit = RLCI_GET_DC_BIT(complete_pdu_ptr->rlc_hdr_data);
    if(dcbit == 0)
    {
      pdu_size_in_bits = (((uint16)complete_pdu_ptr->iovec.length) << 3);
      (void)dsm_extract(complete_pdu_ptr->iovec.ptr, 
                      complete_pdu_ptr->iovec.offset, temp_data, 4);
      temp_word = rlc_copy_to_word(temp_data);
      pdu_offset = 4;
    }
    else
    {
      pdu_size_in_bits = (((uint16) (complete_pdu_ptr->iovec.length + 
                                     RLC_AM_PDU_HDR_SIZE_BYTES)) << 3);
      (void)dsm_extract(complete_pdu_ptr->iovec.ptr, 
                      complete_pdu_ptr->iovec.offset, temp_data, 2);
      
      temp_word = (uint32)(((complete_pdu_ptr->rlc_hdr_data & 0xFF) << 8) | 
                           ((complete_pdu_ptr->rlc_hdr_data & 0xFF00) >> 8) |
                           ((uint32)temp_data[0] << 16) | 
                           ((uint32)temp_data[1] << 24));
      pdu_offset = 2;
    }
  }
  else
  {
#ifdef FEATURE_UM_PDU_LOGGING
    if (ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED)
    {
      pdu_size_in_bits = (uint16) ((complete_pdu_ptr->iovec.length + 
                                    RLC_UM_PDU_HDR_SIZE_BYTES) << 3);
      if ((dsm_extract(complete_pdu_ptr->iovec.ptr, 
                       complete_pdu_ptr->iovec.offset, temp_data, 3)) > 0)
      {
        temp_word = (uint32) (((complete_pdu_ptr->rlc_hdr_data >> 8)& 0xFF)| 
                              ((uint32)temp_data[0] << 8) | 
                              ((uint32)temp_data[1] << 16) | 
                              ((uint32)temp_data[2] << 24));
      }
      pdu_offset = 3;
    }
#endif
  }
  
  array_offset = bytes_read = bytes_to_read = bytes_written = 0;
  
  memset(rlc_debug_pdu_dump_array, 0, (sizeof(uint32) * RLC_DEBUG_PDU_DUMP_WORDS));
  rlc_debug_pdu_size_bits = pdu_size_in_bits;
  num_words = CONVERT_BITS_TO_WORDS(pdu_size_in_bits);
  num_bytes = CONVERT_BITS_TO_BYTES(pdu_size_in_bits);
  
  /* Store the first word */
  rlc_debug_pdu_dump_array[array_offset] = temp_word;
  array_offset++;
  
  /* Read all the subsequent bytes into array of small memory and 
   * then copy to global word memory for dumping */
  while (bytes_read < num_bytes)
  {
    bytes_to_read = RLC_MIN_OF_THE_TWO(RLC_DEBUG_PDU_DUMP_BYTES, (num_bytes - bytes_read));
    if ((dsm_extract(complete_pdu_ptr->iovec.ptr, pdu_offset, 
                     rlc_debug_pdu_bytes, bytes_to_read)) > 0)
    {
      bytes_written = 0;
      while (bytes_written < bytes_to_read)
      {
        rlc_debug_pdu_dump_array[array_offset] = rlc_copy_to_word(rlc_debug_pdu_bytes + bytes_written);
        array_offset++;
        bytes_written += 4;
      }
    }
    bytes_read += bytes_to_read;
  }
  MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC_PDU_DUMP: rlc_id %d - PDU Size %d bits",
                                             complete_pdu_ptr->rlc_id,
                                             pdu_size_in_bits);
  
  rlc_debug_print_data(num_words, rlc_debug_pdu_dump_array);
}

/*===========================================================================

FUNCTION rlc_enh_logging_data_handler

DESCRIPTION
    - Logging functionality called for all the PDUs which belongs to given LC Id     
      If first_pdu_on_this_lch is TRUE, initialise the num_pdu to zero

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_logging_data_handler(rlci_dl_ctrl_blk_type *ctrl_blk_ptr, 
                                  complete_pdu_type *complete_pdu_ptr,
                                  boolean first_pdu_on_this_lch)
{
  uint8 temp_data[4];
  uint16 pdu_size_in_bits;
  uint32 temp_word;
  rlc_dl_chan_am_pdu_type  *dl_chan_am_pdu_ptr;
  uint16 dcbit;
#ifdef FEATURE_UM_PDU_LOGGING
  rlc_dl_chan_um_pdu_type  *dl_chan_um_pdu_ptr;
#endif
  
  if (ctrl_blk_ptr->lc_mode == UE_MODE_ACKNOWLEDGED)
  {
    dl_chan_am_pdu_ptr = &(ctrl_blk_ptr->ctrl.am_ctrl.dl_chan_am_pdu);
    
    if (first_pdu_on_this_lch == TRUE)
    {
      dl_chan_am_pdu_ptr->num_pdu = 0;
    }
    
    if (dl_chan_am_pdu_ptr->num_pdu >= MAX_DL_RLC_LOG_PDUS)
    {
      return;
    }

    if (rlc_debug_var & RLC_DBG_DL_PDU_DUMP)
    {
      uint32 nwords =  (complete_pdu_ptr->iovec.length /4) + 1;
      uint32 idx;

      for (idx = 0; idx < nwords; idx++)
      {
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "DL PDU: 0x%x",
                                                    rlc_copy_to_word(complete_pdu_ptr->iovec.ptr->data_ptr+complete_pdu_ptr->iovec.offset+4*idx));
      } 
    }
        
    /*-----------------------------------------------------------
      Store the number of pdus, pdu size, 4bytes of the pdu,
      data id, control id in dl_am_pdu_log for purpose of qcat
      logging.
    -----------------------------------------------------------*/
    dcbit = RLCI_GET_DC_BIT(complete_pdu_ptr->rlc_hdr_data);
    if(dcbit == 0)
    {
      pdu_size_in_bits = (((uint16)complete_pdu_ptr->iovec.length) << 3);
      (void)dsm_extract(complete_pdu_ptr->iovec.ptr, 
                      complete_pdu_ptr->iovec.offset, temp_data, 4);
      temp_word = rlc_copy_to_word(temp_data);

      if(rlc_debug_var & RLC_DBG_DL_CTRL)
      {
        MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: NA: rlc_id %d, rlc_hdr_data 0x%x, temp_word 0x%x",
                 complete_pdu_ptr->rlc_id,
                 complete_pdu_ptr->rlc_hdr_data, 
                 temp_word);
      }
    }
    else
    {
      pdu_size_in_bits = (((uint16) (complete_pdu_ptr->iovec.length + 
                                     RLC_AM_PDU_HDR_SIZE_BYTES)) << 3);
      (void)dsm_extract(complete_pdu_ptr->iovec.ptr, 
                      complete_pdu_ptr->iovec.offset, temp_data, 2);
      
      temp_word = (uint32)(((complete_pdu_ptr->rlc_hdr_data & 0xFF) << 8) | 
                           ((complete_pdu_ptr->rlc_hdr_data & 0xFF00) >> 8) |
                           ((uint32)temp_data[0] << 16) | 
                           ((uint32)temp_data[1] << 24));
    }
    
#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
    dl_chan_am_pdu_ptr->pdu_size[dl_chan_am_pdu_ptr->num_pdu] = pdu_size_in_bits;
#else
    dl_chan_am_pdu_ptr->pdu_size = pdu_size_in_bits;
#endif
    dl_chan_am_pdu_ptr->log_data[dl_chan_am_pdu_ptr->num_pdu] = temp_word;
    dl_chan_am_pdu_ptr->num_pdu++;
  }
  else
  {
#ifdef FEATURE_UM_PDU_LOGGING
    if (ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED)
    {
      dl_chan_um_pdu_ptr = &(ctrl_blk_ptr->ctrl.um_ctrl.dl_chan_um_pdu);
      
      if (first_pdu_on_this_lch == TRUE)
      {
        dl_chan_um_pdu_ptr->num_pdu = 0;
      }
      
      if (dl_chan_um_pdu_ptr->num_pdu >= MAX_DL_RLC_LOG_PDUS)
      {
        return;
      }      
      dl_chan_um_pdu_ptr->pdu_size = (((uint16) (complete_pdu_ptr->iovec.length + 
                                               RLC_UM_PDU_HDR_SIZE_BYTES)) << 3);
      if ((dsm_extract(complete_pdu_ptr->iovec.ptr, 
                       complete_pdu_ptr->iovec.offset, temp_data, 3)) > 0
         )
      {
        temp_word = (uint32) (((complete_pdu_ptr->rlc_hdr_data >> 8)& 0xFF)| 
                              ((uint32)temp_data[0] << 8) | 
                              ((uint32)temp_data[1] << 16) | 
                              ((uint32)temp_data[2] << 24));
        dl_chan_um_pdu_ptr->log_data[dl_chan_um_pdu_ptr->num_pdu] = temp_word;
        dl_chan_um_pdu_ptr->num_pdu++;
      }
    }
#endif
  }

  if (rlc_debug_pdu_dump)
  {
   rlc_enh_logging_data_dump(ctrl_blk_ptr, complete_pdu_ptr);
  }
}



/*===========================================================================

FUNCTION rlc_enh_logging_post_data_handler

DESCRIPTION
    - Logging functionality called for all the PDUs which belongs to given LC Id     

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_logging_post_data_handler(rlci_dl_ctrl_blk_type *ctrl_blk_ptr, 
                                       uint8 lc_id, boolean hs_path)
{
  if ((ctrl_blk_ptr->lc_mode == UE_MODE_ACKNOWLEDGED) && 
      (ctrl_blk_ptr->ctrl.am_ctrl.dl_chan_am_pdu.num_pdu > 0))
  {
    /* Log 0x413B - Non EHS Calls, EHS Call SRB */
#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
    if ((hs_path == FALSE) || 
        (mac_ehs_query_hs_type() != MAC_EHS) || 
        (lc_id < RLC_DL_AM_USR_PLANE_START_LC_ID))
#endif
    {
      rlc_dl_enh_tx_am_pdu_log_pkt(ctrl_blk_ptr, lc_id);
    }
  }
  else
  {
#ifdef FEATURE_UM_PDU_LOGGING
    if ((ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED) &&
        (ctrl_blk_ptr->ctrl.um_ctrl.dl_chan_um_pdu.num_pdu > 0))
    {
      rlc_dl_enh_tx_um_pdu_log_pkt(ctrl_blk_ptr);
    }
#endif
  }
}

#endif /*FEATURE_RLC_QXDM_LOGGING */

/*===========================================================================

FUNCTION rlci_discard_sdu_ptr

DESCRIPTION
    For every DSM Buffer/item in the packet chain, discard the buffer/item as 
    per conditions below
    If the DSM Buffer/item app_field is zero, 
       - DISCARD the DSM Buffer/item by calling dsmi_free_buffer
    Otherwise
       - Nothing

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_discard_sdu_ptr
(
 dsm_item_type **dsm_ptr,
 const char * file,
 uint32 line
)
{
  dsm_item_type *ptr = *dsm_ptr;
  
  while (ptr != NULL)
  {
    if (ptr->app_field == 0)
    {
      if (rlc_debug_var & RLC_DBG_REASSEMBLY_1)
      {
        MSG_9(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH,
              "RLC DBG DL: rlc_sdu_discard: dsm_ptr 0x%x, pkt_ptr 0x%x, data_ptr 0x%x, ref_cnt %d, app_field 0x%x, dup_ptr 0x%x, used 0x%x size 0x%x",
               ptr, ptr->pkt_ptr, ptr->data_ptr, ptr->references, ptr->app_field, ptr->dup_ptr, ptr->used, ptr->size, 0);
      }
      /* dsmi_free_buffer returns the next item in the chain */
      ptr = dsmi_free_buffer(ptr, file, line);
    }
    else
    {
      ptr = ptr->pkt_ptr;
    }
  }
}

/*===========================================================================

FUNCTION rlc_discard_dsm_item

DESCRIPTION
    Discard the DSM Item as follows
    If the app_field is greater than zero, decrement it.
    If the resulting app_field is zero, 
       - DISCARD the DSM item by calling dsm_free_packet
    Otherwise
       - Nothing

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlci_discard_dsm_item
(
 dsm_item_type **dsm_ptr,
 const char * file,
 uint32 line
)
{
  dsm_item_type *ptr;
  ptr = *dsm_ptr;

  L2_MUTEX_LOCK(&rlc_mac_dl_pdu_mutex);

  if (ptr != NULL)
  {
    if (ptr->app_field > 0)
    {
      ptr->app_field--; 
      if (ptr->app_field == 0)
      {
        if(rlc_debug_var & RLC_DBG_MEM_LEAK)
        {
          dsmi_touch_item(*dsm_ptr, rlc_exit, __LINE__);
        }
        if (rlc_debug_var & RLC_DBG_REASSEMBLY_1)
        {
          MSG_9(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH,
                "RLC DBG DL: rlc_discard: dsm_ptr 0x%x, pkt_ptr 0x%x, data_ptr 0x%x, ref_cnt %d, app_field 0x%x, dup_ptr 0x%x, used 0x%x size 0x%x",
                 ptr, ptr->pkt_ptr, ptr->data_ptr, ptr->references, ptr->app_field, ptr->dup_ptr, ptr->used, ptr->size, 0);
        }
        dsmi_free_packet(dsm_ptr, file, line);
      }
    }
    else
    {
      ERR_FATAL("RLC_ERR: rlc_discard_dsm_item: app_field is already zero", 
                 0, 0, 0);
    }
  }
  else
  {
    ERR_FATAL("RLC_ERR: rlc_discard_dsm_item: dsm_ptr is NULL ptr", 
               0, 0, 0);
  }

  L2_MUTEX_UNLOCK(&rlc_mac_dl_pdu_mutex);

}

/*===========================================================================

FUNCTION rlc_discard_pdu

DESCRIPTION
    Discard the PDU if not NULL

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_discard_pdu
(
  complete_pdu_type       *complete_pdu_ptr
)
{

  if (NULL != complete_pdu_ptr->iovec.ptr)
  {
    rlc_discard_dsm_item(&(complete_pdu_ptr->iovec.ptr));
    complete_pdu_ptr->iovec.ptr = NULL;
  }
  else
  {
    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: NULL iovec ptr for LC %d",
                                                complete_pdu_ptr->rlc_id);
  }
  
}

/*===========================================================================

FUNCTION rlc_enh_calc_rlc_hdr_length

DESCRIPTION
    - Return the RLC header length, which is 
        2 for AM Data PDU, 0 for AM Ctrl PDU, 1 for UM and 0 for TM     

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 rlc_enh_calc_rlc_hdr_length(rlc_lc_id_type rlc_id, uint8 first_byte, boolean *is_tm_mode)
{
  uint8 length = 0;
  rlci_dl_ctrl_blk_type           *ctrl_blk_ptr;

  ctrl_blk_ptr = RLCI_GET_CTRL_BLK_PTR(RLCI_GET_CTRL_INDEX(rlc_id));
  if (ctrl_blk_ptr != NULL)
  {
    if (ctrl_blk_ptr->lc_mode == UE_MODE_ACKNOWLEDGED)
    {
      if (first_byte & 0x80)
      {
        length = 2;
      }
    }
    else 
    {
      if ((ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED) 
          #ifdef FEATURE_UMTS_BMC
           || (ctrl_blk_ptr->lc_mode == UE_MODE_DL_CTCH)
          #endif 
          )
           
      {
        length = 1;
      }
      else
      {
        *is_tm_mode = TRUE;
      }
    }
  }
  else
  {
    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: No Control Blk exists for this rlc_id %d",
                                                rlc_id);
  }
  
  return (length);
}
/*===========================================================================

FUNCTION rlc_update_nak_status

DESCRIPTION
    - This function sets/resets the nak status for a range of pdus with sn 
      starting with (fsn) and ending with sn (lsn) (fsn and lsn inclusive).   

DEPENDENCIES
  None.

INPUT ARGUMENTS
  nak_bitmap - pointer to bitmap array
  fsn        - fist sequence number for which status is to be updated.
  lsn        - last sequence number for which status is to be updated.
  update     - pdu status 
               FALSE - ACK
               TRUE  - NAK
  mod        - modulus to use..

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_update_nak_status
(
  uint16                *nak_bitmap,
  uint16                fsn,
  uint16                lsn,
  boolean               update,
  uint16                mod
)
{
  uint8             cnt = 0;
  uint16            last_sn = 0;
/*------------------------------------------------------------------------*/

  if ( !RLCI_SEQ_GE( lsn, fsn, RLC_AM_MODULUS ) )
  {
    return;
  }

  /*------------------------------------------------------------------------
   Loop and update status from fn to lsn..
  ------------------------------------------------------------------------*/
  do
  {
    /*----------------------------------------------------------------------
     We store pdu status as an array of 16 bit values, where each bit
     corresponds to a pdu sn. We work on one 16-bit value at a time.
     get the last sn which the 16-bit value having fsn corresponds to. 
    ----------------------------------------------------------------------*/
    cnt = RLCI_GET_AM_COUNT_STATUS_OPT_BITMAP(fsn);
    last_sn = ( fsn + cnt ) & (RLC_AM_MODULUS - 1);  

    /*----------------------------------------------------------------------
      If lsn is greater than the last sn  which the 16-bit value having
      fsn corresponds to (last_lsn), update the status for pdus fron fsn to 
      last_lsn, update fsn to point to next sn after last_lsn.
    ----------------------------------------------------------------------*/
    if ( RLCI_SEQ_GT( lsn, last_sn, RLC_AM_MODULUS ) )
    {
      if ( TRUE == update )
      {
        /*------------------------------------------------------------------
          Mark PDUs from fsn to last_sn as naked.
        ------------------------------------------------------------------*/
  /*lint -e502 */
        RLCI_SET_AM_SN_RANGE_NAK_STATUS( nak_bitmap, fsn, last_sn, mod );
  /*lint +e502 */
        fsn = RLCI_AM_SEQ_INC( last_sn );
      }
      else
      {
        /*------------------------------------------------------------------
          Mark PDUs from fsn to last_sn as received.
        ------------------------------------------------------------------*/
        RLCI_RESET_AM_SN_RANGE_NAK_STATUS( nak_bitmap, fsn, last_sn, mod );
        fsn = RLCI_AM_SEQ_INC( last_sn );
      }
    }
    /*----------------------------------------------------------------------
      If last_sn is less than the last sn  which the 16-bit value having
      fsn corresponds to (last_lsn), update the status for pdus from fsn to 
      lsn, since both last_sn and fsn fall in the same 16-bit value.
    ----------------------------------------------------------------------*/
    else
    {
      last_sn = lsn;
      if ( TRUE == update )
      {
        /*------------------------------------------------------------------
          Mark PDUs from fsn to last_sn as naked.
        ------------------------------------------------------------------*/
  /*lint -e502 */
        RLCI_SET_AM_SN_RANGE_NAK_STATUS( nak_bitmap, fsn, lsn, mod );
  /*lint +e502 */
      }
      else
      {
        /*------------------------------------------------------------------
          Mark PDUs from fsn to last_sn as received.
        ------------------------------------------------------------------*/
        RLCI_RESET_AM_SN_RANGE_NAK_STATUS( nak_bitmap, fsn, lsn, mod );
      }
    }

  }while ( last_sn != lsn );

  return;
}
/*===========================================================================

FUNCTION rlc_get_nak_bitmap

DESCRIPTION
    - This function gets the nak bitmap structure maintained on DL.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  lc_id      - logical channel id.

RETURN VALUE
  nak bitmap - pointer to nak bitmap structure.

SIDE EFFECTS
  None.

===========================================================================*/
rlc_opt_status_type  *rlc_get_nak_bitmap
(
  byte               lc_id  
)
{
  rlci_dl_ctrl_blk_type      *ctl_blk_ptr = NULL;
  rlc_opt_status_type        *bitmap = NULL;
/*------------------------------------------------------------------------*/

  ctl_blk_ptr = get_am_ctl_blk(lc_id);
  bitmap = &ctl_blk_ptr->ctrl.am_ctrl.dl_status_opt_bitmap;
  
  return ( bitmap ); 
}
/*===========================================================================

FUNCTION rlc_get_nak_status

DESCRIPTION
    - This function gets the nak status indicating whether we have any naks
      pending to be transmited or not.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  nak_bitmap - pointer to bitmap array
  fsn        - fist sequence number for which status is needed.
  lsn        - last sequence number for which status is needed.
  mod        - modulus to use..

RETURN VALUE
  nak status - TRUE - nak is present.
               FALSE - no naks are there.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_get_nak_status
(
  byte                  lc_id,
  uint16                fsn,
  uint16                lsn,
  uint16                mod 
)
{
  uint8               cnt = 0;
  rlci_dl_ctrl_blk_type *ctl_blk_ptr;
  uint16 last_sn;
  uint16 *nak_bitmap = NULL;
/*------------------------------------------------------------------------*/

  ctl_blk_ptr = get_am_ctl_blk(lc_id);

  if ( ctl_blk_ptr == NULL )
  {
    ERR( " Bad i/p arg",0,0,0 );
    return FALSE;
  }

  if ( !RLCI_SEQ_GE( lsn, fsn, RLC_AM_MODULUS ) )
  {
    return FALSE;
  }

  nak_bitmap = ctl_blk_ptr->ctrl.am_ctrl.dl_status_opt_bitmap.status_bitmap;

  /*------------------------------------------------------------------------
   Loop and update status from fn to lsn..
  ------------------------------------------------------------------------*/
  do
  {
    /*----------------------------------------------------------------------
     We store pdu status as an array of 16 bit values, where each bit
     corresponds to a pdu sn. We work on one 16-bit value at a time.
     
     get the last sn which the 16-bit value having fsn corresponds to. 
    ----------------------------------------------------------------------*/
    cnt = RLCI_GET_AM_COUNT_STATUS_OPT_BITMAP(fsn);
    last_sn = ( fsn + cnt ) & (RLC_AM_MODULUS - 1);  

    /*----------------------------------------------------------------------
      If lsn is greater than the last sn  which the 16-bit value having
      fsn corresponds to (last_lsn), update the status for pdus fron fsn to 
      last_lsn, update fsn to point to next sn after last_lsn.
    ----------------------------------------------------------------------*/
    if ( !RLCI_SEQ_GT( lsn, last_sn, RLC_AM_MODULUS ) )
    {
      last_sn = lsn;
    }
    /*----------------------------------------------------------------------
      If we have any bit set between fsn and lsn return TRUE..
    ----------------------------------------------------------------------*/
  /*lint -e502 */
    if( RLCI_GET_AM_SN_RANGE_NAK_STATUS( nak_bitmap, fsn, last_sn, mod ) )
  /*lint +e502 */
    {
      return TRUE;
    }

    fsn = RLCI_AM_SEQ_INC( last_sn );

  }while (last_sn != lsn);

  return (FALSE);

}

/*===========================================================================

FUNCTION rlc_get_num_nak

DESCRIPTION
    - This function returns the number number of sn_li pairs in the 
      DL nak status array.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  lc_id      - logical channel id.


RETURN VALUE
  num_sn_li - number of sn_li pairs in the DL nak status array.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_get_num_nak
(
  byte                  lc_id  
)
{
  rlci_dl_ctrl_blk_type *ctl_blk_ptr;
/*------------------------------------------------------------------------*/

  ctl_blk_ptr = get_am_ctl_blk(lc_id);

  return (ctl_blk_ptr->ctrl.am_ctrl.num_sn_li);
}

/*===========================================================================

FUNCTION rlc_get_num_nak_sn_li

DESCRIPTION
    - This function calculates and returns the number number of sn_li pairs 
      in the nak status array..

DEPENDENCIES
  None.

INPUT ARGUMENTS
  lc_id      - logical channel id.
  fsn        - fist sequence number from which sn_li is counted.
  lsn        - last sequence number till which sn_li is counted.
  mod        - modulus to use..


RETURN VALUE
  num_sn_li - number number of sn_li pairs in the nak status array.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlc_get_num_nak_sn_li
(
  byte                  lc_id,
  uint16                fsn,
  uint16                lsn,
  uint16                mod 
)
{
  rlci_dl_ctrl_blk_type *ctl_blk_ptr;
  uint16 temp_sn, num_sn_li = 0, sn_li_cnt = 0;
  uint16 *nak_bitmap = NULL;
/*------------------------------------------------------------------------*/

  ctl_blk_ptr = get_am_ctl_blk(lc_id);

  if ( ctl_blk_ptr == NULL )
  {
    ERR( " Bad i/p arg",0,0,0 );
    return 0;
  }

  if ( !RLCI_SEQ_GE( lsn, fsn, RLC_AM_MODULUS ) )
  {
    return 0;
  }

  if ( !RLCI_SEQ_DIFF( lsn, fsn, RLC_AM_MODULUS ) )
  {
    return 0;
  }

  nak_bitmap = ctl_blk_ptr->ctrl.am_ctrl.dl_status_opt_bitmap.status_bitmap;

  for (temp_sn = fsn; RLCI_SEQ_GT( lsn, temp_sn, RLC_AM_MODULUS); 
         (temp_sn = RLCI_AM_SEQ_INC(temp_sn)))
  {
    if (RLCI_GET_AM_STATUS_OPT_BITMAP(nak_bitmap, temp_sn, mod))
    {
      sn_li_cnt++;
      if (sn_li_cnt > 15)
      {
        sn_li_cnt = 0;
        num_sn_li++;
      }
    }
    else
    {
      sn_li_cnt = 0;
      num_sn_li++;
    }
  }

  if (sn_li_cnt > 0)
  {
    num_sn_li++;
  }
  return (num_sn_li);
}
/*===========================================================================

FUNCTION rlc_set_num_nak_sn_li

DESCRIPTION
    - This function sets the value of number of sn_li pairs, num_sn_li.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  lc_id      - logical channel id.
  num_sn_li  - value for num_sn_li

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_set_num_nak_sn_li
(
  byte                  lc_id,
  uint16                num_sn_li
)
{
  rlci_dl_ctrl_blk_type *ctl_blk_ptr;
/*------------------------------------------------------------------------*/

  ctl_blk_ptr = get_am_ctl_blk(lc_id);

  if ( ctl_blk_ptr == NULL )
  {
    ERR( " Bad i/p arg",0,0,0 );
    return;
  }
  ctl_blk_ptr->ctrl.am_ctrl.num_sn_li = num_sn_li;
  return;
}
/*===========================================================================

FUNCTION rlci_enh_update_nak_list

DESCRIPTION
    - As part of Post-Processing, Status message will be prepared as below
      - Prepare the LIST SUFI in which, 
        SN is the sequence number in which first hole is existing and 
        L is the number of consecutive holes present      

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_update_nak_list(rlci_dl_ctrl_blk_type *ctrl_blk_ptr)
{
  uint16                        vr_h;
  rlci_dl_am_ctrl_type         *am_ctrl_ptr;
  rlc_dl_am_reseq_q_type       *dl_reseq_q_ptr;
  rlc_opt_status_type           dl_status_opt_bitmap;
  rlci_dl_am_state_type         *dl_state_ptr = NULL;
  uint16 vr_r, max_rx_sn;
  uint8 distance = 0; /*li value in sl_li pair for list sufi*/
  uint16 num_sn_li = 0; /*number of sn_li pairs for list sufi*/

  rex_crit_sect_type *datapath_mutex = NULL;

  L2_ACQ_DL_LOCK();
  am_ctrl_ptr = &(ctrl_blk_ptr->ctrl.am_ctrl);
  dl_reseq_q_ptr = am_ctrl_ptr->dl_reseq_q;
  
  vr_h = am_ctrl_ptr->dl_state_var.vr_h;

  memset(&dl_status_opt_bitmap,0,sizeof(rlc_opt_status_type));
  /*---------------------------------------------------------------------
    Backup DL status Array.
  ---------------------------------------------------------------------*/
  datapath_mutex = L2_ACQ_UL_LW_LOCK(am_ctrl_ptr->am_common->ul_control_id);

  WCDMA_MEMCPY(&dl_status_opt_bitmap,
               sizeof(rlc_opt_status_type),
               &ctrl_blk_ptr->ctrl.am_ctrl.dl_status_opt_bitmap, 
               sizeof(rlc_opt_status_type));

  L2_ACQ_UL_LW_UNLOCK(datapath_mutex);
  dl_state_ptr = &am_ctrl_ptr->dl_state_var;

  /*-------------------------------------------------------------------------
    Initialize num_sn_li to 0, we will be recalculating number of sn_li pairs 
    here.
  -------------------------------------------------------------------------*/
  num_sn_li = 0;

  /*------------------------------------------------------------------------
    Set NAK bits for SN less than vr_r.
  ------------------------------------------------------------------------*/
  if (RLCI_SEQ_GT(dl_state_ptr->vr_r, dl_state_ptr->old_vr_r, RLC_AM_MODULUS))
  {
    for (vr_r = dl_state_ptr->old_vr_r; 
          RLCI_SEQ_GT(dl_state_ptr->vr_r, vr_r, RLC_AM_MODULUS);
          (vr_r = RLCI_AM_SEQ_INC(vr_r)))
    {
      RLCI_SET_AM_SN_STATUS_OPT_BITMAP( 
               dl_status_opt_bitmap.status_bitmap, vr_r, 
               RLC_AM_HALF_MOD);

      if (rlc_debug_var & RLC_DBG_DL_AM)
      {
        MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "BITMAP_TST:num_sn_li %d, rcvd_sn %d",
                                                   num_sn_li, vr_r );
      }
    }
  }

  /*---------------------------------------------------------------------
    if vr_h == vr_r, we dont have any further holes, copy back updated
    DL status array and num_sn_li, relase mutex.
  ---------------------------------------------------------------------*/
  if (vr_h == dl_state_ptr->vr_r)
  {
    datapath_mutex = L2_ACQ_UL_LW_LOCK(am_ctrl_ptr->am_common->ul_control_id);

    WCDMA_MEMCPY(&ctrl_blk_ptr->ctrl.am_ctrl.dl_status_opt_bitmap,
                 sizeof(rlc_opt_status_type),
                 &dl_status_opt_bitmap,
                 sizeof(rlc_opt_status_type));

    ctrl_blk_ptr->ctrl.am_ctrl.num_sn_li = num_sn_li;
    am_ctrl_ptr->dl_state_var.dl_vr_h_status = vr_h;

    L2_ACQ_UL_LW_UNLOCK(datapath_mutex);
    L2_ACQ_DL_UNLOCK();
    return;
  }

  /*---------------------------------------------------------------------
    Get MAX RX SN, vr_h or vr_mr
  ---------------------------------------------------------------------*/
  max_rx_sn = RLCI_SEQ_GT(dl_state_ptr->vr_h, 
                          dl_state_ptr->vr_mr, RLC_AM_MODULUS) ? 
                          dl_state_ptr->vr_mr : dl_state_ptr->vr_h;

  /*---------------------------------------------------------------------
    Update DL status array, resetting NAK bits for PDUs received and 
    re-calculating sn_li pairs.
  ---------------------------------------------------------------------*/
  for (vr_r = am_ctrl_ptr->dl_state_var.vr_r; 
       (RLCI_SEQ_GT(max_rx_sn, vr_r, RLC_AM_MODULUS) == TRUE);
       (vr_r = RLCI_AM_SEQ_INC(vr_r)))
  {
    if (dl_reseq_q_ptr->pdu_info[RLCI_GET_RESEQ_Q_IDX(vr_r)].dsm_ptr != NULL)
    {
      /*---------------------------------------------------------------------
        PDU is received RESET corresponding NAK bit...
      ---------------------------------------------------------------------*/
      RLCI_RESET_AM_SN_STATUS_OPT_BITMAP( 
             dl_status_opt_bitmap.status_bitmap, vr_r, RLC_AM_HALF_MOD);
      /*---------------------------------------------------------------------
        This PDU breaks sn_li list just increment num_sn_li and reset 
        distance to start counting next sn_li pair from next pdu onwards.
      ---------------------------------------------------------------------*/
      if (distance > 0)
      {
        num_sn_li++;
        distance = 0;
      }
    }
    else 
    {
      /*---------------------------------------------------------------------
        This is hole increment distance and num_sn_li pair accordingly..
      ---------------------------------------------------------------------*/
      distance++;
      if (distance > 15)
      {
        num_sn_li++;
        distance = 0;
      }
      if ((ctrl_blk_ptr->ctrl.am_ctrl.dl_ctl_cnfg->rlc_info.status_info.missing_pdu) && 
         (RLCI_SEQ_GE(vr_r, am_ctrl_ptr->dl_state_var.prev_vr_h, RLC_AM_MODULUS)))
      {
        dl_state_ptr->rcvd_poll = TRUE; 
#ifdef FEATURE_RLC_MISSING_PDU_REPEAT_STATUS
        #error code not present
#endif
      }
    }
  }
  /*---------------------------------------------------------------------
    Count for last sn_li pair..
  ---------------------------------------------------------------------*/
  if (distance > 0)
  {
    num_sn_li++;
  }
  /*---------------------------------------------------------------------
    Copy back updated DL status array and num_sn_li, relase mutex.
  ---------------------------------------------------------------------*/
  datapath_mutex = L2_ACQ_UL_LW_LOCK(am_ctrl_ptr->am_common->ul_control_id);

  WCDMA_MEMCPY(&ctrl_blk_ptr->ctrl.am_ctrl.dl_status_opt_bitmap,
               sizeof(rlc_opt_status_type),
               &dl_status_opt_bitmap,
               sizeof(rlc_opt_status_type));
    
  ctrl_blk_ptr->ctrl.am_ctrl.num_sn_li = num_sn_li;

  am_ctrl_ptr->dl_state_var.dl_vr_h_status = max_rx_sn;
  
  L2_ACQ_UL_LW_UNLOCK(datapath_mutex);

  if (rlc_debug_var & RLC_DBG_DL_AM)
  {
    MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "BITMAP_TST:num_sn_li %d, vr_r %d, max_rx_sn %d",
         ctrl_blk_ptr->ctrl.am_ctrl.num_sn_li,
                                               am_ctrl_ptr->dl_state_var.vr_r,
                                               max_rx_sn);
  }

  L2_ACQ_DL_UNLOCK();
} /* rlci_enh_update_nak_list() */

/*===========================================================================

FUNCTION rlci_enh_status_generation

DESCRIPTION
    - As part of Post-Processing, Status message will be prepared
      - Copy NAK item from NAK_LIST_Q to TX_NAK_Q
      - Let the UL know that it has to send a status report to the peer entity
      - Clear the rcvd poll bit if set
      - Turn the status prohibit flag to TRUE to prohibit the sending further status

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlci_enh_status_generation(rlci_dl_ctrl_blk_type *ctrl_blk_ptr)
{
  rlci_dl_am_ctrl_type *am_ctrl_ptr = &(ctrl_blk_ptr->ctrl.am_ctrl);
  rlci_dl_am_state_type  *dl_state_var_ptr = &(am_ctrl_ptr->dl_state_var);

  am_ctrl_ptr->dl_status_opt_bitmap.next_sn_valid = FALSE;
  rlci_set_ul_num_sn_li(am_ctrl_ptr->am_common->ul_data_id,
            am_ctrl_ptr->num_sn_li);
  if(rlc_debug_var & RLC_DBG_DL_CTRL)
  {
    MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL_CTRL: Sending status rpt for lc %d - report_vr_h %d, vr_r %d",
             am_ctrl_ptr->am_common->dl_data_id,
                                               dl_state_var_ptr->report_vr_h,
                                               dl_state_var_ptr->vr_r);
  }

  /* Let the UL know that it has to send a status report to the peer entity */
  rlci_rx_status_changed(am_ctrl_ptr->am_common->ul_data_id, TRUE);

  /* Clear the rcvd poll bit if set */
  dl_state_var_ptr->rcvd_poll = FALSE;

  /* Turn the status prohibit flag to TRUE to prohibit the sending further status */
  if (am_ctrl_ptr->dl_ctl_cnfg->rlc_info.status_info.status_info_incl &&
      am_ctrl_ptr->dl_ctl_cnfg->rlc_info.status_info.status_prohibit)
  {
    dl_state_var_ptr->status_prohibit = TRUE;

    if(rlc_debug_var & RLC_DBG_DL_CTRL)
    {    
      MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "DL_CTRL: Turn on Status Prohibit LC %d, timer_val %d",
               am_ctrl_ptr->am_common->dl_data_id, 
                                                 am_ctrl_ptr->dl_ctl_cnfg->rlc_info.status_info.status_prohibit);
    }
  }
}

/*===========================================================================

FUNCTION rlci_enh_dl_reassembly_post_pdu

DESCRIPTION
    - For AM, Once the logical channel is changed, call the Post processing 
      functionality which will work on the received PDUs in this call
       - If needed NAK list will be prepared
       - If needed, reassembly functionality is called to prepare SDUs out of
         received in-seq PDUs.
       - If needed and allowed, Status message will be prepared

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_reassembly_post_pdu(rlci_dl_ctrl_blk_type *ctrl_blk_ptr)
{
  rlci_ul_ctrl_blk_type  *ul_ctrl_blk_ptr;
  rlci_dl_am_state_type  *dl_state_var_ptr;
  rex_crit_sect_type     *datapath_mutex = NULL;
  rlc_lc_id_type         lc_id;

  dl_state_var_ptr = &(ctrl_blk_ptr->ctrl.am_ctrl.dl_state_var);
  rlci_enh_update_nak_list(ctrl_blk_ptr);
  lc_id = ctrl_blk_ptr->ctrl.am_ctrl.am_common->ul_data_id;
  ul_ctrl_blk_ptr = rlci_get_ctl_blk_ptr(lc_id);

#ifdef FEATURE_RLC_MISSING_PDU_REPEAT_STATUS  
  #error code not present
#endif
  /* If allowed and required, Send the status */
  if (dl_state_var_ptr->rcvd_poll == TRUE) 
  { 
    L2_ACQ_DL_LOCK();	
    datapath_mutex = L2_ACQ_UL_LW_LOCK(lc_id);	
    if((dl_state_var_ptr->status_prohibit == FALSE) ||
       ((NULL != ul_ctrl_blk_ptr)&&(ul_ctrl_blk_ptr->ctrl.am_ctrl.ul_state.status_rpt == TRUE)))
    {
      rlci_enh_status_generation(ctrl_blk_ptr);
    }
    L2_ACQ_UL_LW_UNLOCK(datapath_mutex);
    L2_ACQ_DL_UNLOCK();
  }
  
  if (rlc_debug_var & RLC_DBG_REASSEMBLY)
  {
    MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "reassembly_post_pdu: old vrR %d, vrR %d",
                                               dl_state_var_ptr->old_vr_r,
                                               dl_state_var_ptr->vr_r);
  }

  /* If there is update in vr_r, call the reassembly functionality */
  if (dl_state_var_ptr->old_vr_r != dl_state_var_ptr->vr_r)
  {
    if (rlc_debug_var & RLC_DBG_REASSEMBLY)
    {
      MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "reassembly_post_pdu: Calling reassemble");
    }
    
    rlci_enh_dl_am_reassemble_data_pdus(ctrl_blk_ptr, dl_state_var_ptr->old_vr_r);
  /* Since there is no lock used inside rlci_enh_dl_am_reassemble_data_pdus(), there is 
     no need to reassembly via posting a signal as done below and doing reassembly 10 SDUs
     at a given time*/
#if 0
      MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "reassembly_post_pdu: Posting Reassembly SIGNAL");
    /* Post a signal to reassemble the PDUs later */
    (void) rex_set_sigs(&wcdma_l2_dl_tcb, RLC_DL_DATA_REASSEMBLY_SIG);
#endif
  }

}

/*===========================================================================

FUNCTION rlc_enh_rcvd_data_post_processing

DESCRIPTION
    - As part of the post_processing functionality
      - 1. If configured, Post the logging data
      - 2. For all the previously received PDUs on prev_lc_id, 
             For AM - Call the Post procesing function
             For UM - Reassembly is taken care during the PDU handling itself
                      If there is anything left over in dl_pdu_info_ptr 
                      for this LgCh,
                        - Update to partial_sdu_ptr
      - 3. If any SDU is enqueued into Water mark and Call back is registerd, 
           Call the callback function to post SDU data to Upper Layers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_enh_rcvd_data_post_processing(rlci_dl_ctrl_blk_type *ctrl_blk_ptr,
                                       uint8 prev_rlc_id, 
                                       boolean hs_path)
{
  rlci_dl_temp_pdu_info_type *dl_pdu_info_ptr;

  if (ctrl_blk_ptr == NULL)
  {
    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: post processing on prev_rlc_id %d - NULL Ctrl Blk",
                                                prev_rlc_id);
    return;
  }
  
  /* 
   * For all the previously received PDUs on prev_lc_id, 
   *   For AM - Call the Post procesing function
   *   For UM - Reassembly is taken care duing the PDU handling itself
   *            If there is anything left over in dl_pdu_info_ptr for this LgCh,
   *            Update to partial_sdu_ptr
   */
  if (ctrl_blk_ptr->lc_mode == UE_MODE_ACKNOWLEDGED)
  {
    /* Update dl stats to prevent RESET on any SRB that is not receiving CTRL ACKs
    because NW is busy scheduling on a higher priority SRB */
    
    if ( ctrl_blk_ptr->lc_type == UE_LOGCHAN_DCCH)
    {
      update_dl_priority_stats(prev_rlc_id);
    }
    
    rlci_enh_dl_reassembly_post_pdu(ctrl_blk_ptr);
    
    #ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
    rlc_enh_hs_rach_check_rach_trigger(prev_rlc_id);
    #endif
  }
  else if (ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED)
  {
    dl_pdu_info_ptr = rlci_enh_get_temp_pdu_info_ptr();
    if (dl_pdu_info_ptr->dsm_ptr != NULL)
    {
      if (dl_pdu_info_ptr->dsm_ptr->references > 0) 
      {
        rlci_enh_update_um_partial_sdu_ptr(ctrl_blk_ptr, 
                                         dl_pdu_info_ptr, NULL);
      }
      else  
      {
        /* Case: SN-JUMP is in same DSM-item
         * {BIG-DSM1:[..._SN-1(LI)_, _SN_, _SN+1_, _SN+10_, _SN+11_,...]}
         *                             ^             ^
                                       ^             ^-<- SN-JUMP is in same BIG-DSM item,
                                       ^             So the DSM-item might be deleted earlier while
                                       ^             discarding PDUs (containing no LI) 
         *                             ^-<-dl_pdu_info (app_field = 0, ref-cnt = 0)
         * dl_pdu_info contains ref. to already freed BIG-DSM1 item. 
         * Hence avoid appending this item, & set dl_pdu_info to zero 
         * (this will help in removing dangling references)
         */
        MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC_DBG: DSM-item (within dl_pdu_info) is already freed LC-id %d vr_ur %d dl_pdu_info_ptr 0x%x",
                                                   prev_rlc_id,
                                                   ctrl_blk_ptr->ctrl.um_ctrl.vr_ur,
                                                   dl_pdu_info_ptr->dsm_ptr);
        
        memset(dl_pdu_info_ptr, 0, sizeof(rlci_dl_temp_pdu_info_type));
        
      }
    }

#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
    rlc_enh_hs_rach_check_rach_trigger(prev_rlc_id);
#endif     
  }
  if (rlc_debug_var & RLC_DBG_REASSEMBLY)
  {
    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "In Post processing - nSDU %d",
                                               ctrl_blk_ptr->n_sdu);
  }

  /* If any SDU in WM Queue and callback configured, post to upper layers */
  if ((ctrl_blk_ptr->n_sdu > 0) && (ctrl_blk_ptr->rlc_post_rx_func_ptr != NULL))
  {
    ctrl_blk_ptr->rlc_post_rx_func_ptr(prev_rlc_id, 
                                       ctrl_blk_ptr->n_sdu, 
                                       ctrl_blk_ptr->rlc_post_rx_func_ptr_para);
    ctrl_blk_ptr->n_sdu = 0;
  }

  /* If configured, Post the logging data */
#ifdef FEATURE_RLC_QXDM_LOGGING
  if (((ctrl_blk_ptr->lc_mode == UE_MODE_ACKNOWLEDGED) &&
        RLCI_GET_AM_LOGGING_CONFIG(prev_rlc_id)) ||
      ((ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED) && 
        RLCI_GET_UM_LOGGING_CONFIG))
  {
    rlc_enh_logging_post_data_handler(ctrl_blk_ptr, prev_rlc_id, hs_path);
  }
#endif

}

/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_handler_hs

DESCRIPTION
    - For every log Channel
       - Process each PDU one after another
    - For AM, Once the logical channel is changed, call the Post processing 
      functionality (Reassembly, Status building, Status preperation)
      which will work on the received PDUs in this call
    - For UM and TM, during the PDU processing itself Post processing is completed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_enh_dl_pdu_data_handler_hs(l1_dec_hs_tsn_info_type *tsn_info_buf_ptr)
{
  uint8                            index, ctrl_index, prev_rlc_id = 0xFF;
  boolean                          first_pdu_on_this_lch = FALSE;
  complete_pdu_type               *complete_pdu_ptr;
  rlci_dl_ctrl_blk_type           *ctrl_blk_ptr = NULL;

  dl_pdu_decode_result = TRUE;
  if (tsn_info_buf_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: RLC Internal TSN Info Q is empty");
    return;
  }

#ifdef FEATURE_WCDMA_DL_DATAPATH_LOG_PKT
  rlc_global_sub_fn = tsn_info_buf_ptr->global_sub_fn;
#endif

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
  if ((tsn_info_buf_ptr->num_partial_pdus != 0) ||
      (tsn_info_buf_ptr->num_complete_pdus == 0))
  {
    ERR_FATAL("TSN %d - Not expected Num partial in RLC - Partial %d, Complete %d", 
               tsn_info_buf_ptr->tsn, tsn_info_buf_ptr->num_partial_pdus, 
               tsn_info_buf_ptr->num_complete_pdus);
  }
#endif
  for (index = 0; index < tsn_info_buf_ptr->num_complete_pdus; index++)
  {
    complete_pdu_ptr = &(tsn_info_buf_ptr->complete_pdu[index]);
    
    if (prev_rlc_id != complete_pdu_ptr->rlc_id)
    {
      if (prev_rlc_id != 0xFF)
      {
        /* Call the post processing function for the data rcvd on prev_rlc_id */
        rlc_enh_rcvd_data_post_processing(ctrl_blk_ptr, prev_rlc_id, TRUE);
      }
      
      /* Get the new ctrl Index and ctrl blk ptr */
      ctrl_index = RLCI_GET_CTRL_INDEX(complete_pdu_ptr->rlc_id);
      if (ctrl_index == INIT_CTL_BLK_ID)
      {
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: rlc_id %d is not yet configured - Discard PDU",
                                                    complete_pdu_ptr->rlc_id);
        rlc_discard_dsm_item(&(complete_pdu_ptr->iovec.ptr));
        continue;
      }
      
      ctrl_blk_ptr = RLCI_GET_CTRL_BLK_PTR(ctrl_index);
      if (ctrl_blk_ptr->lc_state == DL_NULL_STATE)
      {
        MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: lc_state %d is NULL for rlc_id %d - Discard PDU",
                                                    ctrl_blk_ptr->lc_state,
                                                    complete_pdu_ptr->rlc_id);
        rlc_discard_dsm_item(&(complete_pdu_ptr->iovec.ptr));
        continue;
      }
      
      ctrl_blk_ptr->n_sdu = 0;
      prev_rlc_id = complete_pdu_ptr->rlc_id;
      first_pdu_on_this_lch = TRUE;
    }
    if (NULL == ctrl_blk_ptr)
    {
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "ctrl_blk_ptr is NULL for LC id %d",
                                                  complete_pdu_ptr->rlc_id);
      continue;
    }
#ifdef FEATURE_RLC_QXDM_LOGGING
    if (((ctrl_blk_ptr->lc_mode == UE_MODE_ACKNOWLEDGED) &&
         (RLCI_GET_AM_LOGGING_CONFIG(prev_rlc_id))) ||
        ((ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED) &&
         (RLCI_GET_UM_LOGGING_CONFIG)))
    {
      rlc_enh_logging_data_handler(ctrl_blk_ptr, complete_pdu_ptr, 
                                   first_pdu_on_this_lch);
    }
#endif

    switch (ctrl_blk_ptr->lc_mode)
    {
      case UE_MODE_ACKNOWLEDGED:
        rlci_enh_dl_process_am_pdu(ctrl_blk_ptr, complete_pdu_ptr, 
                                   first_pdu_on_this_lch);
       break;
       
      case UE_MODE_UNACKNOWLEDGED:
#ifdef FEATURE_UMTS_BMC
      case UE_MODE_DL_CTCH:
#endif      
        complete_pdu_ptr->rlc_hdr_data = (uint32) (complete_pdu_ptr->rlc_hdr_data >> 8);
        rlci_enh_dl_process_um_pdus(ctrl_blk_ptr, complete_pdu_ptr);
       break;
       
      case UE_MODE_TRANSPARENT:
       MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: rlc_enh_dl_pdu_data_handler_hs shouldn't get TM PDU");
       break;
       
      default:
       MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: rlc_enh_dl_pdu_data_handler_hs - Unknown Mode",
                                                   ctrl_blk_ptr->lc_mode);
       break;
    }
    
    first_pdu_on_this_lch = FALSE;
    
    /* 
     * PDU is consumed by the RLC. Either into reassembly queue or 
     * discarded after processing.
     */
    complete_pdu_ptr->iovec.ptr = NULL;
  }
  
  /* Call the post processing function for the data rcvd on prev_rlc_id */
  rlc_enh_rcvd_data_post_processing(ctrl_blk_ptr, prev_rlc_id, TRUE);

#ifdef FEATURE_RLC_QXDM_LOGGING
#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
  /* Call the log packed for EHS Call */
  if (mac_ehs_query_hs_type() == MAC_EHS)
  {
    rlc_dl_enh_r7_am_pdu_log_pkt(&downlink_table);
  }
#endif
#endif

  /* Call the MAC-HS API to add processed TSN to free TSN Queue */
  mac_hs_enqueue_free_tsn(tsn_info_buf_ptr);
}

/*===========================================================================

FUNCTION rlc_enh_handle_tm_data

DESCRIPTION
    - Process TM data, if present and configured to process in interrupt context
    - If some data present in TM with interrupt context TRUE, 
        - Process the TM PDUs
    - If some more PDUs are left in the rcvd, copy them into local buffer
      and back into rlc_enh_data_ptr
    - If nothing, return the entry to free queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_handle_tm_data(rlc_enh_data_ind_type *rlc_enh_data_ptr)
{
  uint8                            index, ctrl_index, num_processed = 0;
  complete_pdu_type               *complete_pdu_ptr;
  rlci_dl_ctrl_blk_type           *ctrl_blk_ptr = NULL;
  
  for (index = 0; index < rlc_enh_data_ptr->num_complete_pdus; index++)
  {
    complete_pdu_ptr = &(rlc_enh_data_ptr->complete_pdu[index]);
    
    /* Get the new ctrl Index and ctrl blk ptr */
    ctrl_index = RLCI_GET_CTRL_INDEX(complete_pdu_ptr->rlc_id);
    if (ctrl_index == INIT_CTL_BLK_ID)
    {
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: rlc_id %d is not yet configured - Discard PDU in Task-context",
                                                  complete_pdu_ptr->rlc_id);
      continue;
    }

    ctrl_blk_ptr = RLCI_GET_CTRL_BLK_PTR(ctrl_index);
    if (ctrl_blk_ptr->lc_state == DL_NULL_STATE)
    {
      MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: lc_state %d is NULL for rlc_id %d - Discard PDU in Task-context",
                                                  ctrl_blk_ptr->lc_state,
                                                  complete_pdu_ptr->rlc_id);
      continue;
    }
      
    /* If rcvd PDU is of TM Mode and need to be processed in Interrupt
     * context, process immediately and update iovec.ptr to NULL
     */
    if ((ctrl_blk_ptr->lc_mode == UE_MODE_TRANSPARENT) &&
        (ctrl_blk_ptr->context == TRUE))
    {
      
      L2_MUTEX_LOCK(&rlc_dl_tm_mutex);

      if (NULL != ctrl_blk_ptr->dl_wm_ptr)
      {
        rlci_enh_dl_reassemble_tm_pdus(ctrl_blk_ptr, complete_pdu_ptr);
        complete_pdu_ptr->iovec.ptr = NULL;
        num_processed++;
      }
      else
      {
        MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: WM NULL for LC %d, lc_state %d, Discard PDU in Task-context",
                                                    complete_pdu_ptr->rlc_id,
                                                    ctrl_blk_ptr->lc_state);
      }

      L2_MUTEX_UNLOCK(&rlc_dl_tm_mutex);
    }
  }
  
  /* 
   * If all the PDUs are processed, update the num_complete_pdus to 0
   * Otherwise, rlc_enh_data_ptr will be enqueue to RLC for processing
   * During Processing all TM data, ISR Mode will not be processed
   */
  if (num_processed == rlc_enh_data_ptr->num_complete_pdus)
  {
    rlc_enh_data_ptr->num_complete_pdus = 0;
  }
}

/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_handler_non_hs

DESCRIPTION
    - For every log Channel
       - Process each PDU one after another
    - For AM, Once the logical channel is changed, call the Post processing 
      functionality (Reassembly, Status building, Status preperation)
      which will work on the received PDUs in this call
    - For UM and TM, during the PDU processing itself Post processing is completed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_enh_dl_pdu_data_handler_non_hs(rlc_enh_data_ind_type  *rlc_enh_data_ptr)
{
  uint8                            index, ctrl_index, prev_rlc_id = 0xFF;
  boolean                          first_pdu_on_this_lch = FALSE;
  complete_pdu_type               *complete_pdu_ptr;
  rlci_dl_ctrl_blk_type           *ctrl_blk_ptr = NULL;
  rlci_dl_temp_pdu_info_type      *dl_pdu_info_ptr;  
  
  if (rlc_enh_data_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: RLC Internal Info Q is empty");
    return;
  }
  dl_pdu_decode_result = TRUE;
  for (index = 0; index < rlc_enh_data_ptr->num_complete_pdus; index++)
  {
    complete_pdu_ptr = &(rlc_enh_data_ptr->complete_pdu[index]);
    
    if (prev_rlc_id != complete_pdu_ptr->rlc_id)
    {
      if (prev_rlc_id != 0xFF)
      {
        /* Call the post processing function for the data rcvd on prev_rlc_id */
        rlc_enh_rcvd_data_post_processing(ctrl_blk_ptr, prev_rlc_id, FALSE);
      }
      
      /* Get the new ctrl Index and ctrl blk ptr */
      ctrl_index = RLCI_GET_CTRL_INDEX(complete_pdu_ptr->rlc_id);
      if (ctrl_index == INIT_CTL_BLK_ID)
      {
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: rlc_id %d is not yet configured - Discard PDU",
                                                    complete_pdu_ptr->rlc_id);
        rlc_discard_pdu(complete_pdu_ptr);
        continue;
      }
      
      ctrl_blk_ptr = RLCI_GET_CTRL_BLK_PTR(ctrl_index);
      if (ctrl_blk_ptr->lc_state == DL_NULL_STATE)
      {
        MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: lc_state %d is NULL for rlc_id %d - Discard PDU",
                                                    ctrl_blk_ptr->lc_state,
                                                    complete_pdu_ptr->rlc_id);
        rlc_discard_pdu(complete_pdu_ptr);
        continue;
      }
      
      ctrl_blk_ptr->n_sdu = 0;
      
      /* Try commenting this code - Not really needed - Added during initial
       * debugging - Not sure really needed - Anju */
       
      /* Clear the temp_pdu_info before starting on UM data Processing */
      if (ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED)
      {
        dl_pdu_info_ptr = rlci_enh_get_temp_pdu_info_ptr();
        memset(dl_pdu_info_ptr, 0, sizeof(rlci_dl_temp_pdu_info_type));
      }
      
      prev_rlc_id = complete_pdu_ptr->rlc_id;
      first_pdu_on_this_lch = TRUE;
    }
    if (NULL == ctrl_blk_ptr)
    {
      MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "ctrl_blk_ptr is NULL for LC id %d",
                                                  complete_pdu_ptr->rlc_id);
      continue;
    }
#ifdef FEATURE_RLC_QXDM_LOGGING
    if (((ctrl_blk_ptr->lc_mode == UE_MODE_ACKNOWLEDGED) &&
         (RLCI_GET_AM_LOGGING_CONFIG(prev_rlc_id))) ||
        ((ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED) &&
         (RLCI_GET_UM_LOGGING_CONFIG)))
    {
      rlc_enh_logging_data_handler(ctrl_blk_ptr, complete_pdu_ptr, 
                           first_pdu_on_this_lch);
    }
#endif
    
    switch (ctrl_blk_ptr->lc_mode)
    {
      case UE_MODE_ACKNOWLEDGED:
        dl_pdu_decode_result = rlc_enh_data_ptr->decode_result;
        rlci_enh_dl_process_am_pdu(ctrl_blk_ptr, complete_pdu_ptr, 
                                   first_pdu_on_this_lch);
       break;
       
      case UE_MODE_UNACKNOWLEDGED:
#ifdef FEATURE_UMTS_BMC
      case UE_MODE_DL_CTCH:
#endif
        complete_pdu_ptr->rlc_hdr_data = (uint32) (complete_pdu_ptr->rlc_hdr_data >> 8);
        rlci_enh_dl_process_um_pdus(ctrl_blk_ptr, complete_pdu_ptr);
       break;
       
      case UE_MODE_TRANSPARENT:
        /* 
         * Handle TM data here, only if configured to be handled in Task context 
         * If configured to be handled in ISR Context, Data is already processed 
         */
        if (ctrl_blk_ptr->context == FALSE)
        {
          rlci_enh_dl_reassemble_tm_pdus(ctrl_blk_ptr, complete_pdu_ptr);
        }
        else if (NULL != complete_pdu_ptr->iovec.ptr)
        {
          MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: Discarding PDU for LC %d, not processed in ISR",
                                                      complete_pdu_ptr->rlc_id);
          dsm_free_packet(&(complete_pdu_ptr->iovec.ptr));
        }
       break;
       
      default:
       MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: Unknown Mode",
                                                   ctrl_blk_ptr->lc_mode);
       break;
    }
    
    first_pdu_on_this_lch = FALSE;
    
    /* 
     * PDU is consumed by the RLC. Either into reassembly queue or 
     * discarded after processing.
     */
    complete_pdu_ptr->iovec.ptr = NULL;
  }
  
  /* Call the post processing function for the data rcvd on prev_rlc_id */
  rlc_enh_rcvd_data_post_processing(ctrl_blk_ptr, prev_rlc_id, FALSE);

  if (rlc_debug_var & RLC_DBG_REASSEMBLY)
  {
    /* Add the entry into free_data_queue */
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC_DL:R99 DATA PTR FREE: enqueue back to free Q ");
  }
  q_put(&rlc_enh_free_data_q, &(rlc_enh_data_ptr->link));
}

/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_handler

DESCRIPTION
    - Process the Non-HS data, if received
       - Get the RLC Info from the rlc_enh_data_queue. Protect this operation with 
         WCDMA_INTLOCK, as it this queue is accessed by MAC-HS also
       - After processing all the entries(PDUs) in the received rlc_enh_data_ptr,
         rlc_enh_data_ptr will be added to free queue
    - Process the HS data, if received
       - Get the TSN from the rlc_tsn_info_q. Protect this operation with WCDMA_INTLOCK
         as it this queue is accessed by MAC-HS also
       - After processing all the entries(PDUs) in the received tsn_info_buf_ptr,
         tsn_info_buf_ptr will be added to free queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_dl_pdu_data_handler(void)
{
  rlc_enh_data_ind_type       *rlc_enh_data_ptr;
  l1_dec_hs_tsn_info_type     *tsn_info_buf_ptr;
  uint32                      num_tsn;

  /* Store the UM and AM log info */
  (void) rlci_enh_dl_log_info_handler();
  
  L2_MUTEX_LOCK(&rlc_dl_data_process_mutex);
  
  /* Process the Non-HS data, if received */
  if ((num_tsn = q_cnt(&rlc_enh_data_q)) > 0)
  {
    while (num_tsn != 0)
    {
      /* Get the RLC Info from the rlc_enh_data_queue */
      if ((rlc_enh_data_ptr = (rlc_enh_data_ind_type *)q_get(&rlc_enh_data_q)) 
             != NULL)
      {
        rlc_enh_dl_pdu_data_handler_non_hs(rlc_enh_data_ptr);
      }
      num_tsn--;
    }
  }
  
  L2_MUTEX_LOCK(&rlc_tsn_mutex);

  if ((l2_get_release_flag_indicator() == L2_REL_INDICATOR_5) && 
      ((num_tsn = q_cnt(&rlc_tsn_info_q)) > 0))
  {
    while (num_tsn != 0)
    {
      /* Get the TSN from the rlc_tsn_info_q */
      if ((tsn_info_buf_ptr = (l1_dec_hs_tsn_info_type *)q_get(&rlc_tsn_info_q)) 
             != NULL)
      {
        /*Set the trace value to DL_TSN_RLC*/
        tsn_info_buf_ptr->tracer = DL_TSN_RLC;
        rlc_enh_dl_pdu_data_handler_hs(tsn_info_buf_ptr);
      }
      num_tsn--;
    }
  }

  L2_MUTEX_UNLOCK(&rlc_tsn_mutex);

  L2_MUTEX_UNLOCK(&rlc_dl_data_process_mutex);
  
  /* Reset counter to indicate that we received a DL PDU */ 
  time_since_last_dl_pdu_rcvd = 0;
  
}

/*===========================================================================

FUNCTION rlc_enh_get_free_data_q_entry

DESCRIPTION
    - Returns entry from rlc_enh_free_data_q. 
      If Quue is empty, NULL will be returned

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

rlc_enh_data_ind_type *rlc_enh_get_free_data_q_entry(void)
{
  return((rlc_enh_data_ind_type *)q_get(&rlc_enh_free_data_q));
}

/*===========================================================================

FUNCTION rlc_enh_add_to_free_data_q_entry

DESCRIPTION
    - Adds the rlc_enh_data entry into Free Data Queue rlc_enh_free_data_q

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rlc_enh_add_to_free_data_q_entry(rlc_enh_data_ind_type *rlc_enh_data_ind_ptr)
{
  q_put(&rlc_enh_free_data_q, &(rlc_enh_data_ind_ptr->link));
}

/* DSM touch function for every DSM item */
static void rlc_touch_entry_dsm_item
(
  rlc_enh_data_ind_type *rlc_enh_data_ptr
)
{
  uint32 i;

  for (i = 0; i < rlc_enh_data_ptr->num_complete_pdus; i++)
  {
    if (rlc_enh_data_ptr->complete_pdu[i].iovec.offset == 0)
    {
      dsmi_touch_item(rlc_enh_data_ptr->complete_pdu[i].iovec.ptr,
                      rlc_entry, __LINE__);
    }
  }
}

/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_handler_mac

DESCRIPTION
    - Process TM data, if present and configured to process in interrupt context
    - If some data present, 
        - Enqueue the RLC entry into rlc_enh_data_queue
        - Post a signal to RLC DL Task to process in DL Task context
      else
        - Return the entry to free queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_dl_pdu_data_ind(rlc_enh_data_ind_type *rlc_enh_data_ptr)
{
  if(rlc_debug_var & RLC_DBG_MEM_LEAK)
  {
    rlc_touch_entry_dsm_item(rlc_enh_data_ptr);
  }

  /* Process TM data, if configured and present */
  rlc_enh_handle_tm_data(rlc_enh_data_ptr);
  
  if (rlc_enh_data_ptr->num_complete_pdus > 0)
  {
    /* Enqueue the RLC entry into rlc_enh_data_queue */
    q_put(&rlc_enh_data_q, &(rlc_enh_data_ptr->link));
    
    /* Post a signal to RLC DL Task to process in DL Task context */
    (void) rex_set_sigs(wcdma_l2_dl_get_tcb(), RLC_DL_DATA_IND_SIG);  
  }
  else
  {
    /* If all the PDUs are consumed, add the rlc_enh_data_ptr to free queue */
    q_put(&rlc_enh_free_data_q, &(rlc_enh_data_ptr->link));
  }
}

/* DSM touch function for every DSM item */
static void rlc_touch_entry_dsm_item_hs
(
  l1_dec_hs_tsn_info_type *rlc_enh_data_ptr
)
{
  uint32 i;

  for (i = 0; i < rlc_enh_data_ptr->num_complete_pdus; i++)
  {
    if (rlc_enh_data_ptr->complete_pdu[i].iovec.offset == 0)
    {
      dsmi_touch_item(rlc_enh_data_ptr->complete_pdu[i].iovec.ptr,
                      rlc_entry, __LINE__);
    }
  }
}

void  rlc_dsm_touch_exit
(
  dsm_item_type *pkt_head_ptr
)
{
  while (pkt_head_ptr != NULL)
  {
    dsmi_touch_item(pkt_head_ptr, rlc_exit, __LINE__);
    pkt_head_ptr = pkt_head_ptr->pkt_ptr;
  }
}

/*===========================================================================

FUNCTION rlc_enh_dl_tsn_flush_hs

DESCRIPTION
    - Flush the TSN from TSN q
    - 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  There might be slight data loss dueing HS STOP

===========================================================================*/
void rlc_enh_dl_tsn_flush_hs(void)
{
  l1_dec_hs_tsn_info_type *tsn_info_buf_ptr = NULL;

  L2_MUTEX_LOCK(&rlc_tsn_mutex);
  
  while ((tsn_info_buf_ptr = (l1_dec_hs_tsn_info_type *)q_get(&rlc_tsn_info_q)) 
             != NULL)
  {
    /*Discard the DSM item and enqueue in free q*/
    mac_hs_dl_discard_tsn(tsn_info_buf_ptr);
    mac_hs_enqueue_free_tsn(tsn_info_buf_ptr);
  }
  
  L2_MUTEX_UNLOCK(&rlc_tsn_mutex);
  
}

/*===========================================================================

FUNCTION rlc_enh_dl_pdu_data_ind_hs

DESCRIPTION
    - Enqueue the tsn entry into rlc_tsn_info_q
    - Post a signal to RLC DL Task to process in DL Task context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_enh_dl_pdu_data_ind_hs(l1_dec_hs_tsn_info_type *tsn_info_buf_ptr)
{

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
  uint8 array_len = 0;

  if (tsn_info_buf_ptr->num_partial_pdus != 0)
  {
    ERR_FATAL("TSN %d - Not expected Num partial in RLC - Partial %d, Complete %d", 
               tsn_info_buf_ptr->tsn, tsn_info_buf_ptr->num_partial_pdus, 
               tsn_info_buf_ptr->num_complete_pdus);
  }

  array_len = sizeof(tsn_info_buf_ptr->complete_pdu)/sizeof(complete_pdu_type);
  /*Corner case: If the tb_id is set to 0xFF discard the last complete PDU which is p-t-c 
    which failed PDU decode*/
  if ((tsn_info_buf_ptr->tb_id == 0xFF) && (tsn_info_buf_ptr->num_complete_pdus > 0) && 
      (tsn_info_buf_ptr->num_complete_pdus < array_len))
  {
     rlc_discard_dsm_item(
     &(tsn_info_buf_ptr->complete_pdu[tsn_info_buf_ptr->num_complete_pdus-1].iovec.ptr));
     tsn_info_buf_ptr->num_complete_pdus--;
     tsn_info_buf_ptr->tb_id = 0;
  }
#endif

  if (tsn_info_buf_ptr->num_complete_pdus == 0)
  {
    //MSG_HIGH("TSN %d with no complete pdus",tsn_info_buf_ptr->tsn,0,0);
    /* If all the PDUs are consumed, add the tsn_info_buf_ptr to free queue */
    mac_hs_enqueue_free_tsn(tsn_info_buf_ptr);
  }
  else
  {
    /*Set the tarcer value to DL_TSN_RLC_Q*/
    tsn_info_buf_ptr->tracer = DL_TSN_RLC_Q;

    if(rlc_debug_var & RLC_DBG_MEM_LEAK)
    {
       rlc_touch_entry_dsm_item_hs(tsn_info_buf_ptr);
    }    

    /* Enqueue the tsn entry into rlc_tsn_info_q */
    q_put(&rlc_tsn_info_q, &(tsn_info_buf_ptr->link));
    
    /* Post a signal to RLC DL Task to process in DL Task context */
    (void) rex_set_sigs(wcdma_l2_dl_get_tcb(), RLC_DL_DATA_IND_SIG);  
  }
}

/*===========================================================================

FUNCTION RLCI_DEBUG_TRACE_PRINT_DATA

DESCRIPTION
  To print the contents of the data in the item_ptr.
  P.S: Needed trace length needs to be passed in number of words
  
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void rlci_debug_trace_print_data(dsm_item_type *walk_item_ptr, 
                                 uint16         needed_trace_length)
{
  uint32        temp_hdr[3];
  uint8         temp_i;
  uint16         length;

  length = ((needed_trace_length > MAX_DATA_LOGGING) ? MAX_DATA_LOGGING : needed_trace_length);
  MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "PDU/SDU contents dumped %d words",
                                             length);
  temp_i = 0;
  while(dsm_extract(walk_item_ptr, temp_i*4, temp_hdr, 12) && (length/3 > 0))
  {
    MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC DATA : 0x%x 0x%x 0x%x",
              RLC_DEBUG_CHANGE_ENDIANNESS(temp_hdr[0]), 
              RLC_DEBUG_CHANGE_ENDIANNESS(temp_hdr[1]), 
              RLC_DEBUG_CHANGE_ENDIANNESS(temp_hdr[2]));
               
    temp_i = temp_i + 3;
    length =  length - 3;
  }

  if (length > 0)
  {
    MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC DATA : 0x%x 0x%x 0x%x",
               RLC_DEBUG_CHANGE_ENDIANNESS(temp_hdr[0]), 
               RLC_DEBUG_CHANGE_ENDIANNESS(temp_hdr[1]), 
               RLC_DEBUG_CHANGE_ENDIANNESS(temp_hdr[2]));
    //length--;
  }
}

#ifdef FEATURE_RLC_QXDM_LOGGING
/*===========================================================================

FUNCTION RLCI_GENERATE_DL_CONFIG_EVENT

DESCRIPTION
  To generate an event whenever a DTCH logical channel is established, modified
  or restablished, to aid in automation. The argument rlc_size is used only
  in the case the UM RLC entity to pass the size to this function, as this
  information is not available in the control block.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void rlci_generate_dl_config_event(rlci_dl_ctrl_blk_type *ctrl_blk,
                                   uint32 rlc_size) 
{
  rlci_event_wcdma_rlc_config_type event_wcdma_rlc_config_data;
  //Initialization
  event_wcdma_rlc_config_data.rlc_id = 0;
  event_wcdma_rlc_config_data.direction_and_rlc_type = DOWNLINK_FIXED_SIZE;
  event_wcdma_rlc_config_data.size =0;
  if(UE_LOGCHAN_DTCH == ctrl_blk->lc_type)
  {
    switch (ctrl_blk->lc_mode)
    {
      case UE_MODE_UNACKNOWLEDGED:
        event_wcdma_rlc_config_data.rlc_id = ctrl_blk->ctrl.um_ctrl.rlc_id;
        event_wcdma_rlc_config_data.direction_and_rlc_type = DOWNLINK_FIXED_SIZE;
        event_wcdma_rlc_config_data.size = (uint16)rlc_size;
        break;
                          
      case UE_MODE_ACKNOWLEDGED:
      case UE_MODE_ACKNOWLEDGED_DATA:
        event_wcdma_rlc_config_data.rlc_id = ctrl_blk->ctrl.am_ctrl.am_common->dl_data_id;
#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
        if (ctrl_blk->ctrl.am_ctrl.dl_ctl_cnfg->am_pdu_size_flexbile == TRUE)
        {
          event_wcdma_rlc_config_data.direction_and_rlc_type = DOWNLINK_FLEXIBLE_SIZE;
          event_wcdma_rlc_config_data.size =
            ( ctrl_blk->ctrl.am_ctrl.dl_ctl_cnfg->am_li_type == LI_7_BITS)?7:15;
        }
        else
#endif
        {
          event_wcdma_rlc_config_data.direction_and_rlc_type = DOWNLINK_FIXED_SIZE;
          event_wcdma_rlc_config_data.size = (uint16)ctrl_blk->ctrl.am_ctrl.dl_ctl_cnfg->dl_rlc_size;
        }


        break;
      default:
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR:Invalid rlc mode for rlc config event %d",
                                                    ctrl_blk->lc_mode);
        return;
    }
    MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "rlc config event rlc id %d direction and rlc type %d size %d",
             event_wcdma_rlc_config_data.rlc_id,
             event_wcdma_rlc_config_data.direction_and_rlc_type,
             event_wcdma_rlc_config_data.size); 
    event_report_payload ( EVENT_WCDMA_RLC_CONFIG, 4, &event_wcdma_rlc_config_data);
  }
}

/*===========================================================================

FUNCTION RLC_ERROR_LOG_SUBMIT

DESCRIPTION
  Function used to log the following RLC error conditions described in 
  section 11.3.4 of 25.322

PARAMETERS  
1. dl_rlc_data_id: Indicates DL rlc logical channel ID; Value starts from > 16 for AM channels
2. rlc_sn: Indicates the erroneous PDU just ignored by UE RLC; Range "0 - 4095" 
           and value of "0xFFFF" indicates that the SN is not-applicable for this
           error condition
3. rlc_err_reason: The below table gives the mapping of the error number 
to error description:
    0. PDU outside window
    1. Timer Discard timeout
    2. Invalid length
    3. PDU duplicates
    4. Buffer full
    5. Invalid size
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void rlc_error_log_submit(rlc_lc_id_type dl_rlc_data_id, uint16 rlc_sn, 
                          rlc_err_reason_type rlc_err_reason)
{
  /* logging code to be added here */
  
}

#endif

/*===========================================================================

FUNCTION RLCI_RESET_STATUS_TIMER_CMD

DESCRIPTION
  This function would process a RLC AM RESET status timer command given to 
  L2 UL task to perform timer from R99-ISR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  rlci_reset_status_timer_cmd()
{
  rlci_dl_ctrl_blk_type *rlc_dl_ctl_blk_ptr;
  uint16 i;

  L2_ACQ_DL_LOCK();

  for (i=0; i<UE_MAX_AM_ENTITY; i++)
  {
    if (rlc_rst_satus_tmr_tbl[i] != INIT_CTL_BLK_ID)
    {
      rlc_dl_ctl_blk_ptr = rlci_dl_get_ctl_blk_ptr(rlc_rst_satus_tmr_tbl[i]);
      
      if(NULL == rlc_dl_ctl_blk_ptr)
      {
        MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC-UL LC Id:%d not found",
                                                    i);
        continue;
      }
      rlc_rst_satus_tmr_tbl[i] = INIT_CTL_BLK_ID;

    /*---------------------------------------------------------------------
      If Status prohibit timer is active for the channel, re-start the
      timer.
    ---------------------------------------------------------------------*/
      if(rlc_dl_ctl_blk_ptr->ctrl.am_ctrl.dl_state_var.status_prohibit)
      {
        rlc_dl_ctl_blk_ptr->ctrl.am_ctrl.dl_state_var.status_prohibit_cnt = 0;
      }
    }
  }
  
  L2_ACQ_DL_UNLOCK();
}

/*===========================================================================

FUNCTION RLC_UL_ENQ_DSM_ITEM_TO_WM

DESCRIPTION
  API to check for SDU length boundary check and enqueue to DL WM queue.

DEPENDENCIES
  None.

RETURN VALUE
  Returns SDU length that is enqueued to WM
  Returns 0 if SDU is not within boundary.

SIDE EFFECTS
  None.

===========================================================================*/

uint16 rlc_ul_enq_dsm_item_to_wm 
(
  rlc_lc_id_type         rlc_id,
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr, 
  dsm_item_type         *sdu_ptr
)
{
  uint16 sdu_len;

  sdu_len = dsm_length_packet(sdu_ptr);
  
  if ( (sdu_len > 0 ) && (sdu_len <= RLC_MAX_SDU_SIZE))
  {
    dsm_enqueue(ctrl_blk_ptr->dl_wm_ptr, &sdu_ptr);
    ctrl_blk_ptr->n_sdu++;
    if(rlc_debug_var & RLC_DBG_DL_WM_CNT)
    {
      MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "RLC_WM_CNT: Wm_Count - %d, number of SDUs - %d",
                                                 ctrl_blk_ptr->dl_wm_ptr->current_cnt,
                                                 ctrl_blk_ptr->n_sdu);
    }
  }
  else
  {
    MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "RLC_ERR: LCID %d Discard SDU due to invalid length %d, sdu_ptr 0x%x",
                                                 rlc_id,sdu_len, sdu_ptr);
    rlc_discard_sdu_ptr(&(sdu_ptr));
    sdu_len = 0;
  }
  return sdu_len;
}

/*===========================================================================
FUNCTION RLC_UL_AM_CHAIN_DSM_ITEM

DESCRIPTION
  Function to check for SDU length boundary check and chain the DSM item to 
  chain_tail_ptr if SDU length is valid.

RETURN VALUE
  Returns SDU Length if valid, else returns 0 if SDU length is not within boundary.
===========================================================================*/

uint16 rlci_ul_am_chain_dsm_item
(
  rlci_dl_ctrl_blk_type  *ctrl_blk_ptr,
  dsm_item_type          *sdu_ptr,
  dsm_item_type         **chain_head_ptr,
  dsm_item_type         **chain_tail_ptr
)
{
  uint16 sdu_len;
  rlc_lc_id_type rlc_id;
  
  rlc_id = ctrl_blk_ptr->ctrl.am_ctrl.am_common->dl_data_id;
  sdu_len = dsm_length_packet(sdu_ptr);
  dsm_item_set_pkt_len(sdu_ptr, sdu_len);
  
  if ( (sdu_len > 0 ) && (sdu_len <= RLC_MAX_SDU_SIZE))
  {                      
    if (*chain_head_ptr == NULL) 
    {
      *chain_head_ptr = *chain_tail_ptr  = sdu_ptr;
    }
    else
    {
      dsm_pkt_chain_link(chain_tail_ptr, sdu_ptr);
    }               
  }
  else
  {
    MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR,"RLC_ERR: LCID %d Discard SDU due to invalid length %d, sdu_ptr 0x%x",
                                                 rlc_id, sdu_len, sdu_ptr);
    rlc_discard_sdu_ptr(&(sdu_ptr));
    sdu_len = 0;
  }
  return sdu_len;
}

#if ((defined FEATURE_DUAL_SIM_WCDMA_TUNEAWAY) || (defined FEATURE_WCDMA_HS_FACH_DRX))
/*===========================================================================

FUNCTION RLC_CHECK_PENDING_DL_SDU

DESCRIPTION
  Checks if there are any PDUs yet to be received or processed
  for a complete SDU.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if more PDUs are expected for a SDU
  FALSE - otherwise 
  
SIDE EFFECTS
  None.

===========================================================================*/

boolean rlc_check_pending_dl_sdu
(
  rlc_lc_id_type lc_id
)
{
  rlci_dl_ctrl_blk_type   *ctl_blk_ptr = NULL;
  rlci_dl_am_state_type   *dl_state_ptr = NULL;
  uint16 index = 0;
  boolean ret_val = FALSE;

  L2_MUTEX_LOCK(&rlc_channel_release_mutex);
  if ((INIT_CTL_BLK_ID == dl_ctl_blk_id_tbl[lc_id]) || 
      ((ctl_blk_ptr = get_am_ctl_blk(lc_id)) == NULL))
  {
    MSG_1(MSG_SSID_WCDMA_RLC, MSG_LEGACY_ERROR, "UL LC ID %d not present, ctrl_blk_ptr is NULL",
                                                lc_id);
  }
  
  if(ctl_blk_ptr != NULL)
  {
    if ( (UE_MODE_ACKNOWLEDGED == ctl_blk_ptr->lc_mode) && (ctl_blk_ptr->ctrl.am_ctrl.dl_reseq_q != NULL) )
    {
      dl_state_ptr = &(ctl_blk_ptr->ctrl.am_ctrl.dl_state_var);
      
      index = RLCI_GET_RESEQ_Q_IDX(dl_state_ptr->old_vr_r);
       
      if ((ctl_blk_ptr->ctrl.am_ctrl.dl_reseq_q->pdu_info[index].dsm_ptr != NULL) || 
         (ctl_blk_ptr->ctrl.am_ctrl.dl_reseq_q->partial_sdu_ptr != NULL) || 
         (dl_state_ptr->vr_r != dl_state_ptr->vr_h))
      { 
        MSG_3(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "pending msg on AM DL old_vr_r|vr_r 0x%x, vr_h 0x%x, partial_sdu_ptr 0x%x",
                                                   ((dl_state_ptr->old_vr_r<<16)|dl_state_ptr->vr_r),
                                                     dl_state_ptr->vr_h,
                                                     ctl_blk_ptr->ctrl.am_ctrl.dl_reseq_q->partial_sdu_ptr);
        ret_val = TRUE;
      }
    }    
    else if ((UE_MODE_UNACKNOWLEDGED == ctl_blk_ptr->lc_mode) &&
       (NULL != dl_um_pdu_info.dsm_ptr))
    {
      MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "pending msg on UM DL dl_um_pdu_info.dsm_ptr 0x%x, lc_id %d",
                                                 dl_um_pdu_info.dsm_ptr,
                                                 lc_id);
      ret_val = TRUE;
    }
    else
    {
      MSG_2(MSG_SSID_WCDMA_RLC, MSG_LEGACY_HIGH, "LC id %d, LC mode %d",
                                                 lc_id, ctl_blk_ptr->lc_mode);
    }    
  }

  L2_MUTEX_UNLOCK(&rlc_channel_release_mutex);
  return(ret_val);
}
#endif

#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*===========================================================================

FUNCTION RLC_CHECK_PENDING_DL_ACTIVE_RB_DATA

DESCRIPTION
  Checks if there are any PDUs yet to be received or processed
  for all active DL RB's.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if more PDUs are expected for a SDU
  FALSE - otherwise 
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rlc_check_pending_dl_active_rb_data(void)
{
  boolean ret_val = FALSE;
  uint8 active_rb = downlink_table.active_ch;
  uint8 ctrl_blk_idx = INIT_CTL_BLK_ID;
  uint8 lc_idx = 0;
  rlci_dl_ctrl_blk_type *ctrl_blk_ptr = NULL;
  
  
  while ((active_rb > 0) && (lc_idx < UE_MAX_UL_LOGICAL_CHANNEL))
  {
    if ((ctrl_blk_idx = dl_ctl_blk_id_tbl[lc_idx]) != INIT_CTL_BLK_ID)
    {
      active_rb--;
      ctrl_blk_ptr = &(downlink_table.ctrl_blk[ctrl_blk_idx]);
      if ((ctrl_blk_ptr->lc_mode == UE_MODE_ACKNOWLEDGED) ||
          (ctrl_blk_ptr->lc_mode == UE_MODE_UNACKNOWLEDGED))
      {
        ret_val = rlc_check_pending_dl_sdu((rlc_lc_id_type)lc_idx);
      }
      if (TRUE == ret_val)
      {
        break;
      }
    }
    lc_idx++;
  }
  return ret_val;
}
#endif /* FEATURE_WCDMA_HS_FACH_DRX */