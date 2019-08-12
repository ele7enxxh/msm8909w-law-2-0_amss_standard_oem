/*!
  @file
  rf_hdr_rx_agc.c

  @details
  This file defines interfaces pertaining to HDR RxAGC configuration.
 
  @addtogroup RF_CDMA_HDR_RXAGC
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/src/rf_hdr_rx_agc.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/06/14   spa     Change rx_nv_path to be per band per logical device
04/11/14   spa     Move configure JD thresh API to cdma rxagc 
02/07/14   cc      Wrong script_index was used in rf_1x_update_rx_agc and 
                   rf_hdr_update_rx_agc
02/03/14   cc      Fix rf_cdma_update_agc crash when it's called
10/09/13   spa     Configure RxAGC data before sending RxAGC config mssg 
10/04/13   SAR     Fixed compiler warnings.
09/25/13   APU     Removed unused SBI specific code.
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/27/13   cd      Add missing clean-up of Rx AGC LNA SM events after configure
07/08/13   spa     Check for result of rf_hdr_mdsp_resume_rx_agc API
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
06/19/13   APU     Fixed warning given by the previous checkin.
06/18/13   APU     Removed redundant cleaning of CCS events. 
06/07/13   spa     Added band as arg to jammer detect thresh API 
06/04/13   spa     Off target compile error fix
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation 
05/28/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/17/13   sty     delete redundant init of rx_agc_param from 
                   rf_hdr_configure_rx_agc
05/09/13   spa     Free track indicator semaphore if sending of RxAGC config 
                   message fails 
04/13/13   spa     Added is_irat_mode flag to configure_rx_agc
04/05/13   spa     Use RF dispatch for HDR Rxagc tracking
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/29/13   sty     updated script_index to have correct value in 
                   rf_hdr_update_rx_agc() - value of -1 is invalid for dime
03/27/13   ry      Added support for WBAGC JD algorithm
02/26/13   spa     Updated the check for max carriers in configure RxAGC
02/21/13   spa     Fix KW error, check for max carriers in configure RxAGC API
01/12/13   cd      RxAGC LNA SM event configuration support
01/04/13   sty     KW fix to use correct band type
12/27/12   shb     Updates for different physical rx_device per band
12/06/12   APU     Replaced LM chains with Modem chains.
12/04/12   aro     Added HDR support for RF-FW synchronization
09/27/12   zhw     Remove bring up hack for LNA switch points
09/20/12   sty     Deleted using fake_NVs - use NV read from qcn
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
08/08/12   sty     Use canned lna_sw pts for RUMI
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
07/05/12   zhw     Tuned fake QCN when RUMI debug flag is enbled
06/18/12   sty     deleted use of RFHDR_RXF_BBF_GAIN_COMP_ADJ_VAL - obsolete 
06/14/12   cd      Added separate functionality to compute LNA switchpoints, 
                   in order to support MCDO OLTA swpt computation
06/06/12   cd      Remove reference to legacy unused NV tables
05/30/12   sa      Fixed klocwork error 
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
05/10/12   zw      Renamed rtr_handle to script_index 
04/30/12   hdz     Added rf_cdma_resume_rx_agc_cb() 
04/11/12   Saul    QFE. Initial support.
04/03/12   aro     callback function for HDR MDSP Rx interfaces
03/29/12   shb     Added rf_state arg to rf_hdr_configure_rx_agc as it is needed 
                   by device lna phase offset query api 
03/28/12   aro     Merged the ic_update and rx_agc_resume interfaces
03/21/12   cd      Display Rx switchpoints when Rx AGC is configured
03/19/12   Saul    MCDO. Rx AGC freq comp and device droop compensation support.
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan  
03/02/12   Saul    MCDO. Rx AGC freq comp and device droop compensation framework. 
02/29/12   Saul    MCDO. WTR HDR RF Cal data adjust.
02/15/12   spa     Returned value of rf_hdr_mdsp_configure_rx_agc updates 
                   ret_val in rf_hdr_configure_rx_agc
02/08/12   hdz     Cleaned up function
02/06/12   hdz     Added rf_hdr_update_rx_agc() and rf_hdr_helper_update_rx_agc
02/01/12   Saul    MCDO. Corrected Rx AGC gain-offset calculations.
01/25/12   Saul    MCDO. Changed RX AGC gain-offset calculations.
01/10/12   Saul    MCDO. RTR now taking EVDO mode. 
                   Allow operation in multi-carrier mode and bandwidths.
01/09/12   Saul    MCDO. Added iqmc_coefs and cal_adj_for_hdr
01/04/12   aro     Code to populate LNA phase offset in HW units
01/04/12   aro     Added interface to query LNA phase offsets for current
                   configuration
12/29/11   aro     Interface to query RxLM chain for the given RxLM buffer
12/29/11   aro     Updated configure RxAGC interface to have RxLM buffer
11/30/11   sty     Added band_chan info as arg in rf_hdr_configure_rx_agc()
08/03/11   sty     Added rf_cdma_check_rise_fall_values(0 call in 
                   rf_hdr_configure_rx_agc()
07/26/11   cd      Rx AGC is configured with new Rx Switchpoint NV values
07/25/11   cd      Null pointer handling for NV get API
07/18/11   aro     Updated Configure RxAGC interface to pass only required
                   param to simplify intelliceiver interface
07/18/11   aro     Documentation Update
06/21/11   Saul    Updates for multi-carrier driver.
06/15/11   Saul    Hard-coded RTR carrier mode to single-carrier.
06/09/11   Saul    Invalidate unused chans in config rx agc
06/09/11   Saul    Updates for multi-carrier driver.
06/08/11   shb     Updated RFDEVICE_GET_COMM_INFO to provide RX SSBI Bus
06/07/11   Saul    Updates for multi-carrier driver.
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/06/11   sty     Added bypass and non-bypass timer info in RxAGC config
06/02/11   sty     Deleted bringup flag read_nv_for_hdr_agc_config
                   Added SSBI bus info
06/02/11   aro     Added code to convert LNA threshold and gain to 1/640th dB
05/24/11   aro     Added Logical Device Mapping to MDSP Chain / NV for SV
05/14/11   sty     fixed bad merge in previous check-in
05/10/11   cd      Removed rf_cdma_nv_get_freq_bin()
05/10/11   sty     Hardcoded values for LNA rise and fall for bringup 
05/10/11   sty     Added bringup flags to skip HDR AGC config from NV
04/21/11   aro     [1] Moved ADC Clock Manager to HAL
                   [2] Moved RxAGC implementation to RFM
04/19/11   cd      Added Rx AGC programming with interpolated values for gain 
                   states G1,G2,G3,G4
04/15/11   sty     Bypass NV access for RUMI bringup
04/11/11   aro     Removed unused RxAGC deactivate interface
03/24/11   aro     Unified the RxAGC Read Interface
03/24/11   aro     Updated HDR RxAGC Configure interface
03/24/11   aro     Fixed the Bug where cumulative RxAGC was not reported
03/22/11   bmg     Switched G0 programming to CDMA config data structure
03/18/11   aro     Added HDR RXF BBF Gain Adjustment
03/17/11   aro     Added Code to move HDR AGC to tracking Mode
03/14/11   aro     Renamed RxAGC structure
03/14/11   aro     Merged the DVGA and LNA offset to Gain Calibration Data
03/14/11   aro     Compiler Warning Fix
03/11/11   aro     Added DVGA offset write
03/09/11   aro     Renamed Config RxAGC function name
03/03/11   aro     Added HDR RxAGC deactivate function
03/02/11   aro     Re-structured the RxAGC Param type
02/24/11   aro     Renamed type name to rf_cdma_nv_band_items_type
02/01/11   aro     Promoted Power Mode types to RFM
01/31/11   aro     Doxygen Documentation
01/23/11   aro     Added Support for HDR RxAGC Query
01/19/11   aro     Added LNA Config and RxAGC activate interfaces
01/19/11   aro     Renamed RF-MDSP filename
01/18/11   aro     Added Configure HDR RxAGC function
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/23/10   aro     Doxygen update
12/23/10   aro     Removed Path to replace with device
12/09/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_hdr_rx_agc.h"
#include "rf_hdr_mdsp.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "rf_cdma_msm.h"
#include "rfcommon_msg.h"
#include "rf_1x_calibration.h"
#include "rfm_internal.h"
#include "rfc_cdma.h"
#include "rfdevice_cdma_interface.h"
#include "modem_mem.h"
#include "rfcommon_math.h"
#include "rf_cdma_data.h"
#include "rf_cdma_intelliceiver.h"
#include "rf_cdma_mc.h"
#include "rf_hdr_mc.h"

/*============================================================================*/
/*!
  @name RxAGC Configure

  @brief
  Functions for the HDR to configure HDR RxAGC
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure HDR RxAGC LNA switchpoints for both SCDO and MCDO operation

  @details
  This function will configure HDR RxAGC LNA switchpoint information based on
  band-chan information. Switchpoints are computed if they are of type OLTA,
  using calibrated offsets for the center frequency. And are checked for
  monotonicity after computation.
  
  @param device
  RF Device on which switchpoints are to be configured
 
  @param num_carriers
  Number of carriers to indicate SC vs MC DO
 
  @param band
  RF band for which RxAGC is to be configured
 
  @param center_chan
  The actual channel the LO is to be tuned to 
 
  @param bw_khz
  The bandwidth of the DO forward link carriers
 
  @param power_mode
  Power Mode on which RxAGC is to be configured
 
  @param rx_agc_param
  The Rx AGC param data structure into which switchpoints
  must be populated
 
  @return
  Status of execution of function
*/
static
boolean
rf_hdr_rx_agc_compute_swpts
(
  rfm_device_enum_type device,
  uint32 num_carriers,
  rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type center_chan,
  uint32 bw_khz,
  rfm_cdma_power_mode_type power_mode,
  rf_cdma_mdsp_rx_agc_config_type *rx_agc_param
)
{
  boolean ret_val = TRUE; /* final result of API */

  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rf_cdma_carrier_mode_type carrier_mode;/* Logical Device */
  const rf_hdr_config_type* rf_hdr_config_ptr; /* Pointer to HDR NV config */
  const rf_cdma_lna_info_type* lna_info_ptr; /* Ptr to mode specific LNA info */
  rfm_lna_gain_state_type gain_state; /* Iterator for gain states */
  rfm_lna_gain_step_type gain_step; /* Iterator for LNA gain steps */
  int16 gain_offset_val; /* Computed gain offset value per state */
  /* Adjusted RF Cal data based on DO carrier bandwidth */
  rfdevice_hdr_adjust_rf_cal_data_type hdr_rfcal_adj_data;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* null pointer check */
  if ( logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_hdr_rx_agc_compute_swpts: Invalid logical device "
                        "for device %d ", device );
    return FALSE;
  }

  /* Get HDR NV configuration data */
  rf_hdr_config_ptr = 
    rf_cdma_nv_get_hdr_data( logical_dev->rx_nv_path[band], band );

  /* null pointer check */
  if ( rf_hdr_config_ptr == NULL )
  {
    RF_MSG_2( RF_ERROR,
              "rf_hdr_rx_agc_compute_swpts: "
              "HDR NV configuration not allocated for Path %d Band %d",
              logical_dev->rx_nv_path[band],
              band);
    return FALSE;
  }

  /* If more than 1 channel... */
  if(num_carriers > 1)
  {
    /* Use multi-carrier settings */
    carrier_mode = RF_CDMA_CARRIER_MODE_MC;
  }
  else
  {
    /* default to single carrier */
    carrier_mode = RF_CDMA_CARRIER_MODE_SC;
  }

  /* Obtain a pointer to the correct LNA information based on
  current HDR carrier mode */ 
  switch ( carrier_mode )
  {
  case RF_CDMA_CARRIER_MODE_SC:
    lna_info_ptr = &rf_hdr_config_ptr->rx_static.lna_info_dor0_a;
    break;

  case RF_CDMA_CARRIER_MODE_MC:
    lna_info_ptr = &rf_hdr_config_ptr->rx_static.lna_info_dorb_mc;
    break;
  }/* switch ( carrier_mode ) */

  /* Obtain cal value from NV for all gain states after interpolation
  for the LO tuned channel */
  for ( gain_state = RFM_LNA_GAIN_STATE_0; 
        gain_state < RFM_LNA_GAIN_STATE_NUM;
        gain_state++ )
  {
    hdr_rfcal_adj_data.lna_offset_adj[gain_state] = 
    rf_cdma_nv_get_gain_cal( logical_dev->rx_nv_path[band],
                             band,
                             center_chan, power_mode,
                             gain_state );
  }

  /* Adjust cal values for HDR. */
  hdr_rfcal_adj_data.bw_khz = bw_khz;
  hdr_rfcal_adj_data.is_valid = FALSE;
  rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[band],
                                 RFDEVICE_ADJUST_RF_CAL_FOR_HDR,
                                 &hdr_rfcal_adj_data);
  if (hdr_rfcal_adj_data.is_valid == FALSE)
  {
    RF_MSG( RF_ERROR,"rf_hdr_rx_agc_compute_swpts: "
                     "HDR RF Cal Adjust data is invalid.");
    ret_val = FALSE;
  }

  if ( ret_val == TRUE )
  {
    /* Compute the rise and fall switchpoints for each gain step
    corresponding to the respective gain state. OLTA switchpoints require the
    calibrated gain offset value, which is provided after interpolation */
    for ( gain_state = RFM_LNA_GAIN_STATE_0; 
          gain_state < RFM_LNA_GAIN_STATE_NUM;
          gain_state++ )
    {
      /* Obtain the gain offset val for this gain state in 1/10th dB */
      gain_offset_val = hdr_rfcal_adj_data.lna_offset_adj[gain_state];
  
      /* Use gain state to compute the gain step value, for switchpoint 
      configuration */
      gain_step = (rfm_lna_gain_step_type)gain_state;
      if ( gain_step < RFM_LNA_GAIN_STEP_NUM )
      {
        /* HDR RxAGC LNA Rise Switchpoint Configuration in dBm10 */
        rx_agc_param->rise_val[gain_step] =
          rf_cdma_lna_get_rise_swpt( lna_info_ptr,
                                     power_mode,
                                     gain_step,
                                     gain_offset_val );
        /* Populate post-rise timer hysteresis value */
        rx_agc_param->bypass_timer_val[gain_step] =
          rf_cdma_lna_get_post_rise_hold( lna_info_ptr,
                                          power_mode,
                                          gain_step );
  
        /* HDR RxAGC LNA Fall Switchpoint Configuration in dBm10 */
        rx_agc_param->fall_val[gain_step] =
          rf_cdma_lna_get_fall_swpt( lna_info_ptr,
                                     power_mode,
                                     gain_step,
                                     gain_offset_val );
        /* Populate post-fall timer hysteresis value */
        rx_agc_param->nonbypass_timer_val[gain_step] =
          rf_cdma_lna_get_post_fall_hold( lna_info_ptr,
                                          power_mode,
                                          gain_step );
      } /* if ( gain_step < RFM_LNA_GAIN_STEP_NUM ) */
    } /*for ( gain_state... ) */

    /* Print out Rx switchpoints before loading to FW */
    RF_MSG_1( RF_LOW, "rf_hdr_rx_agc_compute_swpts: LO Tuned Chan %d",
              center_chan );

    RF_MSG_8( RF_MED, "rf_hdr_rx_agc_compute_swpts: "
              "LNA SP "
              "[F1 %d, R1 %d, F2 %d, R2 %d, F3 %d, R3 %d, F4 %d, R4 %d] dBm10",
              rx_agc_param->fall_val[RFM_LNA_GAIN_STEP_0_TO_1],
              rx_agc_param->rise_val[RFM_LNA_GAIN_STEP_0_TO_1],
              rx_agc_param->fall_val[RFM_LNA_GAIN_STEP_1_TO_2],
              rx_agc_param->rise_val[RFM_LNA_GAIN_STEP_1_TO_2],
              rx_agc_param->fall_val[RFM_LNA_GAIN_STEP_2_TO_3],
              rx_agc_param->rise_val[RFM_LNA_GAIN_STEP_2_TO_3],
              rx_agc_param->fall_val[RFM_LNA_GAIN_STEP_3_TO_4],
              rx_agc_param->rise_val[RFM_LNA_GAIN_STEP_3_TO_4] );

    /* check if rise-fall values satisfy the conditions neeeded by FW */
    ret_val &= rf_cdma_check_rise_fall_values ( rx_agc_param->rise_val, 
                                                rx_agc_param->fall_val );
  } /* if ( ret_val == TRUE ) */

  if(ret_val == FALSE)
  {
    RF_MSG_4( RF_ERROR, "rf_hdr_rx_agc_compute_swpts: "
                        "FAILED for device %d band %d "
                        "num_carriers %d power_mode %d", 
              device, band, num_carriers, power_mode );
  }

  return ret_val;

} /* rf_hdr_rx_agc_compute_swpts */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure HDR RxAGC

  @details
  This function will configure HDR RxAGC which includes SBI information, LNA 
  Rise, Fall, Offset, and Bypass/Non-bypass Timer values
  
  @param device
  RF Device on which AGC is to be configured
 
  @param rxlm_handle
  RxLM handle for the device for which RxAGC is to be configured
 
  @param num_carriers
  Number of carrier for which RxAGC is to be configured
 
  @param band
  RF band for which RxAGC is to be configured
 
  @param chan_array
  Array of RF channels for which RxAGC is to be configured
 
  @param tune_lo_to_band
  The actual band the LO is to be tuned to 
 
  @param tune_lo_to_chan
  The actual channel the LO is to be tuned to 
 
  @param acq_mode_data
  Structure containing the HDR RxAGC Mode information

  @param power_mode
  Power Mode on which RxAGC is to be configured
 
  @param rf_state
  CDMA state of receive device. Needed to get LNA Phase offsets as they can
  be different for Diversity vs SHDR mode
 
  @param is_irat_mode
  Boolean flag to check if the API has been called in IRAT mode or in online 
  mode, the difference being that in online mode, we wait for the RxGC Track 
  Indicator response from FW, to ensure that RxAGC has converged wheras in IRAT
  mode RF does not need to wait for the response.
 
  @param carrier_index_array
  Contains carrier to narrow-band mappings
 
  @return
  Status of execution of function
*/
boolean
rf_hdr_configure_rx_agc
(
  rfm_device_enum_type device,
  lm_handle_type rxlm_handle,
  uint32 num_carriers,
  rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const chan_array,
  const rfm_cdma_chan_type tune_lo_to_chan,
  const rfm_hdr_agc_acq_type* const acq_mode_data,
  rfm_cdma_power_mode_type power_mode,
  rf_cdma_state_type rf_state,
  boolean is_irat_mode,
  const uint16 * const carrier_index_array
)
{
  boolean ret_val = TRUE; /* final result of API */
  /* Configure SBI for RxAGC */
  rf_cdma_mdsp_rx_agc_config_type rx_agc_param; /* Container for LNA data */
  rfm_lna_gain_state_type gain_state; /* Iterator for gain states */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rf_cdma_carrier_mode_type carrier_mode;/* Logical Device */
  rfm_cdma_chan_type chans[RFM_CDMA_CARRIER_NUM];/* Chans for which 
                                             RxAGC is to be configured */
  int32 freq_offsets[RFM_CDMA_CARRIER_NUM]; /* Contains frequency offsets */
  rfm_cdma_carrier_en_mask_type carrier_mask; /* Contains active carriers mask */

  uint32 bw_khz;/* The bandwidth in use. */
  uint8 carrier_idx; /* Variable for carrier indexing */   
  int16 gain_offset_val;
  rfdevice_hdr_adjust_rf_cal_data_type hdr_rfcal_adj_data;
  rfdevice_cdma_multi_carrier_droop_compensations_type droop_compensations;
  int16 rx_device_comp; /* Contains compensation value from Rx device */
  int16 cal_comp_val; /* Contains calibration compensation value */
  uint16 carrier_loc; /* The actual narrow-band location for the carrier */
  uint16 channel; /* Contains the channel */
  uint32* freq_offsets_droop; /* frequency offsets related to droop */
  int16* compensations_droop; /* compensations related to droop */
  int32 freq_offset; /* Contains frequency offset */
  uint16 index; /* For general purpose indexing */
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

  rfdevice_cdma_lna_phase_offset_type lna_phase;
  /*--------------------------------------------------------------------------*/

  /* Initialize structure.*/
  memset(&lna_phase,0,sizeof(rfdevice_cdma_lna_phase_offset_type));

  /* Check for number of carriers to be less than the maximum*/
  if ( num_carriers > RFM_CDMA_CARRIER_NUM)
  {
    RF_MSG_3( RF_ERROR, "rf_hdr_configure_rx_agc: Number of carriers cannot "
                        "exceed %d for device :%d, num carriers is:%d",
                      RFM_CDMA_CARRIER_NUM, device, num_carriers ); 
    return FALSE;
  }

    /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( device );

  /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "rf_hdr_configure_rx_agc: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* null pointer check */
  if ( logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_hdr_configure_rx_agc: Invalid logical device "
                        "for device %d ", device );
    return FALSE;
  }

  /* Get Rx Device Droop Compensation Values */
  droop_compensations.num_items = 0;
  rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[band],
                                 RFDEVICE_MULTI_CARRIER_DROOP_COMPENSATION,
                                 &droop_compensations);

  /* There must be at least 1 element provided, even if its values are zero */
  if( (droop_compensations.num_items == 0 ) || 
      (droop_compensations.compensations_ptr == NULL))
  {
    RF_MSG( RF_ERROR, "rf_hdr_configure_rx_agc: Must provide Droop Compensations. "
                      "None provided." );
    return FALSE;
  }

  /* Allocation and check memory */
  freq_offsets_droop = (uint32 *) 
                  modem_mem_alloc( (sizeof(uint32) * droop_compensations.num_items ),
                                   MODEM_MEM_CLIENT_RFA );
  if( freq_offsets_droop == NULL )
  {
    RF_MSG( RF_ERROR, "rf_hdr_configure_rx_agc: Could not allocate memory." );
    ret_val = FALSE;
  }

  /* Allocation and check memory */
  compensations_droop = (int16 *) 
                  modem_mem_alloc( (sizeof(int16) * droop_compensations.num_items ),
                                   MODEM_MEM_CLIENT_RFA );
  if( compensations_droop == NULL )
  {
    RF_MSG( RF_ERROR, "rf_hdr_configure_rx_agc: Could not allocate memory." );
    ret_val = FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /* all checks done and all checks passed -- can safely proceed now  */ 
  /*--------------------------------------------------------------------------*/ 

  /* Init rx_agc_param to all zeros*/
  memset( &(rx_agc_param), 0, sizeof(rf_cdma_mdsp_rx_agc_config_type) );

  /* Do work only if all is well so far */
  if( ret_val == TRUE )
  {
    for(carrier_idx = 0; carrier_idx < num_carriers; ++carrier_idx)
    {
      chans[carrier_idx] = chan_array[carrier_idx];
    }
  
    for( carrier_idx = carrier_idx; carrier_idx < RFM_CDMA_CARRIER_NUM; 
         ++carrier_idx)
    {
      chans[carrier_idx] = RF_CDMA_INVALID_CHAN;
    }
  
    rf_cdma_compute_carrier_info( band, chan_array, num_carriers, carrier_index_array, 
                                  &carrier_mask, freq_offsets, 0 );
    
    /* If more than 1 channel... */
    if(num_carriers > 1)
    {
      /* Use multi-carrier settings */
      carrier_mode = RF_CDMA_CARRIER_MODE_MC;
    }
    else
    {
      /* default to single carrier */
      carrier_mode = RF_CDMA_CARRIER_MODE_SC;
    }
  
    bw_khz = rf_cdma_compute_bandwidth( num_carriers, band, chans );
    
    /* Configure WB Index */
    rx_agc_param.wideband_index =  
      (uint8) rfc_cdma_get_rx_modem_chain ( device , band ) ;
  
    /* Obtain a pointer to the correct LNA information based on
    current HDR carrier mode */ 
    switch ( carrier_mode )
    {
    case RF_CDMA_CARRIER_MODE_SC:
      break;
  
    case RF_CDMA_CARRIER_MODE_MC:
      {
        /* Populate arrays for interpolation */
        for( index = 0; index < droop_compensations.num_items; ++index )
        {
          freq_offsets_droop[index] = droop_compensations.compensations_ptr[index].freq;
          compensations_droop[index] = droop_compensations.compensations_ptr[index].compensation;
        }
      }
    break;
    }/* switch ( carrier_mode ) */

    /* Get LNA Phase offsets from the device for geiven band and channel */
    rfdevice_cdma_get_lna_phase_offsets( logical_dev->rx_device[band], 
                                         band,
                                         tune_lo_to_chan, power_mode, 
                                         rf_state, bw_khz, &lna_phase );
  
    for( carrier_idx = 0; carrier_idx < num_carriers; ++carrier_idx )
    {
      carrier_loc = carrier_index_array[ carrier_idx ];
      channel = chan_array[ carrier_idx ];
      freq_offset = freq_offsets[ carrier_loc ];
  
    /* Obtain calibration compensation value */
      cal_comp_val = rf_cdma_get_calibration_compensation( device,
                                                           band,
                                                           chans[carrier_idx] );
  
    /* ONLY WHEN in MCDO mode ... */
    if( carrier_mode == RF_CDMA_CARRIER_MODE_MC )
    {
      /* Obtain Rx device's interpolated compensation based on frequency offset */
        rx_device_comp = rf_lininterp_u32_s16( freq_offsets_droop, 
                                               compensations_droop,
                                               droop_compensations.num_items, 
                                               abs( freq_offset ) );
    }
    else
    {
      rx_device_comp = 0;
    }
       
    /* Obtain cal value from NV for all gain states after interpolation */
      for ( gain_state = RFM_LNA_GAIN_STATE_0; 
            gain_state < RFM_LNA_GAIN_STATE_NUM; gain_state++ )
    {
      hdr_rfcal_adj_data.lna_offset_adj[gain_state] = 
        rf_cdma_nv_get_gain_cal( logical_dev->rx_nv_path[band],
                                 band,
                                 channel, power_mode,
                                 gain_state );
    }
    /* Adjust cal values for HDR. */
    hdr_rfcal_adj_data.bw_khz = bw_khz;
    hdr_rfcal_adj_data.is_valid = FALSE;
    rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[band],
                                   RFDEVICE_ADJUST_RF_CAL_FOR_HDR,
                                   &hdr_rfcal_adj_data);
    if(hdr_rfcal_adj_data.is_valid == FALSE)
    {
      RF_MSG( RF_ERROR,
              "rf_hdr_configure_rx_agc: HDR RF Cal Adjust data is invalid." );
      ret_val = FALSE;
      break;
    }
  
    /* Convert cal values for all gain states to 1/640th dB unit.
    Compute the LNA phase offset in modem HW units. */
    for ( gain_state = RFM_LNA_GAIN_STATE_0; gain_state <RFM_LNA_GAIN_STATE_NUM;
          gain_state++ )
    {
      /* Populate the LNA phase offset in HW units */
      rx_agc_param.lna_phase_offset[gain_state] = 
            rf_cdma_msm_convert_lna_phase(lna_phase.offset_in_deg[gain_state]);
   
      /* Obtain the gain offset val for this gain state in 1/10th dB */
      gain_offset_val = hdr_rfcal_adj_data.lna_offset_adj[gain_state];
  
      /* ONLY WHEN in MCDO mode ... */
      if( carrier_mode == RF_CDMA_CARRIER_MODE_MC )
      {
        /* If in MCDO ... */
        /* Account for Rx device compensation */
        gain_offset_val += rx_device_comp;
      } /* if( carrier_mode == RF_CDMA_CARRIER_MODE_MC ) */

      /* Convert to 1/640th dB */
        rx_agc_param.calibrated_gain_val[carrier_loc][gain_state] = 
                                                    gain_offset_val * 64;

      /* Add back the calibration compensation value to DVGA offset value */
        rx_agc_param.calibrated_gain_val[carrier_loc][gain_state] += 
                                                    cal_comp_val;
      } /*for ( gain_state... ) */
    } /* for (each carrier...)*/

    /* Compute the rise and fall switchpoints for each gain step
    corresponding to the respective gain state. OLTA switchpoints require the
    calibrated gain offset value, which is provided after interpolation.
    We use the tune_lo_to_chan Channel to compute switchpoints for both
    SCDO and MCDO */
    ret_val &= rf_hdr_rx_agc_compute_swpts( device, 
                                            num_carriers,
                                            band,
                                            tune_lo_to_chan,
                                            bw_khz,
                                            power_mode,
                                            &rx_agc_param );


    /* If all conditions are still good */
    if ( ret_val == TRUE )
    {
          /* Configure RxAGC data for HDR */
          ret_val = rf_cdma_mc_configure_rx_agc_data( RFM_1XEVDO_MODE, device, 
                                                      band, rxlm_handle, 
                                                      carrier_mode, bw_khz, 
                                                      power_mode,&rx_agc_param );

      /* Program AGC params to firmware */
        /* If in IRAT mode, RF does not wait for RxAGC track indicator 
           response. Therefore we set the response callback param to NULL
           Because the configure RxAGC API checks for this callback to be 
           non-NULL , if non-NULL it registers to wait for this response
           otherwise it doesn't */ 
        if ( is_irat_mode == TRUE )
        {
          RF_MSG( RF_MED, "rf_hdr_configure_rx_agc:Not registering for "
                          "RxAGC track indicator RSP in IRAT mode");
          rsp_cb_param.rsp_handler = NULL;
          rsp_cb_param.handler_data = NULL;
          ret_val &= rf_hdr_mdsp_configure_rx_agc( logical_dev->rx_mdsp_path, 
                                                 carrier_mode,
                                                 &rx_agc_param, acq_mode_data, 
                                                 NULL, NULL, NULL );
        }/*if ( is_irat_mode == TRUE )*/
        else
        {
          /* RF is in Online mode, register with the appropriate callback */
          fwrsp_hk->rxagc_trk_ind.device = device;
          fwrsp_hk->rxagc_trk_ind.sema_token = rfcommon_semaphore_pop_item();
          rsp_cb_param.rsp_handler = rf_hdr_rxagc_track_ind_cb;
          rsp_cb_param.handler_data = &fwrsp_hk->rxagc_trk_ind;

        /* Send message to FW */
          ret_val &= rf_hdr_mdsp_configure_rx_agc( logical_dev->rx_mdsp_path, 
                                                 carrier_mode,
                                                 &rx_agc_param, acq_mode_data, 
                                                 &rsp_cb_param, NULL, NULL );

        /* Check if sending of message was success/failure */
        if ( ret_val == FALSE )
      {
          /* Message sending failed, free the semaphore */
          RF_MSG_1( RF_HIGH, "rf_hdr_configure_rx_agc: Send HDR RxAGC mssg"
                    " failed for device %d , freeing the semapohore" 
                    " for track indication response", device);
         rfcommon_semaphore_push_item( fwrsp_hk->rxagc_trk_ind.sema_token );

        }/*if ( ret_val == FALSE )*/

      }/*if ( is_irat_mode != TRUE )*/

      } /* if ( ret_val == TRUE ) */
    else
    {
      RF_MSG( RF_ERROR, "rf_hdr_configure_rx_agc: Invalid values for "
                        "rise/fall switch-points ");
      ret_val = FALSE;
    }
  } /* if( ret_val == TRUE ) */

  /* Free allocated memory */
  if( freq_offsets_droop != NULL )
  {
    modem_mem_free( freq_offsets_droop, MODEM_MEM_CLIENT_RFA );
  }
  if( compensations_droop != NULL )
  {
    modem_mem_free( compensations_droop, MODEM_MEM_CLIENT_RFA );
  }

  if(ret_val == FALSE)
  {
    RF_MSG_4( RF_ERROR, "rf_hdr_configure_rx_agc: FAILED for device %d band %d "
              "num_carriers %d power_mode %d", 
              device, band, num_carriers, power_mode );
  }

  return ret_val;

} /* rf_hdr_configure_rx_agc */

/*============================================================================*/
  
/*! @} */

/*============================================================================*/
/*!
  @name RxAGC Control

  @brief
  Functions for the HDR to control HDR RxAGC operation
*/
/*! @{ */

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
void
rf_hdr_rx_agc_freeze
(
  rfm_device_enum_type device
)
{
  return;
} /* rf_hdr_rx_agc_freeze */

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
void
rf_hdr_rx_agc_unfreeze
(
  rfm_device_enum_type device
)
{
  return;
} /* rf_hdr_rx_agc_unfreeze */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update hdr RxAGC for a given device

  @details
  This API update hdr RxAGC using the following MDSP msg:
    - stop rx agc
    - configure rx agc
    - resume rx agc
 
  @param device
  RF Device of operation
 
  @return
  TRUE if reloading is successfull; FALSE otherwise.
*/
boolean
rf_hdr_update_rx_agc
(
  rfm_device_enum_type device
)
{
  const rf_cdma_data_status_type *dev_status = NULL;
  const rfc_cdma_logical_device_params_type *logical_dev;
  boolean status = TRUE;
  boolean ret_val = TRUE;
  rfm_hdr_agc_acq_type agc_mode_info; /* hdr RxAGC Mode info */
  rf_cdma_mdsp_ic_update_type agc_cfg;
  /* FW response callback param*/
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
  /* FW response housekeeping data */
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

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
  
  /* Stop RxAGC */
  rf_hdr_mdsp_stop_rx_agc( logical_dev->rx_mdsp_path, NULL, NULL, NULL );

  /* As FW stops RxAGC after 52uS of sending STOP Rx response, wait
    for that duration */
  DALSYS_BusyWait(866);

  /* Indicate the AGC Mode as Resume Normal to specify that Accum init
    should not be latched and used from the value saved by FW side. */
  agc_mode_info.mode = RFM_HDR_AGC_MODE_RESUME_WITH_CURR;
  /*agc_mode_info.acq_duration = RF_CDMA_IC_1X_ACQ_DURATION_US;*/

  /* Configure hdr RxAGC */
  ret_val &= rf_hdr_configure_rx_agc( device,
                                      dev_status->rxlm_handle,
                                      dev_status->num_carriers,
                                      dev_status->curr_band,
                                      dev_status->curr_chans,
                                      dev_status->lo_tuned_to_chan,
                                      &agc_mode_info,
                                      dev_status->intelliceiver.power_mode,
                                      dev_status->data_hdr.rf_state,
                                      FALSE,/* is_irat_mode */
                                      dev_status->carrier_index
                                    );

  /* Send up_rx_agc message only if RX_AGC_CFG is successfull */
  if ( ret_val == TRUE )
  {
    agc_cfg.script_index = -1;
    agc_cfg.rxlm_handle = dev_status->rxlm_handle;
    agc_cfg.settling_time = 1024;

    /* Get FW Response Data */
    fwrsp_hk = rf_cdma_get_fwrsp_data( device );

      /* NULL Pointer Check */
      if ( fwrsp_hk == NULL )
      {
        RF_MSG( RF_ERROR, "rf_hdr_update_rx_agc: NULL FWRSP Data" );
        status = FALSE;
      } /* if ( fwrsp_data == NULL ) */
      else
      {
        fwrsp_hk->ic_update_rsp.device = device;
        fwrsp_hk->ic_update_rsp.sema_token = rfcommon_semaphore_pop_item();
        fwrsp_hk->ic_update_rsp.blocking_call = TRUE;
        rsp_cb_param.rsp_handler = rf_cdma_ic_update_rsp_cb;
        rsp_cb_param.handler_data = &fwrsp_hk->ic_update_rsp;

        status = rf_hdr_mdsp_resume_rx_agc( logical_dev->rx_mdsp_path, &agc_cfg, 
                                   &rsp_cb_param, NULL, NULL  );
      }
      rf_hdr_mc_rx_agc_convergence( device );

  }/*if ( ret_val == TRUE )*/
  else
  {
    /* This has landed in unrecoverable situation as RxAGC is stopped and
      IC_UPDATE could not done because of failure in RX_AGC_CFG */
    RF_MSG( RF_ERROR, "rf_hdr_update_rx_agc: update RxAGC failed");
  
    status = FALSE;
  }
  
  /* Unconditionally clean-up the Rx AGC LNA SM events here, after 
  Rx AGC configuration is completed */
  status &= rf_cdma_mc_cleanup_rx_agc_lna_ccs_events( device );
    
  return status;
}
#endif /* FEATURE_CDMA1X */

/*! @} */

/*! @} */

