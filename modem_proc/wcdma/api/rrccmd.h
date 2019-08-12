#ifndef RRCCMD_H
#define RRCCMD_H
/*===========================================================================
                      R R C    C O M M A N D S

DESCRIPTION

  This module contains external definitions for the
  command queues used by RRC task. This file should be
  included by any external module sending commands to RRC's
  command queues.


Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccmd.h_v   1.58   18 Jul 2002 19:55:04   vkhanna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rrccmd.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   sp      Made changes for Mav debug snapshot
01/12/16   sn      Changes for FR30881
10/16/15   as      Added code to support LTA durting activation time
10/07/15   vi      Changes for force DRX and skip idlemode measurements based on QMI request 
01/29/15   bc      Changes to crash the UE if there is no response to STATE_CHANGE_IND from LLC 
                   for more than 5 seconds.
01/20/15   bc      Changes to crash the UE if there is no response to ACTIVATION_IND 
                   from NAS during WTOG CCO.
11/14/14   sr      Code reduction to remove GET_ALL_SIBS handling
11/10/14   sp      Made changes to save procedure substates in crash debug info
07/17/14   sr      Changes for NV refresh FR
06/11/14   bc      Made changes to reduce DCH to FACH cell selection time.
04/25/14   vi      Made changes to handle LTE deprioritization request in W-RRC
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
11/05/13   rmn     Changes for displaying Partial PLMN Search Results during Manual Search
12/06/13   vi      Added new command handling RRC_MUTLIMODE_SUBS_CHGD_IND from NAS to set the correct AS ID
11/27/13   sp      Made changes to add nv support and to save time ticks for 
                   rr to/from rrc mm to/from rrc and to add msg_type for crash debug ota changes
09/16/13   sn      G2W TA feature changes
07/08/13   sn      Changes for LTE CSG feature
06/27/13   gv      Added code to update AS ID change due to mode pref change when sim is not available
05/22/13   vi      Added new command for SGLTE
04/23/13   sg      Removed featurization flags in api directory
04/09/13   gv      Fixed compilation issue
12/25/12   db      Made changes to requeue transition to disconnected triggered by CU when there is no response
12/07/12   sn      CSG feature changes
06/20/11   kp      Added code to trigger close_session_req for HFN 
                   mismatch recovery.
10/04/12   pm      Removed unnecessary comments
09/21/12   gv      Fixed compilation issue when FEATURE_QTA is defined
09/10/12   gv      Added support for new interface between WRRC and TDS RRC
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/25/12   pm      Added _get_cm_cmd_buf() & rrc_put_cm_cmd() function prototypes to fix compilation warnings.
05/24/12   sks     Added support for QCHAT specific dynamic DRX.
04/12/12   as      Added code to support feature NB/WB handover support
04/09/12   gv      Added code to prevent CPU hogging during G2W redirection
03/27/12   sks     Added support for abort handling during WTOL PS handover.
02/23/12   sks     Added support for WTOL PSHO.
02/15/12   pm      Defeaturization changes
12/21/11   xl      Roll back to #16
12/21/11   xl      Merge from MPSS.NI.1.1
12/13/11   sks     Added support for LTOW SRVCC.
12/12/11   gv      Added code to support free floating of tasks
12/05/11   pm      Added code to support cell id request by CM in DCH state"
11/18/11   mn      Made changes to disable reselection to LTE after attach reject from network
                   with cause 14.
10/31/11   sks     Added support for FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB.
07/29/11   ad      Made changes for CMI Phase 4
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/21/11   rl      Added code to support L2W CGI 
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/10/11   sks     Made changes to implement 5s guard timer for LTOW redirection.
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/09/11   rl      Fixed compiler warning
02/03/11   ad      Added changes to refeaturize the API files
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/17/11   ad      Mde changes to fix compilation errors in RRC SW Decoupling
01/13/11   ad      Added changes removing the defeaturization for FEATURE_MODEM_MBMS
01/12/11   ad      Added changes for RRC SW decoupling
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/22/10   sks     Added support for handling EM call during WRLF.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/07/10   rm      Added interface strucures for 3GPP CSFB
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
09/22/10   kp      Added code for Qchat timer that delay HSPA status indication
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
09/09/10   rm      Made changes to hold dedicated priorities during OOS
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/15/10   gkg     Made changes to put changes for "Multiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
11/27/09   gkg     RRC changes required to integrate QChat Porting Kit Optimizations 
                   into AMSS. The changes are under feature FEATURE_QCHAT.
11/02/09   sks     Renamed RRC_WTOG_BPLMN_ABORT_GUARD_TIMER_EXPIRED_IND to
                   RRC_WTOG_ABORT_GUARD_TIMER_EXPIRED_IND.
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
05/08/09   ss      Updated Copyright Information
03/16/09   kp      Added structure rrc_cipher_config_cnf for existing cmd
                   i.d RRC_CIPHER_CONFIG_CNF.
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
12/10/08   ps      Made changes to not allocate memory from static buffers
                   only for the timer expiry Ind, it should be allocated 
                   from heap only
06/11/08   sk      Added RRC_WTOG_BPLMN_ABORT_GUARD_TIMER_EXPIRED_IND and 
                   RRC_CONTINUE_STOP_MODE_REQ
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7. Removed feature
                   FEATURE TFCS CONTROL ENABLE, FEATURE CELLPCH URAPCH
08/25/08   sks     Added RRC_EXIT_FROM_PWR_SAVE for key press.
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
03/07/08   vr      Added RRC_CPHY_MBMS_SERVING_CELL_CHANGE_IND and RRC_CPHY_MCCH_WEAK_IND. Removed
                   RRC_CPHY_MBMS_BEST_CELL_IND command since it was obsolete
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
02/07/07   da      Added support for FEATURE_PLI_NMR_UTRAN
12/21/07   vr      Adding commands for FEATURE_MBMS
09/11/07   kp      Code clean up, as RLC suspend is done as a function call.
09/10/07   pk      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
05/25/07   vg      added support for FEATURE_CGPS and FEATURE_CGPS_UMTS_CP_WCDMA.
05/23/07   pk      Added a new command RRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND
                   to indicate the expiry of the full frequency scan timer associated
                   with the manual PLMN search. 
05/03/07   vm      Added fix for CR 118055. Added another argument to the function
                   rrc_free_embedded_buf()
04/19/07   ps      Made modifications to use a array of static blocks for the
                   allocation of internal commands
03/28/07   kp      Added a new command RRC_BCCH_MODIFICATION_TIMER_EXPIRED_IND
                   for indicating BCCH Modification timer expiry.
03/16/07   vm      Added command numbers in the comments for faster debugging.
03/04/07   vm      Added support for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2.
02/08/07   kp      Removed extra hop of command interface between SMC & LLC,
                   for RRC_RESUME_RLC_RB_REQ & RRC_UPDATE_UL_CIPHER_KEYS_REQ.
                   Now SMC directly places these request to L2 task & L1 respectively
                   bypassing LLC.
01/04/07   kp      Added a new command for Updating Uplink Ciphering Keys
12/11/06   ps      To allocate 10 blocks of internal cmd buffer statically and
                   if not not available there, then only go the the heap.Changes have been made
                   under the feature flag FEATURE_RRC_STATIC_BUFFER_ALLOC
11/23/06   kp      SMC suspend request & confirmation will be through call back function
                   with RLC, Cleaning up code for command interface.
09/25/06   kp      Added support for sib_change_ind interface change
08/28/06   da      Added support for OOS area handling during transition from DCH.
08/25/06   vm      Added support for FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT.
07/21/06   vm      Added support for the feature UOOSI.
06/07/06   kp      Added support for six hours sib update
05/02/06   sg      Added support for single to dual mode changes, a new structure
                   RRC_MODE_STATUS_IND is added to inform MEAS and UECI
                   modules to inform about the mode change from WCDMA only to
                   automatic mode
01/25/06   bd      Lint clean-up
12/30/05   sg      Added support for handling BPLMN Abort search from NAS
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search.
11/04/05  da/vr    Made changes to detect UE inactivity and take appropriate actions.
07/21/05   ss      Added RRC_IDT_COMPLETED_FOR_DOMAIN message. This will be sent to
                   registerd procedures like SCRR upon completion of IDT procedure.
07/08/05   vm      FEATURE_OUT_OF_SERVICE_ENHANCEMENT: Added support for OOS in
                   Connected Mode feature.
                   Added a new command RRC_CONN_MODE_OOS_TIMER_EXPIRED_IND.
06/02/05   vr      Added a new RRC command RRC_IRAT_ABORT_IND to support
                   Interrat reselection abort in Idle mode
05/09/05   vm/ab   Added new MM to RRC command RRC_NW_SEL_MODE_RESET_IND and
                   new command structure rrc_nw_sel_mode_reset_ind_type
05/03/05   sm      Added rrc_cmd_type: 'rrc_abort_ciph_config_req' and new
                   cmd RRC_ABORT_CIPH_CONFIG_REQ to ABORT on-going ciphering
                   configuration.
04/20/05   sgk     Added new cmd RRC_EQ_PLMN_LIST_CHANGE_IND to the list of
                   possible cmds from MM.
03/03/05   vm      Added a new command RRC_FREQ_REDIRECT_WAIT_TIMER_EXPIRED_IND
                   for inter-frequency redirection feature.
12/17/04   vr      Added CM queue and its supporting functions for
                   WCDMA -> 1x handover
12/01/04   bu      Added l1_dl_weak_ind_type to rrc_cmd_data_u_type.
11/22/04   vm      Added RRC_BPLMN_SRCH_TIMER_EXPIRED_IND and
                   RRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND.
11/22/04   da      Added RRC_PHY_CHAN_FAILURE_TIMER_EXPIRED_IND.
11/18/04   vm      Added RRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND.
11/18/04   vk      Added RRC_RRC_CON_DELAY_TIMER_EXPIRED_IND to RRC Internal Cmds.
11/15/04   vk      Added rrc_abort_req_type structure for RRC_ABORT_REQ
09/15/04   sk      Added RRC_OOS_TIMER_EXPIRED_IND to RRC internal commands.
08/23/04   bu      Added RRC_CPHY_DL_WEAK_IND to the list of cmds sent by L1.
08/05/04   bu      Added RRC_CSP_WCDMA_RESUMED_IND to RRC internal commands.
07/16/04   ttl     Added TFC Subset support.
05/05/04   bu      Added RRC_FREQ_SCAN_TIMER_EXPIRED_IND to RRC internal commands.
03/22/04   kc      Added prototype for rrc_free_embedded_buf().
03/19/04   kc      Added internal commands to route PCCO commands.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
02/20/04   bu      Added RRC_CPHY_OPERATIONAL_ERROR_IND to rrc_cmd_e_type.
                   Added l1_op_err_ind_type to rrc_cmd_data_u_type. The structure
                   definition is in l1rrcif.h.
02/12/04   bu      Added RRC_ERR_FATAL_IND to the internal RRC command list.
02/02/04   vk      Added support for processing A-GPS related cmds under
                   FEATURE_MMGPS
12/24/03   svk     Added support for URA_UPDATE under FEATURE_CELLPCH_URAPCH
11/20/03   bu      Added RRC_DCH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND.
11/11/03   kc      Added support for processing SRNS relocation related cmds.
10/14/03   kc      Added support for HO_CIPH_REQ to pass ciphering algo,
                   rcvd in HO to UTRAN command.
10/10/03   jh      Added support for RRC_CIPH_CONFIG_REQ in IDLE for GtoW
                   handovers.
09/03/03   bu      Added RRCTMR_DELAY_ERR_FATAL_TIMER_EXPIRED_IND.
07/30/03   rj      Modified RRC_SMC_UDT_L2_ACK_IND command to
                   RRC_SMC_UDT_L2_ACK_IND.
07/21/03   kc      Added RRC_SMC_UDT_L2_ACK_IND to internal cmd list
06/23/03   vn      Added RRCTMR_L1_DEADLOCK_DETECT_TIMER_EXPIRED_IND to the
                   list of commands.
05/23/03   bu      Added RRC_MODE_CHANGE_REQ to rrc_cmd_e_type.
04/30/03   kc      Added 2 internal cmd's RRC_HO_CIPH_CNF  and RRC_HO_CIPH_REQ
                   to support GtoW ciphering.
04/24/04   rj      Added RRC Internal commands RRC_SIG_CONN_RELEASE_REQ and
                   RRC_SIG_CONN_RELEASE_CNF commands.
04/24/03   bu      Removed usage of FEATURE_L1_ACT_TIME. Defined seq_num in
                   rrc_cmd_hdr_type.
04/10/03   bu      Added internal RRC command type rrc_lower_layer_deact_req_type
                   and rrc_deact_req sent by MM to RRC in rrc_cmd_data_u_type.
02/18/03   vn      Added RRC_DCH_FACH_CELL_SELECTION_TIMER_EXPIRED_IND to the
                   list of commands.
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to
                   INTERRAT_HANDOVER_GTOW
02/14/03   kc      Added function types for GtoW Interrat Handover.
01/15/03   sk      Added the function declaration for rrc_free_rr_cmd_buf()
                   Included "rrcrrif.h" header file
12/04/02   xfg     Added a new command: RRC_CHANGE_MODE_IND to support mode change
11/06/02   xfg     Removed #ifdef FEATURE DUALMODE BASELINE for L1 Comand ID
11/04/02   rj      Added a new command RRC_IGNORE_BCCH_MOD_TIMER_EXPIRED_IND
                   to indicate that the timer to ignore BCCH Modifcation is
                   Expired.
11/04/02   xfg     Added command type: rrc_stop_wcdma_mode_req_type to support
                   deep sleep
10/03/02   xfg     Fixed a compilation error due to the previous change.
10/03/02   vk      Added new RRC internal command RRC_COMPRESSED_MODE_IND
                   that signals compressed mode run time error to Physical
                   Channel Reconfiguration procedure.
10/03/02   xfg     Added new L1 command structures for dual-mode support
09/04/02   kc      Added commands to support ciphering
08/21/02   vk      Added command RRC_DELETE_MEAS_REQ to indicate the measurement
                   identity to be deleted in case of run time error due to
                   overlap in compressed mode patterns under FEATURE CM SUPPORTED
08/16/02   xfg     Added #ifdef FEATURE_WCDMA for not breaking GSM only build.
08/06/02   xfg     Added MM command RRC_ACTIVATION_RSP for inter-RAT operations.
08/06/02   rj      Added MM commands RRC_INVALIDATE_SIM_DATA_REQ,
                   RRC_SIM_INSERTED_REQ, and RRC_SIM_UPDATE_REQ and removed
                   MM commands RRC_PAGING_CTRL_REQ and RRC_CIPHER_REQ for
                   new RRC-MM interface.
07/31/02   xfg     Added command Ids for RR to RRC I/F, and get buffer and
                   put command functions for support dual-mode inter-RAT HO.
07/26/02   vk      Put l1_compressed_mode_ind_type under FEATURE CM SUPPORTED
07/18/02   vk      Put the previous checkin for compressed mode under feature
                   FEATURE CM SUPPORTED
07/18/02   djm     Addition of compressed mode indication to rrc command
06/28/02   xfg     Removed ifndef FEATURE_GSM for Dual-mode build
05/29/02   ram     Added commands RRC_SIB7_EXP_TIMER_EXPIRED_IND and
                   RRC_SIB7_UPDATE_STATUS_IND to rrc_cmd_e_type.
05/17/02   vk      Added RRC_CMAC_MEAS_CTRL_CNF which is used by MAC to indicate
                   to RRC about the status of Uplink Traffic Volume Config
                   command. Also updated the structure cmac_ul_traffic_ind and
                   added a new structure mac_meas_ctrl_cnf to support Traffic
                   Volume Measurements. These changes have been put under the
                   FEATURE TRAFFIC VOLUME MEAS
05/16/02   bu      Merged the following from cell reselection branch:
           bu      Added RRC_CELL_RESELECTION_REQ and RRC_CELL_RESELECTION_CNF
                   to rrc_cmd_e_type. Added rrc_cell_reselection_req_type and
                   rrc_cell_reselection_cnf_type to rrc_cmd_data_u_type.
04/29/02   xfg     Added new interface changes for Cheetah baseline. All
                   changes are marked with FEATURE DUALMODE BASELINE tag.
03/13/02   bu      Added RRC_CPHY_ACT_CANCEL_CNF to rrc_cmd_e_type and
                   l1_act_time_cancel_cnf_type rrc_cmd_data_u_type union if
                   FEATURE_L1_ACT_TIME is defined.
03/01/02   rj      Added RRC internal commands RRC_RNTI_UPDATE_REQ and
                   RRC_RNTI_UPDATE_CNF to update or delete C-RNTI and/orU-RNTI.
02/14/02   rj      Added RRC internal commands RRC_T302_EXPIRED_IND,
                   RRC_T305_EXPIRED_IND, RRC_T316_EXPIRED_IND,
                   RRC_T317_EXPIRED_IND, RRC_T307_EXPIRED_IND,
                   RRC_T314_EXPIRED_IND and RRC_T315_EXPIRED_IND for timers
                   that are required for CELL UPDATE procedure.
02/14/02   xfg     Added a feature tag: FEATURE_TC for LB changes
02/02/02   ram     Added RRC_SIB_CHANGE_IND command which is used by the SIB
                   procedure to inform other procedures of a SIB in the serving
                   cell that had been changed.
01/28/02   xfg     Added new TC commands for the loopback test
12/06/01   rj      Added FEATURE_GSM to hide lower layer interfaces from MM for
                   GSM.
11/05/01   vk      Added RRC_CPHY_MEAS_CTRL_CNF command to indicate to Meas
                   procedure if meas ctrl message has been accepted
10/15/01   ram     Added RRC_SIB_WAIT_TIMER_EXPIRED_IND command for
                   indicating to the sib procedure that the sib wait
                   timer has expired.
10/16/01   vn      Added RRC_LOWER_LAYER_DEACT_REQ (and Confirm) to request
                   lower layer deactivation from LLC, RRC_CPHY_DEACTIVATE_CNF
                   for the deactivation confirmation from L1 and
                   RRC_CPHY_OUT_OF_SERVICE_AREA_IND for Out of service area
                   indication from L1.
10/15/01   rj      Added command RRC_INITIATE_CELL_SELECTION_IND to trigger
                   Cell Selection by the CSP procedure.
10/15/01   gs      Added command Id RRC_CPHY_RL_FAILURE_IND for Radio Link
                   Failure to the  definition of rrc_cmd_e_type. Added
                   structure l1_phychan_est_ind of l1_phychan_est_ind_type
                   type to union rrc_cmd_data_u_type in structure definition of
                   rrc_cmd_type.
09/16/01   vn      Added RRC_CPHY_DRX_CNF and its command structure to
                   rrc_cmd_e_type and rrc_cmd_type respectively.
09/14/01   xfg     Added  RRC_TIMER_FOR_RSSI_BER_EXPIRED_IND command for
                   reporting RSSI & BER to CM
08/31/01   vn      Support for frequency scan - added RRC_CPHY_FREQ_SCAN_CNF
                   and l1_freq_scan_cnf_type to the relevant data structures.
08/29/01   vk      Added command RRC_CELL_SELECTED_MEAS_REQ and its data
                   structure so that cell selection procedure can post
                   command to measurement procedure to tell L1 to start
                   doing meas based on the given SIB data
08/06/01   rj      Added data structure for RRC_CELL_UPDATE_COMPLETE_CNF
                   command.
07/27/01   rj      Added new internal command RRC_DISABLE_PREV_FREQ_IND to
                   disable the previous frequency reselection for wait time.
07/20/01   rj      Added new internal commands for T304 timer expiration and
                   to initiate UE Capability Information Procedure. Added
                   command structures for RRC_INITIATE_UE_CAPABILITY_REQ and
                   RRC_CELL_UPDATE_INITIATE_REQ.
06/07/01   vn      Added RRC_CRLC_RELEASE_ALL_CNF to confirm release of all RLC
                   channels.
05/04/01   vn      Added RRC_ACT_REQ and RRC_DEACT_REQ.
04/29/01   rj      Added command RRC_UPDATE_SIBS_IND to send BCCH modification
                   info to the SIB procedure from Paging Type 1 procedure.
03/29/01   kmp     Added RRC_CPHY_CELL_TRANSITION_CNF to the rrc_cmd_e_type.
                   Updated RRC_CRLC_*M_*L_CONFIG_CNF to more closely match
                   the structure definition. Removed #inlcude l1task.h.
03/27/01   rj      Fixed mismatches in the previous check-in and renamed
                   rrc_cnf_cmd to rrc_cmd_data_u_type. Added template
                   data structure for RRC_LAI_LIST_REQ command.
03/15/01   upen    Made union rrc_cnf_cmd into a typedef
03/14/01   rj      Added new internal commands for interactions between
                   direct transfer procedures and Cell update procedure. Added
                   command structure to the RRC_CLOSE_SESSION_REQ command.
02/28/01   ram     Added confirm commands for all rlc configuration commands.
02/12/01   rj      Added new commands RRC_T308_EXPIRED_IND, and
                   RRC_TIMER_FOR_RCR_L2ACK_EXPIRED_IND for RRC Connection
                   Release procedure and the command
                   RRC_TIMER_FOR_L2ACK_EXPIRED_IND is renamed to
                   RRC_TIMER_FOR_RCE_L2ACK_EXPIRED_IND.
01/25/01   rj      Added new RRC_TIMER_FOR_L2ACK_EXPIRED_IND to indicate failure
                   to received L2 ACK status. Removed  RRC_UE_CAMP_ON_IND command
                   since RRC_NEW_CELL_IND can serve the same puprpose.
01/23/01   kmp     Added new command and confirmation for requesting L1 services
                   through the Lower Layer Controller (LLC). Removed the
                   RRC_CPHY_SET_SFN_CNF, RRC_CPHY_SET_CFN_CNF, and
                   RRC_CPHY_IMMEDIATE_CELL_EVAL_CNF commands since they are
                   no longer required.
01/19/00   vn      Added Commands related to Cell change viz Cell Change req,
                   Cell Change Cnf and New Cell Ind.
01/17/00   vn      Removed l2ultask.h and l2dltask.h - these files should not be
                   included here - leads to compiler errors.
01/09/01   ram     Removed rrc_state_change_req_type command that's no longer
                   needed. State change requests are handled by direct function
                   call.
01/09/01   ram     In rrc_cmd_e_type added a new command RRC_NEW_STATE_IND which
                   is sent to the state change manager whenver RRC enters a new
                   state. Removed RRC_STATE_CHANGE_REQ which is not needed. In
                   rrc_cmd_type added two new types rrc_new_state_ind_type and
                   rrc_state_change_ind_type.
01/04/01   ram     In rrc_cmd_type, modified the name "response" to "get_sibs_cnf"
                   to be consistent with the command naming convention. Added a
                   new internal command RRC_GET_SPECIFIC_SIBS_CNF as a confirmation
                   for the RRC_GET_SPECIFIC_SIBS_REQ.
12/22/00   rj      Added new internal commands RRC_TIMER_TO_CAMP_ON_EXPIRED_IND,
                   RRC_WAIT_TIMER_EXPIRED_IND, RRC_T300_EXPIRED_IND,
                   RRC_REDIRECT_REQ and RRC_REDIRECT_CNF are added for
                   RRC Connection Establishment procedure.
12/20/00   ram     Added RRC_GET_SPECIFIC_SIBS_REQ command id to rrc_cmd_e_type
                   and rrc_get_specific_sibs_req_type command to rrc_cmd_type.
                   Changed the name sib_cell_info to rrc_get_all_sibs_req.
12/15/00   vn      Removed rrc_close_session_req to work with r1.4 of rrcmmif.h
                   Fixed compiler errors.
12/15/00   rj      Modified command RRC_GET_ALL_SIBS to RRC_GET_ALL_SIBS_REQ and
                   added RRC_UE_CAMP_ON_IND, RRC_NEW_CELL_IND, RRC_STATE_CHANGE_REQ
                   and RRC_STATE_CHANGE_IND commands. Modified rrc_cmd_type to add
                   data structures for RRC_CM_CHANNEL_CONFIG_REQ and
                   RRC_CM_CHANNEL_CONFIG_CNF
12/14/00   vn      Added addnl MM to RRC cmds in rrc_cmd_e_type and rrc_cmd_type.
12/12/00   kmp     Changed RRC_CM_CHANNEL_SETUP_REQ to RRC_CM_CHANNEL_CONFIG_REQ.
                   Changed RRC_CM_CHANNEL_SETUP_CNF to RRC_CM_CHANNEL_CONFIG_CNF.
                   Removed RRC_CM_CHANNEL_RELEASE_REQ and RRC_CM_CHANNEL_RELEASE_CNF.
12/06/00   ram     Merged the different command types to a single enumerated
                   command type containing all command ids for RRC commands.
                   Added specific start values for command ids from different
                   sources to help with debugging the command queues.
                   Merged the command structures into a single rrc_cmd_type.
                   Included rrcintif.h which now contains all internal
                   command definitions for RRC.
12/04/00   kmp     Fixed a typo in the rlc section. Changed rlc_ul_chsuspend_cnf_type
                   to rlc_ul_suspend_cnf_type in the rrc_rlc_cmd_type.
12/01/00   ram     Changed rrclcp.h to rrclcm.h. Changed l1rrcctrl.h into
                   l1rrcif.h and l1task.h
11/30/00   ram     Updated rrc_l1_cmd_type to include the union of all the l1
                   commands sent to RRC.
11/29/00   ram     Removed the function prototype rrc_free_cmd_buf(). This
                   belongs in rrccmdi.h since no task other than RRC uses it.
11/29/00   kmp     Added L1, RLC, and MAC command IDs. Added lower layer command
                   structure definitions as well for the NAS.
11/29/00   ram     Removed tab characters introduced by the editor. Changed
                   the name rrc_sib_cell_info_type to rrc_get_all_sibs_type
                   in order to be consistant with the command id RRC_GET_ALL_SIBS.
11/27/00   ram     Updated to include all externalized function prototypes.
11/22/00   ram     Added two MM command ids RRC_SERVICE_REQ and RRC_EST_REQ. Removed
                   The MAC and RLC command ids that were incorrect. Updated L1
                   command ids.
11/22/00   ram     Removed "_CMD" from the command Ids. The command ids already have
                   _REQ, _CNF etc attached to them which makes _CMD redundant.
11/15/00   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "queue.h"
#include "rrcmmif.h"
#include "tc_rrc.h"
#include "rrcintif.h"
#include "l1rrcif.h"
#include "macrrcif.h"
#include "rlcrrcif.h"
#include "rrc_rr_types.h"
#include "rrcrrif.h"


#include "rrclsmif.h"
#include "wcdma_rrc_msg.h"
#include <lte_rrc_irat_msg.h>
#include <lte_irat_types.h>

#include "rrc_tdsrrc_if.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                 RRC COMMAND IDs                        */
/*                                                        */
/*  Command Ids for commands sent from various tasks to   */
/*  RRC are defined here. All command ids, regardless of  */
/*  which command queue is used to queue them are defined */
/*  in a single enum type given below.                    */
/*                                                        */
/*  NOTE! When you add a new command ID please make sure  */
/*  the corrosponding command structure is added to the   */
/*  structure rrc_cmd_type that contains all the commands.*/
/*                                                        */
/*--------------------------------------------------------*/
/*lint -save -e793 */
typedef enum
{
  /* NOTE: When adding new commands to this enum type
  please add the command to the appropriate group based
  on the command type. Each group has been given a specific
  base number in order to assist debugging the command
  queues. */

/*--------------------------------------------------------*/
/* Command Ids of Internal RRC commands                   */
/*--------------------------------------------------------*/

  RRC_INT_CMD_BASE = 0x01000000,  /* Internal commands start here */

  /* 0x01000001: Command posted by LLC to indicate that RLC downlink SDU has been received */
  RRC_DOWNLINK_SDU_IND,

  /* 0x01000003: Command posted by LLC to indicate that RLC downlink L2 ACK has been received*/
  RRC_DOWNLINK_L2ACK_IND,

  /* 0x01000004: Command posted from any RRC module to LLC to request a reset of L1. In other
   * words, no physical channels are active, and L1 is waiting for an
   * CPHY_ACQ_REQ.
   */
  RRC_RESET_L1_REQ,

  /* 0x01000005: */
  RRC_RESET_L1_CNF,

  /* 0x01000007: Command to get specific SIBs from the serving cell or a neighbor cell. */
  RRC_GET_SPECIFIC_SIBS_REQ,

  /* 0x01000008: Command posted from SIB module to another RRC module to confirm a
   * RRC_GET_SPECIFIC_SIBS_REQ command.
   */
  RRC_GET_SPECIFIC_SIBS_CNF,

  /* 0x01000009: Command posted by timer when TIMER_TO_CAMP_ON is expired */
  RRC_TIMER_TO_CAMP_ON_EXPIRED_IND,

  /* 0x0100000A: Command posted by timer when WAIT TIME Timer is expired */
  RRC_WAIT_TIMER_EXPIRED_IND,

  /* 0x0100000B: Command used to indicate that Phy Chan Failure timer is expired */
  RRC_PHY_CHAN_FAILURE_TIMER_EXPIRED_IND,

  /* 0x0100000C: Command posted by timer when RRC Connection Delay Timer is expired */
  RRC_RRC_CON_DELAY_TIMER_EXPIRED_IND,

  /* 0x0100000D: Command posted by timer when Timer T300 is  expired */
  RRC_T300_EXPIRED_IND,

  /* 0x0100000E: Command posted by timer when Timer T308 is  expired */
  RRC_T308_EXPIRED_IND,

  /* 0x0100000F: Command posted by timer when Timer T304 is  expired */
  RRC_T304_EXPIRED_IND,

  /* 0x01000010: Command posted by timer when Timer T302 is  expired */
  RRC_T302_EXPIRED_IND,

  /* 0x01000011: Command posted by timer when Timer T305 is  expired */
  RRC_T305_EXPIRED_IND,

  /* 0x01000012: Command posted by timer when Timer T316 is  expired */
  RRC_T316_EXPIRED_IND,

  /* 0x01000013: Command posted by timer when Timer T317 is  expired */
  RRC_T317_EXPIRED_IND,

  /* 0x01000014: Command posted by timer when Timer T307 is  expired */
  RRC_T307_EXPIRED_IND,

  /* 0x01000015: Command posted by timer when Timer T314 is  expired */
  RRC_T314_EXPIRED_IND,

  /* 0x01000016: Command posted by timer when Timer T315 is  expired */
  RRC_T315_EXPIRED_IND,

  /* 0x01000017: Command posted by timer when RCE Timer for L2 ACK  is  expired */
  RRC_TIMER_FOR_RCE_L2ACK_EXPIRED_IND,

  /* 0x01000018: Command posted by timer when RCR Timer for L2 ACK  is  expired */
  RRC_TIMER_FOR_RCR_L2ACK_EXPIRED_IND,

  /* 0x01000019: Command posted by RCE to CSP with either frequency or System redirection */
  RRC_REDIRECT_REQ,

  /* 0x0100001A: Command posted by CSP to RCE after CSP camped on to redirected frequency or
   * system
   */
  RRC_REDIRECT_CNF,

  /* 0x0100001B: Command posted to State Change Manager when RRC state changes. */
  RRC_NEW_STATE_IND,

  /* 0x0100001C: Command poster by State Change Manager to other RRC procedures
   * to notify them of the state transitions.
   */
  RRC_STATE_CHANGE_IND,

  /* 0x0100001D: Command posted to Cell Change manager requesting a Cell Change */
  RRC_CELL_CHANGE_REQ,

  /* 0x0100001E: Command posted by Cell Change Manager in response to Cell Change Req.
   * This command is posted to the procedure that requested Cell Change.
   */
  RRC_CELL_CHANGE_CNF,

  /* 0x0100001F: Command posted by Cell Change manager to inform procedures of a change
   * in cell.
   */
  RRC_NEW_CELL_IND,

  /* 0x01000020: Command posted from any RRC module to LLC to setup/reconfig/release
   * connected mode channels.
   */
  RRC_CHANNEL_CONFIG_REQ,

  /* 0x01000021: Command posted from LLC to any RRC module to confirm a setup/reconfig/release
   * of connected mode channels.
   */
  RRC_CHANNEL_CONFIG_CNF,

  /* 0x01000022: Command used to request any one of a number of L1 or L2 commands as
   * specified in rrc_llc_cmd_type
   */
  RRC_LLC_CMD_REQ,

  /* 0x01000023: Command used to confirm any one of a number of L1 or L2 commands as
   * specified in rrc_llc_cmd_type
   */
  RRC_LLC_CMD_CNF,

  /* 0x01000024: Command used to confirm that CELL UPDATE procedure is completed. This will
   * be in response to RRC_CELL_UPDATE_INITAITE_REQ
   */
  RRC_CELL_UPDATE_COMPLETE_CNF,

  /* 0x01000025: Command used to initiate CELL UPDATE procedure. It includes the reason to for
   * CELL UPDATE Procedure initiation
   */
  RRC_CELL_UPDATE_INITIATE_REQ,

  /* 0x01000026: Command used to inform SIB procedure that SIBs modified. */
  RRC_UPDATE_SIBS_IND,

  /* 0x01000027: Command used to initiate UE Capability Information procedure */
  RRC_INITIATE_UE_CAPABILITY_REQ,

  /* 0x01000028: Command used by the RCE procedure to indicate CSP to exculde previous frequency
   * from the cell reselection for the specified Wait Time
   */
  RRC_DISABLE_PREV_FREQ_IND,

  /* 0x01000029: Command posted by cell selection procedure to measurement procedure
   * to read SIB11/12 and then inform L1 to start measurements
   */
  RRC_CELL_SELECTED_MEAS_REQ,

  /* 0x0100002A: Command used to perform the periodically RSSI&BER report to the CM */
  RRC_TIMER_FOR_RSSI_BER_EXPIRED_IND,

  /* 0x0100002B: Command used to trigger cell selection */
  RRC_INITIATE_CELL_SELECTION_IND,

  /* 0x0100002C: Command to request deactivation of lower layers */
  RRC_LOWER_LAYER_DEACT_REQ,

  /* 0x0100002D: Confirmation for lower layer deactivation */
  RRC_LOWER_LAYER_DEACT_CNF,

  /* 0x0100002E: Command used to inform the SIB procedure that the SIB wait time has
   * expired
   */
  RRC_SIB_WAIT_TIMER_EXPIRED_IND,

  /* 0x0100002F: Command posted by SIB procedure to other procedures when
   * a SIB in the serving cell has been modified by the UTRAN
   */
  RRC_SIB_CHANGE_IND,

  /* 0x01000030: Command to RRC LLC update C-RNTI and/or U-RNTI */
  RRC_RNTI_UPDATE_REQ,

  /* 0x01000031: Command to Confirm the updatation of C-RNTI and/or U-RNTI */
  RRC_RNTI_UPDATE_CNF,

  /* 0x01000032: Command to stop WCDMA mode in the lower layers */
  RRC_STOP_LOWER_LAYER_REQ,

  /* 0x01000033: Command to Confirm the stop of WCDMA mode */
  RRC_STOP_LOWER_LAYER_CNF,

  /* 0x01000034: Command to notify a RRC mode transition */
  RRC_MODE_CHANGE_IND,

  /* 0x01000035: Command to RRC LLC for cell reselection */
  RRC_CELL_RESELECTION_REQ,

  /* 0x01000036: Command to confirm the reselection request */
  RRC_CELL_RESELECTION_CNF,

  /* 0x01000037: Command to give the SIB7 update status. Posted by
   * the SIB procedure to other RRC procedures
   */
  RRC_SIB7_UPDATE_STATUS_IND,

  /* 0x01000038: Command to indicate that the SIB7 expiration
   * timer has expired.
   */
  RRC_SIB7_EXP_TIMER_EXPIRED_IND,

  /* 0x01000039: Command from LLC to indcate end of reconfig procedure,
   * and SMC can continue security configuration
   */
  RRC_RECONFIG_COMPLETE_IND,

  /* 0x0100003A: Command from LLC to indcate configuration of lower layers
   * with ciphering information passed in ciph_config_req
   */
  RRC_CIPH_CONFIG_REQ,

  /* 0x0100003B: Command from LLC to indcate configuration of lower layers
   * with ciphering information passed in ciph_config_req
   */
  RRC_CIPH_CONFIG_CNF,

  /* 0x0100003C: Command to be send from SMC to LLC to abort ongoing
   * ciphering configuraiton at RLC and revert to old set of keys.
   * NOTE: There is no CNF associated with this command.
   */
  RRC_ABORT_CIPH_CONFIG_REQ,

  /* 0x0100003D: Command from Timer to indicate that the timer to ignore
   * BCCH Modification info has expired
   */
  RRC_IGNORE_BCCH_MOD_TIMER_EXPIRED_IND,

  /* 0x0100003E: Command from Timer to indicate that the timer for cell
   * selection going from DCH->FACH has expired
   */
  RRC_DCH_FACH_CELL_SELECTION_TIMER_EXPIRED_IND,

  /* 0x0100003F: Command from Procedure to SCRR to release Signalling Connections */
  RRC_SIG_CONN_RELEASE_REQ,

  /* 0x01000040: Command from SCRR to Procedure confirming that Signalling Connections
   * are released
   */
  RRC_SIG_CONN_RELEASE_CNF,

  /* 0x01000041: */
  RRC_CHANGE_MODE_REQ,

  /* 0x01000042: */
  RRC_CHANGE_MODE_CNF,

  /* 0x01000043: */
  RRC_NAS_DATA_TRANSFER_IND,

  /* 0x01000044: */
  RRC_REVERT_TO_UTRAN_CONFIG_REQ,

  /* 0x01000045: */
  RRC_REVERT_TO_UTRAN_CONFIG_CNF,

  /* 0x01000046: */
  RRC_DELETE_MEAS_REQ,

  /* 0x01000047: */
  RRC_COMPRESSED_MODE_IND,

  /* 0x01000048: */
  RRC_CSP_ACQ_REQ,

  /* 0x01000049: */
  RRC_CSP_ACQ_CNF,

  /* 0x0100004A: */
  RRC_HO_CIPH_REQ,

  /* 0x0100004B: */
  RRC_HO_CIPH_CNF,

  /* 0x0100004C: Command from Timer to indicate that the timer for detection
   * of L1 deadlock condition has expired.
   */
  RRCTMR_L1_DEADLOCK_DETECT_TIMER_EXPIRED_IND,

  /* 0x0100004D: Command from Timer to indicate that the timer to delay ERR_FATAL
   * for CPHY_SETUP_CNF or CPHY_CELL_TRANSITION_CNF has expired
   */
  RRCTMR_DELAY_ERR_FATAL_TIMER_EXPIRED_IND,

  /* 0x0100004E: IND to inform UDT procedure that SMC has gotten L2 ACK and it is safe to
   * transmit Uplink Messages
   */
  RRC_SMC_DDT_L2_ACK_IND,

  /* 0x0100004F: Command from TMR to indicated that the timer for cell
   * selection from DCH to CELL_PCH or URA_PCH has expired
   */
  RRC_DCH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND,

  /* 0x01000050: Req/CNF between Reconfig-Procedures and SMC to trigger re-establishment
   * of RLC radio bearers
   */
  RRC_RE_EST_RLC_FOR_SRNS_REQ,

  /* 0x01000051: */
  RRC_RE_EST_RLC_FOR_SRNS_CNF,

  /* 0x01000052: Req/CNF between SMC-LLC to re-establish RLC radio bearers
   */
  RRC_LLC_RE_EST_RLC_REQ,

  /* 0x01000053: */
  RRC_LLC_RE_EST_RLC_CNF,

  /* 0x01000054: */
  RRC_ERR_FATAL_IND,

  /* 0x01000055: */
  RRC_T3174_EXPIRED_IND,

  /* 0x01000056: */
  RRC_CSP_CAMP_REQ,

  /* 0x01000057: */
  RRC_CSP_CAMP_CNF,

  /* 0x01000058: */
  RRC_CONN_SETUP_IND,

  /* PLEASE ADD ALL NEW Internal Commands HERE */

  /* 0x01000059: Command used to initiate URA UPDATE procedure. It includes the reason to for
   * URA UPDATE Procedure initiation
   */
  RRC_URA_UPDATE_INITIATE_REQ,

  /* 0x0100005A: Command used to confirm that URA UPDATE procedure is completed. This will
   * be in response to RRC_URA_UPDATE_INITAITE_REQ
   */
  RRC_URA_UPDATE_COMPLETE_CNF,

  /* 0x0100005B: Command used to indicate to CSP that full frequency scan can be initiated */
  RRC_FREQ_SCAN_TIMER_EXPIRED_IND,

  /* 0x0100005C: Command used to indicate that OOS timer is expired */
  RRC_OOS_TIMER_EXPIRED_IND,

  /* 0x0100005D: Command used to indicate that Deep Sleep no service timer has expired */
  RRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND,

  /* 0x0100005E: Command used to indicate to CSP that WCDMA has been resumed (by CHO) */
  RRC_CSP_WCDMA_RESUMED_IND,

  /* 0x0100005F: Command used to indicate that BPLMN 2G->3G search timer has expired */
  RRC_BPLMN_SRCH_TIMER_EXPIRED_IND,

  /* 0x01000060: Command used to indicate that BPLMN 2G->3G full scan timer has expired */
  RRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND,

  /* 0x01000061: Command used to indicate that the wait timer started for the last camped
   * frequency during inter frequency redirection has expired.
   */
  RRC_FREQ_REDIRECT_WAIT_TIMER_EXPIRED_IND,

  /* 0x01000062: Command used to abort interrat operation when a higher priority request
   * needs to be processed
   */
  RRC_IRAT_ABORT_IND,

  /* 0x01000063: Command used to indicate that the 30s specific PLMN search timer when
   * UE is OOS in Connected Mode has expired
   */
  RRC_CONN_MODE_OOS_TIMER_EXPIRED_IND,

  /* 0x01000064: Command used to indicate that the inactivity timer when
   * has expired
   */
   RRC_INACTIVITY_TIMER_EXPIRED_IND,

  /* 0x01000065: Command used for indicating the completion of IDT procedure for a particular domain
   * This is used by release procedure
   */
  RRC_IDT_COMPLETED_FOR_DOMAIN,

  /* 0x01000066: Command used to indicate that BPLMN 3G->3G search timer has expired */
  RRC_WTOW_BPLMN_DRX_SRCH_TIMER_EXPIRED_IND,

  /* 0x01000067: Command used to indicate that MAX BPLMN 3G->3G search timer allowed has expired */
  RRC_BPLMN_GUARD_SRCH_TIMER_EXPIRED_IND,

  /* 0x01000068: Command used to indicate that BPLMN 3G->3G,2G search can be started */
  RRC_BPLMN_SEARCH_RESUME_IND,

  /* 0x01000069: Command sent by other procedures to CSP to suspend the BPLMN search */
  RRC_BPLMN_SUSPEND_REQ,

  /* 0x0100006A: Command sent by CSP to the procedure that sent suspend request for suspending BPLMN search */
  RRC_BPLMN_SUSPEND_CNF,

  /* 0x0100006B: command sent from MCM to Meas and UECI on RRC_CHANGE_MODE_IND */
  RRC_MODE_STATUS_IND,

  /* 0x0100006C: Command used to indicate for updating six hrs sib timer*/
  RRC_SIX_HRS_SIB_TIMER_UPDATE_IND,

  /* 0x0100006D: Command used to indicate that Uniform OOS search timer has expired */
  RRC_UNIFORM_OOS_SEARCH_TIMER_EXPIRED_IND,

  /* 0x0100006E: Command used to put UE into deep sleep when UE is OOS in Connected Mode
   */
  RRC_CONN_MODE_DEEP_SLEEP_REQ,

  /* 0x0100006F: Command used to wakeup UE from deep sleep when UE is OOS in Connected Mode
   */
  RRC_CONN_MODE_DEEP_SLEEP_WAKEUP_REQ,

  /* 0x01000070: Command used to indicate that the initial Tmins no deep sleep
   * timer has expired. The timer is started when UE goes OOS in Connected Mode.
   */
  RRC_CONN_MODE_OOS_INITIAL_NO_DS_TIMER_EXPIRED_IND,

  /* 0x01000071: Command used to indicate that the deep sleep timer has expired and UE
   * should now wake up and search for service again
   */
  RRC_CONN_MODE_OOS_DS_TIMER_EXPIRED_IND,

  /* 0x01000072:  */
  RRC_RB_PROC_COMPL_IND,

  /* 0x01000073: Command used to indicate SIB change Gaurd timer expired*/
  RRC_SIB_CHANGE_GUARD_TIMER_EXPIRED_IND,

  /* 0x01000074: this command is sent by LLC to CSP after receiving START_CNF
   * from L1 in Connected Mode
   */
  RRC_CONN_MODE_DEEP_SLEEP_WAKEUP_CNF,

  /*0x01000075: Command used to indicate BCCH Modification timer expiry*/
  RRC_BCCH_MODIFICATION_TIMER_EXPIRED_IND,

  /* 0x01000076: Command used to indicate that  Manual PLMN search full scan timer has expired */
  RRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND,

  /* 0x01000077: Command from timer proc to indicate that the timer for cell
   * selection going from FACH->FACH/PCH has expired
   */
  RRC_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND,

  /* 0x01000080: Command posted by RRC when GSTK queries RRC for
   * Network Measurement information
   */
  RRC_GSTK_UTRAN_NMR_INFO_REQ,

  /* 0x01000081: Command from timer proc to indicate that the timer for cell
   * selection on GtoW redirection has expired.
   */
  RRC_GTOW_REDIRECTION_WAIT_TIMER_EXPIRED_IND,

  /* 0x01000082: Command from timer proc to indicate the guard timer for 
   * abort W2G BPLMN has expired.
   */
  RRC_WTOG_ABORT_GUARD_TIMER_EXPIRED_IND,

  /* 0x01000083: Command from CSP proc to indicate MCM can go ahead with 
   * stop wcdma request
   */
  RRC_CONTINUE_STOP_MODE_REQ,

  /*0x01000084: Command for expiry of timer T320. T320 is the wait timer for CU. 
        UTRAN may use this timer to queue the UE in Cell_PCH or URA_PCH state.*/
  RRC_T320_EXPIRED_IND,

  /*0x01000085: Command for requesting "Change of UE Capability" thru COUEC proc*/
  RRC_COUEC_REQ,
  
  /*0x01000086: Command for confirmation of "Change of UE Cap." request*/
  RRC_COUEC_CNF,

  /*0x01000087: Command for expiry of COUEC timer*/
  RRC_TIMER_COUEC_EXPIRED_IND,

  /*0x01000088 : Command used to indicate T319 expiry*/
  RRC_T319_EXPIRED_IND,

  /*0x01000089: Command used to indicate that Dedicated priority validity timer expired*/
  RRC_DED_PRI_VALDITY_TIMER_EXPIRED_IND,

  /*0x01000090: Command used to indicate that Dedicated priority info has changed*/
  RRC_DED_PRI_CHANGE_IND,

  RRC_WRM_PREEMPT_IND,

  RRC_NO_RESOURCE_AVAILABLE_FROM_L1,

  /*0x0100008B : Command used to indicate T323 expiry*/
  RRC_T323_EXPIRED_IND,

/* Command used to indicate that EOOS full scan timer has expired */
RRC_EOOS_FULL_SCAN_TIMER_EXPIRED_IND,

/* Command used to indicate WRM Unlock_By timer expired */
RRC_DUAL_SIM_WRM_UNLOCK_BY_TIMER_EXP_IND,

  RRC_QCHAT_FA_RRC_TIMER_EXPIRED_IND,

  RRC_IHO_OPEN_DOMAIN_IND,

  RRC_LTE_TO_WCDMA_SRCH_TIMER_EXPIRY_IND,

  RRC_INTERNAL_CSFB_ABORT_REQ,

/* Command used to initiate RLC_UL_DATA_IND procedure */
  RRC_RLC_UL_DATA_IND,

  RRC_GTOW_REDIRECTION_LFS_TIMER_EXPIRED_IND,

  RRC_CSG_ASF_SRCH_TIMER_EXPIRY_IND,

  RRC_CU_TRANS_TO_DISCON_TIMER_EXPIRED,

 RRC_SIB_STOP_PARTITION_TMR_EXPIRED_IND,
 
 RRC_SIB_MASK_INVERT_TMR_EXPIRED_IND,

 RRC_SIB_EMPTY_MASK_TMR_EXPIRED_IND,

  /*Timer to report partial PLMN LIST to NAS during Manual PLMN Search*/ 
  RRC_BPLMN_PARTIAL_REPORT_TIMER_EXPIRED_IND,

  RRC_TEN_MIN_SIB_TIMER_UPDATE_IND,

  RRC_NV_REFRESH_IND,

  RRCTMR_ACT_RSP_EF_TIMER_EXPIRED_IND,

  RRCTMR_WT_STATE_CHANGE_EF_TIMER_EXPIRED_IND,
  
  RRC_QSH_PROCESS_CMD,

  RRC_POST_BUFFER_OTA_IND,

  RRC_ACTIVATION_TIMER_EXPIRY_FOR_BUFFER_OTA,
/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by L1                 */
/*--------------------------------------------------------*/

  RRC_L1_CMD_BASE = 0x02000000,  /* L1 commands start here */

  /* 0x02000001 */
  RRC_CPHY_ACQ_CNF,

  /* 0x02000002 */
  RRC_CPHY_SETUP_CNF,

  /* 0x02000003 */
  RRC_CPHY_ERROR_IND,

  /* 0x02000004 */
  RRC_CPHY_IDLE_CNF,

  /* 0x02000005 */
  RRC_CPHY_CELL_SELECTION_CNF,

  /* 0x02000006 */
  RRC_CPHY_CELL_RESELECTION_IND,

  /* 0x02000007 */
  RRC_CPHY_INTER_FREQ_BCH_CNF,

  /* 0x02000008 */
  RRC_CPHY_MEASUREMENT_IND,

  /* 0x02000009 */
  RRC_CPHY_CAPABILITY_CNF,

  /* 0x0200000A */
  RRC_CPHY_INTEGRITY_KEY_MAUC_CNF,

  /* 0x0200000B */
  RRC_CPHY_PHYCHAN_ESTABLISHED_IND,

  /* 0x0200000C */
  RRC_CPHY_IN_SYNC_IND,

  /* 0x0200000D */
  RRC_CPHY_OUT_OF_SYNC_IND,

  /* 0x0200000E */
  RRC_CPHY_RL_FAILURE_IND,

  /* 0x0200000F */
  RRC_CPHY_IMMEDIATE_MEAS_CNF,

  /* 0x02000010 */
  RRC_CPHY_IMMEDIATE_RESELECTION_CNF,

  /* 0x02000011 */
  RRC_CPHY_CELL_TRANSITION_CNF,

  /* 0x02000012 */
  RRC_CPHY_FREQ_SCAN_CNF,

  /* 0x02000013 */
  RRC_CPHY_DRX_CNF,

  /* 0x02000014 */
  RRC_CPHY_OUT_OF_SERVICE_AREA_IND,

  /* 0x02000015 */
  RRC_CPHY_DEACTIVATE_CNF,

  /* 0x02000016 */
  RRC_CPHY_MEAS_CTRL_CNF,

  /* 0x02000017 */
  RRC_CPHY_ACT_CANCEL_CNF,

  /* 0x02000018 */
  RRC_CPHY_START_WCDMA_MODE_CNF,

  /* 0x02000019 */
  RRC_CPHY_STOP_WCDMA_MODE_CNF,

  /* 0x0200001A */
  RRC_CPHY_SUSPEND_WCDMA_MODE_CNF,

  /* 0x0200001B */
  RRC_CPHY_RESUME_WCDMA_MODE_CNF,

  /* 0x0200001C */
  RRC_CPHY_COMPRESSED_MODE_IND,

  /* 0x0200001D */
  RRC_CPHY_OPERATIONAL_ERROR_IND,

  /* 0x0200001E */
  RRC_CPHY_DL_WEAK_IND,

  /* 0x020000027 indicate RRC to initiate sending the measurement results 
  *on RACH in uplink by transitioning to eFACH state*/
  RRC_CPHY_EPCH_TO_EFACH_TRANSITION_IND,

  /*Nudging RRC means telling RRC to bring up PCCPCH_S, so we can reestablish
   *timing of the network
   */
  RRC_CPHY_NUDGE_FOR_SFN_MISS_MATCH_REQ,

  /* modem stat response from L1 to RRC */
  RRC_CPHY_MODEM_STATISTICS_RSP,

  /* MDSP halt indication */
  RRC_CPHY_MDSP_HALT_IND,
  /* WL1 recovery CNF*/
  RRC_CPHY_RECOVER_WCDMA_CNF,
  RRC_CPHY_ACTIVATION_TIMER_EXPIRY_IND,
  /* WL1 has started QTA*/
  RRC_CPHY_START_QTA_CNF,
  /* WL1 has come out of QTA*/
  RRC_CPHY_STOP_QTA_CNF,

  /* PLEASE ADD ALL NEW L1 Commands HERE */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by MAC                */
/*--------------------------------------------------------*/

  RRC_MAC_CMD_BASE = 0x03000000,  /* MAC commands start here */
  RRC_CMAC_UL_HFN_CNF,
  RRC_CMAC_DL_HFN_CNF,
  RRC_CMAC_UL_TRAFFIC_IND,
  RRC_CMAC_STATUS_IND,
  RRC_CMAC_MEAS_CTRL_CNF,
  RRC_CMAC_UL_TFC_SUBSET_CFG_CNF,
  /* PLEASE ADD ALL NEW MAC Commands HERE */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by RLC                */
/*--------------------------------------------------------*/

  RRC_RLC_CMD_BASE = 0x04000000,  /* RLC commands start here */
  RRC_CRLC_STATUS_IND,
  RRC_CRLC_UL_HFN_CNF,
  RRC_CRLC_DL_HFN_CNF,
  RRC_CRLC_DL_TM_CONFIG_CNF,
  RRC_CRLC_UL_TM_CONFIG_CNF,
  RRC_CRLC_DL_UM_CONFIG_CNF,
  RRC_CRLC_UL_UM_CONFIG_CNF,
  RRC_CRLC_AM_CONFIG_CNF,
  RRC_CRLC_RELEASE_ALL_CNF,
  RRC_CRLC_RESUME_CNF,
  RRC_CRLC_PDU_INVALID_IND,
  /* PLEASE ADD ALL NEW RLC Commands HERE */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by MM                 */
/*--------------------------------------------------------*/

  RRC_MM_CMD_BASE = 0x05000000,  /* MM commands start here */

  /* 0x05000001*/
  RRC_SERVICE_REQ,

  /* 0x05000002 */
  RRC_EST_REQ,

  /* 0x05000003 */
  RRC_DATA_REQ,

  /* 0x05000004 */
  RRC_OPEN_SESSION_REQ,

  /* 0x05000005 */
  RRC_CLOSE_SESSION_REQ,

  /* 0x05000006 */
  RRC_ABORT_REQ,

  /* 0x05000007 */
  RRC_PLMN_LIST_REQ,

  /* 0x05000008 */
  RRC_ACT_REQ,

  /* 0x05000009 */
  RRC_DEACT_REQ,

  /* 0x0500000A */
  RRC_STOP_WCDMA_MODE_REQ,

  /* 0x0500000B */
  RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ,

  /* 0x0500000C */
  RRC_INVALIDATE_SIM_DATA_REQ,

  /* 0x0500000D */
  RRC_SIM_INSERTED_REQ,

  /* 0x0500000E */
  RRC_SIM_UPDATE_REQ,

  /* 0x0500000F */
  RRC_ACTIVATION_RSP,

  /* 0x05000010 */
  RRC_CHANGE_MODE_IND,

  /* 0x05000011 */
  RRC_MODE_CHANGE_REQ,

  /* 0x05000012 */
  RRC_EQ_PLMN_LIST_CHANGE_IND,

  /* 0x05000013 */
  RRC_NW_SEL_MODE_RESET_IND,

  /* 0x05000014 */
  RRC_BPLMN_SEARCH_ABORT_REQ,    /* Added for WTOW/WTOG BPLMN SEARCH ABORT */

  /* 0x05000015 */
  RRC_EXIT_FROM_PWR_SAVE,

  /* 0x05000016 */
  RRC_DS_STATUS_CHANGE_IND,

  /* 0x05000017 */
  RRC_FAST_DORMANCY_REQ,       /*Added for fast dormancy request trigger*/

  /* 0x05000018 */
  RRC_DS_TUNEAWAY_STATUS_CHANGE_IND,

  /* 0x05000019 */
  RRC_CSFB_CALL_STATUS_IND, /*CSFB call status indication*/

  /* 0x0500001A */
  RRC_LTE_RESELECTION_ALLOWED_STATUS_IND,

  /* 0x0500001B */
  RRC_LAI_REJECT_LIST_IND,

  /* 0x0500001C */
  RRC_TUNEAWAY_ALLOWED_IND,

  /* 0x0500001D */
  RRC_SET_DRX_IND,

  /* 0x0500001E */
  RRC_CSG_WHITE_LIST_UPDATE_IND,

  /*Used by TDS */
  /* 0x0500001F */
  RRC_UE_MODE_IND,

  /* 0x05000020*/
  RRC_MODE_PREF_CHANGE_IND,

  /* 0x05000021*/
  RRC_MULTIMODE_SUBS_CHGD_IND,

  /* 0x05000022*/
  RRC_PSEUDO_LTE_LIST_UPDATE_IND,

/* PLEASE ADD ALL NEW MM Commands HERE */
/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by TC                 */
/*--------------------------------------------------------*/

  RRC_TC_CMD_BASE = 0x06000000,  /* TC commands start here */
  RRC_TC_MODE_CHANGE,
  RRC_TC_TEST_LOOPBACK_IND,
  /* PLEASE ADD ALL NEW TC Commands HERE */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by RR                 */
/*--------------------------------------------------------*/

  RRC_RR_CMD_BASE = RRC_RR_CMD_ID_BASE, /* RR commands starting
                                           value(0x07000000),
                                           the actual command Ids are
                                           defined in rrc_rr_types.h */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by LSM */
/*--------------------------------------------------------*/

  RRC_LCS_CMD_BASE = 0x08000000,  /* LCS commands start here */
  RRC_MEASUREMENT_IND, /*0x08000001  A-GPS Meas Report */

  RRC_GPS_CELL_ID_REQUEST, /*0x08000002*/

  /*Command Ids of RRC commands Sent by CM.*/
  RRC_CM_CMD_BASE = 0x09000000,

  /*CM will request RRC to read CID in DCH state*/
  RRC_CM_DCH_CELL_ID_REQUEST,

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by QChat              */
/*--------------------------------------------------------*/

  /* QChat commands starting value(0x0B000000) */
  RRC_QCHAT_CMD_BASE = 0x0B000000,
  
  /* 0x0B000001 */
  RRC_QCHAT_HANDLE_CELL_UPDATE,

/*--------------------------------------------------------*/
/* Common command for all the UMIDs sent by LTE RRC                */
/*--------------------------------------------------------*/
  RRC_LTE_RRC_CMD = 0x0C000000,

/*--------------------------------------------------------*/
/* Common command for all the Free floating sent by L1 */
/*--------------------------------------------------------*/
  RRC_L1_FF_CMD_BASE = 0x0D000000,

  RRC_CPHY_DATA_FOR_UI_DISP_CNF,

  RRC_CPHY_RSSI_CNF,

  RRC_CPHY_CHECK_S_CRIT_CNF,

  RRC_CPHY_CHECK_SLEEP_REQ,

  RRC_CPHY_GET_IMM_MEAS_RSP,

  RRC_CPHY_UTRAN_NMR_INFO_CNF,

  RRC_CPHY_ADDITIONAL_MEAS_DATA_RSP,

/*--------------------------------------------------------*/
/* All the commands ids sent by TDS RRC are present in rrc_tdsrrc_api.h */
/*--------------------------------------------------------*/
  RRC_TDSRRC_CMD_ID_BEGIN = RRC_TDSRRC_CMD_ID_BASE,

  RRC_TDSRRC_CMD_ID_END = 0x0EFFFFFF,

/*--------------------------------------------------------*/
/* Common command for all the UMIDs sent by QMI RRC                */
/*--------------------------------------------------------*/
  RRC_QMI_CMD = 0x0F000000,
/*--------------------------------------------------------*/
/* End of RRC command ids                                 */
/*--------------------------------------------------------*/
  RRC_MAX_CMDS
}rrc_cmd_e_type;
/*lint -restore */

/* Below #defines are added for logging purpose only */
  #define rrc_cmd_e_type_value_0x1000000 RRC_INT_CMD_BASE   

   
  #define rrc_cmd_e_type_value_0x1000001 RRC_DOWNLINK_SDU_IND
   
  #define rrc_cmd_e_type_value_0x1000002 RRC_GET_ALL_SIBS_REQ

  #define rrc_cmd_e_type_value_0x1000003 RRC_DOWNLINK_L2ACK_IND

  #define rrc_cmd_e_type_value_0x1000004 RRC_RESET_L1_REQ

  #define rrc_cmd_e_type_value_0x1000005 RRC_RESET_L1_CNF

  #define rrc_cmd_e_type_value_0x1000006 RRC_GET_ALL_SIBS_CNF

  #define rrc_cmd_e_type_value_0x1000007 RRC_GET_SPECIFIC_SIBS_REQ

  #define rrc_cmd_e_type_value_0x1000008 RRC_GET_SPECIFIC_SIBS_CNF

  #define rrc_cmd_e_type_value_0x1000009 RRC_TIMER_TO_CAMP_ON_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x100000a RRC_WAIT_TIMER_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x100000b RRC_PHY_CHAN_FAILURE_TIMER_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x100000c RRC_RRC_CON_DELAY_TIMER_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x100000d RRC_T300_EXPIRED_IND
  
  #define rrc_cmd_e_type_value_0x100000e RRC_T308_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x100000f RRC_T304_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000010 RRC_T302_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000011 RRC_T305_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000012 RRC_T316_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000013 RRC_T317_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000014 RRC_T307_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000015 RRC_T314_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000016 RRC_T315_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000017 RRC_TIMER_FOR_RCE_L2ACK_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000018 RRC_TIMER_FOR_RCR_L2ACK_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000019 RRC_REDIRECT_REQ

  #define rrc_cmd_e_type_value_0x100001a RRC_REDIRECT_CNF

  /* 0x0100001B: Command posted to State Change Manager when RRC state changes. */
  #define rrc_cmd_e_type_value_0x100001b RRC_NEW_STATE_IND

  /* 0x0100001C: Command poster by State Change Manager to other RRC procedures
   * to notify them of the state transitions.
   */
  #define rrc_cmd_e_type_value_0x100001c RRC_STATE_CHANGE_IND

  /* 0x0100001D: Command posted to Cell Change manager requesting a Cell Change */
  #define rrc_cmd_e_type_value_0x100001d RRC_CELL_CHANGE_REQ

  /* 0x0100001E: Command posted by Cell Change Manager in response to Cell Change Req.
   * This command is posted to the procedure that requested Cell Change.
   */
  #define rrc_cmd_e_type_value_0x100001e RRC_CELL_CHANGE_CNF

  /* 0x0100001F: Command posted by Cell Change manager to inform procedures of a change
   * in cell.
   */
  #define rrc_cmd_e_type_value_0x100001f RRC_NEW_CELL_IND

  /* 0x01000020: Command posted from any RRC module to LLC to setup/reconfig/release
   * connected mode channels.
   */
  #define rrc_cmd_e_type_value_0x1000020 RRC_CHANNEL_CONFIG_REQ

  /* 0x01000021: Command posted from LLC to any RRC module to confirm a setup/reconfig/release
   * of connected mode channels.
   */
  #define rrc_cmd_e_type_value_0x1000021 RRC_CHANNEL_CONFIG_CNF

  /* 0x01000022: Command used to request any one of a number of L1 or L2 commands as
   * specified in rrc_llc_cmd_type
   */
  #define rrc_cmd_e_type_value_0x1000022 RRC_LLC_CMD_REQ

  /* 0x01000023: Command used to confirm any one of a number of L1 or L2 commands as
   * specified in rrc_llc_cmd_type
   */
  #define rrc_cmd_e_type_value_0x1000023 RRC_LLC_CMD_CNF

  /* 0x01000024: Command used to confirm that CELL UPDATE procedure is completed. This will
   * be in response to RRC_CELL_UPDATE_INITAITE_REQ
   */
  #define rrc_cmd_e_type_value_0x1000024 RRC_CELL_UPDATE_COMPLETE_CNF

  /* 0x01000025: Command used to initiate CELL UPDATE procedure. It includes the reason to for
   * CELL UPDATE Procedure initiation
   */
  #define rrc_cmd_e_type_value_0x1000025 RRC_CELL_UPDATE_INITIATE_REQ

  /* 0x01000026: Command used to inform SIB procedure that SIBs modified. */
  #define rrc_cmd_e_type_value_0x1000026 RRC_UPDATE_SIBS_IND

  /* 0x01000027: Command used to initiate UE Capability Information procedure */
  #define rrc_cmd_e_type_value_0x1000027 RRC_INITIATE_UE_CAPABILITY_REQ

  /* 0x01000028: Command used by the RCE procedure to indicate CSP to exculde previous frequency
   * from the cell reselection for the specified Wait Time
   */
  #define rrc_cmd_e_type_value_0x1000028 RRC_DISABLE_PREV_FREQ_IND

  /* 0x01000029: Command posted by cell selection procedure to measurement procedure
   * to read SIB11/12 and then inform L1 to start measurements
   */
  #define rrc_cmd_e_type_value_0x1000029 RRC_CELL_SELECTED_MEAS_REQ

  /* 0x0100002A: Command used to perform the periodically RSSI&BER report to the CM */
  #define rrc_cmd_e_type_value_0x100002a RRC_TIMER_FOR_RSSI_BER_EXPIRED_IND

  /* 0x0100002B: Command used to trigger cell selection */
  #define rrc_cmd_e_type_value_0x100002b RRC_INITIATE_CELL_SELECTION_IND
  
  /* 0x0100002C: Command to request deactivation of lower layers */
  #define rrc_cmd_e_type_value_0x100002c RRC_LOWER_LAYER_DEACT_REQ

  /* 0x0100002D: Confirmation for lower layer deactivation */
  #define rrc_cmd_e_type_value_0x100002d RRC_LOWER_LAYER_DEACT_CNF

  /* 0x0100002E: Command used to inform the SIB procedure that the SIB wait time has
   * expired
   */
  #define rrc_cmd_e_type_value_0x100002e RRC_SIB_WAIT_TIMER_EXPIRED_IND

  /* 0x0100002F: Command posted by SIB procedure to other procedures when
   * a SIB in the serving cell has been modified by the UTRAN
   */
  #define rrc_cmd_e_type_value_0x100002f RRC_SIB_CHANGE_IND

  /* 0x01000030: Command to RRC LLC update C-RNTI and/or U-RNTI */
  #define rrc_cmd_e_type_value_0x1000030 RRC_RNTI_UPDATE_REQ

  /* 0x01000031: Command to Confirm the updatation of C-RNTI and/or U-RNTI */
  #define rrc_cmd_e_type_value_0x1000031 RRC_RNTI_UPDATE_CNF

  /* 0x01000032: Command to stop WCDMA mode in the lower layers */
  #define rrc_cmd_e_type_value_0x1000032 RRC_STOP_LOWER_LAYER_REQ

  /* 0x01000033: Command to Confirm the stop of WCDMA mode */
  #define rrc_cmd_e_type_value_0x1000033 RRC_STOP_LOWER_LAYER_CNF

  /* 0x01000034: Command to notify a RRC mode transition */
  #define rrc_cmd_e_type_value_0x1000034 RRC_MODE_CHANGE_IND

  /* 0x01000035: Command to RRC LLC for cell reselection */
  #define rrc_cmd_e_type_value_0x1000035 RRC_CELL_RESELECTION_REQ

  /* 0x01000036: Command to confirm the reselection request */
  #define rrc_cmd_e_type_value_0x1000036 RRC_CELL_RESELECTION_CNF

  /* 0x01000037: Command to give the SIB7 update status. Posted by
   * the SIB procedure to other RRC procedures
   */
  #define rrc_cmd_e_type_value_0x1000037 RRC_SIB7_UPDATE_STATUS_IND

  /* 0x01000038: Command to indicate that the SIB7 expiration
   * timer has expired.
   */
  #define rrc_cmd_e_type_value_0x1000038 RRC_SIB7_EXP_TIMER_EXPIRED_IND

  /* 0x01000039: Command from LLC to indcate end of reconfig procedure
   * and SMC can continue security configuration
   */
  #define rrc_cmd_e_type_value_0x1000039 RRC_RECONFIG_COMPLETE_IND

  /* 0x0100003A: Command from LLC to indcate configuration of lower layers
   * with ciphering information passed in ciph_config_req
   */
  #define rrc_cmd_e_type_value_0x100003a RRC_CIPH_CONFIG_REQ

  /* 0x0100003B: Command from LLC to indcate configuration of lower layers
   * with ciphering information passed in ciph_config_req
   */
  #define rrc_cmd_e_type_value_0x100003b RRC_CIPH_CONFIG_CNF

  /* 0x0100003C: Command to be send from SMC to LLC to abort ongoing
   * ciphering configuraiton at RLC and revert to old set of keys.
   * NOTE: There is no CNF associated with this command.
   */
  #define rrc_cmd_e_type_value_0x100003c RRC_ABORT_CIPH_CONFIG_REQ

  /* 0x0100003D: Command from Timer to indicate that the timer to ignore
   * BCCH Modification info has expired
   */
  #define rrc_cmd_e_type_value_0x100003d RRC_IGNORE_BCCH_MOD_TIMER_EXPIRED_IND

  /* 0x0100003E: Command from Timer to indicate that the timer for cell
   * selection going from DCH->FACH has expired
   */
  #define rrc_cmd_e_type_value_0x100003e RRC_DCH_FACH_CELL_SELECTION_TIMER_EXPIRED_IND

  /* 0x0100003F: Command from Procedure to SCRR to release Signalling Connections */
  #define rrc_cmd_e_type_value_0x100003f RRC_SIG_CONN_RELEASE_REQ

  /* 0x01000040: Command from SCRR to Procedure confirming that Signalling Connections
   * are released
   */
  #define rrc_cmd_e_type_value_0x1000040 RRC_SIG_CONN_RELEASE_CNF

  /* 0x01000041: */
  #define rrc_cmd_e_type_value_0x1000041 RRC_CHANGE_MODE_REQ

  /* 0x01000042: */
  #define rrc_cmd_e_type_value_0x1000042 RRC_CHANGE_MODE_CNF

  /* 0x01000043: */
  #define rrc_cmd_e_type_value_0x1000043 RRC_NAS_DATA_TRANSFER_IND

  /* 0x01000044: */
  #define rrc_cmd_e_type_value_0x1000044 RRC_REVERT_TO_UTRAN_CONFIG_REQ

  /* 0x01000045: */
  #define rrc_cmd_e_type_value_0x1000045 RRC_REVERT_TO_UTRAN_CONFIG_CNF

  /* 0x01000046: */
  #define rrc_cmd_e_type_value_0x1000046 RRC_DELETE_MEAS_REQ

  /* 0x01000047: */
  #define rrc_cmd_e_type_value_0x1000047 RRC_COMPRESSED_MODE_IND

  /* 0x01000048: */
  #define rrc_cmd_e_type_value_0x1000048 RRC_CSP_ACQ_REQ

  /* 0x01000049: */
  #define rrc_cmd_e_type_value_0x1000049 RRC_CSP_ACQ_CNF

  /* 0x0100004A: */
  #define rrc_cmd_e_type_value_0x100004a RRC_HO_CIPH_REQ

  /* 0x0100004B: */
  #define rrc_cmd_e_type_value_0x100004b RRC_HO_CIPH_CNF

  /* 0x0100004C: Command from Timer to indicate that the timer for detection
   * of L1 deadlock condition has expired.
   */
  #define rrc_cmd_e_type_value_0x100004c RRCTMR_L1_DEADLOCK_DETECT_TIMER_EXPIRED_IND

  /* 0x0100004D: Command from Timer to indicate that the timer to delay ERR_FATAL
   * for CPHY_SETUP_CNF or CPHY_CELL_TRANSITION_CNF has expired
   */
  #define rrc_cmd_e_type_value_0x100004d RRCTMR_DELAY_ERR_FATAL_TIMER_EXPIRED_IND

  /* 0x0100004E: IND to inform UDT procedure that SMC has gotten L2 ACK and it is safe to
   * transmit Uplink Messages
   */
  #define rrc_cmd_e_type_value_0x100004e RRC_SMC_DDT_L2_ACK_IND

  /* 0x0100004F: Command from TMR to indicated that the timer for cell
   * selection from DCH to CELL_PCH or URA_PCH has expired
   */
  #define rrc_cmd_e_type_value_0x100004f RRC_DCH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND

  /* 0x01000050: Req/CNF between Reconfig-Procedures and SMC to trigger re-establishment
   * of RLC radio bearers
   */
  #define rrc_cmd_e_type_value_0x1000050 RRC_RE_EST_RLC_FOR_SRNS_REQ

  /* 0x01000051: */
  #define rrc_cmd_e_type_value_0x1000051 RRC_RE_EST_RLC_FOR_SRNS_CNF

  /* 0x01000052: Req/CNF between SMC-LLC to re-establish RLC radio bearers
   */
  #define rrc_cmd_e_type_value_0x1000052 RRC_LLC_RE_EST_RLC_REQ

  /* 0x01000053: */
  #define rrc_cmd_e_type_value_0x1000053 RRC_LLC_RE_EST_RLC_CNF

  /* 0x01000054: */
  #define rrc_cmd_e_type_value_0x1000054 RRC_ERR_FATAL_IND

  /* 0x01000055: */
  #define rrc_cmd_e_type_value_0x1000055 RRC_T3174_EXPIRED_IND

  /* 0x01000056: */
  #define rrc_cmd_e_type_value_0x1000056 RRC_CSP_CAMP_REQ

  /* 0x01000057: */
  #define rrc_cmd_e_type_value_0x1000057 RRC_CSP_CAMP_CNF

  /* 0x01000058: */
  #define rrc_cmd_e_type_value_0x1000058 RRC_CONN_SETUP_IND

  /* PLEASE ADD ALL NEW Internal Commands HERE */

  /* 0x01000059: Command used to initiate URA UPDATE procedure. It includes the reason to for
   * URA UPDATE Procedure initiation
   */
  #define rrc_cmd_e_type_value_0x1000059 RRC_URA_UPDATE_INITIATE_REQ

  /* 0x0100005A: Command used to confirm that URA UPDATE procedure is completed. This will
   * be in response to RRC_URA_UPDATE_INITAITE_REQ
   */
  #define rrc_cmd_e_type_value_0x100005a RRC_URA_UPDATE_COMPLETE_CNF

  /* 0x0100005B: Command used to indicate to CSP that full frequency scan can be initiated */
  #define rrc_cmd_e_type_value_0x100005b RRC_FREQ_SCAN_TIMER_EXPIRED_IND

  /* 0x0100005C: Command used to indicate that OOS timer is expired */
  #define rrc_cmd_e_type_value_0x100005c RRC_OOS_TIMER_EXPIRED_IND

  /* 0x0100005D: Command used to indicate that Deep Sleep no service timer has expired */
  #define rrc_cmd_e_type_value_0x100005d RRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND

  /* 0x0100005E: Command used to indicate to CSP that WCDMA has been resumed (by CHO) */
  #define rrc_cmd_e_type_value_0x100005e RRC_CSP_WCDMA_RESUMED_IND

  /* 0x0100005F: Command used to indicate that BPLMN 2G->3G search timer has expired */
  #define rrc_cmd_e_type_value_0x100005f RRC_BPLMN_SRCH_TIMER_EXPIRED_IND

  /* 0x01000060: Command used to indicate that BPLMN 2G->3G full scan timer has expired */
  #define rrc_cmd_e_type_value_0x1000060 RRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND

  /* 0x01000061: Command used to indicate that the wait timer started for the last camped
   * frequency during inter frequency redirection has expired.
   */
  #define rrc_cmd_e_type_value_0x1000061 RRC_FREQ_REDIRECT_WAIT_TIMER_EXPIRED_IND

  /* 0x01000062: Command used to abort interrat operation when a higher priority request
   * needs to be processed
   */
  #define rrc_cmd_e_type_value_0x1000062 RRC_IRAT_ABORT_IND

  /* 0x01000063: Command used to indicate that the 30s specific PLMN search timer when
   * UE is OOS in Connected Mode has expired
   */
  #define rrc_cmd_e_type_value_0x1000063 RRC_CONN_MODE_OOS_TIMER_EXPIRED_IND

  /* 0x01000064: Command used to indicate that the inactivity timer when
   * has expired
   */
   #define rrc_cmd_e_type_value_0x1000064 RRC_INACTIVITY_TIMER_EXPIRED_IND

  /* 0x01000065: Command used for indicating the completion of IDT procedure for a particular domain
   * This is used by release procedure
   */
  #define rrc_cmd_e_type_value_0x1000065 RRC_IDT_COMPLETED_FOR_DOMAIN

  /* 0x01000066: Command used to indicate that BPLMN 3G->3G search timer has expired */
  #define rrc_cmd_e_type_value_0x1000066 RRC_WTOW_BPLMN_DRX_SRCH_TIMER_EXPIRED_IND

  /* 0x01000067: Command used to indicate that MAX BPLMN 3G->3G search timer allowed has expired */
  #define rrc_cmd_e_type_value_0x1000067 RRC_BPLMN_GUARD_SRCH_TIMER_EXPIRED_IND

  /* 0x01000068: Command used to indicate that BPLMN 3G->3G2G search can be started */
  #define rrc_cmd_e_type_value_0x1000068 RRC_BPLMN_SEARCH_RESUME_IND

  /* 0x01000069: Command sent by other procedures to CSP to suspend the BPLMN search */
  #define rrc_cmd_e_type_value_0x1000069 RRC_BPLMN_SUSPEND_REQ

  /* 0x0100006A: Command sent by CSP to the procedure that sent suspend request for suspending BPLMN search */
  #define rrc_cmd_e_type_value_0x100006a RRC_BPLMN_SUSPEND_CNF

  /* 0x0100006B: command sent from MCM to Meas and UECI on RRC_CHANGE_MODE_IND */
  #define rrc_cmd_e_type_value_0x100006b RRC_MODE_STATUS_IND

  /* 0x0100006C: Command used to indicate for updating six hrs sib timer*/
  #define rrc_cmd_e_type_value_0x100006c RRC_SIX_HRS_SIB_TIMER_UPDATE_IND

  /* 0x0100006D: Command used to indicate that Uniform OOS search timer has expired */
  #define rrc_cmd_e_type_value_0x100006d RRC_UNIFORM_OOS_SEARCH_TIMER_EXPIRED_IND

  /* 0x0100006E: Command used to put UE into deep sleep when UE is OOS in Connected Mode
   */
  #define rrc_cmd_e_type_value_0x100006e RRC_CONN_MODE_DEEP_SLEEP_REQ

  /* 0x0100006F: Command used to wakeup UE from deep sleep when UE is OOS in Connected Mode
   */
  #define rrc_cmd_e_type_value_0x100006f RRC_CONN_MODE_DEEP_SLEEP_WAKEUP_REQ

  /* 0x01000070: Command used to indicate that the initial Tmins no deep sleep
   * timer has expired. The timer is started when UE goes OOS in Connected Mode.
   */
  #define rrc_cmd_e_type_value_0x1000070 RRC_CONN_MODE_OOS_INITIAL_NO_DS_TIMER_EXPIRED_IND

  /* 0x01000071: Command used to indicate that the deep sleep timer has expired and UE
   * should now wake up and search for service again
   */
  #define rrc_cmd_e_type_value_0x1000071 RRC_CONN_MODE_OOS_DS_TIMER_EXPIRED_IND

  /* 0x01000072:  */
  #define rrc_cmd_e_type_value_0x1000072 RRC_RB_PROC_COMPL_IND

  /* 0x01000073: Command used to indicate SIB change Gaurd timer expired*/
  #define rrc_cmd_e_type_value_0x1000073 RRC_SIB_CHANGE_GUARD_TIMER_EXPIRED_IND

  /* 0x01000074: this command is sent by LLC to CSP after receiving START_CNF
   * from L1 in Connected Mode
   */
  #define rrc_cmd_e_type_value_0x1000074 RRC_CONN_MODE_DEEP_SLEEP_WAKEUP_CNF

  /*0x01000075: Command used to indicate BCCH Modification timer expiry*/
  #define rrc_cmd_e_type_value_0x1000075 RRC_BCCH_MODIFICATION_TIMER_EXPIRED_IND

  /* 0x01000076: Command used to indicate that  Manual PLMN search full scan timer has expired */
  #define rrc_cmd_e_type_value_0x1000076 RRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND

  /* 0x01000077: Command from timer proc to indicate that the timer for cell
   * selection going from FACH->FACH/PCH has expired
   */
  #define rrc_cmd_e_type_value_0x1000077 RRC_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND

  /* 0x01000080: Command posted by RRC when GSTK queries RRC for
   * Network Measurement information
   */
  #define rrc_cmd_e_type_value_0x1000078 RRC_GSTK_UTRAN_NMR_INFO_REQ

  /* 0x01000081: Command from timer proc to indicate that the timer for cell
   * selection on GtoW redirection has expired.
   */
  #define rrc_cmd_e_type_value_0x1000079 RRC_GTOW_REDIRECTION_WAIT_TIMER_EXPIRED_IND
  

  /* 0x01000082: Command from timer proc to indicate the guard timer for 
   * abort W2G BPLMN has expired.
   */
  #define rrc_cmd_e_type_value_0x100007a RRC_WTOG_ABORT_GUARD_TIMER_EXPIRED_IND

  /* 0x01000083: Command from CSP proc to indicate MCM can go ahead with 
   * stop wcdma request
   */
  #define rrc_cmd_e_type_value_0x100007b RRC_CONTINUE_STOP_MODE_REQ

  /*0x01000084: Command for expiry of timer T320. T320 is the wait timer for CU. 
        UTRAN may use this timer to queue the UE in Cell_PCH or URA_PCH state.*/
  #define rrc_cmd_e_type_value_0x100007c RRC_T320_EXPIRED_IND

  /*0x01000085: Command for requesting "Change of UE Capability" thru COUEC proc*/
  #define rrc_cmd_e_type_value_0x100007d RRC_COUEC_REQ
  
  /*0x01000086: Command for confirmation of "Change of UE Cap." request*/
  #define rrc_cmd_e_type_value_0x100007e RRC_COUEC_CNF

  /*0x01000087: Command for expiry of COUEC timer*/
  #define rrc_cmd_e_type_value_0x100007f RRC_TIMER_COUEC_EXPIRED_IND

  /*0x01000088 : Command used to indicate T319 expiry*/
  #define rrc_cmd_e_type_value_0x1000080 RRC_T319_EXPIRED_IND

  /*0x01000089: Command used to indicate that Dedicated priority validity timer expired*/
  #define rrc_cmd_e_type_value_0x1000081 RRC_DED_PRI_VALDITY_TIMER_EXPIRED_IND

  /*0x01000090: Command used to indicate that Dedicated priority info has changed*/
  #define rrc_cmd_e_type_value_0x1000082 RRC_DED_PRI_CHANGE_IND

  #define rrc_cmd_e_type_value_0x1000083 RRC_WRM_PREEMPT_IND

  #define rrc_cmd_e_type_value_0x1000084 RRC_NO_RESOURCE_AVAILABLE_FROM_L1

  /*0x0100008B : Command used to indicate T323 expiry*/
  #define rrc_cmd_e_type_value_0x1000085 RRC_T323_EXPIRED_IND

/* Command used to indicate that EOOS full scan timer has expired */
  #define rrc_cmd_e_type_value_0x1000086 RRC_EOOS_FULL_SCAN_TIMER_EXPIRED_IND

/* Command used to indicate WRM Unlock_By timer expired */
  #define rrc_cmd_e_type_value_0x1000087 RRC_DUAL_SIM_WRM_UNLOCK_BY_TIMER_EXP_IND

  #define rrc_cmd_e_type_value_0x1000088 RRC_QCHAT_FA_RRC_TIMER_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000089 RRC_IHO_OPEN_DOMAIN_IND

  #define rrc_cmd_e_type_value_0x100008a RRC_LTE_TO_WCDMA_SRCH_TIMER_EXPIRY_IND


  #define rrc_cmd_e_type_value_0x100008b RRC_INTERNAL_CSFB_ABORT_REQ

/* Command used to initiate RLC_UL_DATA_IND procedure */
  #define rrc_cmd_e_type_value_0x100008c RRC_RLC_UL_DATA_IND

  //#define rrc_cmd_e_type_value_0x100008x RRC_CU_TRANS_TO_DISCON_TIMER_EXPIRED
  #define rrc_cmd_e_type_value_0x100008d RRC_GTOW_REDIRECTION_LFS_TIMER_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x100008e RRC_CSG_ASF_SRCH_TIMER_EXPIRY_IND

  #define rrc_cmd_e_type_value_0x100008f RRC_CU_TRANS_TO_DISCON_TIMER_EXPIRED

  #define rrc_cmd_e_type_value_0x1000090 RRC_SIB_STOP_PARTITION_TMR_EXPIRED_IND
 
  #define rrc_cmd_e_type_value_0x1000091 RRC_SIB_MASK_INVERT_TMR_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000092 RRC_SIB_EMPTY_MASK_TMR_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000093 RRCTMR_ACT_RSP_EF_TIMER_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000094 RRCTMR_WT_STATE_CHANGE_EF_TIMER_EXPIRED_IND

  #define rrc_cmd_e_type_value_0x1000095 RRC_QSH_PROCESS_CMD
/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by L1                 */
/*--------------------------------------------------------*/

  #define rrc_cmd_e_type_value_0x2000000 RRC_L1_CMD_BASE  /* L1 commands start here */

  /* 0x02000001 */
  #define rrc_cmd_e_type_value_0x2000001 RRC_CPHY_ACQ_CNF

  /* 0x02000002 */
  #define rrc_cmd_e_type_value_0x2000002 RRC_CPHY_SETUP_CNF

  /* 0x02000003 */
  #define rrc_cmd_e_type_value_0x2000003  RRC_CPHY_ERROR_IND

  /* 0x02000004 */
  #define rrc_cmd_e_type_value_0x2000004 RRC_CPHY_IDLE_CNF

  /* 0x02000005 */
  #define rrc_cmd_e_type_value_0x2000005 RRC_CPHY_CELL_SELECTION_CNF

  /* 0x02000006 */
  #define rrc_cmd_e_type_value_0x2000006 RRC_CPHY_CELL_RESELECTION_IND

  /* 0x02000007 */
  #define rrc_cmd_e_type_value_0x2000007 RRC_CPHY_INTER_FREQ_BCH_CNF

  /* 0x02000008 */
  #define rrc_cmd_e_type_value_0x2000008 RRC_CPHY_MEASUREMENT_IND

  /* 0x02000009 */
  #define rrc_cmd_e_type_value_0x2000009 RRC_CPHY_CAPABILITY_CNF

  /* 0x0200000A */
  #define rrc_cmd_e_type_value_0x200000a RRC_CPHY_INTEGRITY_KEY_MAUC_CNF

  /* 0x0200000B */
  #define rrc_cmd_e_type_value_0x200000b RRC_CPHY_PHYCHAN_ESTABLISHED_IND

  /* 0x0200000C */
  #define rrc_cmd_e_type_value_0x200000c RRC_CPHY_IN_SYNC_IND

  /* 0x0200000D */
  #define rrc_cmd_e_type_value_0x200000d RRC_CPHY_OUT_OF_SYNC_IND

  /* 0x0200000E */
  #define rrc_cmd_e_type_value_0x200000e RRC_CPHY_RL_FAILURE_IND

  /* 0x0200000F */
  #define rrc_cmd_e_type_value_0x200000f RRC_CPHY_IMMEDIATE_MEAS_CNF

  /* 0x02000010 */
  #define rrc_cmd_e_type_value_0x2000010 RRC_CPHY_IMMEDIATE_RESELECTION_CNF

  /* 0x02000011 */
  #define rrc_cmd_e_type_value_0x2000011 RRC_CPHY_CELL_TRANSITION_CNF

  /* 0x02000012 */
  #define rrc_cmd_e_type_value_0x2000012 RRC_CPHY_FREQ_SCAN_CNF

  /* 0x02000013 */
  #define rrc_cmd_e_type_value_0x2000013 RRC_CPHY_DRX_CNF

  /* 0x02000014 */
  #define rrc_cmd_e_type_value_0x2000014 RRC_CPHY_OUT_OF_SERVICE_AREA_IND

  /* 0x02000015 */
  #define rrc_cmd_e_type_value_0x2000015 RRC_CPHY_DEACTIVATE_CNF

  /* 0x02000016 */
  #define rrc_cmd_e_type_value_0x2000016 RRC_CPHY_MEAS_CTRL_CNF

  /* 0x02000017 */
  #define rrc_cmd_e_type_value_0x2000017 RRC_CPHY_ACT_CANCEL_CNF

  /* 0x02000018 */
  #define rrc_cmd_e_type_value_0x2000018 RRC_CPHY_START_WCDMA_MODE_CNF

  /* 0x02000019 */
  #define rrc_cmd_e_type_value_0x2000019 RRC_CPHY_STOP_WCDMA_MODE_CNF

  /* 0x0200001A */
  #define rrc_cmd_e_type_value_0x200001a RRC_CPHY_SUSPEND_WCDMA_MODE_CNF

  /* 0x0200001B */
  #define rrc_cmd_e_type_value_0x200001b RRC_CPHY_RESUME_WCDMA_MODE_CNF

  /* 0x0200001C */
  #define rrc_cmd_e_type_value_0x200001c RRC_CPHY_COMPRESSED_MODE_IND

  /* 0x0200001D */
  #define rrc_cmd_e_type_value_0x200001d RRC_CPHY_OPERATIONAL_ERROR_IND

  /* 0x0200001E */
  #define rrc_cmd_e_type_value_0x200001e RRC_CPHY_DL_WEAK_IND

  /* 0x020000027 indicate RRC to initiate sending the measurement results 
  *on RACH in uplink by transitioning to eFACH state*/
  #define rrc_cmd_e_type_value_0x200001f RRC_CPHY_EPCH_TO_EFACH_TRANSITION_IND

  /*Nudging RRC means telling RRC to bring up PCCPCH_S so we can reestablish
   *timing of the network
   */
  #define rrc_cmd_e_type_value_0x2000020 RRC_CPHY_NUDGE_FOR_SFN_MISS_MATCH_REQ

  /* modem stat response from L1 to RRC */
  #define rrc_cmd_e_type_value_0x2000021 RRC_CPHY_MODEM_STATISTICS_RSP

  /* MDSP halt indication */
  #define rrc_cmd_e_type_value_0x2000022 RRC_CPHY_MDSP_HALT_IND
  /* WL1 recovery CNF*/
  #define rrc_cmd_e_type_value_0x2000023 RRC_CPHY_RECOVER_WCDMA_CNF
  
  #define rrc_cmd_e_type_value_0x2000024 RRC_CPHY_ACTIVATION_TIMER_EXPIRY_IND,
  /* WL1 has started QTA*/
  #define rrc_cmd_e_type_value_0x2000025 RRC_CPHY_START_QTA_CNF,
  /* WL1 has come out of QTA*/
  #define rrc_cmd_e_type_value_0x2000026 RRC_CPHY_STOP_QTA_CNF,

  /* PLEASE ADD ALL NEW L1 Commands HERE */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by MAC                */
/*--------------------------------------------------------*/

  #define rrc_cmd_e_type_value_0x3000000 RRC_MAC_CMD_BASE   /* MAC commands start here */
  #define rrc_cmd_e_type_value_0x3000001 RRC_CMAC_UL_HFN_CNF
  #define rrc_cmd_e_type_value_0x3000002 RRC_CMAC_DL_HFN_CNF
  #define rrc_cmd_e_type_value_0x3000003 RRC_CMAC_UL_TRAFFIC_IND
  #define rrc_cmd_e_type_value_0x3000004 RRC_CMAC_STATUS_IND
  #define rrc_cmd_e_type_value_0x3000005 RRC_CMAC_MEAS_CTRL_CNF
  #define rrc_cmd_e_type_value_0x3000006 RRC_CMAC_UL_TFC_SUBSET_CFG_CNF
  /* PLEASE ADD ALL NEW MAC Commands HERE */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by RLC                */
/*--------------------------------------------------------*/

  #define rrc_cmd_e_type_value_0x4000000 RRC_RLC_CMD_BASE  /* RLC commands start here */
  #define rrc_cmd_e_type_value_0x4000001 RRC_CRLC_STATUS_IND
  #define rrc_cmd_e_type_value_0x4000002 RRC_CRLC_UL_HFN_CNF
  #define rrc_cmd_e_type_value_0x4000003 RRC_CRLC_DL_HFN_CNF
  #define rrc_cmd_e_type_value_0x4000004 RRC_CRLC_DL_TM_CONFIG_CNF
  #define rrc_cmd_e_type_value_0x4000005 RRC_CRLC_UL_TM_CONFIG_CNF
  #define rrc_cmd_e_type_value_0x4000006 RRC_CRLC_DL_UM_CONFIG_CNF
  #define rrc_cmd_e_type_value_0x4000007 RRC_CRLC_UL_UM_CONFIG_CNF
  #define rrc_cmd_e_type_value_0x4000008 RRC_CRLC_AM_CONFIG_CNF
  #define rrc_cmd_e_type_value_0x4000009 RRC_CRLC_RELEASE_ALL_CNF
  #define rrc_cmd_e_type_value_0x400000a RRC_CRLC_RESUME_CNF
  /* PLEASE ADD ALL NEW RLC Commands HERE */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by MM                 */
/*--------------------------------------------------------*/

  #define rrc_cmd_e_type_value_0x5000000 RRC_MM_CMD_BASE  /* MM commands start here */

  /* 0x05000001*/
  #define rrc_cmd_e_type_value_0x5000001 RRC_SERVICE_REQ

  /* 0x05000002 */
  #define rrc_cmd_e_type_value_0x5000002 RRC_EST_REQ

  /* 0x05000003 */
  #define rrc_cmd_e_type_value_0x5000003 RRC_DATA_REQ

  /* 0x05000004 */
  #define rrc_cmd_e_type_value_0x5000004 RRC_OPEN_SESSION_REQ

  /* 0x05000005 */
  #define rrc_cmd_e_type_value_0x5000005 RRC_CLOSE_SESSION_REQ

  /* 0x05000006 */
  #define rrc_cmd_e_type_value_0x5000006 RRC_ABORT_REQ

  /* 0x05000007 */
  #define rrc_cmd_e_type_value_0x5000007 RRC_PLMN_LIST_REQ

  /* 0x05000008 */
  #define rrc_cmd_e_type_value_0x5000008 RRC_ACT_REQ

  /* 0x05000009 */
  #define rrc_cmd_e_type_value_0x5000009 RRC_DEACT_REQ

  /* 0x0500000A */
  #define rrc_cmd_e_type_value_0x500000a RRC_STOP_WCDMA_MODE_REQ

  /* 0x0500000B */
  #define rrc_cmd_e_type_value_0x500000b RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ

  /* 0x0500000C */
  #define rrc_cmd_e_type_value_0x500000c RRC_INVALIDATE_SIM_DATA_REQ

  /* 0x0500000D */
  #define rrc_cmd_e_type_value_0x500000d RRC_SIM_INSERTED_REQ

  /* 0x0500000E */
  #define rrc_cmd_e_type_value_0x500000e RRC_SIM_UPDATE_REQ

  /* 0x0500000F */
  #define rrc_cmd_e_type_value_0x500000f RRC_ACTIVATION_RSP

  /* 0x05000010 */
  #define rrc_cmd_e_type_value_0x5000010 RRC_CHANGE_MODE_IND

  /* 0x05000011 */
  #define rrc_cmd_e_type_value_0x5000011 RRC_MODE_CHANGE_REQ

  /* 0x05000012 */
  #define rrc_cmd_e_type_value_0x5000012 RRC_EQ_PLMN_LIST_CHANGE_IND

  /* 0x05000013 */
  #define rrc_cmd_e_type_value_0x5000013 RRC_NW_SEL_MODE_RESET_IND

  /* 0x05000014 */
  #define rrc_cmd_e_type_value_0x5000014 RRC_BPLMN_SEARCH_ABORT_REQ    /* Added for WTOW/WTOG BPLMN SEARCH ABORT */

  /* 0x05000015 */
  #define rrc_cmd_e_type_value_0x5000015 RRC_EXIT_FROM_PWR_SAVE

  /* 0x05000016 */
  #define rrc_cmd_e_type_value_0x5000016 RRC_DS_STATUS_CHANGE_IND

  /* 0x05000017 */
  #define rrc_cmd_e_type_value_0x5000017 RRC_FAST_DORMANCY_REQ       /*Added for fast dormancy request trigger*/

  /* 0x05000018 */
  #define rrc_cmd_e_type_value_0x5000018 RRC_DS_TUNEAWAY_STATUS_CHANGE_IND

  #define rrc_cmd_e_type_value_0x5000019 RRC_CSFB_CALL_STATUS_IND /*CSFB call status indication*/

  #define rrc_cmd_e_type_value_0x500001a RRC_LTE_RESELECTION_ALLOWED_STATUS_IND

  #define rrc_cmd_e_type_value_0x500001b RRC_LAI_REJECT_LIST_IND

  #define rrc_cmd_e_type_value_0x500001c RRC_TUNEAWAY_ALLOWED_IND
  #define rrc_cmd_e_type_value_0x500001d RRC_SET_DRX_IND
  /* 0x0500001E */
  #define rrc_cmd_e_type_value_0x500001e RRC_CSG_WHITE_LIST_UPDATE_IND,

  /*Used by TDS */
  /* 0x0500001F */
  #define rrc_cmd_e_type_value_0x500001f RRC_UE_MODE_IND,

  /* 0x05000020*/
  #define rrc_cmd_e_type_value_0x5000020 RRC_MODE_PREF_CHANGE_IND,

  /* 0x05000021*/
  #define rrc_cmd_e_type_value_0x5000021 RRC_MULTIMODE_SUBS_CHGD_IND,

  /* 0x05000022*/
  #define rrc_cmd_e_type_value_0x5000022 RRC_PSEUDO_LTE_LIST_UPDATE_IND


/* PLEASE ADD ALL NEW MM Commands HERE */
/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by TC                 */
/*--------------------------------------------------------*/

  #define rrc_cmd_e_type_value_0x6000000 RRC_TC_CMD_BASE  /* TC commands start here */
  #define rrc_cmd_e_type_value_0x6000001 RRC_TC_MODE_CHANGE
  #define rrc_cmd_e_type_value_0x6000002 RRC_TC_TEST_LOOPBACK_IND
  /* PLEASE ADD ALL NEW TC Commands HERE */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by RR                 */
/*--------------------------------------------------------*/

  #define rrc_cmd_e_type_value_0x7000000 RRC_RR_CMD_BASE /* RR commands starting
                                           value(0x07000000)
                                           the actual command Ids are
                                           defined in rrc_rr_types.h */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by LSM */
/*--------------------------------------------------------*/

  #define rrc_cmd_e_type_value_0x8000000 RRC_LCS_CMD_BASE  /* LCS commands start here */
  #define rrc_cmd_e_type_value_0x8000001 RRC_MEASUREMENT_IND /*0x08000001  A-GPS Meas Report */

  #define rrc_cmd_e_type_value_0x8000002 RRC_GPS_CELL_ID_REQUEST /*0x08000002*/

  /*Command Ids of RRC commands Sent by CM.*/
  #define rrc_cmd_e_type_value_0x9000000 RRC_CM_CMD_BASE

  /*CM will request RRC to read CID in DCH state*/
  #define rrc_cmd_e_type_value_0x9000001 RRC_CM_DCH_CELL_ID_REQUEST

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by QChat              */
/*--------------------------------------------------------*/

  /* QChat commands starting value(0x0B000000) */
  #define rrc_cmd_e_type_value_0xb000000 RRC_QCHAT_CMD_BASE
  
  /* 0x0B000001 */
  #define rrc_cmd_e_type_value_0xb000001 RRC_QCHAT_HANDLE_CELL_UPDATE

/*--------------------------------------------------------*/
/* Common command for all the UMIDs sent by LTE RRC                */
/*--------------------------------------------------------*/
  #define rrc_cmd_e_type_value_0xc000000 RRC_LTE_RRC_CMD

/*--------------------------------------------------------*/
/* Common command for all the Free floating sent by L1 */
/*--------------------------------------------------------*/
  #define rrc_cmd_e_type_value_0xd000000 RRC_L1_FF_CMD_BASE

  #define rrc_cmd_e_type_value_0xd000001 RRC_CPHY_DATA_FOR_UI_DISP_CNF

  #define rrc_cmd_e_type_value_0xd000002 RRC_CPHY_RSSI_CNF

  #define rrc_cmd_e_type_value_0xd000003 RRC_CPHY_CHECK_S_CRIT_CNF

  #define rrc_cmd_e_type_value_0xd000004 RRC_CPHY_CHECK_SLEEP_REQ

  #define rrc_cmd_e_type_value_0xd000005 RRC_CPHY_GET_IMM_MEAS_RSP

  #define rrc_cmd_e_type_value_0xd000006 RRC_CPHY_UTRAN_NMR_INFO_CNF

  #define rrc_cmd_e_type_value_0xd000007 RRC_CPHY_ADDITIONAL_MEAS_DATA_RSP

/*--------------------------------------------------------*/
/* End of RRC command ids                                 */
/*--------------------------------------------------------*/
  #define rrc_cmd_e_type_value_0xd000008 RRC_MAX_CMDS



/* L1 enums commands*/
  #define l1_cmd_enum_type_value0x0 CPHY_CAPABILITY_REQ    
  #define l1_cmd_enum_type_value0x1 CPHY_IDLE_REQ                
  #define l1_cmd_enum_type_value0x2 CPHY_DEACTIVATE_REQ         
  #define l1_cmd_enum_type_value0x3 CPHY_STOP_WCDMA_MODE_REQ    
  #define l1_cmd_enum_type_value0x4 CPHY_START_WCDMA_MODE_REQ     
  #define l1_cmd_enum_type_value0x5 CPHY_SUSPEND_WCDMA_MODE_REQ  
  #define l1_cmd_enum_type_value0x6 CPHY_RESUME_WCDMA_MODE_REQ   
  #define l1_cmd_enum_type_value0x7 CPHY_FREQ_SCAN_REQ            
  #define l1_cmd_enum_type_value0x8 CPHY_ACQ_REQ                  
  #define l1_cmd_enum_type_value0x9 CPHY_CELL_PARM_REQ           
  #define l1_cmd_enum_type_value0xa CPHY_ASET_UPDATE_REQ        
  #define l1_cmd_enum_type_value0xb CPHY_HARD_HANDOFF_REQ       
  #define l1_cmd_enum_type_value0xc CPHY_BCH_SCHEDULE_REQ        
  #define l1_cmd_enum_type_value0xd CPHY_SETUP_REQ                
  #define l1_cmd_enum_type_value0xe CPHY_DRX_REQ                 
  #define l1_cmd_enum_type_value0xf CPHY_CIPHERING_KEY_REQ      
  #define l1_cmd_enum_type_value0x10 CPHY_MEASUREMENT_REQ         
  #define l1_cmd_enum_type_value0x11 CPHY_CELL_SELECTION_REQ      
  #define l1_cmd_enum_type_value0x12 CPHY_IMMEDIATE_MEAS_REQ      
  #define l1_cmd_enum_type_value0x13 CPHY_INTEGRITY_KEY_REQ       
  #define l1_cmd_enum_type_value0x14 CPHY_INTEGRITY_KEY_MAUC_REQ  
  #define l1_cmd_enum_type_value0x15 CPHY_IMMEDIATE_RESELECTION_REQ
  #define l1_cmd_enum_type_value0x16 CPHY_CELL_RESELECTION_RSP     
  #define l1_cmd_enum_type_value0x17 CPHY_CELL_TRANSITION_REQ      
  #define l1_cmd_enum_type_value0x18 CPHY_ACT_TIME_CANCEL_REQ      
  /* Commands from MAC */
  #define l1_cmd_enum_type_value0x19 CPHY_UL_TX_CONTROL_REQ        
  #define l1_cmd_enum_type_value0x1a CPHY_MAC_UL_CONFIG_CNF        
  #define l1_cmd_enum_type_value0x1b CPHY_MAC_DL_CONFIG_CNF        
  #define l1_cmd_enum_type_value0x1c CPHY_MAC_DL_CONFIG_AT_L1_FAIL_CNF 

  /* Commands from LSM (Location Services Manager) */
  #define l1_cmd_enum_type_value0x1d CPHY_LSM_TIMETAG_REQ         
  #define l1_cmd_enum_type_value0x1e CPHY_LSM_CELL_MEAS_REQ       

  /* Commands from BMC (Broadcast Multicast Control) */
  #define l1_cmd_enum_type_value0x1f CPHY_BMC_SCHEDULING_REQ       
  #define l1_cmd_enum_type_value0x20 CPHY_BMC_SKIP_REQ             
  #define l1_cmd_enum_type_value0x21 CPHY_BMC_DEACTIVATE_REQ       
  #define l1_cmd_enum_type_value0x22 CPHY_CELL_BAR_REQ             
  #define l1_cmd_enum_type_value0x23 CPHY_UPD_DL_SYNC_PARMS        

  /* Commands for SIBS */
  #define l1_cmd_enum_type_value0x24 CPHY_SIB_SCHED_CHANGE_IND     

  #define l1_cmd_enum_type_value0x25 CPHY_GO_TO_SLEEP_IND          

  /* MBMS related command and indication */
  /* ----------------------------------- */

  /* MCCH DRX request */
  #define l1_cmd_enum_type_value0x26 CPHY_MBMS_MCCH_DRX_REQ         
  /* MBMS IDLE request */
  #define l1_cmd_enum_type_value0x27 CPHY_MBMS_IDLE_REQ            
  /* MBMS Preferred layer (FLC) request */
  #define l1_cmd_enum_type_value0x28 CPHY_MBMS_PL_PARM_REQ          

  #define l1_cmd_enum_type_value0x29 CPHY_UPD_OTA_PARMS_IND         
  /* Commands from QChat */
  #define l1_cmd_enum_type_value0x2a CPHY_QCHAT_START_IMM_ORIG      

  /*Nudging RRC means telling RRC to bring up PCCPCH_S, so we can reestablish
   *timing of the network
   */
  #define l1_cmd_enum_type_value0x2b CPHY_NUDGE_FOR_SFN_MISS_MATCH_RSP 

  /*RRC request L1 to provide info for modem stat*/
  #define l1_cmd_enum_type_value0x2c CPHY_GET_MODEM_STATISTICS_REQ   

  #define l1_cmd_enum_type_value0x2d CPHY_RSSI_REQ                   

  #define l1_cmd_enum_type_value0x2e CPHY_DATA_FOR_UI_DISP_REQ       
  #define l1_cmd_enum_type_value0x2f CPHY_CHECK_SLEEP_RSP            

  #define l1_cmd_enum_type_value0x30 CPHY_CHECK_S_CRIT_REQ           

  #define l1_cmd_enum_type_value0x31 CPHY_NMR_INFO_REQ               
  
  #define l1_cmd_enum_type_value0x32 CPHY_ADDITIONAL_MEAS_DATA_REQ  

  #define l1_cmd_enum_type_value0x33 CPHY_GET_IMM_MEAS_REQ           
  /* Number of External Commands. Should be the last element.*/
  #define l1_cmd_enum_type_value0x38 CPHY_NUM_EXT_REQ_CMDS           



/* L2 Dl enums for logging purpose*/
	#define wcdma_l2_dl_cmd_enum_type_value100 RLCI_DL_TX_AM_STATUS  	/* Copy nak_list_q (DL) to tx_nak_q (UL) */
  
	#define wcdma_l2_dl_cmd_enum_type_value101 CMAC_DL_CONFIG_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value102 CRLC_DL_TM_CONFIG_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value103 CRLC_DL_UM_CONFIG_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value104 RLCI_DL_CONFIG_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value105 RLCI_DL_RESET_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value106 RLCI_UL_RESET_DONE
  
	#define wcdma_l2_dl_cmd_enum_type_value107 RLCI_DL_RELEASE_ALL_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value108 CRLC_DL_CONTINUE_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value109 CRLC_DL_CIPHER_CONFIG_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value110 CRLC_DL_HFN_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value111 RLC_REGISTER_AM_SDU_DISCARD_CALLBACK_REQ 
	#define wcdma_l2_dl_cmd_enum_type_value112 RLC_PROCESS_MAC_HS_RESET_REQ
	#define wcdma_l2_dl_cmd_enum_type_value113 CRLC_DL_ABORT_CIPHER_CONFIG_REQ
  
	#define wcdma_l2_dl_cmd_enum_type_value114 RLC_HANDLE_SDU_DISCARD_TIMEOUT_EVENT
	#define wcdma_l2_dl_cmd_enum_type_value115 RLC_PDCP_CLEANUP_WM,



/*UL L2 enums for logging purpose*/
  #define wcdma_l2_ul_cmd_enum_type_value100 CMAC_UL_CONFIG_REQ       
  #define wcdma_l2_ul_cmd_enum_type_value101 CMAC_UL_TRAFFIC_VOLUME_CONFIG_REQ
  #define wcdma_l2_ul_cmd_enum_type_value102 CMAC_UL_BLOCK_RACH_TRAFFIC_REQ
  #define wcdma_l2_ul_cmd_enum_type_value103 CMAC_UL_TFC_SUBSET_CFG_REQ
  #define wcdma_l2_ul_cmd_enum_type_value104 CMAC_HS_UL_PROCESS_HDR_LOG_PKT_REQ
  #define wcdma_l2_ul_cmd_enum_type_value105 CMAC_HS_UL_PROCESS_ETFCI_STATUS_LOG_PKT_REQ

  #define wcdma_l2_ul_cmd_enum_type_value106 CRLC_UL_TM_CONFIG_REQ
  #define wcdma_l2_ul_cmd_enum_type_value107 CRLC_UL_UM_CONFIG_REQ
  #define wcdma_l2_ul_cmd_enum_type_value108 CRLC_AM_CONFIG_REQ
  #define wcdma_l2_ul_cmd_enum_type_value109 CRLC_AM_RESET_REQ
  #define wcdma_l2_ul_cmd_enum_type_value110 CRLC_UL_CIPHER_CONFIG_REQ

  #define wcdma_l2_ul_cmd_enum_type_value111 CRLC_UL_RESUME_REQ
  #define wcdma_l2_ul_cmd_enum_type_value112 CRLC_UL_HFN_REQ
  #define wcdma_l2_ul_cmd_enum_type_value113 CRLC_RELEASE_ALL_REQ
  #define wcdma_l2_ul_cmd_enum_type_value114 CRLC_RELEASE_ALL_WITH_MBMS_REQ
  #define wcdma_l2_ul_cmd_enum_type_value115 CRLC_UL_REG_FOR_DATA_IND_REQ

  #define wcdma_l2_ul_cmd_enum_type_value116 CRLC_UL_UNREG_FOR_DATA_IND_REQ
  #define wcdma_l2_ul_cmd_enum_type_value117 RLC_REGISTER_AM_SDU_CONFIRM_CALLBACK_REQ
  #define wcdma_l2_ul_cmd_enum_type_value118 RLC_OPEN_RX_WIN
  #define wcdma_l2_ul_cmd_enum_type_value119 RLC_CLOSE_RX_WIN
  #define wcdma_l2_ul_cmd_enum_type_value120 RLCI_UL_RESET_REQ

  #define wcdma_l2_ul_cmd_enum_type_value121 RLCI_DL_CONFIG_CNF
  #define wcdma_l2_ul_cmd_enum_type_value122 RLCI_DL_RESET_DONE
  #define wcdma_l2_ul_cmd_enum_type_value123 RLC_UL_PURGE_WM_REQ
  #define wcdma_l2_ul_cmd_enum_type_value124 CRLC_UL_ABORT_CIPHER_CONFIG_REQ
  #define wcdma_l2_ul_cmd_enum_type_value125 CRLC_UL_CONTINUE_REQ
  
  #define wcdma_l2_ul_cmd_enum_type_value126 RLCI_UL_TX_AM_STATUS
  #define wcdma_l2_ul_cmd_enum_type_value127 RLCI_UL_DUP_RESET_REQ
  #define wcdma_l2_ul_cmd_enum_type_value128 MAC_RACH_STATUS_IND       
  #define wcdma_l2_ul_cmd_enum_type_value129 WCDMA_L2_UL_MAX_CMDS  


  #define rrc_to_mm_sink_e_type_value100 RRC_EST_CNF
  #define rrc_to_mm_sink_e_type_value101 RRC_REL_IND
  #define rrc_to_mm_sink_e_type_value102 RRC_SYNC_IND
  #define rrc_to_mm_sink_e_type_value103 RRC_SERVICE_CNF
  #define rrc_to_mm_sink_e_type_value104 RRC_SERVICE_IND
  #define rrc_to_mm_sink_e_type_value105 RRC_SERVICE_IND_FROM_OTA_MSG,
  #define rrc_to_mm_sink_e_type_value106 RRC_PLMN_LIST_CNF
  #define rrc_to_mm_sink_e_type_value107 RRC_ABORT_IND
  #define rrc_to_mm_sink_e_type_value108 RRC_PAGE_IND
  #define rrc_to_mm_sink_e_type_value109 RRC_DATA_IND
  #define rrc_to_mm_sink_e_type_value110 RRC_OPEN_SESSION_CNF
  #define rrc_to_mm_sink_e_type_value111 RRC_CLOSE_SESSION_IND
  #define rrc_to_mm_sink_e_type_value112 RRC_CLOSE_SESSION_CNF
  #define rrc_to_mm_sink_e_type_value113 RRC_ABORT_CNF
  #define rrc_to_mm_sink_e_type_value114 RRC_DEACT_CNF
  #define rrc_to_mm_sink_e_type_value115 RRC_MODE_CHANGE_CNF
  #define rrc_to_mm_sink_e_type_value116 RRC_STOP_WCDMA_MODE_CNF
  //#define rrc_to_mm_sink_e_type_value117 RRC_STOP_TDSCDMA_MODE_CNF //STOP_TDSCDMA_MODE_CNF_VAL,
  #define rrc_to_mm_sink_e_type_value117 RRC_ACTIVATION_IND
  #define rrc_to_mm_sink_e_type_value118 RRC_INTERRAT_CELL_CHANGE_IND
  
  #define rrc_to_mm_sink_e_type_value119 RRC_HSDPA_HSUPA_CELL_IND
  
  #define rrc_to_mm_sink_e_type_value120 RRC_HSPA_CALL_STATUS_IND
  
  
  
  #define rrc_to_mm_sink_e_type_value121 RRC_CONNECTED_MODE_SERVICE_STATUS_IND
  
  
  #define rrc_to_mm_sink_e_type_value122 RRC_FAST_DORMANCY_CNF

  #define rrc_to_mm_sink_e_type_value123 MM_DATA_IND // RR_DATA_IND
	 

  #define rrc_to_mm_sink_e_type_value124 RRC_WTOG_CCO_COMPLETE_IND
  
  
  #define rrc_to_mm_sink_e_type_value125 RRC_MBMS_CONN_REQ_IND

  #define rrc_to_mm_sink_e_type_value126 RRC_SUITABLE_SEARCH_END_IND
  #define rrc_to_mm_sink_e_type_value127 RRC_DISABLE_BPLMN_IND
  #define rrc_to_mm_sink_e_type_value128 RRC_SRVCC_HANDOVER_FAILURE_IND
  #define rrc_to_mm_sink_e_type_value129 RRC_PCH_STATE_IND
  #define rrc_to_mm_sink_e_type_value130 RRC_MM_SINK_CMD_MAX


  #define l1_ul_phychan_enum_type_value0 L1_UL_PHYCHAN_DPCH
  #define l1_ul_phychan_enum_type_value1 L1_UL_PHYCHAN_PRACH
  #define l1_ul_phychan_enum_type_value2 L1_UL_PHYCHAN_PCPCH
  #define l1_ul_phychan_enum_type_value3 L1_UL_PHYCHAN_HS_PRACH
  #define l1_ul_phychan_enum_type_value4 L1_UL_PHYCHAN_HS_DPCH
  #define l1_ul_phychan_enum_type_value5 L1_UL_PHYCHAN_SEC_DPCH
  #define l1_ul_phychan_enum_type_value6 L1_UL_NUM_PHYCHAN
  #define l1_ul_phychan_enum_type_value7 L1_UL_PHYCHAN_NONE


  #define l1_dl_phychan_enum_type_value0 L1_DL_PHYCHAN_PCCPCH_S
  #define l1_dl_phychan_enum_type_value1 L1_DL_PHYCHAN_PCCPCH_N
  #define l1_dl_phychan_enum_type_value2 L1_DL_PHYCHAN_SCCPCH0
  #define l1_dl_phychan_enum_type_value3 L1_DL_PHYCHAN_SCCPCH1
  #define l1_dl_phychan_enum_type_value4 L1_DL_PHYCHAN_PICH
  #define l1_dl_phychan_enum_type_value5 L1_DL_PHYCHAN_AICH
  #define l1_dl_phychan_enum_type_value6 L1_DL_PHYCHAN_HS_RACH_AICH
  #define l1_dl_phychan_enum_type_value7 L1_DL_PHYCHAN_DPCH
  #define l1_dl_phychan_enum_type_value8 L1_DL_PHYCHAN_HS_RACH_FDPCH
  #define l1_dl_phychan_enum_type_value9 L1_DL_PHYCHAN_FDPCH
  #define l1_dl_phychan_enum_type_value10 L1_DL_PHYCHAN_SEC_FDPCH
  #define l1_dl_phychan_enum_type_value11 L1_DL_PHYCHAN_PDSCH
  #define l1_dl_phychan_enum_type_value12 L1_NUM_DL_PHYCHAN
  #define l1_dl_phychan_enum_type_value13 L1_DL_PHYCHAN_NOCHAN


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/*
** RRC Command header. This header is a part of all commands sent
** to the RRC layer. This contains the command id for the command sent
** to RRC, and a Queue link in order to put the command in the
** appropriate queue
*/

typedef struct
{
  q_link_type   link;        /* Queue link     */
  rrc_cmd_e_type cmd_id;     /* The command Id */
  /* Other items may be added here for debugging purposes.
  this is still TBD. */
  uint8         seq_num;
}rrc_cmd_hdr_type;


typedef union
{
  msgr_hdr_s msg_hdr;
  /*LTOW requests*/
  wcdma_rrc_lte_resel_req_type wcdma_rrc_lte_resel_req;
  wcdma_rrc_lte_abort_resel_req_type wcdma_rrc_lte_abort_resel_req;
  wcdma_rrc_lte_redir_req_type wcdma_rrc_lte_redir_req;
  wcdma_rrc_lte_abort_redir_req_type wcdma_rrc_lte_abort_redir_req;
  lte_irat_plmn_srch_req_s wcmd_rrc_lte_plmn_srch_req;
  lte_irat_abort_plmn_srch_req_s wcdma_rrc_lte_abort_plmn_srch_req;
  wcdma_rrc_lte_utra_capabilities_req_type wcdma_rrc_lte_utra_capabilities_req;
  wcdma_rrc_lte_psho_req_type wcdma_rrc_lte_psho_req;
  wcdma_rrc_lte_abort_psho_req_type wcdma_rrc_lte_abort_psho_req;
  wcdma_rrc_lte_get_cgi_req_type  wcdma_rrc_lte_cgi_req;
  wcdma_rrc_lte_abort_cgi_req_type wcdma_rrc_lte_abort_cgi_req;
  wcdma_rrc_lte_depri_freq_req_type  wcdma_rrc_lte_depri_freq_req;
  /*Responses for WTOL requests*/
  lte_rrc_wcdma_resel_failed_rsp_s lte_rrc_wcdma_resel_failed_rsp;
  lte_rrc_wcdma_abort_resel_rsp_s lte_rrc_wcdma_abort_resel_rsp;
  lte_rrc_wcdma_redir_failed_rsp_s lte_rrc_wcdma_redir_failed_rsp;
  lte_rrc_wcdma_abort_redir_rsp_s lte_rrc_wcdma_abort_redir_rsp;
  lte_irat_plmn_srch_rsp_s lte_rrc_wcdma_plmn_srch_rsp;
  lte_irat_abort_plmn_srch_rsp_s lte_rrc_wcdma_abort_plmn_srch_rsp;
  lte_rrc_eutra_capabilities_rsp_s lte_rrc_eutra_capabilites_rsp;
  lte_rrc_get_dedicated_pri_rsp_s lte_rrc_get_dedicated_pri_rsp;
  lte_rrc_wcdma_psho_rsp_s  lte_rrc_wcdma_psho_rsp;
  lte_rrc_wcdma_abort_psho_rsp_s  lte_rrc_wcdma_abort_psho_rsp;
  lte_rrc_wcdma_resel_rsp_s lte_rrc_wcdma_resel_rsp;

}wcdma_rrc_lte_irat_cmd_u_type;

#define RRC_QSH_CLIENT_METRIC_TIMER_EXPIRY_ARR_MAX    16
#define QSH_CLIENT_METRIC_TIMER_EXPIRY_ARR_MAX 16
typedef struct
{
  /*! category mask: use enum qsh_cat_e values to define bit mask
      Example: QSH_CAT_CFG | QSH_CAT_PERF ... */
  uint32    category_mask;  
  
  /*! whether or not to reset statistics after logging */
  boolean   reset;
} rrc_qsh_client_analysis_s;

typedef struct
{
  /*! pointer to memory */
  void *  addr;
  
  /*! size of memory in bytes */
  size_t  size_bytes;
} rrc_qsh_iovec_s;

typedef struct
{
  /*! 
	@brief
	Bitmask of all tags for which dump is requested for the client.
   
	@note
	If the value is QSH_DUMP_TAG_MASK_ANY then client is free 
	to make a decision what to log and dump size shall at least be
	cumulative size of all tags supported by client.
 
	@note
	Supported max # of tags per client is QSH_EXT_DUMP_TAG_MAX_COUNT.
  */
  uint32	dump_tag_mask;

  /*! 
	@brief
	Storage for requested dump tags (internally partitioned by QSH).
   
	@note
	The iovec shall at least hold cumulative size of tags requested in 
	the dump_tag_mask.
  */
  rrc_qsh_iovec_s				dump_iovec;
  
  /*! QSH internal use only. Client shall not change this field. */
  uint32					cookie_qsh_internal_use; 
} rrc_qsh_client_dump_collect_s;
typedef struct
{
  /*! metric id */
  uint32   metric_id;
  
  /*! subscriber id */
  uint8  subs_id;

  /*! time since last expiry, or time since metric start if first call */
  uint16            time_elapsed_ms;
} rrc_qsh_client_metric_timer_expiry_params_s;

typedef struct
{
  /*! number of metrics being configured 
    (must be 0 <= n < QSH_CLIENT_METRIC_TIMER_EXPIRY_ARR_MAX) */
  uint32                                    metric_id_count;
  
  /*! array of param structs */
  rrc_qsh_client_metric_timer_expiry_params_s   params[
    RRC_QSH_CLIENT_METRIC_TIMER_EXPIRY_ARR_MAX];
} rrc_qsh_client_metric_timer_expiry_s;

typedef struct
{
  /*! metric id */
  uint32                 id;

  /*! action */
  uint8             action;
  
  /*! period in ms */
  uint32                          sampling_period_ms;
  
  /*! subscriber id */
  uint8          subs_id;
  
  /*! pointer to configuration specific to this metric */
  void *                          cfg_data_ptr;

  /*! start address for metric */
  void *                          start_addr;
  
  /*! metric size in bytes */
  size_t                          size_bytes;

  /*! metric context id that the client needs to pass into 
      qsh_client_metric_log(...) after writing a metric */
  void*  metric_context_id;
} rrc_qsh_client_metric_cfg_s;
typedef struct
{
  /*! whether or not to enable notification of the event */
  uint8  action;

  /*! event_id */
  uint32  id;

  /*! event specific cfg ptr */
  void *              event_data_ptr;
  
  /*! QSH internal use only. Client shall not change this field. */
  uint32              cookie_qsh_internal_use; 
} rrc_qsh_client_event_cfg_s;

typedef union
{
  /*! config for QSH_ACTION_ANALYSIS */
  rrc_qsh_client_analysis_s             analysis;
  
  /*! config for QSH_ACTION_DUMP_COLLECT */
  rrc_qsh_client_dump_collect_s         dump_collect;
  
  /*! config for QSH_ACTION_METRIC_TIMER_EXPIRY */
  rrc_qsh_client_metric_timer_expiry_s  metric_timer_expiry;
  
  /*! config for QSH_ACTION_METRIC_CFG */
  rrc_qsh_client_metric_cfg_s           metric_cfg;
  
  /*! config for QSH_ACTION_EVENT_CFG */
  rrc_qsh_client_event_cfg_s            event_cfg;
} rrc_qsh_client_action_params_u;

typedef struct 
{
  /*! client */
  uint32                   client;

  /*! action indicating whether to run analysis, dump, metric cfg ... */   
  uint8                action;
  
  /*! configuration for the action indicated in the above field */
  rrc_qsh_client_action_params_u  action_params;
  
  /*! whether the registered cb can run in sync/async mode */
  uint8           action_mode;

  /*! unique value for each run - should be later passed in 
      qsh_client_cb_done() after processing indicated action */
  uint32            context_id;
} rrc_qsh_client_cb_params_s;

typedef struct
{
  q_link_type           link;

  wcdma_rrc_lte_irat_cmd_u_type cmd;
} wcdma_rrc_lte_irat_cmd_type;

typedef union
{
  msgr_hdr_s msg_hdr;
  /*QMI TO W requests*/
  wcdma_rrc_qmi_mtc_cfg_req_type       wcdma_rrc_qmi_mtc_cfg_req;
}wcdma_rrc_qmi_cmd_u_type;

typedef struct
{
  q_link_type           link;

  wcdma_rrc_qmi_cmd_u_type cmd;
} wcdma_rrc_qmi_cmd_type;

/*--------------------------------------------------------*/
/*                 RRC COMMAND BUFFER                     */
/*                                                        */
/*  The Command buffer used for all RRC command queues is */
/*  defined here. The command buffer contains a RRC       */
/*  command header and a union of all possible commands   */
/*  for any RRC command queue                             */
/*--------------------------------------------------------*/
/* Update rrc_cmd_int_q_u_type along with rrc_cmd_data_u_type 
   for new Internal commands
 */
typedef union
{

  /* Union of individual command structures */
  /*--------------------------------------------------------*/
  /* Command structures for Internal RRC commands           */
  /*--------------------------------------------------------*/
  rrc_downlink_sdu_ind_type       downlink_sdu_ind;
  rrc_downlink_l2ack_ind_type     downlink_l2ack_ind;
  rrc_sib_event_cnf_type          get_sibs_cnf;
  rrc_get_specific_sibs_req_type  get_specific_sibs_req;
  rrc_redirect_req_type           redirect_req;
  rrc_redirect_cnf_type           redirect_cnf;
  rrc_new_state_ind_type          new_state_ind;
  rrc_state_change_ind_type       state_change_ind;
  rrc_cell_change_req_type        cell_change_req;
  rrc_cell_change_cnf_type        cell_change_cnf;
  rrc_new_cell_ind_type           new_cell_ind;
  rrc_llc_cmd_req_type            rrc_llc_req;
  rrc_llc_cmd_cnf_type            rrc_llc_cnf;
  rrc_channel_config_req_type     chan_config_req;
  rrc_channel_config_cnf_type     chan_config_cnf;
  rrc_update_sibs_ind_type        update_sibs_ind;
  rrc_initiate_ue_cap_req_type    initiate_ue_cap;
  rrc_initiate_cu_req_type        initiate_cell_update;
  rrc_initiate_uu_req_type        initiate_ura_update;
  rrc_cu_complete_cnf_type        cu_complete_cnf;
  rrc_disable_prev_freq_type      disable_prev_freq;
  rrc_cell_selected_meas_req_type cell_selected_meas_req;
  rrc_initiate_cell_sel_ind_type  initiate_cell_sel_ind;
  rrc_sib_change_ind_type         sib_change_ind;
  rrc_rnti_update_req_type        rnti_update_req;
  rrc_rnti_update_cnf_type        rnti_update_cnf;
  rrc_cell_reselection_req_type   cell_resel_req;
  rrc_cell_reselection_cnf_type   cell_resel_cnf;
  rrc_ul_suspend_req_type         rrc_ul_suspend_req;
  rrc_ul_resume_req_type          rrc_ul_resume_req;
  rrc_sig_conn_release_req_type   rrc_sig_conn_req;
  rrc_sig_conn_release_cnf_type   rrc_sig_conn_cnf;
  rrc_ciph_config_req_type        rrc_ciph_config_req;

  rrc_abort_ciph_config_req_type rrc_abort_ciph_config_req;

  rrc_ho_ciph_req_type            ho_ciph_req;
  rrc_stop_lower_layer_req_type   stop_lower_layer_req;
  rrc_stop_lower_layer_cnf_type   stop_lower_layer_cnf;
  rrc_mode_change_ind_type        mode_change_ind;
  rrc_sib7_update_status_ind_type sib7_update_status;
  rrc_change_mode_req_type        change_mode_req;
  rrc_change_mode_cnf_type        change_mode_cnf;
  rrc_delete_meas_req_type        delete_meas_id_req;
  rrc_csp_acq_req_type            rrc_csp_acq_req;
  rrc_csp_acq_cnf_type            rrc_csp_acq_cnf;
  rrc_ho_ciph_cnf_type            rrc_ho_ciph_cnf;
  rrc_lower_layer_deact_req_type  lower_layer_deact_req;
  rrc_re_est_rlc_srns_req_type    rrc_re_est_srns_req;
  rrc_re_est_rlc_srns_cnf_type    rrc_re_est_srns_cnf;
  rrc_llc_re_est_req_type         rrc_re_est_req;
  rrc_csp_camp_req_type           rrc_csp_camp_req;
  rrc_csp_camp_cnf_type           rrc_csp_camp_cnf;
  rrc_conn_setup_ind_type         rrc_conn_setup_ind;
  rrc_irat_abort_ind_type         rrc_irat_abort_ind;
  rrc_idt_complete_cnf            idt_complete_cnf;
  rrc_csp_bplmn_suspend_req_type    rrc_csp_bplmn_suspend_req;
  rrc_csp_bplmn_suspend_cnf_type    rrc_csp_bplmn_suspend_cnf;
  /* added internal structure for dual mode handling will be set by MCM to CSP*/
  rrc_mode_status_ind_type          rrc_mode_status_ind;
  rrc_gstk_utran_nmr_info_req_type   rrc_gstk_utran_nmr_info_req;
  /*Internal commands added for requesting & confirmation of
    "Change of UE Capability" */
  rrc_couec_req_type rrc_couec_req;
  rrc_couec_cnf_type rrc_couec_cnf;
  rrc_cipher_config_cnf_type rrc_cipher_config_cnf;
  rrc_reset_l1_req_type rrc_reset_l1_req;

  rrc_open_domain_ind_type rrc_open_domain_ind;

  rrc_mcfg_refresh_info_s_type rrc_nv_refresh;
  rrc_qsh_client_cb_params_s  rrc_qsh_cmd;
  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by L1          */
  /*--------------------------------------------------------*/
  l1_idle_cnf_type                idle_cnf;
  l1_acq_cnf_type                 acq_cnf;
  l1_freq_scan_cnf_type           freq_scan_cnf;
  l1_immediate_cell_eval_cnf_type immed_cell_eval_cnf;
  l1_setup_cnf_type               setup_cnf;
  l1_cell_select_cnf_type         cell_sel_cnf;
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
  l1_cell_transition_cnf_type     cell_transition_cnf;
  l1_drx_cnf_type                 drx_cnf;
  l1_phychan_est_ind_type         l1_phychan_est_ind;
  l1_meas_ctrl_cnf_type           meas_ctrl_cnf;
  l1_act_time_cancel_cnf_type     act_cancel_cnf;
  l1_compressed_mode_ind_type     cm_ind;
  l1_deactivate_cnf_type          deactivate_cnf;
  l1_stop_cnf_type                stop_cnf;
  l1_start_cnf_type               start_cnf;
  l1_suspend_cnf_type             suspend_cnf;
  l1_resume_cnf_type              resume_cnf;
  l1_op_err_ind_type              op_err_ind;
  l1_dl_weak_ind_type             dl_weak_ind;
  l1_out_of_service_type          oos_ind;
  l1_modem_stat_rsp_cmd_type      modem_stat_rsp;
  l1_s_criteria_cnf_type          s_criteria_cnf;
  l1_rssi_info_struct_type        rssi_cnf;
  l1_ui_display_info_struct_type  data_for_ui_disp_cnf;
  l1_sleep_req_type               l1_sleep_req;
  l1_measured_results_struct_type add_meas_info_rsp;
  l1_process_imm_meas_cnf_type    imm_meas_cnf;
  l1_utran_nmr_info_cnf_type      nmr_cnf;

  l1_start_qta_cmd_type           start_qta_cnf;

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by MAC         */
  /*--------------------------------------------------------*/
  mac_status_ind_e_type           cmac_status_ind;
  mac_ul_traffic_meas_ind_type    cmac_ul_traffic_ind;
  mac_meas_ctrl_cnf_type          mac_meas_ctrl_cnf;
  mac_tfc_subset_cfg_cnf_type     mac_tfc_subset_cfg_cnf_info;


  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by RLC         */
  /*--------------------------------------------------------*/
  rlc_ul_hfn_cnf_type        crlc_ul_hfn_cnf;
  rlc_dl_hfn_cnf_type        crlc_dl_hfn_cnf;
  rlc_ul_resume_cnf_type     crlc_resume_cnf;
  rlc_status_ind_type        crlc_status_ind;
  rlc_ul_tm_config_cnf_type  crlc_ul_tm_config_cnf;
  rlc_dl_tm_config_cnf_type  crlc_dl_tm_config_cnf;
  rlc_ul_um_config_cnf_type  crlc_ul_um_config_cnf;
  rlc_dl_um_config_cnf_type  crlc_dl_um_config_cnf;
  rlc_am_config_cnf_type     crlc_am_config_cnf;

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by MM          */
  /*--------------------------------------------------------*/
  rrc_est_req_type           rrc_est_req;
  rrc_service_req_type       rrc_service_req;
  rrc_sim_inserted_req_type  rrc_sim_inserted_req;
  rrc_sim_update_req_type    rrc_sim_update_req;
  rrc_activation_rsp_type    activation_rsp;
  rrc_data_req_type          data_req;
  rrc_open_session_req_type  open_sess_req;
  rrc_close_session_req_type close_sess_req;
  rrc_plmn_list_req_type     plmn_list_req;
  rrc_forbid_lai_list_update_req_type forbid_lai_list_update_req;
  rrc_stop_wcdma_mode_req_type stop_wcdma_mode_req;

  rrc_change_mode_ind_type   rrc_change_mode_ind;
  rrc_abort_req_type         rrc_abort_req;
  rrc_deact_req              deact_req;
  rrc_nw_sel_mode_reset_ind_type    rrc_nw_sel_mode_reset_ind;
  rrc_ds_status_change_ind_type  rrc_ds_status_change_ind;
  rrc_subscription_id_type rrc_subscription_id;
  rrc_ds_tuneaway_status_change_ind_type rrc_ds_tuneaway_status_change_ind;
  rrc_fast_dormancy_req_type  rrc_fast_dormancy_req;
  rrc_csfb_call_status_ind_type rrc_csfb_call_status_ind;
  rrc_lai_reject_list_ind_type      rrc_lai_reject_list_ind;
  rrc_sm_tuneaway_allowed_ind_type  rrc_sm_tuneaway_allowed_ind;
  rrc_lte_reselection_allowed_status_ind_type rrc_lte_resel_allowed_status_ind;
  rrc_set_drx_ind_type  rrc_set_drx_ind;
  rrc_csg_white_list_type white_list;
  rrc_ue_mode_ind_type rrc_ue_mode_ind;
  rrc_multimode_subs_chgd_ind_type rrc_multimode_subs_chgd_ind;
  rrc_pseudo_lte_list_update_ind_type rrc_psuedo_lte_list_update_ind;
  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by TC          */
  /*--------------------------------------------------------*/

  rrctc_mode_change_T        mode_change;
  rrctc_test_loopback_ind_T  test_loopback_ind;

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by LSM         */
  /*--------------------------------------------------------*/

  rrc_meas_rpt_ind_type    meas_rpt_ind;


  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by QMI          */
  /*--------------------------------------------------------*/
  wcdma_rrc_qmi_cmd_type wcdma_rrc_qmi_cmd;

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by MBMS Manager*/
  /*--------------------------------------------------------*/
  wcdma_rrc_lte_irat_cmd_type wcdma_rrc_lte_irat_cmd;

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by TDS RRC */
  /*--------------------------------------------------------*/

    /* Requests from TDS RRC received by RRC */
  rrc_tdsrrc_stop_wcdma_req_type rrc_tdsrrc_stop_wcdma_req;

  /* Responses from TDS RRC for the requests sent by RRC */
  tdsrrc_rrc_stop_tds_cnf_type tdsrrc_rrc_stop_tds_cnf;
}rrc_cmd_data_u_type;


typedef struct
{
  /* Command header common for all commands */
  rrc_cmd_hdr_type    cmd_hdr;

  rrc_cmd_data_u_type cmd;

}rrc_cmd_type;




/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION RRC_GET_L1_CMD_BUF

DESCRIPTION

  This function returns a L1 command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_l1_cmd_buf( void );



/*===========================================================================

FUNCTION RRC_GET_MAC_CMD_BUF

DESCRIPTION

  This function returns a MAC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_mac_cmd_buf( void );



/*===========================================================================

FUNCTION RRC_GET_RLC_CMD_BUF

DESCRIPTION

  This function returns a RLC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_rlc_cmd_buf( void );


/*===========================================================================

FUNCTION RRC_GET_MM_CMD_BUF

DESCRIPTION

  This function returns a MM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_mm_cmd_buf( void );


/*===========================================================================

FUNCTION RRC_GET_TC_CMD_BUF

DESCRIPTION

  This function returns a TC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_tc_cmd_buf( void );


/*===========================================================================

FUNCTION RRC_PUT_L1_CMD

DESCRIPTION

  This function puts a command buffer into RRC's l1
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_l1_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_l1_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);



/*===========================================================================

FUNCTION RRC_PUT_MAC_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mac
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_mac_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_mac_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);





/*===========================================================================

FUNCTION RRC_PUT_RLC_CMD

DESCRIPTION

  This function puts a command buffer into RRC's rlc
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_rlc_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_rlc_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);



/*===========================================================================

FUNCTION RRC_PUT_MM_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mm
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_mm_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_mm_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);


/*===========================================================================

FUNCTION RRC_PUT_TC_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mm
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_tc_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_tc_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);

/*===========================================================================

FUNCTION RRC_FREE_CMD_BUF

DESCRIPTION

  This function frees a command buffer removed from any RRC
  command queue. Only commands removed from RRC's command queues
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_free_cmd_buf
(
  rrc_cmd_type *cmd_buf    /* Command Buffer                               */
);


/*===========================================================================

FUNCTION RRC_FREE_RR_CMD_BUF

DESCRIPTION

  This function frees a RR command buffer removed from RRC's RR
  command queue. Only commands removed from RRC's RR command queue
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_free_rr_cmd_buf
(
  rrc_rr_cmd_type *cmd_buf    /* Command Buffer */
);

/*===========================================================================

FUNCTION RRC_FREE_TDSRRC_CMD_BUF

DESCRIPTION

  This function frees a TDS RRC command buffer removed from RRC's TDS RRC
  command queue. Only commands removed from RRC's TDS RRC command queue
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/


void rrc_free_tdsrrc_cmd_buf
(
  rrc_cmd_type *cmd_buf    /* Command Buffer */
);


/*===========================================================================

FUNCTION RRC_GET_LSM_CMD_BUF

DESCRIPTION

  This function returns a LSM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_lsm_cmd_buf( void );

/*===========================================================================

FUNCTION RRC_PUT_LSM_CMD

DESCRIPTION

  This function puts a command buffer into RRC's LSM
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_lsm_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_lsm_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);



/*====================================================================
FUNCTION: rrcllc_free_embedded_buf()

DESCRIPTION:
  This function frees an embedded pointer (if any) from the command,
  queue.  Embedded memory allocations are not freed by rrc_free_cmd_buf.
  Only the memory allocated for rrc_cmd_type is freed.  This is needed
  to free memory allocations in CPHY_MEASUREMENT_REQ and TVM_REQ from
  RRC to L1 and RRC to MAC, when they are not routed through LLC.  If
  these commands are routed to LLC, LLC takes care of sending them to
  L1/MAC and deleting them if they are not routed and discarded..
  This function also frees memory allocated for the NAS Payload and
  measurement ind

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

=====================================================================*/
void rrc_free_embedded_buf
(
  rrc_cmd_type *cmd_buf,    /* Command Buffer */
  void         *ptr         /* ptr to the payload that needs to be freed */
);

/*===========================================================================

FUNCTION RRC_PUT_CM_CMD

DESCRIPTION
  This function puts a command buffer into RRC's cm
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_cm_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_cm_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);

/*===========================================================================

FUNCTION RRC_GET_CM_CMD_BUF

DESCRIPTION

  This function returns a CM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.
DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS
  None.

===========================================================================*/
rrc_cmd_type *rrc_get_cm_cmd_buf( void );
/*===========================================================================

FUNCTION rrc_put_l1_ff_cmd

DESCRIPTION

  This function puts a command buffer into RRC's l1 FF
  command queue. Note that the command buffer must have been
  requested through a call to rrc_get_l1_ff_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void rrc_put_l1_ff_cmd
(
  rrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);

/*===========================================================================

FUNCTION rrc_get_l1_ff_cmd_buf

DESCRIPTION

  This function returns a L1 command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling rrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_cmd_type *rrc_get_l1_ff_cmd_buf( void );

/*====================================================================
FUNCTION: rrc_send_lte_cmd

DESCRIPTION:
Sends command to LTE RRC

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

=====================================================================*/
void rrc_send_lte_cmd
(
msgr_hdr_struct_type* hdr, 

msgr_umid_type umid, 

uint32 size
);
#endif /* RRCMD.H */
