/*!
  @file
  rfwcdma_cmd_proc.c

  @brief
  This module is reponsible for WCDMA command processing
*/

/*==================================================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

==================================================================================================*/

/*==================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/mc/src/rfwcdma_cmd_proc.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
03/12/15   gh      Add dispatch handler for RFLM_WCDMA_DO_THERM_READ_CMD
06/19/14   aro     Added Support for Prep/Exec Temp Comp Split
05/15/14   aro     Registered do_mtpl command
05/15/14   aro     Added cmd proceccsing module initialization and deinitzation funtions
05/01/14   aro     Initial Release

==================================================================================================*/

#include "comdef.h"
#include "rfwcdma_cmd_proc.h"
#include "rflm_wcdma_msg.h"
#include "rfwcdma_core_txplim.h"
#include "rf_dispatch.h"
#include "rf_task.h"
#include "rfcommon_msg.h"
#include "msgr_umid.h"
#include "rfwcdma_core_temp_comp.h"
#include "rfwcdma_core_auto_pin.h"
#include "rfm_wcdma_umid.h"
#include "rf_apps_task.h"
#include "rfwcdma_core_temp_comp.h"

/*============================================================================*/
/*!
  @name RF APPS dispatcher

  @brief
  This module contains the interfaces exported by WCDMA to perform WCDMA
  command dispatching by RF Apps Task
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  RF APPS task command dispatch for WCDMA
 
  @details
  This function will be called by RF_APPS task when commands registered for
  WCDMA is called received by the task

  @param cmd
  Pointer to the data structure containing the commmand data
*/
static void
rfwcdma_apps_dispatch
(
  rf_apps_cmd_type *cmd
)
{
  /* Parameter validation */
  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR,"rfwcdma_apps_dispatch: Null Command Data");
    return;
  } /* if ( cmd == NULL ) */

  /* Dispatch command */
  switch ( MSGR_ID_VAL(cmd->msgr_hdr.id) )
  {
  case MSGR_ID_VAL(RFA_RF_WCDMA_TEMP_COMP_UPDATE_CMD):
    rfwcdma_temp_comp_update_handler();
    break;

  default:
    RF_MSG_1( RF_ERROR, "rfwcdma_apps_dispatch: Invalid Command 0x%x",
              cmd->msgr_hdr.id );
    break;
  } /* switch ( MSGR_ID_VAL(cmd->msgr_hdr.id) ) */

  return;
} /* rfwcdma_apps_dispatch */

/*! @} */

/*================================================================================================*/
/*!
  @name WCDMA command processor

  @brief
  This module contains the interfaces used to initizlize and de-initialize WCDMA command
  processor
*/
/*! @{ */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Perform WCDMA command processing initialization
 
  @details
  This function is called during WCDMA RF initialization to perform all necessary registration of
  commands that WCDMA RF is resposibble for handling.
 
  @return
  Flag indicating the status of initialization
*/
boolean
rfwcdma_cmd_proc_init
(
  void
)
{
  boolean ret_val = TRUE; /*  return variable  */

  /*----------------------------------------------------------------------------------------------*/
  /* Register all commands that RF WCDMA is the recipient of. These registrations are done
  with the dispatcher module with corresponding Task handler */
  /*----------------------------------------------------------------------------------------------*/

  /* Register command to perform MTPL computer as a part of Power Metering and estimation */
  ret_val &= rf_dispatch_register_command( rf_task_get_dispatch_handle(),
                                           MSGR_TECH_RFLM, 
                                           RFLM_WCDMA_DO_MTPL_CMD, 
                                           rfwcdma_core_txplim_do_mtpl_handler,
                                           NULL,
                                           1<<RF_DISPATACH_MSGR_REG );

  /* Register command to trigger THERM read */
  ret_val &= rf_dispatch_register_command( rf_task_get_dispatch_handle(),
                                           MSGR_TECH_RFLM, 
                                           RFLM_WCDMA_DO_THERM_READ_CMD, 
                                           rfwcdma_core_temp_comp_read_therm_handler,
                                           NULL,
                                           1<<RF_DISPATACH_MSGR_REG );

    /* Register command to perform auto pin process operation  */
  ret_val &= rf_dispatch_register_command( rf_task_get_dispatch_handle(),
                                           MSGR_TECH_RFLM, 
                                           RFLM_WCDMA_TRIGGER_AUTO_PIN_READ_CMD, 
                                           rfwcdma_auto_pin_trigger_read_handler,
                                           NULL,
                                           1<<RF_DISPATACH_MSGR_REG );


  /* Register RF APPS WCDMA dispatcher*/
  rf_apps_dispatch_register( rfwcdma_apps_dispatch, RF_APPS_WCDMA_ID );

  RF_MSG_1( RF_HIGH, "rfwcdma_cmd_proc_init: [Status %d]", ret_val );

  return ret_val;
} /* rfwcdma_cmd_proc_init */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Perform WCDMA command processing deinitialization
 
  @details
  This function is called during WCDMA RF einitialization to perform all necessary de-registration 
  of commands that WCDMA RF is resposibble for handling.
 
  @return
  Flag indicating the status of deinitialization
*/
boolean
rfwcdma_cmd_proc_deinit
(
  void
)
{
  boolean ret_val = TRUE; /*  return variable  */

  /*----------------------------------------------------------------------------------------------*/
  /* De-Register all commands */
  /*----------------------------------------------------------------------------------------------*/

  /* Register command to perform MTPL computer as a part of Power Metering and estimation */
  ret_val &= rf_dispatch_deregister_command( rf_task_get_dispatch_handle(),
                                             MSGR_TECH_RFLM, 
                                             RFLM_WCDMA_DO_MTPL_CMD );

  ret_val &= rf_dispatch_deregister_command( rf_task_get_dispatch_handle(),
                                             MSGR_TECH_RFLM, 
                                             RFLM_WCDMA_DO_THERM_READ_CMD );

  RF_MSG_1( RF_HIGH, "rfwcdma_cmd_proc_deinit: [Status %d]", ret_val );

  return ret_val;
} /* rfwcdma_cmd_proc_deinit */

/*! @} */

