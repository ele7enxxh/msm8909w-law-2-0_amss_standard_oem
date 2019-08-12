/*! 
  @file
  rf_1x_ept.c
 
  @brief
  This file defines the 1x interfaces to be used for Envelope Power Tracking. 
 
  @addtogroup RF_CDMA_1X_XPT
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rf_1x_ept.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/10/15   vr      Add support for XPT cal when Drx path not present
11/25/14   jmf     [selftest] Support for CDMA VSWR v2 processing
10/20/14   cd      Rx_device information should be band-specific
08/19/14   jmf     samp_rate query for correct FBRx LM buffer
07/23/14   cd      Update ET delay interface to RFLM, to avoid computation in 
                   critical path
02/21/14   jmf     C2k FBRx Caps for Droop/DC/RSB Cal
01/31/14   jmf     Added support for FBRx Type IQ capture
01/13/14   cd      Add ET delay programming to RFLM for online mode
01/06/14   cd      Linearizer and Tx AGC updates for loading xPT information to 
                   RFLM
12/26/13   sar/hdz Remove et_txlm_hanlde from Bolt c2k code, and use one single 
                   TxLm handle allocation.
11/14/13   kma     Added pin calibration support
10/16/13  cc       Add new ET logging for FTM AGC logs
08/26/13   shb     Updates for new C++ transceiver device interface
08/09/13   APU     KW fix.
08/01/13   Saul    XPT. ET delay vs temp FTM char support
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
07/19/13   vr      Changes to incorporate device dependennt ICIFIR filter settings
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
07/02/13   Saul    XPT. ET delay compensation
06/19/13   APU     Added new NV for ET delay. 
06/03/13   spa     Migrate to new RF dispatch for XPT FW response mssgs 
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/13/13   Saul    XPT. Support EPT_ET_CFG.
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/26/13   aro     F3 message update to match TxMgr log parser
04/22/13   jmf     Implement xPT processing Enum Type
04/08/13   jmf     Remove hard-coding of DPD config params, get from NV
03/19/13   aro     Deleted ET config path
03/19/13   aro     Modularized FB config into transceiver, asm, and rxlm
03/19/13   aro     Added XPT CDMA cal deconfig
03/19/13   aro     Added call to XPT cal config from protocal IQ capture
03/11/13   spa     Updated config_et_tx to not wait for pa config response
03/01/13   Saul    CDMA. FEATURE_RF_XPT_CDMA_TEMP -> FEATURE_RF_HAS_XPT_SUPPORT
03/01/13   Saul    XPT cfg blocking for first response during cal and second
                   response when not in cal.
03/01/13   hdz     Move npa vote from rf_1x_ept.c to rfm_1x_ftm.c
02/28/13   hdz     Added npa vote to 144 MHz at first trig and unvote at last trig
02/27/13   aro     Compiler warning fix
02/27/13   aro     Mainlined debug flag for Tx resource manager
02/27/13   aro     Interface to determine XPT mode of operation based on MSM
                   and NV
02/27/13   Saul    CDMA. ET now uses separate envelope scale from EPT.
02/26/13   Saul    CDMA. Set EPT switch-point to 42 dBm when ET is enabled
02/25/13   aro     Updated XPT configure interface to pass XPT mode from
                   calling function
02/22/13   Saul    Tx ET Path Delay NV support.
02/21/13   aro     Updated configure XPT interface to push band and NV chain
                   parameter for calling function
02/21/13   aro     CDMA Tx Resource manager implementation
02/14/13   Saul    CDMA. Use DPD CFG NV to enable XPT.
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/22/13   aro     Code to use unified IQ capture param
01/22/13   aro     Added common XPT capture type
01/22/13   aro     Removed unused variables
01/22/13   aro     Removed DPD index from IQ capture interface
01/21/13   aro     Cleaned up IQ Capture and XPT config interfaces
01/21/13   Saul    CDMA. Mainline XPT items.
01/18/13   aro     Renamed function name to have XPT
01/18/13   aro     Removed memory allocation for IQ capture
01/18/13   aro     Deprecated XPT polling code
01/18/13   aro     Deleted nikel-XPT deprecated FW responses
01/18/13   aro     Deleted nikel-XPT specific functions
01/16/13   Saul    CDMA. Enabled XPT/ET tempcomp.
01/15/13   cd      Remove temporary feature for XPT response
01/15/13   jmf     FW DPD processing param rx_compact changed to 1 from 0.
01/14/13   Saul    CDMA. 1x XPT/ET call support.
01/09/13   sty     Use RFCOMMON_MDSP_SNUM_TECH instead of RF_CDMA_MDSP_SNUM_TECH
12/19/12   cd      Fixes for ET Tx configuration in FTM mode only
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/18/12   cd      Additional changes for FTM ET Tx including xPT config 
                   response handling
12/18/12   sty     Added band info in arg list for rf_cdma_msm_configure_txlm
12/11/12   Saul    CDMA. TX AGC XPT Dev. Get max PA state based on lin NV.
12/10/12   jmf     Remove last_trig dependency of setting dpd proc params
12/06/12   jmf     Included rfcommon_mdsp.h to pass hard-coded FW DPD proc param
12/06/12   APU     Replaced LM chains with Modem chains.
12/04/12   aro     F3 message update
11/16/12   Saul    Sample Capture. Use common response.
11/15/12   Saul    CDMA. TX AGC XPT.
11/08/12   cd      Fixed compiler warnings
11/08/12   cd      Added functionality to configure ET path
10/25/12   cd/Saul Generalized load DPD table API
                   - Changed request packet type
                   - Added response packet type
                   - Made XPT IQ capture common.
10/03/12   Saul    CDMA Sample Capture. Num samples 12K. Capture done message.
09/28/12   aro     Migrated 1x MC to use new RF-FW Sync
09/19/12   Saul    CDMA Sample Capture. Capture type, do leakage flag, 
                   stop tx agc update fw command
09/10/12   aro     SNUM dispatch callback function for IQ Capture and
                   DPD expansion
09/07/12   aro     Renamed the DPD expansion function
09/05/12   aro     FWRSP registration for IC_UPDATE and EPT IQ Capture command
09/05/12   aro     Interface to indicate DPD coeff expansion
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
09/04/12   aro     Added band and channel to XPT FB config interface
09/04/12   Saul    CDMA. Warning fixes.
08/22/12   aro     SW memory allocation fixes
08/22/12   cri     XPT support for CDMA FW interfaces
08/22/12   aro     Changes to migrate to new precise time profiler
08/31/12   Saul    CDMA. Added support for XPT_PROCESSING_DONE FW message
08/30/12   Saul    CDMA. Removed unnecessary code that caused crash in ept cfg.
08/21/12   APU     Added support to get modem_chain from rfc_device_info
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/16/12   aro     Compiler warning fix
08/08/12   vs      Added support to query XPT config params
08/07/12   aro     Enabled NV load for DPD config
08/07/12   sty     Warning fixes 
07/31/12   aro     Removed Tx IQ gain override from XPT configure
07/25/12   aro     DPD index in IQ Capture command
07/20/12   aro     Added support to do polling for XPT expansion
07/17/12   aro     Enhanced support of configure feedback path
07/16/12   aro     Added support for dynamic memory allocation for IQ
                   capture buffers
07/09/12   aro     Moved XPT responses to XPT module
07/06/12   aro     F3 message update
07/06/12   aro     Corrected bad error checking og Max number of XPT override
06/26/12   aro     F3 message update for uniformity
06/26/12   aro     Deleted unused parameter from XPT configure function
06/26/12   aro     Deleted IQ Capture init function
06/26/12   aro     Renamed function to FB configure
06/26/12   aro     Updated variable to flag Expansion at the start of IQ capture
06/25/12   aro     Error checking for max XPT override count
06/25/12   aro     Created XPT calibration module in Data structure
06/25/12   aro     Created helper functions to initialize Cal data
06/25/12   aro     Error checking for max IQ capture number while performing
                   IQ capture
06/25/12   aro     Doxygen blocks
06/25/12   aro     Added code to re-initialize XPT variables
06/25/12   aro     Added code to load XPT config param from NV
06/25/12   aro     Fixed bug where RxLM buffer was not getting updated
06/25/12   aro     Added error checking in XPT override
06/14/12   aro     Added check in IQ Cpature function to prevent sending IQ
                   capture command when last process is still pending
06/13/12   aro     Fixed single instance IQ Capture
06/13/12   aro     Implemented DPD coefficient interface
06/13/12   aro     Update XPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from XPT payloads
06/11/12   aro     Added MDSP path/device to XPT configure function
06/11/12   aro     Function ordering
06/11/12   aro     Removed redundant XPT configure function
06/08/12   aro     Updated XPT IQ capture interface to pass device

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rfc_cdma.h"
#include "rf_cdma_utils_freq.h"
#include "rf_cdma_nv.h"
#include "rf_1x_mdsp.h"
#include "rfm_1x_ftm.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_intelliceiver.h"
#include "rfcommon_msg.h"
#include "rfm_internal.h"
#include "rf_cdma_ept.h"
#include "rf_cdma_utils.h"
#include "modem_mem.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_time_profile.h"
#include "rf_1x_tx_agc.h"
#include "rfcommon_mdsp.h"
#include "rf_1x_ept.h"
#include "rf_cdma_mc.h"
#include "rf_1x_mc.h"
#include "rflm_c2k_mc.h"
#include "ftm_calibration_data_module.h"
#include "rfcommon_mc.h"
#include "rfdevice_class.h" /* rfdevice_id_get() */

/*! These two constants are used by XPT capture configuration for the purpose of self-test */
/*! Whenever capture type of EVM or VSWR is requested these values take effect instead of  */
/*! coming from DPD CONFIG NV */
#define FTM_1X_SELFTEST_SEARCH_CENTER 36
#define FTM_1X_SELFTEST_SEARCH_WIDTH 15


/*============================================================================*/
/*!
  @name Feedback Path Configuration

  @brief
  This section contains functions to be used for feedback path configuration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize XPT Capture
 
  @details
  This function will configure the digital Rx front end to be capable
  of doing XPT IQ Capture
 
  @param device_fb
  RFM Device on which XPT IQ Capture is to be done
 
  @param rxlm_fb_handle
  RxLM handle pertaining to teh device for which the IQ capture is to be done
 
  @param band
  RFM CDMA band for which the Feeback path is to be configured
 
  @param band
  RFM CDMA channel for which the Feeback path is to be configured
 
  @return
  Flag indicating the status of the XPT capture initialition
*/
boolean
rf_1x_xpt_configure_fb_rxlm
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  lm_handle_type rxlm_fb_handle,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
)
{
  rfm_mode_enum_type curr_mode;
  rf_cdma_msm_static_rxlm_cfg_type static_cfg;
  rf_cdma_msm_dynamic_rxlm_cfg_type dynamic_cfg;
  boolean ret_val;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rf_cdma_data_status_type *dev_status_w;/* Dev Status Read Ptr*/
  rfm_cdma_power_mode_type power_mode;
  rfdevice_id_enum_type device_id;

  /* Get Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device_fb );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device_fb );

  /* Perform NULL Pointer check */
  if ( ( dev_status_w == NULL ) || ( logical_dev == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_xpt_configure_fb_rxlm: Dev[ x %d ] NULL "
              "data", device_fb );
    return FALSE;
  } /* if ( dev_status_w == NULL ) */

  /* Get current Mode */
  curr_mode = rfm_get_current_mode( device_fb );

  /* Current Mode needs to be 1x */
  if ( curr_mode != RFM_1X_MODE )
  {
    RF_MSG_2( RF_ERROR, "rf_1x_xpt_configure_fb_rxlm: Dev[ x %d ] "
              "Bad Mode %d", device_fb, curr_mode );
    return FALSE;
  } /* if ( curr_mode != RFM_1X_MODE ) */
  
  /* Udate RxLM Handle */
  dev_status_w->rxlm_handle = rxlm_fb_handle;

  /* Update Static Configuration */
  static_cfg.adc_clock_freq = rf_cdma_msm_get_adc_samp_freq( rxlm_fb_handle, 
                                                             band, chan );
  static_cfg.bw_khz = RF_CDMA_BW_1X;
  static_cfg.config_as_et = TRUE;

  /* Update Dynamic Configuration - No Dynamic Update */
  memset(&dynamic_cfg, 0x0, sizeof(rf_cdma_msm_dynamic_rxlm_cfg_type));
  dynamic_cfg.apply_dynamic_cfg = FALSE;
  
  /* check if the feedback device has a valid rxlm chain, if it does not
  then we cannot use it for configuring the feedback chain, as an alternative
  use the primary defive for configuring the rxlm chain for feedback.*/
  if ( logical_dev->ftm_rxlm_chain_onex < RXLM_CHAIN_MAX )
  {
  dynamic_cfg.modem_chain = rfc_cdma_get_rx_modem_chain ( device_fb , band ) ;
    device_id = rfdevice_id_get(logical_dev->rx_device[band]);
    static_cfg.rxlm_adc = logical_dev->rxlm_adc;
   /* Get curretn power mode */
   power_mode = rf_cdma_ic_get_current_power_mode(
                                    &(dev_status_w->intelliceiver) );
   ret_val = TRUE;
  }
  else
  {
   /* no diversity device, use primary instead to configure fb rxlm*/
    rf_cdma_data_status_type *dev_status_w_prx;/* Dev Status Read Ptr*/
    const rfc_cdma_logical_device_params_type *logical_dev_prx;
    /* Get Device Status */
    dev_status_w_prx = rf_cdma_get_mutable_device_status( device );
    /* Get Logical Device Param */
    logical_dev_prx = rfc_cdma_get_logical_device_params( device );

    /* Perform NULL Pointer check */
    if ( ( dev_status_w_prx == NULL ) || ( logical_dev_prx == NULL ) )
    {
      RF_MSG_1( RF_ERROR, "rf_1x_xpt_configure_fb_rxlm: Dev[ x %d ] NULL "
                "data", device );
      ret_val =  FALSE;
    } /* if ( dev_status_w == NULL ) */
    else
    {
      /* No seperate modem_chain for fbrx .. lets use 1 */
      dynamic_cfg.modem_chain = 1;
      device_id = rfdevice_id_get(logical_dev_prx->rx_device[band]);
      static_cfg.rxlm_adc = logical_dev_prx->rxlm_adc;
  /* Get curretn power mode */
  power_mode = rf_cdma_ic_get_current_power_mode(
                                        &(dev_status_w_prx->intelliceiver) );
      ret_val = TRUE;
    }
  }/* if ( logical_dev->ftm_rxlm_chain_onex < RXLM_CHAIN_MAX )*/

  if ( ret_val == TRUE )
  {
  /* Configure RxLM */
    ret_val = rf_cdma_msm_configure_rxlm( device_id,
  				rxlm_fb_handle, RFM_1X_MODE,
 			        band, power_mode, static_cfg, dynamic_cfg );
  }

  RF_MSG_3( RF_MED, "rf_1x_xpt_configure_fb_rxlm : FB-Dev[ %d ] "
            "[Et Modem Chain %d][Status %d]", 
            device_fb, dynamic_cfg.modem_chain , ret_val ) ;

  return ret_val;

} /* rf_1x_xpt_configure_fb_rxlm */

/*! @} */

/*============================================================================*/
/*!
  @name XPT Calibration

  @brief
  This section contains functions for XPT calibration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configuration for XPT calibration

  @details
  XPT configuration. Message to indicate that new dpdDataInfo and/or dpdData
  are avl on shared memory

  @param device
  Primary device for which XPT is to be configured
 
  @param device_fb
  Feedback device for which XPT is to be configured
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
rf_1x_xpt_cal_configure
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb
)
{
  boolean ret_val = TRUE;
  rf_cdma_ept_data_type *ept_data; /* XPT Data */

  /* This number should come from FW */
  #define MAX_NUM_OF_IQ_CAP 20

  /* Get XPT Data */
  ept_data = rf_cdma_get_ept_data();

  /*Perform NULL pointer check*/
  if ( ept_data  == NULL )
  {
    RF_MSG_2( RF_ERROR, "rf_1x_xpt_cal_configure: Dev[ %d %d ] NULL XPT data",
              device, device_fb );
    return FALSE;
  } /* if( ept_data  == NULL ) */

  /* Initialize XPT Data */
  ept_data->cal_data.count_iq_capture = 0;
  
  RF_MSG_3( RF_HIGH, "rf_1x_xpt_cal_configure: Dev[ %d %d ] [Status %d]", 
            device, device_fb, ret_val ); 

  return ret_val;

} /* rf_1x_xpt_cal_configure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Trigger IQ Capture on the FW

  @details
  This function will call RF-MDSP function to trigger the IQ Capture
  for XPT for 1x.
 
  @param device
  Primary device for which IQ capture is to be done

  @param device_fb
  Feedback device for which IQ capture is to be done

  @param capture_param
  Variable to store IQ capture parameter
  
  @param samp_rate
  Pointer return to hold sampling rate of capture

  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
rf_1x_xpt_iq_capture
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  rfm_1x_iq_capture_param_type capture_param,
  uint32 * samp_rate
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status read Pointer*/
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device*/
  const rfc_cdma_logical_device_params_type *logical_dev_fb; /* Logical Device*/
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;
  lm_handle_type rxlm_handle_for_cap; 

  rf_cdma_ept_data_type *ept_data; /* XPT Data */
  boolean ret_val = TRUE;

  /* Get Current Device Status for the given Device, logical device param,
  and XPT CDMA data */
  dev_status_r = rf_cdma_get_device_status( device_fb );
  logical_dev = rfc_cdma_get_logical_device_params( device );
  logical_dev_fb = rfc_cdma_get_logical_device_params( device_fb );
  ept_data = rf_cdma_get_ept_data();

  /*Perform NULL pointer check*/
  if ( ( logical_dev  == NULL ) || ( logical_dev_fb  == NULL ) ||
       ( dev_status_r  == NULL ) || ( ept_data  == NULL ) )
  {
    RF_MSG_2( RF_ERROR, "rf_1x_xpt_iq_capture: Dev[ %d %d ] NULL Data",
              device, device_fb );
    return FALSE;
  } /* if( logical_dev  == NULL ) */

  /* Validate Parameter : If it is first trigger, then the IQ capture count
  has to be zero */
  if ( capture_param.first_trig == TRUE )
  {
    if ( ept_data->cal_data.count_iq_capture != 0 )
    {
      RF_MSG_3( RF_ERROR, "rf_1x_xpt_iq_capture: Dev[ %d %d ] IQ Capture "
                "nonZero(%d) for FirstTrigger", device, device_fb,
                ept_data->cal_data.count_iq_capture );
      return FALSE;
    } /* if ( ept_data->cal_data.count_iq_capture != 0 ) */

  } /* if ( first_trig == TRUE ) */

  /* Update the IQ Capture counter */
  ept_data->cal_data.count_iq_capture++;

  if (capture_param.capture_type == XPT_CAPTURE_FBRX)
  {
    rxlm_handle_for_cap =  dev_status_r->fbrx_lm_handle;
  }
  else
  {
    rxlm_handle_for_cap =  dev_status_r->rxlm_handle;
  }

  if ( capture_param.first_trig==TRUE ) 
  {
    /* set up common capture config params in SMEM, only if first trig */
    ret_val = rfcommon_mdsp_setup_capture_params( (uint8)logical_dev->tx_mdsp_path,
                                                  (uint8)logical_dev->rx_mdsp_path,
                                                  (uint8)rxlm_handle_for_cap,
                                                  (uint8)capture_param.capture_type, 
                                                  (boolean)(0x1 & (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_RX_COMPACT)), 
                                                  capture_param.num_samples,
                                                  capture_param.split_samp, // no of samples before split
                                                  capture_param.do_processing
                                                  ); 
    if ( capture_param.do_processing > PROC_NONE ) 
    {
  /* set up common DPD config params in SMEM, only if first trig */
      if ( (capture_param.capture_type == XPT_CAPTURE_SELF_TEST_EVM) ||
           (capture_param.capture_type == XPT_CAPTURE_SELF_TEST_VSWR)  )
      {
          /* set up common DPD config params in SMEM for new EVM search window, only if first trig */
            ret_val = rfcommon_mdsp_setup_dpd_proc_params(
                           (int16)FTM_1X_SELFTEST_SEARCH_CENTER,
                           (uint8)FTM_1X_SELFTEST_SEARCH_WIDTH,
                           (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_FIRST_BIN),
                           (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_EXTRAP_BIN),
                           (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_MIN_BIN_COUNT),
                           (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_MIN_FIRST_BIN_COUNT),
                           (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_MIN_LAST_BIN_COUNT),
                           (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_SPEC_INV),
                             capture_param.do_processing
                          );
          RF_MSG_1(RF_HIGH, "rf_1x_ept.c: For Selftest EVM/VSWR type, set search center = %d",
                    FTM_1X_SELFTEST_SEARCH_CENTER);
      } else
      {
    ret_val = rfcommon_mdsp_setup_dpd_proc_params(
       (int16)rf_cdma_xpt_get_cfg_param(device, EPT_CFG_CMN_SEARCH_CENTER),
       (uint8)rf_cdma_xpt_get_cfg_param(device, EPT_CFG_CMN_SEARCH_WIDTH),
       (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_FIRST_BIN),
       (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_EXTRAP_BIN),
       (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_MIN_BIN_COUNT),
       (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_MIN_FIRST_BIN_COUNT),
       (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_MIN_LAST_BIN_COUNT),
       (uint8)rf_cdma_xpt_get_cfg_param(device, DPD_CFG_SPEC_INV),
         capture_param.do_processing
      );
      }
    } /* if  ( do_processing == 1 )  */
  } /* if (  first_trig==TRUE )  */

  /* Get pointer to current IQ capture data in order to update delay cap samp rate and  type */
  rfcommon_xpt_fetch_iq_capture_data(&iq_cap_data);
  if (samp_rate != NULL)
  {
    *samp_rate = rf_cdma_mc_get_samp_rate((uint8)rxlm_handle_for_cap);
    iq_cap_data.samp_rate = *samp_rate;
  }
  else
  {
    iq_cap_data.samp_rate = 10000000;
    RF_MSG(RF_MED,"rf_1x_xpt_iq_capture: Samp Rate not queried, ET delay value returned in 1/1000th of samples");
  }

  iq_cap_data.delay_capture_type = RF_COMMON_XPT_DELAY_CAP_INVALID;
  iq_cap_data.capture_type = capture_param.capture_type;
  /*Initialize Pin IQ capture tyoe to INVALID*/
  iq_cap_data.pin_capture_type = RF_COMMON_XPT_PIN_CAP_INVALID;

  /* only update the delay map for correct capture type */
  if ((rfcommon_xpt_capture_type)capture_param.capture_type == XPT_CAPTURE_DELAY)
  {
    iq_cap_data.delay_capture_type = RF_COMMON_XPT_DELAY_CAP_NONLTE;
    RF_MSG_4(RF_MED,"rf_1x_xpt_iq_capture: Push to delay map - Chan: %d, delayCapType: %d, CaptBuf: %d, ResBuf: %d",
                                        iq_cap_data.channel,
                                        iq_cap_data.delay_capture_type,
                                        capture_param.buffer_id.capt_buf_id,
                                        capture_param.buffer_id.res_buf_id);
    ret_val &= ftm_cal_data_push_delay_data_to_delay_map(
                     iq_cap_data.channel,
                     iq_cap_data.delay_capture_type,
                     capture_param.buffer_id.capt_buf_id,
                     capture_param.buffer_id.res_buf_id);

  }

  /*Update Pin IQ capture data into Pin map structure*/
  if ((rfcommon_xpt_capture_type)capture_param.capture_type == XPT_CAPTURE_PIN)
  {
    /*Set Pin IQ capture type*/
    iq_cap_data.pin_capture_type = RF_COMMON_XPT_PIN_CAP_ALL_TECHS;
    ret_val &= ftm_cal_data_push_pin_data_to_pin_map(
                     iq_cap_data.channel,
                     capture_param.buffer_id.capt_buf_id,
                     capture_param.buffer_id.res_buf_id);
    RF_MSG_4(RF_MED,"rf_1x_xpt_iq_capture: Push to pin map - Chan: %d, pinCapType: %d, CaptBuf: %d, ResBuf: %d",
                                        iq_cap_data.channel,
                                        iq_cap_data.pin_capture_type,
                     capture_param.buffer_id.capt_buf_id,
                     capture_param.buffer_id.res_buf_id);

  }


  /* Store the IQ Caprure Data */
  ret_val &= rfcommon_xpt_store_iq_capture_data( &iq_cap_data );

  /* Enable XPT Capture */
  ret_val &= rf_1x_mdsp_xpt_capture( capture_param.first_trig,
                                     capture_param.last_trig,
                                     capture_param.buffer_id.capt_buf_id,
                                     capture_param.buffer_id.res_buf_id,
                                     NULL, 
                                     NULL,
                                     NULL );

  /* Send Debug Message */
  RF_MSG_4( RF_MED, "rf_1x_xpt_iq_capture: [Status %d] Dev[ %d %d ] "
            "IQnum [%d]", ret_val, device, device_fb, 
            ept_data->cal_data.count_iq_capture );

  /* return */
  return ret_val;

} /* rf_1x_xpt_iq_capture */

/*! @} */

/*============================================================================*/
/*!
  @name XPT Online

  @brief
  This section contains functions for XPT calibration
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
rf_1x_xpt_configure
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
  const rf_1x_config_type *rf_1x_config_ptr;   /* Pointer to 1x RF NV data */
  const rf_cdma_tx_static_type *tx_cdma_config_ptr;
  /* FW response housekeeping data */
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;
  /* callback params for FW rsp*/ 
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param; 
  /* flag for cal state or not */
  boolean is_cal_mode = FALSE;
  const rf_cdma_data_status_type* dev_status_r ;
  const rfc_cdma_logical_device_params_type *logical_device_ptr = NULL;
  rf_cdma_data_status_type* dev_status_w = NULL;

  /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( device );

  /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "XPT rf_1x_xpt_configure: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /* ensure that NV is valid for given device and band */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( tx_nv_path, band );

  /* NULL Pointer Check */
  if ( NULL == rf_1x_config_ptr )
  {
    RF_MSG_1( RF_ERROR, "XPT rf_1x_xpt_configure: Dev[ %d ] Failed NV",
              device ); 
    return FALSE;
  } /* if ( NULL == rf_1x_config_ptr ) */

  /* Aditional NULL pointer checks */
  if ( NULL == rf_1x_config_ptr->cdma_common_ptr )
  {
    RF_MSG_1( RF_ERROR, "XPT rf_1x_xpt_configure: Dev[ %d ] CDMA Common NV", 
              device ); 
    return FALSE;
  } /* if ( NULL == rf_1x_config_ptr->cdma_common_ptr ) */

  /* Get Pointer to Tx Static table */
  tx_cdma_config_ptr = &(rf_1x_config_ptr->cdma_common_ptr->tx_static);
  
  /* Aditional NULL pointer checks */
  if ( NULL == tx_cdma_config_ptr )
  {
    RF_MSG_1( RF_ERROR, "XPT rf_1x_xpt_configure: Dev[ %d ] CDMA Tx NV",
              device ); 
    return FALSE;
  } /* if ( NULL == tx_cdma_config_ptr ) */

  /* Get Logical Device Param */
  logical_device_ptr = rfc_cdma_get_logical_device_params( device );
  if ( (logical_device_ptr == NULL) ||
       (logical_device_ptr->tx_device[band] == NULL) )
  {
    RF_MSG_1( RF_ERROR, "XPT rf_1x_xpt_configure: Null CDMA logical Tx device.", 
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

  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* NULL pointer check */
  if ( dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR, "XPT rf_1x_xpt_configure: Null CDMA device_status_w ",
		                 device);
    return FALSE;
  }/*if ( dev_status_w == NULL )*/

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
    RF_MSG_2( RF_ERROR, "XPT rf_1x_xpt_configure: Dev[ %d ] CDMA Invalid"
                        "XPT Mode %d", device, xpt_mode ); 
    return FALSE;
  } /* if ! ( xpt_mode == ET_CFG ) */

  xpt_config_data.et_env_delay = 
    rf_cdma_xpt_get_path_delay ( logical_device_ptr->tx_device[band],
                                 &(rf_1x_config_ptr->cdma_common_ptr->tx_cal) ,
                                 band, chan,
                                 ( rfm_get_calibration_state() == FALSE ),
                                 dev_status_r->temp_comp.is_last_reading_valid,
                                 dev_status_r->temp_comp.last_temp_sensor_read ) ;
  xpt_config_data.ept_env_scale_nominal = 
    tx_cdma_config_ptr->ept_cfg_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
  dev_status_w->ept_env_scale_nominal = 
    tx_cdma_config_ptr->ept_cfg_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
  dev_status_w->et_env_scale_nominal = 
    tx_cdma_config_ptr->ept_cfg_params[ET_CFG_CMN_ENV_SCALE_UNITY];
  xpt_config_data.et_env_scale_nominal = 
    tx_cdma_config_ptr->ept_cfg_params[ET_CFG_CMN_ENV_SCALE_UNITY];
  xpt_config_data.prim_txlm_handle = prim_txlm_handle;
  xpt_config_data.prim_tx_chain = 
    rfc_cdma_get_tx_modem_chain( device, band );

  /* XPT Config */
  ret_val &= rf_1x_mdsp_configure_xpt( &xpt_config_data,
                                       is_cal_mode,
                                       &rsp_cb_param,
                                       NULL,
                                       NULL );

  /* Configure ET delay value to RFLM */
  ret_val &= rflm_1x_mc_config_tx_et_delay( prim_txlm_handle,
                                            xpt_config_data.et_env_delay );

  RF_MSG_2( RF_MED , 
            "XPT rf_1x_xpt_configure: Dev[%d] [Status: %d]", device, ret_val );

  return ret_val;

} /* rf_1x_xpt_configure */

/*! @} */

/*============================================================================*/
/*!
  @name ET interface functions

  @brief
  This section contains functions for ET functionality
*/
/*! @{ */

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
rf_1x_xpt_config_done_rsp_cb
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
    RF_MSG( RF_ERROR, "rf_1x_xpt_config_done_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_xpt_config_rsp_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_xpt_config_done_rsp_cb: NULL CB Data" );
    return FALSE;
  } /* if ( cb_data == NULL ) */

   RF_MSG_4( RF_MED, "[1X][FW->RF] [D%d] 1X_XPT_CONFIG RSP 0x%x [ %d | %d ]", 
             cb_data->device,
             snum_data->snum, 
             RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
             RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum) );

   /* Post the Semaphore only if an active semaphore is available */
   if ( cb_data->sema_token != NULL )
   {
     rfcommon_semaphore_post( cb_data->sema_token );
   } /* if ( cb_data->sema_token != NULL ) */
   else
   {
     ret_val = FALSE;
     ERR_FATAL( "rf_1x_xpt_config_done_rsp_cb: NULL Sem Token", 0, 0, 0 );
   } /* if ! ( cb_data->sema_token != NULL ) */

   RF_MSG( RF_LOW, "rf_1x_xpt_config_done_rsp_cb: Handled" );

   return ret_val;

} /* rf_1x_xpt_config_done_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for XPT Transition Complete Reponse received through RF task
 
  @param req_ptr
  Pointer to RF Task command
*/
boolean
rf_1x_xpt_trans_complete_rsp_cb
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
   RF_MSG( RF_ERROR, "rf_1x_xpt_trans_complete_rsp_cb: NULL CB Data" );
   return FALSE;
  } /* if ( cb_data == NULL ) */

  /* Typecast the payload */
  rsp_payload = (cfw_xpt_trans_compl_payload_t*)req_ptr->payload;

  RF_MSG_5( RF_MED, "[1X][FW->RF] [D%d] XPT_TRANSITION_RSP  0x%x [ %d | %d ]"
           " XPT Mode %d",
           cb_data->device,
           snum_data->snum, 
           RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
           RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum),
           rsp_payload->cur_mode );

  /* Post the Semaphore only if an active semaphore is available */
  if ( cb_data->sema_token != NULL )
  {
   rfcommon_semaphore_post( cb_data->sema_token );
  } /* if ( cb_data->sema_token != NULL ) */
  else
  {
   ret_val = FALSE;
   ERR_FATAL( "rf_1x_xpt_trans_complete_rsp_cb: NULL Sem Token", 0, 0, 0 );
  } /* if ! ( cb_data->sema_token  != NULL ) */

  RF_MSG( RF_LOW, "rf_1x_xpt_trans_complete_rsp_cb: Handled" );

  return ret_val;

} /* rf_1x_xpt_trans_complete_rsp_cb */
#endif /* FEATURE_CDMA1X */
/*! @} */

