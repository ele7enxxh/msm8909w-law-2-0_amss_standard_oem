
#ifndef RFDEVICE_PAPM_PHYSICAL_DEVICE_H
#define RFDEVICE_PAPM_PHYSICAL_DEVICE_H
/*!
   @file
   rfdevice_papm_physical_device.h

   @brief
   This file contains definition & prototypes for papm physical device interface

*/
/*==============================================================================

  Copyright (c) 2013 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_papm/api/rfdevice_papm_physical_device.h#1 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------

04/22/14  vv     Initial Revision
=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "rfdevice_papm_common.h"
#include "rfdevice_physical_device.h"

class papm_physical_device : public rfdevice_physical_device
{
public:

   papm_physical_device(rfc_phy_device_info_type* cfg);
   
   virtual bool load_self_cal(const char* str);

   virtual bool perform_self_cal(const char* str);
    
   virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type* device_cfg);
  
   virtual rfdevice_logical_component* get_component(rfdevice_type_enum_type type, int instance);

   virtual bool validate_self_cal_efs(void);

   papm_physical_device* papm_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

private:


  /* PAPM */
  void create_papm_object(rfc_logical_device_info_type* device_cfg);

  rfdevice_papm_common* papm_obj_ptr;


 };

#endif