
#ifndef QFE2320_PHYSICAL_DEVICE_AG_H
#define QFE2320_PHYSICAL_DEVICE_AG_H
/*!
   @file
   qfe2320_physical_device_ag.h

   @brief
   qfe2320 physical device driver

*/

/*===========================================================================

Copyright (c) 2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2320/api/qfe2320_physical_device_ag.h#1 $ 

when       who   what, where, why
--------   ---   ---------------------------------------------------------------

04/23/14    vv    Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "qfe2320_pa.h"
#include "qfe2320_asm.h"
#include "qfe2320ept_pa.h"
#include "qfe2320ept_asm.h"


#include "rfdevice_physical_device.h"

class qfe2320_physical_device : public rfdevice_physical_device
{
public:
  qfe2320_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);

   virtual rfdevice_logical_component* get_component(rfdevice_type_enum_type type, int instance);
  
  virtual bool validate_self_cal_efs(void);
  
  	void qfe2320_set_pa_on_flag(boolean pa_on_status);
	
	boolean qfe2320_get_pa_on_status();  

  qfe2320_physical_device* qfe2320_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

private:

  /* PA */
  void create_pa_object(rfc_logical_device_info_type *logical_device_cfg);

  qfe2320_pa* qfe2320_pa_obj_ptr;
  qfe2320ept_pa* qfe2320_pa_ept_obj_ptr;

  /* ASM */
  void create_asm_object(rfc_logical_device_info_type *logical_device_cfg);
  

	boolean pa_on_status_phy;

  qfe2320_asm* qfe2320_asm_obj_ptr;
  qfe2320ept_asm* qfe2320_asm_ept_obj_ptr;

};
#endif
