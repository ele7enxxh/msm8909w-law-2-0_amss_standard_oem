
#ifndef QFE2085FC_COUPLER_CONFIG_MAIN_AG_H
#define QFE2085FC_COUPLER_CONFIG_MAIN_AG_H
/*
WARNING: This QFE2085FC_V200 driver is auto-generated.

Generated using: qcoupler_autogen.pl 
Generated from-  

	File: QFE2085FC_RFFE_Settings.xlsx 
	Released: 4/26/2016
	Author: rspring
	Revision: 2.2
	Change Note: Added B12/13/28A/28B in Coupler
	Tab: qfe2085fc_v200_cpl_settings

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

$Header: //source/qcom/qct/modem/rfdevice/qfe1035/main/latest/etc/qcoupler_autogen.pl#2 60============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

    
#include "rf_rffe_common.h"
#include "rfdevice_qcoupler_typedef.h"
#include "qfe2085fc_v00_coupler_config_ag.h" 
#include "qfe2085fc_v200_coupler_config_ag.h" 

#ifdef __cplusplus
extern "C" {
#endif  

boolean rfdevice_qcoupler_qfe2085fc_validate_n_create_cfg_ag
( 
  rfc_phy_device_info_type* cfg,  
  rfdevice_id_enum_type logical_rf_device_id ,
  uint8 chip_rev,  
  rfdevice_qcoupler_settings_type* qpa_settings,
  rfdevice_qcoupler_func_tbl_type* pa_fn_ptrs
);

#ifdef __cplusplus
}
#endif
#endif