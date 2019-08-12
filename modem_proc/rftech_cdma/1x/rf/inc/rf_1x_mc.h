#ifndef RF_1X_MC_H
#define RF_1X_MC_H

/*!
  @file
  rf_1x_mc.h
 
  @brief
  RF Driver's 1x MC interface file.

  @details
  This file exports the definitions and declartions to be used by 1x MC Layer.
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_mc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/10/16   rs      Compiler error fix for C2K disabled builds
03/16/16   vr      1X2G QTA 
06/28/14   fh      Added api for cleanup script
06/28/13   spa     Remove is_irat_mode flag from enable diversity API  
06/27/13   spa     Add 'is_irat_mode' flag to APIs to check if API call is in 
                   IRAT or non IRAT scenarios 
06/06/13   spa     Removed unused FWRSP handlers
05/30/13   spa     Removed unused callback APIs and structures
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/11/13   spa     Removed defs related to pa config response message (unused)
02/21/13   spa     Add Callback and handler for pilot meas stop stream for FW
02/19/13   spa     Added API to handle pilot meas config response from FW
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/21/13   nrk     Changed input param type path to device for rf_1x_mc_get_synth_lock_status
01/14/13   Saul    CDMA. 1x EPT/ET call support.
11/28/12   nrk     Added API to get RF path state and synth state.    
11/28/12   aki	   Added rf_1x_mc_deinit
11/08/12   cd      Removed Tx AGC override from MC layer
09/10/12   aro     Removed White space
09/10/12   aro     SNUM dispatch callback functions for TX_STOP, TX_START,
                   IRAT_RX_START
08/28/12   rsr     Added support for TXAGC override.
07/30/12   sty     deleted redundant rf_1x_mc_ept_capture_cb()
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
07/09/12   aro     Moved EPT responses to EPT module
06/08/12   aro     Deleted EPT IQ capture callback function
06/06/12   aro     Restored EPT Changes
06/06/12   aro     Sequence number implementation for FW messages
06/06/12   aro     Handling of IRAT_RX_START_RSP
06/06/12   aro     Temporarily reverted EPT Changes
06/05/12   anr     Initial EPT Support
05/21/12   spa     Added rf_1x_mc_tear_down() prototype 
05/09/12   aro     Updated RX_START to use Sem Pool for synchronization
04/12/12   aro     Set blocking call variable before sending the message
                   to firmware
04/11/12   aro     OQPCH update to indicate 1xL1 if the timed sleep
                   passed in FW or not
04/03/12   aro     Updated FWS config function to perform SemWait in MC
04/03/12   aro     Updated FWS config function to manage state transition in
                   FTM
04/03/12   aro     CB functions in TX_START and TX_STOP interfaces
04/03/12   aro     CB functions in RX_STOP interface
04/02/12   aro     Migrated RX_START synchronization using callback function
04/03/12   aro     Moved RxAGC convergence to a separate function to be called
                   from RX_START callback function
03/22/12   aro     Deleted unused types/functions
03/22/12   aro     Added data structure to hold FW responses data with
                   an accessor function
03/22/12   aro     Migrated 1x FW response handling to RF_FWRSP task
03/22/12   aro     Deleted old RF_TASK based dispatching of CDMA
                   firmware responses
03/13/12   aro     Moved FW State config and DAC cal response handling to RF
02/27/12   spa     Updated exec_wakeup_rx to use CDMA specific band and chan 
02/23/12   spa     Updated the type of band/chan to be rfm_cdma_band_class_type
                   and rfm_cdma_chan_type for rf_1x_mc_enable
01/05/12   aro     Added prototype for complete timed sleep function
11/10/11   Saul    SV Limiting. Initial support.
10/31/11   aro     Implemented handler functions for Tx Start and Tx 
                   stop responses
10/31/11   aro     Added handler for Rx Stop response
10/31/11   aro     Added RF Task handler function for RX_START_RSP
10/12/11   bmg     Added DSI SAR control to target max power limit
09/07/11   aro     Updated RX_STOP interface to have offline QPCH flag 
06/07/11   aro     Replaced Wakeup/Sleep Tx implementation with MC function
06/06/11   sty     Added prototypes for enable/disable and retune APIs
06/02/11   aro     Removed CB and userdata from MC interface signature
06/02/11   aro     Implemented 1x Sleep interface
06/02/11   aro     Added MC implementation of 1x execute wakeup
06/02/11   aro     Added MC implementation of 1x prep wakeup
06/02/11   aro     Added 1x Enable and Disable implementation
03/11/11   aro     Compiler Warning fix
01/18/11   sty     Added rf_1x_mc_init()
01/03/11   sty     Added C++ protection
12/20/10   aro     Removed including of rfcom.h
12/16/10   aro     [1] Moved 1x Init to make CDMA init
                   [2] Deleted unused stubs
12/09/10   aro     Initial version

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rfm_async_types.h"
#include "lm_types.h"
#include "rex.h"
#include "sys.h"
#include "rf_cmd_dispatcher.h"
#include "rf_cmd_interface.h"
#include "rfm_cdma_band_types.h"
#include "rf_fwrsp_task.h"
#include "rf_cdma_mdsp.h"
#include "rf_1x_fw_response.h"
#include "rfcom.h"
#include "rf_dispatch_snum.h"
#include "rf_cdma_data.h"
#include "rfc_cdma.h"


#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mc_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mc_deinit
(
  void
);


boolean
rf_1x_mc_enter_mode
(
  rfm_device_enum_type device,
  void* const user_data_ptr,
  rfm_cb_handler_type cb_handler
);

/*----------------------------------------------------------------------------*/
int32
rf_1x_mc_exit_mode
(
  rfm_device_enum_type device,
  rfm_mode_enum_type to_mode
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_enable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_disable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_prep_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan,
  boolean is_irat_mode
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_exec_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  boolean is_irat_mode,
  const rfdevice_meas_scenario_type meas_type
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  boolean is_irat_mode
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_timed_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 rtc_cx1_trigger,
  const boolean is_offline_qpch
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_complete_timed_sleep_rx
(
  const rfm_device_enum_type device,
  boolean timed_sleep_success,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_wakeup_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_sleep_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_retune_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan,
  boolean is_irat_mode
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_enable_diversity
(
  const rfm_device_enum_type master_dev,
  const lm_handle_type rxlm_handle_master_dev,
  const rfm_device_enum_type slave_dev,
  const lm_handle_type rxlm_handle_slave_dev
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_1x_mc_disable_diversity
(
  const rfm_device_enum_type slave_dev,
  const lm_handle_type rxlm_handle_slave_dev
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mc_rx_agc_convergence
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16
rf_1x_mc_get_total_tx_filtered_power
( 
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_rx_start_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_rx_stop_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_tx_start_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_tx_stop_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_fws_config_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_irat_rx_start_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_pilot_meas_config_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_pilot_meas_stop_stream_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mc_tx_update_sar_backoff
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mc_tear_down
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mc_reload_nv
(
  rfm_device_enum_type device, 
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mc_get_synth_lock_status
(
  rfm_device_enum_type device,
  rfcom_mode_enum_type rf_mode,
  rfcom_synth_type synth_type,
  void *is_synth_locked
);
/*----------------------------------------------------------------------------*/
boolean
rf_1x_mc_send_start_rx_helper 
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
rf_1x_mc_send_stop_rx_helper 
(
  const  rfm_device_enum_type device,
  const  rf_cdma_data_status_type* dev_status_r,
  const  lm_handle_type rxlm_handle,
  const  rfc_cdma_logical_device_params_type* logical_dev,
  uint32 rtc_cx1_trigger,
  boolean skip_rf_front_end_programming,
  boolean is_offline_qpch
  );
#ifdef __cplusplus
}
#endif
#endif /* FEATURE_CDMA1X */
#endif /* RF_1X_MC_H */
