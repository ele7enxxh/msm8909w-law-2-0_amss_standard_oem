/*! 
  @file
  rfcommon_fw_response.c
 
  @brief
  This file includes functions pertaining to handling of FW responses RF COMMON
 
  @addtogroup RF_COMMON_FW_RESPONSE
  @{
*/
 
/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_fw_response.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/21/15   vv      Added support for f/w response on ADC VCM calibration  
03/31/14   cri     Remove deprecated TxOverride commands
03/20/14   cri     Remove deprecated DPD MSGR commands
02/25/14   hdz     Changed AsDiv MSGR to RFLM
02/12/14   zhw     Tuner AOL/CL RFLM MSGR update
07/19/13   aro     Featurized ASDIV
07/17/13   aro     Added proper featurization to disable ASDiv on Triton
07/15/13   aro     Compilation fix (temp)
07/13/13   aro/pl  Added handling of fw response for ASDiv
06/24/13   JJ      Added msgr_client check 
06/10/13   ndb     Added support for tuner closed loop meas response via rf_dispatch
04/12/13   Saul    [Common] Use RF dispatch for fw responses.
03/21/13   zhw     Added support for tuner update response via rf_dispatch
03/11/13   sar     Updated file for APQ, GNSS only target.
11/30/12   Saul    MSGR. fw_response_dereg APIs extract client from data structs.
11/28/12   Saul    Sample Capture. Temp features to FEATURE_RF_HAS_XPT_SUPPORT
11/16/12   Saul    Sample Capture. Use latest FW interface.
11/16/12   Saul    Sample Capture. Use common response.
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
08/28/12   rsr     Added support for TXAGC override.
08/14/12   Saul    COMMON. Registering msgr messages by default.
08/03/12   Saul    OFF-Target. Added test for XPT sample capture.
08/02/12   Saul    COMMON. Added support to /free DPD tables response.
08/02/12   Saul    COMMON. Added support to allocate/write/free DPD tables.
07/26/12   Saul    COMMON. Skip registering "fake" MSGR commands.
07/23/12   Saul    COMMON. Renamed MSGR_MODULE_RF_CDMA to MSGR_MODULE_RF_COMMON
07/23/12   Saul    RF COMMON. MDSP/MSGR framework. Initial Revision.

==============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "comdef.h"
#include "rfcommon_data.h"
#include "rfcmd_msg.h"
#include "rfcommon_msg.h"
#include "rf_fwrsp_task.h"
#include "rfa_msgr.h"
#include "rfcommon_mdsp_data.h"
#include "rfcommon_mdsp.h"
#include "rfcommon_mc.h"
#include "rf_dispatch.h"
#include "rflm_cmn_msg.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch function for RF COMMON commands dispatched from RF_TASK
 
  @details
  This function is called by RF_TASK when it receives messages/commands for
  RF COMMON
  
  @param req_ptr
  Pointer to RF Task command
*/
static void
rfcommon_fw_response_dispatch
(
  rf_fwrsp_cmd_type* req_ptr
)
{
  msgr_umid_type rf_uimd; /* Received UMID */

  /* Validate Request Pointer */
  if( req_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_fw_response_dispatch: NULL Request" );
    return;
  } /* if( req_ptr == NULL ) */

  /* Get UMID */
  rf_uimd = req_ptr->msgr_hdr.id;

  /* Check if the dispatched message is valid for RF COMMON */
  if( MSGR_TECH_MODULE_VAL(rf_uimd) != MSGR_RFA_FW &&
      MSGR_TECH_MODULE_VAL(rf_uimd) != MSGR_RFLM_CMN )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_fw_response_dispatch: Cannot handle UMID 0x%x",
              rf_uimd );
    return;
  }

  /* check if the new RF dispatch implementation is to be used */
  if ( rfcommon_debug_flags.use_new_fwrsp == TRUE )
  {
    /* Dispatch based on the UMID */
    switch( rf_uimd )
    {
      case RFLM_CMN_TUNER_CL_RSP:
      case RFLM_CMN_TUNER_AOL_RSP:
      case RFA_FW_ADC_VCM_CAL_DONE_RSP:
      #ifdef FEATURE_RF_ASDIV
      #ifndef FEATURE_TRITON_MODEM
      case RFLM_CMN_ASD_RSP:
      #endif /* FEATURE_TRITON_MODEM */
      #endif /* FEATURE_RF_ASDIV */
       rf_dispatch_command( rf_fwrsp_task_get_dispatch_handle(), req_ptr);
       break;
      case RFA_FW_SAMPLE_CAPT_DONE_RSP:
       rfcommon_mc_handle_capture_done_response( req_ptr );
       break;

      case RFA_FW_XPT_CAL_PROC_DONE_RSP:
       rfcommon_mc_handle_processing_done_response( req_ptr );
       break;
      default:
        RF_MSG_3( RF_ERROR, "rfcommon_fw_response_dispatch: Unsupported UMID 0x%x [ %d | %d ]",
                  rf_uimd, 
                  RFCOMMON_MDSP_SNUM_TECH(rf_uimd),
                  RFCOMMON_MDSP_SNUM_COUNT(rf_uimd) );
        break;
    } /* switch( req_ptr->msgr_hdr.id ) */
  } /* if ( rfcommon_debug_flags.use_new_fwrsp == TRUE ) */
  else
  {
    /* Dispatch based on the UMID */
    switch( rf_uimd )
    {
      case RFA_FW_SAMPLE_CAPT_DONE_RSP:
       rfcommon_mc_handle_capture_done_response( req_ptr );
       break;

      case RFA_FW_XPT_CAL_PROC_DONE_RSP:
       rfcommon_mc_handle_processing_done_response( req_ptr );
       break;
      case RFLM_CMN_TUNER_CL_RSP:
      case RFLM_CMN_TUNER_AOL_RSP:
      case RFA_FW_ADC_VCM_CAL_DONE_RSP:
      case RFLM_CMN_ASD_RSP:
       rf_dispatch_command( rf_fwrsp_task_get_dispatch_handle(), req_ptr);
       break;

      default:
        RF_MSG_3( RF_ERROR, "rfcommon_fw_response_dispatch: Unsupported UMID 0x%x [ %d | %d ]",
                  rf_uimd, 
                  RFCOMMON_MDSP_SNUM_TECH(rf_uimd),
                  RFCOMMON_MDSP_SNUM_COUNT(rf_uimd) );
        break;
    } /* switch( req_ptr->msgr_hdr.id ) */
  } /* if ( rfcommon_debug_flags.use_new_fwrsp != TRUE ) */

  return;

} /* rfcommon_fw_response_dispatch */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize dispatcher for common commands processed through RF Task

  @details
  This function will register all common commands to RF Task dispatcher and
  also registers with message router, if required.
 
  @param msgr_client
  Message router client ID for registration
 
  @param mailbox
  Message router ID for registration

  @return
  TRUE on success and FALSE on failure
*/
boolean
rfcommon_fw_response_registration
(
  void
)
{
  rf_fwrsp_dispatch_status_type dis_res; /* cmd_dispatch result */
  boolean init_status = TRUE;
  errno_enum_type msgr_res = E_SUCCESS; /* holds registration result */  
  const rf_fwrsp_task_msgr_type *fwrsp_msgr = NULL;
  rfcommon_fw_response_data_type *fwrsp_data = NULL;
  msgr_client_t *msgr_client = NULL;
  msgr_id_t mailbox;

  /* Get Pointer to FWRSP MSGR param */
  fwrsp_msgr = rf_fwrsp_get_msgr_param();

  /* NULL Pointer Check */
  if ( fwrsp_msgr == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_fw_response_registration: NULL FWRSP" );
    return FALSE;
  } /* if ( fwrsp_msgr == NULL ) */

  /* Get FW Response Data */
  fwrsp_data = rfcommon_get_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_fw_response_registration: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  msgr_client = (msgr_client_t*)(&(fwrsp_msgr->client));
  mailbox = fwrsp_msgr->mb;

  if(msgr_client == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_fw_response_registration: NULL MSGR client."
            " Cannot proceed to register dispatcher and messages."
            " Common functionality/messages may fail." );
    return FALSE;
  }

  /* Register handler for this function */
  dis_res = rf_fwrsp_dispatch_register( MSGR_TECH_RFA, 
                                        MSGR_MODULE_RF_MAX,
                                        rfcommon_fw_response_dispatch );

  if ( dis_res != RF_FWRSP_CMD_REG_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rfcommon_fw_response_registration: Failed RF Common "
                      "registration"  );    
    init_status = FALSE;
  }

  /* Register handler for this function */
  dis_res = rf_fwrsp_dispatch_register( MSGR_TECH_RFLM, 
                                        MSGR_MODULE_RF_MAX,
                                        rfcommon_fw_response_dispatch );

  if ( dis_res != RF_FWRSP_CMD_REG_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rfcommon_fw_response_registration: Failed RFLM Common "
                      "registration"  );    
    init_status = FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /* Register Response Messages */
  /*--------------------------------------------------------------------------*/

  /* RFA_FW_CAPTURE_DONE_RSP response */
  msgr_res = msgr_register( MSGR_RFA_RF_COMMON, 
                            msgr_client, 
                            mailbox,
                            RFA_FW_SAMPLE_CAPT_DONE_RSP );

  if ( msgr_res != E_SUCCESS )
  {
    RF_MSG_3( RF_ERROR, "rfcommon_fw_response_registration: Failed MSGR "
              "registration with RF COMMON for 0x%x [ %d | %d ]", 
              RFA_FW_SAMPLE_CAPT_DONE_RSP, 
              RFCOMMON_MDSP_SNUM_TECH(RFA_FW_SAMPLE_CAPT_DONE_RSP),
              RFCOMMON_MDSP_SNUM_COUNT(RFA_FW_SAMPLE_CAPT_DONE_RSP) );    
    init_status = FALSE;
  }

  /* RFA_FW_XPT_CAL_PROC_DONE_RSP response */
  msgr_res = msgr_register( MSGR_RFA_RF_COMMON, 
                            msgr_client, 
                            mailbox,
                            RFA_FW_XPT_CAL_PROC_DONE_RSP );

  if ( msgr_res != E_SUCCESS )
  {
    RF_MSG_3( RF_ERROR, "rfcommon_fw_response_registration: Failed MSGR "
              "registration with RF COMMON for 0x%x [ %d | %d ]", 
              RFA_FW_XPT_CAL_PROC_DONE_RSP, 
              RFCOMMON_MDSP_SNUM_TECH(RFA_FW_XPT_CAL_PROC_DONE_RSP),
              RFCOMMON_MDSP_SNUM_COUNT(RFA_FW_XPT_CAL_PROC_DONE_RSP) );    
    init_status = FALSE;
  }

  return init_status;

} /* rfcommon_fw_response_registration */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize dispatcher for RF COMMON commands processed through RF Task

  @details
  This function will unregister message router commands, release
  allocated semphores and unregister RF task.
 
  @param msgr_client
  Message router client ID for unregistration
 
  @return
  TRUE on success and FALSE on failure
*/
boolean
rfcommon_fw_response_deregistration
(
  void
)
{
  boolean deinit_status = TRUE;
  errno_enum_type msgr_res = E_SUCCESS; /* holds registration result */
  const rf_fwrsp_task_msgr_type *fwrsp_msgr = NULL;
  rfcommon_fw_response_data_type *fwrsp_data = NULL;
  msgr_client_t *msgr_client = NULL;

  /* Get Pointer to FWRSP MSGR param */
  fwrsp_msgr = rf_fwrsp_get_msgr_param();

  /* NULL Pointer Check */
  if ( fwrsp_msgr == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_fw_response_deregistration: NULL FWRSP" );
    return FALSE;
  } /* if ( fwrsp_msgr == NULL ) */

  /* Get FW Response Data */
  fwrsp_data = rfcommon_get_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_fw_response_deregistration: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  msgr_client = (msgr_client_t*)(&(fwrsp_msgr->client));

  /*--------------------------------------------------------------------------*/
  /* De-register Responses */
  /*--------------------------------------------------------------------------*/

  
  /* De-register RFA_FW_CAPTURE_DONE_RSP response */
  msgr_res = msgr_deregister( MSGR_RFA_RF_COMMON, 
                              msgr_client, 
                              RFA_FW_SAMPLE_CAPT_DONE_RSP );

  if ( msgr_res != E_SUCCESS )
  {
    RF_MSG_3( RF_ERROR, "rfcommon_fw_response_deregistration: Failed MSGR "
              "de-registration with RF COMMON for 0x%x [ %d | %d ]", 
              RFA_FW_SAMPLE_CAPT_DONE_RSP,
              RFCOMMON_MDSP_SNUM_TECH(RFA_FW_SAMPLE_CAPT_DONE_RSP),
              RFCOMMON_MDSP_SNUM_COUNT(RFA_FW_SAMPLE_CAPT_DONE_RSP) );    
    deinit_status = FALSE;
  }

  /* De-register RFA_FW_XPT_CAL_PROC_DONE_RSP response */
  msgr_res = msgr_deregister( MSGR_RFA_RF_COMMON, 
                              msgr_client, 
                              RFA_FW_XPT_CAL_PROC_DONE_RSP );

  if ( msgr_res != E_SUCCESS )
  {
    RF_MSG_3( RF_ERROR, "rfcommon_fw_response_deregistration: Failed MSGR "
              "de-registration with RF COMMON for 0x%x [ %d | %d ]", 
              RFA_FW_XPT_CAL_PROC_DONE_RSP,
              RFCOMMON_MDSP_SNUM_TECH(RFA_FW_XPT_CAL_PROC_DONE_RSP),
              RFCOMMON_MDSP_SNUM_COUNT(RFA_FW_XPT_CAL_PROC_DONE_RSP) );    
    deinit_status = FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /* RF Task de-registration */
  /*--------------------------------------------------------------------------*/

  rf_fwrsp_dispatch_deregister( MSGR_TECH_RFA, MSGR_MODULE_RF_MAX );

  /* Return Status */
  return deinit_status;

} /* rfcommon_fw_response_deregistration */

/*! @} */

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
