
#ifndef QFE2340_PHYSICAL_DEVICE_AG_H
#define QFE2340_PHYSICAL_DEVICE_AG_H
/*!
   @file
   qfe2340_physical_device_ag.h

   @brief
   qfe2340 physical device driver

*/

/*===========================================================================

Copyright (c) 2013-2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2340/api/qfe2340_physical_device_ag.h#1 $ 

when       who   what, where, why
--------   ---   ---------------------------------------------------------------

04/23/14    vv    Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rf_rffe_common.h"
#include "qfe2340_pa.h"
#include "qfe2340_asm.h"
#include "qfe2340v3p0_pa.h"
#include "qfe2340v3p0_asm.h"
#include "rfdevice_physical_device.h"

class qfe2340_physical_device : public rfdevice_physical_device
{
public:
  qfe2340_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);

   virtual rfdevice_logical_component* get_component(rfdevice_type_enum_type type, int instance);
  
  virtual bool validate_self_cal_efs(void);

  qfe2340_physical_device* qfe2340_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;


  void qfe2340_set_pa_on_flag(boolean pa_on_status);
	
  boolean qfe2340_get_pa_on_status();  
	
private:

  /* PA */
  void create_pa_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_logical_component* qfe2340_pa_obj_ptr;

  /* ASM */
  void create_asm_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_logical_component* qfe2340_asm_obj_ptr;
  
  
  boolean pa_on_status_phy;

};
#endif
