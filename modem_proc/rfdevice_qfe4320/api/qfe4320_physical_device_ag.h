#ifndef QFE4320_PHYSICAL_DEVICE_AG_H
#define QFE4320_PHYSICAL_DEVICE_AG_H
/*!
   @file
   qfe4320_physical_device_ag.h

   @brief
   qfe4320 physical device driver

*/

/*===========================================================================

Copyright (c) 2013-2016 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe4320/api/qfe4320_physical_device_ag.h#1 $ 

when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
05/31/16   dbc   Enable EFS PA Support 
03/12/15   dbc   Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "rfdevice_qasm.h"
#include "rfdevice_qpa_4g.h"
#include "rfdevice_qtherm.h"

#include "rfdevice_physical_device.h"
#include "qfe4320_asm_config_main_ag.h"
#include "qfe4320_et_pa_config_main_ag.h"
#include "qfe4320_ept_pa_config_main_ag.h"
#include "qfe4320_tdd_pa_config_main_ag.h"
#include "qfe4320_gsm_pa_config_main_ag.h"
#include "qfe4320_therm_config_main_ag.h"

class qfe4320_physical_device : public rfdevice_physical_device
{
public:
  qfe4320_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);
  
  virtual bool validate_self_cal_efs(void);

  virtual bool supports_efs_dat(void);

  qfe4320_physical_device* qfe4320_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

  uint8 chip_rev;

private:

  /* PA */
  void create_pa_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qpa_4g* qfe4320_pa_et_obj_ptr;
  rfdevice_qpa_4g* qfe4320_pa_ept_obj_ptr;
  rfdevice_qpa_4g* qfe4320_pa_tdd_obj_ptr;
  rfdevice_qpa_4g* qfe4320_pa_gsm_obj_ptr;

  /* ASM */
  void create_asm_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qasm* qfe4320_asm_lb_obj_ptr;
  rfdevice_qasm* qfe4320_asm_mb_obj_ptr;
  rfdevice_qasm* qfe4320_asm_tx_obj_ptr;

  /* THERM */
  void create_therm_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qtherm* qfe4320_therm_obj_ptr;

};
#endif
