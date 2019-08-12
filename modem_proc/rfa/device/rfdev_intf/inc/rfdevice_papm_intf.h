#ifndef RFDEVICE_PAPM_INTF_H
#define RFDEVICE_PAPM_INTF_H
/*!
   @file
   rfdevice_papm_intf.h

   @brief
   
*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_papm_intf.h#1 $

when       who   what, where, why
--------   ---   ------------------------------------------------------------------- 
11/19/14   vv    Added rfdevice_reg_dump() API  for debug
10/23/14   vv    Added QFE3100 support
10/21/14   px    Added RFM Device support for PAPM
05/08/13   spa   Added #def for min Tx bias voltage 
05/08/13   jyu   Added timing support for rfdevice_papm_set_mode_bias 
03/22/13   sr    Changes to make the interfaces consistent with other devices.
03/07/13   sr    Changed the "pwr_tracker" name to "papm".
11/02/12   bn    Adding LTE BW param in enable function
08/23/12   shb   Moved rfdevice_papm_mode_type to 
                 rfdevice_interface/api/rfdevice_papm.h 
07/26/12   bn    Initial release
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_papm.h"
#ifdef __cplusplus
extern "C" {
#endif

/* #def to be used by all techs when they want to set the QPOET to minimum 
bias voltage, in scenarios where minimum Tx power is required. The device driver 
internally checks for the bias voltage passed in and floors it to the minimum 
possible value for it, for example if for a particular QPOET the minimum bias 
possible is 400MV, it will have a check for a value less than 400mV being passed 
in*/
#define RFDEVICE_PAPM_MIN_TX_BIAS_VOLTAGE                                      0
/*===========================================================================
                         Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
/* function prototypes */


boolean rfdevice_papm_enable
(
  void *device_ptr, rfm_device_enum_type rfm_device, rfcom_mode_enum_type tech, rfcom_band_type_u band, rfdevice_papm_bw_enum_type lte_bw, 
  void *buff_obj_ptr, rf_device_execution_type execution_type
);

boolean rfdevice_papm_disable
(
  void *device_ptr, rfm_device_enum_type rfm_device, rfcom_mode_enum_type tech, void *buff_obj_ptr, rf_device_execution_type exec_type
);

boolean rfdevice_papm_set_mode_bias
(
  void *device_ptr, rfm_device_enum_type rfm_device, rfcom_mode_enum_type tech, rfcom_band_type_u band, rfdevice_papm_mode_type xpt_mode, 
  uint16 bias, void *buff_obj_ptr, rf_device_execution_type dev_action
);

boolean rfdevice_papm_set_mode_bias_no_trigger
(
  void *device_ptr, rfm_device_enum_type rfm_device, rfcom_mode_enum_type tech, rfcom_band_type_u band, rfdevice_papm_mode_type xpt_mode, 
  uint16 bias, void *buff_obj_ptr, rf_device_execution_type dev_action
);

boolean rfdevice_papm_set_mode_bias_w_time
(
  void *device_ptr, rfm_device_enum_type rfm_device, rfcom_mode_enum_type tech, rfcom_band_type_u band, rfdevice_papm_mode_type xpt_mode, 
  uint16 bias, void *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing
);

boolean rfdevice_papm_set_mode_bias_w_time_no_trigger
(
  void *device_ptr, rfm_device_enum_type rfm_device, rfcom_mode_enum_type tech, rfcom_band_type_u band, rfdevice_papm_mode_type xpt_mode, 
  uint16 bias, void *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing
);

boolean rfdevice_papm_xpt_linearizer_settings
(
  void *device_ptr, rfm_device_enum_type rfm_device, rfcom_mode_enum_type tech, rfcom_band_type_u band, 
  rfdevice_papm_mode_type xpt_mode, uint16 bias, uint8 *settings
);

boolean rfdevice_papm_trigger
(
  void *device_ptr, void *buff_obj_ptr, rfm_device_enum_type rfm_device, rf_device_execution_type exec_type
);

boolean rfdevice_pam_get_et_voltage_min_max
(
  void *device_ptr, rfdevice_papm_misc_info *ET_voltage_info
);


boolean rfdevice_papm_get_device_info
(
  void *device_ptr, uint16 *mfg_id, uint16 *prd_id, uint16 *rev_id
);

boolean rfdevice_papm_dump_reg
(
  void *device_ptr
);

boolean rfdevice_papm_get_max_bw_supported
(
  void *device_ptr,
  rfm_device_enum_type rfm_device, 
  rfcom_lte_band_type rf_band, 
  uint32* max_bw_khz
);
  
  
#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_PAPM_INTF_H */


