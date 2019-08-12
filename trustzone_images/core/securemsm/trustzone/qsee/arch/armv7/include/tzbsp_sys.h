#ifndef TZBSP_SYS_H
#define TZBSP_SYS_H

/**
@file tzbsp_sys.h
@brief Header file for TZBSP system/CPU level functions.

@section S1 SYS Functions

The TZBSP SYS API is a collection of system level configuration and
maintenance functions, including cache invalidation and cleaning,
interrupt disabling/enabling and secure/non-secure memory area checks.

@defgroup tzbsp_sys_group TZBSP SYS Functions
@ingroup tzbsp_sys_group
@{
*/
/*===========================================================================
   Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/include/tzbsp_sys.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/24/10   tk       Initial version.
11/09/10   pre      Added performance monitoring

===========================================================================*/

/*------------------------------------------------------------------------
                            PUBLIC TYPES
------------------------------------------------------------------------*/

/*===========================================================================

                              DEFINES

===========================================================================*/
#include <comdef.h>

/**
 * @name Interrupt mask bits
 *
 * Bit patterns for interrupt enabling/disabling. The bit locations
 * match the actual CPSR bits.
 *
 * @see \c tzbsp_int_enable and \c tzbsp_int_disable.
 */
/*@{*/
#define TZBSP_EA_BIT     (1<<8) /**< External Abort mask bit */
#define TZBSP_IRQ_BIT    (1<<7) /**< IRQ mask bit */
#define TZBSP_FIQ_BIT    (1<<6) /**< FIQ mask bit */
/*@}*/

/**
 * Constants for CPSR mode bits.
 */
#define CPSR_MODE_USR           0x10
#define CPSR_MODE_FIQ           0x11
#define CPSR_MODE_IRQ           0x12
#define CPSR_MODE_SVC           0x13
#define CPSR_MODE_MON           0x16
#define CPSR_MODE_ABT           0x17
#define CPSR_MODE_UND           0x1b
#define CPSR_MODE_SYS           0x1f

#define CPSR_MODE_MASK          0x1f /* CPSR mask for CPU mode. */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
 * Set of common events that can be logged using performance monitors
 */
typedef enum
{
  TZBSP_PM_EVT_SW_INC              = 0,
  TZBSP_PM_EVT_IF_CACHE            = 1,
  TZBSP_PM_EVT_IF_TLB              = 2,
  TZBSP_PM_EVT_MEM_CACHE_REFILL    = 3,
  TZBSP_PM_EVT_MEM_CACHE_ACCESS    = 4,
  TZBSP_PM_EVT_MEM_TLB             = 5,
  TZBSP_PM_EVT_MEM_R_AEXEC         = 6,
  TZBSP_PM_EVT_MEM_W_AEXEC         = 7,
  TZBSP_PM_EVT_I_AEXEC             = 8,
  TZBSP_PM_EVT_EXCEPTION           = 9,
  TZBSP_PM_EVT_EXCEPTION_RETURN    = 10,
  TZBSP_PM_EVT_I_CONTEXTIDR        = 11,
  TZBSP_PM_EVT_SW_PC               = 12,
  TZBSP_PM_EVT_IMM_BRANCH_AEXEC    = 13,
  TZBSP_PM_EVT_RETURN_AEXEC        = 14,
  TZBSP_PM_EVT_UALIGN_MEM_I_AEXEC  = 15,
  TZBSP_PM_EVT_BRANCH_PREDICT_FAIL = 16,
  TZBSP_PM_EVT_CYCLE_CNT           = 17,
  TZBSP_PM_EVT_BRANCH_PREDICT      = 18,
  TZBSP_PM_EVT_RESERVED_0          = 19,
  TZBSP_PM_EVT_RESERVED_1          = 20,
  TZBSP_PM_EVT_RESERVED_2          = 21,
  TZBSP_PM_EVT_RESERVED_3          = 22,
  TZBSP_PM_EVT_RESERVED_4          = 23,
  TZBSP_PM_EVT_RESERVED_5          = 24,
  TZBSP_PM_EVT_RESERVED_6          = 25,
  TZBSP_PM_EVT_RESERVED_7          = 26,
  TZBSP_PM_EVT_RESERVED_8          = 27,
  TZBSP_PM_EVT_RESERVED_9          = 28,
  TZBSP_PM_EVT_RESERVED_A          = 29,
  TZBSP_PM_EVT_RESERVED_B          = 30,
  TZBSP_PM_EVT_RESERVED_C          = 31,
  TZBSP_PM_EVT_INVALID
} tzbsp_pm_evt_type;

/**
 * Branch predictor invalidate all
 */
void tzbsp_bp_invalidate_all(void);

/**
 * Enables data caching. This function does not invalidate the data
 * cache.
 */
void tzbsp_dcache_enable(void);

/**
 * Disables data caching. This function does not clean the data cache.
 */
void tzbsp_dcache_disable(void);

/**
 * @return \c TRUE if d-cache is enabled, \c FALSE if disabled.
 */
boolean tzbsp_dcache_is_enabled(void);

/**
 * Enables L1 instruction cache. This function does not invalidate the
 * instruction cache.
 */
void tzbsp_icache_enable(void);

/**
 * Disables L1 instruction cache.
 */
void tzbsp_icache_disable(void);

/**
 * @return \c TRUE if i-cache is enabled, \c FALSE if disabled.
 */
boolean tzbsp_icache_is_enabled(void);

/**
 * Enables the selected interrupts.
 *
 * @param [in] flags Bit mask for enabling interrupts. Possible values
 * are \c TZBSP_EA_BIT, \c TZBSP_IRQ_BIT and \c TZBSP_FIQ_BIT which can
 * be combined with bitwise OR, other bits are ignored.
 */
void tzbsp_int_enable(uint32 flags);

/**
 * Disables the selected interrupts.
 *
 * @param [in] flags Bit mask for disabling interrupts. Possible
 * values are \c TZBSP_EA_BIT, \c TZBSP_IRQ_BIT and \c TZBSP_FIQ_BIT
 * which can be combined with bitwise OR, other bits are ignored.
 */
void tzbsp_int_disable(uint32 flags);

/**
 * Read the current interrupt flags
 *
 * @return Return the CPSR for the following bits TZBSP_EA_BIT,
 *         TZBSP_IRQ_BIT, TZBSP_FIQ_BIT
 */
uint32 tzbsp_int_read(void);

/**
 * Disables all interrupts.
 *
 * @return The original interrupt bits from CPSR.
 *
 * @see tzbsp_int_restore
 */
uint32 tzbsp_int_disable_all(void);

/**
 * Restores previously disabled interrupts.
 *
 * @param [in] flags The interrupt bits to restore to CPSR.
 *
 * @see tzbsp_int_disable_all
 */
void tzbsp_int_restore(uint32 flags);

/**
 * Executes the Data Memory Barrier instruction (dmb).
 */
void tzbsp_dmb(void);

/**
 * Executes the Data Synchronization Barrier instruction (dsb).
 */
void tzbsp_dsb(void);

/**
 * Executes the instruction Synchronization Barrier instruction (isb).
 */
void tzbsp_isb(void);

/**
 * Returns the CPU number of the current execution context.
 *
 * @return 0 for CPU0, 1 for CPU1, ..., n for CPUn.
 */
uint32 tzbsp_cur_cpu_num(void);

/**
 * The mutex structure used to provide simultaneous access to shared resources
 */
typedef struct
{
  volatile uint32 lock;
} tzbsp_mutex_t;

/**
 * Initialize the mutex structure. The mutex functions are aliased so that the
 * standard C libary will use the below implementation.
 *
 * @returns non-zero to tell the ARM C libraries that we are running
 *          multithreaded code.
 */
uint32 tzbsp_mutex_init(tzbsp_mutex_t *mutex);

/**
 * Perform a spinlock on the mutex
 */
void tzbsp_mutex_lock(tzbsp_mutex_t *mutex);

/**
 * Unlock the mutex
 */
void tzbsp_mutex_unlock(tzbsp_mutex_t *mutex);

/**
 * Perform a spinlock on the mutex
 *
 * @note If there exists shared memory that could get corrupted
 * if an interrupt occured (from either the secure or ns side),
 * then make sure you disable FIQ's/IRQ's for a short amount of time
 * These locks are safe to share with interrupt handlers
 */
static inline void tzbsp_mutex_lock_irq(tzbsp_mutex_t *mutex, uint32 *flags)
{
  uint32 tmp = 0;

  /* TODO: Verify SCTLR.NMFI bit can be written too */
  tmp = tzbsp_int_disable_all();
  tzbsp_mutex_lock(mutex);

  if (flags != NULL) *flags = tmp;
}

/**
 * Unlock the mutex and restore interrupts
 */
static inline void tzbsp_mutex_unlock_irq(tzbsp_mutex_t *mutex, uint32 *flags)
{
  tzbsp_mutex_unlock(mutex);
  if (flags != NULL)
    tzbsp_int_restore(*flags);
  else
    /* Turn on everything */
    tzbsp_int_enable(TZBSP_EA_BIT | TZBSP_IRQ_BIT | TZBSP_FIQ_BIT);
}

/**
 * Initializes the performance monitors.  Performance monitors are
 * composed of a number of event counters and one cycle counter.  The
 * event counters and cycle counters must be enabled independently
 *
 * @returns The number of available event counters
 */
uint32 tzbsp_pm_init(void);

/**
 * @param[in] use_cycle_divisor If set to 1 then the increment the
 *                              cycle counter every 64 clock cycles;
 *                              otherwise the cycle counter is
 *                              incremented every clock cycle.
 */
void tzbsp_pm_enable_cycle_cntr(boolean use_cycle_divisor);

/**
 * Stops and clears the cycle counter.
 *
 * @param[out] cnt The last read value of the cycle counter
 *
 * @returns Whether the returned cycle counter value is valid
 */
boolean tzbsp_pm_disable_cycle_cntr(uint32 *cnt);

/**
 * Sets the cycle counter back to zero
 */
void tzbsp_pm_clr_cycle_cntr(void);

/**
 * @param[out] cnt Cycle counter value
 *
 * @returns whether the cycle counter value is valid
 */
boolean tzbsp_pm_read_cycle_cntr(uint32 *cnt);

/**
 * @returns whether the cycle counter is active
 */
boolean tzbsp_pm_is_cycle_cntr_active(void);

/**
 * Freeing an event counter does not clear the overflow or any
 * previous value.  The only way to clear an event counter is to call
 * the API that clears all the event counters.
 *
 * @sa tzbsp_pm_clr_evt_cntrs
 *
 * @param[in] cntr_uid UID of event counter to deallocate and return
 *                     to pool
 * @param[out] cnt Value of deallocated event counter
 *
 * @returns whether the event counter value is valid
 */
boolean tzbsp_pm_free_evt_cntr(uint32 cntr_uid, uint32 *cnt);

/**
 * Enabling an event counter clears the overflow for the counter, but
 * does not clear any previous value.  The only way to clear an event
 * counter is to call the API that clears all the event counters.
 *
 * @sa tzbsp_pm_clr_evt_cntrs
 *
 * @param[in] evt Type of event to enable.  Right now only common
 *                event types are supported
 * @returns -1 if no free counters are available, otherwise it returns
 *          the UID of the newly allocated counter.
 */
int32 tzbsp_pm_enable_evt_cntr(tzbsp_pm_evt_type evt);

/**
 * @param[in] cntr_uid UID of event counter to read current value of
 * @param[out] cnt Count of the given event
 *
 * @returns whether the event counter value is valid
 */
boolean tzbsp_pm_read_evt_cntr(uint32 cntr_uid, uint32 *cnt);

/**
 * @param[in] cntr_uid UID of event counter to check the state of
 * @returns Whether the given event counter is actively counting
 */
boolean tzbsp_pm_is_evt_cntr_active(uint32 cntr_uid);

/**
 * @param[in] cntr_uid UID of event counter to check the type
 *                     of event that it is logging
 * @returns The type of event being logged by the given counter
 */
tzbsp_pm_evt_type tzbsp_pm_evt_cntr_uid_to_evt(uint32 cntr_uid);

/**
 * Resets count of all available event counters.  Does not reset the
 * cycle counter.  There is no supported method of clearing just one
 * event counter.
 *
 * @warning This will reset any active event counters
 */
void tzbsp_pm_clr_evt_cntrs(void);

/**
 * Increments the SW controlled event counter.
 */
void tzbsp_pm_inc_sw_cntr(uint32 cntr_uid);

/**
 * Read the current value of TTBR0
 *
 * @return Return value in TTBR0 register
 */
uint32 rcp15_ttbr0(void);

/**
 * Write a new value in to TTBR0
 *
 * Writes TTBR0 and performs the appropriate ISB/DSB/TLBIALL operations
 */
void wcp15_ttbr0_safe(uint32);

/**
 * Write a new value in to VBAR
 *
 * Writes VBAR and performs an ISB/DSB operation
 */
void wcp15_vbar_safe(uint32);

/** Secure Configuration Register Bits */
#define SCR_IRQ_BIT             0x02    // IRQ bit
#define SCR_FIQ_BIT             0x04    // FIQ bit
#define SCR_EA_BIT              0x08    // EA  bit

/**
 * Read the current value of SCR
 *
 * @return Return value in SCR
 */
uint32 rcp15_scr(void);

/**
 * Write a new value in to SCR
 *
 * Writes SCR
 */
void wcp15_scr(uint32);

/* TODO This satisfies a crypto_rearch dependency, should be removed. */
extern void* tzbsp_malloc(uint32 size);
extern void tzbsp_free(void* ptr);

/** @} End of tzbsp_sys_group */
#endif /* TZBSP_SYS_H */
