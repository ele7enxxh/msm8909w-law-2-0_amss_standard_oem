/*! 
  @file
  rf_1x_fw_response.c
 
  @brief
  This file includes functions pertaining to handling of FW responses for 1x
 
  @addtogroup RF_CDMA_1X_RF_FW_RESPONSE
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rf_1x_fw_response.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/23/13   sty     compiler warning fixes
06/03/13   spa     Migrate to new RF dispatch for XPT FW response mssgs 
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation 
04/05/13   spa     Use new dispatcher if debug flag is enabled else use old one
03/11/13   spa     Removed refs to unused UMID CDMA_FW_PA_PARAMS_CONFIG_RSP 
03/01/13   Saul    CDMA. FEATURE_RF_XPT_CDMA_TEMP -> FEATURE_RF_HAS_XPT_SUPPORT
02/21/13   spa     Register/deregister for pilot stop stream message from FW
02/19/13   spa     Register/deregister for pilot meas config response from FW 
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/18/13   aro     Deleted nikel-XPT deprecated FW responses
01/14/13   Saul    CDMA. 1x EPT/ET call support.
12/18/12   cd      XPT config response handling
11/30/12   Saul    MSGR. fw_response_dereg APIs extract client from data structs.
11/16/12   Saul    Sample Capture. Use common response.
09/19/12   Saul    CDMA Sample Capture. Featurization update.
08/31/12   Saul    CDMA. Added support for EPT_PROCESSING_DONE FW message
07/23/12   Saul    COMMON. Renamed MSGR_MODULE_RF_CDMA to MSGR_MODULE_RF_COMMON
07/09/12   aro     Moved EPT responses to EPT module
07/06/12   aro     Uncommented EPT to use temporary feature
06/06/12   aro     Restored EPT Changes
06/06/12   aro     Sequence number implementation for FW messages
06/06/12   aro     Handling of IRAT_RX_START_RSP
06/06/12   aro     Temporarily reverted EPT Changes
06/05/12   anr     Initial EPT Support
05/09/12   aro     Updated RX_STOP, TX_START, TX_STOP to use Sem Pool for 
                   synchronization
05/09/12   aro     Updated RX_START to use Sem Pool for synchronization
05/08/12   aro     Initial support for semaphore POOL
05/08/12   aro     Renamed function to sem_create and senm_destroy
05/02/12   aro     Support to detect POST-BEFORE-WAIT
04/11/12   aro     OQPCH update to indicate 1xL1 if the timed sleep
                   passed in FW or not
04/04/12   aro     Fixed dispatcher deregistration
03/28/12   aro     Support to handle FW response message
03/22/12   aro     Removed extra F3 message
03/22/12   aro     Added data structure to hold FW responses data with
                   an accessor function
03/22/12   aro     Migrated 1x FW response handling to RF_FWRSP task
03/21/12   aro     Functions to perform registration and deregistration
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
#include "rf_1x_fw_response.h"
#include "rf_1x_mc.h"
#include "rf_cdma_intelliceiver.h"
#include "cdmafw_msg.h"
#include "msgr_umid.h"
#include "rfcommon_msg.h"
#include "rf_fwrsp_task.h"
#include "rfa_msgr.h"
#include "rf_cdma_data.h"
#include "rf_1x_ept.h"
#include "rfm_internal.h"
#include "rf_dispatch.h"

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
rf_1x_fw_response_dispatch
(
  rf_fwrsp_cmd_type* req_ptr
)
{

  msgr_umid_type rf_uimd; /* Received UMID */

  /* Validate Request Pointer */
  if( req_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_fw_response_dispatch: NULL Request" );
    return;
  } /* if( req_ptr == NULL ) */

  /* Get UMID */
  rf_uimd = req_ptr->msgr_hdr.id;

  /* Check if the dispatched message is valid for 1x */
  if( MSGR_TECH_MODULE_VAL(rf_uimd) != MSGR_CDMA_FW )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_fw_response_dispatch: Cannot handle UMID 0x%x",
              rf_uimd );
    return;
  }

  /* Dispatch based on the UMID */
  switch( rf_uimd )
  {
  /* Use new dispatch for these UMIDS*/
  case CDMA_FW_RX_START_RSP:
  case CDMA_FW_RX_STOP_RSP:
  case CDMA_FW_TX_START_RSP:
  case CDMA_FW_TX_STOP_RSP:
  case CDMA_FW_STATE_CFG_RSP:
  case CDMA_FW_INTELLICEIVER_UPDATE_RSP:
  case CDMA_FW_IRAT_RX_START_RSP:
  case CDMA_FW_PILOT_MEAS_CFG_RSP:
  case CDMA_FW_PILOT_MEAS_STOP_STREAM_RSP:
  case CDMA_FW_XPT_CONFIG_RSP:
  case CDMA_FW_XPT_TRANS_COMPL_RSP: 
     rf_dispatch_command( rf_fwrsp_task_get_dispatch_handle(),
                         req_ptr );
    break;

   default:
      RF_MSG_1( RF_ERROR, "rf_1x_fw_response_dispatch: Invalid UMID 0x%x",
                rf_uimd );
      break;
  } /* switch( req_ptr->msgr_hdr.id ) */

  return;

} /* rf_1x_fw_response_dispatch */

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
rf_1x_fw_response_registration
(
  void
)
{
  rf_fwrsp_dispatch_status_type dis_res; /* cmd_dispatch result */
  boolean init_status = TRUE;
  const rf_fwrsp_task_msgr_type *fwrsp_msgr = NULL;
  rf_1x_fw_response_data_type *fwrsp_data = NULL;

  /* Get Pointer to FWRSP MSGR param */
  fwrsp_msgr = rf_fwrsp_get_msgr_param();

  /* Get FW Response Data */
  fwrsp_data = rf_cdma_get_1x_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_msgr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_fw_response_registration: NULL MSGR Data" );
    return FALSE;
  } /* if ( fwrsp_msgr == NULL ) */

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_fw_response_registration: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /* Register handler for this function */
  dis_res = rf_fwrsp_dispatch_register( MSGR_TECH_CDMA, MSGR_MODULE_RF_MAX,
                                        rf_1x_fw_response_dispatch );

  if ( dis_res != RF_FWRSP_CMD_REG_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_1x_fw_response_registration: Failed CDMA Tech "
                      "registration"  );    
    init_status = FALSE;
  }

  return init_status;

}
/* rf_1x_fw_response_registration */

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
rf_1x_fw_response_deregistration
(
   void
)
{
  boolean deinit_status = TRUE;
  const rf_fwrsp_task_msgr_type *fwrsp_msgr = NULL;
  rf_1x_fw_response_data_type *fwrsp_data = NULL;

  /* Get Pointer to FWRSP MSGR param */
  fwrsp_msgr = rf_fwrsp_get_msgr_param();

  /* NULL Pointer Check */
  if ( fwrsp_msgr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_fw_response_deregistration: NULL MSGR Data" );
    return FALSE;
  } /* if ( fwrsp_msgr == NULL ) */

  /* Get FW Response Data */
  fwrsp_data = rf_cdma_get_1x_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_fw_response_deregistration: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /*------------------------------------------------------------------------*/
  /* RF Task de-registration */
  /*------------------------------------------------------------------------*/

  rf_fwrsp_dispatch_deregister( MSGR_TECH_CDMA, MSGR_MODULE_RF_MAX );

  /* Return Status */
  return deinit_status;

} /* rf_1x_fw_response_deregistration */
#endif /* FEATURE_CDMA1X */
/*! @} */


