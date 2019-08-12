
#ifndef QFE1040_PHYSICAL_DEVICE_AG_H
#define QFE1040_PHYSICAL_DEVICE_AG_H
/*
WARNING: This QFE1040 physical device driver is auto-generated.

Generated using: qfe_physical_device_autogen.pl 
Generated from-  

	File: QFE1040_RFFE_Settings.xlsm 
	Released: 4/2/2014
	Author: Diapa Sanogo
	Revision: 8
	Change Note: Added underscore in the enable_Tx and enable_Rx in v40 tab

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of QFE1040 CHIP.

  Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe1040/api/qfe1040_physical_device_ag.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 07:59:16 $ 

when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
08/07/14   vv    Added support to decouple qasm factory to chip specific drivers  
06/19/14   vv    Added support for physical device based sleep/wakeup voting
05/12/14   vv    Initial version
============================================================================*/ 

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rfdevice_qasm.h"

#include "rfdevice_physical_device.h"
#include "qfe1040_asm_config_main_ag.h"

class qfe1040_physical_device : public rfdevice_physical_device
{
public:
  qfe1040_physical_device(rfc_phy_device_info_type* cfg);

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);
    
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);
  
  virtual bool validate_self_cal_efs(void);

  virtual bool vote_sleep_wakeup(rfc_logical_device_info_type *logical_device_cfg, bool sleep_wakeup);

  qfe1040_physical_device* qfe1040_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;
 
  uint8 chip_rev;

/* 
  One bit per logical component to track the sleep/wakeup votes 
  Sleep vote resets the bit
  Wakeup vote sets the bit
*/
  uint64 sleep_wakeup_vote;

private:

  /* ASM */
  void create_asm_object(rfc_logical_device_info_type *logical_device_cfg);


  rfdevice_qasm* qfe1040_asm_lb_w0_obj_ptr;
  rfdevice_qasm* qfe1040_asm_mb_w0_obj_ptr;
  rfdevice_qasm* qfe1040_asm_hb_w0_obj_ptr;
  rfdevice_qasm* qfe1040_asm_lb_w1_obj_ptr;
  rfdevice_qasm* qfe1040_asm_mb_w1_obj_ptr;
  rfdevice_qasm* qfe1040_asm_hb_w1_obj_ptr;

};


/*enum listing the logical components withtin the chip*/
typedef enum
{
  QFE1040_LB_W0_ASM=0,
  QFE1040_MB_W0_ASM,
  QFE1040_HB_W0_ASM,
  QFE1040_LB_W1_ASM,
  QFE1040_MB_W1_ASM,
  QFE1040_HB_W1_ASM,
  QFE1040_LOGICAL_INVALID,
}qfe1040_logical;

#endif