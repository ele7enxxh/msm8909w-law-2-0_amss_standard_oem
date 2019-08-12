#ifndef RFDEVICE_TDSCDMA_PAPM_INTF_H
#define RFDEVICE_TDSCDMA_PAPM_INTF_H
/*!
   @file
   rfdevice_tdscdma_papm_intf.h

   @brief
   
*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.


                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_tdscdma_papm_intf.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------- 
03/19/15   al     KW error fix - Add array length as an input parameter to get PAPM info  
10/27/14   vv     Add papm device info api support
05/08/13   jyu    Added timing support for set_mode_bias
03/22/13   sr     changes to make the interfaces consistent with other devices
03/07/13   sr     changed the "pwr_tracker" name to "papm"
12/21/12   jyu    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rfdevice_papm.h"
#include "rfdevice_papm_intf.h"
#include "rfdevice_tdscdma_type_defs.h"
#include "rf_buffer_intf.h"

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
                         Data Definitions
==============================================================================*/

/*==============================================================================
                         Function Prototypes
==============================================================================*/

boolean rfdevice_tdscdma_use_papm
(
  rfm_device_enum_type rfm_device, 
  rfcom_tdscdma_band_type rf_band
);

boolean rfdevice_tdscdma_papm_enable
(
  rfm_device_enum_type rfm_device, 
  rfcom_tdscdma_band_type rf_band, 
  void *script_ptr, rf_device_execution_type exec_type
);

boolean rfdevice_tdscdma_papm_disable
(
  rfm_device_enum_type rfm_device,
  rfcom_tdscdma_band_type rf_band,
  void *script_ptr,
  rf_device_execution_type exec_type 
);

boolean rfdevice_tdscdma_papm_set_mode_bias
(
  rfm_device_enum_type rfm_device, 
  rfcom_tdscdma_band_type rf_band,
  rfdevice_papm_mode_type xpt_mode, 
  uint16 bias, 
  rf_device_execution_type dev_action, 
  void *script_ptr, 
  int16 script_timing
);

boolean rfdevice_tdscdma_papm_xpt_linearizer_settings
(
  rfm_device_enum_type rfm_device, 
  rfcom_tdscdma_band_type rf_band,
  rfdevice_papm_mode_type xpt_mode, 
  uint16 bias, 
  uint8 *settings
);

boolean rfdevice_tdscdma_papm_get_device_info
(
    rfm_device_enum_type rfm_device,
    rfcom_tdscdma_band_type rf_band,
    uint16 *mfg_id,
    uint16 *prd_id,
    uint16 *rev_id,
	uint16 array_len
);


#ifdef __cplusplus
}
#endif

#endif /* !RFDEVICE_TDSCDMA_PAPM_INTF_H */



