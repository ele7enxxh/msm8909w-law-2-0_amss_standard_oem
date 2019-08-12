
#ifndef QFE3320_ASM_CONFIG_MAIN_AG_H
#define QFE3320_ASM_CONFIG_MAIN_AG_H
/*
WARNING: This QFE3320_MB driver is auto-generated.

Generated using: qasm_autogen.pl 
Generated from-  

	File: QFE3320_RFFE_Settings.xlsx 
	Released: 3/27/2015
	Author: rspring
	Revision: 3.07
	Change Note: FOR: ET, Changed REG06 to 0x018 and REG30 to 0x17 for SOA for B12/17 ONLY
	Tab: qfe3320_mb_asm_settings

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe3320/api/qfe3320_asm_config_main_ag.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 07:59:30 $ 

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

    
#include "rf_rffe_common.h"
#include "rfdevice_qasm_typedef.h"
#include "qfe3320_tx_asm_config_ag.h" 
#include "qfe3320_lb_asm_config_ag.h" 
#include "qfe3320_mb_asm_config_ag.h" 

#ifdef __cplusplus
extern "C" {
#endif  

boolean rfdevice_qasm_qfe3320_validate_n_create_cfg_ag
( 
  rfc_phy_device_info_type* cfg,  
  rfdevice_id_enum_type logical_rf_device_id ,
  uint8 chip_rev,
  rfdevice_qasm_settings_type* qpa_settings,
  rfdevice_qasm_func_tbl_type* pa_fn_ptrs
);

#ifdef __cplusplus
}
#endif
#endif