#ifndef RFDEVICE_HDET_LTE_INTF_H
#define RFDEVICE_HDET_LTE_INTF_H
/*!
   @file
   rfdevice_hdet_lte_intf.h

   @brief
   Interfaces for RF Upper Layer to operate HDET on the HDET Device
 
*/

/*===========================================================================

Copyright (c) 2012 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_hdet_lte_intf.h#1 $

when       who   what, where, why
--------   ---   ------------------------------------------------------------------- 
06/16/14   sbm    HDET online brinup
05/21/13   sbm    K sensor changes 
01/24/13   php    Get HDET object from device 
01/15/13   ndb    API updates for WTR1605 compatibility 
12/04/12   pv     API updates for WTR compatibility.
11/20/12   gvn    Featurize LTE for Triton  
10/20/12   kai    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FEATURE_LTE
#include "rfdevice_lte_type_defs.h"

/*===========================================================================
                         Data Definitions
===========================================================================*/

/*===========================================================================
                           Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
/* function prototypes */

boolean rfdevice_hdet_lte_init
(
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint32 tx_freq,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
);

boolean rfdevice_hdet_lte_enable
(
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_enable_type *hdet_data,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
);

boolean rfdevice_hdet_lte_disable
(
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
);

boolean rfdevice_hdet_lte_meas_temp_comp
(
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
);

boolean rfdevice_hdet_lte_measure_temp_comp_cb
(
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  qfe_script_type* temp_comp_data_ptr
);

boolean rfdevice_hdet_lte_do_full_hdet
(
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_settings_type *hdet_data,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
);

boolean rfdevice_hdet_lte_do_hdet_read
(
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_settings_type *hdet_data,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
);

boolean
rfdevice_hdet_lte_setup_script
(
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
);

boolean rfdevice_hdet_lte_get_hdet_setting
(
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
  rfdevice_lte_script_data_type* script_data_ptr,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
);

boolean rfdevice_get_hdet_lte_val
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
);


#ifdef __cplusplus
}
#endif

#endif /*FEATURE_LTE*/
#endif /* RFDEVICE_HDET_LTE_INTF_H */


