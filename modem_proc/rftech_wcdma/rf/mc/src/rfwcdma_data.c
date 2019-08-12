/*! 
  @file
  rfwcdma_data.c
 
  @brief
  This file contains Centralized Global Data used by Common WCDMA Driver.
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/mc/src/rfwcdma_data.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/10/14   vs      Added flag to enable nbee debug info
11/17/14   rmb     Added Dynamic spur mitigation feature flag. 
07/10/14   aa      Enable plim
06/30/14   aro     Added Plim Data Init function
06/26/14   vbh     Initialize the dbg flag for script based tune 
06/19/14   aro     Added Support for Prep/Exec Temp Comp Split
06/06/14   ac      nbr support
05/15/14   aro     Interface to override TxPlim override mode
02/04/14   vbh     Initial revision on Bolt  
10/01/13   aro     Converted variable to non STATIC
09/30/13   aro     Doxygen Update
09/30/13   aro     Add Data module initialization
09/30/13   aro     Variable to enable/disable wakeup optimization
09/13/13   aro     Added data structure for critical section lock
09/13/13   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfwcdma_data.h"
#include "rfcommon_msg.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Variable to store global Data to be used by WCDMA Driver
*/
static rfwcdma_data_type rfwcdma_data;

/*----------------------------------------------------------------------------*/
/*! Temporary Flag to enable wakeup optimzation */
boolean rfwcdma_enable_wakeup_opt = TRUE;

/*============================================================================*/
/*!
  @name Critical Section

  @brief
  This section includes interfaces to update and query the critical section
  data
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set WCDMA Lock Data

  @details
  This function will set the WCDMA LOCK data.

  @param lock_data
  Lock data to be set as WCDMA Lock
*/
void
rfwcdma_set_lock_data
(
  rf_lock_data_type *lock_data
)
{
  rfwcdma_data.intf_crit_section = lock_data;
} /* rf_cdma_get_lock_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  GetW CDMA Lock Data

  @details
  This function will return the WCDMA LOCK data.

  @return
  Writeable Pointer to WCDMA Lock Data
*/
rf_lock_data_type*
rfwcdma_get_lock_data
(
  void
)
{

  /* throw error message if NULL pointer */
  if ( rfwcdma_data.intf_crit_section == NULL )
  {
    RF_MSG( RF_ERROR,"rfwcdma_get_lock_data: Null pointer !! "
                     "Critical Section not initialized !!");
  } /* if ( rfwcdma_mc_state.crit_section == NULL ) */

  return rfwcdma_data.intf_crit_section;
} /* rfwcdma_get_lock_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get WCDMA temp comp Lock Data

  @details
  This function will return the WCDMA temp comp LOCK data.

  @return
  Writeable Pointer to WCDMA temp comp Lock Data
*/
rf_lock_data_type*
rfwcdma_get_temp_comp_lock_data
(
  void
)
{

  return &(rfwcdma_data.tempcomp_crit_section);
} /* rfwcdma_get_temp_comp_lock_data */

/*! @} */


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
  Init WCDMA debug flags

  @details
  This function will initialize WCDMA debug flags
*/
static void
rfwcdma_init_debug_flags
(
  void
)
{
  rfwcdma_data.debug_flags.enable_opt_wakeup = rfwcdma_enable_wakeup_opt;
  
  rfwcdma_data.debug_flags.enable_rf_nbr_operations = TRUE;
  
  rfwcdma_data.debug_flags.enable_script_based_tune = FALSE; 
  
  rfwcdma_data.debug_flags.apply_notch_msm = TRUE;
  
  rfwcdma_data.debug_flags.enable_nbee_cal_dbg = FALSE;
  
} /* rfwcdma_init_debug_flags */

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
rfwcdma_get_debug_flags
(
  void
)
{

  return &(rfwcdma_data.debug_flags);

} /* rfwcdma_get_debug_flags */

/*! @} */


/*============================================================================*/
/*!
  @name Power Limiting

  @brief
  This section includes interfaces used for WCDMA power limiting
*/
/*! @{ */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get WCDMA Power limiting data

  @details
  This function will return WCDMA power limiting data

  @return
  Writeable Pointer to WCDMA power limiting data
*/
rfwcdma_power_limiting_data_type*
rfwcdma_get_power_limiting_data
(
  void
)
{
  return &(rfwcdma_data.plim_data);

} /* rfwcdma_get_power_limiting_data */

/*! @} */


/*============================================================================*/
/*!
  @name Data Initialization

  @brief
  This section includes interfaces pertaining to initialization of WCDMA
  Data module
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Init WCDMA Power limiting data

  @details
  This function will initialize WCDMA power limiting data
*/
static void
rfwcdma_init_data_plim
(
  void
)
{

  /* Enable Plim by default. During Tx initialization, HDET NV will also be checked to enable or
  disable PLIM. This flag is mainly used for debugging purpose*/
  rfwcdma_data.plim_data.enable_plim = TRUE;

} /* rfwcdma_init_data_plim */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Init WCDMA Data module

  @details
  This function will initialize WCDMA data module
*/
void
rfwcdma_init_data
(
  void
)
{

  rfwcdma_init_debug_flags();

  rfwcdma_init_data_plim();

} /* rfwcdma_init_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get pointer to temperature compensation data
 
  @param device
  RFM device for which the temp comp data is requested
 
  @return
  Pointer to temperature compensation data for the given device
*/
rfwcdma_temp_comp_data_type*
rfwcdma_data_get_temp_comp
(
   rfm_device_enum_type device
)
{
  if ( device < RFWCDMA_DATA_MAX_DEVICE )
  {
    return &(rfwcdma_data.radio_status[device].temp_comp);
  } /* if ( device < RFWCDMA_DATA_MAX_DEVICE ) */
  else
  {
    return NULL;
  } /* if ( device < RFWCDMA_DATA_MAX_DEVICE ) */
} /* rfwcdma_data_get_temp_comp */

/*! @} */

