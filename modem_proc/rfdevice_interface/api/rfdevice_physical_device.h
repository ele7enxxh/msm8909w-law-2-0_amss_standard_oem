#ifndef RFDEVICE_PHYSICAL_DEVICE_H
#define RFDEVICE_PHYSICAL_DEVICE_H
/*!
  @file
  rfdevice_physical_device.h 

  @brief
  Common interface to an instance of a physical chip in the system.
*/

/*===========================================================================

  Copyright (c) 2012 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_physical_device.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/09/16   dbc     Add support supports_efs_dat() API
10/08/15   px      Added support for QPA/QASM common tab voting
10/01/14   dbc     Remove XSW APIs 
08/14/14   dbc     Added support for physical device tracking of XSW position
07/28/14   vv      Added mutex protection support for all physical devices
06/19/14   vv      Added support for physical device based sleep/wakeup voting
05/12/14   vv      Update physical device interface
04/08/13   bmg     Changed self-calibration interface to character string
11/06/12   bmg     Created

===========================================================================*/

#include "comdef.h"
#include "rfdevice_logical_component.h"
#include "rfc_common.h"

#ifdef __cplusplus
extern "C" {
#include "rfcommon_locks.h"
}
#endif

class rfdevice_physical_device
{
public:

  rfdevice_physical_device();
  virtual rfdevice_logical_component* get_component(rfdevice_type_enum_type type, int instance);

  /*Will make the below function pure virtual once we have physical device creation enabled for all devices*/
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type* device_cfg);

  virtual bool perform_self_cal(const char* str) = 0;
  virtual bool load_self_cal(const char* str) = 0;

  virtual ~rfdevice_physical_device();

 /*To support physical device based sleep/wakeup voting*/
 virtual bool vote_sleep_wakeup(rfc_logical_device_info_type *logical_device_cfg, bool sleep_wakeup);
  virtual bool vote_common_sleep_wakeup(rfc_logical_device_info_type *logical_device_cfg, bool sleep_wakeup);
  
  virtual bool has_common_tab();

  virtual bool supports_efs_dat();

  /*! rfdevice_physical global mutex - one per instance.Can be used by all physical devices to protect critical section code. 
 Should be initialized during device configuration before any other usage */
 rf_lock_data_type rfdevice_physical_critical_sect;
};

#endif /* RFDEVICE_PHYSICAL_DEVICE_H */

