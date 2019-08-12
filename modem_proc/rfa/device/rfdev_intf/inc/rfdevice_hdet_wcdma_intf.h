#ifndef RFDEVICE_HDET_WCDMA_INTF_H
#define RFDEVICE_HDET_WCDMA_INTF_H
/*!
   @file
   rfdevice_hdet_wcdma_intf.h

   @brief
   

*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_hdet_wcdma_intf.h#1 $

when       who   what, where, why
--------   ---   ------------------------------------------------------------------- 
05/31/16   sub   Fix QSEQ Max Power Dip
05/15/14   aro   Interfaces to get ADC trigger script and perform HDET read
04/25/13   rmb   Added rfdevice_hdet_wcdma_measure_temp_comp() API to do temperature compensation for QFE TDET.
03/11/13   sar   Updated file for APQ, GNSS only target.
02/14/13   ac    all the apis to take the same parameter to create the object
02/01/13   ac    added the common apis to support online event update
10/15/12   kai   Added APIs and implementations
09/26/12   vb    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_WCDMA

#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_wcdma_type_defs.h"

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

boolean rfdevice_hdet_wcdma_init
(
  rfm_device_enum_type rfm_dev, void *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfcom_wcdma_band_type rf_band, 
  uint32 tx_freq
);

boolean rfdevice_hdet_wcdma_get_mdsp_config_data
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  void *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_hdet_mdsp_config_data_type *data 
);
  
boolean rfdevice_hdet_wcdma_read_mdsp_triggered_incident
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint16 *hdet_val
);

boolean rfdevice_hdet_wcdma_tx_hdet_read
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  boolean mdsp_triggered_read, 
  uint16* hdet_val,
   void *buff_obj_ptr,
  rf_device_execution_type dev_action
);

boolean rfdevice_hdet_wcdma_tx_hdet_setup
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rf_device_execution_type dev_action,
  void* buff_obj_ptr
);

boolean rfdevice_hdet_wcdma_measure_temp_comp
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rf_device_execution_type dev_action,
  void *buff_obj_ptr
);

/*------------------------------------------------------------------------------------------------*/
boolean 
rfdevice_hdet_wcdma_get_trigger_scripts
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rf_buffer_intf* hdet_scripts,
  boolean is_ftm
);

/*------------------------------------------------------------------------------------------------*/
boolean 
rfdevice_hdet_wcdma_read_hdet
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  boolean mdsp_triggered_read,
  rfdevice_wcdma_hdet_read_type *hdet_output
);

#ifdef __cplusplus
}
#endif
#endif /* FEATURE_WCDMA */

#endif /* RFDEVICE_HDET_WCDMA_INTF_H */


