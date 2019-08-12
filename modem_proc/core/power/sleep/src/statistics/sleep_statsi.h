#ifndef SLEEP_STATSI_H
#define SLEEP_STATSI_H
/*==============================================================================
  FILE:         sleep_statsi.h
  
  OVERVIEW:     Internal functions & types to gather and adjust
                sleep statistics
                
                Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/statistics/sleep_statsi.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "LprTypes.h"
#include "sleep_stats.h"
#include "sleep_lpr.h"
#include "lookup_table_types.h"

/*
===============================================================================
                           INTERNAL TYPE DEFINITIONS
===============================================================================
*/

/** 
 * SLEEPSTATS_INITIAL_CYCLE_COUNT
 * 
 * @brief Initial number of sleep cycle statistics to gather before making
 *        any latency adjustments
 *        Note that this value must be less than the final cycle count
 */
#define SLEEPSTATS_INITIAL_CYCLE_COUNT  0x00000020

/** 
 * SLEEPSTATS_FINAL_CYCLE_COUNT
 * 
 * @brief Maximum statistics to gater count value. Adjustments will be made 
 *        after every x number of cycles once this value is reached.
 *        Note that this value must be greater than the initial cycle count   
 */
#define SLEEPSTATS_FINAL_CYCLE_COUNT    0x00000800

/** 
 * sleepStats_os_overhead
 * 
 * @brief This struct contains pc enter and exit overhead for previous
 *        PC operation
 */
typedef struct sleepStats_os_overhead_s
{
  uint32  count;
  uint64  enter;
  uint64  exit;
} sleepStats_os_overhead;

/**
 * RPM_master_stats
 *
 * @brief RPM data fields for shutdown & wakeup
 */ 
typedef struct RPM_master_stats_s
{
  /* Bitmask of active cores - core 0 = bit 0 */
  uint32 active_cores;  

  /* number of times all cores have power collapsed */
  uint32 num_shutdowns; 

  /* timestamp of last shutdown of core */
  uint64 shutdown_req;  

  /* timestamp of last master wakeup ind  */
  uint64 wakeup_ind; 
  
  /* timestamp of last bring up req  */   
  uint64 bringup_req;

  /* timestamp of last bring up ack */
  uint64 bringup_ack;

  /* 0=scheduled, 1=rude */
  uint32 wakeup_reason; 

  /* time taken (in ticks) for the last W->S transition */
  uint32 last_sleep_transition_duration;

  /* time taken (in ticks) for the last S->W transition */
  uint32 last_wake_transition_duration; 

  uint32 reserved[19];
}RPM_master_stats;

/**
 * sleepStats_synth_s
 *
 * @brief Time statistics data structure for synth modes
 *        For max, min and total:
 *         Negative values indicate an early value
 *         Positive values indicate a late value
 */ 
typedef struct sleepStats_synth_s
{
  int32   max;        /* Maximum time value of given statistic duration */
  int32   min;        /* Minimum time value of given statistic duration */
  int64   total;      /* Running total of time statistics */
  uint32  count;      /* Number of data points gathered for statistics */
  uint32  adjustCnt;  /* Number of data points required before (any) action 
                         is taken*/
}sleepStats_synth;

/**
 * sleepStats_lprm_s
 *
 * @brief Time statistics data structure for LPRM's
 */
typedef struct sleepStats_lprm_s
{
  uint64  running_time;  /* Time measurement total execution time */
  uint32  max_lat;       /* Time measurement maximum value */
  uint32  min_lat;       /* Time measurement minimum value*/
}sleepStats_lprm;

/**
 * sleepStatsInput_s
 *
 * @brief Values required to record and adjust sleep statistics
 */
typedef struct sleepStats_input_s
{
  /* Currently selected synth mode to enter */
  CLprm *mode;    
  
  /* Frequency lookup table pointer for current frequency
   * (from sleep_lookup_table_get_entry) */
  sleep_freq_lookup_table *pfreq_LUT;

  /* Wakeup timestamp of master (from SPM bringup req, or qtmr) */
  uint64 actual_wakeup_time;

  /* Wakeup reason (scheduled, unscheduled) */
  sleepStats_wakeup_reason master_wakeup_reason;

  /* interrupt VID value at master wakeup */
  uint32 master_interrupt;

  /* Expected sleep exit timestamp */
  uint64 hard_deadline;

  /* Sleep exit timestamp from STM */
  uint64 sleep_exit_stm;

  /* Expected master wakeup timestamp */
  uint64 backoff_deadline;

  /* OS overhead from last power collapse */
  sleepStats_os_overhead  os_overhead;
}sleepStats_input;


/*
===============================================================================
                              EXTERN DECLARATIONS
===============================================================================
*/

/**
 * sleepStats_init
 *
 * @brief Initializes statistic subsystem
 * 
 */
void sleepStats_init(void);

/**
 * sleepStats_getRpmDataPtr
 *
 * @brief Returns the RPM shared memory statistics location
 * 
 */
RPM_master_stats *sleepStats_getRpmDataPtr(void);

/**
 * sleepStats_addStatsElem
 *
 * @brief Adds a lprm & synth mode wakeup statistic element 
 *  
 * @param hCLprm synth mode pointer
 */
void sleepStats_addStatsElem(CLprm *hCLprm);

/**
 * sleepStats_addPowerCollapseOverheadElem
 *
 * @brief Adds a OS overhead statistic element 
 */
void sleepStats_addPowerCollapseOverheadElem(void);

/**
 * sleepStats_initLprmStatData
 *
 * @brief Initializes Synth & LPR mode statistic memory 
 *
 * @param hCLprm synth mode pointer
 */
void sleepStats_initLprmStatData(CLprm *hCLprm );

/**
 * sleepStats_logWakeup
 *
 * @brief Main sleep statistics funtion that will log wakeup data and adjust 
 *        the latency of synth modes if necessary.
 *  
 * @param statInput: Required data input for logging and adjustments 
 */
void sleepStats_logWakeup(sleepStats_input *statInput);

/**
 * @brief Gets special LPRM time stamp values that were previously put.
 *
 * @param type: ENUM time data type
 * 
 * @return Time value requested
 */
uint64 sleepStats_getLprTimeData(sleepStats_timedata_type type);

/**
 * @brief Gets special values that were previously put.
 *
 * @param type: ENUM time data type
 * 
 * @return Data value requested
 */
uint32 sleepStats_getMiscData(sleepStats_misc_type type);

/**
 * @brief Gets special LPRM latency values that were previously put.
 *        
 * @param type: ENUM latency type 
 *  
 * @return Latency value requested
 */
uint32 sleepStats_getLastLprLatency(sleepStats_latency_type type);


void sleepStats_updateGeneric(sleepStats_generic *stats, uint64 value);

#endif //SLEEP_STATSI_H
