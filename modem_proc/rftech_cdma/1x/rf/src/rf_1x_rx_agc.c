/*!
  @file
  rf_1x_rx_agc.c

  @details
  This file defines interfaces pertaining to 1x RxAGC configuration.
 
  @addtogroup RF_CDMA_1X_RXAGC
  @{ 

*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rf_1x_rx_agc.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/06/14   spa     Change rx_nv_path to be per band per logical device
02/07/14   cc      Wrong script_index was used in rf_1x_update_rx_agc and 
                   rf_hdr_update_rx_agc 
10/09/13   spa     Add rf mode param to configure RxAGC data 
10/04/13   SAR     Fixed compiler warnings.
09/25/13   APU     Removed unused SBI specific code.
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/27/13   cd      Add missing clean-up of Rx AGC LNA SM events after configure
07/08/13   spa     Check for result of rf_1x_mdsp_resume_rx_agc API
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
06/04/13   cd      Moved clean-up of Rx AGC LNA SM events to MC
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation 
05/17/13   sty     Init rx_agc_param to 0's in rf_1x_configure_rx_agc()
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/29/13   sty     updated script_index to have correct value in 
                   rf_1x_update_rx_agc() - value of -1 is invalid for dime
01/12/13   cd      RxAGC LNA SM event configuration support
12/27/12   shb     Updates for different physical rx_device per band
12/06/12   APU     Replaced LM chains with Modem chains.
12/04/12   aro     F3 message update
09/28/12   aro     Migrated 1x MC to use new RF-FW Sync
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/21/12   cd      Avoid rx gain cal computation if in cal state 
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
06/06/12   cd      Remove reference to legacy unused NV tables
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
05/10/12   zw      Renamed rtr_handle to script_index 
04/30/12   hdz     Added rf_cdma_resume_rx_agc_cb()
04/11/12   Saul    QFE. Initial support.
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
04/02/12   aro     Added CB as argument in Rx MDSP interfaces
03/29/12   shb     Added rf_state arg to rf_1x_configure_rx_agc as it is needed 
                   by device lna phase offset query api 
03/28/12   aro     Merged the ic_update and rx_agc_resume interfaces
03/28/12   aro     Added callback function as a parameter of RxAGC resume
                   function
03/19/12   Saul    MCDO. Rx AGC freq comp and device droop compensation support.
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan  
03/02/12   Saul    MCDO. Rx AGC freq comp and device droop compensation framework.
02/15/12   spa     Returned value of rf_1x_mdsp_configure_rx_agc updates ret_val 
                   in rf_1x_configure_rx_agc  
02/08/12   hdz     Fixed compiler warning
02/08/12   hdz     Cleaned up function
02/06/12   hdz     Added rf_1x_update_rx_agc for reloading rx agc in calls 
01/04/12   aro     Code to populate LNA phase offset in HW units
01/04/12   aro     Added interface to query LNA phase offsets for current
                   configuration
12/29/11   aro     Interface to query RxLM chain for the given RxLM buffer
12/29/11   aro     Updated configure RxAGC interface to have RxLM buffer
08/11/11   sar     Fixed KW errors.
08/04/11   aro     Removed debug flag to override PM for no iC case
08/03/11   sty     Added rf_cdma_check_rise_fall_values() in 
                   rf_1x_configure_rx_agc()
07/26/11   cd      Rx AGC is configured with new Rx Switchpoint NV values
07/25/11   cd      Null pointer handling for NV get APIs
07/18/11   aro     Updated RxAGC Config interface to pass AGC Mode Param
06/17/11   cd      Retrieve Rx switchpoint data from new NV data structures. 
                   Interface added, but data populated to mDSP is still from old
                   NV tables until new Rx switchpoint NV items are functional.
06/10/11   Saul    Updated CDMA BW defines.
06/08/11   shb     Updated RFDEVICE_GET_COMM_INFO to provide RX SSBI Bus
06/07/11   aro     Fixed device Path
06/07/11   Saul    Updates for multi-carrier driver.
06/07/11   aro     Replaced SV debug flag with Device check (temp)
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/02/11   aro     Change to temporarily use Chain 0 LNA SP for all chains
06/02/11   aro     Removed debug flag
06/02/11   aro     [1] Populated Bypass and NonBypass timer
                   [2] Added debug flag to overrride PM for NV query
06/02/11   aro     Added code to convert LNA threshold and gain to 1/640th dB
05/24/11   aro     Updated interface to send WB index info to MDSP
05/25/11   aro     Added debug code to enable SV bringup
05/24/11   aro     Added Logical Device Mapping to MDSP Chain for SV support
05/16/11   aro     Added change to convert LNA Threshold to 1/640th dB
05/13/11   aro     Filled in RxAGC parameters
05/11/11   sty     Added flags to skip NV read during Rx AGC config
05/10/11   cd      Removed rf_cdma_nv_get_freq_bin()
05/11/11   sty     Added flags to skip NV read during Rx AGC config
05/05/11   adk     Fixed function pointer mismatch and other compiler warnings.
04/21/11   aro     Moved Get Calibration compensation function to Common
04/17/11   sty     Bypass NV operations in rf_1x_configure_rx_agc()
04/15/11   cd      - Rx AGC is now configured with interpolated values for all 
                     LNA gain states
                   - Code clean up
04/14/11   aro     Temporary Crash Fix
03/22/11   bmg     Switched G0 programming to CDMA config data structure
03/16/11   aro     Added Rx SBI Bus Param in Configure RxAGC
03/16/11   aro     Changed Device Param to MDSP Chain
03/14/11   aro     Renamed RxAGC structure
03/14/11   aro     [1] Removed activate RxAGC interface
                   [2] Merged the DVGA and LNA offset to Gain Calibration Data
03/02/11   aro     Re-structured the RxAGC Param type
02/25/11   aro     Added support for DVGA offset
02/24/11   aro     Renamed type name to rf_cdma_nv_band_items_type
02/03/11   aro     Moved new Device interface to separate file
02/01/11   aro     Promoted Power Mode types to RFM
01/23/11   aro     Updated RxAGC config Interface
01/19/11   aro     Renamed RF-MDSP filename
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
01/04/11   aro     Added RxAGC Config Load and Activate functions
12/29/10   aro     Removed HWIO to replace with RF-MDSP Call
12/29/10   aro     Doxygen update
12/29/10   aro     Updated rf_1x_rx_agc_configure() to config LNA
12/29/10   aro     Updated rf_1x_rx_agc_configure() functionality
12/23/10   aro     Doxygen update
12/23/10   aro     [1] Added syub to configrue RxAGC
                   [2] Removed Path to replace with device
12/09/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_1x_rx_agc.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "rf_cdma_lna.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_msm.h"
#include "rf_1x_calibration.h"
#include "rfc_card.h"
#include "rfm_internal.h"
#include "rfcommon_msg.h"
#include "rfdevice_cdma_interface.h"
#include "rf_cdma_data.h"
#include "rf_cdma_intelliceiver.h"
#include "rf_cdma_mc.h"

/*============================================================================*/
/*!
  @name RxAGC Configure

  @brief
  Functions for the 1x to configure HDR RxAGC
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure 1x RxAGC

  @details
  This function will configure 1x RxAGC which includes SBI information, LNA 
  Rise, Fall, Offset, and Bypass/Non-bypass Timer values
  
  @param device
  RF Device on which AGC is to be configured
 
  @param rxlm_handle
  RxLM handle for the device for which RxAGC is to be configured
 
  @param band
  Band for which RxAGC is to be configured
 
  @param chan
  Channel for which RxAGC is to be configured

  @param power_mode
  Power Mode on which RxAGC is to be configured
 
  @param rf_state
  CDMA state of receive device. Needed to get LNA Phase offsets as they can
  be different for Diversity vs SHDR mode
 
  @param acq_mode_data
  Structure containing the 1x RxAGC Mode information
 
  @return
  Status of execution of function
*/
boolean
rf_1x_configure_rx_agc
(
  rfm_device_enum_type device,
  lm_handle_type rxlm_handle,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  rfm_cdma_power_mode_type power_mode,
  rf_cdma_state_type rf_state,
  const rf_1x_agc_param_type* const acq_mode_data
)
{
  boolean ret_val; /* final result of API */
  /* Configure SBI for RxAGC */
  rf_cdma_mdsp_rx_agc_config_type rx_agc_param; /* Container for LNA data */
  rfm_lna_gain_state_type gain_state; /* Iterator for gain states */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_1x_config_type* rf_1x_config_ptr; /* Pointer to 1x NV config */
  rfm_lna_gain_step_type gain_step; /* Iterator for LNA gain steps */
  int16 gain_offset_val;
  rfdevice_cdma_lna_phase_offset_type lna_phase;

  /* Initialize structure.*/
  memset(&lna_phase,0,sizeof(rfdevice_cdma_lna_phase_offset_type));

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR,"rf_1x_configure_rx_agc: Null pointer returned to logical_dev ");
    return FALSE;
  }

  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG( RF_ERROR,"rf_1x_configure_rx_agc: invalid rx_mdsp_path ");
    return FALSE;
  }

  /* Get 1x NV configuration data */
  rf_1x_config_ptr = 
    rf_cdma_nv_get_1x_data( logical_dev->rx_nv_path[band], 
                            band );

  if ( rf_1x_config_ptr == NULL )
  {
    RF_MSG_2( RF_ERROR,
              "rf_1x_configure_rx_agc: 1x NV configuration not allocated for "
              "Path %d Band %d",
              logical_dev->rx_nv_path[band],
              band );
    return FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /* all checks passed - safe to proceed                                      */
  /*--------------------------------------------------------------------------*/
  /* Init rx_agc_param to all zeros*/
  memset( &(rx_agc_param), 0, sizeof(rf_cdma_mdsp_rx_agc_config_type) );

  /* Configure WB Index */
  rx_agc_param.wideband_index = (uint8)
                                 rfc_cdma_get_rx_modem_chain ( device , band ) ;

  /* Get LNA Phase offsets from the device for geiven band and channel */
  rfdevice_cdma_get_lna_phase_offsets( logical_dev->rx_device[band],
                                       band,
                                       chan,
                                       power_mode, 
                                       rf_state,
                                       RF_CDMA_BW_1X,
                                       &lna_phase );

  /* Obtain cal value from NV for all gain states after interpolation, and
  convert to 1/640th dB unit.
 
  Also, compute the rise and fall switchpoints for each gain step
  corresponding to the respective gain state. OLTA switchpoints require the
  calibrated gain offset value, which is provided after interpolation
 
  Compute the LNA phase offset in modem HW units. */
  for ( gain_state = RFM_LNA_GAIN_STATE_0;
        gain_state < RFM_LNA_GAIN_STATE_NUM;
        gain_state++ )
  {
    /* Populate the LNA phase offset in HW units */
    rx_agc_param.lna_phase_offset[gain_state] = 
          rf_cdma_msm_convert_lna_phase(lna_phase.offset_in_deg[gain_state]);

    /* Get gain cal only if we're not in calibration mode */
    if ( rfm_get_calibration_state() == FALSE )
    {
      /* Obtain the gain offset val for this gain state in 1/10th dB */
      gain_offset_val =
         rf_cdma_nv_get_gain_cal( logical_dev->rx_nv_path[band],
                 band,
                 chan, power_mode, gain_state );
    }
    else
    {
      gain_offset_val = 0;
    }

    /* Convert to 1/640th dB */
    rx_agc_param.calibrated_gain_val[RFM_CDMA_CARRIER_0][gain_state] = gain_offset_val * 64;

    /* Add back the calibration compensation value to gain offset value */

    rx_agc_param.calibrated_gain_val[RFM_CDMA_CARRIER_0][gain_state] +=
       rf_cdma_get_calibration_compensation( device, band, chan );

    /* Use gain state to compute the gain step value, for switchpoint 
    configuration */
    gain_step = (rfm_lna_gain_step_type)gain_state;
    if ( gain_step < RFM_LNA_GAIN_STEP_NUM )
    {
      /* 1x RxAGC LNA Rise Switchpoint Configuration */
      rx_agc_param.rise_val[gain_step] =
        rf_cdma_lna_get_rise_swpt( &rf_1x_config_ptr->rx_static.lna_info,
                                   power_mode,
                                   gain_step,
                                   gain_offset_val
                                 );
      /* Populate post-rise timer hysteresis value */
      rx_agc_param.bypass_timer_val[gain_step] =
        rf_cdma_lna_get_post_rise_hold( &rf_1x_config_ptr->rx_static.lna_info,
                                        power_mode,
                                        gain_step
                                      );

      /* 1x RxAGC LNA Fall Switchpoint Configuration */
      rx_agc_param.fall_val[gain_step] =
        rf_cdma_lna_get_fall_swpt( &rf_1x_config_ptr->rx_static.lna_info,
                                   power_mode,
                                   gain_step,
                                   gain_offset_val
                                 );
      /* Populate post-fall timer hysteresis value */
      rx_agc_param.nonbypass_timer_val[gain_step] =
        rf_cdma_lna_get_post_fall_hold( &rf_1x_config_ptr->rx_static.lna_info,
                                        power_mode,
                                        gain_step
                                      );
    } /* if ( gain_step < RFM_LNA_GAIN_STEP_NUM ) */
  } /*for ( gain_state = RFM_LNA_GAIN_STATE_0;
        gain_state < RFM_LNA_GAIN_STATE_NUM;
        gain_state++ ) */

  /* check if rise-fall values satisfy the conditions neeeded by FW */
  if ( rf_cdma_check_rise_fall_values ( rx_agc_param.rise_val, 
                                        rx_agc_param.fall_val ) == TRUE )
  {

      /* Configure RxAGC data for 1x */
      ret_val = rf_cdma_mc_configure_rx_agc_data( RFM_1X_MODE, device, 
                                                  band, rxlm_handle, 
                                                  RF_CDMA_CARRIER_MODE_SC, 
                                                  RF_CDMA_BW_1X, 
                                                  power_mode, &rx_agc_param);

    if ( ret_val == TRUE )
    {
    /* Program AGC params to firmware */
    ret_val = rf_1x_mdsp_configure_rx_agc( logical_dev->rx_mdsp_path, 
                                           &rx_agc_param, acq_mode_data, NULL,
                                           NULL, NULL );
  }
  else
  {
      RF_MSG( RF_ERROR, "rf_1x_configure_rx_agc: Failed in RxAGC data config");
    }

  }
  else
  {
    RF_MSG( RF_ERROR, "rf_1x_configure_rx_agc: Invalid values for "
                      "rise/fall switch-points ");
    ret_val = FALSE;
  }

  if( ret_val == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_1x_configure_rx_agc: API FAILED.");
  }

  return ret_val;

} /* rf_1x_configure_rx_agc */

/*! @} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function halts AGC module in order to deploy operations such as power
  mode change.

  @details
  It follows a certain desired sequence to freeze both LNA state and AGC
  value, which will not change until the corresponding unfreezing function
  gets called.
  
  @param device
  RF Device on which AGC is to be Frozen
*/
void rf_1x_rx_agc_freeze
(
  rfm_device_enum_type device
)
{
  return;
} /* rf_1x_rx_agc_freeze */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function resumes the AGC module for the normal operation from 
  the previous freezing state.

  @details
  It follows a certain desired sequence to restore the previous LNA state and
  AGC value, which were freezed by the earlier AGC freezing function.
  
  @param device
  RF Device on which AGC is to be unfrozen
*/
void rf_1x_rx_agc_unfreeze
(
  rfm_device_enum_type device
)
{
  return;
} /* rf_1x_rx_agc_unfreeze */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is the interface to reload 1x rx agc

  @details
  This API will reload 1x rx agc using the following MDSP msg in order:
    - Stop rx agc
    - Configure rx agc
    - Resume rx agc
  
  @param device
  RF Device on which AGC is to be unfrozen
 
  @return
  TRUE if reloading is successful; FALSE otherwise
*/
boolean
rf_1x_update_rx_agc
(
  rfm_device_enum_type device
)
{
  const rf_cdma_data_status_type *dev_status = NULL;
  const rfc_cdma_logical_device_params_type *logical_dev;
  boolean status = TRUE;
  boolean ret_val = TRUE;
  rf_1x_agc_param_type agc_mode_info; /* 1x RxAGC Mode info */
  rfm_cdma_band_class_type band; /* Band  */
  rfm_cdma_chan_type chan;
  rf_cdma_mdsp_ic_update_type agc_cfg;
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

  /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( device );

  /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_update_rx_agc: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /* Get device Status */
  dev_status = rf_cdma_get_device_status( device );
  
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL || dev_status == NULL )
  {
    /* If this physical device can't be translated to a logical device,
        then no band is supported. */
    RF_MSG_1( RF_ERROR, "rf_1x_update_rx_agc: Invalid Dev = %d",
              device );
  
    return FALSE;
  }
  
  /* Populate Band and Channel */
  band = dev_status->curr_band;
  chan = dev_status->curr_chans[RFM_CDMA_CARRIER_0];

  /* Stop RxAGC */
  rf_1x_mdsp_stop_rx_agc( logical_dev->rx_mdsp_path, NULL, NULL, NULL );

  /* As FW stops RxAGC after 52uS of sending STOP Rx response, wait
    for that duration */
  DALSYS_BusyWait(52);

  /* Indicate the AGC Mode as Resume Normal to specify that Accum init
    should not be latched and used from the value saved by FW side. */
  agc_mode_info.mode = RF_1X_AGC_MODE_RESUME_WITH_CURR;
  agc_mode_info.acq_duration = RF_CDMA_IC_1X_ACQ_DURATION_US;

  /* Configure 1x RxAGC */
  ret_val &= rf_1x_configure_rx_agc( device,
                                     dev_status->rxlm_handle,
                                     band,
                                     chan,
                                     dev_status->intelliceiver.power_mode,
                                     dev_status->data_1x.rf_state,
                                     &agc_mode_info );

  /* Send up_rx_agc message only if RX_AGC_CFG is successfull */
  if ( ret_val == TRUE )
  {
    rf_1x_fw_response_data_type *fwrsp_data = NULL;

    /* Get a pointer to the FW Response Data */
    fwrsp_data = rf_cdma_get_1x_fw_response_data();

    /* If pointer to the task data cannot be achieved, then it is a
    fatal error */
    if ( fwrsp_data == NULL )
    {
      RF_MSG( RF_ERROR, "rf_1x_update_rx_agc: NULL FWRSP Data" );
      status = FALSE;
    } /* if ( fwrsp_data == NULL ) */
    else
    {
      /* Start Rx */
      agc_cfg.script_index = -1;
      agc_cfg.rxlm_handle = dev_status->rxlm_handle;
      agc_cfg.settling_time = 1024;

      fwrsp_hk->ic_update_rsp.device = device;
      fwrsp_hk->ic_update_rsp.sema_token = rfcommon_semaphore_pop_item();
      fwrsp_hk->ic_update_rsp.blocking_call = TRUE;
      rsp_cb_param.rsp_handler = rf_cdma_ic_update_rsp_cb;
      rsp_cb_param.handler_data = &fwrsp_hk->ic_update_rsp;

      status = rf_1x_mdsp_resume_rx_agc( logical_dev->rx_mdsp_path, &agc_cfg, 
                                &rsp_cb_param, NULL, NULL );

    } /* if ! ( fwrsp_data == NULL ) */
  } /* if ( ret_val == TRUE ) */
  else
  {
    /* This has landed in unrecoverable situation as RxAGC is stopped and
      IC_UPDATE could not done because of failure in RX_AGC_CFG */
    RF_MSG( RF_ERROR, "rf_1x_update_rx_agc: update RxAGC failed");
  
    status = FALSE;
  } /* if ! ( ret_val == TRUE ) */
  
  /* Unconditionally clean-up the Rx AGC LNA SM events here, after 
  Rx AGC configuration is completed */
  status &= rf_cdma_mc_cleanup_rx_agc_lna_ccs_events( device );
    
  return status;
}
#endif /* FEATURE_CDMA1X */
/*! @} */
