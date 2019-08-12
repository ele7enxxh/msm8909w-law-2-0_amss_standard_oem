/*!
  @file
  qsh_util.h

  @brief
  Contains definitions required to be public. Client should not use anything
  in this file.
*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/qsh/backend/qsh_util.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/11/15   ca      CR 920884: QSH rule management feature.
07/06/15   mm      CR 866981: Adding support for Linux MOB
03/03/15   mm      Removed EVENT
02/19/15   mm      Added EVENT to qsh_msg_type_e
01/22/15   mm      Interface changes for dumping data structures
11/21/14   mm      Added TRM client
08/11/14   sn      Changed client CFM -> CFCM
05/23/14   mm      Initial check-in                 
==============================================================================*/

#ifndef QSH_UTIL_H
#define QSH_UTIL_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#ifndef TEST_FRAMEWORK
  #define FEATURE_QSH_ON_TARGET
  #define FEATURE_QSH_WDOG_HB_DYNAMIC
#endif

#include <comdef.h>
#include <rex.h>
#include <ULog.h>
#include <ULogFront.h>
#include <msg.h>
#include <qsh_types.h>
#include <qsh_str.h>
#ifdef FEATURE_QSH_WDOG_HB_DYNAMIC
#include <dog_hb_rex.h>
#endif
#include <qsh_legacy.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief ULog handle used by QSH clients.
*/
extern ULogHandle qsh_ulog_handle;

/*==============================================================================

                             MACROS

==============================================================================*/

/*! @brief
    This Macro provides the compiler with branch prediction information.

    Calling QSH_LIKELY(expr) informs the compiler that it is expected that
    expr == 1 <NOTE: expr has to be equal to 1 not greater than 1>.

    Calling QSH_UNLIKELY(expr) informs the compiler that it is expected
    that expr is going to be 0, hence dont perform prefetching of instructions
    inside.

    @usage
    Use "if (QSH_LIKELY(a==b))" {} if expression '(a==b)' is expected to be
    TRUE most of the time else use "if (QSH_UNLIKELY(a==b)"
*/
#if defined(__GNUC__) && !defined(_lint)
  #define QSH_LIKELY(x) __builtin_expect((x), 1)
  #define QSH_UNLIKELY(x) __builtin_expect((x), 0)
#else
  #define QSH_LIKELY(x) (x)
  #define QSH_UNLIKELY(x) (x)
#endif /* ! (__GNUC__) */

/*! @brief Expands to name of category index enum.
*/
#define QSH_CAT_IDX(cat) cat ## _IDX

/*! Macros to get the number of arguments passed in at compile-time. Works for
    0 <= n <= 10 arguments.
*/
#define QSH_NARG(...) QSH_NARG_WRAPPER(_, ##__VA_ARGS__, QSH_NARG_REV_n())
#define QSH_NARG_WRAPPER(...) QSH_NARG_n(__VA_ARGS__)
#define QSH_NARG_n(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, n, ...) n
#define QSH_NARG_REV_n() 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0

/*! @brief Logging with variable arguments for each message type.
*/
#ifdef FEATURE_QSH_ON_TARGET
  #define QSHI_LOG(client, category, msg_type, summary, ...) \
    msg_type ## _LOG_n(QSH_NARG(__VA_ARGS__), client, category, summary, \
    ##__VA_ARGS__)
#else
  #define QSHI_LOG(client, category, msg_type, summary, ...)
#endif

#define QSH_MSG_TYPE_HIGH_LOG_n(n, client, category, summary, ...) \
  QSH_LOG_HIGH_n(n, client, category, summary, ##__VA_ARGS__)
  
#define QSH_MSG_TYPE_MEDIUM_LOG_n(n, client, category, summary, ...) \
  QSH_LOG_MEDIUM_n(n, client, category, summary, ##__VA_ARGS__)
  
#define QSH_MSG_TYPE_INFO_LOG_n(n, client, category, summary, ...) \
  QSH_LOG_INFO_n(n, client, category, summary, ##__VA_ARGS__)
  
#define QSH_MSG_TYPE_DEBUG_LOG_n(n, client, category, summary, ...) \
  QSH_LOG_DEBUG_n(n, client, category, summary, ##__VA_ARGS__)

#if !defined(QSH_DISABLE_HIGH) && defined(FEATURE_QSH_ON_TARGET)
  #define QSH_LOG_HIGH_n(n, client, category, summary, ...) \
    do \
    { \
      if(QSH_LIKELY(qsh_can_log(client, category, QSH_MSG_TYPE_HIGH, \
        (n) + 1))) \
      { \
        qsh_log_check_high(client, QSH_CAT_IDX(category)); \
        ULOG_RT_PRINTF_ ## n( \
          qsh_ulog_handle, \
          client ## _STR_PAD " | " category ## _STR_PAD " | HIGH  | " \
            summary, \
          ##__VA_ARGS__); \
      } \
    } while(0)
#else
  #define QSH_LOG_HIGH_n(n, client, category, summary, ...)
#endif

#if !defined(QSH_DISABLE_MEDIUM) && defined(FEATURE_QSH_ON_TARGET)
  #define QSH_LOG_MEDIUM_n(n, client, category, summary, ...) \
    do \
    { \
      if(QSH_LIKELY(qsh_can_log(client, category, QSH_MSG_TYPE_MEDIUM, \
        (n) + 1))) \
      { \
        qsh_log_check(client, QSH_CAT_IDX(category)); \
        ULOG_RT_PRINTF_ ## n( \
          qsh_ulog_handle, \
          client ## _STR_PAD " | " category ## _STR_PAD " | MED   | " \
            summary, \
          ##__VA_ARGS__); \
      } \
    } while(0)
#else
  #define QSH_LOG_MEDIUM_n(n, client, category, summary, ...)
#endif

#if !defined(QSH_DISABLE_INFO) && defined(FEATURE_QSH_ON_TARGET)
  #define QSH_LOG_INFO_n(n, client, category, summary, ...) \
    do \
    { \
      if(QSH_LIKELY(qsh_can_log(client, category, QSH_MSG_TYPE_INFO, \
        (n) + 1))) \
      { \
        ULOG_RT_PRINTF_ ## n( \
          qsh_ulog_handle, \
          client ## _STR_PAD " | " category ## _STR_PAD " | INFO  | " \
            summary, \
          ##__VA_ARGS__); \
      } \
    } while(0)
#else
  #define QSH_LOG_INFO_n(n, client, category, summary, ...)
#endif

#if !defined(QSH_DISABLE_DEBUG) && defined(FEATURE_QSH_ON_TARGET)
  #define QSH_LOG_DEBUG_n(n, client, category, summary, ...) \
    do \
    { \
      if(QSH_LIKELY(qsh_can_log(client, category, QSH_MSG_TYPE_DEBUG, \
        (n) + 1))) \
      { \
        ULOG_RT_PRINTF_ ## n( \
          qsh_ulog_handle, \
          client ## _STR_PAD " | " category ## _STR_PAD " | DEBUG | " \
            summary, \
          ##__VA_ARGS__); \
      } \
    } while(0)
#else
  #define QSH_LOG_DEBUG_n(n, client, category, summary, ...)
#endif

/*! Legacy log macros maintained here.
*/
#define QSH_LOG_HIGH_0(client, category, summary) \
  QSH_LOG_HIGH_n(0, client, category, summary)
#define QSH_LOG_HIGH_1(client, category, summary, a1) \
  QSH_LOG_HIGH_n(1, client, category, summary, a1)
#define QSH_LOG_HIGH_2(client, category, summary, a1, a2) \
  QSH_LOG_HIGH_n(2, client, category, summary, a1, a2)
#define QSH_LOG_HIGH_3(client, category, summary, a1, a2, a3) \
  QSH_LOG_HIGH_n(3, client, category, summary, a1, a2, a3)
#define QSH_LOG_HIGH_4(client, category, summary, a1, a2, a3, a4) \
  QSH_LOG_HIGH_n(4, client, category, summary, a1, a2, a3, a4)
#define QSH_LOG_HIGH_5(client, category, summary, a1, a2, a3, a4, a5) \
  QSH_LOG_HIGH_n(5, client, category, summary, a1, a2, a3, a4, a5)
#define QSH_LOG_HIGH_6(client, category, summary, a1, a2, a3, a4, a5, a6) \
  QSH_LOG_HIGH_n(6, client, category, summary, a1, a2, a3, a4, a5, a6)
#define QSH_LOG_HIGH_7(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7) \
  QSH_LOG_HIGH_n(7, client, category, summary, a1, a2, a3, a4, a5, a6, a7)
#define QSH_LOG_HIGH_8(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7, a8) \
  QSH_LOG_HIGH_n(8, client, category, summary, a1, a2, a3, a4, a5, a6, a7, \
    a8)
#define QSH_LOG_HIGH_9(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7, a8, a9) \
  QSH_LOG_HIGH_n(9, client, category, summary, a1, a2, a3, a4, a5, a6, a7, \
    a8, a9)
#define QSH_LOG_HIGH_10(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7, a8, a9, a10) \
  QSH_LOG_HIGH_n(10, client, category, summary, a1, a2, a3, a4, a5, a6, a7, \
    a8, a9, a10)
    
#define QSH_LOG_MEDIUM_0(client, category, summary) \
  QSH_LOG_MEDIUM_n(0, client, category, summary)
#define QSH_LOG_MEDIUM_1(client, category, summary, a1) \
  QSH_LOG_MEDIUM_n(1, client, category, summary, a1)
#define QSH_LOG_MEDIUM_2(client, category, summary, a1, a2) \
  QSH_LOG_MEDIUM_n(2, client, category, summary, a1, a2)
#define QSH_LOG_MEDIUM_3(client, category, summary, a1, a2, a3) \
  QSH_LOG_MEDIUM_n(3, client, category, summary, a1, a2, a3)
#define QSH_LOG_MEDIUM_4(client, category, summary, a1, a2, a3, a4) \
  QSH_LOG_MEDIUM_n(4, client, category, summary, a1, a2, a3, a4)
#define QSH_LOG_MEDIUM_5(client, category, summary, a1, a2, a3, a4, a5) \
  QSH_LOG_MEDIUM_n(5, client, category, summary, a1, a2, a3, a4, a5)
#define QSH_LOG_MEDIUM_6(client, category, summary, a1, a2, a3, a4, a5, a6) \
  QSH_LOG_MEDIUM_n(6, client, category, summary, a1, a2, a3, a4, a5, a6)
#define QSH_LOG_MEDIUM_7(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7) \
  QSH_LOG_MEDIUM_n(7, client, category, summary, a1, a2, a3, a4, a5, a6, a7)
#define QSH_LOG_MEDIUM_8(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7, a8) \
  QSH_LOG_MEDIUM_n(8, client, category, summary, a1, a2, a3, a4, a5, a6, a7, \
    a8)
#define QSH_LOG_MEDIUM_9(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7, a8, a9) \
  QSH_LOG_MEDIUM_n(9, client, category, summary, a1, a2, a3, a4, a5, a6, a7, \
    a8, a9)
#define QSH_LOG_MEDIUM_10(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7, a8, a9, a10) \
  QSH_LOG_MEDIUM_n(10, client, category, summary, a1, a2, a3, a4, a5, a6, a7, \
    a8, a9, a10)

#define QSH_LOG_INFO_0(client, category, summary) \
  QSH_LOG_INFO_n(0, client, category, summary)
#define QSH_LOG_INFO_1(client, category, summary, a1) \
  QSH_LOG_INFO_n(1, client, category, summary, a1)
#define QSH_LOG_INFO_2(client, category, summary, a1, a2) \
  QSH_LOG_INFO_n(2, client, category, summary, a1, a2)
#define QSH_LOG_INFO_3(client, category, summary, a1, a2, a3) \
  QSH_LOG_INFO_n(3, client, category, summary, a1, a2, a3)
#define QSH_LOG_INFO_4(client, category, summary, a1, a2, a3, a4) \
  QSH_LOG_INFO_n(4, client, category, summary, a1, a2, a3, a4)
#define QSH_LOG_INFO_5(client, category, summary, a1, a2, a3, a4, a5) \
  QSH_LOG_INFO_n(5, client, category, summary, a1, a2, a3, a4, a5)
#define QSH_LOG_INFO_6(client, category, summary, a1, a2, a3, a4, a5, a6) \
  QSH_LOG_INFO_n(6, client, category, summary, a1, a2, a3, a4, a5, a6)
#define QSH_LOG_INFO_7(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7) \
  QSH_LOG_INFO_n(7, client, category, summary, a1, a2, a3, a4, a5, a6, a7)
#define QSH_LOG_INFO_8(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7, a8) \
  QSH_LOG_INFO_n(8, client, category, summary, a1, a2, a3, a4, a5, a6, a7, \
    a8)
#define QSH_LOG_INFO_9(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7, a8, a9) \
  QSH_LOG_INFO_n(9, client, category, summary, a1, a2, a3, a4, a5, a6, a7, \
    a8, a9)
#define QSH_LOG_INFO_10(client, category, summary, a1, a2, a3, a4, a5, a6, \
  a7, a8, a9, a10) \
  QSH_LOG_INFO_n(10, client, category, summary, a1, a2, a3, a4, a5, a6, a7, \
    a8, a9, a10)

/*! @brief Macro to abstract logging with "if" check.
*/
#ifdef FEATURE_QSH_ON_TARGET
  #define QSHI_CHECK_LOG(expr, client, category, msg_type, summary, ...) \
    msg_type ## _CHECK_LOG_n(QSH_NARG(__VA_ARGS__), expr, client, category, \
      summary, ##__VA_ARGS__)
#else
  #define QSHI_CHECK_LOG(expr, client, category, msg_type, summary, ...)
#endif
  
#define QSH_MSG_TYPE_HIGH_CHECK_LOG_n(n, expr, client, category, summary, ...) \
  QSH_CHECK_LOG_HIGH_n(n, expr, client, category, summary, ##__VA_ARGS__)
  
#define QSH_MSG_TYPE_MEDIUM_CHECK_LOG_n(n, expr, client, category, summary, \
  ...) \
  QSH_CHECK_LOG_MEDIUM_n(n, expr, client, category, summary, ##__VA_ARGS__)
  
#define QSH_MSG_TYPE_INFO_CHECK_LOG_n(n, expr, client, category, summary, \
  ...) \
  QSH_CHECK_LOG_INFO_n(n, expr, client, category, summary, ##__VA_ARGS__)
  
#if !defined(QSH_DISABLE_HIGH) && defined(FEATURE_QSH_ON_TARGET)
  #define QSH_CHECK_LOG_HIGH_n(n, expr, client, category, summary, ...) \
    do \
    { \
      if (QSH_UNLIKELY(expr)) \
      { \
        QSH_LOG_HIGH_n(n, client, category, summary, ##__VA_ARGS__); \
      } \
      else \
      { \
        qsh_log_check_no_error(client, QSH_CAT_IDX(category)); \
      } \
    } while (0)
#else
  #define QSH_CHECK_LOG_HIGH_n(n, expr, client, category, summary, ...)
#endif

#if !defined(QSH_DISABLE_MEDIUM) && defined(FEATURE_QSH_ON_TARGET)
  #define QSH_CHECK_LOG_MEDIUM_n(n, expr, client, category, summary, ...) \
    do \
    { \
      if (QSH_UNLIKELY(expr)) \
      { \
        QSH_LOG_MEDIUM_n(n, client, category, summary, ##__VA_ARGS__); \
      } \
      else \
      { \
        qsh_log_check_no_error(client, QSH_CAT_IDX(category)); \
      } \
    } while (0)
#else
  #define QSH_CHECK_LOG_MEDIUM_n(n, expr, client, category, summary, ...)
#endif

#if !defined(QSH_DISABLE_INFO) && defined(FEATURE_QSH_ON_TARGET)
  #define QSH_CHECK_LOG_INFO_n(n, expr, client, category, summary, ...) \
    do \
    { \
      if (QSH_UNLIKELY(expr)) \
      { \
        QSH_LOG_INFO_n(n, client, category, summary, ##__VA_ARGS__); \
      } \
    } while (0)
#else
  #define QSH_CHECK_LOG_INFO_n(n, expr, client, category, summary, ...)
#endif

/*! @brief Provides a mapping between category enum and string representation.
*/
#define QSH_CAT_TPUT_STR    "TPUT"
#define QSH_CAT_CFG_STR     "CFG"
#define QSH_CAT_DSM_STR     "DSM"
#define QSH_CAT_PERF_STR    "PERF"
#define QSH_CAT_OTHER_STR   "OTHER"
#define QSH_CAT_EVENT_STR   "EVENT"

/*! @brief Category names padded to maximum width.
*/
#define QSH_CAT_TPUT_STR_PAD    QSH_CAT_TPUT_STR " "
#define QSH_CAT_CFG_STR_PAD     QSH_CAT_CFG_STR "  "
#define QSH_CAT_DSM_STR_PAD     QSH_CAT_DSM_STR "  "
#define QSH_CAT_PERF_STR_PAD    QSH_CAT_PERF_STR " "
#define QSH_CAT_OTHER_STR_PAD   QSH_CAT_OTHER_STR ""
#define QSH_CAT_EVENT_STR_PAD   QSH_CAT_EVENT_STR ""

/*==============================================================================

                         OS Abstraction Layer.

==============================================================================*/

typedef rex_sigs_type qsh_signal_mask_s; 
typedef rex_sigs_type qsh_signal_result_t;

typedef struct
{
  rex_tcb_type * tcb;
  rex_sigs_type  signal_id;
} qsh_signal_s;

#ifdef FEATURE_QSH_WDOG_HB_DYNAMIC
typedef struct
{
  dog_report_type  handle;
  qsh_signal_s      signal;
}qsh_wdog_hb_info_s;
#endif


/*==============================================================================

  FUNCTION:  qsh_signal_mask_add

==============================================================================*/
/*!
  @brief
  Adds a signal to the mask

*/
/*============================================================================*/
INLINE void qsh_signal_mask_init ( qsh_signal_mask_s* mask )
{
  *mask = 0;
}

/*==============================================================================

  FUNCTION:  qsh_signal_mask_add

==============================================================================*/
/*!
  @brief
  Adds a signal to the mask

*/
/*============================================================================*/
INLINE void qsh_signal_mask_add ( qsh_signal_mask_s* mask, qsh_signal_s* signal )
{
  *mask |= signal->signal_id;
}

/*==============================================================================

  FUNCTION:  qsh_signal_mask_remove

==============================================================================*/
/*!
  @brief
  Removes a signal from the mask

*/
/*============================================================================*/
INLINE void qsh_signal_mask_remove ( qsh_signal_mask_s* mask, qsh_signal_s* signal )
{
  *mask &= ~signal->signal_id;
}

/*==============================================================================

  FUNCTION:  qsh_signal_mask_wait

==============================================================================*/
/*!
  @brief
  Removes a signal from the mask

*/
/*============================================================================*/
INLINE qsh_signal_result_t qsh_signal_mask_wait( qsh_signal_mask_s* mask )
{
  return (qsh_signal_result_t)rex_wait( (rex_sigs_type)*mask );
}

/*==============================================================================

  FUNCTION:  qsh_signal_is_set

==============================================================================*/
/*!
  @brief
  Checks to see if a signal is set in the signal result.

  @return true if the signal was set, false otherwise.
*/
/*============================================================================*/
INLINE boolean qsh_signal_is_set
(
  const qsh_signal_result_t* result,
  const qsh_signal_s* signal
)
{
  return ((*result & signal->signal_id) != 0);
}
/*==============================================================================

  FUNCTION:  qsh_signal_set

==============================================================================*/
/*!
  @brief
  Sets the signal

*/
/*============================================================================*/
INLINE void qsh_signal_set( qsh_signal_s* signal )
{
  (void)rex_set_sigs( signal->tcb, signal->signal_id);
}


/*==============================================================================

  FUNCTION:  qsh_signal_clear

==============================================================================*/
/*!
  @brief
  Clears the signal

*/
/*============================================================================*/
INLINE void qsh_signal_clear( qsh_signal_s* signal )
{
  (void) rex_clr_sigs( signal->tcb, signal->signal_id );
}

/*==============================================================================

  FUNCTION:  qsh_signal_init

==============================================================================*/
/*!
  @brief
  Initializes the signal

*/
/*============================================================================*/
INLINE void qsh_signal_init
(
  qsh_signal_s* signal,
  rex_tcb_type* tcb,
  rex_sigs_type signal_id
)
{
  signal->tcb = tcb;
  signal->signal_id = signal_id;
}




/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  qsh_can_log

==============================================================================*/
/*!
  @brief
  To check if the logging is possible for this particular client.
*/
/*============================================================================*/
boolean qsh_can_log
(
  qsh_clt_e       client,
  qsh_cat_e       category,
  qsh_msg_type_e  msg_type,
  uint32          num_log_words
);

/*==============================================================================

  FUNCTION:  qsh_log_check

==============================================================================*/
/*!
  @brief
  Increments the stats indicating that a condition was checked.
*/
/*============================================================================*/
void qsh_log_check
(
  qsh_clt_e       client,
  qsh_cat_idx_e   category_idx
);

/*==============================================================================

  FUNCTION:  qsh_log_check_high

==============================================================================*/
/*!
  @brief
  Increments the stats indicating that a condition was checked with message
  type HIGH.
*/
/*============================================================================*/
void qsh_log_check_high
(
  qsh_clt_e       client,
  qsh_cat_idx_e   category_idx
);

/*==============================================================================

  FUNCTION:  qsh_log_check_no_error

==============================================================================*/
/*!
  @brief
  Increments the stats indicating that a condition was checked with no error.
*/
/*============================================================================*/
void qsh_log_check_no_error
(
  qsh_clt_e       client,
  qsh_cat_idx_e   category_idx
);

#endif /* QSH_UTIL_H */
