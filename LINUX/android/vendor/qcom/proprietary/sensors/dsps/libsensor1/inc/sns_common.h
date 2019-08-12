#ifndef SNS_COMMON_H
#define SNS_COMMON_H

/*============================================================================

  @file sns_common.h

  @brief
  This file contains common definitions for Sensors framework

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sensor1.h"
#include "sns_osa.h"
#include <stdlib.h>
#include <stdbool.h>
#include "cutils/log.h"
#include <common_log.h>

/*============================================================================
                                  Typedefs
============================================================================*/
/*
 * The error code definition within the sensor framework
 */
typedef enum {
  SNS_SUCCESS   = 0,
  SNS_ERR_BUFFER,       /* Value in the buffer is wrong */
  SNS_ERR_NOMEM,        /* Insufficient memory to process the request */
  SNS_ERR_INVALID_HNDL, /* Invalid handle */
  SNS_ERR_UNKNOWN,      /* Unknown error */
  SNS_ERR_FAILED,       /* Failure in general */
  SNS_ERR_NOTALLOWED,   /* Not allowed operation */
  SNS_ERR_BAD_PARM,     /* One or more parameters have invalid value */
  SNS_ERR_BAD_PTR,      /* Invalid pointer. This may be due to NULL values */
  SNS_ERR_BAD_MSG_ID,   /* The message ID is not supported */
  SNS_ERR_BAD_MSG_SIZE, /* The message size of the message ID does not match */
  SNS_ERR_WOULDBLOCK,   /* Indicating the data is tentatively unavailable */
  SNS_ERR_NOTSUPPORTED  /* Indicating that API is not supported */
} sns_err_code_e;

/*============================================================================
                       MACRO definition
============================================================================*/
/*
 * Define SNS_ASSERT related macro definitions
 */
#if defined(SNS_LA)
# define SNS_ASSERT(value) if((value)!=true)                          \
  {                                                                   \
    while (1)                                                         \
    {                                                                 \
      LOGE("!!!SNS ASSERT:%s!!!File %s line %d",                      \
           #value, __FILE__, (int)__LINE__);                          \
      abort();                                                        \
    }                                                                 \
  }
#endif

#ifndef UNREFERENCED_PARAMETER
# define UNREFERENCED_PARAMETER(x) (void)x;
#endif /* UNREFERENCED_PARAMETER */

#endif /* SNS_COMMON_H */
