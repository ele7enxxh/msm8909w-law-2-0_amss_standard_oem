#ifndef TZBSP_LOG_H
#define TZBSP_LOG_H

/**
@file tzbsp_log.h
@brief Trustzone Logging

Contains Macros for logging TZ debug messages. By default all logs go to the
internal ring buffer. Logs can also be dumped to JTAG based oncompile time flag
\c TZBSP_JTAG_LOGGING.

*/
/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/inc/hyp_log.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
12/16/12   mg       Support new log buffer management
04/29/10   ssm      Created

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stdarg.h> /* for va_list */

/*=========================================================================== 
    Preprocessor Definitions and Constants
============================================================================*/
/* HYP Diag Magic number */
#define HYP_DIAG_MAGIC_NUM   0x6879706D /* 'h', 'y', 'p', 'm' */

#define HYP_MSG_LOW      (0)
#define HYP_MSG_MED      (1)
#define HYP_MSG_HIGH     (2)
#define HYP_MSG_ERROR    (3)
#define HYP_MSG_FATAL    (4)
#define HYP_MSG_DEBUG    (5)

/**
 * Hypervisor log macro to record debug messages to a ring buffer or a term window.
 * JTAG dumping of logs is controlled by compile time flag \c TZBSP_JTAG_LOGGING.
 *
 * @param [in] xx_prio - Priority of the message to be logge, one of the
 * @param [in] xx_fmt  - Format string for output message.
 * @param [in] ...     - Arguments to format string.
 */
#define HYP_LOG(xx_prio, xx_fmt, ...)                                   \
  do {                                                                  \
    hyp_log(xx_prio, xx_fmt, ##__VA_ARGS__);                            \
  } while (0)

#define HYP_LOG_ERR(xx_fmt, ...) \
  HYP_LOG(HYP_MSG_ERROR, xx_fmt, ##__VA_ARGS__)

/* Logs a code. */
#define HYP_LOG_CODE(xx_prio, xx) \
  HYP_LOG(xx_prio, "(%u)", xx)

/**
 * Shortcut macro for conditinally logging an error, log is only printed if
 * error condition is present in \c pred.
 */
#define HYP_LOG_ERR_E(pred, xx_fmt, ...)                                 \
  if(E_SUCCESS != (pred))                                                \
  {                                                                      \
    HYP_LOG(HYP_MSG_ERROR, xx_fmt, ##__VA_ARGS__);                       \
  }

/**
 * The mutex structure used to provide simultaneous access to shared resources
 */
typedef struct
{
  volatile uint32 lock;
} hyp_mutex_t;
/*===========================================================================

          FUNCTIONS

============================================================================*/
void hyp_log_init(void);

void hyp_milestone(uint32 code);

void hyp_log(uint32 pri, const char* fmt, ...);



/**
 * Initialize the mutex structure. The mutex functions are aliased so that the
 * standard C libary will use the below implementation.
 *
 * @returns non-zero to tell the ARM C libraries that we are running
 *          multithreaded code.
 */
uint64 hyp_mutex_init(hyp_mutex_t *mutex);
/**
 * Perform a spinlock on the mutex
 */
void hyp_mutex_lock(hyp_mutex_t *mutex);
/**
 * Unlock the mutex
 */
void hyp_mutex_unlock(hyp_mutex_t *mutex);


/**
 * Disables all interrupts.
 *
 * @return The original interrupt bits from SPSR_EL2
 *
 * @see hyp_int_restore
 */
uint64 hyp_int_disable_all(void);

/**
 * Restores previously disabled interrupts.
 *
 * @param [in] flags The interrupt bits to restore to CPSR.
 *
 * @see hyp_int_disable_all
 */
void hyp_int_restore(uint32 flags);

void hyp_log_warmboot_entry(uint32 cpu);
void hyp_log_warmboot_exit(uint32 cpu);
void hyp_log_exception(uint32 ex, uint32 code);
void hyp_log_registers(uint32 *ptr);
#endif /* TZBSP_LOG_H */
