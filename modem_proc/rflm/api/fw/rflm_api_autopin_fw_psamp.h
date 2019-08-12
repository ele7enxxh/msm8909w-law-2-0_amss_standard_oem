#ifndef RFLM_API_AUTOPIN_FW_PSAMP_H
#define RFLM_API_AUTOPIN_FW_PSAMP_H

/*
  @file
  rflm_api_autopin_fw_psamp.h

  @brief
  This file contains interfaces defined by RFLM AutoPin for FW PSAMP.
*/

/*=============================================================================
 
  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved. By accepting this
  material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_autopin_fw_psamp.h#1 $

when       who    what, where, why
--------   ---    ------------------------------------------------------------- 
07/31/15   cdb    Initial version

===========================================================================*/

#include "fw_psamp_autopin_intf.h"

/* ===========================================================================
 * AutoPin Results 
 * =========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

/*!
  @brief
  Use by FW to provide IQ processing results to RFLM AUTOPIN.

  @detail
  Input: pointer to FW results of the autopin IQ processing
  
  @return
*/
/*============================================================================*/
extern void rflm_autopin_update_fw_amam_proc_result(fw_psamp_autopin_result_s * pout);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_API_AUTOPIN_FW_PSAMP_H */
