#ifndef RFDEVICE_HDET_CDMA_INTF_H
#define RFDEVICE_HDET_CDMA_INTF_H
/*!
   @file
   rfdevice_hdet_cdma_intf.h

   @brief
   Interfaces for RF Upper Layer to operate HDET on the HDET Device
 
*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_hdet_cdma_intf.h#1 $

when       who   what, where, why
--------   ---   ------------------------------------------------------------------- 
03/11/13   sar     Updated file for APQ, GNSS only target.
12/06/12   spa    Added interface to get FW HDET trigger scripts
10/16/12   kai    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X

#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_cdma_type_defs.h"

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

boolean rfdevice_hdet_cdma_init
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint32 tx_freq
);

boolean rfdevice_hdet_cdma_setup
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfdevice_cdma_hdet_power_mode_type power_mode
);

boolean rfdevice_hdet_cdma_enable
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action
);

boolean rfdevice_hdet_cdma_disable
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action
);

boolean rfdevice_hdet_cdma_read_hdet
(
  void *device_ptr,
  boolean mdsp_triggered_read,
  rfdevice_cdma_hdet_read_type *hdet_output
);

uint16 rfdevice_hdet_cdma_multi_read_hdet
(
  void *device_ptr,
  uint16 num_of_reads
);

boolean rfdevice_hdet_cdma_get_hdet_param
(
  void *device_ptr,
  rfdevice_cdma_hdet_param_type *hdet_param
);
  
boolean rfdevice_hdet_cdma_get_hdet_scripts
(
  void *device_ptr,
  rfdevice_cdma_hdet_scripts_type* hdet_scripts
);


boolean rfdevice_hdet_cdma_get_fw_trigger_scripts
(
   void *device_ptr,
   rf_buffer_intf* hdet_scripts
);

#ifdef __cplusplus
}
#endif
#endif /* FEATURE_CDMA */

#endif /* RFDEVICE_HDET_CDMA_INTF_H */


