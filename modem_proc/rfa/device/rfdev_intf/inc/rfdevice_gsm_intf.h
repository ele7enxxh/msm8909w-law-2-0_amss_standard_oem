#ifndef RFDEVICE_GSM_INTF_H
#define RFDEVICE_GSM_INTF_H

/*!
  @file
  rfdevice_gsm_intf.h

  @brief
  This is an abstract software interface to control or query the RF
  device for the GSM operation.

  @details
  This is a generic GSM RF device driver interface which consists
  of a collection of data and function pointers for GSM operation. The
  interface is transparent to its clients. In another word, the client
  does not need to know what physical RF devices it's talking to. The dynamic
  binding between the logical radio and the physical radio is done in RFC
  (RF Configuration) which is a combination of an RF Card and an QCT modem. 
 
  RFC knows what specific RF devices are present in an installed RF card.
  It's also aware of which specific RF device is in use for a given GSM
  RF band. Once the connection betwen the logical radio path and the physical
  radio path is determined, RFC will call the explicit RF device driver to
  update the data and function pointers structure.
 
  After the data and function pointer structure is updated, all the device
  driver data and functions within above structure are mapped to an exact RF
  device, thus allowing GSM Main Control, Core and Feature to control or
  query the RF Device.

*/

/*===========================================================================

Copyright (c) 2008 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_gsm_intf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/21/15   sg      Passing band info while generating GSM Tx/Rx 
                   SBI scripts to support band split across 2 WTRs
04/01/15   zg      Added power voting api
10/14/14   ec      Added wrapper for new JDET timer setting API
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status 
08/28/14   shb     Deleted unused get_critical_section API 
07/28/14   dps     Added support for LIF feature 
07/25/14   hm      Added API to indicate whether Enhanced RX is supported
07/13/14   ec      Add device APIs to build Jammer Detector scripts
06/14/14   sb      Add hooks for sawless support
05/29/14   sb      IP2 cal bug fixes, updates 
04/16/14   sb      IP2 cal Interface and functionality changes
03/26/14   sb      Add support for IP2 cal for Dime PM 
03/11/14   dps     Added rfdevice_gsm_rx_enter_mode and rfdevice_gsm_tx_enter_mode 
                   as helper functions for rfdevice_gsm_enter_mode API
02/24/14   dps     Added rfdevice_gsm_init_once API for rx burst timing 
12/20/13   dps     Removed unused APIs 
12/13/13   dps     Added wrapper API for get_trans_script 
11/12/13   dps     Updated for C++ driver conversion. Replaced void return 
                   types with booleans.
11/08/13   hm      GSM interface changes for IRAT cases to determine WXE memory segment
10/26/13   shb     Updated IRAT APIs to support preload
08/01/13   sb      Added interface chanegs to support GSM RX diversity
04/24/13   cj      Added rfdevice_gsm_rx_disable.
03/11/13   sar     Updated file for APQ, GNSS only target.
01/17/13   cj      Added rfdevice_gsm_rxonly_init to initialize target WTR
12/26/12   sb      Added helper functions for Therm read support
11/16/12   sb      Made changes to convert RF_ON to SSBI  
10/09/12   sr      Added new interface for dime IRAT.
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
08/07/12   shb     Moved all function pointer and data types to new file 
                   rfdevice_gsm_intf.h. This file now contains function
                   prototypes only
08/10/12   vrb     Updated push band port APIs to temp use global rfgsm_ds
08/07/12   sb      Made MC/Core and rfdevice changes to support refactored WTR1605 changes
08/02/12   shb     Added rfdevice_gsm_rx/tx_set_band_data
07/27/12   sr      renamed rfdevice related enum types.
07/11/12   sr      Added interfaces to push the band-port info device.
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
06/20/12   shb     Updated GSM device enter_mode and set_band APIs to accept 
                   script buffer
05/11/12   sb      Added changes for debug 
03/26/12   av      IRAT code cleanup for W and G. Remove rfdevice_meas.
09/25/11   vb      Support for DAC cal using different Irefs
09/09/11   av      Separate init once and enter_mode api for IRAT cases
08/31/11   vrb     Moving KV calibration algorithm to device driver
07/12/11   vrb     Added critical section locks for RFDevice gsm APIs
07/05/11   vrb     Dual RTR: Added rfgsm_ds as argument to GSM device intf fns
07/01/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
04/04/11   bmg     Fixed rfgsmlib_get_kv_cal_code_type to match actual use.
04/01/11   bmg     Added extern "C" for C++ safety.
03/21/11   sr      Added doxygen comments for device library interface.
03/10/11   sr      Moved Tx power related functions to new rfgsm_core_tx_power.c file  
05/12/10   jps     Added tune_rx_direct function pointer  
04/14/10   jps     Changes for GSM Tx SAW/SAW-less support 
02/25/10   sr      made changes to return the uint16 for rfgsmlib_get_env_backoff_type
01/21/10   xw      Lint error fixes  
12/28/09   sr      High/Medium lint errors fix
12/09/09   vrb     Added enum for run-time RTR rev checking to control the 
                   SBI write at end of each burst to turn off the RxPLL
11/30/09   jps     Added function pointer for therm_read 
10/26/08   sr      warnings fix
10/22/08   sr      changed the return datatype to fix warning
10/05/08   xw      Initial version.

============================================================================*/
#include "comdef.h"
#ifdef FEATURE_GSM

#include "rfdevice_gsm_type_defs.h"
#include "rfc_card_gsm.h"
#include "rfdevice_trx_gsm_rx.h"
#include "rfdevice_trx_gsm_tx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RFDEVICE_MAX_DEVICE_INSTANCES_PER_BAND 5   //This should be equal to RFC_MAX_DEVICE_INSTANCES_PER_BAND


boolean rfdevice_gsm_enter_mode
(
  rfm_device_enum_type rfm_dev,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

boolean rfdevice_gsm_rx_enter_mode
(
  rfm_device_enum_type rfm_dev,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

boolean rfdevice_gsm_tx_enter_mode
(
  rfm_device_enum_type rfm_dev,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

boolean rfdevice_gsm_set_sbi_burst_script(rfm_device_enum_type rfm_dev,rfgsmlib_sbi_burst_struct *script_infor, rf_buffer_intf *buffer);

int32 rfdevice_gsm_cmd_dispatch( rfm_device_enum_type rfm_dev, rf_path_enum_type path, rfdevice_cmd_enum_type cmd, void *data );


uint16 rfdevice_gsm_therm_read(rfm_device_enum_type rfm_dev);

boolean rfdevice_gsm_set_rx_band
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

boolean rfdevice_gsm_set_tx_band
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

boolean rfdevice_gsm_tune_rx
(
  rfm_device_enum_type rfm_dev, 
  uint16 arfcn,
  boolean enh_rx_on_flag,
  int32 enh_rx_freq_offset_hz
);

boolean rfdevice_gsm_set_rx_gain_range(rfm_device_enum_type rfm_dev, int32 rx_gain_range);

boolean rfdevice_gsm_tune_tx(rfm_device_enum_type rfm_dev, uint16 arfcn);

boolean rfdevice_gsm_set_rgi(rfm_device_enum_type rfm_dev, uint16 rgi,rfgsm_modulation_type mod_type,
                          uint8 slot_num, boolean set_script );

boolean rfdevice_gsm_enable_kv_cal(rfm_device_enum_type rfm_dev,boolean flag);


/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_execute_kv_cal(rfm_device_enum_type rfm_dev,rfgsmlib_kv_code_type *kv_value, uint8 num_iter);


/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_set_kv_cal_code(rfm_device_enum_type rfm_dev, uint16 code);

/*----------------------------------------------------------------------------*/
int16 rfdevice_gsm_compute_final_kv_cal(rfm_device_enum_type rfm_dev, int16 kv, uint16 r1);

/*----------------------------------------------------------------------------*/
int16 rfdevice_gsm_get_tx_sbi_start_delta(void);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_set_tx_phase_const(rfm_device_enum_type rfm_dev, boolean on_off, rf_buffer_intf *buffer);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_tune_rx_direct
(
   rfm_device_enum_type rfm_dev, 
   rfcom_gsm_band_type band, uint16 arfcn, 
   boolean coarse_tune_cal_flag
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_build_meas_script
(
   rfm_device_enum_type rfm_dev,
   rfdevice_gsm_meas_param_type gsm_meas_param,
   rfdevice_meas_script_data_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_set_rfc_gsm_core_ptr(rfm_device_enum_type rfm_dev,rfc_gsm_core_config_type *rfc_gsm_core_ptr);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_set_band_port(rfm_device_enum_type rfm_dev,
                                rfcom_gsm_band_type rfcom_band,
                                uint8 rx_band_port,
                                uint8 tx_band_port);


/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_rx_set_band_data(rfdevice_trx_gsm_rx *rx_device,
                                      rfcom_gsm_band_type rfcom_band,
                                      int32* data_array,
                                      uint8 array_size);

boolean rfdevice_gsm_tx_set_band_data(rfdevice_trx_gsm_tx *tx_device,
                                      rfcom_gsm_band_type rfcom_band,
                                      int32* data_array,
                                      uint8 array_size);

boolean rfdevice_gsm_rx_set_stop_script(rfm_device_enum_type rfm_dev,
                                         rf_buffer_intf *buffer,
                                         rfcom_gsm_band_type band);

boolean rfdevice_gsm_tx_set_stop_script(rfm_device_enum_type rfm_dev,
                                         rf_buffer_intf *buffer,
                                         rfcom_gsm_band_type band);

boolean rfdevice_gsm_tx_set_trans_script(rfm_device_enum_type rfm_dev,
                                         rf_buffer_intf *buffer);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_get_trans_script
(
  rfm_device_enum_type rfm_dev,
  rf_buffer_intf* script_buffers_agc_gmsk[RFDEVICE_GSM_MAX_TX_SLOTS],
  rf_buffer_intf* script_buffers_agc_8PSK[RFDEVICE_GSM_MAX_TX_SLOTS]
);

boolean rfdevice_gsm_set_start_script(rfm_device_enum_type rfm_dev, rfdevice_operation_mode oper_mode, 
	                                       rfgsmlib_sbi_burst_struct *script_infor, rf_buffer_intf *buffer, 
	                                       uint16 *buffer_mask, boolean inc_drx,
	                                       rfcom_gsm_band_type band);

boolean rfdevice_gsm_prog_cal_tx_start_script(rfm_device_enum_type rfm_dev, uint16 arfcn, int8 extcal, rf_buffer_intf *buffer);

uint16 rfdevice_gsm_get_therm_read(rfm_device_enum_type rfm_dev,rf_buffer_intf *buffer);

boolean rfdevice_gsm_rxonly_init ( rfm_device_enum_type rfm_dev,
                                    rf_device_execution_type execution_type,
                                    rf_buffer_intf* script_buffer );

boolean rfdevice_gsm_rx_disable(  rfm_device_enum_type rfm_dev,
                                rf_device_execution_type execution_type,
                                rf_buffer_intf* script_buffer );

boolean rfdevice_gsm_init_once(rfm_device_enum_type rfm_dev);


boolean rfdevice_gsm_build_ip2_cal_setup_script(rfm_device_enum_type rfm_dev, 
                                                rfcom_gsm_band_type rfcom_band, 
                                                int32 min_im2_val, 
                                                uint8 min_im2_step_idx, 
                                                boolean first_iteration,
                                                rf_buffer_intf *buffer);



boolean rfdevice_gsm_build_ip2_cal_cleanup_script(rfm_device_enum_type rfm_dev, 
                                                  rfcom_gsm_band_type rfcom_band, 
                                                  rf_buffer_intf *buffer,
                                                  uint8 dev_instance);

boolean rfdevice_gsm_build_ip2_cal_step_script( rfm_device_enum_type rfm_dev, 
                                                rfcom_gsm_band_type rfcom_band, 
                                                rf_buffer_intf **buffer_arr, 
                                                rfgsm_ip2_cal_alg_params_struct* ip2_cal_info,
                                                boolean first_iter,
                                                uint8 dev_instance,
                                                rfgsm_ip2_cal_grid_info_type *grid_info_ptr );


boolean rfdevice_gsm_get_ip2_cal_params(rfm_device_enum_type rfm_dev,
                                        boolean default_flag,
                                        rfgsm_ip2_cal_params_type* ip2_cal_params_ptr,
                                        rfcom_gsm_band_type rfcom_band);

boolean rfdevice_gsm_update_ip2_cal_results(rfm_device_enum_type rfm_dev,
                                            rfcom_gsm_band_type rfcom_band,
                                            uint8 min_im2_step_idx,
                                            int32 min_im2_val,
                                            boolean first_iter,
                                            boolean write_to_efs);

boolean rfdevice_gsm_get_sawless_support(rfm_device_enum_type rfm_dev,
                                         rfcom_gsm_band_type rfcom_band,
                                         uint8* flag);


/*----------------------------------------------------------------------------*/

boolean rfdevice_gsm_build_jdet_read_script( rfm_device_enum_type rfm_dev,
                                             rfdevice_cmn_jdet_sample_type sample, 
                                             rf_buffer_intf *script_buffer );

boolean rfdevice_gsm_enable_jdet( rfm_device_enum_type rfm_dev,
                                  uint16 holdoff_time_us,
                                  rf_device_execution_type exec_type,
                                  rf_buffer_intf *script_buffer );

boolean rfdevice_gsm_disable_jdet( rfm_device_enum_type rfm_dev,
                                   rf_device_execution_type exec_type,
                                   rf_buffer_intf *script_buffer );

boolean rfdevice_gsm_get_rx_enh_rx_support ( rfm_device_enum_type rfm_dev,
                                          boolean* is_enh_rx_supported );

/*----------------------------------------------------------------------------*/
rfdevice_rx_vco_path_enum_type rfdevice_gsm_rx_get_vco_path
(
  rfdevice_trx_gsm_rx* rx_device,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_rx_pwr_sleep
(
  rfm_device_enum_type device,
  rfcom_gsm_band_type rf_band
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_rx_pwr_wakeup
(
  rfm_device_enum_type device,
  rfcom_gsm_band_type rf_band
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_tx_pwr_sleep
(
  rfm_device_enum_type device,
  rfcom_gsm_band_type rf_band 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_gsm_tx_pwr_wakeup
(
  rfm_device_enum_type device,
  rfcom_gsm_band_type rf_band
);


#ifdef __cplusplus
}
#endif
#endif /* FEATURE_GSM */

#endif /* RFDEVICE_GSM_INTF_H */
