#ifndef __RFDEVICE_TRX_LTE_RX_ADAPTER_H_INCL__
#define __RFDEVICE_TRX_LTE_RX_ADAPTER_H_INCL__
/*!
   @file
   rfdevice_trx_lte_rx_adapter.h

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


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_lte_rx_adapter.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
02/23/15   smi   Updated get_tune_script
09/16/15   sd    Interface changes for Rx AGC reload
05/20/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
05/15/15   ry    Device interface updates for RFC re-design for split Rx/Tx
02/25/15   sma   Scell tear down changes for CA NC 
01/08/15   aks   Updated get_gain_default_pwr_val to accept operation_mode as an input 
12/16/14   aks   Updated exit and get_tune_script APIs to accept an array 
                 through which they can enforce retune of an active Rx chain
12/04/14   dps   Added support for NV input to RxRSB calibration 
11/13/14   zg    Added new API of get_vco_path
08/29/14   aks   Compiler Warning fix 
08/28/14   shb   Deprecated API to return critical_section_ptr
08/26/14   aks   Updated get_tune_script to accept cell_idx as an argument
08/26/14   aks   Added API to check if addition of an Rx Chain requires retune of an existing
                 Rx/Tx Chain
08/14/14   aks   Added API to get the default gain offsets for each gain state
08/14/14   aks   Updated Doxygen Comments 
08/08/14   sma   Added commit to efs for writing RSB data once 
08/01/14   ndb   Added support for extended EARFCN for LTE
07/30/14   aks   Added API to return tune Script as separate Preload and Trigger Scripts 
07/16/14   aks   Added API to get AFC adjusted Notch Frequency 
07/07/14   aks   Updated Rx Cmd Dispatch to accept rf_band as an input argument
07/07/14   aks   Updated tune_to_chan to accept Call Back API from tech driver 
                 for LTE CA Spur Mitigation
07/01/14   aks   Updated tune_to_chan to accept a flag to disable spur mitigation
07/01/14   aks   Added rx_agc_gain_state as an argument to get_rsb_coeff and calc_rsb_nv_params
06/12/14   aks   Added API to get AFC adjusted Notch Frequency
06/06/14   aks   Updated API get_lna_gain_script to accept intra_band_ca_flag 
                 as an argument
03/12/14   npi   Check for Rx intraband CA support
12/16/13   aks   Added new API to return RX_EN script 
11/24/13   aks   Added arg in device IRAT API for startup vs cleanup
11/19/13   aks   Overloaded function build_meas_script to support IRAT on Bolt+WTR3925
11/18/13   aks   Added a new API to return LNA gain script
11/07/13   rp    Added and API to return common device class pointer.
09/26/13   kab   Fix for using stg_is_bad_fuse value from actual STG involved in RSB Cal
09/18/13   php   Pick LO Div ratio based Type 1 Spur present
09/10/13   aks   Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

#ifdef FEATURE_LTE
#include "rfdevice_lte_type_defs.h"
#include "rfdevice_trx_lte_rx.h"
#include "rfdevice_rxtx_common_class.h"

class rfdevice_trx_lte_rx_adapter:public rfdevice_trx_lte_rx
{
public:

   /* Constructor */
  rfdevice_trx_lte_rx_adapter(
    rfdevice_rxtx_common_class *cmn_rxtx_dev,
    rfdevice_trx_phy_path_enum_type phy_path
  );

  /*----------------------------------------------------------------------------*/
  /* Destructor */
  ~rfdevice_trx_lte_rx_adapter();

  /*----------------------------------------------------------------------------*/
  virtual boolean rx_init(
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
  virtual boolean get_tune_script(
    rfcom_lte_band_type rf_band,
    uint32 freq_khz,
    rfcom_lte_bw_type rfcom_bw,
    rfm_device_enum_type rfm_device,
    rflte_mc_cell_idx_type cell_idx,
    boolean intra_band_CA_status,
    boolean disable_spur_mitigation,
    rfdevice_meas_script_data_type* script_data_ptr,
    rfdevice_lte_spur_mitigation_type *spur_mitigation_params,
    rfdevice_lte_spur_mitigation_cb rf_lte_spur_mitigation_cb,
    rfdevice_lte_carrier_info_type *rf_lte_carrier_info,
    rfdevice_trx_lte_rx** tuned_devices,
    rfdevice_rx_mode_type current_rx_mode,
    rfdevice_lte_lif_type* lif_params,
    rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
    boolean use_alt_gain = FALSE,
    boolean single_rx_active = FALSE
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean tune_to_chan(
    rfcom_lte_band_type rf_band,
    uint32 freq,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device,
    boolean intra_band_CA_status,
    boolean disable_spur_mitigation,
    rfdevice_trx_lte_rx** tuned_devices,
    rfdevice_rx_mode_type current_rx_mode,
    rfdevice_lte_spur_mitigation_type *spur_mitigation_params,
    rfdevice_lte_spur_mitigation_cb rf_lte_spur_mitigation_cb,
    rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
    boolean use_alt_gain = FALSE
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean disable( 
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean enable( 
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band 
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
  virtual boolean wakeup(
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean exit(
    rfcom_lte_band_type rf_band,
    rfcom_device_enum_type rfm_device,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfdevice_lte_carrier_info_type *rf_lte_carrier_info,
    boolean intra_band_ca_flag
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean set_band_data( 
    rfcom_lte_band_type band,
    int32* data_array,
    uint8 array_size,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_timing_info(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type current_lte_band,
    rfdevice_lte_rx_timing_enum_type rx_timing_type,
    int16 *timing_result
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean build_meas_script(
    rfcom_lte_band_type band,
    uint32 freq,
    rfcom_lte_bw_type rfcom_bw,
    rf_buffer_intf* buf,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean build_meas_script(
    rfcom_lte_band_type band,
    uint32 freq,
    rfcom_lte_bw_type rfcom_bw,
    rfdevice_meas_scenario_type meas_scenario,
    rfdevice_meas_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device,
    rfdevice_trx_lte_rx** tuned_devices,
    rfdevice_rx_mode_type rx_mode,
    boolean use_alt_gain = FALSE
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_gain_table(
    rfdevice_lte_get_rx_gain_state_tbl_settings_type *lna_gain_state_tbl,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_lna_gain_script(
    rfcom_lte_band_type rf_band,
    uint8 rx_agc_gain_state,
    rfdevice_lte_script_data_type* script_data_ptr,
    boolean intra_band_ca_status,
    rfdevice_trx_lte_rx** tuned_devices,
    rfdevice_rx_mode_type current_rx_mode
  );
  /*----------------------------------------------------------------------------*/
  virtual boolean set_gain_table( 
    rfdevice_lte_set_rx_gain_state_type *lna_gain_state_tbl,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_rsb_coeff( 
    rfdevice_lte_rx_rsb_offset_type *rsb_data,
    rfm_device_enum_type rfm_device,
    rfdevice_lte_spur_mitigation_type *spur_mitigation_params,
    uint8 rx_agc_gain_state,
    rfdevice_lte_rx_rsb_cal_data_type *nv_rsb_data,
    boolean use_alt_gain = FALSE
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean calc_rsb_nv_params( 
    rfm_device_enum_type rfm_dev,
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type rx_bw,
    uint64   i_sq,
    uint64   q_sq,
    int64    i_q,
    uint16   therm_data,
    int16*   sin_theta_final,
    int16*   cos_theta_final, 
    int16*   gain_inv_final,
    uint8    stg_is_bad_fuse,
    uint8    rx_agc_gain_state,
    boolean  use_alt_gain = FALSE
  );

  /*----------------------------------------------------------------------------*/
   virtual boolean commit_rsb_data_to_efs (
     rfm_device_enum_type rfm_device,
     rfcom_lte_band_type rf_band, 
     rfdevice_rsb_cal_verfication_data_type* calibrated_list);

  /*----------------------------------------------------------------------------*/
  virtual boolean set_common(
    rfcom_lte_band_type rf_band, 
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type rfm_device
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean config_stg(
    rfm_device_enum_type rfm_dev, 
    rfcom_lte_band_type band, 
    uint8 *stg_is_bad_fuse,
    void *data,
    rfdevice_lte_script_data_type* script_data_ptr 
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_stg_chan(
    rfm_device_enum_type rfm_dev, 
    rfcom_lte_band_type band, 
    rfcom_lte_earfcn_type *channel,
    boolean *data_valid_status, 
    rfdevice_lte_script_data_type* script_data_ptr 
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean disable_stg(
    rfm_device_enum_type rfm_dev,
    rfdevice_lte_script_data_type* script_data_ptr
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean enter_critical_section();

  /*----------------------------------------------------------------------------*/
  virtual boolean leave_critical_section();

  /*----------------------------------------------------------------------------*/
  rfdevice_rxtx_common_class* get_common_device_pointer(void);

  /*----------------------------------------------------------------------------*/
  virtual boolean get_rx_intra_ca_support(
    rfcom_lte_band_type band,
    rfdevice_lte_get_rx_intra_ca_info_type* ca_info
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_afc_adjusted_notch_freq(
    rfcom_lte_band_type rf_band,
    int32 tx_freq_error_hz, 
    uint8 notch_cnt,
    int32* notch_freq_list_hz,
    int32* updated_notch_freq_list_hz
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean get_gain_default_pwr_val(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfcom_lte_bw_type rfcom_bw,
    boolean intra_band_ca_status,
    rfdevice_operation_mode operation_mode, 
    rfdevice_lte_get_rx_gain_settings_type* gain_settings,
    boolean use_alt_gain = FALSE
  );

  /*----------------------------------------------------------------------------*/
  virtual boolean check_retune_reqd(
    rfcom_lte_band_type rf_band,
    uint32 freq_khz,
    rfcom_lte_bw_type rfcom_bw,
    rfm_device_enum_type rfm_device,
    boolean intra_band_CA_status,
    uint8 array_size,
    rfdevice_lte_retune_reqd_type* retune_array,
    rfdevice_lte_retune_reqd_event_type retune_event
  );

  /*----------------------------------------------------------------------------*/
  rfdevice_rx_vco_path_enum_type get_vco_path
  (
    rfcom_lte_band_type band,
    int32* data_array,
    uint8 array_size
  );

private:
  rfdevice_lte_rx_type *rx_device;

  rfdevice_rxtx_common_class* common_rxtx_dev;
};

#endif /* ifdef FEATURE_LTE */
#endif // __RFDEVICE_TRX_LTE_TX_ADAPTER_H_INCL__
