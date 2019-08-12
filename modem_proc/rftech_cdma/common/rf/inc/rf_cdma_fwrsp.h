#ifndef RF_CDMA_FWRSP_H
#define RF_CDMA_FWRSP_H

/*! 
  @file
  rf_cdma_fwrsp.h
 
  @brief
  This file includes functions pertaining to handling of FW responses for cdma
*/
 
/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_fwrsp.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/10/14   JJ      Change smem_addr_fw type to void*
04/08/14   JJ      Added an api to send oqpch_search_done msg to FW
11/11/13   JJ      Added smem_addr in rf_cdma_fws_cfg_rsp_data_type
06/03/13   spa     Migrate to new RF dispatch for XPT FW response mssgs 
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
09/28/12   aro     Added blocking call flag to ic_update hk data
09/10/12   aro     Added HDR RxAGC logging using RF-FW Sync
09/10/12   aro     SNUM dispatch callback function for IQ Capture and
                   DPD expansion
09/10/12   aro     HDR callback function for SNUM dispatch
09/10/12   aro     SNUM dispatch callback functions for TX_STOP, TX_START,
                   IRAT_RX_START
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
#include "rfcommon_semaphore.h"
#include "rfm_async_types.h"
#include "rfm_device_types.h"

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma Rx Start Response message  */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_cdma_rx_start_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma iRAT Rx Start 
Response message  */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

} rf_cdma_irat_rx_start_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma Rx Stop Response message  */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

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

  boolean timed_sleep_success;
  /*!< Variable to indicate, if the timed_sleep_rx is successful in FW side */

} rf_cdma_rx_stop_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma Tx Start Response message  */
typedef struct
{

  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_cdma_tx_start_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma Tx Stop Response message  */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_cdma_tx_stop_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma FW state configuration Response 
message */
typedef struct
{

  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

  void* smem_addr_fw;
  /*!< shared memory address passed from FW after configuring FW state. In 
  enable modulator, we will pass this shared memory addr to L1  */

} rf_cdma_fws_cfg_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma FW DAC cal response message */
typedef struct
{

  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_cdma_dac_cal_cfg_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma iC Update response message */
typedef struct
{

  rfm_device_enum_type device;
  /*!< Variable to store the active device on which cdma intelliceiver is running 
  currently running. */

  rfcommon_semaphore_token_type *sema_token;
  /*!< Semaphore to block on IC_UPDATE_RSP when the update is sent by 
  Power mode Override only */

  boolean blocking_call;
  /*!< Flag indicating, if the RX_STOP message was sent as blocking call or 
  not. If it were a blocking call, the handler function will post 
  the semaphore. */ 

} rf_cdma_ic_update_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for RxAGC Track indicator message */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device on which cRxAGC track indicator 
  message is expcted. */

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_cdma_rxagc_trk_ind_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for RxAGC logging message */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device on which cRxAGC track indicator 
  message is expcted. */

} rf_cdma_rxagc_logging_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma EPT IQ Capture
Response message  */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

} rf_cdma_ept_iq_capture_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma Pilot meas config
Response message  */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

} rf_cdma_pilot_meas_config_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma Pliot meas stop stream
Response message  */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_cdma_pilot_meas_stop_stream_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma XPT config Response message */ 
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_cdma_xpt_config_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for XPT transition complete Response 
message */ 
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

  rfcommon_semaphore_token_type *sema_token;
  /*!< semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */

} rf_cdma_xpt_trans_compl_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for cdma stop OQPCH search
Response message  */
typedef struct
{
  rfm_device_enum_type device;
  /*!< Variable to store the active device for which the response is to be 
  handled */

} rf_cdma_stop_oqpch_search_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure containing housekeeping data for all cdma commands */
typedef struct
{

  rf_cdma_rx_start_rsp_data_type rx_start_rsp;
  /*!< Housekeeping data for Rx Start Response  */

  rf_cdma_irat_rx_start_rsp_data_type irat_rx_start_rsp;
  /*!< Housekeeping data for iRAT Rx Start Response  */

  rf_cdma_rx_stop_rsp_data_type rx_stop_rsp;
  /*!< Housekeeping data for Rx Stop Response  */

  rf_cdma_tx_start_rsp_data_type tx_start_rsp;
  /*!< Housekeeping data for Tx Start Response  */

  rf_cdma_tx_stop_rsp_data_type tx_stop_rsp;
  /*!< Housekeeping data for Tx Stop Response  */

  rf_cdma_fws_cfg_rsp_data_type fws_cfg_rsp;
  /*!< Housekeeping data for cdma FW state configuration Response message */

  rf_cdma_dac_cal_cfg_rsp_data_type dac_cal_rsp;
  /*!< Housekeeping data for cdma FW DAC cal response message */

  rf_cdma_ic_update_rsp_data_type ic_update_rsp;
  /*!< Housekeeping data for cdma iC Update response message */

  rf_cdma_rxagc_trk_ind_rsp_data_type rxagc_trk_ind;
  /*!< Housekeeping data for cdma RxAGC Track indicator message message */

  rf_cdma_rxagc_logging_data_type rxagc_logging;
  /*!< Structure to store housekeeping data for RxAGC logging message */

  rf_cdma_ept_iq_capture_rsp_data_type iq_capture_rsp;
  /*!< Housekeeping data for cdma IQ Capture response message */

  rf_cdma_pilot_meas_config_data_type pilot_meas_cfg;
  /*!< Structure to store housekeeping data for Pilot meas config resp messg */

  rf_cdma_pilot_meas_stop_stream_data_type pilot_meas_stop_stream;
  /*!< Structure to store housekeeping data for Pilot meas stop resp messg */

  rf_cdma_xpt_config_rsp_data_type xpt_cfg_rsp;
  /*!< Structure to store housekeeping data for XPT config resp messg */

  rf_cdma_xpt_trans_compl_rsp_data_type xpt_trans_compl_cfg_rsp;
  /*!< Structure to store housekeeping data for XPT trans compl resp messg */

  rf_cdma_stop_oqpch_search_data_type stop_oqpch_search_rsp;
  /*!< Structure to store housekeeping data for stop OQPCH Search resp messg */
} rf_cdma_fwrsp_data_type;


#endif /* RF_CDMA_FWRSP_H */



