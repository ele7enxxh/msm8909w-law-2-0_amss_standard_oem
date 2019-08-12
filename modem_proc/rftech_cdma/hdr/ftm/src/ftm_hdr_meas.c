/*!
  @file ftm_hdr_meas.c

  @brief
  This file contains all declarations and definitions necessary to use
  the hdr FTM Measurement Main Control.

  @details
  APIs in this file follow suite with othe techs to support IRAT mode in FTM.
  The funciton pointer table follows same pattern for all techs,but 
  functionality is specific to the tech owning the table.

  @addtogroup RF_FTM_HDR_MEAS
  @{

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/src/ftm_hdr_meas.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/14   vs      Fix linker errors on W/T compile out flavors
01/31/14   pk      Compiler warning fix
07/10/13   JJ      Rename lagacy flag rumi_bup_cdma  
07/05/13   spa     Register/deregister FTM FW messages
04/25/13   spa     Added support to send/not send pilot meas config
03/26/13   spa     Updated configure FW state API call
03/12/13   spa     Emulate sequence executed by L1 when exiting meas as source
02/22/13   spa     Check for source/target & set FW state accordingly
02/21/13   spa     Send pilot stop stream for tune back when HDR is target
02/19/13   spa     Initial Check-in
=============================================================================*/
#include "rfcom.h"
#include "rfmeas_types.h"
#include "rfmeas_mc.h"
#include "ftm_meas.h"

#include "rf_hdr_mc.h"
#include "rf_hdr_meas.h"
#include "ftm_hdr_control.h"
#include "ftm_hdr_meas.h"
#include "rfmeas_hdr.h"
#include "rfmeas_ftm_mdsp.h"
#include "ftm.h"
#include "ftm_common_control.h"
#include "ftm_msg.h"
#include "rfm_internal.h"
#include "ftm_cdma_data.h"
#include "hdrfw_msg.h"
#include "rfm_hdr_ftm.h"

static rfm_meas_hdr_enter_param_type ftm_hdr_meas_enter_param;
static rfm_meas_hdr_exit_param_type ftm_hdr_meas_exit_param;
static rfm_meas_hdr_setup_and_build_scripts_param_type ftm_hdr_meas_setup_param;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the device on which measurement is being done

  @details
  This function will query the generic test param structure to get the device on 
  which measurements are being done
 
  @param test_param_ptr
  Pointer to generic structure holding IRAT info
 
  @return
  Current device
*/
static rfm_device_enum_type 
ftm_hdr_meas_get_current_device
(
  rfm_meas_generic_params_type *test_param_ptr
)
{
  rfm_device_enum_type device = RFM_INVALID_DEVICE;

  /* Currently hard coded to device 0 */
  if( rf_cdma_debug_flags.temp_workaround == TRUE )
  {
    /*device = test_param_ptr->device;*/
    device = RFM_DEVICE_0;
  }

  return device;
}/* ftm_hdr_meas_get_current_device */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to get the RxLM buffer of source tech

  @details
  This function will get the RxLM buffer of the source tech,
  If hdr is the source tech, it will have a valid RxLm buffer at the time IRAT 
  will start. This populates the test param pointer with the correct RxLm buffer
 
  @pre
  hdr Should already be tuned, and must have valid RxLM buffer
 
  @param test_param_ptr
  Pointer to the generic structure containing test params
 
  @return
  Flag indicating the if the API was success or not
 
  @retval TRUE
  Valid RxLM buffer was found and populated in
  
  @retval FALSE
  Either test param pointer is NULL or the source tech is invalid
 
*/
static boolean 
ftm_hdr_meas_get_src_rxlm_buffer
(
  rfm_meas_generic_params_type *test_param_ptr
)
{
  /* Pointer to data structure holding FTM CDMA device info */
  ftm_cdma_data_status_type *device_status = NULL;
  /* RF device */
  /* Hard code the device for now, this will come from test param ptr */
  rfm_device_enum_type device = ftm_hdr_meas_get_current_device(test_param_ptr);

  /* Check if the test param pointer is valid */
  if(test_param_ptr == NULL)
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_meas_get_src_rxlm_buffer : test param pointer "
             "is null ");
    return FALSE;
  }

  /* Check if the tech is valid, must be hdr in this context */
  if(test_param_ptr->src_tech != RFM_1XEVDO_MODE)
  {
    FTM_MSG_1(FTM_ERROR , 
          "ftm_hdr_meas_get_src_rxlm_buffer : source tech invalid: %d",
          test_param_ptr->src_tech );

    return FALSE;
  }

  /* Get Device Status pointer */
  device_status = ftm_cdma_data_get_device_ptr( device );

  if ( device_status == NULL )
  {
    FTM_MSG_1(FTM_ERROR , "ftm_hdr_meas_get_src_rxlm_buffer : "
          "NULL data for device : %d", device); 
    return FALSE;
  }

  /* Populate test param ptr with rxlm buffer info */
  test_param_ptr->src_rxlm_buf_idx_ant0 = device_status->rxlm_handle;

  return TRUE;
}/*ftm_hdr_meas_get_src_rxlm_buffer*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to enable or disable FW

  @details
  This function will enable or disable the FW, depending on what is required

  @param device
  The device for current scenario
 
  @param fw_state
  Active/Disable/Meas
 
  @return
  none
*/
static void 
ftm_hdr_meas_fw_on_off_control
(
  rfm_device_enum_type device,
  rfm_cdma_fw_state_type fw_state
)
{
  if( fw_state != RFM_CDMA_FW_STATE_DISABLE )
  {
    ftm_hdr_vote_fw_state(device);
  }
  else
  {
    /* unvote if disable requested  */
    ftm_hdr_unvote_fw_state(device);
  }
  /* Change FW state */
  ftm_hdr_configure_fw_state( device, fw_state );

  return;

}/*ftm_hdr_meas_fw_on_off_control*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to get the enter params for hdr

  @details
  This function will populate the hdr specific values, using the generic 
  structure, basically it interprets the generic structure into a strcuture 
  which hdr understands

  @param test_param_ptr
  Pointer to the generic structure containing test params
 
  @return
  void pointer to the hdr specific structure , containing enter parameters
  API will return NULL pointer in case of failure
*/ 
static void* 
ftm_hdr_meas_get_enter_param
(
  rfm_meas_generic_params_type *test_param_ptr
)
{
  void *enter_param_ptr = NULL;

  /* Test Param pointer cannot be NULL  */
  if(test_param_ptr == NULL)
  {
    FTM_MSG(FTM_ERROR , 
        "ftm_hdr_meas_get_enter_param : test param pointer is null ");
    /* Return NULL pointer (failure)*/
    return enter_param_ptr;
  }

  /* Either source or target must be hdr, in this context*/
  if((test_param_ptr->src_tech != RFM_1XEVDO_MODE) &&
     (test_param_ptr->tgt_tech != RFM_1XEVDO_MODE))
  {
    /* Throw error message */
    FTM_MSG_2(FTM_ERROR , 
          "ftm_hdr_meas_get_enter_param: ERROR :src tech %d and tgt tech %d", 
          test_param_ptr->src_tech, test_param_ptr->tgt_tech);
    /* Return NULL pointer (failure)*/
    return enter_param_ptr;
  }
  else if (test_param_ptr->src_tech == RFM_1XEVDO_MODE)
  {
    /* Source tech is hdr, populate members accordingly */
    ftm_hdr_meas_enter_param.device = 
                                 ftm_hdr_meas_get_current_device(test_param_ptr);
    ftm_hdr_meas_enter_param.band_chan.band = test_param_ptr->src_band ;
    ftm_hdr_meas_enter_param.band_chan.chan_num = test_param_ptr->src_chan;
    ftm_hdr_meas_enter_param.rxlm_buf_index = test_param_ptr->src_rxlm_buf_idx_ant0 ;
    /* Cast to void pointer */
    enter_param_ptr = (void*)&ftm_hdr_meas_enter_param;
  }
  else
  {
    /* Call API to enable FW in MEAS state */
    ftm_hdr_meas_fw_on_off_control( ftm_hdr_meas_get_current_device(test_param_ptr), 
                                    RFM_CDMA_FW_STATE_MEAS );
    /* Target tech is hdr, populate members accordingly */
    ftm_hdr_meas_enter_param.device = 
                                 ftm_hdr_meas_get_current_device(test_param_ptr);
    ftm_hdr_meas_enter_param.band_chan.band = test_param_ptr->tgt_band ;
    ftm_hdr_meas_enter_param.band_chan.chan_num = test_param_ptr->tgt_chan;
    ftm_hdr_meas_enter_param.rxlm_buf_index = test_param_ptr->tgt_rxlm_buf_idx_ant0 ;
    /* Cast to void pointer */
    enter_param_ptr = (void*)&ftm_hdr_meas_enter_param;
  }
  /* Return void pointer, pointing to the hdr specific enter params */
  return (enter_param_ptr);
}/*ftm_hdr_meas_get_enter_param*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to get the setup params for hdr

  @details
  This function will populate the hdr specific values, using the generic 
  structure, basically it interprets the generic structure into a strcuture 
  which hdr understands

  @param test_param_ptr
  Pointer to the generic structure containing test params
 
  @return
  void pointer to the hdr specific structure , containing setup parameters
  API will return NULL pointer in case of failure
*/
static void* 
ftm_hdr_meas_get_setup_param
(
  rfm_meas_generic_params_type *test_param_ptr
)
{
  void *setup_param_ptr = NULL;
  
  /* Parameter check, test param pointer cannot be NULL */
  if(test_param_ptr == NULL)
  {
    FTM_MSG(FTM_ERROR , 
        "ftm_hdr_meas_get_setup_param : test param pointer is null ");
    return setup_param_ptr;
  }

  /* L2X or X2L , Either source or target must be hdr in this context */
  if((test_param_ptr->src_tech == RFM_1XEVDO_MODE) || 
     (test_param_ptr->tgt_tech == RFM_1XEVDO_MODE))
  {
    ftm_hdr_meas_setup_param.source_tech = test_param_ptr->src_tech;
    ftm_hdr_meas_setup_param.source_band = test_param_ptr->src_band;
    ftm_hdr_meas_setup_param.target_tech = test_param_ptr->tgt_tech;
    ftm_hdr_meas_setup_param.target_band = test_param_ptr->tgt_band;
    /* hdr is the source tech */
    if(test_param_ptr->src_tech == RFM_1XEVDO_MODE)
    {
      ftm_hdr_meas_setup_param.device = 
                                 ftm_hdr_meas_get_current_device(test_param_ptr);
      ftm_hdr_meas_setup_param.band_chan.band = test_param_ptr->src_band ;
      ftm_hdr_meas_setup_param.band_chan.chan_num = test_param_ptr->src_chan;
      ftm_hdr_meas_setup_param.buffer_index = test_param_ptr->src_buffer_index;
      ftm_hdr_meas_setup_param.rxlm_buf_index = test_param_ptr->src_rxlm_buf_idx_ant0;

    }
    else
    {
      ftm_hdr_meas_setup_param.device = 
                                 ftm_hdr_meas_get_current_device(test_param_ptr);
      ftm_hdr_meas_setup_param.band_chan.band = test_param_ptr->tgt_band ;
      ftm_hdr_meas_setup_param.band_chan.chan_num = test_param_ptr->tgt_chan;
      ftm_hdr_meas_setup_param.buffer_index = test_param_ptr->tgt_buffer_index;
      ftm_hdr_meas_setup_param.rxlm_buf_index = test_param_ptr->tgt_rxlm_buf_idx_ant0;
    }
    /* cast to void pointer */
    setup_param_ptr = (void*)&ftm_hdr_meas_setup_param;
  }
  /* hdr is not source or target, throw error message */
  else
  {
    /* Throw error message */
    FTM_MSG_2(FTM_ERROR , 
          "ftm_hdr_meas_get_setup_param: ERROR :src tech %d and tgt tech %d", 
          test_param_ptr->src_tech, test_param_ptr->tgt_tech);
  }

  /* return void pointer pointing to setup params*/
  return (setup_param_ptr);
}/*ftm_hdr_meas_get_setup_param*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to get the exit params for hdr

  @details
  This function will populate the hdr specific values, using the generic 
  structure, basically it interprets the generic structure into a strcuture 
  which hdr understands

  @param test_param_ptr
  Pointer to the generic structure containing test params
 
  @return
  void pointer to the hdr specific structure , containing exit parameters
  API will return NULL pointer in case of failure
*/
static void* 
ftm_hdr_meas_get_exit_param
(
  rfm_meas_generic_params_type *test_param_ptr
)
{
  void *exit_param_ptr = NULL;

  /* Parameter Check, test param pointer cannot be NULL */
  if(test_param_ptr == NULL)
  {
    FTM_MSG(FTM_ERROR , 
        "hdr Exit : test param pointer is null ");
    return exit_param_ptr;
  }

  /* At least one tech should be hdr in this context */
  if((test_param_ptr->src_tech != RFM_1XEVDO_MODE) &&
     (test_param_ptr->tgt_tech != RFM_1XEVDO_MODE))
  {
    FTM_MSG_2(FTM_ERROR , 
          "hdr Exit called for src tech %d and tgt tech %d",
          test_param_ptr->src_tech, test_param_ptr->tgt_tech);
    return exit_param_ptr;
  }
  else if (test_param_ptr->src_tech == RFM_1XEVDO_MODE)
  {
    /* hdr is source tech */
    ftm_hdr_meas_exit_param.device = 
                                 ftm_hdr_meas_get_current_device(test_param_ptr);
    ftm_hdr_meas_exit_param.band_chan.band = test_param_ptr->src_band ;
    ftm_hdr_meas_exit_param.band_chan.chan_num = test_param_ptr->src_chan;
    ftm_hdr_meas_exit_param.rxlm_buf_index = test_param_ptr->src_rxlm_buf_idx_ant0;
    /* cast to void pointer */
    exit_param_ptr = (void*)&ftm_hdr_meas_exit_param;
  }
  else
  {
    /* hdr is target tech */
    ftm_hdr_meas_exit_param.device = 
                                 ftm_hdr_meas_get_current_device(test_param_ptr);
    ftm_hdr_meas_exit_param.band_chan.band = test_param_ptr->tgt_band ;
    ftm_hdr_meas_exit_param.band_chan.chan_num = test_param_ptr->tgt_chan;
    ftm_hdr_meas_exit_param.rxlm_buf_index = test_param_ptr->tgt_rxlm_buf_idx_ant0;
    /*cast to void pointer */
    exit_param_ptr = (void*)&ftm_hdr_meas_exit_param;
  }

  /* return pointer to exit params */
  return (exit_param_ptr);
}/*ftm_hdr_meas_get_exit_param*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to enter hdr measurement

  @details
  This function will call the meas API using the measurement params

  @param meas_enter_param
  Pointer to the structure containing enter params
 
  @return
  TRUE on Success , FALSE on failure 
*/
static boolean 
ftm_hdr_meas_enter_meas
(
  rfm_meas_enter_param_type *meas_enter_param
)
{
  rfm_meas_result_type status = RFM_MEAS_COMMON_FAILURE;
  boolean result = TRUE;

  if(meas_enter_param == NULL)
  {
    FTM_MSG(FTM_ERROR , 
          "ftm_hdr_meas_enter_meas: meas_enter_param is NULL" );
    return FALSE;
  }
  /* Call API to enter measurement with the enter params */
  status = rfmeas_mc_enter(meas_enter_param, NULL);

  if(status != RFM_MEAS_COMMON_SUCCESS)
  {
    FTM_MSG_1(FTM_ERROR , 
          "ftm_hdr_meas_enter_meas: rfmeas_mc_enter failed with status: %d ",
          status );

    result = FALSE;
  }

  return result;
}/*ftm_hdr_meas_enter_meas*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to build scripts for IRAT

  @details
  This function will use the setup params and build the script needed for
  executing measurement

  @param meas_setup_param
  Pointer to the generic structure containing setup params
 
  @return
  TRUE on Success , FALSE on failure 
*/
static boolean 
ftm_hdr_meas_build_script
(
  rfm_meas_setup_param_type *meas_setup_param
)
{
  rfm_meas_result_type status = RFM_MEAS_COMMON_FAILURE;
  boolean result = TRUE;
  if(meas_setup_param == NULL)
  {
    FTM_MSG(FTM_ERROR , 
          "ftm_hdr_meas_build_script: meas_setup_param is NULL" );
    return FALSE;
  }
  /* Call API to build scripts  with setup params */
  status = rfmeas_build_scripts(meas_setup_param, NULL);

  /* Mark failure if API is not success */
  if(status != RFM_MEAS_COMMON_SUCCESS)
  {
    FTM_MSG_1(FTM_ERROR , 
          "ftm_hdr_meas_build_script: rfmeas_build_scripts failed with status:"
          "%d ",status );
    result = FALSE;
  }

  return result;
}/*ftm_hdr_meas_build_script*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to exit hdr measurement

  @details
  This function will exit measurement using the exit params

  @param meas_exit_param
  Pointer to the structure containing exit params
 
  @return
  TRUE on Success , FALSE on failure 
*/
static boolean 
ftm_hdr_meas_exit_meas
(
  rfm_meas_exit_param_type *meas_exit_param
)
{
  rfm_meas_result_type status = RFM_MEAS_COMMON_FAILURE;
  boolean result = TRUE;

  if(meas_exit_param == NULL)
  {
    FTM_MSG(FTM_ERROR , 
          "ftm_hdr_meas_exit_meas: meas_exit_param is NULL" );
    return FALSE;
  }
  /* Call API to build scripts  with exit params */
  status = rfmeas_mc_exit(meas_exit_param, NULL);

  if(status != RFM_MEAS_COMMON_SUCCESS)
  {
    /* Mark failure if API is not success */
    FTM_MSG_1(FTM_ERROR , 
          "ftm_hdr_meas_exit_meas: ftm_hdr_meas_exit_meas failed with status:"
          "%d ",status );
    result = FALSE;
  }

  return result;
}/*ftm_hdr_meas_exit_meas*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to exit hdr measurement and disable related processes

  @details
  This function will exit hdr, i.e it will stop all hdr related processes,
  Namely, stop RxAGC and stop FW. This calls the RF level meas API to disable 
  RxAGC, and then disables FW and MCPM

  @param test_param_ptr
  Pointer to the generic structure containing test params
 
  @param use_src_param
  Flag to tell whether tech is source or target (used in scenarios like w-w,
  L-L, or T-T )
 
  @return
  TRUE on Success , FALSE on failure 
*/
static boolean 
ftm_hdr_meas_exit_hdr
(
  rfm_meas_generic_params_type *test_param_ptr, 
  boolean use_src_param
)
{
  boolean api_status = TRUE;
  /* Return type for cleanup API*/
  rfm_meas_result_type cleanup_result = RFM_MEAS_COMMON_SUCCESS;

  if( test_param_ptr == NULL )
  {
    FTM_MSG(FTM_ERROR, "ftm_hdr_meas_exit_hdr:NULL param pointer");
    return FALSE;
  }

  /* call cleanup measurement here */
  /* Step : 1. Stop AGC, check if hdr is source or target , use the 
     corresponding test params*/ 
  /* hdr is source tech */
  if( (test_param_ptr->src_tech == RFM_1XEVDO_MODE) &&
      (test_param_ptr->tgt_tech != RFM_1XEVDO_MODE ) )
  {
    /* Do nothing */
  }
  else if( (test_param_ptr->src_tech != RFM_1XEVDO_MODE) &&/* hdr is target */
         (  test_param_ptr->tgt_tech == RFM_1XEVDO_MODE ) )
  {
    /* Check if pilot meas stop needs to be sent, this is used in 
    scenarios where we need to test if L1 sends/does not send this mssg */
    if( rf_cdma_debug_flags.send_pilot_meas_config == TRUE)
    {
      FTM_MSG(FTM_MED, "ftm_hdr_meas_exit_hdr:Sending pilot meas stop"
                         "stream message");
      /*send pilot meas stop stream to FW*/ 
      api_status = rfm_hdr_ftm_pilot_meas_stop_stream(
                               ftm_hdr_meas_get_current_device(test_param_ptr));
    }/*if( rf_cdma_debug_flags.send_pilot_meas_config == TRUE)*/
    else
    {
      api_status = TRUE;
      FTM_MSG(FTM_MED, "ftm_hdr_meas_exit_hdr:Not sending pilot meas stop"
                         "stream ");
    }/*if( rf_cdma_debug_flags.send_pilot_meas_config != TRUE)*/  
  }
  /* Everything else is an error ! */
  else
  {
    FTM_MSG_3(FTM_ERROR, 
          "ftm_hdr_meas_exit_hdr: Wrong tech for disabling FW, use_src_param %d," 
          "src_tech %d, tgt_tech %d", use_src_param, test_param_ptr->src_tech, 
          test_param_ptr->tgt_tech); 
    api_status = FALSE;
  }

  /* Check if cleanup was successful */
  if ( cleanup_result == RFM_MEAS_COMMON_FAILURE )
  {
    FTM_MSG_3(FTM_ERROR, 
          "ftm_hdr_meas_exit_hdr:Failed to disable RxAGC use_src_param %d, " 
          "src_tech %d, tgt_tech %d",use_src_param, test_param_ptr->src_tech, 
          test_param_ptr->tgt_tech);
    return api_status;
  }

  /* Step 2: Stop FW */
  if(((test_param_ptr->src_tech == RFM_1XEVDO_MODE) &&
      (test_param_ptr->tgt_tech != RFM_1XEVDO_MODE)) ||
     ((test_param_ptr->tgt_tech == RFM_1XEVDO_MODE) &&
      (test_param_ptr->src_tech != RFM_1XEVDO_MODE)))
  {
    /* Call API to disable FW*/
    ftm_hdr_meas_fw_on_off_control( ftm_hdr_meas_get_current_device(test_param_ptr), 
                                    RFM_CDMA_FW_STATE_DISABLE );
    api_status = TRUE;
  }
  else
  {
    FTM_MSG( FTM_ERROR, 
          "ftm_hdr_meas_exit_hdr: source and target both hdr not supported" ); 
    api_status = FALSE;
  }

  if(api_status != TRUE)
  {
    FTM_MSG_2(FTM_ERROR ,
          "ftm_hdr_meas_exit_hdr: failed to disable FW src_tech %d, tgt_tech %d",
          test_param_ptr->src_tech, test_param_ptr->tgt_tech);
  }

  return api_status;
}/* ftm_hdr_meas_exit_hdr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to enter hdr 

  @details
  This function will enter and start RxAGC measurement using the 'enter' params
  This first enables FW and MCPM clocks, then sends the pilot config message to
  FW, FW proceeds to execute the IRAT script,execute RxLM & enable RxAGC
 
  @param test_param_ptr
  Pointer to the generic structure containing test params
 
  @param
  Flag to tell whether tech is source or target (used in scenarios like w-w,
  L-L, or T-T )
 
  @return
  TRUE on Success , FALSE on failure 
*/
static boolean 
ftm_hdr_meas_enter_hdr
(
  rfm_meas_generic_params_type *test_param_ptr, 
  boolean use_src_param
)
{
  boolean api_status = FALSE;

  if(test_param_ptr == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_hdr_meas_enter_hdr:NULL param pointer");
    return FALSE;
  }

  /* Step 1: Enable FW */
  if((test_param_ptr->tgt_tech == RFM_1XEVDO_MODE) &&
     (test_param_ptr->src_tech != RFM_1XEVDO_MODE) )
  {
    /* Call API to enable FW in MEAS state if HDR is target */
    ftm_hdr_meas_fw_on_off_control( ftm_hdr_meas_get_current_device(test_param_ptr), 
                                    RFM_CDMA_FW_STATE_MEAS );
    api_status = TRUE;
  }
  else if((test_param_ptr->tgt_tech != RFM_1XEVDO_MODE) &&
          (test_param_ptr->src_tech == RFM_1XEVDO_MODE) )
  {
    /* Call API to enable FW in ACTIVE state if HDR is target */
    ftm_hdr_meas_fw_on_off_control( ftm_hdr_meas_get_current_device(test_param_ptr), 
                                    RFM_CDMA_FW_STATE_ACTIVE );
    api_status = TRUE;
  }
  else
  {
    FTM_MSG( FTM_ERROR, 
          "ftm_hdr_meas_enter_hdr: source and target both hdr not supported" ); 
    api_status = FALSE;
  }

  if(api_status != TRUE)
  {
    FTM_MSG_3(FTM_ERROR, 
          "ftm_hdr_meas_enter_hdr:Failed to enable FW use_src_param %d, " 
          "src_tech %d, tgt_tech %d",use_src_param, test_param_ptr->src_tech, 
          test_param_ptr->tgt_tech);
    return api_status;
  }

  /* Step 2: Send pilot config message to FW which will execute the script,
  execute RxLM and enable RxAGC if HDR is the target tech*/
  if((test_param_ptr->tgt_tech == RFM_1XEVDO_MODE) &&
     (test_param_ptr->src_tech != RFM_1XEVDO_MODE) )
  {
    /* Check if pilot meas config needs to be sent, this is used in 
    scenarios where we need to test if L1 sends/does not send this mssg */
    if( rf_cdma_debug_flags.send_pilot_meas_config == TRUE)
    {
      FTM_MSG( FTM_MED, 
            "ftm_hdr_meas_enter_hdr:Sending pilot meas config message " ); 
      /* Send the FLL XO config message */
      api_status = rfm_hdr_ftm_fll_xo_cfg_msg(                           
                              ftm_hdr_meas_get_current_device(test_param_ptr));
      /* Send the pilot meas config message */ 
      api_status = rfm_hdr_ftm_pilot_meas_cfg(                           
                              ftm_hdr_meas_get_current_device(test_param_ptr)); 
    }
    else
    {
      /* Set API status as TRUE and print message*/
      api_status = TRUE;
      FTM_MSG( FTM_MED, 
            "ftm_hdr_meas_enter_hdr:Not sending pilot meas config message " ); 
    }
  }
  else if((test_param_ptr->tgt_tech != RFM_1XEVDO_MODE) &&
          (test_param_ptr->src_tech == RFM_1XEVDO_MODE) )
  {
    /* Emulate HDR-L1 here*/
    ftm_hdr_rfm_callback_data_type cb_payload;
    /* To track if rfm APIs passed/failed*/
    int32 rfm_status;
    /* Populate the CB Payload */ 
    cb_payload.device = ftm_hdr_meas_get_current_device(test_param_ptr); 
    /* Call RFM Function to Sleep Rx */
    rfm_status = rfm_hdr_sleep_rx( ftm_hdr_meas_get_current_device(test_param_ptr), 
                                   test_param_ptr->src_rxlm_buf_idx_ant0, 
                                   &ftm_hdr_rfm_callback,
                                   &cb_payload );

    if( rfm_status < 0 )
    {
      FTM_MSG_1( FTM_ERROR, "ftm_hdr_meas_enter_hdr : RFM failure in sleep Rx " 
                 "Status =%d", rfm_status );
      api_status = FALSE;
    }
    else
    {
      /*Call RFM API to disable HDR*/
      rfm_status = rfm_hdr_disable( ftm_hdr_meas_get_current_device(test_param_ptr), 
                                    test_param_ptr->src_rxlm_buf_idx_ant0, 
                                    &ftm_hdr_rfm_callback, 
                                    &cb_payload );

      if( rfm_status < 0 )
      {
        FTM_MSG_1( FTM_ERROR, "ftm_hdr_meas_enter_hdr : RFM failure in disable "
                   "HDR, Status =%d", rfm_status );
        api_status = FALSE;
      }
      else
      {
        /* Mark success only if we get to this point without failing!*/
        api_status = TRUE;
      }/*if ! ( rfm_status < 0 )*/
    }/*if ! ( rfm_status < 0 )*/
  }
  else
  {
    FTM_MSG( FTM_ERROR, 
          "ftm_hdr_meas_enter_hdr: source and target both HDR not supported" ); 
    api_status = FALSE;
  }

  if(api_status != TRUE)
  {
    FTM_MSG(FTM_ERROR, "ftm_hdr_meas_enter_hdr:Failed to enter HDR "); 
  }

  return api_status;

}/*ftm_hdr_meas_enter_hdr*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer table for HDR APIs

  @details
  This funciton table contains all HDR specific APIs. The FTM IRAT framework 
  calls a generic set of APIs, and is directed to the tech specific APIs via 
  this funciton table.
*/
static ftm_meas_func_tbl_type ftm_hdr_meas_apis = 
{
   ftm_hdr_meas_get_src_rxlm_buffer,
   ftm_hdr_meas_get_enter_param,
   ftm_hdr_meas_get_setup_param,
   ftm_hdr_meas_get_exit_param,
   ftm_hdr_meas_enter_meas,
   ftm_hdr_meas_build_script,
   ftm_hdr_meas_exit_hdr,
   ftm_hdr_meas_enter_hdr,
   ftm_hdr_meas_exit_meas
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Registers for hdr FTM IRAT

  @details
  This function will register for hdr FTM IRAT, it will register the funciton 
  pointers,allocate memory for hdr specific structures 
 
*/
void 
ftm_hdr_meas_register
(
  void
)
{

  boolean result = TRUE;
  
  #ifdef FTM_HAS_IRAT
  result = ftm_meas_register(RFM_1XEVDO_MODE, &ftm_hdr_meas_apis);
  #endif
  /* proceed only if register was success*/
  if( result == TRUE )
  {
    memset(&ftm_hdr_meas_enter_param,0,sizeof(rfm_meas_hdr_enter_param_type));
    memset(&ftm_hdr_meas_exit_param,0,sizeof(rfm_meas_hdr_exit_param_type));
    memset(&ftm_hdr_meas_setup_param,0,
           sizeof(rfm_meas_hdr_setup_and_build_scripts_param_type));

    /*Register HDR fw messages */
    rfm_hdr_ftm_register_fw_msgs();
  }
  else
  {
    FTM_MSG(FTM_ERROR , "ftm_hdr_meas_get_src_rxlm_buffer : ftm_meas_register"
                        " failed");
  }

  return;
}/* ftm_hdr_meas_register */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-registers for hdr FTM IRAT

  @details
  This function will de-register for hdr FTM IRAT
 
*/
void 
ftm_hdr_meas_deregister
(
  void
)
{

  boolean result = TRUE;
  /* Deregister as HDR */
  result = ftm_meas_deregister(RFM_1XEVDO_MODE);

  /* De-register FTM FW messages */
  rfm_hdr_ftm_deregister_fw_msgs();

  if( result == FALSE )
  {
    FTM_MSG( FTM_ERROR , "ftm_hdr_meas_get_src_rxlm_buffer : "
             "ftm_meas_deregister failed");
  }

  return;

}/* ftm_hdr_meas_deregister */

/*! @} */
