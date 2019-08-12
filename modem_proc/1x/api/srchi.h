#ifndef SRCHI_H
#define SRCHI_H
/*===========================================================================

                   S R C H I    H E A D E R    F I L E

DESCRIPTION
   This file contains definitions used internally by the Search task.
   It should NOT be included by any source file outside the Search task!

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srchi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/16   sst     Expose srch_state to CP for logging with QSH module
02/05/16   pk      1x2GTA Feature changes
11/26/14   jh      remove qw.h include
08/21/12   dkb     Added SRCH_ACCESS_STATE to srch state enumeration type
08/08/11   cjb     Export API: srch_filt_iir()
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/01/11   sst     Added state SRCH_LTO1X_STATE for LTE->1x TT, Meas, CSFB
01/26/11   vks     Remove unnecessary rf header file include.
11/18/10   jtm     Removed featurization.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
09/09/10   mca     Lowered RGS timer to 1000ms
06/21/10   adw     Added "is asleep" function.
06/10/20   jtm     Eliminate dependency on log_dmss.h
02/16/10   jtm     Removed FEATURE_PLT.
02/12/10   sst     Remove srch_peak_[de]init prototypes
11/20/09   bb      Added IS_INACTIVE_STATE macro
10/15/09   bb      Added support for "LTE to 1X IRAT core" feature
10/02/09   sst     Added IS_INIT_STATE macro
09/25/09   vks     Moved inclusion of srch_dep.h to srch_lib_int.c from
                   srchi.h
09/18/09   adw     Added srchcom_sm and peak prototypes for dynamic memory
                   allocation. Mainlined FEATURE_ACQ2.
08/05/09   jtm     Lint fixes.
07/29/09   bb      Added nghbr_srch_mode field to the srch_parm_struct_type
07/28/09   mca     Fix compiler errors for Genesis
06/17/09   adw     Disable message flag debug information.
06/11/09   jtm     Replaced FEATURE_1X_Q6_CRIT_SECT with T_QDSP6.
05/11/09   jtm     Added include for log_dmss.h to support diag decoupling.
05/01/09   adw     Re-added NOTUSED macro for SCMM bring up.
04/23/09   adw     Removed NOTUSED macro already defined in comdef.h.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Cleaned up incs and eliminated implicit includes.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
03/20/09   adw     Include srch_sect.h for srch_sect_struct_type.
03/19/09   adw     Include deint_v.h for deint_rc_type.
03/18/09   adw     Removed unnecessary includes.
03/16/09   jtm     Lint fixes.
03/16/09   adw     Replaced MSM_LOCK with INTLOCK to remove dep on msm_help.h.
03/16/09   adw     Changes to replace INTLOCKs for q6 port.
                   Added macros to support q6 atomic and critical sections.
03/09/09   jtm     Fixed compiler warnings
02/10/09   adw     Merge from 2h08 branch, based on VU_MODEM_1X_SRCH.13.00.01
11/19/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_SRCH.12.00.13
08/28/08   adw     Included srch_dep.h to verify internal feature dependencies.
08/13/08   adw     Added FEATURE_SRCH_MSG_LITE for ULC.
                   Replaced STM_NULL_PAYLOAD_CHECK w/ SRCH_NULL_PAYLOAD_CHECK.
08/13/08   adw     Featurized SRCH_NULL_PTR_CHK to remove MSG_ERROR for ULC.
08/24/07   sst     Updated init function param to receive srch_cmd_type
                   Removed _pc_ and _zz_ functions that no longer exist
05/02/07   cab     Moved to new sleep voting
12/15/06   tjc     Modified traffic to use the sector module
12/14/06   pa      Removed unused function srch_get_freq_est().
10/18/06   bn      Added new functions to handle the time transfering from
                   DO to 1x
10/13/06   trc     Changes for new STM grammar
08/03/06   tjc     Removed unused signals
07/19/06   trc     Decoupling STM from SRCH
07/11/06   tjc     Modified the traffic state to use the state machine module
07/11/06   tjc     Implemented searcher peak module
06/06/06   pa      Removed unused variable srch_last_good_reacq.
06/01/06   tjc     Removed subsignal functions
05/31/06   tjc     Signal cleanup
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
01/17/06   awj     Lint cleanup; type matching
12/02/05   awj     Added FEATURE_SRCH_RELA_RX0_ONLY
11/04/05   kwo     Remove IDLE_HO_ALLOWED_NEIG() macro
11/02/05   awj     Stripped FEATURE_SRCH_PCG, mainlined RTL and IRM
10/25/05   awj     Removed global NV variables, created srch_nv.[ch]
10/18/05   awj     Lint cleanup
10/17/05   kwo     Added SRCH_NULL_PTR_CHK()
10/10/05   awj     Moved srch_report_frame to srchtc.c
09/23/05   ejv     Deprecate rex timer types/calls.
                   Change sum_power to 32 bits.  Delete unused macros,
                   variables, and function calls.
09/19/05   ejv     Remove unused defines.
09/08/05   ejv     Add functions from srch.h.
08/31/05   ejv     Update FEATURE_SRCH_PC2 featurization.  Remove now unused
                   signal checks.  Move IS-95B enum to TC files.
08/11/05   sst     Make FREQ_TIMER_MS 10 secs
08/05/05   ejv     Remove srch_rssi_to_db.
08/03/05   ejv     Mainline FEATURE_TRANS_MANAGER.
07/28/05   ejv     Extend lecio support throughout SRCH task.
07/27/05   pa      Added CHIPS_PER_ROLL, CHIPX8_PER_ROLL, CHIPX8_ROLL_MASK.
07/01/05   ejv     Removed unused ecio_thresh_eng.
06/24/05   ejv     Moved TC-specific struct types and timers to TC files.
06/24/05   ejv     Delete srch_freq_timer_done.
06/23/05   ejv     Move DB_DOWN to srch_conv.h.
06/22/05   sfm     Removed RF_TUNE_DONE() macro
06/13/05   br      Removed unused PPM macros.
06/10/05   ejv     Remove include of srch_fing_driver.h.
05/31/05   rng     Merged in from SHDR Sandbox.
05/24/05   ejv     Removed unused externs.  Moved other to more appropriate
                   locations inside the library.
05/17/05   kwo     Cleanup for library packing
05/16/05   sst     Removed Acq1 and FEATURE_ACQ1_AND_ACQ2
05/04/05   ejv     Moved Antenna-specific Parms to srch_fing_driver.h.
04/25/05   kwo     Merge in of FEATURE_SRCH_PC2
04/01/05   sfm     Added srch_eng to srch_path_struct_type.  Added #defines for
                   path_type.  Removed srch_fing_earliest_fing().
02/17/05   abh/gs  Removed quantization effects on the TrkLo PDM
12/15/04   kwo     Rework of DH interface
12/06/04   bt      Added SRCH_ENTER/LEAVE_CRIT_SECT defines.
12/01/04   rng     Removed depricated srch_powerdown() and
                   srch_store_freq_est().
11/15/04   ejv     Replaced rf_ with rfm_ equivalents.
11/04/04   kwo     Translated DB_DOWN to use srch_conv_db_adj()
11/03/04   kwo     Removed obsolete fade command support
10/25/04   ejv     Removed srch_update_sch_walsh_len.
10/15/04   kwo     Removed unslew
10/12/04   kwo     Removed srchpc_last_slot_eng
10/11/04   ejv     Unfeaturize pilot walsh values.
10/07/04   sfm     Removed include of srch_fing_driver.h
09/30/04   ejv     Moved SRCH_USE_PREV_TRK_LO and srch_cmd_q from srch.h.
09/29/04   sfm     Changed include of srch_fing.h to srch_fing_driver.h
09/27/04   ejv     Moved TC prototypes to srchtc.h.
09/23/04   kwo     Added state macros, FRAME_DONE(), demod info packet support
09/13/04   sst     Integrated with ACQ2.0 dev, featurized as FEATURE_ACQ2
09/09/04   ejv     Remove REL_C_DV featurization.
09/03/04   kwo     Linear ecio triage cleanup
09/03/04   ejv     Add ficch_lcm and cpcch_info.
08/27/04   ejv     Mainline more features.
08/26/04   kwo     Modified DB_DOWN to return a uint32
08/24/04   ejv     Removed srch_win_pool and srch_win_q.
08/23/04   rng     Add define for void_srch_wait.
08/23/04   kwo     Modified srch_filt_iir() to support uint32
08/18/04   gs      Added prototype for srch_get_freq_range().
08/12/04   dna     Transitions to SRCH_START_STATE now go to SRCH_CDMA_STATE
07/26/04   ejv     Remove srch_msm_version.
07/22/04   kwo     Removed finger ecio table
07/22/04   br      Removed unused GPS macros.
07/20/04   gs      Modify VALID_BAND_CHAN to support band classes other than
                   0 and 1.
07/19/04   ejv     Mainline FEATURE_IS95B_MAHHO, _SYS_RESELEC, PILOT_RPT and
                   _ACCESS_HO.
07/08/04   ejv     Remove srch_catnap_ctr_dz.  Move SRCH_INACTIVE_STATE to
                   replace SRCH_DZ_STATE in state enum.
06/30/04   dna     Changed SRCH_START_STATE to SRCH_INACTIVE_STATE.
                   Added srch_exit_service(), called when we enter Inactive
                   state or when we return to CDMA state from any state other
                   than inactive.
06/22/04   br      Moved support for PDSM_SRCH_INTERFACE. Removed unused
                   variables.
06/18/04   dd      Removed GPS NV items.
06/16/04   jcm     Moved TC Specific functions to srchtc_util.h/.c
06/15/04   kwo     Moved srch_pc_eval_handoff to srch_idle_util
06/02/04   jcm     Convert to using srchi_t types header file
06/01/04   jcm     Added VALID_BAND_CHAN() Macro
05/28/04   jcm     Sector module integration
05/27/04   ejv     Mainline FEATURE_BROADCAST_SMS.
                   Renamed FEATURE_ENHANCED_TRIAGE as SRCH_ENHANCED_TRIAGE.
                   Enable SRCH_ENHANCED_TRIAGE.
05/14/04   kwo     Changed return type of srch_set_parm()
04/21/04   sfm     Changed for traffic channel manager
04/07/04   ejv     Removed unused variable from srch_parm_struct_type.
04/02/04   ejv     Removed srch_hho_in_progress and added
                   srch_tc_get_hho_status().
03/23/04   ejv     Support for parms per antenna, including 5 new types
                   and function srch_set_ant_status().
03/16/04   ejv     Additional FEATURE_TRANS_MANAGER checks.
03/04/04   ejv     Removed old/unused externs.  Added new functions
                   srch_tc_cfs_timers_init, srch_tc_get_off_time_fwd(),
                   srch_tc_get_cfs_state(), and srch_tc_get_cfs_state().
                   Removed srch speed from interface to srchtc_cfs_num_srch().
02/24/04   ddh     Cleaned up lint disable/enable
02/20/04   ejv     Added more FEATURE_SEARCH1 checks.
02/17/04   dd      Added srch_gps_handler() prototype from srchgps.h.
02/11/04   ejv     Mainline FEATURE_IS95B.
02/11/04   ejv     Changed FEATURE_IS2000C_DV to FEATURE_IS2000_REL_C_DV.
02/10/04   jcm     Removed first pass of lint errors
02/10/04   ejv     Added FEATURE_TRANS_MANAGER.
02/09/04   sfm     Added #define for FULL_ACQ_AGC_RSSI_THRESH
02/04/04   br      Added separate free queue for PPM search request buffers.
02/02/04   ejv     Removed unused and unnecessary variables.  Removed
                   macro UPDATE_SLOT_MASK and FEATURE_FAST_FINGER_SLEW.
01/30/04   kwo     Removed PC_HO_TIMER_DONE(), PC_THROT_TIMER_DONE()
01/28/04   ejv     Mainline T_MSM6500.
01/21/04   sfm     Added dv_pilot_info to srch_tmp_aset_struct_type
01/21/04   jcm     Removed reminents of the delay timer
01/19/04   ejv     Remove unused variables.
01/19/04   ejv     Remove unused TD-related drivers that used both 8x
                   and 16x searchers, since 16x does not exist.
01/16/04   ejv     Remove FEATURE_SEARCH2, FEATURE_SRCH_SCHEDULER, and
                   FEATURE_SRCH_SCHEDULER_MONITOR.
01/09/04   bt      Added srchpc_update_aset prototype.
01/09/04   gs      Added srch_last_good_win_siz declaration and prototype
                   for srch_sect_find_any_freq.
01/05/04   ejv     Removed unused srch_clr_timer, srch_set_timer,
                   srch_tick_timer and srch_tick_expired.
12/30/03   sfm     Changed extern for srchtc_cfs_visit_timer to be a rex timer
                   and added an extern for srchtc_period_timer
12/19/03   kwo     Cleaned up some ZZ1 style slot tracking vars
12/12/03   jcm     Retain energy of old neighbors when updating neighbor list
12/04/03   aar     Removed srch_rtc_roll_isr since it's currently unused.
11/19/03   jcm     Added FILT_N_GROW() and FILT_I4_GROW() Macros
11/13/03   ejv     Compile out srchtc_process_sch_transaction for 6700.
10/28/03   ddh     Moved NOTUSED macro here from stm.h
10/16/03   kwo     Removed FEATURE_ZZ2_OFREQ_TIMER
                   Added srch_pc_eval_handoff()
10/10/09   bt      Moved srch_comb_post_slam_tt_fing into srch_comb.c
10/07/03   kwo     Added FEATURE_ZZ2_OFREQ_TIMER
09/30/03   ejv     Remove obsolete feature FEATURE_GPS_DISABLE_CHIPX8_SLEEP.
                   Mainlined FEATURE_GPS_SRCH_ENGINE.
09/26/03   ejv     Remove old/unused macros, variables and functions.  Add
                   srch_wakeup_isr prototype from srch.h.
09/24/03   ejv     Extern srch_update_sch_walsh_len.
09/10/03   aaj     Added prototype for RSSI to dB conversion function
09/10/03   kwo     Modified srch_calc_total_eng() to take a thresh arg
08/11/03   va      Moved definition of PPM relevant structures to srch_hw.h
08/05/03   ejv     Remove FEATURE_FING_MERGE_DETECT.
07/29/03   ejv     Removed srchtri_delayed_fing_posn.
07/28/03   ejv     Mainline FEATURE_ZZ2.
07/24/03   ejv     Add srch_ch_rc_type from srch.c and extern srch_rc.
07/14/03   ejv     Remove FEATURE_SPECIAL_MDR.
07/07/03   ejv     Mainline FEATURE_ANTIREGISTRATION_BIAS.
                   Mainline FEATURE_ENHANCED_STANDBY_III.
                   Mainline FEATURE_TRIAGE_LOGGING.
07/02/03   ejv     Fixed featurization.
06/23/03   ejv     Remove FEATURE_SEARCH3.
06/19/03   ejv     Update interface to srch_rpt_ta().
06/15/03   rdh     Added srch4 dump processing signals.
06/11/03   rdh     Streamlined search governor.
05/29/03   kwo     Added srch_normalize_eng()
04/28/03   sst     Mainlined FEATURE_SD20 and SRCH_ENABLE_SD20
                   Added support for searcher's 1 & 4 for sys_meas
04/16/03   aar     Added RTC related macros/variables.
04/03/04   bt      Removal of QPCH PLT support.
03/24/03   ejv     Moved placement of SRCH_PCG_STATE in enum.
03/24/03   sst     Added defines for ChipX16 conversions
02/18/03   br      Added proper featurization.
02/18/03   va      SRCH4 AFLT support.
02/14/03   kwo     Added Finger RSSI to ecio table
02/11/03   gs      Move SRCH_ENABLE_SD20 definition into custsrch.h.
02/11/03   kwo     Mainlined call to srch_reset_pilot_info
02/05/03   gs/kwo  Add in Rel A support for ZZ2.
02/05/03   gs/br   Add GSC_TIMER_EXPIRED definition.
02/04/03   gs      Add srchint_slam_at_reacq flag.
02/03/03   gs      Merge with MSM_SRCH.08.06.60, specifically adding the
                   reacq parameter definitions shared between ZZ and PCG.
01/28/03   sst     Encapsulated srch_win_center to srch_hw.c/h
01/17/03   bt      Merge FEATURE_ZZ2 from SRCH_SLOTTED_DEV.00.02.00.
01/14/03   gs      Merge with MSM_SRCH.08.06.57
01/13/03   sst     Removed freq_incl from sect struct, All pilots now have
                   freq and band class info.
01/09/03   gs      Brought in consistency check for srchzz_wakeup_state
12/23/02   ejv     Changed DOG_RPT to SRCH_DOG_RPT.
12/03/02   pj      Partial merge with 5500 development (MSM_SRCH.08.06.52).
12/02/02   ejv     Additional EBCSMS changes.
11/26/02   bt      merge FEATURE_ZZ2 from SRCH_SLOTTED_DEV.00.01.03
11/25/02   ejv     Support for TD on NCC.
11/25/02   ejv     Mainlined !FEATURE_SRCH_PC_IFACE.
11/22/02   va      Added srchzz_chipx8_sleep_allowed_int variable
11/19/02   gs      Move SRCH_OOK_SUBSIG back to SRCH_OOK_SIG
11/05/02   gs      Move SRCH_OOK_SIG to SRCH_OOK_SUBSIG
10/31/02   bt      add micro processor dump macro
10/30/02   gs      Merge with MSM_SRCH.08.06.47
10/21/02   rm      Added search4 lost dump timer
10/09/02   va      Removed SRCH4_TRIG_DONE Macro
10/04/02   sst     Added support for 4 & 6 searcher peaks
10/02/02   gs      Merge with MSM_SRCH.08.06.42
07/30/02   gs      Initial merge into perforce for cougar.
06/24/02   aar     Added variable declarations for GPS fake DZ sleep.
06/03/02   aar     Replaced srch_get_srchzz_chipx8_sleep_allowed with srch_get_
                   srchzz_keep_chipx8_on.
05/31/02   ejv     Removed vc_osc_ctl.
05/31/02   ejv     Added lc_mask to srch_idle_info_type for PLT.
05/30/02   aar     Added declaration of variable srchzz_chipx8_sleep_allowed.
05/30/02   ejv     Mainlined T_MSM5000.
05/24/02   ejv     Added FEATURE_IS2000_REL_A_SVD.
05/21/02   ejv     Mainlined FEATURE_IS2000.
05/09/02   br      Moved some subsignal handling function prototypes to srch.h.
05/02/02   ejv     Removed FEATURE_QUARTER_ROLL_TIMELINE.
04/24/02   br      Removed srchzz_enable_and_init_rf_digital() prototype.
04/22/02   ejv     Added defines from srch.h.
04/17/02   ejv     Added section for doing searcher feature sanity checks.
03/21/02   br      Removed FEATURE_SRCH_BATCH_LOG. Added proper GPS and PPM
                   feature names. Rearanged gpsOne definitions.
03/16/02   br      Added support for the new GPS software architecture and
                   removed subsignal definitions.
03/05/02   ejv     Extern new flag srch_hsd_call_active.
03/01/02   aar     Changes related to new NV item: NV_GPS_DOPP_SDEV_I.
02/27/02   ejv     Featurization around "bcch_supported" is changed.
02/26/02   ejv     Removed duplicate definitions.
02/22/02   ejv     FEATURE_IS2000_REL_A_NCC -> FEATURE_IS2000_REL_A_CC.
02/21/02   ejv     Added FEATURE_SRCH_PC_IFACE and FEATURE_IS2000_REL_A_NCC.
02/20/02   va      Support for searcher 4 for MSM 6050 and for going to GPS
                   from ACQ.
02/11/02   rm      Fixes for loading the long code mask for QPCH (MSM6050).
                   Scale the qpch soft decisons and threshold values to
                   be backward compatible with 5100.
02/05/02   ejv     Removed srch_qpch_enabled and added srch_qpch_is_enabled().
01/30/02   ejv     Removed FEATURE_JSTD008_DEBUG.
01/18/02   sst     Moved Subsignal Definitions - from srch.h
01/16/02   ejv     Added T_MSM6000.
01/12/02   sst     Modified to use the new srch_* files that replaced srchdrv.c
12/02/01   br      Added SRCH_RESTART_SEARCH_SUBSIG for GPS MSTO. Rearanged
                   subsignal definitions. Added srch_cdma_search_active()
                   definition.
12/02/01   pj      Added SRCH_GPS_NEED_RX_TX_SUBSIG for GPS MSTO.
11/30/01   gs      Remmoved unneeded srchtc_next_sector prototype.
11/30/01   gs      Changed srchtc_next_sector prototype to no longer
                   depend on passed-in slew.
11/21/01   br      Changed status dump occurrence type and added new function
                   definition srch_collect_srch_sect_requests().
11/15/01   ejv     Changed FEATURE_TRIAGE_DEBUG to FEATURE_TRIAGE_LOGGING.
11/12/01   pj      Added GPS antenna offset #defines.
11/01/01   gs      Changed EXPECTED_MAX_OFF_TIME_FWD to 3 frames from 6 frames.
10/08/01   ejv     Added additional REL_A PLT support.
09/25/01   rm      Added a flag used to indicate disabling of finger merge
                   detection when going to sleep.
09/25/01   ejv     Added new function srch_agc_to_dbm().  Added common_path
                   structure for energy storage.  Added pilot_rec and
                   pilot_rec_type to fingers.  Added FEATURE_IS2000_REL_A_TD.
09/17/01   pj      Changed FEATURE organization of SUBSIG definitions to allow
                   non-GPS_MODE builds to compile.
09/11/01   lcc     1. Modified mc_trans in srch_parm_struct_type to contain
                     queues for transaction management instead just an array of
                     2.
                   2. Changed maximum number of pending transactions from
                      2 to 10.
09/07/01   aaj     Added QPCH ULPN mask changes for MSM5500
08/22/01   ejv     Added CDMA_CHIP_RATE and CDMA_CHIP_RATE_PER_MS.
07/10/01   rm      Removed declaration of srch_fing_reset_frame_cnt.
07/06/01   br      Added support for abort_srch_callback in SRCH scheduler.
                   Added srch_stop_current_ppm_srch() definition.
06/25/01   ejv     Renamed srch_fing_cfg_fch_qof() to srch_fing_set_qof().
                   Added function srch_fing_cfg_all_qof().
06/20/01   ejv     Removed srch_winsize_slew.
                   Mainlined FEATURE_SRCH_SLEW_IN_DRV.
                   Removed FEATURE_DIAG_V7_LOG_IFACE.
06/12/01   abh     Changed prototype for the srch_gps_record_fing_comb_offset
                   function. Added variables to handle the gps time transfer
                   problem.
06/05/01   ejv     Created FEATURE_FAST_FINGER_SLEW and replaced some T_MSM5000
                   checks with FEATURE_IS2000.
06/05/01   rdh     Changed the meaning of temperature estimation constants.
05/30/01   ejv     Removed #ifdef from around 95B handoff params.
05/28/01   pj      Added srch_srch3fw_ready_timer.
05/21/01   br      More SRCH Scheduler support.
05/18/01   br      Added support for subsignals. Cease sharing srch_gps_timer
                   between GPS and PPM modes. Increased SRCH3 lost status dump
                   timeout period. Changed search duration determination for
                   setting lost search dump timer. Corrected certain
                   featurizations.
05/17/01   rm      Added a function prototype for srch_set_finger_pair_merge().
05/03/01   ejv     Added macros ABS and LABS.
05/01/01   br      Added SRCH Scheduler support. Moved searcher slewing to
                   SRCH drivers. Added shoulder search mode support.
04/26/01   rdh     Changes for 8X searching in all states, code streamlining.
04/10/01   ejv     Externed new function srch_start_rand_buf_fill for FEATURE_DH.
03/29/01   ejv     Added FEATURE_IS2000_P2.
03/16/01   ejv     Removed ULPN mask changes for MSM5100.
03/14/01   ejv     Modified comment.
03/14/01   ejv     Removed FEATURE_IS2000_REL_A from finger structure.
03/12/01   dgy     Added a variable for FEATURE_IS2000_QPCH_CCI.
03/05/01   ejv     Removed #ifdef from around srch_rssi_timer.
03/02/01   br      Added initial support for gpsOne PPM on MSM5105
                   (FEATURE_MSM5105_PPM_SRCH_ENGINE).
02/28/01   br      Added FEATURE_SEARCH3 to featurize searcher 3 operation.
02/27/01   ejv     Added FEATURE_IS2000_REL_A.
02/27/01   ejv     Removed srch_reacq_mstr_posn_set.
02/16/01   dgy     Added FEATURE_IS2000_QPCH_CCI, the initial support for CCI.
02/14/01   ejv     Added changes for setting the ULPN mask on MSM5100.
02/14/01   rdh     Added initial support for System Determination 2.0.
02/07/01   dgy     Added a variable for PC framing type.
02/07/01   ejv     Removed temporary changes for MSM5100.
02/02/01   br      Added FEATURE_GPS_MODE to featurize GPS operation.
01/31/01   rm      Added FEATURE_HW_FING_MERGE_DETECT.
01/31/01   br      Renamed FEATURE_GPS_MODE into FEATURE_GPS_SRCH_ENGINE.
01/25/00   abh     Changes to read the GPS RF Loss calibration value from NV.
01/22/01   ejv     Added SEARCH1_DUMP and SEARCH2_DUMP macros.
01/22/01   ejv     Mainlined T_SLEEP.
01/18/01   ejv     Added MSM5100 support.
01/10/01   dgy     For FEATURE_UP_CLOCK_SWITCHING, used TCXO for the uP clock
                   during the catnaps between PI1 and PI2 and between PI2 and
                   the PC slot boundary in QPCH.
01/08/01   ejv     Added new sub-structures for channel parameters.
12/14/00   ejv     Added another parameter to srch_fing_cfg_qof().
12/12/00   ejv     Moved srch_restore_pch() for PLT only.
12/11/00   pj      Put srch_enable_moving_psd code inside FEATURE_GPSONE ifdefs,
                   rather than simply in FEATURE_IS95B* ifdefs.
12/07/00   bgc     Added srch_restore_pch().
11/22/00   dgy     Added support for catnaps between PI2 and PCH slot boundary.
11/21/00   bgc     Updated for PLT support.
11/06/00   ejv     Added new searcher driver interface.
11/03/00   br      Exchanged gpsOne function srch_master_fing_get_pos() with
                   srch_get_master_finger_data().
11/02/00   dgy     Added a variable and moved a definition to srchzz.c for
                   FEATURE_IS2000_QPCH.
10/23/00   ejv     Externed srch_total_sleep_dur.
09/19/00   abh     Added support for using the RF Delay calibration of GPS Measurements
                   according to the values stored in NV.
09/18/00   ejv     Externed new function srch_trans_qpch_to_pc().
09/14/00   day     Added srch_gps_record_fing_comb_offset() prototype.
09/12/00   day     Move srchzz_enable_and_init_rf_digital() prototype outside
                   of FEATURE_IS2000_QPCH.
09/12/00   pj/abh  Added master_fing_get_pos() prototype.
09/11/00   ajn/dgy Added the QPCH timeline that wakes up before the first PI
           /ejv    bit, goes back to sleep immediately if any of the two PI
                   bits is zero, and stays awake for PCH if both PI bits are
                   non-zero.
09/08/00   eh      Declared qpch channel estimator functions.
09/07/00   lc      Added function prototype for srchtc_process_sch_transaction().
09/06/00   eh      Declared srchzz_enable_and_init_rf_digital().
09/01/00   pj      Added new SRCH_GPS_SRCH_DONE macro.
08/21/00   ejv     Removed FEATURE_PILOT_SCANNER and SRCH_PRE_IS95B_BUILD.
08/06/00   abh/br  Added support for SRCH3 lost status dump and memset().
08/04/00   ejv     Externed srch_get_slot_status() and 2 new last slot qwords.
07/28/00   fc      Mainlined FEATURE_CS_ACTION_TIME_TRANS.
07/13/00   yll     Added "Tune Away Without Drop" capability for FEATURE_GPSONE.
06/21/00   abh     Added macro definition for GPS Srch Status Dump.
06/29/00   abh/br  Added MSM3300 support - FEATURE_GPSONE and FEATURE_SRCH_BATCH_LOG.
                   Changed local definitions from srchtc.c to global and externed
                   srchtc.c function variables to support new features.
06/19/00   ejv     Externed srch_qpch_log_data.
06/13/00   ejv     Removed srch_enable/disable_demod_bypass from slow clock
                   ifdef.  Edited memcpy prototype to match memory.h/string.h.
06/12/00   lc/dgy  Added FEATURE_CS_ACTION_TIME_TRANS to streamline the processing of
                   MC transactions and to support soft handoff transactions.
06/12/00   ejv     Externed srch_fpc_enabled and added ttl_slow to fing struct.
06/09/00    eh     Added support for search window offset.
06/06/00   ejv     Changed int8 to int1.
05/26/00   ejv     Doubled ROLLS_PER_MAX_SLOT_CYCLE.  Externed new slot mask
                   and slot counting variables.  Added new macros for slot
                   mask checking/clearing.  Externed some ZZ functions
                   which are need in PC state.
05/23/00    eh     Added srch_qpch_enabled.
05/15/00   rdh     Simplified and consolidated fing lock threshold variables.
05/11/00   rdh/wli Added improvements to power control symbol combiner handling.
04/24/00   dgy     Added full support for SCH, including handoff, Active set
                   updating, and action time processing.
04/10/00   dgy/ejv Added FEATURE_IS2000_QPCH.
04/10/00   ejv     Added CHIPX8_PER_OFFSET and PILOT_PN_MASK.
03/16/00   ejv     Added ROLLS_PER_MAX_SLOT_CYCLE.
02/25/00   ejv     Changed #ifdef FEATURE_DIAG_STREAMING_OUTPUT to
                   #ifndef FEATURE_DIAG_V7_LOG_IFACE.
02/24/00    eh     Declared srch_enable_demod_bypass and
                   srch_disable_demod_bypass.
02/14/00   rdh     Added FEATURE_FING_MERGE_DETECT.
02/11/00   ejv     Wrapped srch_bypass_interface in FEATURE_SLEEP_SLOW_CLOCK.
02/02/00   dgy     Added support for action time processing.
01/27/00   ejv     Added FEATURE_KEYPRESS_OPTIMIZATION.
01/19/00   ejv     Increased SRCH2_SW_Q_LEN to 42.
01/13/00   ejv     Externed srch_bypass_interface().
01/10/00   rdh     Removed window segmenting.
01/10/00   ajn     Re-Added FEATURE_ENHANCED_STANDBY_III & QUARTER_ROLL_TIMELINE
                   support.
12/21/99   rdh     Added FEATURE_ENHANCED_STANDBY_III.
12/13/99   ejv     Changes for FEATURE_SEARCH2.  Removed duplicate extern
                   of srch_freeze_iq() (it is also in srch.h now).
11/24/99   ajn     Finger "Change Flags" for Supplimental Channel support
                   TEMP_HACKS_FOR_TIGER removed
11/10/99   ejv     Added FEATURE_SEARCH2 and FEATURE_SEARCH2_IDLE.
11/08/99   dgy     Added function prototypes for srch_fing_cfg_qof() and
                   srch_reset_qof().
10/27/99   ajn     Added QPCH support
10/21/99   ejv     Changed srch_wait to use rex_sigs_type instead of words.
10/20/99   dgy     Added support for QOF.
10/07/99   lcc     Merged from PLT 2.0:
           ajn/rm  Added in support for SCH (for PLT).
10/06/99   ejv     Removed some parameters from the srch_msm_config_struct_
                   type.  Removed use_max_inx from srch_sort_sect input list.
10/05/99   wjn     Exported variable srch_ho_to_oband for sparse AGC shipment
                   to DM before hard handoff to a different band class.
10/04/99   ejv     Merged FEATURE_DIAG_STREAMING_OUTPUT.
09/30/99   yus     IS95B code review cleanup.
09/07/99   ejv     Added prototype for function srch_freeze_iq.
08/25/99   wjn     Export srchpc_rssi_rpt and srchpc_sys_reselect_check() for
                   Analog System Reselection algorithm to be run in ZZ state.
08/23/99   ejv     Mainlined FEATURE_ENHANCED_STANDBY_II.  Removed prototype
                   for srch_sect_end.
08/13/99   ajn     Fixed the INCW_MAX and INCI_MAX macros.
08/12/99   ejv     Lint cleanup.
08/12/99   rm      Removed functions declarations relating to enabling
                   I/Q Channels. New Macros have been defined in msm_drv.h.
08/12/99   yus     Made several modifications of the structures and command
                   names under FEATURE_IS95B_MAHHO
08/12/99   yus     Changed FEATURE_IS95B_CFS and FEATURE_IS95B_HARD_HO to
                   FEATURE_IS95B_MAHHO.
08/06/99   ejv     yshi: Added FEATURE_IS95B_CFS support.  ajn: Mainlined
                   FEATURE_ENHANCED_STANDBY and FEATURE_VIRTUAL_COMBINER.
                   Removed pre-MSM3000 support.  Added initial Tiger2 support.
07/24/99   jcw     Merged in PLT changes.
07/23/99   wjn     Added the band class information in macro
                   CURRENT_FREQ_NEIG(i) definition and in function prototypes
                   srch_sect_find() and srch_nset_dup().
07/20/99   yus     Added ota_win_n member in the sect structure. It's used for
                   saving the over-the-air window size.
07/20/99   yus     Defined WINDOW_SIZE_14. It's used for enable 8x mode if
                   neighbor window size larger than that (IS95B system only.)
07/19/99   ml      Added support for PLT.
07/01/99   yus     De-coupling FEATURE_SPECIAL_MDR with FEATURE_IS95B_PILOT_RPT
                   if SRCH_PRE_IS95B_BUILD is defined.
06/28/99   yus     Merged IS95B features to the ASW tip.
06/28/99   yus     Added #ifdef SRCH_PRE_IS95B_BUILD for pre-95b backward build.
06/24/99   dgy     Changed PCS AGC threshold to -100 dBm. Added a new define.
06/10/99   yus     Merged IS95B Hard Hardoff ( w/o Candidate Frequency Search.)
06/09/99   rdh     Merged FEATURE_PILOT_SCANNER.
06/08/99   wli     Removed slow clock code due to redesign.
05/26/99   ejv     Removed prototype for srch_labs, which no longer exists.
05/25/99   ejv     Added srch_fing_disable and srch_fing_restart.
05/24/99   yus     Added IS95B Periodic PSMM feature.
05/03/99   ejv     Added raw_rssi to the fings struct to store the energy
                   needed for reporting Gen TA packet data.
03/22/99   kmp     Merged the changes listed below from the TGP IS-95B baseline.
                   03/10/99   yus     Added access_attempted to
                                      srch_sect_struct_type.
04/22/99   ejv     Added the definition of WIN_TAB_SIZ from srch.c.
04/08/99   ejv     Removed redundant variable declarations.
02/26/99   yus     Merged IS95B features to ARM based code( see below for the
                   features.)
                   02/03/99   ks      Fixed the AEHO and AHO ifdef feature
                                      definitions.
                   11/25/98   yus     Added P_REV checking.
                   11/25/98   yus     Added IS95B Neighbor search enhancements.
                   11/25/98   yus     Added IS95B Access handoff and Access
                                      Entry handoff support.
                   11/25/98   yus     Added IS95B Pilot reporting.
                   11/25/98   yus     Changed #ifdef FEATURE_JSTD008 to run time
                                      checking.
                   09/23/98   yus     Added IS95B soft handoff algorithm.
                   09/23/98   yus     Added IS95B system reselection in Idle
                                      feature.
04/06/99   yus     Changed the value of MAX_NUM_SUP_CH_SUPPORT_IF_FOUR_FINGS
                   to zero.
04/05/99   yus     Added function srch_4th_fing_enable for enabling/disabling
                   the fourth finger.
02/16/99   ejv     Added last_win_siz to srch_sect_struct_type.
02/03/99   ejv     yus: Merge updates for MDR features.
02/03/99   ejv     yus: Added IS95B MDR support.
02/03/99   ejv     Removed debug code, srch_sleep_roll_isr.
02/02/99   ejv     Changed T_VIRTUAL_COMBINER to FEATURE_VIRTUAL_COMBINER.
02/01/99   ejv     Added a number of defines and prototypes for rdh's
                   FEATURE_ENHANCED_STANDBY_II.
01/21/99   ejv     Edited FEATURE_DH - no longer uses DMA.
01/13/99   ejv     ajn: Added knowledge about time reference slamming to
                   srch_comb_arm_slam & added srch_get_vsc_time( )
01/06/99   ejv     Removed _cdecl and ROM - not necessary for ARM.
12/16/98   ejv     Merged code for SLOW_CLOCK.
10/16/98   ejv     Merged changes to support Panther.
10/08/98   ejv     Changed FEATURE_NEW_SLEEP_TIMELINE to
                   FEATURE_ENHANCED_STANDBY.
09/28/98   ajn     Avoid slew computation during finger slew problem.
09/28/98   ejv     Added _cdecl to force C calling to specific functions
                   for Pascal builds.
09/25/98   ejv     Changed prototype of srch_get_freq_est to handle an int2.
09/24/98   ejv     Added const to 2nd parameter in memcpy() to silence
                   compile warnings and match memory.h declaration.
08/24/98   ejv     Removed pilot scanner variables.
08/18/98   ejv     Removed srch_msm versions since hw.h versions are now used.
08/18/98   ejv     Added dh task support, must define FEATURE_DH.
08/18/98   ejv     Added rdh/bgc's FEATURE_DYNAMIC_THROTTLE to the governor.
07/01/98   ajn     Added FEATURE_NEW_SLEEP_TIMELINE macro.
05/13/98   ajn     Used a delayed timeline for MSM2.3 in paging/idle sleep
                   mode.
04/21/98   mk      Added support for msm2310.
04/13/98   dgy     Include customer.h only if CUST_H is defined. Changed
                   FING_CNT and added general TA packet and function to set
                   power combiner threshold for MSM2300.
03/16/98   scdb    Merved CLOCK 2.0 changes. This removes the SRCH_TIMEKEEP_10
                   define.
03/04/98   yus/smo Added new ML eng_to_ecio tables for use in 8x searcher.
02/20/98   smo     Removed new clock support code via ifdef SRCH_TIMEKEEP_10.
                   This represents use of timekeeping VU clock01.00.00.
11/25/97   dgy/smo Changes to use new eng_to_ecio table pointer.
11/17/97   dgy/smo Support for enhanced acquisition algorithm, use of recent
                   temp array.
10/28/97   smo     Added support for msm2300, 8x searcher.
10/28/97   smo     Added support for msm2300, A4.
10/01/97   smo     Added msm2300 support, new srch_msm_version structure.
06/27/97   rdh     Added regional defines for Pilot Scanner.
06/19/97   rdh     Added T_Q, removed pre-NGP support, updated copyright.
05/20/97   rdh     Added Low Power Mode support.
03/21/97   smo     Added customer.h include for FEATUER_JSTD008
03/14/97   jjn     Added (TG==T_MD) to the appropriate conditional compile
                   statements for the Module
01/22/97   rdh     Added srch_get_stop_slew pulled from ZZ state.
12/12/96   smo     Increased SRCH_GOVERN_TIMEOUT to 16ms. Added constant
                   SRCH_GOVERN_ADJ_TIME for governer adjust periods.
11/22/96   rdh     Added Hutch's T_T target definitions.
11/05/96   smo     Increased CDMA_RSSI timeout period to 100ms.
10/22/96   rdh     More ROM savings to make 800 fit 4M flash.
09/26/96   rdh     Improved SC/PC thrash protection with dual-mode sys det.
09/19/96   rdh     Added unconditional PC state idle handoff throttling.
08/15/96   rdh     Added idle handoff delay timers/sigs.
07/11/96   smo     Added more debug information for priority scanning.
06/27/96   rdh     Changed AGC thresh to -90dBm on Systems recommendation.
05/20/96   smo     Added debug variables for JSTD-008 priority scanning
05/20/96   rdh     Added prototypes for sleep-related system time inc functions.
05/09/96   smo     Added JSTD-008 features and definitions.
05/07/96   rdh     Added T_COMP state machine vars in trpt structure.
05/01/96   rdh     Added sleep holdoff time, orig_pend flag for rescan-send.
04/09/96   rdh     Added "fast-revisit" struct members, pathlog proto. consts.
03/06/96   smo     Made RSSI timeout value 100ms for ISS2.
02/29/96   rdh     Added poke capability for thresholds after SRCH_ASET_F.
02/28/96   rdh     Got rid of extra searching Nset, changed over to tick26.
02/01/96   rdh     Added srch_expected_win_pos and srch_sect_stop prototype.
11/30/95   rdh     Added fade report support and internal Nset-keep feature.
11/03/95   rdh     Reduced Micro-acq AGC threshold to -100 from -104 dBm.
09/07/95   rdh     Added FILT_I4 for ZZ slow-open SCI fixes.
09/04/95   rdh     More pre-review cleanup of speed throttling constants.
08/23/95   rdh     Pre- code review cleanup checkin to save RAM/ROM.
07/21/95   rdh     ROM'd some large tables to save RAM space.
06/28/95   rdh     Changes to support fast mini/micro acq on Deep Sleep wakeup.
06/01/95   rdh     Mods for search driver break-up, CSS.
05/10/95   rdh     Added NGP-related interrupt handler declarations.
05/09/95   rdh     Added search speed throttling to allow all window sizes.
05/03/95   rdh     Added agc/pctl enabled flag field.
04/18/95   ras     Added T_I2 for ISS2 operation.
04/14/95   rdh     Added CDMA RSSI processing for all targets.
04/13/95   rdh     Gemini change: Use new sparse AGC/pctl logging packet.
04/06/95   rdh     Made lost dump recovery more responsive (faster).
03/31/95   rdh     Eliminated win_z variability as ZZ now spirals on reacq.
03/30/95   rdh     Merged in lost dump recovery timer code.
03/30/95   rdh     Elim slam bug catch - separated sym siz change from slam arm.
03/23/95   rdh     Added new srch_dz_state definitions for deep sleep.
01/10/95   rdh     Bug fix for PhsIII release - got rid of old "nset_received"
01/09/95   rdh     IS-95 Phase III release:
                    - Added segmented search max calc var.
01/06/95   rdh     IS-95 Phase III upgrades:
                    - Mods for .pos, time reference rework.
                    - Added find_earliest_path, mods to find_earliest_fing.
                    - Made state and status code types consistent with calls.
                    - Added REX timer for handoff/slew throttling.
                    - PC state idle structure streamlining.
10/31/94   rdh     Added PC_ASSIGN needed parameters to idle structure.
10/24/94   rdh     Added PN mask autoswitching capability and slew state
                   MSM hardware error detection declarations.
09/06/94   rdh     Added finger struct referencing var and no path counter
                   in sect struct for individual window pilot centering.
08/04/94   rdh     Removal of annoying tabs/spaces.
07/30/94   rdh     Added IS-95 phase III upgrades.
04/05/94   jah     Added prototype for srch_find_earliest_finger().
03/01/94   jah     Changed cur_state's to srch_state_code_type.
01/06/94   jah     Changed watchdog iface to use dog_report().
12/09/93   jah     Added finger tracing externs under SRCH_FING_DEBUG define.
11/19/93   jah     Added extern for srch_rudely_awakened.
08/05/93   jah     Added prototype for srch_set_sym_siz()
07/26/93   ptw     Added SET_* definitions.
06/18/93   ptw     Added cell_type struct.
06/08/93   ptw     Added support for pwr_comb_ind bit (pci)
04/30/93   ptw     Added sleep stuff.
04/28/93   jai     Added routines to store and retrieve frequency estimates and
                   added freqeuncy estimate callback routine.
10/13/92   ptw     Added srch_cdma_freq variable.
10/12/92   ptw     Moved some regional declarations from srch.h
04/30/92   ptw     Removed FATAL macro (replaced by ERR_FATAL in msg.h)
01/22/92   ptw     Carved this module out of monolithic SRCH.C module.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"

/* Cp */
#include "cai_v.h"

/* Srch */
#include "srch_hw_t.h"
#include "srch_rx_t.h"
#include "srch_v.h"
#include "srchmc_v.h"

/* Other */
#include "dog.h"
#include "log.h"
#include "queue.h"
#include "rex.h"
#include "timer.h"


/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                           Search Task States
--------------------------------------------------------------------------*/

/* DO NOT modify the order of this enum, as it will affect the
   post-processing tools - add new states at the end, before
   SRCH_MAX_STATE.  Also check srch.c to update srch_state_str if
   necessary */
typedef enum
{
  SRCH_INACTIVE_STATE = 1,  /* SRCH is not active */
  SRCH_CDMA_STATE     = 2,  /* Initial state for CDMA operation */
  SRCH_ACQ_STATE      = 3,  /* CDMA acquisition of the pilot channel */
  SRCH_SC_STATE       = 4,  /* CDMA reception of the sync channel */
  SRCH_SLEW_STATE     = 5,  /* CDMA transition from sync to paging channel */
  SRCH_PC_STATE       = 6,  /* CDMA operation on the paging channel */
  SRCH_ZZ_STATE       = 7,  /* CDMA slotted mode sleep state */
  SRCH_TC_STATE       = 8,  /* CDMA operation on the traffic channel */
  SRCH_UNSLEW_STATE   = 9,  /* Obsolete! Don't use */
  SRCH_PCG_STATE      = 10, /* Obsolete! Don't use */
  SRCH_POWERUP_STATE  = 11, /* Initial state on power up */
  SRCH_TT_STATE       = 12, /* HDR Time Transfering */
  SRCH_LTO1X_STATE    = 13, /* LTE -> 1x TT, CSFB, Resel, Meas */
  SRCH_ACCESS_STATE   = 14, /* CDMA system access state */
  SRCH_TA_STATE       = 15, /* Traffic TuneAway State */
  SRCH_MAX_STATE        /* Must remain last item in enum. */
}
srch_state_enum_type;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern srch_state_enum_type srch_get_state( void );

/*===========================================================================

FUNCTION SRCH_FILT_IIR

DESCRIPTION    Gradually grow filter until max filter length, then let it be
               a fixed length filter. This lets the filter quickly ramp and
               be equivalent to the normal IIR filter.  Growing filter has a
               straight linear averaging characteristic while the fixed filter
               has the normal (n-1)/n characteristic

DEPENDENCIES   None

RETURN VALUE   New filtered value

SIDE EFFECTS   None

===========================================================================*/
extern uint32 srch_filt_iir
(
  uint32* filter,  /* Filter */
  uint32 input,    /* Value to filter in */
  uint32 len,      /* Length of filter */
  uint32 max       /* Maximum filter length, if non-zero then filter length
                      is grown but will not exeed this value */
);

#endif /* SRCHI_H */

