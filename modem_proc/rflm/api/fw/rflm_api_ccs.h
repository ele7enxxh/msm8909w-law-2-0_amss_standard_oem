/*!
  @file
  rflm_api_ccs.h

  @brief
  RFLM API CCS PLL configurations and voting
  
  @detail
  This file contains RFLM API CCS PLL configurations and voting

*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_ccs.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/30/14   ra     Initial version.
==============================================================================*/

#ifndef RFLM_API_CCS_FW_H
#define RFLM_API_CCS_FW_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_cmn.h"



#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  uint32  * ccs_info; /* imported by fw image download . contain ccs rf image information */
  uint32 * ccs_intf; /* imported by fw image download . contain ccs rf image interface */
} rflm_ccs_image_params_t;

/*==============================================================================

FUNCTION:  rflm_CCS_configure_plls

==============================================================================*/
/*!
  @brief
  Configure CCS image data pointers

  @detail
  FW downloades the CCS image in certain targets. Solely in this case, FW will
  call this API to provide pointers obtained from the RF portion of CCS image 
 
  @return
  void
*/
/*============================================================================*/
extern void rflm_ccs_set_image_params(uint32 ccs_info_addr, uint32 ccs_intf_addr );


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* RFLM_API_CCS_FW_H */

