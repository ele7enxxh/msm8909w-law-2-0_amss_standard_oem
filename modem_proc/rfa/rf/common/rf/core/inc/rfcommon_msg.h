#ifndef RF_COMMON_CORE_RF_MSG_H
#define RF_COMMON_CORE_RF_MSG_H

/*!
  @file
  rfcommon_msg.h

  @brief
  Diagnostic message services for the RF driver.
*/

/*==============================================================================

  Copyright (c) 2011, 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/16/16   rs      Added debug message capability to avoid F3 flood
02/10/13   APU     Added capability to print Debug messages on Engg builds.
01/10/13   aro     Added macros for string messages
01/10/11   bmg     Initial Release.

==============================================================================*/

#include "msg.h"

/*!
  @defgroup RF_COMMON_MSG Common RF Message Services
  @{
*/

/*!
  @name RF Messaging filter levels

  @details
  These definitions are to be used with the RF messaging macros to define
  the filtering level of the associated message.  Messages can be filtered
  at compile time and at runtime based on build settings and QXDM mask
  settings.
  @{
*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Low priority RF messages.

  @details
  Message priority for logging general logic flow and state information
  for the RF driver.
*/
#define RF_LOW   MSG_LEGACY_LOW

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Medium priority RF messages.

  @details
  Message priority for logging key logic flow and state information of
  sub-modules which is key for debugging RF driver sub-module operation.

  Medium level messages may be used to trace major RF driver function
  call flow.
*/
#define RF_MED   MSG_LEGACY_MED

/*----------------------------------------------------------------------------*/
/*!
  @brief
  High priority RF messages.

  @details
  High priority messages should be reserved for information that is
  critical to debugging the fundamental operation of the RF driver,
  and would be expected in any log record of failing behavior.

  High priority messages are not expected to contain general software
  module debug state or flow information beyond the highest level
  "L1 uses cases."  Each L1 use case may be expected to generate one
  or two high messages from MC or Core.  Technology sub-modules should
  not use high priority messages except in extraordinary cases.
*/
#define RF_HIGH  MSG_LEGACY_HIGH

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Error priority RF messages.

  @details
  Error level messages are used to convey textual information to
  the diagnostic log which contains details about recoverable
  errors.  An error level message indiciates a critical failure
  in the RF driver call sequence or hardware operation, but denotes
  a situation in which a resonable and well-defined recovery is
  possible.
*/
#define RF_ERROR MSG_LEGACY_ERROR

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Fatal priority RF messages.

  @details
  Fatal level messages are used to convey textual information to
  the diagnostic log when an un-recoverable error in the RF driver
  software or hardware occurs.  It is expected that the modem
  subsystem will be halting due to this error condition.  These
  messages may provide more extensive detail about the fatal error
  than ERR_FATAL() allows.
*/
#define RF_FATAL MSG_LEGACY_FATAL

/*!
  @}
*/

/*!
  @name RF Messaging output macros.

  @details
  These macros support outputting diagnostic messages with varying numbers of
  formatting parameters. The mask level should always be one of the RF
  filtering levels specified in this interface definition.
  @{
*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with no format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string.
*/
#define RF_MSG( xx_ss_mask, xx_fmt ) \
  MSG( MSG_SSID_RF, xx_ss_mask, xx_fmt )


/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with one format parameter.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with one printf-style format specifier.

  @param xx_arg1
  First argument for string formatting.
*/
#define RF_MSG_1( xx_ss_mask, xx_fmt, xx_arg1 ) \
  MSG_1( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with two format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with two printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.
*/
#define RF_MSG_2( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 ) \
  MSG_2( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with three format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with three printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.
*/
#define RF_MSG_3( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 ) \
  MSG_3( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with four format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with four printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.
*/
#define RF_MSG_4( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 ) \
  MSG_4( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with five format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with five printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.
*/
#define RF_MSG_5( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5 ) \
  MSG_5( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with six format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with six printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.
*/
#define RF_MSG_6( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6 ) \
  MSG_6( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with seven format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with seven printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.

  @param xx_arg7
  Seventh argument for string formatting.
*/
#define RF_MSG_7( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7 ) \
  MSG_7( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with eight format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with eight printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.

  @param xx_arg7
  Seventh argument for string formatting.

  @param xx_arg8
  Eight argument for string formatting.
*/
#define RF_MSG_8( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8 ) \
  MSG_8( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Diag message with nine format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with nine printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.

  @param xx_arg7
  Seventh argument for string formatting.

  @param xx_arg8
  Eight argument for string formatting.

  @param xx_arg9
  Nineth argument for string formatting.
*/
#define RF_MSG_9( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9 ) \
  MSG_9( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9 )


//#define RF_DEBUG_ON 

#ifdef RF_DEBUG_ON
#define RF_MSG_DBG   RF_MSG
#define RF_MSG_DBG_1 RF_MSG_1
#define RF_MSG_DBG_2 RF_MSG_2
#define RF_MSG_DBG_3 RF_MSG_3
#define RF_MSG_DBG_4 RF_MSG_4
#define RF_MSG_DBG_5 RF_MSG_5
#define RF_MSG_DBG_6 RF_MSG_6
#define RF_MSG_DBG_7 RF_MSG_7
#define RF_MSG_DBG_8 RF_MSG_8
#else
#define NOP_FUNC       
#define RF_MSG_DBG(...)       (void)0
#define RF_MSG_DBG_1(...)     (void)0
#define RF_MSG_DBG_2(...)     (void)0
#define RF_MSG_DBG_3(...)     (void)0
#define RF_MSG_DBG_4(...)     (void)0
#define RF_MSG_DBG_5(...)     (void)0
#define RF_MSG_DBG_6(...)     (void)0
#define RF_MSG_DBG_7(...)     (void)0
#define RF_MSG_DBG_8(...)     (void)0
#endif


/*!
  @}
*/

/*!
  @}
*/

/*!
  @name RF String Messaging output macros.

  @details
  These macros support outputting diagnostic messages with varying numbers of
  formatting parameters. The mask level should always be one of the RF
  filtering levels specified in this interface definition.
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Sting Diag message with one format parameter.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with one printf-style format specifier.

  @param xx_arg1
  First argument for string formatting.
*/
#define RF_STR_MSG_1( xx_ss_mask, xx_fmt, xx_arg1 ) \
  MSG_SPRINTF_1( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver String Diag message with two format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with two printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.
*/
#define RF_STR_MSG_2( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 ) \
  MSG_SPRINTF_2( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver String Diag message with three format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with three printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.
*/
#define RF_STR_MSG_3( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 ) \
  MSG_SPRINTF_3( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver String Diag message with four format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with four printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.
*/
#define RF_STR_MSG_4( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 ) \
  MSG_SPRINTF_4( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver String Diag message with five format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with five printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.
*/
#define RF_STR_MSG_5( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5 ) \
  MSG_SPRINTF_5( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver String Diag message with six format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with six printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.
*/
#define RF_STR_MSG_6( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6 ) \
  MSG_SPRINTF_6( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver String Diag message with seven format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with seven printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.

  @param xx_arg7
  Seventh argument for string formatting.
*/
#define RF_STR_MSG_7( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7 ) \
  MSG_SPRINTF_7( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver String Diag message with eight format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with eight printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.

  @param xx_arg7
  Seventh argument for string formatting.

  @param xx_arg8
  Eight argument for string formatting.
*/
#define RF_STR_MSG_8( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8 ) \
  MSG_SPRINTF_8( MSG_SSID_RF, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8 )

/*!
  @}
*/

/*!
  @}
*/

/*!
  @name RF Debug Messaging output macros.

  @details
  These macros support outputting diagnostic messages with varying numbers of
  formatting parameters. The mask level should always be one of the RF
  filtering levels specified in this interface definition.
  @{
*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with no format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string.
*/
#define RF_MSG_DEBUG( xx_ss_mask, xx_fmt ) \
  MSG( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt )


/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with one format parameter.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with one printf-style format specifier.

  @param xx_arg1
  First argument for string formatting.
*/
#define RF_MSG_DEBUG_1( xx_ss_mask, xx_fmt, xx_arg1 ) \
  MSG_1( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt, xx_arg1 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with two format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with two printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.
*/
#define RF_MSG_DEBUG_2( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 ) \
  MSG_2( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with three format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with three printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.
*/
#define RF_MSG_DEBUG_3( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 ) \
  MSG_3( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with four format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with four printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.
*/
#define RF_MSG_DEBUG_4( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 ) \
  MSG_4( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with five format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with five printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.
*/
#define RF_MSG_DEBUG_5( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5 ) \
  MSG_5( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with six format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with six printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.
*/
#define RF_MSG_DEBUG_6( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6 ) \
  MSG_6( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with seven format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with seven printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.

  @param xx_arg7
  Seventh argument for string formatting.
*/
#define RF_MSG_DEBUG_7( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7 ) \
  MSG_7( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with eight format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with eight printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.

  @param xx_arg7
  Seventh argument for string formatting.

  @param xx_arg8
  Eight argument for string formatting.
*/
#define RF_MSG_DEBUG_8( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8 ) \
  MSG_8( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver Debug Diag message with nine format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with nine printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.

  @param xx_arg3
  Third argument for string formatting.

  @param xx_arg4
  Fourth argument for string formatting.

  @param xx_arg5
  Fifth argument for string formatting.

  @param xx_arg6
  Sixth argument for string formatting.

  @param xx_arg7
  Seventh argument for string formatting.

  @param xx_arg8
  Eight argument for string formatting.

  @param xx_arg9
  Nineth argument for string formatting.
*/
#define RF_MSG_DEBUG_9( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9 ) \
  MSG_9( MSG_SSID_RF_DEBUG, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9 )

/*!
  @}
*/

/*!
  @}
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif  /* RF_COMMON_CORE_RF_MSG_H */

