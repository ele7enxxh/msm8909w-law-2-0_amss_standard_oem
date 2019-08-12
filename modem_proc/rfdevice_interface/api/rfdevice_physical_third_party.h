
#ifndef RFDEVICE_THIRD_PARTY_PHYSICAL_H
#define RFDEVICE_THIRD_PARTY_PHYSICAL_H
/*!
   @file
   rfdevice_physical_third_party.h

   @brief
   rfdevice_physical_third_party device driver

*/

/*===========================================================================

Copyright (c) 2013-2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_physical_third_party.h#1 $ 

when       who   what, where, why
--------   ---   ---------------------------------------------------------------

05/12/14    vv    Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/





#include "rfdevice_physical_device.h"
#include "rfdevice_msg_log.h"
#define MAX_COMPONENTS_PER_THIRD_PARTY_CHIP 5

class rfdevice_physical_third_party : public rfdevice_physical_device
{
public:
  rfdevice_physical_third_party(rfc_phy_device_info_type* cfg);

  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type* device_cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);

  rfc_phy_device_info_type* third_party_phy_dev_cfg;
  /*Store Logical Component*/
  rfdevice_logical_component* store_object(rfdevice_logical_component* logical_dev_obj);

protected:


  /*Pointer array to store the logical components per third party device*/
  rfdevice_logical_component* logical_component_structure[MAX_COMPONENTS_PER_THIRD_PARTY_CHIP];

};
#endif
