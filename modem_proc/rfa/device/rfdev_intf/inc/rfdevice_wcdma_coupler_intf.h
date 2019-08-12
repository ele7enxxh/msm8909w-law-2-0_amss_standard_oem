#ifndef RFDEVICE_WCDMA_COUPLER_INTF_H
#define RFDEVICE_WCDMA_COUPLER_INTF_H
/*!
   @file
   rfdevice_wcdma_coupler_intf.h

   @brief
   
*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_wcdma_coupler_intf.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------- 
04/08/14   yb    Initial version 
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_WCDMA
#include "rfcom.h"
#include "rfdevice_coupler_intf.h"
#include "rfdevice_wcdma_type_defs.h"
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

boolean rfdevice_wcdma_coupler_config
(
  rfm_device_enum_type device, 
  rfdevice_coupler_tech_cfg_type tech_cfg_type, 
  rf_buffer_intf *script, 
  rf_device_execution_type execution_type,
  int16 script_timing    
);

#ifdef __cplusplus
}
#endif
#endif /* FEATURE_WCDMA */
#endif /* RFDEVICE_COUPLER_INTF_H */