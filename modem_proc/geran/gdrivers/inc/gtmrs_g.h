#ifndef GTMRS_G_H
#define GTMRS_G_H
/*===========================================================================

                 G S M   T I M E R S  H E A D E R    F I L E

DESCRIPTION
  This module contains declarations and definitions to interface
  with the GSM timers module.

Copyright (c) 2001-2015 Qualcomm Technologies, Inc.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gtmrs_g.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   ---       --------------------------------------------------------
25/05/16    rv       CR998395: Excessive F3 logging in  GERAN L1
09/10/15   akh       CR912421 FN/QS not correct for active ttg, sleep ttg resp delayed by 1s
05/06/15   zf/cs     CR808438: Correction on GPS timetag reporting in wakeup
01/04/15   df        CR798078 Sleep between QTA gaps
12/12/14   sk        CR763710 Ensure NBCCH is read when consistently colliding with serving PCH or other sub's PCH
04/11/14   ab        CR646584: GSM Timeline reporting for WLAN COEX
17/10/14   ak        CR730234 Sleep debug code tidy up
04/09/14   zf        CR702512: Correction in detecting GSTMR interrupt pending
06/08/14   df        CR536059 Allow Asynchronous sleep for all idle activities
08/05/14   hd        CR660875: Defined gstmr_init_odd_even_fnmod4()
18/07/14   zf        CR696301: Do not wait for RF wakeup CNF if RF has not to been initialized
11/06/14   npt       CR630396 Wake-up error recovery mechanism
30/05/14   pg        CR655759 Feature cleanup of FEATURE_BOLT_MODEM and FEATURE_DIME_MODEM
12/05/14   ssh       CR661514 Additional protection around panic reset handling [workaround for buffer overflow]
07/04/14   cja       CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
04/04/14   cah       CR643764 VSTMR multi-sim restruct and FEATURE_MSIM_VSTMR removal
19/03/14   cja       CR632037 Use RF Task for X2G
06/03/14   ka        CR623971: Macro IS_FRAME_NUM_LATER missing parenthesis causing havoc/no TBF suspension
03/03/14   npt       CR622060 - Rework wake-up
18/02/14   ssh       CR616652: Use both gstmr_is_int_pending and mod4 FN to check for pending GSTMR interrupt.
13/02/14   ka        CR614268: Add debug for BPLMN search not resuming
12/02/14   cja       CR610689 Add API gstmr_rd_hw_fn_qs (synchronised FN and qs)
30/01/14   cja       CR608949 When register gstmr, set FN based on fn_modMaxFN when deregistered.
24/01/14   cah       CR598552: enable sleep for DSDS GSTMR+VSTMR
20/01/14   cos       CR604531 Synchronize interrupt deregistration and ISR handling:
                 GERAN updates for treating VSTMR deregister based on the current task context
13/01/14   cja       CR599086 For all X/G transitions sleep and wake-up gstmr
29/11/13   sp        CR580821: Add F3s to track CM mode configurations.
16/01/13   sp        CR600563: Handle ISR Overrun from Idle State machines instead of Warmup ISR
15/01/14   ssh       CR600467:GL1 to implement a GTS recovery mechanism if there is a ISR overrun
09/01/14   cah       CR598075  PCH decode failures observed on sub 1 after voice call on sub 2
17/12/13   cja       CR590767 Bolt G2W - add gap start time to g2w startup
16/12/13   xz        CR580972 when handler is full, and fail to register, Extend buffer length.
26/11/13   npt/ip    CR582180 Enable sleep on Bolt
19/11/13   ssh/ip    CR547956 DSDS/DSDA GL1 timeline Opt (parallelize the RF config. with GL1 warm-up)
11/14/13   ab        CR 554820 DSDA GL1 - Update Tx power as per slot transmission for RF COEX
18/10/13   npt       CR542638 - Changes to support sleep on Bolt
20/08/13   npt       CR530340 - Added VSTMR VFR handling
14/08/13   npt       CR528491 - Use mod4096 instead of mode4 for interactions with FW
15/07/13   sp        CR508123: Update panic reset to use multi-sim data space
15/05/13   cs        TSTS Updates
13/05/13   jk        CR463435:Using DAL interface to calculate GSM sleep time tag
22/02/13   pg        CR455347: Expose gstmr_wakeup_init as external API
07/02/13   pg        CR449164: include TT adjustment in sleep calculation
06/08/13   pg        CR449662: add GSTMR start and end timestamps
30/01/13   jk        CR434373 - Reporting GSM sleep time tag to GPS
25/01/13   pg        CR444247: GSTMR compatability changes for Dime V1 & V2 HW
06/12/12   npt       CR346414 - Remove first GSTMR after wake-up
14/11/12   npt       CR420954 - Added changes for power collapse
07/11/12   dv        CR418770 T2G: Race condition between GL1 issuing async int
                             from gstmr_isr & GFW clearing buffer
30/05/12   pg        Add access function for reading mod4 HW frame number
29/03/12   cja       CR341225 RxFE recovery for X2G
22/11/11   jj        CR319573 incorporate with review comments
16/11/11   jj        CR319573   changes for  profiling the modem performance
03/11/11   og        CR316831. Remove customer's compiler/linker errors.
12/11/10   ws        Renamed MICRO_GSM_TIME_RD to GSM_TIME_RD for NikeL modem
10/05/11   ky        Enhancing the handler logging for rf tx function
09/03/11   ky        When ISR over run happens, if ISR delay is more than 625qs, we go through
                     the trace buffer and find the task which is delaying the ISR and print the
                     task name and amount of duration it has run.
09/12/10   cs        CR264682 Add support for asynch dsp completion interrupt
12/11/10   ws        Renamed MICRO_GSM_TIME_RD to GSM_TIME_RD for NikeL modem
18/11/10   ky        Added pcycle count into the isr performance logging handler once
                     per frame.
09/11/10   ky        Removed the FEATURE_GSM_ISR_HANDLER_PERFORMACE_LOGGING from the code
                     so that this logging always enabled by default. Isr usage calculation
                     done by using the sysclks instead of qs count
12/10/10   ky        Added qsym count to mdsp cmd trace buffer
08/10/10   og        Fixes to CR217386 and CR182441.
02/09/10   ky        Added quarter symbol count value to the gl1 handlers logging
                     and replaced ERR FATAL with warning when MDSP DEBUG ENHANCEMENTS
                     FEATURE is not enabled. Moved GL1 Handler logging under
                     FEATURE_GSM_ISR_HANDLER_PERFORMACE_LOGGING. if NV item 1027 is invalid
                     then gl1 crashes for any subset of errors reported by mdsp.
                     Crashing fw when seq num mismatch is moved out of MDSP DEBUG ENHAN
                     CEMENT FEATURE
02/09/10   cja       Fix compilation warnings
29/06/10   ky        Added the performace logging for gstmr handlers and rf api's
15/04/09   agv       Mustang API gstmr_qsym_at_int_start to simulate interrupt
                     pending check.
04/28/06   gfr       Added gstmr_disable_interrupt, gstmr_enable_interrupt,
                     and gstmr_clear_interrupt
01/13/05   gfr       Added gstmr_enable_odd_even_check() to replace global
                     variable
10/17/05   gfr       Add support for GSTMR tracing
10/12/05   gfr       Make reg/dereg of handlers while in the ISR have
                     deterministic behavior
07/27/05   gw        Added prototypes for gstmr_start_sleep() and
                     gstmr_start_sleep_was_ok().
03/16/05   gfr       Added function prototype for gstmr_force_reinit.
11/23/04   gfr       Added support for a gstmr adjust debug trace buffer.
06/08/04   gfr       Added FNmo13 for GPRS.
03/26/04   gfr       Added function prototype.
01/16/04   bm        Added enable, disable macros for gstmr_odd_even_check_disable
11/05/03   gw        Added prototypes for gstmr_set_terminal_count_symbol()
                     and gstmr_is_int_pending().
08/11/03   jc        Add function prototype.
10/01/03   asr       Added function prototype for GSTMR_SET_FN()
08/11/03   jc        Add function prototype.
04/11/03   ATM       Moved Frame Number management entirely inside GTMRS
12/04/02   ATM       Add new constants used for CompressedMode handover calcs
06/28/02   jc        Add new function, correct deregister_handler.
06/10/02   jc        Add support for deregister_handler.
04/30/02   mk        Added gstmr_rd_symbol_count(), gstmr_rd_odd_even().
                     Fixed prototype for gstmr_adjust_terminal_count().
02/27/02   JC        Renamed gstmr_reset() to gstmr_gsm_core_reset().
01/16/02   mk        Added support for afc/tt.
04/03/01   JC        Added this header block and some prototypes

===========================================================================*/

#include "geran_variation.h"

#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "timetick.h"

#ifdef FEATURE_GERAN_VSTMR
#include "vstmr_geran.h"
#endif

#include "geran_multi_sim.h"
#include "gtmrs.h"
#include "l1_log.h"         // This module's header file
#include "log.h"            // For log_commit() and log_alloc() and log codes

#include "log_codes_gsm.h"
#include "log_codes_gsm_v.h"  // For GSM log codes

/* Constants used for coordinating WCDMA to GSM stack changes */
#define GSTMR_GSM_TICK_DELAY       0

/* DSDA type to define GTMR hardware
 * GSTMR_0 = legacy
 * GSTMR_1 = DSDA G1 GSTMR
 */
typedef enum
{
  GSTMR_NONE = -1,
  GSTMR_0 = 0,
  GSTMR_1 = 1
#if defined ( FEATURE_TRIPLE_SIM )
  , GSTMR_2 = 2
#endif /* FEATURE_TRIPLE_SIM */
}gtmrs_hw_t;

typedef enum {
   CLEAR_COMMIT = 0,
   TX_BURST_COMMIT = 1,
   RX_BURST_COMMIT = 2,
   RF_WAKEUP_COMMIT = 4,
   WAKEUP_GSTMR_ISR_COMMIT = 8,
   WAKEUP_COMMIT_ALL = 255
} gl1_hw_sleep_commit_T;

extern boolean gl1_hw_is_forced_wakeup(gas_id_t gas_id);

#define NO_OF_FRAMES_DELAY_FOR_PANIC_RESET_TRIGGER 3

#define DEF_SYMB_TERMINAL_COUNT     1249 /* 1250 symbols (0..1249) per frame */
#define DEF_QSYMB_TERMINAL_COUNT    3
#define DEF_PRESCALE_TERMINAL_COUNT 10   /* incremented by gsm_clk */
#define DEF_RAW_13MHZ_TERMINAL_COUNT (NUM_GSM_CLKS_PER_FRAME - 1)    // 59999
#define DEF_QSYMB_TIME_TRACKING_ADJ   16

extern void gstmr_init( gas_id_t gas_id );
extern void gstmr_force_reinit (gas_id_t gas_id);

extern void gl1_hw_sleep_set_commit_check( gl1_hw_sleep_commit_T gl1_hw_sleep_commit_check, gas_id_t gas_id );
extern void gl1_hw_sleep_set_commit_mask( gl1_hw_sleep_commit_T gl1_hw_sleep_commit_check, gas_id_t gas_id );
extern void gl1_hw_sleep_commit( gas_id_t gas_id );

extern boolean gl1_hw_sleep_get_in_warmup_check( gas_id_t gas_id );

extern uint32 gl1_hw_sleep_get_sys_clk_to_ols( gas_id_t gas_id );
extern uint32 gl1_hw_sleep_get_qs_to_ols( gas_id_t gas_id );

extern void gl1_hw_rf_result_monitor_timer_init(gas_id_t gas_id );
extern rex_timer_type   RF_cmd_RESULT_counter[];
extern void gl1_hw_rf_result_monitor_timer_deactivate(gas_id_t gas_id);

#ifndef FEATURE_GERAN_VSTMR
extern void gstmr_delay_interrupt( uint16   delay_qs,gas_id_t gas_id );
#endif
extern uint32  gstmr_start_sleep( uint32 delay,gas_id_t gas_id );
extern boolean gstmr_start_sleep_was_ok( gas_id_t gas_id );

extern uint32 gstmr_rd_symbol_count( gas_id_t gas_id  );
extern uint32 gstmr_rd_frame_time_count( gas_id_t gas_id  );

extern uint32 gstmr_rd_odd_even( gas_id_t gas_id );

extern uint32 gstmr_rd_mod4_frame_num( gas_id_t gas_id  );

extern uint32 gstmr_qs_in_next_frame_to_xo(uint16 qs_offset,gas_id_t gas_id);
extern gsmtr_fn_qs_type gstmr_rd_hw_fn_qs_geran(gas_id_t gas_id);

extern boolean gstmr_is_within_delayed_window (gas_id_t gas_id);
extern uint32 gstmr_qsym_at_int_start (gas_id_t gas_id);

/* GSM TDMA frame number -- access functions */
extern void   GSTMR_REWIND_FN(  uint32 fn_lag, gas_id_t gas_id );

extern void GSTMR_INC_FN_BY_ONE( gas_id_t gas_id );
extern void GSTMR_DEC_FN_BY_ONE( gas_id_t gas_id );

extern void    gstmr_set_terminal_count_symbol( uint32 symbol_count ,gas_id_t gas_id);
extern boolean gstmr_is_int_pending( gas_id_t gas_id );

extern uint32 calc_garb_gstmr_tick( uint32 *ustmr, gas_id_t gas_id );

#ifndef FEATURE_GERAN_VSTMR
extern uint32 geran_rd_hw_frame_count_gstmr(gas_id_t gas_id);
#endif

extern void    gstmr_disable_interrupt( gtmrs_hw_t gtmrs_hw );

extern void    gstmr_enable_interrupt( gtmrs_hw_t gtmrs_hw );

extern void    gstmr_clear_interrupt( gtmrs_hw_t gtmrs_hw );

extern void    gstmr_asynch_dsp_int( gas_id_t gas_id );
extern boolean gstmr_get_asynch_int_executing( gas_id_t gas_id  );
extern void    gstmr_set_asynch_int_executing( boolean is_asynch_int_running,gas_id_t gas_id );
extern void    gstmr_asynch_silent_frame_isr( gas_id_t gas_id );
#ifndef FEATURE_GERAN_VSTMR
extern void    gstmr_restore_delay_interrupt( gas_id_t gas_id);
#endif
extern void    wakeup_gstmr_isr( gas_id_t gas_id );
extern boolean wakeup_check_isr_overrun(gas_id_t gas_id);
#ifndef FEATURE_GERAN_VSTMR
extern void gl1_hw_sleep_set_gts_gstmr_fired(gas_id_t gas_id);
#endif /*FEATURE_GERAN_VSTMR*/

void gl1_ustmr_gstmr_snapshot_before_sleep(gas_id_t gas_id);
void gl1_get_gsm_time_tag( gas_id_t gas_id, uint32 *fn, uint32 *qsym);

#ifndef FEATURE_GERAN_VSTMR
int32 gstmr_get_tc_adjustment(gas_id_t gas_id);
#endif
void gstmr_adjust_via_online_start(uint32 gTCAdjust);

void gstmr_config_hw(gtmrs_hw_t gtmrs_hw,gas_id_t gas_id);

uint32 GSTMR_GET_FN_GERAN(gas_id_t gas_id);

void   GSTMR_SET_FN_GERAN( uint32 new_fn, gas_id_t gas_id  );

void   GSTMR_ADVANCE_FN_GERAN( uint32 fn_diff, gas_id_t gas_id );

void gstmr_wakeup_init_geran( sys_modem_as_id_e_type as_id );

extern gas_id_t gstmr_determine_gas_id( int gstmr_hw );

extern int gstmr_determine_hw_id( gas_id_t gas_id );

extern void gstmr_frame_tick_process( gas_id_t gas_id );

extern void gstmr_adjust_trace (int16 new_tc_adjust, int32 new_tc_set,gas_id_t gas_id);

#ifndef FEATURE_GERAN_VSTMR
extern void gstmr_adjust_terminal_count_geran(int16 quarter_symbol_count,sys_modem_as_id_e_type as_id);
#endif

extern boolean gstmr_ok_to_send_gstmr_steal_req(gas_id_t gas_id);

extern void gstmr_set_steal_notifcation_flag(gas_id_t gas_id);

extern void gstmr_clear_steal_notifcation_flag(gas_id_t gas_id);

boolean gstmr_is_tdma_handler_registered (gas_id_t gas_id);

extern void gstmr_init_odd_even_fnmod4(gas_id_t gas_id);

/* ---------------------------------------------------------------------- */
/*                Frame Number Management Variables                       */
/* ---------------------------------------------------------------------- */
/* see GSM 4.08, 10.5.2.30, Request Ref */

typedef struct
{

/* Supporting frame counters */
uint16                 FNmod42432;/* see GSM 4.08, 10.5.2.30, Request Ref */
uint16                 FNmod102;
uint16                 FNmod104;
uint16                 FNmod51;
uint16                 FNmod26;
uint16                 FNmod416;
uint16                 FNmod52;
uint16                 FNmod13;
uint16                 FNmod8; /* for SAR Reporting */
} supporting_frame_counters_t;

extern supporting_frame_counters_t frame_counters[];

/*===========================================================================

                         MACRO DECLARATIONS

===========================================================================*/
#define DEF_RAW_13MHZ_MAX_TC            ((1 << 16) - 2)              // 0xfffe (TIME_TC_VALUE must be programmed to TC-1)
#define DEF_USTMR_MASK                  ((1 << 27) - 1)              // 0x7ffffff
#define DEF_USTMR_OVER_GSTMR            0xBD0C                       // USTMR(19.2M)/GSTMR(13M) * 2^15
#define DEF_USTMR_TO_GSTMR_DELAY        3                            // 3 is sync delay from UNIV_STMR to G_STMR
#define DEF_ONLINE_START_MARGIN         535                          // conservative margin to ensure online start FT
                                                                     // occurs before FT due to frame rollover

#define DEF_ONLINE_START_TRIGGER_TIME   (DEF_RAW_13MHZ_MAX_TC - 35)  // online start to allows trigger at 65500


#ifndef FEATURE_GERAN_VSTMR
#define G_STMR_MICRO_IRQ_MSB_IGNORE_FRAME_NUM (2 << HWIO_G_STMR_SW_IRQ_CONFIG_RW_SW_IRQ_MSB_IGNORE_SHFT)
#define G_STMR_MICRO_IRQ_ENABLE               (1 << HWIO_G_STMR_SW_IRQ_CONFIG_RW_SW_IRQ_ENA_SHFT)
#define G_STMR_SYM_TO_RAW_13MHZ               48
#define G_STMR_QSYM_TO_RAW_13MHZ              12
#define G_STMR_RAW_13MHZ_TO_SYM               1366 /* 2^16 / 48 */
#define G_STMR_RAW_13MHZ_TO_QSYM              5462 /* 2^16 / 12 */
/* FCW = round((COUNT_FREQ/XO_FREQ)*(2^30-1))
e.g. if COUNT_FREQ = 13 MHz, XO_FREQ = 19.2 MHz, then FCW = 0x2B555555 */
#define G_STMR_FCW_CTL_13MHZ                  0x2B555555
#endif

#define GL1_CONVERT_QS_TO_US(qs_value)  ((60490 * (qs_value) ) >> 16)
#define GSTMR_NO_OF_SYS_CLKS_IN_FRAME   151
#define GSTMR_CONVERT_SYS_CLKS_TO_ISR_DELAY(sclk_value)  (((sclk_value) * 21701)>>16)


#define GL1_NO_OF_TASKNAME_CHARACTERS 16

/* Copied from gl1_drx_is_frame_number_later() in l1_drx.c.
 * TODO - clean up l1_drx.c and move comments here.
 */
#define IS_FRAME_NUM_LATER( fn_a, fn_b ) \
   ( ( (fn_a) > (fn_b) ) ? \
      (((fn_a) - (fn_b)) < FRAMES_PER_HYPERFRAME/2) : \
      (((fn_b) - (fn_a)) > FRAMES_PER_HYPERFRAME/2) )


#define IS_FRAME_NUM_LATER_OR_EQUAL( fn_a, fn_b ) \
   ( IS_FRAME_NUM_LATER( fn_a, fn_b ) || \
     (fn_a == fn_b) \
   )

/* Add this definition to log gstmr adjustments to a trace buffer */
#define DEBUG_GSTMR_ADJUST_TRACE
#ifdef DEBUG_GSTMR_ADJUST_TRACE
typedef struct
{
   uint32 FN;
   int16  tc_adjust;  /* Set to 0 if tc_set is set */
   int    tc_set;     /* Set to -1 if tc_adjust is set */
} gstmr_adjust_trace_type;

#define MAX_GSTMR_ADJUST_TRACE 20
extern uint16                  gstmr_adjust_trace_index;
extern gstmr_adjust_trace_type gstmr_adjust_trace_buf[];
#endif

#define MAX_NUM_ENTRIES                  ( NUM_GERAN_DATA_SPACES * 7 )
#define MAX_NUM_REGISTRATIONS            (MAX_NUM_ENTRIES - 1)

/* GSTMR tracing function */
#ifdef FEATURE_GSTMR_TRACE
void gstmr_trace (const char *fname, uint32 line, int32 param);
#define GSTMR_TRACE(p) gstmr_trace(__FILENAME__,__LINE__,(int32)(p))
#else
#define GSTMR_TRACE(p)
#endif

#define GL1_HAND_PERF_LOG_BUFFER_SIZE 100

/* Struct to hold gl1_handler_timestamp_event_type*/
typedef enum
{
    START_GSTMR_ISR = 0,
    STOP_GSTMR_ISR,
    START_GSM_TDMA_ISR,
    STOP_GSM_TDMA_ISR,
    START_MDSP_SWITCH_COMMAND_BUFFER_ISR,
    STOP_MDSP_SWITCH_COMMAND_BUFFER_ISR,
    START_TX_CLK_OFF_ISR,
    STOP_TX_CLK_OFF_ISR,
    START_CM_MODE_SW_ISR,
    STOP_CM_MODE_SW_ISR,
    START_DISABLE_EGPRS_MODE_ISR,
    STOP_DISABLE_EGPRS_MODE_ISR,
    START_GSAC_CLK_OFF_ISR,
    STOP_GSAC_CLK_OFF_ISR,
    START_GSTMR_FN_ISR,
    STOP_GSTMR_FN_ISR,
    START_RX_RF,
    STOP_RX_RF,
    START_TX_RF,
    START_TX_RF_SET_BAND,
    STOP_TX_RF_SET_BAND,
    START_TX_RF_TUNE_TO_CHAN,
    STOP_TX_RF_TUNE_TO_CHAN,
    START_TX_RF_POWER_CONTROL,
    STOP_TX_RF_POWER_CONTROL,
    STOP_TX_RF,
    START_PWR_MEAS_RF,
    STOP_PWR_MEAS_RF,
    START_SWEEP_PWR_MEAS_RF,
    STOP_SWEEP_PWR_MEAS_RF,
#ifdef FEATURE_GSM_ISR_PROFILING
    START_DEDICATED_MODE_HANDLER,
    STOP_DEDICATED_MODE_HANDLER,
    START_TRANSFER_MODE_HANDLER,
    STOP_TRANSFER_MODE_HANDLER,
    START_DTM_MODE_HANDLER,
    STOP_DTM_MODE_HANDLER,
    START_MSG_FRAME_NOTIFICATION_HANDLER,
    STOP_MSG_FRAME_NOTIFICATION_HANDLER,
    START_DL_HANDLER,
    STOP_DL_HANDLER,
    START_DL_PDCH_HANDLER,
    STOP_DL_PDCH_HANDLER,
    START_UL_HANDLER,
    STOP_UL_HANDLER,
    START_UL_PDCH_HANDLER,
    STOP_UL_PDCH_HANDLER,
    START_ACQ_HANDLER,
    STOP_ACQ_HANDLER,
    START_INT_MEAS_HANDLER,
    STOP_INT_MEAS_HANDLER,
#endif /*FEATURE_GSM_ISR_PROFILING */
    START_MULTISIM_DEACT,
    STOP_MULTISIM_DEACT,
    START_MULTISIM_INACT_TCXO,
    STOP_MULTISIM_INACT_TCXO,
    START_MULTISIM_INACT_TXRX,
    STOP_MULTISIM_INACT_TXRX,
    START_MULTISIM_INACT_MDSP,
    STOP_MULTISIM_INACT_MDSP,
    START_SRCHCM_GSM_ISR,
    STOP_SRCHCM_GSM_ISR
} tag_id;

typedef struct
{
  tag_id tag;

  /* Timestamp of event*/
  timetick_type    gl1_timestamp;

#ifdef FEATURE_GERAN_VSTMR
    uint32           modMaxFN_frame_num;
#endif

  uint16           mod4_frame_num;

  uint32           gl1_qsymcount;

  unsigned long long  pcycle;

  /* GSM Frame Number */
  uint32           frame_number;
} gl1_handler_timestamp_event_type;

typedef struct
{
  /* Index of last event */
  uint32                   last;

  /* Buffer to hold trace events */
  gl1_handler_timestamp_event_type events[GL1_HAND_PERF_LOG_BUFFER_SIZE];
} gl1_handler_timestamp_type;

extern void gl1_log_handler_timestamp_event(tag_id tag,gas_id_t gas_id);
typedef struct
{
  uint32        timestamp;
  char          taskname[GL1_NO_OF_TASKNAME_CHARACTERS];
  uint32        tick_duration;
  uint32        to_tcb;
} gl1_trace_buffer_type;

#define GL1_HANDLER_PERFORMANCE_LOGGING(tag,gas_id) \
            gl1_log_handler_timestamp_event(tag,gas_id);

typedef struct
{
  gl1_handler_timestamp_type gl1_handler_timestamp;

#ifdef FEATURE_GSM_L4_TRACE_BUFF
  gl1_trace_buffer_type  gl1_trace_buffer[GL1_MAX_NO_TRACE_BUFFER_ENTRIES];
#endif

#ifdef FEATURE_MAINTAIN_GTIME_IN_WCDMA_DEBUG
  #error code not present
#endif

  /* Variables indicating if we have already initialized */
  boolean gstmr_gsm_core_initialized;
  boolean gstmr_initialized;
  boolean gstmr_gsm_core_reinit;
  boolean gstmr_reinit;

  /* GSM/TDMA frame number */
  uint32 gstmr_FN;
#ifdef FEATURE_GERAN_VSTMR
  uint32 gstmr_FN_modMaxFN;
  uint32 vstmr_sleep_fn;
  uint32 vstmr_deregister_sw_fn;
  uint32 vstmr_deregister_fn_modMaxFN;
#else /* FEATURE_GERAN_VSTMR */
  uint32 gstmr_FN_mod4;
#endif /* !FEATURE_GERAN_VSTMR */

  /* number of handlers registered to the gstmr isr */
  uint16 num_handlers;

  /* number of qs to delay the irq to the ARM */
  uint32 gstmr_delay_qs;

/* Remember pending timing adjustments and terminal count sets
 * so that one of each can be handled in the same frame.
 */
  int16  tc_adjust_qs;
  uint32 tc_set;

#ifdef FEATURE_GERAN_VSTMR
  uint64 UstmrSleepStartTime;
#else
  uint32 UstmrSleepStartTime;
#endif
  uint32 GstmrSleepStartTime;
  uint32 GsmFNSleepStart;

  /* This is used to decide whether to run the odd even check */
  boolean gstmr_odd_even_check;
  boolean init_odd_even_check;
  uint32 last_oe;

  /* Store the status of the last start sleep attempt so
   * it can be checked later for error checking.
   */
  boolean gstmr_last_start_sleep_was_ok;

  int16 gstmr_handler_executing;

  /* The qs value when the current or last gstmr ISR started executing */
  uint32 gstmr_qs_start;
  
  /* The qs value when the current or last gstmr ISR started executing */
  uint32 gstmr_qs_end;
  
  /* The ustmr tick value when the current or last gstmr ISR started executing */
  uint64 gstmr_ustmr_start;

  /* Tracks if there was GSTMR fired whilst doing Go To Sleep procedure */
  boolean gl1_hw_sleep_gts_gstmr_fired;

  /* Index of last handler to run this frame in case new handlers are added
     during the ISR execution */
  int16 last_handler;

#ifdef FEATURE_GSM_ISR_PROFILING
  uint16 time_result_buffer [50];
#endif /* FEATURE_GSM_ISR_PROFILING */

/* Installed handlers */
  gstmr_handler_type gstmr_handlers[MAX_NUM_ENTRIES];

#ifdef DEBUG_GSTMR_ADJUST_TRACE
  uint16                  gstmr_adjust_trace_index;
  gstmr_adjust_trace_type gstmr_adjust_trace_buf[MAX_GSTMR_ADJUST_TRACE];
#endif

  /* Used to detect if asynch int executed already for normal gstmr operation */
  boolean gstmr_asynch_int_executing;
  gtmrs_hw_t gtmrs_hw;
  boolean gstmr_disabled_for_sleep;
  boolean fnqs_rd_disabled_for_slamming;

#ifdef FEATURE_GERAN_VSTMR
  /*vstmr view data*/
  vstmr_geran_view_s *geran_view;
  vstmr_geran_event_tbl_s  geran_event_tbl;
  void *geran_isr_cb_arg;
  void (*vstmr_isr_callback)(struct vstmr_geran_event_tbl_s_ *event_tbl, void *arg, uint32 pending);
#endif
}gtmrs_data_t;

extern gtmrs_data_t gtmrs_data[];

/* -----------GSTMR_QBIT_ADJ---------------- */
LOG_RECORD_DEFINE(LOG_GSM_GSTMR_ADJ_C)
    int16   tc_adjust;
LOG_RECORD_END

/* USTMR conversion */
#ifdef FEATURE_GERAN_VSTMR
#define USTMR_MAX_TICKS               0x0ffffff /* 24 bit */
#else /* FEATURE_GERAN_VSTMR */
#define USTMR_MAX_TICKS               0x7ffffff /* 27 bit */
#endif /* FEATURE_GERAN_VSTMR */

/* USTMR_FROM_QS = ( qs * ( 17.7 * 32 ) / 32 */
#define USTMR_NORM_SHIFT              ( 5 )
#define USTMR_FROM_QS( qs )           ( ( ( (qs) * 567 ) + \
                                          ( 1 << ( USTMR_NORM_SHIFT - 1 ) ) ) >> USTMR_NORM_SHIFT )

#define USTMR_PER_FRAME               ( 88608 ) /* 19.2MHz * 4.615ms = 19.2 * 4615 */
#define USTMR_PER_SLOT                ( USTMR_PER_FRAME / GL1_DEFS_SLOTS_IN_FRAME )
#define USTMR_PER_QS                  ( USTMR_FROM_QS( (1) ) )
#define USTMR_SLOT_GUARD_PERIOD       ( USTMR_FROM_QS( (10) ) ) /* 10 qs */
#define USTMR_FRAME_TICK_SETUP_MARGIN ( USTMR_PER_SLOT )

#define USTMR_IS_LATER( ustmr_a, ustmr_b ) \
    ( ( (ustmr_a) > (ustmr_b) ) ? \
      ( ( (ustmr_a) - (ustmr_b) ) < (USTMR_MAX_TICKS / 2 ) ) : \
      ( ( (ustmr_b) - (ustmr_a) ) > (USTMR_MAX_TICKS / 2 ) ) )

#define USTMR_IS_CONFLICT( ustmr_a_s, ustmr_a_e, ustmr_b_s, ustmr_b_e ) \
    ( !( USTMR_IS_LATER( (ustmr_b_s), (ustmr_a_e) ) || \
         USTMR_IS_LATER( (ustmr_a_s), (ustmr_b_e) ) ) )

#define USTMR_ADD( ustmr_a, ustmr_b ) \
    ( ( (ustmr_a) + (ustmr_b) < (USTMR_MAX_TICKS) ) ? \
      ( (ustmr_a) + (ustmr_b) ) : \
      ( (ustmr_a) + (ustmr_b) - (USTMR_MAX_TICKS) ) )
#define USTMR_SUB( ustmr_a, ustmr_b ) \
    ( ( (ustmr_a) >= (ustmr_b) ) ? \
      ( (ustmr_a) - (ustmr_b) ) : \
      ( (USTMR_MAX_TICKS) + (ustmr_a) - (ustmr_b) ) )

#define QS_FROM_USTMR(ustmr) ( (ustmr) / USTMR_PER_QS )

/* Search Widths */
#define USTMR_DEFAULT_SEARCH_WIDTH ( USTMR_FROM_QS( ( GL1_HW_DEFAULT_SEARCH_WIDTH * 4 ) / 2 ) )
#define USTMR_TCH_SEARCH_WIDTH     ( USTMR_FROM_QS( ( GL1_HW_TCH_SEARCH_WIDTH * 4 ) / 2 ) )
#define USTMR_GPRS_SEARCH_WIDTH    ( USTMR_FROM_QS( ( GL1_HW_GPRS_SEARCH_WIDTH * 4 ) / 2 ) )

#endif /* GTMRS_G_H */
