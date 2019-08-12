#ifndef TZBSP_TIMER_H
#define TZBSP_TIMER_H

/**
@file tzbsp_timer.h
@brief Provides API to install and control secure interrupts.
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_timer.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
03/06/11   tk       Initial version.

===========================================================================*/
#include <com_dtypes.h>
#include "tzbsp_isr.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

typedef enum
{
  TZBSP_TIMER_SECURE        = 0, /**< CPU specific secure CP15 QTimer. */
  TZBSP_TIMER_SECURE_COMMON = 1, /**< Common secure QTimer. */
} tzbsp_timer_et;

/* Watchdog Interrupt IDs */
typedef enum
{
  TZBSP_WDOG_INT_SEC = 0,
  TZBSP_WDOG_INT_NON_SEC
} tzbsp_wdog_int_id;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Converts milliseconds to microseconds.
 */
static inline uint64 tzbsp_ms_to_us(uint64 ms)
{
  return ms * 1000ULL;
}

/**
 * Converts microseconds to milliseconds.
 */
static inline uint64 tzbsp_us_to_ms(uint64 us)
{
  return us / 1000ULL;
}

/**
 * Starts timer, the timer will expire once.
 *
 * @param [in] tid      Timer identifier.
 * @param [in] timeout  Timer expiration time in microseconds.
 */
void tzbsp_gpt_one_shot_start(tzbsp_timer_et tid, uint64 timeout);

/**
 * Stops timer.
 *
 * @param [in] tid      Timer identifier.
 */
void tzbsp_gpt_stop(tzbsp_timer_et tid);

/**
 * Returns the current timer count.
 *
 * @param [in] tid      Timer identifier.
 *
 * @return Timer count in microseconds.
 */
uint64 tzbsp_gpt_count(tzbsp_timer_et tid);

/**
 * Installs interrupt handler for timer interrupt.
 *
 * @param [in] tid      Timer identifier.
 * @param [in] fn   ISR function.
 * @param [in] ctx  Context associated with the ISR function, can be \c NULL.
 *
 * @return E_SUCCESS on success, an error code otherwise.
 */
int tzbsp_gpt_install_isr(tzbsp_timer_et tid, void* (*fn) (void*), void* ctx);

/**
 * Enables timer interrupt on interrupt controller.
 *
 * @param [in] tid      Timer identifier.
 */
void tzbsp_gpt_enable_int(tzbsp_timer_et tid);

/**
 * Disables timer interrupt on interrupt controller.
 *
 * @param [in] tid      Timer identifier.
 */
void tzbsp_gpt_disable_int(tzbsp_timer_et tid);

/**
 * Starts secure watchdog timer. Only the watchdog bark can be handled, i.e.
 * whenever the associated ISR in entered, the handling is for watchdog bark.
 * If the watchdog bites, then the devices is reset. If bite timeout is less or
 * equal to bark timeout, then the expiring timer will reset the device without
 * ever hitting the associated ISR.
 *
 * @param [in] bark_timeout  Bark expiration time in milliseconds.
 * @param [in] bite_timeout  Bite expiration time in milliseconds.
 */
void tzbsp_wdt_start(uint32 bark_timeout, uint32 bite_timeout);

/**
 * Stops secure watchdog timer.
 */
void tzbsp_wdt_stop(void);

/**
 * Pets the secure watchdog.
 */
void tzbsp_wdt_pet(void);

/**
 * Returns the current secure watchdog count.
 *
 * @return Watchdog count in milliseconds.
 */
uint32 tzbsp_wdt_count(void);

/**
 * Installs interrupt handler for watchdog interrupt.
 *
 * @param [in] wid  One of \c TZBSP_WDOG_INT* IDs.
 * @param [in] fn   ISR function.
 * @param [in] ctx  Context associated with the ISR function, can be \c NULL.
 *
 * @return E_SUCCESS on success, an error code otherwise.
 */
int tzbsp_wdt_install_isr(tzbsp_wdog_int_id wid, void* (*fn) (void*), void* ctx);

/**
 * Enables watchdog interrupt on interrupt controller.
 *
 * @param [in] wid  One of \c TZBSP_WDOG_INT* IDs.
 */
void tzbsp_wdt_enable_int(tzbsp_wdog_int_id wid);

/**
 * Disables watchdog interrupt on interrupt controller.
 *
 * @param [in] wid  One of \c TZBSP_WDOG_INT* IDs.
 */
void tzbsp_wdt_disable_int(tzbsp_wdog_int_id wid);

/**
 * Retrieves the raw current value of the uptime counter.
 *
 * @note Not re-entrant.
 *
 * @return The the raw current value of the uptime counter.
 */
uint64 tzbsp_get_uptime_count_raw(void);

/**
 * Retrieves the urrent value of the uptime counter.
 *
 * @note Not re-entrant.
 *
 * @return The the current value of the uptime counter in
 *         microseconds.
 */
uint64 tzbsp_get_uptime_count(void);

#endif /* TZBSP_TIMER_H */
