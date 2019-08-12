/*===========================================================================
                        M O D E M C F G    INTERNAL   (modem_cfg_osal.h)

DESCRIPTION

   OS Abstraction layer for modem_cfg module 
   (to allow it to compile with any OS layer or even without it) 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/inc/mcfg_int.h#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
09/07/12     bd    Move condition check macros to mcfg_int.h, add timestamps
01/18/12     sbt   Create

===========================================================================*/

#ifndef MCFG_INT_H
#define MCFG_INT_H

#include "comdef.h"
#include "msg.h"
#include "mcfg_feature_config.h"

#include <stringl/stringl.h>
#include "mcfg_log.h"
/*=============================================================================
 
  Constants and Macros
 
=============================================================================*/

#ifndef MSG_SSID_MCFG
  #define MSG_SSID_MCFG    MSG_SSID_BT
#endif


/* Error messages */
#include <stdio.h>
#include "timetick.h"
char sMcfg_LogMsg[192];
extern boolean mcfg_efslog_enabled;

#define MCFG_MSG_TIME_VAR_INIT timetick_type _mcfg_time_ms = timetick_get_ms()
#define MCFG_MSG_TIME_FMT(fmt) "[%02lu.%03lu] " fmt "\n", (_mcfg_time_ms / 1000), (_mcfg_time_ms % 1000)

#define MCFG_MSG_ERROR(xx_fmt) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Err: " xx_fmt)); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG(MSG_SSID_MCFG, MSG_LEGACY_ERROR, "=MCFG= " xx_fmt)
  
#define MCFG_MSG_ERROR_1(xx_fmt, xx_arg1) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Err: " xx_fmt), xx_arg1); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG_1(MSG_SSID_MCFG, MSG_LEGACY_ERROR, "=MCFG= " xx_fmt, xx_arg1)

#define MCFG_MSG_ERROR_2(xx_fmt, xx_arg1, xx_arg2) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Err: " xx_fmt), xx_arg1, xx_arg2); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG_2(MSG_SSID_MCFG, MSG_LEGACY_ERROR, "=MCFG= " xx_fmt, xx_arg1, xx_arg2)

#define MCFG_MSG_ERROR_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Err: " xx_fmt), xx_arg1, xx_arg2, xx_arg3); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG_3(MSG_SSID_MCFG, MSG_LEGACY_ERROR, "=MCFG= " xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define MCFG_MSG_ERROR_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Err: " xx_fmt), xx_arg1, xx_arg2, xx_arg3, xx_arg4); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG_4(MSG_SSID_MCFG, MSG_LEGACY_ERROR, "=MCFG= " xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define MCFG_MSG_ERROR_5(fmt, a1, a2, a3, a4, a5) \
  MSG_5(MSG_SSID_MCFG, MSG_LEGACY_ERROR, "=MCFG= " fmt, a1, a2, a3, a4, a5)

#define MCFG_MSG_ERROR_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_6(MSG_SSID_MCFG, MSG_LEGACY_ERROR, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6)

#define MCFG_MSG_ERROR_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_7(MSG_SSID_MCFG, MSG_LEGACY_ERROR, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7)

#define MCFG_MSG_ERROR_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_8(MSG_SSID_MCFG, MSG_LEGACY_ERROR, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7, a8)
  
#define MCFG_MSG_HIGH(xx_fmt) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Log: " xx_fmt)); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG(MSG_SSID_MCFG, MSG_LEGACY_HIGH, "=MCFG= " xx_fmt);

#define MCFG_MSG_HIGH_1(xx_fmt, xx_arg1) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Log: " xx_fmt), xx_arg1); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG_1(MSG_SSID_MCFG, MSG_LEGACY_HIGH, "=MCFG= " xx_fmt, xx_arg1)

#define MCFG_MSG_HIGH_2(xx_fmt, xx_arg1, xx_arg2) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Log: " xx_fmt), xx_arg1, xx_arg2); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG_2(MSG_SSID_MCFG, MSG_LEGACY_HIGH, "=MCFG= " xx_fmt, xx_arg1, xx_arg2)

#define MCFG_MSG_HIGH_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Log: " xx_fmt), xx_arg1, xx_arg2, xx_arg3); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG_3(MSG_SSID_MCFG, MSG_LEGACY_HIGH, "=MCFG= " xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define MCFG_MSG_HIGH_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Log: " xx_fmt), xx_arg1, xx_arg2, xx_arg3, xx_arg4); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG_4(MSG_SSID_MCFG, MSG_LEGACY_HIGH, "=MCFG= " xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define MCFG_MSG_HIGH_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  if (mcfg_efslog_enabled) { \
      MCFG_MSG_TIME_VAR_INIT; \
      snprintf(sMcfg_LogMsg, sizeof(sMcfg_LogMsg), MCFG_MSG_TIME_FMT("Log: " xx_fmt), xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5); \
      mcfg_save_log_entry(sMcfg_LogMsg); \
  } else (void)0; \
  MSG_5(MSG_SSID_MCFG, MSG_LEGACY_HIGH, "=MCFG= " xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define MCFG_MSG_HIGH_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_6(MSG_SSID_MCFGFG, MSG_LEGACY_HIGH, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6)

#define MCFG_MSG_HIGH_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_7(MSG_SSID_MCFG, MSG_LEGACY_HIGH, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7)

#define MCFG_MSG_HIGH_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_8(MSG_SSID_MCFG, MSG_LEGACY_HIGH, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Informational messages */
#define MCFG_MSG_MED(fmt) \
  MSG(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt)

#define MCFG_MSG_MED_1(fmt, a1) \
  MSG_1(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1)

#define MCFG_MSG_MED_2(fmt, a1, a2) \
  MSG_2(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2)

#define MCFG_MSG_MED_3(fmt, a1, a2, a3) \
  MSG_3(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3)

#define MCFG_MSG_MED_4(fmt, a1, a2, a3, a4) \
  MSG_4(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4)

#define MCFG_MSG_MED_5(fmt, a1, a2, a3, a4, a5) \
  MSG_5(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4, a5)

#define MCFG_MSG_MED_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_6(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6)

#define MCFG_MSG_MED_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_7(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7)

#define MCFG_MSG_MED_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_8(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Debug messages */
#define MCFG_MSG_LOW(fmt) \
  MSG(MSG_SSID_MCFG, MSG_LEGACY_LOW, "=MCFG= " fmt)

#define MCFG_MSG_LOW_1(fmt, a1) \
  MSG_1(MSG_SSID_MCFG, MSG_LEGACY_LOW, "=MCFG= " fmt, a1)

#define MCFG_MSG_LOW_2(fmt, a1, a2) \
  MSG_2(MSG_SSID_MCFG, MSG_LEGACY_LOW, "=MCFG= " fmt, a1, a2)

#define MCFG_MSG_LOW_3(fmt, a1, a2, a3) \
  MSG_3(MSG_SSID_MCFG, MSG_LEGACY_LOW, "=MCFG= " fmt, a1, a2, a3)

#define MCFG_MSG_LOW_4(fmt, a1, a2, a3, a4) \
  MSG_4(MSG_SSID_MCFG, MSG_LEGACY_LOW, "=MCFG= " fmt, a1, a2, a3, a4)

#define MCFG_MSG_LOW_5(fmt, a1, a2, a3, a4, a5) \
  MSG_5(MSG_SSID_MCFG, MSG_LEGACY_LOW, "=MCFG= " fmt, a1, a2, a3, a4, a5)

#define MCFG_MSG_LOW_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_6(MSG_SSID_MCFG, MSG_LEGACY_LOW, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6)

#define MCFG_MSG_LOW_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_7(MSG_SSID_MCFG, MSG_LEGACY_LOW, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7)

#define MCFG_MSG_LOW_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_8(MSG_SSID_MCFG, MSG_LEGACY_LOW, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Messages allowing string values via the %s keyword. These are separate from 
   the regular messages since they can have a performance impact. */
#define MCFG_MSG_SPRINTF_1(fmt, a1) \
  MSG_SPRINTF_1(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1)

#define MCFG_MSG_SPRINTF_2(fmt, a1, a2) \
  MSG_SPRINTF_2(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2)

#define MCFG_MSG_SPRINTF_3(fmt, a1, a2, a3) \
  MSG_SPRINTF_3(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3)

#define MCFG_MSG_SPRINTF_4(fmt, a1, a2, a3, a4) \
  MSG_SPRINTF_4(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4)

#define MCFG_MSG_SPRINTF_5(fmt, a1, a2, a3, a4, a5) \
  MSG_SPRINTF_5(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4, a5)

#define MCFG_MSG_SPRINTF_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_SPRINTF_6(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6)

#define MCFG_MSG_SPRINTF_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_SPRINTF_7(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7)

#define MCFG_MSG_SPRINTF_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_SPRINTF_8(MSG_SSID_MCFG, MSG_LEGACY_MED, "=MCFG= " fmt, a1, a2, a3, a4, a5, a6, a7, a8)


#ifdef MCFG_DEBUG
  #define MCFG_ERR_FATAL(str, a,b,c)    ERR_FATAL(str, a, b, c)
#else
  #define MCFG_ERR_FATAL(str, a,b,c)    ERR_FATAL(str, a, b, c)
//  #define MCFG_ERR_FATAL(str, a,b,c)    MCFG_MSG_ERROR_3(str, a, b, c)
#endif

  /*! @brief Helper macro to return from a void function if a pointer is NULL
  */
#define MCFG_CHECK_NULL_PTR_RET(ptr) \
    if ((ptr) == NULL) \
    { \
      MCFG_MSG_ERROR("Unexpected NULL pointer!"); \
      return; \
    }
  
  /*! @brief Helper macro to return FALSE if a pointer is NULL
  */
#define MCFG_CHECK_NULL_PTR_RET_FALSE(ptr) \
    if ((ptr) == NULL) \
    { \
      MCFG_MSG_ERROR("Unexpected NULL pointer!"); \
      return FALSE; \
    }
  
  /*! @brief Helper macro to return FALSE if a pointer referent is NULL
  */
#define MCFG_CHECK_NULL_REFERENT_RET_FALSE(ref) \
    if ((ref) == 0) \
    { \
      MCFG_MSG_ERROR("Unexpected NULL at reference!"); \
      return FALSE; \
    }
  
  /*! @brief Helper macro to return NULL if a pointer is NULL
  */
#define MCFG_CHECK_NULL_PTR_RET_NULL(ptr) \
    if ((ptr) == NULL) \
    { \
      MCFG_MSG_ERROR("Unexpected NULL pointer!"); \
      return NULL; \
    }
  
  /*! @brief Helper macro to return 0 if a pointer is NULL
  */
#define MCFG_CHECK_NULL_PTR_RET_ZERO(ptr) \
    if ((ptr) == NULL) \
    { \
      MCFG_MSG_ERROR("Unexpected NULL pointer!"); \
      return 0; \
    }

  /*! @brief Helper macro to return 0 if a pointer is NULL
  */
#define MCFG_CHECK_NULL_PTR_RET_ERROR(ptr, error) \
    if ((ptr) == NULL) \
    { \
      MCFG_MSG_ERROR_1("Unexpected NULL pointer, return error %d!", error); \
      return (error); \
    }

  /*! @brief Helper macro to return if a condition is false
  */
#define MCFG_ASSERT_COND_RET(condition) \
    if ((condition) == FALSE) \
    { \
      MCFG_MSG_ERROR("Unexpected condition!"); \
      return; \
    }

/*! @brief Helper macro to return FALSE if a condition is false
  */
#define MCFG_ASSERT_COND_RET_FALSE(condition) \
    if ((condition) == FALSE) \
    { \
      MCFG_MSG_ERROR("Unexpected condition!"); \
      return FALSE; \
    }

  /*! @brief Helper macro to return ZERO if a condition is false
  */
#define MCFG_ASSERT_COND_RET_ZERO(condition) \
    if ((condition) == FALSE) \
    { \
      MCFG_MSG_ERROR("Unexpected condition!"); \
      return 0; \
    }

  /*! @brief Helper macro to return an error if a condition is false
  */
#define MCFG_ASSERT_COND_RET_ERROR(condition, error) \
    if ((condition) == FALSE) \
    { \
      MCFG_MSG_ERROR_1("Unexpected condition, return error %d!", error); \
      return (error); \
    }

#endif /* MCFG_INT_H */
