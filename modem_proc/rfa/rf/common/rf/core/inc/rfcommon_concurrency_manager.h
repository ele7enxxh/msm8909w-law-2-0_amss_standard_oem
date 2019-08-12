#ifndef RFCOMMON_CONCURRENCY_MANAGER_H
#define RFCOMMON_CONCURRENCY_MANAGER_H
/*!
   @file
   rfcommon_concurrency_manager.h 

   @brief
 
 
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
Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 

                       EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------------------------------------------------------------------------------
11/10/14   kg      Add ftm api for setting concurrency manager debug mode
08/04/14   tks     Updated return type for validate scenario api 
07/22/14   tks     Updated concurrency mgr interface 
05/28/14   tks     Added interface api's for trm support
05/12/14   tks     Updated api names to indicate rx behavior
04/23/14   tks     Initial revision 
 
==============================================================================*/

/*----------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                  */
/*----------------------------------------------------------------------------*/
#include "rfm_device_types.h"
#include "rfcom.h" 
#include "rfc_common.h"
#include "rfm_device_config_type.h"

/*----------------------------------------------------------------------------*/
/*                              DEFINTION                                     */ 
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to capture necessary elements per logical device 
*/
typedef struct
{
  rfm_device_enum_type logical_dev; /*!< Current logical device */
  rfcom_mode_enum_type curr_mode;   /*!< Current tech in use    */
  int                  curr_band;   /*!< Current band in use    */
  uint8                alt_path;    /*!< Alternate path         */
  uint8                wb_index;    /*!< Wideband index to use  */
}rfcmn_state_info_type; 

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to capture the current state of each logical device 
 
*/
typedef struct
{
  /* State information for all WAN related logical devices */
  rfcmn_state_info_type curr_state[RFM_MAX_WAN_DEVICES]; 
}rfcmn_state_mgr_type; 

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration to indicate specific status
 
*/
typedef enum
{
  /* Indicates path selection op failed */ 
  RFCMN_PATH_SEL_FAILED,        

  /* Indicates path selection op succeeded */
  RFCMN_PATH_SEL_SUCCESS,       

  /* Indicates path selection isn't supported */
  RFCMN_PATH_SEL_NOT_SUPPORTED, 

  /* Indicates ALT PATH reconfig failed in RFC */
  RFCMN_PATH_SEL_PORT_UPDATE_FAILED, 

  /* Indicates concurrency manager state update is successful */
  RFCMN_CONCURRENCY_MGR_UPDATE_SUCCESS, 
  
  /* Indicates concurrency manager state update has failed */
  RFCMN_CONCURRENCY_MGR_UPDATE_FAILED, 

  /* Indicates desired path select is invalid */
  RFCMN_PATH_SEL_INVALID

}rfcmn_status_type;

/*----------------------------------------------------------------------------*/
/*                          FUNCTION PROTOTYPE                                */ 
/*----------------------------------------------------------------------------*/

// Only interface function prototypes are defined here 

/*----------------------------------------------------------------------------*/
rfcmn_status_type rfcmn_concurrency_mgr_rx_init
(
  void
); 

/*----------------------------------------------------------------------------*/
rfcmn_status_type rfcmn_concurrency_mgr_update_rx_state
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band
); 

/*----------------------------------------------------------------------------*/
rfcmn_status_type rfcmn_concurrency_mgr_rx_cleanup
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band
);

/*----------------------------------------------------------------------------*/
rfcmn_status_type rfcmn_concurrency_mgr_update_rx_path
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band,
  uint8 *alt_path
); 

/*----------------------------------------------------------------------------*/
rfcmn_status_type rfcmn_concurrency_mgr_update_irat_rx_path
(
  rfm_device_enum_type rfm_dev,
  rfcom_mode_enum_type curr_mode,
  int curr_band,
  uint8 *alt_path
); 

/*----------------------------------------------------------------------------*/
boolean rfcmn_concurrency_mgr_rx_update_device_alloc
(
  rfm_device_allocation_type *dev_list
); 

/*----------------------------------------------------------------------------*/
rfm_device_allocation_status_type* rfcmn_concurrency_mgr_rx_validate_scenario
(
  rfm_device_client_info_type *dev_client
); 

/*----------------------------------------------------------------------------*/
boolean rfcmn_concurrency_mgr_set_debug_mode
(
  uint32 debug_mask,
  boolean debug_en
);

#endif
