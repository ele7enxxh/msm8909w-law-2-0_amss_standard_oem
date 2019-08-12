#ifndef MCCDMA_I_H
#define MCCDMA_I_H
/*===========================================================================

    M A I N   C O N T R O L   T A S K   C D M A    S U B - S Y S T E M
                       H E A D E R    F I L E

DESCRIPTION
  This file data and function declarations necessary for the CDMA Sub-system
  of the Main Control task.

  Copyright (c) 1990 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccdma.h_v   1.53   02 Oct 2002 13:49:34   sbrelsfo  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccdma_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/21/16   sjo     Dont DTX frames while CFS is in progress.
--------   ---     ------------------------------------------------------------
07/21/16   ppr     1x2GTA:Buffer UHDM msg if CFS in prog
06/30/16   ppr     Chgs to add RatMask payload to Incoming_page call rpts to CM
06/28/16   sjo     Debug buffer enhancements. Added more confirmation rpts to
                   mmoc_cnf_dbg_buf. Added mc_dbg_msgr_q debug buffer also.
05/16/16   ppr     Chgs to honor UE mode change and 1X_to_LTE_RESEL Abort reqs
04/22/16   ppr     1X2GTA:Chgs to save rcvd ESCAM OTA msg.
12/11/15   ppr     1xSRLTE:Chgs to remove base_id rel code for allowing 
                   Access HO back to SRLTE MT page rcvd BS
09/30/15   ppr     New EFS Item chgs for 1xSRLTE MT Page Acc HO feature
09/04/15   ppr     1xSRLTE:Chgs to allow Access HO back to SRLTE MT page rcvd
                   BS during page resp even if it is disabled from BS side
03/10/16   sjo     1X2GTA: Buffer the fade timeout cmd coming during QTA.
03/09/16   ppr     1x2GTA Feature changes
02/18/16   bb      UE side enhancement chgs to fix same NGBR_CONFIG=0 PN's 
                   having diff Paging Channels configured in NL ovhd msg
02/17/16   agh     Optimization to add previous serving channel to NL rxed on
                   current serving channel after handoff if not already present
10/21/15   sjo     Support for FR-29020: Power Saving Mode w/o NW coordination
09/16/15   ppr     Reg Throttling chgs for Transceiver request denial cases
09/16/15   agh     For Split overheads, honor system Release from SRCH only
                   if at least one OTA is received in current wakeup cycle
09/16/15   ppr     Chgs to avoid sys lost due to BS insanity timer expiry 
                   while in split ovhd collection
09/16/15   agh     Added Diag cmd to modify Ovhds Refresh time for testing
09/09/15   agh     Changes to support Voice Adapter based Audio architecture
08/27/15   ppr     Increased MCC_MAX_DTMFS_OUT to 30
06/11/15   sjo     Enhance debugging interface between MMOC and CP
03/13/15   ppr     Chgs to honor "acquired_rf_resources" flag in 
                   SRCH_ACCESS_READY_R rpt from SRCH
02/27/15   ppr     Timer based mech to clean up bs_info[] heap mem
01/23/15   agh     Enhance debugging interface between SRCH and CP
12/29/14   agh     Validate ovhds for T42/fade timeouts based on ovhds expiry
12/03/14   agh     Maintain flag for SRCH to differentiate between SRLTE and 
                   split overheads cases of forced sleep
12/02/14   agh     Support for new customer specific voice service option
11/19/14   agh     Call cleanup in case of CSFB MT and MO call race condition
11/11/14   ppr     Changes to handle n/w Err REDIR to HDR after MT CSFB ESR
11/11/14   agh     New API for SRCH to check for cached ovhds
11/11/14   agh     Don't perform SPM validation for traffic to Idle transition
11/11/14   gga     Changes to have Efficient Piecewise Aggregation of 
                   Overhead Messages 
11/11/14   agh     1X overheads optimizations and direct TC->IDLE transition
09/03/14   agh     Fixed p_rev synchronization issues betwen 1X-CP and DS
08/06/14   agh     Debugging enhancement for MMOC sanity timer related cmds 
07/21/14   gga     FR 20298: SIM Swap without RESET if carrier settings have 
                   changed
07/17/14   ppr     Added CALL_END cmd support for MT call in Access state
06/23/14   ppr     Graceful handling for NV Read/Write error in Card mode
04/10/14   ssh     Process MMOC/SRCH commands while waiting for page_cnf.
03/14/14   ssh     Fix to perform the Timer Based registration in SRLTE mode.
03/27/14   ssh     1xSLTE changes to relase sec chain before asking for pri.
03/19/14   ppr     SRVCC Feature Changes
03/18/14   agh     FR 16951: Avoiding BSR from C2K->LTE when UE supports 
                   C2K-LTE Cell Reselection
03/11/14   ssh     1xSRLTE: interface b/w MMODE & 1x to avoid mode switch while 
                   1x MT call is in progress.
03/04/14   gga     Added debugs to print band class and protocol revision.
03/03/14   ssh     1xSRLTE feature code.
02/13/14   ppr     New API for DS to register call back API to get notified 
                   with PREV in SYNC and ESPM.
01/08/14   agh     FR 15489: UIM Clients to Handle SIM BUSY Indications 
12/11/13   ppr     Changes to use  mmgsdi_session_read_prl()- API to read
                   NV#257 - NV_ROAMING_LIST_683_I in card mode
11/22/13   ppr     CSFB:1x CGI Reporting feature changes
10/31/13   gga     FR 17021: R8 CSFB Call setup optimization
10/10/13   gga     FR#3064: OOS Optimization for C+G DSDX
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
08/05/13   gga     Feature cleanup
06/27/13   ppr     Support for ignoring GSRDM msg
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
05/31/13   srk     Mainlined FEATURE_MODEM_1X_NON_SIG.
05/23/13   nee     FEATURE LTE Deprioritization changes.
05/02/13   ppr     SV mode - Reject 1x voice page while VOLTE call is active
04/12/13   gga     Added a handling when acknowledgement of access messages
                   is not received within a specified time.
03/29/13   gga     Added the default value of Pilot PN while initializing the
                   RCP data
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE
                   in Non-SV mode
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
02/28/13   ppr     PPP cleanup changes when moving from 1x to LTE in SV mode
02/12/13   ssh     Stored DBM content instead of pointer.
01/24/13   ppr     Moving EFS related reads to mc_init_after_task_start()
01/03/13   nee     Added a header file "stringl.h" for inclusion of memscpy().
11/19/12   ssh     Added SMS over CSFB support.
11/14/12   ppr     Phase2 - Changes for Specific call end reasons
11/08/12   ppr     Phase1 Changes for Specific call end reasons
09/27/12   jtm     Wrap dtmf buffer declaration with FEATURE_1X_CP_MEM_OPT.
09/25/12   ppr     Added defencive NULL ptr access check for cur_bs_ptr
09/15/12   ppr     FR#2370 - 1x memory optimization changes
09/04/12   jj      Customer feature to allow OTASP only when BIP on LTE/HDR is
                   not in progress.
07/24/12   jj      Changes to allocate call_id in in tc_inti state for all MT
                   calls and use the call_id assigned in tunnel mode for ECAM
                   AM 1.
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
06/28/12   ppr     Replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
06/22/12   ppr     Changes in MC for not to do ERR_FATAL for Illegal DTMF digit
06/22/12   ppr     FR-2454 NV Item support for OTASP_MAX_UNLOCK_ATTEMPTS
06/21/12   ssh     NV based disabling of 1X to LTE reselections.
06/15/12   ppr     Feature Cleanup
05/24/12   ppr     Feature Cleanup
05/09/12   ppr     Feature Cleanup
04/13/12   ppr     Reverting changes related to replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
                   so Rollback to previous Revision
04/10/12   ppr     Replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
03/13/12   jj      Changes to indicate Codec/Handoff events to upper layers.
01/11/12   jj      Changes to support SCI modification by QCHAT.
02/29/12   ssh     Added events for 1X to LTE reselection.
02/21/12   ppr     Feature Cleanup
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/04/12   ppr     Feature Cleanup
01/19/12   ppr     KDDI req FR1665 - Reject incoming SO35/36 call
                   if it is not supported
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
11/30/11   srk     Feature clean up: Mainlined and Internalized Features
08/29/11   cjb     Renamed FEATURE_MODEM_1X_IRAT_1XTOL to FEATURE_1X_TO_LTE.
08/01/11   jj      Support for SO33 in SVLTE scenarios.
06/14/11   ssh     Added ATIM support to perform the 1X to LTE reselection.
06/15/11   ag      Changes to handle GEM related failure scenarios.
06/01/11   jj      CSIM: Added changes to remove blocked client id registration
                   during task init time and move session id reg to a later
                   time.
04/29/11   ag      Support for NV based control for 1xAdvanced.
04/21/11   jj      CSFB: Added chanes to support SSD operation, Unique Challenge
                   in CSFB mode.
04/28/11   jtm     Added mc_i.h include.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/21/11   ag      Removed duplicate comments in revision history.
04/19/11   jj      CSFB: Code merge from rel/1H11 branch.
03/03/11   jj      CSFB: Added support for Reselection with time transfer
                   and ECAM paging channel assignment.
02/24/11   jj      CSFB: Added changes to support unified Time Transfer approach
                   for eCSFB UHDM based MO/MT calls.
04/18/11   ag      Support for power down and power up registration after
                   OTASP commit.
04/13/11   ssh     Added support to do the power-up reg in case of ignoring
                   a page due to DDTM settings.
03/10/11   ag      Used periodic timer mechanism provided by timer services
                   instead of REX timer for modem statistics reporting.
02/24/11   ssh     Removed the inclusion of unused header files.
02/23/11   ssh     Fixed the compiler warning.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
12/20/10   azh     Decoupled WMS APIs from 1XCP.
12/15/10   ag      SU level CMI clean up.
12/10/10   azh     Added overhead update to SD when the overhead config changes.
12/05/10   jj      CSFB: Added support for Rel8/Rel9 CSFB.
11/15/10   jtm     Initial SU API split for CP.
10/21/10   azh     Fixed Compiler warnings and lint warnings.
09/20/10   jj      CSFB:Added funtion to handle Activation report/cnf.
09/17/10   ag      Added support for dynamic DOG.
09/16/10   ag      Removed inclusion of voc.h if FEATURE_MVS_MIGRATE is enabled.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT
07/22/10   ag      Fixed issues in processing radio configration parameters.
07/22/10   ag      Added support for GEM reject cases.
05/19/10   ss      Added extern definition for mcc_is_voc_acquired flag.
05/11/10   jj      CSIM Fix: removed call to get card type.
05/05/10   ssh     Added mcc_get_last_info_rec_with_cm_rpt() function to get
                   the last rec, for which, MC should send report to CM.
04/26/10   jj      CSFB change: IRAT core services code update.
04/23/10   ag      Support for 1x Modem statistical report.
01/26/10   ssh     Added the support for blocking non-voice pages during
                   emergency callback mode.
01/21/10   ag      Added check to make sure RCs (11,8) are sent together as
                   preferred RCs in origination and page response messages.
01/15/10   jj      Implemented IRAT core services.
01/10/10   jj      CSIM P2 changes, migration to MMGSDI APIs
12/07/09   ag      Added support for reporting FER when it is unknown.
12/02/09   ssh     Removed unused function mcc_voc_selftest()
10/15/09   ag      Support for sending FER info to CM
09/28/09   ag      1X Advanced Signaling Support - Phase 2
09/23/09   ag      Added support for sending neighbor search mode to searcher
08/30/09   jj      Added include of uim_v.h as part of CMI.
06/24/09   pk      Added MEID_ME to CDMA structure for FEATURE_HWID_SUPPORT
06/02/09   jj      Moved a function from here to mccdma.h which gets used
                   outside modem AU.
04/09/09   ag      CMI phase2 changes.
03/20/09   jtm     Added txcmc_v.h include.
03/04/09   adw     Moved types exported via mclog to public header.
03/04/09   adw     Added dh_v.h include.
12/02/08   adw     Removed txcmc_v.h, file no longer exists.
12/02/08   adw     Removed rxcmc_v.h, file no longer exists.
06/26/08   bb      Changes for FEATURE_8K_STUFF_TO_BE_PURGED feature
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
01/16/08   pg/yll  Added support for FEATURE_FULL_TIME_SHDR_3 so that it
                   allows 1x to do acq on RX0 regardless of the state of DO.
12/14/07   an      Added Passport Mode
12/13/07   sb      Check ddtm only in Idle state, not System Access.
07/27/07   pg      Added mccdma_get_cur_bs_info() to pass current base station
                   info to upper layer.
06/14/07   an      Added support for White Grey Black Card Operation
05/14/07   pg      Do not let srch do QPCH after a failed reacq.
03/09/07   pg      removed dependency on WMS and make sure it only depends on
                   CM for SMS.
02/23/07   sb      Added support for operational mode feature.
02/03/07   fc      Corrected the ovhd rxed macros.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
10/27/06   fh      AKA/AES: changed CRC-8 calculation algorithm to handle more
                            general cases.
10/19/06   fc      Code review changes.
09/30/06   fc      Featurization changes.
09/30/06   fh      AKA/AES: moved the 8-bit CRC calculation from mccaes.c into
                            mccsup.c
09/19/06   fh      AKA/AES: added sa_msg_tx_msg to the data structure of
                    cdma_ms_type so that Security Mode Request Message can
                    be sent in CDMA_MSG sub state.
09/14/06   fh      AKA/AES: added a new field to aes_status to carry the
                            crypto-sync for AES encryption/decryption use.
08/04/06   fh      AKA/AES: cleaned up the usage of AES capability functions
07/17/06   fc      Optimize the implementation of pseudo bs.
07/13/06   fh      AKA/AES: Cleaned up AES capability helper functions.
07/11/06   fc      Updated function prototype for mcc_send_cm_srv_sys_info.
07/07/06   fc      Moved member with size dependent on JCDMA to the end of
                   cdma_ms_type so as to make cp libary code common to JCDMA
                   and non-JCDMA build.
07/06/06   fc      Renamed ABORT_RF_RESOURCE_LOST to ABORT_SRCH_REQUEST_SYS_RESTART.
06/29/06   fc      Obsolete offline_cmd from cdma structure.
06/23/06   pg      Added support for RPC.
06/22/06   jqi     Eliminate a copy of PRL in CP/OTASP module to reduce the
                   memory by the size of MAX PRL size and use SD PRL for
                   uploading.
06/19/06   fh      Added two function prototypes for mux files to encrypt/decrypt
                   voice service frames. Note that these two function prototypes
                   along with the two functions in mccdma.c are not under feature
                   FEATURE_IS2000_REL_A_AES on purpose. The function body has
                   the appropriate features to process the encryption/decryption.
06/15/06   fc      Added definition for pseudo pilot pn.
06/12/06   fc      Added support for service status order.
06/12/06   fc      Added mob_qos in cdma_ms_type.
06/09/06   fc      Moved all gsrm/egsrm related functions back to mccidl.c.
                   Fixed the data corruption issue of sharing a common data
                   structure for gsrm/egsrm and srdm processing.
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
06/02/06   fc      Cleanup umts interface.
06/01/06   an      Added the bypass registration indicator changes
05/31/06   pg      Changed function mcc_inform_sd_redirection() interface.
                   Release D EGSRM code cleanup.
05/30/06   fc      Added support of MC command processing in slotted mode.
05/30/06   fc      Removed obsolete element end_by_data from cdma_ms_type.
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/19/06   fc      Added enum mcc_tranceiver_request_status_type.
05/18/06   fc      Added mcc_tx_int_msg and mcc_rx_int_msg.
05/17/06   fh      Merged in new development of AKA/AES features
04/26/06   an      Fixed Lint Errors
04/24/06   fc      Fixed Rel A featurixation.
04/12/06   pg      Added support for release D EGSRM.
03/14/06   sb      Added fix for KDDI related to high persistence.
03/01/06   fh      Added AES Support
02/16/06   dna     Avoid HDR doing extra PCCC negotiation.
02/07/06   fc      Mainlined FEATURE_MC_SUPPORTS_SHDR.
02/07/06   fc      Updated function prototypes.
02/13/06   an      Added ECCLM Cross Band Hashing Support
01/20/06   fc      Merged the following :
           yll     Added support for AKA.
01/06/06   fc      Cleanup on OOS changes.
11/03/05   an      Moved MCC_CHK_PTR macro definition to mc.h from mccdma.h
10/28/05   an      Made band class/subclass code review changes
10/19/05   sb      Removed MAX_BANDS_IN_GROUP.
10/15/05   fc      Cleanup NCC featurization.
10/14/05   an      Added Band Class/Subclass Reporting Feature.
07/26/05   fc      Fixed lint errors.
07/25/05   fc      Changes for Rel D Addendum f-csh messages.
07/22/05   va      Removed is_jcdma_emergency flag
07/15/05   fc      Fixed lint errors.
07/11/05   fh      Cleaned up header file including from MUX:
                     changed rxc.h to rxcmc.h
                     changed txc.h to txcmc.h
07/08/05   fh      Added new definitions for SHDR pahse II
                     added boolean transceiver_request_pending into
                      the data structure cdma_ms_type
                     removed function mcc_send_srch_access_cmd
                     added the following two functions:
                       mcc_wait_transceiver_resource
                       mcc_request_transceiver_resource
07/18/05   fc      Removed multiple definition of meid to fix compile error.
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
05/20/05   pg      Removed FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Renamed FEATURE_JUMP to FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Added support for early jump to hyperspace under FEATURE_JUMP.
04/27/05   az      Submitted changes for CR 62361. This also requires
                   corresponding changes in NV. Changed the name of a #define
04/25/05   fc      Merged the following from MSMSHARED:
04/20/04   fc       Changes for the update of reg_sid, reg_nid, reg_reg_zone,
                    auth and rand during AHO/APHO.
04/04/05   fc      Merged the following from MSMSHARED:
03/31/05   fc       Added support for EPZID.
03/28/05   fc      Added support for DCA.
03/25/05   az      Changed the name from MCC_USE_DEFAULT_SCI to MCC_NULL_SCI.
03/25/05   az      Created a #define for initial value of last_reg_sci.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/14/05   an       Added special dup detection function for ecam
02/18/05   fc      Merged the following from MSMSHARED:
02/09/05   fh       changed including of ds.h to be ds707.h, as data team requested
02/08/05   an       Mainlined IS95B features
02/09/05   va      Merged following from REL_B_C archive:
02/03/05   fc       Fixed lint error.
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
01/10/05   va      Merged following from REL_B_C archive:
12/16/04   an        Added hashed_cdmach to cdma_bs_type
12/06/04   yll       Added interface change for apiOne.
12/22/04   pg      Moved mcc_flush_rxtx_q() to mccsup.c.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
10/29/04   pg      Added support for bypass 2 good frames at TCI state.
10/25/04   az      Supported for Rel C sdb_supported bit in ITSPM and UHDM
09/27/04   va      Merge from REL B/C archive
07/23/04   yll      Mainlined the FEATURE_INT_ROAMING.
07/21/04   sj       Added support for umts to 1x handover.
07/12/04   pg       Added is_cdma_subs_avail to cdma_ms_type to indicate
                    current CDMA/AMPS/HDR subscription availability status.
09/21/04   az      Changed feature name from FEATURE_REL_D_NEG_SCI to
                   FEATURE_IS2000_REL_D_NEG_SCI
09/16/04   fc      Added support for reverse DV.
08/17/04   az      Rel D Negative SCI changes
08/09/04   fc      Service negotiation changes for Release C and D.
08/09/04   bkm     Converted T50 and T51 timers to callback timers.
08/03/04   ht      Added Rel D MEID LAC support
06/16/04   va      Added tc_tx_on and tc_tx_to_be_on variables
06/16/04   fc      Bundle Aset update with channel config update.
                   Rel C addendum changes on ECAM.
06/14/04   fc      Renamed sr_id_included to is_sr_id_valid.
06/08/04   sb      Fixed featurization of long_code struct.
05/28/04   bkm     Merged from IS2000_REL_B_C:
05/17/04   sb      Typedef'd off_time structure.
05/17/04   va      Merged following from MSMSHARED:
03/11/04   yll      Added support for KDDI requirement to ignore async data
                  and async fax 1x pages during HDR connected state.
03/05/04   bkm      Lint cleanup.
04/09/04   sb      Typedef'd long_code structure
03/36/04   bkm     Merged from IS2000_REL_B_C
03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
03/19/04   va      Merged following from MSMSHARED
02/27/04  jrp     Added ABORT_UNSUPPORTED_PARAMETER to access_result_type for
               case of unsupported REACH parameters.
02/24/04  RC      Added support for FEATURE_INT_ROAMING
02/27/04  bkm     Moved typedef for access_result_type from mccsa.c to this file.
                  Added extern def of calc_psist(). These are now used in mccreg.c.
03/12/04   fc      Added the definition of MCC_PRI_BCCH_BCN.
02/09/04   va      Consolidated TC to Idle related info in a separate
                   data structure
02/05/05   bkm     Merged from IS2000_REL_B_C
01/09/04   yll     Fixed a compile error when FEATURE_BROADCAST_SMS is
                     undefined.
02/05/04   bkm     Removed obsolete macro ALLOCATED_CDMA_CHN that always
                   returned true.  Many MSMs ago it was decided that L3
                   should not verify channels.
01/05/04   bkm     Added boolean tc_tx_on to cdma_ms_type.
11/13/03   yll     Added support for JCDMA emergency calls.
10/30/03   az      Added function declaration mcc_is_current_band_class_in_group
                   for FEATURE_ENHANCED_BAND_CLASS
10/23/03   va      Added New fields for TC to Idle in cdma_ms_type
10/23/03   yll     Added support for updating HDR PANID.
10/23/03   bkm     Added types for enhanced Idle and Access HO events.
10/15/03   is      Added stale_info in mcc_pd_bs_info_type for
                   FEATURE_GPSONE_BS_INFO
10/03/03   yll     Added mccidl_is_fast_raho().
10/03/03   bkm     Using explicit values to define enum values in
                   mccdma_page_match_result_type to potentially allow
                   for bit-mask like operations.
09/11/03   yll     Added FEATURE_ACP to analog code.
08/25/03   anr     added changes to support setting base_lat and base_long
                   in BSINFO struct.
09/04/03   fc      Added ruim_id_avail to CDMA structure.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/11/03   az      Added support for FEATURE_MODIFIED_SCI
08/08/03   fc      Renamed caii_sec_mode_msg_type to caii_sec_mode_type.
08/06/03   sb      Added support for CDMA Off Time Reporting.
07/30/03   az      Added support to return overflow size for MO SDB
07/30/03   az      Added support for FEATURE_SO_8026
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
05/27/03   az      Removed imsi_hash_key from QPCH featurization. This
                   to fix the incorrect reporting of Hash key during
                   RAHO.
04/14/03   va      Changed data structure for TC to idle transition.
04/07/03   bkm     Removed code for obsolete variable cdma_active_set.
03/21/03   fc      Removed qpch_bi_supported and qpch_power_level_bcast from
                   mcc_complete_sys_parms_type.
01/23/03   yll     Added mcc_sdb_supported function.
12/12/02   hxw     Added l2_time_out in mcc_pd_layer2_type.
11/27/02   ph      Fixed compile error.
11/25/02   va      Merged gpsOne force dormancy changes MSM archive.
11/18/02   va      Added prototype for mcc_send_orig_fail_retry_delay
11/14/02   sb      Featurized rand parameters under FEATURE_AUTH.
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   az      Removed the feature TMOBILE
10/31/02   az      Added support for HDR Data dedicated transmission mode
                   access prevention.
10/24/02   yll     Changed the mcc_bc_match prototype.
10/17/02   sb      Added access function for slot_cycle_index_p.
10/02/02   sb      Renamed is_silent_redial, silent_redial_count to orig_reason, orig_count.
10/01/02   az      Mainlined FEATURE_IS2000.
09/30/02   sb      Mainlined FEATURE_SD20.
09/04/02   AP      Included wmsbc.h for FEATURE_BROADCAST_SMS and
                   FEATURE_CDSMS support.
09/02/02   fc      Added macro NLM_MSG_IS_RECVD.
08/30/02   ts      Added support for Run Time R-UIM Enable.
08/13/02   HQ      Added FEATURE_CDSMS.
08/08/02   fc      Added function prototype mcc_update_acc_msg_seq.
08/01/02   fc      Changed mcc_parse_page function prototype.
                   Added mcc_increment_pag_3_or_fccch_3 function prototype.
07/10/02   fc/lc   Corrected MS unnecessary staying awake for changes in APM
                   and EAPM.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
06/27/02   fc      Changed UPM function names.
06/21/02   ph      changed cmmc.h to cmxll.h
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
06/12/02   yll     Added mccidl_assert_sleep_ok() prototype.
05/20/02   fc      Added support for Universal Page Message.
05/06/02   yll     Changed the broadcast SMS function names
04/30/02   sb      Added layer to payload of mcc_send_orig_failure.
04/12/02   lh      Added support for subaddress in Orig_C.
04/09/02   yll     Added Broadcast SMS support on IS2000 RelA common channel.
04/01/02   fc      Added function prototype mcc_is_upd_ovhd_msg_okay.
03/28/02   sb      Added mcc_rand_parms_type.
03/13/02   fc      RelA addendum 2 changes
03/09/02   lh      Merged from common archive.
02/13/02   ts      Include uim.h when FEATURE_UIM_RUIM is defined.  This allows
                   FEATURE_AUTH to be not defined with FEATURE_UIM_RUIM.
02/07/02   hxw     Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper
                   Base Station ID.
02/22/02   fc      Added support for Release A common channels.
02/13/02   va      Merged from MSM archive
02/06/02   sh      Added hybird operation support
01/31/02   hxw     Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper
                   Base Station ID.
02/07/02   lh      Added parameter to process_intercept function.
01/22/02   va      Fixed some errros for code to compile with REL_A turned off
01/14/02   va      Removed is_cur_bs_cs_supported function
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/13/01   va      Merged in from SVD branch.
11/30/01   va      Made "cs_supported" part of cdma_ms_type struct
08/28/01   lh      Support for concurrent service.
08/23/01   ph      Update to tc_so_change() for SVD.
11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
10/02/01   kk      Qos Parameters Added.
09/06/01   fc      Changed the function prototype of mcc_parse_page.
                   Removed the function prototypes is_in_access_ho_list and
                   is_access_ho_allowed.
                   Removed extern definitions sleep_slot_mask, bc_slot_cycle,
                   mccdma_bc_init and mccdma_bc_page.
08/22/01   sb      Changed encryption fields in mccdma_chnasn_am0_type and
                   mccdma_pc_ord_type.
08/16/01   kk      Merge from common archive.
                   - Defined a variable for RUIM.
08/08/01   yll     Merged in the following from the MSM archive:
                   Added esn_me and ruim_id to the cdma structure.
                   Rid unused mcc_ds_buf.
07/16/01   fc      Moved cai_fccch_rate_type to cai.h.
07/06/01   ph      Added support for Callback Timers
06/26/01   ph      Merged in from MSM_CP.05.00.81
06/11/01   fc      Added extern definition for mc_update_last_cdma_entry
                   (SD20 only).
06/06/01   lcc     Changed retry_type in retry_delay_type to cai_retry_order_type
                   and changed prototype of update_retry_delay to use
                   cai_retry_order_type.
05/29/01   fc      Added mcc_redir_info_type definition (SD20 only).
05/25/01   ts      Added an interface to the new UIM server.
06/14/01   va      Changed substate enums to reflect the new TC substates and
                   Call Control instance substates.
05/17/01   ph      Merged in from MSM_CP.05.00.76
04/17/01   va      Added nghbr_pilot_rec_type in the neighbor list.
                  Fixed bug in GNLM_MSG_IS_RECVD and GNLM_MSG_NOT_RECVD macro.
04/12/01   fc      Moved all events logging function data types / prototypes
                  to mclog.h and mclog.c.
04/10/01   lcc     Added some macros and changed macros RATE_SET_OF_FWD_RC and
                  RATE_SET_OF_REV_RC.
04/18/01   va      Changed "pilot_rec_type" name  in nl to "nghbr_pilot_rec_type"
03/28/01   va      Merged the folowing from common archive.
03/06/01 fc      Added support for events report: Message received, Message
                   transmitted, Timer expired, Counter threshold and Call
                   processing state changes.
03/07/01   ph      changed prototype of mcc_info_rec() to reflect new compute_independent_
                   msg_len() in mccsa.c
03/02/01   ph      Merged in from MSM_CP.05.00.55
02/16/01   fc      Added support for system determination 2.0. All changes
                  are featurized under FEATURE_SD20.
02/16/01   sh      added support for MC_END_F ack
01/24/01   ych     Merged T-53/JCDMA features for SK.
01/16/01   jq      Moved/renamed mccsch_retry_delay_type, mccsch_retry_delay to
                   here.
12/14/00   cah     Remove a uasmsi.h inclusion for FEATURE_SMS.
12/13/00   cah     Replaced obsolete smsi.h inclusion with uasmsi.h.
11/28/00   ck      Externalised the definition for RUIM Access buffer.
                   Added ruim_ota_features to the cdma structure.
02/27/01   kk      added new origination parameters to cdma_ms_type
02/26/01   ph      removed func mcc_check_ext_encryption_fields_is_okay() (rel A)
01/10/01   va      Added support for storing RELA overhead messages/info in the
                   BS info data structure.
01/10/01   ph      1. Added Encryption, SR3 fields to mccdma_chnasn_am0_type.
                   2. Added Encryption fields to mccdma_pc_ord_type.
                   3. Added Security Mode Command Message to mccdma_matched_msg_type.
                   4. Added prototype for mcc_check_ext_encryption_fields_is_okay().
11/13/00   lcc     Added function prototype for mcc_update_bs_p_rev.
11/01/00   lcc     Added drs member in cdms_ms_type to support DRS bit in ORM.
09/11/00   yll     Added support for Position Determination DBM.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
08/23/00   jq      Added mcc_set_rsch_dtx_sig ()
08/18/00   fc      Added retry_type and retry_delay to mccdma_pc_ord_type.
07/21/00   fc      Used enumeration type for FPC mode.
07/17/00   jq      added end_reason to cdma_ms_type.
07/14/00   ks      defined t_slotted in cdma_ms_type. Defined ecl_rx, egsd_rx
                   in cdma_msg_type. Define ecl, egsd in cdma_bs_type. These
                   fields support new PC messages and Slotted timer.
07/14/00   va      Added pwr_cntl_delay and rev_fch_gating_on in cdma_ms_type.
07/06/00   yll     Added the interface to provide the MSID and registered BS
                   info, include: band class, channel, bs ID, SID and NID.
06/26/00   sh      added AEHO_OVHD_MSG_OK
06/14/00   ks      Added band_class parameter to cdma_rf_init function.
06/04/00   jq      Changed parameter list of mcc_parse_page() and
                   mcc_detect_dup_bcast()
05/26/00   jq      Added slot_mask related defines and processing functions.
                   Fixed MAX_BC_PER_PAGE_MSG.
04/25/00   sh      Increase MC_MAX_BASES to 10
04/11/00   lcc     Added data structure and members to support QPCH.
04/06/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
           jq      Added Broadcast SMS feature support.
03/10/00   ry      Added IS2000 featurization
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ht      Add mcsyspr_set_last_cdma function prototype.
           ram     Fixed a typo in my last check-in which caused problems
                   in the SUPPORTED_BANDS, SUPPORTED_CDMA_BANDS, and
                   SUPPORTED_OP_MODES macros for trimode targets.
           ck      Added band class to the cdma_bs_type and also the to
                   the mcc_entry_init prototype.
           ram     Extended the Macros SUPPORTED_BANDS, SUPPORTED_CDMA_BANDS,
                   SUPPORTED_OP_MODES to include Trimode and Dual CDMA band
                   Targets.
01/26/00   va      Merged in changes from GV40401
                   Removed mcc_handoff_timer.
                   Modified SUPPORTED_OP_MODES to allow op codes 0 and 1 for
                   IS-95 MS in cellular or PCS band.
12/20/99   va      Added new data struct to store power control info temporarily.
                   Added fpc_subchan_gain to cdma_ms_type.
11/15/99   va      IS2000 cHANGES FOR VOICE.
                   Support for QOF.
                   mccdma_chnasn_type expanded for support of enhanced ECAM.
                   New mcros added for RC_CLASS() determination.
                   mcc_info_rec prototype changed.
10/28/99   lcc     Moved PLT related code to plt_mc.c.  Also added extern
                   function declarations for some functions used in plt_mc.c
08/24/99   fc      Removed nset_cnt from plt_rpt_ovhd_rec_type because SRCH
                   will provide the count of pilots in Pilot Strength
                   Measurement report.
08/21/99   ks      Modified plt_rpt_ovhd_rec_type and othrpt_pilot_info to
                   support pilot reporting for already attempted pilots.
08/13/99   lh      CM and SMS phase 2 implementations.
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
           ry      Fixed a merge problem in which a #endif for FEATURE_ACP
                   was misplaced
07/28/99   lh      CM and two-way SMS support added.
07/28/99   lh      CM support added.
07/22/99   fc      Changes in Pilot Reporting to support Access Probe Handoff.
07/19/99   abh     Changes for Access Probe Handoff.
07/09/99   ry      Added a new rex time for IS95B hard handoff
06/24/99   ks      Fixed a typo in AHO_OVHD_MSG_OK macro
06/22/99   kmp     Moved the definitions for IMSI_S1_ZERO, IMSI_S2_ZERO,
                   IMSI_MCC_ZERO and IMSI_11_12_ZERO to this file from
                   mccdma.c since these values are also used in mccidl.c in
                   function proc_true_imsi() when FEATURE_IS95B_TRUE_IMSI is
                   defined.
06/17/99   kmp     Merged in the following IS-95B changes.
06/13/99   fc      Changed the data structure of plt_rpt_ovhd_rec_type,
                   aho_pilot_info_type and othrpt_pilot_info.
06/07/99   ks      Added roam_indi field in mccdma_pc_ord_type structure.
06/04/99   jq      Added cdma_sa_orig_ministate_type for Release on Access
                   Channel feature
06/04/99   sh      changed data type from byte to int1 for add_intercept and
                   drop_intercept in mcc_ho_cb_type
05/26/99   fc      Added the support for closed loop power control step size
                   in the cdma_ms_type structure.
05/24/99   doj     Added p_rev_in_use to cdma_ms_type (for FEATURE_IS95B).
                   This was already done in the IS-95B branch is required
                   for all builds not just FEATURE_IS95B.
05/12/99   ks      Added Macros to check if AMPS is supported for a band class.
05/10/99   kmp     Merged the following IS-95B related changes and changed
                   T_SMS, T_ACP and T_AUTH to FEATURE_SMS, FEATURE_ACP,
                   and FEATURE_AUTH respectively.
           ks      Added AHO_OVHD_MSG_OK macro to validate overhead messages
                   during Access Handoff or Access probe Handoff.
05/06/99   kmp     Merged IS-95B changes below and changed T_SMS, T_ACP and
                   T_AUTH to FEATURE_SMS, FEATURE_ACP, and FEATURE_AUTH
                   respectively.
           kmp     Modified the mcc_pc_frames array to be 3D by adding
                   ADDR_TYPE checking for duplicate page messages for IS-95B.
           ks      Addded an external function is_in_access_ho_list.
           ks      Added an external function is_access_ho_allowed.
           kmp     Merged the changes listed below from the TGP IS-95B baseline.
           fc      Put in changes based on code review input.
           kmp     Merged the changes listed below from the TGP IS-95B baseline.
           fc      Put in changes based on code review input.
           jq      Modified some naming due to code review
           fc      Fixed lint error.
           ks      Removed CDMA_IDLE_AHO from cdma_idle_state_type enum.
           kmp     changed type of cdma.ho_state to cdma_ho_type from a word
                   Added include for mccsrch.h for pc_srch_rpt function proto.
           fc      Modified comments. Renamed plt_rpt_bookeep structure to
                   plt_rpt_ovhd_rec and pilot_rpt_bookeep structure to
                   pilot_rpt_ovhd_rec.
           jq      Added declaration for pc_srch_rpt, so lint is happy
           jq      Modified mccdma_chnasn_type, and channel assignment
                   related structures to accomodate both CAM and ECAM
           lh      Authentication using IMSI_M.
           fc      Defined ACCESS_HO_LIST, OTHER_REPORTED_LIST and
                   PLT_RPT_BOOKEEP.
           fc      Added PLT_RPT_BKP, ACC_HO_LIST and OTHER_RPTED_LIST data
                   types for pilot reporting. Added ACCESS_ATTEMPTED field in
                   CDMA_NGHBR_SET_TYPE to support pilot reporting. Defined
                   NO_PREV_PILOT. Defined ACC_MSG_NUM_PLT_ALLOWED() and
                   ACC_MSG_PLT_RPT_NEEDED() macros.
           ks      Removed CDMA_HO_STATE from main state enumerations.
           sh      Added mcc_send_srch_parms function.
           ks      Defined a new enumeration for various handoffs.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           kmp     Added Phased Deployment support, added PACA cancel substate
                   to the cdma_sa_state_type enum.
           sh      added IS95B soft handoff parameters
           ks      Added a new macro to check status of Nghbr List messages.
           ks      Added IS95B overhead message processing fields. Added fields
                   ho_sa_state and ho_state fields for Access Handoff. Added
                   cdma_prev_bs_info data type for holding previous BS info.
           lh      p_rev_in_use is added to cdma_ms_type structure.
           lh      IS-95B True IMSI implementation added.
06/21/99   lh      Two-way SMS and CM support.
06/01/99   sh      added mcc_check_alert_signal_info()
04/25/99   abh     Added prototype for mcc_init_ovhd_info, which was introduced to
                   move the initilisation of bs_info array at the start of CDMA Acquisition
                   process, rather than after getting Sync Message.
04/07/99   abh     Changes for the new Queue Interface.
03/18/99   ck      Added imsi_t variables, auth_data and spc_change_enabled
                   to cdma structure.
03/18/99   ck      Do not define mcc_voc_selftest if FEATURE_VOCODER_MANAGER.
                   Merged the change from SPSW rev 1.85 by ro
03/09/99   ram     Externalised mcc_mdr_duration_timer for IS95B MDR
02/19/99   ram     Externalised mcc_scam_start_timer for IS95B MDR
01/27/99   ck      mcc_pref_voice_so is ifdefed under FEATURE_EVRC_SO_MGMT
12/08/98   dna     Fixed some compile warnings (merged from ASW 1.11)
10/09/98   ck      Externalised mcc_pref_voice_so
09/15/98   pms     Backed out _near pointers
09/04/98   pms     Neared the variables to save ROM space.
08/25/98   ck      Added functions mcc_queue_dh_rpt() and mcc_dh_cmd() to
                   support the dh task.
08/24/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
08/20/98   ck      Removed mcc_voc_capability definition, no longer needed.
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
07/09/98   lcc     Fixed problem with illegal characters after #endif statement
07/02/98   pms     Added element "end_by_data" to cdma_ms_type struct.
07/02/98   pms     Added prototype for process_ds_call_failed().
06/25/98   kis     Added digit_mode and ASCII related fields in cdma_ms_type.
05/21/98   abh     MAX_SID_NID changed to MAX_HOME_SID_NID for SSPR-800
02/18/98   dna     Added SUPPORTED_CDMA_BANDS and BIT_MASK macro to allow
                   System Determination to run-time check for supported band.
01/27/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/27/98   ych     Featurized mcc_idle_timeout and defined a constant
                   MAX_DWORD_VAL. Also added customer.h to the include file
                   list
08/17/97   ych     Changed the MCC_VALID_OVHD_TIMEOUT timer value to 30000
                   from 10000.
07/15/97   dna     Moved SUPPORTED_BANDS and SUPPORTED_OP_MODES here.
06/16/97   dna     New func. sets hold_orig and timer when returning to idle
05/20/97   ck      Added support for LPM.
04/18/97   dna     Changed some #ifdefs to make class 1 IMSI its own feature
01/25/97   dna     More flexible reject of status request.
12/13/96   jca     Added new fields to mccdma_chnasn_am2_type.
10/18/96   dna     Added TEMP_NL_SEQ
09/13/96   dna     Added cdma.otasp_call flag.
09/11/96   jca     Changed mcc_xlate_dtmf() to mcc_encode_dtmf().  Added new
                   function mcc_decode_dtmf().
08/08/96   dna     Add Service Programming Code to cdma. structure for OTASP.
07/10/96   day     Add mcc_pc_pg_frames array for layer2 duplicate detection.
                   Add input parameter to identify page/non-page messages
                   in mcc_detect_duplicate.
06/25/96   dna     For FEATURE_OTASP, added mob_dir to cdma structure.
06/10/96   jpf     Removed BS tmsi_zone from cdma_ms_type.
05/28/96   dna     Added imsi_addr_num for class 1 IMSI support.
05/24/96   jjw     Gemini & ISS2 Data Services release.
05/15/96   dna     Changed auth_on to auth_mode and encryption_enabled to
                   encrypt_mode.
05/13/96   jca     Added band_class and block_or_sys to cdma_ms_type.
04/15/96   jca     Changed cdma_dtmf_type to accomodate queued Burst DTMFs.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
03/13/96   day     Added J-STD-008 PCS message modifications.  Using FEATURE_JSTD008
                   ifdef.
02/08/96   jca     Put ()s around macros to make Lint happy.
12/08/95   bns     Removed flag for ISS2 forced registration
12/06/95   bns     Added flag for ISS2 forced registration
11/29/95   jca     Deleted sid_acq field from cdma_ms_type.
11/14/95   jca     Removed support for configurable service option 1 def.
10/05/95   dna     Added cdma.authr_valid so zero could be valid RAND value
08/18/95   dna     Changed mcc_pc_frames array, added assign mode '101'
08/17/95   jca     Added function mcc_process_info_records().
08/10/95   rdb     Added declarations to support enhanced service messaging.
07/26/95   jca     Added an_chan_type field to mccdma_chnasn_am3_type.
07/20/95   jca     Added support for Subscriber Access Control.
07/13/95   rdb     Added types for assign mode 4 of chan assign message
                   Added serv_neg to cdma structure.
06/25/95   dy      Added CDMA SMS data structures and functions to support
                   transmission of SMS messages.
06/09/95   jca     Changed mcc_wait() interface to support new queue sigs.
06/07/95   dna     Added Authentication Definitions
11/14/94   dna     Added headers for all externalized functions.
                   Added page_chan_s to cdma_ms_type.
08/09/94   jca     Deleted function mcc_get_voc_version and other misc mods.
05/12/94   gb      Added new test commands.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/20/94   jca     Modified for IS-95 Phase 2 release.
12/22/93   jca     Modified for IS-95 Phase 1 release.
03/24/93   gb      Removed debug t_tdrop variable.
07/24/92   jai     Changed externs for several procedures for modified calling
                   parameters.
06/26/92   jai     Created file for DMSS.
12/20/90   jai     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "customer.h"
#include "comdef.h"
#include "modem_1x_defs.h"
#include "queue.h"
#include "qw.h"
#include "mc.h"
#include "mc_v.h"
#include "mc_i.h"
#include "mcc.h"
#include "mcc_v.h"
#include "mcc_i.h"
#include "mccrxtx.h"
#include "rxtx.h"
#include "rxtx_v.h"
#include "nv.h"
#include "srchmc.h"
#include "cmxll.h"
#include "db.h"
#include "caii.h"
#include "caix.h"
#include "caix_i.h"


#include "auth.h"
#include "auth_v.h"
#include "auth_i.h"


#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#include "uim_v.h"
#endif /* FEATURE_UIM_RUIM */

#include "ds707_extif.h"

#include "otaspi_i.h"
#include "dh.h"
#include "dh_v.h"


#include "mccsrch_i.h"
#include "mccqpch_i.h"

#include "rxcmc_i.h"
#include "txcmc.h"
#include "txcmc_i.h"
#include "sd.h"
#include "mccdma.h"
#include "mccdma_v.h"
#include "mccdma_i.h"


#ifdef FEATURE_CSIM
#include "mmgsdilib.h"
#endif


#include "sys.h"
#include "sys_v.h"
#include "stringl.h"
#include "cm.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h" 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#include "onex_voice_if.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* ----------------------------------------
** Indications of Direct Channel Assignment
** ---------------------------------------- */
typedef enum
{
  CDMA_DCA_NONE_IND,        /* No Direct Channel Assignment Indication */
  CDMA_DCA_UPD_OVH_IND,     /* Direct Channel Assignment Update Overhead Indication */
  CDMA_DCA_RES_IND,         /* Direct Channel Assignment Response Indication */
  CDMA_DCA_FAIL_IND         /* Direct Channel Assignment Failure Indication */

} mccdma_dca_ind_type;

/* ----------------------------------
** Powerdown registration types
** ---------------------------------- */

typedef enum
{
  CDMA_NORMAL,
  CDMA_LPM,
  CDMA_NAM_CHANGE,
  CDMA_SHUTDOWN
#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
  , CDMA_PPP_CLEANUP
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */
} cdma_powerdown_reg_type;

/* ------------------------------
** IMSI types
** ------------------------------ */
typedef enum
{
  CDMA_IMSI_M,
  CDMA_IMSI_T

} cdma_imsi_type;

#ifdef FEATURE_1X_TO_LTE
/* ENUM to define the type of the status of the reselection from 1x to lte. */
typedef enum
{
  MCC_1X_TO_LTE_RESEL_NOT_STARTED,
  MCC_1X_TO_LTE_RESEL_IN_PROC,
  MCC_1X_TO_LTE_RESEL_ABORTED,
  MCC_1X_TO_LTE_RESEL_FAILED,
  MCC_1X_TO_LTE_RESEL_SUCCESS
}mccidl_1x_to_lte_resel_status_type;
#endif /* FEATURE_1X_TO_LTE */

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
/* PPP_CLEANUP_NON_SV_MODE: ENUM to define state of deregister_f cmd processing */
typedef enum
{
  MCC_DEREGISTER_NONE,
  MCC_DEREGISTER_RSP_PENDING,
  MCC_DEREGISTER_PWRUP_REG_PENDING
}deregister_state_type;
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

/* Use an internal data type to refer to Fields related to
   CHIND data struct */
typedef caii_ext_chnasn_pilot_info_type mccdma_pilot_info_type;

/* Type which contains dialed digits                                       */

typedef struct
{
  byte digits[CM_MAX_DIALED_DIGITS];
    /* Dialed digits */
  byte num_digits;
    /* Number of digits in digits array */
  byte digits_sent;
    /* Indicator of how many digits were sent in the Origination Message
       or need to be sent during the Conversation state */
} cdma_dial_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CHANNEL ASSIGNMENT and EXTENDED CHANNEL ASSIGNMENT MESSAGE (see IS-95B
 * Section 7.7.2.3.2.8/21). CAM and ECAM share this internal data structure
 * so the processing of both messages is the same.  ECAM is basically a super
 * set of CAM, with assign_mode 100 and 101 of CAM mapped to assign_mode 00
 * and 01 of ECAM.*/

/* CAM assign_mode 000 and 100, ECAM assign_mode 00 and 100*/
typedef struct
{
  boolean direct_ch_assign_ind; /* Direct Channel Assignment Indicator */

  boolean freq_incl;            /* Frequency included indicator */
  byte default_config;          /* Default configuration */
  boolean bypass_alert_answer;  /* Bypass indicator */
  byte num_pilots;              /* Number of pilot records following */
                                /* Please note that in IS95-B std, this field is the
                                 * actual pilot number - 1, but here, num_pilot is
                                 * the actual number of pilot to be compatible with
                                 * CAM implementation */
  byte granted_mode;            /* Granted mode */
  word cdma_freq;               /* Frequency assignment (only present if
                                   freq_incl field is set to 1) */
  byte band_class;              /* Band class */
  byte frame_offset;            /* Frame offset */
  byte encrypt_mode;            /* Message encryption mode */

  byte d_sig_encrypt_mode;      /* Signaling Encryption Mode */
  byte key_size;                /* Key Size */
  boolean c_sig_encrypt_mode_incl; /* Common Channel Encrypt Mode */
  byte c_sig_encrypt_mode;      /* Common Channel Encrypt Mode */
  boolean x3fl_1xrl_incl;       /* SR3 center freq included */
  byte x1rl_freq_offset;        /* SR3 center freq */

  /* Active Set Pilot record information */
  //The following pilot rec is used becos it is a superset of all other pilot rec types
  mccdma_pilot_info_type pilot_rec[CAI_EXT_CHNASN_AM4_MAX];
  cai_radio_config_type for_rc; //RC of the forward CH
  cai_radio_config_type rev_rc; // RC of the reverse CH
  cai_chind_type ch_ind;
  byte fpc_subchan_gain; //FPC subchan gain for FFPC
  byte rl_gain_adj; // Not used for the time being, phase 2 stuff 4-bit 2's compliment
  byte fpc_fch_init_setpt; // Init setpt for FCH for FFPC
  byte fpc_fch_fer; // Target FER for puter loop forward power control
  byte fpc_fch_min_setpt; // This and the following parameter given the setpt range
  byte fpc_fch_max_setpt;
  byte fpc_dcch_init_setpt; // Init setpt for DCCH  for FFPC
  byte fpc_dcch_fer; // Target FER for puter loop forward power control
  byte fpc_dcch_min_setpt; // This and the following parameter given the setpt range
  byte fpc_dcch_max_setpt;
  boolean fpc_pri_chan; // Set to "0" i.e FCH till DCCH is supported

  boolean x3_fch_info_incl;
  caii_ext_ch_3x_fch_type fch_info[CAI_EXT_CHNASN_AM4_MAX];
  boolean x3_dcch_info_incl;
  caii_ext_ch_3x_dcch_type dcch_info[CAI_EXT_CHNASN_AM4_MAX];

  boolean rev_fch_gating_on; //Indicates if 8th rate gating is on or off */
  boolean rev_pwr_cntl_delay_incl; /* Is Rev pwr cntl delay incl */
  byte rev_pwr_cntl_delay; /* Rev pwr Cntl delay */

  boolean plcm_type_incl;    /* Public Long Code Mask type included indicator */
  byte plcm_type;            /* Public Long Code Mask type indicator */
  qword plcm_39;             /* 39 LSB bits of Public Long Code Mask */

  byte sr_id_restore;        /* Service reference identifier to be restored */

  caii_sr_id_bitmap_type sr_id_restore_bitmap;
                             /* Bitmap of service reference identifers
                                to be restored */

  boolean msg_int_info_incl; /* Message integrity information included
                                indicator */
  boolean change_keys;       /* Change key indicator */
  boolean use_uak;           /* Use UAK indicator */

  byte ext_ch_ind;                     /* Extended channel indicator */

  boolean full_ci_feedback_ind;        /* Full C/I feedback rate indicator */
  byte rev_cqich_frame_offset;         /* Reverse channle quality indicator channel
                                          frame offset */
  byte rev_cqich_reps;                 /* Reverse channel quality indicator channel
                                          repetition factor */
  byte rev_ackch_reps;                 /* Reverse acknowledgment channel repetition
                                          factor */
  byte for_pdch_rc;                    /* Forward packet data channel radio
                                          configuration */

  byte rev_pdch_rc;                    /* Reverse packet data channel radio
                                          configuration */
  byte for_cpcch_rate;                 /* F-CPCCH rate */

  boolean pdch_group_ind_incl;         /* Packet data channel indicator
                                          inclusion flag */
  boolean for_pdch_parms_incl;         /* Indicator of the inclusion of forward
                                          packet data channel configuration
                                          fields */
  boolean for_pdch_rlgain_incl;        /* F-PDCH parameters related to reverse
                                          link adjustment gains included
                                          indicator */
  byte rlgain_ackch_pilot;             /* R-ACKCH to pilot adjustment gain */
  byte rlgain_cqich_pilot;             /* R-CQICH to pilot adjustment gain */
  byte num_soft_switching_frames;      /* Number of frames for R-CQICH soft
                                          switching */
  byte num_softer_switching_frames;    /* Number of frames for R-CQICH softer
                                          switching */
  byte num_soft_switching_slots;       /* Number of slots per frame for R-CQICH soft
                                          switching */
  byte num_softer_switching_slots;     /* Number of slots per frame for R-CQICH
                                          softer switching */
  boolean chm_switching_params_incl;   /* Control hold mode fields inclusion
                                          indicator */
  byte num_soft_switching_frames_chm;  /* Number of frames for R-CQICH soft
                                          switching while in control hold */
  byte num_softer_switching_frames_chm;/* Number of frames for R-CQICH
                                          softer switching while in control
                                          hold */
  byte num_soft_switching_slots_chm;   /* Number of slots per frame for
                                          R-CQICH soft switching while in
                                          control hold */
  byte num_softer_switching_slots_chm; /* Number of slots per frame for
                                          R-CQICH softer swicthing while in
                                          control hold */
  byte pdch_soft_switching_delay;      /* F-PDCH soft switching delay */
  byte pdch_softer_switching_delay;    /* F-PDCH softer switching delay */

  caii_rev_pdch_parm_type rev_pdch_parm;/* Reverse packet data channel parameters */
  boolean sync_id_incl;                 /* SYNC ID included indicator */
  uint8 sync_id_len;                    /* SYNC ID length */
  uint8 sync_id[CAI_SYNC_ID_LEN_MAX];   /* SYNC ID */
  uint8 config_msg_seq;                 /* Configuration messsage sequence number */
  boolean use_rtc_nom_pwr;
    /* Indicates if RTC NOM PWR to be used for TCI.
       Initialized to TRUE in mcc_match_msg() when DCA is first received.
       Set to FALSE in layer2_ack_processing() when Layer 2 Ack is received for
       PRM. */
  uint8 rtc_nom_pwr;                    /* Reverse traffic channel nominal power */
  boolean respond_ind;                  /* Response requested indicator */
  boolean direct_ch_assign_recover_ind; /* Direct channel assignment recover indicator */
  boolean fixed_preamble_transmit_ind;  /* Fixed preamble length transmission indicator */
  uint8 fixed_num_preamble;             /* Traffic channel preamble length */
  boolean early_rl_transmit_ind;        /* Early reverse link transmission indicator */
  boolean fundicated_bcmc_ind;          /* BCMC on fundicated channel indicator */

} mccdma_chnasn_am0_type;

/* CAM assign_mode 001 and 101, ECAM assign_mode 01 */
typedef struct
{
  boolean respond;       /* Respond on new Access Channel indicator */
  boolean freq_incl;     /* Frequency included indicator */
  byte band_class;       /* Band class */
  word cdma_freq;        /* Frequency assignment (only present if
                            freq_incl field is set to 1) */
  word num_pilots;       /* Number of entries in pilot_pn array */
                         /* Please note that in IS95-B std, this field is the
                          * actual pilot number - 1, but here, num_pilot is
                          * the actual number of pilot to be compatible with
                          * CAM implementation */
  word pilot_pn[CAI_EXT_CHNASN_AM1_MAX]; /* Pilot PN sequence offset index */

} mccdma_chnasn_am1_type;

/* CAM assign_mode 010, ECAM assign_mode 10 */
typedef struct
{
  boolean respond;        /* Respond on analog control channel indicator */
  byte analog_sys;        /* System indicator */
  boolean use_analog_sys; /* Use analog system indicator */
  byte band_class;        /* Band class */

} mccdma_chnasn_am2_type;

/* CAM assign_mode 011, ECAM assign_mode 11 */
typedef struct
{
  word sid;              /* System identification of the analog system */
  byte vmac;             /* Voice mobile station attenuation code */
  word analog_chan;      /* Voice channel number */
  byte scc;              /* SAT color code */
  boolean mem;           /* Message encryption mode indiator */
  byte an_chan_type;     /* Analog voice channel type */
  byte band_class;       /* Band class */

} mccdma_chnasn_am3_type;

/* Union of Channel Assignment messages */
typedef union
{
  mccdma_chnasn_am0_type am0;
  mccdma_chnasn_am1_type am1;
  mccdma_chnasn_am2_type am2;
  mccdma_chnasn_am3_type am3;

} mccdma_ca_ord_type;


/* Channel Assignment message type */
typedef struct
{

  byte msg_type;     /* Message type */
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
  mccdma_ca_ord_type mode; /* Union for different assign modes */

} mccdma_chnasn_type;


/* This data struct is used for temporarily storing forward pwr ctrl info
   till the action time is reached, so that it can be updated in
   the non negotiable SCR */
typedef struct
{
  byte trans_num;             /* Trans num of this cmd given to lower layers */
  byte pwr_cntl_step;         /* Pwr Control Step */
  boolean fpc_incl;           /* Is FPC values included */
  cai_fpc_mode_type fpc_mode; /* FPC mode */
  boolean fpc_pri_chan;       /* FPC primary channel */
  boolean fpc_olpc_fch_incl;  /* Is FCH outer loop params included */
  byte fpc_fch_fer;           /* FCH target FER */
  byte fpc_fch_min_setpt;     /* FCH min setpt */
  byte fpc_fch_max_setpt;     /* FCH max setpt */
  boolean fpc_olpc_dcch_incl; /* Is DCCH outer loop parameters included */
  byte fpc_dcch_fer;          /* DCCH target FER */
  byte fpc_dcch_min_setpt;    /* DCCH min setpt */
  byte fpc_dcch_max_setpt;    /* DCCH max set point */

} mccdma_pwr_ctrl_info_type;

typedef struct
{
  /* The following variables are needed to support the CDMA Off Time
     Reporting feature. */
  boolean cdma_off_time_supported;  /* Off Time Report Message supported */
  word cdma_off_time_threshold;     /* Off Time Report threshold (in ms) */
  word srch_off_time_estimate;      /* SRCH's estimate of the search time */
  qword cdma_off_time_timestamp;    /* Time at which the off-time starts */
  boolean sent_off_time_message;    /* Indicates whether an Off Time Msg was sent */
} mccdma_off_time_type;

/* This struct contains info that may be needed for TC to Idle transition */
typedef struct
{
  boolean release_to_idle_ind; /* Indicates if Rel to Idle is allowed */
  /* Std added this field only in REL C but internally it is useful
   * even if REL C is not defined */
  caix_chan_type idle_ovhd_chan; /* Ovhd channel MS was idling on*/
  uint8 idle_band_class;         /* Band class in the idle state */
  uint16 idle_cdmach;            /* CDMA FREQ when in the Idle state */
  uint16 idle_sid;               /* SID when in the idle state */
  uint16 idle_nid;               /* NID when in the idle state */
  uint8  idle_bs_p_rev;          /* P_REV of the BS when in the idle state */
  boolean idle_td_supported;
  /* Note : Other info like PCH and BCCH info is also needed for this but they are
     already stored in the "cdma" struct and hence not under this feature */

} mccdma_stored_tc_to_idle_info_type;

typedef struct
{
  boolean   info_valid;
  uint8     release_type;
  uint16    sid;
  uint16    nid;
  boolean   freq_incl;
  uint16    cdma_freq;
  uint8     band_class;
  uint8     page_ch;
  uint8     prat;
  uint8     sr1_bcch_code_chan_non_td;
  boolean   sr1_crat_non_td;
  uint8     sr1_brat_non_td;
  uint8     sr1_td_mode;
  uint8     sr1_bcch_code_chan_td;
  boolean   sr1_crat_td;
  uint8     sr1_brat_td;
  boolean   num_pilots_d2i_incl;
  uint8     num_pilots_d2i;
  uint16    pilot_pn[CAI_MAX_NUM_D2I_PILOTS];

} mccdma_rxed_tc_to_idle_info_type;

/* ----------------------------------------------------------
** Private_lcm_active indicates whether the mobile is using
** the public long code mask or the private long code mask.
** If the mobile is using public, public_lcm_type indicates
** which public type it is using.
** Bs_plcm_39 holds the plcm_39 sent by the base station if
** the mobile is using the base station-specified plcm.
** Note that we do not store the long code mask itself, we
** generate it as needed.
** ---------------------------------------------------------- */
typedef struct
{
  cai_plcm_type public_lcm_type;

  qword bs_plcm_39;
  boolean private_lcm_active;

} mccdma_long_code_type;

typedef enum
{
  NO_PENDING,        /* No pending message to be sent after traffic channel is setup */
  SEC_MODE_CMP_ORD   /* Security Mode Completion Order after receiving ECAM message */

} mccdma_pending_aka_msg_type;

/* location constant to remember the MS AES proposals */
#define MCCDMA_AES_PROPOSE_BUF_SIZE    5

typedef struct
{
  uint32 sseq_h;             /* the proposed sseq_h */
  uint8  cmea_key[8];        /* the generated CMEA key when sending the ROP. Note that
                              * this does not apply to AC SMRM and TC SMRM since they
                              * do not generate CMEA key
                              */
} mccdma_aes_propose_type;

/* the following structure stores AES status information */
typedef struct
{
  uint8  c_sig_encrypt_mode;   /* encryption mode of common channel signaling message */
  uint8  d_sig_encrypt_mode;   /* encryption mode of dedicated channel signaling message */
  uint8  bs_sig_aes_sup;       /* current BS Signaling AES support information */
  uint8  bs_ui_aes_sup;        /* current BS UI AES support information */
  mccdma_aes_propose_type ms_proposals[MCCDMA_AES_PROPOSE_BUF_SIZE];
  uint8  last_ms_proposal_index;  /* the buf index to save the proposal */
  uint8  sig_decrypt_failures;  /* the number of most recent consecutive decryption failures */
  uint8  fresh[5];              /* this is for debugging only. save the most recently used FRESH */
  uint8  voice_so_sr_id;        /* this is specially for voice encryption/decryption use. It stores
                                 * the sr_id of the voice service. If no voice service option,
                                 * this field has value of MCCAES_INVALID_VOICE_SR_ID
                                 */
  uint8  voice_so_ui_encrypt_mode;   /* this is specially for voice encryption/decryption use. It stores
                                      * the ui_encrypt_mode of the voice service. If no voice service option,
                                      * this field has value of CAI_ENC_MODE_DISABLED
                                      */
  uint32 aes_tx_ext_sseq;
  /* this is the crypto-sync to be used for AES encryption.
   * if MI is in place, this will be the value from the MI
   * processing, otherwise, AES will get the value.
   *
   * Logically this is different from the standard. But
   * practically in implementation, this is the same -- as
   * long as both AES and MI share the same ext_sseq and only
   * one of them updates the value after taking the value.
   */

} mccdma_aes_status_type;

/* ------------------------------
** MC Call End reason types
** ------------------------------ */
typedef enum
{
  MC_CALL_ORIG_ERR_ACC_FAIL_REJ_ORD,
  /* BS reject order */
  MC_CALL_ORIG_ERR_ACC_FAIL_RETRY_ORD,
  /* BS retry order */
  MC_CALL_ORIG_ERR_TIMEOUT_T42,
  /* T42 expiry */
  MC_CALL_ORIG_ERR_TIMEOUT_T40,
  /* T40 expiry */
  MC_FADE_SRV_INIT_FAIL,
  /*Fade the call Service Initialization failure */
  MC_FADE_T50_EXP,
  /*Fade the call T50m expiry */
  MC_FADE_T51_EXP,
  /*Fade the call T51m expiry */
  MC_FADE_RL_ACK_TIMEOUT,
  /* Fade the call Ack timeout due to 12 retransmission*/
  MC_FADE_BAD_FL,
  /* Fade the call Bad FWD link or T5M expiry*/
  MC_CALL_ORIG_ERR_TRM_REQ_FAIL,
  /* TRM (transceiver resource) request failed */
  MC_CALL_ORIG_ERR_TIMEOUT_T41,
  /* Unable to update overhead msgs (T41m expiry) */

  MC_CALL_ORIG_ERR_MAX

} mc_call_end_reason_type;

typedef  struct
{
  mc_1xadvanced_capability_status status;
  /* Status as per NV setting */

  boolean rc_bitmap_capability_ext_allowed;
  /* RC bitmap and Capability Record Extension support indicator */
} mccdma_1xadv_status_type;

typedef union
{
  cm_dbm_cmd_info_s_type non_sms_payload;
  cm_sms_msg_type  sms_payload;
}mc_dbm_buf_type;

#ifdef FEATURE_1X_CP_IGNORE_GSRDM
/* Type which contains GSRDM msg ignore specific information */
typedef struct
{
  word pilot_pn;
    /* pilot pn offset */
  word cdma_ch;
    /* CDMA channel base station is using */
  byte band_class;
    /* Band class of the base station */
  byte config_msg_seq;
    /* Configuration message sequence number */
  boolean gsrdm_ignore_timer_on;
    /* Indicates whether ignore GSRDM timer is set or not */
}gsrdm_ignore_info_type;
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
/* Type to store Sim Busy Feature related global variables */
typedef struct
{
  /* To keep track of sim busy status received from MMGSDI */
  boolean is_sim_busy;
  /* Is true when a cmd has been sent to auth task via mcc_auth_cmd, 
     but it's processing has not yet started, i.e. it's waiting in queue */
  boolean is_auth_pending_in_queue ;
  /* Keeps track if the error returned by auth task is due to sim busy*/ 
  boolean is_auth_err_sim_busy;
} sim_busy_type;
#endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/* Structure to send the MCFG in order to register for NV 
 * Refresh indicationm */
extern mcfg_refresh_registration_s_type mcfg_refresh_registration;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_1X_SPLMN_SUPPORT
/* Type to store splmnid feature related global variables */
typedef struct
{
  /* Indicates if overhead info has been sent to SD or not */
  boolean sd_ovds_sent;
  /* Indicates if SPLMN list has been sent to SD */
  boolean splmn_list_sent;      
  /* Indicates if sending splmn list to SD is enabled or not */
  boolean c2k_resel_splmn_supp;
} splmn_sup_type;
#endif /* FEATURE_1X_SPLMN_SUPPORT */

#ifdef FEATURE_1X_OVHD_OPTIMIZATION
/* Type to store 1X overheads optimization related global variables */
typedef struct
{
/* NV item to indicate what all ovhds optimizations are supported */
  byte ovhd_optimization_sup;
  /* Indicates if the overheads are to be stored after overheads validity
  ** timer has expired. */
  boolean validate_exp_ovhds;
  /* Indicates if we need to reprocess the overheads in case we are using
  ** cached overheads */
  boolean proc_stored_ovhds;
  /* Marks if the cached overheads are ok or not while we are processing
   * stored overheads in the API mccidl_process_stored_ovhds. This information 
   * is required to be stored at the start of the API since we temporarily mark
   * some ovhds as INVALID while processing stored ovhds and if SRCH 
   * asynchronously queries the ovhd configuration through the API 
   * mcc_is_cached_ovhd_msg_okay() while some ovhds are temporarily marked
   * as Invalid, it will incorrectly consider the ovhds to be not current. 
   */
  boolean is_cached_ovhds_ok;
} ovhd_optimization_type;
#endif  /* FEATURE_1X_OVHD_OPTIMIZATION */

#ifdef FEATURE_SIMUL_1X_LTE
/* Type to store SRLTE MT page rcvd BS specific info */
typedef struct
{
  word pilot_pn;   
    /* pilot pn offset */
  byte band_class; 
    /* Band class of the base station */
  word cdma_ch;
    /* CDMA channel base station is using */
} srlte_mt_page_rcvd_info_type;
#endif /* FEATURE_SIMUL_1X_LTE */

/* Enum type values to keep track of HHO progress */
typedef enum
{
  HHO_NONE,                   /* Default case */
  HHO_MSG_RCVD,               /* Handoff msg received */
  HHO_MSG_BUFFERED,           /* Handoff msg buffered to process it later */
  HHO_TXNS_IN_PROG,           /* HHO txns execution under progress */
  HHO_RIF_IN_PROG,            /* HHO RIF proc's in progress */
  HHO_RIF_RETURN_TXN_IN_PROG  /* HHO RIF Return txn execution in progress */
} hho_prog_type;

/* Enum type values to keep track of CFS progress */
typedef enum
{
  CFS_NONE,                 /* Default case */
  CFS_READY_RPT_RCVD,       /* Rcvd SRCH_CFS_READY_R from SRCH */
  CFS_READY_RPT_BUFFERED,   /* CFS rpt buffered to process it later */
  CFS_IN_PROG,              /* CFS is in progress */
} cfs_prog_type;

/* Type to store 1x Tune Away rel global variables 
 * during 1x data call in trafifc state */
typedef struct
{
  /* Tune Away Reason */
  srch_ta_rsn_type ta_type;
  
  /* Tune Away Req Start time */
  qword start_time;  

  /* Sclks timestamp at which current TA is registered */
  uint32 ta_reg_time_sclks;

  /* Sclks timestamp at which next TA is registered */
  uint32 ta_next_reg_time_sclks;  

  /* To keep track if a fade timeout happended during QTA */
  boolean qta_fade_cmd_pending;

  /* To keep track if HHO Defer timer expired */
  boolean is_hho_defer_timer_expired;
  
  /* To keep track if SHO Defer timer expired */
  boolean is_sho_defer_timer_expired;

  /* To keep track if CFS Single search in progress */
  boolean is_cfs_single_srch_in_prog;

  /* To keep track if CFS Defer timer expired */
  boolean is_cfs_defer_timer_expired;

  /* To keep track of MC updated CFS status to SRCH */
  boolean srch_cfs_proc_status;
  
  /* To keep track of Priority Inversion in Progress or not */
  boolean prio_inv_in_prog;
  
  /* Enum variable to keep track of HHO progress */
  hho_prog_type hho_prog;
  
  /* Enum variable to keep track of CFS progress */
  cfs_prog_type cfs_prog;
  
}tc_data_ta_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type which contains Mobile Station specific variables                   */

/* ----------------------------------------------
** Many of these fields are to store CDMA Numeric
** Information defined in IS-95A section 1.1.2.2
** ---------------------------------------------- */
typedef struct
{
  word curr_state;     /* Current state of CDMA state machine */
  byte curr_nam;       /* Current NAM */
  boolean serv_neg;    /* TRUE  => service negotiation  */
                       /*  FALSE => service option negotiation */
  word so_req;         /* Service Option of outstanding request */
  word so_cur_pri;     /* Current Primary service option */
  word so_cur_sec;     /* Current Secondary sevice option */
  word cust_so;        /* Special SO to be set by customer */
  dword esn;           /* Electronic serial number */
  uint16 cdmach;       /* Current CDMA Channel number */
  mcc_entry_type *entry_ptr;
    /* Pointer to mode entry information received from main MC */

  /* Following are IMSI_O items */
  dword imsi_s1;       /* First part of Operational IMSI (IMSI_O_S1) */
  word imsi_s2;        /* Second part of Operational IMSI (IMSI_O_S2) */
  qword imsi_s;        /* IMSI_S1 and IMSI_S2 converted to a qword imsi_s */
  byte imsi_11_12;     /* Mobile Network Code (MNC) for IMSI_O address */
  word mcc;            /* Mobile Country Code (MCC) for IMSI_O address */
  byte imsi_addr_num;  /* Number of digits in NMSI - 4, or class 0 IMSI flag */

  /* Following are IMSI_M items.  */
  dword imsi_m_s1;       /* First part of MIN based IMSI (IMSI_M_S1) */
  word imsi_m_s2;        /* Second part of MIN based IMSI (IMSI_M_S2) */
  qword imsi_m_s;        /* IMSI_M_S1 and IMSI_M_S2 converted to a qword */
  byte imsi_m_11_12;     /* Mobile Network Code (MNC) for IMSI_M address */
  word imsi_m_mcc;       /* Mobile Country Code (MCC) for IMSI_M address */
  byte imsi_m_addr_num;  /* Number of digits in NMSI - 4,or class 0 IMSI flag*/

  /* Following are IMSI_T items.  */
  dword imsi_t_s1;       /* First part of True IMSI (IMSI_T_S1) */
  word imsi_t_s2;        /* Second part of True IMSI (IMSI_T_S2) */
  qword imsi_t_s;        /* IMSI_T_S1 and IMSI_T_S2 converted to a qword */
  byte imsi_t_11_12;     /* Mobile Network Code (MNC) for IMSI_T address */
  word imsi_t_mcc;       /* Mobile Country Code (MCC) for IMSI_T address */
  byte imsi_t_addr_num;  /* Number of digits in NMSI - 4,or class 0 IMSI flag*/

  cdma_imsi_type imsi_type; /* indicator of the type of IMSI_O */

  boolean mob_term_home;
    /* Indicator of whether phone is configured to receive calls when using
       a home SID,NID pair */
  boolean mob_term_for_sid;
    /* Indicator of whether phone is configured to receive calls as a
       foreign SID roamer */
  boolean mob_term_for_nid;
    /* Indicator of whether phone is configured to receive calls as a
       foreign NID roamer */
  byte accolc;         /* Access overload class */

  nv_sid_nid_pair_type pair[NV_MAX_HOME_SID_NID]; /* SID,NID pairs for MIN */

  dword imsi_hash_key;/* MIN based hash key */
  byte mob_cai_rev;   /* Protocol revision number of the mobile station */
  byte mob_model;     /* Model number of the mobile station */
  word mob_firm_rev;  /* Firmware revision number for mobile station */
  byte scm;           /* Station class mark */
  word code_chan;
    /* Code channel for channel assignment or CDMA-to-CDMA handoff */

  byte td_mode;
    /* SR1 TD mode (OTD or STS) */
  byte td_power_level;
    /* SR1 TD transmit power level */
  byte bcch_td;
    /* SR1 TD CDMA Broadcast Control Channel walsh */
  byte bcch_non_td;
    /* SR1 Non-TD CDMA Broadcast Control Channel walsh */
  cai_bcch_rate_type brat_td;
    /* SR1 TD Broadcast Control Channel data rate */
  cai_bcch_rate_type brat_non_td;
    /* SR1 Non-TD Broadcast Control Channel data rate */
  cai_code_rate_type bcch_code_rate_td;
    /* SR1 TD Broadcast Control Channel code rate */
  cai_code_rate_type bcch_code_rate_non_td;
    /* SR1 Non-TD Broadcast Control Channel code rate */
  byte bcch;
    /* Current CDMA Broadcast Control Channel walsh */
  cai_bcch_rate_type brat;
    /* Current Broadcast Control channel data rate */
  cai_code_rate_type bcch_code_rate;
    /* Current Broadcast Control channel code rate */
  byte fccch_id;
    /* Current CDMA Forward Control Channel number */
  byte num_fccch_s;
    /* Number of Forward Common Control Channels on current CDMA channel */

  byte pagech;
    /* Current CDMA Paging Channel number */
  byte page_chan_s;
    /* Number of Paging Channels on current CDMA channel */
  cai_pch_rate_type prat;
    /* Current CDMA Paging channel data rate */
  int8 slot_cycle_index_p;
    /* SLOT_CYCLE_INDEXp - the slot cycle index prefered by user */

  int8 neg_slot_cycle_index_p;
    /* NEG_SLOT_CYCLE_INDEXp - the negative slot cycle index prefered by user */

  uint16 idle_pgslot;
  /* Hashed Paging Channel slot (0-2047).  Used on the Traffic Channel. */

  int8 last_sci_reg;
    /* The last SCI value that MS registered with the BS */
  int8 curr_sci_pref;
    /* The current slot cycle index prefered by upper layer */
#ifdef FEATURE_QCHAT
  int8 use_sci_pref;
    /* The new slot cycle index requested by upper layer */
#endif /* FEATURE_QCHAT */
  mccdma_chnasn_type chnasn;
    /* Channel assignment message received during the Idle state or System
       Access state and retained for use in TC state*/
  caii_escam_type escam;
    /* Extended Supplemental Channel Assignment Message received in TC state */
  boolean call_orig;
    /* Indicator of whether the call in progress is mobile originated (TRUE)
       or mobile terminated (FALSE) */
  boolean rel_orig;
    /* Indicator of whether the user initiated the call release (TRUE)
       or the release was initiated by the base station (FALSE) */
  byte reg_type;
    /* Type of registration required -- this field must be set when the
       next state is CDMA_REG - the codes are defined in the IS-95 spec */
  cdma_powerdown_reg_type powerdown_reg_type;
    /* type of powerdown registration required- whether from powerdown
       or from lpm */
  word sa_tc_state;
    /* State that should be entered after Update Overhead Information substate */
  cdma_ho_type ho_state;
    /* Keeps track of various Handoff states */
  word ho_sa_tc_state;
    /* This holds the state that should be entered after Update Overhead Information
       substate during Access Entry handoff */
  byte sa_resp_msg;
    /* Message to be sent, during order/message response substate, in
       response to a received message */
  caii_ac_ord_type sa_resp_ord;
    /* Specific order to transmit when the sa_resp_msg is an Order Message */

  boolean digit_mode;      /* Identifies between DTMF and ASCII call type */
  byte number_type;        /* Number type for an ASCII mode */
  byte number_plan;        /* Number plan for an ASCII mode */
  cdma_dial_type dial;     /* Structure containing digits dialed by user */
  byte remaining_digits;   /* Number of digits that didn't fit in origination */
  byte frame_offset;       /* Current Traffic Channel frame offset, in units of 1.25 msec */
  byte hdm_seq;            /* Handoff Direction Message sequence number */

  boolean vp_desired;      /* Indicates if the user wants voice privacy.  This
                              is the value of the pm bit in a page response or
                              origination */
  byte encrypt_mode;       /* Indicates if message encryption is on */
  byte auth_mode;          /* Indicates if authentication data sent on access */
  byte band_class;         /* CDMA band class (cellular or PCS) */
  nv_block_or_sys_enum_type block_or_sys; /* Cellular serv-sys (A or B) or
                                             PCS frequency block (A-F)   */
  dword rand;          /* Value of RAND used in last computation of AUTHR for
                          registrations */
  boolean authr_valid; /* Signifies whether the authr value can be used */
  dword authr;         /* Value of AUTHR used is registration messages */
  dword randu;         /* Random value received in Auth Challenge Order */

  caii_tmsi_data_type assigning_tmsi;
  byte tmsi_code_len;
  boolean tmsi_full_timer_set;

  otaspi_mob_dir_data_type mob_dir; /* Mobile Directory Number */

  byte spc_status;

  byte spc[NV_SEC_CODE_SIZE]; /* Service Programming Code */

  otasp_call_type otasp_call; /* Indicates service provisioning type */

    /* OTA capability features supported by the card */
  ruim_otasp_features_type ruim_ota_features;

  boolean otapa_enabled; /* Indicate if OTAPA is enabled for current NAM */
  boolean nam_lock;      /* Indicates if the current NAM is protected by SPASM */


  dword auth_data;            /* auth data for a mobile originated call */
  boolean spc_change_enabled; /* indicates if user has enabled the spc change */
  byte p_rev_in_use;          /* P_REV_IN_USE */
  byte ds_p_rev_in_use;       /* P_REV to be sent to DS task */
  byte pwr_cntl_step;         /* Closed loop power control step size */
  byte fpc_subchan_gain;      /* FPC subchan gain */
  byte end_reason;            /* reason for ending a call */
  word t_slotted;             /* Slotted timer value */
  boolean  rev_fch_gating_on; /* Rev FCH 8th-rate gating mode on indicator */
  byte  rev_pwr_cntl_delay;   /* Rev pwr ctrl delay */
  boolean drs;                /* DRS bit in Origination message */
  byte call_id;               /* Temporary storage for call_id */

  boolean orig_reason;            /* Indicates if silent redial due to L2 failure. */
  byte orig_count;
    /* If orig_reason is 1: Number of silent redials due to L2 failure;
       Else, Number of silent redials for the desired service option that have
       failed since the last successful connection (not implemented). */
  boolean is_packet_data;         /* indicates if packet data origination */
  boolean is_dormant_packet_data; /* indicates if dormant packet data */
  boolean is_emergency;           /* if this is an emergency call */
  boolean cs_supported;           /* Indicates if Concurrent Services is supported by BS */
  boolean mob_qos;                /* Permission indicator for Qos in Origination Message*/
  boolean qos_parms_incl;         /* Qos Parameters indicator */
  byte qos_parms_len;
  byte qos_parms[CAI_ORIG_QOS_LEN_MAX];
  boolean is_sr_id_valid;         /* Indicates if SR_ID is valid */
  byte sr_id;                     /* SR_ID for the 1st call */

  boolean pilot_info_req_supported; /* Pilot information request supported indicator */


  byte mo_dbm_type;  /* mobile orig'ed data burst msg type */
  mc_dbm_buf_type dbm_buf; /* MO DBM data */
  boolean dbm_wait_for_l2_ack;
          /* Indicates if MC is waiting for layer 2 ack for an MO DBM request */
          /* It is set to TRUE if it is waiting for L2 ack                    */
          /* It is set to FALSE if it is *not* waiting for L2 ack.            */
          /* At a time, MC can wait for either SMS, PD or SDB L2 ack only.     */

  dword esn_me;             /* ESN_ME */

  qword meid_me;            /* MEID_ME */

  boolean ruim_id_avail;    /* Indicates if R-UIM ID is available */
  dword ruim_id;            /* R-UIM ID */

  mc_ddtm_cntl_type ddtm; /* data dedicated transmission mode.
                             This value is set by CM through CM_DDTM_CNTL_F */
  mc_ddtm_cntl_type             cm_ddtm;     /* DDTM settings from CM */
  mc_ddtm_cntl_type             mc_ddtm;     /* DDTM settings internally from MC */


  boolean is_cdma_subs_avail; /* current CDMA/AMPS/HDR subscription
                                 availability status */

  boolean transceiver_request_pending;

  /* Set/Reset ONLY by ITSPM, UHDM. Used ONLY in traffic state.
     It indicates whether BS supports SDB on traffic channel.  */
  boolean sdb_supported;

  mccdma_long_code_type long_code;

  boolean tc_tx_on;       /* Current status of the transmitter  on TC*/
  boolean tc_tx_to_be_on; /* Indicates the new state of the transmitter */

  qword meid;             /* Mobile Equipment Identification Number */
  unsigned long  meidhash[5];     /* MEID SHA-1 Hash value */

  /* The following variables are needed to support the REL_B TC to Idle
    transition */
  mccdma_stored_tc_to_idle_info_type tc_to_idle_info;
  /* The following variables are needed to support the CDMA Off Time
     Reporting feature. */
  mccdma_off_time_type off_time_info;

  boolean chm_supported; /* Indicates if 1x control hold is supported by BS*/
  boolean for_pdch_supported; /* Indicates if DV is supported by BS */
  mccdma_rxed_tc_to_idle_info_type rxed_tc_to_idle_info; /* Indicates Directo to Idle
                                  info, if received */
  boolean use_sync_id; /* Indicates if BS supports SYNC ID while in traffic */

  mccdma_dca_ind_type dca_ind; /* Direct Channel Assignment indication. */

  /* after receiving ECAM, the mobile may need send Security Mode Completion
   * Message or Mobile Reject Order to BS on traffic channel. These two
   * messages need be sent out when the mobile comes into traffic channel
   * so we set pending flag here
   */
  mccdma_pending_aka_msg_type aka_msg_pending;

  boolean gen_cmeakey;

  mccdma_aes_status_type aes_status;

  byte sa_msg_tx_msg; /* this is the message type to be sent in CDMA_MSG */

  boolean uim_ruim_support_sci; /* Indicates whether FEATURE_UIM_RUIM_SUPPORT_SCI supported or not */

  boolean disable_so35_so36;  /* Indicates whether SO35 and SO36 support disabled or not */

  uint8 otasp_max_unlock_attempts;  /* Otasp Max unlock attempts allowed */

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
  /* PPP_CLEANUP_NON_SV_MODE: Deregister Timer Value (msec's)*/
  dword deregister_timer_val_msec;
  /* PPP_CLEANUP_NON_SV_MODE: Indicates state of deregister_f cmd processing */
  deregister_state_type deregister_state;
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

  /* ---------------------------------------------------------------------------
  ** The size of below members varies with whether JCDMA is supported or not.
  ** Important !!! Please do not add new member that has no size dependency on
  ** JCDMA after them.
  ** --------------------------------------------------------------------------- */
  mc_calling_party_number_type  calling_party_number;  /* Calling party number record */
  mc_subaddress_type            calling_party_subaddr; /* Calling party subaddress record */
  mc_subaddress_type            called_party_subaddr;  /* Called Party subaddress record */

  mccdma_1xadv_status_type      onexadv_status;        /* 1xAdv status information */

  mmgsdi_client_id_type client_id;
  mmgsdi_session_id_type session_id;

  boolean csfb_mode;           /* Indicates if csfb_mode is active */
  boolean csfb_1x_transition;  /* Indicates if there is a transition to 1x */

  mc_jcdma_me_capability_type jcdma_support; /* JCDMA supported. */
  boolean is_page_in_sv_mode;
  boolean is_srlte_incoming_call; /* if there is any SRLTE incoming call */
  
  #ifdef FEATURE_SIMUL_1X_LTE  
  /* Str to store SRLTE MT page rcvd BS specific info */
  srlte_mt_page_rcvd_info_type srlte_mt_page_rcvd_info; 
    
  /* Flag to indicate if 1xSRLTE MT page access handoff feature supported */
  boolean srlte_mt_page_acc_ho_supported;
  #endif /* FEATURE_SIMUL_1X_LTE */

  boolean is_sib8_proc_deferred;  /* if the SIB8 Procesing was deferred, e.g., in SRLTE mode */
  uint32 codec_rate;
  boolean disable_1x_to_lte_reselection;
  boolean cmas_dup_det_supported;
  boolean is_use_csfb_call_id; /* Flag to indicate if call id is allocated in CSFB mode */
  /* Variable to get slot on which card's subscription is available.
   * This is one of the input parameter for the GSTK API we use to
   * check if BIP session is in progress.
   */
  mmgsdi_slot_id_enum_type curr_slot_id;
  /* This value will be filled from the EFS. For targets which need
   * OTASP/BIP support, this flag needs to be set.
   */
  boolean is_check_bip_otasp_concurrency;
  /* Flag to indicate OTASP is allowed and BIP is stopped. */
  boolean is_start_otasp_stop_bip;

  mc_call_end_reason_type call_end_reason; /* Call end reason type */
  boolean specific_call_end_stats_supported;

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
  boolean csfb_call_setup_optimised;
  /* Boolean to store the value of the EFS item; to determine if the
  ** setup time for redirection based CSFB calls is to be minimised */
#endif /* FEATURE_CSFB_CALL_SETUP_OPT */

#ifdef FEATURE_PPP_CLEANUP_SV_MODE
  /* PPP_CLEANUP_SV_MODE: if this is special service data call origination */
  boolean is_special_srv_data_call;
#endif /* FEATURE_PPP_CLEANUP_SV_MODE */

  /* NV#70360 - Indicates whether to reject 1x voice page
   * while VOLTE call is active */
  boolean rej_voice_page_for_volte;

#ifdef FEATURE_1X_CP_IGNORE_GSRDM
  /* GSRDM msg ignore specific information */
  gsrdm_ignore_info_type gsrdm_ignore_info;
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */

  sd_ss_sys_lost_e_type system_lost_reason;
  /* Indicates the reason with which we are going to inform SD of the
   * system lost */

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  /* Structure to store Sim Busy feature related global variables */
  sim_busy_type sim_busy;
#endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

#ifdef FEATURE_1X_SPLMN_SUPPORT      
  /* Structure to store splmid feature related global variables */
  splmn_sup_type splmn_sup;
#endif /* FEATURE_1X_SPLMN_SUPPORT */

  /* NV#72625 - Whether to use default NV values for 
   * NV Read/Write error during Powerup load */
  boolean use_def_val_for_pwrup_nv_err;

#ifdef FEATURE_1X_OVHD_OPTIMIZATION
/* Structure to store 1X overheads optimization related global variables */
  ovhd_optimization_type ovhd_opt;
#endif  /* FEATURE_1X_OVHD_OPTIMIZATION */

#ifdef FEATURE_1X_SPLIT_OVERHEAD
  /* Flag to indicate if split ovhd across multiple wakeup is supported */
  boolean split_ovhd_supported;
  
  /* SRCH will get this flag status and if it is set to TRUE 
   * then SRCH will ingore unlock callback from TRM and 
   * block sending SYS_REL report to MC while in split overhead collection */
  boolean split_ovhd_block_sys_rel_flag;

  /* Stores the timestamp of the most recent wakeup report from SRCH 
   * on current BS */
  qword wakeup_rpt_time;
#endif /* FEATURE_1X_SPLIT_OVERHEAD */

  /* Time after which stored overheads will be considered expired. It is in 
   * 20 ms units. By default it will be set to specs defined value i.e 10 mins
   * in 20 ms units.
   * It can only be modified through Diag cmd for testing purposes */
  uint32 ovhd_refresh_time;

  /* Consecutive Transceiver Request denial count for Registration attempt */
  byte reg_trans_req_denial_cnt;

/* Flag to indicate whether READY_IND is pending to be sent to CM */
  boolean psm_ready_ind_pending;
/*Last Power Saving Mode (PSM) status sent to CM */
  sys_psm_status_e_type psm_status;

  /* Structure to store 1x Tune Away rel global variables 
   * during 1x data call in trafifc state */
  tc_data_ta_type tc_data_ta;

  
  /* Type of UE mode status change rcvd from MMODE */
  sys_ue_mode_e_type ue_mode;
    
  /* Current UE mode substate SRLTE status rcvd from MMODE */
  boolean is_ue_mode_substate_srlte;

} cdma_ms_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type which contains message sequence numbers and message receipt flags  */

typedef struct
{
  qword msg_time;
    /* Time of receipt of most recently received message for this base
       station */
  boolean sy_rx;
    /* indicator of whether valid Sync channel information is in the
       sy structure */
  boolean sp_rx;
    /* System Parameter Message received */
  boolean nl_rx;
    /* Neighbor List Message received */
  boolean cl_rx;
    /* CDMA Channel List Message received */
  boolean gnl_rx;
    /* Extended CDMA Channel List Message Received */
  boolean ecl_rx;

    /* Extended Global Service Redirection Message Received */
  boolean egsd_rx;
  boolean ap_rx;
    /* Access Parameter Message received */
  boolean esp_rx;
    /* Extended System Parameters Message received */
  boolean gsd_rx;
    /* Global Service Redirection Message received */

  boolean a41spm_rx;
  /* ANSI_41 System Parameters Message received */
  boolean mcrrpm_rx;
  /* MC-RR Parameters Message received */
  boolean unlm_rx;
  /* Universal Neighbor List Message */
  boolean eapm_rx;
  /* Extended Access Parameters Message */

  boolean a41randm_rx;
  /* Ansi-41 RAND Message */

#ifdef FEATURE_1X_TO_LTE
  boolean atim_rx;
  /* Alternative Technologies Information Message */
#endif /* FEATURE_1X_TO_LTE */
  byte config_msg_seq;
    /* Configuration Message sequence number which applies to the received
       System Parameter, Neighbor List, CDMA Channel List, Extended System
       Parameter and Global Service Redirection messages */
  byte acc_msg_seq;
    /* Access Parameter sequence number which applies to the received
       Access Parameter message */

} cdma_msg_rx_type;

typedef struct
{
  byte num_fccch;
  cai_fccch_rate_type fccch_rate;
  cai_code_rate_type  fccch_code_rate;
  byte                fccch_code_chan[CAI_MAX_NUM_FCCCH];
} cdma_fccch_info_type;

typedef struct
{
  byte                  num_bcch_bcast; /* Number of non-primary BCCH chans */
  struct
  {
    byte                bcch_code_chan; /* walsh code index for the BCCH */
    cai_bcch_rate_type  brat;           /* BCCH data rate */
    cai_code_rate_type  bcch_code_rate; /* BCCH code rate */
  } bcch_info[CAI_MAX_NUM_BCCH_BCAST];

}cdma_bcast_bcch_info_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   Types to store Neighbor Set and Active Set                            */

typedef struct
{
  word pilot_pn;  /* Neighbor pilot PNs */
  byte age;       /* AGE of neighbor */
  byte config;    /* Neighbor configuration */
  byte search_pri;     /* Pilot Channel search priority */
  boolean freq_incl;   /* Frequency included indicator */
  byte band;           /* Neighbor band class */
  word freq;           /* Neighbor frequency assignment */
  boolean access_entry_ho;    /* If Access Entry HO allowed */
  boolean access_ho_allowed;  /* If ACCESS HO and Access Probe allowed */
  byte  srch_win_nghbr;       /* Neighbor window for the pilot */
  boolean timing_incl;        /* If Timing included for the Neighbor */
  byte nghbr_tx_offset;       /* Neighbor TX offset */
  byte nghbr_tx_duration;     /* Neighbor TX Duration */
  byte nghbr_tx_period;       /* Neighbor TX PERIOD */
  boolean add_pilot_rec_incl;
  cai_pilot_rec_type nghbr_pilot_rec_type;
  caii_add_pilot_rec_type nghbr_pilot_rec;
  byte srch_offset_nghbr;

  boolean bcch_supported;   /* If BCCH is supported by this neighbor */

} cdma_nghbr_set_type;

typedef struct
{
  byte pilot_inc;       /* Current pilot increment between neighbors */
  byte nghbr_max_age;   /* Maximum AGE for neighbors */
  byte nghbr_srch_mode; /* Neighbor Search Mode */
  word num_nghbr_pns;   /* Number of PNs in the Neighbor Set */
  cdma_nghbr_set_type nset[SRCH_NSET_MAX];   /* Neighbor set */

} cdma_nghbr_type;


/* Data Structure that contains the complete system parameters of a BS
   i.e SPM, ESPM, MC-RR and ANSI-41 sys paramaters, Most of the MC-RR and
   ANSI_41 parameters are stored in sp and esp data structure,  The rest
   are stored as separate parameters in this data structure */
typedef struct
{
  caii_sysparm_type sp;
    /* Most recently received System Parameter Message */
  caii_ext_sysparm_type esp;
  word ota_mcc;               /* Over The Air MCC */
  byte ota_imsi_11_12;        /* Over The Air IMSI_11_12 */
    /* Most recently received Extended System Parameters Message */

  byte qpch_code_chan[CAI_MAX_NUM_QPCH]; /* code channel index of the QPCH for SR3 */
  cdma_fccch_info_type fccch_info; /* FCCCH infomation */
  cdma_bcast_bcch_info_type bcast_bcch_info; /* Broadcast BCCH channel info */

  boolean sending_a41_rand; /* ANSI-41 Rand Message expected to be sent */

} mcc_complete_sys_parms_type;

/* On the Paging Channel, authentication values (auth and rand) are contained in
   Access Parameters Message.  On BCCH, auth is set by sending_rand in MC-RR, while
   rand is contained in Ansi-41 RAND.  Store in one structure for easy access. */
typedef struct
{
  byte  auth;
  dword rand;

} mcc_rand_parms_type;

typedef struct
{
  caix_chan_type ovhd_chan;
  byte ovhd_walsh_ch;
    /* Broadcast Control channel or Paging channel mobile has been
       assigned to on this base station. The base station can have different
       parameters for different channels */

  cai_pch_rate_type prat;
    /* Current CDMA Paging channel data rate */
  cai_bcch_rate_type  brat;           /* BCCH data rate */
  cai_code_rate_type  bcch_code_rate; /* BCCH code rate */

} mcc_ovhd_chan_info_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type which contains base station specific information                   */

typedef struct
{
  boolean valid_pn;
    /* Indicator of whether pilot_pn is valid
       (i.e. whether entry has been written to or not) */
  word pilot_pn;
    /* pilot pn offset */
  boolean td_supported ;
    /* Indicates if this freq supports TD, so that we can access the correct
     BCCH info from the SYNC message */

  /* We need complete TD info about this if we have to do a direct
  TC to idle transition */
  byte td_mode;
    /* SR1 TD mode (OTD or STS) */
  byte td_power_level;
    /* SR1 TD transmit power level */

  cdma_msg_rx_type rx;
    /* structure containing information about which messages have been
       received */
  caii_sync_type sy;
    /* Most recently received Sync Channel message */
  caii_accparm_type ap;
    /* Most recently received Access Parameter Message */
  cdma_nghbr_type nl;
    /* Most recently received Neighbor List Message */
  caii_glob_redirect_type gsd;
    /* Most recently received Global Service Redirection Message */
  byte nl_config_msg_seq;
    /* Config Seq num of most recently received Neighbor List Message */
  caii_chnlist_type cl;
    /* Most recently received Channel List Message */
  byte gnl_config_msg_seq;
    /* Config msg sequence of General Neighbor List Message */
  caii_analog_nghbr_type anl;
    /* Analog Neighbor List as received part of GNLM */
  boolean setting_search_win;
    /* flag to indicate if the GNLM is received and SRCH WIN needs be be
       defaulted: A case where GNLM arrives before the SPM */

  /* Extended Global Service Redirection Message */
  caii_ext_glb_redir_type egsd;
  /* Extended Channel List Message */
  caii_ext_chnlist_type  ecl;

#ifdef FEATURE_1X_TO_LTE
  /* Alternative Technologies Information Message */
  caii_alt_tech_info_msg_type atim;

  /* Traffic Channel Alternative Technologies Information Message */
  caii_tc_alt_tech_info_msg_type     tc_atim;
#endif /* FEATURE_1X_TO_LTE */
  caii_en_ac_parm_type eapm;
   /* Enhanced access parameters message */
  byte a41spm_config_msg_seq;
  /*  config_msg_seq of ANSI 41 system parameters message */
  byte mcrrpm_config_msg_seq;
  /*  config_msg_seq of MC-RR parameters message */
  byte unlm_config_msg_seq;
  /*  config_msg_seq of UNLM  message */
#ifdef FEATURE_1X_TO_LTE
  byte atim_config_msg_seq;
  /*  config_msg_seq of ATIM  message */
#endif /* FEATURE_1X_TO_LTE */

  mcc_complete_sys_parms_type csp;
    /* Complete sys parameters; contains SP, ESP, MC-RR and ANSI-41 data */

  mcc_rand_parms_type rp;

  byte band_class;
    /* Band class of the base station */
  word cdma_ch;
    /* CDMA channel base station is using */
  boolean ovhd_chan_info_valid;
    /*Info about Channel on which OVHDs are received */
  mcc_ovhd_chan_info_type ovhd_chan_info;

  mccqpch_channel_type qpch_num;
    /* Quick paging channel number.  The number is generated when needed so that
       we don't need to determine if the right messages have arrived.  When this
       information is needed, i.e. entering sleeping, we are sure that all messages
       are current. */
  dword imsi_hash_key;

  byte hashed_band;     /* CDMA hash band as received in ECCLM */
  word hashed_cdmach;   /* CDMA hash frequency as received in the CLM or ECLM */

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
  lte_rrc_global_cell_id_t     global_cell_id;
    /* Cell id of the LTE system as received in the redirection request.
    ** This value is generated when the redirection based calls are made */
  lte_rrc_plmn_s    redir_plmn;
    /* PLMN id of the LTE system as received in the redirection request*/
#endif /* FEATURE_CSFB_CALL_SETUP_OPT */

} cdma_bs_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   External declarations for structures declared in mc_cdma.c            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* All mobile station specific variables */
extern cdma_ms_type cdma;

/* Pointer to Current Base Station in Base Station Information array */
extern cdma_bs_type *cur_bs_ptr;

/* Variable to store EFS read for reg_after_ddtm_page_drop */
extern boolean reg_after_ddtm_page_drop_supported;

/* This variable is to store the DS call back API function ptr 
 * and this call back API will be get called when SYNC/ESPM received */

extern mcc_ds_sync_espm_prev_cb_type mcc_ds_sync_espm_prev_cb;

/* To store BS1 info required to add it in BS2 NSET whenever we perform 
 * handoff from BS1 to BS2 */
extern cdma_nghbr_set_type mcc_old_bs_nghbr_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumerations for substates                                              */

/* State codes are separated by 256 and substate codes are consecutive.
   No substates are needed for the CDMA_ENTER or CDMA_EXIT states.         */

/* ---------------------------------
** Substates of Initialization State
** --------------------------------- */
typedef enum
{
  CDMA_PIL_ACQ = CDMA_INIT,
  CDMA_SYNC_ACQ,             /* Sync Channel Acquisition (part of IS-95
                                Sync Channel Acquisition substate) */
  CDMA_SYNC_MSG,             /* Process Sync channel Message (part of
                                IS-95 Sync Channel Acquistion substate) */
  CDMA_JUMP,                 /* Jump to Hyperspace (IS-95 Timing Change
                                Substate) */
  CDMA_UNJUMP                /* Jump back from Hyperspace */
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  , CDMA_TT
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

} cdma_init_state_type;

/* -----------------------
** Substates of Idle State
** ----------------------- */
typedef enum
{
  CDMA_IDLE_INIT = CDMA_IDLE,    /* Initial entry into idle */
  CDMA_IDLE_MSG_PENDING,         /* Had to send ACK and leave msg pending.
                                    System Access finished, process msg now */
  CDMA_UNSLOTTED,                /* Idle not operating in slotted mode */
  CDMA_SLOTTED,                  /* Idle operating in slotted mode */
  CDMA_OVHD                      /* System Access Update Overhead info */

} cdma_idle_state_type;

/* --------------------------------
** Substates of System Access State
** -------------------------------- */
typedef enum
{
  CDMA_UPDATE_OVH = CDMA_SYS_ACC, /* Update Overhead Information */
  CDMA_ORIG,                      /* Mobile Station Origination Attempt */
  CDMA_PAGE_RES,                  /* Page Response */
  CDMA_ORD_RES,                   /* Mobile Station Order/Message Response */
  CDMA_REG,                       /* Registration Access */
  CDMA_MSG,                       /* Message transmission */
  CDMA_PACA_CAN                   /* PACA Cancel Substate */

} cdma_sa_state_type;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* --------------------------------
** Substates of CSFB State
** -------------------------------- */
typedef enum
{
  CDMA_CSFB_IDLE = CDMA_CSFB, /* CSFB Idle State */
  CDMA_CSFB_ACCESS,           /* CSFB Access State */
  CDMA_CSFB_EXIT              /* CSFB Exit State */
} cdma_csfb_state_type;

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* Substates of Origination/MT CSFB call Page Response state. 
 * With Release Order on AC feature, the mobile could be waiting 
 * for Lay2 ACK of Release Order instead of CAM/ECAM
 * in the Origination/MT CSFB call Page Response state. */
typedef enum
{
  CDMA_ORIG_PAGE_NORM,  /* Orig/Page Rsp state while not in rel process */
  CDMA_ORIG_RELS,       /* Origination state while in release process */
  CDMA_PAGE_RELS        /* MT CSFB call Page Rsp state while in rel process */
} cdma_sa_orig_or_page_ministate_type;

typedef enum
{
  NOT_ABORT,
  ABORT_PSIST,
  ABORT_MSG_TOO_LONG,
  ABORT_NO_ACC_PARA,
  ABORT_XLATE_ERROR,
  ABORT_HO_LIST,

  #ifdef FEATURE_IS2000_REL_A
  ABORT_ACCT_BLOCK,
  #endif /* FEATURE_IS2000_REL_A */

  ABORT_SRCH_REQUEST_SYS_RESTART,
  ABORT_NUM_REASONS

} access_result_type;

/* ----------------------------------
** Substates of Traffic Channel State
** ---------------------------------- */
typedef enum
{
  CDMA_TCI = CDMA_TC,            /* Traffic Channel Initialization */
  CDMA_TRAFFIC,                  /* Traffic State */
  CDMA_RELEASE                   /* Release */

} cdma_tc_state_type;

/* ----------------------------------
** Substates of VOIP Handoff State
** ---------------------------------- */
typedef enum
{
  CDMA_WAIT_FOR_SRDM = CDMA_VOIP_HO,   /* Wait for an SRDM from CM */
  CDMA_SRCH_GET_TIMING,                /* Wait for SRCH to acquire the timing.
                                          Send the Origination Message. */
  CDMA_WAIT_FOR_UHDM,                  /* Wait for a UHDM to be received from bs */
  CDMA_PREPARE_FOR_TCH,                /* Prepare to go to the Traffic Channel */
  CDMA_TC_INIT,                        /* Traffic Channel Initialization */
  CDMA_WAIT_FOR_RESOURCES              /* Wait for CM to end the VOIP call and
                                          tell the lower layers to go to TCH */
} cdma_voip_ho_state_type;

// This new struct was introduced for FEATURE_IS2000
// All code channels are now replaced by this struct
// For bw compatbility the QOF mask Id can be set to 0
typedef struct
{
  word code_chan;
  byte qof_mask_id;

} cdma_walsh_fn_type;


/* -------------------------------------------
** Position Determination L2 Ack Status types
** ------------------------------------------- */
typedef enum
{
  CDMA_NO_PD_L2_ACK_PENDING,    /* No PD L2 Ack info in L2 Data Structure,
                                ** i.e., mcc_layer2
                                */
  CDMA_PD_L2_ACK_WAITING       /* PD L2 Ack info is in L2 Data Structure */

} cdma_pd_l2_ack_status_type;


#ifdef FEATURE_SO_8026
/* -------------------------------------------------
** Proprietary Service Option call status types
** MS while in access state during 8026 call flow
** transitions through these states until it finishes
** and returns to idle. The last state, before it
** returns to idle, for a successful call flow, is
** CDMA_8026_L2_ACK_MO_SDB_NOT_PENDING.
** ------------------------------------------------- */
typedef enum
{
  CDMA_8026_CALL_STATUS_NONE = -1,

  CDMA_8026_L2_ACK_PENDING,       /* MT SDB has been received.
                                     Its L2 is waiting to be piggybagged
                                     with outgoing MO SDB.
                                  */

  CDMA_8026_L2_ACK_NOT_PENDING,   /* MT SDB has been received.
                                     Its L2 ack has been sent out as an
                                     MS ACK order since piggybag timer
                                     t2m(200 msec) expired.
                                     We are yet waiting for MO SDB.
                                  */

  CDMA_8026_L2_ACK_MO_SDB_NOT_PENDING /* Neither L2 ack nor MO SDB is
                                         pending
                                      */

} cdma_8026_call_status_type;
#endif /* FEATURE_SO_8026 */

/* Status of blocked wait */
typedef enum
{
  MCC_BLOCKED_WAITING,             /* MC task is doing a blocked wait */
  MCC_BLOCKED_WAIT_FAILURE,        /* MC task blocked wait failed */
  MCC_BLOCKED_WAIT_SUCCESS,        /* MC task blocked wait was successful */
  MCC_BLOCKED_WAIT_NONE            /* MC task isn't doing a blocked wait */
} mcc_blocked_wait_status_type; 

/* Status of transceiver request */
typedef enum
{
  MCC_TRANSCEIVER_RESOURCE_GRANTED, /* Granted RF resource */
  MCC_TRANSCEIVER_REQUEST_TIMEOUT,  /* Timeout on RF resource request */
  MCC_TRANSCEIVER_RESOURCE_LOST,     /* Lost RF resource */
  MCC_TRANSCEIVER_RESOURCE_DENIAL   /* RF resource denial */
} mcc_transceiver_request_status_type;

/* -----------------------------
** Message received channel type
** ----------------------------- */
typedef struct
{
  word                   cdmach;
  cai_rx_channel_id_type chan_type;
  byte                   chan_id;
} cdma_msg_rxed_chan_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Miscellaneous definintions                                              */

#define MCC_VALID_CONFIG (cai_tmo.t31m / 20L)
   /* Maximum time for which configuration parameters are considered
      valid (10 minutes in 20ms frames) */
      
#define PRI_PAGECH 1
  /* The primary paging channel (IS-95 section 7.1.3.4) */

/* Pseudo Pilot PN for other technology -> 1x handover. Pilot PN is 9-bit,
   0xffff will not collide with a valid pilot PN value.*/
#define PSEUDO_PILOT_PN 0xffff

/* Maximum number of groups */
#define BAND_INDEX_NA 0xFF     /* Band Index Not Applicable */

#define MAX_BAND_GROUPS 2

#define CHANGE_PAGE_CHAN_WAIT 80
  /* Time (in ms) after searcher is told to change paging channels
     at which we can be sure the new mask is in effect */

#define INVALID_TMSI_CODE 0xFFFFFFFFUL
  /* Value used to indicate invalid TMSI code */

#define IMSI_CLASS_0_ADDR_NUM 0xFF
#define IMSI_CLASS_1_ADDR_NUM_MAX 7

#define CLASS_1_IMSI(a)  \
((((byte) (a)) <= IMSI_CLASS_1_ADDR_NUM_MAX) ? TRUE : FALSE)
  /* cdma.imsi_addr_num (type byte) should be passed as "a". */

#define MCC_VALID_OVHD_TIMEOUT 30000
  /* Maximum time to obtain updated overhead messages arriving
     on the Paging Channel while operating in CDMA_IDLE_INIT state */

#ifdef FEATURE_1X_SPLIT_OVERHEAD
#define MCC_SPLIT_OVHD_REM_BS_INSANITY_TIME 10000
/* Remaining BS Insanity timer value during which SRCH should ignore 
 * the unlock callback from TRM and should block sending 
 * System Release rpt to MC while in split overhead collection */
#endif /* FEATURE_1X_SPLIT_OVERHEAD */

#define MCC_MIN_SCI    0

#define MCC_MAX_SCI    7
#define MCC_NULL_SCI  0x7f
#define MCC_DEFAULT_SCI 1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Constants for origination message translation                           */

#define MCC_DIAL_0    0xA
#define MCC_DIAL_STAR 0xB
#define MCC_DIAL_PND  0xC
#define MCC_DIAL_NULL 0x0
#define MCC_DIAL_ILLEGAL 0xFF

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                     BAND CLASS MASKS                                    */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                   OPERATING MODE MASKS                                  */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#define J_STD_008_OP_MODE_MASK (1 << CAI_J_STD_008_OP_MODE) /* ANSI JSTD008 */
#define IS95_CDMA_OP_MODE_MASK (1 << CAI_IS95_CDMA_OP_MODE) /* IS-95 CDMA   */
#define IS95_ANAL_OP_MODE_MASK (1 << CAI_IS95_ANAL_OP_MODE) /* IS-95 analog */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Specify which Band Class(es) and Operating Mode(s) is (are) supported   */
/* depending on which features are being compiled in.                      */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define SUPPORTED_OP_MODES (IS95_CDMA_OP_MODE_MASK | J_STD_008_OP_MODE_MASK)


/* Macros to check if the AMPS is supported for a given band class */
#define IS_AMPS_MODE_SUPPORTED (SUPPORTED_OP_MODES & IS95_ANAL_OP_MODE_MASK)

/*---------------MACRO for FEATURE_IS2000-----------------------*/
#define RC_CLASS(rc)  ( ((byte)rc > (byte)CAI_RC_2)? 2:1)
#define RATE_SET_OF_FWD_RC(rc) \
       ( (((byte)rc==(byte)CAI_RC_1)||  \
          ((byte)rc==(byte)CAI_RC_3)||  \
          ((byte)rc==(byte)CAI_RC_4)||  \
          ((byte)rc==(byte)CAI_RC_11)|| \
          ((byte)rc==(byte)CAI_RC_12) \
         )? 0:1)
#define RATE_SET_OF_REV_RC(rc) \
       ( (((byte)rc==(byte)CAI_RC_1)||  \
          ((byte)rc==(byte)CAI_RC_3)||  \
          ((byte)rc==(byte)CAI_RC_8) \
         )? 0:1)
#define RATE_SET_TO_MUX(rate_set) (rate_set+1)
#define MUX_TO_RATE_SET(mux) (mux-1)

/* Macros to check if RCs support 1X Advanced */
#define FWD_RC_SUPPORTS_1X_ADV(rc)  \
       ( ( ((byte)rc==(byte)CAI_RC_11)||  \
           ((byte)rc==(byte)CAI_RC_12)   \
         )? 1:0 )

#define REV_RC_SUPPORTS_1X_ADV(rc)   ( ( ((byte)rc==(byte)CAI_RC_8)  )? 1:0 )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Checks if the Band group is in the specified range
*/

#define MCC_BAND_GROUP_IN_RANGE(gp)  \
    ((gp > MCC_MIN_GROUP) && (gp < MCC_MAX_GROUP))

/* Band Groups.
*/
typedef enum
{
  MCC_MIN_GROUP = -1,

  MCC_CELLULAR_GROUP,
  MCC_PCS_GROUP,

  MCC_MAX_GROUP

} cdma_band_class_group_type;

/* Band class table row entry
*/
typedef struct
{
  cdma_band_class_group_type band_group;
  uint8 num_bands;
  uint8 band_class[CAI_MAX_BAND_CLASSES];
} mcc_band_group_type;

/* CDMA message types */
typedef enum
{
  CDMA_MSG_TYPE_NONE = 0,
  CDMA_MSG_TYPE_REG = (1 << 0),
  CDMA_MSG_TYPE_NORMAL_ORM = (1 << 1),
  CDMA_MSG_TYPE_OTASP_ORM = (1 << 2),
  CDMA_MSG_TYPE_EMERG_ORM = (1 << 3),
  CDMA_MSG_TYPE_PAGE = (1 << 4),
  CDMA_MSG_TYPE_DBM = (1 << 5),
  CDMA_ALL = 0xFF
} cdma_msg_type;

/* Mask to store messages to be blocked */
extern byte cdma_block_action_mask;

#ifdef FEATURE_IS2000_REL_A_AES
/* the following are location constants to remember the MS AES proposals */
#define MCCDMA_AES_REG_PROPOSE         0
#define MCCDMA_AES_ORIG_PROPOSE        1
#define MCCDMA_AES_PAGE_RESP_PROPOSE   2
#define MCCDMA_AES_AC_SMRM_PROPOSE     3
#define MCCDMA_AES_TC_SMRM_PROPOSE     4

#define MCCDMA_AES_NO_PROPOSE          0xff

#define CRC8_TABLE_SIZE  256
#define CRC8_SIZE        8
#endif /* FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_IS2000_1X_ADV
#define MCC_RCP_RECS_SIZE (CAI_RC_PARM_MAX * 2)
 /* Reserver twice the space for potential ASET pilots in case
    ECAM/(X)HDM is extended with RCP record in a GEM */

/* Invalid Pilot PN value while initializing RCP data */
#define MCC_INVALID_PILOT_PN 0xFFFF

/* INVALID Pilot PN to indicate nghbr info is not valid */
#define INVALID_PN 0xFFEE

/* Invalid SRCH window neighbor */
#define INVALID_WIN_NGHBR 0xFF

/* State of a pilot in Radio Configuration Parameters */
typedef enum
{
  MCC_RCP_PILOT_INVALID = 0,
   /* Pilot is not in ASET */
  MCC_RCP_PILOT_UNKNOWN,
   /* Pilot may be in ASET, will be known after ECAM or (X)HDM is processed */
  MCC_RCP_PILOT_ACTIVE
   /* Pilot is in ASET */
} mccdma_rcp_pilot_state;

/* Structure for parameters records for pilots assigned to the mobile */
typedef struct
{
  byte num_rc_params_rec;
   /* Number of RC Parameters records */
  struct
  {
    word pilot_pn;
     /* Pilot PN */
    mccdma_rcp_pilot_state pilot_state;
     /* State of the pilot */
    cai_rc_pilot_params_type rc_pilot_params;
     /* Parameters specific to a pilot */
  } rc_params_recs[MCC_RCP_RECS_SIZE];
} mccdma_pilots_rc_recs_type;

/* Structure for Radio configuration Parameters data stored in mobile */
typedef struct
{
  cai_rcp_rxc_txc_type rx_tx_params;
    /* Parameters required for RX/TX operation */
  mccdma_pilots_rc_recs_type pilots_recs;
    /* Parameters records for pilots assigned to the mobile */
} mccdma_rcp_data_type;

/* RCP record received in GEM */
extern caii_radio_config_params_type mcc_gem_rcp_rec;

/* Indicates if RCP record is received as a general extension record
 * with CAM/ECAM or (X)HDM or ESCAM */
extern boolean mcc_rcp_extended;

/* Buffer to store Radio Configuration Parameters */
extern mccdma_rcp_data_type mcc_rcp_data;

/* Buffer to store Radio Configuration Parameters during hard handoffs
   with return_if_fail clause */
extern mccdma_rcp_data_type mcc_rcp_data_old;
#endif /* FEATURE_IS2000_1X_ADV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Structure to hold the intermediate channel list on receiving the ECCLM */
typedef struct
{
  byte cdma_band;         /* CDMA band */
  word cdma_freq;         /* CDMA frequency */
  byte cdma_freq_weight;  /* CDMA frequency weight */
  byte ecclm_band_index;  /* Index to ECCLM num_band loop in ECCLM */
  byte ecclm_freq_index;  /* Index to ECCLM num_freq/num_add_freq */
} cdma_int_chn_list_type;

typedef struct
{
  uint8   band_class;
  uint8   pch;
  uint16  cdma_channel;
  uint16  pn;

} mccdma_pch_id_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type which contains fields of a matched order (unbundled)               */

typedef struct
{
  byte msg_type;            /* Message type */
  caii_pc_hdr_type hdr;     /* Paging Channel common header fields */
  byte order;               /* Order code */
  byte ordq;                /* Order qualification code */
  dword authbs;             /* For BS Challenge Confirmation Order only */
  byte roam_indi;           /* Enhanced Roaming Indication for CAI_REG_ACC */

  #ifdef FEATURE_IS2000_REL_A
  byte c_sig_encrypt_mode;    /* Signaling Encryption Mode */
  byte key_size;            /* Key Size */
  #endif

  #ifdef FEATURE_IS2000_REL_C
  boolean msg_int_info_incl;/* Message integrity info incl. */
  boolean change_keys;      /* Change keys indicator */
  boolean use_uak;          /* Use UIM Authentication Key indicator */
  #endif /* FEATURE_IS2000_REL_C */

  byte retry_type;          /* Retry type for CAI_RETRY_ORDER */
  byte retry_delay;         /* Retry delay for CAI_RETRY_ORDER */

} mccdma_pc_ord_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Union for received non-duplicate message addressed to the mobile        */

/* ------------------------------------------------------------------
** Note - The msg_type and hdr fields of all structures in this union
** can be accessed through the caii_gen_pc_type.  This union and the
** supporting order and channel assignment types allow processing of
** the layer 2 fields of a directed paging channel message to be done
** without reguard to the message type.
** ------------------------------------------------------------------ */

typedef union
{
  caii_gen_pc_type            gen_pc;
  mccdma_pc_ord_type          pc_ord; /* Defined above */
  mccdma_chnasn_type          chnasn; /* Defined above */
  caii_pc_tmsi_asgn_type      pc_tmsi;
  caii_pc_burst_type          pc_burst;
  caii_pc_auth_ch_type        pc_auth;
  caii_pc_ssd_type            pc_ssd;
  caii_pc_feature_type        pc_feat;
  caii_serv_redirect_type     serv_redir;
  caii_pc_streq_type          pc_streq;
  caii_paca_type              paca;

  #ifdef FEATURE_IS2000_REL_A
  caii_sec_mode_cmd_type  sec_mode_cmd;
  #endif

  #ifdef FEATURE_IS2000_REL_C
  caii_auth_req_type          auth_req;
  #endif

} mccdma_matched_msg_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type containing fields for Position Determination layer 2 ack information
*/
typedef struct
{
  byte ack_seq;         /* PD Msg sequence to ack */
  byte ack_type;

  /* Indicator of pending L2 Ack */
  cdma_pd_l2_ack_status_type ack_status;
  boolean l2_time_out;   /* Indicator of Layer2 time out */

} mcc_pd_layer2_type;


#ifdef FEATURE_SO_8026
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type containing fields for Proprietary Service Option layer 2 ack
 * information and call flow status
*/
typedef struct
{
  boolean mcc_is_so_8026;

  byte ack_seq;         /* 8026 Msg sequence to ack */
  byte ack_type;        /* The ack type */

  /* Indicator of call flow status */
  cdma_8026_call_status_type call_status;

} mcc_8026_call_info_type;
#endif /* FEATURE_SO_8026 */

#ifdef FEATURE_GPSONE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type containing fields for Position Determination Fast Dormancy Information
*/
typedef struct
{
  word cdma_ch;           /* CDMA channel base station is using */
  word pilot_pn;          /* pilot pn offset */
  word base_id;           /* Base station identification */
  word sid;               /* System Identification */
  word nid;               /* Network Identification */
  byte band_class;        /* Band class of the base station */
  boolean dorm_ind;       /* Indicate PD if fast dormancy is needed. */
  qword ts;               /* Time of receiving SPM */
  boolean still_active;   /* Indicator of serving system in Active Set */
  uint32 base_lat;        /* Base Station Latitude */
  uint32 base_long;       /* Base Station Longitude */

} mcc_pd_ft_dm_info_type;
#endif /* FEATURE_GPSONE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type containing fields for layer 2 processing of paging channel msgs    */

typedef struct
{
  byte msg_seq;
  byte ack_seq;
  byte ack_type;
  byte valid_ack;
  boolean idle_msg_pending;
  mccdma_matched_msg_type pending_idle_msg;

} mcc_layer2_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type and queues for DTMF command procesing                              */

typedef struct
{
  q_link_type link;        /* Queue link */
  mc_name_type dtmf_cmd;   /* Type of DTMF command received from UI */
  union
  {
    word ms;      /* Number of ms key depressed (MC_STOP_CONT_DTMF) */
    byte digit;               /* Digit to send (MC_START_CONT_DTMF) */
    mc_burst_dtmf_type burst; /* Burst DTMF data (MC_BURST_DTMF_F)  */
  } parm;
} cdma_dtmf_type;

/* Maximum number of outstanding DTMF commands */
#define MCC_MAX_DTMFS_OUT 30

extern q_type mcc_dtmf_q;
  /* queue to place outstanding dtmf commands */

#ifndef FEATURE_1X_CP_MEM_OPT
extern q_type mcc_dtmf_free_q;
  /* queue to place free buffers to be placed on the mcc_dtmf_free_q */

/* Buffers for outstanding continuous DTMF commands */
extern cdma_dtmf_type cdma_dtmf_bufs[];
#endif /* !FEATURE_1X_CP_MEM_OPT */

/* Default value for prev_pilot                                            */
#define NO_PREV_PILOT -1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type for Pilot Report Processing                                        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Structure to hold pilot reporting overhead information */

typedef struct
{
  word msg_len;
    /* Length of Access Channel message excluding pilot report information */
  byte num_pilots_allowed;
    /* Number of pilots that can be reported in Access Channel Msg */
  int2 first_pilot;
    /* Active pilot of first access probe */
  int2 prev_pilot;
    /* Active pilot for the immediately previous access probe */
  byte num_pilots_attempted;
    /* Number of pilots that has been access attempted */
  boolean first_probe;
    /* Flag if first access probe */
  boolean apho_rebuild;
    /* Flag if ACCESS_HO_LIST need to build because of access probe handoff */
  cdma_nghbr_set_type attempt_list[CAI_N13M];
    /* List of pilots that have been access attempted */

} plt_rpt_ovhd_rec_type;

/* Structure to hold Access Handoff List */

typedef struct
{
  int2 pilot_pn;
    /* ---------------------------------------------------------------------
    ** Pilot PN is used as a key to perform the record matching of Access
    ** Handoff List against the Searcher Pilot Strength Measurement report
    ** during update
    ** ---------------------------------------------------------------------*/
  int2    pilot_pn_phase;   /* Pilot pn phase */
  byte    pilot_strength;   /* Pilot strength */
  boolean access_attempted; /* Whether access is attempted */

} aho_pilot_info_type ;

typedef struct
{
  byte                num_pilots; /* number of pilots in the list */
  aho_pilot_info_type pilot_list[CAI_N13M];

} aho_list_type;

/* Structure to hold Other Reported List */

typedef struct
{
  int2    pilot_pn;
    /* -----------------------------------------------------------------
    ** Pilot PN is used as a key to perform the record matching of Other
    ** Reported List against the SRCH NSet during swapping
    ** ----------------------------------------------------------------- */
  int2    pilot_pn_phase;   /* Pilot pn phase */
  byte    pilot_strength;   /* Pilot strength */
} othrpt_pilot_info_type ;

typedef struct
{
 byte                   num_pilots; /* number of pilots in the list */
 othrpt_pilot_info_type pilot_list[CAI_MAX_ADD_PILOTS];
} othrpt_list_type;

#ifndef FEATURE_1X_CP_MEM_OPT
#ifdef FEATURE_UIM_RUIM
  /* Buffer used to retrieve the result of R-UIM operations */
extern byte uim_access_buffer[UIM_MAX_CHARS];
#endif /* FEATURE_UIM_RUIM */
#endif /* !FEATURE_1X_CP_MEM_OPT */

extern cdma_bs_type *mcc_saved_current_bs_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                    MACROS                                               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_IGNORE_GSRDM
/* Macro to determine GSRDM ignore timer value (15 - mins) */
#define GSRDM_IGNORE_TIMER_VAL_MS 900000
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */

#ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
/* Macro for bs_info[] heap memory cleanup timer value (30 - mins) */
#define BS_INFO_HEAP_MEM_TIMER_VAL_MIN 30
#endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */

/* Macro to determine if additional overhead messages are required before
   a System Access can be attempted */

#define INVALID_SEQ 0xff
#define TEMP_NL_SEQ 0x7f
  /* CONFIG_MSG_SEQ is 6 bits, so anything over 0x3f is invalid */

#define CLR_ACC_MSG_SEQ() \
    (cur_bs_ptr->rx.acc_msg_seq = INVALID_SEQ)

#define AP_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.ap_rx && ( cur_bs_ptr->ap.acc_msg_seq == cur_bs_ptr->rx.acc_msg_seq ) )

/* GNLM, NLM, SPM, ESPM, CLM, ECLM, AMS41SPM, MCRRPM, UNLM, GSRDM and EGSRDM are
   considered as received if the rx flag of the overhead message is equal to TRUE and
   one of the following condition is fullfilled:
   1) rx.config_msg_seq is not equal to INVALID_SEQ and the overhead message's
      config_msg_seq value is equal to rx.config_msg_seq.

   OR

   2) rx.config_msg_seq is equal to INVALID_SEQ and the overhead message's
      config_msg_seq value is not equal to INVALID_SEQ.

      (Notice: We get into this condition after idle handoff. After idle handoff,
      we force MS to stay awake to wait for a GPM, UPM or any overhead message
      with config_msg_seq by changing the value of rx.config_msg_seq to
      INVALID_SEQ instead of forcing the MS to wait for a GPM or UPM in assigned
      slot that could takes a long time which hurts standby time. After MS idle on
      the new pilot, the value of rx.config_msg_seq will be updated when a GPM, UPM
      or any ovehead with config_msg_seq is received.
*/

#define GNLM_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.gnl_rx && ( cur_bs_ptr->gnl_config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->gnl_config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#define GNLM_MSG_NOT_RECVD  ( !GNLM_MSG_IS_RECVD )

#define NLM_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.nl_rx && ( cur_bs_ptr->nl_config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->nl_config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

#define SP_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.sp_rx && ( cur_bs_ptr->csp.sp.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->csp.sp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#define SP_MSG_NOT_RECVD ( !SP_MSG_IS_RECVD )

#ifdef FEATURE_1X_TO_LTE
#define ATIM_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.atim_rx && ( cur_bs_ptr->atim_config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->atim_config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#endif /* FEATURE_1X_TO_LTE */

#define ESP_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.esp_rx && (cur_bs_ptr->csp.esp.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->csp.esp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#define ESP_MSG_NOT_RECVD ( !ESP_MSG_IS_RECVD )

#define NGHBR_LIST_MSGS_NOT_RECVD ( !NLM_MSG_IS_RECVD || !GNLM_MSG_IS_RECVD )

#define CHAN_LIST_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.cl_rx && ( cur_bs_ptr->cl.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->cl.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

#define GSR_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.gsd_rx && ( cur_bs_ptr->gsd.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->gsd.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

#define EXT_CHAN_LIST_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.ecl_rx && ( cur_bs_ptr->ecl.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->ecl.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

#define EGSR_MSG_IS_RECVD \
    ( cur_bs_ptr != NULL && cur_bs_ptr->rx.egsd_rx && ( cur_bs_ptr->egsd.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->egsd.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

/* Macro to determine if a PN offset is valid */
#define VALID_PN( pn )  \
  ( cdma_nghbrs.pilot_inc ? ((pn % cdma_nghbrs.pilot_inc ) == 0 ) : FALSE )

/* Macro to determine if a code channel is valid */
#define VALID_CODE_CHN( chn)  \
  ( (chn > 0) && (chn < 128 ) ) //Range extended for FEATURE_IS2000
/* Macro to determine if a QOF mask id is valid */
#define VALID_QOF_MASK_ID( id)  \
  ( id < 4 ) //Range extended for FEATURE_IS2000

/* Macro to determine next ACK_REQ_SEQ to be sent to the
   base station */
/*
#define NEXT_ACK( ack_req ) \
  ( ( ack_req + 1) % 3 )
*/

/* Macro to find main state from state variable      */

#define MAIN_STATE(state)  (state & 0xFF00 )

#ifdef FEATURE_CSIM
#define MC_INVALID_CDMA_CLIENT_ID 0xFFFF
#define MC_INVALID_CDMA_SESSION_ID 0xFFFF

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define  MCC_MMGSDI_RSP_SIG      0x0002
#endif

typedef enum
{
  SLOTTED_PAGE,                   /* Slotted Page Message */
  NON_SLOTTED_PAGE,               /* Page Message */
  GENERAL_PAGE                    /* General Page Message */

} mcc_page_enum_type;

typedef struct
{
  byte ack_type;                  /* Ack type */
  byte msg_seq;                   /* msg_seq of page addressed to mobile */
  boolean special_service;        /* whether or not the page included SO */
  word service_option;            /* Service Option (SO) included */
  dword tmsi_code;                /* TMSI code from general page record */

} mcc_page_info_type;

typedef struct
{
  boolean                 return_if_fail; /* Return if fail flag */
  byte                    num_recs;       /* Number of redirection records */
  sd_redir_rec_s_type     *redir_rec_list;/* Redirection record list ptr */

} mcc_redir_info_type;

typedef enum
{
  MEAS_BACK_IGNORED, /* Ignore background system measurement request  */
  MEAS_BACK_ALLOWED, /* Allow background system measurement request   */
  MEAS_BACK_EXIT     /* Exit on background system measurement request */

} mcc_sys_meas_mode_type;

/* ----------------------------
** CDMA Channel monitoring mask
** ---------------------------- */
#define MCC_NONE         0x00
#define MCC_PCH          0x01

/* ---------------------------
** Channel monitoring priority
** --------------------------- */
typedef enum
{
  MCC_FIRST_PRIORITY,
  MCC_SECOND_PRIORITY,
  MCC_INVALID_PRIORITY

} mcc_channel_monitor_priority_type;

/* ------------------
** FER info reporting
** ------------------ */
#define MCC_FER_INFO_INTERVAL 5000
#define MCC_FER_UNKNOWN 0XFFFF

/* Keys used to identify BS info */
typedef struct
{
  caix_chan_type  ovhd_chan;  /* Type of overhead channel used */
  word            pilot_pn;   /* Pilot PNs */
  word            cdma_ch;    /* Current CDMA Channel number */
  byte            band_class; /* Band class */
  byte            ovhd_walsh_ch; /* Current Broadcast Control or Paging Channel number */
  word            sid;        /* System identification */
  word            nid;        /* Network identification */
} bs_info_id_type;

/*===========================================================================

                      DATA DECLARATIONS

===========================================================================*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   External declarations for structures declared in mc_cdma.c            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* if Data Services are compiled in */
/* Regional definitions to support DS-originated voice calls               */
extern boolean mcc_dsvoice;   /* indicates Voice call originated by DS task */

/* Internal format of Status Response message to be transmitted
   on the Access Channel */
extern caii_ac_strsp_type cdma_strsp;

#ifdef FEATURE_CP_MEM_REDUCTION
#define MC_MAX_BASES 5
  /* Maximum number of base stations for which overhead information will
     be stored */
#else
#define MC_MAX_BASES 10
#endif /* FEATURE_CP_MEM_REDUCTION */

#ifdef FEATURE_1X_CP_MEM_OPT
/* Base Station specific variables */
extern cdma_bs_type *bs_info;
#else /* !FEATURE_1X_CP_MEM_OPT */
/* Base Station specific variables */
extern cdma_bs_type bs_info[MC_MAX_BASES];
#endif /* FEATURE_1X_CP_MEM_OPT */

#define INVALID_SID_NID_VAL 0xFFFF
/* Pointer to Previous Base Station in Base Station Information array */
extern cdma_bs_type *last_bs_ptr;

/* Last state of CDMA state machine to process */
extern word cdma_last_state;

#define MCC_MAX_ACK_TYPES 2
#define MCC_MAX_MSG_SEQ 8

  #define MCC_MAX_ADDR_TYPE 8
  /* IMSI_S, ESN, IMSI, TMSI, BROADCAST. There are 3 unused fields, that  */
  /* are in the array.                                                    */
  /* Refer to sections 6.6.2.1.2 and 7.7.2.3.1 in IS-95B for more info    */

/* Structure containing Paging Channel or Forward Common Control Channel  */
/* duplicate msg sequence time stamps for non-page messages.              */
extern qword
  mcc_idle_frames[MCC_MAX_ACK_TYPES][MCC_MAX_MSG_SEQ][MCC_MAX_ADDR_TYPE];

/* Structure containing Paging Channel or Forward Common Control Channel  */
/* duplicate msg sequence time stamps for page records.                   */
extern qword
  mcc_idle_pg_frames[MCC_MAX_MSG_SEQ];


/* Timers */
extern rex_timer_type  mcc_srv_timer;
extern rex_timer_type  mcc_full_tmsi_timer;
extern rex_timer_type  mcc_tmsi_exp_timer;
extern rex_timer_type  mcc_slotted_timer;  /* Slotted Timer */


extern rex_timer_type  mcc_l2_ack_timer;


#ifdef FEATURE_GPSONE
extern mcc_pd_ft_dm_info_type mcc_pd_ft_dm_info;
/* Buffer saving proper Base Station Info for PD session. */
#endif /* FEATURE_GPSONE */

#ifdef FEATURE_SO_8026
extern rex_timer_type  mcc_8026_l2_ack_timer;
#endif /* FEATURE_SO_8026 */

/* this timer is a callback timer. when a transceiver request
 * times out on waiting for resource, the callback function
 * will be called. This timer is originally defined in mccdma.c
 */
extern rex_timer_type mcc_tr_request_timer;

extern rex_timer_type mcc_t50_timer;
extern rex_timer_type mcc_t51_timer;

extern rex_timer_type mcc_fer_info_timer;

extern rex_timer_type  mcc_access_guard_timer;
  /* Timer for guarding access attempts */

/* Call back timer for reporting CDMA statistics to upper layers */
extern timer_type mcc_stats_reporting_timer;

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
/* PPP_CLEANUP_NON_SV_MODE:Deregister powerdown / powerup time out timer */
extern timer_type mccreg_deregister_timer;
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_1X_CP_IGNORE_GSRDM
/* GSRDM Ignore timer */
extern rex_timer_type gsrdm_ignore_timer;
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */

#ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
/* bs_info[] heap memory cleanup timer */
extern rex_timer_type bs_info_heap_mem_timer;
#endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* CSFB CGI Req Timer */
extern rex_timer_type mcc_csfb_cgi_req_timer;

/* CSFB Retain MT Page Context Timer */
extern timer_type mcc_csfb_retain_mt_page_cxt_timer;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* 1X2G Tune Away HHO defer timer */
extern rex_timer_type mcc_ta_hho_defer_timer;

/* 1X2G Tune Away HHO execution timer */
extern timer_type mcc_ta_hho_exec_timer;

/* 1X2G Tune Away SHO defer timer */
extern rex_timer_type mcc_ta_sho_defer_timer;

/* 1X2G Tune Away CFS defer timer */
extern rex_timer_type mcc_ta_cfs_defer_timer;

/* 1X2G Tune Away Prioriy Inversion timer */
extern rex_timer_type mcc_ta_prio_inv_timer;

extern nv_cmd_type mcc_nv_buf;      /* Buffer for command to NV task */
extern nv_item_type mcc_nv_data;    /* Buffer for data from NV task */
extern rxtx_cmd_type mcc_rxtx_buf;  /* Buffer for command to Layer 2 task */

#ifdef FEATURE_CSIM
extern mmgsdi_return_enum_type mcc_mmgsdi_rpt_status;
extern mmgsdi_cnf_type *mcc_mmgsdi_rpt_ptr;
#endif /* FEATURE_CSIM */

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
extern boolean mcc_is_mvs_acquired;
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

/* Debug handoff parameters */
typedef struct
{
  byte t_add;
  byte t_drop;
  byte t_tdrop;
  byte t_comp;
  byte win_a;
  byte win_n;
  byte win_r;
  byte nghbr_max_age;

  byte soft_slope;
  int1 add_intercept;
  int1 drop_intercept;

} mcc_ho_cb_type;

/* Last entry reason to CDMA and Analog call processing  */
typedef struct
{
  mcc_entry_reason_type cdma;
    /* Last entry reason to CDMA call processing */

} mcc_status_type;

extern mcc_ho_cb_type mcc_ho_debug;
extern mcc_ho_cb_type mcc_ho_real;

/* Internal format of (Enhanced) Access Channel Message                               */
extern caii_tx_msg_type mcc_tx_int_msg;

/* External format of (Enhanced) Access Channel Message                               */
extern cai_tx_msg_type mcc_tx_ext_msg;

/* Information needed for Paging/Access Channel Layer 2 processing         */
extern mcc_layer2_type mcc_layer2;


/* Structure containing Preferred Voice Service Option information         */
extern nv_pref_voice_so_type mcc_pref_voice_so;


extern boolean mcc_is_config_chgd;

/* Set values as bit positions to allow for possible treatment like a
** bit mask.
*/
typedef enum
{
  NO_MATCH = 0,
  PAGE_MATCH = 1,
  BROADCAST_MATCH = 2
} mccdma_page_match_result_type;

/* This variable should be replaced by mcc_is_bcsms_enabled() */
extern boolean mcc_bc_sms_enable;

/* buffer to hold the copy of the roaming list that is in NV */
extern nv_roaming_list_type mcc_roaming_list;

extern mcc_pd_layer2_type mcc_pd_layer2;

/* The maximum that can be supported now is
   MAX_BAND_GROUPS (2). Please increase the value of
   MAX_BAND_GROUPS when adding more than 2 Band Groups in
   Band Class table
*/
extern mcc_band_group_type mcc_band_table[MAX_BAND_GROUPS];

#ifdef FEATURE_SO_8026
extern mcc_8026_call_info_type mccsa_8026_call_info;
#endif /* FEATURE_SO_8026 */

/*
   mcc_burst_overflow_size contains the number of bytes the Burst message
   (PD or SDB) is exceeding its limit of successful transmission.
*/
extern uint16 mcc_burst_overflow_size;

/* The variable to store the information if the idle hard handoff happened
** or not. This varaible is being made global instead of already present
** local variable in mcidl_handoff. It is done so as to reduce the call
** setup time for redirection based CSFB calls */
extern boolean mcc_idle_hard_handoff;

#ifdef FEATURE_CSIM
extern mmgsdi_app_enum_type mcmmgsdi_app_type;
#endif /* FEATURE_CSIM */

/* Buffer to hold CDMA statistics */
extern sys_cdma_information_s_type    cdma_stats;

/* Current bitmask for CDMA statistics */
extern uint32                         cdma_stats_mask;

/* Timer duration for CDMA statistics reporting to CM */
#define MCC_STATS_REPORTING_INTERVAL  3000  /* 3 seconds */

/* Flag to indicate whether power down and power up registrations post OTASP
 * are pending or not. This flag is set to TRUE when call ends after successful
 * OTASP commit. It is set to FALSE when power down and power up registrations
 * procedures are done.
 */
extern boolean mcc_post_otasp_reg_pending;

extern nv_roaming_list_type nv_pr_list; /* The Preferred Roaming List */

#ifdef FEATURE_1X_CUST_VOICE_SO
/* Callback type to register API to handle customer specific service option */
typedef void (*mcc_encryption_callback_type) ( qword sys_time,
                                           uint8  *frame_buf, 
                                           uint32 frame_offset,
                                           uint32 frame_bits );

/* Callback type to register API to handle customer specific service option */
typedef void (*mcc_decryption_callback_type) ( qword sys_time,
                                           uint8  *frame_buf, 
                                           uint32 frame_offset,
                                           uint32 frame_bits );

/* Placeholder for API to handle customer specific service option */
extern mcc_encryption_callback_type cust_voice_encrypt_algorithm;

/* Placeholder for API to handle customer specific service option */
extern mcc_decryption_callback_type cust_voice_decrypt_algorithm;
#endif /* FEATURE_1X_CUST_VOICE_SO */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enum used to indicate what message type to retrieve from BS Info array  */

typedef enum
{
  MC_SYNC_MSG = 0x10,
       /* a Sync Channel message type */
  MC_RX_MSG
    /* Message receipt structure */

} cdma_mc_msg_type;

/* Structure containing exit information */
extern mcc_exit_type mcc_exit;

/* Timers */
extern rex_timer_type mcc_state_timer;
extern rex_timer_type  mcc_action_timer;
extern rex_timer_type  mcc_cmd_timer;
extern rex_timer_type  mcc_bs_insanity_timer;
extern rex_timer_type  mcc_so_ctl_timer;

extern txc_cmd_type mcc_tx_buf;     /* Buffer for command to Transmit task */
extern rxc_cmd_msg_type mcc_rx_buf; /* Buffer for command to Receive task */
extern srch_cmd_type mcc_srch_buf;  /* Buffer for command to Search task */

#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
/* No of ASIDs (subscriptions) for the UE */
#define NUM_ASID 2

/* Voice Adapter parameters for each ASID received during adapter registration
 * as part of OPEN command */
extern onex_voc_cmd_open_t vadapter_params_map[NUM_ASID];

/* Voice Adapter parameters for ASID on which CDMA is currently active */
extern onex_voc_cmd_open_t vadapter_params;
#endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */

/* ASID for subscription on which CDMA is currently active */
extern sys_modem_as_id_e_type mcc_asid;

/* Bit mask for the active RATs in the current subscription. This will be
 * used by 1X-CP to derive the VSID corresponding to current subscription
 * and the VSTMR id to be configured */
extern uint32 mcc_curr_sub_rat_mask;

/* Need to buffer a copy of UHDM msg if CFS with WTR Hop is in Progress */
extern mccrxtx_msg_type *mcc_temp_uhdm_msg_ptr;

/* 1X2GTA feature related variables */
#define ONEX2GTA_TUNEAWAY_NOT_REGISTERED 0xFFFFFFFF

/* 1X2GTA: QTA Reject Threshold time (ms) past the registered TA Time */
#define ONEX2GTA_QTA_REJ_THRESHOLD_TIME 8

/* 1X2GTA:Minimum required time in msec before next TA to proceed with HHO */
#define ONEX2GTA_HHO_MIN_REQ_TIME 250

/* 1X2GTA: Allowed HHO execution time in msec before honoring TA */
#define ONEX2GTA_HHO_ALLOWED_EXEC_TIME 500

/* 1X2GTA: Max HHO deferrable time in msec */
#define ONEX2GTA_HHO_MAX_DEF_TIME 2000

/* 1X2GTA: Max SHO deferrable time in msec */
#define ONEX2GTA_SHO_MAX_DEF_TIME 2000

/* 1X2GTA: Max CFS deferrable time in msec */
#define ONEX2GTA_CFS_MAX_DEF_TIME 2000

/* 1X2GTA: Max CFS without WTR req proc time in msec */
#define ONEX2GTA_CFS_MIN_REQ_TIME 120

/* 1X2GTA: Min buffer time in msec after TA complete 
 * to process pending CFS Rpt */
#define ONEX2GTA_TA_COMP_BUF_TIME 100

/* 1X2GTA: Max Prioriy Inversion time in msec  */
#define ONEX2GTA_TA_PRIO_INV_TIME 500

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MCC_GET_SLOT_CYCLE_INDEX

DESCRIPTION
  Returns the slot cycle index preferred by the user.

DEPENDENCIES

RETURN VALUE
  Slot cycle index.

SIDE EFFECTS

===========================================================================*/

extern int8 mcc_get_slot_cycle_index(void);


/*===========================================================================

FUNCTION mccidl_clear_pd_layer2_ack

DESCRIPTION
  Clear PD L2 Ack status and Layer2 Ack timer.

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS

===========================================================================*/

extern void mccidl_clear_pd_layer2_ack ( void );


/*===========================================================================

FUNCTION MCCIDL_ASSERT_SLEEP_OK

DESCRIPTION
  This function asserts the mccidl_sleep_ok signal.

  If two events occur which both trigger a sleep, the MCC task may go
  to sleep before the second trigger is removed from the queue.  Any
  sleep command which was generated before the "sleep_ok_time" is
  ignored to prevent this lingering sleep trigger from causing the
  mobile to go to sleep immediately after waking up.

DEPENDENCIES
  None.

RETURN VALUE
  true      sleep ok was asserted.
  false     sleep ok was not asserted.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean mccidl_assert_sleep_ok
(
  qword rpt_time
    /* Time the sleep trigger was issued */
);

/*===========================================================================

FUNCTION CDMA_INIT

DESCRIPTION
  This procedure determines which substate of the initialization substate
  should be processed and vectors control to the appropriate procedure
  to process the substate.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word cdma_init( void );     /* Initialization state */

/*===========================================================================

FUNCTION CDMA_IDLE

DESCRIPTION
  This procedure determines which mode of idle operation is required and
  calls the appropriate procedure to handle the designated mode.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word cdma_idle( void );     /* Idle state */

/*===========================================================================

FUNCTION CDMA_SA

DESCRIPTION
  This function is the lowest level state machine for the System Acces
  state.  It determines which sub-state needs to be processed and calls
  the appropriate procedure to process the sub-state.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  The mobile will attempt to access the system and send a message to the
  base station during the System Access state.

===========================================================================*/

extern word cdma_sa( void );       /* System Access state */

/*===========================================================================

FUNCTION CDMA_TC

DESCRIPTION
  This function is the lowest level state machine for the Mobile Station
  Control on the Traffic Channel state. It determines which sub-state needs to
  be processed and calls the appropriate procedure to process the sub-state.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word cdma_tc( void );       /* Traffic Channel state */

/*===========================================================================

FUNCTION MCC_GET_MSG

DESCRIPTION
  This procedure checks inside the base station information array for a
  message received for the currently active base station and returns a
  pointer to the requested message if a valid message is found.

DEPENDENCIES
  Requested Message must be one that is stored in the base station
  information array.

RETURN VALUE
  A pointer to the requested message if a valid message is found.  NULL if
  the message is not found.

SIDE EFFECTS
  During this procedure the base station information array will be checked
  for old information.  Any base station whose information is over 10
  minutes old will be marked inactive.

===========================================================================*/

extern void *mcc_get_msg
(
  word msg_type
    /* message type to retrieve */
);

/*===========================================================================

FUNCTION MCC_GET_BS_INFO

DESCRIPTION
  This procedure checks inside the base station information array for an
  entry for the specified paging channel and returns a pointer to the
  information if the entry is found.  The time input parameter specifies
  when the information message will be used (thus can be used to check for
  expiration).  If NULL (a pointer) time is specified, the information will
  be returned without checking for expiration.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the requested information is found.  FALSE otherwise.

SIDE EFFECTS
  During this procedure the base station information array will be checked
  for old information.  Any base station whose information is over 10
  minutes old will be marked inactive.

===========================================================================*/

boolean mcc_get_bs_info
(
  bs_info_id_type *bs_id,
    /* specifies the broadcast control or paging channel to which the message
       belongs */
  qword info_use_time,
    /* the time the bs_info is used */
  cdma_bs_type **bs
    /* for returning the BS info */
);

/*===========================================================================

FUNCTION MCC_PUT_SEQ

DESCRIPTION
  This procedure places a message sequence number (overhead or access
  parameters) into the base station information array for the currently
  active base station.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Message receipt flags for the base station may change.

===========================================================================*/

extern void mcc_put_seq
(
  byte seq_num,                  /* sequence number to insert */
  boolean acc_seq                /* TRUE = acc_msg_seq, FALSE = ovhd_msg_seq */
);

/*===========================================================================

FUNCTION MCC_PROCESS_REORDER

DESCRIPTION
  This function processes a reorder order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_process_reorder(void);

/*===========================================================================

FUNCTION PROCESS_INTERCEPT

DESCRIPTION
  This function processes an intercept order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void process_intercept(word curr_state);

/*===========================================================================

FUNCTION MCC_MS_ORIG

DESCRIPTION
  This function processes the Mobile Station Origination operation.
  During this operation the number dialed by the user is stored.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the header within the input mc_msg_type requests that the buffer
  be placed on a queue after processing is completed this procedure will
  do so.

===========================================================================*/

extern void mcc_ms_orig
(
  mc_origination_type * orig_parms
    /* Pointer to message received from the handset */
);

/*===========================================================================

FUNCTION MCC_MS_SMS

DESCRIPTION
  This function processes the Mobile Station SMS operation.
  During this operation the SMS message data sent by the user is stored.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the header within the input mc_msg_type requests that the buffer
  be placed on a queue after processing is completed this procedure will
  do so.

===========================================================================*/

extern void mcc_ms_sms
(
  mc_msg_type *cmd_ptr
    /* Pointer to message received from the handset */
);

/*===========================================================================

FUNCTION MCC_VALIDATE_SO_REQ

DESCRIPTION
  Determines if the service option request is acceptable

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets value of cdma.so_req and cdma.service_option_1

===========================================================================*/

extern void mcc_validate_so_req
(
  mcc_page_info_type *page_info
    /* Pointer to the body of the matched page */
);

/*===========================================================================

FUNCTION MCC_COMP_ADDR

DESCRIPTION
  This function takes a pointer to the header of a paging channel message
  and checks to see if it is addressed to the mobile.
  When IMSI addressing gets added this function will become much larger.
  This function is called from mccidl.c and mccsa.c.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the address matches, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_comp_addr
(
  caii_pc_hdr_type *msg_hdr,
    /* Header of paging channel message to match address of */
  qword msg_frame
    /* Msg frame, in 20 ms */
);

/*===========================================================================

FUNCTION MCC_MATCH_MSG

DESCRIPTION
  This function checks all messages except page messages to see if the
  address matches and if the message is not a duplicate.  If both are
  true the message is coppied into  matched_message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the address of the message matches and the message is
  not a duplicate, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean mcc_match_msg
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mccdma_matched_msg_type *mm_ptr,
    /* Pointer to struct to copy message into if match is found. */
  boolean store_chnasn_for_tci,
    /* Indicates if TC CAM and ECAM should be stored for TCI operation. */
  word curr_state
    /* State currently being processed */
);

/*===========================================================================

FUNCTION MCC_PARSE_PAGE

DESCRIPTION
  This function takes a pointer to a page message and searches through all
  the pages in it to see if any are addressed to the mobile.  If one is
  found, information from the page is coppied into a general structure
  to avoid having to parse the page twice. Since a mobile specific page match
  has highest priority, if a match is found, all other page records can be
  discarded, if not found, all page records (including broadcast pages if
  supported) need to be checked.  This function is called from mccidl.c and
  mccsa.c.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a page is found which is addressed to the mobile and is not a
  duplicate.  FALSE otherwise.

SIDE EFFECTS
  Stores the new config and access message sequences
  If broadcast paging is supported and broadcast pages are found, the
  BURST_TYPEs and BC_ADDRs fields are copied into mccdma_bc_page_rec[].

===========================================================================*/

extern mccdma_page_match_result_type mcc_parse_page
(
  word new_state,
    /* Next state to be processed */
  boolean pending_bcast,
    /* Indicates if it is a pending broadcast page */
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mcc_page_info_type *page_info
);

/*===========================================================================

FUNCTION MCC_RXTX_CMD

DESCRIPTION
  This function sends a command to the Layer 2 task.  If the Layer 2
  responds with a status other than RXTX_DONE_S, this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_rxtx_cmd
(
  rxtx_cmd_type *cmd_ptr
    /* pointer to command to send to Layer 2 task */
);

/*===========================================================================

FUNCTION MCC_NV_CMD
  This function sends a command to the NV task.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  Status returned by nv_cmd procedure.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

extern nv_stat_enum_type mcc_nv_cmd
(
  nv_cmd_type *cmd_ptr
    /* Pointer to command to send to NV */
);

/*===========================================================================

FUNCTION MCC_MMGSDI_READ_PRL

DESCRIPTION
  This API is used to read NV#257 - NV_ROAMING_LIST_683_I in card mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_mmgsdi_read_prl( void );

/*===========================================================================

FUNCTION MCC_ENCODE_DTMF

DESCRIPTION
  This function converts a called-address digit received from UI
  (in ASCII format) to BCD format.

DEPENDENCIES
  None

RETURN VALUE
  Translated character.

SIDE EFFECTS
  None

===========================================================================*/

extern byte mcc_encode_dtmf
(
  byte ui_char
    /* ASCII digit received from the user interface */
);

/*===========================================================================

FUNCTION MCC_DECODE_DTMF

DESCRIPTION
  This function converts a BCD encoded called-address digit
  to ASCII format.

DEPENDENCIES
  None

RETURN VALUE
  Translated character.

SIDE EFFECTS
  None

===========================================================================*/

extern byte mcc_decode_dtmf
(
  byte bcd_digit
    /* BCD digit to be converted to ASCII equivalent */
);

/*===========================================================================

FUNCTION MCC_GET_MIN_INFO

DESCRIPTION
  This function loads the CDMA structure with the information for the
  orig_min.

DEPENDENCIES
  Depends on current nam being in cdma structure.

RETURN VALUE
  Indicator of whether all parameters were successfully read from NV.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_get_min_info ( void );

/*===========================================================================

FUNCTION MCC_DETECT_DUPLICATE_ECAM

DESCRIPTION
  This function detects whether a duplicate ECAM message has been received
  on the Paging Channel. This function does not record the time, because the
  main mcc_detect_duplicate function takes care of that. This function is
  called only for detecting duplicate ECAMs so that we dont consider it as
  an ack for the previous transmission.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - A duplicate ECAM was received on the Paging Channel.
  FALSE - The received ECAM was NOT a duplicate.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_detect_duplicate_ecam
(
  /* Header of ECAM to process layer 2 fields of */
  caii_pc_hdr_type *hdr
);

/*===========================================================================

FUNCTION MCC_DETECT_DUPLICATE

DESCRIPTION
  This function detects whether a duplicate message has been received
  on the Paging Channel.  If a duplicate message was NOT received this
  function records the time at which the message was received.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - A duplicate message was received on the Paging Channel.
  FALSE - The received Paging Channel message was NOT a duplicate.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_detect_duplicate
(
  boolean page,
    /* Identify if page or non-page message */
  caii_pc_hdr_type *hdr
    /* Header of paging channel message to process layer 2 fields of */
);

/*===========================================================================

FUNCTION MCC_RESET_IDLE_FRAMES

DESCRIPTION
  This function resets the mcc_idle_frames and mcc_idle_pg_frames array
  to indicate that no messages received on the Paging Channel or Forward
  Common Control Channel will be duplicates.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_reset_idle_frames( void );

/*===========================================================================

FUNCTION MCC_IDLE_HO_RX

DESCRIPTION
  This procedure clears messages received from the current base station and
  sends a request to the receive task to perform idle handoff processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mcc_rxtx_q will be emptied.

===========================================================================*/

extern void mcc_idle_ho_rx( void );

/*===========================================================================

FUNCTION MCC_PC_ASSIGN

DESCRIPTION
  This procedure does the required processing for a paging channel assignment
  (mode '001' or '101') from Idle or System Access state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_pc_assign
(
  word new_state,
    /* Next state to be processed */
  mccdma_chnasn_type *pc_assign_msg_ptr
    /* Message addressed to this mobile which specifies a Paging Channel
       assignment (ASSIGN_MODE equal to '001' or '101') */
);


/*===========================================================================

FUNCTION MCC_AUTH_CMD

DESCRIPTION
  This function sends a command to the Authentication Task.  If the
  task responds with a status other than AUTH_RECEIVED_S,  this
  function will call ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_auth_cmd
(
  auth_cmd_type *cmd_ptr
    /* pointer to command to send to Transmit task */
);

/*===========================================================================

FUNCTION MCC_RESYNC_AUTH

DESCRIPTION
  This function sends a command to re-synchronize the Authentication Task
  command queue by queueing a resync command and waiting for it to be
  processed.  If auth has other commands on its queue or is currently
  processing some other command, we will wait for it to finish everything
  and eventually get to our resync command and send the resync report.
  Any other reports it may send before that are discarded and returned to the
  free queue, so they will not be received later when they are not expected.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcc_resync_auth( void );

/*===========================================================================

FUNCTION MCC_QUEUE_AUTH_RPT

DESCRIPTION
  This function enqueues a specified report from the Authentication Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mcc_queue_auth_rpt
(
  auth_rpt_type *rpt_ptr
    /* Pointer to authentication report */
);


#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION MCC_UIM_CMD

DESCRIPTION
  This function sends a command to the UIM Task.  It then waits for
  the initial command response from the UIM server.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Waits for command to be received and UIM task to START processing
  the command.

===========================================================================*/

extern void mcc_uim_cmd
(
  uim_cmd_type *cmd_ptr
    /* pointer to command to send to UIM task */
);

/*===========================================================================

FUNCTION MCC_QUEUE_UIM_RPT

DESCRIPTION
  This function enqueues a specified report from the UIM server Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mcc_queue_uim_rpt
(
  uim_rpt_type *rpt_ptr
    /* Pointer to UIM server report */
);
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_CSIM

/*===========================================================================

FUNCTION MCC_QUEUE_MMGSDI_RPT

DESCRIPTION
  This function enqueues a specified report from the MMGSDI
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_queue_mmgsdi_rpt
(
  mmgsdi_cnf_enum_type  cnf,
  mmgsdi_cnf_type       *mcmmgsdi_cnf_ptr
);
#endif


/*===========================================================================

FUNCTION MCC_QUEUE_DH_RPT

DESCRIPTION
  This function enqueues a specified report from the DH Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_queue_dh_rpt
(
  dh_rpt_type  *dh_rpt_buf_ptr
    /* Pointer to dh report buffer filled in by dh task */
);


/*===========================================================================

FUNCTION TC_SO_CHANGE

DESCRIPTION
  This function changes service options.  It does not do any checking to
  see if the new service option is different from the current service option.
  All error checking regarding the new service option should be done prior to
  calling this procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void  tc_so_change
(
  caii_srv_cfg_type  *cfg_ptr
  // config to get SO's from
);

/*===========================================================================

FUNCTION TC_INFORM_SCR_CHANGE

DESCRIPTION
  This function informs lower layers about the SCR change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes get queued up to take effect at the action time

===========================================================================*/

extern void tc_inform_scr_change( word new_so, byte action_time, boolean use_pending_cfg);

/*===========================================================================

FUNCTION MCC_INFO_BAND_CLASS

DESCRIPTION
  This function builds the band class information record into a specified
  buffer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_info_band_class(caii_band_rec_type * rec_ptr);

/*===========================================================================

FUNCTION MCC_INFO_REC

DESCRIPTION
  This function builds a specified information record into a specified
  buffer.

DEPENDENCIES

RETURN VALUE
   This function returns an integer value as follows:
    -1 - if the type of the requested information record is invalid or
         the specified qualification information is invalid.
     0 - if the requested record will not fit in the specified buffer.
     n - where n is a positive integer indicating the size, in bytes,
         of the requested information record.

SIDE EFFECTS
  None

===========================================================================*/

extern int mcc_info_rec
(
  boolean pc,
    /* TRUE  if status request received on paging chan;
       FALSE if on traffic chan */
  byte band,
    /* Band class qualifier */
  byte op_mode,
    /* Operating mode qualifier */
  byte req_typ,
    /* Type of information record requested */
  void *buf_ptr,
    /* Pointer to place to build information record. */
  word buf_siz,
    /* Size of information record buffer, in bytes */
  byte *reject_reason
    /* Reason for rejecting the request */
);

/*===========================================================================

FUNCTION MCC_PROCESS_INFO_RECORDS

DESCRIPTION
  This function processes the Feature Notification, Alert With Information
  and Flash With Information messages.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an Alert With Information Message contains a signal record with
  SIGNAL_TYPE field set to 1 or 2.  FALSE otherwise. (This information is
  needed during the Conversation State).

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mcc_process_info_records
(
  byte call_id,
    /* Call Id for CM */
  byte msg_type,
    /* Message type */
  byte num_recs,
    /* Number of information records in message */
  byte *rec_ptr
    /* Pointer to information records */
);

/*===========================================================================

FUNCTION MCC_GET_STATE

DESCRIPTION
  Returns the current state of the phone.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern byte mcc_get_state( void );


/*===========================================================================

FUNCTION MCC_DH_CMD

DESCRIPTION
  This function sends a command to the DH Task.  If the
  task responds with a status other than AUTH_RECEIVED_S,  this
  function will call ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_dh_cmd
(
  dh_cmd_type *cmd_ptr
    /* pointer to command to send to Transmit task */
);


/*====================================================================

FUNCTION MCC_DEFAULT_IMSI_S1

DESCRIPTION
  This routine will compute default IMSI_S1 value as per IS-95B section 6.3.
  The algorithm is copied from uinv_default_imsi_s1.

DEPENDENCIES
  This function requires that the esn has been loaded into cdma.esn structure.

RETURN VALUE
  This function returns the encoded value of IMSI_S1 with the four least
  significant digits set to ESNp, converted directly from binary to decimal,
  modulo 10000. The other digits are set to zero.

SIDE EFFECTS

====================================================================*/
dword mcc_default_imsi_s1(void);

/* <EJECT> */

/*===========================================================================

FUNCTION MCC_GET_AUTH_IMSI_S1

DESCRIPTION
  This function returns correct imsi_s1 for authentication purposes.

DEPENDENCIES
  None

RETURN VALUE
  IMSI_M_S1 if IMSI_M is programmed. Otherwise, return IMSI_T_S1 (See
  IS-95B section 6.3.12.1 ).

SIDE EFFECTS
  None

===========================================================================*/
dword mcc_get_auth_imsi_s1(void);

/*===========================================================================

FUNCTION MCC_GET_AUTH_IMSI_S2

DESCRIPTION
  This function returns correct imsi_s2 for authentication purposes.

DEPENDENCIES
  None

RETURN VALUE
  IMSI_M_S2 if IMSI_M is programmed. Otherwise, return IMSI_T_S2 (See
  IS-95B section 6.3.12.1 ).

SIDE EFFECTS
  None

===========================================================================*/
word mcc_get_auth_imsi_s2(void);

/*===========================================================================

FUNCTION MCC_CM_MC_RPT

DESCRIPTION
  This function sends a report to the CM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_cm_mc_rpt
(
  cm_mc_rpt_type *cmd_ptr
    /* pointer to command to send to cm */
);

/*===========================================================================

FUNCTION mcc_check_alert_signal_info

DESCRIPTION
  This function checks whether the info record contains a signal info record
  or not and it also checks the whether the signal type is set to 1 or 2
  if the info record contains a signal info record

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an Alert With Information Message contains a signal record with
  SIGNAL_TYPE field set to 1 or 2. or it does not contain a signal information record
  FALSE otherwise. (This information is needed during the Conversation State).

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_check_alert_signal_info
(
  byte msg_type,
    /* Message type */
  byte num_recs,
    /* Number of information records in message */
  byte *rec_ptr
    /* Pointer to information records */
);


/*===========================================================================

FUNCTION MCC_SEND_MO_DBM_STATUS

DESCRIPTION
  This function sends a status report to the CM task for a mobile originated
  data burst message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_mo_dbm_status
(
  byte burst_type,
    /* Data Burst Message type, currently, it supports only SMS and PD */
  cm_dbm_mo_status_e_type err_code
    /* Mobile originated data burst message error code */
);

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
/*===========================================================================

FUNCTION MCC_SEND_DEREGISTER_STATUS

DESCRIPTION
  This function sends deregister_f cmd processing status report to the CM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_deregister_status
(
  cm_dereg_rsp_cause_e_type status_code
    /* Deregister cmd status code */
);

/*===========================================================================

FUNCTION MCC_PROC_DEREGISTER_TIMER_EXPIRY

DESCRIPTION
  This function is to handle deregister powerdown / powerup registration timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_proc_deregister_timer_expiry(void);

#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION MCC_PROC_CSFB_RETAIN_MT_PAGE_CXT_TIMER_EXPIRY

DESCRIPTION
  This function is to handle CSFB Retain MT page context timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_proc_csfb_retain_mt_page_cxt_timer_expiry(void);
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================

FUNCTION MCC_CHECK_MO_DBM_STATUS

DESCRIPTION
  This function checks if the MC is waiting for a layer2 ack for a specific
  data burst message (SMS or PD), and if so, sends out a status report to CM
  task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_check_mo_dbm_status
(
  cm_dbm_mo_status_e_type err_code
    /* Mobile originated data burst message error code */
);

/*===========================================================================

FUNCTION MCC_SEND_MT_DBM_DATA

DESCRIPTION
  This function sends the data to the CM task for a mobile terminated
  data burst message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_mt_dbm_data
(
  uint8 dbm_burst_type,
    /* The data burst type */
  uint8* data,
    /* Pointer to the paylaod */
  uint8 data_len
    /* The number of bytes in the payload */
);



/*===========================================================================

FUNCTION MCC_SEND_ORIG_FAILURE

DESCRIPTION
  This function sends a report to the CM task for an origination failure .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_orig_failure(cm_call_orig_fail_e_type err_code, byte call_id,
         cm_call_orig_fail_layer_e_type layer);

/*===========================================================================

FUNCTION MCC_SEND_MT_ORIG_FAILURE

DESCRIPTION
  This function sends a report to the CM task for a mobile terminated call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  cdma.call_end_reason will be reset to MC_CALL_ORIG_ERR_MAX.

===========================================================================*/

void mcc_send_mt_orig_failure
(
  cm_call_orig_fail_e_type err_code
    /* Error code for the MT call orig failure */
);

/*===========================================================================

FUNCTION mcc_set_layer2_ack

DESCRIPTION
  Set Layer2 ack info.

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS

===========================================================================*/

void mcc_set_layer2_ack
(
  byte ack_seq,
  byte ack_type,
  byte valid_ack
);


/*===========================================================================

FUNCTION MCC_BC_MATCH

DESCRIPTION
  This function parse the *pc_hdr to see if it is a BC message header that
  contains the expected BC_ADDR.

DEPENDENCIES
  It depends on the calling function to made sure that *pc_hdr contains enough
  useful bits (at least CAII_BC_HDR_LEN bits). And the current slot is the
  scheduled BC SMS slot.

RETURN VALUE
  TRUE if it contains the expected BC_ADDR, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

uint16 mcc_bc_match(byte *bcast_hdr);


/* <EJECT> */
/*===========================================================================

FUNCTION MCC_SET_RSCH_DTX_SIG

DESCRIPTION
  This function sets the MCC_TXC_REV_DTX_SIG. It is called by TXC when it
  detects REV DTX.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_set_rsch_dtx_sig (void);

/*===========================================================================

FUNCTION CDMA_EXIT_INIT

DESCRIPTION
  This procedure initializes the Exit state for CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cdma_exit_init( void );

/*===========================================================================

FUNCTION CDMA_CONTINUE

DESCRIPTION
  This function is called to reset the CDMA acquisition parameters when
  the call to function mcsys_cdma_continue() indicates that we are to
  continue operating in CDMA mode without passing control back to the
  system determination level first (i.e., we are currently operating on
  the preferred system).

DEPENDENCIES
  None

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  RXTX and Searcher tasks will go to Start mode.  Receive and Transmit
  tasks will reset to CDMA initialization mode.

===========================================================================*/

word cdma_continue
(
  boolean voc_pwr_off,
    /* Indicator of whether vocoder power needs to be turned off */
  mcc_entry_type *mcc_entry_reason_ptr
    /* Contains entry parameters */
);

/*===========================================================================

FUNCTION MCC_UPDATE_BS_P_REV

DESCRIPTION
   Updates internal data and report events when BS _PREV is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_update_bs_p_rev
(
  uint8 bs_p_rev
);

/*===========================================================================

FUNCTION update_retry_delay

DESCRIPTION
  This function informs DS the latest retry delay duration by calling DS's
  registered retry delay processing function.  It uses the current system
  time and given retry delay to compute the actual retry timer duration in
  unit of milliseconds.

  SCRM retry timer duration = (time left before the next 80ms boundary +
                               retry delay * 320) ms

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void update_retry_delay
(
  cai_retry_order_type  retry_type, /* Origination, Resource Request or SCRM */
  uint8 retry_delay
);

/*===========================================================================

FUNCTION mcc_send_orig_fail_retry_delay

DESCRIPTION
  This function informs DS the latest retry delay duration for an origination
  by sending comand CM_CALL_FAIL_RETRY_ORDER_F to CM. It uses the current
  system time and given retry delay to compute the actual retry timer
  duration in unit of milliseconds.

  ORIG retry timer duration = (time left before the next 80ms boundary +
  (time unit is 1 min)         least significant 7 bits of retry delay * 60000) ms

  ORIG retry timer duration = (time left before the next 80ms boundary +
  (time unit is 1 sec)         retry delay * 1000) ms

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_send_orig_fail_retry_delay
(
  uint8  retry_type,
  uint8  retry_delay,
  uint16 so_req,
  uint8 call_id
);

/*===========================================================================

FUNCTION MC_GET_ENTRY

DESCRIPTION
  The function copies the last CDMA and Analog call processing entry reason
  to specified input structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.


SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_get_entry(mcc_status_type *);

/*===========================================================================

FUNCTION MCC_SYS_MEAS_CMD

DESCRIPTION
  The function processes system measurement request from system determination.
  It prepares the system measurement command and sends it to Search. If
  Search rejects the system measurement request, report to system
  determination of all pilots with minimum RXPWR / ECIO and determines
  whether continues on the current system based on the suggested SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if stay on current system. Otherwise, FALSE.


SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_sys_meas_cmd(void);

/*===========================================================================

FUNCTION MCC_SYS_MEAS_RPT

DESCRIPTION
  The function reports the system measurements based on search report to
  system determination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_sys_meas_rpt
(
  mccsrch_rpt_type *rpt_ptr /* Pointer to SRCH report */
);

/*===========================================================================

FUNCTION MCC_CONTINUE_ON_CURR_SYS

DESCRIPTION
  Based on passed in SS-Action, this function checks whether the MS can
  continue on its current system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SS-Action allows MS to continue on the current system.
  FALSE if SS-Action specifies change of mode or request dedicated
    system measurement.

SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_continue_on_curr_sys
(
  mcc_sys_meas_mode_type meas_mode, /* Type of measurment mode allowed */
  sd_ss_act_e_type       ss_action  /* SS-Action                       */
);

/*===========================================================================

FUNCTION MCC_INFORM_SD_CHAN_ASSIGN

DESCRIPTION
  This function informs system determination of the paging / traffic channel
  assignement.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the paging / traffic channel assignment is accepted by system
    determination.
  FALSE if the paging / traffic channel assignment is rejected by system
    determination.

SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_inform_sd_chan_assign
(
  mccdma_chnasn_type *chan_assign_msg_ptr,
    /* Message addressed to this mobile which specifies a Paging / Traffic
       Channel assignment */
  mcc_sys_meas_mode_type sys_meas_mode
    /* Mode of system measurement allowed */
);

/*===========================================================================

FUNCTION MCC_INFORM_SD_REDIRECTION

DESCRIPTION
  This function informs system determination of the redirection.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the redirection is accepted by system determination.
  FALSE if the redirection is rejected by system determination.

SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_inform_sd_redirection
(
  sd_ss_redir_e_type  redir_type, /* Type of redirection */
  mcc_redir_info_type *redir_info /* Redirection information */
);

#ifdef FEATURE_1X_SPLMN_SUPPORT
/*===========================================================================

FUNCTION MCC_SEND_SD_SPLMN_LIST

DESCRIPTION
  This function sends the splmn list and numEutraFrequencies received in 
  ATIM message to SD.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_send_sd_splmn_list
(
  void
);
#endif /* FEATURE_1X_SPLMN_SUPPORT */

/*===========================================================================

FUNCTION MCC_SET_CURR_NAM

DESCRIPTION
  This function saves the new NAM when the NAM is changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_set_curr_nam
(
  mc_nam_changed_type nam_changed /* Pointer to new NAM info */
);

/*===========================================================================

FUNCTION  MCC_IS_OVHD_CFG_OKAY

DESCRIPTION
  This function checks if all ovhd messages' config message sequence are
  current.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs' config message sequence are current , FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_ovhd_cfg_okay( void );

/*===========================================================================

FUNCTION  MCC_IS_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if all ovhd messages are current

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_ovhd_msg_okay( void );

#ifdef FEATURE_1X_OVHD_OPTIMIZATION
/*===========================================================================

FUNCTION  MCC_IS_CACHED_OVHD_MSG_OKAY

DESCRIPTION
    This function checks if all ovhd messages are current.
    This API takes into account that rx.config_msg_seq might be marked as INVALID
    for validating cached or expired overheads and this API still returns TRUE if
    cached overheads are current except for rx.config_msg_seq.
  
  DEPENDENCIES
    This API CANNOT be used to determine if MC can go to Sleep state or if 
    overheads are current to perform any access attempt.


RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_cached_ovhd_msg_okay( void );
#endif /* FEATURE_1X_OVHD_OPTIMIZATION */

/*===========================================================================

FUNCTION  MCC_IS_UPD_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if all ovhd messages necessary for Update Overhead info
  substate are current

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_upd_ovhd_msg_okay( void );

/*===========================================================================

FUNCTION  MCC_IS_AHO_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if ovhd messages  necessary for Access HO are current

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_aho_ovhd_msg_okay( void );

/*===========================================================================

FUNCTION  MCC_IS_AEHO_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if ovhd messages  necessary for Access Entry HO
  are current

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_aeho_ovhd_msg_okay( void );

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION  MCC_IS_PCH_BCCH_INFO_AVAILABLE

DESCRIPTION
  This function checks if info abt ovhd channel (PCH/BCCH) is available
  for this BS

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_pch_bcch_info_available
(
  bs_info_id_type *basic_bs_info,
  int *bs_index
);
#endif


/*===========================================================================

FUNCTION  MCC_ARE_RAND_PARMS_OKAY

DESCRIPTION
  This function checks if rand parameters are current.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we are on the Paging Channel, since Rand Parameters always come
  in the Access Parameters Message on PCH.
  FALSE if the base station is sending ANSI-41 RAND Message and
  it is not current; TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_are_rand_parms_okay(void);

/*===========================================================================

FUNCTION  MCC_CLEAR_OVHD_MSGS

DESCRIPTION
  This function clears the "rx" flag of all messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_clear_ovhd_msgs( void );

/*===========================================================================

FUNCTION  MCC_CLEAR_OVHD_MSGS_PREV_MISMATCH

DESCRIPTION
  This function marks all messages that depends on p_rev for processing.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_clear_ovhd_msgs_prev_mismatch( void );

/*===========================================================================

FUNCTION  MCC_CLEAR_RXTX_Q

DESCRIPTION
  This function will clear the RxTx queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_clear_rxtx_q( void );

/*===========================================================================

FUNCTION  mcc_expired_timer_callback_func

DESCRIPTION
  This is the function that will be executed (in MC context) upon
  the expiration of a MC timer that uses the Asynchronous Procedure
  Call (APC) feature of REX+. The function queues the timer ID (argument)
  of the expired timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_expired_timer_callback_func
(
  unsigned long
);

/*===========================================================================

FUNCTION  mcc_timer_on_callback_q

DESCRIPTION
  This function walks through the queue of expired timers to see if the
  argument (timer ID) is present in it.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if timer ID on queue.
  FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_timer_on_callback_q( unsigned long );

/*===========================================================================

FUNCTION  mcc_get_type_specific_timer_on_callback_q

DESCRIPTION
  This function walks through the queue of expired timers to see if the
  argument (timer type) is present in it. If present, it retrieves the timer ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if timer ID is being returned in argument.
  FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_get_type_specific_timer_on_callback_q
(
  unsigned long timer_type, unsigned long * ret_timer_id
);

/*===========================================================================

FUNCTION  mcc_purge_callback_timer_q

DESCRIPTION
  This function purges the queue of expired timers and also clears the
  MCC_CALLBACK_TIMER_SIG

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_purge_timer_callback_q(void);

/*===========================================================================

FUNCTION  mcc_set_callback_timer

DESCRIPTION
  This function should be used to set timers that utilise the REX
  callback mechanism within MCC. It clears the timer, its entry in
  the callback queue, and sets the timer.

DEPENDENCIES
  None.

RETURN VALUE
  last countdown of timer

SIDE EFFECTS
  remove the entry, (if any), corresponding to callback_timer_id from
  callback queue.

===========================================================================*/

extern rex_timer_cnt_type mcc_set_callback_timer
(
    rex_timer_type       *p_timer,   /* pointer to timer to set */
    rex_timer_cnt_type    msecs,      /* value in milliseconds   */
    unsigned long         callback_timer_id
);

/*===========================================================================

FUNCTION XLATE_DIALED_NUM

DESCRIPTION
  This function fills a cdma_dial_type buffer with the phone number
  dialed by the user translated into the digit code required by the
  base station. It will also fill in the number of digits dialed into
  the cdma_dial_type buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void xlate_dialed_num
(
  mc_origination_type * orig_parms,
    /* pointer to recieved message */
  cdma_dial_type *buf_ptr
    /* pointer to buffer to place translated phone number into */
);

/*===========================================================================

FUNCTION  MCC_UPDATE_ACC_MSG_SEQ

DESCRIPTION
  This function updates the stored access message sequence.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_update_acc_msg_seq
(
  byte acc_msg_seq /* Access message sequence */
);

/*===========================================================================

FUNCTION MCC_INCREMENT_PAG3_OR_FCCCH_3

DESCRIPTION
  This function increments count PAG_3 or FCCCH_3.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mcc_increment_pag_3_or_fccch_3(void);

/*===========================================================================

FUNCTION MCCIDL_GET_SLOT_CYCLE_INDEX_S

DESCRIPTION
  This procedure gets the slot cycle index.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern int8 mccidl_get_slot_cycle_index_s(void);

/*===========================================================================

FUNCTION MCCIDL_SET_SLOT_CYCLE_INDEX_S

DESCRIPTION
  This procedure sets the slot cycle index.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mccidl_set_slot_cycle_index_s(int8 sci_s);


#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION MCCIDL_INIT_IDLE_PGSLOT

DESCRIPTION
  This procedure inits the current idle page slot on power up to assume
  it is using imsi_m.  That way, if HDR acquires before 1x it will negotiate
  PreferredControlChannelCycle (PCCC) based on this value rather than zero.
  If 1x is using imsi_t, then HDR will still need to change its PCCC later,
  but if 1x ends up using imsi_m, no change will be needed.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mccidl_init_idle_pgslot(void);

#endif /* FEATURE_HDR */


#ifdef FEATURE_DDTM_CNTL
/*===========================================================================

FUNCTION mcc_set_ddtm_status

DESCRIPTION

  This function sets the variable cdma.ddtm. This variable affects
  the following:
  1] 1x registration maybe prevented.
  2] All mobile directed messages maybe be ignored and if so no
     L2 ack for these messages are sent.
  3] All Page message may be ignored but broadcast SMS will
     be processed
  4] All overhead messages including GSRM will be processed.

  It is set when in data dedicated transmission mode, no interruption to
  the current call is needed. It can also be set for allowing only specific
  pages during Emergency callback mode.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_ddtm_status
(
  mc_ddtm_cntl_type *ddtm
);
#endif /* FEATURE_DDTM_CNTL */

void mccidl_set_sci_mask(word new_sci_mask);

/*===========================================================================

FUNCTION mcc_set_current_preferred_sci

DESCRIPTION
  This sets the current preferred SCI value. The preferred SCI in NV is
  not updated

DEPENDENCIES
  None.

RETURN VALUE
  The preferred sci value MS is actually set to

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_set_current_preferred_sci(int8 sci_p);

/*===========================================================================

FUNCTION mcc_get_current_preferred_sci

DESCRIPTION
  Gets the current preferred SCI value. This may be different from the one
  that is stored in NV.

DEPENDENCIES
  None.

RETURN VALUE
  The current preferred sci value.

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_get_current_preferred_sci(void);

/*===========================================================================

FUNCTION mcc_set_last_sci_reg

DESCRIPTION
  This sets the last registered SCI value. The last registered SCI in NV is
  not updated

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_last_sci_reg(int8 sci);

/*===========================================================================

FUNCTION mcc_get_last_sci_reg

DESCRIPTION
  Gets the last registered SCI value. This may be different from the one
  that is stored in NV.

DEPENDENCIES
  None.

RETURN VALUE
  The last registered sci value.

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_get_last_sci_reg(void);

/*===========================================================================

FUNCTION MCC_CONVERT_PREF_SCI_TO_OTA_SCI

DESCRIPTION

  This function converts the preferred SCI from internal format to OTA format
  to be packed by parser.

  If the auto_msg_supported bit in ESP is set, it uses the following
  conversion mapping for Negative SCI.

    Preferred SCI               OTA SCI
    -------------------------   -------
      0 - 1.28s (16 slots)       000
      1 - 2.56s (32 slots)       001
      2 - 5.12s (64 slots)       010
      3 - 10.24s (128 slots)     011
     -4 - 0.080s (1 slot)        100
     -3 - 0.160s (2 slots)       101
     -2 - 0.320s (4 slots)       110
     -1 - 0.640s (8 slots)       111


  Otherwise, it uses the following conversion mapping:

    Preferred SC  P_REV_IN_USE  Negative SCI supported  OTA SCI
    ------------- ------------  ----------------------  ---------------------
    > 0           N/A           N/A                     Same as preferred SCI
    < 0           < 11          N/A                     0
    < 0           >= 11         No                      0
    < 0           >= 11         Yes                     Same as preferred SCI

DEPENDENCIES
  None.

RETURN VALUE
  Preferred SCI value to be transmitted over the air

SIDE EFFECTS
  None.

===========================================================================*/
int8 mcc_convert_pref_sci_to_ota_sci(void);

/*===========================================================================

FUNCTION MCC_SET_OPERATIONAL_SCI

DESCRIPTION

  This function updates the operation SCI, slot_cycle_index_s
  from OTA MAX_SCI.

  If the auto_msg_supported bit in ESP is set, it uses negative SCIp,
  otherwise it used positive SCIp. The conversion mapping for Negative SCI
  is as follows

DEPENDENCIES
  None.

RETURN VALUE
  TRUE   - if overheads are current, slot_cycle_index_s is set appropriately.
  FALSE  - if overheads are not current, slot_cycle_index_s cannot be set.

SIDE EFFECTS
  slot_cycle_index_s is updated.

===========================================================================*/

boolean mcc_set_operational_sci(void);

/*===========================================================================

FUNCTION MCC_CALCULATE_SCI_MASK

DESCRIPTION
  Returns (power(2,i) - 1), where i = solt cycle index

DEPENDENCIES
  None.

RETURN VALUE
  sci_mask

SIDE EFFECTS
  None

===========================================================================*/

word mcc_calculate_sci_mask(int8 sci);

/*===========================================================================

FUNCTION mcc_set_ovhd_refresh_time

DESCRIPTION
  This function is used to modify the ovhd refresh time i.e. the time after  
  which stored overheads are considered to be expired.
  This is purely needed for testing puposes particularly to test split  
  overheads feature.

DEPENDENCIES
  None.

RETURN VALUE
  The overhead refresh time finally set

SIDE EFFECTS
  None.

===========================================================================*/

uint16 mcc_set_ovhd_refresh_time
(
  uint16 refresh_time
);

/*===========================================================================

FUNCTION MCCIDL_MSG_HANDLER

DESCRIPTION
  This function processes a message received from the Receive task.  Based
  on the message type it will call the appropriate procedure to process the
  message.

DEPENDENCIES
  Input message is in internal format.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  If the header within the input caii_rx_msg_type requests that the buffer
  be placed on a queue after processing is completed this procedure will
  do so.

===========================================================================*/

word mccidl_msg_handler
(
  mccrxtx_msg_type *msg_ptr,
    /* CDMA message in internal format */
  word new_state
    /* Next state to be processed */
);

/*===========================================================================

FUNCTION MCCIDL_PROC_IGNORED_OVHD

DESCRIPTION
  This function processed rescan or (extended) global service redirection
  that was ignored because overheads are not current at the time
  SPM / ANS41SPM / GSRM / EGSRM was received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

word mccidl_proc_ignored_ovhd
(
  word curr_state /* Current state */
);

/*===========================================================================

FUNCTION MCCIDL_GET_NGHBR_INDEX

DESCRIPTION
  This procedure gets the index of a specified pilot in the neighbor list.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the index value of this pilot in the neighbor list.
  "-1" indicates that this pilot is not in the neighbor list.

SIDE EFFECTS
  None.

===========================================================================*/

int2 mccidl_get_nghbr_index
(
  word pilot_pn,
  word new_freq,
  byte new_band
);

/*===========================================================================

FUNCTION MCCIDL_HANDOFF

DESCRIPTION
  This procedure performs handoff processing when the Mobile is in
  Idle state and System Access state.  Idle Handoff, Access Entry HO,
  Access Probe Handoff and Access Handoff are processed with this
  function.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_handoff
(
  word new_pilot,
    /* New pilot received in searcher report */
  word new_freq,
    /* New cdma frequency received in searcher report. */
  byte new_band_class,
    /* New band class received in searcher report */
  word new_state,
    /* Next state to be processed */
  int8 ecio_old_pilot,
  int8 ecio_new_pilot,
  cdma_ho_type ho_type
    /* Information for event reporting */
);

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_IDLE_ASET

DESCRIPTION
  This procedure sends the Search task a SRCH_IDLE_ASET_F command to update
  the active set information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_idle_aset(void);

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_PARMS

DESCRIPTION
  This procedure sends the Search task a SRCH_PARM_F command to update
  searh parameters.

DEPENDENCIES
  mcc_ho_real must have been updated appropriately

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_parms(void);

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_IDLE_NSET

DESCRIPTION
  This procedure sends the Search task a SRCH_IDLE_NSET_F command to update
  the neighbor set information.

DEPENDENCIES
  Depends on new neighbor list message being in bs_info.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_idle_nset( void );

/*===========================================================================

FUNCTION MCCIDL_SAVE_MSG_RXED_CHAN

DESCRIPTION
  This function saves what channel message was received on.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mccidl_save_msg_rxed_chan(void);


/*===========================================================================

FUNCTION MCCIDL_INIT_CHANNEL_MONITORING

DESCRIPTION
  This procedure initialize channel monitoring state variables.

DEPENDENCIES
  None.

RETURN VALUE
  Number of channel configurations in the priority list.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_init_channel_monitoring(void);

/*===========================================================================

FUNCTION MCCIDL_SUPERVISE_CHANNEL_MONITORING

DESCRIPTION
  This procedure updates the requirements of channel monitoring. It also
  notifies the lower layers if the requirement changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_supervise_channel_monitoring
(
  word new_state,
    /* Next state to be processed */
  mccsrch_wakeup_rpt_type *wakeup_rpt_ptr,
    /* Search wakeup report */
  boolean enh_bc_done
    /* Is channel switching due to enh broadcast SMS done */

);

/*===========================================================================

FUNCTION MCCIDL_CLEAR_BS_INSANITY_TIMER

DESCRIPTION
  This procedure clears the BS insanity timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_clear_bs_insanity_timer(void);

/*===========================================================================

FUNCTION MCCIDL_IS_ACCESS_FADE_TIMER_IN_USE

DESCRIPTION
  This procedure returns the value of mccidl_fast_raho.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_is_access_fade_timer_in_use(void);

/*===========================================================================

FUNCTION MCC_ADD_GROUP_IN_BAND_TABLE

DESCRIPTION
  Adds the specified group in the table. It also optionally adds all the
  band classes associated with it. If the bad group already exists, it will
  not add a duplicate group. Only band classes, if any associated with
  this group are added.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_add_group_in_band_table
(
  cdma_band_class_group_type gp,
  uint8 num_bands,
  uint8 * band_class
);

/*===========================================================================

FUNCTION MCC_ADD_BAND_CLASS_IN_GROUP
  Adds the specified band class in the specified group. It calls
  mcc_add_group_in_band_table to perform its task.

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_add_band_class_in_group
(
  cdma_band_class_group_type gp,
  uint8 band_class
);

/*===========================================================================

FUNCTION MCC_REMOVE_BAND_CLASS_FROM_GROUP
  Removes the band class from the specified group

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_remove_band_class_from_group
(
  cdma_band_class_group_type gp,
  uint8 band_class
);

/*===========================================================================

FUNCTION MCC_REMOVE_BAND_CLASS
  Removes the first encountered band class entry in any group. It calls
  mcc_remove_band_class_from_group to perform its job.

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_remove_band_class
(
  uint8 band_class
);

/*===========================================================================

FUNCTION MCC_PRINT_BAND_CLASS_TABLE

DESCRIPTION
  Prints the band class table.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_print_band_class_table
(
  void
);

/*===========================================================================

FUNCTION MCC_BAND_CLASS_INIT

DESCRIPTION
  Initializes the band class table.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_band_class_init(void);

/*===========================================================================

FUNCTION MCC_GET_GROUP_FOR_BAND_CLASS

DESCRIPTION
  Given the band class, it returns the group it belongs to.

DEPENDENCIES
  None.

RETURN VALUE
  The group that the specified band class belongs to.

SIDE EFFECTS
  None.

===========================================================================*/

cdma_band_class_group_type mcc_get_group_for_band_class(uint8 band_class);

/*===========================================================================

FUNCTION MCC_IS_CURRENT_BAND_CLASS_IN_GROUP

DESCRIPTION
  Determines if the current Band Class (cdma.band_class) belongs to the
  specified group.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if yes, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_current_band_class_in_group(cdma_band_class_group_type gp);

/*===========================================================================

FUNCTION MCC_IS_BAND_CLASS_IN_GROUP

DESCRIPTION
  Determines if the specified Band Class belongs to the specified group.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if yes, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_band_class_in_group
(
  cdma_band_class_group_type gp,
  uint8 band_class
);

/*===========================================================================

FUNCTION MCC_IS_BAND_CLASS_SUPPORTED

DESCRIPTION
  Determines if the specified band class is present in any group.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the band class is supported, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_band_class_supported(uint8 band_class);

/*===========================================================================

FUNCTION MCC_SET_SCM_FOR_BAND_CLASS

DESCRIPTION
  Sets the SCM bits depending on which group the supplied band class belongs

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SCM bits were successfully set, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_set_scm_for_band_class(uint8 band_class, byte * scm);

#ifdef FEATURE_SO_8026
/*===========================================================================

FUNCTION mcc_8026_reset_call_flow_info

DESCRIPTION
  Initialize 8026 call flow parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void mcc_8026_reset_call_flow_info(void);

/*===========================================================================

FUNCTION mcc_8026_restore_l2_ack_info

DESCRIPTION
  Restore the L2 ack information from mccsa_8026_call_info to mcc_layer2
  data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void mcc_8026_restore_l2_ack_info(void);

/*===========================================================================

FUNCTION mcc_8026_store_l2_ack_info

DESCRIPTION
  Store 8026 L2 Ack information and start T2m(200 msec) Ack timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void mcc_8026_store_l2_ack_info(void);

/*===========================================================================

FUNCTION mcc_8026_l2_ack_timeout_handler

DESCRIPTION
  T2m (200 msec) timer expired. This function processes this timer.
  It sends MS ACK order for the previously received MT SDB within
  8026 call flow.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS

===========================================================================*/

word mcc_8026_l2_ack_timeout_handler(word curr_state);
#endif /* FEATURE_SO_8026 */

/*===========================================================================

FUNCTION MCCIDL_IS_FAST_RAHO

DESCRIPTION
  This procedure returns the value of mccidl_fast_raho.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccidl_is_fast_raho(void);

/*===========================================================================

FUNCTION MCCDMA_CLEAR_MC_QUEUES_SIGS_TIMERS

DESCRIPTION
  This fn will clear all of MC's timers, It clears all of MC's signals
  and purges RXTX, RXC & SRCH queues.

DEPENDENCIES
  None

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  Any active MC signal is cleared & any otems on the queue are lost

===========================================================================*/

void mccdma_clear_mc_queues_sigs_timers( void );

#if (defined (FEATURE_HDR_HANDOFF) && defined (FEATURE_IS2000_REL_A))
/*===========================================================================

FUNCTION MCC_UPDATE_HDR_PANID

DESCRIPTION
  This function updates the HDR PANID - Previous Access Network Identifier.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_update_hdr_panid
(
  mc_digital_mode_type last_act_data_net
);
#endif /* FEATURE_HDR_HANDOFF && FEATURE_IS2000_REL_A */

/*===========================================================================
FUNCTION MCC_WAIT_TRANSCEIVER_RESOURCE

DESCRIPTION
  This function sends a SRCH_ACCESS_F command to Search when MC requests an
  update of transceiver resource and wait until it obtains the resource or
  request timeout or the lost of resource.

  To be safe for the coding, a timer is applied while waiting for the searcher
  report SRCH_ACCESS_READY_R. For simplicity reason, the timer value is chosen
  to be the same as T33M which is the same as the one used in
  mcc_request_transceiver_resource.

  When the timer goes out, it does not do anything here. Mobile will come into
  Access Failure when it assumes that the transceiver resource is available.

DEPENDENCIES
  None

RETURN VALUE
  Status of transceiver request.

SIDE EFFECTS
  None

===========================================================================*/

mcc_transceiver_request_status_type mcc_wait_transceiver_resource
(
  srch_access_perf_enum access_perf,
  srch_access_type_enum access_type
);

/*===========================================================================
FUNCTION MCC_REQUEST_TRANSCEIVER_RESOURCE

DESCRIPTION
  This function sends a SRCH_ACCESS_F command to Search when MC requests
  transceiver resource before it comes into Access State from Idle state.
  Upon receiving this command, Search will try to acquire the transeiver
  resource with specified parameters, and send a SRCH_ACCESS_READY_R when
  the resource is ready.

  Timer T33 (300ms) is applied to the waiting for the reply from the Searcher.

  This function returns immediately after starting the timer and sending
  the request to the Searcher.

  Function mccidl_proc_srch_rpt in mccidl.c will process the Search report
  SRCH_ACCESS_READY_R. Function mccidl_online in mccidl.c will process the
  timer expiry of T33m.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function would force SRCH task to sleep and rude wakeup if the LTE 
  is holding the chain 0 and 1x is holding the chain 1.

===========================================================================*/

word mcc_request_transceiver_resource
(
  srch_access_perf_enum     access_perf,
  srch_access_type_enum     access_type,
  word                      sa_tc_state,
  word                      new_state
);

/*===========================================================================
FUNCTION MCC_CELLULAR_BAND_IS_SUPPORTED

DESCRIPTION
  This function checks whether cellular band class is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the MS supports some cellular band class

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_cellular_band_is_supported(void);

/*===========================================================================
FUNCTION MCC_PCS_BAND_IS_SUPPORTED

DESCRIPTION
  This function checks whether PCS band class is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the MS supports some PCS band class

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_pcs_band_is_supported(void);

/*===========================================================================

FUNCTION MCC_SEND_CM_SRV_SYS_INFO

DESCRIPTION
  This function sends the CM task CM_SRV_SYS_INFO_F

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_send_cm_srv_sys_info
(
  word rcv_fields,
  word sid,
  word nid,
  word reg_zone,
  byte pzid
);

/*===========================================================================

FUNCTION MCC_SETUP_REJECT_ORD

DESCRIPTION
  This function sets up the reject order to be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mcc_setup_reject_ord
(
  byte ordq,
    /* reason for rejecting the message */
  byte rej_msg_type,
    /* the type of message being rejected */
  byte rej_order,
    /* if the rej_msg_type is an order, this is the type of order */
  byte rej_ordq
    /* if the rej_msg_type is an order, this is the ordq of the rej_order */
);

#if defined(FEATURE_IS2000_REL_A_AES)
/*===========================================================================

FUNCTION MCC_CMEA_KEY_AND_VPM

DESCRIPTION
  This function generates CMEA key and sets up VPM

DEPENDENCIES
  None

RETURN VALUE
  Voice privacy mode indicator.

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_cmea_key_and_vpm
(
  boolean voice_privacy_desired, /*lint -esym(715, voice_privacy_desired)*/
  word    curr_state
);
#endif /* FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION MCC_SETUP_VPM_AND_AKEY

DESCRIPTION
  This function performs voice privacy setup and kicks off the process of
  calculating authentication keys.

DEPENDENCIES
  None

RETURN VALUE
  Voice privacy mode indicator.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_setup_vpm_and_akey
(
  boolean voice_privacy_desired,
    /* Indicates whether VPM cannot supported for the call type. */
  byte bs_auth_mode
    /* Auth mode of BS. */
);

/*===========================================================================

FUNCTION MCC_CHECK_IF_TRAF_CHAN_ASSGN_IS_OKAY

DESCRIPTION
  This function checks to see if a received traffic channel assignment is
  okay.

DEPENDENCIES
  None

RETURN VALUE
  The reason for rejecting the ECAM.

SIDE EFFECTS
  None

===========================================================================*/

uint8 mcc_check_if_traf_chan_assgn_is_okay
(
  mccdma_chnasn_type *chnasn_ptr /* Pointer to matched (E)CAM message */
);

/*===========================================================================

FUNCTION MCC_PROC_TRAF_CHAN_ASSGN

DESCRIPTION
  This function performs traffic channel assignment processing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_proc_traf_chan_assgn
(
  mccdma_chnasn_type *chnasn_ptr
    /* Pointer to (Extended) Channel Assignment Message */
);

/*===========================================================================

FUNCTION MCC_CDMA_CHANS_ARE_SUPPORTED

DESCRIPTION
  This function indentifies a list of channel frequencies that the mobile
  supported in input_redir_rec and copies these valid channels along
  with other information in input_redir_rec into output_redir_rec.
  It returns TRUE if at least one freq belongs to the specified band in
  input_redir_rec.

DEPENDENCIES
  None

RETURN VALUE
  Boolean value indicating whether at least one freq belongs to the
  specified band class in input_redir_rec.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_cdma_chans_are_supported
(
  const caii_redirect_rec2_type  *input_redir_rec,
  sd_redir_rec_cdma_s_type *output_redir_rec
);

/*===========================================================================

FUNCTION MCC_SEND_TXC_EARLY_JUMP_CMD

DESCRIPTION
  This function sends request to Transmit task to Initialize encoder timing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_send_txc_early_jump_cmd (void);

/* ==========================================================================

FUNCTION MCCSA_ACCESS_ATTEMPT_IN_PROGRESS

DESCRIPTION
  This function checks whether access attempt is in progress.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if access in progress. Otherwise, FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccsa_access_attempt_in_progress(void);

/*===========================================================================

FUNCTION MCCSA_SET_PREV_PILOT

DESCRIPTION
  This function sets Previous pilot PN and its neighbor record in Access
  Attempt list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsa_set_prev_pilot_info ( cdma_bs_type *old_bs_ptr );

/*===========================================================================

FUNCTION MCCSA_UPDATE_ACCESS_ATTEMPT_LIST

DESCRIPTION
  This function updates Access Attempt list with info from neighbor list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsa_update_access_attempt_list(void);

/* ==========================================================================

FUNCTION MCCSA_UPDATE_SRCH_NEIGHS

DESCRIPTION
  This function updates the neighbor set which will be sent to Search task
  with info from Access Attempt list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsa_update_srch_neighs(void);

/*===========================================================================

FUNCTION MCCSA_IS_IN_ACCESS_HO_LIST

DESCRIPTION
  This function validates if the Mobile is allowed for Access Handoff and
  if it is in the ACCESS_HO_LIST

DEPENDENCIES
  Depends on the P_REV_IN_USE (atleast 4) and Access Handoff Feature allowed.

RETURN VALUE
  boolean (TRUE/FALSE)

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccsa_is_in_access_ho_list(word pilot_pn);

/*===========================================================================

FUNCTION MCCSA_CALC_PSIST

DESCRIPTION
  This function calculates values to be used for the persistence test
  specified in IS-95 6.6.3.1.1.1 and 6.6.3.1.1.2.  The result is used
  in the TX task to compare against a pseudo-random number to determine
  if the mobile station will transmit during a given access slot.

DEPENDENCIES
  None

RETURN VALUE
  Returns a boolean value indicating if the access attempt should be
  aborted.

SIDE EFFECTS
  None

===========================================================================*/

access_result_type mccsa_calc_psist
(
  cai_tx_channel_id_type chan,
    /* one of {CAI_ACH, CAI_EACH} to indicate that the parm_ptr is a ap or eap */
  void *parm_ptr,
    /* Pointer to current Access Parameters Message */
  word curr_state
    /* Current System Access substate */
);

/*===========================================================================
FUNCTION MCCSA_START_ACCESS_GUARD_TIMER

DESCRIPTION
  This function will dynamically calculate the value of timer as per the
  number of sequence and number of probes received in Access Parameter Mesage.
  This function will start the access guard timer and will wait for
  L2 Ack until expiry.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void mccsa_start_access_guard_timer(void);

/*===========================================================================

FUNCTION MCCSA_TX_STOP

DESCRIPTION
  This function commands tx to stop transmitting accesses but keep the
  transmit clock running while an System Access Attempt is in progress.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsa_tx_stop( void );

#ifdef FEATURE_SO_8026
/*===========================================================================

FUNCTION mccsa_8026_send_l2_ack_order

DESCRIPTION
  Sends L2 ack order for 8026 call flow.

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/

word mccsa_8026_send_l2_ack_order( word curr_state);
#endif /* FEATURE_SO_8026 */

#ifdef FEATURE_HIGH_PSIST
/*==========================================================================

FUNCTION MCC_SEND_CM_PSIST_INFO

DESCRIPTION
  This function sends the CM task CM_PSIST_F

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_send_cm_psist_info
(
 cm_psist_e_type psist
);
#endif /* FEATURE_HIGH_PSIST */

/*===========================================================================

FUNCTION MCC_CHECK_IF_CSCH_SRDM_IS_OKAY

DESCRIPTION
  This function validates the service redirection message to see if it can be
  supported by the MS.

DEPENDENCIES
  None

RETURN VALUE
  TRUE indicates MS supports the service redirection. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_check_if_csch_srdm_is_okay
(
  caii_serv_redirect_type *serv_redir_ptr
);


/*===========================================================================

FUNCTION MCC_USE_PSEUDO_BS_INFO

DESCRIPTION
  This function saves the value of cur_bs_ptr to
  mcc_pseudo_bs_info.old_cur_bs_ptr and changes the value of cur_bs_ptr to
  point to mcc_pseudo_bs_info.pseudo_bs.

  It also initializes mcc_pseudo_bs_info.pseudo_bs to make it as if MS has
  SPM, ESPM and APM for the target 1x system for the handover.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_use_pseudo_bs_info
(
  byte band_class,
  word cdma_ch
);

/*===========================================================================

FUNCTION MCC_USE_1X_BS_INFO

DESCRIPTION
  This function set the value of cur_bs_ptr back to
  mcc_pseudo_bs_info.old_cur_bs_ptr.

DEPENDENCIES
  mcc_go_to_pseudo_bs must be executed prior.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_use_1x_bs_info
(
  void
);

#ifdef FEATURE_IS2000_REL_A_AES
/*===========================================================================

FUNCTION MCC_MS_SUPPORTS_SIG_AES

DESCRIPTION
  This function checks if MS supports signaling AES

DEPENDENCIES
  None

RETURN VALUE
  TRUE if yes, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_ms_supports_sig_aes(void);

/*===========================================================================

FUNCTION MCC_MS_SUPPORTS_UI_AES

DESCRIPTION
  This function checks if MS supports user information AES

DEPENDENCIES
  None

RETURN VALUE
  TRUE if yes, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_ms_supports_ui_aes(void);

/*===========================================================================

FUNCTION mcc_bs_sig_aes_sup

DESCRIPTION
  This function returns the current Signaling AES support of current BS

DEPENDENCIES
  None

RETURN VALUE
   It returns TRUE if current BS supports signaling AES. Otherwise returns FALSE


SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_bs_sig_aes_sup
(
  void
);


/*===========================================================================

FUNCTION mcc_bs_enc_sup

DESCRIPTION
  This function returns the encryption support of current BS

DEPENDENCIES
  None

RETURN VALUE
   It returns TRUE if current BS supports encryption. Otherwise returns FALSE


SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_bs_enc_sup
(
  void
);


/*===========================================================================

FUNCTION mcc_bs_ui_aes_sup

DESCRIPTION
  This function returns the UI AES support of current BS

DEPENDENCIES
  None

RETURN VALUE
   It returns TRUE if current BS supports UI AES. Otherwise returns FALSE


SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_bs_ui_aes_sup(void);

/*===========================================================================

FUNCTION mcc_disable_aes

DESCRIPTION
  This function sets all the all the encrypt modes to be DISABLED for AES
  status fields

DEPENDENCIES
  None

RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/

void mcc_disable_aes(void);


/*===========================================================================

FUNCTION MCC_CALCULATE_CRC_8

DESCRIPTION
  This function calculates 8-bit CRC. The poly is:
     G(x)=x^8 + x^7 + x^4 + x^3 + x + 1

DEPENDENCIES
  None

RETURN VALUE
  an unsigned char of 8 bit CRC

SIDE EFFECTS
  None

===========================================================================*/
uint8 mcc_calculate_crc_8(
                           /* the buffer pointer */
                           const uint8 *buf_ptr,

                           uint16 start_pos,
                           /* the starting position of the buffer to caluclate the CRC */

                           uint16 bits /* the number of bits over which to calculate CRC8 */
                          );
#endif /* FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION MCCDMA_ENCRYPT_VOICE_FRAME

DESCRIPTION
  This function encrypts a voice frame when voice encryption is turned on
  over voice services


DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_encrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              );


/*===========================================================================

FUNCTION MCCDMA_DECRYPT_VOICE_FRAME

DESCRIPTION
  This function decrypts a voice frame when voice decryption is turned on over
  voice services


DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_decrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              );

#ifdef FEATURE_1X_CUST_VOICE_SO
/*===========================================================================

FUNCTION MCCDMA_CUST_ENCRYPT_VOICE_FRAME

DESCRIPTION
  This function encrypts a voice frame with voice encryption algorithm
  specified by the customer if the voice call is established with 
  customer specified voice encryption service option.


DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_cust_encrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              );


/*===========================================================================

FUNCTION MCCDMA_CUST_DECRYPT_VOICE_FRAME

DESCRIPTION
  This function decrypts a voice frame with voice decryption algorithm
  specified by the customer if the voice call is established with 
  customer specified voice decryption service option.

DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_cust_decrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              );

#endif /* FEATURE_1X_CUST_VOICE_SO */

#if defined(FEATURE_IS2000_REL_A_AES)
/*===========================================================================

FUNCTION MCCIDL_SEND_SMRM

DESCRIPTION
  This function sends SMRM when needed by AKA/AES to resynchronize
  crypto-sync

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 mccidl_send_smrm
(
  uint16 curr_state
);
#endif /* FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION MCCIDL_TRIGGER_SLEEP

DESCRIPTION
  This function forces SRCH & RXC tasks in sleep.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_trigger_sleep(void);

/*===========================================================================

FUNCTION MCCIDL_START_SLEEP

DESCRIPTION
  This function sends a command to the searcher task to go to sleep.

DEPENDENCIES
  None.

RETURN VALUE
  New state.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_start_sleep
(
  word new_state
);

#ifdef FEATURE_1X_SPLIT_OVERHEAD
/*===========================================================================

FUNCTION MCC_IS_SPLIT_OVHDS_SUPPORTED

DESCRIPTION
   This function returns whether the split overhead across multiple wakeups
   is supported or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if NV#73501 is set as'1'

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_split_ovhds_supported(void);

/*===========================================================================

FUNCTION MCC_SET_SRCH_FORCED_SLEEP

DESCRIPTION
   This function sets the indicator that MC is in forced sleep as a result of
   SYSTEM_RELEASE_R report from SRCH for split overheads

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_srch_forced_sleep(boolean flag);

/*===========================================================================

FUNCTION MCC_IS_SRCH_FORCED_SLEEP

DESCRIPTION
   This function returns whether MC is in forced sleep as a result of
   SYSTEM_RELEASE_R report from SRCH for split overheads or not

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if MC is in forced sleep for split overheads

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_srch_forced_sleep(void);

/*===========================================================================

FUNCTION MCC_SET_SPLIT_OVHD_BLOCK_SYS_REL_FLAG

DESCRIPTION
   This function will set cdma global var flag to force SRCH to ignore 
   unlock callback from TRM and block sending System Release rpt to MC 
   while in split overhead collection

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_split_ovhd_block_sys_rel_flag( boolean flag );

/*===========================================================================

FUNCTION MCC_GET_SPLIT_OVHD_BLOCK_SYS_REL_FLAG

DESCRIPTION
   This function will return cdma global var flag to force SRCH to ignore 
   unlock callback from TRM and block sending System Release rpt to MC 
   while in split overhead collection

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If SRCH need to block sending system release rpt to MC
  FALSE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_get_split_ovhd_block_sys_rel_flag( void );

#endif /* FEATURE_1X_SPLIT_OVERHEAD */


/*===========================================================================

FUNCTION MCCIDL_BLOCKED_RUDE_WAKEUP

DESCRIPTION
  This function performs the rudewakup and returns only once the Reacq is 
  done.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_blocked_rude_wakeup(word new_state);

/*===========================================================================

FUNCTION MCC_CHANGE_IMSI_TYPE

DESCRIPTION
  This function updates the imsi type, if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the imsi was changed; FALSE otherwise.

SIDE EFFECTS
  The cdma imsi variables may change.

===========================================================================*/

boolean mcc_change_imsi_type( boolean imsi_t_supported );

/*===========================================================================

FUNCTION MCC_SET_ADDRESS
  This function sets the address fields for a message to be transmitted
  on the access channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_set_address
(
  caii_ac_hdr_type *hdr,
    /* Pointer to header of access channel message to be transmitted */
  word             msg_type /*lint -esym(715,msg_type*/
    /* message type */
);


/*===========================================================================

FUNCTION MCC_CALCULATE_AUTH_DATA

DESCRIPTION
  This function figures out what the auth_data should be for authenticating
  an origination or SMS.

DEPENDENCIES
  The digits passed in must be in a byte array and be encoded in BCD.

RETURN VALUE
  The computed value of auth_data

SIDE EFFECTS
  None

===========================================================================*/

dword mcc_calculate_auth_data
(
  byte digits[],
    /* Dialed digits or destination address digits */
  byte array_len,
    /* The number of digits in the above array */
  boolean digit_mode
    /* TRUE if digits are ASCII, FALSE if digits are DTMF */
);


/*===========================================================================

FUNCTION MCCIDL_RETRYING_SLEEP

DESCRIPTION
  This function returns whether the phone is in the process of retrying
  sleep via OOS phase 1.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we're retrying sleep in between reacq failures, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_retrying_sleep(void);

/*===========================================================================

FUNCTION MCCIDL_UPDATE_SLEEP_SLOT_MASK

DESCRIPTION
  This procedure updates sleep_slot_mask with OR operation with input mask.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_update_sleep_slot_mask(byte mask);

/*===========================================================================

FUNCTION MCCNP_ITEM_IS_BLOCKED

DESCRIPTION
  Checks if the passed msg type is blocked

DEPENDENCIES
  None

RETURN VALUE
  True    Passed msg type is blocked

  False   Passed msg type is not blocked

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_msg_is_blocked (cdma_msg_type msg_type);

/*===========================================================================

FUNCTION MCC_PASSPORT_MODE_IS_WORLD

DESCRIPTION
  Returns TRUE if the passport mode is world.

DEPENDENCIES
  None

RETURN VALUE
  True    Passport mode is world

  False   Passport mode is jcdma

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_passport_mode_is_jcdma ( void );

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION MCC_PROC_PC_GEN_EXTN_RECORDS

DESCRIPTION
  This function processes the general extension records carried in the paging channel
  general extension message.

DEPENDENCIES
  None.

RETURN VALUE
  Error code.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 mcc_proc_pc_gen_extn_records
(
  caii_pc_gen_extn_type *pc_gem
    /* Pointer to General Extension Message whose records are to be processed */
);

/*===========================================================================

FUNCTION MCC_VALIDATE_RADIO_CONFIG_PARAMS

DESCRIPTION
  This function validates the fields in Radio Configuration Parameters message
  or record. It flags an error if any of the field is outside its valid range.

DEPENDENCIES
  None.

RETURN VALUE
  0 if message is OK; 1 otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mcc_validate_radio_config_params
(
  caii_radio_config_params_type *rcp
    /* Pointer to radio config parameters record */
);

/*===========================================================================

FUNCTION MCC_SET_DEFAULT_RC_PILOT_PARAMS

DESCRIPTION
  This function sets the default values in a RC parameters record for one
  pilot as given in section 2.6.17 of C.S0005-E.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_default_rc_pilot_params
(
  cai_rc_pilot_params_type* rc_pilot_params
);

/*===========================================================================

FUNCTION MCC_SET_DEFAULT_RCP_DATA

DESCRIPTION
  This function sets all the values in a RCP data structure to the default
  values as given in section 2.6.16 of C.S0005-E.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_default_rcp_data
(
  mccdma_rcp_data_type* rcp_data
   /* Pointer to structure containing RCP data */
);

/*===========================================================================

FUNCTION MCC_UPDATE_RCP_DATA

DESCRIPTION
  This function updates the RCP data buffers. It sets all values of
  mcc_rcp_data to default if no RCP structure is passed. If RCP record
  structure is passed, then it updates the values with those received
  in RCP structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_update_rcp_data
(
  caii_radio_config_params_type *rcp
    /* Pointer to radio config parameters record */
);

/*===========================================================================

FUNCTION MCC_SYNC_RCP_DATA_AND_ASET

DESCRIPTION
  This function set the pilot state of pilots in RCP data according to
  whether they are in ASET or not.
  If pilot is in ASET then state will be marked as MCC_RCP_PILOT_ACTIVE.
  If pilot is not in ASET, then state will be marked as MCC_RCP_PILOT_INVALID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_sync_rcp_data_and_aset
(
  void
);

/*===========================================================================

FUNCTION MCC_GET_RXC_TXC_RC_PARAMS

DESCRIPTION
  This function gets the values for RC Paramaters that to be passed to
  RXC and TXC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_get_rxc_txc_rc_params
(
  cai_rcp_rxc_txc_type* rcp_rxc_txc_ptr
);

/*===========================================================================

FUNCTION MCC_GET_PILOTS_RC_PARAM_RECS

  This function updates the records pointer with the Radio Config Parameter
  records for the Active Pilots.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcc_get_pilots_rc_param_recs
(
  cai_rcp_srch_type *rcp_srch_ptr
  /* Pointer to structure containing number of RC Parameters records
     and RC parameters records */
);

/*
#ifdef FEATURE_IS2000_1X_ADV_DEBUGGING
extern void print_rcpm(caii_radio_config_params_type *rcpm);
extern void print_radio_config_params
(
   byte num_rc_params_rec,
   caii_rc_params_record_type rc_params_records[]
);
extern void print_rcpm_rxc_txc
(
  cai_rcp_rxc_txc_type rcpm_rxc_txc
);
extern void print_rcpm_srch
(
  cai_rcp_srch_type rcpm_srch
);
#endif *//* FEATURE_IS2000_1X_ADV_DEBUGGING */
#endif /* FEATURE_IS2000_1X_ADV */

#ifdef FEATURE_CSIM
/*===========================================================================

FUNCTION MC_MMGSDI_CLIENT_ID_PTR

DESCRIPTION

  Return a pointer to mc_mmgsdi_client_id, which contains the client ID
  provided by MMGSDI to MC.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to client ID provided by MMGSDI to MC.

SIDE EFFECTS
  none

===========================================================================*/
mmgsdi_client_id_type  *mc_mmgsdi_client_id_ptr( void );


/*===========================================================================
FUNCTION MCMMGSDI_CLIENT_ID_REG_CB

DESCRIPTION
  Callback function from MMGSDI to acknowledge the MC registration for events

DEPENDENCIES
  MC should have called client ID reg function  prior to calling this.

SIDE EFFECTS
  None
===========================================================================*/
void mcmmgsdi_client_id_reg_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);


/*===========================================================================
FUNCTION MCMMGSDI_SESSION_REG_CB

DESCRIPTION
  This function handles the MMGSDI SESSION OPEN command callback.
  It will assign the global Session ID value which will be used in mmgsdi
  function calls

DEPENDENCIES
   Valid client ID should have been obtained before calling this.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcmmgsdi_session_reg_cb
(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

/*===========================================================================
FUNCTION MCC_MMGSDI_READ_WRITE_OPR_CB

DESCRIPTION
  Call back function associated with each read/write operation.

RETURN VALUE
  mmgsdi_appNone_enum_type

DEPENDENCIES
  None
===========================================================================*/
extern void mcc_mmgsdi_read_write_opr_cb
(
                       mmgsdi_return_enum_type  status,
                       mmgsdi_cnf_enum_type      cnf,
                       const mmgsdi_cnf_type     *cnf_ptr
                       );

/*===========================================================================

FUNCTION mcc_mmgsdi_read_prl_cb()

DESCRIPTION 
  Call back API for reading PRL through MMGSDI API

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
extern void mcc_mmgsdi_read_prl_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type    *cnf_ptr
);

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
/*===========================================================================

FUNCTION MC_MMGSDI_EVT_CB()

DESCRIPTION 
  This callback function will be called by MMGSDI to notify MC 
  about card related events.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  May change the mc_sim_busy global variable
  
===========================================================================*/
extern void mc_mmgsdi_evt_cb
(
  /*Event reported by mmgsdi*/
  const mmgsdi_event_data_type *event_ptr
);

/*===========================================================================

FUNCTION MC_SEND_MMGSDI_CANCEL

DESCRIPTION
  This function is responsible for cancelling NVRUIM's pending command(s)
  with MMGSDI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mc_send_mmgsdi_cancel
(
  void
);

/*===========================================================================

FUNCTION MC_MMGSDI_MONITOR_QUEUES

DESCRIPTION
  This function is responsible for Monitoring the CMD, SRCH, MSGR
  and RXTX queues while waiting for response from MMGSDI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May cancel all pending requests to MMGSDI or remove reports
  from srch/rxtx/msgr queues.

===========================================================================*/

void mc_mmgsdi_monitor_queues
(
  void
);

/*===========================================================================

FUNCTION MC_SET_SIM_BUSY_STATUS

DESCRIPTION
  This function sets the status of sim, i.e.
  it is busy or not according to the status
  received from mmgsdi.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mc_set_sim_busy_status
(
  boolean status
);

/*===========================================================================

FUNCTION MC_GET_SIM_BUSY_STATUS

DESCRIPTION
  This function returns the status of sim, i.e.
  it is busy or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  Sim Busy event received from MMGSDI
  FALSE MC has not been notified of sim busy event

SIDE EFFECTS
  None

===========================================================================*/

boolean mc_get_sim_busy_status
(
  void
);

/*===========================================================================

FUNCTION MC_SET_AUTH_ERR_STATUS

DESCRIPTION
  This function sets the error reason returned
  by auth task to sim busy.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mc_set_auth_err_status
(
  boolean status
);

/*===========================================================================

FUNCTION MC_GET_AUTH_ERR_STATUS

DESCRIPTION
  This function checks if error returned by auth task 
   is due to sim busy event.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  Auth error is due to Sim Busy
  FALSE Auth error is not due to Sim Busy

SIDE EFFECTS
  None

===========================================================================*/

boolean mc_get_auth_err_status
(
  void
);

#endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

/*===========================================================================
FUNCTION MCC_MMGSDI_WAIT

DESCRIPTION
  Wait for the signal set from MMGSDI.
RETURN VALUE
  Signal mask from signals that were set after rex_wait call..
DEPENDENCIES
  None
===========================================================================*/

extern rex_sigs_type  mcc_mmgsdi_wait
(
  /* Mask of signals to wait for */
  rex_sigs_type  sigs
);

#endif /* (FEATURE_CSIM) */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

  FUNCTION MC_MCFG_EVT_CB
  
  DESCRIPTION 
    This method is used to register with Modem Configuration. 
    It will be called under MCFG context to post the NV REFRESH Indication
    status whenever SIM SWAP without Reset is going to happen. It will also 
    post the MC_NV_REFRESH_F command to the MC task with NV Refresh status.
  
  DEPENDENCIES
    None.
    
  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None.
  
===========================================================================*/
extern boolean mc_mcfg_evt_cb
(
  /*Event reported by Modem Configuration */
   mcfg_refresh_info_s_type *p_info
);

/*===========================================================================

  FUNCTION MC_SEND_REFRESH_DONE
  
  DESCRIPTION 
    This method is used to send the NV Refresh done indication to MCFG. 
  
  DEPENDENCIES
    None.
    
  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None.
  
===========================================================================*/
extern void mc_send_refresh_done
(
  /* Reference ID sent to MCFG */
  uint32 reference_id
);

#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================

FUNCTION MCC_STATS_INIT

DESCRIPTION
  This function Initialize the CDMA stats buffer and clear the mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_stats_init
(
  void
);

/*===========================================================================

FUNCTION MCC_STATS_REPORTING_TIMER_CB_FUNC

DESCRIPTION
  This is the function that will be executed (in MC context) upon
  the expiration of the statistics reporting timer .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_stats_reporting_timer_cb_func
(
  int4 ms_interval
);

/*===========================================================================

FUNCTION MCC_SET_CDMA_STATS_MASK
  This function sets the statistics mask for CDMA (CP and Searcher).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_set_cdma_stats_mask
(
  uint32 bit_mask
);

/*===========================================================================

FUNCTION MCC_RESET_SCH_STATS
  This function resets the CDMA statistics related to SCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_reset_sch_stats
(
  void
);
/*===========================================================================

FUNCTION MCC_GET_LAST_INFO_REC_WITH_CM_RPT

DESCRIPTION
  This function gives the record number of the last info record, for which,
  MC should send report to CM.

DEPENDENCIES

RETURN VALUE
  Returns the record number of the last info record, for which,
  MC should send report to CM.
  Returns Zero, if there is no record, for which, MC should send report to CM.

SIDE EFFECTS
  None


===========================================================================*/
extern byte mcc_get_last_info_rec_with_cm_rpt
(
  byte msg_type,
    /* Message type */
  byte num_recs,
    /* Number of information records in message */
  byte *rec_ptr
    /* Pointer to information records */
);

/*===========================================================================

FUNCTION CDMA_ENTER_INIT

DESCRIPTION
  This procedure is initializes variables used in the entrance state for
  the CDMA state machine.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  Certain variables used by the cdma_enter state will be initialized.

===========================================================================*/

word cdma_enter_init
(
  mcc_entry_type *mcc_entry_reason_ptr  /* Reason for entry */
);

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION MCC_RPT_AUTO_ACT_IND

DESCRIPTION
  This procedure reports Auto Activation of 1X Stack to MMODE,for IRAT switche
  like REDIR/RESEL.
  After reporting Act Indication, CP will block on reeipt of ACT_CNF or any
  deactivate attempt by MMOC.Upon reeiving ACT_CNF CP will consider itself
  operational on native 1X mode.

DEPENDENCIES
  None.

RETURN VALUE
  Next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_rpt_auto_act_ind(prot_act_e_type auto_act_rsn,word curr_state);

/*===========================================================================
FUNCTION MCCIDL_PROC_CSFB_CGI_REQ

DESCRIPTION
  This API will do the processing of CGI Req from LTE-RRC in native 1X mode.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mccidl_proc_csfb_cgi_req( void );

/*===========================================================================
FUNCTION MCC_CSFB_SEND_SRCH_TT_CMD

DESCRIPTION
  This procedure sends the Search task a SRCH_LTE_1X_TT_F command to make SRCH
  do the time transfer form LTE to 1X.
  With this command SRCH will:
  o Calculate system Time
  o Find the peak
  o Loack the finger on peak
  o Slam the combiner with LC state.

DEPENDENCIES
  Should have received Timing information/ pn, band/chan from LTE, to perform
  time transfer.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_srch_send_tt_cmd(void);

/*===========================================================================

FUNCTION MCC_RPT_HANDOVER_IND

DESCRIPTION
  This procedure reports Handover Indication from LTE to 1X to MMODE,for CSFB
  cases like ECAM AM=1.
  After reporting Act Indication, CP will block on reeipt of HANDOVER_CNF or
  any deactivate attempt by CM.Upon reeiving HANDOVER_CNF CP will consider itself
  operational on native 1X mode.

DEPENDENCIES
  None.

RETURN VALUE
  Next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_rpt_hanover_ind(word curr_state);

/*===========================================================================

FUNCTION MCC_CSFB_UPDATE_SRVCC_MODE

DESCRIPTION
  This API will updates the SRVCC mode value

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_update_srvcc_mode(boolean srvcc_mode);

/*===========================================================================

FUNCTION MCC_CSFB_GET_SRVCC_MODE

DESCRIPTION
  This API will return the SRVCC mode value

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if CSFB SRVCC mode activated, FALSE otherwise 

SIDE EFFECTS
  None.

===========================================================================*/
boolean mcc_csfb_get_srvcc_mode(void);

#endif /*FEATURE_MODEM_1X_IRAT_LTO1X*/

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
/*===========================================================================

FUNCTION MCC_COMPARE_PLMN

DESCRIPTION
  This function will check if the Camped PLMN stucture are equal or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If the CAMPLED PLMN structures are equal
  FALSE: If the CAMPLED PLMN structures are not equal

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mcc_compare_plmn(lte_rrc_plmn_s, lte_rrc_plmn_s);

#endif /* FEATURE_CSFB_CALL_SETUP_OPT */
/*===========================================================================

FUNCTION MCC_IS_UIM_RUIM_SUPPORT_SCI_SUPPORTED

DESCRIPTION
  This function provides whether FEATURE_UIM_RUIM_SUPPORT_SCI supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the FEATURE_UIM_RUIM_SUPPORT_SCI is supported.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mcc_is_uim_ruim_support_sci_supported(void);

/*===========================================================================

FUNCTION MCC_IS_SO35_SO36_DISABLED

DESCRIPTION
  This function provides whether SO35/SO36 disabled or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the SO35/SO36 is disabled.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mcc_is_so35_so36_disabled(void);

/*===========================================================================

FUNCTION MCC_OTASP_MAX_UNLOCK_ATTEMPTS_ALLOWED

DESCRIPTION
  This function returns Otasp Max unlock attempts allowed.

DEPENDENCIES
  None

RETURN VALUE
  Otasp Max unlock attempts allowed

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 mcc_otasp_max_unlock_attempts_allowed(void);

/*===========================================================================

FUNCTION MCC_IS_JCDMA_SUPPORTED

DESCRIPTION
  This function provides the JCDMA service availability.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the jcdma service is available.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mcc_is_jcdma_supported(void);

/*===========================================================================

FUNCTION MCC_IS_WAKEUP_ALLOWED

DESCRIPTION
  This function checks if the SRCH was forced sleep due to SRLTE incoming call
  if so, then SRCH shouldn't be awake if we are still waiting for ESR to be 
  completed.

DEPENDENCIES
  None.

RETURN VALUE
  True : if SRCH can do the wakeup now.
  False: if SRCH should avoid this wakeup.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mcc_is_wakeup_allowed(void);

/*===========================================================================

FUNCTION  mcc_clr_callback_timer

DESCRIPTION
  This function should be used to clr timers that utilise the REX
  callback mechanism within MCC. It clears the timer and its entry in
  the callback queue.

DEPENDENCIES
  None.

RETURN VALUE
  countdown of timer

SIDE EFFECTS
  remove the entry, (if any), corresponding to callback_timer_id from
  callback queue.

===========================================================================*/

extern rex_timer_cnt_type mcc_clr_callback_timer
(
    rex_timer_type       *p_timer,   /* pointer to timer to set */
    unsigned long         callback_timer_id
);


/*===========================================================================

FUNCTION MC_UPDATE_LAST_CDMA_ENTRY

DESCRIPTION
  This procedure updates the last CDMA entry reason based on the input
  parameter. It should be solely used by non MC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void mc_update_last_cdma_entry
(
  mcc_entry_reason_type entry_reason
);

/*===========================================================================

FUNCTION CDMA_EXIT

DESCRIPTION
  This procedure is the exit state for the CDMA state machine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RXTX task will go to start mode,  Receive task will exit CDMA mode,
  Transmit task will exit CDMA mode and Search task will go to Start
  mode.

===========================================================================*/

void cdma_exit
(
  boolean voc_pwr_off
    /* indicator of whether vocoder power needs to be turned off */
);

/*===========================================================================

FUNCTION MCC_SEND_SRCH_PARMS

DESCRIPTION
  This procedure sends the updated search parameters to the searcher.

DEPENDENCIES
  mcc_ho_real must have been updated appropriately

RETURN VALUE
  None.

SIDE EFFECTS
  The search parmameters in the searcher will be changed.

===========================================================================*/

void mcc_send_srch_parms( void );


/*===========================================================================

FUNCTION CDMA_RF_INIT

DESCRIPTION
  This procedure initializes rf for the CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cdma_rf_init
(
  word channel,
    /* channel to acquire */
  byte band_class
    /*  band class to acquire in */
);


/*===========================================================================

FUNCTION MCC_PUT_MSG

DESCRIPTION
  This procedure places a message for the currently active base station
  into the base station information array.

DEPENDENCIES
  Message to be placed in base station information array must be a
  valid type.

RETURN VALUE
  None.

SIDE EFFECTS
  This procedure will examine the message sequences of the messages
  being placed into the base station information array and adjust the
  message receipt flags and the message sequence numbers accordingly.

===========================================================================*/

extern void mcc_put_msg
(
  byte msg_type,      /* message type being placed in base station info */
  caii_rx_msg_type *msg_ptr          /* pointer to message to be placed */
);

/*===========================================================================
FUNCTION MCC_INIT_OVHD_INFO

DESCRIPTION
 This function initialises the Base station Info array to ensure that
 all the overhead messages are marked as invalid. This is done so that
 the Overhead Messages are processed properly when we re-enter CDMA.

 This function must be called each time CDMA is entered.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

void mcc_init_ovhd_info(void);

#ifdef FEATURE_1X_OVHD_OPTIMIZATION
/*===========================================================================

FUNCTION MCC_IS_OPT_SUPPORTED

  This function checks if a specific 1X overheads optimization feature
  is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  True : if the feature corresponding to the input flag is supported 
  False: otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_opt_supported(byte flag);

/*===========================================================================

FUNCTION MCC_ARE_CACHED_OVHDS_AVAILABLE

DESCRIPTION
  This function checks if overheads are cached for the BS provided
  as input to it. This API is meant to be used only for TC to IDLE
  transition and should not be considered a generic API for checking
  cached overheads.

DEPENDENCIES
  Overheads need to be current, then only are considered useful

RETURN VALUE
  None.

SIDE EFFECTS
 None

===========================================================================*/

boolean mcc_are_cached_ovhds_available
(
  bs_info_id_type * bs_info_id,
  int * index_found
);

/*===========================================================================

FUNCTION MCC_VALIDATE_SID_NID

DESCRIPTION
  This procedure checks if SID/NID in the bs info array entry match 
  the sid/nid provided in the arguments.

DEPENDENCIES
  None.

RETURN VALUE
  True - SID/NID match
  False - SID/NID do not match

SIDE EFFECTS
 None

===========================================================================*/

boolean mcc_validate_sid_nid
(
  byte i,
    /* Index into the bs info array */
  word sid,
    /* SID to match against BS info entry */
  word nid
    /* SID to match against BS info entry */
);
#endif /* FEATURE_1X_OVHD_OPTIMIZATION */

/*===========================================================================

FUNCTION MCC_ENTRY_INIT

DESCRIPTION
  This procedure initializes an entry in the base station information array
  with the parameters for the base station having the given pilot_pn and
  using the given cdma channel and paging channel.  If an entry already exists
  for the base station with the given pilot_pn and channel configuration, a
  new entry will not be created.  Since a base station may have different
  parameters for different channels, a separate entry will be kept for each
  different channel we use.  This procedure will set the current base station
  pointer to the correct entry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the base station information array is full, this procedure will
  replace the entry with the oldest time stamp with this entry.

===========================================================================*/

extern void mcc_entry_init
(
  bs_info_id_type *bs_info_id,
  boolean         td_supported
);

/*===========================================================================

FUNCTION MCC_ARE_OVHDS_NOT_EXPIRED

DESCRIPTION
  This API checks if the overheads on the current base station are expired or 
  not based on T31 timer value.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Overheads are not expired
  FALSE - Overheads are expired

SIDE EFFECTS
  None
===========================================================================*/

extern boolean mcc_are_ovhds_not_expired( void );

/*===========================================================================
FUNCTION MCC_SRCH_CMD

DESCRIPTION
  This function sends a command to the Searcher task.  If the Searcher task
  responds with a status other than SRCH_DONE_S,  this function will call
  ERR_FATAL to log the fatal error.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.  If this function returns, the status returned by the Searcher
  task was SRCH_DONE_S.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_srch_cmd
(
  srch_cmd_type *cmd_ptr
    /* pointer to command to send to Searcher */
);

/*===========================================================================

FUNCTION MCC_RXC_CMD

DESCRIPTION
  This function sends a command to the CDMA Receive subtask.  If the Receive
  responds with a status other than RXC_DONE_S,  this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.  If this function returns the status returned by RXC was RXC_DONE_S.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_rxc_cmd
(
  rxc_cmd_msg_type *cmd_ptr
    /* pointer to command to send to Receive task */
);

/*===========================================================================

FUNCTION MCC_TXC_CMD

DESCRIPTION
  This function sends a command to the CDMA Transmit subtask.  If the Transmit
  task responds with a status other than TXC_DONE_S,  this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_txc_cmd
(
  txc_cmd_type *cmd_ptr
    /* pointer to command to send to Transmit task */
);

/*===========================================================================

FUNCTION PERMUTE_ESN

DESCRIPTION
  This function permutes the ESN as described in IS-95 section 6.1.3.1.8.

DEPENDENCIES
  None.

RETURN VALUE
  Permuted esn.

SIDE EFFECTS
  None.

===========================================================================*/

extern dword permute_esn
(
  dword esn  /* ESN to permute */
);

/*===========================================================================

FUNCTION MCC_IS_REG_AFTER_DDTM_PAGE_DROP_SUPPORTED

DESCRIPTION
  This function checks if reg after ddtm page drop is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if reg after ddtm page drop is supported.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_reg_after_ddtm_page_drop_supported(void);

/*===========================================================================

FUNCTION MCC_IS_CFS_IN_PROG

DESCRIPTION
  This function checks if CFS is in progress or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if CFS is in progress.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_cfs_in_prog(void);

/*===========================================================================

FUNCTION MCC_IS_LTA_REQ_PENDING

DESCRIPTION
  This function checks if LTA Request pending in mcc_srch_q.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if LTA Request pending in mcc_srch_q 

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_lta_req_pending(void);

/*===========================================================================

FUNCTION MCC_IS_HHO_IN_PROG

DESCRIPTION
  This function checks if Hard Handoff is in progress or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if Hard Handoff is in progress.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_hho_in_prog(void);

/*===========================================================================

FUNCTION TC_PROC_DATA_TA_CTRL_CMD

DESCRIPTION
  This API will process TA control cmd from data moudle and will send
  SRCH_TUNE_AWAY_CNTL_F to SRCH to honor TA requests during data call
  traffic state.

DEPENDENCIES
  Data moudle will post this cmd to MC task, once it is done with LCP
  handshake during data call traffic channel establishment.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_proc_data_ta_ctrl_cmd 
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
);

/*===========================================================================

FUNCTION TC_DATA_TA_COMP_CONFIG_TXC

DESCRIPTION
  This function will send the required cmds to re-configure Tx to continue 
  with data call.

  This will be called in LTA case only, as we shutdown Tx while LTA Start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_data_ta_comp_config_txc (void);


/*===========================================================================

FUNCTION TC_DATA_TA_PRE

DESCRIPTION
  Pre-processing procedures for Tune Away Request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_data_ta_pre (void);

/*===========================================================================

FUNCTION TC_DATA_TA_POST

DESCRIPTION
  Post-processing procedures for Tune Away Complete.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_data_ta_post 
(
  boolean empty_ta_gap
    /* Boolean to give Empty TA Gap info */
);

/*===========================================================================

FUNCTION TC_SEND_DATA_TA_START_TO_MUX

DESCRIPTION
  This API will send required cmds to configure MUX (Rx,Tx)
  to perform Tune Away.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_send_data_ta_start_to_mux (void);

/*===========================================================================

FUNCTION TC_SEND_DATA_TA_COMP_TO_MUX

DESCRIPTION
  This API will send required cmds to re-configure MUX (Rx,Tx)
  back to data traffic state after Tune Away Complete.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_send_data_ta_comp_to_mux (void);

/*===========================================================================

FUNCTION TC_WAIT_FOR_DATA_TA_COMPLETE

DESCRIPTION
  This function will monitor different queues while 
  waiting for Tune Away complete report from SRCH.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If it is a Empty TA Gap
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean tc_wait_for_data_ta_complete (void);

/*===========================================================================

FUNCTION TC_CHECK_IF_DATA_TA_ALLOWED

DESCRIPTION
  This function will check if Data Tune Away allowed or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data Tune Away is allowed
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean tc_check_if_data_ta_allowed (void);

/*===========================================================================

FUNCTION TC_PROC_DATA_TA_REQ

DESCRIPTION
  This function will process the Tune Away request from SRCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_proc_data_ta_req (void);


/*===========================================================================

FUNCTION MCC_TA_INIT

DESCRIPTION
  This function Initializes 1X2G Tune Away Feature related items.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_ta_init (void);

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_HHO_DEFER_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away HHO (Hard Handoff) defter timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_proc_ta_hho_defer_timer_expiry(void);

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_HHO_EXEC_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away HHO (Hard Handoff) execution timer expiry.
  This timer running means, HHO execution is in progress and we should not 
  honor any TA requests from SRCH till this timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcsup_proc_ta_hho_exec_timer_expiry(void);

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_SHO_DEFER_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away SHO (Soft Handoff) defter timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_proc_ta_sho_defer_timer_expiry(void);

/*===========================================================================

FUNCTION MCCTCSUP_TA_IS_CFS_RPT_DEFERRRED

DESCRIPTION
  This API will check if we have enough time to process CFS rpt OR it should
  be deferred to process later.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If CFS rpt to be deferred.
  FALSE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcctcsup_ta_is_cfs_rpt_deferrred(void);

/*===========================================================================

FUNCTION MCCTCSUP_CHECK_AND_PROC_BUF_CFS_RPT

DESCRIPTION
  This API will check if any CFS rpt got buffered and it should be processed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_check_and_proc_buf_cfs_rpt(void);

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_CFS_DEFER_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away CFS defter timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_proc_ta_cfs_defer_timer_expiry(void);

/*===========================================================================

FUNCTION MCCTCSUP_IS_COND_CFS

DESCRIPTION
  This API will check if the CFS is conditional/unconditional CFS.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If it is conditional CFS.
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcctcsup_is_cond_cfs(void);

/*===========================================================================

FUNCTION MCCTCSUP_UPDATE_SRCH_CFS_PROC_STATUS

DESCRIPTION
  This API will updates CFS Proc status to keep track of boolean status
  returned to SRCH

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcsup_update_srch_cfs_proc_status(boolean status);

/*===========================================================================

FUNCTION MCCTCSUP_GET_SRCH_CFS_PROC_STATUS

DESCRIPTION
  This API will updates CFS Proc status to keep track of boolean status
  returned to SRCH

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: TRUE if CFS Proc status boolean set
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mcctcsup_get_srch_cfs_proc_status(void);

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_PRIO_INV_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away Prioriy Inversion timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_proc_ta_prio_inv_timer_expiry(void);

/*===========================================================================

FUNCTION MCCTCSUP_SEND_TA_PRIO_INV_CMD

DESCRIPTION
  This API will send Prioriy Inversion cmd to SRCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_send_ta_prio_inv_cmd(boolean prio_inv_req);

/*===========================================================================
FUNCTION MCCSA_WAIT_FOR_GEN_AUTH_SIGNATURE

DESCRIPTION
  This procedure waits for the authentication task to return an
  authentication signature value to be put in a transmittied message.
  While waiting we check for various other events and may decide to
  change states.  If this occurs the access attempt we were going to
  start is aborted.  If all goes well we receive the auth_signature
  and place it in the buffer supplied as one of the parameters.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  Coppies the auth signature value returned from the authentication task
  into a buffer pointed to by the first parameter.
===========================================================================*/
extern word mccsa_wait_for_gen_auth_signature
(
  dword *auth,
    /* Where to place computed auth signature value */

  #ifdef FEATURE_UIM_RUIM
  byte *auth_mode,
    /* Points to auth mode field in access message */
  #endif /* FEATURE_UIM_RUIM */

  word curr_state
    /* current state being processed */
);

/*===========================================================================
FUNCTION MCC_ENQUEUE_DBG_MMOC_CMD

DESCRIPTION
  This procedure maintains the mc_dbg_cmd_q which keeps track of sanity
  timer associated commands received from MMOC. This will help in debugging 
  MMOC sanity timer related issues as we will have the necessary information
  available from the dumps to find the processing history of MMOC sanity timer
  associated commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern void mcc_enqueue_dbg_mmoc_cmd
(
    mc_msg_type *mmoc_cmd,
      /* Command being processed by MC */
    word prev_state,
      /* State before this command is processed */
    word next_state,
      /* State after this command is processed */
    boolean is_ignored,
      /* Indicates if the command was processed or ignored by MC */
    mc_api_name_type api_name
      /* API in which this cmd is processed */

);

/*===========================================================================
FUNCTION MCC_ENQUEUE_DBG_MMOC_CNF

DESCRIPTION
  This procedure will keep track whether the confirmation or the indication
  has been sent to mmoc. This will help in debugging issues like whether the
  mmoc commands have been processed and the confirmation or indication has 
  been sent back to mmoc. These information will be present in the dumps.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void mcc_enqueue_dbg_mmoc_cnf
(
	mc_api_name_type api_name,
      /* API in which this cmd is processed */
	mc_mmoc_cnf_name_type mmoc_cnf_name,
	  /* MMOC cnf/ind which was sent */
	prot_trans_type	trans_id
	  /* Transaction id the cmd for which cnf or ind is send */
);
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================
FUNCTION MCC_ENQUEUE_DBG_MSGR_CMD

DESCRIPTION
  This procedure maintains the mc_dbg_msgr_rcv which keeps track of the 
  commands received in msgr queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern void mcc_enqueue_dbg_msgr_cmd
(
  mc_msgr_rcv_msg_type *msgr_cmd,
    /* Command received in msgr queue */
  word prev_state,
    /* State before this command is processed */
  word next_state,
    /* State after this command is processed */
  mc_api_name_type api_name
    /* API in which this cmd is processed */
);

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================
FUNCTION MCC_ENQUEUE_DBG_SRCH_CMD

DESCRIPTION
  This procedure maintains the mccsrch_dbg_buf which keeps track of SRCH 
  reports sent to CP. This will help in debugging crashes where SRCH -> CP 
  interaction is not clear as we will have the necessary information available
  from the dumps to find the processing history of SRCH reports.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void mcc_enqueue_dbg_srch_cmd
(
  mccsrch_report_code_type rpt,
    /* SRCH rpt being processed by MC */
  word prev_state,
    /* State before this report is processed */
  word next_state,
    /* State after this report is processed */
  mccidl_sleep_state_type prev_sleep_state,
    /* Sleep state before this report is processed */
  mccsrch_api_name_type api_name
    /* API in which this report is processed */
);

/*===========================================================================

FUNCTION MCCSA_SETUP_AUTH_MSG_INT_FIELDS

DESCRIPTION
  This function setups the authentication fields and message integrity
  fields.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
/* Following parameters not used with some FEATURE define combinations */
/*lint -esym(715, burst_type) */
/*lint -esym(715, data_len) */
/*lint -esym(715, data_ptr) */
/*lint -esym(715, digit_mode) */
extern word mccsa_setup_auth_msg_int_fields
(
  word curr_state,
  caii_tx_msg_type *msg_ptr,
  byte burst_type,    /*lint -esym(715,burst_type)*/
  byte data_len,      /*lint -esym(715,data_len)*/
  byte *data_ptr,     /*lint -esym(715,data_ptr)*/
  boolean digit_mode, /*lint -esym(715,digit_mode)*/
  byte num_of_digits, /*lint -esym(715,num_of_digits)*/
  caii_ac_auth_msg_int_type *auth_msg_int_ptr
);

#ifdef FEATURE_1X_CUST_VOICE_SO
/*===========================================================================

FUNCTION MCC_REG_ENCRYPT_DECRYPT_CB

DESCRIPTION
  This function registers the callback functions for providing customer 
  specific algorithms required for customer specific voice SO.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_reg_encrypt_decrypt_cb
(
  mcc_encryption_callback_type encryption_cb_func,
  mcc_decryption_callback_type decryption_cb_func
);

/*===========================================================================

FUNCTION MCC_SET_CUST_SO

DESCRIPTION
  This API sets the voice SO registered by the customer through NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_set_cust_so( word so );
#endif /* FEATURE_1X_CUST_VOICE_SO */

#ifdef FEATURE_QCHAT

/*===========================================================================

FUNCTION mcc_rqst_set_preferred_sci

DESCRIPTION
  This sets the current preferred SCI value, as requested by the upper layers.
  The preferred SCI in NV is not updated.

  This SCI here after will be used to indicate n/w as UE's SCI and will be set
  to preferred SCI  and operational SCI after successful registration with
  network.

DEPENDENCIES
  None.

RETURN VALUE
  The current preferred sci value MS is actually set to now.

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_rqst_set_preferred_sci(int8 set_sci_p);

/* ==========================================================================
FUNCTION GET_MAX_SLOT_CYCLE_INDEX_1X

DESCRIPTION
  The max slot cycle index supported by AN. Used for QChat application only.

DEPENDENCIES
  None.

RETURN VALUE
  byte - slot_cycle_index

SIDE EFFECTS
  None.

============================================================================*/
byte get_max_slot_cycle_index_1x(void);

#endif /* FEATURE_QCHAT */

/*===========================================================================
FUNCTION MCC_CM_MC_RPT2

DESCRIPTION
  This function sends a report to the CM task, based on what needs to be sent.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_cm_mc_rpt2
(
  sys_codec_ho_cmd cmd_type
    /* report type to send to cm */
);

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION MCC_CSFB_MT_CALL_CLEANUP

DESCRIPTION
  This function performs cleanup for any pending MT CSFB call. It releases 
  the call_id allocated for the MT call and resets the CSFB call status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  CSFB MT status is cleared.

===========================================================================*/
void mcc_csfb_mt_call_cleanup (  void );
#endif /*FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
/*===========================================================================

FUNCTION MCCIDL_PROC_DATA_BURST

DESCRIPTION
  This function processes a Data Burst Message received on the
  Paging Channel or Forward Common Control Channel while in Idle State.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word mccidl_proc_data_burst
(
  mccdma_matched_msg_type *matched_msg_ptr,
    /* Pointer to Paging Channel Data Burst Message */
  word new_state
    /* Next state to be processed */
);
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */

#ifdef FEATURE_1X_CP_IGNORE_GSRDM
/*===========================================================================

FUNCTION MCCIDL_GSRDM_IGNORE_TIMER_EXPIRY

DESCRIPTION
  This API will try to process the stored GSRDM message
  once the GSRDM ignore timer is expired on current system.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed

SIDE EFFECTS
  None

===========================================================================*/
extern word mccidl_gsrdm_ignore_timer_expiry(word new_state);
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */


/*===========================================================================

FUNCTION MCCIDL_PROC_ACCESS_TRNS_RES_DENIAL

DESCRIPTION
  This API will handle Transceiver res denial for any access attempt
  and will do send MO/MT fail rpt to CM if required.

DEPENDENCIES
  None

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_proc_access_trns_res_denial
(
  word curr_state /* Current state */
);

/*===========================================================================

FUNCTION MCCIDL_RESET_IDLE_PARAMS

DESCRIPTION
  This API will reset required idle parameters to move back to Idle state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_reset_idle_params( void );

#ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
/*===========================================================================

FUNCTION MCC_FREE_BS_INFO_ARRAY

DESCRIPTION
  This API will de-allocate bs_info[] array heap memory
  once the bs_info[] heap memory cleanup timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcc_free_bs_info_array( void );
#endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */
#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
/*===========================================================================

FUNCTION MCC_GET_VADAPTER_PARAMS

DESCRIPTION

  This API returns the registered parameters required to cummunicate
  with the CDMA voice adapter. Of the registration parameters included in 
  the returned value of type cdma_ivoice_cmd_open_t only session context,
  uplink and dowlink callbacks and event callback are of interest to the
  caller. This API is meant to be called only by MUX or CP.
  If there is no valid session context present this API will set NULL for 
  session context field of the returned value. It is the responsibility of 
  the caller to handle NULL session_context appropriately.

DEPENDENCIES
  For this API to return correct value:
    1. ASID for 1X should have already been set by CM
    2. Voice Adapter should have registered the session context with 1X

RETURN VALUE
  Registered params for communicating with Voice Adapter corresponding 
  to current ASID

SIDE EFFECTS
  None

===========================================================================*/
extern onex_voc_cmd_open_t mcc_get_vadapter_params
(  
  void
);

/*===========================================================================

FUNCTION MCC_SET_AVG_ENC_RATE

DESCRIPTION

  This API updates the average encoder rate global variable whenever MC task  
  sends the average encoder rate EVENT to Voice Adapter. Here the rate is
  the unaltered rate and the rate is NOT multiplied by a factor of 2 unlike
  old MVS based audio architecture.
  This API is meant to be invoked by 1X-CP only.

DEPENDENCIES
  This API should only be invoked during a traffic call

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_set_avg_enc_rate
(  
  uint32 avg_enc_rate
);

/*===========================================================================

FUNCTION MCC_GET_AVG_ENC_RATE

DESCRIPTION

  This API returns the average encoder rate to the caller. Here the rate is
  the unaltered rate and the rate is NOT multiplied by a factor of 2 unlike
  old MVS based audio architecture.
  This API is meant to be invoked by 1X-MUX only.

DEPENDENCIES
  This API should only be invoked during a traffic call

RETURN VALUE
  Average encoder rate (uint32)

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 mcc_get_avg_enc_rate
(  
  void
);

#endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */

/*===========================================================================

FUNCTION MCC_GET_ASID

DESCRIPTION

  This API returns the ASID value for the current CDMA subscription
  If 1X_CP has not yet been informed by MMode of the ASID value
  this API will return SYS_MODEM_AS_ID_NONE

DEPENDENCIES
  None.

RETURN VALUE
  ASID

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type mcc_get_asid
(  
  void
);

/*===========================================================================

FUNCTION MCC_GET_RAT_MASK

DESCRIPTION

  This API returns the RAT_MASK value for the current CDMA subscription
  If 1X_CP has not yet been informed by MMode of the RAT_MASK value
  this API will return 0.

DEPENDENCIES
  None.

RETURN VALUE
  RAT_MASK

SIDE EFFECTS
  None

===========================================================================*/
uint32 mcc_get_rat_mask
(  
  void
);

/*===========================================================================

FUNCTION MCC_GET_VSTMR_ID

DESCRIPTION

  This API returns the VSTMR ID that needs to be configured by 1X-L1.
  VSTMR ID is derived based on Subscription capabilty of current CDMA 
  subscription.
  This API is meant to be invoked by 1X-MUX only.

DEPENDENCIES
  This API should only be invoked during a traffic call

RETURN VALUE
  VSTMR id

SIDE EFFECTS
  None

===========================================================================*/
extern vstmr_rtc_id mcc_get_vstmr_id
(  
  void
);

/*===========================================================================

FUNCTION MCCSA_INCREMENT_APHO_DONE_COUNT

DESCRIPTION

  This API will increment the apho gone count.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccsa_increment_apho_done_count
(
  void
);

#ifdef FEATURE_SIMUL_1X_LTE
/*===========================================================================

FUNCTION MCC_UE_NOT_ON_SRLTE_PAGE_RCVD_BS

DESCRIPTION

  This API checks whether UE is on the same BS,
  where we received SRLTE MT page or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If UE is on SRLTE MT Page rcvd BS.
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_ue_not_on_srlte_page_rcvd_bs
(
  void
);
#endif /* FEATURE_SIMUL_1X_LTE */

#endif /*MCCDMA_I_H*/
