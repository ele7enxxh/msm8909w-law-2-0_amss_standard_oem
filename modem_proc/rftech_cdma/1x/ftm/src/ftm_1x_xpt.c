/*!
  @file
  ftm_1x_xpt.c

  @brief
  This module contains FTM 1x XPT Control APIs which are used to control
  the 1x Radio for XPT functionality.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/src/ftm_1x_xpt.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/10/15   vr      Add support for XPT cal when Drx path not present
04/10/15   vr      Use unique RxLM handle for XPT FB capture 
04/07/15   vr      Fixed call flow in ftm_1x_xpt_cal_config
02/04/14   jmf     Feedback Setup for XPT config updated with 
                   support for configuring fb gain state
01/31/14   jmf     Add support for FBRx type XPT IQ Capture
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/03/12   Saul    XPT. Use MCPM API to vote for cal clock speed.
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/22/13   jmf     Implement xPT processing Enum Type
04/09/13   aro     Removed unnecessary Data Module hookup
03/19/13   aro     Modularized FB config into transceiver, asm, and rxlm
03/19/13   aro     Added XPT CDMA cal deconfig
03/19/13   aro     Added call to XPT cal config from protocal IQ capture
03/19/20   aro     Added Data module hookup
03/13/13   sn      Dual WTR fix for XPT cal
03/05/13   hdz     Added ftm_1x_xpt_cal_config()
01/22/13   aro     Code to use unified IQ capture param
01/22/13   aro     Added common XPT capture type
01/22/13   aro     Removed DPD index from IQ capture interface
01/18/13   aro     Renamed function name to have XPT
01/18/13   aro     Removed nikel-XPT specific code
12/13/12   Saul    Common. Changed XPT capture, dpd, tx agc override intf.
11/28/12   Saul    Sample Capture. Temp features to FEATURE_RF_HAS_XPT_SUPPORT
11/16/12   Saul    Sample Capture. Use common response.
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
10/25/12   cd/Saul Initial Release. Generalized load DPD table API
                   - Changed request packet type
                   - Added response packet type
                   - Made XPT IQ capture common.

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "ftm.h"
#include "ftm_1x_xpt.h"
#include "ftm_common_xpt.h"
#include "ftm_msg.h"
#include "rfm_1x.h"
#include "rfm_1x_ftm.h"
#include "rfcommon_core_xpt.h"
#include "rfm_cdma_ftm.h"
#include "ftm_cdma_data.h"
#include "ftm_common_control.h"
#include "rfcommon_mdsp.h"
#include "ftm_calibration_v3_xpt.h"
#include "rfcommon_data.h"
#include "rfcommon_mc.h"
#include "mcpm_api.h"


/*============================================================================*/
/*!
  @name FTM 1x XPT external interfaces

  @brief
  This block includes the FTM APIs for the XPT functionality externalized to
  other modules
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Trigger Single instance of IQ Capture

  @details
  This function will call RF-MDSP function to trigger the IQ Capture
  for EPT for 1x.
 
  @param device
  Primary device for the Transmit path for IQ capture
 
  @param device_fb
  Feedback device for the IQ capture
 
  @param xpt_capture_type
  Indicate what mode the capture is for e.g. EPT/ET
 
  @param num_samples
  Number of TXC/RX samples to capture.Must be a multiple of 1024(1K)
 
  @param first_trig
  Flag to do initialization.This must be set in 1st trigger
 
  @param last_trig
  Flag to do de-initialization.This must be set in last trigger 
 
  @param do_processing
  Flag to indicate if firmware should do DPD estimation on the captured samples.

  @param buffer_id
  Buffer index to store captured data and processed data

  @param samp_rate
  Pointer return to hold sampling rate of capture
  
  @return
  TRUE is set successfully; FALSE otherwise

*/
boolean
ftm_1x_xpt_iq_capture
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  uint8 xpt_capture_type,
  uint16 num_samples,
  boolean first_trig,
  boolean last_trig,
  rfcommon_xpt_processing_type do_processing,
  rf_common_xpt_buffer_id_type buffer_id,
  uint32 * samp_rate
)
{
  boolean ret_val = TRUE;
  ftm_cdma_data_status_type dev_state; /* Var to hold current state */
  rfm_1x_iq_capture_param_type capture_param; /* Capture param */
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;
  boolean iq_cap_status;

  (void)ftm_cdma_data_get_device_status( device, &dev_state );  

 
  /*--------------------------------------------------------------------------*/
  /* IQ Capture */
  /* Simulating : IQ Captures with Tx COnfig */
  /*--------------------------------------------------------------------------*/

  /* Based on payload data, populate the capture_type */
  capture_param.capture_type = 
       ftm_common_xpt_map_capture_type( 
             (ftm_calibration_xpt_iq_capture_enum)xpt_capture_type );
  capture_param.num_samples = num_samples;
  capture_param.first_trig = first_trig;
  capture_param.last_trig = last_trig;
  capture_param.do_processing = do_processing;
  capture_param.buffer_id = buffer_id;
  capture_param.split_samp = 0;

  /* Store the IQ Caprure Data */
  iq_cap_data.device = device;
  iq_cap_data.device_fb = device_fb;
  iq_cap_data.channel = dev_state.curr_chan;
  iq_cap_data.is_last_trig = last_trig;
  iq_cap_data.buffer_id = buffer_id;
  iq_cap_data.capture_type = capture_param.capture_type;
  ret_val &= rfcommon_xpt_store_iq_capture_data( &iq_cap_data );

  /* Perform EPT Capture */
  iq_cap_status = rfm_1x_ftm_xpt_iq_capture( device, 
                                             device_fb, 
                                             capture_param,
                                             samp_rate );

  ret_val &= iq_cap_status;

  /* if xpt_iq_capture failed, unvote modem bus clk immediately;
     otherwise, modem bus clk will be unvoted at capture done response
     when last_trig is true*/
  if ( iq_cap_status == FALSE )
  {
    if (capture_param.capture_type == XPT_CAPTURE_FBRX)
    {
      // do nothing
      FTM_MSG(FTM_HIGH, "ftm_1x_xpt_iq_capture: FBRx type IQ Capture failed,"
                         " use feedback_setup api to turn off FB device");
    }
    else
    {
      ftm_1x_xpt_cal_deconfig( device, device_fb );
    }
  }

  /* F3 message */
  FTM_MSG_3( FTM_HIGH, "ftm_1x_xpt_iq_capture: Dev %d, %d "
                       "[Status %d]", device, device_fb, ret_val );

  return ret_val;

} /* ftm_1x_xpt_iq_capture */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Xpt cal configuration

  @details
  This function will call ftm APIs to configure feedback path to be ready for
  IQ capture.
 
  @param device
  Primary device for the Transmit path for IQ capture
 
  @param device_fb
  Feedback device for the IQ capture
 
  @param xpt_capture_type
  Indicate what mode the capture is for e.g. EPT/ET

  @param num_samples
  Number of samples for IQ capture
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
ftm_1x_xpt_cal_config
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb
)
{
  boolean ret_val = TRUE;
  ftm_cdma_data_status_type dev_state; /* Var to hold current state */
  ftm_cdma_data_status_type *dev_fb_state; /* Var to hold current state */
  mcpm_mcvsrequest_parms_type params;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Dev */
  lm_status_type rx_buffer_allocated;

  /* Step 1: FTM configuration */
  /* FTM Enter to 1x mode for fedback path + Get FTM Status for FB device */
  ftm_rfmode_enter( device_fb, FTM_STATE_1X );
  rfm_enter_mode( device_fb, RFM_1X_MODE, NULL, NULL,0 );
  (void)ftm_cdma_data_get_device_status( device, &dev_state );  

  /* Get pointer to device data */
  dev_fb_state = ftm_cdma_data_get_device_ptr( device_fb );

  if ( dev_fb_state == NULL )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_1x_xpt_cal_config: Failed to get dev data ptr "
                         "for device %d", device_fb); 
    return FALSE;
  }

  /* Get logical device parameter */
  logical_dev = rfm_cdma_ftm_get_logical_device_params(device_fb);

  /* validate for NULL ptr */
  if ( logical_dev == NULL )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_1x_xpt_cal_config: Failed to get valid logical "
                         "device for device %d", device_fb); 
    return FALSE;
  }

  /* Configure EPT  */
  ret_val &= rfm_1x_ftm_xpt_cal_configure( device, device_fb );

  /*--------------------------------------------------------------------------*/
  /* FB_CONFIG */
  /*--------------------------------------------------------------------------*/
 /* Allocate RxLM buffer for feedback path */
  /* if this check is succesful, that means that drx path is valid, follow
  normal procedure here*/
  if ( logical_dev->ftm_rxlm_chain_onex < RXLM_CHAIN_MAX )
  { 
    
    /* Allocate RxLM buffer */
    rx_buffer_allocated = rxlm_allocate_buffer( 
                                               logical_dev->ftm_rxlm_chain_onex,
                                               LM_1X,
                                               &dev_fb_state->xpt_fb_handle);
   }
  else
  {
    /* check failed, use workaround for the feedback device */
    rxlm_allocate_buffer( (rxlm_chain_type)device_fb,
                          LM_1X,
                          &dev_fb_state->xpt_fb_handle);

    rx_buffer_allocated = LM_SUCCESS;
  }


   /* If RxLM buffer allocation for XPT feedback path is successful, proceed
  otherwise return failure */
  if ( rx_buffer_allocated == LM_SUCCESS )
  {
  /* Configure feedback RxLM */
    rfm_1x_ftm_config_fb_rxlm( device,
                               device_fb, 
                               dev_fb_state->xpt_fb_handle,
                                        dev_state.curr_band,
                                        dev_state.curr_chan );

  /* Configure feedback transceiver */
    rfm_cdma_ftm_config_fb_transceiver( device,
                                                 device_fb, 
                                                 RFM_1X_MODE,
                                                 dev_state.curr_band,
                                                 dev_state.curr_chan,
                                                 2 /* Enum to be defined in FBRx component */
                                               );

  /* Configure feedback ASM */
    rfm_cdma_ftm_config_fb_asm( device_fb, 
                                         dev_state.curr_band, 
                                         TRUE);

  /* Vote modem bus clk to 144 MHz*/
  params.mcvs_req_type = MCVS_FULL_REQUEST;
  params.modem_clk_bus_KHz = 144000;
  params.mcvs_update_info = MCPM_MCVS_CLKBUS_UPDATE;
  MCPM_MCVSConfig_Modem(MCPM_1X_TECH, &params);
  FTM_MSG_1( FTM_LOW, "ftm_1x_xpt_cal_config: Offline speed returned by MCPM %d", 
             MCPMDRV_Get_Modem_Offline_Clock(MCPM_1X_TECH) );
  }/* if ( rx_buffer_allocated == LM_SUCCESS ) */
  else
  {
    FTM_MSG_1( FTM_LOW, "ftm_1x_xpt_cal_config: Failed to allocate rxlm "
                        "handle for device: %d", device_fb );
    ret_val = FALSE;
  }

  FTM_MSG_1( FTM_LOW, "ftm_1x_xpt_cal_config: xpt cal config done %d", 
             ret_val );

  return ret_val;
} /* ftm_1x_xpt_cal_config */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Xpt cal deconfiguration

  @details
  This function will call ftm APIs to deconfigure feedback path after
  IQ capture is completed.
 
  @param device
  Primary device for the Transmit path for IQ capture
 
  @param device_fb
  Feedback device for the IQ capture
  
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
ftm_1x_xpt_cal_deconfig
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb
)
{
  boolean ret_val = TRUE;
  lm_status_type rx_buffer_deallocated;

  ftm_cdma_data_status_type dev_state; /* Var to hold current state */
  ftm_cdma_data_status_type *dev_fb_state; /* Var to hold current state */

  (void)ftm_cdma_data_get_device_status( device, &dev_state );  

/* Get pointer to device data */
  dev_fb_state = ftm_cdma_data_get_device_ptr( device_fb );

  if ( dev_fb_state == NULL )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_1x_xpt_cal_config: Failed to get dev data ptr "
                         "for device %d", device_fb); 
    return FALSE;
  }
  /* Disable ET Path */
  rfm_cdma_ftm_deconfig_fb_transceiver( device, device_fb, RFM_1X_MODE );

  ret_val &= rfm_cdma_ftm_config_fb_asm( device_fb, 
                                         dev_state.curr_band, 
                                         FALSE);
  rx_buffer_deallocated =
    rxlm_deallocate_buffer( dev_fb_state->xpt_fb_handle );

  if ( rx_buffer_deallocated != LM_SUCCESS )
  {
    FTM_MSG_1( FTM_LOW, "ftm_1x_xpt_cal_deconfig: RxLM deallocation failed"
                        " for device %d", device );
    ret_val = FALSE;

  }

  /* UnVote modem bus clk*/
  MCPM_MCVSConfig_Release(MCPM_1X_TECH);
  FTM_MSG_1( FTM_LOW, "ftm_1x_xpt_cal_deconfig: Offline speed returned by MCPM %d", 
             MCPMDRV_Get_Modem_Offline_Clock(MCPM_1X_TECH) );

  FTM_MSG_1( FTM_LOW, "ftm_1x_xpt_cal_deconfig: xpt cal deconfig done %d", 
             ret_val );

  return ret_val;
} /* ftm_1x_xpt_cal_deconfig */

#endif /* FEATURE_CDMA1X */
/*! @} */
