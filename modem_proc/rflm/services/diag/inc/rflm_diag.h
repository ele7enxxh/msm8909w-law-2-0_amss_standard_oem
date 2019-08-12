/*!
  @file
  rflm_diag.h

  @brief
  RFLM diagnostic services.

  @detail
  Provides the following features, similar to AMSS diag:
  - Message ("F3") logging, forwarded to SW
  - Trace logging, to memory buffer
  - Fatal error handling

*/

/*===========================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/diag/inc/rflm_diag.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2015-01-22   ra      Add more support to use __FILENAME__ to avoid file paths in trace msg
2014-06-06   jc      use __FILENAME__ to avoid file paths in trace msg
2014-04-15   jc      alias untimed trace msg for off target builds
2014-06-03   aa      Updated diag msg enum for FBRx 
2013-12-02   jc      Allow including log.h
2013-10-30   jc      Removed unnecessary include checks
2013-09-19   jc      Modified to use rflm_diag_error_fatal_handler
2013-07-18   jc      Port from fw_diag.h

===========================================================================*/
#ifndef RFLM_DIAG_H
#define RFLM_DIAG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "rflm_features.h"

/* FW diag cannot coexist with AMSS diag. Assert that those files have not
   already been included, and remove them from further includes.
   With QTF, diag functions are overloaded, so allow them.
*/
#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_NORMAL)

#include "msg.h"
#include "msg_qsr.h"

#ifdef ASSERT_H
#error "Conflicting AMSS diag header: assert.h"
#endif
#define ASSERT_H

#ifdef EVENT_H
#error "Conflicting AMSS diag header: event.h"
#endif
#define EVENT_H

#endif

#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_FW_LIB)

#ifndef __FILENAME__
#define __FILENAME__ __FILENAME__
#endif

#include "fw_diag.h"
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief RFLM fatal error shutdown phases.
*/
typedef enum
{
  RFLM_DIAG_ERROR_PHASE_SETUP,
  RFLM_DIAG_ERROR_PHASE_SAVE,
  RFLM_DIAG_ERROR_PHASE_CLEANUP,
  RFLM_DIAG_NUM_ERROR_PHASES
} rflm_diag_error_phase_t;


/*! @brief Subsystem generating a diagnostic message.
*/
typedef enum
{
  RFLM_DIAG_MSG_SSID_NONE = 0,  /*!< Unspecified/generic message */
  RFLM_DIAG_MSG_SSID_LEGACY,    /*!< Legacy ERR and MSG macros, probably from SW */
  RFLM_DIAG_MSG_SSID_DIAG_SVC,  /*!< Diagnostic service */
  RFLM_DIAG_MSG_SSID_INIT,      /*!< Initialization */
  RFLM_DIAG_MSG_SSID_SMEM_SVC,  /*!< FW SMEM service */
  RFLM_DIAG_MSG_SSID_TIME_SVC,  /*!< Time service */
  RFLM_DIAG_MSG_SSID_PROF,      /*!< Profiling */
  RFLM_DIAG_MSG_SSID_RTOS,      /*!< RTOS module */
  RFLM_DIAG_MSG_SSID_RFCMD,     /*!< RF command interface */
  RFLM_DIAG_MSG_SSID_FWS,       /*!< FW service SW interface */
  RFLM_DIAG_MSG_SSID_MEM,       /*!< FW memory manager */
  RFLM_DIAG_MSG_SSID_RXLM,      /*!< RXLM driver messages */
  RFLM_DIAG_MSG_SSID_TXLM,      /*!< TXLM driver messages */
  RFLM_DIAG_MSG_SSID_CCS,       /*!< CCS driver messages */
  RFLM_DIAG_MSG_SSID_MSGR,      /*!< Message router module */
  RFLM_DIAG_MSG_SSID_DPD,       /*!< DPD module */
  RFLM_DIAG_MSG_SSID_1X,        /*!< 1x */
  RFLM_DIAG_MSG_SSID_MCDO,      /*!< 1xEV-DO */
  RFLM_DIAG_MSG_SSID_WCDMA,     /*!< WCDMA */
  RFLM_DIAG_MSG_SSID_GERAN,     /*!< GERAN */
  RFLM_DIAG_MSG_SSID_LTE,       /*!< LTE */
  RFLM_DIAG_MSG_SSID_TDSCDMA,   /*!< TDSCDMA */
  RFLM_DIAG_MSG_SSID_CMN_TXAGC, /*!< COMMON TXAGC */
  RFLM_DIAG_MSG_SSID_CMN_FBRX,  /*!< COMMON FBRX */
  RFLM_DIAG_MSG_SSID_CMN_AUTOPIN,  /*!< COMMON AutoPin */
} rflm_diag_msg_ssid_e;


/*! @brief Trace macros.

    Trace data is stored in a memory buffer for offline analysis. 
    TRACE_ calls are lightweight and non-blocking, so they may be called
    from fast interrupt handlers.

    Only up to 5 arguments are supported, based on the number of arguments passed
    in registers in the Q6 ABI - this improves the efficiency of the trace
    function.
*/
#define RFLM_DIAG_TRACE_LOW(ssid, fmt, ... ) \
  RFLM_DIAG_TRACE( LOW, ssid, fmt, ## __VA_ARGS__)

#define RFLM_DIAG_TRACE_MED(ssid, fmt, ... ) \
  RFLM_DIAG_TRACE( MED, ssid, fmt, ## __VA_ARGS__)

#define RFLM_DIAG_TRACE_HIGH(ssid, fmt, ... ) \
  RFLM_DIAG_TRACE( HIGH, ssid, fmt, ## __VA_ARGS__)

#define RFLM_DIAG_TRACE_ERROR(ssid, fmt, ... ) \
  RFLM_DIAG_TRACE( ERROR, ssid, fmt, ## __VA_ARGS__)

  
#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_FW_LIB)

#define RFLM_DIAG_TRACE(type, ssid, fmt, ...) \
        FW_DIAG_TRACE( FW_DIAG_MSG_TYPE_##type, FW_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__ )

#else 


#ifdef TEST_FRAMEWORK  
#error code not present
#else

#define RFLM_DIAG_TRACE(type, ssid, fmt, ...) do { \
  static const rflm_diag_const_msg_param_t _rflm_diag_const_msg_param RFLM_DIAG_CONST_SECTION = \
  _RFLM_DIAG_CONST_MSG_PARAM(fmt, RFLM_DIAG_MSG_TYPE_##type, ssid, ## __VA_ARGS__ ); \
  _RFLM_DIAG_CHECK_MSG_ARGS(fmt, ## __VA_ARGS__); \
  _RFLM_DIAG_MSG_FUNC(rflm_diag_trace_, &_rflm_diag_const_msg_param, ## __VA_ARGS__); \
} while (0)
#endif

#endif


/*! @brief Untimed trace macros.

    Same as trace macros, but trace messages are not timestamped (use
    a timestamp of zero).
*/
#define RFLM_DIAG_TRACE_UNTIMED_LOW(ssid, fmt, ... ) \
  RFLM_DIAG_TRACE_UNTIMED( LOW, ssid, fmt, ## __VA_ARGS__)

#define RFLM_DIAG_TRACE_UNTIMED_MED(ssid, fmt, ... ) \
  RFLM_DIAG_TRACE_UNTIMED( MED, ssid, fmt, ## __VA_ARGS__)

#define RFLM_DIAG_TRACE_UNTIMED_HIGH(ssid, fmt, ... ) \
  RFLM_DIAG_TRACE_UNTIMED( HIGH, ssid, fmt, ## __VA_ARGS__)

#define RFLM_DIAG_TRACE_UNTIMED_ERROR(ssid, fmt, ... ) \
  RFLM_DIAG_TRACE_UNTIMED( ERROR, ssid, fmt, ## __VA_ARGS__)

#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_FW_LIB)

#define RFLM_DIAG_TRACE_UNTIMED(type, ssid, fmt, ...) \
        FW_DIAG_TRACE_UNTIMED( FW_DIAG_MSG_TYPE_##type, FW_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__ )

#else 

#ifdef TEST_FRAMEWORK  
#error code not present
#else

#define RFLM_DIAG_TRACE_UNTIMED(type, ssid, fmt, ...) do { \
  static const rflm_diag_const_msg_param_t _rflm_diag_const_msg_param RFLM_DIAG_CONST_SECTION = \
  _RFLM_DIAG_CONST_MSG_PARAM(fmt, RFLM_DIAG_MSG_TYPE_##type, ssid, ## __VA_ARGS__ ); \
  _RFLM_DIAG_CHECK_MSG_ARGS(fmt, ## __VA_ARGS__); \
  _RFLM_DIAG_MSG_FUNC(rflm_diag_trace_untimed_, &_rflm_diag_const_msg_param, ## __VA_ARGS__); \
} while (0)

#endif

#endif


/*! @brief Message macros.

    Messages are sent to the diag server to be forwarded to QXDM, and are 
    also added to the trace buffer.
    A MSG_ call should be treated as an "expensive" operation, since it
    may incur context switches and interprocessor communication.
    MSG_ macros may not be used within fast interrupt context.

    Up to 9 arguments can be provided. 
*/
#define RFLM_DIAG_MSG_LOW(fmt, ... ) \
  RFLM_DIAG_MSG( LOW, RFLM_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__)

#define RFLM_DIAG_MSG_MED(fmt, ... ) \
  RFLM_DIAG_MSG( MED, RFLM_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__)

#define RFLM_DIAG_MSG_HIGH(fmt, ... ) \
  RFLM_DIAG_MSG( HIGH, RFLM_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__)

#define RFLM_DIAG_MSG_ERROR(fmt, ... ) \
  RFLM_DIAG_MSG( ERROR, RFLM_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__)


/* There is no MSG_FATAL. Use ERR_FATAL instead. */


#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_NORMAL)

// For SW build, ssid is forced to MSG_SSID_RF and the diag msg macro
// is defined to use SW diag msg function instead of fw one.
// See modem_proc\core\api\services\msg_diag_service.h
//     modem_proc\core\api\services\msg_qsr.h
#define RFLM_DIAG_MSG(type, ssid, fmt, ... ) \
  do { \
    XX_MSG_V3_CONST (MSG_SSID_RF, MSG_LVL_##type, fmt); \
    msg_v3_send_var (&xx_msg_v3_const, \
                     (uint32)_RFLM_DIAG_NUM_ARGS(__VA_ARGS__), \
                     ## __VA_ARGS__); \
  } while (0)

#elif (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_STANDALONE)

#define RFLM_DIAG_MSG(type, ssid, fmt, ... ) do { \
  static const rflm_diag_const_msg_param_t _rflm_diag_const_msg_param RFLM_DIAG_CONST_SECTION = \
  _RFLM_DIAG_CONST_MSG_PARAM(fmt, RFLM_DIAG_MSG_TYPE_##type, ssid, ## __VA_ARGS__ ); \
  _RFLM_DIAG_CHECK_MSG_ARGS(fmt, ## __VA_ARGS__); \
  _RFLM_DIAG_MSG_FUNC(rflm_diag_msg_, &_rflm_diag_const_msg_param, ## __VA_ARGS__); \
} while (0)

#else

#define RFLM_DIAG_MSG(type, ssid, fmt, ... )  \
        FW_DIAG_MSG( FW_DIAG_MSG_TYPE_##type, FW_DIAG_MSG_SSID_NONE, fmt, ## __VA_ARGS__ )
#endif


/*! @brief ERR_FATAL macro
    Outputs the given message+arguments, and generates a fatal error.

    Only up to 3 arguments can be provided, for compatibility with AMSS diag.
*/
#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_FW_LIB)

// use FW diag fatal error
#define RFLM_DIAG_ERR_FATAL FW_DIAG_ERR_FATAL
#define RFLM_DIAG_ERR_FATAL_INTERNAL_ARGS FW_DIAG_ERR_FATAL_INTERNAL_ARGS

#else

#ifdef TEST_FRAMEWORK
#error code not present
#else // for normal build
#define RFLM_DIAG_ERR_FATAL(fmt, ... ) do { \
  static const rflm_diag_const_msg_param_t _rflm_diag_const_msg_param RFLM_DIAG_CONST_SECTION = \
    _RFLM_DIAG_CONST_MSG_PARAM(fmt, RFLM_DIAG_MSG_TYPE_FATAL, RFLM_DIAG_MSG_SSID_NONE, ## __VA_ARGS__ ); \
  _RFLM_DIAG_CHECK_MSG_ARGS(fmt, ## __VA_ARGS__); \
  _RFLM_DIAG_MSG_FUNC(rflm_diag_fatal_, &_rflm_diag_const_msg_param, ## __VA_ARGS__); \
  /*lint -unreachable */ \
} while (0)

/*! @brief ERR_FATAL_INTERNAL_ARGS macro
    Similar to ERR_FATAL, except the arguments are for internal use only,
    not part of the format string.
*/
#define RFLM_DIAG_ERR_FATAL_INTERNAL_ARGS(fmt, ... ) do { \
  static const rflm_diag_const_msg_param_t _rflm_diag_const_msg_param RFLM_DIAG_CONST_SECTION = \
    _RFLM_DIAG_CONST_MSG_PARAM(fmt, RFLM_DIAG_MSG_TYPE_FATAL, RFLM_DIAG_MSG_SSID_NONE); /* Define with zero args */\
  _RFLM_DIAG_CHECK_MSG_ARGS(fmt); /* Ensure format doesn't require args */ \
  _RFLM_DIAG_MSG_FUNC(rflm_diag_fatal_, &_rflm_diag_const_msg_param, ## __VA_ARGS__); /* Args will be saved */ \
  /*lint -unreachable */ \
} while (0)

#endif

#endif



/*! @brief ASSERT macro
    Enforces that the given expression is true, otherwise generates a fatal
    error. 
    This is ALWAYS a fatal error. Should you wish to have a "debug-only assert",
    define your own macro that expands to ASSERT() or empty.
    With QTF, an ASSERT macro is already provided, so use it.
*/
#ifdef TEST_FRAMEWORK
  #error code not present
#else
#undef ASSERT
#define ASSERT( expr ) \
  if ( EXPECT( !(expr), 0 ) ) \
  { \
    RFLM_DIAG_ERR_FATAL("Assertion (" #expr ") failed"); \
  }
#endif

/*! @brief RFLM_ASSERT_ARGS macro 
    Allows passing arguments to be saved when ASSERT fails.
    They will not be output explicitly because there are no format specifiers
    for them, but they can be extracted from fatal error parameters.
    int err = foo();
    ASSERT(err == 0, err);
    If fails, prints "Assertion (err == 0) failed", "err" arg is saved.
*/
/*lint -emacro((506),RFLM_ASSERT_ARGS) */
#define RFLM_ASSERT_ARGS( expr, ... ) \
  if ( EXPECT( !(expr), 0 ) ) \
  { \
    RFLM_DIAG_ERR_FATAL_INTERNAL_ARGS("Assertion (" #expr ") failed (see args)", ## __VA_ARGS__); \
  }


/*! @brief Binary log packet header. This should only be accessed through
    rflm_diag_log_header_init().
*/
typedef struct
{
  /* To avoid exposing MSGR API to all modules using logging, we reserve 
     enough storage here for MSGR header, to be filled in by the logging 
     code. There is a compiler assert to ensure the sizes match. */
  uint64       msgr_hdr_reserved[2];   /*!< Reserved for MSGR header */

} rflm_diag_log_packet_header_t;




/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void rflm_diag_init(void);


extern void rflm_diag_add_error_callback
(
  rflm_diag_error_phase_t phase,
  void (*cb)(void)  
);


extern void rflm_diag_remove_error_callback
(
  rflm_diag_error_phase_t phase,
  void (*cb)(void)  
);



/*===========================================================================

                    FOR INTERNAL USE ONLY

===========================================================================*/

/*! @brief Place diag constant data in a special section, if required.
    Currently this is disabled.
*/
#define RFLM_DIAG_CONST_SECTION 


/*! @brief Diagnostic message types. 
    These values correspond to the bit positions in AMSS diag.
*/
typedef enum
{
  RFLM_DIAG_MSG_TYPE_LOW   = 0,
  RFLM_DIAG_MSG_TYPE_MED   = 1,
  RFLM_DIAG_MSG_TYPE_HIGH  = 2,
  RFLM_DIAG_MSG_TYPE_ERROR = 3,
  RFLM_DIAG_MSG_TYPE_FATAL = 4

} rflm_diag_msg_type_e;


/*! @brief Struct for constant message parameters. 
    Note this is declared packed to save memory, since it's rarely accessed at runtime.
*/
typedef PACK(struct)
{
  const char         *filename;       /*!< Filename generating the message */
  const char         *format;         /*!< Format string for message */
  uint16              line;           /*!< Line number of message */
  rflm_diag_msg_type_e  type;           /*!< Message type */
  rflm_diag_msg_ssid_e  ssid;           /*!< Subsystem ID generating the message */
  uint8               num_args;       /*!< Number of message arguments */

} rflm_diag_const_msg_param_t;


/*! @brief Calculate the base filename (no path) at compile time. 
    Note prepending a '/' so the function never returns NULL.
    If this is changed, need to update rflm_diag_filename() function to do
    the equivalent operation.
*/
#ifdef _lint
#define _RFLM_DIAG_BASE_FILENAME(f) f
#else

#ifdef __llvm__
// __FILENAME__ should be provided by build system with stripped paths.
// For LLVM, __builtin_strrchr() does not generate static char string
#ifdef __FILENAME__
#define _RFLM_DIAG_BASE_FILENAME(f) __FILENAME__
#else
#define _RFLM_DIAG_BASE_FILENAME(f) f
//#warning "No base filename for diag - using full name"
#endif
#else // not LLVM
#define _RFLM_DIAG_BASE_FILENAME(f) (__builtin_strrchr("/" f,'/') + 1)
#endif

#endif

/*! @brief Initialize constant message parameters.
*/
#define _RFLM_DIAG_CONST_MSG_PARAM(fmt, type, ssid, ...)  { \
    _RFLM_DIAG_BASE_FILENAME(__FILENAME__), \
    fmt, \
    __LINE__, \
    type, \
    ssid, \
    _RFLM_DIAG_NUM_ARGS(__VA_ARGS__) \
  } 


/*! @brief Internal macro for variable-length message function call.
    Expands an argument list into a function call named for the number of 
    arguments:
      _RFLM_DIAG_MSG_FUNC(rflm_diag_msg_,msg)     => rflm_diag_msg_0(msg)
      _RFLM_DIAG_MSG_FUNC(rflm_diag_msg_,msg,1,2) => rflm_diag_msg_2(msg,1,2)
*/
#define _RFLM_DIAG_MSG_FUNC(func, msg, ...) \
  _RFLM_DIAG_MSG_FUNC_EXPAND(func, _RFLM_DIAG_NUM_ARGS(__VA_ARGS__), ## __VA_ARGS__)(msg, ## __VA_ARGS__)
#define _RFLM_DIAG_MSG_FUNC_EXPAND(func, num, ...) \
  _RFLM_DIAG_MSG_FUNC_EXPAND2(func, num, ## __VA_ARGS__)
#define _RFLM_DIAG_MSG_FUNC_EXPAND2(func, num, ...) \
  func##num



/*! @brief Count of the number of arguments in a variadic macro.
    Original idea from Laurent Deniau, comp.std.c
*/
#define _RFLM_DIAG_NUM_ARGS(...)  _RFLM_DIAG_NUM_ARGS_M1(dummy, ## __VA_ARGS__)
#define _RFLM_DIAG_NUM_ARGS_M1(...) _ARGUMENT_22(__VA_ARGS__, \
  20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define _ARGUMENT_22( \
  a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, \
  a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,...) a22


/*! @brief Check MSG_ and TRACE_ macro arguments against the format string.
    This uses a dummy function that's called with the macro arguments,
    which does Lint checking but nothing else.
*/
#if ( defined(_lint) )

  /*lint +e557 +e558 +e559 +e560 +e566 +e567 +e626 +e719 +e816 
    Turn on all lint printf format checking. This is safe to
    enable globally, since printf format errors should always be reported.
  */
  /*lint -printf(1, _RFLM_DIAG_CHECK_MSG_ARGS ) Check this function like printf */
  STATIC_INLINE void _RFLM_DIAG_CHECK_MSG_ARGS(const char *fmt, ...) { }

#else

  /* GCC is very picky about enforcing argument types, and it will warn 
     about a %d used with a long argument (and int32 is declared as long) even
     though they are the same size.
     Too many spurious warnings - leave it disabled, rely on the lint checks.

     STATIC_INLINE  __attribute__((format(printf,1,2))) void _RFLM_DIAG_CHECK_MSG_ARGS(const char *fmt, ...) { }
   */
  #define _RFLM_DIAG_CHECK_MSG_ARGS(...)

#endif


/*! @brief Attribute to indicate the function does not return.
    Compiler can generate smaller code from the caller if present.
*/
#define _RFLM_DIAG_NORETURN   __attribute__((noreturn))


/* The following functions are used internally by the diag macros. */

extern void rflm_diag_trace_0
(
  const rflm_diag_const_msg_param_t *msg
);

extern void rflm_diag_trace_1
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1
);

extern void rflm_diag_trace_2
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2
);

extern void rflm_diag_trace_3
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3
);

extern void rflm_diag_trace_4
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4
);

extern void rflm_diag_trace_5
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4,
  int32 arg5
);


extern void rflm_diag_trace_untimed_0
(
  const rflm_diag_const_msg_param_t *msg
);

extern void rflm_diag_trace_untimed_1
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1
);

extern void rflm_diag_trace_untimed_2
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2
);

extern void rflm_diag_trace_untimed_3
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3
);

extern void rflm_diag_trace_untimed_4
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4
);

extern void rflm_diag_trace_untimed_5
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4,
  int32 arg5
);



extern void rflm_diag_msg_0
(
  const rflm_diag_const_msg_param_t *msg
);

extern void rflm_diag_msg_1
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1
);

extern void rflm_diag_msg_2
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2
);

extern void rflm_diag_msg_3
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3
);

extern void rflm_diag_msg_4
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4
);

extern void rflm_diag_msg_5
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4,
  int32 arg5
);

extern void rflm_diag_msg_6
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4,
  int32 arg5,
  int32 arg6
);

extern void rflm_diag_msg_7
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4,
  int32 arg5,
  int32 arg6,
  int32 arg7
);

extern void rflm_diag_msg_8
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4,
  int32 arg5,
  int32 arg6,
  int32 arg7,
  int32 arg8
);

extern void rflm_diag_msg_9
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4,
  int32 arg5,
  int32 arg6,
  int32 arg7,
  int32 arg8,
  int32 arg9
);



extern void _RFLM_DIAG_NORETURN rflm_diag_fatal_0
(
  const rflm_diag_const_msg_param_t *msg
);

extern void _RFLM_DIAG_NORETURN rflm_diag_fatal_1
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1
);

extern void _RFLM_DIAG_NORETURN rflm_diag_fatal_2
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2
);

extern void _RFLM_DIAG_NORETURN rflm_diag_fatal_3
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3
);

extern void _RFLM_DIAG_NORETURN rflm_diag_fatal_4
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4
);

extern void _RFLM_DIAG_NORETURN rflm_diag_fatal_5
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4,
  int32 arg5
);

#ifdef __cplusplus
} // extern "C"
#endif


#endif /* RFLM_DIAG_H */

