
#ifndef QPA4340_ASM_CONFIG_AG_H
#define QPA4340_ASM_CONFIG_AG_H/*
WARNING: This QPA4340 driver is auto-generated.

Generated using: qasm_2g_autogen.pl 
Generated from-  

	File: QPA4340_RFFE_Settings.xlsm 
	Released: 6/30/2016
	Author: rspring
	Revision: 2.6
	Change Note: Removed REG0 on ENABLE_TX script in ASM, as well as TRIGGER_ENABLE_RX.  This fixes the issue of having Band/BW dependent setting for PA.
	Tab: qpa4340_asm_settings

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qpa4340/api/qpa4340_asm_config_ag.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:25 $ 

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

 
#include "comdef.h"
#include "rfc_common.h"
#include "rfdevice_qasm_2g_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif  


boolean rfdevice_qasm_2g_qpa4340_construct_driver_ag
(
  rfdevice_qasm_2g_settings_type* qasm_settings
);

#ifdef __cplusplus
}
#endif
#endif