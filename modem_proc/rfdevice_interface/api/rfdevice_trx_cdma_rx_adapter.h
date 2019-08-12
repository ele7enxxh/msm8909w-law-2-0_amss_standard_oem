#ifndef RFDEVICE_TRX_CDMA_RX_ADAPTER_H
#define RFDEVICE_TRX_CDMA_RX_ADAPTER_H
/*!
   @file
   rfdevice_trx_cdma_rx_adapter.h

   @brief

*/

/*===========================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_cdma_rx_adapter.h#1 $

when       who    what, where, why
--------   ---   -------------------------------------------------------------------
09/16/15   sd    Interface changes for Rx AGC reload
07/31/15   zhw   Compiler error fix
05/19/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask 
05/15/15   ry    Device interface updates for RFC re-design for split Rx/Tx
01/16/15   shb   Added get_discontinuous_rx_script()
11/13/14   zg    Added new API of get_vco_path
08/01/14   lmiao Added new multi_carrier_droop_compensation() API to support cdma mcdo
06/19/14   bm    Updated create_tune_script() interface to support SSMA
03/09/14   pk    Added interface to query DLNA support
11/13/13   shb   Corrected arg in CDMA meas script API
11/11/13   shb   Added new CDMA RX create_meas_script API
11/07/13   rp    Added and API to return common device class pointer.
08/27/13  shb/sr Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"
#ifdef FEATURE_CDMA1X
#include "rfm_cdma_power_mode_types.h"
#include "rfdevice_cdma_type_defs.h"
#include "rfdevice_trx_cdma_rx.h"

class rfdevice_trx_cdma_rx_adapter:public rfdevice_trx_cdma_rx
{

public:
  rfdevice_trx_cdma_rx_adapter(rfdevice_rxtx_common_class *cmn_rxtx_dev,
                               rfdevice_trx_phy_path_enum_type phy_path);

  virtual ~rfdevice_trx_cdma_rx_adapter();

  virtual void init();

  virtual boolean enter_critical_section( void );

  virtual boolean leave_critical_section( void );

  virtual boolean create_sleep_script(rf_buffer_intf *buf_obj_ptr,
                                      rfdevice_trx_cdma_rx* paired_device,
                                      rfdevice_rx_mode_type rx_mode,
                                      uint32 rx_bw_khz,
                                      uint32 tx_bw_khz);

  virtual boolean create_disable_diversity_script(rf_buffer_intf *buf_obj_ptr,
                                                  rfdevice_trx_cdma_rx* paired_device,
                                                  rfdevice_rx_mode_type rx_mode,
                                                  uint32 rx_bw_khz,
                                                  uint32 tx_bw_khz);

  virtual boolean create_tune_script(rfm_mode_enum_type mode,
                                     rfm_cdma_band_class_type band,
                                     rfm_cdma_chan_type chan,
                                     uint32 rx_bw_khz,
                                     uint32 tx_bw_khz,
                                     rfm_cdma_power_mode_type power_mode,
                                     rf_buffer_intf *buf_obj_ptr,  
                                     rfdevice_cdma_script_operation_type script_mode,
                                     rf_cdma_spur_cb_type rf_cdma_program_wtr_notch_cb,
                                     rfdevice_cdma_ssma_callback_data_type* cb_data,
                                     rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
                                     boolean use_alt_gain = FALSE);

  virtual boolean multi_carrier_droop_compensation(rfdevice_cdma_multi_carrier_droop_compensations_type_v2 *data,
                              uint32 bwHz);

  virtual boolean create_meas_script
  (
     rfm_mode_enum_type mode,
     rfm_cdma_band_class_type band,
     rfm_cdma_chan_type chan,
     uint32 rx_bw_khz,
     uint32 tx_bw_khz,
     rfm_cdma_power_mode_type power_mode,
     rfdevice_meas_scenario_type meas_type,
     rfdevice_meas_script_data_type *script_data_ptr,
     boolean use_alt_gain = FALSE
  );

  virtual boolean create_enable_diversity_script(rfm_mode_enum_type mode,
                                                 rfm_cdma_band_class_type band,
                                                 rfm_cdma_chan_type chan,
                                                 uint32 rx_bw_khz,
                                                 uint32 tx_bw_khz,
                                                 rfm_cdma_power_mode_type power_mode,
                                                 rf_buffer_intf *buf_obj_ptr,
                                                 rfdevice_trx_cdma_rx* paired_device,
                                                 rfdevice_rx_mode_type rx_mode,
                                                 rfdevice_operation_mode operation_mode = RFDEVICE_OPER_MODE_STANDALONE,
                                                 rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
                                                 boolean use_alt_gain = FALSE);

  virtual boolean create_lna_gain_script(rfm_cdma_band_class_type band,
                                         rfm_cdma_power_mode_type power_mode,
                                         uint32 rx_bw_khz,
                                         rfm_lna_gain_state_type gain_state,
                                         rf_buffer_intf *buf_obj_ptr,
                                         rfdevice_trx_cdma_rx* paired_device,
                                         rfdevice_rx_mode_type rx_mode );

  virtual boolean cmd_dispatch(rfdevice_cmd_enum_type cmd,
                               void *data);


  virtual boolean post_pll_lock_processing(void);

  virtual boolean ic_set_linearity(rfm_cdma_power_mode_type power_mode);

  virtual boolean is_ic_supported(void);

  virtual boolean is_dlna_supported(void);

  virtual boolean set_ic_cal_data(rfdevice_cdma_ic_thres_param_type *cal_data,
                                 rfm_cdma_band_class_type band,
                                 rfm_cdma_power_mode_type power_mode);

  virtual rfm_cdma_power_mode_type get_ic_state(void);

  virtual rfdevice_cdma_ic_cal_data_type ic_calibration(int16 power);

  virtual boolean ic_reset_jammer_detector(void);

  virtual rfdevice_cdma_ic_power_mode_config_type get_ic_pm_config(rfm_cdma_band_class_type band,
                                                                   uint32 rx_bw_khz,
                                                                   rfdevice_trx_cdma_rx* paired_device,
                                                                   rfdevice_rx_mode_type rx_mode);

  virtual boolean get_lna_phase_offsets(rfm_cdma_band_class_type band,
                                        rfm_cdma_chan_type chan,
                                        rfm_cdma_power_mode_type power_mode,
                                        rf_cdma_state_type state,
                                        uint32 rx_bw_khz,
                                        rfdevice_cdma_lna_phase_offset_type *lna_phase);

  virtual boolean set_band_data(rfm_cdma_band_class_type band,
                                int32* data_array,  
                                uint8 array_size);

  virtual boolean get_rx_on_off_script(boolean rf_on,
                                       rf_buffer_intf* script_buffer,  
                                       int16 time_delays_us);

  virtual boolean get_discontinuous_rx_script(rfm_cdma_band_class_type band,
                                              boolean rx_on,
                                              rf_buffer_intf* script_buffer);

  rfdevice_rxtx_common_class* get_common_device_pointer(void); 

  rfdevice_rx_vco_path_enum_type get_vco_path(int32* data_array,uint8 array_size);

private:

  rfdevice_cdma_rx_type *rx_device;

  rfdevice_rxtx_common_class* common_rxtx_dev;

};

#endif

#endif /* RFDEVICE_TRX_CDMA_RX_ADAPTER_H */
