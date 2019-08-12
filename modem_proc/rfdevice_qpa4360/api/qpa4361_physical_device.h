
#ifndef QPA4361_PHYSICAL_DEVICE_AG_H
#define QPA4361_PHYSICAL_DEVICE_AG_H
/*!
   @file
   qpa4361_physical_device_ag.h

   @brief
   qpa4361 physical device driver

*/

/*===========================================================================

Copyright (c) 2013-2016 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qpa4360/api/qpa4361_physical_device.h#1 $ 

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
05/09/16   dbc   Enable EFS PA Support
02/24/16   bz    Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rfdevice_qasm_2g.h"
#include "rfdevice_qpa_4g.h"

#include "rfdevice_physical_device.h"
#include "qpa4361_pa_config_main_ag.h"
#include "qpa4361_asm_config_main_ag.h"
#include "qpa4361_gsm_pa_config_main_ag.h"

class qpa4361_physical_device : public rfdevice_physical_device
{
public:
  qpa4361_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);
  
  virtual bool validate_self_cal_efs(void);

  virtual bool supports_efs_dat(void);

  qpa4361_physical_device* qpa4361_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

  uint8 chip_rev;

private:

  /* PA */
  void create_pa_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qpa_4g* qpa4361_pa_obj_ptr;
  rfdevice_qpa_4g* qpa4361_pa_gsm_obj_ptr;

  /* ASM */
  rfdevice_qasm_2g* qpa4361_asm_obj_ptr;
  void create_asm_object(rfc_logical_device_info_type *logical_device_cfg);


};
#endif
