/*!
  @file
  rf_1x_tx_agc.c

  @details
  This file defines interfaces pertaining to 1x TxAGC configuration.
 
  @addtogroup RF_CDMA_1X_TXAGC
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rf_1x_tx_agc.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/13/16   pkm     Make sure Pin/Pout not applied in temp comp when Autopin is running.
12/20/15   vr      Autopin check-in
01/16/15   vr     CDMA predictive clipping NV based power thresh support
12/23/14   vr      Get max linearzer cal power from TxAGC and update PLIM .
11/10/14   zhw     Dynamic Predictive Cliping support
09/26/14   JJ      Moved fbrx temp comp from temp comp read handler api to config
                   tx agc api
07/21/14   cd      Add predictive clipper support for 1x, DO rev0, DO revA
05/02/14   cd      8-PA state functionality updates
04/16/14   cd      Cleanup unused MDSP interfaces and Tx AGC functionality
04/01/14   APU     PA params update hook-up for different RC/data rate configs.
03/13/14   spa     Add flag for update_xpt_mode in config_tx_agc
01/08/14   cd      Provide xPT type to RFLM per PA state
01/03/14   hdz     Do rf_cdma_tx_agc_config only when temp_comp.init_trigger
                   is TRUE
12/10/13   sty     deleted set-but-unused variable -- tx_template_id
12/06/13   cd      Load nominal baseband gains to RFLM
11/25/13   cd      Remove flag-based functionality for legacy Tx AGC
11/08/13   cd      Remove support for legacy FW interfaces
10/30/13   cd      Remove legacy FW stop Tx AGC support for Bolt cal mode
10/25/13   cd      Use default for linearizer limit
10/25/13   cd      Linearizer limit updated only if not using FED Tx AGC
10/18/13   cd      Added flag to support RFLM TxAGC conditionally in FW
10/09/13   cd      Configure common Tx AGC after linearizer loading
10/08/13   cd      Add support for common TxAGC debug flag
10/03/13   cd      Hook up PA switchpoint loading to RFLM
10/02/13   JJ      Skip TX_OVERRIDE since FW has deprecated these messages 
09/27/13   cd      Hook up common load linearizer interface to CDMA
09/26/13   cd      Added functionality to load to RFLM through common TxAGC
12/17/13   vr      Fix for lin table corruption when no QPOET
07/10/13   zhw     Remove dev_status_w in mc layer call flow
07/10/13   JJ      Remove func rf_1x_tx_agc_set_qpoet_bias()
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
06/18/13   JJ      Fixed a typo in func rf_1x_configure_tx_agc()
06/07/13   APU     Check for _tx_agc_load_linearizer() before writing to FW.
04/29/13   Saul    1X XPT. Intermittent cal failure fix.
04/23/13   APU     Removed old unused iqgain_params and incoporated CC comments.
04/19/13   APU     Removed:
                   1>  the older unused _configure_tx_linearizer() function.
                   2>  the older unused _get_iqgain_params() function.
04/17/13   zhw     Support for variable transaction in QPOET/PA TxAGC templates
04/12/13   cd      Remove max PA state functionality
03/28/13   zhw     Clean up obsolete pa_range and apt_val FW interface
03/22/13   sr      changes to PAPM interface.
03/07/13   sr      changed the "pwr_tracker" name to "papm".
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
02/05/13   cd      Re-introduce cal-mode functionality, by ensuring that FW is 
                   first in Tx override mode
02/04/13   cd      Reverted cal mode flag change, until FW support is available
02/04/13   cd      Provide cal mode flag to FW based on calibration state
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/30/13   zhw     Pass in high/low band info for FW TxAGC logging
01/16/13   Saul    CDMA. Enabled EPT/ET tempcomp.
01/14/13   Saul    CDMA. 1x EPT/ET call support.
12/19/12   cd      Fixes for ET Tx configuration in FTM mode only
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/11/12   zhw     Stop FW Tx AGC in APT Cal 
12/11/12   Saul    CDMA. TX AGC XPT Dev. Get max PA state based on lin NV.
12/07/12   spa     Populate FW smem with QPOET data in RFcal
12/06/12   spa     Bug fixes in set QPOET bias API
12/06/12   APU     Replaced LM chains with Modem chains.
12/05/12   spa     Added RF mode param to load linearizer and HDET APIs
11/29/12   zhw     PA API refactor and make pa_script API static
11/21/12   spa     Added API to set QPOET bias
11/12/12   zhw     Load FW mdsp interface with PA Q current RFFE writes
11/15/12   Saul    CDMA. TX AGC XPT.
11/07/12   cd      Corrected merge issue. Call updated IQ gain scaling API
11/02/12   zhw     Remove deprecated pa stm data members
10/31/12   zhw     Compiler warning fix. memset pa_stm_data before use
10/29/12   zhw     Support for query device_config from RFC and device driver
10/29/12   spa     Call update PA params before get tx agc params because the 
                   pa_r_map is needed in get tx agc params to calculate bitmasks 
10/23/12   zhw     Remove hardcoding in get_template_id()
10/16/12   sty     Removed all refs to FEATURE_RF_HAS_QFE1320
10/07/12   zhw     Error check for result TRUE before sending to FW
10/04/12   spa     Added check for valid SSBI/RFFE PA before generating scripts  
10/02/12   zhw     Support for PA state setting for SSBI/RFFE PA
09/26/12   zhw     Configure TxAGC with Template ID from MC layer
09/25/12   zhw     Refactor configure tx agc API, avoid using void pointer
09/24/12   zhw     Fix OFT Compiler error
09/20/12   zhw     PA scripting support
09/14/12   sty     Code cleanup
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/22/12   cri     EPT support for CDMA FW interfaces
08/21/12   cd      Avoid linearizer computation if in cal state 
08/08/12   cvd     Replace timetick with rfcommon_time_profile interface
06/29/12   spa     Compiler warning fix 
06/28/12   APU     Removed old QFE specific calls. Replaced with new generic PA 
                   interface. 
06/28/12   APU     Changed QFE to the new generic PA interface. 
04/11/12   Saul    QFE. Initial support.
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
03/22/12   sty     Deleted redundant rf_cdma_nv_set_pa_range_map_arr()
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/16/12   hdz     Corrected expected msg and rename 
                   temp_comp.is_combined_with_freq_comp to
                   temp_comp.init_trigger 
03/08/12   cd      Configure Tx AGC with GTR Thresh information
02/29/12   cd      - Provide support to load data Tx switchpoint NV
                   - Cleaned up legacy band nv tbl reference and pa backoffs
                   - Added rf_1x_tx_agc_config_pa_params() to update PA Params
02/08/12   hdz     Cleaned up function. 
02/06/12   hdz     Added rf_1x_update_tx_agc() for reloading tx agc in calls
02/02/12   sty     Use is_voice flag from device status in rf_1x_configure_tx_agc
02/02/12   sty     Fixed bugs with intial value of invalid_swpt_val
01/27/12   cd      Use default IQ gain settings when Tx is in access state 
01/19/12   cd      Use linearizer limit configuration API from PLIM module
01/18/12   aro     Updated interface to query Modem specific TxGAC param to use
                   TxLM chain rather than device
01/20/12   sty     Use 40dBm as threshold to invalidate a PA state
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
12/07/11   aro     Added interface to query Tx RGI address
12/02/11   cd      Added support to configure Tx AGC for access and traffic with 
                   new switchpoints 
12/01/11   cd      Provide IQ gain params based on default TxLM and NV data 
11/04/11   cd      Add FW flag to Tx AGC config message to indicate cal/non-cal
                   mode
11/03/11   cd      Update plim data with max linearizer entry that's loaded to 
                   FW 
10/18/11   cd      Removed old linearizer functionality
10/11/11   aro     Added debug code to display PA switch points
10/10/11   aro     Removed PA backoff from the PA Rise and PA Fall for
                   PA State 2 to PA State 3 transition causing Tx power
                   wraparound at Max Power
09/01/11   cd      Program APT LUTs to FW when configuring the linearizer
08/30/11   shb     Removed pwr lim setting from rf_1x_configure_tx_agc - will 
                   be handled by 1x plim module
08/29/11   aro     Added temporary PA Switchpoint backoff
08/17/11   cd      Fixed KW warnings
08/16/11   cd      Added debug flag for functionality to load FW linearizers 
                   with old tx linearizer data structures or new multi-lin
                   data structures
08/08/11   aro     Added code not to clear Tx overrides in calibration Mode
07/20/11   sty     Changed arg-list for rf_1x_configure_tx_agc() - takes in 
                   NV pointers now
07/07/11   sty     Reworked rf_1x_configure_tx_agc to return correct API result
                   and bail out of actual configuration on error
07/05/11   bmg     Changed power limit interface to use dBm10.
06/29/11   sty     Read max_pwr_limit from rf_cdma_debug_flags
06/28/11   sty     Changed max_pwr limit
06/24/11   sty     Check for null pointers
06/13/11   sty     Renamed rf_cdma_txagc.h to rf_cdma_tx_agc.h
06/13/11   sty     Moved MSM-specific data to HAL
06/09/11   cd      Removed PA R map load check. NV functionality provides the 
                   appropriate PA R Map
06/08/11   shb     Updated rft_get_tx_agc_pdm_write_reg_address() to provide
                   SSBI bus
06/08/11   cd      Deprecated old PA R Map NV loading support
06/07/11   aro     Replaced SV debug flag with Device check (temp)
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/06/11   aro     [1] Pushed PA Range Map converion to RF-MDSP layer
                   [2] Re-sequenced MDSP call to send Tx AGC configure message
                   only after LUT programming in SMEM
05/24/11   aro     [1] Added Logical Device Mapping to MDSP Chain / NV for SV
                   [2] Updated interface to send GRFC info to MDSP
05/26/11   aro     [1] Replaced MDSP chain with logical device mapping
                   [2] Added debug flag for device switch in SV mode
05/13/11   sty     Fixed warnings
05/12/11   sty     Skip NV read for bringup in rf_1x_configure_tx_agc()
05/10/11   cd      - Remove old NV table pointer usage
                   - Removed rf_cdma_utils_get_freq_bin() and 
                     rf_cdma_nv_get_freq_bin()
05/04/11   shb     Pass band to RF device in TX LUT address/data query functions
05/02/11   cd      Added error msg for malloc failure
04/26/11   aro     Removed Enable/Disable Tx interfaces
04/22/11   cd      KW fixes to check mem alloc for tx_lin_data
03/25/11   sty     Renamed header file from rf_cdma_txlin.h to rf_cdma_txagc.h  
03/24/11   sty     Abstracted out functionality f rf_1x_load_tx_linearizers() 
                   into rf_cdma_load_tx_linearizers(). This enables HDR and 1x 
                   to use rf_cdma_load_tx_linearizers()
03/21/11   sty     Added debug_non_monotonic flag  
03/16/11   sty     Added band_chan arg to rf_1x_configure_tx_agc
03/16/11   sty     Fixed bug in rf_1x_load_tx_linearizers() when input table is 
                   not monotonically increasing
                   Changed variable name from temp_pwr to pwr_for_curr_index
03/16/11   aro     Fixed Bad Merge
03/16/11   aro     Used Modem memalloc to allocate memory for Linearizer Table
03/16/11   aro     Changed Device Param to MDSP Chain
03/16/11   aro     [1] Interface name update
                   [2] Renamed enum to RF_CDMA_TX_LINEARIZER_NUM
03/16/11   aro     Removed unnecessary include
03/16/11   aro     Cleaned up Override Interface + added device argument
03/15/11   sty     Fixed case where non-monotonic tx powers were being uploaeded
                   to mdsp
03/15/11   aro     Renamed Configure Tx Timimg Interface
03/15/11   aro     [1] Added support for consolidated Tx Linearizer
                   [2] Compiler Warning Fix
03/15/11   aro     Consolidated Tx configuration to PA STM and TX
                   Linearizer interfaces
03/14/11   sty     Used renamed #defines
03/14/11   sty     Fixed warnings
03/14/11   sty     Renamed API to rf_1x_load_tx_linearizers
                   Changed behavior for rf_1x_load_tx_linearizers if LUT or 
                   Power table is invalid
03/14/11   sty     Fixed warnings
03/10/11   sty     Modified rf_1x_load_mdsp_tables() to call loading of 
                   Linearizers with raw txAGC - the conversion will now be done
                   within mdsp module
                   Moved all mdsp-related #defines to mdsp module
03/09/11   sty     Changed datatype of lut_index and mdsp_index
03/09/11   sty     Fixed doxygen warning 
03/09/11   sty     Changed signature of rf_1x_load_mdsp_tables - need band and 
                   channel info.
                   Changed data types of few vars in rf_1x_load_mdsp_tables() 
03/08/11   sty     Refactored rf1x_* to rf_1x_*
                   Changed implementation of rf_1x_load_mdsp_tables() 
03/07/11   sty     Renamed rf1x_mdsp_config_lut to rf_1x_mdsp_config_lut
03/07/11   sty     Refactored few of rf1x_mdsp* APIs to rf_1x_mdsp*
                   Renamed index "i" to pa_state in rf_1x_load_mdsp_tables
                   Made min and max offset look-up a one-time thing
03/04/11   sty     Fixed warnings
03/03/11   sty     Fixed warnings
03/03/11   sty     Using rf_limit_s32 for unsigned caused crashed - used 
                   rf_limit_u32
03/03/11   sty     Added header files
03/03/11   sty     Replaced RF_SATURATE with rf_limit_s32
03/03/11   sty     Program variables in rf_1x_configure_tx_agc() by using 
                   values from NV (they were hard-coded earlier)
                   Added rf1x_load_mdsp_tables() to load linearizer tables to FW
03/02/11   sty     Renamed rf1xhdr_mdsp_pa_param_type to 
                   rf_cdma_mdsp_pa_param_type
02/18/11   sty     Use rf_timetick_type for time-profiling
02/07/11   sty     Added time-profiling code
02/03/11   aro     Moved new Device interface to separate file
01/30/11   sty     Added more debug flags in rf_1x_configure_tx_agc() 
01/25/11   sty     warning fixes
01/24/11   sty     Changed rf_1x_tx_agc_get_config
01/19/11   aro     Renamed RF-MDSP filename
01/12/11   sty     Initial Release.

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfcommon_time_profile.h"
#include "modem_mem.h"
#include "rf_1x_tx_agc.h"
#include "rf_cdma_tx_agc_msm.h"
#include "rf_1x_mdsp.h"
#include "rfcommon_msg.h"
#include "rf_cdma_data.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_tx_agc.h"
#include "rfm_internal.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_core_txlin.h"
#include "rf_cdma_mdsp.h"
#include "rf_cdma_msm.h"
#include "rf_1x_power_limiting.h"
#include "rfcommon_time_profile.h"
#include "rf_1x_ept.h"
#include "rf_cdma_constants.h"
#include "rf_cdma_mc.h"
#include "rfcommon_fbrx_api.h"

/* PA and ASM */
#include "rfdevice_pa_intf.h"
#include "rfdevice_asm_intf.h"

/* New Buffer Interface */
#include "rf_hal_buffer.h"
#include "rf_cdma_auto_pin.h"
#include "rfcommon_autopin_api.h"
#include "rflm_api_autopin_hw.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return 1x TxAGC IQ Gain Params for different reverse link configurations

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
 
  @param rf_1x_config_ptr
  Pointer to 1x NV configuration data
 
  @param traffic_state
  Indicate whether the IQ gain params must be provided for Access or
  Traffic state
 
  @return 
  TRUE on success and FALSE on failure
*/
static
boolean
rf_1x_tx_agc_get_iqgain_params
(
  rf_1x_mdsp_tx_apt_adj_type *iq_gain_params,
  const rf_1x_config_type *rf_1x_config_ptr,
  boolean traffic_state
)
{
  /* 1x APT adjust data in NV */
  const rf_cdma_tx_apt_adj_val_type *iq_gain_nv_ptr;
  /* Iterator */
  int rc_type;

  /* Retrieve a pointer to the APT adjust data in NV */
  iq_gain_nv_ptr = &(rf_1x_config_ptr->tx_static.apt_adj_value[0]);

  /* In cal mode or when Tx is in "Access" state,
  populate the entire table with defaults */
  if ( rfm_get_calibration_state() == TRUE ||
       traffic_state == FALSE ||
       rf_cdma_debug_flags.use_default_iq_gain == 1 )
  {
    for ( rc_type = RF_CDMA_NV_1X_TX_IQGAIN_PILOT;
          rc_type < RF_CDMA_NV_1X_TX_IQGAIN_RC_NUM;
          rc_type++ )
    {
      /* Provide unity scaling in 2Q14 */
      iq_gain_params->rc[rc_type].iq_gain_scale_factor = 1 << 14;
      iq_gain_params->rc[rc_type].lutDbOffset = 0;
    }
  }
  else
  {
    for ( rc_type = RF_CDMA_NV_1X_TX_IQGAIN_PILOT;
          rc_type < RF_CDMA_NV_1X_TX_IQGAIN_RC_NUM;
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

} /* rf_1x_tx_agc_get_iqgain_params */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain and apply 1x TxAGC IQ Gain Params for different reverse link 
  configurations

  @details
  1> Obtains the IQ gain scale factors and the LUT offset values
  for different reverse link configurations, based on NV data.
  2> Sends the IQ gain params to RFLM to be applied. 
 
  @param device
  RF Device for which IQ settings are to be applied.
 
  @param iq_gain_params
  Out parameter, into which the computed IQ gain values and LUT
  offsets are populated
 
  @param rf_1x_config_ptr
  Pointer to 1x NV configuration data
 
  @param traffic_state
  Indicate whether the IQ gain params must be provided for Access or
  Traffic state
 
  @param size_iq_gain_params
  This param conatils the size of CDMA IQ gain param table and
  is passed to RFLM to make sure that there is no mismatch
  between RF and FED.
 
  @return 
  TRUE on success and FALSE on failure
*/

boolean 
rf_1x_tx_agc_get_and_set_iqgain_params
(
  const rf_1x_config_type *rf_1x_config_ptr,
  boolean traffic_state 
)
{
    /* Tx IQ Gain data for different reverse link configurations */
  rf_1x_mdsp_tx_apt_adj_type tx_iq_gain_params ;

  /* Get the IQ gain params from NV */
  boolean ret_val = rf_1x_tx_agc_get_iqgain_params ( &tx_iq_gain_params , 
                                                     rf_1x_config_ptr, 
                                                     traffic_state ) ;
  if ( ret_val == TRUE )
  {
    /* Send the params to FED now */
    ret_val = 
      rf_cdma_mdsp_rflm_set_iqgain_params ( RFM_1X_MODE , 
                                            &(tx_iq_gain_params.rc[0]) ,
                                            sizeof (tx_iq_gain_params) ) ;
  }
  return ret_val ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return 1x TxAGC Configuration

  @details
  This function will configure 1x TxAGC. This is configure SBI information
  required for TxAGC operation. All TxAGC-related paramaters are expected to
  be configured in this API.
  
  @param device
  RF Device on which AGC is to be configured.

  @param rf_band
  Band to which device is tuned
 
  @param rf_channel
  Channel to which device is tuned
 
  @param txlm_handle
  TxLM handle for the LM chain on which device is tuned
 
  @param plim_data
  Pointer to the power limiting data for current device and mode
 
  @param rf_1x_config_ptr
  Pointer to 1x NV configuration
 
  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_1x_configure_tx_agc
(
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type const rf_band,
  uint16 const rf_channel,
  lm_handle_type txlm_handle,
  const rf_1x_config_type *rf_1x_config_ptr
)
{
  boolean result = TRUE; /* final result of API */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r;
  const rf_cdma_tx_static_type *tx_cdma_config_ptr =
    &(rf_1x_config_ptr->cdma_common_ptr->tx_static);

  /* Structure to hold PA State Machine Data */
  rf_cdma_mdsp_pa_stm_config_type pa_stm_data;
  /* holds the time spent in this function */
  rf_time_tick_type time_spent_in_func; 
  rf_time_type time_spent_in_func_val;

  /* Tx Operation mode to be provided as part of Tx AGC Configuration */
  rf_cdma_mdsp_tx_mode_type tx_agc_mode;
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
  /* Start Profiling */
  time_spent_in_func= rf_time_get_tick();

  /* Device Status Write Pointer */
  dev_status_r = rf_cdma_get_device_status( device );
  
  /* NULL pointer check*/
  if( dev_status_r == NULL )
  {
      RF_MSG_1( RF_ERROR,"rf_1x_configure_tx_agc: NULL data for device %d",
                device );
      return FALSE; 
  }/* if(  dev_status_w == NULL ) */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_configure_tx_agc: "
                      "Null pointer returned to logical_dev ");
    return FALSE;
  }

  modem_chain = rfc_cdma_get_tx_modem_chain ( device , rf_band ) ;

  /* memset everything in pa_stm_data to 0 before use*/
  memset( &pa_stm_data, 0, sizeof(rf_cdma_mdsp_pa_stm_config_type) );

  /* Update the PA Params based on device status */
  result &= rf_1x_tx_agc_update_pa_params ( device,
                                            rf_band,
                                            rf_channel,
                                            rf_1x_config_ptr,
                                            &pa_stm_data );

  /* get msm-specific TX AGC parameters */
  result &= rf_1x_get_tx_agc_params( device,
                                     rf_band,
                                     modem_chain ,
                                     &pa_stm_data ); 

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

  /* Get IQ gain scaling for different reverse link configurations */
  result &= 
    rf_1x_tx_agc_get_and_set_iqgain_params ( rf_1x_config_ptr,
                     dev_status_r->traffic_state );

  /* Determine High/Low band for FW TxAGC logging */
  is_high_band = rf_cdma_is_high_band( rf_band );

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
	  RF_MSG( RF_MED,"rf_1x_configure_tx_agc: Skipping linearizer and "
					 "related updates for this temp comp" );
   }

  /* continue with configuration routines only if pre-config steps were
     successful */
  if ( result == TRUE )
  {
    if( rfm_get_calibration_state() == TRUE )
    {

      /* Set AGC mode to cal operation calibration state. FW will not
      run AGC if this flag is set */
      tx_agc_mode = RF_CDMA_MDSP_TX_MODE_CAL_OVERRIDE;

      /* Never want Stop tx AGC to be set before wake up Tx */
      result &= rf_1x_mdsp_stop_txagc_update( FALSE );

      if( result == FALSE )
      {    
        RF_MSG( RF_ERROR, "rf_1x_configure_tx_agc: "
                          "Failed to Reset FW Stop TxAGC Flag ");
      }
    }
    else if ( config_linearizer == TRUE )
    {
      /* Set AGC mode to normal operation when not in calibration state */
      tx_agc_mode = RF_CDMA_MDSP_TX_MODE_NORMAL;

      /* Populate LUT and power tables into tx_lin_data */
      /* Uses tx_multi_lin_cal NV data to load linearizers */
      /* Populate tx_lin_data with PA Q-current RFFE data */
      result &= rf_cdma_tx_agc_load_linearizer( RFM_1X_MODE,
                                                device, rf_band, rf_channel, 
                                                TX_LINEARIZER_VOICE,
                                                &pa_stm_data,
                                                txlm_handle );
      if ( result != TRUE )
      {
        RF_MSG ( RF_ERROR , "rf_1x_configure_tx_agc(): "
                       "Call to rf_cdma_tx_agc_load_linearizer() has failed " ) ;
      }
    }
    else
    {
      /* Set AGC mode to normal operation when not in calibration state */
      tx_agc_mode = RF_CDMA_MDSP_TX_MODE_NORMAL;
    }

    /* Configure TxAGC. This funtion should be called after LUT is programmed
    to the SMEM, since FW latches the LUT immediately after this message.
    The APT PDM LUTs are programmed to SMEM as part of configuring the Tx
    linearizer. The LUT is latched into an internal buffer from SMEM
    once APT is enabled using this message*/
    if ( result == TRUE )
    {
      /* Initialize the xpt config params */
      xpt_config_params.apt_iq_gain_nominal = 
                                             dev_status_r->cdma_iq_gain_val;
      xpt_config_params.ept_iq_gain_nominal = dev_status_r->ept_iq_gain_val;
      xpt_config_params.et_iq_gain_nominal = dev_status_r->ept_iq_gain_val;
      xpt_config_params.et_env_scale_nominal = 
               rf_cdma_xpt_get_cfg_param(device, ET_CFG_CMN_ENV_SCALE_UNITY);
      xpt_config_params.cdma1x_pred_clip_thresh = 
               rf_cdma_xpt_get_cfg_param(device, XPT_CFG_CDMA_CLIP_THR_1X);

      /* Populate pred_clip on/off threshold. Must be called before 
      mdsp_configure_tx_agc*/
      result &= rf_cdma_txagc_get_pred_clip_thresh(rf_band,
                                                   &pa_stm_data, 
                                                   &xpt_config_params );

      result &= rf_1x_mdsp_configure_tx_agc( 
               logical_dev->tx_mdsp_path, 
               &pa_stm_data, tx_agc_mode, 
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
                             RFM_1X_MODE,
                             txlm_handle, 
                             rf_cdma_xpt_get_cfg_param(device, XPT_CFG),
                             &pa_stm_data );
  
      /* Get current log data */
      log_data = rfcommon_core_txlin_get_log_data(RFCOM_1X_MODE,device);

      if (NULL == log_data)
      {
        RF_MSG( RF_ERROR, "rf_1x_configure_tx_agc: "
                         "Null pointer returned to log_data from TxAGC ");
        return FALSE;
      }
      else
      {
       /* Use a max linearizer limit */
       rf_1x_plim_update_lin_limit( device, log_data->max_calibrated_power );
      }
      }/* if ( config_linearizer == TRUE )  */
   }/* if ( result == TRUE ) */

    /* Clear all the overrides. Overrides shoul;d be cleared only if we are
    not in Calibration State. This is required because in calibration mode,
    overrride might be set before configuring TxAGC. Thus these overrides should 
    not cleared for proper Tx calibration.*/

    /* Only for Freq comp will do override. For temp comp, it is not */
    if ( ( tx_agc_mode == RF_CDMA_MDSP_TX_MODE_NORMAL) && 
         (dev_status_r->temp_comp.init_trigger == TRUE) && 
         (rf_cdma_debug_flags.bolt_rumi == FALSE) )
    {
      rf_1x_mdsp_tx_closed_loop_clear_override( logical_dev->tx_mdsp_path,
                                                NULL, NULL, NULL );
      rf_1x_mdsp_tx_agc_output_clear_override( logical_dev->tx_mdsp_path,
                                               NULL, NULL, NULL );
      rf_1x_mdsp_tx_open_loop_clear_override( logical_dev->tx_mdsp_path,
                                               NULL, NULL, NULL );
      rf_1x_mdsp_pa_range_clear_override( logical_dev->tx_mdsp_path, NULL,
                                          NULL, NULL );
      rf_1x_mdsp_pa_state_clear_override( logical_dev->tx_mdsp_path,
                                          NULL, NULL, NULL );

      RF_MSG( RF_MED,"rf_1x_configure_tx_agc: Clearing Override as" );

    }
    else
    {
      RF_MSG( RF_MED,"rf_1x_configure_tx_agc: Not clearing Override as"
              "triggered by temp comp" );
    }

    /* --------------------------- FBRx Temp Comp ----------------------- */
    /* Query therm bins from tx temp comp NV */
    tx_therm_bins = rf_1x_config_ptr->cdma_common_ptr->
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
        RF_MSG_1( RF_ERROR, "rf_1x_configure_tx_agc: "
                            "FBRx Temp Comp failed %d", fbrx_temp_comp_result);
      } 
    }
    else
    {
      RF_MSG( RF_ERROR, "rf_1x_configure_tx_agc: Therm bins is NULL!");
    }

  }/* if ( result == TRUE ) */
  else
  {
    RF_MSG(RF_ERROR, "rf_1x_configure_tx_agc: Error occured in pre-config "
                     "steps - bailing out ");
  }

  /* get time elapsed in this function */
  time_spent_in_func_val = rf_time_get_elapsed( time_spent_in_func, 
                                                       RF_USEC );

  if( result == FALSE )
  {
    RF_MSG_3( RF_ERROR, "rf_1x_configure_tx_agc : FAILED for device %d band %d channel %d",            
              device, rf_band, rf_channel );
  }
  else
  {
    RF_MSG_3( RF_MED, "rf_1x_configure_tx_agc : device %d time_taken = %d, result = %d ",            
              device, time_spent_in_func_val, result );
  }

  return result;

} /* rf_1x_configure_tx_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update 1x Tx PA Params configuration

  @details
  This function will configure 1x TxAGC with updated PA Params. 
  
  @param device
  RF Device on which Tx PA params are to be configured.

  @param rf_band
  Band to which device is tuned
 
  @param rf_channel
  Channel to which device is tuned
 
  @param rf_1x_config_ptr
  Pointer to 1x NV configuration
 
  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_1x_tx_agc_update_pa_params
(
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type const rf_band,
  uint16 rf_channel,
  const rf_1x_config_type *rf_1x_config_ptr,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data
)
{
  boolean result = TRUE; /* final result of API */
  const rf_cdma_data_status_type *dev_status_r;
  const rf_1x_tx_static_type *tx_1x_config_ptr;
  rf_cdma_tx_static_type *tx_static_struct;
  /* Ptr to Tx switchpoints */
  const rf_cdma_tx_switch_points_type *tx_1x_swpts_ptr;
  rfm_pa_gain_state_type pa_state;
  uint8 num_active_pa_states = 0;

  /* Storage for relevant 1x device status data */
  boolean voice_call = TRUE;
  rf_1x_tx_agc_sch_rate_type sch_rate;

  /* Device Status Write Pointer */
  dev_status_r = rf_cdma_get_device_status( device );
  if ( dev_status_r == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_tx_agc_update_pa_params: "
                      "Null pointer returned to dev_status ");
    return FALSE;
  }

  if ( rf_1x_config_ptr != NULL )
  {
    tx_static_struct = &(rf_1x_config_ptr->cdma_common_ptr->tx_static);
    /* Pointer to 1x static NV data */
    tx_1x_config_ptr = &(rf_1x_config_ptr->tx_static);
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_1x_tx_agc_update_pa_params: "
                      "Null pointer returned to 1x NV pointer ");
    return FALSE;
  }

  /* Obtain the switchpoints in dBm10 */
  if ( dev_status_r->traffic_state )
  {
    voice_call = dev_status_r->data_1x.is_voice_call;
    if ( voice_call == TRUE )
    {
      /* Use voice switchpoint configuration */
      tx_1x_swpts_ptr = &tx_1x_config_ptr->voice_swpts;
    }
    else
    {
      /* SCH is on. Use data switchpoint configuration */
      if ( dev_status_r->data_1x.sch_rate < RF_1X_TX_AGC_SCH_RATE_NUM )
      {
        sch_rate = dev_status_r->data_1x.sch_rate;
      }
      else
      {
        sch_rate = RF_1X_TX_AGC_SCH_FULL_RATE;
      }
      tx_1x_swpts_ptr = &tx_1x_config_ptr->data_swpts_rsch[sch_rate];
    }        
  }
  else
  {
    /* We're not in traffic, but in access state. Program the access
    switchpoints */
    tx_1x_swpts_ptr = &tx_1x_config_ptr->access_swpts;
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
                                   tx_1x_swpts_ptr->rise[pa_state];
    /* Populate PA Fall Val */
    pa_stm_data->fall_val[pa_state] = 
                                   tx_1x_swpts_ptr->fall[pa_state];
  }

  /* Populate PA Hysterisis Timer */
  pa_stm_data->pa_hyst_timer = tx_1x_swpts_ptr->fall_hold_time;
  /* Populate Predictive clipper power threshold */
  pa_stm_data->pred_clip_en_thresh = tx_static_struct->pred_clip_nv.enable_threshold;
  pa_stm_data->pred_clip_dis_thresh = tx_static_struct->pred_clip_nv.disable_threshold;

  RF_MSG_8( RF_LOW, "rf_1x_tx_agc_update_pa_params: "
            "PA SP [F0 %d, F1 %d, F2 %d, F3 %d, "
            "F4 %d, F5 %d, F6 %d, F7 %d] dBm10", 
            tx_1x_swpts_ptr->fall[RFM_PA_GAIN_STATE_0], 
            tx_1x_swpts_ptr->fall[RFM_PA_GAIN_STATE_1], 
            tx_1x_swpts_ptr->fall[RFM_PA_GAIN_STATE_2], 
            tx_1x_swpts_ptr->fall[RFM_PA_GAIN_STATE_3],
            tx_1x_swpts_ptr->fall[RFM_PA_GAIN_STATE_4],
            tx_1x_swpts_ptr->fall[RFM_PA_GAIN_STATE_5],
            tx_1x_swpts_ptr->fall[RFM_PA_GAIN_STATE_6],
            tx_1x_swpts_ptr->fall[RFM_PA_GAIN_STATE_7] );
  RF_MSG_8( RF_LOW, "rf_1x_tx_agc_update_pa_params: "
            "PA SP [R0 %d, R1 %d, R2 %d, R3 %d, "
            "R4 %d, R5 %d, R6 %d, R7 %d] dBm10", 
            tx_1x_swpts_ptr->rise[RFM_PA_GAIN_STATE_0], 
            tx_1x_swpts_ptr->rise[RFM_PA_GAIN_STATE_1], 
            tx_1x_swpts_ptr->rise[RFM_PA_GAIN_STATE_2], 
            tx_1x_swpts_ptr->rise[RFM_PA_GAIN_STATE_3],
            tx_1x_swpts_ptr->rise[RFM_PA_GAIN_STATE_4],
            tx_1x_swpts_ptr->rise[RFM_PA_GAIN_STATE_5],
            tx_1x_swpts_ptr->rise[RFM_PA_GAIN_STATE_6],
            tx_1x_swpts_ptr->rise[RFM_PA_GAIN_STATE_7] );

  return result;

} /* rf_1x_tx_agc_update_pa_params */

#endif /* FEATURE_CDMA1X */
/*! @} */
