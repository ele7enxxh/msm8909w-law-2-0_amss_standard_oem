#ifndef __RFDEVICE_TRX_LTE_TX_ADAPTER_H_INCL__
#define __RFDEVICE_TRX_LTE_TX_ADAPTER_H_INCL__

/*!
   @file
   rfdevice_trx_lte_tx_adapter.h

   @brief

*/

/*===========================================================================

  Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

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


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_lte_tx_adapter.h#1 $

when       who    what, where, why
--------   ---   -------------------------------------------------------------------
05/20/15   dps    Added support for power logging using rfdevice_wtr_config_bitmask 
12/10/14   aks    Added API to get max BW supported by Tx port
08/29/14   aks    Compiler Warning fix 
08/26/14   aks    Updated get_tune_script to accept cell_idx as an argument
08/18/14   aks    Added API to get Tx tune as preload and trigger script  
08/14/14   aks    Updated Doxygen Comments 
08/01/14   ndb    Added support for extended EARFCN for LTE
08/01/14   aks    Updated Tx Disable to accept rf_band as an input argument
07/07/14   aks    Updated cmd_dispatch to accept rf_band as an input argument
07/01/14   aks    Updated tune_to_chan to accept a flag to disable spur mitigation
04/29/14   xsh    Modify the fbrx rsb cal API to ensure calibrated rsb cal can pass back
01/02/14   aks    Removed set_port from rfdevice_trx_lte_tx class 
11/20/13   aks    Device driver updates to enable AFC on all bands
11/15/13   aks    Device driver updates to enable AFC
11/07/13   rp     Added and API to return common device class pointer.
10/25/13   aka    LTE FB path RSB cal
09/10/13   aks    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

#ifdef FEATURE_LTE
#include "rfdevice_lte_type_defs.h"
#include "rfdevice_trx_lte_tx.h"

class rfdevice_trx_lte_tx_adapter:public rfdevice_trx_lte_tx
{
  public:

  /* Constructor */
  rfdevice_trx_lte_tx_adapter(
    rfdevice_rxtx_common_class* cmn_rxtx_dev,
    rfdevice_trx_phy_path_enum_type phy_path
  );

  /* Destructor */
  ~rfdevice_trx_lte_tx_adapter();

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_init(
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean set_mode(
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean config_band(
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean tune_to_chan (
    rfcom_lte_band_type rf_band, 
    uint32 freq, 
    int32 rx_freq_error,
    rfdevice_dpd_enum_type dpd_state,
    rfdevice_lte_script_data_type* script_data_ptr,
    boolean ftm_mode,
    rfm_device_enum_type rfm_device,
    boolean disable_spur_mitigation,
    rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_tune_script (
    rfcom_lte_band_type rf_band, 
    uint32 freq_khz, 
    rfcom_lte_bw_type rfcom_bw,
    int32 tx_freq_error_hz,
    rfdevice_dpd_enum_type dpd_state,
    rflte_mc_cell_idx_type cell_idx,
    rfdevice_meas_script_data_type* script_data_ptr, 
    boolean ftm_mode,
    rfm_device_enum_type rfm_device,
    boolean disable_spur_mitigation,
    rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL
  );

  /*----------------------------------------------------------------------------*/
  virtual rfdevice_tx_saw_port_type get_port_type( 
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_et_delay_comp ( 
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type rfcom_bw,
    int16* delay_comp_x10ns
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean disable(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band, 
    rfdevice_lte_script_data_type* script_data_ptr
  );

  /*----------------------------------------------------------------------------*/
  virtual int32 cmd_dispatch(
    int cmd,
    void *data,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean set_bw(
    rfcom_lte_bw_type rfcom_bw,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean set_band_data(
    rfcom_lte_band_type band,
    int32* data_array,
    uint8 array_size,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual const int8* get_bbf_resp (
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type rfcom_bw,
    rfm_device_enum_type rfm_device);

  /*----------------------------------------------------------------------------*/
  virtual boolean get_timing_info(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_tx_timing_enum_type tx_timing_type,
    int16 *timing_result
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean tune_to_default_port (
    rfcom_lte_band_type rf_band, 
    uint32 freq_khz, 
    int32 rx_freq_error_hz,
    rfdevice_dpd_enum_type dpd_state,
    rfdevice_lte_script_data_type* script_data_ptr,
    boolean ftm_mode,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_txlo_dcoc_value (
    rfcom_lte_band_type band,
    uint32 *dcoc_value,
    boolean *nv_active
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean calc_fbrx_rsb_nv_params ( 
    rfm_device_enum_type rfm_dev,
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type tx_bw,
    rfdevice_rsb_cal_data_type *rsb_data
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_fbrx_rsb_coeff (
    rfdevice_lte_rx_rsb_offset_type *rsb_data,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_stg_chan (
    rfm_device_enum_type rfm_dev, 
    rfcom_lte_band_type band, 
    rfcom_lte_earfcn_type *channel,
    boolean *data_valid_status, 
    rfdevice_lte_script_data_type* script_data_ptr 
  ); 

  /*----------------------------------------------------------------------------*/
  virtual boolean config_stg_chan ( 
    rfm_device_enum_type rfm_dev, 
    rfcom_lte_band_type band,
    uint8 *stg_is_bad_fuse,
    void *data,
    rfdevice_lte_script_data_type* script_data_ptr 
  ); 

  /*----------------------------------------------------------------------------*/
  virtual boolean save_txlo_dcoc_value ( 
    rfcom_lte_band_type band,
    uint32 dcoc_value 
  );
 
  /*----------------------------------------------------------------------------*/
  virtual boolean get_tx_pll_script(
    int32 rx_freq_error_in_hz,
    rfcom_lte_earfcn_type rf_chan_rx, 
    rfcom_lte_earfcn_type rf_chan_tx,   
    rfcom_lte_band_type rf_band,
    uint8* tx_pll_tuned_flag,   
    rfdevice_lte_script_data_type *script_data_ptr
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_max_bw_supported(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    uint32 *max_bw_khz
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean enter_critical_section();

  /*----------------------------------------------------------------------------*/
  virtual boolean leave_critical_section();

  /*----------------------------------------------------------------------------*/
  rfdevice_rxtx_common_class* get_common_device_pointer(void);

private:
  
  rfdevice_lte_tx_type* tx_device;

  rfdevice_rxtx_common_class* common_rxtx_dev;
};

#endif /* ifdef FEATURE_LTE */
#endif // __RFDEVICE_TRX_LTE_TX_ADAPTER_H_INCL__

