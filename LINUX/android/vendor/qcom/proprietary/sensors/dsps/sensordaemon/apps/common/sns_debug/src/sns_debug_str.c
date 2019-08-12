/*============================================================================
@file
sns_debug_str.c

@brief
Contains main implementation of receiving and processing
debug strings on Apps processor.

Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*=====================================================================
  INCLUDE FILES
  =======================================================================*/
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define LOG_TAG "Sensors"

#include "sns_common.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"

#ifndef _WIN32
#  include <utils/Log.h>
#  include <common_log.h>
#  include <sys/system_properties.h>
#else /* ifndef _WIN32 */
#  include "sns_debug_str.tmh"
#endif /* _WIN32 */

#define BUFSZ 1024

#define DAEMON_DEBUG_PROP_NAME "debug.qualcomm.sns.daemon"

#ifndef LOGV_IF
#define LOGV_IF ALOGV_IF
#endif /* LOG_IF */
#ifndef LOGD_IF
#define LOGD_IF ALOGD_IF
#endif /* LOG_IF */
#ifndef LOGI_IF
#define LOGI_IF ALOGI_IF
#endif /* LOG_IF */
#ifndef LOGW_IF
#define LOGW_IF ALOGW_IF
#endif /* LOG_IF */
#ifndef LOGE_IF
#define LOGE_IF ALOGE_IF
#endif /* LOG_IF */

/*===========================================================================
  GLOBAL VARIABLES
  ============================================================================*/

typedef enum {
  LOG_LEVEL_ALL,
  LOG_LEVEL_VERBOSE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_DISABLED
} log_level_e;

/* Global variable to hold debug message filtering mask */
#ifdef _WIN32
static uint64_t dbg_str_mask_g=0xFFFFFFFFFFFFFFFF; //Win32: disable all messages
static log_level_e g_log_level = LOG_LEVEL_ALL;
#else
static uint64_t dbg_str_mask_g=0x2; //All bits are set are except DIAG
static log_level_e g_log_level = LOG_LEVEL_WARN;
#endif


/*===========================================================================
  FUNCTIONS
  ============================================================================*/
/*===========================================================================

  FUNCTION:   sns_debug_printf_string

  ===========================================================================*/
/*!
  @brief
  Prints out the debug string.

  @param[i] module_id         : Module id assigned by Sensor Message Router
  @param[i] priority          : Priority of the message string
  @param[i] debug_str         : Debug string
  @param[i] fmt_params_ptr*   : Pointer to format parameter structure

  @return
  No return value.
*/
/*=========================================================================*/
void sns_debug_printf_string(sns_debug_module_id_e module_id,
                             uint8_t priority,
                             const char *debug_str,
                             const debug_params_s *fmt_params_ptr)
{
  char buf[BUFSZ];
  int index = 0;
  int strsz;

  UNREFERENCED_PARAMETER(module_id);

  if ( (fmt_params_ptr == NULL) || (debug_str == NULL) ||
       (priority > SNS_MSG_ERROR) )
  {
    LOGI("Format parameters OR Debug String is NULL or Priority Incorrect");
    return;
  }

  // Implementation using printf, Can be seen using ADB LogCat
  strsz = snprintf(buf+index, BUFSZ-index,
                   "%s(%d):",
                   fmt_params_ptr->filename,fmt_params_ptr->line_num);

  if( strsz > 0 && strsz <= BUFSZ )
  {
    index = strsz;
  }
  else
  {
    LOGI("Error %d in snprintf!", strsz);
  }

  switch (fmt_params_ptr->num_params_valid)
  {
    case 0:
      snprintf(buf+index, BUFSZ-index,
               "%s",debug_str);
      break;
    case 1:
      snprintf(buf+index, BUFSZ-index,
               debug_str,fmt_params_ptr->param1);
      break;
    case 2:
      snprintf(buf+index, BUFSZ-index,
               debug_str,fmt_params_ptr->param1,fmt_params_ptr->param2);
      break;
    case 3:
      snprintf(buf+index, BUFSZ-index,
               debug_str,
               fmt_params_ptr->param1,
               fmt_params_ptr->param2,
               fmt_params_ptr->param3);
      break;
  };
  if( index > 0 )
  {
    buf[BUFSZ-1] = '\0';
    switch(priority)
    {
      case SNS_MSG_LOW:
        LOGV_IF(g_log_level <= LOG_LEVEL_VERBOSE,
                "%s",buf);
        break;

      case SNS_MSG_MEDIUM:
        LOGD_IF(g_log_level <= LOG_LEVEL_DEBUG,
                "%s",buf);
        break;

      case SNS_MSG_HIGH:
        LOGI_IF(g_log_level <= LOG_LEVEL_INFO,
                "%s",buf);
        break;

      case SNS_MSG_FATAL:
        LOGW_IF(g_log_level <= LOG_LEVEL_WARN,
                "%s",buf);
        break;

      case SNS_MSG_ERROR:
      default:
        LOGE_IF(g_log_level <= LOG_LEVEL_ERROR,
                "%s",buf);
        break;
    }; //end of switch
  }
  //Implementation using DIAG done through MACROS

} //end of function sns_debug_printf_string

/*===========================================================================

  FUNCTION:   sns_debug_set_dbg_str_mask

===========================================================================*/
/*===========================================================================*/
/*!
  @brief
  Sets the debug string mask. Essentially a copy of the mask bits to a global
  variable visible to only this file is made.

  @param[i] bit_mask         : Debug string bit mask

  @return
  No return value.
*/
/*=========================================================================*/
void sns_debug_set_dbg_str_mask(uint64_t bit_mask)
{
  char                debug_prop[PROP_VALUE_MAX];
  int                 debug_prop_len;

  /* Get the current Android log level from the android property */

  debug_prop_len = __system_property_get( DAEMON_DEBUG_PROP_NAME, debug_prop );
  if( debug_prop_len == 1 ) {
    switch( debug_prop[0] ) {
      case '0':
        g_log_level = LOG_LEVEL_DISABLED;
        break;
      case '1':
        g_log_level = LOG_LEVEL_ALL;
        break;
      case 'v':
      case 'V':
        g_log_level = LOG_LEVEL_VERBOSE;
        break;
      case 'd':
      case 'D':
        g_log_level = LOG_LEVEL_DEBUG;
        break;
      case 'i':
      case 'I':
        g_log_level = LOG_LEVEL_INFO;
        break;
      case 'w':
      case 'W':
        g_log_level = LOG_LEVEL_WARN;
        break;
      case 'e':
      case 'E':
        g_log_level = LOG_LEVEL_ERROR;
        break;
      default:
        break;
    }
    LOGE("%s: Setting log level to %d", __FUNCTION__, g_log_level);
  } else if( debug_prop_len > 1 ) {
    LOGE("%s: invalid value for %s: %s. Enabling all logs", __FUNCTION__,
         DAEMON_DEBUG_PROP_NAME, debug_prop );
    g_log_level = LOG_LEVEL_ALL;
  }

  dbg_str_mask_g = bit_mask;

} //end of sns_debug_set_dbg_str_mask

/*===========================================================================

  FUNCTION:   sns_debug_is_module_disabled

===========================================================================*/
/*===========================================================================*/
/*!
  @brief
  Returns 1 if the the module's debug strings need to be filtered.

  @param[i] module_id        : Debug module id

  @return
  uint8_t : 1 - Module's messages should be filtered
            0 - Module's messages should NOT be filtered
*/
/*=========================================================================*/
uint8_t sns_debug_is_module_disabled(sns_debug_module_id_e module_id)
{
  uint64_t   temp_i=1;
  uint8_t    ret_value=0;

  /* Check if the messages from the module are enabled
   * based on the debug string filter mask
   */
  if ((dbg_str_mask_g & (temp_i << module_id)) != 0)
  {
    ret_value = 1;
  }
  return ret_value;
} //end of sns_debug_set_dbg_str_mask
