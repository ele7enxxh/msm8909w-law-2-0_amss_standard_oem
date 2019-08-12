
#ifndef tp_antenna_tuner_PHYSICAL_DEVICE_AG_H
#define tp_antenna_tuner_PHYSICAL_DEVICE_AG_H
/*!
   @file
   tp_antenna_tuner_physical_device.h

   @brief
   tp_antenna_tuner physical device driver

*/

/*===========================================================================

Copyright (c) 2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_tptuner/common/inc/tp_antenna_tuner_physical_device.h#1 $ 

when       who   what, where, why
--------   ---   ---------------------------------------------------------------

05/14/14    vv    Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "tp_antenna_tuner.h"


class tp_antenna_tuner_physical_device : public rfdevice_physical_device
{
public:
  tp_antenna_tuner_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);
  
  virtual rfdevice_logical_component* get_component(rfdevice_type_enum_type type, int instance);
  
  virtual bool validate_self_cal_efs(void);
  
  tp_antenna_tuner_physical_device* tp_antenna_tuner_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

private:

  /* TP TUNER */
  void create_tp_tuner_object(rfc_logical_device_info_type *logical_device_cfg);

  tp_antenna_tuner* tp_antenna_tuner_obj_ptr;

};
#endif
