#ifndef QSEE_MACCHIATO_ENV_H_
#define QSEE_MACCHIATO_ENV_H_

/** @file macchiato.h
 * @brief
 * This file contains the definitions of the constants, data structures
 * and interfaces to the QSEE Macchiato key provisioning lib.
 */

/*===========================================================================
 Copyright (c) 2014 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/15   cz       Added more log types
11/20/14   qxu      Initial Version.

===========================================================================*/
#include <stdint.h>
#include "tzbsp_log.h"

#define MACC_LOG_MSG_LOW   TZBSP_MSG_LOW
#define MACC_LOG_MSG_MED   TZBSP_MSG_MED
#define MACC_LOG_MSG_HIGH  TZBSP_MSG_HIGH
#define MACC_LOG_MSG_ERROR TZBSP_MSG_ERROR
#define MACC_LOG_MSG_FATAL TZBSP_MSG_FATAL
#define MACC_LOG_MSG_DEBUG TZBSP_MSG_DEBUG

//#define MACC_LOG(xx_prio, xx_fmt, ...) qsee_log(xx_prio, xx_fmt, ...)
#define MACC_LOG tzbsp_log
#define MACC_APP_LOG qsee_log


#endif /* QSEE_MACCHIATO_ENV_H_ */
