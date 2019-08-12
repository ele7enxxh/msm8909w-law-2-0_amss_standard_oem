#ifndef SLEEP_STATS_H
#define SLEEP_STATS_H
/*=============================================================================

  FILE:            sleep_stats.h
  
  OVERVIEW:        Provides the functions & types to gather and adjust sleep
                   statistics
 
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                   All Rights Reserved.
                   Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/api/power/sleep/sleep_stats.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/

/*
===============================================================================
                           GLOBAL TYPE DEFINITIONS
===============================================================================
*/

typedef uint32 sleepStats_lpr_handle;

/**
 * sleepStats_generic_s
 *
 * @brief Basic time statistics data structure
 */
typedef struct sleepStats_generic_s
{
  uint32  count;  /* Time measurement count */
  uint64  total;  /* Time measurement total execution time */
  uint64  max;    /* Time measurement maximum value */
  uint64  min;    /* Time measurement minimum value*/
}sleepStats_generic;

/**
 * sleepStats_mode_stats_s
 *
 * @brief Basic statistic data for low power modes
 */
typedef struct sleepStats_mode_stats_s
{
  /* Number of ticks we were in this low power mode the last 
   * time it was entered */  
  uint64 last_mode_time;

  /* Running total of ticks we were in this low power mode */
  uint64 in_mode_time;

  /* Total number of times this mode is entered @ any frequency */
  uint32 mode_run_cycles;
}sleepStats_mode_stats;

/**
 * sleep_latency_type
 *
 * @brief Enum for LPR latency values
 *  
 * Values will be used by latency put function to record LPR additional latency 
 * times  
 */ 
typedef enum sleepStats_latency_type
{
  SLEEP_STATS_ENTER_LATENCY_TYPE = 0,
  SLEEP_STATS_EXIT_LATENCY_TYPE,
  SLEEP_STATS_BACKOFF_LATENCY_TYPE,
  SLEEP_STATS_NUM_LATENCY_TYPE
}sleepStats_latency_type;

/**
 * sleep_timestamp_type
 *
 * @brief Enum for LPR timestamp values
 *  
 * Values will be used by sleepStat_getLastLprLatency to record certain LPR 
 * times 
 */ 
typedef enum sleepStats_timedata_type
{
  SLEEP_STATS_TIME_WAKEUP_TYPE = 0,     /* Actual wakeup, from RPM or QTMR    */
  SLEEP_STATS_TIME_MSTR_SHUTDOWN_TYPE,  /* Last point in sleep code before PC */
  SLEEP_STATS_TIME_MSTR_RETURN_TYPE,    /* First point in sleep code after PC */
  SLEEP_STATS_NUM_TIMEDATA_TYPE
}sleepStats_timedata_type;

/**
 * sleepStats_misc_type
 *
 * @brief Enum for miscellaneous statistics values
 *  
 * Values will be used by sleepStat_getLastMiscData to record certain LPR events
 */ 
typedef enum sleepStats_misc_type
{
  SLEEP_STATS_MISC_WAKEUP_REASON_TYPE = 0, /* Wake reason(RPM/SA, un/scheduled*/
  SLEEP_STATS_MISC_PC_L2_MODE_TYPE,        /* Cache mode  ((non)retention */
  SLEEP_STATS_NUM_MISC_TYPE
}sleepStats_misc_type;

/**
 * sleepStats_wakeup_reason
 *
 * @brief Enum for wakeup reason
 *  
 * Values will be used by sleepStats_putMiscData to record certain LPR 
 * data. 
 *  
 * Do not change the values of these without verifying usage logic 
 */ 
typedef enum sleepStats_wakeup_reason
{
  SLEEP_STATS_WAKEUP_RPM_UNSCHEDULED = 0,   /* RPM unscheduled wakeup */
  SLEEP_STATS_WAKEUP_RPM_SCHEDULED,         /* RPM scheduled wakeup */
  SLEEP_STATS_WAKEUP_RPM_UNKNOWN,           /* RPM unknown wakeup */

  SLEEP_STATS_WAKEUP_SA_SCHEDULED,          /* Standalone scheduled wakeup */
  SLEEP_STATS_WAKEUP_SA_UNSCHEDULED         /* Standalone unscheduled wakeup */
}sleepStats_wakeup_reason;

/*
===============================================================================
                              EXTERN DECLARATIONS
===============================================================================
*/

/**
 * @brief Saves special LPRM time stamp values that will be retreived later.
 *
 * This function is intended to be called only in the enter & exit functions
 * of the LPR.
 * 
 * @param value: Time value in ticks 
 * @param type: ENUM time data type  
 */
void sleepStats_putLprTimeData(uint64 value, 
                               sleepStats_timedata_type type);

/**
 * @brief Saves the local latency value of an LPR that will be 
 *        retreived & used later in latency calculations.
 *
 * This function is intended to be called only in the enter & exit functions
 * of the LPR and should be used if the SLEEP_MODE_ATTR_NO_AUTO_LATENCY_TRACK
 * bit is set for the LPRM.
 *  
 * Each type should be called only once (data saved) per sleep cycle.
 * 
 * @param value: Local latency value in ticks 
 * @param latType: ENUM latency type
 */
void sleepStats_putLprLatency(uint32 value,
                              sleepStats_latency_type type);

/**
 * @brief Saves special values that will be retreived later.
 *
 * This function is intended to be called only in the enter & exit functions
 * of the LPR.
 * 
 * @param value: Data
 * @param type: ENUM time data type  
 */
void sleepStats_putMiscData(uint32 value, 
                            sleepStats_misc_type type);

/**
 * @brief Gets the LPRM cycle statistic data 
 *
 * @param lprHandle: LPR handle obtained from sleepStats_get_lpr_handle()
 * @param modeStats: If successful, requested data
 */
void sleepStats_get_lpr_stats(sleepStats_lpr_handle lprHandle,
                              sleepStats_mode_stats *modeStats);

/**
 * @brief Gets a LPR handle for use with sleep statistic functions
 *
 * @param lprName: LPR name 
 *                 This string can be obtained from looking in the LPR XML
 *                 configuration files or in the generated
 *                 "SleepLPR_LookupTable" data structure.  
 * @return Handle value, or 0 if error
 */
sleepStats_lpr_handle sleepStats_get_lpr_handle(const char *lprName);

/**
 * @brief Starts (or stops) the tracking of the time while in any uncached
 *        type of power collapse. 
 *  
 * @param enable: TRUE or FALSE to enable or disable tracking. 
 *                Any call to enable will reset the statistics
 */
void sleepStats_trackPowerCollapseTime(uint8 enable);

/**
 * @brief Retreives the current power collapse time statistic data structure. 
 *        A call to enable the statistic gathering must be performed first for
 *        any meaningful data to be returned. 
 *  
 * @note Only uncached power modes are reported in the statistics and both
 *       the last cycle and total times will include overhead.
 *  
 * @return Pointer to the current power collapse statistical data. 
 *         (See definition of sleepStats_mode_stats for specific data)
 */
const sleepStats_mode_stats *sleepStats_getPowerCollapseTime(void);

#endif //SLEEP_STATS_H
