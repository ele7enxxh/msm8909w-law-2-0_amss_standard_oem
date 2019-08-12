
#ifndef qfe3320_PHYSICAL_DEVICE_AG_H
#define qfe3320_PHYSICAL_DEVICE_AG_H
/*!
   @file
   qfe3320_physical_device_ag.h

   @brief
   qfe3320 physical device driver

*/

/*===========================================================================

Copyright (c) 2013-2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe3320/api/qfe3320_physical_device_ag.h#1 $ 

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
08/07/14   vv    Added support to decouple qasm/qpa factory to chip specific drivers 
04/23/14   vv    Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "rfdevice_qasm.h"
#include "rfdevice_qpa.h"
#include "rfdevice_qtherm.h"

#include "rfdevice_physical_device.h"
#include "qfe3320_asm_config_main_ag.h"
#include "qfe3320_pa_config_main_ag.h"
#include "qfe3320_therm_config_main_ag.h"

class qfe3320_physical_device : public rfdevice_physical_device
{
public:
  qfe3320_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);

  virtual bool vote_sleep_wakeup(rfc_logical_device_info_type *logical_device_cfg, bool sleep_wakeup);
  
  virtual bool validate_self_cal_efs(void);

  qfe3320_physical_device* qfe3320_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

  uint8 chip_rev;

  /* 
    One bit per logical component to track the sleep/wakeup votes 
    Sleep vote resets the bit
    Wakeup vote sets the bit
  */
  uint64 sleep_wakeup_vote_mb_asm;
  uint64 sleep_wakeup_vote_lb_asm;


private:

  /* PA */
  void create_pa_object(rfc_logical_device_info_type *logical_device_cfg);

#if (defined(FEATURE_BOLT_MODEM))  
  rfdevice_qpa* qfe3320_pa_obj_ptr;
#endif

  rfdevice_qpa* qfe3320_pa_gsm_obj_ptr;
  rfdevice_qpa* qfe3320_pa_ept_obj_ptr;
  rfdevice_qpa* qfe3320_pa_tdd_obj_ptr;

  /* ASM */
  void create_asm_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qasm* qfe3320_asm_lb_obj_ptr;
  rfdevice_qasm* qfe3320_asm_mb_obj_ptr;
  rfdevice_qasm* qfe3320_asm_tx_obj_ptr;

  /* THERM */
  void create_therm_object(rfc_logical_device_info_type *logical_device_cfg);

  rfdevice_qtherm* qfe3320_therm_obj_ptr;


};

/*enum listing the logical components withtin the chip*/
typedef enum
{
  QFE3320_LB_ASM=0,
  QFE3320_MB_ASM,
  QFE3320_TX_ASM,
  QFE3320_LOGICAL_INVALID
}qfe3320_logical;

#endif