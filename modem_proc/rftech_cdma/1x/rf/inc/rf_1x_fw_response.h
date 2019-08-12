#ifndef RF_1X_FW_RESPONSE_H
#define RF_1X_FW_RESPONSE_H

/*! 
  @file
  rf_1x_fw_response.h
 
  @brief
  This file includes functions pertaining to handling of FW responses for 1x
*/
 
/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_fw_response.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/11/13   spa     Removed unused def rf_pa_params_cfg_rsp_data_type
02/21/13   spa     housekeeping structure for Pilot stop stream response from FW
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/14/13   Saul    CDMA. 1x EPT/ET call support.
12/18/12   cd      xPT config response handling
11/30/12   Saul    MSGR. fw_response_dereg APIs extract client from data structs.
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
06/14/12   aro     Removed unused EPT data structure
06/05/12   anr     Initial EPT Support
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
05/09/12   aro     Updated RX_START to use Sem Pool for synchronization
04/11/12   aro     OQPCH update to indicate 1xL1 if the timed sleep
                   passed in FW or not
03/28/12   aro     Support to handle FW response message
03/22/12   aro     Migrated 1x FW response handling to RF_FWRSP task
03/22/12   aro     Added data structure to hold FW responses data with
                   an accessor function
03/21/12   aro     Functions to perform registration and deregistration
03/18/12   aro     Added UMID logging
03/13/12   aro     Moved FW State config and DAC cal response handling to RF
03/13/12   aro     Moved tech specific definitions to tech file
03/13/12   aro     Implemented function to de-initilize RF_TASK dispatch
                   for 1x
03/12/12   aro     Function to perform initialization of 1x dispatcher
                   for RF_TASK. Function does tech / module registration with
                   RF_TASK, msgr registration, and semaphore initialization
03/12/12   aro     Handler functions for 1x UMIDs 
03/12/12   aro     Added handler for 1x Temp Comp read 
03/08/12   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "msgr_types.h"
#include "rfcommon_semaphore.h"
#include "rfm_async_types.h"
#include "rfm_device_types.h"

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for 1x Rx Start Response message  */
typedef struct
{
  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_1x_rx_start_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for 1x Rx Stop Response message  */
typedef struct
{
  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

  rfm_cb_handler_type callback;
  /*!< Call back function passed in by 1xL1 which will be executed, when 
  Rx Stop message is sent as asynchronous call.  A function pointer that will be 
  called when the entire sleep sequence is complete.  This function will be 
  passed the RFM_1X_RX_TIMED_SLEEP_COMPLETE event when all actions associated 
  with this API are finished*/

  void *userdata;
  /*!< An opaque data handle that may be used by the calling module to pass
  additional information to the callback function. */

  boolean blocking_call;
  /*!< Flag indicating, if the RX_STOP message was sent as blocking call or 
  not. If it were a blocking call, the handler function will post 
  the semaphore. */ 

  rfm_device_enum_type active_device;
  /*!< Variable indicating the last active device for which the response 
  is pending. */

  boolean timed_sleep_success;
  /*!< Variable to indicate, if the timed_sleep_rx is successful in FW side */

} rf_1x_rx_stop_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for 1x Tx Start Response message  */
typedef struct
{
  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_1x_tx_start_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for 1x Tx Stop Response message  */
typedef struct
{
  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_1x_tx_stop_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for 1x FW state configuration Response 
message */
typedef struct
{

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_1x_fw_state_cfg_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for 1x FW DAC cal response message */
typedef struct
{

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_1x_fw_dac_cal_cfg_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for 1x iC Update response message */
typedef struct
{

  rfm_device_enum_type active_dev;
  /*!< Variable to store the active device on which 1x intelliceiver is running 
  currently running. */

  rfcommon_semaphore_token_type *sema_token;
  /*!< Semaphore to block on IC_UPDATE_RSP when the update is sent by 
  Power mode Override only */

} rf_1x_ic_update_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for EPT Capture Response 
message */
typedef struct
{

  rfcommon_semaphore_token_type sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_1x_ept_capture_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for EPT Expansion Done Response 
message */
typedef struct
{

  rfcommon_semaphore_token_type sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_1x_ept_expansion_done_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for EPT Expansion Done Response 
message */
typedef struct
{

  rfcommon_semaphore_token_type* sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_1x_xpt_config_done_rsp_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for 1x XPT Transition Complete Response message  */
typedef struct
{
  uint8 xpt_mode;
  /*!< The current XPT mode being used by FW */

} rf_1x_xpt_transition_complete_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for pilot meas stop stream mssg */
typedef struct
{

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_1x_pilot_meas_stop_stream_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure containing housekeeping data for all 1x commands */
typedef struct
{

  rf_1x_rx_start_rsp_data_type rx_start_rsp;
  /*!< Housekeeping data for Rx Start Response  */

  rf_1x_rx_stop_rsp_data_type rx_stop_rsp;
  /*!< Housekeeping data for Rx Stop Response  */

  rf_1x_tx_start_rsp_data_type tx_start_rsp;
  /*!< Housekeeping data for Tx Start Response  */

  rf_1x_tx_stop_rsp_data_type tx_stop_rsp;
  /*!< Housekeeping data for Tx Stop Response  */

  rf_1x_fw_state_cfg_rsp_data_type fws_cfg_rsp;
  /*!< Housekeeping data for 1x FW state configuration Response message */

  rf_1x_fw_dac_cal_cfg_rsp_data_type dac_cal_rsp;
  /*!< Housekeeping data for 1x FW DAC cal response message */

  rf_1x_ic_update_rsp_data_type ic_update_rsp;
  /*!< Housekeeping data for 1x iC Update response message */

  rf_1x_ept_capture_rsp_data_type ept_capture_rsp;
  /*!< Housekeeping data for EPT Capture response message */

  rf_1x_ept_expansion_done_rsp_data_type ept_expansion_done_rsp;
  /*!< Housekeeping data for EPT Expansion done response message */

  rf_1x_xpt_config_done_rsp_data_type xpt_config_done_rsp;
  /*!< Housekeeping data for XPT config done response message */

  rf_1x_xpt_transition_complete_rsp_data_type xpt_trans_complete_rsp;
  /*!< Housekeeping data for XPT Transition Complete response message */

  rf_1x_pilot_meas_stop_stream_rsp_data_type pilot_meas_stop_stream_rsp;
  /*!< Housekeeping data for 1x FW state configuration Response message */

} rf_1x_fw_response_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_1x_fw_response_registration
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_fw_response_deregistration
(
  void
);

#endif /* RF_1X_FW_RESPONSE_H */


