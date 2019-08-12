#ifndef RFM_ASYNC_TYPES_H
#define RFM_ASYNC_TYPES_H

/*!
  @file
  Common types used by asynchronous RFM interfaces.

  @details
  This file exports types requried by all RFM functions that implement
  an asynchronous interface.
*/

/*==============================================================================

  Copyright (c) 2009, 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_async_types.h#1 $ 

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/14/14   vbh     Added callback enums for Tx routines
05/19/14   ak      Adding Enums for New Temp Comp Implementation.
03/11/13   dw      Added BRFM_WCDMA_TX_RETUNE_BW_COMPLETE
04/11/12   aro     OQPCH update to indicate 1xL1 if the timed sleep
                   passed in FW or not
09/07/11   aro     Added timed Rx Stop completion Event
04/11/11   aro     Added new 1x Wakeup Rx Prepare Callback action
04/11/11   aro     Added new Wakeup Rx Prepare Callback action
04/07/11   sar     Relocated for CMI and removed conditional #def's. 
03/11/11   aro     Added RFM Failure Callback Event
02/09/11   aro     [1] Compile Error Fix
                   [2] Documentation Update
02/09/11    dw     Add two WCDMA CB events
02/03/11   aro     Documentation Update
01/10/11   aro     Removed unused enums
01/08/11   aro     Added Enums for Wait Time
09/02/09   bmg     SVDO fixes
02/24/09   bmg     Initial revision.

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Time in microseconds the caller should wait for the RF operation to
  complete.  If this time is negative, it indicates the function returned
  an error.
*/
typedef int rfm_wait_time_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type to indicate the RFM API execution status. If this number is
  negative, it indicates the function returned an error.
*/
typedef int32 rfm_api_status_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Event types that can be passed into RFM API callback handlers.

  @details
  Generic event types passed into call back handler when call back
  handler is called during RF hardware warmup or setting times.  This
  allows call back handlers to be called at multiple stages during RF
  hardware warmup and settling times, with each call to the call back
  handler signifying a specific event.  This provides finer timing
  granularity to allow the caller to perform various actions while
  waiting for various stages of RF warmup and settling time to complete.
  Note: This enum definition must match the corresponding RF common enum
  type definition.
*/
typedef enum
{
  RFM_TIME_DELAY_EXPIRED,  /*!< RF time delay has completely expired. */
  RFM_1X_ENABLE_COMPLETE, /*!< The 1x subsystem in the SVDO mode is enabled. */
  RFM_1X_DISABLE_COMPLETE, /*!< The 1x subsystem in the SVDO mode is disbled. */
  RFM_1X_RX_WAKEUP_PREP_COMPLETE, /*!< 1x receiver wakeup Prep is done. */
  RFM_1X_RX_WAKEUP_COMPLETE, /*!< 1x receiver wakeup is done. */
  RFM_1X_RX_SLEEP_COMPLETE, /*!< 1x receiver sleep is done. */
  RFM_1X_RX_TIMED_SLEEP_COMPLETE, /*!< 1x receiver timed sleep is done. */
  RFM_1X_TX_WAKEUP_COMPLETE, /*!< 1x transmitter wakeup is done. */
  RFM_1X_TX_SLEEP_COMPLETE, /*!< 1x transmitter sleep is done. */
  RFM_1X_TUNE_COMPLETE, /*!< The 1x radio has finished re-tuning. */
  RFM_1X_ENABLE_DIV_COMPLETE, /*!< 1x diversity radios have been linked. */
  RFM_1X_DISABLE_DIV_COMPLETE, /*!< 1x diversity radios have been unlinked. */
  RFM_1X_RX_TIMED_SLEEP_FW_FAILED, /*!< Event indicating that FW failed to 
                                    successfully complete Rx Timed sleep*/    
  RFM_HDR_ENABLE_COMPLETE, /*!< The HDR subsystem in the SVDO mode is enabled. */
  RFM_HDR_DISABLE_COMPLETE, /*!< The HDR subsystem in the SVDO mode is disbled. */
  RFM_HDR_RX_WAKEUP_PREP_COMPLETE, /*!< HDR receiver wakeup Prep is done. */
  RFM_HDR_RX_WAKEUP_COMPLETE, /*!< HDR receiver wakeup is done. */
  RFM_HDR_RX_SLEEP_COMPLETE, /*!< HDR receiver sleep is done. */
  RFM_HDR_TX_WAKEUP_COMPLETE, /*!< HDR transmitter wakeup is done. */
  RFM_HDR_TX_SLEEP_COMPLETE, /*!< HDR transmitter sleep is done. */
  RFM_HDR_TUNE_COMPLETE, /*!< The HDR radio has finished re-tuning. */
  RFM_HDR_ENABLE_DIV_COMPLETE, /*!< HDR diversity radios have been linked. */
  RFM_HDR_DISABLE_DIV_COMPLETE, /*!< HDR diversity radios have been unlinked. */

  RFM_WCDMA_AGC_CMD_COMPLETE, /*!< WCDMA AGC Command Complete */
  RFM_WCDMA_AGC_CMD_FAIL, /*!< WCDMA AGC Command Fail */

  RFM_ENTER_MODE_COMPLETE, /*!< The radio has transition to the newly requested mode. */

  RFM_WCDMA_PREP_WAKEUP_RX_COMPLETE, /*!< WCDMA prep wakeup Rx Complete */
  RFM_WCDMA_EXEC_WAKEUP_RX_COMPLETE, /*!< WCDMA exec wakeup Rx Complete */
  RFM_WCDMA_SLEEP_RX_COMPLETE, /*!< WCDMA exec sleep Rx Complete */
  RFM_WCDMA_TX_RETUNE_BW_COMPLETE, /*!< WCDMA Tx retune BW Complete */
  RFM_WCDMA_TX_INIT_COMPLETE,
  RFM_WCDMA_TX_DEINIT_COMPLETE,
  RFM_WCDMA_TX_ENABLE_COMPLETE,
  RFM_WCDMA_TX_DISABLE_COMPLETE,
  RFM_WCDMA_TEMP_COMP_PREP_COMPLETE, /*!< WCDMA temp comp prep complete */
  RFM_WCDMA_TEMP_COMP_PREP_IGNORED, 
	/*!< Event indicating that Prep Temp comp was ignored due to 
	invalid RF state */
  RFM_WCDMA_TEMP_COMP_COMPLETE, /*!< WCDMA temp comp update complete */
  RFM_WCDMA_TEMP_COMP_ABORT_COMPLETE, /*!< WCDMA temp comp abort complete */


  RFM_EXECUTION_FAILURE, /*!< RFM has failed to execute successfully */

  
  RFM_NUM_CB_EVENTS, /*! Number of Callback events */

} rfm_cb_event_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Asynchronous RFM interface callback type.

  @details
  Function signature of the callback expected by asynchronous RFM
  interface functions.  RFM callback functions are required to accept
  two parameters, to indiciate what event this callback invocation is
  responding to.
*/
typedef void (*rfm_cb_handler_type)( rfm_cb_event_enum_type, void* );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Duplicate type name to satisfy legacy code

  @deprecated
  New interfaces should use rfm_cb_handler_type.
*/
typedef rfm_cb_handler_type rfcom_cb_handler_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Duplicate type name to satisfy legacy code

  @deprecated
  New interfaces should use rfm_cb_event_enum_type.
*/
typedef rfm_cb_event_enum_type rfcom_cb_event_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Duplicate type name to satisfy legacy code

  @deprecated
  New interfaces should use RFM_TIME_DELAY_EXPIRED.
*/
#define RFCOM_TIME_DELAY_EXPIRED RFM_TIME_DELAY_EXPIRED

#ifdef __cplusplus
}
#endif
#endif /* RFM_ASYNC_TYPES_H */

