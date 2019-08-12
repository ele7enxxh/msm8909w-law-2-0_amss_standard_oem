#ifndef _LIBSENSOR1_OSA_H_
#define _LIBSENSOR1_OSA_H_
/*=============================================================================
  @file libsensor1_osa.h

  @brief Defines the OS-specific types used in the sensor1 library.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

/*=============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/* Android defines PRIxPTR incorrectly. Fix it here */
#ifdef SNS_LA
# undef PRIxPTR
# define PRIxPTR "x"

# undef LOG_TAG
# define LOG_TAG "libsensor1"
# include <cutils/log.h>
# include <common_log.h>
# define LOG_VERBOSE(...) ALOGV_IF( (sensor1_log_level <= LOG_LEVEL_VERBOSE), __VA_ARGS__ )
# define LOG_DEBUG(...) ALOGD_IF( (sensor1_log_level <= LOG_LEVEL_DEBUG), __VA_ARGS__ )
# define LOG_INFO(...) ALOGI_IF( (sensor1_log_level <= LOG_LEVEL_INFO), __VA_ARGS__ )
# define LOG_WARN(...) ALOGW_IF( (sensor1_log_level <= LOG_LEVEL_WARN), __VA_ARGS__ )
# define LOG_ERROR(...) ALOGE_IF( (sensor1_log_level <= LOG_LEVEL_ERROR), __VA_ARGS__ )
#endif /* SNS_LA */

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

void
sensor1_osa_init_once( void );

#endif /* _LIBSENSOR1_OSA_H_ */
