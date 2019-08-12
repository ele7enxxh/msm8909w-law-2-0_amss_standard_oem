#ifndef QSEE_LOG_H
#define QSEE_LOG_H

/*============================================================================
  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_log.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     --------------------------------------------------------
04/02/14    tp     Removaed flush file api
02/07/13    hw     disable the debug log for TZ
12/16/12    mg     route all tz_log calls to qsee_log
10/15/12    hw     include the tz log header to remove compile warning.
                   when qsee_log is ready, the whole tz_log header will be removed.
06/22/12    rk     Redefine log definition macro
08/04/11    rv     Initial Revision

=========================================================================== */
#include <comdef.h>
#include <stdarg.h>
#include <string.h>


/* Flags to control logging.  Enable the flag which will allow
 * the logs with corresponding log level.  For e.g., if we set 
 * ENABLE_QSEE_LOG_MSG_ERROR to 1 then we will get only error logging */
#define ENABLE_QSEE_LOG_MSG_LOW      0
#define ENABLE_QSEE_LOG_MSG_MED      0
#define ENABLE_QSEE_LOG_MSG_HIGH     0
#define ENABLE_QSEE_LOG_MSG_ERROR    1
#define ENABLE_QSEE_LOG_MSG_FATAL    1
#define ENABLE_QSEE_LOG_MSG_DEBUG    1


/* Priority of debug log messages */
#define QSEE_LOG_MSG_LOW      0
#define QSEE_LOG_MSG_MED      1
#define QSEE_LOG_MSG_HIGH     2
#define QSEE_LOG_MSG_ERROR    3
#define QSEE_LOG_MSG_FATAL    4
#define QSEE_LOG_MSG_DEBUG    5

extern void qsee_log(uint8 pri, const char* fmt, ...);

/**
 * @brief Logs to the file in HLOS. Do check if this API is 
 *        supported on your platform
 *
 * @param[in] None
 *
 * @return 
 *
 */
#define QSEE_LOG(xx_prio, xx_fmt, ...)                                            \
      do {                                                                        \
	    if((xx_prio == QSEE_LOG_MSG_LOW) && (ENABLE_QSEE_LOG_MSG_LOW)) {          \
		  qsee_log(xx_prio, #xx_fmt, ##__VA_ARGS__); }                            \
		else if((xx_prio == QSEE_LOG_MSG_MED) && (ENABLE_QSEE_LOG_MSG_MED)) {     \
		  qsee_log(xx_prio, #xx_fmt, ##__VA_ARGS__);                    }         \
		else if((xx_prio == QSEE_LOG_MSG_HIGH) && (ENABLE_QSEE_LOG_MSG_HIGH)) {   \
		  qsee_log(xx_prio, #xx_fmt, ##__VA_ARGS__);                    }         \
        else if((xx_prio == QSEE_LOG_MSG_ERROR) && (ENABLE_QSEE_LOG_MSG_ERROR)) { \
		  qsee_log(xx_prio, #xx_fmt, ##__VA_ARGS__);                    }         \
		else if((xx_prio == QSEE_LOG_MSG_FATAL) && (ENABLE_QSEE_LOG_MSG_FATAL)) { \
		  qsee_log(xx_prio, #xx_fmt, ##__VA_ARGS__);                    }         \
        else if((xx_prio == QSEE_LOG_MSG_DEBUG) && (ENABLE_QSEE_LOG_MSG_DEBUG)) { \
		  qsee_log(xx_prio, #xx_fmt, ##__VA_ARGS__);                    }         \
	  } while (0)

/**
 * @brief Logs to the shared buffer.
 *
 * @param[in] None
 *
 * @return 
 *
 */

void qsee_printf(const char* fmt, ...);

/*************************************************************
 * The following TZ logging will be removed when QSEE logging 
 * is ready 
************************************************************/
#define TZ_LOG_ON


/* If defined, will use simple file based logging instead of the fancy 
** TZ->Kernel->User-Space shared memory based logging mechanism.
*/
#define TZ_FILE_LOGGING_ON

/* Flags to control logging.  Enable the flag which will allow
 * the logs with corresponding log level.  For e.g., if we set 
 * ENABLE_TZ_LOG_MSG_ERROR to 1 then we will get only error logging */
#define ENABLE_TZ_LOG_MSG_LOW       0
#define ENABLE_TZ_LOG_MSG_MED       0
#define ENABLE_TZ_LOG_MSG_HIGH      0
#define ENABLE_TZ_LOG_MSG_ERROR     1
#define ENABLE_TZ_LOG_MSG_FATAL     1
#define ENABLE_TZ_LOG_MSG_DEBUG     0 
#define ENABLE_TZ_LOG_MSG_VERBO     0

 
 
/* Priority of debug log messages */
#define TZ_LOG_MSG_LOW      (0)
#define TZ_LOG_MSG_MED      (1)
#define TZ_LOG_MSG_HIGH     (2)
#define TZ_LOG_MSG_ERROR    (3)
#define TZ_LOG_MSG_FATAL    (4)
#define TZ_LOG_MSG_DEBUG    (5)


/* Maximum length of log message...rest will be trimmed */
#define TZ_LOG_MAX_LEN  120

/**
 * Structure that defines Meta info which is contained at the
 * begining of shared buffer. This is for book keeping of TZ and
 * HLOS driver
 */
typedef struct
{
  uint32  version;            /* For future use */
  uint32  start_offset;       /* To be written by HLOS driver only */
  uint32  end_offset;         /* To be written by TZ driver only */
} __attribute__ ((packed)) tz_sb_log_meta_info_t;

/**
 * Structure to define format of log message
 */
typedef struct
{
  uint8  len;            /* Length of message...max is TZ_LOG_MAX_LEN */
  uint8  priority;       /* Priority of log message */
  uint8* msg_ptr;        /* Physical address of where the msg starts */
} __attribute__ ((packed)) tz_sb_log_msg_t;



/**
 * PR macro to log debug messages to a ring buffer
 *
 * @param [in] xx_prio - Priority of the message to be logge, one of the
 *                       constants \c TZ_PR_<priority>
 * @param [in] xx_fmt  - Format string for output message.
 * @param [in] ...     - Arguments to format string.
 */
#ifdef TZ_LOG_ON
    #define TZ_APPS_RAW(xx_prio, xx_fmt, ...)                                \
      do {                                                                   \
	    if((xx_prio == TZ_LOG_MSG_LOW) && (ENABLE_TZ_LOG_MSG_LOW)) {         \
		  qsee_log(xx_prio, xx_fmt, ##__VA_ARGS__);}                          \
		else if((xx_prio == TZ_LOG_MSG_MED) && (ENABLE_TZ_LOG_MSG_MED)){     \
		  qsee_log(xx_prio, xx_fmt, ##__VA_ARGS__);}                          \
		else if((xx_prio == TZ_LOG_MSG_HIGH) && (ENABLE_TZ_LOG_MSG_HIGH)){   \
		  qsee_log(xx_prio, xx_fmt, ##__VA_ARGS__);}                          \
		else if((xx_prio == TZ_LOG_MSG_ERROR) && (ENABLE_TZ_LOG_MSG_ERROR)){ \
		  qsee_log(xx_prio, xx_fmt, ##__VA_ARGS__);}                          \
		else if((xx_prio == TZ_LOG_MSG_FATAL) && (ENABLE_TZ_LOG_MSG_FATAL)){ \
		  qsee_log(xx_prio, xx_fmt, ##__VA_ARGS__);}                          \
		else if((xx_prio == TZ_LOG_MSG_DEBUG) && (ENABLE_TZ_LOG_MSG_DEBUG)){ \
		  qsee_log(xx_prio, xx_fmt, ##__VA_ARGS__);}                          \
	  } while (0)
#else
    #define TZ_APPS_RAW(xx_prio, xx_fmt, ...)
#endif // TZ_LOG_ON

/**
 * General macro TZ_APPS_LOG.
 */
#define TZ_APPS_LOG(xx_prio, xx_fmt, ...) \
  TZ_APPS_RAW(xx_prio, "[%s:%d] " xx_fmt "\n", __FUNCTION__,__LINE__, ##__VA_ARGS__)
  

/**
 * Shortcut macro for error/debug logging
 */
#define TZ_LOG_ERR(xx_fmt, ...) \
  TZ_APPS_LOG(TZ_LOG_MSG_ERROR, xx_fmt, ##__VA_ARGS__)

#define TZ_LOG_DBG(xx_fmt, ...) \
  TZ_APPS_LOG(TZ_LOG_MSG_DEBUG, xx_fmt, ##__VA_ARGS__)

#define TZ_LOG_BUF(ptr, size) 

#if ENABLE_TZ_LOG_MSG_VERBO
	#define TZ_LOG_VBR(xx_fmt, ...) TZ_LOG_DBG(xx_fmt, ##__VA_ARGS__)
#else
	#define TZ_LOG_VBR(xx_fmt, ...) 
#endif

#if 0
/**
 * This function is used to log to a ring buffer.
 *
 * @param [in] pri  Prioriy of message to be logged.
 * @param [in] fmt  String describing the format of msg.
 * @param [in] ...  Variable argument list.
 */
void tz_log(uint8 pri, const char* fmt, ...);

#endif

#endif /*QSEE_LOG_H*/

