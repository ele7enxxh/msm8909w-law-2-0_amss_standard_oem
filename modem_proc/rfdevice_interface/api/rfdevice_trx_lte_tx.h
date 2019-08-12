#ifndef RFDEVICE_TRX_LTE_TX_H
#define RFDEVICE_TRX_LTE_TX_H

/*!
  @file
  rfdevice_trx_lte_tx.h

  @brief
  Base class for LTE RF device interface 

*/

/*===========================================================================

Copyright (c) 2012 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfdevice/interface/main/latest/api/rfdevice_trx_lte_tx.h#7

when       who    what, where, why
--------   ---   -------------------------------------------------------------------
07/27/15   bm     Renamed get_single_rb_scripts() to get_4fmod_scripts()
                  Added new API get_dac_backoff_value() 
06/01/15   bm     Added API to get single_rb_scripts from WTR device
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
4/29/14    xsh    Modify the fbrx rsb cal API to ensure calibrated rsb cal can pass back
01/02/14   aks    Removed set_port from rfdevice_trx_lte_tx class 
12/09/13   rp     Added an interface to retrieve common device pointer associated with 
                  given tech trx class.
11/20/13   aks    Device driver updates to enable AFC on all bands
11/15/13   aks    Device driver updates to enable AFC
11/07/13   rp     Added and API to return common device class pointer.
10/25/13   aka    LTE FB path RSB cal
08/20/13   aks    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"

#ifdef  FEATURE_LTE
#include "rfdevice_lte_type_defs.h"
#include "rfdevice_type_defs.h"
#include "rflte_ext_mc.h"

#ifdef __cplusplus
#include "rfdevice_class.h"
#include "rfdevice_rxtx_common_class.h"


class rfdevice_trx_lte_tx:public rfdevice_class 
{
  public:

  /* Constructor */
  rfdevice_trx_lte_tx();

  /*----------------------------------------------------------------------------*/
  /* Destructor */
  ~rfdevice_trx_lte_tx();

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_init ( 
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device 
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean set_mode(
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean config_band (
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device
  )=0;

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
  )=0;

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
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual rfdevice_tx_saw_port_type get_port_type(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band 
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_et_delay_comp ( 
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type rfcom_bw,
    int16* delay_comp_x10ns
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean disable(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band, 
    rfdevice_lte_script_data_type* script_data_ptr
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual int32 cmd_dispatch(
    int cmd,
    void *data,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device 
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean set_bw (
    rfcom_lte_bw_type rfcom_bw,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device 
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean set_band_data( 
    rfcom_lte_band_type band,
    int32* data_array,
    uint8 array_size,
    rfm_device_enum_type rfm_device
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual const int8* get_bbf_resp(
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type rfcom_bw,
    rfm_device_enum_type rfm_device
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_timing_info(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_tx_timing_enum_type tx_timing_type,
    int16 *timing_result
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean tune_to_default_port (
    rfcom_lte_band_type rf_band, 
    uint32 freq_khz, 
    int32 rx_freq_error_hz,
    rfdevice_dpd_enum_type dpd_state,
    rfdevice_lte_script_data_type* script_data_ptr,
    boolean ftm_mode,
    rfm_device_enum_type rfm_device
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_txlo_dcoc_value (
    rfcom_lte_band_type band,
    uint32 *dcoc_value,
    boolean *nv_active 
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean calc_fbrx_rsb_nv_params ( 
    rfm_device_enum_type rfm_dev,
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type tx_bw,
    rfdevice_rsb_cal_data_type *rsb_data
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_fbrx_rsb_coeff ( 
    rfdevice_lte_rx_rsb_offset_type *rsb_data,
    rfm_device_enum_type rfm_device
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_stg_chan(
    rfm_device_enum_type rfm_dev, 
    rfcom_lte_band_type band, 
    rfcom_lte_earfcn_type *channel,
    boolean *data_valid_status, 
    rfdevice_lte_script_data_type* script_data_ptr
  )=0; 

  /*----------------------------------------------------------------------------*/
  virtual boolean config_stg_chan(
    rfm_device_enum_type rfm_dev, 
    rfcom_lte_band_type band,
    uint8 *stg_is_bad_fuse,     
    void *data,
    rfdevice_lte_script_data_type* script_data_ptr
  )=0; 

  /*----------------------------------------------------------------------------*/
  virtual boolean save_txlo_dcoc_value ( 
    rfcom_lte_band_type band,
    uint32 dcoc_value 
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual  boolean get_tx_pll_script(
    int32 rx_freq_error_in_hz,
    rfcom_lte_earfcn_type rf_chan_rx, 
    rfcom_lte_earfcn_type rf_chan_tx,   
    rfcom_lte_band_type rf_band,
    uint8* tx_pll_tuned_flag,   
    rfdevice_lte_script_data_type *script_data_ptr
  )=0;

  virtual boolean get_max_bw_supported(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    uint32 *max_bw_khz
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean enter_critical_section() = 0;

  /*----------------------------------------------------------------------------*/
  virtual boolean leave_critical_section() = 0;

  /*----------------------------------------------------------------------------*/
  virtual rfdevice_rxtx_common_class* get_common_device_pointer() = 0;

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean tx_pwr_sleep( void )
  {
    RF_MSG( RF_ERROR, "tx_pwr_sleep: Derived class does not support this API!" );
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean tx_pwr_wakeup (void ) 
  {
    RF_MSG( RF_ERROR, "tx_pwr_wakeup: Derived class does not support this API!" );
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual
  boolean get_4fmod_scripts(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type rfcom_bw,
    rf_device_execution_type execution_type,
    rfdevice_lte_4fmod_data_type* wtr_4fmod_data
  )
  {
    RF_MSG( RF_ERROR, "get_single_rb_scripts: Derived class does not support this API!" );
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual
  boolean get_dac_backoff_value(
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type rfcom_bw,
    uint16 *dac_backoff_val
  )
  {
    RF_MSG( RF_ERROR, "get_dac_backoff: Derived class does not support this API!" );
    return FALSE;
  }

};

#else

struct rfdev_lte_tx;
typedef struct rfdev_lte_rx rfdevice_trx_lte_tx;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
*/

rfdevice_rxtx_common_class* 
rfdevice_lte_tx_get_common_device_pointer(rfdevice_trx_lte_tx* tx_device);


#endif /* #ifdef __cplusplus */
#endif /* #ifdef FEATURE_LTE */
#endif /* __RFDEVICE_TRX_LTE_TX_H_INCL__ */

