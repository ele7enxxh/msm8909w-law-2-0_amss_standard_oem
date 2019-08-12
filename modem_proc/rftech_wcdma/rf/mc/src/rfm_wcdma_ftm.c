/*! 
  @file
  rfm_wcdma_ftm.c
 
  @brief
  This file defines the RFM-WCDMA interfaces to be used by <b>FTM module 
  only</b>.
 
  @addtogroup RF_WCDMA_RFM_FTM
  @{
*/

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/mc/src/rfm_wcdma_ftm.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/06/14   aro     Support to override TempComp mode of operation
05/15/14   aro     Interface to override TxPlim override mode
02/04/14   vbh     Initial revison on Bolt. 
10/01/13   aro     Added interface to query debug flags
09/30/13   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfm_wcdma_ftm.h"
#include "rfwcdma_data.h"
#include "rfcom.h"
#include "rfcommon_msg.h"
#include "rfwcdma_core_util.h"
#include "rfm_wcdma.h"
#include "rfwcdma_core_txplim.h"

/*============================================================================*/
/*!
  @name Debug Flags

  @brief
  This section includes interfaces to update and query the debug flags used
  to WCDMA driver
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get WCDMA debug flags

  @details
  This function will return WCDMA debug flags

  @return
  Writeable Pointer to WCDMA debug flags
*/
rfwcdma_data_debug_flags_type*
rfm_wcdma_ftm_get_debug_flags
(
  void
)
{

  return rfwcdma_get_debug_flags();

} /* rfm_wcdma_ftm_get_debug_flags */

/*! @} */

/*============================================================================*/
/*!
  @brief
  Get current RxAGC information

  @details
  This function can be called from FTM to get the current RxAGC information

  @param device 
  RFM device on which the RxAGC value is requested
  
  @param multicarrier_idx
  RF COM multicarrier id type indicating the number of active carriers

  @param agc_unit
  Unit for representation of RxAGC value

  @param agc_info
  Pointer to hold the RxAGC information

  @return
  Flag indicating the status of the AGC info request
*/
boolean
rfm_wcdma_ftm_get_rxagc_info
(
  uint8 device,
  uint8 multicarrier_idx,
  uint8 agc_unit,
  rfm_wcdma_ftm_rxagc_info *agc_info
)
{
  int16 rxagc_val_db10;

  if (agc_info == NULL)
  {
    RF_MSG_1(RF_LOW, "rfm_wcdma_ftm_get_rxagc_info :"
               "Info pointer passed is NULL - Dev %d", device );
     return FALSE;
  } /* if (agc_info == NULL) */

  agc_info->lna_state = rfwcdma_core_util_get_lna_state(device);
  
  rxagc_val_db10 = rfm_wcdma_get_rxagc( device, 
                                        RFCOM_SERVINGCELL_RXAGC,
                                        agc_unit,
                                        multicarrier_idx );   

  agc_info->rxagc_val = -1060 + rxagc_val_db10 + 512;

  RF_MSG_4( RF_HIGH, "rfm_wcdma_ftm: AGC Info requested on Device: %d, " 
            "Multicarrier index :%d. RxAGC Value is: %d dBm, LNA state is:%d",
            device, multicarrier_idx, agc_info->rxagc_val, agc_info->lna_state);

  return TRUE; 

} /* rfm_wcdma_ftm_get_rxagc_info */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Override the power limiting mode of operation
 
  @details
  This function overrides the PLIM loop to enable or disable mode
 
  @param device
  RFM Device on which PLIM is to be overriden
 
  @param plim_enable
  Flag indicating, if the PLIM is to be enabled
 
  @return
  Status of plim override
*/
boolean
rfm_wcdma_ftm_plim_override_mode
(
  rfm_device_enum_type device,
  boolean plim_enable
)
{
  return ( rfwcdma_plim_override_mode(device, plim_enable));
  
} /* rfm_wcdma_ftm_plim_override_mode */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Override TempComp to disable or enable mode
 
  @details
  This function will be used to override the TempComp mode of operation to disable.
 
  @param device
  RFM device on which the temp comp is to be overridden
 
  @param enable_tempcomp
  Flag indicating if the temp comp is to be disabled or not
 
  @return
  Flag indication status of TempComp override
*/
boolean
rfm_wcdma_ftm_temp_comp_override
(
  rfm_device_enum_type device,
  boolean enable_tempcomp
)
{
  boolean ret_val = TRUE;

  if ( enable_tempcomp == TRUE )
  {
    ret_val &= rfwcdma_tempcomp_override_enable(device);
    RF_MSG_1( RF_HIGH, "rfm_wcdma_ftm_temp_comp_override: TempComp Overridden to enable - "
                       "Device %d", device );
  } /* if ( enable_tempcomp == TRUE ) */
  else
  {
    ret_val &= rfwcdma_tempcomp_override_disable(device);
    RF_MSG_1( RF_HIGH, "rfm_wcdma_ftm_temp_comp_override: TempComp Overridden to disable - "
                       "Device %d", device );
  } /* if ! ( enable_tempcomp == TRUE ) */

  return ret_val;

} /* rfm_wcdma_ftm_temp_comp_override */


/*! @} */

