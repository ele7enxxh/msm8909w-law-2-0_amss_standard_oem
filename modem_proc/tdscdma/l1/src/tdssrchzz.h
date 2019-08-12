#ifndef TDS_SRCH_ZZ_H
#define TDS_SRCH_ZZ_H
/*===========================================================================

            S R C H  Z Z (sleep)   R E L A T E D   D E C L A R A T I O N S

DESCRIPTION This file contains the declarations for the srch module 
related to sleep and wakeup

EXTERNALIZED FUNCTIONS

tdssrchzz_prepare_for_sleep
  This function is called to indicate to srch that a sleep cycle is immanent. 
  This function initiates aborting of all searching in progress, and saves
  the PN positions of the fingers via a status dump. When done, it posts a 
  TDS_DRX_SLEEP_CMD local command.

srchzz_prepare_for_wakeup
  This function is called upon wakeup, after RF blocks have warmed up.
  This function initiates reacquisition searching and triage upon successful
  reacq. Finally, it posts a TDS_DRX_SLEEP_CMD local command with the status of
  reacquisition.
  

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

REFERENCES

Copyright (c) 2001-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdssrchzz.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/14   cliu    Register GSM bands with TRM before call trm_reserve_at for feature DR_DSDS
07/11/12   zl      Correct T2L measurement scheduling issues
07/10/12   zl      Optimize T2L measurement scheduling
07/14/10   mm      Fix for compiler warnings 
06/21/10   uk      Compiler warning fix
06/02/10   stk     Added support for W2LTE measurements and reselection under 
                   FEATURE_TDSCDMA_ABSOLUTE_PRIORITY & FEATURE_TDSCDMA_TO_LTE
09/04/09   saik    Use new srch api to determine if reacq srch is in progress.
06/03/09   stk     Fixed compiler warning - moved tdssrchzz_config_cell_prior_triage() to srchzz.c
05/27/09   stk     Fixed Lint warnings
05/08/09   sup      Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to tdsdec.h, tdsenci.h, tdsl1m.h, tdssrch.h, tdssrchi.h, tdssrchlog.h
04/22/09   stk     Compiler warning fixes 
03/13/09   rmak    Added tdssrchzz_resp_early_rf_sleep to support Early RF Sleep functionality
12/09/08   uk      Mainling FEATURE_WCDMA_DL_ENHANCED, FEATURE_ENHANCED_CELL_RESELECTION, 
                   FEATURE_WCDMA_DRX_MANAGER, FEATURE_WCDMA_OFFLINE_SEARCHER 
                   and FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT 
10/03/08   hk      Bringing in 7k Mailine fixes
09/14/08   uk      Adding tdssrchzz_substate_type and srch_pending bmask for srch
                   done when aset srch is done on aset_reacquisition after reacq fail
06/10/08   ms      Added PACKED_POST attribute for structures to make it
                   compatible for GNU compiler.
12/03/07   sk      Declaration for function to start full search on PL info req
11/05/07   ub      Fixed new cell reselecion feature merge
11/01/07   ub      MBMS: DRX mode filtering when MCCH/MTCH enabled protitypes
10/26/07   sk      Checking in MBMS branch code into mainline with MBMS feature disabled
10/04/07   sk      Code cleanup
08/20/07   kps     Change reacq srch counter variable names
07/17/07   sk      Fix a compilation error due to upmerge to 4111
05/23/07  ub/sk    Intergrate GSM meas and code cleanup
04/28/07   scm     Change DRX_ENTITY_BCCH_S(N) to TDSDRXMGR_ENTITY_BCCH_INT(EXT).
04/25/07  ub/sk    Inter freq resel with MBMS changes
04/01/07  ub/sk    FEATURE_ENHANCED_CELL_RESELECTION changes
12/12/06   mc      Synced up for MBMS Phase2A integration
03/03/07   scm     Change logical entity bitmask size from uint16 to uint32.
02/22/07   scm     Temporary stubs for new SRCH API.
02/20/07   scm     Data definitions to interface with new DRX Manager.
02/07/06   scm     New tdssrchzz_substate: SRCHZZ_NSET_FOR_SIBS for waking from
                   sleep to acquire neighbor and read SIBs.
12/22/05   gv      Modified the SRCHZZ_ASET_HB substate to SRCHZZ_ASET_NSET_HB.
           Also removed the substates SRCHZZ_NSET_LIST and 
           SRCHZZ_OF_NSET_LIST as these list searches are now performed
           as a part of the substate SRCHZZ_ASET_NSET_HB (20ms).
12/09/04   sh      Added SRCHZZ_ASET_HB substate for aset search using search
                   heartbeat event.
10/28/04   sh      Added tdssrchzz_gsm_clk_ena_req to indicate if srch needs to
                   wait until GSM clk and TX System Time is turned on after
                   a SIB mod update.
06/05/04   asr     Extern declarations of tdssrchzz_reset_reacq_list_passed_good_dl() 
                   and tdssrchzz_get_reacq_list_passed_good_dl().
05/11/04   scm     Extern tdssrchzz_reacq_slew and tdssrchzz_num_failed_reacquisitions.
04/28/04   sh      Added substate for inter-frequency list searching.
04/12/04   sh      Removed srchzz substate SRCHZZ_WOKEN_UP.
04/05/04   sh      Added two new states to srchzz.
01/16/04   asr     Added extern declaration of tdssrchzz_using_searcher_ref_position
11/12/03   sh      Added substates for inter-frequency searching in Idle.
07/28/03   scm     Support for query of various DRX reacquisition statistics.
05/27/03   asr     Created extern declaration of tdssrchzz_reacq_list_search_in_progress.
04/31/03   asr     Created extern declaration of tdssrchzz_just_sent_oos.
04/17/02   sh      Added function proto srchzz_save_and_disable_fing_info().
02/12/03   sh      Added function proto tdssrchzz_send_srch_ready_for_sleep().
06/04/02   sh      Added function srchzz_declare_reacq_status_drx().
05/18/02   sh      Added zz substates for step 1/2/3 and list searches
                   during cell reselection evaluation and other support
                   variables.
01/31/02   asr     created file
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdssrchcmd.h"
#include "tdssrch.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define TDSSRCHZZ_ASET_MAX_IN_IDLE 1
#define TDSSRCHZZ_MAX_NUM_CELLS (TDSSRCHZZ_ASET_MAX_IN_IDLE + \
                              TDSSRCHSET_NBCH_SET_MAX)

  /* Timers */  
#define TDSSRCHZZ_ASET_LIST_SRCH_TIMER              0x1
#define TDSSRCHZZ_INTRA_LIST_SRCH_TIMER             0x2
#define TDSSRCHZZ_INTRA_FULL_SRCH_TIMER             0x4
#define TDSSRCHZZ_INTER_F0_LIST_SRCH_TIMER          0x8
#define TDSSRCHZZ_INTER_F1_LIST_SRCH_TIMER          0x10
#define TDSSRCHZZ_INTER_F0_FULL_SRCH_TIMER          0x20
#define TDSSRCHZZ_INTER_F1_FULL_SRCH_TIMER          0x40
#define TDSSRCHZZ_IRAT_RSSI_TIMER                   0x80
#define TDSSRCHZZ_IRAT_BSIC_TIMER                   0x100
#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
#define TDSSRCHZZ_LTE_CELL_DETECT_SRCH_TIMER        0x200
#define TDSSRCHZZ_LTE_CELL_MEAS_SRCH_TIMER          0x400
#endif
#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
#define TDSSRCHZZ_IRAT_HIGH_PRIO_RSSI_TIMER         0x800
#endif

#define TDSSRCHZZ_INTERF_OFREQ_FREQ_MAX                (11+3)

/*--------------------------------------------------------------------------
                          ZZ SEARCH SUBSTATES

--------------------------------------------------------------------------*/
 typedef enum 
 {
   TDSSRCHZZ_ASLEEP,       /* Stop Search        */
 
//#ifdef FEATURE_IRAT_RESELECTION
   TDSSRCHZZ_IRAT,         /* IRAT search        */
//#endif
#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
   TDSSRCHZZ_LTE,
#endif
   /* TDS SRCHZZ substate*/
   TDSSRCHZZ_INACTIVE,   
   TDSSRCHZZ_MAIN,
   TDSSRCHZZ_NEIGHBOR,
   TDSSRCHZZ_STATE_MAX        /* Last item in enum. */
 }tdssrchzz_substate_enum_type;


typedef struct 
{
  uint16 intra_freq_meas_status;
  uint16 inter_freq_meas_status[TDSSRCHZZ_INTERF_OFREQ_FREQ_MAX];
  uint16 inter_rat_meas_status;
  uint16 timer_expiry_status;
}tdssrchzz_idle_meas_status_struct_type;



/* Counters and defines to maintain statistics on reacquisition slew values
** during DRX operation.  Values can be queried by the QXDM.  All slews are
** in units of chipx8. */
typedef PACKED struct PACKED_POST
{
  uint32 srchzz_reacq_slew_small_cnt;    /* (-20...20)                   */
  uint32 srchzz_reacq_slew_med_cnt;      /* (-256...-21) or (21...256)   */
  uint32 srchzz_reacq_slew_large_cnt;    /* (-768...-257) or (257...768) */
  uint32 srchzz_reacq_slew_abnormal_cnt; /* (< -768) or (> 768)          */
} srchzz_reacq_slew_stats_type;

extern srchzz_reacq_slew_stats_type tdssrchzz_reacq_slew_stats;


/* Counters to maintain statistics on successful and failed reacquisition
** searches during DRX operation.  Values can be queried by the QXDM. */
typedef PACKED struct PACKED_POST
{
  uint32 srchzz_reacq_list_success_cnt;
  uint32 srchzz_reacq_list_failure_cnt;
  uint32 srchzz_reacq_123_success_cnt;
  uint32 srchzz_reacq_123_failure_cnt;
} srchzz_reacq_srch_stats_type;

extern srchzz_reacq_srch_stats_type tdssrchzz_reacq_srch_stats;

extern tdssrchzz_substate_enum_type tdssrchzz_substate;

extern tdssrchzz_substate_enum_type tdssrchzz_substate_tmp;


/* Flag to indicate if SRCH has to wait until GSM clock is enabled
 * before sending sending SRCH_DONE cmd to DRX. This flag will be set
 * when GSM clock needs to be enabled after a SIB modification update.
 */
extern boolean tdssrchzz_gsm_clk_ena_req;

extern boolean tdssrchzz_reacq_failed;
extern boolean tdssrchzz_reacq_inprogress;
extern boolean tdssrchzz_delay_sleep_for_reacq_failed;
extern boolean tdssrchzz_retrial_reacq ;

/* To count the number of contiguous failed reacquisitions. */
extern uint16 tdssrchzz_num_failed_reacq;

//extern tdssrchzz_substate_type_enum_type tdssrchzz_substate_type;
//extern tdssrchzz_srch_mode_handler_struct* tdssrchzz_srch_mode_handler[TDSSRCHZZ_SRCH_MODE_NA];
//extern tdssrchzz_srch_mode_struct_type tdssrchzz_mode_info;
extern tdssrchzz_idle_meas_status_struct_type tdssrchzz_meas_status;

#define TDSSRCHZZ_CHECK_IRAT_DUE( ) \
    ((tdssrchzz_meas_status.timer_expiry_status & (TDSSRCHZZ_IRAT_BSIC_TIMER | TDSSRCHZZ_IRAT_RSSI_TIMER)) != 0)
#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
/* Check if inter freq full search or list srch timer has expired*/
#define TDSSRCHZZ_CHECK_INTERF_DUE( ) \
    (tdssrchzz_meas_status.timer_expiry_status & (TDSSRCHZZ_INTER_F1_LIST_SRCH_TIMER | TDSSRCHZZ_INTER_F0_LIST_SRCH_TIMER \
     | TDSSRCHZZ_INTER_F0_FULL_SRCH_TIMER | TDSSRCHZZ_INTER_F1_FULL_SRCH_TIMER) )

#define TDSSRCHZZ_CHECK_IRAT_HIGH_PRIO_RSSI_DUE( ) \
    ((tdssrchzz_meas_status.timer_expiry_status & (TDSSRCHZZ_IRAT_HIGH_PRIO_RSSI_TIMER)) != 0)
#endif
#define TDSSRCHZZ_CHECK_IRAT_RSSI_DUE() \
    ((tdssrchzz_meas_status.timer_expiry_status &  TDSSRCHZZ_IRAT_RSSI_TIMER) != 0)
#define TDSSRCHZZ_CHECK_IRAT_BSIC_DUE() \
      ((tdssrchzz_meas_status.timer_expiry_status &  TDSSRCHZZ_IRAT_BSIC_TIMER) != 0)

#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
/* Check if LTE Cell Detect search or Cell Meas srch timer has expired*/
#define TDSSRCHZZ_CHECK_LTE_SRCH_DUE( ) \
      (tdssrchzz_meas_status.timer_expiry_status & (TDSSRCHZZ_LTE_CELL_DETECT_SRCH_TIMER | TDSSRCHZZ_LTE_CELL_MEAS_SRCH_TIMER ))

/* Check if LTE Cell Detect search timer has expired*/
#define TDSSRCHZZ_CHECK_LTE_CELL_DETECT_SRCH_DUE( ) \
        (tdssrchzz_meas_status.timer_expiry_status & TDSSRCHZZ_LTE_CELL_DETECT_SRCH_TIMER)

/* Check if LTE Cell Meas search timer has expired*/
#define TDSSRCHZZ_CHECK_LTE_CELL_MEAS_SRCH_DUE( ) \
        (tdssrchzz_meas_status.timer_expiry_status & TDSSRCHZZ_LTE_CELL_MEAS_SRCH_TIMER)
#endif

#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
#define TDSSRCHZZ_CHECK_LTE_MEAS_DUE()\
  (tdssrchzz_meas_sched_manager.lte_meas_timer_expiry_status & TDSSRCHZZ_LTE_MEAS_TIMER)
/*Chek whether this is for next DRX cycle. */
#define TDSSRCHZZ_CHECK_LTE_MEAS_DUE_NEXT_DRX()\
  (tdssrchzz_meas_sched_manager.lte_meas_count >= (tdssrchzz_meas_sched_manager.lte_meas_period -1))

#define TDSSRCHZZ_CHECK_LTE_DETECT_DUE_NEXT_DRX()\
  (tdssrchzz_meas_sched_manager.lte_detect_meas_count >= (tdssrchzz_meas_sched_manager.lte_detect_meas_period -1))

#define TDSSRCHZZ_CHECK_LTE_DETECT_MEAS_DUE()\
  (tdssrchzz_meas_sched_manager.lte_meas_timer_expiry_status & TDSSRCHZZ_LTE_DETECT_MEAS_TIMER)


#define TDSSRCHZZ_RESET_LTE_MEAS_TIMER( ) \
{  /*tdssrchzz_meas_sched_manager.lte_meas_timer_expiry_status&=(~TDSSRCHZZ_LTE_MEAS_TIMER);\ */ \
   tdssrchzz_meas_sched_manager.lte_meas_count = 0; \
   tdssrchzz_meas_sched_manager.lte_meas_timer_expiry_status &= ~(TDSSRCHZZ_LTE_MEAS_TIMER);}

#define TDSSRCHZZ_RESET_LTE_MEAS_TIMER_NEXT_DRX( ) \
{    \
   tdssrchzz_meas_sched_manager.lte_meas_count_reset_pending = TRUE;  \
}

#define TDSSRCHZZ_RESET_LTE_DETECT_TIMER( ) \
{  tdssrchzz_meas_sched_manager.lte_meas_timer_expiry_status&=(~TDSSRCHZZ_LTE_DETECT_MEAS_TIMER); \
   tdssrchzz_meas_sched_manager.lte_detect_meas_count = 0;}

#endif

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION     SRCHZZ_SEND_SRCH_READY_FOR_SLEEP

DESCRIPTION
  This function is called when DRX has finished demodulating the PICH
  and search is free to tune away from the serving cell to perform
  measurements.

  This function will check to see if inter-frequency and/or inter-RAT
  measurements are pending and sends a local command to L1M to initiate
  these measurements, if not, it will send a local command DRX indicating
  searcher module is ready to sleep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initiates inter-frequency and/or inter-RAT measurements if required,
  else indicates to DRX that search is ready to sleep.

===========================================================================*/

extern void tdssrchzz_send_srch_ready_for_sleep( void );

/*===========================================================================

FUNCTION     TDSSRCHZZ_WAIT_ON_SRCH_SLEEP

DESCRIPTION
  This function handles the DRX->SEARCHER 'go to sleep' request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void tdssrchzz_wait_on_srch_sleep(void);
/*===========================================================================

FUNCTION     TDSSRCHZZ_WAKEUP_CELL

DESCRIPTION
  This function is called by DRX Manager to instruct SRCH to beging tracking
  a cell.  

DEPENDENCIES
  None.

PARAMETERS
  wakeup_set_ptr - Array of cell structure(s) to acquire and track.
  boolean ue_wakeup - TRUE: no any channel is on, UE just is wakeup.
                                  - FALSE: at lease one channel is on, and UE already is awake.

RETURN VALUE
  None.
===========================================================================*/
extern void tdssrchzz_wakeup_cell
(
  tdssrch_wakeup_set_struct_type *wakeup_set_ptr,
  boolean ue_wakeup
);

/*===========================================================================

FUNCTION     SRCHZZ_INIT_DRX_SRCH_MODE

DESCRIPTION
  Thisn function is called on entering idle mode. 
  Reset all idle timer and initialize the search mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_init_drx_srch_mode(void);
/*===========================================================================

FUNCTION     SRCHZZ_INCREMENT_IDLE_MEAS_TIMERS

DESCRIPTION
  This function icnrements the timers which are running for idle mode searches. The increment value is 
  sfn based.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_increment_idle_meas_timers (void);

/*===========================================================================

FUNCTION     SRCHZZ_UPDATE_TIMER_EXPIRY_STATUS

DESCRIPTION
  This fucntions updates the timer expiry status of all searches based on the current running timer and the
  max periodic timer value. This is called from the scheduler.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_update_timer_expiry_status(void);
/*===========================================================================

FUNCTION     SRCHZZ_CHECK_IRAT_SRCH

DESCRIPTION
  This function checks if valid irat cells are there to be searched.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdssrchzz_check_irat_srch( void );
/*===========================================================================

FUNCTION     SRCHZZ_SET_ALL_SRCH_DUE

DESCRIPTION
  This fucntions sets the timer expiry status of all searches. This function is called on reacq failure or on
  starting cell reselection when all searches need to be triggered.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_set_all_srch_due(void);

/*===========================================================================

FUNCTION     SRCHZZ_RESET_ALL_SRCH_DUE

DESCRIPTION
  This fucntions sets the timer expiry status of all searches. This function is called on reacq failure or on
  starting cell reselection when all searches need to be triggered.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_reset_all_srch_due(void);

/*===========================================================================

FUNCTION     SRCHZZ_RESET_IDLE_MEAS_TIMER

DESCRIPTION
  This function reset the timers corresponding to a particular search.
  Timer expiry status is first reset and the incrementing search timer is also reset. This function should
  when a search has been completed or after a cell reselection.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_reset_idle_meas_timer (uint16 timer_bmsk);
extern void tdssrchzz_check_n_do_resel_eval(void);
extern void tdssrchzz_ready_to_sleep(void);

extern void tdssrchzz_check_n_do_next_lte_srch(void);

#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY

/*===========================================================================
FUNCTION       tdssrchzz_init_meas_sched_manager_for_abs_prio

DESCRIPTION
  This function initialize the meas sched manager if absolute priority info provided
  by SIB19

DEPENDENCIES
  None

RETURN VALUE
	None
SIDE EFFECTS
  None
  
===========================================================================*/
void tdssrchzz_init_meas_sched_manager_for_abs_prio(void);

/*===========================================================================
FUNCTION       tdsrchzz_check_inter_f_abs_priority_scheduling

DESCRIPTION
  This function checks if the GSM cell qualifies for measurements in the 
  current DRX cycle or not. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If the passed in GSM cell qualifies for measurements in the current DRX cycle. 
  FALSE - If the passed in GSM cell doesn't qualify for measurements in the current DRX cycle.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsrchzz_check_inter_f_abs_priority_scheduling(int8 freq_index);

/*===========================================================================
FUNCTION       tdssrchzz_check_gsm_cell_abs_priority_scheduling

DESCRIPTION
  This function checks if the GSM cell qualifies for measurements in the 
  current DRX cycle or not. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If the passed in GSM cell qualifies for measurements in the current DRX cycle. 
  FALSE - If the passed in GSM cell doesn't qualify for measurements in the current DRX cycle.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean tdssrchzz_check_gsm_cell_abs_priority_scheduling(tdssrchset_gsm_cell_struct_type	*gsm_cell_ptr);
/*===========================================================================

FUNCTION     SRCHZZ_SET_MAX_TIMER_FOR_PRIOR_SCHEDULING

DESCRIPTION
  This function checks what is the current scheduling mode according to
  priority and sets the max timer for all searches accordingly

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_set_max_timer_for_prior_scheduling(void);

/*===========================================================================

FUNCTION     tdssrchzz_set_max_timer_for_prior_scheduling_LTE

DESCRIPTION
  This function checks what is the current scheduling mode according to
  priority and sets the max timer for LTE searches accordingly.
  Especially, it will use or not use Fast measurement table for LTE based
  whether Tresel for LTE is running or not. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_set_max_timer_for_prior_scheduling_LTE(void);

/*===========================================================================

FUNCTION     tdssrchzz_reset_all_srch_due_for_abs_prio

DESCRIPTION
  

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_reset_all_srch_due_for_abs_prio(void);

/*===========================================================================

FUNCTION     tdssrchzz_set_lte_timers_to_max

DESCRIPTION    this function set the LTE timers to the maximal value, so that 
               LTE detection and measurement can be scheduled right after SIB 19
               is configured.  

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_set_lte_timers_to_max(void);

#endif

/*===========================================================================

                        DATA DECLARATIONS FOR TDSCDMA

===========================================================================*/

  /* Timers */  
#define TDSSRCHZZ_IRAT_RSSI_MEAS_TIMER                 0x01
#define TDSSRCHZZ_IRAT_BSIC_RC_TIMER                      0x02
#define TDSSRCHZZ_IRAT_BSIC_ID_TIMER                      0x04

#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
#define TDSSRCHZZ_LTE_MEAS_TIMER                    0x01
#define TDSSRCHZZ_LTE_DETECT_MEAS_TIMER             0x02
#endif

/* Minimum bsic reconfirmation periodicity */
/*zy modify from subframe to frame,because drx length is in unit of 10ms*/
#define TDSSRCHZZ_MAX_BSIC_RC_DUE_FRAMES  3000 /* 30s*/

#ifdef FEATURE_TDSCDMA_DRX_MEAS_SCHE_OPT
typedef enum
{
  TDSSRCHZZ_FAST_MODE,
  TDSSRCHZZ_BASELINE_MODE,
  TDSSRCHZZ_SLOW_MODE1,
  TDSSRCHZZ_SLOW_MODE2,
  TDSSRCHZZ_MEAS_PERIOD_MODE_NUM
}
tdssrchzz_meas_period_mode_enum_type;
#endif

typedef struct
{
  /* frequency index in tdssrchset_freq_set[] */
  uint8 freq_index;
  /* measurement counter,  its unit is sub-frame.*/
  #ifdef FEATURE_TDSCDMA_DRX_MEAS_SCHE_OPT
  uint32 count[TDSSRCHZZ_MEAS_PERIOD_MODE_NUM];
  #else
  uint32 count;
  #endif
   /*The index point to rset,it points out which cell will be scheduled next time */
  uint8 scan_rset_index;
}tdssrchzz_freq_info;

typedef enum
{
  TDSSRCHZZ_IDLE_TIMESLOT,
  TDSSRCHZZ_IDLE_SUBFRAME,
}
tdssrchzz_sched_mode;

typedef struct
{
  /* intra-freq measurement scheduling information */
  tdssrchzz_freq_info intra_freq;

  /* inter-freq measurement scheduling information */
  uint8 interf_num;
  tdssrchzz_freq_info inter_freq[TDSSRCHZZ_INTERF_OFREQ_FREQ_MAX];

  uint16 intraf_meas_period;  
  #ifdef FEATURE_TDSCDMA_DRX_MEAS_SCHE_OPT
  tdssrchzz_meas_period_mode_enum_type meas_period_mode;  
  uint16 interf_meas_period[TDSSRCHZZ_MEAS_PERIOD_MODE_NUM];
  /* each bit is corresponding to one freq idx in tdssrchset_freq_set[] */
  uint32 tds_meas_timer_expiry_status[TDSSRCHZZ_MEAS_PERIOD_MODE_NUM];  
  #else
  uint16 interf_meas_period;
  /* each bit is corresponding to one freq idx in tdssrchset_freq_set[] */
  uint32 tds_meas_timer_expiry_status;  
  #endif

  /* For idle sub-frame, if we are waiting for any measurement results, 
   * we still think it is idle timeslot mode now, then we can continue
   * scheduling GSM measurement.*/
  tdssrchzz_sched_mode sched_mode;

  /* inter-RAT rssi measurement scheduling information */
  uint32 irat_rssi_count;
  uint32 irat_rssi_meas_period;
  /* inter-RAT bsic reconfirmation scheduling information*/
  uint32 irat_bsic_rc_count;
  uint32 irat_bsic_rc_meas_period;

  /* each bit is corresponding to one measurement type */
  uint16 irat_meas_timer_expiry_status;
  /*T2L measurement timers and expiry status*/
  #if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
  uint32 lte_meas_count;
  uint32 lte_meas_period;
  uint32 lte_detect_meas_count;
  uint32 lte_detect_meas_period;
  /*For precise measurement timing in all priority mode*/
  //uint32 lte_meas_count_all[TDSSRCHLTE_MAX_NUM_FREQ];
  boolean lte_meas_count_reset_pending;
  uint16 lte_meas_timer_expiry_status;
  #endif  
}tdssrchzz_meas_sched_manager_struct_type;

/*===========================================================================

FUNCTION     TDSSRCHZZ_DRX_MEAS_SCHEDULER

DESCRIPTION
  Search Scheduler in drx mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_drx_meas_scheduler ( void );

/*===========================================================================

FUNCTION     TDSSRCHZZ_INIT_MEAS_SCHED_MANAGER

DESCRIPTION
  This function init drx measurement scheduling manager for periodic measurements. 
  This is called after reselecting to/selecting a cell

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/

extern void tdssrchzz_init_meas_sched_manager (void);


/*===========================================================================

FUNCTION     SRCHZZ_SET_RES_EVAL_STATUS

DESCRIPTION
  This function resets the resel eval globals bases on the current search mode.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resel Eval will be reset and will be done only after the next set of measurements are complete.

===========================================================================*/
extern void tdssrchzz_set_res_eval_status(void);

/*===========================================================================

FUNCTION     TDSSRCHZZ_PROC_RSCP_DONE

DESCRIPTION
  This function is used to check if SRCH can go to sleep.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: SRCH can go to sleep. FALSE: SRCH cannot go to sleep.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchzz_proc_rscp_done(uint8 message_id);

/*===========================================================================

FUNCTION     TDSSRCHZZ_WAKEUP_FOR_PICH

DESCRIPTION
  This function is used to check if SRCH can go to sleep.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: SRCH can go to sleep. FALSE: SRCH cannot go to sleep.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdssrchzz_check_sleep_conditions(void);

extern boolean tdssrchzz_trigger_rscp_meas_on_scell_barred(tdsl1_cell_bar_info_e_type l1_cell_bar_info);

extern void tdssrchzz_send_reacq_cmd(tdssrchset_cell_struct_type_tmp * reacq_cell);
extern void tdssrchzz_proc_reacq_rsp(tfw_srch_reacq_result_t *reacq_rsp);
extern void tdssrchzz_restore_tfw_state_n_timing (void);

extern void tdssrchzz_reset_tds_drx_meas_pending(uint8 freq_index);

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
/*===========================================================================

FUNCTION     tdssrchzz_register_gsm_band_before_sleep

DESCRIPTION
  Register GSM band, which is to be scheduled in next DRX cycle, to TDRM before go to sleep.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 tdssrchzz_register_gsm_band_before_sleep (void); 
#endif /*FEATURE_TDSCDMA_TRM_NEW_API*/

#ifdef FEATURE_TDSCDMA_PCH_FET
/*===========================================================================

FUNCTION     tdssrchzz_set_pending_ts0_for_pch_fet

DESCRIPTION
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tdssrchzz_set_pending_ts0_for_pch_fet_cnt(uint8 value);

/*===========================================================================

FUNCTION     tdssrchzz_updt_pending_ts0_for_pch_fet_cnt

DESCRIPTION  decrease pending counter
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tdssrchzz_updt_pending_ts0_for_pch_fet_cnt(void);

/*===========================================================================

FUNCTION     tdssrchzz_get_pending_ts0_for_pch_fet

DESCRIPTION
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 tdssrchzz_get_pending_ts0_for_pch_fet_cnt(void);

#endif // FEATURE_TDSCDMA_PCH_FET

#ifdef FEATURE_TDSCDMA_DSDS 
/*===========================================================================

FUNCTION     tdssrchzz_calc_t2g_dur_next_drx

DESCRIPTION
  calculate t2g measure duration for the next DRX cycle before go to sleep.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 tdssrchzz_calc_t2g_duration_next_drx (void);
#endif /*FEATURE_TDSCDMA_DSDS*/

#endif /* SRCH_ZZ_H */
