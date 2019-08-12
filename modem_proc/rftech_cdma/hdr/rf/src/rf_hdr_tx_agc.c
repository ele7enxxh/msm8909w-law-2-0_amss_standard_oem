/*!
  @file
  rf_hdr_tx_agc.c

  @details
  This file defines interfaces pertaining to HDR TxAGC configuration.
 
  @addtogroup RF_CDMA_HDR_TXAGC
  @{ 

*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/src/rf_hdr_tx_agc.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/13/16   pkm     Make sure Pin/Pout not applied in temp comp when Autopin is running.
12/20/15   vr      Autopin check-in
01/16/15   vr      CDMA predictive clipping NV based power thresh support
11/20/14   zhw     Dynamic Predictive Clipping support
11/12/14   spa     Change tx_nv_path to be per band per logical device
09/26/14   JJ      Moved fbrx temp comp from temp comp read handler api to config
                   tx agc api
07/21/14   cd      Add predictive clipper support for 1x, DO rev0, DO revA
05/02/14   cd      8-PA state functionality updates
04/16/14   cd      Cleanup unused MDSP interfaces and Tx AGC functionality
04/01/14   APU     PA params update hook-up for different RC/data rate configs.
03/13/14   spa     Add flag for update_xpt_mode in config_tx_agc
03/10/14   sty     KW fix - null pointer check
02/18/14   cd      Provide baseband gain values to RFLM for APT and ET modes
01/17/14   spa     Remove redundant call to rf_cdma_tx_agc_config API
01/08/14   cd      Provide xPT type to RFLM per PA state
01/03/14   hdz     Do rf_cdma_tx_agc_config only temp_comp.init_trigger is TRUE
12/13/13   sty     deleted refs to template_id
11/25/13   cd      Remove flag-based functionality for legacy Tx AGC
11/08/13   cd      Remove support for legacy FW interfaces
10/25/13   cd      Use default for linearizer limit
10/25/13   cd      Linearizer limit updated only if not using FED Tx AGC
10/09/13   cd      Configure common Tx AGC after linearizer loading
10/08/13   cd      Add support for common TxAGC debug flag
10/03/13   cd      Hook up PA switchpoint loading to RFLM
09/27/13   cd      Hook up common load linearizer interface to CDMA
09/26/13   cd      Added functionality to load to RFLM through common TxAGC
08/08/13   Saul    XPT. KW Fixes.
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
06/07/13   APU     Check for _tx_agc_load_linearizer() before writing to FW. 
04/23/13   APU     Removed old unused iqgain_params and incoporated CC comments.
04/19/13   APU     Removed:
                   1>  the older unused _configure_tx_linearizer() function.
                   2>  the older unused _get_iqgain_params() function.
04/17/13   zhw     Support for variable transaction in QPOET/PA TxAGC templates 
04/12/13   cd      Remove max PA state functionality
02/27/13   aro     Mainlined debug flag for Tx resource manager
02/27/13   aro     Interface to determine XPT mode of operation based on MSM
                   and NV
02/26/13   Saul    CDMA. Set EPT switch-point to 42 dBm when ET is enabled
02/25/13   aro     Updated XPT configure interface to pass XPT mode from
                   calling function
02/22/13   Saul    CDMA. Config xpt mode based on dpd info in linearizer.
02/21/13   aro     Updated configure XPT interface to push band and NV chain
                   parameter for calling function
02/21/13   aro     CDMA Tx Resource manager implementation
02/08/13   Saul    CDMA. Set XPT mode with EPT/ET only when such mode is enabled.
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/30/13   zhw     Pass in high/low band info for FW TxAGC logging
01/16/13   Saul    CDMA. Enabled EPT/ET tempcomp.
01/15/13   Saul    CDMA. HDR EPT/ET call support.
12/19/12   cd      Fixes for ET Tx configuration in FTM mode only
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/13/12   Saul    CDMA. TX AGC XPT Dev. HDR
12/11/12   sty     KW fix
12/11/12   Saul    CDMA. TX AGC XPT Dev. Get max PA state based on lin NV.
12/06/12   APU     Replaced LM chains with Modem chains.
12/05/12   spa     Added RF mode param to load linearizer and HDET APIs
11/29/12   zhw     PA API refactor and make pa_script API static
11/12/12   zhw     Load FW mdsp interface with PA Q current RFFE writes
11/02/12   zhw     Remove deprecated pa stm data members
10/31/12   zhw     Compiler warning fix. memset pa_stm_data before use
10/29/12   zhw     Support for query device_config from RFC and device driver
10/29/12   spa     Call update PA params before get tx agc params because the 
                   pa_r_map is needed in get tx agc params to calculate bitmasks 
10/23/12   zhw     Remove hardcoding in get_template_id() 
10/16/12   sty     Removed all refs to FEATURE_RF_HAS_QFE1320
10/07/12   zhw     Error check for result TRUE before sending to FW
10/04/12   spa     Added check for valid SSBI/RFFE PA before generating scripts  
10/02/12   zhw     Support for SSBI / GRFC based PA
09/26/12   zhw     Configure TxAGC with Template ID from MC layer 
09/25/12   zhw     Refactor configure tx agc API, avoid using void pointer
09/24/12   zhw     Fix OFT Compiler error
09/21/12   zhw     Support for PA RFFE Scripting
09/14/12   sty     Code cleanup
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/08/12   cvd     Replace timetick with rfcommon_time_profile interface
08/22/12   cri     EPT support for CDMA FW interfaces
06/29/12   spa     Compiler warning fix  
06/28/12   APU     Changed QFE to the new generic PA interface. 
04/18/12   aro     Added callback function as parameter to override functions
04/11/12   Saul    QFE. Initial support.
04/04/12   aro     Added callback functions to HDR Tx interfaces
03/22/12   sty     Deleted redundant call to rf_cdma_nv_set_pa_range_map_arr()
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/14/12   cd      Update PA fall hysteresis timer from NV
03/08/12   cd      Configure Tx AGC with GTR Thresh information
02/29/12   cd      - Provide support to load DOr0 and DOrA switchpoint NV for 
                   different RRIs
                   - Cleaned up legacy band nv tbl reference and pa backoffs
02/16/12   Saul    MCDO. Tx now using non-APT linearizer. 
02/09/12   Saul    MCDO. Tx AGC uses MCDO switch-points.
02/08/12   hdz     Cleaned up function 
02/06/12   hdz     Added rf_hdr_update_tx_agc for reloading tx agc in calls
01/23/12   sty     Deleted references to rf_cdma_debug_flags.btf 
                   Added nv_path and band to arg-list for rf_hdr_get_tx_agc_params
01/19/12   cd      Use linearizer limit configuration API from PLIM module
01/18/12   aro     Updated interface to query Modem specific TxGAC param to use
                   TxLM chain rather than device
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
01/03/12   aro     Added TxLM Chain specific to 1x and HDR in logical
                   device param
12/07/11   aro     Added interface to query Tx RGI address
12/02/11   cd      Added support to configure Tx AGC for access and traffic with 
                   new switchpoints  
12/01/11   cd      Provide IQ gain params based on default TxLM and NV data 
11/03/11   cd      Update plim data with max linearizer entry that's loaded to 
                   FW 
10/18/11   cd      Removed old linearizer functionality
10/11/11   aro     Added debug code to display PA switch points
10/06/11   aro     Disabled PA Back off for HDR
09/11/11   aro     Merged CDMA device interface to delete
                   rfdevice_1x_interface.h
09/01/11   cd      Add APT support
                   - APT LUTs are loaded to FW when configuring the Tx 
                   linearizer
08/30/11   shb     Removed pwr lim setting from rf_hdr_configure_tx_agc - will 
                   be handled by hdr plim module
08/29/11   aro     Added temporary PA Switchpoint backoff
08/17/11   cd      Fixed KW warnings
08/16/11   cd      Added debug flag for functionality to load FW linearizers 
                   with old tx linearizer data structures or new multi-lin
                   data structures 
07/20/11   sty     Changed arg-list for rf_hdr_configure_tx_agc() - takes in 
                   NV pointers now
07/14/11   bmg     Reduced the chance of integer overflow in
                   rf_hdr_set_tx_power_adjust().
07/07/11   sty     Reworked rf_hdr_configure_tx_agc to return correct API result
                   and bail out of actual configuration on error
07/05/11   bmg     Changed power limit interface to use dBm10.
06/29/11   sty     Read max_pwr_limit from rf_cdma_debug_flags
06/28/11   sty     Changed max_pwr limit 
06/13/11   sty     Renamed rf_cdma_txagc.h to rf_cdma_tx_agc.h
06/13/11   sty     Moved MSM-specific data to HAL
                   Added time-spent in rf_hdr_configure_tx_agc()
06/09/11   cd      Removed PA R map load check. NV functionality provides the 
                   appropriate PA R Map
06/08/11   cd      Deprecated old PA R Map NV loading support
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/06/11   aro     Compiler Error Fix
05/31/11   aro     Added NULL pointer check for Logical Device param
05/24/11   aro     Added Logical Device Mapping to MDSP Chain / NV for SV
05/19/11   sty     Added test hook for BTF
05/18/11   sty     Added grfc info to be passed to FW
05/10/11   cd      - Remove old NV table pointer usage
                   - Removed rf_cdma_utils_get_freq_bin() and 
                     rf_cdma_nv_get_freq_bin()
05/04/11   shb     Pass band to RF device in TX LUT address/data query functions
05/02/11   cd      Added error msg for malloc failure
04/26/11   aro     Removed Enable/Disable Tx interfaces
04/25/11   aro     Compiler error fix
04/25/11   sty     Bypassed NV/GRFC and RTR writes for RUMI build
04/22/11   cd      KW fixes to check mem alloc for tx_lin_data
03/31/11   aro     Re-designed MDSP override Interfaces
03/29/11   sty     Warning fix
03/25/11   sty     Added rf_hdr_set_tx_power_adjust()
                   Use PA rise/fall values as read from NV
03/25/11   sty     Renamed header file from rf_cdma_txlin.h to rf_cdma_txagc.h 
03/24/11   aro     [1] Re-designed Configure TxAGC Interface
                   [2] Deleted unused interfaces
03/24/11   sty     [1] Changes in loading of Tx linearizer - deleted 
                   rfhdr_mdsp_config_lut_temp()
                   [2] Use rf_cdma_load_tx_linearizers() to populate the tables 
                   and call rf_hdr_mdsp_configure_tx_linearizer() to load tables 
                   to FW
03/21/11   sty     Doxygen update
03/21/11   sty     Added time-profiling to rf_hdr_configure_tx_agc
                   Started using NV
                   Added more mdsp config calls
                   Added band_chan as arg in rf_hdr_configure_tx_agc
03/02/11   sty     Renamed rf1xhdr_mdsp_pa_param_type to 
                   rf_cdma_mdsp_pa_param_type
02/23/11   sty     Fixed warnings
02/04/11   sty     Fixed doxygen warnings
02/02/11   sty     Initial Release
==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "modem_mem.h"
#include "rf_hdr_tx_agc.h"
#include "rf_cdma_tx_agc_msm.h"
#include "rf_hdr_mdsp.h"
#include "rfcommon_msg.h"
#include "rfdevice_cdma_interface.h"
#include "rf_cdma_tx_agc.h"
#include "rf_cdma_utils.h"
#include "rfcommon_math.h"
#include "rfm_internal.h"
#include "rfc_cdma.h"
#include "rf_cdma_utils_freq.h"
#include "rf_cdma_mdsp_types.h"
#include "rfcommon_core_txlin.h"
#include "rf_cdma_mdsp.h"
#include "rf_cdma_msm.h"
#include "rf_hdr_power_limiting.h"
#include "rf_cdma_data.h"
#include "rfcommon_time_profile.h"
#include "rf_hdr_ept.h"
#include "rf_cdma_constants.h"
#include "rf_cdma_mc.h"
#include "rfcommon_fbrx_api.h"
#include "rf_cdma_auto_pin.h"

/* PA and ASM */
#include "rfdevice_pa_intf.h"
#include "rfdevice_asm_intf.h"

/* New Buffer Interface */
#include "rf_hal_buffer.h"

#include "rfcommon_autopin_api.h"
#include "rflm_api_autopin_hw.h"


/*============================================================================*/
/*!
  @name TxAGC Configure

  @brief
  Functions for the HDR to configure HDR TxAGC
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the HDR Tx closed loop power

  @details
  This API allows HDR to set the desired closed loop power if the
  Tx AGC is in override mode.

  This API does not set or clear the override mode status.

  @param device
  The RF device for which Tx power needs to be set
 
  @param band
  Current RF band
 
  @param power_dbx2
  Tx power to be set (1/2 dB units) 
 
   @param curr_nominal_pwr_dB256
  The current nominal power to be used in Tx AGC calculations (dB256)

*/
void
rf_hdr_set_tx_power_adjust
(
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type const band,
  int16 const power_dbx2,
  int16 const curr_nominal_pwr_dB256
)
{
  int mdsp_txpwr_dB512; /* Holds adjust value to send to TxAGC (mdsp units)*/
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
    /* Nominal Power (turnaround equivalent) is specified with value 
       given, so we must subtract out the turnaround accounted for in 
       the AGC. Convert to 1/512th units (taken by AGC driver).
    */
    mdsp_txpwr_dB512 =
                    (power_dbx2 * 256) 
                  + (curr_nominal_pwr_dB256 * 2)
                  - (rf_cdma_get_turnaround_constant(band) * 512);
  
    /* Check if the value is within accepted range of the DSP interface */
    mdsp_txpwr_dB512 = rf_limit_s32( mdsp_txpwr_dB512, 
                                     RF_CDMA_MDSP_MIN_TX_PWR,
                                     RF_CDMA_MDSP_MAX_TX_PWR );
  
    rf_hdr_mdsp_tx_closed_loop_override( logical_dev->tx_mdsp_path, 0, 
                                         (int16)mdsp_txpwr_dB512, NULL, NULL,
                                         NULL );
  
    RF_MSG_1( RF_HIGH, "rf_hdr_set_tx_power_adjust: RF power adjusted by: %d", 
              mdsp_txpwr_dB512/512 );
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_hdr_set_tx_power_adjust : NULL Device Param"
                      " Pointer" );
  }

} /* rf_hdr_set_tx_power_adjust */

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return HDR TxAGC IQ Gain Params for different reverse link configurations

  @details
  Computes the IQ gain scale factors and the LUT offset values for different
  reverse link configurations, based on NV data. These values are provided 
  through SMEM to FW and are latched with the Tx AGC configuration message.
 
  NV provides the IQ gain offset in dBx10. This function computes the IQ 
  gain scaling factor in HW units that must be applied on top of nominal IQ 
  gain.
 
  @internal
  RF calibration is done with default TxLM configuration and should not be
  affected by static NV. Hence, we bypass NV computations and load the
  default link manager configuration through this function.
  E.g. calibration is done with RC1 waveform, and using default config.
  Normal operation in RC1 will use the calibrated waveform AND the NV
  offsets into consideration.
 
  @param iq_gain_params
  Out parameter, into which the computed IQ gain values and LUT offsets
  are populated
 
  @param rf_hdr_config_ptr
  Pointer to 1x NV configuration data
 
  @return 
  TRUE on success and FALSE on failure
*/
static
boolean
rf_hdr_tx_agc_get_iqgain_params
(
  rf_hdr_mdsp_tx_apt_adj_type *iq_gain_params,
  const rf_hdr_config_type *rf_hdr_config_ptr
)
{
  /* HDR APT adjust data in NV */
  const rf_cdma_tx_apt_adj_val_type *iq_gain_nv_ptr;
  /* Iterator */
  int rc_type;

  /* Retrieve a pointer to the APT adjust data in NV */
  iq_gain_nv_ptr = &(rf_hdr_config_ptr->tx_static.apt_adj_value[0]);

  /* In cal mode, populate the entire table with defaults */
  if ( rfm_get_calibration_state() == TRUE ||
       rf_cdma_debug_flags.use_default_iq_gain == 1 )
  {
    for ( rc_type = RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_PILOT;
          rc_type < RF_CDMA_NV_HDR_TX_IQGAIN_RC_NUM;
          rc_type++ )
    {
      /* Provide unity scaling in 2Q14 */
      iq_gain_params->rc[rc_type].iq_gain_scale_factor = 1 << 14;
      iq_gain_params->rc[rc_type].lutDbOffset = 0;
    }
  }
  else
  {
    for ( rc_type = RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_PILOT;
          rc_type < RF_CDMA_NV_HDR_TX_IQGAIN_RC_NUM;
          rc_type++ )
    {
      /* Offset the default IQ gain value with the data from NV and
      convert it from Q15 into 2Q14 HW units */
      iq_gain_params->rc[rc_type].iq_gain_scale_factor =
        (uint16)((iq_gain_nv_ptr[rc_type].iq_gain_adj)
                >> 1);
      /* Provide the offset as a negative value for LUT offset, as FW
      will apply it to txTotal with negative logic */
      iq_gain_params->rc[rc_type].lutDbOffset =
        (iq_gain_nv_ptr[rc_type].lut_offset *(-1));
    }  
  }

  return TRUE;

} /* rf_hdr_tx_agc_get_iqgain_params */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain and apply HDR TxAGC IQ Gain Params for different
  reverse link configurations

  @details
  1> Obtains the IQ gain scale factors and the LUT offset values
  for different reverse link configurations, based on NV data.
  2> Sends the IQ gain params to RFLM to be applied. 
 
  @param device
  RF Device for which IQ settings are to be applied.
 
  @param iq_gain_params
  Out parameter, into which the computed IQ gain values and LUT
  offsets are populated
 
  @param rf_hdr_config_ptr Pointer to 1x NV configuration data
 
  @param size_iq_gain_params
  This param conatils the size of CDMA IQ gain param table and
  is passed to RFLM to make sure that there is no mismatch
  between RF and FED.
 
  @return 
  TRUE on success and FALSE on failure
*/

boolean 
rf_hdr_tx_agc_get_and_set_iqgain_params
(
  const rf_hdr_config_type *rf_hdr_config_ptr 
 )
{
  /* Tx IQ Gain data for different reverse link configurations */
  rf_hdr_mdsp_tx_apt_adj_type tx_iq_gain_params ;

  /* Get the IQ gain params from NV */
  boolean retval = rf_hdr_tx_agc_get_iqgain_params ( &tx_iq_gain_params , 
                                                 rf_hdr_config_ptr ) ;
  if ( retval == TRUE )
  {
    /* Send the params to FED now */
    retval = 
      rf_cdma_mdsp_rflm_set_iqgain_params ( RFM_1XEVDO_MODE , 
                                            &(tx_iq_gain_params.rc[0]) ,
                                            sizeof (tx_iq_gain_params) ) ;
  }
  return retval ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return HDR TxAGC Configuration

  @details
  All TxAGC-related paramaters are expected to be configured in this API. 

  @param device
  RF device for which TXLM needs to be configured
 
  @param num_carriers
  Number of carriers for which TxAGC is to be configured
 
  @param rf_band
  Band to which device is tuned
 
  @param rf_channel
  Channel to which device is tuned 
 
  @param txlm_handle
  TxLM handle for the LM chain on which device is tuned

  @param rf_hdr_config_ptr
  Pointer to HDR NV configuration
 
  @return TRUE on success and FALSE on failure
*/
boolean
rf_hdr_configure_tx_agc
(
  rfm_device_enum_type const device,
  uint32 num_carriers,
  rfm_cdma_band_class_type const rf_band,
  uint16 const rf_channel,
  lm_handle_type txlm_handle,
  const rf_hdr_config_type *rf_hdr_config_ptr
)
{
  boolean result = TRUE; /* final result of API */
  rf_cdma_mdsp_pa_stm_config_type pa_stm_data; /* PA STM Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rf_time_tick_type hdr_txagc_time; /* holds the time spent in this function */
  rf_time_type hdr_txagc_time_val;
  rf_cdma_carrier_mode_type carrier_mode;/* Denotes whether operating in SC or MC mode */
  const rf_cdma_tx_static_type *tx_cdma_config_ptr =
    &(rf_hdr_config_ptr->cdma_common_ptr->tx_static);

  const rf_cdma_data_status_type *dev_status_r;
  /* Tx threshold indicator NV params */  
  rf_cdma_mdsp_tx_gtr_thresh_type tx_thresh_params;
  /* FW TxAGC Logging is_high_band or not*/
  boolean is_high_band;
  /* XPT config params */
  rf_cdma_mdsp_xpt_config_type xpt_config_params;
  uint32 modem_chain;
  uint16 *tx_therm_bins = NULL;   /*tx therm bins from static NV*/
  rfcommon_fbrx_error_type fbrx_temp_comp_result; /* fbrx temp comp status */

  const rfcommon_core_txlin_log_data_type* log_data;

  /* Linearizer configuration optional */
  boolean config_linearizer = TRUE;
  /* Start Time Measurement */
  hdr_txagc_time = rf_time_get_tick();

  /* Device Status Write Pointer */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( dev_status_r == NULL )
  {
    RF_MSG( RF_ERROR, "rf_hdr_configure_tx_agc: "
                      "Null pointer returned for dev_status_r ");
    return FALSE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_hdr_configure_tx_agc: "
                      "Null pointer returned to logical_dev ");
    return FALSE;
  }

  modem_chain = rfc_cdma_get_tx_modem_chain ( device , rf_band );

  /* If more than 1 channel... */
  if ( num_carriers > 1 )
  {
    /* Use multi-carrier settings */
    carrier_mode = RF_CDMA_CARRIER_MODE_MC ;
  }
  else
  {
    /* default to single carrier */
    carrier_mode = RF_CDMA_CARRIER_MODE_SC ;
  }

  /* memset everything in pa_stm_data to 0 before use*/
  memset( &pa_stm_data, 0, sizeof( rf_cdma_mdsp_pa_stm_config_type ) );

  /* Update the PA Params based on device status */
  result &= rf_hdr_tx_agc_update_pa_params( device,
                                            num_carriers,
                                            rf_band,
                                            rf_channel,
                                            rf_hdr_config_ptr,
                                            &pa_stm_data );

  /* get msm-specific TX AGC parameters */
  result &= rf_hdr_get_tx_agc_params( device,
                                      modem_chain ,
                                      logical_dev->tx_nv_path[rf_band], rf_band,
                                      &pa_stm_data ); 

  /* Get and set IQ gain params for different reverse link configurations */
  result &= rf_hdr_tx_agc_get_and_set_iqgain_params ( rf_hdr_config_ptr ) ;

  /* Determine High/Low band for FW TxAGC logging */
  is_high_band = rf_cdma_is_high_band( rf_band );

  /* Update Tx GTR Threshold params */
  tx_thresh_params.feature_enabled = 
    tx_cdma_config_ptr->tx_gtr_thresh_data.active;
  tx_thresh_params.fall_value =
    rf_cdma_mdsp_dbm10_to_txagc(
      tx_cdma_config_ptr->tx_gtr_thresh_data.fall_value,
      rf_cdma_get_turnaround_constant(rf_band)*10 );
  tx_thresh_params.rise_value =
    rf_cdma_mdsp_dbm10_to_txagc(
      tx_cdma_config_ptr->tx_gtr_thresh_data.rise_value,
      rf_cdma_get_turnaround_constant(rf_band)*10 );

   /* Linearizer and related configuration must only be performed if one of 
  these conditions is met */
  if ( dev_status_r->temp_comp.init_trigger == TRUE )
  {
     config_linearizer = TRUE;
  }
  else if ( rf_cdma_get_pout_comp_skip( device) == TRUE &&
  	    rflm_autopin_sanity_check(dev_status_r->txlm_handle) )
  {
     config_linearizer = FALSE;
     RF_MSG( RF_MED,"rf_hdr_configure_tx_agc: Skipping linearizer and "
                    "related updates for this temp comp" );
  }
  
  if ( config_linearizer == TRUE )
  {
  if ( carrier_mode == RF_CDMA_CARRIER_MODE_MC )
  {
    /* Populate CDMA's tx_lin_data with tx_multi_lin_cal NV data */
    /* Populate CDMA's tx_lin_data with PA Q-current RFFE data */
    result &= rf_cdma_tx_agc_load_linearizer( RFM_1XEVDO_MODE,
                                              device, rf_band, rf_channel, 
                                              TX_LINEARIZER_DATA_TYPE_2,
                                              &pa_stm_data,
                                              txlm_handle );
  }
  else
  {
    /* Populate CDMA's tx_lin_data with tx_multi_lin_cal NV data */
    /* Populate CDMA's tx_lin_data with PA Q-current RFFE data */
    result &= rf_cdma_tx_agc_load_linearizer( RFM_1XEVDO_MODE,
                                              device, rf_band, rf_channel, 
                                              TX_LINEARIZER_VOICE,
                                              &pa_stm_data,
                                              txlm_handle );
  }
  }/* if ( config_linearizer == TRUE ) */

  /* continue with configuration routines only if pre-config steps were
     successful */
  if ( result == TRUE )
  {
    /* Configure TxAGC. This funtion should be called after LUT is programmed
    to the SMEM, since FW latches the LUT immediately after this message.
    The APT PDM LUTs are programmed to SMEM as part of configuring the Tx
    linearizer. The LUT is latched into an internal buffer from SMEM
    once APT is enabled using this message*/
    if ( result == TRUE)
    {
      /* Initialize the xpt config params */
      if ( carrier_mode == RF_CDMA_CARRIER_MODE_MC )
      {
        xpt_config_params.apt_iq_gain_nominal = 
                                               dev_status_r->mcdo_iq_gain_val ;
      }
      else /* if ( carrier_mode == RF_CDMA_CARRIER_MODE_SC ) */
      {
        xpt_config_params.apt_iq_gain_nominal = 
                                               dev_status_r->cdma_iq_gain_val ;
      }
      xpt_config_params.ept_iq_gain_nominal = dev_status_r->ept_iq_gain_val;
      xpt_config_params.et_iq_gain_nominal = dev_status_r->ept_iq_gain_val;
      xpt_config_params.et_env_scale_nominal = 
               rf_cdma_xpt_get_cfg_param(device, ET_CFG_CMN_ENV_SCALE_UNITY);
      xpt_config_params.dor0_pred_clip_thresh = 
               rf_cdma_xpt_get_cfg_param(device, XPT_CFG_CDMA_CLIP_THR_DOR0);
      xpt_config_params.dorA_pred_clip_thresh = 
               rf_cdma_xpt_get_cfg_param(device, XPT_CFG_CDMA_CLIP_THR_DORA);

      /* Populate pred_clip on/off threshold. Must be called before 
      mdsp_configure_tx_agc*/
      result &= rf_cdma_txagc_get_pred_clip_thresh(rf_band,
                                                   &pa_stm_data, 
                                                   &xpt_config_params );

      result &= rf_hdr_mdsp_configure_tx_agc( 
               logical_dev->tx_mdsp_path, 
               &pa_stm_data,  
               tx_thresh_params, 
               is_high_band,
               rf_cdma_xpt_get_cfg_param(device, XPT_CFG),
               rf_cdma_get_turnaround_constant(rf_band),
               &xpt_config_params,
               NULL, NULL, NULL ); 

      /*! do rf_cdma_txagc_config only when temp_comp.init_trigger is true;*/
      if(dev_status_r->temp_comp.init_trigger == TRUE )
      {
        /* Configure TxAGC information to RFLM through common */
        result &= rf_cdma_tx_agc_config( device, rf_band, txlm_handle );
      }        
      /* Only update PA Params and lin limit if linearizer was configured */
      if ( config_linearizer == TRUE )
      {
      /* We need to update the XPT mode after we are done with linearizer 
      loading*/ 
      pa_stm_data.update_xpt_mode = TRUE;

      /* Update PA state params to RFLM */
      result &= rf_cdma_mdsp_config_pa_params( 
                             RFM_1XEVDO_MODE,
                             txlm_handle, 
                             rf_cdma_xpt_get_cfg_param(device, XPT_CFG),
                             &pa_stm_data );

        /* Get current log data */
        log_data = rfcommon_core_txlin_get_log_data(RFCOM_1XEVDO_MODE,device);

        if (NULL == log_data)
        {
          /* Use a default max linearizer limit of 30 dBm in this case */
          rf_hdr_plim_update_lin_limit( device, 300 );
        }
        else
        {
          /* Use a max linearizer limit */
          rf_hdr_plim_update_lin_limit( device, log_data->max_calibrated_power );
        }
      }/* if ( config_linearizer == TRUE ) */
    }/* if ( result == TRUE) */

    /* --------------------------- FBRx Temp Comp ----------------------- */
    /* Query therm bins from tx temp comp NV */
    tx_therm_bins = rf_hdr_config_ptr->cdma_common_ptr->
                                 tx_static.lin_temp_comp_data.tx_therm_bins;

    /* Update fbrx temp comp with queried therm bins */ 
    if (tx_therm_bins != NULL) 
    {
      fbrx_temp_comp_result = rfcommon_fbrx_mc_update_temp_comp(
                                                     txlm_handle,
                                                     dev_status_r->temp_comp.last_temp_sensor_read,
                                                     tx_therm_bins);
      if (fbrx_temp_comp_result != RFCOMMON_FBRX_SUCCESS)
      {
        RF_MSG_1( RF_ERROR, "rf_hdr_configure_tx_agc: "
                            "FBRx Temp Comp failed %d", fbrx_temp_comp_result);
      } 
    }
    else
    {
      RF_MSG( RF_ERROR, "rf_hdr_configure_tx_agc: Therm bins is NULL!");
    }

  } /* if ( result == TRUE ) */
  else
  {
    RF_MSG(RF_ERROR, "rf_hdr_configure_tx_agc: Error occured in pre-config "
                     "steps - bailing out ");
  }

  hdr_txagc_time_val = rf_time_get_elapsed( hdr_txagc_time, RF_USEC );

  if( result == FALSE )
  {
    RF_MSG_3( RF_ERROR, 
              "rf_hdr_configure_tx_agc: FAILED for device %d band %d channel %d",            
              device, rf_band, rf_channel );
  }
  else
  {
    RF_MSG_2( RF_MED, "rf_hdr_configure_tx_agc : time_taken = %d, result = %d ",            
              hdr_txagc_time_val, result );
  }

  return result;

} /* rf_hdr_configure_tx_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update HDR Tx PA Params configuration

  @details
  This function will configure HDR TxAGC with updated PA Params. 
  
  @param device
  RF device for which TXLM needs to be configured
 
  @param num_carriers
  Number of carriers for which TxAGC is to be configured
 
  @param rf_band
  Band to which device is tuned
 
  @param rf_channel
  Channel to which device is tuned
 
  @param rf_hdr_config_ptr
  Pointer to HDR NV configuration
 
  @return TRUE on success and FALSE on failure
*/
boolean
rf_hdr_tx_agc_update_pa_params
(
  rfm_device_enum_type const device,
  uint32 num_carriers,
  rfm_cdma_band_class_type const rf_band,
  uint16 rf_channel,
  const rf_hdr_config_type *rf_hdr_config_ptr,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data
)
{
  boolean result = TRUE; /* final result of API */
  const rf_cdma_data_status_type *dev_status_r;
  const rf_hdr_tx_static_type *tx_hdr_config_ptr;
  rf_cdma_tx_static_type *tx_static_struct;
  const rf_cdma_tx_switch_points_type *tx_hdr_swpts_ptr; /* Ptr to Tx switchpoints */
  rf_cdma_carrier_mode_type carrier_mode;/* Denotes whether operating in SC or MC mode */
  rfm_pa_gain_state_type pa_state;
  uint8 num_active_pa_states = 0;

  /* Storage for relevant HDR device status data */
  rf_hdr_tx_agc_rri_type rri;

  /* Device Status Write Pointer */
  dev_status_r = rf_cdma_get_device_status( device );
  if ( dev_status_r == NULL )
  {
    RF_MSG( RF_ERROR, "rf_hdr_tx_agc_update_pa_params: "
                      "Null pointer returned to dev_status ");
    return FALSE;
  }

  if ( rf_hdr_config_ptr != NULL )
  {
    tx_static_struct = &(rf_hdr_config_ptr->cdma_common_ptr->tx_static);
    /* Pointer to HDR static NV data */
    tx_hdr_config_ptr = &(rf_hdr_config_ptr->tx_static);
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_hdr_tx_agc_update_pa_params: "
                      "Null pointer returned to HDR NV pointer ");
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
    
  /* Obtain the switchpoints in dBm10 */
  if ( dev_status_r->traffic_state )
  {
    /* Obtain a pointer to the Tx switchpoints based on
    current HDR carrier mode */ 
    switch ( carrier_mode )
    {
    case RF_CDMA_CARRIER_MODE_SC:
      rri = dev_status_r->data_hdr.rri;
      if ( rri <= RF_HDR_TX_AGC_153K6_BPS_RRI )
      {
        tx_hdr_swpts_ptr = &tx_hdr_config_ptr->dor0_swpts_rri[rri];
      }
      else if ( rri == RF_HDR_TX_AGC_REVA_RRI )
      {
        tx_hdr_swpts_ptr = &tx_hdr_config_ptr->dorA_swpts;
      }
      else
      {
        tx_hdr_swpts_ptr = &tx_hdr_config_ptr->dor0_swpts_rri[0];
      }
      break;
  
    case RF_CDMA_CARRIER_MODE_MC:
      tx_hdr_swpts_ptr = &tx_hdr_config_ptr->dorB_mc_swpts;
      break;

    default:
      tx_hdr_swpts_ptr = &tx_hdr_config_ptr->dor0_swpts_rri[0];
      break;
    }
  }
  else
  {
    tx_hdr_swpts_ptr = &tx_hdr_config_ptr->access_swpts;
  }

  /* Update active pa state information */
  if ( tx_static_struct != NULL )
  {
    num_active_pa_states = tx_static_struct->num_active_pa_states;
  }
  else
  {
    num_active_pa_states = 0;
  }/* if ( tx_static_struct != NULL ) */

  for ( pa_state = RFM_PA_GAIN_STATE_0; 
        pa_state < RFM_PA_GAIN_STATE_NUM; 
        pa_state++ )
  {
    if ( pa_state < num_active_pa_states )
    {
      pa_stm_data->active_pa_state[pa_state] = TRUE;
    }
    else
    {
      pa_stm_data->active_pa_state[pa_state] = FALSE;
    }

    /* Populate PA Rise Val */
    pa_stm_data->rise_val[pa_state] = 
                                   tx_hdr_swpts_ptr->rise[pa_state];
    /* Populate PA Fall Val */
    pa_stm_data->fall_val[pa_state] = 
                                   tx_hdr_swpts_ptr->fall[pa_state];
  }

  /* Populate PA Hysterisis Timer */
  pa_stm_data->pa_hyst_timer = tx_hdr_swpts_ptr->fall_hold_time;
  /* Populate Predictive clipper power threshold */
  pa_stm_data->pred_clip_en_thresh = tx_static_struct->pred_clip_nv.enable_threshold;
  pa_stm_data->pred_clip_dis_thresh = tx_static_struct->pred_clip_nv.disable_threshold;

  RF_MSG_8( RF_LOW, "rf_hdr_tx_agc_update_pa_params: "
            "PA SP [F0 %d, F1 %d, F2 %d, F3 %d, "
            "F4 %d, F5 %d, F6 %d, F7 %d] dBm10", 
            tx_hdr_swpts_ptr->fall[RFM_PA_GAIN_STATE_0], 
            tx_hdr_swpts_ptr->fall[RFM_PA_GAIN_STATE_1], 
            tx_hdr_swpts_ptr->fall[RFM_PA_GAIN_STATE_2], 
            tx_hdr_swpts_ptr->fall[RFM_PA_GAIN_STATE_3],
            tx_hdr_swpts_ptr->fall[RFM_PA_GAIN_STATE_4],
            tx_hdr_swpts_ptr->fall[RFM_PA_GAIN_STATE_5],
            tx_hdr_swpts_ptr->fall[RFM_PA_GAIN_STATE_6],
            tx_hdr_swpts_ptr->fall[RFM_PA_GAIN_STATE_7] );
  RF_MSG_8( RF_LOW, "rf_hdr_tx_agc_update_pa_params: "
            "PA SP [R0 %d, R1 %d, R2 %d, R3 %d, "
            "R4 %d, R5 %d, R6 %d, R7 %d] dBm10", 
            tx_hdr_swpts_ptr->rise[RFM_PA_GAIN_STATE_0], 
            tx_hdr_swpts_ptr->rise[RFM_PA_GAIN_STATE_1], 
            tx_hdr_swpts_ptr->rise[RFM_PA_GAIN_STATE_2], 
            tx_hdr_swpts_ptr->rise[RFM_PA_GAIN_STATE_3],
            tx_hdr_swpts_ptr->rise[RFM_PA_GAIN_STATE_4],
            tx_hdr_swpts_ptr->rise[RFM_PA_GAIN_STATE_5],
            tx_hdr_swpts_ptr->rise[RFM_PA_GAIN_STATE_6],
            tx_hdr_swpts_ptr->rise[RFM_PA_GAIN_STATE_7] );

  return result;

} /* rf_hdr_tx_agc_update_pa_params */


#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */
