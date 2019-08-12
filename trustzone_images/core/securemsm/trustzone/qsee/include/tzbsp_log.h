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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_log.h#1 $
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

#include <stdarg.h>
#include <comdef.h>

/*===========================================================================

                              DEFINES

===========================================================================*/

#define TZBSP_LOG_TARGET_RING_BUFFER_BIT        (1<<0)
#define TZBSP_LOG_TARGET_JTAG_BIT               (1<<1)

#define TZBSP_MSG_LOW      (0)
#define TZBSP_MSG_MED      (1)
#define TZBSP_MSG_HIGH     (2)
#define TZBSP_MSG_ERROR    (3)
#define TZBSP_MSG_FATAL    (4)
#define TZBSP_MSG_DEBUG    (5)

/**
 * TZBSP macro to log debug messages to a ring buffer or a term window. JTAG
 * dumping of logs is controlled by compile time flag \c TZBSP_JTAG_LOGGING.
 *
 * @param [in] xx_prio - Priority of the message to be logge, one of the
 *                       constants \c TZBSP_MSG_<priority>
 * @param [in] xx_fmt  - Format string for output message.
 * @param [in] ...     - Arguments to format string.
 */
#define TZBSP_LOG(xx_prio, xx_fmt, ...)                                 \
  do {                                                                  \
    tzbsp_log(xx_prio, xx_fmt, ##__VA_ARGS__);                          \
  } while (0)

/**
 * Shortcut macro for error logging, @see TZBSP_LOG.
 */
#define TZBSP_LOG_ERR(xx_fmt, ...) \
  TZBSP_LOG(TZBSP_MSG_ERROR, xx_fmt, ##__VA_ARGS__)

/**
 * Shortcut macro for conditinally logging an error, log is only printed if
 * error condition is present in \c pred.
 */
#define TZBSP_LOG_ERR_E(pred, xx_fmt, ...)                              \
  if(E_SUCCESS != (pred))                                               \
  {                                                                     \
    TZBSP_LOG(TZBSP_MSG_ERROR, xx_fmt, ##__VA_ARGS__);                  \
  }

/**
 * Shortcut macro for debug logging, @see TZBSP_LOG.
 */
#define TZBSP_LOG_DBG(xx_fmt, ...) \
  TZBSP_LOG(TZBSP_MSG_DEBUG, xx_fmt, ##__VA_ARGS__)

/**
 * This function is used to log to a ring buffer. The function is intlocked,
 * hence printing from ISRs is allowed.
 *
 * @param [in] pri  Prioriy of message to be logged.
 * @param [in] fmt  String describing the format of msg.
 * @param [in] ...  Variable argument list.
 */
void tzbsp_log(uint32 pri, const char* fmt, ...);

/**
 * This function is used to log to a ring buffer. The function is intlocked,
 * hence printing from ISRs is allowed.
 *
 * @param [in] pri  Prioriy of message to be logged.
 * @param [in] fmt  String describing the format of msg.
 * @param [in] ap   Variable argument list pointer.
 */
void tzbsp_log_with_ap(uint32 pri, const char* fmt, va_list ap);

/**
 * This function is used to log to a ring buffer. The function is intlocked,
 * hence printing from ISRs is allowed.
 *
 * @param [in] fmt          String describing the format of msg.
 * @param [in] ap           Variable argument list pointer.
 * @param [in] log          Log structure
 * @param [in,out] log_pos  Next free position in log buffer
 * @param [in] log_len      Log ring buffer length
 */
void tzbsp_log_to_buffer(const char* fmt, va_list ap, void* log, void* log_pos,
                         uint32 log_len);

/**
 * Clears the log ring buffer.
 */
void tzbsp_log_clear(void);

/**
 * Initializes the logging module.  If this isn't called during boot, then all
 * logging functions (e.g. TZBSP_LOG) effectively become NOPs.
 */
void tzbsp_log_init(void);

#endif /* TZBSP_LOG_H */
