#ifndef RF_CDMA_MC_H
#define RF_CDMA_MC_H

/*!
  @file
  rf_cdma_mc.h
 
  @brief
  RF Driver's Common CDMA MC interface file.

  @details
  This file exports the definitions and declartions to be used by Common CDMA
  MC Layer.
*/

/*==============================================================================

  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_mc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/16/16   vr      1X2G QTA  
11/17/15   vr      Added dynamic txr_iq_dac setting update support 
01/27/15   vc      Propogating immediate flag for FTM AsDiv support
10/23/14   spa     Add device as arg and rename API for create ASM wakeup script
10/06/14   zhw     Call wtr sleep script before rx_stop during retune
08/14/14   sbo     Added API rf_cdma_mc_set_tuner_mode_using_dummy_script
07/17/14   spa     Added API rf_cdma_mc_update_alt_rx_path
06/28/14   fh      Added api for cleanup script 
06/19/14   spa     Added SSMA callback API, API for dyn notch update triggering
06/03/14   zhw     Save curr_band for retune before invalidating rf status
04/11/14   APU     Enabled AsDIV
03/26/14   spa     Added API rf_cdma_mc_get_btf_delay to return BTF delays
03/18/14   APU     1> Have DM do the buffer deallocation.
                   2> Centralize allocation and association to 1 place.
01/20/14   spa     Add API for immediate LNA writes in cal (debug only)  
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
07/18/13   zhw     Add AsDiv Support for setting GRFC switches
07/11/13   spa     Move APIs for mem corruptions sanity check to rf_cdma_data
07/05/13   zhw     Tuner CL NV support
07/02/13   spa     Remove script mode arg from configure front end for rx wakeup
07/01/13   APU     Call RX_STOP from 1 place and remove 
                   rf_1x_cleanup_post_measurement() because we use sleep when 
                   call meas_exit()
06/27/13   spa     Add 'is_irat_mode' flag to APIs to check if API call is in 
                   IRAT or non IRAT scenarios 
06/26/13   zhw     Tuner CL support. Register call back after Tx start
06/21/13   zhw     Tuner CL support. Remove unused data struct
06/16/13   zhw     Added tech mc API for Antenna Switch Diversity support
06/14/13   spa     Use CDMA wrapper object for antenna tuner
06/14/13   sty     deleted duplicate prototype for 
                   rf_cdma_mc_configure_front_end_for_rx_wakeup
06/13/13   zhw     Added callback func and data for Tuner CL support
06/04/13   cd      Add support to clean-up Rx AGC LNA SM events
06/05/13   APU     Removed tx_static_nv_cal_ptr from helper_for_retune() APIs.
05/29/13   spa     Added support to get Rx On/Off scripts
05/29/13   spa     Renamed configure_on_off to configure_tx_on_off
05/20/13   APU     Made RFFE APIs common to HDR and 1X.
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
04/18/13   sty/fh  Added prototype for rf_cdma_get_rf_onoff_settings
03/11/13   aro     Added cdma_deinit function to cleanup CDMA
02/25/13   zhw     Added MC CCS event cleanup API
02/22/13   aro     Migrate XPT functions to common EPT module
01/28/13   zhw     RF front end device wakeup/sleep API refactor 
01/16/13   zhw     Use NV Mapping for PA gain range during PA device wake up
01/12/13   cd      RxAGC LNA SM event configuration support
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
11/06/12   zhw     Support for RF device scripting (PA/ASM/QPOET)
10/18/12   spa     Added rf_cdma_mc_configure_on_off for Rf on/off 
09/21/12   sty     Deleted device pointer from  rf_cdma_process_wakeup_failure
05/16/12   hdz     Added rf_cdma_data_sanity_check() 
05/14/12   hdz     Moved rf_apps_cdma_cmd_enum_type to rf_cdma_apps_task.h
05/11/12   spa     Added rf_cdma_mc_is_band_chan_supported prototype
04/18/12   hdz     Added rf_apps_cmd_enum_type
03/29/12   sty     Added rf_cdma_process_wakeup_failure()
03/26/12   hdz     Added rf_cdma_update_agc()
12/13/11   aro     Check for RFM APIs to see if all required tasks are running
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
01/31/11   aro     [1] Renamed CDMA get RF WU time function
                   [2] Added function to query Max device supported
01/25/11   sty     Updated copyright info
01/25/11   sty     Added rf_cdma_mc_get_warmup_time
12/20/10   aro     Updated to make C++ include compatible
12/20/10   aro     Removed including of rfcom.h
12/17/10   aro     Removed Device Argument from CDMA Init function
12/09/10   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rex.h"
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*! Structure to hold the data to be used by the callback function used to 
create AsDiv script
*/
typedef struct
{
  rfm_device_enum_type device;

  rfm_cdma_band_class_type band;
  /*!< CDMA RFM Band class type */

  boolean immediate;
  /*!< Flag indicating if a script is to be created or immediate action is to
  be done */

} rf_cdma_asdiv_xsw_script_data;
#endif /* FEATURE_RF_ASDIV */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  A data type to hold paramters relevant while doing QTA.
*/
typedef struct
{
  int dummy;
} rfm_cdma_qta_params;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_init
(
  boolean common_init_successful,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_deinit
(
  void
);

/*----------------------------------------------------------------------------*/
int32
rf_cdma_get_warmup_time
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_required_tasks_running
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_process_wakeup_failure
(
  const rfm_device_enum_type device,   
  const rfm_mode_enum_type rf_mode,    
  const rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_update_agc
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_is_band_chan_supported
(
  const rfm_device_enum_type device,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_configure_tx_on_off
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_mode_enum_type rf_mode,
  uint32 buffer_index
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_get_rf_onoff_settings
(
  const rfm_device_enum_type device,
  const rfm_mode_enum_type rf_mode,
  const rfm_cdma_band_class_type band,
  const boolean rf_on_off,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_buffer_intf *settings_buffer
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_configure_rx_on_off
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_mode_enum_type rf_mode,
  uint32 buffer_index
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_create_rx_asm_xsw_wakeup_script
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band,
  const rfm_mode_enum_type mode,
  rf_buffer_intf *rf_event_script
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_create_rx_asm_sleep_script
(
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band,
  const rfm_mode_enum_type mode,
  rf_buffer_intf *rf_event_script
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_create_tx_asm_pa_qpoet_wakeup_script
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band,
  const rfm_mode_enum_type mode,
  rf_buffer_intf *rf_event_script
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_create_tx_asm_pa_qpoet_sleep_script
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band,
  const rfm_mode_enum_type mode,
  rf_buffer_intf *rf_event_script
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_create_rx_tranceiver_sleep_script
(   
   rfdevice_trx_cdma_rx* rx_device,
   const uint32 rx_bw_khz,
   const uint32 tx_bw_khz,  
   rf_buffer_intf* rx_tx_settings 
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_cleanup_ccs_event
(
  rfm_device_enum_type device,
  rf_cdma_event_type event_type
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_configure_front_end_for_retune 
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type new_band,
  const rfm_cdma_chan_type new_chan,
  const rfm_cdma_power_mode_type power_mode,
  const uint32 rx_bw_khz,
  const uint32 tx_bw_khz,
  const rfdevice_cdma_script_operation_type script_mode,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rf_cdma_tx_static_type *tx_static_nv_cal_ptr,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr,
  rf_cdma_state_type rf_state,
  rfm_cdma_band_class_type old_band,
  rf_buffer_intf* rx_tx_settings
) ;

/*----------------------------------------------------------------------------*/

boolean
rf_cdma_mc_configure_front_end_for_rx_sleep
(  
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const boolean is_slave_device,
  const rfm_cdma_band_class_type band,
  const uint32 script_index,
  const uint32 rx_bw_khz,
  const uint32 tx_bw_khz,
  const rfc_cdma_logical_device_params_type *logical_dev,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_configure_front_end_for_tx_sleep
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cdma_band_class_type band,
  const uint32 script_index,
  const rfc_cdma_logical_device_params_type *logical_dev,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_configure_front_end_for_rx_wakeup
(
  const rfm_device_enum_type device,
  const rfm_mode_enum_type rf_mode,
  const boolean is_slave_device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan,
  const rfm_cdma_power_mode_type power_mode,
  const uint32 script_index,
  const uint32 rx_bw_khz,
  const uint32 tx_bw_khz,
  const rfc_cdma_logical_device_params_type *logical_dev,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr,
  boolean is_irat_mode,
  const rfdevice_meas_scenario_type meas_type
) ;

/*----------------------------------------------------------------------------*/

boolean 
rf_cdma_mc_configure_front_end_for_tx_wakeup
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_mode_enum_type rf_mode,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan,
  const uint32 rx_bw_khz,
  const uint32 tx_bw_khz,
  const uint32 script_index,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rf_cdma_tx_static_type *tx_static_nv_cal_ptr,
  ant_tuner_device_tokens_type* ant_tuner_script_token_ptr
) ;

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_mc_get_samp_rate
(
  uint8 rx_lm_buf_index
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_cleanup_rx_agc_lna_ccs_events
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_mc_tuner_close_loop_meas_cb
(
  void* cb_data
);

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_set_antenna_to_position
(
  const rfm_device_enum_type device,
  const uint8 position,
  const boolean immediate ,
  void* cb_func,
  void* cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_abort_set_antenna_to_position
(
  const rfm_device_enum_type device,
  const uint8 position,
  void* cb_func,
  void* cb_data
);
#endif /* FEATURE_RF_ASDIV */

/*----------------------------------------------------------------------------*/
void
rf_cdma_reset_sw_state
(
   const rfm_device_enum_type device
) ;

/*----------------------------------------------------------------------------*/
boolean  
rf_cdma_mc_immediate_lna_write
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state
);

/*----------------------------------------------------------------------------*/

uint32*
rf_cdma_mc_get_aol_tx_dm_buffer 
(
   rfm_device_enum_type device 
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_get_btf_delay
(
  rfm_mode_enum_type curr_mode,
  rfm_device_enum_type device,
  const rf_cdma_data_status_type *dev_status_r,
  const rfc_cdma_logical_device_params_type *logical_dev,
  int16 *btf_cx8
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_trigger_dynamic_notch_update
(
  rfm_device_enum_type device,
  rfdevice_cdma_rx_reco_type ssma_reco,
  uint8 num_notch_filters_used,
  boolean clear_notch
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_trigger_dyn_txr_iq_dac_update
(
  rfm_device_enum_type device,
  const rfm_cdma_band_class_type band

);

/*----------------------------------------------------------------------------*/
void
rf_cdma_mc_ssma_notch_callback
( 
   rfdevice_cdma_ssma_callback_data_type *cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_update_alt_rx_path 
(  
  const rfm_mode_enum_type curr_mode,
  const rfm_device_enum_type device ,
  const rfm_cdma_band_class_type band,
  const boolean is_irat_mode
);

/*----------------------------------------------------------------------------*/
void rf_cdma_mc_set_tuner_mode_using_dummy_script
(  
  const rfm_device_enum_type device 
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_disable_ic
(
  const rfm_device_enum_type device   
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_enable_ic
(
  const rfm_device_enum_type device   
) ;

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_mc_get_pa_switchpoints
( 
  rfm_device_enum_type      device , 
  rfm_cdma_band_class_type  band , 
  int16                     pa_rise_swpts[] , 
  int16                     pa_fall_swpts[] , 
  int16*                    active_pa_states
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_prep_qta
(
   const rfm_device_enum_type          device,
   rfm_cdma_qta_params*                params  
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_end_qta
(
   const rfm_device_enum_type          device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_helper_for_tune_back
(
  const rfm_device_enum_type device,
  const boolean is_slave_device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  uint16 lo_tuned_to_chan,
  uint32 script_index,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz,
  rfm_cdma_power_mode_type power_mode,
  boolean is_irat_mode,
  const rfdevice_meas_scenario_type meas_type,
  const rfm_mode_enum_type rf_mode
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rf_cdma_mc_tune_back
(
  const rfm_device_enum_type master_dev,
  const rfm_mode_enum_type rf_mode
);

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_MC_H */

