
#ifndef QPA4340_PHYSICAL_DEVICE_AG_H
#define QPA4340_PHYSICAL_DEVICE_AG_H
/*!
   @file
   qpa4340_physical_device_ag.h

   @brief
   qpa4340 physical device driver

*/

/*===========================================================================

Copyright (c) 2015-2016 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qpa4340/api/qpa4340_physical_device.h#1 $ 

when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
05/19/16   dbc   Enable EFS PA Support 
02/18/16   dbc   Add Sopport for QPA4340_ET logical PA 
10/20/15   px    Add common tab sleep/wakeup voting
11/07/14   sn    Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "rfdevice_qasm_2g.h"
#include "rfdevice_qpa_4g.h"
#include "rfdevice_qtherm.h"

#include "rfdevice_physical_device.h"
#include "qpa4340_asm_config_main_ag.h"
#include "qpa4340_pa_config_main_ag.h"
#include "qpa4340_et_pa_config_main_ag.h"

/* Bitmask for bits reserved for logical PAs */
#define QPA4340_PA_VOTE_MASK 0xFF
/* Number of bits to reserve for logical PAs */
#define QPA4340_PA_VOTE_RESERVED_BITS 8

class qpa4340_physical_device : public rfdevice_physical_device
{
public:
  qpa4340_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);
  
  virtual bool validate_self_cal_efs(void);

  virtual bool vote_sleep_wakeup(rfc_logical_device_info_type *logical_device_cfg, bool sleep_wakeup);

  virtual bool vote_common_sleep_wakeup(rfc_logical_device_info_type *logical_device_cfg, bool sleep_wakeup);

  virtual bool has_common_tab();

  virtual bool supports_efs_dat(void);

  qpa4340_physical_device* qpa4340_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

  uint8 chip_rev;

/* 
  One bit per logical component to track the sleep/wakeup votes 
  Wakeup vote sets the bit, Sleep vote clears the bit
*/
  uint64 sleep_wakeup_vote;

private:

  /* PA */
  void create_pa_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qpa_4g* qpa4340_pa_obj_ptr;
  rfdevice_qpa_4g* qpa4340_et_pa_obj_ptr;

  /* ASM */
  void create_asm_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qasm_2g* qpa4340_asm_obj_ptr;
  rfdevice_qasm_2g* qpa4340_tdd_asm_obj_ptr;

};
#endif
