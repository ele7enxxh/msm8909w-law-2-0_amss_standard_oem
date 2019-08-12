#ifndef RFDEVICE_COUPLER_INTF_H
#define RFDEVICE_COUPLER_INTF_H
/*!
   @file
   rfdevice_coupler_intf.h

   @brief
   
*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_coupler_intf.h#1 $

when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
04/08/14   yb    Initial version
==============================================================================*/

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_coupler.h"
#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                         Data Definitions
==============================================================================*/


/*==============================================================================
                         Prototypes
==============================================================================*/

boolean rfdevice_coupler_config
(
  void *device_ptr, rfdevice_coupler_tech_cfg_type tech_cfg_type, 
  void *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing
);

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_COUPLER_INTF_H */


