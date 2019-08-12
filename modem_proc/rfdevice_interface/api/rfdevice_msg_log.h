#ifndef RFDEVICE_MSG_LOG_H
#define RFDEVICE_MSG_LOG_H

/*!
  @file
  rfdevice_msg_log.h

  @brief
  Diagnostic message services for the RF device driver.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_msg_log.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/16/14   px     Move device F3s to MSG_MASK_5 to MSG_MASK_8.
03/25/13   sr     Made changes to make multi-line safe macros.
03/23/13   sr     Initial Release.

==============================================================================*/

#include "msg.h"
#include "log.h"

/*!
  @defgroup RFDEVICE_MSG_LOG RF device Message Services
  @{
*/

#define RFDEVICE_MSG_LEGACY_LOW     MSG_MASK_5
#define RFDEVICE_MSG_LEGACY_MED     MSG_MASK_6
#define RFDEVICE_MSG_LEGACY_HIGH    MSG_MASK_7
#define RFDEVICE_MSG_LEGACY_ERROR   MSG_MASK_8

/*!
  @name RF Device Messaging output macros.

  @details
  These macros support outputing diagnostic messages when the respective
  device log is enabled with varying numbers of
  formatting parameters.  The mask level should always be one of the RF
  filtering levels specified in this interface definition.
  @{
*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with no format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string.
*/
#define RFDEVICE_LOG_MSG(log_id, xx_ss_mask, xx_fmt ) \
  if (log_status(log_id)) { MSG( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt ); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with one format parameter.

  @param log_id
  The log_code filtering for diag message output.
 
  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with one printf-style format specifier.

  @param xx_arg1
  First argument for string formatting.
*/
#define RFDEVICE_LOG_MSG_1(log_id, xx_ss_mask, xx_fmt, xx_arg1 ) \
  if (log_status(log_id)) { MSG_1( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1 ); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with two format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with two printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.
*/
#define RFDEVICE_LOG_MSG_2(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 ) \
  if (log_status(log_id)) { MSG_2( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 ); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with three format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
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
#define RFDEVICE_LOG_MSG_3(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 ) \
  if (log_status(log_id)) { MSG_3( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 ); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with four format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
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
#define RFDEVICE_LOG_MSG_4(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 ) \
  if (log_status(log_id)) { MSG_4( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4 ); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with five format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
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
#define RFDEVICE_LOG_MSG_5(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5 ) \
  if (log_status(log_id)) { MSG_5( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with six format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
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
#define RFDEVICE_LOG_MSG_6(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6 ) \
  if (log_status(log_id)) { MSG_6( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with seven format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
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
#define RFDEVICE_LOG_MSG_7(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7 ) \
  if (log_status(log_id)) { MSG_7( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with eight format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
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
#define RFDEVICE_LOG_MSG_8(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8 ) \
  if (log_status(log_id)) { MSG_8( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with nine format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
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
#define RFDEVICE_LOG_MSG_9(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9 ) \
  if (log_status(log_id)) { MSG_9( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9); }

/*!
  @}
*/

/*!
  @}
*/

/*!
  @name RF String Messaging output macros.

  @details
  These macros support outputing diagnostic messages with varying numbers of
  formatting parameters.  The mask level should always be one of the RF
  filtering levels specified in this interface definition.
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Sting Diag message with one format parameter.

  @param log_id
  The log_code filtering for diag message output.
 
  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with one printf-style format specifier.

  @param xx_arg1
  First argument for string formatting.
*/
#define RFDEVICE_LOG_STR_MSG_1(log_id, xx_ss_mask, xx_fmt, xx_arg1 ) \
  if (log_status(log_id)) { MSG_SPRINTF_1( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver String Diag message with two format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
  @param xx_ss_mask
  The message level mask for build and QXDM filtering.

  @param xx_fmt
  Message string with two printf-style format specifiers.

  @param xx_arg1
  First argument for string formatting.

  @param xx_arg2
  Second argument for string formatting.
*/
#define RFDEVICE_LOG_STR_MSG_2(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2 ) \
  if (log_status(log_id)) { MSG_SPRINTF_2( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2); }

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver String Diag message with three format parameters.

  @param log_id
  The log_code filtering for diag message output.
 
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
#define RFDEVICE_LOG_STR_MSG_3(log_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3 ) \
  if (log_status(log_id)) { MSG_SPRINTF_3( MSG_SSID_RF, RFDEVICE_##xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3); }


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

#endif  /* RFDEVICE_MSG_LOG_H */

