#ifndef RRCINTIF_H
#define RRCINTIF_H
/*===========================================================================
         R R C   I N T E R N A L   C O M M A N D    I N T E R F A C E S

DESCRIPTION

  This module contains definitions for commands used 
  for RRC's internal command queue. These are commands sent from
  one RRC procedure to another. These commands can be queued
  only into rrc's internal command queue.


Copyright (c) 2000-2009 Qualcomm Technologies, Inc. 
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcintif.h_v   1.55   10 Jul 2002 12:15:02   vkhanna  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rrcintif.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/16   vi      Made changes to pass the correct reason in transition request to wl1 when reselection is failed due to pending l2 ack
12/10/14   sp      Made changes to revert reordering of structures
12/03/14   sp      Made changes to save one byte status in cmd_ptr to crash debug global
11/12/14   sr      Code reduction to remove GET_ALL_SIBS handling
07/18/14   sr      Changes for NV refresh FR
06/26/14   sn      WRRC changes for FR20484
26/05/14   bc      Made changes to send correct failure reason to GSM during GTOW reselection 
                   failure due to either PLMN mismatch or S-criteria failure.
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
11/07/13   bc      Made changes to abort sib reading if we find that the cell is barred 
                   or la is forbidden.
06/12/13   ad      Made changes to configure from OC when cell update confirm takes to 
                   PCH state and OC is set.This will remove the use of 
                   RRCLLC_CHAN_CFG_EPCH_HRNTI_ONLY cause.
06/07/13   vi      Created ALLOC_ASN1ARRAY2 macro
04/24/13   sn      WRRC side of changes to meet CSG ADAPT TCs
12/12/12   mp      Fixing KW errors
12/07/12   sn      CSG feature changes
11/06/12   sr      Added code changes to remember BPLMN suspend request and send confirm when 
                   CSP is in ACQ CNF or FREQ_SCAN_CNF state and BPLMN abort is already pending
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/06/12   ad      Changes for HS-RACH feature
03/21/12   sn      Changes to include LAC info when providing cell info to GPS in DCH state
01/12/12   gv      Added code to support FEATURE_PPAC
12/25/11   xl      Roll back to #15
12/25/11   xl      Merge from MPSS.NI.1.1
12/13/11   sks     Added support for LTOW SRVCC.
11/30/11   sn      Created a new update_sibs cause to handle update_sib event 
                   posted by csp when csfb call is cancelled. 
09/30/11   gv      CMI de-featurization
08/10/11   su      Added code to not invalidate the SIBDB entry 
                   when we read the SIBs through OTA during CSFB. 
07/29/11   mk      Removed nested coment for warning fix
07/29/11   ad      Made changes for CMI Phase 4
05/30/11   rl      CMI de-featurization
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
04/20/11   as      Made changes to handle cell update confirm with dhrnti and epch state
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11   ad      Added changes to refeaturize the API files
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/24/11   ad      Made changes to fix compilation errors on 76xx target for SW decoupling
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/17/11   ad      Made changes in RRC related to L2 SW Decoupling
01/17/11   ad      Made changes to fix the compilation errors of RRC SW Decoupling
01/13/11   ad      Added changes removing the defeaturization for FEATURE_MODEM_MBMS
01/12/11   ad      Added changes for RRC SW decoupling
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/14/10   rm      Replaced gstki.h inclusion with gstk_exp_v.h for CMI targets
10/11/10   rm      Added code to support 3GPP CSFB
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
09/20/10   prk/kp  Added code to support FEATURE_WCDMA_HS_FACH
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
05/24/10   as      Made changes to index SIBDB with common PLMN ID for shared cell
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/12/10   dm      Added new element cause for Update SIBS.
03/25/10   rm      Added failure cause in change mode confirm
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
11/04/09   sks     Added code to support inter-RAT redirection via connection reject
                   and both inter-RAT and inter-freq redirection via connection release.
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
05/22/09   rm      Added new channel config reason RRCLLC_CHAN_CFG_REASON_BPLMN_CTCH_CONFIG
05/08/09   ss      Updated Copyright Information
03/16/09   kp      Added structure rrc_cipher_config_cnf for existing cmd
                   i.d RRC_CIPHER_CONFIG_CNF.
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
12/26/08   sk      Added RRC_TX_TO_DISCON_UNSUPPORTED_REDIRECTION 
12/08/08   ns      Added support for FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
11/27/08   sk      Added a new CU cause for BPLMN 320ms
09/12/08   kp      Added code for REl7 under FEATURE_WCDMA_REL7. Removed
                    feature flag FEATURE_CELLPCH_URAPCH
03/07/08   vr/da   Added commands to support MBMS in DCH state
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
02/07/07   da      Added support for FEATURE_PLI_NMR_UTRAN
01/28/08   vr      Changed feature definition of Inter SIB Sleep from FEATURE_INTER_SIB_SLEEP
                   to (defined(FEATURE_INTER_SIB_SLEEP) || defined(FEATURE_WCDMA_DRX_MANAGER))
                   since the new DL code in L1 uses FEATURE_WCDMA_DRX_MANAGER instead of
                   FEATURE_INTER_SIB_SLEEP
12/21/07   vr      Support for FEATURE MBMS
11/28/07   rm      Added support for handling multiple RLC operations on a
                   single RB under FEATURE MULTIPLE RLC OPERATIONS
09/11/07   kp      Code clean up, as RLC suspend is done as a function call.
09/10/07   pk      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
03/29/07   segu    Added new enum SIB_EVENT_FAILED_FPLMN in rrc_sib_status_e_type
                   under the feature flag FEATURE_SKIP_SIBS_FOR_FPLMN
03/28/07   bu      Added appropriate featurization around a few variables.
03/28/07   kp      Removed BCCH Modification members in structure rrc_update_sibs_ind_type 
                   as its not used now, since entire BCCH mod timer is implemented in rrcpg1.c.
01/04/07   kp      Added new members CIPH_CONFIG_UL_KEYS & CIPH_CONFIG_DL_KEYS
                   to enum rrc_ciph_config_e_type for updating DL & UL Ciphering 
                   Keys Separately.
12/18/06  pk       Added a new member RRC_TX_TO_DISCON_HARD_ABORT in the 
                   structure rrc_tx_to_discon_reason_e_type under the feature flag 
                   FEATURE_RRC_HARD_ABORT.
11/23/06   kp      SMC suspend request & confirmation will be through call back function 
                   with RLC, Cleaning up code for command interface.
09/25/06   kp      Added support for sib_change_ind interface change
09/05/06   vm      Defined new enum type rrc_tx_to_discon_reason_e_type and also
                   added this enum type to rrc_initiate_cell_sel_ind_type structure. 
06/13/06   segu    Added support for DSAC feature implementation
05/02/06   sg      Added support for single to dual mode changes.
04/14/06   vr      Added failure reason in rrc_downlink_l2ack_ind_type structure.
03/15/06   vr      Changes to compile file in GSM PLT
02/21/06   vr      Support to send Interrat reason in suspend and resume commands to L1
2/15/06    sgk     Added activation time cancel cnf enum and structure to llc 
                   cmd cnf, added RRCLLC_CNAN_CONFIG_CNF_CFG_CANCELLED to the 
                   chan config status. 
02/07/06   vr      Changed feature name from FEATURE_SIB_SLEEP_OPTIMIZATION 
                   to FEATURE_INTER_SIB_SLEEP
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
12/21/05   vr      Support for FEATURE_SIB_SLEEP_OPTIMIZATION.Added a new command 
                   sib_sched_change_ind_req in l1_req
12/15/05   kc      Added RRC_SIB_MANUAL_SCAN type to list of sib-scan types
11/09/05   kc      Changes to setup PCCPCH based on L1 request
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search. Added
                   two new sib scan types - RRC_SIB_WTOW_AUTO_BPLMN_SCAN and 
                   RRC_SIB_WTOW_MANUAL_BPLMN_SCAN. Also added two new structures
                   rrc_csp_bplmn_suspend_req_type and rrc_csp_bplmn_suspend_cnf_type.
10/04/05   vr      Support for GTOW Packet Cell Change Order.Added a failure reason
                   in RRC_CONN_SETUP_IND
09/28/05   vm      Added support for redirection to another frequency + psc via
                   RRC Connection Setup message. Added new redirection result cause
                   RRC_CAMPED_ON_REDIRECTED_CELL in enum type rrc_redirect_cause_e_type
                   and added new variable psc in structure rrc_redirect_req_type.
07/21/05   ss      Added a new rrc_idt_complete_cnf for carrying information about one
                   domain completion
07/16/05   ss      Added a new l1_cmd_type for passing connected mode sync 
                   timers.
07/15/05   vm      Added support for not updating band information for 
                   inter-RAT handover, CCO, and reselections.
                   Removed band_pref from rrc_csp_acq_req_type and 
                   rrc_csp_camp_req_type.
06/02/05   vr      Added a new type rrc_irat_abort_ind_type to support
                   Interrat reselection aborts in idle mode
05/05/05   vm/bu   Added support for T-barred Feature. Added cell_bar_req to 
                   l1_req_cmd structure.
05/03/05   sm      Added rrc_abort_ciph_config_req_type, and 
                   rrc_abort_ciph_action_e_type to indicate LLC about action
                   required to revert back to old security configuration
04/14/05   da      Added new rrcllc_chan_cfg_status_e_type
                   RRCLLC_CHAN_CFG_FAILURE_BACK_TO_OLD_CONFIG to indicate that
                   going back to old config failed (in case of dch->dch failure)
04/04/05   bu      Added support for inter-RAT redirection feature
                   . Added new redirect cause types: RRC_REDIRECT_GSM_FAILURE and
                     RRC_REDIRECT_RESUME_FAILURE.
                   . Added new enum type for channel configuration
                     RRCLLC_CHAN_CFG_REASON_PHYCHAN_RESETUP.
03/04/05   kc      Added status to Change-Mode-Cnf to handle failures in IHO and CHO
                   resumption
03/03/05   vm      Added support for inter-frequency redirection feature.
                   Added variable wait_timer to rrc_redirect_req_type
                   structure, and enum type rrc_redirect_cause_e_type to 
                   rrc_redirect_cnf_type structure.
02/22/05   sk      Added a new cmd type "rlc_ul_purge_wm_ind_type ul_purge_wm_ind"
                   in typedef union l2_ul_req_cmd. 
02/09/05   kc      Added new SIB-Status type to handle multiple SIB-Mod-Ind
01/03/05   sk      Added a new channel config reason for pch/fach reconfiguration
12/03/04   da      Removed support for FEATURE_HOME_ZONE_SERVICE from this file and 
                   put in rrcmmif.h
12/02/04   da      Added Support for feature FEATURE_HOME_ZONE_SERVICE.
11/22/04   vm      Added support for feature FEATURE_GTOW_BACKGROUND_PLMN_SEARCH.
                   Added new sib scan types.
11/22/04   da      Added new cell update cause RRC_PHY_CHAN_FAILURE
11/02/04   svk     Added STOP_RB and CONTINUE_RB to rrc_rb_cfg_e_type.
10/28/04   vk      Changed type of gtow_ho_active variable from uint16 to boolean
10/28/04   vk      Changed name of dummy to gtow_ho_active in cell selected meas req cmd
09/15/04   sk      Added following two chan_confing_reason types in the enum
                   that are used for OOS handling.
                   RRCLLC_CHAN_CFG_REASON_OUT_OF_SERVICE,
                   RRCLLC_CHAN_CFG_REASON_RE_ENTERING_SERVICE 
08/05/04   bu      Added RRC_TRANSITION_TO_DISCONNECTED to rrc_cell_sel_cause_e_type.
                   Added rrc_proc_e_type to rrc_initiate_cell_sel_ind_type.
07/15/04   kc      Added bool flag to indicate whether bcch_modification_time
                   is present or not.
07/15/04   sk      Added new field in channel_config_reason enum.
06/01/04   bu      Added network_select_mode and band_pref in rrc_csp_acq_req_type
                   and rrc_csp_camp_req_type.
04/20/04   svk     Added transaction_id field in channel config cmd.
04/02/04   bu      Added additional fields in rrc_csp_camp_cnf_type for GTOW PCCO.
03/19/04   kc      Added internal interfaces for GtoW PCCO support.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
03/04/04   ttl     Added FEATURE_CELLPCH_URAPCH for code of Cell_PCH/URA_PCH.
12/24/03   svk     Added support for URA_UPDATE procedure.
12/03/03   sk      Support for Radio lonk failure. under feature RL_FAILURE_HANDLING
11/12/03   sk      Added ul data registration/unregistration commands for 
                   cell_PCH/URA_PCH support
11/11/03   kc      Added support for SRNS relocation internal structures.
10/14/03   kc      Added support for ciph_algo in RRC_CIPH_CONFIG_REQ to pass
                   ciphering algorithm received in HO to UTRAN command.
10/10/03   jh      Added support for RRC_CIPH_CONFIG_REQ in IDLE for GtoW
                   handovers.
08/04/03   vn      Added additional fields in rrc_rnti_update_req_type
                   to support RLC re-establishment.
05/29/03   vk      Fixed a minor Lint error in structure declaration
04/30/03   kc      Added struct for ho_ciph_cnf_type return status to support
                   Interrat Ciphering Handover from GtoW RAT.
04/24/03   rj      Added data structures for RRC_SIG_CONN_RELEASE_REQ and
                   RRC_SIG_CONN_RELEASE_CNF.
04/24/03   bu      Removed FEATURE_L1_ACT_TIME usage. Removed l1_setup_cmd_type
                   from rrc_llc_cmd_req_type.
04/10/03   bu      Defined rrc_lower_layer_deact_req_type.
03/24/03   ram     Fixed a syntax problem in a enum type when the feature
                   FEATURE_RRC_FDD_CELL_RESEL_IN_FACH was defined.
02/18/03   vn      Added directed frequency and Primary Scrambling Code in 
                   rrc_initiate_cell_sel_ind_type.
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to 
                   INTERRAT_HANDOVER_GTOW
02/14/03   kc      Added csp_acq_req and csp_acq_cnf internal structs to support
                   acquisition for GtoW Handover
01/15/03   sk      Changed the coding style of some structures in order to 
                   eliminate the lint errors.
11/19/02   vn      Added a new enum to rrc_new_cell_status_e_type to indicate
                   that Idle mode Cell Reselection is in progress.
11/08/02   xfg     Changed cause type to l1_stop_cause_enum_type in 
                   rrc_stop_lower_layer_req_type
11/04/02   xfg     Added cause field in rrc_stop_lower_layer_req_type to 
                   support deep sleep
09/14/02   vn      Changes for DCH->FACH transitions: Added new parameters - 
                   release_phychls and release_trchls in Channel Config Req,
                   new cell change types in RRC_CELL_CHANGE_REQ and new cell
                   sel cause type in RRC_INITIATE_CELL_SELECTION_IND.
09/20/02   bu      Changed rrc_sib_status_e_type, rrc_cell_change_status_e_type,
                   and rrc_channel_config_cnf_type to support cell reselection
                   in CELL_FACH state. Added rrcllc_chan_cfg_status_e_type. 
09/11/02   xfg     Changed rrc_llc_cmd_cnf_type to support suspend/resume
                   operations. Changes were made under inter-RAT feature defines
09/04/02   kc      Added ciphering related structures. suspend_req_type and
                   resume_req_type
08/21/02   vk      Added structure rrc_delete_meas_req_type that specifies
                   measurement identity to be deleted under FEATURE CM SUPPORTED
08/02/02   xfg     Added rrc_change_mode_req and rrc_change_mode_cnf for
                   supporting WCDMA to GSM handover dual-mode feature.
07/31/02   vk      Updated the format of previous comments
07/31/02   vk      Modified rrc_channel_config_cnf_type structure to contain
                   compressed mode status. This status is set to TRUE when l1_status
                   obtained is FAILURE and compressed_mode_status is set to TRUE.
                   Changes have been put under FEATURE CM SUPPORTED
07/09/02   vk      Defined param for CMAC_UL_BLOCK_RACH_TRAFFIC_REQ command
06/14/02   vk      Removed RRC_LL_CMAC_MEAS_CTRL_CNF command as it is directly
                   routed through dispatcher to Measurements
06/14/02   bu      Use mm_as_cell_access_status_T instead of 
                   rrc_cell_access_status_e_type for DUALMODE in 
                   rrc_cell_change_req_type.
05/14/02   ram     Added a new structure rrc_sib7_update_status_ind_type
                   to be used by the SIB procedure when sending the status
                   of a SIB7 update requested by another procedure.
                   Added procedure name field to rrc_get_all_sibs and
                   rrc_get_specific_sibs structures since they could
                   be used by more than one procedure.
05/17/02   vk      Added mac_ul_traffic_vol_config structure to specify the
                   Traffic Volume Measurements parameters that are sent from
                   RRC to MAC. Also added RRC_LL_CMAC_MEAS_CTRL_CNF in structure
                   rrc_ll_cnf_e_type to indicate to LLC the command to be posted
                   to RRC on getting confirmation indication from MAC. Added
                   mac_meas_ctrl_cnf structure to indicate the parameters
                   received from MAC for the confirmation to the Uplink
                   Traffic Volume Measurement Request. These changes have been
                   put under FEATURE TRAFFIC VOLUME MEAS
05/16/02   bu      Merged the following from cell reselection branch:
           bu      Defined command structures for RRC_CELL_RESELECTION_REQ and
                   RRC_CELL_RESELECTION_CNF.
05/14/02   xfg     Added dummy_signaling_enabled field in rrc_channel_config_req
                   for the loopback mode 2 dummy signaling test.
05/14/02   ram     Changed the rrc_state_change_ind_type structure to accomodate
                   multiple procedure names in the same command.
04/29/02   xfg     Added the new interface changes for Cheetah 1.0. All changes
                   are marked with FEATURE DUALMODE BASELINE.
03/13/02   bu      Featurized the act_cancel in l1_req_cmd union. If 
                   FEATURE_L1_ACT_TIME is defined include l1_setup_cmd_type in
                   rrc_llc_cmd_req_type.
03/12/02   bu      Changed the status field in rrc_rnti_update_cnf_type to
                   uecomdef_status_e_type from a boolean.
03/06/02   vn      Added Procedure ID field to rrc_rnti_update_req_type.
03/01/02   rj      Defined command structures for RRC_RNTI_UPDATE_REQ and
                   RRC_RNTI_UPDATE_CNF.
02/28/02   rj      Added additional field for destination state in structure
                   rrc_initiate_cell_sel_ind_type and added additional enums
                   for cell selection initiation causes.
02/20/02   vn      Added additional fields in NEW_CELL_IND command to indicate
                   if the cell is different from the previous cell AND if
                   RCE needs to be informed.
02/14/02   kc      Added a structure to define UTRAN Mobility Information Cnf type
02/07/02   xfg     Defined an enum type for loopback test mode:rrc_lb_test_mode_e_type
02/05/02   xfg     Added lb_test_mode field in rrc_channel_config_req_type for
                   RRC_CHANNEL_CONFIG_REQ
02/02/02   ram     Added a structure rrc_sib_change_ind_type for SIB change
                   notifications sent by the SIB procedure.
11/05/01   vk      Defined command structure for RRC_CPHY_MEAS_CTRL_CNF
10/22/01   vn      Added cell change type in Cell Change Request to 
                   indicate the various cell change causes.
10/17/01   rj      Added RRC_CELL_SEL_RB0_SETUP_FAILURE to enum 
                   rrc_cell_sel_cause_e_type.
10/15/01   rj      Defined command structure for RRC_INITIATE_CELL_SELECTION_IND.
08/31/01   vn      Frequency scan support - added l1_freq_scan_cmd_type to
                   rrc_llc_cmd_req_type and l1_freq_scan_cnf_type to 
                   rrc_llc_cmd_cnf_type.
08/29/01   vk      Defined rrc_cell_selected_meas_req_type structure for 
                   RRC_CELL_SELECTED_MEAS_REQ command
08/13/01   ram     Removed aset update structure and immedieate measurement structure
                   from the union l1_req_cmd since they were removed from the l1
                   files.
08/08/01   vn      Added field in Cell Change Request to pass on Cell Access Status.
08/06/01   rj      Updated command structure for RRC_CELL_UPDATE_INITIATE_REQ
                   and defined command structure for RRC_CELL_UPDATE_COMPLETE_CNF.
07/27/01   rj      Defined command structure for RRC_DISABLE_PREV_FREQ_IND.
07/20/01   rj      Defined command structures for RRC_INITIATE_UE_CAPABILITY_REQ
                   and RRC_CELL_UPDATE_INITIATE_REQ. Added a new parameter
                   rrc_channel_config_cnf_required to the rrc_channel_config_req_type
                   to indicate whether confirmation is required or not. Removed
                   parameter pdcp_seq_num from rrc_rb_config_type since PDCP sequence
                   number is always associated with a Radio Bearer.
06/08/01   rj      Modified L2 ACK status to uecomdef_status_e_type from
                   rrc_l2ack_status_e_type to match RLC declarations and deleted 
                   rrc_l2ack_status_e_type since it is no longer required.
04/29/01   rj      Added rrc_update_sibs_ind_type structure for RRC_UPDATE_SIBS_IND
                   command.
04/23/01   ram     Removed "previous_state" from structure rrc_new_state_ind_type
                   since it's no longer needed.
03/30/01   kmp     Put back union of Lower Layer commands into rrc_llc_cmd_cnf_type.
                   Added l1_cell_transition_cnf_type to this union.
03/26/01   kmp     Removed union from rrc_llc_cmd_cnf_type. Remove #include
                   rrcasn1.h.
03/23/01   ram     Changed dl_sdu_type from uint8 to uint32 since the PDU type
                   defined by ASN.1 has been changed to 4 digits in the Dec. 2000
                   version of the RRC Spec.
03/16/01   vn      Changes due to LED ASN1: rrc_int_cell_id_type. Made changes for              
                   adding Cell Transition Req in LLC cmd req and cnf structures.
03/15/01   upen    Created typedef unions l1_req_cmd ,l2_dl_req_cmd ,l2_ul_req_cmd.
01/30/01   rj      uint8 for lc_id in rrc_downlink_l2ack_ind_type and 
                   rrc_downlink_sdu_ind_type is replaced with rlc_lc_id_type
                   from uecomdef.h
01/30/01   vn      Added new cell status to rrc_cell_change_cnf_type.
01/25/01   vn      Added a new enum - rrc_camped_on_status_e_type and renamed
                   rrc_cell_camping_status_e_type for clarity.
01/23/01   kmp     Updated for new commands necessary for LLC to handle all
                   lower layer requests and confirmations.
01/17/00   vn      Added data structures rrc_cell_change_req_type, 
                   rrc_cell_change_cnf_type and rrc_new_cell_ind_type to support
                   Cell change mechanism.
01/09/01   ram     Added two new structures rrc_new_state_ind_type and rrc_state
                   _change_ind_type to support RRC's state change manager. Removed
                   rrc_state_change_req_type that is not used anymore.
01/04/01   ram     Fixed tab problem in previous checkin. Added 2001 copyright.
01/04/01   ram     Changed name rrc_sib_event_resp_type to rrc_sib_event_cnf_type to
                   be consistent with the command naming convention. Corrected
                   misplaced comments for the rrc_sib_event_cnf_type.
12/22/00   rj      Added a new field sdu_type to RRC_DOWNLINK_SDU_IND command and 
                   added data structures for new internal commands required by 
                   the RRC Connection Establishment procedure.
12/20/00   ram     Added a new command definition rrc_get_specific_sibs_type. This
                   is used to request a specific set of SIBs to be collected by
                   the SIB procedure.
12/15/00   ram     Changed name rrc_get_all_sibs_type to rrc_get_all_sibs_req_type
                   to be consistent with other RRC command type definitions.
                   Added additional parameters PLMN and Cell ids and the
                   index into the sib database where the SIBs for that cell
                   are stored.
12/13/00   rj      Added dl_sdu_length field to rrc_downlink_sdu_ind_type.
12/13/00   kmp     Added #include "rrcdata.h". Updated the 
                   rrc_im_channel_setup_req_type and rrc_im_channel_setup_cnf_type
                   to include an rrc_proc_e_type. Also added and rb_id_type
                   to the rrc_im_channel_setup_cnf_type. Broke up the config_status
                   in rrc_cm_channel_config_cnf into fields for RLC, MAC and L1.
                   Changed name of rlc/mac/l1_success to rlc/mac/l1_status.
12/11/00   kmp     Updated the IM channel structures and added those for 
                   connected mode.
                   Changed the rrc_im_channel_cnf structure types from boolean
                   to uecomdef_status_e_type.
12/08/00   ram     Moved internal command definitions from rrcllc.h to this file.
                   Included uecomdef.h which defines rb_id_type.
12/07/00   rj      rrc_downlink_sdu_ind_type and rrc_downlink_l2ack_ind_type
                   are moved from rrclcm.h into this file.
12/06/00   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

#include "comdef.h"
#include "rrcdata.h"
#include "uecomdef.h"
#include "rrcmmif.h"
#include "l1rrcif.h"
#include "l1task.h"
#include "wcdmamvsif.h"
#include "sys_plmn_selection.h"
#include "gstk_exp_v.h"
#include "lte_rrc_irat_msg.h"
#include "wcdma_rrc_msg.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Command structures for all internal commands included here */

/*---------------------------------------*/
/*                                       */
/*     Structure for RRC_GET_ALL_SIBS    */
/*     Command.                          */
/*                                       */
/*---------------------------------------*/

/* Substitute of ALLOC_ASN1ARRAY2*/
#define ALLOC_ASN1ARRAY2(pctxt,pseqof,type) ALLOC_ASN1ARRAY1(pctxt,pseqof,type); \
if((pseqof)->elem == 0) {ERR_FATAL("Memory not available",0,0,0);}


/* enum type for sib reading to distinguing different handling of GET_SPECIFIC_SIBS for
   normal, background manual vs automatic */
typedef enum
{
  RRC_SIB_NORMAL,
  RRC_SIB_AUTO_BPLMN_SCAN,  
  RRC_SIB_WTOW_AUTO_BPLMN_SCAN,
  RRC_SIB_WTOW_MANUAL_BPLMN_SCAN,
  RRC_SIB_MANUAL_BPLMN_SCAN,
  RRC_SIB_MANUAL_SCAN,
  RRC_SIB_CELL_ID_READ_IN_DCH_SCAN,
  RRC_SIB_READ_FROM_SIB_CONTAINER
}rrc_sib_scan_e_type;


#define RRC_MAX_SIB_CONT_LEN 0x1000 /*4096 Bytes*/
typedef struct
{
  uint16 cont_len;
  uint8 *cont_ptr;  /*Use a pointer */
}rrc_System_Information_Container_type;

/*---------------------------------------*/
/*                                       */
/*   Structure for RRC_GET_SPECIFIC_SIBS */
/*   Command.                            */
/*                                       */
/*---------------------------------------*/


/* Use to send a command to SIB procedure to collect specific SIBs for a
cell */
typedef struct
{
  rrc_proc_e_type  procedure;  /* The procedure requesting the SIBs */
  uint32 freq;            /* Frequency of the cell */
  uint32 scrambling_code; /* Scrambling Code of the cell */
  boolean for_serv_cell;  /* Set to TRUE if this is for the
                          current serving cell. Set to FALSE if
                          it's for a neighbor cell. */
  boolean for_csg_cell;   /* set to TRUE for ASF and femto specific reqs*/
  boolean sib_req[RRC_TOT_SIBS];
                                /* Set each SIB that is needed
                                 to TRUE. Other SIBs should be set
                                 to false. Use "rrc_SIB_Type" from
                                 rrcasn1.h to fill up this array. */
  rrc_sib_scan_e_type  sib_scan_type;
                                
  rrc_System_Information_Container_type  *sys_info_cont_ptr;
}rrc_get_specific_sibs_req_type;

/*---------------------------------------*/
/*                                       */
/*  Structure for RRC_GET_ALL_SIBS_CNF,  */
/*  and RRC_GET_SPECIFIC_SIBS_CNF        */
/*  Commands.                            */
/*                                       */
/*---------------------------------------*/
/* Enumerated type of status to be sent to the calling
entity */
typedef enum
{
  SIB_EVENT_SUCCEEDED,                 /* SIB event succeeded */
  SIB_EVENT_FAILED_LOWER_LAYERS,       /* SIB event failed due to failure to configure lower layers */
  SIB_EVENT_FAILED_INCOMPATIBLE_STATE, /* SIB event failed due to incompatible RRC state */
  SIB_EVENT_FAILED_TIMEOUT,            /* SIB event failed due to SIB timeout */
  SIB_EVENT_FAILED_INVALID_SIBS,       /* SIB event failed due to Invalid SIBs */
  SIB_EVENT_FAILED_INVALID_EVENT,      /* SIB received invalid event */
  SIB_EVENT_VALID_DUPLICATE_UPDATE_IND,/* SIB UPDATE event failed due to new UPDATE EVENT */
  SIB_EVENT_FAILED_FPLMN,              /* SIB event failed due to F(Forbidden)-PLMN  */
  SIB_EVENT_FAILED_CELL_BAR,           /* SIB event aborted due to cell bar */
  SIB_EVENT_FAILED_FREQ_BAR,           /* SIB event aborted due to frequency bar */
  SIB_EVENT_FAILED_CSG_NW,             /* SIB event aborted due to non white CSG*/
  SIB_EVENT_FAILED_FB_LA,              /*SIB event aborted due to forbidden LA*/
  SIB_EVENT_FAILED_SCRITERIA,          /* SIB event aborted due to S-criteria Failure*/
  SIB_EVENT_FAILED_PLMN_MISMATCH,      /*SIB event aborted due to PLMN mismatch */
  SIB_EVENT_FAILED_L2_ACK_WAIT,
  SIB_EVENT_FAILED_OTHER               /* SIB event failed due to any other reason */
  //May need more
}rrc_sib_status_e_type;

typedef enum
{
 RRC_RNTI_UPDATE,                 /* Use the new RNTI */          
 RRC_RNTI_DELETE,                 /* Delete the current RNTI */
 RRC_RNTI_NOCHANGE                /* No change in RNTI status */
}rrc_rnti_cmd_status_e_type;

typedef enum
{
  RRC_FAILURE_NONE,
  RRC_MAX_RACH_ATTEMPTS,
  RRC_CONNECTION_REJECT,
  RRC_CONN_FAILURE_OTHER
}rrc_conn_setup_failure_e_type;

/* Structure to be used when sending an event to confirm the
reception of SIBs. */
typedef struct
{ 
  rrc_proc_e_type procedure;    /* The procedure that requested the SIBs */
  rrc_sib_status_e_type status; /* Status of the SIB event-success or failure*/
  /* The following information is valid only if the event succeeded */
  uint32 sib_index_for_cell;       /* Index into SIB database where SIBs for
                                   this cell are stored. */
  rrc_plmn_identity_type plmn_id;  /* PLMN Id of the cell */
  rrc_int_cell_id_type  cell_id;   /* CELL Id of the cell */
  rrc_int_csg_id_type       csg_id;     /* CSG Id of the cell */
  boolean is_csg;                  /* above field is valid if its set to TRUE */
  uint32 freq;                     /* Frequency of the cell */
  uint32 scrambling_code;          /* Scrambling Code of the cell */

  /* This Boolean will indicate whether SIB5bis is scheduled in this cell or not
   *  Value = TRUE means SIB5bis is scheduled in this cell
   *  Value = FLASE means SIB5 is scheduled in this cell
   *  Currently SIB fills this Boolean for only below CSP Scan types  
   * RRC_CSP_PLMN_LIST_SCAN corresponding SIB scan type à RRC_SIB_MANUAL_SCAN
   * RRC_CSP_WTOW_BPLMN_MANUAL_SCAN corresponding SIB scan type à RRC_SIB_WTOW_MANUAL_BPLMN_SCAN
   * RRC_CSP_GTOW_BPLMN_MANUAL_SCAN corresponding SIB scan type à RRC_SIB_MANUAL_BPLMN_SCAN
   * So, CSP to look at this Boolean only for above scan types
   */
  boolean is_sib5bis_scheduled; 
  boolean is_sib_read_from_container;/* To know whether SIBs read from container or over OTA*/
  uint8 lac[RRC_MAX_GSMMAP_LAC_LENGTH];
}rrc_sib_event_cnf_type;


/* This enumerates the status of UTRAN directed redirection */
typedef enum
{
  RRC_REDIRECT_SUCCESS,
  RRC_REDIRECT_FAILURE
}rrc_redirect_status_e_type;

/* This enumerates the cause of success/failure for UTRAN directed redirection */
typedef enum
{
  /* UE was able to camp on the frequency designated by UTRAN */
  RRC_CAMPED_ON_REDIRECTED_FREQ,

  /* UE was able to camp on the frequency and PSC designated by UTRAN */
  RRC_CAMPED_ON_REDIRECTED_CELL,

  /* UE was not able to camp on the redirected frequency, but was able to
   * camp on another frequency different from the last camped frequency on
   * which RRC Connection Request was sent
   */
  RRC_CAMPED_ON_DIFFERENT_FREQ,

  /* UE was not able to camp on the redirected frequency or any other frequency
   * before the expiry of the wait timer. After timer expiry, UE was able to
   * camp on the last camped frequency on which RRC Connection Request was sent
   */
  RRC_CAMPED_ON_ORIGINAL_FREQ,

  /* UE failed to camp on any frequency and hence is OOS */
  RRC_FAILED_TO_CAMP,

  /* UE received inter-frequency redirection which is not supported */
  RRC_FREQ_REDIRECTION_NOT_SUPPORTED,

  /* UE received interRAT redirection which is not supported */
  RRC_RAT_REDIRECTION_NOT_SUPPORTED,                           

  /* UE received redirection which is not supported */
  RRC_REDIRECTION_NOT_SUPPORTED,

  /* RCE passed inconsistent data to CSP or RRC was not in Connecting State
   * when CSP received RRC_REDIRECT_REQ from RCE
   */
  RRC_RCE_INCOMPATIBLE,
  
  /* UE was not able to camp on GSM */
  RRC_REDIRECT_GSM_FAILURE,

  /* UE was not able to camp on GSM and WCDMA could not be resumed */
  RRC_REDIRECT_RESUME_FAILURE
}rrc_redirect_cause_e_type;



/* Enumerated type for selecting type of re-direction */
typedef enum
{
  RRC_FREQ_REDIRECTION,                    /* Frequency redirection */
  RRC_INTER_SYSTEM_GSM,                   /* Inter-system redirection to GSM */
  RRC_INTER_SYSTEM_LTE,                   /* Inter-system redirection to LTE */
}rrc_redirect_e_type;

typedef struct
{
   boolean uarfcn_UL_present;              /* Boolean to indicate whether 
                                              Downlink UARFCN is included */
   uint16 uarfcn_UL;                       /* Uplink UTRA Absolute Radio
                                              Frequency Channel Number */
   uint16 uarfcn_DL;                       /* Downlink UTRA Absolute Radio
                                              Frequency Channel Number */
}rrc_freq_info_type;

typedef enum
{
  RRC_UE_CAPABILITY_ORIGINATOR_UE,        /* UE originated UE Update Capability
                                           procedure */
  RRC_UE_CAPABILITY_ORIGINATOR_UTRAN      /* UTRAN originated UE Update 
                                             Capability procedure  */

  
/*This is used by UECI procedure to differentiate between COUEC procedure and normal processing.*/

  ,RRC_UE_CAPABILITY_ORIGINATOR_COUEC      /* COUEC originated UE Update Capability procedure*/

  ,RRC_UE_CAPABILITY_ORIGINATOR_NONE
} rrc_ue_capability_originator_e_type;

typedef enum
{
  RRC_CELL_RESELECTION,                   /* Cell Reselection */
  RRC_PERIODIC_CELL_UPDATE,               /* Periodic Cell Update */
  RRC_UL_DATA_TRANSMISSION,               /* UL Data Transmission */
  RRC_PAGING_RESPONSE,                    /* Paging Response */
  RRC_RADIO_LINK_FAILURE,                 /* Radio Link Failure */
  RRC_REENTERED_SERVICE_AREA,             /* Re-entered Service area */
  RRC_UNRECOVERABLE_ERROR,                 /* Unrecoverable error */
  RRC_PHY_CHAN_FAILURE,                    /* Physical Channel Failure */
  RRC_LOSSY_PAGE_RECEPTION,
  RRC_PCH_TO_FACH_TRANSITION_REQ,  /* ePCH to eFACH state transition Request */
  RRC_CU_CAUSE_NONE
} rrc_cell_update_cause_e_type;

/* URA update causes */

typedef enum
{
  RRC_URA_RESELECTION,                      /* Change of URA id */
  RRC_PERIODIC_URA_UPDATE                 /* Periodic URA update */
}rrc_ura_update_cause_e_type;


typedef enum
{
  RRC_CELL_SEL_RADIO_LINK_FAILURE,        /* Cell selection due to radio link
                                             failure */
  RRC_CELL_SEL_RB0_SETUP_FAILURE,         /* Cell selection due to radio link
                                             failure */
  RRC_OUT_OF_SERVICE_AREA_IND,            /* Cell Selection due to Out of 
                                             service area */
  RRC_TIMER_T316_EXPIRED,                 /* Timer T316 expired */

  RRC_TRANSITION_FROM_DCH,                /* This is when cell selection is to 
                                           * be triggered going out from CELL_DCH to
                                           * any other connected mode state.
                                           */
  RRC_TRANSITION_TO_DISCONNECTED,         /* This is when some procedure wants to 
                                           * take UE to IDLE and initiate cell selection
                                           */
  RRC_TRANSITION_FROM_FACH,     
  RRC_CELL_SEL_ON_MDSP_HALT,
  RRC_CELL_SEL_MAX_CAUSE                 
}rrc_cell_sel_cause_e_type;

/* enum type for loopback test mode */
typedef enum
{
  LB_TEST_MODE_NONE,
  LB_TEST_MODE_1,
  LB_TEST_MODE_2
} rrc_lb_test_mode_e_type;

/* enum type for re-establishing radio bearers */
typedef enum
{
  RRC_RE_EST_RB_2,
  RRC_RE_EST_NON_RB_2_UM_AM,
  RRC_RE_EST_RB_INVALID
}rrc_rlc_re_est_for_srns_rb_e_type;

/* enum type for ciphering config */
typedef enum
{
  CIPH_CONFIG_WCDMA,
  CIPH_CONFIG_GTOW,
  CIPH_CONFIG_LTOW,
  CIPH_CONFIG_CS,
  CIPH_CONFIG_CS_PLUS_PS,
/*Used to Update Up link keys to L1*/
  CIPH_CONFIG_UL_KEYS,
/*Used to Update Down link keys to L1 */
  CIPH_CONFIG_DL_KEYS,
  CIPH_CONFIG_MAX
} rrc_ciph_config_e_type;

typedef enum
{
  /* Revert to old set of ciphering keys */
  RRC_ABORT_CIPH_ACTION_L1KEY_ONLY = 0x1,
  /* ABORT Cipher config sent to RLC */
  RRC_ABORT_CIPH_ACTION_RLC_ONLY = 0x2,
  RRC_ABORT_CIPH_ACTION_L1KEY_RLC_RESUME = 0x3
} rrc_abort_ciph_action_e_type;

#define HSDPA_CAT_INCL 0x01
#define HSUPA_CAT_INCL 0x02

/*Following structure declares the UE capabilities that will
    be maintained throughout the RRC Connection*/
typedef struct
{
  uint8 is_valid_mask;
  uint8 hsdpa_category;
  uint8 hsupa_category;	
} rrc_ue_cap_update_during_rrc_conn_type;

/*Following enum is used by COUEC procedure to notify the status of UE 
    capability change request to requesting procedure */
typedef enum
{
  RRC_UE_CAP_CHANGE_ACCEPTED,
  RRC_UE_CAP_CHANGE_REFUSED,
  RRC_UE_CAP_CHANGE_UNSUPPORTED,
  RRC_UE_CAP_CHANGE_NONE
}rrc_couec_cnf_e_type;



/*Structure for invoking RRC_COUEC_REQ*/
typedef struct 
{
  rrc_proc_e_type proc_id;
  rrc_ue_cap_update_during_rrc_conn_type ue_cap_requested;  	
}rrc_couec_req_type;

/*Structure for the response RRC_COUEC_CNF*/
typedef struct
{
  rrc_proc_e_type proc_id;
  rrc_couec_cnf_e_type ueci_cnf;
}rrc_couec_cnf_type;


/*Following enum is used by LLC procedure to notify the status of 
CIPHER_CONFIG_REQ sent by SMC */
typedef enum
{
  RRC_CIPHER_CONFIG_SUCCESS,
  RRC_CIPHER_CONFIG_FAIL_DIDNT_CONFIG_LL
}rrc_cipher_config_cnf_e_type;

/*Structure for the response RRC_CIPHER_CONFIG_CNF*/
typedef struct
{
  rrc_cipher_config_cnf_e_type cause;
}rrc_cipher_config_cnf_type;


/*---------------------------------------------------------------------------
  Type for storing event data for RRC_DOWNLINK_SDU_IND_EVENT.
---------------------------------------------------------------------------*/
typedef struct 
{
  rlc_lc_id_type lc_id;                    /* Unique identifier for the 
                                              Downlink RLC logical channel.*/
  uint32 dl_sdu_length;                    /* Length of downlink sdu 
                                              in bytes */
  uint32 dl_sdu_type;                      /* SDU type received over the air.
                                              This is same as PDU num in 
                                              ASN1 */
                                             
  uint8 *dl_sdu;                           /* Pointer to the Downlink SDU. */

  void *decoded_msg;                       /* Pointer to the decoded 
                                              Downlink OTA message. MSCH messages */ 
} rrc_downlink_sdu_ind_type;

/*---------------------------------------------------------------------------
  Type for storing event data for RRC_DOWNLINK_L2ACK_IND_EVENT.
---------------------------------------------------------------------------*/
typedef struct 
{
  rlc_lc_id_type lc_id;                         /* Unique identifier for the 
                                                   Downlink RLC logical channel.*/  
  uint8 mui;                                    /* Message Unit Identifier. */
  uecomdef_status_e_type status;                /* Indicates success or failure of
                                                   transmission of the SDU. */                                        
  rlc_sdu_tx_fail_reason_e_type failure_reason; /* Failure reason(for failure cases) */
} rrc_downlink_l2ack_ind_type;


/* This structure defines the parameters needed to RRC_REDIRECT_REQ
   command */
typedef struct
{
  rrc_proc_e_type proc_id;
  rrc_redirect_e_type choice;              /* Represents whether Freq or
                                              System re-direction */
  union
  { 
    rrc_freq_info_type freq;               /* Re-direction details */
  rrc_rr_gsm_cell_list_type GSM_cell_info_list;
    lte_rrc_wcdma_redir_earfcn_list_s EUTRA_target_cell_info;
  }u;

  rrc_scr_code_type psc;

  uint32 wait_time;
}rrc_redirect_req_type;

/* This structure defines the parameters needed to RRC_REDIRECT_CNF
   command */

typedef struct
{
  /* Frequency redirection succeeded or failed */
  rrc_redirect_status_e_type status;

  /* Frequency redirection success/failure cause */
  rrc_redirect_cause_e_type  cause;
}rrc_redirect_cnf_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_new_state_ind_type

This is sent to the RRC state change manager. This contains
the new RRC state and the previous RRC state. When the State
Change Manager receives this information it sends state change
notification to all the procedures that have registered to be
notified of this state transition.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_state_e_type new_state;      /* The newly changed state */
/* This has been added , so that boothe internal commands and the 
 * RRC cmds have the same padding bits added by ARM. 
 * As the size of the padding bits is determined by the highest data type
 * present in the structure in ARM.*/
  uint64 dummy2;
}rrc_new_state_ind_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_state_change_ind_type

This is the structure for the command sent by the RRC state
change manager to other RRC procedures. This is sent to notify
procedures of a particular state change. Only procedures that
register for this type of state change will receive the notice.
The Dispatcher will look at the procedure name in this structure
and call the appropriate procedure to process this command.

Due to the high number of procedures that need to get state
change indication, one command is used to notify multiple 
procedures. The Dispatcher will send the same command to multiple
procedures if needed.
--------------------------------------------------------------------*/
typedef struct
{
  uint32 num_procedures; /* Number of procedures that should get
                         the command. */
  rrc_proc_e_type  procedure[RRC_TOT_SCN]; 
                         /* Name of each procedure            */
  rrc_state_e_type previous_state; /* RRC's previous state    */ 
  rrc_state_e_type new_state;      /* The newly changed state */
} rrc_state_change_ind_type;


/* Enumerated types for Cell Camped on Status */
typedef enum
{
  RRC_CAMPED_ON,
  RRC_NOT_CAMPED_ON
}rrc_camped_on_status_e_type;


/* Enumerated types for New Cell status */
typedef enum
{
  /* A new cell has been selected */
  RRC_NEW_CELL_SELECTED,
  /* The cell has been lost and NO new cell selected */
  RRC_CELL_LOSS_NO_NEW_CELL,

  /* The cell is currently un-available as Idle mode cell
   * reselection is in progress. Note that this status type
   * is to be actually used only when Cell Reselection in FACH
   * is NOT enabled.
   */
//  RRC_CELL_LOSS_IDLE_RESEL_IN_PROG,

  RRC_MAX_NEW_CELL_STATUS_TYPE
}rrc_new_cell_status_e_type;

/* Enumerates the various causes for cell change */
typedef enum
{
  RRC_CHANGE_CELL_SELECTION,
  RRC_CHANGE_CELL_RESELECTION,
  RRC_CHANGE_RETURN_TO_SERVICE_AREA,
  RRC_CHANGE_CELL_DCH_TO_CELL_FACH,
  RRC_CHANGE_CELL_DCH_TO_URA_PCH,
  RRC_CHANGE_CELL_DCH_TO_CELL_PCH,
  RRC_CHANGE_CELL_FACH_TO_CELL_FACH,
  RRC_CHANGE_CELL_FACH_TO_CELL_PCH,
  RRC_CHANGE_CELL_FACH_TO_URA_PCH
}rrc_cell_change_e_type;

/* Enumerates Cell Change status types */
typedef enum
{
  /* Cell change was successfully accomplished */
  RRC_CELL_CHANGE_SUCCESS,
  /* Cell change was successful but the freq and scr_code yielded a cell 
   * of a PLMN different than one asked 
   */
  RRC_CELL_CHANGE_SUCCESS_DIFF_PLMN,
  /* Cell change was successful but the freq and scr_code yielded a cell 
   * of a Cell ID different than one asked 
   */
  RRC_CELL_CHANGE_SUCCESS_DIFF_CELLID,
  /* Cell change failed due to failure to configure lower layers */
  RRC_CELL_CHANGE_FAILURE_LOWER_LAYERS,
  /* Cell change failed due to Incompatible RRC state */
  RRC_CELL_CHANGE_FAILURE_INCOMPATIBLE_STATE,
  /* Cell change failed, due to any other reason */
  RRC_CELL_CHANGE_FAILURE_OTHER
  //More later
}rrc_cell_change_status_e_type;      

/* Enumerates transition to disconnected reason which is used by CSP.
 * Other reasons should be added as and when they are needed by CSP.
 */
typedef enum
{
  /* This cause is set when an Emergency Call is attempted by the User */
  RRC_TX_TO_DISCON_EM_CALL,                

  /* This cause is set when a RL failure happens the T314/T315 = 0 */
  RRC_TX_TO_DISCON_RL_FAILURE_T314_T315_0, 

  /* This cause is set when hard abort flag is received in RRC_ABORT_REQ from NAS */
  RRC_TX_TO_DISCON_HARD_ABORT,
  /* This cause is GSM redirection in connection reject with dual mode disabled*/
  RRC_TX_TO_DISCON_UNSUPPORTED_REDIRECTION,
  /* This cause is set when the cause is none of the above */
  RRC_TX_TO_DISCON_OTHER
} rrc_tx_to_discon_reason_e_type;    

/* This structure defines parameters needed for RRC_CELL_CHANGE_REQ */
typedef struct
{
  /* Indicates the procedure requesting Cell Change */
  rrc_proc_e_type   req_proc_id;

  /* Indicates Cell Camping status for the new cell */ 
  rrc_new_cell_status_e_type    new_cell_camp_status;

  mm_as_cell_access_status_T     new_cell_access;

  /* TRUE  if DSAC is present
     FALSE if DSAC is not present */
  boolean dsac_present;

  /* store the new cs celll access status */
  mm_as_cell_access_status_T     new_cs_cell_access;

  /* store the new ps celll access status */
  mm_as_cell_access_status_T     new_ps_cell_access; 

  /* TRUE  if PPAC is present
     FALSE if PPAC is not present */
  boolean ppac_present;

  /* Stores the new cell CS reg status */
  boolean                        new_cell_cs_reg_allowed;

  /* Storest the new cell PS reg status */
  boolean                        new_cell_ps_reg_allowed;

  /* Stores the new cell paging response status */
  mm_as_page_rsp_e_type          new_cell_page_rsp_allowed;

  /* The Cell ID as received in SIB 3 */
  rrc_int_cell_id_type          cell_id;

  /* CSG ID as received in SIB 3 */
  rrc_int_csg_id_type               csg_id;

  /* The PLMN ID as received in MIB */
  rrc_plmn_identity_type      plmn_id;

  /* Scrambling Code of the Camped on Cell */
  rrc_scr_code_type           scr_code;

  /* Frequency of the Camped on Cell */
  rrc_freq_type               freq;

  /* Type of cell change - determines CCM behavior on a Cell Change */
  rrc_cell_change_e_type  cell_change_type;

  boolean is_valid_nw_sharing;
  rrc_plmn_identity_type common_plmn_id;

}rrc_cell_change_req_type;


/* This structure defines parameters needed for RRC_CELL_CHANGE_CNF */
typedef struct
{
  /* Indicates the procedure for whom Cell Change Confirmation 
   * is intended.
   */
  rrc_proc_e_type   req_proc_id;

  /* Indicates the status of Cell Change. */
  rrc_cell_change_status_e_type  cell_change_status;

  /* Indicates New Cell status  */ 
  rrc_new_cell_status_e_type    new_cell_status;

  /* The Cell ID as received in SIB 3 */
  rrc_int_cell_id_type          cell_id;

  /* The PLMN ID as received in MIB */
  rrc_plmn_identity_type        plmn_id;

  /* Scrambling Code of the Camped on Cell */
  rrc_scr_code_type         scr_code;

  /* Frequency of the Camped on Cell */
  rrc_freq_type             freq;

}rrc_cell_change_cnf_type;


/* This structure defines parameters needed for RRC_NEW_CELL_IND */
typedef struct
{
  /* The Procedure ID that must not be informed of New Cell 
   * Note that this is not the only procedure not to be informed. The
   * procedures to be informed are hardcoded but if the procedure included
   * here is one of those, it should not be informed.
   */
  rrc_proc_e_type   no_ind_proc_id;

  /* This flag indicates if the newly camped on cell is the same as 
   * the one camped on before
   */
  boolean   cell_same_as_before;

  /* This flag indicates if the RCE procedure needs to be sent the 
   * New Cell Indication.
   */
  boolean   inform_procedure_rce;

  /* Indicates New Cell Status */ 
  rrc_new_cell_status_e_type    new_cell_status;

}rrc_new_cell_ind_type;


/*-------------------------------------------------------------------
ENUMERATION: rrc_rb_cfg_e_type

This enum indicates what type of action is to be performed on a per
radio bearer basis.
--------------------------------------------------------------------*/
typedef enum 
{
  NO_ACTION_ON_RB,
  ESTABLISH_RB,
  RELEASE_RB,
  RECONFIG_RB,
  RECONFIG_RB_OTA
} rrc_rb_cfg_e_type;

#define MODIFY_RB_CFG         0x0001 /*RB is modified through OTA*/
#define STOP_RB_CFG             0x0002 /*RB is Stopped*/
#define CONTINUE_RB_CFG     0x0004 /*RB is continue*/
/*-------------------------------------------------------------------
ENUMERATION: rrc_cmd_dest_e_type

This enum indicates which lower layer the command is destined for.
--------------------------------------------------------------------*/
typedef enum 
{
  NO_LAYER,
  LAYER1_PHY,
  LAYER2_MAC_UL,
  LAYER2_MAC_DL,
  LAYER2_RLC_UL,
  LAYER2_RLC_DL
} rrc_cmd_dest_e_type;
/* Below #defines are added for logging purpose only */
#define rrc_cmd_dest_e_type_value0 NO_LAYER
#define rrc_cmd_dest_e_type_value1 LAYER1_PHY
#define rrc_cmd_dest_e_type_value2 LAYER2_MAC_UL
#define rrc_cmd_dest_e_type_value3 LAYER2_MAC_DL
#define rrc_cmd_dest_e_type_value4 LAYER2_RLC_UL
#define rrc_cmd_dest_e_type_value5 LAYER2_RLC_DL

/*-------------------------------------------------------------------
STRUCTURE: rrc_rb_config_type

This structure defines the parameters needed to setup/release/reconfig
radio bearers.
--------------------------------------------------------------------*/
typedef struct
{
  uint8                rb_id;
  rrc_rb_cfg_e_type         rb_config;
  uint16 ul_cfg_mask; /*bit mask for UL Actions*/
  uint16 dl_cfg_mask; /*bit mask for DL Actions*/


} rrc_rb_config_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_llc_cmd_req_type

This structure defines the parameters needed to configure the lower
layers including L1, MAC, and RLC. This encompasses all commands to
the lower layers and provides a single command for all lower layer
configurations.
--------------------------------------------------------------------*/
typedef union
{
  l1_cap_cmd_type                 cap;
  l1_acq_cmd_type                 acq;
  l1_freq_scan_cmd_type           freq_scan;
  l1_cell_parm_cmd_type           cell_parm;
  l1_immediate_cell_eval_cmd_type immed_cell_eval;
  l1_meas_cmd_type                meas;
  l1_hard_handoff_req_cmd_type    hho;
  l1_sfn_cmd_type                 sfn;
  l1_cfn_cmd_type                 cfn;
  l1_bch_schedule_cmd_type        bch_sched;
  l1_drx_cmd_type                 drx;
  l1_ciphering_key_cmd_type       cipher_key;
  l1_cell_selec_cmd_type          cell_sel;
  l1_intg_key_cmd_type            intg_key;
  l1_intg_key_mauc_cmd_type       key_mauc;
  l1_immediate_reselec_cmd_type   immed_rsel;
  l1_cell_reselect_rsp_cmd_type   rsel_rsp;
  l1_act_time_cancel_cmd_type     act_cancel;
  l1_cell_transition_cmd_type     cell_transition;
  l1_cell_bar_cmd_type            cell_bar_req;
  l1_upd_dl_sync_parms_cmd_type   connected_timer_info;
  l1_suspend_cmd_type             suspend_req;
  l1_resume_cmd_type              resume_req;
  l1_sib_sched_change_ind_type    sib_sched_change_ind_req;
  l1_upd_ota_parms_ind_cmd_type upd_ota_parms;
} l1_req_cmd;

typedef union
{
  mac_dl_config_type             mac_config_info;
  rlc_dl_tm_config_type          dl_tm_config;
  rlc_dl_um_config_type          dl_um_config;
  rlc_dl_cipher_config_type      dl_cipher_config;
  rlc_dl_hfn_type                dl_hfn;
} l2_dl_req_cmd;

typedef union
{
  mac_ul_config_type              mac_config_info;
  rlc_ul_tm_config_type           ul_tm_config;
  rlc_ul_um_config_type           ul_um_config;
  rlc_am_config_type              am_config;
  rlc_ul_cipher_config_type       ul_cipher_config;
  rlc_ul_resume_type              ul_resume;
  rlc_ul_hfn_type                 ul_hfn;
  rlc_ul_register_for_data_ind_type ul_reg_data_ind;
  rlc_ul_unregister_for_data_ind_type ul_unreg_data_ind;
  mac_ul_traffic_meas_config_type* mac_ul_traffic_vol_config;
  boolean                          block_rach;
  rlc_ul_purge_wm_ind_type ul_purge_wm_ind;
} l2_ul_req_cmd;

typedef struct
{
  rrc_proc_e_type       procedure;
  rrc_cmd_dest_e_type   cmd_dest;
  l1_cmd_hdr_type        l1_cmd;
  wcdma_l2_dl_cmd_hdr_type     l2_dl_cmd;
  wcdma_l2_ul_cmd_hdr_type     l2_ul_cmd;
  l1_req_cmd           * l1_req;
  l2_dl_req_cmd        * l2_dl_req;
  l2_ul_req_cmd        * l2_ul_req;
} rrc_llc_cmd_req_type;

/*-------------------------------------------------------------------
ENUM: rrc_ll_cnf_e_type

This enum defines the internal RRC confirmation types that the Lower
Layer Controller (LLC) uses to inform other procdures of command 
confirmations or event indications from L1, MAC and RLC.
--------------------------------------------------------------------*/
typedef enum
{
  /* Layer 1 cnfs and inds */
  RRC_LL_CPHY_ACQ_CNF,
  RRC_LL_CPHY_ERROR_IND,
  RRC_LL_CPHY_CELL_SELECTION_CNF,
  RRC_LL_CPHY_CELL_RESELECTION_IND,
  RRC_LL_CPHY_IMMEDIATE_CELL_EVAL_CNF,
  RRC_LL_CPHY_INTER_FREQ_BCH_CNF,
  RRC_LL_CPHY_SET_SFN_CNF,
  RRC_LL_CPHY_SET_CFN_CNF,
  RRC_LL_CPHY_MEASUREMENT_IND,
  RRC_LL_CPHY_CAPABILITY_CNF,
  RRC_LL_CPHY_INTEGRITY_KEY_MAUC_CNF,
  RRC_LL_CPHY_PHYCHAN_ESTABLISHED_IND,
  RRC_LL_CPHY_IN_SYNC_IND,
  RRC_LL_CPHY_OUT_OF_SYNC_IND,
  RRC_LL_CPHY_CELL_RESELECTION_FAIL_IND,
  RRC_LL_CPHY_IMMEDIATE_MEAS_CNF,
  RRC_LL_CPHY_IMMEDIATE_RESELECTION_CNF,
  RRC_LL_CPHY_CELL_TRANSITION_CNF,
  RRC_LL_CPHY_FREQ_SCAN_CNF,
  RRC_LL_CPHY_MEAS_CTRL_CNF,
  RRC_LL_CPHY_SUSPEND_CNF,
  RRC_LL_CPHY_RESUME_CNF,
   RRC_LL_CPHY_ACT_CANC_CNF,

  /* MAC cnfs */
  RRC_LL_CMAC_UL_HFN_CNF,
  RRC_LL_CMAC_DL_HFN_CNF,
  RRC_LL_CMAC_UL_TRAFFIC_IND,
  RRC_LL_CMAC_STATUS_IND,

  /* RLC cnfs */
  RRC_LL_CRLC_SUSPEND_CNF,
  RRC_LL_CRLC_STATUS_IND,
  RRC_LL_CRLC_UL_HFN_CNF,
  RRC_LL_CRLC_DL_HFN_CNF 

}rrc_ll_cnf_e_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_llc_cmd_cnf_type

This structure defines the parameters needed to configure the lower
layers including L1, MAC, and RLC. This encompasses all commands to
the lower layers and provides a single command for all lower layer
configurations.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type       procedure;
  rrc_ll_cnf_e_type     ll_cnf;

  union
  {
    l1_acq_cnf_type                 acq_cnf;
    l1_freq_scan_cnf_type           freq_scan_cnf;
    l1_cell_select_cnf_type         cell_sel_cnf;
    l1_immediate_cell_eval_cnf_type immed_cell_eval_cnf;
    l1_capability_cnf_type          cap_cnf;
    l1_intg_key_mauc_cnf_type       key_mauc_cnf;
    l1_immediate_reselec_cnf_type   immed_rsel_cnf;
    l1_immediate_meas_cnf_type      immed_meas_cnf;
    l1_error_ind_type               err_ind;
    l1_cell_reselec_ind_type        cell_rsel_ind;
    l1_measurement_ind_type         meas_ind;
    l1_out_sync_ind_type            out_sync_ind;
    l1_in_sync_ind_type             in_sync_ind;
    l1_phychan_est_ind_type         phy_est_ind;
    l1_cell_transition_cnf_type     cell_tran_cnf;
    l1_meas_ctrl_cnf_type           l1_meas_ctrl_cnf;  
    l1_suspend_cnf_type             suspend_cnf;
    l1_resume_cnf_type              resume_cnf;
    l1_act_time_cancel_cnf_type     act_cancel_cnf;          
  } cnf;

} rrc_llc_cmd_cnf_type;

/*-------------------------------------------------------------------
ENUM: rrcllc_chan_cfg_reason_e_type
This enum defines the reasons of the Channel configuration
--------------------------------------------------------------------*/
typedef enum
{
  RRCLLC_CHAN_CFG_REASON_NONE,             /* Channels config reason none */
  RRCLLC_CHAN_CFG_REASON_RL_FAILURE,     /* Channel config reason RL failure */
  RRCLLC_CHAN_CFG_REASON_RLC_CONFIG_ONLY,     /* Channel config reason RLC config only */
  RRCLLC_CHAN_CFG_REASON_PCH_TO_FACH,     /* Channel config for pch2fach transition */
  RRCLLC_CHAN_CFG_REASON_OUT_OF_SERVICE, /* Channel config for out of service service */
  RRCLLC_CHAN_CFG_REASON_RE_ENTERING_SERVICE, /* Channel config for re-entering service */
  RRCLLC_CHAN_CFG_REASON_PHYCHAN_RESETUP, /* Channel config for appropriate physical channels
                                           * to be resetup after WTOG failure
                                           */
  RRCLLC_CHAN_CFG_REASON_RECONFIG_PCH_FACH,
  RRCLLC_CHAN_CFG_REASON_SIB_READ_IN_DCH, /* Channel config for setting up S-BCCH in CELL DCH */
  RRCLLC_CHAN_CFG_REASON_WITHIN_FACH,
  RRCLLC_CHAN_CFG_REASON_FACH_TO_PCH,
  RRCLLC_CHAN_CFG_REASON_HS_FACH_DEDICATED_OPS, /* Channel config for re-entering service */
  RRCLLC_CHAN_CFG_REASON_HS_FACH_COMMON_OPS, /* Channel config for re-entering service */
  RRCLLC_CHAN_CFG_EPCH_HRNTI_ONLY
  /*add the new reasons below */
} rrcllc_chan_cfg_reason_e_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_channel_config_req_type

This structure defines the parameters needed to setup Idle mode
an connected mode channels.
--------------------------------------------------------------------*/
typedef struct
{
  /* Only 2 procedures can setup up idle mode channels */
  rrc_proc_e_type       procedure;

  /* Useful for associating a channel config confirm with a channel
  config request */
  uint32                transaction_id;                   

  /* Indicates if a state changed is required or not */
  boolean               rrc_state_change_required;

 /* Indicates if a confirmation is required or not */
  boolean               rrc_channel_config_cnf_required;

  /* The next state */
  rrc_state_e_type      next_state;

  /* The number of RBs to configure */
  int                   num_rb_to_config;

  /* The maximum number of idle mode RBs to configure is 4 */
  /* The maximum number of signalling RBs to configure is 4 */
  rrc_rb_config_type    rb[MAX_RB];

  /* Loopback Test Mode */
  rrc_lb_test_mode_e_type lb_test_mode;

  /* Loopback Mode Signaling Test Mode */
  boolean               dummy_signaling_enabled;

  /* Indicate if all Physical and Transport channels need to be
   * released, leaving the logical channels intact.
   */
  boolean               release_phychls;
  boolean               release_trchls;
  /*reason for channel configuration. It is useful for some conditional
  operations */
  rrcllc_chan_cfg_reason_e_type       chan_config_reason;

  boolean               is_oc_set;
} rrc_channel_config_req_type;



/*-------------------------------------------------------------------
ENUM: rrcllc_chan_cfg_status_e_type
This enum defines the status of the Channel configuration
--------------------------------------------------------------------*/
typedef enum
{
  RRCLLC_CHAN_CFG_SUCCESS,             /* Channels configured successfully */
  RRCLLC_CHAN_CFG_FAILURE_PHY_CHL,     /* Channel configuration failed due to 
                                          physical channel failue */
  RRCLLC_CHAN_CFG_FAILURE_LOG_CHL,     /* Channel configuration failed due to
                                          logical channel configuration failure */
  RRCLLC_CHAN_CFG_FAILURE_INCOMPATIBLE_STATE,  /* Channel configuration failed due to
                                          incompatible RRC state */
  RRCLLC_CHAN_CFG_FAILURE_BACK_TO_OLD_CONFIG,  /* Channel configuration failed due to*/
  RRCLLC_CHAN_CFG_FAILURE_CFG_CANCELLED, 
  RRCLLC_CHAN_CFG_FAILURE_OTHER        /* Any other LLC internal reason for failure */
} rrcllc_chan_cfg_status_e_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_channel_config_cnf_type

This structure indicates whether the channel config request was 
successful or not. Only the first radio bearer id in the list is
returned if more than one radio bearer was configured.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type               procedure;
  
  /* Useful for associating a channel config confirm with a channel
  config request. This will currently be used by the SIB procedure. 
  */
  uint32                        transaction_id;  

  rrcllc_chan_cfg_status_e_type chan_cfg_status;
  /* TRUE means Failure is due to compressed mode */
  boolean compressed_mode_status;

  boolean inter_freq_hho_status;
} rrc_channel_config_cnf_type;

/*-------------------------------------------------------------------
ENUM: rrcsib_update_sibs_cause_e_type
This enum defines the status of the Channel configuration
--------------------------------------------------------------------*/
typedef enum
{
  RRCSIB_DEFERRED_SIB_READ,             /* Channels configured successfully */
  RRCSIB_SFN_UPDATE,
  RRCSIB_REAL_SIB_MOD,
  RRCSIB_SIX_HR_TMR_EXPIRY,
  RRCSIB_CSFB_CALL_CANCL,
  RRCSIB_OTHER
} rrcsib_update_sibs_cause_e_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_update_sibs_ind_type

This structure indicates defines the parameters for RRC_UPDATE_SIBS_IND
command. 
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type         procedure;
  uint32                  mib_value_tag;
  boolean                 real_sib_mod;
rrcsib_update_sibs_cause_e_type cause;

} rrc_update_sibs_ind_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_initiate_ue_cap_req_type

This structure indicates defines the parameters for 
RRC_INITIATE_UE_CAPABILITY_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{
  rrc_ue_capability_originator_e_type originator; /* Originator of UE 
                                                     Update Capability
                                                     procedure */
  uint8 transaction_id;                          /* Transacation Id
                                                     received through
                                                     UE Capability 
                                                     Enquiry.*/
} rrc_initiate_ue_cap_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_initiate_cu_req_type

This structure indicates defines the parameters for 
RRC_CELL_UPDATE_INITIATE_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{
  rrc_cell_update_cause_e_type cause;  /* Cause for the Cell Update to
                                          perform */
  boolean cu_complete_cnf_required;    /* This indicates whether 
                                          Cell Update initiated proc
                                          needs completed confirmation
                                          or not.*/
  rrc_proc_e_type         procedure;   /* RRC procedure that initiates Cell
                                          Update*/
} rrc_initiate_cu_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_initiate_uu_req_type

This structure indicates defines the parameters for 
RRC_URA_UPDATE_INITIATE_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{
  rrc_ura_update_cause_e_type cause;  /* Cause for the URA Update to
                                          perform */
  boolean uu_complete_cnf_required;    /* This indicates whether 
                                          URA Update initiated proc
                                          needs completed confirmation
                                          or not.*/
  rrc_proc_e_type         procedure;   /* RRC procedure that initiates URA
                                          Update*/
} rrc_initiate_uu_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_cu_complete_cnf_type

This structure indicates defines the parameters for 
RRC_CELL_UPDATE_COMPLETE_CNF command. 

--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type         procedure;   /* RRC procedure waiting for 
                                          Cell Update to complete */
} rrc_cu_complete_cnf_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_rnti_update_req

This structure indicates defines the parameters for 
RRC_RNTI_UPDATE_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{  
  rrc_proc_e_type  procedure;               /* The procedure ID of the 
                                                sender */
  boolean cnf_required;                     /* Indicates the procedure
                                             needs confirmation */
  rrc_rnti_cmd_status_e_type crnti_action;  /* Action for C-RNTI*/
  rrc_int_c_rnti_type  crnti;               /* New C-RNTI value */

  rrc_rnti_cmd_status_e_type urnti_action;  /* Action for U-RNTI*/

  rrc_int_c_rnti_type  urnti;               /* New C-RNTI value */

  boolean rlc_re_establish_srb;             /* Indicates if RLC entities for
                                             * RB2, RB3 and RB4 need to be 
                                             * re-established.
                                             */

  boolean rlc_re_establish_rab;             /* Indicates if all User plane 
                                             * RBs need to be re-established.
                                             */
} rrc_rnti_update_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_rnti_update_cnf

This structure indicates defines the parameters for 
RRC_RNTI_UPDATE_CNF command. 

--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type  procedure;          /* RRC procedure waiting for 
                                          LLC to configure MAC */
  uecomdef_status_e_type status;       /* Indicates status  of RNTI 
                                          configuration */

} rrc_rnti_update_cnf_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_cell_reselection_req

This structure indicates defines the parameters for 
RRC_CELL_RESELECTION_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type  procedure;          /* RRC procedure waiting for 
                                        LLC to configure lower layers */
  /* Freq. of the cell to be reselected */
  rrc_freq_type  freq;

  /* Scrambling Code of the Cell */
  rrc_scr_code_type   scr_code;

  /* The PLMN ID  */
  rrc_plmn_identity_type      plmn_id;

  /* Cell ID */
  rrc_int_cell_id_type        cell_id;

} rrc_cell_reselection_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_cell_reselection_cnf

This structure indicates defines the parameters for 
RRC_CELL_RESELECTION_CNF command. 

--------------------------------------------------------------------*/
typedef struct
{
  uecomdef_status_e_type status;       /* Indicates status  of cell reselection 
                                          configuration */

} rrc_cell_reselection_cnf_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_disable_prev_freq_type

This structure indicates defines the parameters for 
RRC_DISABLE_PREV_FREQ_IND command. 

--------------------------------------------------------------------*/
typedef struct
{
  uint32 wait_time;                               /* Duration for which
                                                     previous freq. 
                                                     needs to be excluded.*/
} rrc_disable_prev_freq_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_cell_selected_meas_req_type

This structure indicates the parameters for 
RRC_CELL_SELECTED_MEAS_REQ command. 

---------------------------------------------------------------------*/
typedef struct
{
  boolean gtow_ho_active;
    /* No parameter */
} rrc_cell_selected_meas_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_utran_mob_info_cnf_type

This structure indicates the parameters for
RRC_UTRAN_MOBILITY_INFO_CNF command.

---------------------------------------------------------------------*/
typedef struct
{
  uint8   transaction_id;
} rrc_utran_mob_info_cnf_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_initiate_cell_sel_ind_type

This structure indicates the parameters for 
RRC_INITIATE_CELL_SELECTION_IND command. 

---------------------------------------------------------------------*/
typedef struct
{
  rrc_cell_sel_cause_e_type  cause;     /* Cell Selection Trigger
                                                   cause */
  rrc_proc_e_type            proc;      /* Procedure */
  rrc_state_e_type dest_state;          /* When Cell selection
                                           successfully camped, it needs 
                                           destination state and then
                                           send RRC_NEW_CELL_IND */
  boolean        dest_freq_present;    /* Indicates if Destination frequency is present */
  rrc_freq_type  dest_freq;            /* Destination Frequency - is 
                                        * relevant only in DCH->FACH transitions
                                        */
  boolean            dest_psc_present;    /* Indicates if Destination PSC is present */
  rrc_scr_code_type  dest_psc;            /* Destination Pri Scrambling Code - is 
                                           * relevant only in DCH->FACH transitions
                                           */
  rrc_tx_to_discon_reason_e_type tx_to_discon_reason; /* Enumerates transition to disconnected reason 
                                                       * which is used by CSP */

} rrc_initiate_cell_sel_ind_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_sib_change_ind_type

This is the structure for the command sent by the SIB
procedure to other RRC procedures. This is sent to notify
procedures of a particular SIB change. Only procedures that
register for this type of sib change will receive the notice.
The Dispatcher will look at the procedure name in this structure
and call the appropriate procedure to process this command.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type  procedure;  /* The procedure that should receive the command */
  rrc_sib_change_mask_type sib_change_mask;   /* Bit mask of all the sibs which have changed */ 
} rrc_sib_change_ind_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_mode_change_ind_type

This is the structure for the command sent by the MCM module
to other RRC procedures. This is sent to notify
procedures of a particular RRC mode change. Only procedures that
register for this change will receive the notice.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type  procedure;  /* The procedure that should receive the command */
  rrc_mode_e_type  from_mode;  /* The mode from which transition occurs */
  rrc_mode_e_type  to_mode;    /* The mode to which transition occurs */
} rrc_mode_change_ind_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_stop_lower_layer_req_type

This is the structure for the command sent by the MCM module
to LLC to request LLC to stop WCDMA mode in the lower layers. 
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type  procedure;  /* The procedure that should receive the command */
  l1_stop_cause_enum_type cause; /* The reason for stopping WCDMA mode */
} rrc_stop_lower_layer_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_stop_lower_layer_cnf_type

This is the structure for the command sent by the LLC module
to MCM to confirm the completion of lower layer WCDMA deactivation.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type  procedure;  /* The procedure that should receive the command */
} rrc_stop_lower_layer_cnf_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_sib7_update_status_ind_type

This is the structure for the command sent by the SIB
procedure to other RRC procedures to notify
procedures of SIB7 update status.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type  procedure;  /* The procedure that should receive the command */
  uecomdef_status_e_type status; /*Indicates success or failure of updating sib7*/
} rrc_sib7_update_status_ind_type;

/*---------------------------------------------------------------------------
  Type to suspend uplink data transmission on a single 
  Unacknowledged or Acknowledged Mode logical channel. 
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* lc_id */
  uint8  n;                             /* SN 'n' until which RLC does not suspend */
} rrc_ul_suspend_info_type;

/*---------------------------------------------------------------------------
  Type to suspend uplink data transmission on one or more Unacknowledged  
  or Acknowledged Mode logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of UM or AM channels to suspend */
  rrc_ul_suspend_info_type   suspend[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Suspend Information for each    */
                                        /* uplink logical channel          */
} rrc_ul_suspend_req_type;

/*---------------------------------------------------------------------------
  Type to resume uplink data transmission on one or more 
  unacknowledged or acknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of UM/AM ch. to resume 
                       transmission on                   */
  uint8  channel[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Uplink logical channels to resume */
} rrc_ul_resume_req_type;


/* Enum to identify the reason for change mode request */
typedef enum {
  INTERRAT_REASON_OTHER,
  INTERRAT_RESELECTION,
  INTERRAT_REDIRECTION,
  INTERRAT_OOS,
  INTERRAT_BPLMN,
  INTERRAT_IHO,
  INTERRAT_CCO,
  INTERRAT_DSIM_TUNEAWAY,
  INTERRAT_REASON_NONE
}rrc_change_mode_reason_e_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_change_mode_req_type

This is the structure for the command sent by the IHO or CSP to request
MCM to change RRC mode to the requested mode.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type  procedure; /* The procedure that sends the command */
  rrc_mode_e_type  mode; /* the mode the sender is requested to change to */
  rrc_change_mode_reason_e_type reason; /* reason for the change */
} rrc_change_mode_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_change_mode_cnf_type

This is the structure for the command sent by the MCM to respond to
RRC_CHANGE_MODE_CNF after it completes the mode change.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_proc_e_type  procedure; /* the destination procedure */
  rrc_mode_e_type  mode;      /* the current RRC mode */
  boolean          status;    /* status flag to indicate success/failure */ 
  wl1_common_fail_enum_type fail_type;
} rrc_change_mode_cnf_type;

typedef struct
{
  uecomdef_status_e_type acq_status;
} rrc_csp_acq_cnf_type;

typedef struct
{
  uint16  freq;                   /* frequency id*/
  uint16  scr_code;               /* scrambling code*/
  boolean pn_offset_present;      /* TRUE means "pn_offset" value is present.  
                                     FALSE otherwise. */
  uint32  pn_pos;                 /* If present, PN offset of the cell in 
                                     chipx1 units. */
  boolean sttd_indicator_present; /* TRUE means "sttd_indicator" value is 
                                     present.  FALSE otherwise. */
  boolean sttd_indicator;         /* If present, TRUE means the cell is using Space 
                                     Time Transmit Diversity. ** FALSE means it is not.  
                                     Described in TS 25.331, 10.3.6.78. */
  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

} rrc_csp_acq_req_type;

typedef struct
{
  uecomdef_status_e_type status;  /*status to indicate if ciph config
                                    was successful or not            */
}rrc_ho_ciph_cnf_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_delete_meas_req_type

This is the structure for the command sent by the physical channel
reconfiguration procedure to the measurement procedure.
--------------------------------------------------------------------*/

typedef struct
{
  uint16 meas_id; /* Measurement to be deleted */
} rrc_delete_meas_req_type;

/* This is the structure used to send the lower_layer_deact_req to LLC */
typedef struct
{
  sys_stop_mode_reason_e_type deact_reason;
} rrc_lower_layer_deact_req_type;

/* This structure defines the parameters needed to RRC_SIG_CONN_RELEASE_REQ
   command */
typedef struct
{
  rrc_cn_domain_identity_e_type cn_domain_id;   

  /* This indicates whether rrc proc needs confirmation or not.*/
  boolean sig_rel_complete_cnf_required; 

  /* proc_id which is sending sig conn release to SCRR 
   * The same proc_id is used for sending response back to proc */
  rrc_proc_e_type rrc_proc;
}rrc_sig_conn_release_req_type;

/* This structure defines the parameters needed to RRC_SIG_CONN_RELEASE_CNF
   command */
typedef struct
{
  /* Indicated success or failure of sig. connection release */
  uecomdef_status_e_type status;       
  
  /* stores proc_id to which sig conn release cnf needs to be sent */
  rrc_proc_e_type rrc_proc;
}rrc_sig_conn_release_cnf_type;

typedef struct
{
  rrc_rlc_re_est_for_srns_rb_e_type rb_type;       /* rb type to re_est rb2 or others     */
  uint32                            hfn_srb;       /* hfn to re-est rb2                   */
  uint32                            hfn_urb;       /* hfn to re-est rb2                   */  
  uecomdef_wcdma_cipher_algo_e_type            ciph_algo_srb; /* ciphering algo after re-est for srb */
  uecomdef_wcdma_cipher_algo_e_type            ciph_algo_urb; /* ciphering algo after re-est for urb */
}rrc_re_est_req_type;

typedef struct
{
  rrc_proc_e_type                   rrc_proc;  /* originating procedure id       */
  rrc_rlc_re_est_for_srns_rb_e_type rb_type;   /* rb type to re_est rb2 or others*/
  boolean                           cnf_reqd;  /* to indicate if cnf is reqd     */
}rrc_re_est_rlc_srns_req_type;

typedef struct
{
  rrc_proc_e_type        rrc_proc;     /* Procedure name to route the cnf       */
  uecomdef_status_e_type status;       /* Indicated success or failure of 
                                          of re-establishment                   */
}rrc_re_est_rlc_srns_cnf_type;
  
typedef struct
{
  rrc_re_est_req_type             am_config;             /* am configuration    */
  rrc_re_est_req_type             ul_um_config;          /* ul-um configuration */
  rrc_re_est_req_type             dl_um_config;          /* dl-um configuration */
}rrc_llc_re_est_req_type;

/* This command indicates the ciphering cause to LLC, whether the cause
   to activate is due to ciphering starting or due to handover        */
typedef struct
{
  rrc_ciph_config_e_type config_cause;
}rrc_ciph_config_req_type;

typedef struct
{
  rrc_abort_ciph_action_e_type abort_action;
} rrc_abort_ciph_config_req_type;

typedef struct
{
  byte  ciphering_key[RRC_MAX_CIPHERING_KEY_LENGTH];
  byte  integrity_key[RRC_MAX_INTEGRITY_KEY_LENGTH];
}rrciho_security_keys_type;

/* This command used by HO_CIPH_REQ to indicate ciphering parameters to be
   passed to SMC procedure from IHO procedure                             */
typedef struct
{
  uecomdef_wcdma_cipher_algo_e_type ciph_algo;
  rrc_cn_domain_identity_e_type  domain_id;
  rrc_ciph_config_e_type   config_cause;   
  rrciho_security_keys_type  security_keys[RRC_MAX_CN_DOMAINS];
}rrc_ho_ciph_req_type;

typedef struct
{
   uecomdef_status_e_type        camping_status;
   rrc_interrat_reject_reason_T  failure_cause;

   /* Time ( in Sec ) when the same cell should be considered */
   uint32                        time_to_reselect;

   /* Thresholds to be applied to Ec/No measurements */
   int16                         squalmin;

   int16                         srxlevmin;

   /* PLMN Id of the WCDMA cell */
   PLMN_id_T                     PLMN;

   /* LAC */
   uint8                         forbidden_lac[2];

} rrc_csp_camp_cnf_type;

typedef struct
{
  uint16                 freq;            /* Frequency */
  uint16                 scr_code;        /* Scrambling code */
  rrc_plmn_identity_type requested_plmn;  /* PLMN Id */
  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

} rrc_csp_camp_req_type;

typedef struct
{
  uecomdef_status_e_type status; /* Indicates if Connection was setup successfully or not */
  rrc_conn_setup_failure_e_type conn_failure_reason; /* Indicates why the connection failed */
} rrc_conn_setup_ind_type;


typedef struct
{
  rrc_proc_e_type procedure; /* Interrat procedure that is currently active */
} rrc_irat_abort_ind_type;

typedef struct
{

  rrc_proc_e_type procedure; 
  rrc_cn_domain_identity_e_type domain_id;   
}rrc_idt_complete_cnf;

typedef enum
{
  BPLMN_SUSPEND_REASON_NONE,
  BPLMN_SUSPEND_REASON_BCCH_MODIFICATION,
  BPLMN_SUSPEND_REASON_CELL_RESELECTION,
  BPLMN_SUSPEND_REASON_UPLINK_DATA,
  BPLMN_SUSPEND_REASON_REGISTRATION,
  BPLMN_SUSPEND_REASON_OTHER
} rrc_bplmn_suspend_cause_e_type;


typedef struct
{
  rrc_proc_e_type procedure; 
  rrc_bplmn_suspend_cause_e_type cause;
} rrc_csp_bplmn_suspend_req_type;

typedef struct
{
  rrc_proc_e_type        procedure;
  uecomdef_status_e_type status;
} rrc_csp_bplmn_suspend_cnf_type;

typedef struct
{ 
  rrc_proc_e_type procedure;
   /* No fields */
} rrc_mode_status_ind_type;

typedef struct
{
  /* MAX number of cells per freq to return back to GSTK */
  uint8 num_cells;
  /* Report type - Intra, Inter or IRAT */
  gstk_nmr_utran_qualifier_enum_type report_type;
  /*only used for GSTK purpose */
  uint32 *ref_id;
}rrc_gstk_utran_nmr_info_req_type;


typedef struct
{
  /* This is set to TRUE if RRC is waiting for L1 to release lock*/
  boolean reset_l1_cnf_needed;
}rrc_reset_l1_req_type;

typedef enum
{
  RRC_CS_DOMAIN,
  RRC_PS_DOMAIN,
  RRC_CS_PLUS_PS_DOMAIN
}rrc_domain_type;

typedef struct
{
  rrc_domain_type    domain;
}rrc_open_domain_ind_type;


/* ----------------------------------------------------------------------------
   ENUM:      MCFG_REFRESH_TYPE_E_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  RRC_MCFG_REFRESH_TYPE_MIN  = 0,  
  RRC_MCFG_REFRESH_TYPE_SLOT = RRC_MCFG_REFRESH_TYPE_MIN,   /**< refresh type for slot specific items */
  RRC_MCFG_REFRESH_TYPE_SUBS,                           /**< refresh type for subscription specific items */
  RRC_MCFG_REFRESH_TYPE_SLOT_N_SUBS,                    /**< refresh type for slot & sub specific items */
  RRC_MCFG_REFRESH_TYPE_MAX  = RRC_MCFG_REFRESH_TYPE_SLOT_N_SUBS,  
} rrc_mcfg_refresh_type_e_type;  

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_REFRESH_INDEX_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  RRC_MCFG_REFRESH_INDEX_MIN = 0,
  RRC_MCFG_REFRESH_INDEX_0 = RRC_MCFG_REFRESH_INDEX_MIN,   /**< index 0 */
  RRC_MCFG_REFRESH_INDEX_1,                            /**< index 1 */
  RRC_MCFG_REFRESH_INDEX_2,                            /**< index 2 */
  RRC_MCFG_REFRESH_INDEX_MAX = RRC_MCFG_REFRESH_INDEX_2,
  RRC_MCFG_REFRESH_INDEX_INVALID = 0x7F,
} rrc_mcfg_refresh_index_type;


typedef struct
{
  rrc_mcfg_refresh_type_e_type type;            /**< Refresh for slot or sub specific items */
  rrc_mcfg_refresh_index_type  slot_index;      /**< Slot index for this refresh */
  rrc_mcfg_refresh_index_type  sub_index;       /**< Subscription index for this refresh */
  uint32                   reference_id;    /**< Need to return this reference ID in mcfg_refresh_done() */
} rrc_mcfg_refresh_info_s_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */




/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION   RRC_GSTK_GET_UTRAN_NMR_INFO

DESCRIPTION

 This function is invoked by GSTK in it's context when it needs some UTRAN 
 network measurment information.  RRC will post an internal command and 
 process the request.

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/
void rrc_gstk_get_utran_nmr_req
(
  uint8 num_cells, gstk_nmr_utran_qualifier_enum_type report_type, uint32 *ref_id
  ,sys_modem_as_id_e_type as_id
);

#endif /* RRCINTIF_H */

