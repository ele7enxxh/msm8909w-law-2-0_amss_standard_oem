#ifndef WL1SLEEP_H
#define WL1SLEEP_H
/*===========================================================================
                 WCDMA L1 SLEEP MODULE

GENERAL DESCRIPTION
  This file contains the code for controlling the operation of WCDMA L1 SLEEP

Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1sleep.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---    --------------------------------------------------------- 
02/24/15   ap     Initial check-in for WL1 QSH framework 
05/11/15   sks    Do not do a sleep abort for timetag request in any L1 state.
05/11/15   rs     Removing references to fast cell deconfig API's
02/17/15   kr     Reset last int from sleep controller in NO LOCK case such that 
                  When RRC queries WL1 for SFN it returns valid value
02/02/15   sl     Ensure W Stack is up when RRC calls DL API "dl_get_cell_timing" 
01/30/15   gv     Fix medium compiler warnings.
01/22/15   skk    Wakeup 6 slots early during reselections to facilitate offline PICH to online PICH transitiion timeline. 
12/1/14    skk    Add RF_WKUP_SLP mutex while putting RF to sleep in DPC task.
01/19/15   pr     Created api is_sleep_drv_not_in_sleep to return the slp ctrl state.
12/25/14   kr     call enter mode for dsds in case Rf is waking up from sleep
12/18/14   sks    Remove exit mode calls from multiple places and place it along with the RF/FW sleep in the FCDC flow.  
12/18/14   rs     Code changes to make sure wakeup searches are issued as HPQ searches
12/11/14   sks    Log the time of prep wakeup done at the end of the call and not at power clock on.
12/03/14   rs     Changes to make sure freq correction is applied before starting agc and also setting the 
                  wakeup_search_pending flag appropriately. 
12/04/14   kr     Remove unused function
11/25/14   sad    Change to handle modify activity failure
11/04/14   rs     Code changes for DRDSDS fallback mechanism - part 2
11/17/14   psr    Moved a few externed variables to srchcrutils
11/11/14   rs     Code changes for DRDSDS fallback mechanism - part 2
10/10/14   skk    Reduced entity rf warmup time to 6ms from 7ms as sleep time is recalculated after RF/FW sleep.
10/14/14   rs     Code changes for DRDSDS fallback mechanism
06/20/14   skk    FR19218: ReacqLessPICH(RLP) porting for JO.
08/29/14   sks    Feature cleanup on BOLT.
08/04/14   rs     Fixed compiler warnings
07/03/14   sks    Changes for FCDC
05/29/14   sks    Combined RF wakeup and tune optimizations on BO.2.0
05/14/14   sks    IDLE Wakeup Optimizations and cleanup
01/16/13   sks    GSTMR changes for W2G, removing redundant featurizations, and removing instances of rf_wu_sc.
01/01/13   kr     making a variable externed
12/16/13   as     Porting G2W TA from Triton to Dime.
12/10/13   pv     FR 17754 - Wsleep API for STMR init at wakeup.
11/25/13   rs     Support for adjusting sleep setup time when RxD is enabled in DSDS.
11/20/13   rs     New algo to calculate sleep setup time when RxD enabled. 
11/14/13   pj     Wrapped HWIO access with WL1_HWIO macro
11/12/13   srk    Reducing the RF wakeup time to 5ms to cutdown channel wakeup time.
09/17/13   jkb    Porting Dual Sim from Triton to Dime
04/08/13   kr     Disabling access to USTMR registers while WL1 is asleep with FW suspended, to prevent watchdog bites.
03/30/13   pv     Support for handling reprogrammed micro wakeup int.
02/20/13   hk     Removed FEATURE_INTER_SIB_SLEEP
02/11/13   pv     New API for setting/getting cell/fing decfg status.
01/16/13   dm     Reduce CTCH awake time per DRX cycle.
11/28/12   pv     Fixed compiler warning.
09/17/12   pv     Externs for cell/fing decfg opt.
09/12/12   pv     Change the QTT config from 5*6 to 6*5 
08/29/12   pv     Remove unused variables and added API for PICH timeline opts.
08/23/12   pv     Externed a flag for wakeup srch. 
11/16/12   pv     Dime sleep support. 
08/15/12   gsk    Updating the RxAGC read with new RF api
07/24/12   geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12   zr     Added feature definitions for Dime
07/05/12   pv     Parallel reacq changes.
07/05/12   gsk    Idle mode FRs 1782/1705/1947/2152/1950
04/05/12   gsk    Added Rxd Idle profiling and optimization changes
01/30/12   vs     Feature cleanup.
10/03/11   pv     Externed the MCPM_STMR_BLOCK registered callback.
09/07/11   pv     Removed declarations for sleep timer 1.
07/08/11   saik   Mainlining FEATURE_WCDMA_8200_SLEEP
04/25/11   mc     SW decoupling
09/20/10   pv     Adjusted the ticks_rem adjust parameter for Genesis timeline optimization.
08/23/10   ks     Removed inclusion of mdsp header for Genesis (featurized)
06/23/10   saik   Reduced inner loop backoff by 1 ms. 
06/04/10   saik   Optimized 8200A sleep timeline.
05/20/09   rmak   Deleted sleep_drv_get_sc_mHz
05/12/09   rmak   Removed wsleep_relinquish_short_ticks and keypad_scan_keypad
05/05/09   rmak   Merged in content from sleepdrv.h and wsleep.h
09/16/08   mg     Checked in initial version of file

===========================================================================*/

/* ==========================================================================
**
** Feature Descriptions
**
** ========================================================================*/

/* -----------------------------------------------------------------------

    FEATURE_DETECT_STMR_SLP_CLK_MISMATCH  - Enabled in all targets

       This feature detects if the SFN goes out of sync with
       network SFN. The elapsed time in system clocks and
       in sleep clocks (reference) is calculated in every DRX cycle.
       Ideally both the elapsed values should be almost same.
       If there is a large difference then the SFN is corrected
       and RRC is nudged to read PCCPCH.


    FEATURE_MAINTAIN_GTIME_THROUGH_WSLEEP - Enabled in all targets

       This feature maintains GFN and GSM time during WCDMA drx cycles.
       Upon wakeup GFN and GSM time are slammed.


    FEATURE_SLEEP_TASK - Enabled in all targets

       This feature enables sleep task.


----------------------------------------------------------------------- */


/* ==========================================================================
**
** Header Includes
**
** ========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"

#include "msm.h"

#include "wl1api.h"
#include "slpc.h"
#include "wl1drxmanager.h"

#if defined(FEATURE_MMGPS) ||        defined(FEATURE_CGPS_TIME_TAGGING)
#include "l1lsmif_v.h"
#endif /* FEATURE_MMGPS || FEATURE_MMGPS_WL1 || FEATURE_CGPS_TIME_TAGGING */

#include "l1msm.h"
#if defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC)
#include "wl1_qsh_ext.h"
#endif /* defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC) */

/* ==========================================================================
**
** Constants
**
** ========================================================================*/

/* RF warmup time including rf_wakeup() in microseconds
 * this shdn't be > 65000 because its used as a word16 parameter
 */
#ifdef FEATURE_WCDMA_REACQLESS_PICH
#define DRX_PRLL_REACQ_TRIG_THRESH 5
#endif
#define RF_WAKE_UP_TIME_IN_MILLISECONDS 6

/* Value to ARM the ST_EVENT state machine */
#define STMR_ARM_ST_EVENT_ENA   (0x1 << WL1_HWIO_SHFT(STMR_ST_EVENT, CTL_ARM_ST_EVENT))

/* Send GO_TO_SLEEP signal when STMR_ST_EVENT occurs */
#define STMR_SLEEP_ARM_ENA      (0x1 << STMR_ST_EVENT_CTL_SLEEP_ARM_SHFT)

/* Capture system time value at ST_EVENT */
#define STMR_STATUS_DUMP_ST_EVENT_V     0x2

#define KEYSENSE_RD MPM_KEYSENSE_GROUP_STATUS
#define STMR_ST_EVENT_CTL_SLEEP_ARM_SHFT WL1_HWIO_RSHFT(STMR_ST_EVENT_CTL_SLEEP_ARM)
#define STMR_ST_EVENT_CTL_EVENT_TIME_SHFT                              \
        WL1_HWIO_RSHFT(STMR_ST_EVENT_CTL_EVENT_TIME)
#define STMR_REF_COUNT_STATUS_MICRO_FRAME_REF_COUNT_BMSK               \
        WL1_HWIO_FMSK(STMR_REF_COUNT_STATUS, MICRO_FRAME_REF_COUNT)

#define MDSP_C_qftEnThreshold MDSP_qftEnThreshold

/* Defintions for bit positions for the debug LED */
#define DEBUG_LED_1_POS 0x2  /* TCXO      */
#define DEBUG_LED_2_POS 0x4  /* DRX state */
#define DEBUG_LED_3_POS 0x8  /* INT       */
#define DEBUG_LED_4_POS 0x10 /* RF        */

/* Initial adjustment of ticks_rem_adj_sleep_longer_Cx8 */
#define TICKS_REM_ADJ_SLEEP_LONGER_CX8 (-7 * 30720) /* 7 ms */

/*------------------------------------------------------------*/
/*  Defintions used by the sleep controller SW                */
/*------------------------------------------------------------*/
#define ONE_MS_IN_CHIPS            (3840)
#define ONE_MS_IN_CX8              (30720)
#define ONE_MS_IN_STMR_TICKS       (15)
#define ONE_FRAME_IN_CHIPS         (38400)
#define ONE_SEC_IN_CX8             (30720000)
#define ONE_SEC_IN_CHIPS           (3840000)
#define ONE_SEC_IN_MS              (1000)
#define ONE_SEC_IN_SC              (32768)
#define SLEEP_CLK_NOMINAL_mHZ      (32765442)
#define MIN_SLEEP_XTAL_MILLI_HZ    (30000000)
#define NOMINAL_FEE_NUM_SLEEP_CLKS (10000)

#define ONE_MS_IN_SC   (1 + SLEEP_CLK_NOMINAL_mHZ/1000000)
#define FIVE_MS_IN_SC  (1 + 5 * SLEEP_CLK_NOMINAL_mHZ/1000000)
#define TEN_MS_IN_SC   (1 + 10 * SLEEP_CLK_NOMINAL_mHZ/1000000)
#define M_BY_N_MS_IN_SC(m,n)  (1 + ((m) * (SLEEP_CLK_NOMINAL_mHZ/10000)/((n) * 100)))

#define WCDMA_QFT_INT_LENGTH_LONG               (30)
#define WCDMA_QFT_INT_LENGTH_SHORT              (15)

#define WCDMA_QFT_THRESHOLD_LONG                (2500000)
#define WCDMA_QFT_THRESHOLD_SHORT               (625000)

#define WCDMA_QTT_COH_INT_LENGTH_STTD_LONG      (4)
#define WCDMA_QTT_COH_INT_LENGTH_LONG           (6)

#define WCDMA_QTT_COH_INT_LENGTH_STTD_SHORT     (4)
#define WCDMA_QTT_COH_INT_LENGTH_SHORT          (5)

#define WCDMA_QTT_NON_COH_INT_LENGTH_STTD_LONG  (7)
#define WCDMA_QTT_NON_COH_INT_LENGTH_LONG       (5)

#define WCDMA_QTT_NON_COH_INT_LENGTH_STTD_SHORT (3)
#define WCDMA_QTT_NON_COH_INT_LENGTH_SHORT      (3)


#ifdef FEATURE_DUAL_SIM
/* Wakeupmgr cancellation algorithm support primitives */

#define DRX_ENABLE_WKUPMGR_CANCEL_SHFT 7
#define DRX_ENABLE_WKUPMGR_CANCEL_OPT_BMSK 0x80

#define DRX_WKUPMGR_CANCEL_FROM_SLEEP_START_INT_SHFT 0
#define DRX_WKUPMGR_CANCEL_FROM_SLEEP_START_INT_BMSK 0x01

#define DRX_WKUPMGR_CANCEL_IN_PROGRESS_SHFT 1
#define DRX_WKUPMGR_CANCEL_IN_PROGRESS_BMSK 0x02

#define DRX_WKUPMGR_SLEEP_START_INT_IN_PROGRESS_SHFT 2
#define DRX_WKUPMGR_SLEEP_START_INT_IN_PROGRESS_BMSK 0x04

#define DRX_DISABLE_WKUPMGR_CANCEL_SUPPORT() (drx_wkupmgr_cancel_bmsk &= (~(1<<DRX_ENABLE_WKUPMGR_CANCEL_SHFT)))
#define DRX_ENABLE_WKUPMGR_CANCEL_SUPPORT() (drx_wkupmgr_cancel_bmsk |= (1<<DRX_ENABLE_WKUPMGR_CANCEL_SHFT))

#define DRX_DISABLE_WKUPMGR_CANCEL_FROM_SLEEP_START_INT() (drx_wkupmgr_cancel_bmsk &= (~(1<<DRX_WKUPMGR_CANCEL_FROM_SLEEP_START_INT_SHFT)))
#define DRX_ENABLE_WKUPMGR_CANCEL_FROM_SLEEP_START_INT() (drx_wkupmgr_cancel_bmsk |= (1<<DRX_WKUPMGR_CANCEL_FROM_SLEEP_START_INT_SHFT))

#define DRX_DISABLE_WKUPMGR_CANCEL_IN_PROGRESS() (drx_wkupmgr_cancel_bmsk &= (~(1<<DRX_WKUPMGR_CANCEL_IN_PROGRESS_SHFT)))
#define DRX_ENABLE_WKUPMGR_CANCEL_IN_PROGRESS() (drx_wkupmgr_cancel_bmsk |= (1<<DRX_WKUPMGR_CANCEL_IN_PROGRESS_SHFT))

#define DRX_DISABLE_WKUPMGR_SLEEP_START_INT_IN_PROGRESS() (drx_wkupmgr_cancel_bmsk &= (~(1<<DRX_WKUPMGR_SLEEP_START_INT_IN_PROGRESS_SHFT)))
#define DRX_ENABLE_WKUPMGR_SLEEP_START_INT_IN_PROGRESS() (drx_wkupmgr_cancel_bmsk |= (1<<DRX_WKUPMGR_SLEEP_START_INT_IN_PROGRESS_SHFT))

#define DRX_CHK_WKUPMGR_CANCEL_IN_PROGRESS() (((drx_wkupmgr_cancel_bmsk>>DRX_WKUPMGR_CANCEL_IN_PROGRESS_SHFT) & 0x1 ) == 1)
#define DRX_CHK_WKUPMGR_SLEEP_START_INT_IN_PROGRESS() (((drx_wkupmgr_cancel_bmsk>>DRX_WKUPMGR_SLEEP_START_INT_IN_PROGRESS_SHFT) & 0x1 ) == 1)
#define DRX_CHK_WKUPMGR_CANCEL_SUPPORTED() (((drx_wkupmgr_cancel_bmsk>>DRX_ENABLE_WKUPMGR_CANCEL_SHFT) & 0x1 ) == 1)

extern uint8 drx_wkupmgr_cancel_bmsk;

#endif /* FEATURE_DUAL_SIM */



#define WL1SLEEP_FW_AWAKE()              (wl1_fw_status == WSLEEP_FW_AWAKE)
#define WSLEEP_FW_SLEEP_IN_PROG()        (wl1_fw_status == WSLEEP_FW_SLEEP_IN_PROGRESS)
#define WL1SLEEP_FW_ASLEEP()             (wl1_fw_status == WSLEEP_FW_ASLEEP)
#define WSLEEP_FW_STATUS()               (wl1_fw_status)

/* SIG used to wait for FW sleep done cmd */
#define WSLEEP_FW_SLEEP_DONE_SIG DRX_PRL_RQ_DL_WRKLP_DONE_SIG


/* ==========================================================================
**
** Macros
**
** ========================================================================*/

/*===========================================================================

MACRO SLEEP_DRV_PROGRAM_SLEEP_INTERVAL, and other macros

DESCRIPTION
    This macro programs SLEEP_INTERVAL register with sleep time specified in
    number of sleep cycles

PARAMETERS
    sleep_interval - Sleep duration specified in number of sleep clock cycles
    Sleep clock frequency is about 32 KHz

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
#define SLEEP_DRV_SET_ST_EVENT_STATUS_DUMP() \
    WL1_HWIO_OUT(STMR_STATUS_DUMP_CMD_MICRO, STMR_STATUS_DUMP_ST_EVENT_V)


/* ==========================================================================
**
** Type Definitions
**
** ========================================================================*/

typedef void (*isr_type)(void);

/* Definition of the states of the Sleep module state machine */
typedef enum
{
  /* Sleep module inactive; go_to_sleep() hasn't been called yet */
  SLEEP_INACTIVE,

  /* Sleep module is active; go_to_sleep has been called but RF isn't to be
   * put to sleep
   */
  SLEEP_MICRO_UP,

  /* RF is in sleep but micro is powered up (no consensus to power down) */
  SLEEP_RF_IN_SLEEP_MICRO_UP,

  /* RF in warm up (micro is up) */
  SLEEP_RF_IN_WARM_UP,

  /* In deep sleep.  RF sleeping.  CPU (and TCXO) are currently up, as required
   * by other subsystem(s).
   */
  SLEEP_RF_IN_SLEEP_MICRO_UP_IN_DEEP_SLEEP

} sleepModuleState_type;


/* Sleep duration in milliseconds */
typedef uint32 sleepDurationMillisec_type;

/* Call back function to wake up the protocol stack */
typedef void (*wakeUpCallBackFunction_type)(void);

#ifdef DEBUG_WSLEEP_PROFILE_TIME_LINE
#error code not present
#endif

/*
 * Type definition for Sleep Contoller Interrupt.
 */
typedef enum
{
  SLEEP_CTLR_INT_NONE,
  SLEEP_CTLR_BEF_SLEEP_START,
  SLEEP_CTLR_SLEEP_START_INT,
  SLEEP_CTLR_MICRO_WAKE_UP_INT,
  SLEEP_CTLR_WU_1_ACT_TIME_0_INT
} sleep_ctlr_int_type;

typedef enum
{
  SLEEP_CTLR_INT_STATUS_ONLINE = 0x01,
  SLEEP_CTLR_INT_STATUS_SLEEP  = 0x02,
  SLEEP_CTLR_INT_STATUS_WARMUP = 0x04
} sleep_ctlr_int_status_type;

/* new state machine to track the FW awake status */
typedef enum
{
  WSLEEP_FW_AWAKE,
  WSLEEP_FW_SLEEP_IN_PROGRESS,
  WSLEEP_FW_ASLEEP
}wsleep_fw_status_enum_type;

wsleep_fw_status_enum_type wl1_fw_status;

/* ==========================================================================
**
** Global Variable Definitions
**
** ========================================================================*/

/*
 * Whether ok to turn off tcxo from sleep driver's point of view
 */
extern boolean wsleep_oktcxo;

extern sleep_ctlr_int_type last_int_from_sleep_ctlr;

/* number of frames for which we haven't calibrated the sleep clk */
extern uint32 num_frames_sleep_clk_uncalibrated;

#ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
extern boolean wtog_bplmn_in_progress;
#endif

#ifdef FEATURE_DUAL_SIM
extern boolean wtow_bplmn_in_progress;
extern uint16 drx_num_cycles_after_mode_switch;
#endif

#ifdef FEATURE_MAINTAIN_GTIME_THROUGH_WSLEEP
extern boolean gsm_clk_enabled;
#endif

extern volatile boolean sleep_drv_ols_has_occured;

extern boolean sleep_drv_multiple_int;

/*
 * Value written into the SLEEP_INTERVAL register
 */
extern uint32 sleep_interval_written_sc;

extern boolean parallel_reacq_done_this_cycle;

/*
 * Indicates whether in good or bad DL conditions (vs CPICH EcIo -8 db)
 * so that we can sleep longer
 */
extern boolean short_qxt;

/*
 * Whether sleep was initiated with RF off or on
 */
extern boolean went_to_sleep_without_rf;

extern uint32 num_sleep_ctlr_cycles;

extern boolean wsleep_wakeup_srch_pending;
extern boolean sleep_drv_force_abort;

extern uint32 wl1sleep_rf_timetaken_prev;

#ifdef FEATURE_DUAL_SIM
extern boolean sleep_drv_wkup_cancel_pending;
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_WCDMA_REACQLESS_PICH
/*Saves the target ticks to PI in the current DRX cycle when RLP is enabled.*/
extern uint32 wsleep_target_ticks_rem;
#endif
/* ==========================================================================
**
** Function declarations
**
** ========================================================================*/

extern uint32 sleep_drv_mult_div(uint32 a, uint32 b, uint32 c);
extern uint32 diff_sfn_sfn(uint32 sfn, uint32 comb_cnt_cx8,
                           uint32 sfn2, uint32 comb_cnt_cx8_2);
/*===========================================================================
FUNCTION     wsleep_exit_mode_op

DESCRIPTION 
   Checks a couple of conditions and calls exit mode on the PRx
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
extern void wsleep_exit_mode_op(void);
/*===========================================================================

FUNCTION     WSLEEP_INIT

DESCRIPTION This function is called to initialize the WCDMA Sleep module,
  and to register a call back function for wake up.
  See further description in the sleep.c file.

DEPENDENCIES
  None

RETURN VALUE
  True if all went well; False otherwise.

SIDE EFFECTS

===========================================================================*/
extern boolean wsleep_init( wakeUpCallBackFunction_type wakeUpCb );

/*===========================================================================

FUNCTION     WSLEEP_ADJ_SLEEP_CLK_FREQ

DESCRIPTION This function conditionally calls a sleep driver function to trigger
computation of sleep clk frequency.

DEPENDENCIES
  None

RETURN VALUE
   None
SIDE EFFECTS

===========================================================================*/
extern void wsleep_adj_sleep_clk_freq(void);

/*===========================================================================

FUNCTION     WSLEEP_FEE_IS_NEEDED

DESCRIPTION This function returns whether FEE is needed.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if FEE is required. FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
extern boolean wsleep_fee_is_needed(void);

/*===========================================================================

FUNCTION     WSLEEP_FEE_IS_POSSIBLE

DESCRIPTION This function returns whether FEE will complete within the next sleep
            of duration avail_msec.

DEPENDENCIES
  None

PARAMETERS
  avail_msec - Duration of the next sleep period in msec.

RETURN VALUE
  TRUE if FEE is possible during this sleep period. FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
extern boolean wsleep_fee_is_possible(uint16 avail_msec);

/*===========================================================================

FUNCTION     GO_TO_SLEEP

DESCRIPTION This function is called to initiate the the sleep
  process for a specified period (in milliseconds).

DEPENDENCIES
  None

PARAMETERS
  wakeUpWithinMs   - Duration of the sleep period.
  handle_waking_up - Entity for which DRX is going to sleep.
  sleep_ta_mode    - G2W TA mode  

RETURN VALUE
  TRUE if all went well; FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
extern boolean go_to_sleep( sleepDurationMillisec_type  wakeUpWithinMs,
                            drx_entity_handle_type   handle_waking_up
                            #ifdef FEATURE_DUAL_SIM
                            , boolean                     sleep_ta_mode
                            #endif
                          );

/*===========================================================================

FUNCTION     GO_TO_SLEEP_WITHOUT_RF

DESCRIPTION This function is called to initiate the the sleep
  process for a specified period (in milliseconds) without
  disabling clk regimes or rf.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if all went well; FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
extern boolean go_to_sleep_without_rf( sleepDurationMillisec_type wakeUpWithinMs );

/*===========================================================================

FUNCTION       SLEEP_ABORT

DESCRIPTION
  This function aborts a sleep in progress.
  See further description in the sleep.c file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sleep_abort( void );


/*===========================================================================

FUNCTION     EXIT_SLEEP_CYCLE

DESCRIPTION This function is called to exit sleep at the end of a drx cycle.
It clears some signals and cleans up the module and prints some stats.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void exit_sleep_cycle(void);



/*===========================================================================

FUNCTION     EXIT_SLEEP_MODE

DESCRIPTION This function is called to turn required clock regimes for demodulation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void exit_sleep_mode(void);



/*===========================================================================

FUNCTION     WSLEEP_DRX_RELEASED

DESCRIPTION This function is called when DRX mode is terminated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void wsleep_drx_released(void);

/*===========================================================================

FUNCTION     SLEEP_PROCESS_TIMER_SIG

DESCRIPTION This function is called to process the timer signal
received by the sleep task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void sleep_process_timer_sig(void);

/*===========================================================================

FUNCTION     SLEEP_INVERT/RESTORE_UART0_RX_DATA_INT_POL

DESCRIPTION These functions are called to reverse or restore the interrupt polarity
of the UART0_RX_DATA interrupt.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void sleep_invert_uart0_rx_data_int_pol(void);
extern void sleep_restore_uart0_rx_data_int_pol(void);

/*===========================================================================

FUNCTION     WSLEEP_READY_FOR_WAKE_UP

DESCRIPTION This function checks for conditions that need to be satisfied for
     reacquisition search to proceed (ref counter should be slammed at on-line-start).


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void wsleep_ready_for_wake_up(void);

#ifdef DEBUG_WSLEEP_PROFILE_TIME_LINE
#error code not present
#else
#define sleep_drv_profile_point(a) /* do nothing */
#endif


/*===========================================================================

FUNCTION SLEEP_DRV_SET_RF_WU_TIME_SC

DESCRIPTION
  This function sets expected RF Warmup Time In Sleep Clocks.
  This value is changed only if the update is less than known estimate.

DEPENDENCIES
    None

PARAMETERS
    None

RETURN VALUE
    None

SIDE EFFECTS
    The variable sleep_drv_rf_wu_time_sc is initialized to 2ms.
    So whenever this function is called, the RF WU value can only be
    adjusted if the estimate is less than 2ms. If more than 2ms, this
    update is ignored

===========================================================================*/
extern void sleep_drv_set_rf_wu_time_sc(uint32 new_rf_wu_sc);

/*===========================================================================

FUNCTION SLEEP_DRV_INIT, SLEEP_DRV_SETUP

DESCRIPTION
    These functions initialize and set up the sleep controller for beginning a sleep cycle

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
extern void sleep_drv_init(void);

/*===========================================================================
FUNCTION     SLEEP_DRV_SETUP

DESCRIPTION
  This function programs the sleep controller for sleep at the beginning of
  a sleep cycle in DRX mode.  Generally we wake up to check PICH.  It is
  also possible that we will wake up to receive CTCH broadcast data.

DEPENDENCIES
  None.

PARAMETERS
  sleep_period_ms  - Duration of the sleep period (sometimes ignored and PICH
                     SFN used instead).
  wake_up_for_ctch - TRUE if we're waking up for CTCH.  FALSE for PICH.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void sleep_drv_setup
(
  uint32                         sleep_period_ms,
  boolean                        wake_up_for_ctch
);

/*===========================================================================

FUNCTION GET_NUM_SLEEP_CLKS_FOR_FEE

DESCRIPTION
    This function returns the number of sleep clks required to run SLEEP_FEE
    based on the length of the current DRX cycle.

DEPENDENCIES
    None

RETURN VALUE
    uint32 - Returns the number of sleep clks for SLEEP_FEE

SIDE EFFECTS
    None

===========================================================================*/
extern uint32 get_num_sleep_clks_for_fee(void);

/*===========================================================================

FUNCTION SLEEP_DRV_RUN_FEE

DESCRIPTION
    This function initiates an FEE (frequency error estimation) to estimate the
    sleep xtal frequency currently.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
extern void sleep_drv_run_fee(void);

/*===========================================================================
FUNCTION        SLEEPDRV_ADJ_SLEEP_CLK_FREQ

DESCRIPTION     This function implements the phase 2 sleep clk calibration algorithm.

DEPENDENCIES

SIDE EFFECTS    None
===========================================================================*/
extern void sleepdrv_adj_sleep_clk_freq(void);

/*===========================================================================

FUNCTION SLEEP_DRV_ABORT

DESCRIPTION
    This function initiates an abort of the sleep cycle

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
extern void sleep_drv_abort(void);

/*===========================================================================

FUNCTION IS_SLEEP_DRV_IN_SLEEP

DESCRIPTION
    This function returns TRUE or FALSE depending on the last interrupt 
    received from the sleep controller. Sleep driver is supposed to be
    awake if the last interrupt is SLEEP_CTLR_INT_NONE

DEPENDENCIES
    None

RETURN VALUE
    TRUE : if last interrupt is SLEEP_CTLR_INT_NONE
    FALSE: all other interrupts other than SLEEP_CTLR_INT_NONE

SIDE EFFECTS
    None

===========================================================================*/
extern boolean is_sleep_drv_not_in_sleep(void);

#if defined(FEATURE_MMGPS) ||        defined(FEATURE_CGPS_TIME_TAGGING)
typedef struct
{
  sleep_ctlr_int_type last_int_from_sleep_ctlr;

  uint32 coarse_time;

  uint32 fine_time;

} wl1_slp_counters_type;

typedef struct
{
  uint16 freq;
  uint16 psc;
  
  wl1_cell_time_type  cell_time;

  uint8  is_valid_flag;

} wl1_cell_time_info_type;

/*===========================================================================
FUNCTION        GET_CURRENT_CELL_TIME_VALUES

DESCRIPTION     Returns the cell time (wl1_cell_time_type).

DEPENDENCIES    None

SIDE EFFECTS    None
===========================================================================*/
extern void get_current_cell_time_values (wl1_slp_counters_type  *wl1_cell_time_values);

/*===========================================================================
FUNCTION        COMPUTE_CURRENT_CELL_TIME

DESCRIPTION     Returns the cell time (wl1_cell_time_info_type).

DEPENDENCIES    None

RETURN VALUE:   Returns whether a sleep timetag was performed.
                TRUE:  Sleep Timetag
                FALSE: Not a Sleep Timetag

SIDE EFFECTS    None
===========================================================================*/
extern boolean compute_current_cell_time(wl1_cell_time_info_type *info,
                               const wl1_slp_counters_type *cell_time_values, boolean allow_sleep_timetag);
#endif /* FEATURE_MMGPS || FEATURE_MMGPS_WL1 || FEATURE_CGPS_TIME_TAGGING */


/*--------------------------------------------------------------------------
 * The following macros program WCDMA Modem Sleep Controller registers
 * Modem Sleep Controller is used in MSM 6260 and above
 *--------------------------------------------------------------------------*/

/* Function prototypes */
extern uint32 sleepdrv_get_coarse_time(void);
extern uint16 sleepdrv_get_fine_time(void);
extern uint16 sleepdrv_get_sleep_timer_1_count(void);
extern uint32 sleepdrv_get_programmed_wakeup_time(void);
extern boolean sleepdrv_set_sleep_timer_1_count(uint32 sclks);
extern uint32 sleepdrv_get_programmed_coarse_time( void);
extern sleep_ctlr_int_status_type sleepdrv_get_sleep_int_status(void);

extern void wl1_sleep_wkup_cb(void);


#define SLEEP_DRV_INSTALL_SLEEP_CTL_ISR()                        \
        {                                                        \
          slpc_set_wakeup_callback(SLPC_WCDMA,wl1_sleep_wkup_cb); \
        }


#define SLEEP_DRV_UN_INSTALL_SLEEP_CTL_ISR()                    \
        {                                                       \
          slpc_set_wakeup_callback(SLPC_WCDMA,  NULL ); \
        }

/* The second variable to the functions sleepctl_set_warmup and sleepctl_set_warmup_no_wakeup
   is the wakeup time in ms. Convert tcxo_pll_wu_Cx8 to ms */
#define SLEEP_DRV_SET_SLP_CTL_SLEEP_INTERVAL()                                        \
      {                                                                             \
        sleepctl_set_warmup(SLEEPCTL_WCDMA,                                         \
                            (rf_wu_Cx8 * ONE_SEC_IN_MS)/ONE_MS_IN_CX8);  \
        sleepctl_set_duration_sys_clk(SLEEPCTL_WCDMA,                               \
                                      (sleep_interval_Cx8 + rf_wu_Cx8)); \
      }

/*===========================================================================
FUNCTION        GET_PICH_CC_CX8

DESCRIPTION     This function returns the combiner counter associated with
                the hardware channel carrying PICH.

DEPENDENCIES    returns the value from the last STMR status dump, NOT the
                dynamic value.

SIDE EFFECTS    None
===========================================================================*/
extern uint32 get_pich_cc_cx8(void);

/*===========================================================================
FUNCTION        SLEEP_DRV_EXIT

DESCRIPTION     This function resets various parameters to their original values.

DEPENDENCIES    None

SIDE EFFECTS    None
===========================================================================*/
extern void sleep_drv_exit(void);

/*===========================================================================
FUNCTION        SLEEP_DRV_QSYMS_UNTIL_GTIME_READY

DESCRIPTION     This function returns the number of quarter symbols until the
                GSM time will be set for the current DRX cycle. It rerurns 0 if
                the GSM time has already been set.

DEPENDENCIES    None

SIDE EFFECTS    None
===========================================================================*/
extern uint32 sleep_drv_qsyms_until_gtime_ready(void);

/*===========================================================================
FUNCTION        SLEEP_DRV_PREPARE_FOR_GTIME_SNAPSHOT

DESCRIPTION     This function sets up in preparation for saving GSM Time information.

DEPENDENCIES    None

SIDE EFFECTS    None
===========================================================================*/
extern void sleep_drv_prepare_for_gtime_snapshot(void);

/*===========================================================================
FUNCTION        SLEEP_DRV_GTIME_SNAPSHOT

DESCRIPTION     This function snapshots and saves the GSM time at the time of going to sleeep.

DEPENDENCIES    None

SIDE EFFECTS    None
===========================================================================*/
extern void sleep_drv_gtime_snapshot(void);

/*===========================================================================

FUNCTION SLEEP_DRV_STMR_STATUS_DUMP

DESCRIPTION
    This is a helper function that does a STMR status dump
    and some associated bookkeeping for GSM time maintenence.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
extern void sleep_drv_stmr_status_dump(void);

/*===========================================================================
FUNCTION        WRITE_SHORT/LONG_QXT_PARAMS

DESCRIPTION     These functions write required parameters for QXT

DEPENDENCIES    None

SIDE EFFECTS    None
===========================================================================*/
extern void write_long_qxt_params(void);
extern void write_short_qxt_params(void);

/*===========================================================================
FUNCTION        WSLEEP_SET_BIT/WSLEEP_CLEAR_BIT

DESCRIPTION     These functions set and clear bits of time test port

DEPENDENCIES    None

SIDE EFFECTS    None
===========================================================================*/
extern void wsleep_set_bit(uint32 bit_pos);
extern void wsleep_clear_bit(uint32 bit_pos);

/*===========================================================================

FUNCTION SLEEP_DRV_COMPUTE_GFN_gQSYM_AT_OLS

DESCRIPTION
    This function calculates the gfn and gqsym that should be configured at ols

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
extern void sleep_drv_compute_gfn_gqsym_at_ols(void);


/*===========================================================================
FUNCTION        SLEEP_DRV_ABORT_TIMEOUT_HANDLED

DESCRIPTION     If the action time and on-line-start interrupts haven't arrived,
                call them, and return TRUE; otherwise return FALSE.
                It is expected that this function be called only up on
                sleep abort processing delayed detection.

DEPENDENCIES    None

SIDE EFFECTS    None
===========================================================================*/
extern boolean sleep_drv_abort_timeout_handled(void);

/*===========================================================================
FUNCTION        SLEEP_DRV_GET_SC_REM_TO_PICH

DESCRIPTION     Returns time in Sleep-Clocks from now to PICH
                This should be used before PICH in a DRX cycle

DEPENDENCIES    None

RETURN VALUE    int32

SIDE EFFECTS    None
===========================================================================*/
extern int32 sleep_drv_get_sc_rem_to_pich(void);

/*===========================================================================
FUNCTION        SLEEP_DRV_GET_SC_SINCE_PICH

DESCRIPTION     Returns time in Sleep-Clocks since PICH till now
                This should be used after PICH in a DRX cycle

DEPENDENCIES    None

RETURN VALUE    int32

SIDE EFFECTS    None
===========================================================================*/
extern int32 sleep_drv_get_sc_since_pich(void);

/*===========================================================================
FUNCTION        SLEEP_DRV_CACHE_SC_AT_PICH

DESCRIPTION     Caches the sc at pich so the sleep_drv_get_sc_since_pich uses current
                cycle. This should be called after PICH and before sc for
                next PICH is calculated

DEPENDENCIES    None

RETURN VALUE    void

SIDE EFFECTS    None
===========================================================================*/
extern void sleep_drv_cache_sc_at_pich(void);

/*===========================================================================
FUNCTION        SLEEP_DRV_GET_SC_REM_TO_PICH_FOR_F3

DESCRIPTION     Returns time in Sleep-Clocks from now to PICH
                This should be used before PICH in a DRX cycle
                Will return 0 if tcxo_was_shut_down to avoid overhead

DEPENDENCIES    sleep_drv_get_sc_rem_to_pich

RETURN VALUE    int32

SIDE EFFECTS    None
===========================================================================*/
extern int32 sleep_drv_get_sc_rem_to_pich_for_f3(void);

/*===========================================================================
FUNCTION        SLEEP_DRV_GET_SC_SINCE_PICH_FOR_F3

DESCRIPTION     Returns time in Sleep-Clocks since PICH till now
                This should be used after PICH in a DRX cycle
                Will return 0 if tcxo_was_shut_down to avoid overhead

DEPENDENCIES    sleep_drv_get_sc_since_pich and sleep_drv_cache_sc_at_pich

RETURN VALUE    int32

SIDE EFFECTS    None
===========================================================================*/
extern int32 sleep_drv_get_sc_since_pich_for_f3(void);

/*===========================================================================
FUNCTION WSLEEP_READY_FOR_STMR_WALL_TIME_SETTLING

DESCRIPTION
  Callback registered with MCPM for setting STMR slam event.This is also called directly
  if it happens we are not sending WAKEUP_REQ to MCPM during wakeup.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
extern void wsleep_ready_for_stmr_wall_time_setting(void);

/*===========================================================================
FUNCTION     WSLEEP_SET_OPTIMIZED_TIMELINE

DESCRIPTION This function is the callback registered with MCPM to be called for 
notifying if WCDMA should use optimized/extended awake timeline. Accordingly,
the warmup time will be changed.

DEPENDENCIES
  MCPM.

RETURN VALUE
  None.  

SIDE EFFECTS
===========================================================================*/
extern void wsleep_set_optimized_timeline(boolean optimized_timeline);

/*===========================================================================
FUNCTION     WSLEEP_SET_WARMUP_TIME

DESCRIPTION This function is the callback registered with MCPM to be called for 
notifying if WCDMA should use optimized/extended awake timeline. Accordingly,
the warmup time will be changed.

DEPENDENCIES
  MCPM.

RETURN VALUE
  None.  

SIDE EFFECTS
===========================================================================*/
extern void wsleep_set_warmup_time(boolean rude_wakeup);

/*===========================================================================
FUNCTION     DRX_SLEEP_SETUP_OPTS

DESCRIPTION This function sets up the DRX optimizations.

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
===========================================================================*/
extern void drx_sleep_setup_opts(void);

extern void wsleep_set_cell_fing_decfg_status(boolean decfg_done);

extern boolean wsleep_get_cell_fing_decfg_status(void);

/*===========================================================================
FUNCTION       WSLEEP_STMR_INIT_AT_WAKEUP

DESCRIPTION     This function sets the STMR events and sets the isrs at 
                wakeup from sleep.

DEPENDENCIES    Called from MCPM STMR callback.

SIDE EFFECTS    None.
===========================================================================*/
extern void wsleep_stmr_init_at_wakeup(void);

/*===========================================================================
FUNCTION  WSLEEP_RFM_PREP_WAKEUP

DESCRIPTION
  This function pretunss to the required frequency and then calls RF API
  to build scripts through RFM PREP API, for the primary chain.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void wsleep_rfm_prep_wakeup(void);

/*===========================================================================
FUNCTION SLEEP_COMPUTE_NEW_SLP_SETUP_TIME

DESCRIPTION
    Function used to compute sleep setup time in case of FULL DIVERSITY this cycle.
    Takes into account W2G client status to compute the new setup time.
    
DEPENDENCIES
    None
    
PARAMETERS
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
void sleep_compute_new_slp_setup_time(void);

/*===========================================================================

FUNCTION     SLEEP_RF_WARM_UP

DESCRIPTION This function is called to announce RF warmup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void sleep_rf_warm_up(void);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION SLEEP_DRV_CANCEL_WAKEUP

DESCRIPTION
    This function cancels the W wakeup and reprograms 
    sleep controller to the next PI occasion 
    
DEPENDENCIES
    None
    
RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
extern void sleep_drv_cancel_wakeup(void);
/*===========================================================================

FUNCTION WL1_DRX_PROCESS_WKUPMGR_CANCEL_CMD

DESCRIPTION
  Local cmd to offload wkupmgr cancallation processing to WL1 task from
  callback context.
    
DEPENDENCIES
    None
    
RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
extern void wl1_drx_process_wkupmgr_cancel_cmd(void);

/*===========================================================================
FUNCTION     DRX_PICH_OPTS_CHK

DESCRIPTION
  This function checks all the conditions for enabling pich opts.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  True - PICH opts can be enabled.
  False - Shouldnt be enabled.
===========================================================================*/
extern boolean drx_pich_opts_chk(void);

/*===========================================================================
FUNCTION     WSLEEP_FW_SLEEP_SUSPEND

DESCRIPTION This function puts FW to sleep and suspend and informs drxofflineprocmgr. 

DEPENDENCIES
  None

PARAMETERS
  

RETURN VALUE
  boolean.

SIDE EFFECTS

===========================================================================*/
extern void wsleep_fw_sleep_suspend(boolean cool_down_opts);
#endif /*FEATURE_DUAL_SIM*/

/*===========================================================================
FUNCTION  WSLEEP_COMPUTE_NEXT_RXD_MODE

DESCRIPTION
  Wrapper function that calls RxD API to compute diversity mode in the
  next drx cycle.
  Diversity mode can be No Diversity or Full diversity
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wsleep_compute_next_rxd_mode(void);

/*===========================================================================
FUNCTION  WSLEEP_POST_RFM_PREP_LOCAL_CMD

DESCRIPTION
  This function posts local cmd to do RFM PREP wakeup in L1 task context.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void wsleep_post_rfm_prep_local_cmd(void);

/*===========================================================================

FUNCTION     wsleep_early_rf_fw_sleep

DESCRIPTION 
  API used to trigger RF and FW sleep one after the other. This can be called 
  in DPC or L1 context

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsleep_early_rf_fw_sleep(uint32  shutdown_rf);

/*===========================================================================

FUNCTION     WSLEEP_FW_START

DESCRIPTION 
  API used to send start command to firmware.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wsleep_fw_start(void);

/*===========================================================================

FUNCTION     WSLEEP_WAIT_FOR_FW_SLP_DONE

DESCRIPTION 
  This API is used to wait for fw response to sleep cmd. Once FW response is
  received FW app is suspended.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wsleep_wait_for_fw_slp_done(void);

/*===========================================================================

FUNCTION     WSLEEP_SET_FW_SLEEP_STATUS

DESCRIPTION 
  API used to set the current FW state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsleep_set_fw_sleep_status(wsleep_fw_status_enum_type fw_status);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION     SET_XO_SLAM_FLAG

DESCRIPTION This function resets the WFW XO slam flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void set_xo_slam_flag(boolean val);

/*===========================================================================

FUNCTION     WL1_GET_CHANNEL_INFO

DESCRIPTION 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsleep_get_channel_info_for_trm_reserve_request(wrm_freq_input_type *band_res_req_info);

/*===========================================================================

FUNCTION     WSLEEP_CHK_RX_MODE_MODE_AT_NXT_WKUP

DESCRIPTION 
   This function determines whether next wakeup should be HYBRID mode or
   DR DSDS mode
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsleep_chk_rx_mode_mode_at_nxt_wkup(void);

#endif /* DUAL_SIM*/

/*===========================================================================
FUNCTION       wsleep_rfm_power_clock_on_at_wakeup

DESCRIPTION     This function calls rfm_power_clock_on() if not in DS mode.

DEPENDENCIES    Called from wakeup callback in L1 context after clocks are enabled.

SIDE EFFECTS    None.
===========================================================================*/
extern void wsleep_rfm_power_clock_on_at_wakeup(void);

/*===========================================================================
FUNCTION  WSLEEP_RFM_WAKEUP_WRAPPER

DESCRIPTION
  Wrapper function for waking up RF. This function also takes care of calling
  agc ttl init to apply the freq error before starting AGC. 
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wsleep_rfm_wakeup_wrapper(wl1_rfm_wakeup_chain_type chain, MDSP_WCDMA_L1_EVENT_CB_FUNC_TYPE *cb_func, boolean flag);

/*===========================================================================

FUNCTION     WL1_RFM_POWER_CLOCK_OFF

DESCRIPTION This function is the wrapper for rfm_power_clock_off() that 
            shuts off RF power and clocks. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_rfm_power_clock_off(uint32 unused);

/*===========================================================================

FUNCTION IS_SLEEP_DRV_IN_SLEEP

DESCRIPTION
    This function returns TRUE or FALSE depending on the last interrupt 
    received from the sleep controller. Sleep driver is supposed to be
    awake if the last interrupt is SLEEP_CTLR_INT_NONE
    
DEPENDENCIES
    None
    
RETURN VALUE
    TRUE : if last interrupt is SLEEP_CTLR_INT_NONE
    FALSE: all other interrupts other than SLEEP_CTLR_INT_NONE

SIDE EFFECTS
    None

===========================================================================*/
extern boolean is_sleep_drv_not_in_sleep(void);

/*===========================================================================
FUNCTION     wsleep_get_Wclk_status

DESCRIPTION 
   Checks if WSTACK & CLOCKs are up
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE if W stack is up & FALSE otherwise

SIDE EFFECTS
  none
===========================================================================*/

extern boolean wsleep_get_Wclk_status(void);
#ifdef FEATURE_QSH_DUMP
extern void wl1_sleep_params_get_debug_info(wl1_drx_sleep_mini_debug_info_struct_type* obj);
#endif

#endif /* WL1SLEEP_H */

