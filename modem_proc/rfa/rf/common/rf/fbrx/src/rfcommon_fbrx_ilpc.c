
/*!
  @file
  rfcommon_fbrx_ilpc.c

  @brief
  This module is reponsible for Power meter based ilpc correction
*/

/*==================================================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/fbrx/src/rfcommon_fbrx_ilpc.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
09/17/14   gh      Update rfcommon_fbrx_ilpc_config_sample_capture
08/15/14   aro/gh  Update ilpc_config_sample_capture
10/08/14    ka     Replace rfc with device api for retrieving device pointer
07/31/14   aro     Added structures to hold ILPC Data
07/31/14   aro     Initial Revision

==================================================================================================*/

#include "comdef.h"
#include "rfcommon_fbrx_data.h"
#include "rfcommon_fbrx_ilpc.h"
#include "rfdevice_cmn_intf.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rfcommon_msm.h"
#include "rfcommon_data.h"
#include "rflm_api_fbrx.h"
#include "rf_hal_buffer.h"
#include "rfcommon_fbrx_ccs.h"
#include "rfcommon_core_utils.h"

/*------------------------------------------------------------------------------------------------*/
/*! Macro to convert the inverse of the passed number to Q16 format */
#define ILPC_INV_Q16(x) (((131072/(x)) + 1) >> 1)

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Initialize ILPC module
 
  @details
  This function will initialize the ILPC module. This will be done when ILPC setup request is done.
 
  @return
  Flag indication status of ILPC module inialization
*/
static boolean
rfcommon_fbrx_ilpc_init
(
  rfcommon_fbrx_data_type* fbrx_data
)
{
  boolean enable_flag = TRUE;

  /* Check if ILPC is overridden to disable mode */
  enable_flag &= (!fbrx_data->ilpc_data.override_disable_ilpc);

  /*  Check NV configuration */
  enable_flag &= TRUE;

  /* Update State */
  fbrx_data->ilpc_data.enable_ilpc = enable_flag;

  RF_MSG( RF_LOW, "[FBRX.ILPC] rfcommon_fbrx_ilpc_init: Initialized" );

  return TRUE;

} /* rfcommon_fbrx_ilpc_init */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Create ILPC events
 
  @details
  This function will create CCS event needed for ILPC operation.
 
  @param device
  RFM device for which ILPC events are to be created
 
  @param tx_handle
  TxLM handle for which ILPC events are to be created
 
  @param fbrx_handle
  FBRx handle for which ILPC events are to be created
 
  @param fbrx_data
  Pointer to data structure containing FBRX data
 
  @return
  Flag indication status of events creation
*/
static boolean 
rfcommon_fbrx_ilpc_create_events
( 
  rfm_device_enum_type device,
  rflm_handle_tx_t tx_handle,
  rflm_handle_rx_t fbrx_handle,
  rfcommon_fbrx_data_type* fbrx_data
)
{
  boolean api_status = TRUE;
  rfdevice_rxtx_common_class *dev_ptr = NULL; 
  rf_buffer_intf *pm_read_script = NULL; /* Buffer object to hold On/OFF scripts */
  rfcommon_mdsp_event_handle** pm_read_event_handle = NULL; /* Unique CCS Event handles */
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;
  rfcom_band_type_u band_union;

  /* Generic CCS event settings */
  const rfcommon_mdsp_event_settings_type event_settings_generic = 
    { 
      FALSE,     /*! Make_atomic */
      FALSE,     /*! Response_required */ 
      NULL,      /*! Callback_ptr */
      &rfcommon_msm_convert_microsecs_to_ustmr_units
    };

  /* Get Device Pointer */
  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
  if (fbrx_data_ptr != NULL)
  {
    band_union = rfcommon_fbrx_mc_convert_uint8_to_rfcom_band((rfm_mode_enum_type)fbrx_data_ptr->fbrx_ctl.rfm_mode, 
                                                              fbrx_data_ptr->fbrx_ctl.fbrx_band);
    dev_ptr = rfcommon_core_get_tx_device_ptr(device, 
                                                 (rfm_mode_enum_type)fbrx_data_ptr->fbrx_ctl.rfm_mode, 
                                                 band_union);
  }

  /* NULL pointer check */
  if (dev_ptr == NULL)
  {
    RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_create_events: Device %d, TxHandle %d, "
                        "FbHandle %d - Null WTR device ptr", device, tx_handle, fbrx_handle );
    return FALSE;
  } /* if (dev_ptr == NULL) */

  /* Get ILPC Power meter read event event handle */
  pm_read_event_handle = rfcommon_fbrx_get_event_handle_ptr(RFLM_FBRX_CCS_READBACK_POWER_EVENT);

  if ( pm_read_event_handle != NULL )
  {
    /* Create generic buffer interface object to hold script settings */
    pm_read_script = (rf_buffer_intf*) rf_buffer_create ( RFCOMMON_FBRX_MAX_SSBI_SCRIPT_SIZE , 
                                                          RFCOMMON_FBRX_MAX_RFFE_SCRIPT_SIZE , 
                                                          RFCOMMON_FBRX_MAX_GRFC_SCRIPT_SIZE ) ;

    /* Check if valid pointer was returned for buffer object */
    if( pm_read_script != NULL )
    {
      /* Clear buffer */
      (void)rf_buffer_clear(pm_read_script);

      /* Call WTR API to get Power Meter readback script */
      api_status &= rfdevice_cmn_get_fbrx_ilpc_readback_script( dev_ptr,
                                                                RFDEVICE_CREATE_SCRIPT,
                                                                pm_read_script );

      if ( api_status == TRUE )
      {
        *pm_read_event_handle = NULL;
        /* Configure Power Meter Readback event using the obtained settings*/
        api_status = rfcommon_fbrx_ccs_configure_event( &event_settings_generic,
                                                        pm_read_script,
                                                        fbrx_handle,
                                                        tx_handle,
                                                        RFLM_FBRX_CCS_READBACK_POWER_EVENT,
                                                        pm_read_event_handle );

        rfcommon_mdsp_event_cleanup(pm_read_event_handle);

        RF_MSG_3( RF_HIGH, "[FBRX.ILPC] rfcommon_fbrx_ilpc_create_events: Device %d, TxHandle %d, "
                  "FbHandle %d - PM Read Event configured", device, tx_handle, fbrx_handle ); 
      } /* if( api_status == TRUE ) */
      else 
      {
        api_status = FALSE;
        RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_create_events: Device %d, TxHandle %d, "
                  "FbHandle %d - Device Script failed", device, tx_handle, fbrx_handle );
      } /* if ! ( api_status == TRUE ) */

      /* Destroy the buffer */
      rf_buffer_destroy(pm_read_script);
    } /* if( pm_read_script != NULL ) */
    else
    {
      RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_create_events: Device %d, TxHandle %d, "
                "FbHandle %d - Buffer creation failed", device, tx_handle, fbrx_handle );
      api_status = FALSE;
    } /* if ! ( pm_read_script != NULL ) */
  } /* if ( pm_read_event_handle != NULL ) */
  else
  {
    RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_create_events: Device %d, TxHandle %d, "
              "FbHandle %d - Invalid Event Handle", device, tx_handle, fbrx_handle );
    api_status = FALSE;
  } /* if ! ( pm_read_event_handle != NULL ) */

  /* Return */
  return api_status;

} /* rfcommon_fbrx_ilpc_create_events */


/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Configure Sample Capture
 
  @details
  This function will push configuration parameter used for ILPC sample capture
 
  @param device
  RFM device for which sample capture configuration is to be pushed.
 
  @param tx_handle
  TxLM handle for which sample capture configuration is to be pushed.
 
  @param fbrx_handle
  FBRx handle for which sample capture configuration is to be pushed.
 
  @param fbrx_data
  Pointer to data structure containing FBRX data
 
  @return
  Flag indication status of sample capture configuration
*/
boolean
rfcommon_fbrx_ilpc_config_sample_capture
(
  rfm_device_enum_type device,
  rflm_handle_tx_t tx_handle,
  rflm_handle_rx_t fbrx_handle
)
{
  rflm_fbrx_dm_template_t *fbrx_dm_ptr;  /* FBRx DM data pointer */
  boolean ret_val = TRUE;
  rfdevice_rxtx_common_class *dev_ptr = NULL; 
  rfdevice_fbrx_ilpc_accum_data_type ilpc_cfg_data;
  rfcommon_fbrx_data_type* fbrx_data = NULL;

  /* Get FBRx Data */
  fbrx_data = rfcommon_get_fbrx_data();

  /* NULL Pointer check */
  if ( fbrx_data == NULL )
  {
    RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_config_sample_capture: Dev %d, TxHandle %d, "
              "FbHandle %d - NULL FBRx Data",device, tx_handle, fbrx_handle );
    return FALSE;
  } /* if ( fbrx_data == NULL ) */

  /* Get FBRX DM data pointer */
  fbrx_dm_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);

  /* NULL pointer check */
  if ( fbrx_dm_ptr == NULL )
  {
    RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_config_sample_capture: Dev %d, TxHandle %d, "
              "FbHandle %d - NULL FBRX DM Data", device, tx_handle, fbrx_handle );
    return FALSE;
  } /* if ( fbrx_dm_ptr == NULL ) */

  /* Get Device Pointer */
  dev_ptr = rfc_common_get_cmn_device_object( device );

  /* NULL pointer check */
  if (dev_ptr == NULL)
  {
    RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_config_sample_capture: Device %d, "
              "TxHandle %d, FbHandle %d - Null WTR device ptr", device, tx_handle, fbrx_handle );
    return FALSE;
  } /* if (dev_ptr == NULL) */

  /* Get Sample Capture data from WTR Device */
  memset( &ilpc_cfg_data, 0x0, sizeof(rfdevice_fbrx_ilpc_accum_data_type) );

  if ( rfdevice_cmn_get_fbrx_ilpc_accum_info( dev_ptr, &ilpc_cfg_data ) == TRUE )
  {
    /* Sample capture duration in STMR units */
    fbrx_dm_ptr->ilpc_meas_config.stmr_capture_duration = 
      rfcommon_msm_convert_microsecs_to_ustmr_units(ilpc_cfg_data.fbrx_ilpc_accum_time);

    /* Number of TXC samples */
    fbrx_dm_ptr->ilpc_meas_config.num_txc_captured_samples = 2048;

    /* Inverse of number of TXC samples is Q16 format */
    fbrx_dm_ptr->ilpc_meas_config.inv_num_txc_captured_samples_Q16 = 
      ILPC_INV_Q16(fbrx_dm_ptr->ilpc_meas_config.num_txc_captured_samples);

    /* Store number of WTR samples */
    fbrx_dm_ptr->ilpc_meas_config.num_wtr_captured_samples = ilpc_cfg_data.fbrx_ilpc_accum_samples;

    /* Inverse of the WTR samples in Q16 Format */
    fbrx_dm_ptr->ilpc_meas_config.inv_num_wtr_captured_samples_Q16 = 
      ILPC_INV_Q16(ilpc_cfg_data.fbrx_ilpc_accum_samples);

    /* Init HDET coexistance parameters */
    fbrx_dm_ptr->ilpc_meas_config.power_meter_hdet_coex = FALSE;
    fbrx_dm_ptr->ilpc_meas_config.power_meter_hdet_cb = NULL;

    RF_MSG_9( RF_HIGH, "[FBRX.ILPC] rfcommon_fbrx_ilpc_config_sample_capture: Device %d, "
              "TxHandle %d, FbHandle %d - [Duration (%d us, %d stmr), TxC Sample (%d, %d Q16), "
              "WTR Sample(%d, %d Q16)]", 
              device, 
              tx_handle, 
              fbrx_handle,
              ilpc_cfg_data.fbrx_ilpc_accum_time,
              fbrx_dm_ptr->ilpc_meas_config.stmr_capture_duration,
              fbrx_dm_ptr->ilpc_meas_config.num_txc_captured_samples,
              fbrx_dm_ptr->ilpc_meas_config.inv_num_txc_captured_samples_Q16,
              ilpc_cfg_data.fbrx_ilpc_accum_samples,
              fbrx_dm_ptr->ilpc_meas_config.inv_num_wtr_captured_samples_Q16 );

  } /* if ( rfdevice_cmn_get_fbrx_ilpc_accum_info( dev_ptr, &fbrx_ilpc_accum_data ) == TRUE ) */
  else
  {
    RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_config_sample_capture: Device %d, "
              "TxHandle %d, FbHandle %d - Null WTR device ptr", device, tx_handle, fbrx_handle );
    ret_val = FALSE;
  } /* if ( rfdevice_cmn_get_fbrx_ilpc_accum_info( dev_ptr, &fbrx_ilpc_accum_data ) == TRUE ) */

  return ret_val;

} /* rfcommon_fbrx_ilpc_config_sample_capture */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Setup ILPC
 
  @details
  This function is called during Tx initialization to setup the ILPC block. This function would
  create events to do WTR power meter reads and also push sample capture configuration to DM. This
  function is executed only if the ILPC is enabled.
 
  @param device
  RFM device on which ILPC is to be set up
 
  @param tx_handle
  Tx Handle corresponding to which ILPC is to be set up
 
  @param fbrx_handle
  FBRx handle corresponding to which ILPC is to be set up
 
  @return
  Flag indicating the status of ILPC setup
*/
boolean
rfcommon_fbrx_ilpc_setup
(
  rfm_device_enum_type device,
  rflm_handle_tx_t tx_handle,
  rflm_handle_rx_t fbrx_handle
)
{
  boolean enable_flag = TRUE;
  rfcommon_fbrx_data_type* fbrx_data = NULL;
  boolean ret_val = TRUE;

  /* Get FBRx Data */
  fbrx_data = rfcommon_get_fbrx_data();

  /* NULL Pointer check */
  if ( fbrx_data == NULL )
  {
    RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_setup: Dev %d, TxHandle %d, FbHandle %d - "
              "NULL FBRx Data",device, tx_handle, fbrx_handle );
    return FALSE;
  } /* if ( fbrx_data == NULL ) */

  /* Initialize ILPC module */
  ret_val &= rfcommon_fbrx_ilpc_init( fbrx_data );

  /* Check if ILPC can be enabled or not */
  if ( fbrx_data->ilpc_data.enable_ilpc == FALSE )
  {
    RF_MSG_3( RF_HIGH, "[FBRX.ILPC] rfcommon_fbrx_ilpc_setup: Dev %d, TxHandle %d, FbHandle %d - "
              "Skipping as FBRx is disabled", device, tx_handle, fbrx_handle );
    return TRUE;
  } /* if ( fbrx_data->ilpc_data.enable_ilpc == FALSE ) */

  /*  */

  /* Create Power Meter Read Event */
  ret_val &= rfcommon_fbrx_ilpc_create_events( device, tx_handle, fbrx_handle, fbrx_data );

  /* Debug message */
  if ( ret_val == TRUE )
  {
    RF_MSG_3( RF_HIGH, "[FBRX.ILPC] rfcommon_fbrx_ilpc_setup: Dev %d, TxHandle %d, "
              "FbHandle %d - Setup Complete", device, tx_handle, fbrx_handle );
  } /* if ( ret_val == TRUE ) */
  else
  {
    RF_MSG_3( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_ilpc_setup: Dev %d, TxHandle %d, "
              "FbHandle %d - Setup Failed", device, tx_handle, fbrx_handle );
  } /* if ! ( ret_val == TRUE ) */

  return ret_val;

} /* rfcommon_fbrx_ilpc_setup */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Override ILPC to disable mode
 
  @details
  This function will be used to override the ilpc mode of operation to disable.
 
  @return
  Flag indication status of ILPC override
*/
boolean
rfcommon_fbrx_iplc_override_disable
(
  void
)
{
  rfcommon_fbrx_data_type* fbrx_data = NULL;

  /* Get FBRx Data */
  fbrx_data = rfcommon_get_fbrx_data();

  /* NULL Pointer check */
  if ( fbrx_data == NULL )
  {
    RF_MSG( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_iplc_override_disable: NULL FBRx Data" );
    return FALSE;
  } /* if ( fbrx_data == NULL ) */

  fbrx_data->ilpc_data.override_disable_ilpc = TRUE;

  RF_MSG( RF_HIGH, "[FBRX.ILPC] rfcommon_fbrx_iplc_override_disable: ILPC Overridden to disable" );

  return TRUE;

} /* rfcommon_fbrx_iplc_override_disable */

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Override ILPC to enable mode
 
  @details
  This function will be used to override the ilpc mode of operation to enable.
 
  @return
  Flag indication status of ILPC override
*/
boolean
rfcommon_fbrx_iplc_override_enable
(
  void
)
{
  rfcommon_fbrx_data_type* fbrx_data = NULL;

  /* Get FBRx Data */
  fbrx_data = rfcommon_get_fbrx_data();

  /* NULL Pointer check */
  if ( fbrx_data == NULL )
  {
    RF_MSG( RF_ERROR, "[FBRX.ILPC] rfcommon_fbrx_iplc_override_enable: NULL FBRx Data" );
    return FALSE;
  } /* if ( fbrx_data == NULL ) */

  fbrx_data->ilpc_data.override_disable_ilpc = FALSE;

  RF_MSG( RF_HIGH, "[FBRX.ILPC] rfcommon_fbrx_iplc_override_enable: ILPC Overridden to enable" );

  return TRUE;

} /* rfcommon_fbrx_iplc_override_enable */



