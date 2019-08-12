#ifndef RFDEVICE_LTE_COUPLER_INTF_H
#define RFDEVICE_LTE_COUPLER_INTF_H
/*!
   @file
   rfdevice_lte_coupler_intf.h

   @brief
   
*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_lte_coupler_intf.h#1 $

when         who   what, where, why
--------   ---   -------------------------------------------------------------------
04/08/14   yb    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rflte_msg.h"
#include "rfdevice_coupler_intf.h"
#ifdef FEATURE_LTE
#include "rfdevice_lte_type_defs.h"
#endif /*FEATURE_LTE*/
#ifdef __cplusplus
extern "C" {
#endif
#ifdef FEATURE_LTE


/*===========================================================================
                         Data Definitions
===========================================================================*/

/*===========================================================================
                         Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
/* function prototypes */


boolean rfdevice_lte_coupler_config
(
  rfm_device_enum_type device, 
  rfdevice_coupler_tech_cfg_type tech_cfg_type,
  rfdevice_lte_script_data_type* script_data_ptr,
  int16 script_timing
);

#endif /*FEATURE_LTE*/

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_COUPLER_INTF_H */


