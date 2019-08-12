#ifndef RFDEVICE_QCOUPLER_API_H
#define RFDEVICE_QCOUPLER_API_H

/*=============================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. 
  Government. Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qcoupler/api/rfdevice_qcoupler_api.h#1 $
  $DateTime: 2016/12/14 01:58:12 $
  $Author: mplcsds1 $
 
when         who   what, where, why
--------   ---   --------------------------------------------------------------- 
08/21/14   vv    Added support to decouple qcoupler factory to chip specific drivers 
                 Clean up unused device constructor
05/27/14   vv    Physical device support
04/07/14   yb    Support for Coupler RFC changes
03/31/14   yb    Fixed compilation error
03/27/14   yb    Attenuation enum for coupler
02/10/14   yb    Initial version
 
=============================================================================*/
  
/*=============================================================================

                            INCLUDE FILES FOR MODULE
                            

=============================================================================*/
#include "rfc_common.h"
#include "rfdevice_coupler.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_qcoupler_typedef.h"

#ifdef __cplusplus

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface function for creating the common driver qcoupler object for Bolt & DPM2.0 PLs

  @details
  The function is used to create the Common Qcoupler driver for the passed configuration.

  @param phy_obj_ptr
  Parent physical device object pointer
 
  @param phy_device_info
  physical device config info
 
  @param logical_device_info
  logical device config info
 
*/
rfdevice_coupler* phy_create_common_qcoupler 
(
                            rfdevice_physical_device* phy_obj_ptr,
                            rfc_phy_device_info_type *phy_device_info, 
                            rfc_logical_device_info_type *logical_device_info,
                            rfdevice_qcoupler_settings_type *qcoupler_settings,
                            rfdevice_qcoupler_func_tbl_type *qcoupler_fn_ptrs
);




#endif

#endif  /* RFDEVICE_QCOUPLER_API_H */