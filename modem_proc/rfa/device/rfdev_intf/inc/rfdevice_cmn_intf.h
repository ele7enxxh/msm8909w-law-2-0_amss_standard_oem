#ifndef RFDEVICE_CMN_INTF_H
#define RFDEVICE_CMN_INTF_H

/*!
  @file
  rfdevice_cmn_intf.h

  @brief
  This file provides common interface functions to configure the RTR/WTR device.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_cmn_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/20/15   vr      Autopin check-in
11/26/15   bp      RFA common support changes for FR25072 
11/16/15   sd      Added support to retrieve FBRX ADC Clock frequency
11/03/15   sd      Added support for debug dump of Autopin sample data
10/20/15   sd      Added API to get WTR capability for Autopin PAR captures
10/06/15   sd      Added support for AutoPin PAR
05/05/14   sma     Added ULCA 20+20 Tx LUT power offset
02/24/15   aks     Device driver updates to enable ADC Vcm Cal 
02/10/15   dps     Added support for Tx RSB calibration on WTR devices
01/05/15   aks     Added a new API for BBRx DC Cal
12/08/14   hs      Added API to set the JDET reading timer.
11/24/14   aks     Updated get_tx_lut_scripts to accept bw union as an input
10/08/14   sd      Added support for Therm enable, therm read and hdet calculation
09/03/14   rsr     Added support for FTM command to retrieve manufacture and product id of transceiver
08/15/14   sd      Added tech argument to FBRX Enable API
08/14/14   shb     Updated fbrx_setup/disable APIs to support slow_ramp ON and 
                   OFF sequence for LDO 26
08/05/14   sd      Added suppport for ILPC
07/25/14   sd      Added frequency arg to FBRX Enable API to handle band
                   dependent sequences
06/24/14   sma     Added LNA gain state for FBRX RSB  
05/29/14   shb     Added frequency arg to FBRX APIs to handle band dependent 
                   sequences
05/23/14   hm      Added API support to indicate PMIC 1V rail shutoff 
05/22/14   bm      Addded API to support RF device light sleep
04/14/14   bm      Changes to decouple GPS and WWAN device ON/OFF state changes 
03/10/14   hm      FBRX enable API enhancement to add delay after LDO enable, if configured
02/14/14   shb     Added API to retrieve FBRX RSB coefficients
02/05/14   hm      Added FBRX DC Cal enable and disable API
02/05/14   shb     Added read_therm API in WTR common class interface
12/16/13   tks     Added prototype for stg enable and disable
10/31/13   shb     Switch to C++ TRX common interface
10/11/13   shb     Added FBRX setup API. Updated FBRx enable/disable for 
                   immediate execution support
10/03/13   APU     Add support for getting new common Tx LUT.
10/01/13   cd      Renamed TxAGC APIs and added execution type param
09/30/13   shb     Added new common TXAGC APIs
08/26/13   shb     Added API to get tech obj ptr in new format
05/20/13   kai     Added new feedback path attenuation state enum
04/18/13   sty/fh  Added RFDEVICE_NO_TIME_OFFSET
12/04/12   dps     Added rfdevice_cmn_cmd_dispatch()
12/03/12   aca     DIME Dac cal
09/24/12   sr      Added initernal_device_cal interface api.  
08/15/12   shb     Moved all function pointer and data types to new file 
                   rfdevice_cmn_typedef.h. This file now contains function
                   prototypes only
08/02/12   kb      Ported CL 2585079 from nikel  
07/27/12   sr      renamed rfdevice related enum types.
07/10/12   vss     Add function ptr for retrieving device type. 
07/09/12   aak     Move Quiet mode API from device to RFC 
06/20/12   shb     Added rf_device_execution_type
06/07/12   shb     Added rfdevice_dpd_enum_type
04/11/12   shb     Added support for shadow backup/restore
03/26/12   vb      Added support for new RF device function to program DAC Iref 
03/15/12   shb     Added arg to return chip process during device creation
02/24/12   shb     Created internal dev cal struct to handle all int device cal
                   and a HDET autocal status type
02/16/12   pv      RF Common Core Device Manager Implementation.  
02/14/12   shb     Removed process/version check interface
01/20/12   tks     Added support for WTR1605 process/version check  
01/20/12   shb     Created generic rfdevice_script_type that can be used by all
                   tech APIs that need to return a SSBI script
01/16/12   aro     PA state as an argument to enable ET path function
01/13/12   aro     Added support for ET path enable for self calibration
01/13/12   aro     Documentation update
11/21/11   vb      Added RFDEVICE_IREF_INVALID in rfdevice_dac_iref_val
11/17/11   aak     Changes to implement Quiet mode  
11/07/11   aak     Changes to implement Quiet mode 
11/02/11   vb      DAC cal should not be performed for Iref less than 90uA,
                   hence reducing the number of DAC Cal points.
10/20/11   shb     Added rfdevice_ssbi_write_type for WTR1605 scripting support
09/27/11   aro     Added interface to load RC tuner calibration data
09/25/11   vb      Support for DAC cal using different Irefs
09/19/11   aro     Changed the Internal calibration error code definition
09/18/11   aro     Updated RC tuner interface to pass generic RC tuner
                   calibration data to remove RTR8600 specific interface
09/14/11   aro     Added error codes for internal calibration
09/14/11   aro     Updated RC tuner calibration interface to include pointer
                   to cal data as function argument
09/14/11   aro     Added doxygen documentation for RC tuner calibration intf
09/14/11   aro     Added internal calibration status enumeration
09/14/11   aro     Added documentation to make the file doxygen compatible
09/14/11   aro     Deleted unused variables
09/14/11   aro     Added Common device interface to perform RC tuner
                   calibration
09/13/11   aro     Updated common interface to have common function table
                   and common device data
09/13/11   aro     Updated the file with doxygen documentation
07/11/11   shb     Updated constness of rfdevice_cmn_type members for 
                   robustness
09/22/10   shb     Added pointer to common data in common device structure
09/22/10   shb     Make common device struct rfdevice_cmn an array of pointers
                   of max supported paths size
09/22/10   shb     Added rfdevice_cmn_cmd_dispatch()
09/22/10   shb     Make common command dispatch function ptr accept a ptr to 
                   rfdevice_cmn_type to access global data
08/13/10   vb      Changed datatype of rfdevice_cmn_type() for merging RTR9K device
07/27/10   shb     rfdevice_cmd_dispatch_type definition added
07/01/10   av      Initial version.

==============================================================================*/

#include "rfdevice_cmn_type_defs.h"
#include "rfdevice_rxtx_common_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
  This is a special value that tells the device API to NOT add any time offset
  to the settings being retrieved
*/
#define RFDEVICE_NO_TIME_OFFSET                                          0X7FFF

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_set_dac_iref
(
  rfdevice_rxtx_common_class* cmn_dev,
  rfdevice_dac_iref_val iref_val,
  boolean program_iref,
  uint32 *agc_gainword  
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_set_et_path
(
  rfdevice_rxtx_common_class *cmn_dev,
  rfm_mode_enum_type mode,
  rfm_fb_attenuation_state_type fb_state,
  boolean enable_et_path
);

/*----------------------------------------------------------------------------*/

rfdev_cmn_status_type 
rfdevice_cmn_power_up
(
  rfdevice_rxtx_common_class *cmn_dev,
  rfdev_cmn_vote_status_type rfdev_vote,
  rfdev_cmn_voting_type rfdev_voting
);

/*----------------------------------------------------------------------------*/

rfdev_cmn_status_type 
rfdevice_cmn_power_up_gps
(
  rfdevice_rxtx_common_class *cmn_dev,
  rfdev_cmn_vote_status_type rfdev_vote,
  rfdev_cmn_voting_type rfdev_voting
);

/*----------------------------------------------------------------------------*/

rfdev_cmn_status_type 
rfdevice_cmn_enable_wwan
(
  rfdevice_rxtx_common_class *cmn_dev,
  rfdev_cmn_vote_status_type rfdev_vote,
  rfdev_cmn_voting_type rfdev_voting
);

/*----------------------------------------------------------------------------*/

rfdev_cmn_status_type 
rfdevice_cmn_enable_gps
(
  rfdevice_rxtx_common_class *cmn_dev,
  rfdev_cmn_vote_status_type rfdev_vote,
  rfdev_cmn_voting_type rfdev_voting
);

/*----------------------------------------------------------------------------*/

boolean 
rfdevice_cmn_power_reset
(
  rfdevice_rxtx_common_class *cmn_dev,
  rfdev_cmn_power_down_type rfdev_power_down_vote
);

/*----------------------------------------------------------------------------*/

boolean
rfdevice_cmn_light_sleep 

(
  rfdevice_rxtx_common_class *cmn_dev,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer  
);

/*----------------------------------------------------------------------------*/

boolean
rfdevice_cmn_backup_state
(
  rfdevice_rxtx_common_class* cmn_dev,
  uint8* backup_token
);

/*----------------------------------------------------------------------------*/

boolean
rfdevice_cmn_restore_state
(
  rfdevice_rxtx_common_class* cmn_dev,
  uint8 backup_token
);

/*----------------------------------------------------------------------------*/

void*
rfdevice_cmn_get_tech_instance
(
  rfdevice_cmn_type* cmn_dev,
  rfm_mode_enum_type rfm_mode,
  rfdevice_chain_enum_type chain 
);

/*----------------------------------------------------------------------------*/

boolean
rfdevice_cmn_do_internal_device_cal
(
  rfdevice_rxtx_common_class* cmn_device,
  rfdevice_cmn_int_dev_cal_data_type* cal_data
);
/*----------------------------------------------------------------------------*/

boolean 
rfdevice_cmn_configure_dac_cal 
(
   rfdevice_rxtx_common_class *cmn_dev, 
   rfdevice_cmn_dac_cal_data_type* dac_cal_data
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_cmd_dispatch
(
  rfdevice_rxtx_common_class* cmn_device,
  rfdevice_cmd_enum_type cmd,
  void* data
);

/*----------------------------------------------------------------------------*/
void*
rfdevice_cmn_get_tech_device
(
  rfdevice_rxtx_common_class* cmn_dev,
  rfm_mode_enum_type rfm_mode,
  rfdevice_rxtx_enum_type rxtx, 
  rfdevice_trx_phy_path_enum_type phy_path
);

/*----------------------------------------------------------------------------*/
boolean 
rfdevice_cmn_get_tx_lut_scripts_with_bw
(
  rfdevice_rxtx_common_class* cmn_device,
  rfm_mode_enum_type tech,
  rfcom_band_type_u band_u,
  rfcom_bw_type_u bw_u,
  uint8 pa_state,
  uint8 rgi,
  rf_device_execution_type execution_type,
  rfdevice_tx_lut_data_type* lut_data,
  rfdevice_tx_power_offset_bw_type* pwr_offset_bw
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_get_tx_lut_scripts
(
  rfdevice_rxtx_common_class* cmn_device,
  rfm_mode_enum_type tech,
  rfcom_band_type_u band_u,
  uint8 pa_state,
  uint8 rgi,
  rf_device_execution_type execution_type,
  rfdevice_tx_lut_data_type* lut_data
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_get_dummy_trigger_script
(
  rfdevice_rxtx_common_class* cmn_device,
  rf_buffer_intf* dummy_trigger_script
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_get_fbrx_setup_script
(
  rfdevice_rxtx_common_class* cmn_device,
  rf_device_execution_type execution_type,
  rf_buffer_intf* fbrx_setup_script,
  uint8* fbrx_setup_t_us,
  rfdevice_fbrx_scenario_type fbrx_ldo_scenario
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_get_fbrx_enable_script
(
  rfdevice_rxtx_common_class* cmn_device,
  uint8 fbrx_gain_state,
  rf_device_execution_type execution_type,
  rf_buffer_intf* fbrx_enable_script,
  boolean add_delay_before_enable_writes,
  uint32 tx_freq_khz,
  rfm_mode_enum_type rfm_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_get_fbrx_disable_script
(
  rfdevice_rxtx_common_class* cmn_device,
  rf_device_execution_type execution_type,
  rf_buffer_intf* fbrx_disable_script,
  rfdevice_fbrx_scenario_type fbrx_ldo_scenario
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_is_cmn_tx_intf_supported
(
  rfdevice_rxtx_common_class* device
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_enable_stg
(
  rfdevice_rxtx_common_class* cmn_device,
  uint64 stg_freq, 
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  rfcom_bw_type_u bw,
  uint8 fbrx_gain_state
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_disable_stg
(
  rfdevice_rxtx_common_class* cmn_device,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer,
  uint64 stg_freq_hz,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  rfcom_bw_type_u bw,
  uint8 fbrx_gain_state
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_read_therm
(
  rfdevice_rxtx_common_class* cmn_device,
  uint16* therm_read
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_fbrx_dc_cal_enable_script
(
  rfdevice_rxtx_common_class* cmn_device,
  rf_device_execution_type execution_type,
  rf_buffer_intf* fbrx_dc_cal_enable_script,
  uint64 tx_lo_freq_hz,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  rfcom_bw_type_u bw,
  uint8 fbrx_gain_state
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_fbrx_dc_cal_disable_script
(
  rfdevice_rxtx_common_class* cmn_device,
  rf_device_execution_type execution_type,
  rf_buffer_intf* fbrx_dc_cal_disable_script,
  uint64 tx_lo_freq_hz,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  rfcom_bw_type_u bw,
  uint8 fbrx_gain_state
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_get_fbrx_rsb_coeff
(
  rfdevice_rxtx_common_class* cmn_device,
  rfm_mode_enum_type tech,
  rfcom_band_type_u band,
  uint8 fbrx_gain_state,
  rfdevice_fbrx_rsb_data_type* rsb_coeff
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_get_1v_rail_shutoff_support
(
  rfdevice_rxtx_common_class* cmn_device,
  boolean* is_shutoff_supported
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_get_fbrx_ilpc_readback_script
(
  rfdevice_rxtx_common_class* cmn_device,
  rf_device_execution_type execution_type,
  rf_buffer_intf* fbrx_ilpc_read_script
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_get_fbrx_ilpc_accum_info
(
  rfdevice_rxtx_common_class* cmn_device,
  rfdevice_fbrx_ilpc_accum_data_type* fbrx_ilpc_accum_data
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_override_fbrx_gain_state
(
  rfdevice_rxtx_common_class* cmn_device,
  rfm_mode_enum_type tech,
  rfcom_band_type_u band,
  uint8 gain_state, 
  uint8 tx_bw, 
  rf_device_execution_type execution_type,
  boolean fbrx_gain_state_override, 
  rf_buffer_intf* script_buf
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_reg_dump
(
 rfdevice_rxtx_common_class* cmn_device,
 rfdevice_reg_dump_cmd_type reg_dump_type
);

/*----------------------------------------------------------------------------*/
boolean rf_device_get_mfg_prd_ids 
(  
   rfdevice_rxtx_common_class* cmn_device, 
   uint16* mfg_id, 
   uint16* prd_id,
   uint16* revision_id   
);

/*----------------------------------------------------------------------------*/
boolean 
rfdevice_cmn_therm_enable
(
  rfdevice_rxtx_common_class* cmn_device,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buf
);

/*----------------------------------------------------------------------------*/
boolean 
rfdevice_cmn_therm_read_and_disable
(
  rfdevice_rxtx_common_class* cmn_device,
  uint16* therm_val
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_calculate_hdet_reading
(
  rfdevice_rxtx_common_class* cmn_device,
  int32 sum_i,
  int32 sum_q,
  uint64 sum_i_q_sqr,
  uint32 fbrx_accum,
  uint8 fbrx_accum_overflow,
  uint16* hdet_reading
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_rxsig_jdet_enable_set_timer
(
  rfdevice_rxtx_common_class* cmn_device,
  uint16 holdoff_time_us,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buf
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_rx_adc_vcm_cal_setup
(
  rfdevice_rxtx_common_class* cmn_device,
  rfdevice_rx_vco_path_enum_type rx_vco
);

/*----------------------------------------------------------------------------*/

boolean rfdevice_cmn_power_lpm
(
  rfdevice_rxtx_common_class *cmn_dev,
  boolean to_enable
);

boolean rfdevice_cmn_fbrx_adc_vcm_cal_setup
(
  rfdevice_rxtx_common_class* cmn_device
);

boolean rfdevice_cmn_enable_wtr_lpm_state_machine
(
  rfdevice_rxtx_common_class* cmn_device,
  boolean enable_wtr_lpm_sm
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_fbrx_autopin_enable
(
  rfdevice_rxtx_common_class* cmn_device,
  rf_device_execution_type execution_type,
  rf_buffer_intf* fbrx_autopin_script,
  uint8 fbrx_gain_state,
  uint16 capture_length,
  boolean debug_enable
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_fbrx_autopin_read_and_disable
(
  rfdevice_rxtx_common_class* cmn_device,
  rfdevice_fbrx_autopin_data_type* fbrx_autopin_data,
  boolean debug_enable,
  uint8* debug_mem
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_autopin_par_support
(
  rfdevice_rxtx_common_class* cmn_device,
  boolean* is_par_supported
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_create_auto_pin_trigger_script
(
  rfdevice_rxtx_common_class* cmn_device_ptr,
  rf_buffer_intf* script_data,
  uint8 script_type,
  uint16 capture_length,
  boolean debug_enable
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_fbrx_adc_clk_freq
(
  rfdevice_rxtx_common_class* cmn_device,
  rfm_mode_enum_type tech,
  uint16 band,
  uint16 chan,
  double *fbrx_adc_clock_freq_mhz
);

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_CMN_INTF_H */

