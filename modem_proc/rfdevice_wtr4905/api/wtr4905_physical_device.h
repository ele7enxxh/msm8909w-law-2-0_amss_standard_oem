#ifndef WTR4905_PHYSICAL_DEVICE_H
#define WTR4905_PHYSICAL_DEVICE_H
/*!
  @file
  wtr4905_physical_device.h 

  @brief
  Software abstraction of a WTR4905 physical device.
*/

/*===========================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_wtr4905/api/wtr4905_physical_device.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/04/14    vv      Update physical/logical device creation with updated physical device interface
04/02/14   dps     Cleanup Klocwork warnings by preventing copying this object
01/03/14   hm      Fixed compiler warnings
12/04/13   rp      Initial version

===========================================================================*/

#include "rfdevice_physical_device.h"
#include "wtr4905_common_device_intf.h"

class wtr4905_physical_device : public rfdevice_physical_device
{
public:
  wtr4905_physical_device(rfc_phy_device_info_type *wtr4905_cfg_info);

  ~wtr4905_physical_device();

  virtual bool load_self_cal(const char* str);
  
  virtual bool perform_self_cal(const char* str);
  
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);


private:

  virtual void create_wtr4905_device(rfc_device_cfg_info_type *device_cfg);

  const char *efs_file_path;
  rfdevice_rxtx_common_class *transceiver_ptr;
  rfc_phy_device_info_type* phy_device_cfg;
  rfdevice_cmn_int_dev_cal_data_type *cal_data;
  
  /*Maintaining old device config sturctue*/
  rfc_device_cfg_info_type device_cfg;
  
  /* Private copy constructor and assignment operator prevent copying
     wtr4905_physical_device objects */
  wtr4905_physical_device& operator=(const wtr4905_physical_device& dev){ return *this; }
  wtr4905_physical_device(const wtr4905_physical_device& src){}
};

#endif /* WTR4905_PHYSICAL_DEVICE_H */

