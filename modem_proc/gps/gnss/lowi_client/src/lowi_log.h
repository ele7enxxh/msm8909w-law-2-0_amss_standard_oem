/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 Log utility

 GENERAL DESCRIPTION
 This header declares a logging utility

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 History:

 Date         User      Change
 =============================================================================
 04/06/2012   ly        Created
 =============================================================================*/
#ifndef __LOG_H__
#define __LOG_H__

#include "comdef.h"
#include "msg_diag_service.h"
#include "log.h"
#include "diag.h"
namespace qc_loc_fw
{

#define MSG_LEN 256

enum ERROR_LEVEL
{
  EL_LOG_OFF = 0, EL_ERROR = 1, EL_WARNING = 2, EL_INFO = 3, EL_DEBUG = 4, EL_VERBOSE = 5, EL_LOG_ALL = 100
};

extern ERROR_LEVEL global_log_level;

extern int format_1(char* buf, uint32 buf_size, const char * const format, ...);
#define log_level(log_level, diag_log_level, local_log_tag, format, ...) \
  do { \
    char buf [MSG_LEN]; \
    if(log_level <= global_log_level) \
    { \
      format_1 (buf, MSG_LEN, format, ##__VA_ARGS__); \
      MSG_SPRINTF_1(MSG_SSID_MGP, diag_log_level, "%s", buf); \
    } \
  } while (0) \

#define log_error(a1, format, ...) log_level(EL_ERROR, MSG_LEGACY_ERROR, a1, format, ##__VA_ARGS__)
#define log_warning(a1, format, ...) log_level(EL_WARNING, MSG_LEGACY_HIGH, a1, format, ##__VA_ARGS__)
#define log_info(a1, format, ...) log_level(EL_INFO, MSG_LEGACY_HIGH, a1, format, ##__VA_ARGS__)
#define log_debug(a1, format, ...) log_level(EL_DEBUG, MSG_LEGACY_MED, a1, format, ##__VA_ARGS__)
#define log_verbose(a1, format, ...) log_level(EL_VERBOSE, MSG_LEGACY_MED, a1, format, ##__VA_ARGS__)

int log_set_global_level(const ERROR_LEVEL level);


//int log_flush_all_local_level();
//int log_flush_local_level_for_tag(const char * const tag);
//int log_set_local_level_for_tag(const char * const tag, const ERROR_LEVEL level);

} // namespace qc_loc_fw

#endif //#ifndef __LOG_H__
