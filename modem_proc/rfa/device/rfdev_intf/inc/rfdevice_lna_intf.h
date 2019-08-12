#ifndef RFDEVICE_LNA_INTF_H
#define RFDEVICE_LNA_INTF_H
/*!
   @file
   rfdevice_lna_intf.h

   @brief

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_lna_intf.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
12/08/14   vv    Initial version
==============================================================================*/

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_cmn_type_defs.h"
#include "rfdevice_lna.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   rfm_device_enum_type rfm_device;
   rfcom_mode_enum_type mode;
   rfcom_band_type_u band;
   rfdevice_lna_trigger_enum_type  trigger_type;
   rfdevice_lna_gain_enum_type lna_gain;
   int8 system_gain;
}rfdevice_lna_config_type;


boolean rfdevice_lna_set_band_map
(
  void *device_ptr,
  rfdevice_lna_config_type *lna_cfg_p,
  int32 *data_ptr, 
  uint8 size
);

boolean rfdevice_lna_wakeup   
(
  void *device_ptr,
  rfdevice_lna_config_type *lna_cfg_p,
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  int16 script_timing
);

boolean rfdevice_lna_sleep   
(
  void *device_ptr,
  rfdevice_lna_config_type *lna_cfg_p,
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  int16 script_timing
);

boolean rfdevice_lna_trigger   
(
  void *device_ptr,
  rfdevice_lna_config_type *lna_cfg_p,
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  int16 script_timing
);

boolean rfdevice_lna_set_gain   
(
  void *device_ptr,
  rfdevice_lna_config_type *lna_cfg_p,
  void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  int16 script_timing
);


boolean rfdevice_get_lna_gain_index 
(
  void *device_ptr,
  rfdevice_lna_config_type *lna_cfg_p,
  rfdevice_lna_gain_enum_type *lna_gain
);

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_LNA_INTF_H */


