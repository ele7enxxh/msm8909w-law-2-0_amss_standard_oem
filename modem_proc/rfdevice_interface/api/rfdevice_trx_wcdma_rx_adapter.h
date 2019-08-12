#ifndef RFDEVICE_TRX_WCDMA_RX_ADAPTER_H
#define RFDEVICE_TRX_WCDMA_RX_ADAPTER_H
/*!
   @file
   rfdevice_trx_wcdma_rx_adapter.h

   @brief

*/

/*==============================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_wcdma_rx_adapter.h#1 $

when       who    what, where, why
--------   ---   ---------------------------------------------------------------
09/16/15   sd    Interface changes for Rx AGC reload
05/21/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
05/15/15   ry    Device interface updates for RFC re-design for split Rx/Tx
12/12/14   dps   Added support for NV input to RxRSB calibration
11/13/14   zg    Added new API of get_vco_path
09/19/14   tks   Added support for db-dc spur mitigation
08/28/14   shb   Deprecated API to return critical_section_ptr
07/23/14   sma   Added commit to efs for writing RSB data once 
07/16/14   sma   Added Rx RSB temp comp per gain state support
12/15/13   tks   Removed old tune to chan and tune to multi chan interface
11/11/13   tks   Added create_irat_script member function  
11/07/13   rp    Added an API to return common device class pointer.
08/13/13   hm    Initial version
==============================================================================*/

#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

#ifdef FEATURE_WCDMA
#include "rfdevice_wcdma_type_defs.h"
#include "rfdevice_trx_wcdma_rx.h"

class rfdevice_trx_wcdma_rx_adapter:public rfdevice_trx_wcdma_rx
{

public:
  rfdevice_trx_wcdma_rx_adapter(rfdevice_rxtx_common_class *cmn_rxtx_dev,
                                            rfdevice_trx_phy_path_enum_type phy_path, uint32 instanceId);

  virtual ~rfdevice_trx_wcdma_rx_adapter();

  virtual boolean init(rfm_device_enum_type rfm_dev, 
                       rf_path_enum_type path,
                       rfcom_mode_enum_type mode,
                       rf_device_execution_type execution_type,
                       rf_buffer_intf* script_buffer);

  virtual boolean enter_mode(rfm_device_enum_type rfm_dev,
                             rf_path_enum_type path,
                             rfcom_mode_enum_type mode,
                             rf_device_execution_type execution_type,
                             rf_buffer_intf* script_buffer);

  virtual boolean enable(rfm_device_enum_type rfm_dev,
                         rf_path_enum_type path,
                         rfcom_wcdma_band_type rf_band,
                         rf_device_execution_type execution_type,
                         rf_buffer_intf* script_buffer);

  virtual boolean set_channel( rfm_device_enum_type rfm_dev, 
                               rfcom_mode_enum_type rf_mode, 
                               rfcom_wcdma_band_type rf_band,
                               rfdevice_wcdma_rx_tune_param_type rx_params, 
                               rf_device_execution_type exec_type,
                               rf_buffer_intf* script_buffer,
                               rf_wcdma_spur_cb_type rf_wcdma_program_wtr_notch_cb,
                               rfdevice_wcdma_spur_param_type* cb_data,
                               boolean enable_notch_support,
                               rfdevice_trx_wcdma_rx* paired_device,
                               rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
                               boolean use_alt_gain = FALSE );

  virtual boolean sleep(rfm_device_enum_type rfm_dev,
                        rf_path_enum_type path,
                        rf_device_execution_type execution_type,
                        rf_buffer_intf* script_buffer);

  virtual int32 cmd_dispatch(rfm_device_enum_type rfm_dev,
                     	     rfcom_wcdma_band_type rf_band,
                             rf_path_enum_type path,
                             rfdevice_cmd_enum_type cmd,
                             void *data);

  virtual boolean set_port(rfm_device_enum_type rfm_dev,
                           rfcom_wcdma_band_type rfcom_band,
                           uint8 rx_band_port);

  virtual boolean set_band_data(rfm_device_enum_type rfm_dev,
                                rfcom_wcdma_band_type band,
                                int32* data_array,
                                uint8 array_size);

  virtual boolean get_lna_state(rfm_device_enum_type rfm_dev,
                                rfdevice_wcdma_lna_param_type *lna_state, 
                                rf_device_execution_type exec_type, 
                                rf_buffer_intf* script_buffer,
                                rfdevice_trx_wcdma_rx* paired_device); 

  virtual boolean build_meas_script(rfm_device_enum_type rfm_dev,
                                    rfdevice_wcdma_meas_param_type param,
                                    rf_buffer_intf* buf,
                                    uint16 *current_size_autob);

  virtual boolean create_irat_script( rfm_device_enum_type rfm_dev,
                                      rfcom_wcdma_band_type band,
                                      rfdevice_wcdma_meas_param_type param,
                                      rfdevice_meas_script_data_type* script_data_ptr,
                                      uint16 *current_size_autob,
                                      rfdevice_trx_wcdma_rx* paired_device,
                                      boolean use_alt_gain = FALSE );
 
  virtual boolean get_cpc_drx_val(rfm_device_enum_type rfm_dev,
                                  rfcom_wcdma_band_type band,
                                  boolean rx_on_off, 
                                  rf_device_execution_type exec_type,
                                  rf_buffer_intf* script_buffer);

  virtual boolean set_lna_offset(rfm_device_enum_type rfm_dev,
                                 rfdevice_wcdma_lna_phase_offset_type *offset, 
                                 rf_device_execution_type exec_type, 
                                 rf_buffer_intf* script_buffer); 

  virtual boolean get_rsb_coeff(rfm_device_enum_type rfm_dev,
                                rfdevice_wcdma_rx_rsb_type *coeff, 
                                boolean *data_valid_status,
                                rf_device_execution_type exec_type, 
                                rf_buffer_intf* script_buffer,
                                rfdevice_wcdma_rx_rsb_cal_data_type* nv_rsb_data,
                                boolean use_alt_gain = FALSE);

  virtual boolean pll_lock_detect(rfm_device_enum_type rfm_dev,
                                  rfcom_wcdma_band_type rf_band,
                                  boolean *pll_lock_status, 
                                  boolean *data_valid_status, 
                                  rf_device_execution_type exec_type, 
                                  rf_buffer_intf* script_buffer); 

  virtual boolean disable(rfm_device_enum_type rfm_dev,
                          rfcom_wcdma_band_type band, 
                          rf_device_execution_type exec_type, 
                          rf_buffer_intf* script_buffer,
                          rfdevice_trx_wcdma_rx* paired_device,
                          rfdevice_rx_mode_type rx_mode); 

  virtual boolean get_stg_chan(rfm_device_enum_type rfm_dev, 
                               rfcom_wcdma_band_type band, 
                               uint16 *channel,
                               boolean *data_valid_status, 
                               rf_device_execution_type exec_type, 
                               rf_buffer_intf* script_buffer); 

  virtual boolean config_stg(rfm_device_enum_type rfm_dev, 
                             rfcom_wcdma_band_type band, 
                             void *data,
                             rf_device_execution_type exec_type, 
                             rf_buffer_intf* script_buffer); 

  virtual boolean set_common(rfm_device_enum_type rfm_dev,
                             rfcom_wcdma_band_type rf_band,
                             rf_device_execution_type execution_type,
                             rf_buffer_intf* script_buffer);

  virtual boolean disable_stg(rfm_device_enum_type rfm_dev, 
                              rf_device_execution_type exec_type, 
                              rf_buffer_intf* script_buffer); 

  virtual boolean rsb_temp_comp(rfm_device_enum_type rfm_dev, 
                                rfcom_wcdma_band_type band,
                                rfdevice_rx_wcdma_rsb_cal_data_type *rsb_data,
                                boolean use_alt_gain = FALSE); 
 
  /*!
   @brief commit_rsb_data_to_efs
   
   @param rfm_dev: RFM Device for which RSB params are desired
   
   @param rf_band: Band in use
   
   @param calibrated_list: list of calibrated devcie, band and gain state
 
   */

  virtual boolean commit_rsb_data_to_efs (
     rfm_device_enum_type rfm_device,
     rfcom_wcdma_band_type rf_band, 
     rfdevice_rsb_cal_verfication_data_type* calibrated_list);

 
  virtual boolean rsb_coeff_temp_comp(rfm_device_enum_type rfm_dev,
                                      rfcom_wcdma_band_type band,
                                      rfcom_multi_carrier_hspa_id_type curr_bw,
                                      uint16 temperature,
                                      uint8 lna_state,
                                      int32* a_coeff,
                                      int32* b_coeff,
                                      rfdevice_wcdma_rx_rsb_cal_data_type* nv_rsb_data,
                                      boolean use_alt_gain = FALSE);
 
  virtual boolean enter_critical_section();

  virtual boolean leave_critical_section();

  virtual void init();

  rfdevice_rxtx_common_class* get_common_device_pointer(void);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Query Rx VCO path data from TRX reciever

    @param data_array
    Array of int32 containing device Rx data for band

    @param array_size
    Size of array of int32 data to be pushed to device

    @return
    VCO path number 
  */
  rfdevice_rx_vco_path_enum_type get_vco_path
  (
    int32* data_array,
    uint8 array_size
  );

private:

  rfwcdma_device_rx_type *rx_device;

  rfdevice_rxtx_common_class* common_rxtx_dev;
  
};

#endif //FEATURE_WCDMA

#endif /* RFDEVICE_TRX_WCDMA_RX_ADAPTER_H */
