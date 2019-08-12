/*! 
  @file
  rf_hdr_ept.c
 
  @brief
  This file defines the HDR interfaces to be used for Envelope Power Tracking. 
 
  @addtogroup RF_CDMA_HDR_EPT
  @{
*/

/*==============================================================================

  Copyright (c) 2012 - 2013  Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/src/rf_hdr_ept.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/23/14   cd      Update ET delay interface to RFLM, to avoid computation in 
                   critical path
02/14/14   cd      Remove sem wait for xPT config message to HDR FW because 
                   xPT config is no longer done through FW on Bolt
01/13/14   cd      Add ET delay programming to RFLM for online mode
12/26/13   sar/hdz Remove et_txlm_hanlde from Bolt c2k code, and use one single 
                   TxLm handle allocation.
08/09/13   APU     Fixed KW error.
08/01/13   Saul    XPT. ET delay vs temp FTM char support
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
07/02/13   Saul    XPT. ET delay compensation
06/03/13   spa     Migrate to new RF dispatch for XPT FW response mssgs 
05/13/13   Saul    XPT. Support EPT_ET_CFG.
04/26/13   aro     F3 message update to match TxMgr log parser
03/01/13   Saul    CDMA. FEATURE_RF_XPT_CDMA_TEMP -> FEATURE_RF_HAS_XPT_SUPPORT
02/27/13   aro     Compiler warning fix
02/27/13   aro     Mainlined debug flag for Tx resource manager
02/27/13   Saul    CDMA. ET now uses separate envelope scale from EPT.
02/25/13   aro     Updated XPT configure interface to pass XPT mode from
                   calling function
02/22/13   Saul    Tx ET Path Delay NV support.
02/21/13   aro     Updated configure XPT interface to push band and NV chain
                   parameter for calling function
02/21/13   aro     CDMA Tx Resource manager implementation
02/14/13   Saul    CDMA. Use DPD CFG NV to enable XPT.
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/21/13   Saul    CDMA. Mainline XPT items.
01/16/13   Saul    CDMA. Enabled EPT/ET tempcomp.
01/15/13   Saul    CDMA. HDR EPT/ET call support.
12/19/12   cd      Fixes for ET Tx configuration in FTM mode only
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
09/10/12   Saul    Warning fixes.
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
07/19/12   cri     Initial revision

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_hdr_ept.h"
#include "rf_cdma_data.h"
#include "rfc_cdma.h"
#include "rf_cdma_nv.h"
#include "rf_hdr_mdsp.h"
#include "rfm_hdr_ftm.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_intelliceiver.h"
#include "rfcommon_msg.h"
#include "rfm_internal.h"
#include "rf_cdma_ept.h"
#include "modem_mem.h"
#include "rfcommon_mdsp.h"
#include "rf_cdma_mc.h"
#include "rflm_c2k_mc.h"

/*============================================================================*/
/*!
  @name EPT Online

  @brief
  This section contains functions for EPT calibration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configuration for XPT

  @details
  XPT configuration. Message to indicate the XPT mode of operation.

  @param device
  Primary device for which XPT is to be configured
 
  @param prim_txlm_handle
  Primary Tx LM handle.
 
  @param et_txlm_handle
  ET Tx LM handle.
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
rf_hdr_xpt_configure
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  rf_cdma_nv_path_type tx_nv_path,
  lm_handle_type prim_txlm_handle,
  xpt_cfg_type xpt_mode
)
{
  boolean ret_val = TRUE;
  rf_cdma_mdsp_xpt_config_type xpt_config_data;
  const rf_hdr_config_type *rf_hdr_config_ptr;   /* Pointer to hdr RF NV data */
  const rf_cdma_tx_static_type *tx_cdma_config_ptr;
  /* callback params for FW rsp*/ 
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param; 
  const rf_cdma_data_status_type* dev_status_r ;
  const rfc_cdma_logical_device_params_type *logical_device_ptr = NULL;

  /* ensure that NV is valid for given device and band */
  rf_hdr_config_ptr = rf_cdma_nv_get_hdr_data( tx_nv_path, band );

  /* NULL Pointer Check */
  if ( NULL == rf_hdr_config_ptr )
  {
    RF_MSG_1( RF_ERROR, "XPT rf_hdr_xpt_configure: Dev[ %d ] Failed NV",
              device ); 
    return FALSE;
  } /* if ( NULL == rf_hdr_config_ptr ) */

  /* Aditional NULL pointer checks */
  if ( NULL == rf_hdr_config_ptr->cdma_common_ptr )
  {
    RF_MSG_1( RF_ERROR, "XPT rf_hdr_xpt_configure: Dev[ %d ] CDMA Common NV", 
              device ); 
    return FALSE;
  } /* if ( NULL == rf_hdr_config_ptr->cdma_common_ptr ) */

  /* Get Pointer to Tx Static table */
  tx_cdma_config_ptr = &(rf_hdr_config_ptr->cdma_common_ptr->tx_static);
  
  /* Aditional NULL pointer checks */
  if ( NULL == tx_cdma_config_ptr )
  {
    RF_MSG_1( RF_ERROR, "XPT rf_hdr_xpt_configure: Dev[ %d ] CDMA Tx NV",
              device ); 
    return FALSE;
  } /* if ( NULL == tx_cdma_config_ptr ) */

  /* Get Logical Device Param */
  logical_device_ptr = rfc_cdma_get_logical_device_params( device );
  if ( (logical_device_ptr == NULL) ||
       (logical_device_ptr->tx_device[band] == NULL) )
  {
    RF_MSG_1( RF_ERROR, "XPT rf_hdr_xpt_configure: Null CDMA logical Tx device.", 
              device);
    return FALSE;
  }

  /* Get Device Status */
  dev_status_r = rf_cdma_get_device_status( device ) ;
  if ( NULL == dev_status_r )
  {
    RF_MSG_1 ( RF_ERROR, "XPT rf_1x_xpt_configure: Null CDMA device_status .", 
              device ) ;
    return FALSE;
  }
  
  /* Based on XPT mode populate field */
  if ( ( xpt_mode == ET_CFG ) ||
       ( xpt_mode == EPT_ET_CFG ) )
  {
    xpt_config_data.ept_enable = TRUE;
    xpt_config_data.et_enable = TRUE;
  } /* if ( xpt_mode == ET_CFG ) */
  else if ( xpt_mode == EPT_CFG )
  {
    xpt_config_data.ept_enable = TRUE;
    xpt_config_data.et_enable = FALSE;
  } /* else if ( xpt_mode == EPT_CFG ) */
  else
  {
    RF_MSG_2( RF_ERROR, "XPT rf_hdr_xpt_configure: Dev[ %d ] CDMA Invalid"
                        "XPT Mode %d", device, xpt_mode ); 
    return FALSE;
  } /* if ! ( xpt_mode == ET_CFG ) */

  xpt_config_data.et_env_delay =
    rf_cdma_xpt_get_path_delay ( logical_device_ptr->tx_device[band],
                                 &(rf_hdr_config_ptr->cdma_common_ptr->tx_cal) ,
                                 band , chan,
                                 ( rfm_get_calibration_state() == FALSE ),
                                 dev_status_r->temp_comp.is_last_reading_valid,
                                 dev_status_r->temp_comp.last_temp_sensor_read ) ;
  xpt_config_data.ept_env_scale_nominal = 
    tx_cdma_config_ptr->ept_cfg_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
  xpt_config_data.et_env_scale_nominal = 
    tx_cdma_config_ptr->ept_cfg_params[ET_CFG_CMN_ENV_SCALE_UNITY];
  xpt_config_data.prim_txlm_handle = prim_txlm_handle;
  xpt_config_data.prim_tx_chain = 
    rfc_cdma_get_tx_modem_chain( device, band );

  /* XPT Config */
  ret_val &= rf_hdr_mdsp_configure_xpt( &xpt_config_data,
                                        &rsp_cb_param,
                                        NULL,
                                        NULL );

  /* Configure ET delay value to RFLM */
  ret_val &= rflm_hdr_mc_config_tx_et_delay( prim_txlm_handle,
                                             xpt_config_data.et_env_delay );

  RF_MSG_2( RF_HIGH, "XPT rf_hdr_xpt_configure: Dev[ %d ] [Status %d] "
                     "NV", device, ret_val );

  return ret_val;

} /* rf_hdr_xpt_configure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for xPT config done Reponse received through RF task
 
  @details
  This function is called when response to xPT config done message is
  sent to RF task. This function will basically post the semaphore so that 
  blocking call can be unblocked.
 
  @param req_ptr
  Pointer to RF Task command
*/
boolean
rf_hdr_xpt_config_done_rsp_cb
(
   void *cmd_ptr,
   const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_xpt_config_rsp_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_hdr_xpt_config_done_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_xpt_config_rsp_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_hdr_xpt_config_done_rsp_cb: NULL CB Data" );
    return FALSE;
  } /* if ( cb_data == NULL ) */

  RF_MSG_4( RF_MED, "[1X][FW->RF] [D%d] HDR_XPT_CONFIG RSP 0x%x [ %d | %d ]", 
            cb_data->device,
            snum_data->snum, 
            RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
            RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum) );

  RF_MSG( RF_LOW, "rf_hdr_xpt_config_done_rsp_cb: Handled" );

  return ret_val;

} /* rf_hdr_xpt_config_done_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for XPT Transition Complete Reponse received through RF task
 
  @param req_ptr
  Pointer to RF Task command
*/
boolean
rf_hdr_xpt_trans_complete_rsp_cb
(
   void *cmd_ptr,
   const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_xpt_trans_compl_rsp_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;

  cfw_xpt_trans_compl_payload_t *rsp_payload;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
   RF_MSG( RF_ERROR, "rf_1x_xpt_trans_complete_rsp_cb: NULL Argument(s)" );
   return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_xpt_trans_compl_rsp_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
   RF_MSG( RF_ERROR, "rf_hdr_xpt_trans_complete_rsp_cb: NULL CB Data" );
   return FALSE;
  } /* if ( cb_data == NULL ) */

  /* Typecast the payload */
  rsp_payload = (cfw_xpt_trans_compl_payload_t*)req_ptr->payload;

  RF_MSG_5( RF_MED, "[HDR][FW->RF] [D%d] XPT_TRANSITION_RSP  0x%x [ %d | %d ]"
           " XPT Mode %d"  ,
           cb_data->device,
           snum_data->snum, 
           RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
           RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum),
           rsp_payload->cur_mode );


  /* Post the Semaphore only if an active semaphore is available */
  if ( cb_data->sema_token != NULL )
  {
   rfcommon_semaphore_post( cb_data->sema_token );
  } /* if ( fwrsp_data->xpt_config_done_rsp.sema_token != NULL ) */
  else
  {
    ret_val = FALSE;
    ERR_FATAL( "rf_hdr_xpt_trans_complete_rsp_cb: NULL Sem Token", 0, 0, 0 );
  } /* if ! ( cb_data->sema_token != NULL ) */

  RF_MSG( RF_LOW, "rf_hdr_xpt_trans_complete_rsp_cb: Handled" );

  return ret_val;

} /* rf_hdr_xpt_trans_complete_rsp_cb */
#endif /* FEATURE_CDMA1X */
/*! @} */
