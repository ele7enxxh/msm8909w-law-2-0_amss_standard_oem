
#ifndef QFE4373FC_EPT_PA_CONFIG_MAIN_AG_H
#define QFE4373FC_EPT_PA_CONFIG_MAIN_AG_H
/*
WARNING: This QFE4373FC_EPT driver is auto-generated.

Generated using: Process_QPA.pl 
Generated from-  

	File: QFE4373FC_RFFE_Settings.xlsx 
	Released: 5/16/2016
	Author: rspring
	Revision: 4.63 ()
	Change Note: FixedleakagecurrentissueduetoREG0x02inSLEEP(valuefrom0x20to0x2C)
	Tab: qfe4373fc_ept_pa_settings

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the QFE4373FC_EPT PA.

  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

$ Header: $
$ Author: $
$ DateTime: $ 

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

    
#include "rf_rffe_common.h"
#include "rfdevice_qpa_4g_typedef.h"
#include "qfe4373fc_ept_pa_config_ag.h" 
#include "qfe4373fc_v2p51_ept_pa_config_ag.h" 

#ifdef __cplusplus
extern "C" {
#endif  

boolean rfdevice_qpa_qfe4373fc_ept_validate_n_create_cfg_ag
( 
  rfc_phy_device_info_type* cfg,   
  rfdevice_id_enum_type logical_rf_device_id , 
  uint8 chip_rev, 
  rfdevice_qpa_4g_settings_type* qpa_settings,
  rfdevice_qpa_4g_func_tbl_type* pa_fn_ptrs
);
   
  
#ifdef __cplusplus
}
#endif
#endif