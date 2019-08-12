#ifndef MCCDMA_H
#define MCCDMA_H

/*===========================================================================

    M A I N   C O N T R O L   T A S K   C D M A    S U B - S Y S T E M
                       H E A D E R    F I L E
*/
/** 
@file mccdma.h

@brief Main Control Task CDMA Subsystem Header File

This file data and function declarations necessary for the CDMA Subsystem
of the Main Control task.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CDMA2k_1X_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and Latex. To edit or update 
      any of the file/group text in the PDF, edit the CDMA2k_1X_rapi_mainpage.dox file or 
      contact Tech Pubs.

      The above description for this file is part of the "main_control_cdma" group 
      description in the CDMA2k_1X_rapi_mainpage.dox file. 
===========================================================================*/

/*===========================================================================
  Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/mccdma.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/11   ssh     Added Doxygen comments. 
11/01/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup. 
10/26/10   jj      Added new state to support CSFB operation. 
09/01/10   azh     Added Doxygen comments. 
11/19/09   ssh     Added mux wrapper functions as part of CMI clean up 
                   activity.
10/12/09   jj      Meta-Comments cleanup for API's exported to DS.
08/31/09   adw     Removed unnecessary customer.h include.
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
06/02/09   jj      Changed an accessor function prototype and externalized
                   a function which gets used outside modem AU.
05/20/09   adw     Cleaned up includes.
05/14/09   jj      CMI phase2 changes.
04/01/09   adw     Added temporary CMI-lite featurization.
03/20/09   jtm     Removed includes for rxcmc.h and txcmc.h
03/04/09   adw     Removed the temporary _v.h include.
                   Added types exported via mclog.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_CP.07.02.09
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
#include "comdef.h"
#include "cai.h"
#include "enc.h"
#include "dec5000.h"

/** @addtogroup main_control_cdma 
@{ */ 

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/** @name Main States
@{ */

#define  CDMA_ENTER            0x0000  /**< Enter state. */
#define  CDMA_INIT             0x0100  /**< Init state. */
#define  CDMA_IDLE             0x0200  /**< Idle state. */
#define  CDMA_SYS_ACC          0x0300  /**< System Access state. */
#define  CDMA_TC               0x0400  /**< Traffic Channel state. */
#define  CDMA_EXIT             0x0500  /**< Exit state. */
#define  CDMA_CC               0x0600  /**< Call Control state. */
#define  CDMA_VOIP_HO          0x0700  /**< Voice-Over-IP Handoff state. */
#define  CDMA_CSFB             0x0800  /**< State for the CSFB mode of operation */
/** @} */ /* end_name Main States */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** Substates of a call control instance in the Traffic Channel state.

    These substates were part of the Traffic Channel state. With Release A and 
    concurrent services, multiple call control instances are allowed, and each 
    instance can be in one of these states.
*/
typedef enum
{
  CDMA_CC_WFO = CDMA_CC,  /**< Waiting For Order substate. */
  CDMA_CC_WFA,            /**< Waiting For Answer substate (an answer from the 
                               mobile station). */
  CDMA_CC_CONV,           /**< Conversation substate. */
  CDMA_CC_RELEASE         /**< Release substate. */
} cdma_cc_state_type;


/** Mobile handoff. */
typedef enum
{
  CDMA_IDLE_HO,    /**< Handoff is idle. */
  CDMA_AEHO,       /**< Access entry handoff. */
  CDMA_AHO,        /**< Access handoff. */
  CDMA_APHO,       /**< Access probe handoff. */
  CDMA_RAHO,       /**< Reacquisition handoff. For event reporting only. */
  CDMA_FAST_RAHO   /**< Fast reacquisition handoff. For event reporting only. */
} cdma_ho_type;

/** @brief Specifies retry information. */
typedef struct
{
  cai_retry_order_type  retry_type; /**< Origination of the retry: resource 
                                         request or SCH request message. */
  boolean infinite_delay;           /**< Indicates an infinite delay: TRUE (infinite 
                                         delay) or FALSE. */
  uint32  retry_delay_duration;     /**< Retry delay duration in milliseconds. */
} retry_delay_type;

/** Decoder rate set. */
typedef enum {
  MC_DEC_RATE_9600    = DEC_RATE_9600,     /**< 9600 bps rate set. */
  MC_DEC_RATE_14400   = DEC_RATE_14400,    /**< 14400 bps rate set. */
  MC_DEC_RATE_INVALID = DEC_RATE_INVALID   /**< Invalid rate set. */
} mc_dec_rate_set_type;

/** Decoder radio configuration. */
typedef enum {     
   MC_DEC_RC1  = DEC_RC1,  /**< Radio configuration type 1. */
   MC_DEC_RC2  = DEC_RC2,  /**< Radio configuration type 2. */
   MC_DEC_RC3  = DEC_RC3,  /**< Radio configuration type 3. */
   MC_DEC_RC4  = DEC_RC4,  /**< Radio configuration type 4. */
   MC_DEC_RC5  = DEC_RC5,  /**< Radio configuration type 5. */
   MC_DEC_RC11 = DEC_RC11  /**< Radio configuration type 11. */
} mc_dec_rc_type;

/**  Decoder supplemental channel rate. */
typedef enum {    
  MC_DEC_SCH_RATE_1X  = DEC_SCH_RATE_1X,  /**< Supplemental channel rate 1. */
  MC_DEC_SCH_RATE_2X  = DEC_SCH_RATE_2X,  /**< Supplemental channel rate 2. */
  MC_DEC_SCH_RATE_4X  = DEC_SCH_RATE_4X,  /**< Supplemental channel rate 4. */
  MC_DEC_SCH_RATE_8X  = DEC_SCH_RATE_8X,  /**< Supplemental channel rate 8. */
  MC_DEC_SCH_RATE_16X = DEC_SCH_RATE_16X, /**< Supplemental channel rate 16. */
  MC_DEC_SCH_RATE_32X = DEC_SCH_RATE_32X  /**< Supplemental channel rate 32. */
} mc_dec_sch_rate_type;

/** Data encoding rate set for enc_data_out(). */
typedef enum {
  MC_ENC_RATE_9600    = ENC_RATE_9600,   /**< 9600 bps rate set. */
  MC_ENC_RATE_14400   = ENC_RATE_14400,  /**< 14400 bps rate set. */
  MC_ENC_RATE_INVALID = ENC_RATE_INVALID /**< Invalid rate set. */
} mc_enc_rate_set_type;

/** Encoder supplemental channel radio configuration. */
typedef enum
{                             
  MC_ENC_SCH_RC_3    = ENC_SCH_RC_3,    /**< Radio configuration type 3. */
  MC_ENC_SCH_RC_4    = ENC_SCH_RC_4,    /**< Radio configuration type 4. */
  MC_ENC_SCH_RC_8    = ENC_SCH_RC_8,    /**< Radio configuration type 8. */
  MC_ENC_NUM_SCH_RCS = ENC_NUM_SCH_RCS  /**< Maximum enumeration value. For internal 
                                             use only.*/
} mc_enc_sch_radio_config_type;

/** Data encoding rates for the enc_sch_data_out() function. */
typedef enum
{                              
  MC_ENC_SCH_8TH_RATE     = ENC_SCH_8TH_RATE,  /**< RC3:    1.5 Kbps, RC4:    1.8 Kbps. */
  MC_ENC_SCH_QTR_RATE     = ENC_SCH_QTR_RATE,  /**< RC3:    2.7 Kbps; RC4:    3.6 Kbps. */
  MC_ENC_SCH_HALF_RATE    = ENC_SCH_HALF_RATE, /**< RC3:    4.8 Kbps; RC4:    7.2 Kbps. */
  MC_ENC_SCH_FULL_RATE    = ENC_SCH_FULL_RATE, /**< RC3:    9.6 Kbps; RC4:   14.4 Kbps. */
  MC_ENC_SCH_2X_RATE      = ENC_SCH_2X_RATE,   /**< RC3:   19.2 Kbps; RC4:   28.8 Kbps. */
  MC_ENC_SCH_4X_RATE      = ENC_SCH_4X_RATE,   /**< RC3:   38.4 Kbps; RC4:   57.6 Kbps. */
  MC_ENC_SCH_8X_RATE      = ENC_SCH_8X_RATE,   /**< RC3:   76.8 Kbps; RC4:  115.2 Kbps. */
  MC_ENC_SCH_16X_RATE     = ENC_SCH_16X_RATE,  /**< RC3:  153.5 Kbps; RC4:  230.4 Kbps. */
  MC_ENC_SCH_32X_RATE     = ENC_SCH_32X_RATE,  /**< RC3:  307.2 Kbps; RC4: not supported. */
  MC_ENC_NUM_SCH_RATES    = ENC_NUM_SCH_RATES  /**< Maximum value of the enumeration.
                                                    For internal use only. */
} mc_enc_sch_rate_type;

/** @} */ /* end_addtogroup main_control_cdma */
/** @addtogroup main_control_cdma 
@{ */

/*===========================================================================

                      FUNCTION DECLARATIONS
===========================================================================*/
/*reg_retry_delay */

/** Typedef for the reg_retry_delay() function. */
typedef void (*retry_delay_inform_func_type) (retry_delay_type retry_parm);
/*~ CALLBACK retry_delay_inform_func_type POINTER */

/**
  Binds a retry delay function to a function pointer. If an old retry delay 
  function already exists, this function overwrites it.

  @param[in] ds_retry_delay_inform    Function pointer to be bound with a retry 
                                      delay function.
  
  @return 
  None. 
 
  @dependencies 
  None.
*/
void reg_retry_delay
(
  retry_delay_inform_func_type ds_retry_delay_inform
);

/*===========================================================================*/
/* mcc_sdb_supported */
/**
  Returns an indication of whether the short data burst is supported by the current 
  base station.

  @return
  TRUE -- Short data burst is supported.\n
  FALSE -- Short data burst is not supported.

  @dependencies 
  None.
*/
extern boolean mcc_sdb_supported(void);

/*===========================================================================*/
/* mccdma_get_cur_bs_info */
/**
  Copies the cur_bs_ptr fields and passes the values to the calling function.

  @param[out] sid               Pointer to the system identifier.
  @param[out] nid               Pointer to the network identifier.
  @param[out] base_id           Pointer to the base station identifier.
  @param[out] base_class        Pointer to the base station class.
  @param[out] pilot_pn          Pointer to the pilot pseudonoise sequence offset index.
  @param[out] packet_zone_id    Pointer to the packet zone identifier.
  @param[out] mcc               Pointer to the mobile country code.
  @param[out] base_lat          Pointer to the base station latitude.
  @param[out] base_long         Pointer to the base station longitude.

  @return 
  None.

  @dependencies 
  None.
*/
void mccdma_get_cur_bs_info
(
  word* sid,
  word* nid,
  word* base_id,
  byte* base_class,
  word* pilot_pn,
  byte* packet_zone_id,
  word* mcc,
  dword* base_lat,
  dword* base_long
);

/*===========================================================================*/
/* mcc_get_cur_cdma_info */
/**
  Returns the current values of certain CDMA attributes.

  @param[out] scm            Pointer to the station class mark.
  @param[out] mob_cai_rev    Pointer to the mobile Common Air Interface revision.
  @param[out] sci_p          Pointer to the preferred slot cycle index.

  @return 
  None. 

  @dependencies 
  None.
*/
void mccdma_get_cur_cdma_info
(
  byte* scm,
  byte* mob_cai_rev,
  int8*  sci_p
);

/*===========================================================================*/
/* mcc_wait */
/**
  Waits for specified signals to be set. While it waits, this function continues 
  to set the watchdog report signal and reset the report timer (mcc_rpt_timer).

  @param[in] mask    Mask of signals.

  @return 
  Signal mask from signals that were set after a rex_wait call.

  @dependencies 
  Depends on previously having used the mcc_rpt_timer to specify when to
  set the watchdog signal.
*/
extern dword mcc_wait
(
  dword mask
);

/*========================================================================*/
/* mcc_wraprxc_get_sch_info */
/**
  Wraps the MUX function, rxc_get_sch_info().

  @param[in] sch_rc          Pointer to the radio configuration of the Supplemental 
                             Channel (SCH) (used only if RS2 is defined).
  @param[in] sch_rate_ptr    Pointer to the current SCH rate.
  @param[in] double_ptr      Indicates whether double-sized PDUs are being used
                             (TRUE for double-sized, FALSE for single-sized). 

  @return 
  TRUE -- An SCH is assigned.\n
  FALSE -- No SCH is assigned.

  @dependencies 
  Compile time dependency on the MUX function, rxc_get_sch_info().
*/
extern boolean mc_wraprxc_get_sch_info
(
  mc_dec_rc_type*       sch_rc,
  mc_dec_sch_rate_type* sch_rate_ptr,
  boolean* double_ptr
);
/*~ FUNCTION mc_wraprxc_get_sch_info  
    ONERROR return FALSE */
/*~ PARAM OUT sch_rc POINTER */
/*~ PARAM OUT sch_rate_ptr POINTER */
/*~ PARAM OUT double_ptr POINTER */

/*========================================================================*/
/* mc_wraprxc_get_fch_rate_set */
/**
  Wraps the MUX function, rxc_get_fch_rate_set().

  @return 
  Rate set of the decoder.

  @dependencies 
  Compile time dependency on the MUX function, rxc_get_fch_rate_set().
*/
extern mc_dec_rate_set_type mc_wraprxc_get_fch_rate_set(void );
/*~ FUNCTION mc_wraprxc_get_fch_rate_set 
    ONERROR return MC_DEC_RATE_INVALID*/

/*========================================================================*/
/* mc_wraprxc_get_dcch_rate_set */
/**
  Wraps the MUX function, rxc_get_dcch_rate_set().

  @return 
  Rate set for the Dedicated Control Channel (DCCH).

  @dependencies 
  Compile time dependency on the MUX function, rxc_get_dcch_rate_set().
*/
extern mc_dec_rate_set_type mc_wraprxc_get_dcch_rate_set(void );
/*~ FUNCTION mc_wraprxc_get_dcch_rate_set 
    ONERROR return MC_DEC_RATE_INVALID*/

/*===========================================================================*/
/* mc_wraprxc_dcch_is_on */
/**
  Wraps the MUX function, rxc_dcch_is_on().

  @return 
  TRUE -- DCCH is on.\n
  FALSE -- DCCH is off.

  @dependencies 
  Compile time dependency on the MUX function, rxc_dcch_is_on().
*/
extern boolean mc_wraprxc_dcch_is_on (void);
/*~ FUNCTION mc_wraprxc_dcch_is_on 
    ONERROR return FALSE */

/*===========================================================================*/
/* mc_wraptxc_get_fch_rate_set */
/**
  Wraps the MUX function, txc_get_fch_rate_set().

  @return 
  Rate set for the Fundamental Channel (FCH).

  @dependencies 
  Compile time dependency on the MUX function, txc_get_fch_rate_set().
*/
extern mc_enc_rate_set_type mc_wraptxc_get_fch_rate_set ( void );
/*~ FUNCTION mc_wraptxc_get_fch_rate_set 
    ONERROR return MC_ENC_RATE_INVALID */

/*===========================================================================*/
/* mc_wraptxc_get_dcch_rate_set*/
/**
  Wraps the MUX function, txc_get_dcch_rate_set().

  @return 
  Rate set for the DCCH.

  @dependencies 
  Compile time dependency on the MUX function, txc_get_dcch_rate_set().
*/
extern mc_enc_rate_set_type mc_wraptxc_get_dcch_rate_set (void );
/*~ FUNCTION mc_wraptxc_get_dcch_rate_set 
    ONERROR return MC_ENC_RATE_INVALID */

/*===========================================================================*/
/* mc_wraptxc_dcch_is_on*/
/**
  Wraps the MUX function, txc_dcch_is_on().

  @return 
  TRUE -- DCCH is on.\n
  FALSE -- DCCH is off.

  @dependencies 
  Compile time dependency on the MUX function, txc_dcch_is_on().
*/
extern boolean mc_wraptxc_dcch_is_on(void );
/*~ FUNCTION mc_wraptxc_dcch_is_on 
    ONERROR return FALSE */

/*===========================================================================*/
/* mc_wraptxc_get_sch_info*/
/**
  Wraps the MUX function, txc_get_sch_info().

  @param[out] sch_rc         Pointer to the radio configuration of the SCH.
  @param[out] sch_rate_ptr   Pointer to the current SCH rate. This is meaningful 
                             only when an SCH is assigned.
  @param[out] double_ptr     Indicates whether double-sized PDUs are being used
                             (TRUE for double-sized, FALSE for single-sized). 
                             This is meaningful only when an SCH is assigned.

  @return 
  TRUE -- An SCH is assigned.\n
  FALSE -- No SCH is assigned.

  @dependencies 
  Compile time dependency on the MUX function, txc_get_sch_info().
*/
extern boolean mc_wraptxc_get_sch_info
(
  mc_enc_sch_radio_config_type* sch_rc,
  mc_enc_sch_rate_type* sch_rate_ptr,
  boolean* double_ptr
);
/*~ FUNCTION mc_wraptxc_get_sch_info  
    ONERROR return FALSE */
/*~ PARAM OUT sch_rc POINTER */
/*~ PARAM OUT sch_rate_ptr POINTER */
/*~ PARAM OUT double_ptr POINTER */

/** @} */ /* end_addtogroup main_control_cdma */

#endif /* MCCDMA_H */
