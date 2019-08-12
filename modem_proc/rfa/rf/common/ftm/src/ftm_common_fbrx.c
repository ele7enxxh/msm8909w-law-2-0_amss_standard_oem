/*!
  @file
  ftm_common_fbrx.c

  @brief
  This module contains interface and implementation of FTM common FBRx.
  
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_fbrx.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/20/14    ka     Defensive code to protect against invalid LM handle
11/10/14    ka     Return residual DC estimate during fbrx gain cal
10/08/14    ka     Replace rfc with device api for retrieving device pointer
09/30/14    aa     Fix compiler warning
09/08/14   jmf     [SelfTest] Implement Tx Power measurement using TxAGC loop
08/26/14    aa     Added support for FBRX QRCT logging
08/11/14    aa     Update run continuous cmd to control fbrx enable/disable
08/01/14    aa     Added support for rsb comp during calv3 opcode based fbrx setup
07/29/14    JJ     Set fbrx result flag to FALSE if flag returned by uk is 0
07/22/14    ka     Return next_rsb for cal
07/22/14    ka     Ensure correct handle for get mode function
06/12/14    ka     Add support to store samples from cal failures
05/26/14    ka     Return results from single trigger cmd
05/07/14    aa     Fix KW error
04/10/14    ka     KW fix
04/07/14    ka     Update single trigger. Remove obsolete code.
03/27/14    ka     Store gain state used for cal
03/20/14    ka     Added support for storing captures during cal
02/27/14    ka     Do not disable WTR during cal
02/17/14    ka     Gain cal fix
02/13/14    ka     Added continuous run api, updated capture function
02/11/14    aa     Fix compiler warning
02/06/14    aa     Updated fbrx uK results
01/30/14    aa     Added API to get valid tx handle for fbrx
01/22/14    ka     Added fbrx set params
12/23/13    ka     Added API for calibration
11/25/13    ka     Cmd fixes, added store results
11/13/13    ka     Added FTM fbrx common test framework
10/30/13    ka     Added bridge config
10/11/13    ka     Added activate chain
09/23/13    ka     Initial version

==============================================================================*/

#include "comdef.h"
#include "ftm.h"
#include "ftm_task.h"
#include "rfcommon_fbrx_api.h"
#include "ftm_common_fbrx.h"
#include "ftm_msg.h"
#include "ftmdiag.h"
#ifdef FEATURE_RF_FBRX
#include "rflm_api_fbrx.h"
#include "rflm_dm_api.h"
#endif
#include "rfc_card.h"
#include "rfdevice_cmn_intf.h"
#include "rflm_api_fbrx_fw_hal.h"
#include "stringl.h" /* for memscpy */
#include "fs_estdio.h"
#include "ftm_common_locks.h"
#include "rf_test_task_v.h"
#include "rfcommon_core_utils.h"

#define RFCOMMON_FBRX_NUM_DATA_BUF 6
#define RFCOMMON_FBRX_DATA_BUF_SIZE 391
#define RFCOMMON_FBRX_MAX_FILE_LENGTH 21
#define RFCOMMON_FBRX_CAL_DEBUG_THRESH 0x2000
#define RFCOMMON_FBRX_MAX_TXPWRMEAS_TIME_P1USEC 300

static lm_handle_type ftm_common_fbrx_test_handle = 0;
static boolean ftm_common_fbrx_test_handle_allocated = FALSE;


typedef struct
{
  uint32 ftm_common_fbrx_rx_data[RFCOMMON_FBRX_DATA_BUF_SIZE];
  uint32 ftm_common_fbrx_tx_data[RFCOMMON_FBRX_DATA_BUF_SIZE];
} ftm_common_fbrx_data_buf_type;

static uint8 ftm_common_fbrx_buf_index = 0;

static ftm_common_fbrx_data_buf_type ftm_common_fbrx_data_bufs[RFCOMMON_FBRX_NUM_DATA_BUF];

static ftm_common_fbrx_log_results_data_type  *ftm_common_fbrx_log_results_data = NULL;

/*============================================================================*/
/*!
  @addtogroup FTM_COMMON_FBRX
 
  @brief
  This module includes the FTM functionality for FBRx.
 
  @{
*/


/*============================================================================*/
/*!
  @name FBRx FTM Test

  @brief
  This section contains FTM test functions for common FBRx.
 
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_enter_mode
 
  @details
  This function tests the common FBRx enter mode API
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_enter_mode(ftm_common_pkt_type *cmd_ptr)
{
  ftm_fbrx_handle_response_type* response_ptr = NULL;
  ftm_fbrx_enter_mode_param_type* param_ptr = NULL;
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_handle_response_type));

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_handle_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_enter_mode_param_type*)&(cmd_ptr->ftm_extended_params);

  if (!ftm_common_fbrx_test_handle_allocated)
  {
    if (param_ptr != NULL)
    {
      response_ptr->status = rfcommon_fbrx_mc_enter_mode(&ftm_common_fbrx_test_handle, 
                                                                           param_ptr->tech);
      response_ptr->handle_id = ftm_common_fbrx_test_handle;
      ftm_common_fbrx_test_handle_allocated = TRUE;

      FTM_MSG_2( FTM_HIGH, "ftm_common_fbrx_enter_mode: result %d, handle %d", 
                 response_ptr->status, 
                 ftm_common_fbrx_test_handle );
    }
  }
  else
  {
    response_ptr->status = FALSE;
    FTM_MSG_3( FTM_ERROR, "ftm_common_fbrx_enter_mode - handle already allocated!", 0, 0, 0);
  }

  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_init_tx
 
  @details
  This function tests the common FBRx init tx API
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_init_tx(ftm_common_pkt_type *cmd_ptr)
{
  ftm_fbrx_handle_response_type* response_ptr = NULL;
  ftm_fbrx_init_tx_param_type* param_ptr = NULL;
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_handle_response_type));
  uint32 tx_handle = ftm_common_fbrx_get_valid_tx_handle();
  rflm_fbrx_dm_template_t* fbrx_data_ptr;

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_handle_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_init_tx_param_type*)&(cmd_ptr->ftm_extended_params);

  if (ftm_common_fbrx_test_handle_allocated)
  {
    if (param_ptr != NULL)
    {
      response_ptr->status = rfcommon_fbrx_mc_init_tx( ftm_common_fbrx_test_handle, param_ptr->cfg, tx_handle);
                                                       
      /* update the fbrx_mode in DM for all valid tx handle in ftm mode */
      for (tx_handle = 0; tx_handle < RFLM_HANDLE_COUNTS ; tx_handle++)
      {
        fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
        if (fbrx_data_ptr != NULL)
        {
          (void)rfcommon_fbrx_mc_update_mode( tx_handle, param_ptr->cfg);
        }
      }

      response_ptr->handle_id = ftm_common_fbrx_test_handle;

      FTM_MSG_3( FTM_HIGH, "ftm_common_fbrx_init_tx: result %d, handle %d, cfg %d", 
                 response_ptr->status, 
                 ftm_common_fbrx_test_handle,
                 param_ptr->cfg );
    }
  }
  else
  {
    response_ptr->status = FALSE;
    FTM_MSG_3( FTM_ERROR, "ftm_common_fbrx_init_tx - handle not allocated!", 0, 0, 0);
  }


  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_enable
 
  @details
  This function tests the common FBRx enable API
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_enable(ftm_common_pkt_type *cmd_ptr)
{
  ftm_fbrx_handle_response_type* response_ptr = NULL;
  ftm_fbrx_enable_param_type* param_ptr = NULL;
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_handle_response_type));

  param_ptr = param_ptr;

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_handle_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_enable_param_type*)&(cmd_ptr->ftm_extended_params);

  if (ftm_common_fbrx_test_handle_allocated)
  {

    response_ptr->status = rfcommon_fbrx_enable(ftm_common_fbrx_test_handle);
    response_ptr->handle_id = ftm_common_fbrx_test_handle;

    FTM_MSG_2( FTM_HIGH, "ftm_common_fbrx_enable: result %d, handle %d", 
               response_ptr->status, 
               ftm_common_fbrx_test_handle);

  }
  else
  {
    response_ptr->status = FALSE;
    FTM_MSG_3( FTM_ERROR, "ftm_common_fbrx_enable - handle not allocated!", 0, 0, 0);
  }


  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_exit
 
  @details
  This function tests the common FBRx exit mode API
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_exit(ftm_common_pkt_type *cmd_ptr)
{
  ftm_fbrx_handle_response_type* response_ptr = NULL;
  ftm_fbrx_exit_param_type* param_ptr = NULL;
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_handle_response_type));

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_handle_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_exit_param_type*)&(cmd_ptr->ftm_extended_params);

  if (ftm_common_fbrx_test_handle_allocated)
  {
    if (param_ptr != NULL)
    {
      response_ptr->status = rfcommon_fbrx_mc_exit(ftm_common_fbrx_test_handle, param_ptr->tech);
      response_ptr->handle_id = ftm_common_fbrx_test_handle;
      ftm_common_fbrx_test_handle_allocated = FALSE;

      FTM_MSG_2( FTM_HIGH, "ftm_common_fbrx_exit: result %d, handle %d", 
                 response_ptr->status, 
                 ftm_common_fbrx_test_handle );
    }
  }
  else
  {
    response_ptr->status = FALSE;
    FTM_MSG_3( FTM_ERROR, "ftm_common_fbrx_exit - handle not allocated!", 0, 0, 0);
  }

  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_activate_chain
 
  @details
  This function tests the common FBRx activate chain API
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_activate_chain(ftm_common_pkt_type *cmd_ptr)
{
  ftm_fbrx_handle_response_type* response_ptr = NULL;
  ftm_fbrx_activate_chain_param_type* param_ptr = NULL;
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_handle_response_type));

  param_ptr = param_ptr;

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_handle_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_activate_chain_param_type*)&(cmd_ptr->ftm_extended_params);

  if (ftm_common_fbrx_test_handle_allocated)
  {
    #ifdef FEATURE_RF_FBRX
    rflm_fbrx_activate_chain(ftm_common_fbrx_test_handle);
    #endif
    response_ptr->status = TRUE; 
    response_ptr->handle_id = ftm_common_fbrx_test_handle;

    FTM_MSG_2( FTM_HIGH, "ftm_common_fbrx_activate_chain: result %d, handle %d", 
               response_ptr->status, 
               ftm_common_fbrx_test_handle);

  }
  else
  {
    response_ptr->status = FALSE;
    FTM_MSG_3( FTM_ERROR, "ftm_common_fbrx_activate_chain - handle not allocated!", 0, 0, 0);
  }


  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_bridge_config
 
  @details
  This function tests the common FBRx bridge config API
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_bridge_config(ftm_common_pkt_type *cmd_ptr)
{
  ftm_fbrx_handle_response_type* response_ptr = NULL;
  ftm_fbrx_bridge_config_param_type* param_ptr = NULL;
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_handle_response_type));

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_handle_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_bridge_config_param_type*)&(cmd_ptr->ftm_extended_params);


  if (param_ptr != NULL)
  {
    #ifdef FEATURE_RF_FBRX
    rflm_fbrx_bridge_config(param_ptr->enable);
    #endif
    response_ptr->status = TRUE;

    FTM_MSG_2( FTM_HIGH, "ftm_common_fbrx_bridge_config: result %d, enable %d", 
               response_ptr->status, 
               param_ptr->enable);
  }
  else
  {
    response_ptr->status = FALSE;
    FTM_MSG_3( FTM_ERROR, "ftm_common_fbrx_bridge_config - NULL param pointer!", 0, 0, 0);
  }


  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_single_trigger
 
  @details
  This function triggers a single capture and processing run of the FBRx uK
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_single_trigger(ftm_common_pkt_type *cmd_ptr)
{
  ftm_fbrx_single_trigger_response_type* response_ptr = NULL;
  ftm_fbrx_single_trigger_param_type* param_ptr = NULL;
  ftm_common_fbrx_capture_input_type input;
  ftm_common_fbrx_capture_result_type output;
  uint8 i;
  rfdevice_rxtx_common_class* dev_ptr = NULL; 
  rflm_fbrx_dm_template_t* fbrx_data_ptr;
  uint32 tx_handle = 0;
  rfcom_band_type_u band_union;
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_single_trigger_response_type));

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_single_trigger_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_single_trigger_param_type*)&(cmd_ptr->ftm_extended_params);


  response_ptr->status = FALSE;

  for (tx_handle=0; tx_handle < RFLM_HANDLE_COUNTS; tx_handle++ )
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
    if (fbrx_data_ptr != NULL )
    {
      if (dev_ptr == NULL)
      {
        band_union = rfcommon_fbrx_mc_convert_uint8_to_rfcom_band((rfm_mode_enum_type)fbrx_data_ptr->fbrx_ctl.rfm_mode, 
                                                                  fbrx_data_ptr->fbrx_ctl.fbrx_band);
        dev_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, 
                                                     (rfm_mode_enum_type)fbrx_data_ptr->fbrx_ctl.rfm_mode, 
                                                     band_union);
        if (dev_ptr != NULL)
        {
          input.tech = fbrx_data_ptr->fbrx_ctl.rfm_mode;
          input.band = rfcommon_fbrx_mc_convert_uint8_to_rfcom_band(input.tech, fbrx_data_ptr->fbrx_ctl.fbrx_band);
        }
      }
    }
  }

  if (param_ptr != NULL)
  {
    #ifdef FEATURE_RF_FBRX
    input.fbrx_gain_state = param_ptr->fbrx_gain_state;
    input.scale_rx = param_ptr->scale_rx;
    input.tx_device_id = param_ptr->tx_device;
    input.ubias = param_ptr->ubias;
    input.store_capture_data = FALSE;
    output = ftm_common_fbrx_capture_and_process(input);
    response_ptr->status = (uint8)output.status;
    response_ptr->gain = output.gain;
    response_ptr->ls_err = output.ls_err;
    for (i=0;i<RFLM_FBRX_XCORR_LOG_SIZE;i++)
    {
      response_ptr->xcorr_log[i] = output.xcorr_log[i];
    }
    #endif /* FEATURE_RF_FBRX */
  }

    FTM_MSG_1( FTM_HIGH, "ftm_common_fbrx_single_trigger: result %d", 
               response_ptr->status);

  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_run_continuous
 
  @details
  This function triggers FBRx to run in normal operation mode
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_run_control(ftm_common_pkt_type *cmd_ptr)
{
  uint32 handle = 0;
  ftm_fbrx_handle_response_type* response_ptr = NULL;
  ftm_fbrx_run_control_param_type* param_ptr = NULL;
  #ifdef FEATURE_RF_FBRX
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;
  #endif
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_handle_response_type));

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_handle_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_run_control_param_type*)&(cmd_ptr->ftm_extended_params);


  response_ptr->status = FALSE;

  if (param_ptr != NULL)
  {
    #ifdef FEATURE_RF_FBRX
    for (handle=0;handle<RFLM_HANDLE_COUNTS;handle++)
    {
      fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);
      if (fbrx_data_ptr != NULL)
      {
        if (param_ptr->enable)
        {
          /* Set override values for continuous operation */
          fbrx_data_ptr->override_ctl.enable = TRUE;
          fbrx_data_ptr->override_ctl.use_count = FALSE;
          fbrx_data_ptr->override_ctl.override_mode = FALSE;

        }
        else
        {
          /* Set override values to stop operation */
          fbrx_data_ptr->override_ctl.enable = FALSE;
          fbrx_data_ptr->override_ctl.use_count = FALSE;
          fbrx_data_ptr->override_ctl.override_mode = TRUE;

        }

        response_ptr->status = TRUE;
        response_ptr->handle_id = handle;

        FTM_MSG_2( FTM_HIGH, "ftm_common_fbrx_run_control: result %d, handle %d", 
                              response_ptr->status, 
                              handle);
      }
    }
    #endif /* FEATURE_RF_FBRX */
  }

  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_enable_rf
 
  @details
  This function tests the common FBRx enable RF dev API
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_enable_rf(ftm_common_pkt_type *cmd_ptr)
{
  ftm_fbrx_handle_response_type* response_ptr = NULL;
  ftm_fbrx_enable_rf_param_type* param_ptr = NULL;
  rfdevice_rxtx_common_class* dev_ptr = NULL; 
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_handle_response_type));
  rflm_fbrx_dm_template_t* fbrx_data_ptr;
  uint32 tx_handle = 0;
  rfcom_band_type_u band_union;

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_handle_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_enable_rf_param_type*)&(cmd_ptr->ftm_extended_params);

  for (tx_handle=0; tx_handle < RFLM_HANDLE_COUNTS; tx_handle++ )
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
    if (fbrx_data_ptr != NULL )
    {
      if (dev_ptr == NULL)
      {
        band_union = rfcommon_fbrx_mc_convert_uint8_to_rfcom_band((rfm_mode_enum_type)fbrx_data_ptr->fbrx_ctl.rfm_mode, 
                                                                  fbrx_data_ptr->fbrx_ctl.fbrx_band);
        dev_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, 
                                                     (rfm_mode_enum_type)fbrx_data_ptr->fbrx_ctl.rfm_mode, 
                                                     band_union);
      }
    }
  }

  if (param_ptr != NULL && dev_ptr != NULL)
  {
    if (param_ptr->enable)
    {
      response_ptr->status = rfcommon_fbrx_mc_enable_rf_dev(dev_ptr, param_ptr->gain_state);
    }
    else
    {
      response_ptr->status = rfcommon_fbrx_mc_disable_rf_dev(dev_ptr);
    }
    response_ptr->handle_id = ftm_common_fbrx_test_handle;

    FTM_MSG_4( FTM_HIGH, "ftm_common_fbrx_enable_rf: result %d, handle %d, enable %d, gain state %d", 
               response_ptr->status, 
               ftm_common_fbrx_test_handle,
               param_ptr->enable,
               param_ptr->gain_state );
  }
  else
  {
    response_ptr->status = FALSE;
    FTM_MSG_3( FTM_ERROR, "ftm_common_fbrx_enable_rf - NULL param pointer!", 0, 0, 0);
  }


  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_store_results
 
  @details
  This function writes the FBRx capture results to EFS
 
*/
ftm_rsp_pkt_type ftm_common_fbrx_store_results(ftm_common_pkt_type *cmd_ptr)
{
  ftm_fbrx_handle_response_type* response_ptr = NULL;
  ftm_fbrx_store_results_param_type* param_ptr = NULL;
  char iq_file_name[RFCOMMON_FBRX_MAX_FILE_LENGTH];
  EFS_EFILE *iq_fp;
  fs_size_t write_result;
  int file_close_res;
  uint8 i;
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_fbrx_handle_response_type));

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_fbrx_handle_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_fbrx_store_results_param_type*)&(cmd_ptr->ftm_extended_params);

  response_ptr->status = TRUE;


  if (param_ptr != NULL)
  {
    if (param_ptr->num_buffers == 0)
    {
    response_ptr->status = rfcommon_fbrx_msm_store_data(param_ptr->num_tx,
                                                        param_ptr->num_rx);
    }
    else
    {
      if (param_ptr->num_buffers > RFCOMMON_FBRX_NUM_DATA_BUF)
      {
        param_ptr->num_buffers = RFCOMMON_FBRX_NUM_DATA_BUF;
      }
      if (param_ptr->num_rx > RFCOMMON_FBRX_DATA_BUF_SIZE)
      {
        param_ptr->num_rx = RFCOMMON_FBRX_DATA_BUF_SIZE;
      }
      if (param_ptr->num_tx > RFCOMMON_FBRX_DATA_BUF_SIZE)
      {
        param_ptr->num_tx = RFCOMMON_FBRX_DATA_BUF_SIZE;
      }
      for (i=0;i<param_ptr->num_buffers;i++)
      {
        snprintf(&iq_file_name[0], (RFCOMMON_FBRX_MAX_FILE_LENGTH+1), "fbrx_buf%05d_tx.bin", i);
        /* Open file for "write" */
        iq_fp = efs_efopen( &iq_file_name[0], "w" );

        if( iq_fp == NULL )
        {
          response_ptr->status = FALSE;
          FTM_MSG_1( FTM_ERROR, "ftm_common_fbrx_store_results: cannot open tx file %d", i);
        }
        /* Write tx samples */
        if( response_ptr->status == TRUE )
        {
          /* Save samples as they appear in lmem. */
          write_result = efs_efwrite( (void*)&(ftm_common_fbrx_data_bufs[i].ftm_common_fbrx_tx_data[0]), 
                                      sizeof(uint32),
                                      param_ptr->num_tx, iq_fp );
          if( write_result == 0 )
          {
            response_ptr->status = FALSE;
          }
        }

        if( iq_fp != NULL )
        {
          file_close_res = efs_efclose( iq_fp );
          if( file_close_res != 0 )
          {
            response_ptr->status = FALSE;
          }
        }
        snprintf(&iq_file_name[0],(RFCOMMON_FBRX_MAX_FILE_LENGTH+1),"fbrx_buf%05d_rx.bin", i);
        /* Open file for "write" */
        iq_fp = efs_efopen( &iq_file_name[0], "w" );

        if( iq_fp == NULL )
        {
          response_ptr->status = FALSE;
          FTM_MSG_1( FTM_ERROR, "ftm_common_fbrx_store_results: cannot open rx file %d", i);
        }
        /* Write tx samples */
        if( response_ptr->status == TRUE )
        {
          /* Save samples as they appear in data buffer. */
          write_result = efs_efwrite( (void*)&(ftm_common_fbrx_data_bufs[i].ftm_common_fbrx_rx_data[0]),  
                                      sizeof(uint32),
                                      param_ptr->num_rx, iq_fp );
          if( write_result == 0 )
          {
            response_ptr->status = FALSE;
          }
        }

        if( iq_fp != NULL )
        {
          file_close_res = efs_efclose( iq_fp );
          if( file_close_res != 0 )
          {
            response_ptr->status = FALSE;
          }
        }
      } /* end for loop */
      ftm_common_fbrx_buf_index = 0;
    }

    FTM_MSG_1( FTM_HIGH, "ftm_common_fbrx_store_results: result %d", 
               response_ptr->status);
  }


  return rsp;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_capture_and_process
 
  @details
  Triggers a single capture and processing of FBRx samples. Intended for use
  during calibration.
 
  @param ftm_common_fbrx_capture_input_type.tx_device_id
           set to 0 for WTR0, 1 for WTR1, etc.
 
  @param ftm_common_fbrx_capture_input_type.xpt_mode - APT/EPT/ET
 
  @param ftm_common_fbrx_capture_input_type.gain_comp
           Scaling factor for processing. Depends on RB allocation for LTE.
           Constant for other techs. Use -32768 for now.
 
  @param ftm_common_fbrx_capture_input_type.fbrx_gain_state
           Set to desired FBRx gain state.
 
*/
ftm_common_fbrx_capture_result_type ftm_common_fbrx_capture_and_process
(
  ftm_common_fbrx_capture_input_type input
)
{
  uint32 handle;
  #ifdef FEATURE_RF_FBRX
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL, *valid_data_ptr=NULL;
  rflm_handle_rx_t fbrx_lm_handle = -1;
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  #endif
  ftm_common_fbrx_capture_result_type result = {0,0,0,0,0,0,0,{0}};
  rfdevice_rxtx_common_class* dev_ptr; 
  boolean dev_status = TRUE;
  uint8 i;
  boolean done = FALSE;
  uint8 counter = 0;
  int32 real_err, imag_err, complex_err;
  rfm_mode_enum_type rfm_tech;
  rfcom_band_type_u rfcom_band;
  int32 real_a, real_b, imag_a, imag_b;
  double mag_sq_b_over_a = 0;
  int32 dc_estimate = 0;


  result.status = RFCOMMON_FBRX_ERROR;

  dev_ptr = rfcommon_core_get_tx_device_ptr((rfm_device_enum_type)input.tx_device_id, input.tech, input.band);

  if (input.fbrx_gain_state > RFLM_FBRX_NUM_GAIN_STATES-1)
  {
    input.fbrx_gain_state = RFLM_FBRX_NUM_GAIN_STATES-1;
  }

  dev_status &= rfcommon_fbrx_mc_enable_rf_dev(dev_ptr, input.fbrx_gain_state);

  #ifdef FEATURE_RF_FBRX
  for (handle=0;handle<RFLM_HANDLE_COUNTS;handle++)
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);

    if (fbrx_data_ptr != NULL)
    {  
      rfm_tech = fbrx_data_ptr->fbrx_ctl.rfm_mode;
      rfcom_band = rfcommon_fbrx_mc_convert_uint8_to_rfcom_band(rfm_tech, fbrx_data_ptr->fbrx_ctl.fbrx_band );

      /* Get FBRx rxlm handle */
      fbrx_lm_handle = rfcommon_fbrx_mc_get_tech_handle( rfm_tech );

      if (fbrx_lm_handle != 0xFFFFFFFF)
      {
        /* Update the IQMC RSB coefficients to FBRX DM */
        api_status = rfcommon_fbrx_mc_update_rsb_coeff( handle, 
                                                        fbrx_lm_handle, 
                                                        rfm_tech,
                                                        rfcom_band );
      }
      else
      {
        api_status = RFCOMMON_FBRX_ERROR;
      }

      if (api_status == RFCOMMON_FBRX_ERROR)
      {
        FTM_MSG( FTM_ERROR, "ftm_common_fbrx_capture_and_process: rfcommon_fbrx_mc_update_rsb_coeff() failed");
      }
      else
      {
        api_status  = rfcommon_fbrx_msm_write_iqmc_to_hw( fbrx_lm_handle, 
                                                          fbrx_data_ptr->rsb_data[input.fbrx_gain_state].iqmc );
        if (api_status == RFCOMMON_FBRX_ERROR)
        {
          FTM_MSG( FTM_ERROR, "ftm_common_fbrx_capture_and_process: rfcommon_fbrx_msm_write_iqmc_to_hw() failed");
        }
      }

      for (i=0;i<RFLM_FBRX_LTE_MAX_RUNS_PER_SF;i++)
      {
        fbrx_data_ptr->fbrx_ctl.uk_results_valid_flag[i] = FALSE;
      }
      /* Set override values for single trigger mode */
      fbrx_data_ptr->override_ctl.counter = 1;
      fbrx_data_ptr->override_ctl.enable = TRUE;
      fbrx_data_ptr->override_ctl.use_count = TRUE;
      fbrx_data_ptr->override_ctl.override_mode = TRUE;
      fbrx_data_ptr->override_ctl.scale_rx = input.scale_rx;
      fbrx_data_ptr->override_ctl.ubias = input.ubias;
      fbrx_data_ptr->override_ctl.gain_state = input.fbrx_gain_state;
    }
  }
  #endif /* FEATURE_RF_FBRX */

  while (!done && counter < 50)
  {
    DALSYS_BusyWait(100);
    for (handle=0;handle<RFLM_HANDLE_COUNTS;handle++)
    {
      fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);
      if (fbrx_data_ptr != NULL)
      {
        if (fbrx_data_ptr->fbrx_ctl.uk_results_valid_flag[0])
        {
          done = TRUE;          
        }
      }
    }
    counter++;
  }

  if (dev_status==TRUE && done==TRUE)
  {
    result.status = RFCOMMON_FBRX_SUCCESS;
  }
  else
  {
    result.status = RFCOMMON_FBRX_ERROR;
    FTM_MSG( FTM_ERROR, "ftm_common_fbrx_capture_and_process: Invalid FBRx capture.");
  }

  #ifdef FEATURE_RF_FBRX
  for (handle=0;handle<RFLM_HANDLE_COUNTS;handle++)
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);
    if (fbrx_data_ptr != NULL)
    {
      if (fbrx_data_ptr->fbrx_ctl.uk_results_valid_flag[0])
    {
       result.gain = fbrx_data_ptr->results_log.results[0].gain_est;
       result.gain_imbalance = fbrx_data_ptr->results_log.results[0].gain_imbalance;
       real_a = (int32)(fbrx_data_ptr->results_log.results[0].model_a);
       real_a >>= 16;
       imag_a = (int32)(fbrx_data_ptr->results_log.results[0].model_a);
       imag_a = (imag_a << 16) >> 16;
       real_b = (int32)(fbrx_data_ptr->results_log.results[0].model_b);
       real_b >>= 16;
       imag_b = (int32)(fbrx_data_ptr->results_log.results[0].model_b);
       imag_b = (imag_b << 16) >> 16;
       mag_sq_b_over_a = ((double)(real_b*real_b + imag_b*imag_b))/((double)(real_a*real_a + imag_a*imag_a));
       dc_estimate = (int32)((double)100.0*log10(mag_sq_b_over_a));
       result.loft = dc_estimate;
       result.phase_imbalance = fbrx_data_ptr->results_log.results[0].phase_imbalance;
       result.rsb = fbrx_data_ptr->results_log.results[0].next_rsb;
       result.ls_err = fbrx_data_ptr->results_log.results[0].ls_err;
       for (i=0;i<RFLM_FBRX_XCORR_LOG_SIZE;i++)
       {
         result.xcorr_log[i] = fbrx_data_ptr->results_log.results[0].xcorr_log[i];
       }
       valid_data_ptr = fbrx_data_ptr;
      }
    }
  }

  complex_err = (int32)(result.ls_err);
  real_err = complex_err >> 16;
  imag_err = (complex_err << 16) >> 16;

  if ( valid_data_ptr != NULL &&
      ((ftm_common_fbrx_buf_index == 0) ||
      ((abs(real_err) > RFCOMMON_FBRX_CAL_DEBUG_THRESH || abs(imag_err) > RFCOMMON_FBRX_CAL_DEBUG_THRESH)
          && (ftm_common_fbrx_buf_index < RFCOMMON_FBRX_NUM_DATA_BUF))))
  {
    FTM_MSG_1( FTM_HIGH, "ftm_common_fbrx_capture_and_process: Saving debug capture %d", ftm_common_fbrx_buf_index );
    FTM_MSG_1( FTM_HIGH, "FBRx gain estimate %d", result.gain );
    FTM_MSG_1( FTM_HIGH, "FBRx RSB 0x%08X", result.rsb );
    for (i=0;i<RFLM_FBRX_XCORR_LOG_SIZE;i++)
    {
      FTM_MSG_2( FTM_HIGH, "FBRx xcorr[%d] = %d", i, result.xcorr_log[i]);
    }

    if (valid_data_ptr->fbrx_ctl.fbrx_mode == 1)
    {
      (void)rfcommon_fbrx_msm_store_data_internal(&(ftm_common_fbrx_data_bufs[ftm_common_fbrx_buf_index].ftm_common_fbrx_tx_data[0]),
                                                  &(ftm_common_fbrx_data_bufs[ftm_common_fbrx_buf_index].ftm_common_fbrx_rx_data[0]),
                                                  RFLM_FBRX_TX_CAP_LEN_MODE1, 
                                                  RFLM_FBRX_RX_CAP_LEN_MODE1);
    }
    else
    {
      (void)rfcommon_fbrx_msm_store_data_internal(&(ftm_common_fbrx_data_bufs[ftm_common_fbrx_buf_index].ftm_common_fbrx_tx_data[0]),
                                                  &(ftm_common_fbrx_data_bufs[ftm_common_fbrx_buf_index].ftm_common_fbrx_rx_data[0]),
                                                  RFLM_FBRX_TX_CAP_LEN_MODE3, 
                                                  RFLM_FBRX_RX_CAP_LEN_MODE3);
    }
    ftm_common_fbrx_buf_index++;
  }
  #endif /* FEATURE_RF_FBRX */

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_lm_test
 
  @details
  This function tests LM config for FBRx
 
*/
void ftm_common_fbrx_lm_test( void )
{
  lm_handle_type handle;
  boolean fbrx_result=FALSE;
  rflm_fbrx_dm_template_t* fbrx_data_ptr;
  uint32 tx_handle = ftm_common_fbrx_get_valid_tx_handle();

  fbrx_result = rfcommon_fbrx_mc_enter_mode(&handle, RFCOM_WCDMA_MODE);
  FTM_MSG_2( FTM_HIGH, "rfcommon_fbrx_mc_enter_mode: result %d, handle %d", fbrx_result, handle );
  fbrx_result = rfcommon_fbrx_mc_init_tx(handle, RFCOMMON_FBRX_UMTS_SC, tx_handle);
  FTM_MSG_2( FTM_HIGH, "rfcommon_fbrx_mc_init_tx: result %d, handle %d", fbrx_result, handle );

  /* update the fbrx_mode in DM for all valid tx handle in ftm mode */
  for (tx_handle = 0; tx_handle < RFLM_HANDLE_COUNTS ; tx_handle++)
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
    if (fbrx_data_ptr != NULL)
    {
      fbrx_result = rfcommon_fbrx_mc_update_mode( tx_handle, RFCOMMON_FBRX_UMTS_SC);
    }
  }

  fbrx_result = rfcommon_fbrx_enable(handle);
  FTM_MSG_2( FTM_HIGH, "rfcommon_fbrx_enable: result %d, handle %d", fbrx_result, handle );
  #ifdef FEATURE_RF_FBRX
  rflm_fbrx_bridge_config(TRUE);
  FTM_MSG_1( FTM_HIGH, "rflm_fbrx_bridge_config on: handle %d", handle );
  rflm_fbrx_activate_chain(handle);
  FTM_MSG_1( FTM_HIGH, "rflm_fbrx_activate_chain: handle %d", handle );
  #endif /* FEATURE_RF_FBRX */
  fbrx_result = rfcommon_fbrx_mc_exit(handle, RFCOM_WCDMA_MODE);
  FTM_MSG_2( FTM_HIGH, "rfcommon_fbrx_mc_exit: result %d, handle %d", fbrx_result, handle );
} /* ftm_common_fbrx_lm_test */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_get_valid_tx_handle
 
  @details
  This function returns a valid Tx handle for FBRX
 
*/

uint32 ftm_common_fbrx_get_valid_tx_handle(void)
{
  rflm_fbrx_dm_template_t* fbrx_data_ptr;
  uint32 tx_handle = 0;
  uint8 i = 0; 

  for (tx_handle=0; tx_handle < RFLM_HANDLE_COUNTS; tx_handle++ )
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
    if (fbrx_data_ptr != NULL )
    {
      for (i = 0; i < 3; i++)
      {
        if (fbrx_data_ptr->fbrx_ctl.uk_results_valid_flag[i] == TRUE)
        {
          FTM_MSG_1( FTM_HIGH, "ftm_common_fbrx_get_valid_tx_handle: tx handle = %d", tx_handle );
          return tx_handle;
        }
      }
    }
  }
  return tx_handle;
} /* ftm_common_fbrx_get_valid_tx_handle */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_set_cal_state
 
  @details
  This function sets the ftm cal mode for FBRX
 
*/
void ftm_common_fbrx_set_cal_state( boolean ftm_cal_mode )
{
  rflm_fbrx_dm_template_t* fbrx_data_ptr;
  uint32 tx_handle;

  for (tx_handle=0; tx_handle < RFLM_HANDLE_COUNTS; tx_handle++ )
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
    if (fbrx_data_ptr != NULL)
    {
      fbrx_data_ptr->fbrx_ctl.ftm_cal_mode = ftm_cal_mode;
    }
  }
} /* ftm_common_fbrx_set_cal_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_get_mode
 
  @details
  This function returns the currently used FBRx mode [0-3]
 
*/
uint8 ftm_common_fbrx_get_mode( void )
{
  rflm_fbrx_dm_template_t* fbrx_data_ptr;
  uint32 tx_handle;
  uint8 fbrx_mode=3;

  tx_handle = ftm_common_fbrx_get_valid_tx_handle();

  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);

  if (fbrx_data_ptr != NULL)
  {      
    fbrx_mode = fbrx_data_ptr->fbrx_ctl.fbrx_mode;
  }

  return fbrx_mode;
}

/*----------------------------------------------------------------------------*/

void ftm_common_fbrx_log_results_init_func( void)
{
  ftm_log_config_table_entry  *log_config_ptr = NULL;
  log_config_ptr = ftm_log_find_log_id( FTM_LOG_FBRX_RESULTS );

  if( log_config_ptr == NULL )
  {
    FTM_MSG_ERROR("NULL pointer obtained for log id %d ",FTM_LOG_FBRX_RESULTS,0,0);
    return;
  }

  ftm_common_enter_critical_section();

  /* must allocate an internal buffer to collect the data */
  ftm_common_fbrx_log_results_data =
  (ftm_common_fbrx_log_results_data_type *)ftm_malloc( log_config_ptr->log_config.num_buffer
                                             * sizeof (ftm_common_fbrx_log_results_data_type)); /*lint !e737 suppress int to unsigned int */

  /* check that we have memory to work with */
  if(ftm_common_fbrx_log_results_data == NULL)
  {
    FTM_MSG_ERROR("ftm_common_fbrx_log_results_init_func: Unable to allocate FBRX results data buffer",0,0,0);
  }

  ftm_common_leave_critical_section();

}
/*----------------------------------------------------------------------------*/

void ftm_common_fbrx_log_results_exit_func( void)
{
  ftm_common_enter_critical_section();
  if(ftm_common_fbrx_log_results_data != NULL)
  {
    ftm_free((void*)ftm_common_fbrx_log_results_data); /* free the internal buffer */
  }
  ftm_common_leave_critical_section();
}
/*----------------------------------------------------------------------------*/

void ftm_common_fbrx_log_results_cb(int4 interval)
{
  static int buf     = 0;
  ftm_common_fbrx_log_results_t  *fbrx_log  = NULL;
  ftm_log_config_table_entry     *log_config_ptr;

  rflm_fbrx_dm_template_t* fbrx_ptr = NULL;
  uint32 tx_handle = 0, ls_err = 0, real_lse = 0, img_lse = 0;
  uint8 buf_idx = 0;
  uint8 run_idx = 0;

  log_config_ptr = ftm_log_find_log_id( FTM_LOG_FBRX_RESULTS);
  if(log_config_ptr == NULL)
  {
   FTM_MSG_ERROR("NULL pointer obtained for log id %d ",FTM_LOG_FBRX_RESULTS,0,0);
   return;
  }

  ftm_common_enter_critical_section();

  for (tx_handle=0; tx_handle < RFLM_HANDLE_COUNTS; tx_handle++ )
  {
    fbrx_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
    if (fbrx_ptr != NULL)
    {
      buf_idx = fbrx_ptr->exp_gain_data.read_buf_idx;

      ftm_common_fbrx_log_results_data[buf].version = 1; //14

      ftm_common_fbrx_log_results_data[buf].txagc = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].last_tx_agc;   

      ftm_common_fbrx_log_results_data[buf].pa = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].pa_state_idx;

      ftm_common_fbrx_log_results_data[buf].gs = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].fbrx_gain_state;

      ftm_common_fbrx_log_results_data[buf].gn_est = 
          fbrx_ptr->results_log.results[run_idx].gain_est;
        
      ftm_common_fbrx_log_results_data[buf].gn_err = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].gain_error_dB10;  

      /* Calculate the mag of LS error from real/imag part */
      ls_err = fbrx_ptr->results_log.results[run_idx].ls_err;
      real_lse = (ls_err & 0xFFFF0000) >> 16;
      img_lse  = (ls_err & 0x0000FFFF);
      ftm_common_fbrx_log_results_data[buf].mag_lse = FTM_COMMON_FBRX_RET_MAG_LSE(real_lse, img_lse);
      
      ftm_common_fbrx_log_results_data[buf].cal_pwr = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].cal_pwr;

      ftm_common_fbrx_log_results_data[buf].cal_gn = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].last_cal_gain;

      ftm_common_fbrx_log_results_data[buf].temp_comp = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].last_temp_comp;

      ftm_common_fbrx_log_results_data[buf].ubias = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].ubias_new;

      ftm_common_fbrx_log_results_data[buf].rxscale = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].rxscale_new;

      ftm_common_fbrx_log_results_data[buf].txscale = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].txscale_new;

      ftm_common_fbrx_log_results_data[buf].tech = fbrx_ptr->fbrx_ctl.tech;

      ftm_common_fbrx_log_results_data[buf].band = fbrx_ptr->fbrx_ctl.fbrx_band;

      ftm_common_fbrx_log_results_data[buf].bandwidth = fbrx_ptr->fbrx_ctl.bandwidth;

      ftm_common_fbrx_log_results_data[buf].fbrx_mode = fbrx_ptr->fbrx_ctl.fbrx_mode;

      ftm_common_fbrx_log_results_data[buf].slot = 
          fbrx_ptr->exp_gain_data.sf_data[buf_idx].slot_data[run_idx].valid_slot_idx;

      ftm_common_fbrx_log_results_data[buf].nv = fbrx_ptr->fbrx_ctl.fbrx_nv_ctrl_status;      
    }
  }

  if(++buf >= log_config_ptr->log_config.num_buffer)
  {
   fbrx_log = (ftm_common_fbrx_log_results_t *)
               ftm_log_malloc( (uint16)FTM_LOG_FBRX_RESULTS,
                                sizeof (ftm_common_fbrx_log_results_t)+ ( log_config_ptr->log_config.num_buffer - 1 )
                                * sizeof(ftm_common_fbrx_log_results_data_type) ); /*lint !e737 suppres int to unsigned int*/
   if(fbrx_log != NULL)
   {
    /* have to copy the internal buffer to the suite_log packet, we cannot
    simply accumulate the data in the suite_log packet directly since any memory
    allocated with ftm_log_malloc must be passed to log_commit soon afterward
    (where "soon" is apparently less than tens of ms) */
    memscpy( ( void*) &( fbrx_log->data),
             sizeof(ftm_common_fbrx_log_results_data_type),
             ( void*) ftm_common_fbrx_log_results_data,
             log_config_ptr->log_config.num_buffer * sizeof( ftm_common_fbrx_log_results_data_type)); /*lint !e737 suppres int to unsigned int*/

    /* send log */
    log_commit( ( ftm_log_type*) fbrx_log );
   }

   buf = 0;
  }

  ftm_common_leave_critical_section();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_meas_ant_pwr
 
  @details
  This function returns the average antenna power estimate for Self Test
 
  @params
  Number of averages, Pointers to populate estimated power values in dB10 and dB100.
  current FBRx Gain Stage in which measurement was made
 
  @return
  Returns TRUE on success, else FALSE
 
*/
boolean ftm_common_fbrx_meas_ant_pwr( 
   int16 * pwr_est_db10, 
   int16 * pwr_est_db100, 
   uint8 num_averages, 
   uint8 * fbrx_gs 
)
{
  boolean done = FALSE, api_status = TRUE;
  uint16 counter = 0, run_idx = 0, read_buf_idx = 0;
  uint32 handle;
  rflm_fbrx_dm_template_t* fbrx_data_ptr;
  double curr_gain_est = 0.0, cal_gain = 0.0, curr_ubias = 0.0, cal_rxscale = 0.0, curr_rxscale = 0.0, cal_ubias = 0.0;
  double pwr_offset_dbl = 0.0, cal_pwr_dbl = 0.0, pwr_est_dbl = 0.0, pwr_est_db10_dbl = 0.0, pwr_est_db10_dbl_avg = 0.0;
  int16 gain_error_db10 = 0, last_tx_agc_db10 = 0, temp_comp_db10 = 0;
  int32 pwr_est_db1000 = 0;
  uint8 avg_iter = 0;
  uint8 iter = 0;
  boolean temp_tx_lin_update_flag[RFLM_HANDLE_COUNTS] = {FALSE};
  uint8 temp_tx_lin_flag_restore_handle_idx[RFLM_HANDLE_COUNTS] = {0xFF};
  uint8 num_tx_lin_flag_restore = 0;

  for (handle = 0; handle < RFLM_HANDLE_COUNTS; handle++)
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);
    if (fbrx_data_ptr != NULL)
    {
      temp_tx_lin_flag_restore_handle_idx[num_tx_lin_flag_restore] = handle;
      temp_tx_lin_update_flag[num_tx_lin_flag_restore] = fbrx_data_ptr->fbrx_ctl.txlin_update_flag;
      num_tx_lin_flag_restore++;
      fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
    }
  }


  FTM_MSG(FTM_ERROR, "Entered ftm_common_fbrx_meas_ant_pwr" );
  for (avg_iter = 0; avg_iter < num_averages; avg_iter++)
  {
    done = FALSE;
    counter = 0;
    for (handle = 0; handle < RFLM_HANDLE_COUNTS; handle++)
    {
      fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);
      if (fbrx_data_ptr != NULL)
      {
        for (run_idx = 0; run_idx < RFLM_FBRX_LTE_MAX_RUNS_PER_SF; run_idx++)
        {
          // Turn all valid handles' return flag to false, because we want to trigger fresh reading
          fbrx_data_ptr->fbrx_ctl.uk_results_valid_flag[run_idx] = FALSE; 
        }
      }
    }

    // Measurement should come automatically from Tx AGC loop, if it is running and the return flag
    // will be updated, wait until the flag is TRUE, max wait time is 30000 us or 30 ms
    while (!done && counter < RFCOMMON_FBRX_MAX_TXPWRMEAS_TIME_P1USEC)
    {
      DALSYS_BusyWait(100);
      for (handle = 0; handle < RFLM_HANDLE_COUNTS; handle++)
      {
        fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);
        if (fbrx_data_ptr != NULL)
        {
          // only check 0-th run data, because currently that has valid meas only
          if (fbrx_data_ptr->fbrx_ctl.uk_results_valid_flag[0]) 
          {
            done = TRUE;
            break; // break handle loop
          }
        }
      }
      if (done == TRUE)
      {
        break; // break counter loop
      }
      counter++;
    }

    if (done == FALSE)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_fbrx_meas_ant_pwr: Measurement did not complete in allotted %d ms or no valid fbrx handle", 
                            RFCOMMON_FBRX_MAX_TXPWRMEAS_TIME_P1USEC/10);
      api_status =  FALSE;
    }

    if (api_status)
    {
      FTM_MSG_3(FTM_HIGH, "ftm_common_fbrx_meas_ant_pwr: FBRx Handle %d has %d valid results with fbrx_gs %d", 
                handle, fbrx_data_ptr->results_log.num_valid_results,
                fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].fbrx_gain_state);
      * fbrx_gs  = fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].fbrx_gain_state;

      // Total RFLM_FBRX_MAX_BUFFER_SIZE read buffers are populated in circular fashion, need to read last valid one
      if (fbrx_data_ptr->exp_gain_data.read_buf_idx == 0)
      {
        read_buf_idx = RFLM_FBRX_MAX_BUFFER_SIZE - 1;
      }
      else
      {
        read_buf_idx = fbrx_data_ptr->exp_gain_data.read_buf_idx - 1;
      }

      run_idx = 0; // only 0-th run per SF was checked earlier and should have valid value

      gain_error_db10 = fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].gain_error_dB10;
      last_tx_agc_db10 = fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].last_tx_agc;

      // power estimate in dB10 from TxAGC update loop, 0.1dB resolution
      pwr_est_db10_dbl += (double)last_tx_agc_db10 - (double)gain_error_db10;

      FTM_MSG_3(FTM_HIGH, "ftm_common_fbrx_meas_ant_pwr: last_tx_agc_db10 (%d) - gain_error_db10 (%d) = pwr_est_db10 (%d) ",
                last_tx_agc_db10,
                gain_error_db10,
                last_tx_agc_db10 - gain_error_db10);


      // calculate power estimate in dB100 as well, 0.01dB resolution, using double-precision math
      cal_pwr_dbl = (double)(fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].cal_pwr);
      temp_comp_db10 = fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].last_temp_comp;
      cal_rxscale = (double)(fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].cal_rxscale);
      cal_ubias = (double)(fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].cal_ubias);
      cal_gain = (double)(fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].last_cal_gain);

      FTM_MSG_5(FTM_HIGH, "cal_pwr_db10: %d, temp_comp_db10: %d, cal_rx_scale: %d, cal_ubias: %d, cal_gain:%d",
                fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].cal_pwr,
                fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].last_temp_comp,
                fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].cal_rxscale,
                fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].cal_ubias,
                fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].last_cal_gain);

      curr_rxscale = (double)(fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].rxscale_new);
      curr_ubias = (double)(fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].ubias_new);
      curr_gain_est = (double)(fbrx_data_ptr->results_log.results[run_idx].gain_est);
      //curr_gain_est = (double)(fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].current_gain);

      FTM_MSG_3(FTM_HIGH, "curr_rxscale: %d, curr_ubias: %d, curr_gain_est: %d", 
                fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].rxscale_new,
                fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].ubias_new,
                fbrx_data_ptr->results_log.results[run_idx].gain_est
                //fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[run_idx].current_gain
                );

      // need to subraction temp_comp_db10 val
      pwr_offset_dbl = 20*log10((curr_gain_est*cal_rxscale*pow(2.0,cal_ubias))/(cal_gain*curr_rxscale*pow(2.0,curr_ubias))); 

      pwr_est_dbl = ((double)cal_pwr_dbl/10) + pwr_offset_dbl;

      pwr_est_db1000 += (int32)(pwr_est_dbl*1000) - (int32)(temp_comp_db10*100);

    }
  }

  if (api_status)
  {
    if ( pwr_est_db1000 < 0)
    {
      *pwr_est_db100 = (int16)((pwr_est_db1000-5)/(10*num_averages));
    }
    else
    {
      *pwr_est_db100 = (int16)((pwr_est_db1000+5)/(10*num_averages));
    }

    FTM_MSG_3(FTM_HIGH, "round ( pwr_est_accum_db1000: %d / 10*num_averages: %d ) = pwr_est_db100: %d", 
              pwr_est_db1000,
              num_averages,
              *pwr_est_db100);

    pwr_est_db10_dbl_avg = pwr_est_db10_dbl*10/num_averages;

    if ( ( ((int16)pwr_est_db10_dbl)% 5 == 0 ) && (((int16)pwr_est_db10_dbl) < 0))
    {
      * pwr_est_db10 = (int16)(((int16)pwr_est_db10_dbl_avg-5)/10);
    }
    else
    {
      * pwr_est_db10 = (int16)(((int16)pwr_est_db10_dbl_avg+5)/10);
    }

    FTM_MSG_3(FTM_HIGH, "round ( pwr_est_accum_db10: %d / num_averages: %d ) = pwr_est_db10: %d", 
              (int32)pwr_est_db10_dbl,
              num_averages,
              *pwr_est_db10);
  }

  for (iter = 0; iter < num_tx_lin_flag_restore; iter++)
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(temp_tx_lin_flag_restore_handle_idx[iter]);
    if (fbrx_data_ptr != NULL)
    {
      fbrx_data_ptr->fbrx_ctl.txlin_update_flag = temp_tx_lin_update_flag[iter];
    }
  }


  return api_status;
}

/*! @} */


/*! @} */


