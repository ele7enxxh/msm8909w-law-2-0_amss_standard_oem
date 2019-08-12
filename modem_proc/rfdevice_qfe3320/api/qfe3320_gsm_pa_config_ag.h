
#ifndef QFE3320_GSM_PA_CONFIG_AG_H
#define QFE3320_GSM_PA_CONFIG_AG_H/*
WARNING: This QFE3320_GSM driver is auto-generated.

Generated using: qpa_autogen.pl 
Generated from-  

	File: QFE3320_RFFE_Settings.xlsx 
	Released: 3/27/2015
	Author: rspring
	Revision: 3.07
	Change Note: FOR: ET, Changed REG06 to 0x018 and REG30 to 0x17 for SOA for B12/17 ONLY
	Tab: qfe3320_gsm_pa_settings

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the QFE3320_GSM PA.

  Copyright (c) 2013, 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe3320/api/qfe3320_gsm_pa_config_ag.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 07:59:30 $ 

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "comdef.h"
#include "rfc_common.h"
#include "rfdevice_qpa_typedef.h"
#ifdef __cplusplus
extern "C" {
#endif  

boolean rfdevice_qpa_qfe3320_gsm_construct_driver_ag
(
  rfdevice_qpa_settings_type* qpa_settings,
  rfdevice_qpa_func_tbl_type* pa_fn_ptrs
);

extern void qfe3320_gsm_convert_port_mapping(rfcom_band_type_u band, rfdevice_pa_bw_enum_type bw, rfm_mode_enum_type mode, uint8 *port_index);



#ifdef __cplusplus
}
#endif
#endif