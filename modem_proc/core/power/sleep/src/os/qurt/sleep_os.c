/*==============================================================================
  FILE:         sleep_os.c

  OVERVIEW:     This file provides the sleep task and OS-specific
                functionality for the processors with Qurt running on them.

  DEPENDENCIES: sleep_task() is the entry point to the lowest-priority thread
                processing.

                Copyright (c) 2011-2015 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/os/qurt/sleep_os.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "rcinit.h"
#include "qurt.h"
#include "npa.h"
#include "timer.h"
#include "CoreTime.h"
#include "CoreVerify.h"
#include "q6protos.h"
#include "sleep_config.h"
#include "CoreConfig.h"
#include "CoreIni.h"
#include "sleep_npa_scheduler.h"
#include "sleep_v.h"
#include "sleep_idle_plugin.h"
#include "sleepi.h"
#include "sleep_target.h"
#include "sleep_lpr.h"
#include "sleep_log.h"
#include "sleep_solver.h"
#include "lookup_table_solver.h"
#include "rpmclient.h"
#include "sleep_stats.h"
#include "sleep_statsi.h"
#include "LprTypes.h"
#include "DDIClock.h"

extern void sleep_holdoff_reg_rcinit_comp_event( void );
/* ==================================================================
                            DEFINES
   ================================================================== */

/* On QDSP6 V4 and above, the L2 Interrupt Controller is attached to INT31
   of QDSP6'es IPEND register. All User configurable interrupts in the system
   are attached ot L2 VIC. This makes it sufficient to check for L2 MSB */

#define IPEND_IS_BIT_SET(ipend_status, bit_pos) (ipend_status) & (1 << bit_pos)

/**
 * MAX_INI_PATH_LEN
 *
 * @brief Max number of characters in the full path to the sleep
 *        ini file.
 */
#define MAX_INI_PATH_LEN           64

/* ==================================================================
                      FORWARD DECLARATIONS
   ================================================================== */
uint32 safe_unsigned_truncate(uint64 a);
uint64 safe_unsigned_addition(uint64 a, uint64 b);
static void sleep_create_in_stm_client_handle_cb( void *context,
                                                  unsigned int event_type,
                                                  void *data,
                                                  unsigned int data_size );

extern CLprRegistry *gLprRegistry;

/* ----------------------- Debug code start ---------------- */
extern uint32 __sleep_in_stm;
/* ----------------------- Debug code end ------------------ */

/* ==================================================================
                        GLOBAL VARIABLES
   ================================================================== */

/**
 * @brief sleep_cfg
 * 
 * Returned by sleep_target_init()
 *
 */
static sleep_target_config* sleep_cfg;

/**
 *
 * @brief gSleepWakeupNode
 *
 * Handle used to query the /core/cpu/wakeup node.
 *
*/
static npa_query_handle gSleepWakeupNode;

/**
 *
 * @brief gSleepMaxDurationNode
 *
 * Handle used to query the /sleep/max_duration node.
 *
*/
static npa_query_handle gSleepMaxDurationNode;

/**
 *
 * @brief gSleepLatencyNode
 *
 * Handle used to query the /core/cpu/latency node.
 *
*/
static npa_query_handle gSleepLatencyNode;

/**
 *
 * @brief gSleepLprNode
 *
 * Handle used to query the /sleep/lpr node.
 *
*/
static npa_query_handle gSleepLprNode;

/**
 * @brief gSleepInStmHandle
 *
 * Client handle used for /npa/in_stm
 *
 */
static npa_client_handle gSleepInStmHandle;

/**
 *
 * @brief sleep_allow_low_power_modes
 *
 * Boolean that indicates whether we should enter any low power modes
 * or not.  This is meant to be used by developers, and set
 * through the debugger, for disabling all low power modes from
 * occurring.
 *
*/
#ifdef FEATURE_DISABLE_SLEEP_MODES
volatile boolean sleep_allow_low_power_modes = FALSE;
#else
volatile boolean sleep_allow_low_power_modes = TRUE;
#endif

/**
 * @brief gSleepDalClkHandle
 *
 * DAL Client of Clock driver for requesting cpu frequency during Sleep.
 */
static DalDeviceHandle *gSleepDalClkHandle = NULL;

/**
 *  @brief gSleepCLprmNotification
 *
 * The CLprm to notify if the sleep duration changes.  This
 * duration could be updated by an enter function.
 */
CLprm *gSleepCLprmNotification;

/**
 *  @brief gSleepHardDeadline
 *
 * The hard deadline (expected sleep exit time) that is
 * currently being used by the idle code, in absolute ticks.  It
 * only makes sense to query this value during the idle context
 * (i.e., from an LPR).
 */
static uint64 gSleepHardDeadline;

/**
 * @brief gLastTimerDeadline
 *
 * The deadline (in absolute sclks) of the last scheduled timer
 * wakeup that was calcaulated by sleep.
 */
static uint64 gLastTimerDeadline;

/**
 * @brief gSleepCycleID
 *
 * Indicates a particular sleep cycle. It is rather simply a counter that
 * is incremented every time latency and power function parameters are
 * changed (as of now just duration)
 */
static uint32 gSleepCycleID = 0;

/**
 * @brief sleep_idle_entry_ptr
 *
 * Pointer to use as the entry point when the system is idle.
 * This pointer is controlled by the /sleep/idle/plugin node.
 */
static sleep_idle_entry_ptr_type sleep_idle_entry_ptr = NULL;

/* ==================================================================
                  INTERNAL FUNCTION DEFINITIONS
   ==================================================================*/

/**
 *
 * @brief sleep_qurt_init
 *
 * Initialize the sleep subsystem.
 *
*/
static void sleep_qurt_init( void )
{
  npa_client_handle sleep_os_idle_plugin_client = NULL;

  /* Initialize the logging subsystem. */
  sleep_log_init();
  sleep_log_set_level( SLEEP_LOG_LEVEL_MAX );

  /* Initialize target-independent NPA nodes. */
  sleep_npa_init();

  /* Initialize LPR / LPRM tracking subsystem. */
  sleep_lpr_init();

  /* Initialize statistical data system */
  sleepStats_init();

  /* Set the initial solver configuration. This needs to be done before any
   * possible access to the fLUT related data to ensure that all structures
   * are initilized before access. */
  sleep_solver_set(&sleep_lookup_table_solver);

  /* Add inital entry using worst case static frequency (at index 0) */
  sleep_lookup_table_add_fLUT(NULL, SleepLPR_static_freq_list[0], gLprRegistry);
  
  /* Initialize target-dependent stuff (nodes, modes, etc.). */
  CORE_VERIFY_PTR( sleep_cfg = sleep_target_init() );

 /* Register for RCInit completion event */
  sleep_holdoff_reg_rcinit_comp_event();

  /* Try to get query handles for the resources we just created.
     These should be ready immediately, but check them anyway. */
  CORE_VERIFY_PTR( gSleepWakeupNode =
                   npa_create_query_handle( "/core/cpu/wakeup" ) );

  CORE_VERIFY_PTR(gSleepMaxDurationNode =
                  npa_create_query_handle( "/sleep/max_duration") );

  CORE_VERIFY_PTR( gSleepLatencyNode =
                   npa_create_query_handle( "/core/cpu/latency" ) );

  CORE_VERIFY_PTR( gSleepLprNode =
                   npa_create_query_handle( "/sleep/lpr" ) );

  /* Register the time that the sleep code takes with the idle plugin node,
   * so it can decide whether to do SWFI-only based on the latency
   * restrictions */
  CORE_VERIFY_PTR( sleep_os_idle_plugin_client =
                   npa_create_sync_client(
                                "/sleep/idle/plugin",
                                "SleepOsLatencyClient",
                                SLEEP_IDLE_PLUGIN_CLIENT_MIN_SLEEP_LATENCY ) );

  /* Issue number of sclks the sleep code takes to execute.  This is the
   * worst-case number seen in the logs. */
  npa_issue_required_request( sleep_os_idle_plugin_client,
                              sleep_cfg->entry_ticks + 
                              sleep_cfg->exit_ticks );

  /* Attaching Dal handle to clock driver */
  CORE_DAL_VERIFY( DAL_DeviceAttach( DALDEVICEID_CLOCK, &gSleepDalClkHandle ) );

  npa_resource_available_cb( "/npa/in_stm",
                             sleep_create_in_stm_client_handle_cb, NULL );

}

/**
 * @brief Sets max frequency at current voltage when executing sleep.
 *
 * This function sets the max frequency for current voltage level when 
 * executing the sleep solver and executing low power modes.
 *
 * @return Returns the new/updated frequency in KHz.
 */
static uint32 sleep_os_set_freq( void )
{
  uint32 curr_cpu_freq_hz;
  uint32 curr_cpu_freq_khz;
  bool bRequired = TRUE;

  CORE_VERIFY_PTR( gSleepDalClkHandle );

  CORE_DAL_VERIFY( 
    DalClock_SetCPUMaxFrequencyAtCurrentVoltage( gSleepDalClkHandle, bRequired,
                                                 &curr_cpu_freq_hz ) 
  );

  curr_cpu_freq_khz = curr_cpu_freq_hz/1000;
  
  sleep_log_printf( SLEEP_LOG_LEVEL_INFO, 1, 
                    "Sleep CPU frequency set (%u Khz)", curr_cpu_freq_khz );

  return curr_cpu_freq_khz;
}

/**
 * @brief Releasing Sleep request on CPU clock
 */
static void sleep_os_unset_freq(void)
{
  uint32 curr_cpu_freq_hz;
  bool bRequired = FALSE;

  CORE_VERIFY_PTR( gSleepDalClkHandle );

  CORE_DAL_VERIFY(
    DalClock_SetCPUMaxFrequencyAtCurrentVoltage( gSleepDalClkHandle, bRequired,
                                                 &curr_cpu_freq_hz )
  );
}

/**
 * @brief Suspend Sleep until processor is idle
 *
 * @note  This call prepares for power collapse and masks the 
 *        global interrupt if HW threads are all wait
 *
 */
static void sleep_os_wait_for_idle(void)
{
  qurt_power_wait_for_idle();
}

/**
 * @brief Execute Halt until interrupt
 *
 */
static void sleep_os_enter_idle(void)
{
  /* Unmask global interrupt which was masked in
   * call to sleep_os_wait_for_idle() and places 
   * sleep task in ready queue. 
   * Returns when any other thread is awakened */
  qurt_power_wait_for_active();
}

/**
 * @ sleep_os_get_pending_int()
 *
 */
unsigned int sleep_os_get_pending_int(void)
{
  return 0;
}

/**
 *
 * @brief sleep_os_get_durations
 *
 * Get the hard duration, soft duration, and latency budget to
 * be used for idle processing.
 *
 * @param *hard_duration:  [output] Buffer to write the hard
 *                         duration.
 * @param *soft_duration:  [output] Buffer to write the soft
 *                         duration.
 * @param *latency_budget: [output] Buffer to write the latency
 *                         budget.
 *
*/
static void sleep_os_get_durations( uint64 *hard_duration,
                                    uint32 *soft_duration,
                                    uint32 *latency_budget )
{
  npa_query_type qres;
    
  /* Query the hard wake-up time using the os abstraction function.
   * NPA queries only support 32 bit return values for scalar resources,
   * We can switch back to using NPA once NPA provide 64-bit support */
  *hard_duration = sleep_os_get_hard_wakeup_time();

  /* Obtain the soft wake-up time by querying max duration node and wakeup 
   * node, and getting the minimum amongst them */
  *soft_duration = sleep_os_get_soft_wakeup_time();

  /* Get the latency budget from the latency node.  The latency node will
   * return the minimum latency budget being requested by all clients.
   * This lets us know how quickly we need to respond to an interrupt when
   * it occurs. */
  CORE_VERIFY( NPA_QUERY_SUCCESS == npa_query( gSleepLatencyNode,
                                               NPA_QUERY_CURRENT_STATE,
                                               &qres ) );

  *latency_budget = (uint32)qres.data.value;
  
}

/**
 *
 * @brief sleep_os_not_enough_time
 *
 * Determine whether there is enough time to perform the entire
 * sleep processing or not.
 *
 * @param min_duration:   The expected sleep duration.
 * @param latency_budget: The latency budget for this sleep
 *                        cycle.
 *
 * @return TRUE if there is not enough time for sleep, otherwise
 *         FALSE.
 *
*/
static boolean sleep_os_not_enough_time( uint64 duration,
                                         uint32 latency_budget,
                                         uint64 min_enter_exit_time )
{
  if( duration <= sleep_cfg->entry_ticks )
  {
    /* By the time we enter sleep and try to set the backoff time, we'd
     * be setting it after the current match value, so it's not worth it. */
    return TRUE;
  }

  if( duration <= ( (uint64)sleep_cfg->entry_ticks +
                    (uint64)sleep_cfg->exit_ticks + 
                    min_enter_exit_time ) )
  {
    /* By the time we run through all of sleep, the timer will
     * have expired, so it's not worth it. */
    return TRUE;
  }

  return FALSE;
}

/**
 * sleep_create_in_stm_client_handle_cb
 *
 * @brief Obtains the client handle for /npa/in_stm node when it
 *        becomes available.
 */
static void sleep_create_in_stm_client_handle_cb( void *context,
                                                  unsigned int event_type,
                                                  void *data,
                                                  unsigned int data_size )
{
  CORE_VERIFY_PTR( gSleepInStmHandle =
                   npa_create_sync_client("/npa/in_stm", "sleep_in_stm_client",
                                          NPA_CLIENT_REQUIRED ) );
}

/* ==================================================================
              EXTERNAL FUNCTION DEFINITIONS
   ================================================================== */

/*
 * sleep_os_get_last_timer_deadline
 *
*/
uint64 sleep_os_get_last_timer_deadline( void )
{
  return gLastTimerDeadline;
}

/*
 * sleep_os_offset_sleep_duration
 *
*/
void sleep_os_offset_sleep_duration( uint64 offset )
{
  /* Adjust the sleep duration, so that the new duration gets passed
   * to the rest of the enter functions. */
  if( NULL != gSleepCLprmNotification )
  {
    offsetSleepDuration( gSleepCLprmNotification, offset );
  }

  /* Adjust the hard deadline as well, in order to correctly calculate
   * whether we woke up late before exiting sleep. */
  gSleepHardDeadline += offset;

  return;
}

/*
 * sleep_get_hard_deadline
*/
uint64 sleep_get_hard_deadline( void )
{
  return gSleepHardDeadline;
}

/*
 * sleep_os_get_hard_wakeup_time
 *
*/
uint64 sleep_os_get_hard_wakeup_time( void )
{
  /* Time (in ticks) until the next scheduled timer expiry */
  uint64 ticks_to_timer_expiry;

  /* Time (in ticks) until the NPA scheduler deadline */
  uint64 ticks_to_npa_expiry;

  /* Retrieve the time (in ticks) until the next scheduled timer expiry.
   * This will skip timers that are deferrable, and push the timer interrupt
   * out to fire at the next non-deferrable timer. */
  ticks_to_timer_expiry = timer_get_time_till_first_non_deferrable_timer_64();

  gLastTimerDeadline = safe_unsigned_addition(CoreTimetick_Get64(),
                                              ticks_to_timer_expiry);

  ticks_to_npa_expiry = sleep_npa_scheduler_get_duration();

  sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 2*2,
                    "Hard wakeup durations (Timer Duration: 0x%llx)"
                    " (NPA Scheduler Duration: 0x%llx)",
                    ULOG64_DATA(ticks_to_timer_expiry), 
                    ULOG64_DATA(ticks_to_npa_expiry) );

  return( MIN( ticks_to_npa_expiry, ticks_to_timer_expiry ) );
}

/*
 * sleep_os_get_soft_wakeup_time
 *
*/
uint32 sleep_os_get_soft_wakeup_time( void )
{
  npa_query_type qres;

  /* Time (in ticks) until the next expiring wakeup deadline */
  uint32 ticks_to_wakeup_expiry;

  /* Time (in ticks) until the max duration deadline */
  uint32 ticks_to_max_duration_expiry;

  /* Get the sleep duration from the wakeup node.  This will return the "soft"
   * wakeup time.  Soft wakeups are those that are hinted at by other
   * subsystems.  Soft wakeups allow us to calculate which mode we will enter,
   * so we don't waste time entering a mode that we are going to have to wake
   * up from soon anyway. */
  CORE_VERIFY( NPA_QUERY_SUCCESS == 
               npa_query( gSleepWakeupNode,
                          SLEEP_QUERY_WAKEUP_TIME,
                          &qres ) );

  ticks_to_wakeup_expiry =  qres.data.value;

  /* Get the sleep duration from the max duration node.  This will return the
   * "soft" max duration.  Soft wakeups are those that are hinted at by other
   * subsystems.  Soft wakeups allow us to calculate which mode we will enter,
   * so we don't waste time entering a mode that we are going to have to wake
   * up from soon anyway. */
  CORE_VERIFY( NPA_QUERY_SUCCESS == 
               npa_query( gSleepMaxDurationNode,
                          NPA_QUERY_CURRENT_STATE,
                          &qres ) );

  ticks_to_max_duration_expiry =  qres.data.value;

  sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 2,
                   "Soft wakeup durations (Wakeup Node Duration: 0x%08x)"
                   " (Max Node Duration: 0x%08x)",
                   ticks_to_wakeup_expiry, 
                   ticks_to_max_duration_expiry );

  return( MIN( ticks_to_wakeup_expiry, ticks_to_max_duration_expiry ) );
}

/* @brief This function reverses any special requirements for sleep task
 *
 * It is designed to be called before exiting sleep_perform_lpm
 */
static void sleep_os_exit_stm(void)
{
  sleep_log_printf_qdss( SLEEP_LOG_LEVEL_PROFILING,
                         SLEEP_EARLY_EXIT_STM_NUM_ARGS,
                         SLEEP_EARLY_EXIT_STM_STR, 
                         SLEEP_EARLY_EXIT_STM);

  qurt_power_shutdown_fail_exit();
  sleep_target_enable_dog();

  if ( NULL != gSleepInStmHandle )
  {
    /* Allow NPA to trigger Event callbacks */
    npa_complete_request( gSleepInStmHandle );
  }
 
  return;
}

/*
 * sleep_os_set_idle_entry
 */
void sleep_os_set_idle_entry( uint32 core_num,
                              sleep_idle_entry_ptr_type new_idle_entry_ptr )
{
  sleep_idle_entry_ptr = new_idle_entry_ptr;
  return;
}

/*
 * sleep_swfi
 */
uint32 sleep_swfi( void )
{
  /* Wait for all other HW threads to go idle. */
  qurt_power_wait_for_idle();

  /* Since we are in this function, sleep idle plugin was set to swfi only
   * when sleep task was last scheduled. Make sure the idle plugin has not 
   * changed since then. If changed, simply return. Else we can proceed
   * with wait_for_active().
   *
   * There is a chance an interrupt could come in after checking and change 
   * idle plugin state but the window is very small and we are relying on 
   * deferrable timers (e.g. dog that are present on all non hloses) which
   * will hopefully prevent target to stay in swfi indefinitely. */
  if ( sleep_swfi == sleep_idle_entry_ptr )
  {
    /* Wait for a thread to go active. */
    sleep_os_enter_idle();
  }
  else
  {
    /* Unmask global interrupt which was masked in
     * call to sleep_os_wait_for_idle()*/
    qurt_power_shutdown_fail_exit();
  }

  return 0;
}

/**
 * @brief This is the main driver loop for sleep task
 *
 */
uint32 sleep_perform_lpm(void)
{
  sleep_solver_input  solver_input;
  sleep_solver_output solver_output;
  npa_query_type      qres;
  uint64              sleep_start_dt;
  uint64              min_duration;
  uint32              pending_ints, num_enabled_lprs, min_latency;
  uint64              sleep_duration;
  uint64              hard_duration, gSleepSoftDeadline;
  uint32              soft_duration;
  uint64              time_til_deadline, curr_time;
  uint32              curFrequency;
  sleepStats_input    stats;
  CLprm               *solver_selected_mode = NULL;

  /* Wait for all other HW threads to go idle. */
  sleep_os_wait_for_idle();

  /* Check to ensure that no rpm msgs are in-flight */
  /* Added here, because overhead of qurt_power_shutdown_prepare() is high */
  if( FALSE == rpm_is_mq_empty() )
  {
    sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 0,
                      "Short SWFI (reason: RPM message in flight)" );
    sleep_os_enter_idle();
    return 0;
  }

  /* Check the flag that allows low power modes */
  if( 0 == sleep_allow_low_power_modes )
  {
    sleep_os_enter_idle();
    return 0;
  }

  /* Check if QuRT can disable interrupts and enter in STM. Also ensure that
   * idle plugin has not changed. Return if any of these fails */
  if ( (sleep_perform_lpm != sleep_idle_entry_ptr) ||  
       (qurt_power_shutdown_prepare() != 0) )
  {
    /* Unmask global interrupt which was masked in
     * call to sleep_os_wait_for_idle() */
    qurt_power_shutdown_fail_exit();
    return 0;
  }

  /* Mark start of this sleep cycle */
  sleep_log_printf_qdss( SLEEP_LOG_LEVEL_PROFILING, SLEEP_ENTER_IDLE_NUM_ARGS,
                         SLEEP_ENTER_IDLE_STR, SLEEP_ENTER_IDLE );

  /* Check that rpm queue is still empty after waiting for all HW threads to
   * idle. */
  if( FALSE == rpm_is_mq_empty() )
  {
    sleep_log_qdss_event( SLEEP_EARLY_EXIT_STM_NUM_ARGS,
                          SLEEP_EARLY_EXIT_STM);

    sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 0,
                      "Short SWFI (reason: RPM message in flight)");
    qurt_power_shutdown_fail_exit();
    sleep_os_enter_idle();
    return 0;
  }

  /* Make sure that idle plugin has not changed since last checked */
  if( sleep_perform_lpm != sleep_idle_entry_ptr )
  {
    sleep_log_qdss_event( SLEEP_EARLY_EXIT_STM_NUM_ARGS,
                          SLEEP_EARLY_EXIT_STM);

    sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 0, 
                      "Sleep cycle skipped "
                      "(reason: Idle Plugin state changed)" );
    qurt_power_shutdown_fail_exit();
    return 0;
  }

  if ( NULL != gSleepInStmHandle )
  {
    /* Prevent NPA from triggering Event Callbacks */
    npa_issue_required_request( gSleepInStmHandle, 1 );
  }

  sleep_target_disable_dog();

  /* Querying sleep node for duration and latency constraints */
  sleep_os_get_durations( &hard_duration, &soft_duration,
                          &solver_input.latency_budget );

  sleep_start_dt = CoreTimetick_Get64();
  
  /* Calculate the expected wake up time to detect a late wakeup later */
  gSleepHardDeadline = safe_unsigned_addition(sleep_start_dt, hard_duration);
  gSleepSoftDeadline  = safe_unsigned_addition(sleep_start_dt, soft_duration);

  min_duration = MIN ( hard_duration, soft_duration );

  /* i) check for timer expiry
   * ii)check for pending Interrupts
   * Abandon LPM effort if either is true */
  pending_ints = qurt_system_ipend_get();
  if( min_duration == 0 || IPEND_IS_BIT_SET(pending_ints, 31) )
  {
    sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 1+(1*2),
                      "Early Exit (ints pending: 0x%08x)"
                      " (min_duration: 0x%llx)",
                      pending_ints, ULOG64_DATA(min_duration) );

    sleep_os_exit_stm();
    return 0;
  }

  /* Get the current LPR registry. */
  CORE_VERIFY(NPA_QUERY_SUCCESS == npa_query( gSleepLprNode,
                                              SLEEP_LPR_QUERY_RESOURCES,
                                              &qres ) );

  CORE_VERIFY( NPA_QUERY_TYPE_REFERENCE == qres.type );
  solver_input.lpr_registry = (CLprRegistry *)qres.data.reference;

  /* Storing sleep cycle id for caching purpose */
  if( 0 == ++gSleepCycleID )
  {
    /* Looks like roll-over happened. */
    gSleepCycleID = 1;
  }
  solver_input.cycle_id = gSleepCycleID;

  /* Set the maximum frequency at current voltage for executing solver+lprs */
  curFrequency = sleep_os_set_freq();

  /* Update frequency table pointer to match current core frequency.
   * Either an exact fLUT match or the closest fLUT will be returned */
  if(sleep_lookup_table_get_entry(curFrequency, &stats.pfreq_LUT) != TRUE) 
  {
    /* If the function returns !TRUE, we need to allocate a new fLUT and realloc
     * all of the cache & statistic arrays outside of STM */ 
    sleep_os_exit_stm();
    stats.pfreq_LUT = sleep_lookup_table_add_fLUT(stats.pfreq_LUT,
                                                  curFrequency, 
                                                  solver_input.lpr_registry);
    sleep_os_unset_freq();
    return 0;
  }

  num_enabled_lprs =
    CLprRegistry_getNumberEnabledLprs( solver_input.lpr_registry, 0 );

  min_latency = CLprRegistry_getMinEnabledModeLatency ( 
                  solver_input.lpr_registry,
                  min_duration,
                  stats.pfreq_LUT, 
                  gSleepCycleID );

  /* Perform early test for corner cases */
  if( num_enabled_lprs == 0 ||
      sleep_os_not_enough_time( min_duration,
                                solver_input.latency_budget,
                                (uint64)min_latency ) )
  {
    /* There's not enough time to do the whole sleep process.
     * Therefore, immediately SWFI until the wakeup, and then exit. */
    sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 2+(2*2),
                      "Short SWFI (hard duration: 0x%llx) "
                      "(soft duration: 0x%llx) (latency budget: 0x%08x)"
                      "(num enabled LPRs: %d)",
                      ULOG64_DATA(hard_duration), ULOG64_DATA(soft_duration),
                      solver_input.latency_budget, num_enabled_lprs );

    sleep_os_exit_stm();
    sleep_os_unset_freq();
    sleep_os_enter_idle();
    return 0;
  }

  /* Maximum number of modes solver can select if executed */
  solver_input.num_of_allowed_modes = 1;

  /* set current frequency index value for solver */
  solver_input.pfreq_LUT = stats.pfreq_LUT;

  /* Fill in solver durations to operate on.  This takes into account the
   * amount of time that's passed since we checked for the durations. */
  curr_time = CoreTimetick_Get64();
  solver_input.hard_duration = gSleepHardDeadline - curr_time;
  solver_input.soft_duration = gSleepSoftDeadline  - curr_time;

  /* Run the solver.  This will get back a list of all low power modes that
   * can be entered based on these parameters. */
  sleep_log_printf_qdss( SLEEP_LOG_LEVEL_INFO, SLEEP_ENTER_SOLVER_NUM_ARGS,
                         SLEEP_ENTER_SOLVER_STR, SLEEP_ENTER_SOLVER, 
                         curFrequency, ULOG64_DATA(solver_input.hard_duration),
                         ULOG64_DATA(solver_input.soft_duration),
                         solver_input.latency_budget );

  solver_output.selected_modes = &solver_selected_mode;
  sleep_solver( &solver_input, &solver_output);

  sleep_log_printf_qdss( SLEEP_LOG_LEVEL_INFO, SLEEP_EXIT_SOLVER_NUM_ARGS,
                         SLEEP_EXIT_SOLVER_STR, SLEEP_EXIT_SOLVER );

  if ( 0 == solver_output.num_of_selected_modes )
  {
    /* No Modes were selected:
     * Execute ALL WAIT until interrupt or timer expiry */
    sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 1+(2*2),
                      "Short SWFI (reason: no modes chosen) "
                      "(hard duration: 0x%llx) "
                      "(soft duration: 0x%llx) (latency budget: 0x%08x)",
                      ULOG64_DATA(solver_input.hard_duration),
                      ULOG64_DATA(solver_input.soft_duration),
                      solver_input.latency_budget );

    sleep_os_exit_stm();
    sleep_os_unset_freq();
    sleep_os_enter_idle();
    return 0;
  }

  /* Entering solver selected Low Power Mode(s) */
  CORE_VERIFY(solver_output.num_of_selected_modes == 1);

  /* Check for pending Interrupts:Abandon LPM effort if interrupt pending */
  pending_ints = qurt_system_ipend_get();
  if(IPEND_IS_BIT_SET(pending_ints, 31) )
  {
    sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 1,
                      "Early Exit (ints pending: 0x%08x)",
                      pending_ints );
    sleep_os_exit_stm();
    sleep_os_unset_freq();
    return 0;
  }

  /* Disable deferrable timers */
  sleep_target_defer_timers();

  /* CLprm to notify if sleep duration changes */
  gSleepCLprmNotification = solver_output.selected_modes[0];

  /* Set selected mode for later use in statistics */
  stats.mode = solver_output.selected_modes[0];

  /* Make sure there's still time to do the selected mode
   *  add 5 tick extra guardband + 
   *  Time from here to start time of first LPRM entry @ worst case frequency
   *  -> ~175 ticks
   */
  time_til_deadline = gSleepHardDeadline - CoreTimetick_Get64();
  if( ( solver_output.enter_time + solver_output.exit_time + 175 + 5 ) >= 
      time_til_deadline ) 
  {
    /* Not enough time:
     * Execute ALL WAIT until interrupt or timer expiry */
    sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 2+(1*2),
                      "Short SWFI (reason: not enough time to enter and exit) "
                      "(enter time: 0x%08x) (exit time: 0x%08x) "
                      "(time til deadline: 0x%llx)",
                      solver_output.enter_time, solver_output.exit_time,
                      ULOG64_DATA(time_til_deadline) );

    sleep_log_printf_qdss( SLEEP_LOG_LEVEL_PROFILING, 
                           SLEEP_EARLY_EXIT_STM_NUM_ARGS,
                           SLEEP_EARLY_EXIT_STM_STR, 
                           SLEEP_EARLY_EXIT_STM);

    stats.sleep_exit_stm = CoreTimetick_Get64();
    qurt_power_shutdown_fail_exit();
    sleep_target_undefer_timers();
    sleep_target_enable_dog();
    if ( NULL != gSleepInStmHandle )
    {
      /* Allow NPA to trigger Event callbacks */
      npa_complete_request( gSleepInStmHandle );
    }
    sleep_os_unset_freq();
    sleep_os_enter_idle();
    return 0;
  }

  /* Calculate the exact duration until the backed off deadline, and
   * pass that to the enter functions. */
  stats.backoff_deadline = gSleepHardDeadline - solver_output.backoff_time;
  sleep_duration = stats.backoff_deadline - CoreTimetick_Get64();

  sleep_log_printf( SLEEP_LOG_LEVEL_INFO, 1 + (3*2),
                    "Entering modes (hard deadline: 0x%llx) "
                    "(backoff deadline: 0x%llx) (backoff: 0x%x) "
                    "(sleep duration: 0x%llx)",
                    ULOG64_DATA(gSleepHardDeadline),
                    ULOG64_DATA(stats.backoff_deadline),
                    solver_output.backoff_time,
                    ULOG64_DATA(sleep_duration) );

  sleep_start_dt = CoreTimetick_Get64() - sleep_start_dt;
  CLprm_enter(solver_output.selected_modes[0], 
              stats.backoff_deadline, 
              stats.pfreq_LUT->data_idx);

  /* Exiting solver selected Low Power Mode(s) */
  sleep_log_printf(SLEEP_LOG_LEVEL_INFO, 0, "Exiting modes");

  /* Get VID register value to report last interrupt that fired */
  stats.master_interrupt = qurt_system_vid_get();

  gSleepCLprmNotification = NULL;
  CLprm_exit(solver_output.selected_modes[0], 
             stats.pfreq_LUT->data_idx);

  /* NPA scheduler can update the hard dealine as part of the enter function. 
   * Set backoff deadline to updated value while still in STM */
  stats.backoff_deadline = gSleepHardDeadline - solver_output.backoff_time;
  
  /* Sleep Exit STM */
  sleep_log_printf_qdss( SLEEP_LOG_LEVEL_PROFILING, 
                         SLEEP_EXIT_STM_NUM_ARGS,
                         SLEEP_EXIT_STM_STR, 
                         SLEEP_EXIT_STM );

  stats.sleep_exit_stm = CoreTimetick_Get64(); /* Actual sleep exit STM time */
  /* ----------------------- Debug code start ---------------- */
  __sleep_in_stm = FALSE;
  /* ----------------------- Debug code end ------------------ */
  qurt_power_shutdown_exit();
  sleep_target_undefer_timers();
  sleep_target_enable_dog();

  if ( NULL != gSleepInStmHandle )
  {
    /* Allow NPA to trigger Event callbacks */
    npa_complete_request( gSleepInStmHandle );
  }

  /* Get last OS boot statistics */
  stats.os_overhead.count = qurt_get_shutdown_pcycles(&stats.os_overhead.enter,
                                                      &stats.os_overhead.exit);

  /* Get sleep wakeup time & reason as recorded from standalone or RPM modes */ 
  stats.actual_wakeup_time = 
    sleepStats_getLprTimeData(SLEEP_STATS_TIME_WAKEUP_TYPE);

  stats.master_wakeup_reason = (sleepStats_wakeup_reason)
      sleepStats_getMiscData(SLEEP_STATS_MISC_WAKEUP_REASON_TYPE);

  /* Fill in remaining stat input data for logging */ 
  stats.hard_deadline = gSleepHardDeadline; 

  /* Log and update any statistic data */
  sleepStats_logWakeup(&stats);

  sleep_log_printf_qdss( SLEEP_LOG_LEVEL_PROFILING, 
                         SLEEP_EXIT_IDLE_NUM_ARGS, 
                         SLEEP_EXIT_IDLE_STR, 
                         SLEEP_EXIT_IDLE,
                         (uint32)sleep_start_dt,
                         ULOG64_DATA(stats.sleep_exit_stm) );
  
  sleep_os_unset_freq();
  return 0;
}

/*
 * sleep_os_create_config_handle
 *
*/
CoreConfigHandle sleep_os_create_config_handle( unsigned int core_num )
{
  char path[MAX_INI_PATH_LEN];

  /* Build the path to the sleep_config.ini file */
  snprintf( path, MAX_INI_PATH_LEN,
            "/nv/item_files/sleep/core%d/sleep_config.ini", core_num );

  return( CoreIni_ConfigCreate( path ) );

}

/*
 * sleep_os_destroy_config_handle
 *
*/
void sleep_os_destroy_config_handle( CoreConfigHandle config )
{
  CoreIni_ConfigDestroy( config );
}

/*
 * Functions below are for managing locks for the LPRMs in order to serialize
 * their enter and exit function calls during idle time. However, for single
 * core environment they are just stub function and not used as of now. We
 * have to define them as they are called in common code. Once we have
 * CoreLock, we can probably use that instead of this.
 */

/*
 * sleep_os_create_mode_fcn_lock
 */
void sleep_os_create_mode_fcn_lock( sleep_lprm *mode )
{
}

/*
 * sleep_fcn_lock
 */
void sleep_fcn_lock( sleep_mode_fn_lock mode_lock )
{
}

/*
 * sleep_fcn_unlock
 */
void sleep_fcn_unlock( sleep_mode_fn_lock mode_lock )
{
}

/**
 *
 * @brief sleep_efs_initialized
 *
 * Initialize the sleep LPR subsystem.  When this callback is
 * called, the EFS subsystem has been initialized.  We rely on
 * that in order to read the config files.
 *
 */
void sleep_efs_initialized( void )
{
  boolean sleep_config_value = FALSE;
  unsigned int ret = FALSE;

  /* Check to see if we should do any LPRMs at all. */
  ret = sleep_config_read_bool( 0, ( char * )"all_lprms", NULL,
                                ( char * )"disable", &sleep_config_value );

  if ( ret )
  {
    sleep_allow_low_power_modes = !sleep_config_value;
  }

  if(sleep_allow_low_power_modes == FALSE)
  {
    sleep_log_printf( SLEEP_LOG_LEVEL_WARNING, 0,
                      "WARNING (message: \"All sleep modes are disabled"
                      " in the global config\")" );
  }
}

/*
 * sleep_task
 *
 */
void sleep_task
(
 /* Parameter received from Main Control task - ignored */
 unsigned long int unused_param
 )
{
  /* Initialize the sleep subsystem */
  sleep_qurt_init();

  /* Start the task */
  rcinit_handshake_startup();

  /* Read NV items, etc. here to check for modes that are disabled. */
  sleep_efs_initialized();

  /* qurt_timer_sleep(sleep_cfg->init_delay_ms * 1000); */

  /* Task processing - Enter as many low power modes as possible. */
  while( 1 )
  {
    /* Call the idle entry point.  This function pointer may be modified
     * by the /sleep/idle/plugin resource. */
    if ( NULL != sleep_idle_entry_ptr )
    {
      sleep_idle_entry_ptr();
    }
  }
}
