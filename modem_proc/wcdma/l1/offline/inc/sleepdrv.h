#ifndef SLEEPDRV_H
#define SLEEPDRV_H
/*===========================================================================

           S L E E P   D R I V E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the external definitions for sleep controller

Copyright (c) (2002-2009) by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/sleepdrv.h_v   1.1   03 Jul 2002 17:48:04   arao  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/sleepdrv.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who         what, where, why
--------   ---         --------------------------------------------------------
05/05/09   rmak        Moved all content to wl1sleep.h
03/25/08   mg          Adjust the sleep longer varible to -ve sign
03/25/08   mg          Adjust the sleep longer variable to accomodiate for increased time line
12/03/08   rmak        Mainline FEATURE_USE_SLEEP_CTLR and FEATURE_QXT
11/30/08   saik        Removed extern references to non-existent variables and added reference to global variable wl1sleep_FEE_time_in_ms
11/19/08   rmak        Added sleep_drv_set_rf_wu_time_sc to allow feedback to sleep_drv for RF WU time
11/08/08   rmak        Added more WSP points
                       Reduced TICKS_REM_ADJ_SLEEP_LONGER_CX8 by moving 3.5ms into Sleep Setup Time
11/04/08   mg          Use qxt defines for all targets
10/22/08   rmak        Sync Latest and Rel7 versions
                       Add FEATURE_WCDMA_SYSTEM_TIMER_REV2
                       Add FEATURE_WFW_SW_INTERFACE
10/09/08   rmak        Mainlined FEATURE_UNIFIED_SLEEP_TASK
                       and FEATURE_WCDMA_USES_SHARED_SLP_CTLR_DRIVER
09/30/08   mg          Added frame_num_immt_sdt
09/24/08   rgn         Added QXT thresholds
08/15/08   uk          Externed wtog_bplmn_in_progress flag
08/05/08   rmak        Split tcxo_pll_and_rf_warm_up_Cx8 into tcxo_pll_wu_Cx8 and rf_wu_Cx8
06/17/08   rmak        Enhanced DEBUG_WSLEEP_PROFILE_TIME_LINE
05/22/08   kps         Changed compute_current_cell_time() to return a boolean.
05/06/08   vsr         Removed unused macros
03/19/08   rmak        Removed sleepDrvCallBackFunction_type
02/28/08   rmak        Added extern uint32 sleep_drv_rf_warmup_time_sc 
02/21/08   rmak        Added 'sc since' functionality
02/06/08   rmak        Increase  TICKS_REM_ADJ_SLEEP_LONGER_CX8 by 2ms
10/10/07   mg          Add sleep_ctlr_int_status_type
09/24/07   rmak        Separated TCXO and RF warmup time
                       Updated return type of sleepdrv_set_sleep_timer_1_count()
08/21/07   rmak        Changed TICKS_REM_ADJ_SLEEP_LONGER_CX8 to 4ms (from 1.5ms) on 6xxx
07/16/07   mg          Support for 6xxx sclk driver
07/13/07   mg          Move definitions from sleepdrvi.h to this file
06/29/07   mg          Make sleep_drv_ols_has_occured volatile
06/06/07   rmak        Introduced get_sc_rem_to_pich_for_f3
05/30/07   mg          Remove unused define SLEEP_CTLR_ON_LINE_START_INT
05/29/06   kps         Support for CGPS
03/15/07   mg          Remove the function sleep_drv_ready_for_wake_up
02/28/07   kps         Fix FEATURE_MMGPS_WL1 feature support
01/29/07   mg          Extern the variable sleep_drv_multiple_int
01/09/07   rmak        Updated sleep_drv_init
12/20/06   rmak        Feature and extern variable cleanup, updated sleep_drv_setup
12/18/06   mg          Clean up
12/15/06   mg          Sleep profile and clean up
12/08/06   rmak        Mainlining FEATURE MODEM SLEEP CONTROLLER INSTANCE 1
                       and FEATURE INTER SYSTEM TIMING MAINT THROUGH SLEEP CTLR
                       Removed FEATURE SLOW REX TICKS DURING WSLEEP
11/13/06   sk          GSM timing error fix. Defined extern frc_immt_sdt_Cx8.
10/18/06   mg          Add sleepdrv_get_programmed_coarse_time and format
10/11/06   mg          Add define for one second in sleep clocks ONE_SEC_IN_SC
10/06/06   mg          7200 change: Add api for sleep controller fine time.
09/20/06   rmak        Added get_sc_rem_to_pich_for_f3() to measure time from any event
                       until pich in sleep clocks (sc rem).
                       Lowered acuracy of M_BY_N_MS_IN_SC to avoid integer overflows
09/12/06   mg          Add ONE_SEC_IN_MS and fix compile warnings.
09/05/06   mg          7200 change: Use Tramp API to clear the sleep timer 1 pending interrupt
08/23/06   mg          Add sleep_drv_are_rex_ticks_extended function
08/09/06   mg          Use Shared fee ctl to estimate FEE
08/09/06   rmak        Added macro M_BY_N_MS_IN_SC(m,n) for fractional time computations
07/20/06   kps         Add get_num_sleep_clks_for_fee() declaration
07/12/06   mg          Shared sleep controller support
07/07/06   mg          Add macros to install/uninstall slp ctlr isr
06/20/06   asr         Added declaration for sleep_drv_abort_timeout_handled()
05/02/06   mg          6280: No need to wait for CC slam sig
05/08/06   au          Replaced calls to MSM_IN and MSM_OUT with HWIO_IN and HWIO_OUT.
04/06/06   mg          Extern sleep_drv_compute_gfn_gqsym_at_ols and gqsym_ols
03/20/06   mg          Add SLEEP_DRV_RESET_SLEEP_TIMER_1() macro
02/10/06   mg          Change the compute_current_cell_time declaration.
11/28/05   mg          Add TICKS_REM_ADJ_SLEEP_LONGER_CX8 definition
11/16/05   mg          Add USB OKTS to SLOW_REX_TICKS_CONSENSUS
09/30/05   mg          Remove sleep_drv_oktcxo, sleep_drv_okts
09/14/05   mg          Change feature 6275_COMPILE to MODEM_SLEEP_CONTROLLER INST 1
09/02/05   mg          6275 change: Remove SLOW_REX_TICKS_CONSENSUS
06/22/05   mg          LED debug defines
06/03/05   mg          Add function sleep_drv_wait_for_comb_slam
05/31/05   mg          Correct the defintion SLEEP_OEM_*_OKT_EXTEND_TICKS                                      
05/31/05   mg          Add OEM 1, 2, 3 sigs to rex tick extension mask
05/31/05   mg          Extern gsm_clk_enabled and sleep_drv_cst_has_occured
05/20/05   mg          6275: Remove MDP from short rex voting, new definitions
05/12/05   mg          6275: Add MDPS QxT definitions
05/09/05   mg          Featurize timetag code for PLT compile.
05/09/05   scm         Add boolean wake_up_for_ctch parameter to sleep_drv_setup().
04/15/05   mg          Add GPS time tag function prototypes  
04/04/05   mg          Use SLEEP_CAMERA_OKTS_SIG voting only for 6275
03/28/05   sr          Added SLEEP_CAMERA_OKTS_SIG to sleep consensus mask
03/24/05   mg          6275: Sleep interval/coarse time read is corrected.
03/18/05   pv          Add SLEEP_UIM_OKTS_SIG TO SLOW_REX_TICKS_CONSENSUS to get rid of
                       uim time outs
03/07/05   mg          Add CDMA_STMR_INT_* definitions for 6275.
02/18/05   mg          6250/6275 Sleep Ctl register access is through macros
02/08/05   scm         Remove LSM_OKTS from SLOW_REX_TICKS_CONSENSUS.
12/30/04   mg          Format the file. No code modifcations
11/10/04   asr         Add externs for short_qxt, and functions to set qxt parameters.
10/26/04   asr         Added extern for ticks_rem_adj_sleep_longer_Cx8 and couple others.
05/12/04   scm         Add SLEEP_LSM_OKTS_SIG to SLOW_REX_TICKS_CONSENSUS.
03/09/04   jac         Added kypd vote in SLOW_REX_TICKS_CONSENSUS
03/01/04   asr         Added SLEEP_CTLR_BEF_SLEEP_START to sleep_ctlr_int_type
01/16/04   asr         Added parens in definition of SLEEP_DRV_GET_SLEEP_CYCLES_REMAINING.
                       Added extern declarations of several FEE variables.
01/15/04    ih         Created SLOW_REX_TICKS_CONSENSUS and OK_FOR_SLOW_REX_TICKS macros
09/25/03   asr         Added declaration of sleep_drv_ready_for_wake_up(), and sleep_drv_slam_gfn(),
                       sleep_drv_qsyms_until_gtime_ready(), sleep_drv_prepare_for_gtime_snapshot(), and
                       sleep_drv_gtime_snapshot().                 
08/07/03    sr         Added function is_sleep_drv_in_sleep() to return the 
                       status of sleep driver.
08/03/03   asr         Declare a sleep_drv_exit() function, and sleep_drv_abort_fee().
07/01/03   xfg         Changed for Thin UI build
02/12/03   asr         Declared function sleepdrv_adj_sleep_clk_freq()
01/28/03   asr         Extern sleep_drv_oktcxo
10/21/02   asr         Removed dup declaration of ONE_MS_IN_CX8
09/09/02   asr         Removed unused code
07/24/02   asr         Added extern declaration of hs_scan_keypad()
07/03/02   asr         Added sleep_drv_oktcxo and fee_completed, boolean flags
06/11/02   asr,abp     Initial cut

===========================================================================*/

#include "wl1sleep.h"

#endif /* SLEEPDRV_H */

