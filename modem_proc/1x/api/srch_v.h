#ifndef SRCH_V_H
#define SRCH_V_H
/*===========================================================================

             S E A R C H   T A S K   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations for use by the Search task, and should
  be included by all modules within the Search task, and by any other task
  which desires to use externally-visible Search-owned functions.

  Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc. 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srch_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/16   ab      Move srch_get_cdma_rssi_all_chains() to srch_v.h
10/27/14   ab      Addded the SRCH ASDIV signal
01/20/14   srk     ASDIV Cleanup - Remove unneeded SRCH ASDIV signal.
05/09/13   cjb     Exporting MEMPOOL capture API outside of 1x-L1.
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
01/25/13   vks     Cleanup srch mdsp module
12/10/12   vks     AsDiv: Do antenna switch only in 1x srch context
12/06/11   mca     Added TSYNC_QMI_SIG
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
12/09/10   mca     Added support for SRCH_API_TEST
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
02/04/10   jtm     Removed FEATURE_PLT.
12/22/09   mca     Added modem_1x_defs.h
11/11/09   vks     Clean up on the previous changes
11/09/09   vks     Added Message Router and Q6 FW support under feature
                   FEATURE_1X_SUPPORTS_MSGR.
10/02/09   sst     SVDO RF & TRMv3 development (featurized)
07/16/09   mca     Shifted some RF SVDO API signals, added two more
04/07/09   adw     Added srch_get_cell_count() to return the active cell size.
04/06/09   mca     Added a bunch of signals for the RF SVDO API call backs
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Cleaned up incs and eliminated implicit includes.
03/18/09   adw     Removed deint.h include.
03/18/09   adw     Initial API interface cleanup effort.
03/04/09   adw     Moved task start functionality to public header.
11/19/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_SRCH.12.00.13
06/20/08   mca     Merge from //depot
01/09/08   aar     Added gpsOne signal for GPS blanking support.
11/16/07   ns      Added gpsOne signal for sending periodic prm.
03/05/07   aps     Correct externalization for the srch mdsp isr api function.
03/05/07   aps     Added srch_mdsp_add_isr_cb().
08/03/06   tjc     Removed unused signals
06/01/06   tjc     Removed subsignal functions
06/01/06   br      Converted gpsOne subsignals into signals.
05/31/06   tjc     Signal cleanup
05/24/06   br      Removed unused gpsOne signals and subsignals.
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
12/02/05   awj     Added FEATURE_SRCH_RELA_RX0_ONLY
11/02/05   awj     Stripped FEATURE_SRCH_PCG, mainlined RTL and IRM
10/05/05   bt      Added PPM_EVENT_OCCURED_SUBSIG.
09/08/05   ejv     Remove unused prototypes and move some to other .h files.
08/31/05   ejv     Update FEATURE_SRCH_PC2 featurization.  Free now unused
                   signal masks.
08/30/05   ejv     Remove featurization from srch_get_rx1_agc.
08/18/05   br      Removed unused GPS subsignal for "Friendly Wake-up" and
                   "DSP Ready" events.
07/01/05   grl     Moved srch_rx tunes to srch subsigs.
06/22/05   sfm     Renamed SRCH_RF_TUNE_TIMER_SIG to SRCH_RF_TUNE_DONE_SIG and
                   removed SRCH_ACQ_TIMER_SIG
06/10/05   bt      Added SRCH_INT_GLOBAL_CMD_SIG, Removed unused SRCH_OOK_SIG.
06/06/05   br      Removed unused subsignals.
05/31/05   rng     Merged from SHDR Sandbox.
05/24/05   ejv     Moved srch_get_fing_rssi to srch_mux.h.
02/07/05   ejv     Moved srch_get_rx_agc to srchmc.h.
01/24/05   ejv     Moved srch_get_pilot_ecio to srchmc.h.
11/12/04   ejv     Remove unused srch_notify_sch_active.
11/10/04   ejv     Moved srch_tune_rfm and srch_get_rx_div_cap to srchmc.h.
10/25/04   ejv     Removed srch_set_fpc_mode and srch_flag_erasure.
10/20/04   ejv     Include mctrans.h in place of mc.h.
10/15/04   ejv     Moved MC-specific data into srchmc.h.
10/15/04   kwo     Removed unslew
09/30/04   ejv     Moved some parms to srchi.h and srch.c.  Added missing
                   "extern"s and removed unused srch_friendly_wakeup.
09/24/04   ajn     Removed srch_get_allow_uP_clock_switch()
09/23/04   kwo     Added Frame sig
09/20/04   br      Added GPS subsignal for System Access State monitoring.
09/13/04   kwo     Added srch_get_channel_estimate() & srch_in_service()
09/12/04   br      Cleaned up unused subsignals and added proper featurization.
09/09/04   ejv     Removed REL_C/D_DV featurization.
09/03/04   ejv     Moved srch_cpcch_info_type.
08/27/04   ejv     Mainline more features.
09/23/04   ejv     Mainlining more of MC-SRCH interface.
                   Removed some unused types.
08/12/04   dna     Transitions to SRCH_START_STATE now go to SRCH_CDMA_STATE
07/23/04   hxw     Added GSC_GSS_SCHEDULER_ONLINE subsignal
07/22/04   br      Removed unused GPS subsignals.
07/19/04   ejv     Mainline FEATURE_IS95B_PPSMM, PILOT_RPT, SYS_RESELEC,
                   MAHHO, and SOFT_HO.
07/07/04   ejv     Add T_WINCE.
06/30/04   dna     Changed SRCH_START_STATE to SRCH_INACTIVE_STATE.
                   Added srch_exit_service(), called when we enter Inactive
                   state or when we return to CDMA state from any state other
                   than inactive.
06/22/04   br      Moved feature PDSM_SRCH_INTERFACE. Mainlined GPS scheduler
                   V2. Removed gpsOne definitions.
06/16/04   br      Removed feature GPSONE_ACQ_MODE. Changed GPSONE_PE
                   featurization.
06/17/04   ejv     Remove srch_report_tc_msg.
06/16/04   jcm     Removed srch_get_short_code_len, no longer used by sleep
06/02/04   ejv/rdh Interface functions to update new log packets.
                   Removed srch_select_8x prototype.
05/27/04   ejv     Mainline FEATURE_BROADCAST_SMS.
05/14/04   kwo     Removed featurization of bcast_index in parm_f
04/29/04   ejv     Create new struct types for DV data.
04/29/04   ejv     Removed comment.
04/21/04   sfm     Changes for traffic channel manager
04/08/04   ejv     Mainline SRCH_HAS_TRK_LO_CNTL and SRCH_HAS_PILOT_GAIN_CTL.
04/05/04   jcm     Removed unused functions: srch_get_rset_pilot(),
                   srch_is_asleep(), srch_get_ref_pn()
04/02/04   ejv     Removed unused command type srch_cf_nset_cmd_type cfnset.
04/01/04   dd      Added elements to the srch_pdsm_start_gps_cmd_type struct.
03/26/04   hxw     Increment SYNC80_NOT_INITIALIZED for Sync80 extended scheme.
03/25/04   ejv     Modified srch_dv_pilot_info_type and srch_dv_cfg_type and
                   updated to support RelD interface.
02/23/04   ddh     Changed type of hdr in srch_pdsm_cmd_type to be
                   a srch_pdsm_cmd_code_type rather than srch_cmd_code_type.
02/12/04   ejv     Added additional FEATURE_TRANS_MANAGER checking.
02/11/04   ejv     Mainline FEATURE_IS95B.
02/11/04   ejv     Changed FEATURE_IS2000C_DV to FEATURE_IS2000_REL_C_DV.
                   Added prototype for new function srch_get_pref_dv_sector.
02/10/04   ejv     Added additional interface updates for 6700.
02/10/04   ejv     Added FEATURE_TRANS_MANAGER.
02/04/04   hxw     Modified srch_pdsm_start_gps_cmd_type to send an array of
                   srch modes to new sched in factory test.
02/02/04   ejv     Removed unused variable and srch_keypress_handler.
01/30/04   kwo     Removed SRCH_PC_HO_TIMER_SIG, SRCH_PC_THROT_TIMER_SIG
01/26/04   sst     Sorted srch signals and subsignals
                   Changed GSC_TIMER_SIG from 0x40000000 to 0x04000000
01/22/04   dd      Added define for GSC_GSS_CMD_SUBSIG - GSC-GSS command sig.
01/21/04   sfm     Added typedef for srch_dv_pilot_info_type and an instance
                   of it to srch_tc_cmd_type and srch_tc_aset_type
01/21/04   jcm     Removed reminents of the delay timer
01/16/04   ejv     Remove FEATURE_SRCH_SCHEDULER.
01/14/04   ddh     Removed FEATURE_BROADCAST_SMS from srch_bc_info_cmd_type
01/12/04   sst     Added function srch_trk_lo_adj_override_and_freeze() and
                   srch_trk_lo_adj_dis_override_and_unfreeze()
11/26/03   yll/fu  Added the RC fields in the srch_tc_cmd_type structure.
11/05/03   sst     Added prototypes for srch_trk_lo_adj_*
11/05/03   fu/aaj  Function prototype for reading agc1 for rx div
11/05/03   br/hxw  Added support for GPS OVERRIDE_CODE_DOPP.
11/04/03   sst     Changed prototype for srch_tune_rfm()
10/28/03   br      Added subsigs for OOS2 support.
10/24/03   sst     Added prototype for srch_tune_rfm(), moved prototype for
                   srch_fing_set_dmod_pilot_gain() to srch_fing.h
09/30/03   ejv     Remove obsolete feature FEATURE_GPS_DISABLE_CHIPX8_SLEEP.
09/26/03   ejv     Remove unused SRCH_FAST_SLEEP_SIG.  Move srch_wakeup_isr
                   prototype to srchi.h.
09/08/03   aaj     Added interface to MC to report Rx Div capability
08/18/03   br/gs   Add SRCH_PDSM_GET_GPS_INFO_F processing and added
                   next_fix_ts to srch_pdsm_start_gps_cmd_type.
08/13/03   gs      Add prototype srch_multimode_sanity_err_fatal.
08/11/03   kwo     Converted sci to a signed value
07/28/03   ejv     Mainline FEATURE_ZZ2.
07/14/03   ejv     Remove FEATURE_SPECIAL_MDR.
06/23/03   ejv     Remove FEATURE_EXT_LINEARIZER and FEATURE_SEARCH3.
                   Define SRCH_UNUSED_SUBSIG1.
06/15/03   rdh     Added srch4 signals, recycling srch3 sigs now unused.
06/09/03   ejv     Remove nop function srch_freeze_iq().
06/09/03   ejv     Added srch_util_dmod_powerup_reset().
05/20/03   rdh     Added Gen Pilot Sets packet, mainlined JCDMA monitor.
05/02/03   br/aar  Added "unsol_resp" field in srch_pdsm_start_gps_cmd_type.
                   Added new field to PDSM 'Start PPM' command.
04/28/03   sst     Mainlined FEATURE_SD20 and SRCH_ENABLE_SD20
04/22/03   br      Added support for sending A-set and C-set to PDSM.
04/09/03   sst     Added define for SRCH_RSV_CFS
04/04/03   kwo     Updated update_aset message to support simultaneous common
                   channels
04/03/03   bt      Remove QPCH PLT support for ZZ2.
03/12/03   ejv     Added FEATURE_IS2000_REL_B.
03/05/03   kwo     Moved demod_chan to l1m and srch_chan
02/18/03   va      SRCH4 AFLT support.
02/11/03   kwo     Mainlined srch_dcch_info
02/05/03   gs/kwo  Add in Rel A support for ZZ2.
02/05/03   gs/br   Remove redundant PCG definition in the parameter structure.
02/03/03   gs      Merge in MSM_SRCH.08.06.60, specifically adding PGI
                   subsignal and adding support for 'SA availability' field in
                   PDSM command.
01/14/03   gs      Merge in MSM_SRCH.08.06.57
12/19/02   ejv     Removed FEATURE_FINGER_POLL.
12/17/02   aar/abh Added field for factory test mode in gsc_init_cmd_type.
12/03/02   pj      Partial merge with 5500 development (MSM_SRCH.08.06.52).
12/02/02   hxw     Added override_jda to turn on/off Jammer Detection Algorithm
                   through DIAG.
11/25/02   ejv     Mainlined !FEATURE_SRCH_PC_IFACE.
11/19/02   gs      Move OOK subsignal signal back to a signal.
11/15/02   ejv     More updates for EBCSMS.
11/12/02   br      Added field to srch_pdsm_get_ppm_rslt_cmd_type sturcture.
11/12/02   gs      finish merge with MSM_SRCH.08.06.47
11/05/02   gs      Partial merge with MSM_SRCH.08.06.47
10/03/02   va      Merged with MSM_SRCH.08.06.39.01.01 (MS-Based Search VU)
10/02/02   gs      Merge with MSM_SRCH.08.06.42
10/01/02   gs      Fixed PLT and factory test mode support
10/01/02   sst     Added signal for SRCH4_LOST_DUMP_SIG
09/11/02   va      Added session_type to start_gps command
07/30/02   gs      Initial merge into perforce for cougar.
07/19/02   pj      Added clock_drift_oos to start_gps command.
07/19/02   br      Mainlined FEATURE_PDSM_SRCH_IFACE_2.
06/27/02   pj      Renamed srch_pgi_leave_service_ts to pgi_leave_service_ts.
06/25/02   aar     Added declaration for srch_get_srchdz_chipx8_sleep_allowed().
06/21/02   pj      Added declaration for srch_pgi_leave_service_ts.
06/14/02   ejv     Changed REL_A_BCSMS to REL_A_CC_BCSMS.
06/13/02   ejv     Wrap fpc_pri_chan with REL_A only.
06/05/02   pj      Added sub-signals for clock switching and "in" CDMA service.
06/03/02   aar     Replaced srch_get_srchzz_chipx8_sleep_allowed with srch_get_
                   srchzz_keep_chipx8_on.
05/31/02   ejv     Removed function srch_get_vc_osc_ctl().
05/31/02   ejv     Added lc_mask to srch_idle_chan_info_type for PLT.
05/30/02   aar     Added function srch_get_srchzz_chipx8_sleep_allowed().
05/24/02   ejv     Added FEATURE_IS2000_REL_A_SVD.
05/21/02   ejv     Mainlined FEATURE_IS2000.
05/17/02   lr      Added SYNC80_NOT_INITIALIZED definition.
05/17/02   lr      Added gps_ref_sync80_pos element to gs_standalone_parms_type
                   structure.
05/17/02   br      Added initial support for FEATURE_PDSM_SRCH_IFACE_2.
05/14/02   ejv     Fixed comment.
05/10/02   abh     Un-featurized session_type field in start_gps command, so
                   that the code can be compiled with old architectures.
05/09/02   br      Moved subsignal handling function prototypes from srchi.h.
05/02/02   ejv     Removed FEATURE_QUARTER_ROLL_TIMELINE.
04/24/02   br      Added support for different gpsOne PDSM session types.
04/22/02   ejv     Moved some defines to other files.
04/17/02   ejv     Removed unused function srch_pcomb_rate_set.
04/17/02   br      Fixed Searcher 4 support.
04/11/02   br      PE enum definition cleanup.
04/08/02   rdh     Added HT's defines for common pilot walsh codes.
03/27/02   lr      Defined NO_BIT_EDGE_DETECTION_FLAG.
03/26/02   br      Removed obsolete FEATURE_GPSONE_SESSION_STAT.
03/19/02   br      Mainlined FEATURE_PPM_DB_2 and FEATURE_GPSONE_GPS_FTEST.
                   Added proper GPS and PPM feature names. Rearranged PPM
                   definitions. Clean up.
03/16/02   br      Added support for the new GPS software architecture and
                   added subsignal definitions.
03/15/02   ejv     Added FEATURE_IS2000_REL_A_BCSMS.
03/09/02   pj      Changes for MS-based positioning (FEATURE_GPSONE_PE).
03/05/02   ejv     Extern prototype for new function srch_notify_sch_active().
02/27/02   ejv     Featurization around "bcch_supported" is changed.
02/25/02   ejv     Changed featurization around srch_chan_config_cmd_type.
02/22/02   ejv     FEATURE_IS2000_REL_A_NCC -> FEATURE_IS2000_REL_A_CC.
02/21/02   ejv     Added FEATURE_SRCH_PC_IFACE and FEATURE_IS2000_REL_A_NCC.
02/20/02   abh     Merged GPSOne code in MSM6050 archive. Also added code to
                   go to GPS mode when Searcher is in Acquisition state.
02/11/02   aar     Re-featurize the IIR mode under the srch_pdsm_start_gps.
02/06/02   ajn     Added SRCH_STANDBY_F
01/18/02   sst     Moved Subsignal Definitions - to srchi.h
01/13/02   sst     Modified to incorporate new srch files from the srchdrv.c
                    breakup (added new include files, global variable externs)
12/02/01   br      Rearanged subsignal definitions. Added subsignals and
                   factory test flags for gpsOne in idle mode support.
11/29/01   aar     Added FEATURE_GPSONE_PLAY_TONE for beeps during tune back.
11/21/01   br      Removed obsolete FEATURE_PPM_DB_1.
11/12/01   pj      Added GPS QOS parameters. Modified GPS factory test
                    parameters.
10/12/01   ajn     Replaced srch_keypress_isr with srch_keypress_handler
10/08/01   ejv     Added additional REL_A PLT support.
06/14/01   ejv     Wrapped srch_finger_lock_handler() in FEATURE_FINGER_POLL.
06/10/01   br      Increased PPM database size.
06/08/01   ejv     Added missing #ifdef FEATURE_GPSONE_SESSION_STAT.
06/05/01   ejv     Replaced some T_MSM5000 checks with FEATURE_IS2000.
06/01/01   ajn     Added FEATURE_SEC.
06/01/01   ejv     Moved code to allow FEATURE_IS95B_MAHHO to be turned off.
05/28/01   pj      Added SRCH_SRCH3FW_READY_TIMER_SIG.
05/24/01   ejv     Removed srch_get_cdma_raw_ecio() from FEATURE_JCDMA.
05/24/01   ejv     Added missing pilot_rec params in srch_cf_nset_cnd_type.
05/20/01   br      Added support for subsignals. Exchanged some srch task
                   signals with subsignals. Cease sharing srch_gps_timer
                   between GPS and PPM modes.
05/19/01   et      Added session_type and gps_stats_type structure. Added
                   function prototypes for functions to get gps stats srch
                   fields.
05/03/01   ejv     Removed srch_t_add_abort_cmd_type.
05/03/01   fc      Removed SRCH_T_ADD_ABORT_ON_F and SRCH_T_ADD_ABORT_OFF_F.
05/01/01   br      Added SRCH Scheduler support.
04/20/01   ejv     Added support for USE_T_ADD_ABORT.
04/18/01   ejv     Enable pilot_rec/_type in Nset update cmd for IS2000.
03/28/01   br      Changed PPM database parameter.
03/05/01   ejv     Updated srch_nset_cmd_type with FEATURE_IS2000_REL_A.
03/02/01   br      Added FEATURE_GPS_MODE to featurize GPS operation.
                   Added FEATURE_PPM_DB_1 and FEATURE_PPM_DB_2 to separate
                   different PPM database processing algorithms for
                   different targets.
02/28/01   ejv     Rearranged a structure for better packing.
02/27/01   ejv     Added FEATURE_IS2000_REL_A.
02/27/01   br      Added FEATURE_SEARCH3 to featurize searcher 3 operation.
02/27/01   pj      Removed SRCH_QDSP_DLOAD_DONE_SIG.
02/26/01   sko     Added srch_get_ref_pn().
02/15/01   bgc     Changed FTM to a runtime mode.
02/15/01   rdh     Added RESELECT_MEAS command for SD2.0, shuffled sys meas
                   command struct members to conform to MC convention.
02/14/01   rdh     Added initial support for System Determination 2.0.
01/26/01   ych     Merged T-53/JCDMA features for SK.
01/22/01   ejv     Mainlined T_SLEEP.
01/08/01   ejv     Added new sub-structures for channel parameters.
11/21/00   bgc     Updated for PLT support.
11/06/00   ejv     Increased Cset to support new drivers interface for ACQ.
09/21/00   pj      Added factory test mode in SA data message from PDSM.
09/12/00   ejv     Removed #ifdef's from around srch_access_inform();
09/07/00   lc      Deleted function prototypes for srchtc_proc_sch_trans()
                   and srchtc_proc_ho_trans().
09/06/00   bc      Added FEATURE_FACTORY_TESTMODE.
09/01/00   day     Change reference to new sensitivity assist data structure.
09/01/00   pj      Added SRCH_GPS_SRCH_DONE_ISR_SIG.
08/24/00   pj      Added new QDSP download signals.
08/21/00   ejv     Removed FEATURE_PRE_95B_BUILD, SRCH_PRE_IS95B_BUILD,
                   FEATURE_PILOT_SCANNER, and FEATURE_SRCH_DYNAMIC_BAND_CLASS.
08/06/00   br      Added support for SRCH3 lost status dump and GPSONE.
08/04/00   br      Added additional FEATURE_GPSONE support.
07/28/00   fc      Mainlined FEATURE_CS_ACTION_TIME_TRANS.
07/16/00   abh     Added Signals for GPS DSP Ready & GPS Dump ISR.
07/16/00   abh     Added the fields for Acq Assist & Sensitivity Assist
                   data in the message from PDSM.
07/07/00   eh/fc   Added support for SCRM processing.
07/07/00   dgy     Added a flag that indicates whether to update CFS set.
06/29/00   abh/br  Added MSM3300 support - FEATURE_GPSONE and FEATURE_SRCH_BATCH_LOG.
06/22/00   bgc     Replaced include file mctrans.h with mctrans_cmd.h.
06/20/00    eh     Swap OOK_RATE_4800bps and OOK_RATE_9600bps definition.
06/12/00   lc/dgy  Added FEATURE_CS_ACTION_TIME_TRANS to streamline the processing of
                   MC transactions and to support soft handoff transactions.
06/12/00   ejv     Externed new function srch_set_fpc_mode.
05/26/00   ejv     Added slot masks and cur_slot_mask parameter to sleep cmd.
05/25/00   ks      Added support for Neighbor Search Window offsets.
05/12/00   day     Check that MC state is not in access state using srch_access_inform
                   before sending threshold report.
05/03/00   wli     Added FEATURE_NONSLOT_OFREQ_NSRCH
04/24/00   dgy     Added full support for SCH, including handoff, Active set
                   updating, and action time processing.
04/21/00   ejv     Added prototype for new function srch_update_rc.
04/10/00   dgy     Added FEATURE_IS2000_QPCH.
03/14/00   ejv     Added FEATURE_BROADCAST_SMS.
02/16/00   lcc     Modified srch_tc_aset_type to contain SCH info.
02/14/00   ejv     Added function srch_set_tx_rate_adj.
02/02/00   dgy     Added support for action time processing.
12/13/99   ajn     Re-Added FEATURE_ENHANCED_STANDBY_III & QUARTER_ROLL_TIMELINE
                   support.
12/13/99   ejv     Changed srch_get_dmod_id to return a word instead of byte.
12/08/99   va/dgy  Added handoff interface for FEATURE_IS2000.
11/29/99   ejv     Externed srch_freeze_iq for RF.
11/24/99   ajn     TEMP_HACKS_FOR_TIGER removed
11/08/99   dgy     Added FCH QOF support for handoffs.
10/27/99   ajn     Added Signal for QPCH operation
10/21/99   eh      Added SRCH_PLT_DEMOD_QPCH_FRAME_F and srch_set_demod_qpch_frame_cmd_type
10/20/99   dgy     Added support for QOF.
10/20/99   ejv     Removed cfs_seq from srch_cf_nset_cmd_type.
10/14/99   wjn     Deleted code that defines/undefines
                   FEATURE_SRCH_DYNAMIC_BAND_CLASS within the scope of Search
                   files.
10/07/99   lcc     Merged from PLT 2.0:
           jcw     Removed rf_present flag since it isn't used.
           eh      add hdr to srch_set_lc_mask_cmd_type
           rm      Added new Interface for SCH (for PLT).
09/30/99   yus     IS95B code review cleanup.
09/14/99   wjn     Defined/Undefined FEATURE_SRCH_DYNAMIC_BAND_CLASS within
                   the scope of Search files.
08/25/99   wjn     Defined SRCH_RUNTIME_CSET_MAX and SRCH_RUNTIME_MAX_CAN_CNT
                   macros so that the maximum Candidate size is determined at
                   run time, depending on P_REV.
08/23/99   ejv     Mainlined FEATURE_ENHANCED_STANDBY_II.
08/12/99   yus     Added function prototype srch_cfs_off_time_fwd
                   (under FEATURE_IS95B_MAHHO.)
08/12/99   yus     Changed FEATURE_IS95B_CFS and FEATURE_IS95B_HARD_HO to
                   FEATURE_IS95B_MAHHO.
08/06/99   ejv     Extended FEATURE_PRE_95B_BUILD.  Merged yshi's FEATURE_
                   IS95B_CFS support.  ajn: Mainlined FEATURE_VIRTUAL_
                   COMBINER.  wjn: Added FEATURE_SRCH_DYNAMIC_BAND_CLASS.
07/28/99   yus     Added PACKED for internal struct of srch_walsh_struct_type.
07/26/99   ejv     yus: Made srch_walsh_struct_type PACKED.  Added structure
                   srch_walsh_struct_type for displaying walsh codes on the DM.
07/24/99   jcw     Merged in support for PLT.
07/23/99   wjn     Added function prototype
                   srch_set_dmod_pilot_gain( byte band_class ) for run time
                   CDMA band class switches.
07/19/99   ml/dgy  Added support for PLT.
07/01/99   yus     De-coupling FEATURE_SPECIAL_MDR with FEATURE_IS95B_PILOT_RPT
                   if SRCH_PRE_IS95B_BUILD is defined.
06/28/99   yus     Merged IS95B features to the ASW tip.
06/28/99   yus     Added #ifdef SRCH_PRE_IS95B_BUILD for pre-95b backward build.
06/10/99   yus     Merged IS95B Hard Hardoff ( w/o Candidate Frequency Search.)
06/09/99   rdh     Merged FEATURE_PILOT_SCANNER, FEATURE_SCAN_FING_LOG.
06/04/99   ejv     Unwrapped srch_get_rx_agc from being conditionally compiled.
05/24/99   yus     Changed add_intercept and drop_intercept to int1 from byte.
05/24/99   yus     Added IS95B Periodic PSMM feature.
04/27/99   yus     Changed SRCH_MAX_CAN_CNT to 10 for IS95B build.
04/22/99   yus     Changed { #if defined(FEATURE_IS95B_MDR) ||
                   defined(FEATURE_SPECIAL_MDR)} to {#if defined(FEATURE_IS95B)
                   || defined(FEATURE_SPECIAL_MDR)} in case of of win_n and win_r
                   in srch_tc_aset_cmd_type.
04/21/99   yus     Added win_n and win_r in srch_tc_aset_cmd_type.
03/22/99   kmp     Merged the changes listed below from the TGP IS-95B baseline.
                   03/10/99   yus     Added access_attempted field.
02/26/99   yus     Merged IS95B features to ARM based code( see below for the
                   features.)
                   12/07/98   yus     Added support for Extended Channel
                                      Assignment Message.
                   11/25/98   yus     Added P_REV checking.
                   11/25/98   yus     Added IS95B Neighbor search enhancements.
                   11/25/98   yus     Added IS95B Access handoff and Access
                                      Entry handoff support.
                   11/25/98   yus     Added IS95B Pilot reporting.
                   11/25/98   yus     Changed #ifdef FEATURE_JSTD008 to run time
                                      checking.
                   10/26/98   yus     Added win_n and win_r into
                                      srch_tc_aset_cmd_type structure.
                   09/23/98   yus     Added IS95B soft handoff algorithm.
                   09/23/98   yus     Added IS95B system reselection in Idle
                                      feature.
                   09/23/98   yus     Included mc.h for P_REV checking.
02/03/99   ejv     yus: Merge updates for MDR features.
02/03/99   ejv     yus: Added IS95B MDR support.
02/03/99   ejv     lcc: Extracted part of tc_set and tc_update_sup commands
                   into types.
02/02/99   ejv     Changed T_VIRTUAL_COMBINER to FEATURE_VIRTUAL_COMBINER.
02/01/99   ejv     Merged changes for FEATURE_ENHANCED_STANDBY_II, including
                   function prototypes for srch_get_fing_rssi and srch_fast_
                   sleep_inform.  Also added FEATURE_EXT_LINEARIZER.
01/13/99   ejv     ajn: Added knowledge about time reference slamming to
                   srch_comb_slam_timeline & added srch_get_vsc_time( )
01/06/99   ejv     Removed _cdecl - not necessary for ARM.
09/28/98   ejv     Added _cdecl to force C calling to specific functions
                   for Pascal builds.
09/18/98   ejv     Added target.h to list of include files.
08/18/98   ejv     Added dh task support, must define FEATURE_DH.
07/30/98   bgc/mk  Added prototype for srch_finger_lock_handler().
03/04/98   yus/smo Added support for Turbo/1x switching, srch_select_8x.
06/19/97   rdh     Added T_Q, removed pre-NGP support, updated copyright.
05/20/97   rdh     Added Low Power Mode support.
03/21/97   smo     Added customer.h include for FEATURE_JSTD008
03/14/97   jjn     Added (TG==T_MD) to the appropriate conditional compile
                   statements for the Module
11/22/96   rdh     Added Hutch's T_T target definitions.
09/19/96   rdh     Added unconditional PC state idle handoff throttling.
08/15/96   rdh     Added idle handoff delay timers/sigs.
06/11/96   rdh     Added SRCH_OFF/ON_TX commands for PSMM holdoff if TX off.
05/08/96   smo     Added JSTD-008 features and definitions.
05/01/96   rdh     Split Csize for real and pre-Cans, add parm to rude_wakeup.
04/09/96   rdh     Added prototype for function to flag search for erasures.
04/01/96   jmk     Added func srch_rude_wakeup to rouse the phone from sleep.
02/28/96   rdh     Added win_a field for HO Dir Mssg command. IS-95 issue.
02/28/96   rdh     Added srch_start_pc_state called interface.
11/30/95   rdh     Added fade report typedef and packet type.
09/07/95   rdh     Passing pilot set energies to DIAG in IS-95 format.
08/28/95   rdh     Let governor have its own REX signal instead of muxing.
08/23/95   rdh     Pre- code review cleanup checkin to save RAM/ROM.
08/18/95   rdh     Added srch_get_pilot_set DIAG support.
07/17/95   rdh     Added pctl Punct rate set function for MSM 2p.
06/16/95   rdh     Added powerdown command so Search can tidy up on powerdown.
06/15/95   rdh     Moved all chipx8 bring-down code into new RXC sleep ISR.
06/01/95   rdh     Mods for search driver break-up.
05/09/95   rdh     Added search speed throttling to allow all window sizes.
04/14/95   rdh     Added CDMA RSSI processing for all targets.
04/13/95   rdh     Gemini change: Provide callable AGC/power control log fnct.
03/30/95   rdh     Merged in lost dump recovery timer code.
03/30/95   rdh     Added srch_control_chipx8_clk to fix runt pulse problem.
03/23/95   rdh     Added duration field to SLEEP_F structure for deep sleep.
01/06/95   rdh     IS-95 Phase III: Added REX sigs for handoff/slew throttling.
11/01/94   rdh     Added PC_ASSIGN command structure gottem from Don.
10/27/94   rdh     Added decl for srch_notify_bad_slot.
10/24/94   rdh     Add PN mask autoswitch capability.
07/30/94   rdh     Add CDMA RSSI declaration for ISS1.
08/09/93   ptw/jah Added signal for sleep, and srch_powerdown() prototype.
10/12/92   ptw     Changes for CAI 2.0+ compliance.
07/20/92   jai     Moved report types to mccsrch.h.
07/16/92   ptw     Added signal definitions, etc. formerly in task.h
07/14/92   ptw     Changed to new cmd_hdr.
06/11/92   ptw     Added some support for slotted mode.  Removed some fossils.
01/31/91   ptw     Added support for pilot strength measurements for forward
                   link power control.
01/22/91   ptw     Added more extensive commenting.
07/20/91   rdb     Got rid of TA report to diag task, added new params to
                   paging channel command.   (Rev 1.2)
06/25/91   rdb     Lots of changes to support jump from sync channel timing
                   to traffic channel timing. (Rev 1.1)
06/02/91   rdb     Initial revision.  (Rev 1.0)
05/21/91   rdb     Revamped on eve of first cut integration.
03/20/91   rdb     Created file.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"

/* Srch */
#include "srchmc_v.h"

/* Other */
#include "task.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define  SRCH_NO_SIGS                                      0
  /* signal mask in case when there are no signals */

#define  SRCH_MSGR_Q_SIG                                   0x00000001
  /* This signal is set when the message router places a message in the
     srch message receive buffer queue */

#define  SRCH_ROLL_ISR_SIG                                 0x00000002
  /* This signal is set by the PN Roll interrupt handler each time a
     PN Roll (26.666... ms boundary of system time according to the
     mobile station's time reference) occurs.  Used by SRCH to synchronize
     events with system time.  */

#define  SRCH_CMD_Q_SIG                                    0x00000004
  /* This signal is set when an item is put on the srch_cmd_q. */

#define  SRCH_DOG_RPT_SIG                                  0x00000008
  /* Dog Heartbeat signal for SRCH task */

#define  SRCH_NV_CMD_SIG                                   0x00000010
  /* This signal is set when NV has completed a commmand for the Search task */

#define  SRCH_DEMOD_CHAIN_TUNE_TIMER_SIG                   0x00000020
  /* Signal used by srch_rx to notify the 1x task of tune completions for
     the Demod chain. */

#define  SRCH_ALT_CHAIN_TUNE_TIMER_SIG                     0x00000040
  /* Signal used by srch_rx to notify the 1x task of tune completions for
     the Alt chain. */

#define  SRCH_INT_GLOBAL_CMD_SIG                           0x00000080
  /* This signal is set when an internal cmd to the global state
     machine group is pending */

#define  SRCH_INT_CMD_SIG                                  0x00000100
  /* This signal is set when an internal cmd is pending */


#define  SRCH_RF_SLEEP_RX_DONE_SIG                         0x00000200
#define  SRCH_RF_WAKEUP_RX_DONE_SIG                        0x00000400

#define  SRCH_CMD_SIG                                      0x00000800
  /* Used for the command interface between SRCH-RXC */

#define  SRCH_RF_SLEEP_TX_DONE_SIG                         0x00001000

#define  SRCH_OFFLINE_SIG        TASK_OFFLINE_SIG       /* 0x00002000 */
  /* This signal is set to tell SRCH to go into offline mode.
     Actually,  this doesn't do anything to SRCH. */

#define  SRCH_STOP_SIG           TASK_STOP_SIG          /* 0x00004000 */
  /* The Main Control Task sets this signal to tell the Searcher Task to
     stop execution. */

#define  SRCH_START_SIG          TASK_START_SIG         /* 0x00008000 */
  /* After creation and initialization, the Searcher Task waits for the Main
     Control Task to set this signal before beginning its normal execution. */

#define  SRCH_RF_TUNE_DONE_SIG                             0x00010000
#define  SRCH_RF_ENABLE_DIV_DONE_SIG                       0x00020000

#define  SRCH_RF_SM_GRANTED_SIG                            0x00010000
#define  SRCH_RF_SM_DENIED_SIG                             0x00020000

#define  SRCH_RF_1X_ENABLE_DONE_SIG                        0x00040000
#define PGI_BLANKING_TIMER_SIG                             0x00040000

#define  SRCH_RF_DISABLE_DIV_DONE_SIG                      0x00080000

#define  SRCH_RF_1X_DISABLE_DONE_SIG                       0x00100000
#define GSC_PERIODIC_PRM_TIMER_SIG                         0x00100000
  /* Signal for sending periodic prm */

#define SRCH_PDSM_CMD_Q_SIG                                0x00200000
  /* This signal is set when an item is put on the srch_pdsm_cmd_q. */

#define  SRCH_RF_WAKEUP_TX_DONE_SIG                        0x00400000
#define PPM_EVENT_OCCURED_SIG                              0x00400000
  /* Signal to indicate that PPM event has occured */

#define PGI_VISIT_ALLOWED_SIG                              0x00800000
  /* This signal marks when Phone-GPS Interface allows GPS visit to happen */

#define TSYNC_QMI_SIG                                      0x00800000
  /* This signal is set when a Tsync QMI event needs to be handled */

#define GSC_GPS_SEARCH_DONE_SIG                            0x01000000
  /* This signal notifies GPS Search Control module that all the GPS searches
     are completed by the GPS Scheduler */

#ifdef FEATURE_MODEM_1X_SRCH_ASD
#define SRCH_ASDIV_SIG                                     0x01000000
#endif /* FEATURE_MODEM_1X_SRCH_ASD */

#define PGI_ACTION_REQUESTED_SIG                           0x02000000
  /* This signal is set when Phone-GPS Interface needs to performe some
     activities in the SRCH task context */

#define GSC_ACTION_REQUESTED_SIG                           0x04000000
  /* This signal is set when GPS Search Control module needs to performe
     some activities in the SRCH task context */

#define PGI_EVENT_OCCURED_SIG                              0x08000000
  /* Signal to indicate that PGI event has occured */

#define SRCH_GPS_TIMER_SIG                                 0x10000000
  /* This signal is used by GPS Search timer */

#define GSC_GSS_CMD_SIG                                    0x20000000
  /* Signal indicating that GPS Scheduler has posted a cmd to
     srch_gpssched_cmd_q */

#define GSC_GSS_SCHEDULER_ONLINE_SIG                       0x40000000
  /* Signal indicating scheduler running into exception */

#define GSC_TIMER_SIG                                      0x80000000
  /* This signal is used by GPS Search Control module universal timer */


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_GET_BEST_RSSI

DESCRIPTION    This function returns a byte representing the best-potential
               CDMA RSSI value in P(AGC) + Ec/Io form.

DEPENDENCIES   srch_?set[], srch_?set_cnt.

RETURN VALUE   A 6-bit RSSI index, 0..63, representing RSSI from
               -75 dBm to -106.5 dBm, in 0.5 dBm steps.

SIDE EFFECTS   None.

===========================================================================*/
extern byte  srch_get_best_rssi ( void );

/*===========================================================================

FUNCTION       SRCH_START_PC_STATE

DESCRIPTION    This function returns TRUE the first time it is called
               after Search enters PC state.

DEPENDENCIES   srch_state.

RETURN VALUE   boolean indicating if this is first call (TRUE).

SIDE EFFECTS   none.

===========================================================================*/
extern boolean srch_start_pc_state
(
  boolean reset_only
);

/*===========================================================================

FUNCTION       SRCH_GET_CDMA_RAW_RSSI

DESCRIPTION    This function returns a int2 representing the active set
               pilot RSSI.

DEPENDENCIES   None.

RETURN VALUE   RSSI value in dBm.

SIDE EFFECTS   None.

===========================================================================*/
extern word  srch_get_cdma_raw_rssi ( void );

/*===========================================================================

FUNCTION       SRCH_GET_CDMA_RAW_ECIO[_ALL_CHAINS]

DESCRIPTION    This function returns a byte representing the active set
               pilot Ec/IO.

               For the ALL_CHAINS version of the function, the chain-specific
               Ec/IO value will be updated in the three respective input
               parameters.  Note that the return value will only reflect the
               combined Ec/IO (matches legacy behavior).

DEPENDENCIES   srch_aset[], srch_aset_cnt.

RETURN VALUE   Negative Ec/IO value in 0.5 dBm steps.

SIDE EFFECTS   None.

===========================================================================*/
extern byte  srch_get_cdma_raw_ecio ( void );

extern byte  srch_get_cdma_raw_ecio_all_chains ( byte* ecio_comb,
                                                 byte* ecio_rx0,
                                                 byte* ecio_rx1 );

/*===========================================================================

FUNCTION SRCH_GET_CDMA_RSSI[_ALL_CHAINS]

DESCRIPTION    This function returns a byte representing the current
      CDMA RSSI value in P(AGC) + Ec/Io form. This byte can then be
      arbitrarily mapped to UI display space using an equation, table,
      or case statement.  RSSI is stored internal to search as a word,
      for better filtering results.

      The RSSI if filtered using an (N-1)/N IIR filter for a smoother signal
      strength display. This CDMA RSSI is written into DB (database)
      for consistency with analog RSSI. This database value is thereby
      updated at a rate determined by the caller.

      The function returns the raw, unfiltered CDMA RSSI. Targets which
      need unfiltered values must call this function explicitly rather
      than using the filtered DB value.

      For the ALL_CHAINS version of the function, the chain-specific RSSI
      value will be updated in the three respective input parameters.  Note
      that the return value and DB update will only reflect the combined RSSI
      (matches legacy behavior).

DEPENDENCIES    srch_aset[], srch_aset_cnt.

RETURN VALUE    A 6-bit RSSI index, 0..63,  representing RSSI from
                -75 dBm to -106.5 dBm, in 0.5 dBm steps.

SIDE EFFECTS    Filtered database CDMA_RSSI is updated.

===========================================================================*/
extern byte  srch_get_cdma_rssi_all_chains ( byte* rssi_comb,
                                             byte* rssi_rx0,
                                             byte* rssi_rx1 );

/*===========================================================================

FUNCTION      SRCH_MEMPOOL_CAPTURE

DESCRIPTION   This function signals FW to do a mempool capture.

DEPENDENCIES  This function will capture mempool data of RF device associated
              with the PRIMARY RX STATE MACHINE.

RETURN VALUE  returns 1 if the mempool capture was a success, 0 in case of
              failure.

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_mempool_capture(void);

#endif /*SRCH_V_H*/
