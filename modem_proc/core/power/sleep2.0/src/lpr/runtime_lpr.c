#include "CoreTime.h"
#include "CoreVerify.h"
#include "sleep_lpr.h"
#include "sleepi.h"
#include "sleep_log.h"
#include "sleep_target.h"
#include "sleep_statsi.h"
#include "sleep_log.h"


/* CPU LPR ****************************************************************/

/**
 * @brief cpu_lpr_init
 * 
 */
void cpu_lpr_init( void )
{
  return;
}

/**
 * @brief 
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void cpu_swfi_runtime_enter( uint64_t wakeup_tick )
{

}

/**
 * @brief 
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void cpu_apcr_runtime_enter( uint64_t wakeup_tick )
{

}

/**
 * @brief cpu_swfi_runtime_exit
 *
 * Will be called when exiting 
 */
void cpu_swfi_runtime_exit( void )
{
  return;
}

/**
 * @brief cpu_apcr_runtime_exit
 *
 * Will be called when exiting 
 */
void cpu_apcr_runtime_exit( void )
{
  return;
}

/* L2 LPR ****************************************************************/
/**
 * @brief 
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void l2_runtime_noret_enter( uint64_t wakeup_tick )
{

}

/**
 * @brief 
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void l2_runtime_ret_enter( uint64_t wakeup_tick )
{

}

/**
 * @brief l2_runtime_exit
 *
 * Will be called when exiting 
 */
void l2_runtime_exit( void )
{
  return;
}

/* L2 LPR ****************************************************************/
/**
 * @brief 
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void tcm_runtime_noret_enter( uint64_t wakeup_tick )
{

}

/**
 * @brief 
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void tcm_runtime_ret_enter( uint64_t wakeup_tick )
{

}

/**
 * @brief tcm_runtime_exit
 *
 * Will be called when exiting 
 */
void tcm_runtime_exit( void )
{
  return;
}
