
#ifndef QFE3320_THERM_CONFIG_MAIN_AG_H
#define QFE3320_THERM_CONFIG_MAIN_AG_H
/*
WARNING: This QFE3320 driver is auto-generated.

Generated using: qtherm_autogen.pl 
Generated from-  

	File: QFE3320_RFFE_Settings.xlsx 
	Released: 3/27/2015
	Author: rspring
	Revision: 3.07
	Change Note: FOR: ET, Changed REG06 to 0x018 and REG30 to 0x17 for SOA for B12/17 ONLY
	Tab: qfe3320_therm_settings

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the QFE3320 THERM.

  Copyright (c) 2013, 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in thermrt,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

$Header: //source/qcom/qct/modem/rfdevice/qtherm/main/latest/etc/qtherm_autogen.pl#4 60============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

    
#include "rf_rffe_common.h"
#include "rfdevice_qtherm_typedef.h"
#include "qfe3320_therm_config_ag.h" 

#ifdef __cplusplus
extern "C" {
#endif  

boolean rfdevice_qtherm_qfe3320_validate_n_create_cfg_ag
( 
  rfc_phy_device_info_type* cfg,   
  rfdevice_id_enum_type logical_rf_device_id , 
  uint8 chip_rev, 
  rfdevice_qtherm_settings_type* qtherm_settings,
  rfdevice_qtherm_func_tbl_type* therm_fn_ptrs
);

#ifdef __cplusplus
}
#endif
#endif