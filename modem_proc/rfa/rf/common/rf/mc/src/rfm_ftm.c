/*! 
  @file
  rfm_ftm.c
 
  @brief
  This file defines the RFM interfaces to be used by <b>FTM module only</b>.
 
  @addtogroup RF_RFM_FTM
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rfm_ftm.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/06/14   aro     Support to override ILPC mode of operation
11/14/13   cd      Support for RFLM-based Tx AGC override
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch 
02/04/13   aro     Migrated the variable to enable/disable slave resource
                   to RF common data
01/23/13   aro     Command to perform slave resource configuration in 
                   Tx resource manager
01/23/13   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_data.h"
#include "ftm_common_xpt.h"
#include "rfcommon_mc.h"
#include "rfcommon_fbrx_ilpc.h"

/*============================================================================*/
/*!
  @name Tx Resource Manager

  @brief
  This section contains functions pertainingt to tx resource manager
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable Slave resource
 
  @details
  This function will be called in debug mode to enable the slave resource. Slave
  resource will be enabled by default; however, if the slave resource is
  disabled for debug purpose, then this function can be called to undo the 
  process. With this, the normal execution of ET/EPT transition will happen
  based on SV or nonSV scenario.
 
  The effect of this function will take place in next tx wakeup sequence. 
*/
boolean
rfm_ftm_configure_xpt_override
(
  rfm_device_enum_type device,
  boolean override_en,
  xpt_cfg_type xpt_mode
)
{

  return (rfcommon_configure_xpt_override( device, override_en, xpt_mode ));

} /* rfm_ftm_configure_xpt_override */

/*! @} */

/*============================================================================*/
/*!
  @name Tx control functionality

  @brief
  This section contains functions pertaining to tx control functionality
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Apply xPT Tx override parameters through common interface for all techs
 
  @details
  Retrieves the current set of Tx overrides from FTM common data and passes it
  on for processing through the common MC interface. This API results in 
  the actual applying of override parameters to HW
  
  @param rfm_mode
  RFM mode for which Tx override is requested

  @param device
  RFM device for which Tx override is requested

  @param rfm_band
  Holder for tech specific band information

  @return
  TRUE if successful, else FALSE
*/
boolean
rfm_ftm_apply_xpt_tx_agc_override
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band
)
{
  boolean api_status = TRUE;
  rfcommon_core_xpt_tx_override_type tx_ovr_params;

  /* Get curr Tx override params from FTM common data */
  api_status &= ftm_common_xpt_get_curr_tx_override( device, &tx_ovr_params );

  /* Common interface to override all Tx params to hardware */
  api_status &= rfcommon_mc_xpt_txagc_override_v2( rfm_mode, device, 
                                                   rfm_band,
                                                   &tx_ovr_params );

  RF_MSG_1( RF_MED, "rfm_ftm_apply_xpt_tx_agc_override: Status %d ", 
            api_status );

  return api_status;

} /* rfm_ftm_apply_xpt_tx_agc_override */

/*! @} */

/*============================================================================*/
/*!
  @name FBRx

  @brief
  This section contains functions pertaining to FBRx
*/
/*! @{ */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Override ILPC to disable or enable mode
 
  @details
  This function will be used to override the ilpc mode of operation to disable.
 
  @param device
  RFM device on which the ILPC is to be overridden
 
  @param ilpc_enable
  Flag indicating if the ILPC is to be disabled or not
 
  @return
  Flag indication status of ILPC override
*/
boolean
rfm_ftm_fbrx_iplc_override
(
  rfm_device_enum_type device,
  boolean ilpc_enable
)
{
  boolean ret_val = TRUE;

  if ( ilpc_enable == TRUE )
  {
    ret_val &= rfcommon_fbrx_iplc_override_enable();
    RF_MSG_1( RF_HIGH, "[FBRX.ILPC] rfm_ftm_fbrx_iplc_override: ILPC Overridden to disable - "
                       "Device %d", device );
  } /* if ( ilpc_enable == TRUE ) */
  else
  {
    ret_val &= rfcommon_fbrx_iplc_override_disable();
    RF_MSG_1( RF_HIGH, "[FBRX.ILPC] rfm_ftm_fbrx_iplc_override: ILPC Overridden to enable - "
                       "Device %d", device );
  } /* if ! ( ilpc_enable == TRUE ) */

  return ret_val;

} /* rfm_ftm_fbrx_iplc_override */


/*! @} */



/*! @} */

