#ifndef RFWCDMA_CORE_XPT_H
#define RFWCDMA_CORE_XPT_H
/*! 
 @file rfwcdma_core_xpt.h 
 
 @brief 
  This is the header file of WCDMA XPT utility functions.
 
*/ 
  
/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_core_xpt.h#1 $
  
When       who     what, where, why 
------------------------------------------------------------------------------- 
09/15/14   vbh     Added device as a parameter to support Tx sharing
02/26/14   zhh    Update XPT Delay structure to support DC-UPA
07/30/13   kai     Added forcing Delay vs Temp comp flag. 
06/23/13   kai     Added WCDMA 16channels xpt delay support
02/27/13   kai     initial version
  
============================================================================*/ 

#include "rfcom.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean rfwcdma_core_xpt_et_enable
(
  rfcom_wcdma_band_type curr_band_tx
);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_core_xpt_et_disable(void);

/*----------------------------------------------------------------------------*/
uint32 rfwcdma_core_xpt_get_cfg_param
(
  rfcom_wcdma_band_type band,
  uint8 xpt_cfg_index
);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_core_xpt_is_enabled
(
  rfcom_wcdma_band_type band
);

/*----------------------------------------------------------------------------*/
int32 rfwcdma_core_xpt_path_delay_comp
(
   rfm_device_enum_type device,
   rfcom_wcdma_band_type band,
   uint16 rf_chan_tx,
   boolean force_delay_temp_comp,
   uint16 therm_scaled_value,
   rfcom_multi_carrier_hspa_id_type tx_multi_carriers
);

#ifdef __cplusplus
}
#endif

#endif /* RFWCDMA_CORE_XPT_H */

