/*! 
 @file rf_tdscdma_core_xpt.c 
 
 @brief 
  This file has a collection of WCDMA XPT utility functions.
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/src/rf_tdscdma_core_xpt.c#1 $
  
When       who     what, where, why 
------------------------------------------------------------------------------- 
08/21/13   jhe     initial version
  
============================================================================*/

#include "rf_tdscdma_core_xpt.h"
#include "rfnv_tdscdma.h"
#include "rfcommon_core_xpt.h"
#include "rf_tdscdma_core_util.h"




/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get XPT Config Param from NV

  @details
  This function will query the XPT Config Param from NV

  @param band

  @param xpt_cfg_index
 
  @return
  Value of the requested XPT Config param
*/
uint32 rf_tdscdma_core_xpt_get_cfg_param
(
  rfcom_tdscdma_band_type band,
  uint8 xpt_cfg_index
)
{
  rfnv_tdscdma_tx_tbl_type *tx_nv_tbl = NULL;
  uint32 xpt_cfg_val = 0;

  tx_nv_tbl = rf_tdscdma_core_util_init_tx_nv_table(band);

  if ( tx_nv_tbl != NULL )
  {
    /* Getting the XPT config param value corresponding to the index */
    xpt_cfg_val = tx_nv_tbl->ept_dpd_cfg_params[xpt_cfg_index];
  }
  else
  {
    MSG_ERROR( "rf_tdscdma_core_xpt_get_cfg_param: Failed to get valid static NV pointer for band %d", band, 0, 0 );
    xpt_cfg_val = 0;
  }
 
  return xpt_cfg_val;
} /* rf_tdscdma_core_xpt_get_cfg_param */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if XPT is enabled
 
  @details
  This function will check the XPT NV to determine if XPT is enabled or not
 
  @param band
  TDSCDMA band for which XPT enable is to be checked
 
  @return
  Flag indicating if XPT is enabled or not.
*/
boolean rf_tdscdma_core_xpt_is_enabled
(
  rfcom_tdscdma_band_type band
)
{
  boolean is_xpt_enabled = FALSE;
  xpt_cfg_type nv_xpt_mode = 0;

  nv_xpt_mode = (xpt_cfg_type) rf_tdscdma_core_xpt_get_cfg_param(band, XPT_CFG);

  if ( ( nv_xpt_mode == EPT_CFG ) ||
       ( nv_xpt_mode == EPT_ET_CFG ) ||
       ( nv_xpt_mode == ET_CFG ) )
  {
    is_xpt_enabled = TRUE;
  }
  else
  {
    is_xpt_enabled = FALSE;
  }

  return is_xpt_enabled;

} /* rf_tdscdma_core_xpt_is_enabled */
