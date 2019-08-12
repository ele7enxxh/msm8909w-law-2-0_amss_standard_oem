#ifndef QSEE_LOG_H
#define QSEE_LOG_H

/**
@file qsee_log.h
@brief Trustzone qsee Logging

Contains Macros for logging TZ qsee application debug messages. By default all
logs go to the internal ring buffer. Logs can also be dumped to JTAG based
oncompile time flag \c TZBSP_JTAG_LOGGING.

*/
/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:


when       who      what, where, why
--------   ---      ------------------------------------
12/16/12   mg       Created

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <stdarg.h>
#include <comdef.h>

/*===========================================================================

                              DEFINES

===========================================================================*/

/**
 * Log ring buffer position management
 */
typedef struct qsee_log_pos_s {
    uint16 wrap;        // # of times log buffer has wrapped around
    uint16 offset;      // next offset in buffer
} qsee_log_pos_t;

/**
 * Log ring buffer management structure.
 */
typedef struct qsee_log_s {
   qsee_log_pos_t   log_pos;    // Free byte position in log buffer
   uint8            log_buf[];  // log data buffer
} qsee_log_t;

/**
 * This function is used to log to a ring buffer.
 *
 * @param [in] fmt  String describing the format of msg.
 * @param [in] ap   Variable argument list pointer.
 */
void qsee_log_with_ap(const char* fmt, ...);

/**
 * This function is used to register and initialize the ring buffer in TZ
 *
 * @param [in] buff_addr   Physical address of log buffer
 * @param [in] buff_len    Total length of log buffer, including control words
 */
void qsee_register_log_buff(void *buff_addr, uint32 buff_len);

#endif /* LOG_H */
