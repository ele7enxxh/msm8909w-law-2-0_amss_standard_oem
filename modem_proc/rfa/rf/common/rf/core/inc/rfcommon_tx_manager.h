#ifndef RFCOMMON_TX_MANAGER_H
#define RFCOMMON_TX_MANAGER_H
/*!
  @file
  rfcommon_tx_manager.h

  @brief
  This module contains prototypes and definitions used by common Tx resource
  manager code.
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_tx_manager.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/26/13   aro     Renamed interface to indicate xpt_resource
03/26/13   aro     MC Callflow illustration
02/21/13   aro     Implemented LM allocation and deallocation for slave resource
02/04/13   aro     Migrated the variable to enable/disable slave resource
                   to RF common data
01/23/13   aro     Debug interface to enable and disable slave resource
12/27/12   aro     Added off-target helper function to query STM state
12/21/12   aro     Interface to release slave tx resource
12/21/12   aro     Interface to request slave tx resource
12/21/12   aro     Interface to release tx resource
12/19/12   aro     Structures to hold Tx resource mgr config data
12/19/12   aro     Interface to request tx resource
12/18/12   aro     Interface to initialize Tx resource manager
12/17/12   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "txlm_intf.h"
#include "lm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the callback data to be passed to the Tx Manager callback
  functions.
*/
typedef struct
{
  uint16 data_size;
  /*!< Size of data in bytes pointed by the opaque pointer. Based on this 
  dynamic memory allocation is done by Tx resource manager in and this many 
  bytes of data will be copied. */ 

  void *data_ptr;
  /*!< Opaque pointer pointing to the tech specific data to be used by 
  the function pointers. The type of this data need to be defined by each 
  tech. This data will be copied by the Tx resource manager and 
  stored internally; thus there is no mandatory need to perform dynamic 
  memory allocation by calling techs. However, tx resource manager will 
  internally perform dynamic memory allocation of data_size bytes and 
  trhe data will be copied. */ 

} rf_tx_resource_mgr_cb_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure used to return parameter from resource manager init function */
typedef struct
{
  boolean init_status;
  /*!< Flag indicating the status of initialization action */

  lm_handle_type lm_handle;
  /*!< LM handle allocated during initialization */

} rf_tx_resource_mgr_init_ret_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare the slave resource in standby mode after the resource could not be
  granted immediately
 
  @details
  This function will be executed when the slave resource could not be granted
  right away. This should basically perform housekeeping work assuming that
  the resource is moved to standby mode.
 
  All actions within this function needs to be synchronous, ie all actions 
  should finish before returning from the function
 
  @param device
  RFM device on which the resource is being moved to standby
 
  @param cb_data
  Opaque pointer pointing to the tech specific data to be used by the tech
  handler function
 
  @return
  Flag indicating the status of the function execution
*/
typedef boolean (*rf_tx_resource_mgr_prep_fp_type)
(
  rfm_device_enum_type device,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup the slave resource to move to ET mode after the resource is granted
 
  @details
  This function will be executed when the slave resource is granted to be
  moved to ET mode. This should sends a command to FW to indicate ET mode.
 
  All actions within this function needs to be synchronous, ie all actions 
  should finish before returning from the function
 
  @param device
  RFM device on which the resource is to be setup
 
  @param slave_txlm_handle
  TxLM handle pertaining to slave resource
 
  @param cb_data
  Opaque pointer pointing to the tech specific data to be used by the tech
  handler function
 
  @return
  Flag indicating the status of the function execution
*/
typedef boolean (*rf_tx_resource_mgr_setup_fp_type)
(
  rfm_device_enum_type device,
  lm_handle_type slave_txlm_handle,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Cleanup the slave resource to move out of ET mode after the resource is 
  released
 
  @details
  This function will be executed when the slave resource is released to be
  moved out of ET mode. This should send a command to FW to indicate no ET mode.
 
  All actions within this function needs to be synchronous, ie all actions 
  should finish before returning from the function
 
  @param device
  RFM device on which the resource is to be cleaned up
 
  @param slave_txlm_handle
  TxLM handle pertaining to slave resource
 
  @param cb_data
  Opaque pointer pointing to the tech specific data to be used by the tech
  handler function
 
  @return
  Flag indicating the status of the function execution
*/
typedef boolean (*rf_tx_resource_mgr_cleanup_fp_type)
(
  rfm_device_enum_type device,
  lm_handle_type slave_txlm_handle,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the resource
 
  @details
  This function will be called when the client (Tech specific MC) requests
  the Tx resource manager to acquire the slave resource. This will also be
  called internally by the STM, when a resource is to be moved to SLAVE mode
  for the first time for a given client.This function will be responsible for
  actions such as allocation of TxLM and updating TxLM SMEM with the ET mode 
  configuration.
 
  All actions within this function needs to be synchronous, ie all actions 
  should finish before returning from the function
 
  @param device
  RFM device on which the resource is to be initialized
 
  @param cb_data
  Opaque pointer pointing to the tech specific data to be used by the tech
  handler function
 
  @return
  Flag indicating the status of the function execution
*/
typedef rf_tx_resource_mgr_init_ret_type (*rf_tx_resource_mgr_init_fp_type)
(
  rfm_device_enum_type device,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-initialize the resource
 
  @details
  This function will be called when the client (Tech specific MC) requests
  the Tx resource manager to release the slave resource. This function will be 
  responsible for actions such as deallocation of TxLM and.
 
  All actions within this function needs to be synchronous, ie all actions 
  should finish before returning from the function
 
  @param device
  RFM device on which the resource is to be deinitialized
 
  @param slave_txlm_handle
  TxLM handle pertaining to slave resource
 
  @param cb_data
  Opaque pointer pointing to the tech specific data to be used by the tech
  handler function
 
  @param deinit_data
  Data to be used during de-initialization
 
  @return
  Flag indicating the status of the function execution
*/
typedef boolean (*rf_tx_resource_mgr_deinit_fp_type)
(
  rfm_device_enum_type device,
  lm_handle_type slave_txlm_handle,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
/*! Structure to store the list of function pointer to be passed into 
Tx resource manager */
typedef struct
{
  rf_tx_resource_mgr_init_fp_type init_fp;
  /*!< Pointer to the function to be executed, when the client (Tech specific 
  MC) requests the Tx resource manager to acquire the slave resource */
  
  rf_tx_resource_mgr_deinit_fp_type deinit_fp;
  /*!< Pointer to the function to be executed, when the client (Tech specific 
  MC) requests the Tx resource manager to release the slave resource */

  rf_tx_resource_mgr_prep_fp_type prep_fp;
  /*!< Pointer to the function to be executed, when slave tx resource could
  not be granted immediately */

  rf_tx_resource_mgr_setup_fp_type setup_fp;
  /*!< Pointer to the function to be executed, when slave tx resource needs to 
  be brought up in ET mode, after the resource is granted */

  rf_tx_resource_mgr_cleanup_fp_type cleanup_fp;
  /*!< Pointer to the function to be executed, when slave tx resource needs to 
  be moved out of ET mode, after the resource is released */

} rf_tx_resource_mgr_fp_type;

/*----------------------------------------------------------------------------*/
/*! Structure to be used the clients to pass the data during slave tx resource 
request */
typedef struct
{
  rf_tx_resource_mgr_fp_type func_ptrs;
  /*!< Structure to store the list of function pointer to be passed into 
  Tx resource manager */

  rf_tx_resource_mgr_cb_data_type cb_data;
  /*!<  Structure to hold the callback data to be passed to the Tx Manager 
  callback functions. */

} rf_tx_resource_mgr_config_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_tx_resource_mgr_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_request_master_tx_resource
(
  rfm_device_enum_type device,
  txlm_chain_type modem_chain,
  rfm_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
boolean
rf_release_master_tx_resource
(
  rfm_device_enum_type device,
  txlm_chain_type modem_chain,
  rfm_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
boolean
rf_request_xpt_tx_resource
(
  rfm_device_enum_type device,
  txlm_chain_type modem_chain,
  rfm_mode_enum_type mode,
  rf_tx_resource_mgr_config_data_type config_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_release_xpt_tx_resource
(
  rfm_device_enum_type device,
  txlm_chain_type modem_chain,
  rfm_mode_enum_type mode
);

#ifdef T_WINNT
#error code not present
#endif /* #ifdef T_WINNT */

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_TX_MANAGER_H */

