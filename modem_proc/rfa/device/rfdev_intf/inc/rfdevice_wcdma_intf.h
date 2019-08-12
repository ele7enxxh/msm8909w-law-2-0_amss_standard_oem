#ifndef RFDEVICE_WCDMA_INTF_H
#define RFDEVICE_WCDMA_INTF_H

/*!
  @file
  rfdevice_wcdma_intf.h

  @brief
  This is an abstract software interface to control or query the RF
  device for the WCDMA operation.

  @details
  This is a generic WCDMA RF device driver interface which consists
  of a collection of data and function pointers for WCDMA operation. The
  interface is transparent to its clients. In another word, the client
  does not need to know what physical RF devices it's talking to. The dynamic
  binding between the logical radio and the physical radio is done in RFC
  (RF Configuration) which is a combination of an RF Card and an QCT modem. 
 
  RFC knows what specific RF devices are present in an installed RF card.
  It's also aware of which specific RF device is in use for a given WCDMA
  RF band. Once the connection betwen the logical radio path and the physical
  radio path is determined, RFC will call the explicit RF device driver to
  update the data and function pointers structure.
 
  After the data and function pointer structure is updated, all the device
  driver data and functions within above structure are mapped to an exact RF
  device, thus allowing WCDMA Main Control, Core and Feature to control or
  query the RF Device.

*/

/*===========================================================================

Copyright (c) 2009 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_wcdma_intf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/12/15   gh      Added APIs for therm_enable and therm_read_and_disable
06/02/15   gh/zhh  Added APIs for therm_enable and therm_read_and_disable
04/01/15   zg      Added power voting api
01/21/15   kai     Added support for Rx RSB calibration
11/13/14   zg      Added new API of get_vco_path
09/19/14   tks     Added support for db-dc spur mitigation
08/28/14   tks     Added support for tx tune - preload and trigger support
07/23/14   sma     Added commit to efs for writing RSB data once
07/16/14   sma     Added Rx RSB temp comp per gain state support
3/24/14    vbh     Added a routine to get common Tx device pointer
12/15/13   tks     Removed un-used api's in rx tune interface
12/15/13   tks     Removed old tune to channel interface 
12/04/13   tks     Added support for tx set channel
10/26/13   shb     Updated IRAT APIs to support preload
09/09/13   hm      hooking up WCDMA C++ device driver interface for WTR
07/08/13   sma     Added API for RSB cal and temp comp
06/26/13   kai     Added API to get ET delay compensation
05/21/13   rmb     Added API to get Tx PLL lock status. 
03/14/13   tks     Added API to disable single tone generator
03/12/13   sn      Added rfdevice_wcdma_tx_tune_default_port
03/11/13   sar     Updated file for APQ, GNSS only target.
02/06/13   rmb     Added support for autoa/autob optimization when building scripts.
12/18/12   ka      Added rfdevice_wcdma_tx_tune_multi_chan
12/12/12   tks     Added new interface function for get rsb coefficients 
12/12/12   tks     Added new interface function for get lna offset 
12/12/12   tks     Added new interface function for get pll lock detect
12/05/12   tks     Updated hdet_read api to accept pointer to hdet_value
11/26/12   tks     Added support for handling multiple transceiver scenario 
11/19/12   tks     Changed return type to void for set_rf_on function
11/16/12   tks     Added function prototype for cpc dtx and cpc dtx interface 
11/01/12   rmb     Fix compilation error by adding an paramater 
                   in rfdevice_wcdma_build_meas_script().
10/18/12   tks     Modified the meas script function to accept band parameter     
10/11/12   ac      Configure RF_ON signal by SSBI
10/05/12   aa      new interface for wtow measurement
09/28/12   ac      device interface to pass band to rfc_wcdma_get_device
09/28/12   swb     Add support for RFC virtual PA mapping   
09/19/12   tks     Added function prototype for rx_lna_setup
09/19/12   tks     Added function prototype for hdet setup, therm read and
                   hdet read 
09/05/12   tks     Changed device interface function prototype to use 
                   logical path instead of physical path  
08/22/12   shb     Deleted rfdevice_interface.h 
08/15/12   shb      Moved all WCDMA data structs from rfdevice_wcdma_intf.h to 
                    rfdevice_wcdma_type_defs.h  
08/02/12   shb     Added rfdevice_wcdma_rx/tx_set_band_data
07/27/12   sr      renamed rfdevice related enum types.
07/20/12   tks     Fix compile errors   
07/17/12   shb     Added rfdevice_wcdma_rx/tx_set_port_func_ptr_type in device    
                   rx and tx function tables 
07/16/12   vb      Add device interface to push port info from rfc.
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
06/21/12   tks     Updated WCDMA interface api to accept script pointer     
06/07/12   shb     Added DPD support in WCDMA TX tune API
04/24/12   dw      Add RxD SBI in W2W buffer to fix issue with CM+RxD on WTR
04/04/12   tks     Add support for Tx phase compensation.
04/04/12   kai     Added critical section protection device interface functions 
03/26/12   av      IRAT code cleanup for W and G. Remove rfdevice_meas.
02/24/12   tks     Modified CPC structure to assign separate members for tx & rx
01/05/12   shb     Updated RSB A, B coeff to be unsigned
12/22/11   ac      lna phase offset compensation
12/01/11   dw      Add support for WCDMA low power mode (DSR)
09/25/11   vb      Support for DAC cal using different Irefs
09/20/11   tks     Added Rx LNA state variable.  
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
04/20/11   jhe     Remove unused wakeup api
04/01/10   dw      Fix WCDMA band type mismatch in tune device API
04/01/11   bmg     Added extern "C" for C++ safety.
11/05/10   kguo    Added band parameter to rfdevice_wcdma_rx_rsb_type
10/20/10   vb      Grouped function pointers inside the device structure as a
                   virtual function table within the main device structure so
                   that the same table can be used for different instances of
                   the same device
10/20/10   vb      Modified all dev interface function pointers to accept a
                   pointer to the device structure as an argument  
10/16/10   vb      Added structure for DC CM. 
10/16/10   vb      Added device interface functions
10/16/10   vb      Cleanup for WCDMA device data structures 
09/27/10   ac      rsb support compatible with device merge
09/2/10    ac      rsb support 
01/21/10   xw      Lint error fixes 
10/05/09   xw      Initial version.

============================================================================*/
#include "comdef.h"
#ifdef FEATURE_WCDMA
#include "rfcom.h"
#include "rfdevice_intf_cmd.h"
#include "rfdevice_type_defs.h"
#include "rfm_path_types.h" 
#include "rfdevice_cmn_intf.h"
#include "rfcommon_locks.h"
#include "rf_buffer_intf.h"
#include "rfdevice_wcdma_type_defs.h"
#include "rfdevice_trx_wcdma_rx.h"
#include "rfdevice_trx_wcdma_tx.h"


#ifdef __cplusplus
extern "C" {
#endif

/*! Structure in RFDevice interface to use this PA lut across RF Devices */
typedef struct
{
  /*! Flag to turn ON/OFF the feature from RFC */
  boolean active;

  /*! Band to PA State to LUT mapping*/
  uint8 band_pa_lut [RFCOM_NUM_WCDMA_BANDS][RFDEVICE_MAX_PA_STATES];

} rfwcdma_rfc_based_lut_mapping_type;

/* Structure to hold PA range map as well as band */
typedef struct
{
  /* PA range map info from NV */
  uint8 pa_range_map[RFDEVICE_MAX_PA_STATES];

  /* Current band */
  rfcom_wcdma_band_type band;
} rfwcdma_pa_range_map_type;

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_init
( 
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type mode,  
  rfcom_wcdma_band_type rf_band,  
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_enter_mode
( 
  rfm_device_enum_type rfm_dev, 
  rfcom_mode_enum_type mode,
  rfcom_wcdma_band_type band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_enable
(
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band,  
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean 
rfdevice_wcdma_rx_set_band_data
(
  rfm_device_enum_type rfm_dev, 
  rfdevice_trx_wcdma_rx* rx_device,
  rfcom_wcdma_band_type band,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_set_channel
(
  rfm_device_enum_type rfm_dev, 
  rfcom_mode_enum_type rf_mode, 
  rfcom_wcdma_band_type rf_band,
  rfdevice_wcdma_rx_tune_param_type rx_params, 
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer,
  rf_wcdma_spur_cb_type rf_wcdma_program_wtr_notch_cb,
  rfdevice_wcdma_spur_param_type* cb_data,
  boolean enable_notch_support
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_sleep
( 
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band, 
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer     
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_lna_setup
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rfdevice_wcdma_lna_param_type *lna_state,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_set_lna_offset
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rfdevice_wcdma_lna_phase_offset_type *offset, 
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_compute_rsb_coeff
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rfdevice_wcdma_rx_rsb_type *coeff, 
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer,
  rfdevice_wcdma_rx_rsb_cal_data_type* nv_rsb_data
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_pll_lock_detect
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  boolean *pll_lock_status, 
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_configure_stg
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  void *data,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_get_stg_channel
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  uint16 *channel,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_get_rsb_cal_data_temp_comp
(
 rfm_device_enum_type rfm_dev, 
 rfcom_wcdma_band_type rf_band,
 rfdevice_rx_wcdma_rsb_cal_data_type *rsb_data
); 
/*----------------------------------------------------------------------------*/
boolean 
rfdevice_wcdma_rx_commit_rsb_data_to_efs(
  rfm_device_enum_type rfm_device,
  rfcom_wcdma_band_type rf_band, 
  rfdevice_rsb_cal_verfication_data_type* calibrated_list);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_get_fbrx_rsb_cal
(
 rfm_device_enum_type rfm_dev, 
 rfcom_wcdma_band_type rf_band,
 rfdevice_wcdma_bw_type tx_bw,
 rfdevice_rx_wcdma_rsb_cal_data_type *rsb_data
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_get_rsb_coeff_online_temp_comp
(
 rfm_device_enum_type rfm_dev, 
 rfcom_wcdma_band_type rf_band,
 rfcom_multi_carrier_hspa_id_type curr_bw,
 uint16 temperature,
 uint8 lna_gain_state,
 int32* a_coeff,
 int32* b_coeff,
 rfdevice_wcdma_rx_rsb_cal_data_type* nv_rsb_data
);
/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_disable_stg
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_disable
( 
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type band,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_set_band_port
(
  rfm_device_enum_type rfm_dev, 
  rfdevice_trx_wcdma_rx* rx_device,
  rfcom_wcdma_band_type rfcom_band,
  uint8 rx_band_port
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_cmd_dispatch
( 
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band, 
  rfdevice_cmd_enum_type cmd, 
  void *data 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_init
( 
  rfm_device_enum_type rfm_dev, 
  rfcom_mode_enum_type mode,
  rfcom_wcdma_band_type rf_band, 
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_enter_mode
( 
  rfm_device_enum_type rfm_dev, 
  rfcom_mode_enum_type mode,
  rfcom_wcdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_enable
( 
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_set_channel
(
  rfm_device_enum_type rfm_dev, 
  rfcom_mode_enum_type rf_mode,
  rfcom_wcdma_band_type rf_band,
  rfdevice_wcdma_tx_tune_param_type tx_params,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_tune_to_chan
(
  rfm_device_enum_type rfm_dev, 
  rfcom_mode_enum_type rf_mode,
  rfcom_wcdma_band_type rf_band,
  rfdevice_wcdma_tx_tune_param_type tx_params,
  rfdevice_meas_script_data_type* script_data_ptr
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_set_band_data
(
  rfm_device_enum_type rfm_dev, 
  rfdevice_trx_wcdma_tx* tx_device,
  rfcom_wcdma_band_type band,
  int32* data_array,
  uint8 array_size,
  uint32 tx_band_pa_lut_map
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_disable
( 
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_set_band_port
(
  rfm_device_enum_type rfm_dev, 
  rfdevice_trx_wcdma_tx* tx_device,
  rfcom_wcdma_band_type band, 
  uint8 tx_band_port,
  uint32 tx_band_pa_lut_map
); 
/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_set_rf_on
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  boolean rf_on,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
); 
/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_cmd_dispatch
( 
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band, 
  rfdevice_cmd_enum_type cmd, 
  void *data 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_therm_read
(
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band,
  uint16 *therm_value, 
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_get_cpc_dtx_val
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  boolean tx_on_off,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer 
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_get_cpc_drx_val
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  boolean rx_on_off,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer 
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_build_meas_script
( 
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band, 
  rfdevice_wcdma_meas_param_type param,
  rfdevice_meas_script_data_type* script_data_ptr,
  uint16 *current_size_autob
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_tune_default_port
( 
  rfm_device_enum_type rfm_device,
  rfdevice_trx_wcdma_tx *tx_device,
  rfcom_mode_enum_type mode,
  rfcom_wcdma_band_type band,
  uint16 chan,
  rfdevice_dpd_enum_type dpd_state,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_meas_rx_common_init 
(
 rfm_device_enum_type device, 
 rfdevice_trx_wcdma_rx* rx_device,
 rfcom_wcdma_band_type band, 
 rf_buffer_intf* script_data_ptr );

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_pll_lock_detect
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  boolean *pll_lock_status, 
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
); 

/*----------------------------------------------------------------------------*/
boolean 
rfdevice_wcdma_tx_get_et_delay_compensation
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  int16* delay_comp_x10ns
);
/*----------------------------------------------------------------------------*/
boolean
valid_trx_device_ptr_rx
(
rfdevice_trx_wcdma_rx** rx_device
);

/*----------------------------------------------------------------------------*/
boolean
valid_trx_device_ptr_tx
(
rfdevice_trx_wcdma_tx** tx_device
);

/*----------------------------------------------------------------------------*/
rfdevice_rxtx_common_class*  
rfdevice_wcdma_tx_get_common_dev_ptr
( 
  rfm_device_enum_type rfm_device,
  rfcom_wcdma_band_type rf_band
);

/*----------------------------------------------------------------------------*/
rfdevice_rx_vco_path_enum_type 
rfdevice_wcdma_rx_get_vco_path
(
  rfdevice_trx_wcdma_rx* rx_device,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_therm_enable
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_therm_read_and_disable
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  uint16* therm_val
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_pwr_sleep
(
  rfm_device_enum_type rfm_dev,   
  rfcom_wcdma_band_type rf_band
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_rx_pwr_wakeup
(
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_pwr_sleep
(
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_wcdma_tx_pwr_wakeup
(
  rfm_device_enum_type rfm_dev, 
  rfcom_wcdma_band_type rf_band
);


#ifdef __cplusplus
}
#endif
#endif /* FEATURE_WCDMA*/
#endif /* RFDEVICE_WCDMA_INTF_H */

