/*! 
  @file
  rf_1x_mdsp_data.c
 
  @brief
  This file manages all the Data used by 1X RF-MDSP. This module is completely
  internal to this layer.
  
  @addtogroup RF_MDSP_CDMA_1X_DATA
  @brief
  This module includes all RF-MDSP related Data for 1x area. <b>This
  module will not export anything to other upper RF layer</b>.
  @{
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/qdsp6_cdma/src/rf_1x_mdsp_data.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/31/13   zhw     Added thread-safe locks for msgr logging data structure
05/31/13   APU     Optimized the check and retrieval of smem address.    
05/28/13   vr      Featurize to exclude from G+W only builds
07/06/12   sty     deleted unused var and APIs
12/14/11   aro     Migrated CDMA MDSP data to CDMA Data module
09/29/11   aro     Added interface to initialize MDSP Data
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
06/09/11   aro     Added accessor fun to get SMEM Data
06/08/11   aro     Added accessor func to get and set FW State data
06/08/11   aro     [1] Added Global Variable to store all 1x RF-MDSP data
                   [2] Added accessor functions for MSGR Messages and Log
06/08/11   aro     Initial Revision
 
==============================================================================*/

#include "comdef.h"

#ifdef FEATURE_CDMA1X
#include "rf_cdma_mdsp_types.h"
#include "rf_cdma_data.h"
#include "rfcommon_msg.h"

/*============================================================================*/
/*!
  @name Data Initialization

  @brief
  This section contains functions used to initialize 1x MDSP data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Initialize MDSP Data structure
 
  @details
  This function will intialize the 1x MDSP Data structure to default state
 
  @return
  Status of MDSP Data initialization
*/
boolean
rf_1x_mdsp_init_data
(
  void
)
{
  rf_cdma_mdsp_data_type *mdsp_data; /* Pointer to MDSP Data */

  /* Get pointer to MDSP Data */
  mdsp_data = rf_cdma_get_mdsp_data();

  /* NULL pointer  */
  if ( mdsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_mdsp_init_data: NULL MDSP Data" );
    return FALSE;
  } /* if ( mdsp_data == NULL ) */

  mdsp_data->onex_mdsp.msg_log.log_en = TRUE;

  /* Initialize msgr logging critical section */
  rf_common_init_critical_section( &(mdsp_data->onex_mdsp.msg_log.crit_section));

  return TRUE;

} /* rf_1x_mdsp_init_data */

/*! @} */

/*============================================================================*/
/*!
  @name Message Router Data Access

  @brief
  This section contains functions used to query Data related to Message Router
  interface.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Pointer to Message Router Data structure

  @details
  This function will return the pointer to the message router data structure.
  This pointer will be used by RF-MDSP itnerface source file to update the 
  message payload.

  @return
  Pointer to the Message router data structure
*/
rf_1x_mdsp_messages_type*
rf_1x_mdsp_get_msgr_data
(
  void
)
{
  rf_cdma_mdsp_data_type *mdsp_data; /* Pointer to MDSP Data */

  /* Get pointer to MDSP Data */
  mdsp_data = rf_cdma_get_mdsp_data();

  /* NULL pointer  */
  if ( mdsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_mdsp_get_msgr_data: NULL MDSP Data" );
    return NULL;
  } /* if ( mdsp_data == NULL ) */

  return &(mdsp_data->onex_mdsp.msgr_data);
} /* rf_1x_mdsp_get_msgr_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Pointer to Message Router Log Data structure

  @details
  This function will return the pointer to the message router Log  data 
  structure.

  @return
  Pointer to the Message router log data structure
*/
rf_1x_mdsp_msg_log_type*
rf_1x_mdsp_get_msgr_log_data
(
  void
)
{
  rf_cdma_mdsp_data_type *mdsp_data; /* Pointer to MDSP Data */

  /* Get pointer to MDSP Data */
  mdsp_data = rf_cdma_get_mdsp_data();

  /* NULL pointer  */
  if ( mdsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_mdsp_get_msgr_log_data: NULL MDSP Data" );
    return NULL;
  } /* if ( mdsp_data == NULL ) */

  return &(mdsp_data->onex_mdsp.msg_log);

} /* rf_1x_mdsp_get_msgr_log_data */

/*! @} */

/*============================================================================*/
/*!
  @name Shared Memory Data Access

  @brief
  This section contains functions used to query Data related to Shared Memory
  interface.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Pointer to Shared Memory Data structure

  @details
  This function will return the pointer to the shared memeory data structure.
  This pointer will be used by RF-MDSP itnerface source file to update or 
  read FW data. This function also stores the memory pointer
  after validating it for the pointer being non-null and if the
  SMEM has been initialized.
 
  @param success
  Output TRUE  If API is successful FALSE otherwise.
 
  @return 
  Pointer to the Shared Memory data structure
*/
rf_1x_mdsp_smem_data_type*
rf_1x_mdsp_get_validated_smem_data   
(
   boolean* success 
)
{
  static boolean is_smem_valid_and_initialized = FALSE ;
  static rf_1x_mdsp_smem_data_type* smem_data = NULL ;

  /* Do expensive one time check */
  if ( FALSE == is_smem_valid_and_initialized )
  {
    smem_data = rf_1x_mdsp_get_smem_data() ;
    if ( smem_data == NULL )
    {
      RF_MSG( RF_ERROR, "rf_1x_mdsp_get_validated_smem_data: SMEM NULL" ) ;
    }  
    else
    {  /* Check if 1X SMEM is initialized */
      if ( smem_data->smem_initialized != TRUE )
      {
        RF_MSG( RF_ERROR,"rf_1x_mdsp_get_validated_smem_data: SMEM not "
                         "Initialized" ) ;
      } 
      else
      {
        is_smem_valid_and_initialized = TRUE ;
      }
    }
  }
  *success = is_smem_valid_and_initialized ;
  return smem_data ;
}

/*!
  @brief
  Get Pointer to Shared Memory Data structure

  @details
  This function will return the pointer to the shared memeory data structure.
  This pointer will be used by RF-MDSP itnerface source file to update or 
  read FW data.

  @return
  Pointer to the Shared Memory data structure
*/
rf_1x_mdsp_smem_data_type*
rf_1x_mdsp_get_smem_data
(
  void
)
{
  rf_cdma_mdsp_data_type *mdsp_data; /* Pointer to MDSP Data */

  /* Get pointer to MDSP Data */
  mdsp_data = rf_cdma_get_mdsp_data();

  /* NULL pointer  */
  if ( mdsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_mdsp_get_smem_data: NULL MDSP Data" );
    return NULL;
  } /* if ( mdsp_data == NULL ) */

  return &(mdsp_data->onex_mdsp.smem_data);

} /* rf_1x_mdsp_get_smem_data */

#endif
/*! @} */

/*! @} */

