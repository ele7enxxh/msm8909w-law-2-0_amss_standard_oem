
#ifndef QFE1040_HB_W1_V00_ASM_CONFIG_AG_H
#define QFE1040_HB_W1_V00_ASM_CONFIG_AG_H/*
WARNING: This QFE1040_HB_W1_V00 driver is auto-generated.

Generated using: qasm_autogen.pl 
Generated from-  

	File: QFE1040_RFFE_Settings.xlsm 
	Released: 1/13/2015
	Author: Ryan Spring
	Revision: 10.18
	Change Note: Added Enable/Disable TX rows on v40 HB W1 for THOR ONLY and NOT FOR BOLT.
	Tab: qfe1040_hb_w1_v00_asm_settings

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe1040/api/qfe1040_hb_w1_v00_asm_config_ag.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 07:59:16 $ 

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

 
#include "comdef.h"
#include "rfc_common.h"
#include "rfdevice_qasm_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif  


boolean rfdevice_qasm_qfe1040_hb_w1_v00_construct_driver_ag
(
  rfdevice_qasm_settings_type* qasm_settings,
  rfdevice_qasm_func_tbl_type* asm_fn_ptrs
);

#ifdef __cplusplus
}
#endif
#endif