#ifndef FTM_MSG_H
#define FTM_MSG_H

/*!
  @file
  ftm_msg.h

  @brief
  Diagnostic message services for the Factory Test Mode.
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/28/11   aro     Initial Release

==============================================================================*/

#include "msg.h"

/*!
  @defgroup FTM_COMMON_MSG Common FTM Message Services
  @{
*/

/*!
  @name FTM Messaging filter levels

  @details
  These definitions are to be used with the FTM messaging macros to define
  the filtering level of the associated message.  Messages can be filtered
  at compile time and at runtime based on build settings and QXDM mask
  settings.
  @{
*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Low priority FTM messages.

  @details
  Message priority for logging general logic flow and state information
  for the FTM driver.
*/
#define FTM_LOW   MSG_LEGACY_LOW

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Medium priority FTM messages.

  @details
  Message priority for logging key logic flow and state information of
  sub-modules which is key for debugging FTM sub-module operation.

  Medium level messages may be used to trace major FTM driver function
  call flow.
*/
#define FTM_MED   MSG_LEGACY_MED

/*----------------------------------------------------------------------------*/
/*!
  @brief
  High priority FTM messages.

  @details
  High priority messages should be reserved for information that is
  critical to debugging the fundamental operation of the FTM driver,
  and would be expected in any log record of failing behavior.

  High priority messages are not expected to contain general software
  module debug state or flow information beyond the highest level
  "L1 uses cases."  Each L1 use case may be expected to generate one
  or two high messages from MC or Core.  Technology sub-modules should
  not use high priority messages except in extraordinary cases.
*/
#define FTM_HIGH  MSG_LEGACY_HIGH

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Error priority FTM messages.

  @details
  Error level messages are used to convey textual information to
  the diagnostic log which contains details about recoverable
  errors.  An error level message indiciates a critical failure
  in the FTM driver call sequence or hardware operation, but denotes
  a situation in which a resonable and well-defined recovery is
  possible.
*/
#define FTM_ERROR MSG_LEGACY_ERROR

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Fatal priority FTM messages.

  @details
  Fatal level messages are used to convey textual information to
  the diagnostic log when an un-recoverable error in the FTM driver
  software or hardware occurs.  It is expected that the modem
  subsystem will be halting due to this error condition.  These
  messages may provide more extensive detail about the fatal error
  than ERR_FATAL() allows.
*/
#define FTM_FATAL MSG_LEGACY_FATAL

/*!
  @}
*/

/*!
  @name FTM Messaging output macros.

  @details
  These macros support outputing diagnostic messages with varying numbers of
  formatting parameters.  The mask level should always be one of the FTM
  filtering levels specified in this interface definition.
  @{
*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with no format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string.
*/
#define FTM_MSG( xx_ss_mask, xx_fmt ) \
  MSG( MSG_SSID_FTM, xx_ss_mask, xx_fmt )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with one format parameter.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with one printf-style format specifier.

  @param xx_arg1
  First argument for string formatting.
*/
#define FTM_MSG_1( xx_ss_mask, xx_fmt, xx_arg1 ) \
  MSG_1( MSG_SSID_FTM, xx_ss_mask, xx_fmt, xx_arg1 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with two format parameters.

  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with two printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.
*/
#define FTM_MSG_2( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 ) \
  MSG_2( MSG_SSID_FTM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with three format parameters.

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
#define FTM_MSG_3( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 ) \
  MSG_3( MSG_SSID_FTM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with four format parameters.

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
#define FTM_MSG_4( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 ) \
  MSG_4( MSG_SSID_FTM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with five format parameters.

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
#define FTM_MSG_5( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5 ) \
  MSG_5( MSG_SSID_FTM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with six format parameters.

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
#define FTM_MSG_6( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6 ) \
  MSG_6( MSG_SSID_FTM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with seven format parameters.

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
#define FTM_MSG_7( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7 ) \
  MSG_7( MSG_SSID_FTM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with eight format parameters.

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
#define FTM_MSG_8( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8 ) \
  MSG_8( MSG_SSID_FTM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM driver Diag message with nine format parameters.

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
#define FTM_MSG_9( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9 ) \
  MSG_9( MSG_SSID_FTM, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9 )

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

#endif  /* FTM_MSG_H */

