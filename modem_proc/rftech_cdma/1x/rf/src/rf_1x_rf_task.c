/*! 
  @file
  rf_1x_rf_task.c
 
  @brief
  This file includes functions pertaining to Tasks used for 1x Operation
 
  @addtogroup RF_CDMA_1X_RF_TASK
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rf_1x_rf_task.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/03/14   cc      Fix rf_cdma_update_agc crash when it's called
05/09/13   APU     Check for msgr_register()'s return value and throw an error on
                   failure.
05/08/13   APU     Fixed 
                   1> A pointer dereferencing bug.
                   2> Register COEX_TX_PWR_LIMIT_REQ with Message router. 
01/30/12   spa     Added support for setting Tx coex power limit.  
10/05/12   av      Expanded rf task architecture to include more tasks. 
07/23/12   Saul    CDMA. Removed unnecessary header.
04/17/12   hdz     Removed RFA_RF_1X_TEMP_COMP_READ_CMD from rf_1x_rf_task;
                   Added it to rf_apps_task
03/28/12   hdz     Added case RFA_RF_1X_TXRXAGC_RELOAD_CMD in dispatcher
03/22/12   aro     Deleted cdma_rf_task file
03/22/12   aro     Removed unsed functions and types
03/22/12   aro     Migrated handling of HDR FW responses to RF_FWRSP task
03/22/12   aro     Deleted unused types/functions
03/22/12   aro     Added data structure to hold FW responses data with
                   an accessor function
03/22/12   aro     Migrated 1x FW response handling to RF_FWRSP task
03/22/12   aro     Deleted old RF_TASK based dispatching of CDMA
                   firmware responses
03/21/12   cd      Remove F3 from dispatch function
03/18/12   aro     Added UMID logging
03/15/12   aro     Fixed invalid check of UMIDs in dispatch function
03/15/12   aro     Added F3 message
03/13/12   aro     Added voting mechanism to ensure that all modules and
                   techs are de-registered before de-allocating the dynamically
                   allocated memeory for Tech and module tables
03/13/12   aro     Add feature to dynamically grow the tech table based on
                   maximum number of Tech IDs
03/13/12   aro     Code to de-register RFA Tech
03/13/12   aro     Updated dispatch handler function to take only command
                   pointer as argument.
03/13/12   aro     Moved FW State config and DAC cal response handling to RF
03/13/12   aro     FW State configuration message
03/13/12   aro     DAC Cal response message
03/13/12   aro     Implemented function to de-initilize RF_TASK dispatch
                   for 1x
03/12/12   aro     Function to perform initialization of 1x dispatcher
                   for RF_TASK. Function does tech / module registration with
                   RF_TASK, msgr registration, and semaphore initialization
03/12/12   aro     Handler functions for 1x UMIDs 
03/12/12   aro     Added handler for 1x Temp Comp read 
03/12/12   aro     Added MSGR commads IDs that can be handled by 1x RF.
03/09/12   aro     Initial 1x RF Task dispatcher implementation
03/08/12   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_1x_rf_task.h"
#include "rf_1x_mc.h"
#include "rfm_1x_umid.h"
#include "rf_cdma_data.h"
#include "rfcommon_msg.h"
#include "rf_cmd_dispatcher.h"
#include "cdmafw_msg.h"
#include "msgr_umid.h"
#include "rfa_msgr.h"
#include "rf_cdma_mc.h"
#include "rf_task_common_services.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API called whenever there is a request for setting coex Tx power limit
 
  @details
  This function is called whenever there is a request for setting the coex Tx 
  power limit from MCS, it processes the message for MC layer
 
  @param msg_ptr
  Pointer to the message structure sent by MCS
*/
static void
rf_1x_rf_task_set_coex_tx_pwr_limit
(
  rfa_1x_set_coex_tx_pwr_limit_req_s* msg_ptr
)
{
  RF_MSG( RF_HIGH, "rf_1x_rf_task_set_coex_tx_pwr_limit: updating SAR/Coex"
                   "Tx power limit not supported for 1x at this point" );

  return;
}/*rf_1x_rf_task_set_coex_tx_pwr_limit*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch function for 1x commands dispatched from RF_TASK
 
  @details
  This function is called by RF_TASK when it receives messages/commands for
  1x
  
  @param req_ptr
  Pointer to RF Task command
*/
static void
rf_1x_rf_task_dispatch
(
  rf_cmd_type* req_ptr
)
{

  msgr_umid_type rf_uimd; /* Received UMID */

  /* Validate Request Pointer */
  if( req_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_rf_task_dispatch: NULL Request" );
    return;
  } /* if( req_ptr == NULL ) */

  /* Get UMID */
  rf_uimd = req_ptr->msgr_hdr.id;

  /* Check if the dispatched message is valid for 1x */
  if( MSGR_TECH_MODULE_VAL(rf_uimd) != MSGR_RFA_RF_1X )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_rf_task_dispatch: Cannot handle UMID 0x%x",
              rf_uimd );
    return;
  }

  /* Dispatch based on the UMID */
  switch( rf_uimd )
  {
    case RFA_RF_1X_TXRX_AGC_RELOAD_CMD:
      rf_cdma_update_agc();
      break;

    case RFA_RF_1X_SET_COEX_TX_PWR_LIMIT_REQ:
      /* This API will process the message for MC */
      rf_1x_rf_task_set_coex_tx_pwr_limit((rfa_1x_set_coex_tx_pwr_limit_req_s*) 
                                           &(req_ptr->msgr_hdr) );
      break;

    default:
      RF_MSG_1( RF_ERROR, "rf_1x_rf_task_dispatch: Invalid UMID 0x%x",
                rf_uimd );
      break;
  } /* switch( req_ptr->msgr_hdr.id ) */

  return;

} /* rf_1x_rf_task_dispatch */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize dispatcher for CDMA commands processed through RF Task

  @details
  This function will register all CDMA commands to RF Task dispatcher and
  also registers with message router, if required.
 
  @param msgr_client
  Message router client ID for registration
 
  @param mailbox
  Messgae router ID for registration

  @return
  TRUE on success and FALSE on failure
*/
boolean
rf_1x_rf_task_dispatch_init
(
  msgr_client_t *msgr_client, 
  msgr_id_t mailbox 
)
{
  rf_cmd_dispatch_status_type dis_res; /* cmd_dispatch result */
  boolean init_status = TRUE;

  /* Register handler for this function */
  dis_res = rf_cmd_dispatch_register_tech(RF_TASK_MAIN, MSGR_TECH_RFA, MSGR_MODULE_RF_1X,
                                           rf_1x_rf_task_dispatch );

  if ( dis_res != RF_CMD_REGISTRATION_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_1x_rf_task_dispatch_init: Failed RFA / RF_1X Tech "
                      "registration"  );    
    init_status = FALSE;
  }

  /* Register with Message router */
  if ( E_SUCCESS != msgr_register ( MSGR_RFA_RF_1X , msgr_client, mailbox,
                              RFA_RF_1X_SET_COEX_TX_PWR_LIMIT_REQ ) )
  {
    RF_MSG( RF_ERROR, 
            "rf_1x_rf_task_dispatch_init: Failed msgr_register Tech registration" ) ;    
    init_status = FALSE;
  }
   /* Register with Message router */
  if ( E_SUCCESS != msgr_register ( MSGR_RFA_RF_1X , msgr_client, mailbox,
                              RFA_RF_1X_TXRX_AGC_RELOAD_CMD ) )
  {
    RF_MSG( RF_ERROR, 
            "rf_1x_rf_task_dispatch_init: Failed msgr_register Tech registration" ) ;    
    init_status = FALSE;
  }

  /* Return Status */
  return init_status;

} /* rf_1x_rf_task_dispatch_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize dispatcher for CDMA commands processed through RF Task

  @details
  This function will unregister message router commands, release
  allocated semphores and unrgister RF task.
 
  @param msgr_client
  Message router client ID for unregistration
 
  @return
  TRUE on success and FALSE on failure
*/
boolean
rf_1x_rf_task_dispatch_deinit
(
  msgr_client_t *msgr_client
)
{
  boolean deinit_status = TRUE;

  /*--------------------------------------------------------------------------*/
  /* RF Task de-registration */
  /*--------------------------------------------------------------------------*/

  rf_cmd_dispatch_deregister_tech( RF_TASK_MAIN, MSGR_TECH_RFA, MSGR_MODULE_RF_1X );

  /* Return Status */
  return deinit_status;

} /* rf_1x_rf_task_dispatch_deinit */
#endif /* FEATURE_CDMA1X */

/*! @} */


