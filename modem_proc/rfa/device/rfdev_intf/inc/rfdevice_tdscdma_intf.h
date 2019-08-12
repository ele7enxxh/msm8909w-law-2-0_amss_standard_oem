#ifndef RFDEVICE_TDSCDMA_INTF_H
#define RFDEVICE_TDSCDMA_INTF_H

/*!
  @file
  rfdevice_tdscdma_intf.h

  @brief
  This is an abstract software interface to control or query the RF
  device for the TDSCDMA operation.

  @details
  This is a generic TDSCDMA RF device driver interface which consists
  of a collection of data and function pointers for TDSCDMA operation. The
  interface is transparent to its clients. In another word, the client
  does not need to know what physical RF devices it's talking to. The dynamic
  binding between the logical radio and the physical radio is done in RFC
  (RF Configuration) which is a combination of an RF Card and an QCT modem. 
 
  RFC knows what specific RF devices are present in an installed RF card.
  It's also aware of which specific RF device is in use for a given TDSCDMA
  RF band. Once the connection betwen the logical radio path and the physical
  radio path is determined, RFC will call the explicit RF device driver to
  update the data and function pointers structure.
 
  After the data and function pointer structure is updated, all the device
  driver data and functions within above structure are mapped to an exact RF
  device, thus allowing TDSCDMA Main Control, Core and Feature to control or
  query the RF Device.

*/

/*===========================================================================

Copyright (c) 2011 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_tdscdma_intf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/01/15   zg      Added power voting api
01/23/15   dps     Added new API for Rx RSB calibration using NV input 
09/21/14   jyu     Integrate Tx WXE support with MC/MDSP
08/28/14   shb     Deleted unused get_critical_section API 
07/17/14   ms      Sawless RX support 
4/30/14     xsh   Added the fbrx rsb cal Implementation API
04/10/14   zg      Added supporting to push band data from RFC.
11/19/13   zg      Added a new API for iRAT
09/16/13   zg      Updates to support C++ class based TDSCDMA RX and TX interface
07/19/13   sma     Added API to return if tx band uses saw or sawless port 
04/30/13   zg      Added rfdevice_tdscdma_rx_get_rsb_nv_params.
03/21/13   ms      Tuner TDET support 
03/20/13   zg      Added STG support for RSB calibration.
03/11/13   sar     Updated file for APQ, GNSS only target.
01/10/13   jyu     Added band info to each interface API and remove hard-coded 
                   band info 
08/22/12   shb     Deleted rfdevice_interface.h 
08/20/12   shb     Moved all TDSCDMA data structs from rfdevice_tdscdma_intf.h 
                   to rfdevice_tdscdma_type_defs.h 
08/16/12   zg      Added WTR device mutex protection. 
07/31/12   qzh     Rfdevice interface change to use rfm_device instead of path
07/27/12   sr      renamed rfdevice related enum types.
07/18/12   shb     Updates for compilation post merge with DIME intf
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/02/12   kb      updated API functions to support script buffers
10/03/11   jyu     Updated rfdevice MC callflow 
08/02/11   jyu     Initial version

============================================================================*/
#include "comdef.h"
#ifdef FEATURE_TDSCDMA
#include "rfa_variation.h"
#include "rfcom.h"
#include "rfdevice_intf_cmd.h"
#include "rfdevice_type_defs.h"
#include "rfdevice_cmn_intf.h"
#include "rf_buffer_intf.h"
#include "rfdevice_tdscdma_type_defs.h"
#include "rfcommon_locks.h"
#include "rfdevice_trx_tdscdma_rx.h"
#include "rfdevice_trx_tdscdma_tx.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_init
( 
  rfm_device_enum_type device,  
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_enter_mode
( 
  rfm_device_enum_type device, 
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_enable
( 
  rfm_device_enum_type device, 
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band, 
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_tune_chan
( 
  rfm_device_enum_type device, 
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  uint16 chan,
  rf_device_execution_type execution_type, 
  rf_buffer_intf *script_buffer,
  uint8 rx_lin_state
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_tune_multi_chan
( 
  rfm_device_enum_type device,  
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  uint16 *chan,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
  );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_sleep
( 
  rfm_device_enum_type device, 
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_cmd_dispatch
( 
  rfm_device_enum_type device,  
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  rfdevice_cmd_enum_type cmd, 
  void *data
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_exit
( 
  rfm_device_enum_type device,
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
 );


/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_init
( 
  rfm_device_enum_type device,
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_enter_mode
( 
  rfm_device_enum_type device, 
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  uint16 chan,
  rfdevice_meas_script_data_type* script_data_ptr
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_enable
( 
  rfm_device_enum_type device,
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  uint16 chan,
  rfdevice_meas_script_data_type* script_data_ptr
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_tune_chan
( 
  rfm_device_enum_type device,
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  uint16 chan,
  rfdevice_meas_script_data_type* script_data_ptr
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_disable
( 
  rfm_device_enum_type device,
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_cmd_dispatch
( 
  rfm_device_enum_type device, 
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  rfdevice_cmd_enum_type cmd, 
  void *data 
 );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_exit
( 
  rfm_device_enum_type device,
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
  );

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_get_stg_channel
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  uint16 *channel,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_configure_stg
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  void *data,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_disable_stg
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type, 
  rf_buffer_intf* script_buffer
); 

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_get_rsb_nv_params
( 
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  uint64   i_sq,
  uint64   q_sq,
  int64    i_q,
  int16    therm_data, 
  int16*   sin_theta_final, 
  int16*   cos_theta_final, 
  int16*   gain_inv_final
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_fbrx_get_rsb_nv_params
( 
  rfm_device_enum_type rfm_device,
  rfcom_tdscdma_band_type rf_band, 
  uint8 tx_bw,  // not used for tds, put it here for sync to other tech and for potential future use
  rfdevice_rsb_cal_data_type *rsb_data

);

/*----------------------------------------------------------------------------*/
rfdevice_tx_saw_port_type rfdevice_tdscdma_tx_get_port_type 
(
  rfcom_tdscdma_band_type rf_band, 
  rfm_device_enum_type rfm_device 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_create_rx_meas_script
(
  rfm_device_enum_type device,
  rfm_mode_enum_type mode,
  rfcom_tdscdma_band_type band,
  uint16 chan,
  rfdevice_meas_scenario_type meas_type,
  rfdevice_meas_script_data_type *script_data_ptr,
  uint8 rx_lin_state
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_set_band_data
(
  rfdevice_trx_tdscdma_rx* rx_device,
  rfcom_tdscdma_band_type band,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
rfdevice_rx_vco_path_enum_type 
rfdevice_tdscdma_rx_get_vco_path
(
  rfdevice_trx_tdscdma_rx* rx_device,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_set_band_data
(
  rfdevice_trx_tdscdma_tx* tx_device,
  rfcom_tdscdma_band_type band,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_get_rsb_coeff
( 
  rfm_device_enum_type device, 
  rfdevice_tdscdma_rx_rsb_type *coeff,
  rfdevice_tdscdma_rx_rsb_cal_data_type *nv_rsb_data
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_pwr_sleep
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type rf_band
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_rx_pwr_wakeup
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type rf_band
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_pwr_sleep
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type rf_band 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_tdscdma_tx_pwr_wakeup
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type rf_band
);


#ifdef __cplusplus
}
#endif

#endif /* FEATURE_TDSCDMA */

#endif /* RFDEVICE_TDSCDMA_INTF_H */
