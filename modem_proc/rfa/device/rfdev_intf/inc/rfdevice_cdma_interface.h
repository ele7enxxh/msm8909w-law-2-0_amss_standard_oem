#ifndef RFDEVICE_CDMA_INTERFACE_H
#define RFDEVICE_CDMA_INTERFACE_H

/*!
  @file
  rfdevice_cdma_interface.h

  @brief
  Contains prototypes and defintions of interface functions to 
  configure the RTR device.
*/

/*==============================================================================

  Copyright (c) 2011 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_cdma_interface.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/06/15   hs      Added a parameter in create_tune_script() to indicate whether the 
                   channel passed in is a Rx channel or not.
01/13/15   shb     Added rfdevice_cdma_get_discontinuous_rx_script
11/13/14   zg      Added new API for get_vco_path
09/11/14   spa     Update interface for create_wakeup_tx_script API
08/01/14   lmiao   Added new rfdevice_cdma_rx_multi_carrier_droop_compensation() wrapper to support cdma mcdo   
06/19/14   spa     Update interface for notch callback API and data
03/09/14   pk      Added interface to query DLNA support
11/13/13   shb     Corrected arg in CDMA meas script API
11/11/13   shb     Added new CDMA RX create_meas_script API
08/26/13   shb     Updates to support C++ class based CDMA RX and TX interface
07/02/13   Saul    XPT. ET delay compensation
05/29/13   spa     Added API to retrieve Rx ON/OFF scripts
03/12/13   sn      Added rfdevice_cdma_tx_tune_default_port
02/20/13   zhw     Return bus from rfdevice_cdma_get_tx_rgi_register
01/10/13   spa     Support for RFC based PA LUT mapping
01/06/13   shb     Added function to return LNA gain script per gain state in 
                   rf_buffer_intf format
12/27/12   shb     Updated rfdevice_cdma_rx_cmd_dispatch to return boolean flag
12/11/12   hdz     Enabled timing support for rfdevice_cdma_tx_get_tx_on_off_script
10/26/12   spa     Added rfdevice_cdma_tx_get_rf_on_off_script for SSBI on/off
09/10/12   spa     Removed references to v2 device scripting APIs 
09/10/12   spa     Removed unused APIs to append old style scripts to newer ones 
09/07/12   spa     Added temporary v2 APIs for rx sleep/tx sleep+wakeup 
09/05/12   spa     Added temporary v2 apis for rx tune/diversity scripting 
08/15/12   shb     Moved all function pointer and data types to new file 
                   rfdevice_cdma_typedef.h. This file now contains function
                   prototypes only
08/08/12   adk     Added support for scripting buffer interface.
08/01/12   shb     Added rfdevice_cdma_rx/tx_set_band_data
07/31/12   spa     Added Dime specific API rfdevice_cdma_tx_get_therm_scripts_v2 
07/27/12   sr      renamed rfdevice related enum types.
06/26/12   cd      Added functionality to obtain power mode configuration 
                   from device
06/15/12   adk     Corrected BB spur mitigation for WTR1605
06/07/12   shb     Added DPD support in CDMA TX tune API
03/29/12   shb     Added LNA Phase Comp support on WTR1605. Required changes in 
                   device interface to include Channel, Bandwidth and CDMA State 
03/21/12   adk     Update rxf_get_iqmc_coef_type to retrieve IQMC coeff for SHDR
03/02/12   Saul    MCDO. Rx AGC freq comp and device droop compensation framework.
03/01/12   sty     Added debug_msgs_enabled to rfdevice_cdma_rx_reco_type
02/29/12   Saul    MCDO. WTR HDR RF Cal data adjust.
02/27/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan   
02/17/12   sty     Changes for SSMA in DORB mode
02/08/12   shb     Deleted unused function pointers from CDMA RX and TX device 
                   virtual function table
02/08/12   shb     Added argument in CDMA device rx tune api to distinguish 
                   between MEAS script generation and standalone wakeup
02/04/12   cd      - Added power mode argument to HDET set up functionality   
                   - Added HDET Power mode enum 
01/20/12   shb     Updated CDMA device interface to use generic 
                   rfdevice_ssbi_write_type and rfdevice_script_type instead of
                   CDMA specific structures
01/11/12   Saul    MCDO. Passing bandwidth for IQMC coefficients query.
                   Passing mode to Tx tune script.
01/09/12   Saul    MCDO. Added iqmc_coefs and cal_adj_for_hdr
01/04/12   aro     Added interface to query LNA phase offset
01/04/12   aro     Added interface to query LNA phase offsets for current
                   configuration
12/28/11   shb     Converted IQMC coefficients to int32 from uint32
12/22/11   shb     Added API to setup_hdet
12/21/11   sty     var name changes
12/19/11   sty     Deleted unused elements in 
                   rfdevice_1x_get_rx_recommendation_args_capsule_type - they 
                   will be needed for MCDO
12/13/11   Saul    MCDO. Passing bandwidth to rc tune per mode.
12/13/11   sty     Changed var name from rfdevice_1x_rx_recommendation_type to 
                   rfdevice_cdma_rx_recommendation_type
12/13/11   Saul    MCDO. Passing rx/tx bandwidth to device driver.
12/10/11   sty     var name changed
12/07/11   aro     Added interface to query Tx RGI address
12/06/11   aro     Added function to sample and average HDET reads
12/05/11   adk     Code cleanup
12/02/11   shb     Added band and power_mode as arg in set_ic_cal_data fp to 
                   avoid using internal device state variables that may not be 
                   correct at time of cal data application
12/01/11   sty     Use rfm_cdma_band_class_type intead of rf_card_band_type
11/29/11   sty     Added members to rfdevice_1x_get_rx_recommendation_args_capsule_type
11/17/11   shb     Updated rf1x_tx_get_therm_param_func_ptr_type to have a 
                   "therm_supported" flag to selectively enable therm reads on
                   a device
11/10/11   hdz     Added support for restoring HKADC setting back to HDET 
                   after thermistor read
10/24/11   hdz     Added support for temp comp   
                   - Added rfdevice_cdma_therm_param_type
                   - Added  rfdevice_cdma_therm_scripts_type
                   - Added  rfdevice_cdma_therm_read_type
10/24/11   hdz     Added new func ptr typs for temp comp   
                   - rf1x_1x_tx_get_therm_param_func_ptr_type
                   - rf1x_tx_get_therm_scripts_func_ptr_type
                   - rf1x_tx_read_therm_func_ptr_type
                   - rf1x_tx_setup_therm_func_ptr_type
10/24/11   hdz     Added members to rfdevice_cdma_func_tbl_type   
                   - get_therm_param
                   - get_therm_scripts
                   - read_therm
                   - setup_therm
10/10/11   aro     Increased maximum script size to 600
09/26/11   adk     Removed temporary code.
09/25/11   vb      Support for DAC cal using different Irefs
09/21/11   adk     Removed obsolete code.
09/15/11   adk     Enabled SBI scripts. Executed scripts in RF SW (for now).
09/11/11   aro     Merged CDMA device interface to delete
                   rfdevice_1x_interface.h
08/05/11   shb     Added API to get HDET scripts for MDSP
08/05/11   shb     Added arg in rfdevice_cdma_read_hdet() to indicate if HKADC
                   conversion has already been triggered by MDSP
08/03/11   shb     Converted rfdevice_cdma_get_tx_timing to 
                   rfdevice_cdma_get_tx_hdet_param
08/02/11   adk     Added prototype for rfdevice_cdma_append_script2_to_script1()
07/26/11   aro     Added interface to query HDET settling time
07/25/11   aro     [1] Created HDET enable/disable/read interfaces
                   [2] Pushed PDET LPF bandwidth to device driver
07/15/11   aro     Pushed JD code analysis inside RTR driver
07/12/11   aro     Added interface to reset JD status
07/12/11   sty     Renamed rfdevice_lut_data_type_new to rfdevice_lut_data_type
07/07/11   adk     Updated SBI script generation code.
07/07/11   sty     Added prototype for rfdevice_cdma_tx_get_lut_table
07/01/11   adk     Unified SBI script generation code and legacy code.
07/01/11   aro     Added interface to perform Intelliceiver Calibration
06/29/11   aro     Added interface to set intelliciever Cal data
06/20/11   aro     Added interface to get intelliceiver Power mode
06/16/11   aro     Added member interface to check intelliceiver availability
06/08/11   shb     Added rfdevice_cdma_rx_set_linearity()
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
05/05/11   adk     Fixed function pointer mismatch and other compiler warnings.
04/29/11   adk     Updated SBI script generation functions.
04/26/11   shb     Added CDMA device diversity enable/disable interface
04/14/11   sty     Added rfdevice_cdma_create_enable_rx_script()
03/09/11   aro     Renamed interface for consistency
02/04/11   sty     [1] Renamed rfdevice_cdma_script_tx_tune to  
                   rfdevice_cdma_create_tx_tune_script
                   [2] Added rfdevice_cdma_create_rx_sleep_script and
                       rfdevice_cdma_create_tx_sleep_script 
02/03/11   aro     Initial Revision.

==============================================================================*/

#include "rfdevice_cdma_type_defs.h"
#include "rfdevice_trx_cdma_rx.h"
#include "rfdevice_trx_cdma_tx.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_rx_enter_critical_section
(
  rfdevice_trx_cdma_rx* rx_device
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_rx_leave_critical_section
(
  rfdevice_trx_cdma_rx* rx_device
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_enter_critical_section
(
  rfdevice_trx_cdma_tx* tx_device
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_leave_critical_section
(
  rfdevice_trx_cdma_tx* tx_device
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_create_rx_enable_diversity_script
(
  rfdevice_trx_cdma_rx* rx_device,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz,
  rfm_cdma_power_mode_type power_mode,
  rf_buffer_intf *buf_obj_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_create_rx_tune_script
(
  rfdevice_trx_cdma_rx* rx_device,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz,
  rfm_cdma_power_mode_type power_mode,
  rf_buffer_intf *buf_obj_ptr,  
  rfdevice_cdma_script_operation_type script_mode,
  rf_cdma_spur_cb_type rf_cdma_program_wtr_notch_cb,
  rfdevice_cdma_ssma_callback_data_type* cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_create_rx_meas_script
(
  rfdevice_trx_cdma_rx* rx_device,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz,
  rfm_cdma_power_mode_type power_mode,
  rfdevice_meas_scenario_type meas_type,
  rfdevice_meas_script_data_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_create_tx_tune_script
(
  rfdevice_trx_cdma_tx* tx_device,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  uint32 tx_bw_khz,
  rfdevice_dpd_enum_type dpd_state,
  rfdevice_meas_script_data_type *script_data,
  boolean is_chan_rx
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_create_rx_sleep_script
(
  rfdevice_trx_cdma_rx* rx_device,
  rf_buffer_intf *buf_obj_ptr,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_create_rx_disable_diversity_script
(
  rfdevice_trx_cdma_rx* rx_device,
  rf_buffer_intf *buf_obj_ptr,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_create_tx_sleep_script
(
  rfdevice_trx_cdma_tx* tx_device,
  rf_buffer_intf *buf_obj_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_rx_cmd_dispatch
(
  rfdevice_trx_cdma_rx* rx_device,
  rfdevice_cmd_enum_type cmd,
  void *data
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_rx_multi_carrier_droop_compensation
(
  rfdevice_trx_cdma_rx* rx_device,
  rfdevice_cdma_multi_carrier_droop_compensations_type_v2 *data, 
  uint32 bwHz
);

/*----------------------------------------------------------------------------*/
boolean 
rfdevice_cdma_tx_cmd_dispatch
(
  rfdevice_trx_cdma_tx* tx_device,
  rfdevice_cmd_enum_type cmd,
  void *data
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_rx_post_pll_lock_processing
(
  rfdevice_trx_cdma_rx* rx_device
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_get_tx_rgi_register
(
  rfdevice_trx_cdma_tx* tx_device,
  rfm_cdma_band_class_type band,
  uint16 *ssbi_bus,
  uint16 *reg1,
  uint16 *reg2,
  uint16 *reg3,
  uint16 *reg4
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_ic_set_linearity
(
  rfdevice_trx_cdma_rx* rx_device,
  rfm_cdma_power_mode_type power_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_is_ic_supported
(
  rfdevice_trx_cdma_rx* rx_device
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_set_ic_cal_data
(
  rfdevice_trx_cdma_rx* rx_device,
  rfdevice_cdma_ic_thres_param_type *cal_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_power_mode_type power_mode
);

/*----------------------------------------------------------------------------*/
rfm_cdma_power_mode_type
rfdevice_cdma_get_ic_state
(
  rfdevice_trx_cdma_rx* rx_device
);

/*----------------------------------------------------------------------------*/
rfdevice_cdma_ic_cal_data_type
rfdevice_cdma_ic_calibration
(
  rfdevice_trx_cdma_rx* rx_device,
  int16 power
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_ic_reset_jammer_detector
(
  rfdevice_trx_cdma_rx* rx_device
);

/*----------------------------------------------------------------------------*/
rfdevice_cdma_ic_power_mode_config_type
rfdevice_cdma_get_ic_pm_config
(
  rfdevice_trx_cdma_rx* rx_device,
  rfm_cdma_band_class_type band,
  uint32 rx_bw_khz
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_get_therm_scripts_v2
(
  rfdevice_trx_cdma_tx* tx_device,
  rf_buffer_intf *temp_comp_settings,
  rfdevice_cdma_therm_param_type *therm_param
);
/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_get_therm_param
(
  rfdevice_trx_cdma_tx* tx_device,
  rfdevice_cdma_therm_param_type *therm_param
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_read_therm
(
  rfdevice_trx_cdma_tx* tx_device,
  boolean mdsp_triggered_read,
  rfdevice_cdma_therm_read_type *therm_output
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_setup_therm
(
  rfdevice_trx_cdma_tx* tx_device,
  rfcom_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_get_dac_cal_iref
(
  rfdevice_trx_cdma_tx* tx_device,
  rfdevice_cdma_dac_cal_iref_type* iref_data
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_get_lut_table
(
  rfdevice_trx_cdma_tx* tx_device,
  rfdevice_lut_data_type* tx_lut_data,
  rfdevice_cdma_lut_type table_type
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_get_lna_phase_offsets
(
  rfdevice_trx_cdma_rx* rx_device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  rfm_cdma_power_mode_type power_mode,
  rf_cdma_state_type state,
  uint32 rx_bw_khz,
  rfdevice_cdma_lna_phase_offset_type *lna_phase
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_rx_set_band_data
(
  rfdevice_trx_cdma_rx* rx_device,
  rfm_cdma_band_class_type band,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_set_band_data
(
  rfdevice_trx_cdma_tx* tx_device,
  rfm_cdma_band_class_type band,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_get_lut_data
(
  rfdevice_trx_cdma_tx* tx_device,
  rfm_cdma_band_class_type band,
  int32* data_array,
  uint8 array_size,
  rfdevice_cdma_lut_type *cdma_tx_pa_lut
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_get_tx_on_off_script
(
  rfdevice_trx_cdma_tx* tx_device,
  boolean rf_on,
  rf_buffer_intf* script_buffer,
  int16 delay
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_rx_get_rx_on_off_script
(
  rfdevice_trx_cdma_rx* rx_device,
  boolean rf_on,
  rf_buffer_intf* script_buffer,
  int16 time_delays_us
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_get_discontinuous_rx_script
(
  rfdevice_trx_cdma_rx* rx_device,
  rfm_cdma_band_class_type band,
  boolean rx_on,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
extern boolean
rfdevice_cdma_create_rx_lna_gain_script
(
  rfdevice_trx_cdma_rx* rx_device,
  rfm_cdma_band_class_type band,
  rfm_cdma_power_mode_type power_mode,
  uint32 rx_bw_khz,
  rfm_lna_gain_state_type gain_state,
  rf_buffer_intf *buf_obj_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_tx_tune_default_port
(
  rfdevice_trx_cdma_tx *tx_device,
  rfcom_mode_enum_type mode,  
  rfm_cdma_band_class_type rf_band,
  rfm_cdma_chan_type chan,
  uint32 tx_bw_khz,
  rfdevice_dpd_enum_type dpd_state,
  rf_device_execution_type execution_type,
  rf_buffer_intf *buf_obj_ptr
);

/*----------------------------------------------------------------------------*/
boolean 
rfdevice_cdma_tx_get_et_delay_compensation
(
  rfdevice_trx_cdma_tx* tx_device,
  int16* delay_comp_x10ns
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cdma_is_dlna_supported
(
  rfdevice_trx_cdma_rx* rx_device
);

/*----------------------------------------------------------------------------*/
rfdevice_rx_vco_path_enum_type 
rfdevice_cdma_rx_get_vco_path
(
  rfdevice_trx_cdma_rx* rx_device,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cdma_rx_pwr_sleep
(
  rfdevice_trx_cdma_rx* rx_device
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cdma_rx_pwr_wakeup
(
  rfdevice_trx_cdma_rx* rx_device
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cdma_tx_pwr_sleep
(
  rfdevice_trx_cdma_tx* tx_device
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cdma_tx_pwr_wakeup
(
  rfdevice_trx_cdma_tx* tx_device
);


#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_CDMA_INTERFACE_H */

