#ifndef RF_TDSCDMA_CORE_XPT_H
#define RF_TDSCDMA_CORE_XPT_H
/*! 
 @file rf_tdscdma_core_xpt.h 
 
 @brief 
  This is the header file of TDSCDMA XPT utility functions.
 
*/ 
  
/*==============================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/
  
/*==============================================================================  
                           EDIT HISTORY FOR FILE 
  
  This section contains comments describing changes made to this file. 
  Notice that changes are listed in reverse chronological order. 

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma_core_xpt.h#1 $
  
When       who     what, where, why 
------------------------------------------------------------------------------- 
08/21/13   jhe     initial version
  
============================================================================*/ 

#include "rfcom.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
uint32 rf_tdscdma_core_xpt_get_cfg_param
(
  rfcom_tdscdma_band_type band,
  uint8 xpt_cfg_index
);

/*----------------------------------------------------------------------------*/
boolean rf_tdscdma_core_xpt_is_enabled
(
  rfcom_tdscdma_band_type band
);

#ifdef __cplusplus
}
#endif

#endif /* RF_TDSCDMA_CORE_XPT_H */

