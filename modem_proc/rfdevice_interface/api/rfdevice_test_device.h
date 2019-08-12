#ifndef RFDEVICE_TEST_DEVICE_H
#define RFDEVICE_TEST_DEVICE_H
/*!
  @file
  rfdevice_test_device.h

  @brief
  Declaration of the off-target test device type.
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_test_device.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/09/14   vv      Updating device creation to new physical device config structures
10/31/13   shb     Converted TRX common device interface to C++
06/17/13   bmg     Created
 
==============================================================================*/
 
#include "rfdevice_physical_device.h"
#include "rfc_common.h"

/*!
  @brief
  A device that can be used for software testing in unit tests and systems
  where no physical hardware components are present.

  @details
  This physical chip implements working stub behavior for any logical
  component that could be present in the RF software system.  It allows
  MC/Core level call flow execution without requring a specific chipset
  assumption, making unit tests and emulation development independent
  of any specific external chipset requirements.
*/
class test_device_physical_device : public rfdevice_physical_device
{
public:
  test_device_physical_device(rfc_phy_device_info_type* cfg);

  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type* logical_device_cfg);

  virtual bool perform_self_cal(const char* str);
  virtual bool load_self_cal(const char* str);

private:
};

#endif /* RFDEVICE_TEST_DEVICE_H */
