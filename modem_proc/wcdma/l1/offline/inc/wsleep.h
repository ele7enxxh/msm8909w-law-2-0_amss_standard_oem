#ifndef WSLEEP_H
#define WSLEEP_H
/*===========================================================================

            S L E E P   R E L A T E D   D E C L A R A T I O N S

DESCRIPTION
  This file defines the external interface (functions, types, data) for
  the sleep module in WCDMA mode.

EXTERNALIZED FUNCTIONS

wsleep_init
  This function is called to initialize the sleep module.

go_to_sleep
  This function is called to initiate going to sleep.
  
sleep_abort
  This function is called to abort the sleep process.

exit_sleep_cycle
  This function is called to reset the sleep state and print some stats 
  on the sleep cycle that just ended.
  
exit_sleep_mode
  This function is called when DRX mode is exitted.

wsleep_enable_agc_and_trk_lo_adj
  Keeps DATAPATH_AGC clock regime and trk_lo_adj PDM active during sleep.

wsleep_enter_deep_sleep
  Called by WCDMA L1 to enter deep sleep (no signal/save power) mode.

wsleep_exit_deep_sleep
  Called by WCDMA L1 to exit deep sleep (no signal/save power) mode.
  
wsleep_ready_for_wake_up
  Called by DRX to check/wait until ref counter has been slammed on wake up.
  
INTERNALIZED FUNCTIONS

sleep_power_down_micro
  This function is called to power down the micro under certain conditions.

sleep_timer_1_isr
  Interrupt service routine for the Sleep controller Timer 1.

wsleep_kick
  This function is called to send a signal to make the Sleep task runnable.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  sleep_init must be called before go_to_sleep
  sleep_abort can be called after go_to_sleep

REFERENCES
  Sleep time line document
  Sleep software SDD

Copyright (c) (2002-2009) by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/sleep.h_v   1.10   03 Jul 2002 17:49:26   arao  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wsleep.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why 
--------   ---     ----------------------------------------------------------
05/05/09   rmak    Moved all content to wl1sleep.h
04/17/09   mg      Fix compile warnings
01/04/09   mg      Change the rf warmup time for 8200
12/08/08   rmak    Mainlined remnants of FEATURE_UNIFIED_SLEEP_TASK
11/13/08   rmak    Fixed Compiler Warning in previous checkin
11/08/08   mg      Remove unused sleep module states
10/09/08   rmak    Mainlined FEATURE_UNIFIED_SLEEP_TASK
                   and FEATURE_WCDMA_USES_SHARED_SLP_CTLR_DRIVER
04/17/08   mg      Added wsleep_enable_agc_and_trk_lo_adj()
03/28/08   rmak    Removed unused APIs wsleep_enable_agc_and_trk_lo_adj
                                   and wsleep_rfagc_disable_trk_lo_adj_pdm
12/13/07   kps     Re-Add support for FEATURE_NEW_SLEEP_API (new sleep voting module)
                   Additional featurization with FEATURE_WL1_NEW_SLEEP_API
11/26/07   kps     Back out support for FEATURE_NEW_SLEEP_API
11/05/07   kps     Add support for FEATURE_NEW_SLEEP_API (new sleep voting module)
03/15/07   mg      Remove set_wcdma_mdsp_clock_to_unassigned
01/09/07   rmak    Removed SLEEP_SETUP_TIME_CX8
01/08/07   mg      Remove silence_micro and silence_micro2
12/18/06   rmak    Removed wsleep_reacq_started()
07/20/06   kps     Add wsleep_fee_is_needed() and wsleep_fee_is_possible() declarations
05/23/06   rmak    Move sleep task functionality under FEATURE_UNIFIED_SLEEP_TASK
02/27/06   mg      Extern the function wsleep_rfagc_disable_trk_lo_adj_pdm
02/15/06   scm     Extern sleep_wake_up_timer for FEATURE_INTER_SIB_SLEEP.
09/30/05   mg      Add wsleep_oktcxo declaration
08/24/05   kps     Move RF_WAKE_UP_TIME_IN_MILLISECONDS here from wsleep.c so 
                   BMC-L1 can use it
06/01/05   mg      Extern num_times_tcxo_turned_off
05/09/05   scm     Add boolean wake_up_for_ctch parameter to go_to_sleep().
04/24/05   asr     Added extern declaration for wsleep_relinquish_short_ticks().
02/18/05   mg      Removed unused state SLEEP_TCXO_PLL_IN_WARM_UP
01/26/05   mg      6275 specific definitions.
12/30/04   mg      Format the file. No code modifcations
07/28/04   rc      Added extern declaration for keypad_scan_keypad() function
03/23/04   sr      Featurized silence_micro and silence_micro2 functions
01/28/04   scm     Move sleepModuleState var info from .c into here.
09/12/03   asr     Declare functions wsleep_ready_for_wake_up() and wsleep_reacq_started()
07/30/03   scm     Add enable_agc_and_trk_lo_adj(), for keeping PDM active during sleep.
05/28/03   scm     Change some comments related to mDSP clock speed switching.
05/21/03   asr     extern declaration of num_frames_sleep_clk_uncalibrated.
04/24/03   scm     Add new routines for WCDMA mDSP clock speed optimizations.
04/01/03   scm     Change sleep_abort() to return void instead of boolean.
02/26/03   asr     Added declaration of wsleep_drx_released()
02/12/03   asr     Added declaration of wsleep_adj_sleep_clk_freq()
11/13/02   scm     Support new deep sleep mode to save power when no signal present.
09/27/02  gw/asr   Merged GSM sleep with WCDMA sleep : moved common code
                   to sleep.h; this file renamed to wsleep.h
09/09/02   asr     Added SLEEP_DIAG_OKTS_SIG
08/13/02   asr     Added SLEEP_BT_OKTS_SIG, extern for sleep_bt_clk_rgm
07/24/02   asr     Added SLEEP_LSM_OKTS_SIG, extern for sleep_uart_clk_regm
                   for uart to vote for tcxo shutdown; renamed exit_sleep()
                   to exit_sleep_cycle()
07/03/02   asr     Added SLEEP_UIM_OKTS_SIG
06/11/02   asr     Added function sleep_process_timer_sig() to the external interface.
                   Further changes to support moving of rf_wakeup() from sleep to L1 context.
06/04/02   asr     Added function go_to_sleep_without_rf().
04/25/02   asr     Redefined signals to sleep task; added signals
                   from UI, HS, VOC, SND;
                   Added in standard macros: SLEEP_ASSERT_OKTS, SLEEP_NEGATE_OKTS
03/19/02   asr     Added signals for USB to report ok/not ok to sleep
03/14/02   asr     Added SLEEP_USB_RPT 
03/11/02   asr     Added exit_sleep() (again!)
03/07/02   asr     removed sleep_intr_update() function (unused)
02/11/01   asr     added exit_sleep()
02/02/02   asr     created file : basic function prototypes

===========================================================================*/

#include "wl1sleep.h"

#endif /* WSLEEP_H */

