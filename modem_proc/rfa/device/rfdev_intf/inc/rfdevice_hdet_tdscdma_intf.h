#ifndef RFDEVICE_HDET_TDSCDMA_INTF_H
#define RFDEVICE_HDET_TDSCDMA_INTF_H
/*!
   @file
   rfdevice_hdet_tdscdma_intf.h

   @brief
   

*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_hdet_tdscdma_intf.h#1 $

when       who   what, where, why
--------   ---   ------------------------------------------------------------------- 
04/30/13   ms    Tuner temp comp support 
03/22/13   ms    Featurize TDSCDMA
02/28/13   ms    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"

#ifdef FEATURE_TDSCDMA
#include "rfdevice_tdscdma_type_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
                         Data Definitions
===========================================================================*/

/*===========================================================================
                           Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
/* function prototypes */

boolean rfdevice_hdet_tdscdma_init
(
  rfm_device_enum_type rfm_dev, void *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfcom_tdscdma_band_type rf_band, 
  uint32 tx_freq
);

boolean rfdevice_hdet_tdscdma_get_mdsp_config_data
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  void *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_hdet_mdsp_config_data_type *data 
);
  
boolean rfdevice_hdet_tdscdma_read_mdsp_triggered_incident
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint16 *hdet_val
);

boolean rfdevice_hdet_tdscdma_tx_hdet_read
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  boolean mdsp_triggered_read, 
  uint16* hdet_val,
   void *buff_obj_ptr,
  rf_device_execution_type dev_action
);

boolean rfdevice_hdet_tdscdma_tx_hdet_setup
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr,
  void*data
);

boolean rfdevice_hdet_tdscdma_calc_hdet_measurement
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rfdevice_tdscdma_calc_hdet_measurement_type*  calc_hdet_meas
);

boolean rfdevice_hdet_tdscdma_tx_hdet_script_read
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr,
  void*data
);

boolean rfdevice_hdet_tdscdma_measure_temp_comp
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf *buff_obj_ptr
);

boolean rfdevice_hdet_tdscdma_measure_temp_comp_cb
(  
   rfm_device_enum_type rfm_dev,
   rfcom_tdscdma_band_type rf_band,
   uint16 numSsbi,
   uint8 address[],
   uint8 measurement[]
);


#ifdef __cplusplus
}
#endif

#endif

#endif /* RFDEVICE_HDET_TDSCDMA_INTF_H */


