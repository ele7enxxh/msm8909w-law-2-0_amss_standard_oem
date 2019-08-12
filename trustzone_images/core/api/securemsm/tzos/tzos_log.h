#ifndef TZOS_LOG_H
#define TZOS_LOG_H

/**
@file TZOS_LOG.h
@brief Trustzone Logging

Contains Macros for logging TZ debug messages. It logs to a ring
buffer. Can also spit the log to a JTAG window based on a
runtime flag that can be set via JTAG.

Contains the public methods and related data types.

*/
/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/tzos/tzos_log.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
04/29/10   ssm      Created

===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

============================================================================*/

#include "comdef.h"


/*===========================================================================

                              PUBLIC DATA TYPES

============================================================================*/

#define TZOS_MSG_LOW      (0)
#define TZOS_MSG_MED      (1)
#define TZOS_MSG_HIGH     (2)
#define TZOS_MSG_ERROR    (3)
#define TZOS_MSG_FATAL    (4)
#define TZOS_MSG_DEBUG    (5)

extern void tzos_log(uint32 pri, char* fmt, ...);

/*!
* @brief
   Macro to log TZOS messages with variable arguments

* @details
   TZOS macro to log debug messages to a ring buffer or a term window.
   The logging to term window is controlled by tz_dbg flag

  @param[in] xx_prio - Priority of the message to be logged
                      - #define TZOS_MSG_LOW      (0)
                      - #define TZOS_MSG_MED      (1)
                      - #define TZOS_MSG_HIGH     (2)
                      - #define TZOS_MSG_ERROR    (3)
                      - #define TZOS_MSG_FATAL    (4)
                      - #define TZOS_MSG_DEBUG    (5)
  @param[in] *xx_fmt  - Format string for output message
*/
#define TZOS_LOG(xx_prio, xx_fmt, ...)                                 \
  do {                                                                 \
    tzos_log(xx_prio, #xx_fmt, ##__VA_ARGS__);                         \
  } while (0)

/*!
* @brief
   Macro to log TZOS error messages

* @details
   TZOS macro to log error messages to a ring buffer or a term window.
   The logging to term window is controlled by tz_dbg flag

  @param[in] *xx_fmt  - Format string for output message

  @dependencies
   None

  @sideeffects
   None.

  @return
   None.
*/
#define TZOS_ERR(xx_fmt, ...) \
  TZOS_LOG(TZOS_MSG_ERROR, xx_fmt, ##__VA_ARGS__)

#endif /* TZOS_LOG_H */
