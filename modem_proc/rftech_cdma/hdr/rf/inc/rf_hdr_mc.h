#ifndef RF_HDR_MC_H
#define RF_HDR_MC_H

/*!
  @file
  rf_hdr_mc.h
 
  @brief
  RF Driver's HDR MC interface file.

  @details
  This file exports the definitions and declartions to be used by HDR MC Layer.
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_mc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/10/16   rs      Compiler error fix for C2K disabled builds
03/16/16   vr      club 1X2G/Do2G QTA changes
09/23/14   pk      compiler warning fix
07/16/14   fh      No rf state update for cleanup script
06/28/14   fh      Added api for cleanup script
07/17/13   JJ      Remove rf_hdr_mc_helper_for_enable_rxagc_logging
07/17/13   JJ      Added func rf_hdr_mc_helper_for_enable_rxagc_logging
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
06/28/13   spa     Remove is_irat_mode flag from enable diversity API  
06/27/13   spa     Add 'is_irat_mode' flag to APIs to check if API call is in 
                   IRAT or non IRAT scenarios 
06/06/13   spa     Removed unused FWRSP handlers
05/30/13   spa     Removed unused callback APIs and structures
04/05/13   spa     Use RF dispatch for HDR Rxagc tracking and logging response
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
02/21/13   spa     Add Callback and handler for pilot meas stop stream for FW
02/19/13   spa     Handle pilot meas config response from FW 
11/28/12   aki     Added rf_hdr_mc_deinit
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
09/10/12   aro     Added HDR RxAGC logging using RF-FW Sync
09/10/12   aro     HDR callback function for SNUM dispatch
06/06/12   aro     Sequence number implementation for FW messages
06/06/12   aro     Handling of IRAT_RX_START_RSP
05/21/12   spa     Added rf_hdr_mc_tear_down() prototype 
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
04/18/12   aro     Moved FWS transition logic in upper layer
04/18/12   aro     Callback function for Rx/Tx Start/Stop interfaces
04/02/12   aro     Migrated RX_START synchronization using callback function
04/02/12   aro     Moved RxAGC convergence to a separate function to be called
                   from RX_START callback function
03/22/12   aro     Removed unsed functions and types
03/22/12   aro     Migrated handling of HDR FW responses to RF_FWRSP task
03/21/12   cd      Added handler for HDR Rx AGC logging message from HDR FW
03/13/12   aro     Moved FW State config and DAC cal response handling to RF
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
02/23/12   spa     Updated the type of band/chan to be rfm_cdma_band_class_type
                   and rfm_cdma_chan_type for rf_hdr_mc_enable  
02/09/11   APU     Add the temp API to be removed ASAP. 
                   rfm_hdr_get_highest_tx_max_power_limit_for_current_band()
11/14/11   Saul    Thermal Backoff. Made implementation more modular.
11/11/11   Saul    Thermal Backoff Support.
11/10/11   Saul    SV Limiting. Initial support.
10/31/11   aro     Added handler for HDR RxAGC trackking indication response
10/31/11   aro     Added handler functions for HDR Rx Start, Rx Stop, Tx Start,
                   and Tx stop responses
10/12/11   bmg     Added DSI SAR control to target max power limit
09/12/11   sty     Added prototype for rf_hdr_mc_sleep_rx() and 
                   rf_hdr_mc_sleep_tx()
06/23/11   sty     Added prototypes for rf_hdr_mc_enable_diversity, 
                   rf_hdr_mc_disable_diversity and rf_hdr_mc_retune
06/16/11   sty     Added prototypes for rf_hdr_mc_prep_wakeup_rx and 
                   rf_hdr_mc_exec_wakeup_rx
06/08/11   aro     Moved wakeup Tx implementation to HD MC
06/02/11   aro     Removed CB and userdata from MC interface signature
06/02/11   aro     Added HDR enable and disable implementation for MC
03/11/11   aro     Compiler Warning fix
01/18/11   sty     Added rf_hdr_mc_init
01/03/11   sty     Added C++ protection
12/16/10   aro     Removed unnecessary functions for SVDO flavor
12/09/10   aro     Initial version

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_HDR
#include "sys.h"
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rfm_async_types.h"
#include "rfm_hdr_agc_types.h"
#include "lm_types.h"
#include "rf_cmd_dispatcher.h"
#include "rf_cmd_interface.h"
#include "rfm_hdr.h"
#include "rfm_cdma_band_types.h"
#include "rf_fwrsp_task.h"
#include "rfcommon_semaphore.h"
#include "rf_cdma_mdsp.h"
#include "rf_hdr_fw_response.h"
#include "rf_dispatch_snum.h"
#include "rf_cdma_data.h"
#include "rfc_cdma.h"


/* To be removed */
#include "rfcom.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data from HDR RxAGC Log Buffer Index 
message */
typedef union
{
  int16 buffer_index;
  /*!< Read index for the ready log buffer in HDR FW SMEM */

  int8 buffer_index_data_array[2];

} rf_hdr_rx_log_buffer_index_data_type; 

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mc_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mc_deinit
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mc_enter_mode
(
  rfm_device_enum_type device,
  void* const user_data_ptr,
  rfm_cb_handler_type cb_handler
);

/*----------------------------------------------------------------------------*/
int32
rf_hdr_mc_exit_mode
(
  rfm_device_enum_type device,
  rfm_device_enum_type to_mode
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_enable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_disable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_wakeup_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_prep_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 num_band_chan,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type  * const chan_array,
  const int32 * const carrier_index,
  const rfm_hdr_agc_acq_type* const agc_mode,
  boolean is_irat_mode,
  const rfdevice_meas_scenario_type meas_type
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_exec_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  boolean is_irat_mode,
  const rfdevice_meas_scenario_type meas_type
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_enable_diversity
(
  const rfm_device_enum_type master_device,
  const lm_handle_type master_rxlm_handle,
  const rfm_device_enum_type slave_device,
  const lm_handle_type slave_rxlm_handle
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_disable_diversity
(
  const rfm_device_enum_type slave_device,
  const lm_handle_type slave_rxlm_handle
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_retune
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 num_band_chan,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const chan_array,
  const int32 * const carrier_index,
  boolean is_irat_mode
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  boolean is_irat_mode
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_hdr_mc_sleep_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mc_rx_agc_convergence
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_rx_start_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_rx_stop_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_tx_start_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_tx_stop_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_fws_config_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_irat_rx_start_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_pilot_meas_config_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_pilot_meas_stop_stream_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_rxagc_track_ind_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_rxagc_logging_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mc_tx_update_sar_backoff
(
  void
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mc_tx_update_sv_limit
(
  int16 max_power_limit_dbm10
);

/*----------------------------------------------------------------------------*/
int16
rfm_hdr_get_highest_tx_max_power_limit_for_current_band
(
   rfcom_device_enum_type       const device,
   rfcom_agc_format_type        const format
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mc_tear_down
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mc_send_start_rx_helper 
(  
  const rfm_device_enum_type device,
  const rf_cdma_data_status_type* dev_status_r,
  const lm_handle_type rxlm_handle,
  const rfc_cdma_logical_device_params_type* logical_dev,
  boolean is_irat_mode,
  boolean is_retune
);
/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mc_send_stop_rx_helper 
(  
  const  rfm_device_enum_type device ,
  const  rf_cdma_data_status_type* dev_status_r ,
  const  lm_handle_type rxlm_handle ,
  const  rfc_cdma_logical_device_params_type* logical_dev ,
  boolean skip_rf_front_end_programming  
);



#ifdef __cplusplus
}
#endif
#endif /* FEATURE_HDR */
#endif /* RF_HDR_MC_H */
