#ifndef QFE4373FC_PHYSICAL_DEVICE_AG_H
#define QFE4373FC_PHYSICAL_DEVICE_AG_H
/*!
   @file
   qfe4373fc_physical_device_ag.h

   @brief
   qfe4373fc physical device driver

*/

/*===========================================================================

Copyright (c) 2013-2016 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe4373fc/api/qfe4373fc_physical_device_ag.h#1 $ 

when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
06/16/16   dbc   Enable EFS PA Support 
03/12/15   dbc   Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "rfdevice_qasm_2g.h"
#include "rfdevice_qpa_4g.h"

#include "rfdevice_physical_device.h"
#include "qfe4373fc_asm_config_main_ag.h"
#include "qfe4373fc_pa_config_main_ag.h"
#include "qfe4373fc_gsm_pa_config_main_ag.h"
#include "qfe4373fc_ept_pa_config_main_ag.h"

class qfe4373fc_physical_device : public rfdevice_physical_device
{
public:
  qfe4373fc_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);
  
  virtual bool validate_self_cal_efs(void);

  virtual bool supports_efs_dat(void);

  qfe4373fc_physical_device* qfe4373fc_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

  uint8 chip_rev;

private:

  /* PA */
  void create_pa_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qpa_4g* qfe4373fc_pa_obj_ptr;
  rfdevice_qpa_4g* qfe4373fc_pa_gsm_obj_ptr;
  rfdevice_qpa_4g* qfe4373fc_pa_ept_obj_ptr;

  /* ASM */
  void create_asm_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qasm_2g* qfe4373fc_asm_obj_ptr;

};
#endif
