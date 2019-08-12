#ifndef RFDEVICE_TRX_CDMA_TX_ADAPTER_H
#define RFDEVICE_TRX_CDMA_TX_ADAPTER_H
/*!
   @file
   rfdevice_trx_cdma_tx_adapter.h

   @brief

*/

/*===========================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_cdma_tx_adapter.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
05/19/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask 
03/06/15   hs    Added a parameter in create_tune_script() to indicate whether the 
                 channel passed in is a Rx channel or not.
09/02/14   shb   CDMA TX Tune API update to support TX WXEfication
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
#include "rfdevice_trx_cdma_tx.h"

class rfdevice_trx_cdma_tx_adapter:public rfdevice_trx_cdma_tx
{

public:
  rfdevice_trx_cdma_tx_adapter(rfdevice_rxtx_common_class *cmn_rxtx_dev,
                               rfdevice_trx_phy_path_enum_type chain);

  virtual ~rfdevice_trx_cdma_tx_adapter();

  virtual boolean enter_critical_section( void );

  virtual boolean leave_critical_section( void );

  virtual void init();

  boolean create_tune_script(rfm_mode_enum_type mode,
                             rfm_cdma_band_class_type band,
                             rfm_cdma_chan_type chan,
                             uint32 tx_bw_khz,
                             rfdevice_dpd_enum_type dpd_state,
                             rfdevice_meas_script_data_type *script_data_ptr,
                             boolean is_chan_rx,
                             rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL);

  boolean create_sleep_script(rf_buffer_intf *buf_obj_ptr);

  boolean cmd_dispatch(rfdevice_cmd_enum_type cmd,
                       void *data);

  boolean get_lut_table(rfdevice_lut_data_type* tx_lut_data,
                     rfdevice_cdma_lut_type table_type);

  boolean read_therm(boolean mdsp_triggered_read,
                  rfdevice_cdma_therm_read_type *therm_output);

  //boolean get_therm_scripts(rfdevice_cdma_therm_scripts_type* therm_scripts);

  boolean get_therm_scripts(rf_buffer_intf *temp_comp_settings,
                            rfdevice_cdma_therm_param_type *therm_param);

  boolean setup_therm(rfcom_mode_enum_type mode);

  boolean get_therm_param(rfdevice_cdma_therm_param_type *therm_param);

  boolean get_dac_cal_iref(rfdevice_cdma_dac_cal_iref_type* iref_data);

  boolean set_band_data(rfm_cdma_band_class_type band,
                        int32* data_array,
                        uint8 array_size);

  boolean get_lut_data(rfm_cdma_band_class_type band,
                       int32* data_array,
                       uint8 array_size,
                       rfdevice_cdma_lut_type *cdma_tx_pa_lut);

  boolean get_tx_on_off_script(boolean rf_on,
                               rf_buffer_intf* script_buffer,
                               int16 delay);

  boolean tune_default_port(rfcom_mode_enum_type mode,  
                            rfm_cdma_band_class_type rf_band,
                            rfm_cdma_chan_type chan,
                            uint32 tx_bw_khz,
                            rfdevice_dpd_enum_type dpd_state,
                            rf_device_execution_type execution_type,
                            rf_buffer_intf *buf_obj_ptr);

  boolean get_et_delay_compensation(int16* delay_comp_x10ns);

  rfdevice_rxtx_common_class* get_common_device_pointer(void); 

private:

  rfdevice_cdma_tx_type *tx_device;

  rfdevice_rxtx_common_class* common_rxtx_dev;

};
#endif

#endif /* RFDEVICE_TRX_CDMA_TX_ADAPTER_H */
