#ifndef QPA4351_PHYSICAL_DEVICE_AG_H
#define QPA4351_PHYSICAL_DEVICE_AG_H
/*!
   @file
   qpa4351_physical_device_ag.h

   @brief
   qpa4351 physical device driver

*/

/*===========================================================================

Copyright (c) 2016 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qpa4351/api/qpa4351_physical_device.h#1 $ 

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
01/03/16   dbc   Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rfdevice_qpa_4g.h"
#include "rfdevice_physical_device.h"
#include "qpa4351_pa_config_main_ag.h"


class qpa4351_physical_device : public rfdevice_physical_device
{
public:
  qpa4351_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);
  
  virtual bool validate_self_cal_efs(void);

  virtual bool has_common_tab();

  qpa4351_physical_device* qpa4351_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

  uint8 chip_rev;

private:

  /* PA */
  void create_pa_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qpa_4g* qpa4351_pa_obj_ptr;

};
#endif
