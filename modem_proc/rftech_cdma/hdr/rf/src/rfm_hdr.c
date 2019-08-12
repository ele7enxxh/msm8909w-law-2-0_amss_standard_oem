/*! 
  @file
  rfm_hdr.c
 
  @brief
  This file defines the RFM-HDR interfaces to be used by external module.
 
  @addtogroup RF_CDMA_HDR_RFM_HDR
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/src/rfm_hdr.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/16/16   vr      club 1X2G/Do2G QTA changes
12/01/14   as/spa  Protected tuner configuration under ASDiv lock
                   Skip update tune code if already under progress
11/12/14   spa     Change tx_nv_path to be per band per logical device
10/13/14   vr      CW Fix
10/07/14   vr      FTM IQ capture changes
09/30/14   ndb     Pass LM_handle in tuner_cb to help with debug
08/28/14   kg      Pass tech_id in tuner_cb to help with debug
07/16/14   fh      No rf state update for cleanup script
06/28/14   fh      Added api for cleanup script
05/12/14   spa     Initialize event handle to NULL before use in AOL
05/02/14   spa     Moved API rfm_hdr_query_iq_sample_line_offset to rfm_hdr_ftm
04/17/14   sty     Call rf_hdr_mdsp_get_tx_agc_logical_data() only in TX mode
04/01/14   APU     PA params update hook-up for different RC/data rate configs.
04/01/14   APU     PA params update hook-up for different RC/data rate configs.
03/28/14   APU     Hook up Tx logging interfaces in SW driver to log data 
                   provided by RFLM.
03/18/14   APU     1> Have DM do the buffer deallocation.
                   2> Centralize allocation and association to 1 place.
03/13/14   spa     Skip xpt update when going from access to voice
02/10/14   cd      Access probe config does not reconfigure entire Tx AGC
01/15/14   JJ      Added api tp get mempool addr from FW shared memory for MCDO
01/06/14   zhw     Access Probe API documentation update
01/06/14   zhw     HDR Tuner AOL support. Use new Tuner AOL driver APIs.
01/03/14   hdz     Set temp_comp.init_trigger to be FALSE when doing access probe
11/20/13   sty     Removed redundant debug flag
11/11/13   JJ      Removed two funcs to set/get fw smem addr
10/21/13   JJ      Added two funcs to set/get fw smem addr
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
10/10/13   zhw     Skip AOL update in Rx only mode.
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
07/02/13   JJ      Enable hdr retune while TX is on
06/28/13   spa     Remove is_irat_mode flag from enable diversity API  
06/27/13   spa     Add 'is_irat_mode' flag to APIs to check if API call is in 
                   IRAT or non IRAT scenarios 
06/14/13   spa     Use CDMA wrapper object for antenna tuner 
06/02/13   JJ      HDR sleep time profiling 
05/29/13   zhw     Remove redundant filter power F3.
05/25/13   zhw     Added support for HDR Tuner AOL scripting.
05/23/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/13/13   sty     Added initial implementation for 
                   rfm_hdr_update_rev_link_carrier_set()
05/08/13   APU     Removed distracting messages for 1x_filtered_power() APIs 
                   due to high frequency of calls from MCS\L1   
05/02/13   Saul    Added debug messages.
04/16/13   APU     Changed the name of rf_hdr_tx_read_filtered_power() API to 
                   rf_hdr_get_filtered_tx_power_v2() 
04/14/13   APU     Added another get_filtered_tx_power API that can query 
                   different kind of filters.
04/10/13   ndb     Removed the unused code.
04/05/13   APU     Hooked the get_filtered_power APIs.
03/19/13   zhw     Fix compiler error
02/15/13   bmg     Antenna tuner periodic update support.
01/30/13   APU     Brought over Enhanced PA backoff code from Nikel.
01/28/13   APU     Implemented rfm_hdr_get_filtered_rx_power() API.
01/16/13   Saul    CDMA. Enabled EPT/ET tempcomp.
01/15/13   Saul    CDMA. HDR EPT/ET call support.
12/19/12   adk     KW fixes
12/11/12   Saul    CDMA. TX AGC XPT Dev. Get max PA state based on lin NV.
10/31/12   adk     Merged antenna tuner code from Nikel.
10/02/12   bmg     Added filtered Rx and Tx power query APIs
09/28/12   zhw     Fix compiler warning
09/27/12   zhw     Remove write_sbi/grfc_via_rfsw flag
08/30/12   bmg     Log HDR IQ data after AGC acquisition is done
08/28/12   aro     Interface for antenna tuner action time
08/23/12   aro     Migrated to percise RF time profiler for RFWU Profiler
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
07/26/12   APU     Added stub for Enhanced PA max power support.
06/29/12   spa     Compiler warning fix  
06/07/12   aro     Change to hold the CDMA lock until RxAGC has converged for
                   enable_diversity to avoid conflict with ACQ_2_TRK_IND message
                   for on-going intelliceiver operation
05/29/12   zhw     Refactored API naming. 
05/29/12   zhw     Moved debug flag do_grfc_write to rf_cdma_data 
05/25/12   spa     Return RFM_CDMA_ERROR_NULL_DATA for any NULL pointers/data 
05/23/12   zhw     Moved debug flag do_sbi_write to rf_cdma_data 
05/21/12   aro     Fixed the PA State query
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
05/03/12   nv      Fixed KW warning
04/27/12   zhaow   Fixed formatting.
04/26/12   zhaow   Updated data_hdr param assignment statement with set APIs.
04/26/12   zhaow   Enabled band and channel validity check in hdr prep_wake_rx 
                   and retune functions
04/26/12   aro     Added generic RFM CDMA API status return type
04/20/12   aro     Unified interface to return TxAGC State parameter
04/18/12   spa     Added rf_hdr_log_wakeup_rx to log HDR warmup time 
04/09/12   aro     Check to make sure that there is no diversity device active
                   while doing primary device sleep
04/09/12   aro     Release CDMA Lock before starting RxAGC convergence
04/06/12   bmg     Corrected HDR Tx AGC return for RFM_CDMA_CARRIER_CUMULATIVE.
03/29/12   sty     use do_grfc_writes_in_rf_sw instead of DO_GRFC_WRITES_IN_RF_SW
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/21/12   APU     Added rf_hdr_get_max_tx_power_for_band() API 
03/15/12   spa     Added max device check for rf_cdma_get_device_status() 
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan  
03/01/12   cd      Fixed compiler warning resulting from unused code 
02/29/12   cd      - Added support for rfm_hdr_notify_data_rate()
                   - Cleaned up legacy band nv tbl reference
                   - Moved nominal power to new HDR data type inside CDMA data
02/23/12   spa     Updated the type of band/chan to be rfm_cdma_band_class_type
                   and rfm_cdma_chan_type for rf_1x_mc_enable
02/09/11   APU     Add the temp API to be removed ASAP. 
                   rfm_hdr_get_highest_tx_max_power_limit_for_current_band()
02/09/12   Saul    MCDO. Tx AGC uses MCDO switch-points.
01/19/12   cd      Configure Tx AGC uses PLIM API, hence removed argument
01/03/12   sty     Added RFM_DEBUG_EN_Q to rfm APIs
12/21/11   aro     Code beautification
12/21/11   sty     fixed warnings
12/16/11   Saul    IRAT. D2L HDR Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
12/13/11   aro     Check in HDR Enable/Disable to see if all required 
                   tasks are running
12/13/11   aro     Check for RFM APIs to see if all required tasks are running
12/10/11   sty     Debug message enhancements 
12/02/11   cd      Added support to configure Tx AGC for access and traffic with 
                   new switchpoints  
11/15/11   adk     Added support to send RTR SBI scripts to FW selectively.
11/14/11   hdz     Called rf_hdr_mc_disable() in rfm_hdr_disable() to reset
                   rf_cdma_data.rf_device_status
11/14/11   Saul    Thermal Backoff. Made implementation more modular.
11/11/11   Saul    Thermal Backoff Support.
09/29/11   aro     Added F3 messages for debug assistance
09/21/11   adk     Removed obsolete code.
09/20/11   aro     Replaced the acquire-lock function to CDMA function
09/13/11   sty     Fixed Tx logging APIs and corrected carrier bound checks
09/13/11   sty     Minor changes in rfm_hdr_retune()
09/12/11   sty     Reworked rfm_hdr_sleep_rx() and rfm_hdr_sleep_tx()
09/07/11   aro     Updated RX_STOP interface to pass the RTC time to stop Rx
09/07/11   shb     Reworked TX AGC Logical data to have carrier and cumulative 
                   total power
08/31/11   shb     Added plim/hdet stop calls in hdr tx sleep
08/29/11   aro     Added implementation for  PA Access probe config
08/12/11   aro     Fixed diversity calibration crash
08/11/11   sty     Added time profiling for RF WU on 1x and HDR
08/11/11   sar     Code review changes. 
08/11/11   sty     Return nominal TC in dB units as required by L1 from 
                   rfm_hdr_get_turnaround_info() 
08/11/11   aro     Added support to allow RF_CDMA_STATE_RX_PREP to 
                   RF_CDMA_STATE_SLEEP transition
08/11/11   sar     Fixed KW errors.
08/05/11   sty     Fixed RFM logging APIs
08/02/11   adk     Updates to ensure that RTR handle is set before it is used.
07/28/11   aro     Removed hardcoding to carrier in Tx query interface
07/23/11   Saul    Converted rx wake up APIs to use GRFC generate script.
07/18/11   sty     Removed mode check from rfm_hdr_get_turnaround_info() since 
                   it is already checked in  rfm_hdr_validate_state()
07/15/11   sty     Added implementation for rfm_hdr_get_turnaround_info
07/14/11   bmg     Changed the checks of the MC functions that return time/
                   error codes to remove RFM_CDMA_HEALTHY_STATE, to avoid
                   implying a state is returned.
07/13/11   aro     Updated intelliceiver interfaces not to have device param
07/11/11   cd      Deleted rf_hdr_grfc functionality
07/07/11   sty     Fixed minor bug in rfm_hdr_set_tx_power_adjust
07/06/11   aro     Updated Lock interfaces to pass the Lock Data
07/05/11   aro     Moved the check of iC support to MC
06/23/11   sty     Fixed compiler warnings
06/23/11   sty     Moved implementation of RFM APIs to MC layer
06/21/11   sty     Code cleanup
06/21/11   Saul    Updates for multi-carrier driver.
06/20/11   aro     Changed function signature of intelliceiver Stop to pass
                   the intelliceiver data and logical device param
06/20/11   aro     Changed function signature of intelliceiver Start to pass
                   the intelliceiver data and logical device param
06/17/11   Saul    Warning fixes that were not previously reported.
06/16/11   adk     Updates to provide buffer_index to FW with SBI scripts.
06/16/11   sty     Moved steps in rfm_hdr_prep_wakeup_rx and 
                   rfm_hdr_exec_wakeup_rx to MC layer
06/16/11   Saul    Fixed HDR diversity.
06/16/11   sar     Included internal rf hdr and common cdma interfaces.
06/15/11   sty     Fixed bug in enable_diversity
06/15/11   sty     Display RxAGC in dBm10
                   Deleted unused #defines
06/14/11   sty     Invoke APIs to convert AGC to requested format
                   Added format arg to agc query APIs
                   Added implementation for rfm_hdr_get_lna_gain_state
06/14/11   Saul    Now configuring CSR freq offsets.
06/13/11   Saul    Now computing center channel for tuning.
06/07/11   sty     Changed return type of rfm_hdr_validate_state() and made
                   related changes 
06/13/11   sty     Added implementation for TxAGC lookup APIs
06/10/11   Saul    Now configuring carriers based on mask.
06/09/11   Saul    Bug fixes for configuring rx agc.
06/09/11   Saul    RxLM updates for multi-carrier driver.
06/08/11   aro     Moved wakeup Tx implementation to HD MC
06/08/11   Saul    Updates for multi-carrier driver.
06/07/11   Saul    Updates for multi-carrier driver.
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/06/11   aro     [1] Added support for simultaneous Tx support
                   [2] Renamed Load GRFC script to sleep/wakeup grfc script
06/06/11   aro     Renamed Scipt Manager header file to GRFC
05/24/11   aro     Added Logical Device Mapping to MDSP Chain / NV for SV
05/26/11   sty     Init rtr_handle to -1 as a temp workaround for FW  
05/22/11   adk     Use modem_mem_alloc/_free() to allocate memory for SBI scripts
05/21/11   aro     Merged Rx Spectral inversion into configure Rxlm function
05/21/11   aro     Moved the Spectral Inversion Call to Prep
05/21/11   aro     Removed obsolete header files
05/20/11   aro     Fixed Rx Spectral Inversion Bug
05/18/11   aro     Added interface to configure spectral inversion
05/17/11   sty     Check for correct state in rfm_hdr_exec_wakeup_rx()
05/13/11   sty     Removed online_flag from rf_cdma_msm_configure_txlm
05/05/11   sty     Deprecated rfm_hdr_wakeup_rx()
05/02/11   sty     Fixed warning
04/29/11   sty     Updated documentation for rfm_hdr_wakeup_exec_rx() 
04/28/11   sty     Invoke rfc_cdma_manage_vregs() correct args
04/28/11   sty     used new APIs for enable and disable diversity 
04/28/11   sty     Implemented enable and disable diversity
04/26/11   aro     Removed Enable/Disable Tx interfaces
04/25/11   sty     Added calibration state in call to rf_cdma_msm_configure_txlm
04/22/11   sty     Deleted unused variable carrier_mask in rfm_hdr_sleep_rx()
04/21/11   aro     Updated CDMA Data interface for RF Calibraiton
04/21/11   aro     Updated CDMA Device Data to have read and mutable interfaces
04/21/11   aro     [1] Moved ADC Clock Manager to HAL
                   [2] Moved RxAGC implementation to RFM
04/21/11   aro     Added rfcommon_locks.h
04/21/11   aro     Renamed LOCKs function to move to Common area
04/17/11   sty     Added mode info to rf_cdma_msm_configure_rxlm()
                   Update Rx and TxLM handles in sleep and wakeup APIs
04/15/11   sty     Moved RUMI bypasses to MC layer
04/14/11   sty     Bypass GRFC and RTR writes for RUMI bringup
04/12/11   aro     Deleted redundant activate RXLM call
04/12/11   aro     Re-sequenced Calls in Wakeup and Sleep functions
04/11/11   aro     Introduced Rx Stop calls for retune and sleep functions
04/11/11   aro     Compiler Warning Fix
04/11/11   aro     [1] Added new Wakeup Rx Prepare/Execute Interfaces
                   [2] Renamed Start rx interface
04/08/11   aro     Added interface for RxAGC Start
04/04/11   aro     Added interface to query HDR Sleep state
03/31/11   sty     Invoke rf_cdma_msm_configure_txlm() instead of 
                   rf_cdma_txlm_configure_hdr()
03/25/11   sty     Added implementation for rfm_hdr_set_tx_power_adjust and
                   rfm_hdr_set_nominal_pwr()
03/24/11   aro     Unified the RxAGC Read Interface
03/24/11   aro     Added Analog script Programming script Interface
03/24/11   aro     Updated HDR RxAGC Configure interface
03/21/11   sty     Added band_chan as arg in rf_hdr_configure_tx_agc
03/11/11   aro     Moved CB to execute CB for all cases
03/10/11   aro     Added CB execution for HDR enable and disable
03/10/11   aro     Enable Diversity Parameter Validation Bug fix
03/09/11   aro     Added placeholder for Enable Diversity
03/09/11   aro     [1] Added critical section protection
                   [2] F3 Message Update
03/09/11   aro     Re-structured RFM APIs to have single return point
03/09/11   aro     Renamed interface for consistency
03/08/11   aro     Fixed bad State update in HDR Rx Sleep
03/07/11   aro     Fixed the Error Check for HDR retune
03/07/11   aro     Compiler Warning Fix
03/03/11   aro     [1] Added support for HDR retune
                   [2] Added RxAGC Stop in HDR Sleep
02/18/11   sty     Use timetick_type for time-profiling
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/17/11   sty     Renamed rfc_card_cdma.h to rfc_cdma.h
02/17/11   aro     Renamed intelliceiver function
02/15/11   sty     Updated doxygen comments and debug message format
                   Deactivate Rx and TxLM in sleep APIs
                   Turn off VREGs only after all operations are completed in 
                   sleep APIs
02/14/11   sty     Fixed warnings and compile error
02/14/11   sty     Call rf_cdma_txlm_configure_hdr() with TxLM handle as arg
02/13/11   aro     Added MC support for HDR Wakeup Rx Interface
02/07/11   sty     Added debug messages in rfm APIs
02/04/11   sty     Added more checks in rfm_hdr_sleep_rx and rfm_hdr_sleep_tx
02/04/11   sty     [1] Renamed rf_hdr_mdsp_activate_device_rx_tune to
                       rf_hdr_mdsp_push_rfdevice_script
                   [2] Updated doxygen comments
                   [3] Added rfm_hdr_sleep_rx and rfm_hdr_sleep_tx
02/04/11   sty     Uncommented call to rf_hdr_mdsp_activate_device_tx_tune
                   Fixed warnings
02/03/11   sty     Added support for rfm_hdr_wakeup_tx
02/03/11   aro     Moved new Device interface to separate file
02/01/11   aro     Promoted Power Mode types to RFM
01/31/11   aro     Fixed F3 Message
01/27/11   aro     Documentation Update
01/27/11   aro     Added RF Warm up Measurement
01/24/11   aro     Changed the Mode to 1x for Device Tune function
01/23/11   aro     Added Support for HDR RxAGC Query
01/20/11   aro     [1] Added Mode Param to Device Inteface
                   [2] Replace Messges with RF Message Macro
01/19/11   aro     Removed MDSP Push Interface for Device
01/19/11   aro     Updated the HDR API with link manaager parameters
01/19/11   aro     Updated RxAGC configuration interface
01/19/11   aro     Renamed RF-MDSP filename
01/18/11   aro     Added initial support for HDR wakeup Rx
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/14/10   aro     Removed HDR Get State function
12/06/10   aro     Initial Revision.

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_hdr.h"
#include "rfm_cdma.h"
#include "rfm_internal.h"
#include "rf_cdma_data.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_intelliceiver.h"
#include "rf_cdma_hdet.h"
#include "rf_hdr_power_limiting.h"
#include "rfc_cdma.h"
#include "rf_hdr_mdsp.h"
#include "rf_hdr_rx_agc.h"
#include "rf_hdr_tx_agc.h"
#include "rfdevice_cdma_interface.h"
#include "rfcommon_msg.h"
#include "rfcommon_locks.h"
#include "rfcommon_time_profile.h"
#include "modem_mem.h"
#include "rf_hdr_mc.h"
#include "rf_cdma_mc.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "rfm_helper_incs.h"
#include "rf_cdma_mdsp_types.h"
#include "log_codes.h"
#include "rf_hdr_log.h"
#include "rfcommon_log.h"
#include "rf_cdma_constants.h"
#include "rfcommon_tuner.h"
#include "rfcommon_mdsp_tuner.h"

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_manager.h"
#endif


/*============================================================================*/
/*!
  @name CDMA HDR subsystem RFM Helper Functions

  @brief
  Helper Functions for HDR RFM interface. These interfaces are not exported
  to other modules. They shall be strictly used by RFM 1x functions only.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return type for the rfm_hdr_validate_state API. A negative value indicates
  that the function failed.  
*/ 
typedef int32 rfm_hdr_validate_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function for validity check on a list of band_chan. Not to be 
  externalized
 
  @param device
  Device to be checked
 
  @param num_band_chan
  The length of the band_chan parameter array.
 
  @param band_chan_array
  An array of band and channel structures for tuning multiple
  carriers in 1xEV-DO revB multi-carrier mode.
*/

boolean
rf_hdr_channel_list_is_supported
(
  const rfm_device_enum_type device,
  const uint32 num_band_chan,
  const sys_channel_type * const band_chan_array
)
{
  uint32 c_idx; /*counter for channel index*/

  /* Check band and channel validity for each entry in the array. If 
  anyone fails, return false */ 
  for( c_idx = 0; c_idx < num_band_chan; c_idx++ ) 
  { 
    if ( rfm_cdma_is_band_chan_supported( device, band_chan_array[c_idx] ) 
         == FALSE ) 
    {
      return FALSE;
    }
  }
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Validate the RF HDR State 

  @details
  This function compares the CDMA internal variables against the expected state
  to indicate the calling RFM function if it is safe to proceed with execution
  of the RFM functionality.

  @param device
  The RF device on which the validation is performed.

  @param exp_state
  The expected RF state.

  @retval RFM_CDMA_ERROR_INVALID_DEV_STATE
  Error code when trying to tune the radio when RF Dev is in Invalid State
 
  @retval RFM_CDMA_ERROR_INVALID_MODE
  Error code when trying to tune the radio in an invalid Mode
 
  @retval RFM_CDMA_HEALTHY_STATE
  when expected and current state of the device are the same
 
  @return
  Status of RF state. Negative value refers to bad state.
*/
static rfm_hdr_validate_t
rfm_hdr_validate_state
(
  rfm_device_enum_type device,
  rf_cdma_state_type exp_state
)
{
  rfm_mode_enum_type curr_mode;
  rfm_hdr_validate_t return_val = RFM_CDMA_HEALTHY_STATE;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* Get Current Mode */
  curr_mode =  rfm_get_current_mode(device);

  /* Validate the current Mode */
  if ( curr_mode != RFM_1XEVDO_MODE )
  {
    RF_MSG_2( RF_ERROR, "rfm_hdr_validate_state: Bad Mode - %d on device %d ",
              curr_mode, device );
    return RFM_CDMA_ERROR_INVALID_MODE;   
  }

  /* Get the device Status for validation */
  dev_status_r = rf_cdma_get_device_status( device );


  if(  dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_validate_state: "
              "Null pointer returned to device %d ", device);
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* State Validation */
  if ( dev_status_r->data_hdr.rf_state != exp_state )
  {
    RF_MSG_5( RF_HIGH, "rfm_hdr_validate_state : Curr(Dev %d, Mode %d, "
              "State %d), Expected(Mode %d, State %d)", device, curr_mode,
              dev_status_r->data_hdr.rf_state, RFM_1XEVDO_MODE, exp_state );
    return RFM_CDMA_ERROR_INVALID_DEV_STATE;   
  }
  else
  {
    RF_MSG_5( RF_LOW, "rfm_hdr_validate_state : Curr(Dev %d, Mode %d, "
              "State %d), Expected(Mode %d, State %d)", device, curr_mode,
              dev_status_r->data_hdr.rf_state, RFM_1XEVDO_MODE, exp_state );
  }
  
  return return_val;

} /* rfm_hdr_validate_state */

/*! @} */

/*============================================================================*/
/*!
  @name CDMA HDR subsystem TRM Control Functions

  @brief
  Functions for TRM to enable and disable the HDR portion of the
  CDMA RF driver.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the HDR subsystem of the CDMA RF driver.

  @details
  Prepares the radio for HDR operation on an initial band and channel.
  The HDR subsystem is initialized in the Sleep state with the requested
  band and channel.

  By initialzing HDR with a band and a channel, this avoids the situation
  where "the driver is on but there is no known channel."  This also
  allows SVDO to be implemented correctly when 1x is in traffic and HDR
  needs to acquire or check paging on the primary chain.  By forcing HDR
  to be enabled with a channel, the driver can detect that wideband Rx
  mode is correct, and tune radio to the correct initial frequency.

  If 1x is already active and on an incompatible channel, this API
  call will return an error.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_HDR_ENABLE_COMPLETE event.  The function will attempt to give
  a best estimate of the time remaining in the return value.

  @param device
  The path to enable the HDR subsystem on.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param band_chan
  A structure containing the band and channel to initialize the path to.

  @param callback
  A function pointer that will be called when the enabling sequence
  is complete.  This function will be passed the RFM_HDR_ENABLE_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_hdr_enable()
  function return until the operation is complete.

  @todo
  Work out what the error return values are for this function.
*/
rfm_wait_time_t
rfm_hdr_enable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const sys_channel_type band_chan,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rf_time_type enable_time; /* TIme spent in function */
  rf_time_tick_type enable_tick; /* TIme spent in function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_hdr_validate_t device_state; /* return value of device_state check */
  boolean are_tasks_running = TRUE;
  rfm_cdma_band_class_type band;
  rfm_cdma_chan_type chan;

  /*Convert RF CDMA sys band and channel type to RF band and channel type*/
  band = rf_cdma_sys_band_type_to_rf_band_type(band_chan.band);
  chan = (rfm_cdma_chan_type)band_chan.chan_num;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start to Time Measurement */
  enable_tick = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_hdr_enable, device, rxlm_handle, &band_chan, &callback);

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_hdr_enable : START - Dev %d, Band %d, Chan %d", device,
            band, chan );

    /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_hdr_validate_state( device, RF_CDMA_STATE_SLEEP );
  
    /* assign the current device state to return value of API */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      ret_val = 0;
      /* Implmentation goes here */
  
     /* Flag Callback event as successful */
      cb_event = RFM_HDR_ENABLE_COMPLETE;
    }
    else
    {
      /* assign the state to return val of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_enable: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* End Time Measurement */
  enable_time = rf_time_get_elapsed( enable_tick, RF_USEC );

  /* update this component of RF WU time */
  rf_cdma_update_wup_time( device, enable_time );

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_5( RF_HIGH, "rfm_hdr_enable : [Status %d] - Dev %d, Band %d, "
            "Chan %d, EnableTime %d", ret_val, device, band, 
            chan, enable_time  );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_3( RF_ERROR, "rfm_hdr_enable: API FAILED. Dev %d, band %d, chan %d",
              device, band_chan.band, band_chan.chan_num );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;
} /* rfm_hdr_enable */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable the HDR subsystem of the CDMA RF driver.

  @details
  Deactivates the HDR portion of the CDMA RF driver.  This API should
  only be called if HDR is in the Sleep state.  If the radio was
  operating in wideband mode, it is reconfigured for narrowband mode.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_HDR_DISABLE_COMPLETE event.  The function will attempt to give
  a best estimate of the time remaining in the return value.

  @param device
  The path to disable the HDR subsystem on.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param callback
  A function pointer that will be called when the disabling sequence
  is complete.  This function will be passed the RFM_HDR_DISABLE_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_hdr_disable()
  function return until the operation is complete.

  @todo
  Work out what the error return values are for this function.
*/
rfm_wait_time_t
rfm_hdr_disable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rf_time_tick_type disable_time; /* TIme spent in function */
  rf_time_type disable_time_val = 0; /* TIme spent in function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_hdr_validate_t device_state; /* return value of device_state check */
  boolean are_tasks_running = TRUE;
  
  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start to Time Measurement */
  disable_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_hdr_disable, device, rxlm_handle, &callback, &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_hdr_disable : START - Dev %d", device );

    /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_hdr_validate_state( device, RF_CDMA_STATE_SLEEP );
  
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Implmentation goes here */
    
      /*Call mc layer hdr_disable handler */
      ret_val = rf_hdr_mc_disable( device, rxlm_handle );
    
      /* Flag Callback event as successful */
      cb_event = RFM_HDR_DISABLE_COMPLETE;
    }
    else
    {
      /* assign the state to return val of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_disable: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* End Time Measurement */
  disable_time_val = rf_time_get_elapsed( disable_time, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_3( RF_HIGH, "rfm_hdr_disable : [Status %d] - Dev %d, DisableTime %d",
           ret_val, device, disable_time_val  );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_disable: API FAILED. Dev %d",
              device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;
} /* rfm_hdr_disable */

/*! @} */


/*============================================================================*/
/*!
  @name HDR L1 Radio Control Functions

  @brief
  Functions for the HDR protocol stack to control the radio
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare to turn on the radio and tune to the given band and channel.

  @details
  Prepare the software to move the HDR reciever from the Sleep state to the Rx 
  state. Powers on all circuits required to recieve an HDR signal and 
  prepares all the scripts required to tune the radio to the requested channel.

  For optimal operation, this API should be called with the same band
  and channel the radio was prepared for in the rfm_hdr_enable(). This function 
  should not be used to re-tune the radio if it already awake.

  @image html rf_cdma_wakeup_rx_sequence.jpg

  @param device
  The receive path to wakeup.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param num_band_chan
  The length of the band_chan parameter array.

  @param band_chan_array
  An array of band and channel structures for tuning multiple
  carriers in 1xEV-DO revB multi-carrier mode.

  @param carrier_index
  An array in integers that specify the carrier the corresponding channel
  should be placed on in the multi-carrier Rx front.  This carrier will
  deliver samples into the corresponding sample RAM for demodulation.

  @param agc_mode
  A structure containing the AGC acquisition mode and any extra parameters
  that should be used during Rx wakeup.

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed the RFM_HDR_RX_WAKEUP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @retval RFM_CDMA_ERROR_INVALID_DEV_STATE
  Error code when trying to tune the radio when RF Dev is in Invalid State
 
  @retval RFM_CDMA_ERROR_INVALID_DEV
  Error code when trying to tune the radio in an invalid Device
 
  @retval RFM_CDMA_ERROR_FAILURE
  Error code when RF Fails to Tune inspite of good parameter and state

  @retval RFM_CDMA_ERROR_INVALID_MODE
  Error code when trying to tune the radio when RF Mode is not correct.
 
  @return
  The anticipated time in microseconds remaining from the rfm_hdr_wakeup_rx()
  function return until the operation is complete.
*/
rfm_wait_time_t
rfm_hdr_prep_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 num_band_chan,
  const sys_channel_type * const band_chan_array,
  const int32 * const carrier_index,
  const rfm_hdr_agc_acq_type* const agc_mode,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rf_time_type prep_wakeup_time;      /* prep-wakeup Time */
  rf_time_tick_type prep_wakeup_tick;      /* prep-wakeup Time */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_hdr_validate_t device_state; /* return value of device_state check */
  boolean are_tasks_running = TRUE;
  rfm_cdma_band_class_type band;/* Band  */
  rfm_cdma_chan_type chan_array[RFM_CDMA_CARRIER_CUMULATIVE] = {0,0,0};/* Channel array */ 
  uint32 c_idx; /*counter for channel index*/

  /* Number of bands is one */
  band = rf_cdma_sys_band_type_to_rf_band_type(band_chan_array[0].band);

  /* Populate the channel array*/
  for( c_idx = 0 ; c_idx < num_band_chan ; c_idx++ )
  {
    chan_array[c_idx] = (rfm_cdma_chan_type)band_chan_array[c_idx].chan_num; 
  }

  /* Start Profiling of this API */
  prep_wakeup_tick = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_hdr_prep_wakeup_rx, device, rxlm_handle, &band_chan_array, 
                 &agc_mode);

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_hdr_prep_wakeup_rx : START - Dev %d, Band %d, "
                     "Chan %d", device, band, chan_array[0] );

  /* If validity check passed, proceed. Else, send error msg and go to call back       
  */ 
  if ( ( rf_hdr_channel_list_is_supported( device, num_band_chan, 
                                         band_chan_array ) == TRUE ) &&
        ( num_band_chan <= RFM_CDMA_CARRIER_CUMULATIVE ) )  
  { 
    /* Acquire Critical Section Lock */
    rf_common_enter_critical_section(rf_cdma_get_lock_data()); 
    
    /* Query if all required tasks are running or not */
    are_tasks_running &= rf_cdma_required_tasks_running(device);
  
    /* Perform RF action, only if all required tasks are running */
    if ( are_tasks_running == TRUE )
    {
      /* Validate the Currente State */
      device_state = rfm_hdr_validate_state( device, RF_CDMA_STATE_SLEEP );
    
      /* Proceed only if state is valid, i.e. device_state is non-negative  */
      if ( device_state == RFM_CDMA_HEALTHY_STATE )
      {
        /* Populate the total wait time */
        ret_val = rf_hdr_mc_prep_wakeup_rx( device, rxlm_handle, num_band_chan,                      
                                            band, chan_array, carrier_index, 
                                            agc_mode, 
                                            FALSE, /* not in IRAT mode */
                                            RFDEVICE_MEAS_INVALID_SCRIPT 
                                          /* Not in IRAT set to invalid script*/);
    
        if ( ret_val >= 0 )
        {
          /* Flag Callback event as successful */
          cb_event = RFM_HDR_RX_WAKEUP_PREP_COMPLETE;
        }
      }
      else
      {  
        /* assign the current device state to return value of API */
        ret_val = device_state;
      }
    } /* if ( are_tasks_running == TRUE ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rfm_hdr_prep_wakeup_rx: Dev %d - Tasks not running",
                device );
    } /* if !( are_tasks_running == TRUE ) */
  
      /* Release Critical Section Lock */
      rf_common_leave_critical_section(rf_cdma_get_lock_data());
  
    } /*   if ( rf_hdr_channel_list_is_supported( device, num_band_chan, 
                                           band_chan_array ) == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, 
              "rfm_hdr_prep_wake_up_rx: Dev %d validity check failed", device);
    ret_val = RFM_CDMA_ERROR_INVALID_BAND_CHAN;
  } /*   if !( rf_hdr_channel_list_is_supported( device, num_band_chan, 
                                         band_chan_array ) == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }
  
  /* Record the execution time statistics for wakeup logging for this device*/
  prep_wakeup_time = rf_time_get_elapsed( prep_wakeup_tick, RF_USEC );

  /* update this component of RF WU time */
  rf_cdma_update_wup_time(device, prep_wakeup_time);

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_3( RF_HIGH, "rfm_hdr_prep_wakeup_rx: [Status %d] - Dev %d, "
            "prep_wakeup_time %d ", ret_val, device, prep_wakeup_time );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_2( RF_ERROR, "rfm_hdr_prep_wakeup_rx: API FAILED. Dev %d, num chans %d",
              device, num_band_chan );
  }

  /* Return the API Wait time */
  return ret_val;

} /* rfm_hdr_prep_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute Wakeup Rx sequence to turn on the radio and tune to the given band 
  and channel.

  @details
  This function will send a command to firmware to indicate that Rx needs to be
  started. The command will indicate FW to activate the RxLM configuration 
  along with the RTR script to be programmed. Based on the Settling time
  sent, FW will start the RxAGC as well.
 
  This API also executes the callback as soon as the above steps are executed. 
 
  When support for handling responses from FW is implemented in RFSW, this API
  will return control to HDR L1 as soon as commands are executed. The callback 
  function will then be moved to the command handler, which would execute the 
  callback when FW sends a response to start_agc command.
 
  @image html rf_cdma_wakeup_rx_sequence.jpg

  @param device
  The receive path to execeute wakeup rx sequence.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed the RFM_HDR_RX_WAKEUP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @retval RFM_CDMA_ERROR_INVALID_DEV_STATE
  Error code when trying to tune the radio when RF Dev is in Invalid State
 
  @retval RFM_CDMA_ERROR_INVALID_DEV
  Error code when trying to tune the radio in an invalid Device
 
  @retval RFM_CDMA_ERROR_FAILURE
  Error code when RF Fails to Tune inspite of good parameter and state

  @retval RFM_CDMA_ERROR_INVALID_MODE
  Error code when trying to tune the radio when RF Mode is not correct.
 
  @return
  The anticipated time in microseconds remaining from the rfm_hdr_wakeup_rx()
  function return until the operation is complete.
*/
rfm_wait_time_t
rfm_hdr_exec_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rf_time_type wakeup_exec_time;          /* Warmup Time            */
  rf_time_tick_type wakeup_exec_tick;          /* Warmup Tick            */
  rfm_cb_event_enum_type cb_event=RFM_EXECUTION_FAILURE;  /* CB event type  */
  rfm_hdr_validate_t device_state;         /* holds status of device */
  boolean are_tasks_running = TRUE;
  rf_hdr_mdsp_rx_agc_read_type rx_agc_read; /* RxAGC Read */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start Profiling of Rx Warm Up */
  wakeup_exec_tick = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_hdr_exec_wakeup_rx, device, rxlm_handle, &callback, 
                 &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_hdr_exec_wakeup_rx : START - Dev %d", device );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_hdr_validate_state( device, RF_CDMA_STATE_RX_PREP );
  
    /* Proceed only if State is valid, i.e. device_state is non-negative  */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {  
      /* Populate the total wait time */
      ret_val = rf_hdr_mc_exec_wakeup_rx( device, rxlm_handle,
                                          FALSE, /* Not in IRAT mode */ 
                                          RFDEVICE_MEAS_INVALID_SCRIPT 
                                          /* Not in IRAT set to invalid script*/);
  
      if ( ret_val >= 0 )
      {
        /* Flag Callback event as successful */
        cb_event = RFM_HDR_RX_WAKEUP_COMPLETE;
      }
    } 
    else
    {
      /* assign the current device state to return value of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_exec_wakeup_rx: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Release Critical Section Lock: Release lock before proceeding with the
  RxAGC convergence. Since there is no RF State variable update during RxAGC
  convergence, there is no need to hold the lock. This way other RF actions
  which requires CDMA lock can proceed. However the HDR callback function
  will be called only after RxAGC converges so that HDR has the
  guarantee from RF that RxAGC coneverges at the end of this function. */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  /* Wait for RxAGXC to converge */
  if ( ret_val ==  RFM_CDMA_HEALTHY_STATE )
  {
    logical_dev = rfc_cdma_get_logical_device_params( device );

    /* check for NULL pointers */
    if ( logical_dev == NULL )
    {
       RF_MSG_1( RF_ERROR, "rfm_hdr_exec_wakeup_rx: NULL data for device %d",
                 device );
      ret_val = RFM_CDMA_ERROR_NULL_DATA;
    } /* if ( logical_dev == NULL ) */
    else
    {
      /* Wait for RxAGC to converge */
      rf_hdr_mc_rx_agc_convergence( device );
  
      /* Get Final RxAGC/ LNA State */
      rf_hdr_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_read );
      
      rx_agc_read.rx_agc[0] = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                    rx_agc_read.rx_agc[0]);
      rx_agc_read.rx_agc[1] = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                    rx_agc_read.rx_agc[1]);
      rx_agc_read.rx_agc[2] = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                    rx_agc_read.rx_agc[2]);
      rx_agc_read.rx_agc[3] = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                    rx_agc_read.rx_agc[3]);
      /* Report final Rx AGC and LNA state */
      RF_MSG_6( RF_MED, "rfm_hdr_exec_wakeup_rx : Dev %d, "
                "RxAGC0 %d (dBm10), RxAGC1 %d (dBm10), "
                "RxAGC2 %d (dBm10), RxAGCAll %d (dBm10), LNA %d",
                device, rx_agc_read.rx_agc[0], rx_agc_read.rx_agc[1], 
                rx_agc_read.rx_agc[2], rx_agc_read.rx_agc[3], 
                rx_agc_read.lna_state );
    } /* if ! ( logical_dev == NULL ) */

    rf_hdr_log_samples( device, RFM_CDMA_CARRIER_0 );

  } /* if ( ret_val ==  RFM_CDMA_HEALTHY_STATE ) */

  /* Execute Callback - this will be moved to MSG router command handler when 
     the framework is implemented */ 
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */
  
  /* Record the execution time statistics for wakeup logging for this device*/
  wakeup_exec_time = rf_time_get_elapsed( wakeup_exec_tick, RF_USEC );

  /* update this component of RF WU time */
  rf_cdma_update_wup_time(device, wakeup_exec_time);

  /* Report wakeup time and final status */
  RF_MSG_3( RF_HIGH, "rfm_hdr_exec_wakeup_rx: [Status %d] - Dev %d, "
            "RxWakeupExecTime %d (us)", ret_val, device, wakeup_exec_time );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_exec_wakeup_rx: API FAILED. Dev %d",
              device );
  }

  rf_cdma_finalize_wup_time(device);

  /*Populate and commit warmup log packet*/
  rf_hdr_log_wakeup_rx( device );

  /* Return the API Wait time */
  return ret_val;

} /* rfm_hdr_exec_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn off the receiver.

  @details
  Moves the HDR radio from the Rx state to the Sleep state.  Any circuits
  that are no longer needed are powered down to conserve current.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_HDR_RX_SLEEP_COMPLETE event.  The function will attempt to give a
  best estimate of the time remaining in the return value.

  @param device
  The receive path to put to sleep.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.
 
  @param callback
  A function pointer that will be called when the entire sleep sequence
  is complete.  This function will be passed the RFM_HDR_RX_SLEEP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_hdr_sleep_rx()
  function return until the operation is complete. Returns a 0 on success (no
  wait time needed) and a negative value on failure
*/
rfm_wait_time_t
rfm_hdr_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_hdr_validate_t device_state_1; /* return value of device_state check */
  rfm_hdr_validate_t device_state_2; /* return value of device_state check */
  rfm_hdr_validate_t device_state_3; /* return value of device_state check */
  boolean div_device; /* Flag to check, if div device is enabled */
  boolean are_tasks_running = TRUE;
  struct
  {
    rf_time_tick_type before_crit_section;
    rf_time_tick_type begin_of_execution;
    rf_time_tick_type end_of_execution;
    rf_time_tick_type after_crit_section;
  } time_profiling;

  time_profiling.before_crit_section = rf_time_get_tick();

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start Profiling of Rx Warm Up */
  time_profiling.begin_of_execution = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_hdr_sleep_rx, device, rxlm_handle, &callback, &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_hdr_sleep_rx : START - Dev %d", device );

    /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Current State */
    device_state_1 = rfm_hdr_validate_state( device, RF_CDMA_STATE_RX_PREP );
  
    /* Validate the Current State */
    device_state_2 = rfm_hdr_validate_state( device, RF_CDMA_STATE_RX );
  
    /* Validate the Current State */
    device_state_3 = rfm_hdr_validate_state( device, 
                                             RF_CDMA_STATE_SHUTDOWN_FOR_IRAT );

    /* Diversity device should be in Sleep state before proceeding with
    Rx Sleep on master device */
    div_device = rf_cdma_has_associated_device( device );

    /* Proceed only if State is valid, i.e. ret_val is non-negative  */
    if ( ( ( device_state_1 == RFM_CDMA_HEALTHY_STATE ) ||
           ( device_state_2 == RFM_CDMA_HEALTHY_STATE ) ||
           ( device_state_3 == RFM_CDMA_HEALTHY_STATE ) ) &&
         ( div_device == FALSE ) )
    {      
      ret_val = rf_hdr_mc_sleep_rx(device, rxlm_handle, 
                                   FALSE /* Not in IRAT mode */ );
  
      if ( ret_val >=0 )
      {
        /* Flag Callback event as successful */
        cb_event = RFM_HDR_RX_SLEEP_COMPLETE;
      }
      else
      {
        /* indicate that API has failed  */
        cb_event = RFM_EXECUTION_FAILURE;
      }
    }
    else
    {
      /* assign the device state to return value of API to indicate error */
      if ( device_state_1 != RFM_CDMA_HEALTHY_STATE )
      {
        ret_val = device_state_1;
      }
      else if ( device_state_2 != RFM_CDMA_HEALTHY_STATE )
      {
        ret_val = device_state_2;
      }
  
      /* indicate failure */
      cb_event = RFM_EXECUTION_FAILURE;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_sleep_rx: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* Finalize the RF Warmup Time */
  time_profiling.end_of_execution = rf_time_get_tick();

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_hdr_sleep_rx : [Status %d] - Dev %d, RxSleepTime %d",
            ret_val, device,
            rf_time_get_difference(time_profiling.begin_of_execution,
                                   time_profiling.end_of_execution,
                                   RF_USEC) );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_sleep_rx: API FAILED. Dev %d",
              device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  time_profiling.after_crit_section = rf_time_get_tick();

     RF_MSG_1( RF_LOW, "critical section wait %d",
               rf_time_get_difference(time_profiling.before_crit_section,
                                   time_profiling.begin_of_execution, RF_USEC));

     RF_MSG_1( RF_LOW, "total time in rfm_hdr_sleep_rx() %d",
               rf_time_get_difference(time_profiling.before_crit_section,
                                   time_profiling.after_crit_section, RF_USEC));

  return ret_val;

} /* rfm_hdr_sleep_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the transmitter.

  @details
  Moves the HDR radio from the Rx state to the RxTx state.  Powers on all
  circuits required to transmit a HDR signal, and insures the transmitter
  is tuned to the correct band and channel.

  The HDR transmitter is controlled by hardware and software signals;
  calling this API does not guarantee the RF chipset will be emitting
  RF power.  The modulator hardware must also be enabled and actively
  controlling the TX_ON and PA_ON hardware controls.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_HDR_TX_WAKEUP_COMPLETE event.  The function will attempt to give a
  best estimate of the time remaining in the return value.

  @param device
  The transmit path to wakeup.
 
  @param txlm_handle
  The Tx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed the RFM_HDR_TX_WAKEUP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_hdr_wakeup_tx()
  function return until the operation is complete.
*/
rfm_wait_time_t
rfm_hdr_wakeup_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rf_time_tick_type wakeup_time; /* holds the time spent in this function */
  rf_time_type wakeup_time_val = 0;/*holds the time spent in this function*/
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_hdr_validate_t device_state; /* return value of device_state check */
  boolean are_tasks_running = TRUE;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* mark start of function */
  wakeup_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_hdr_wakeup_tx, device, txlm_handle, &callback, &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_hdr_wakeup_tx : START - Dev %d", device );

    /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_hdr_validate_state( device, RF_CDMA_STATE_RX );
  
    /* Proceed only if State is valid, i.e. device_state is non-negative  */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Call 1x Main Control to Prepare Tx for Wakeup */
      ret_val = rf_hdr_mc_wakeup_tx( device, txlm_handle );
  
      /* Based on Status of MC execution, set CB Event Flag */
      if ( ret_val >= 0 )
      {
        /* Flag Callback event as successful */
        cb_event = RFM_HDR_TX_WAKEUP_COMPLETE;   
      }
    } /* if ( ret_val == RFM_CDMA_HEALTHY_STATE ) */
    else
    {
      /* assign the current device state to return value of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_wakeup_tx: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* get time elapsed in this function */
  wakeup_time_val = rf_time_get_elapsed( wakeup_time, RF_USEC );
  
  /* Report wakeup time, Tx AGC and PA state */
  RF_MSG_3( RF_HIGH, "rfm_hdr_wakeup_tx : [Status %d] - Dev %d, TxWakeupTime %d",
            ret_val, device, wakeup_time_val );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_wakeup_tx: API FAILED. Dev %d",
              device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_hdr_wakeup_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable the transmitter.

  @details
  Moves the HDR radio from the RxTx state to the Rx state.  Any circuits
  that are no longer needed are powered down to conserve current.

  This API will disable the RF ASIC transmitter, and prevent any power
  from being radiated from the device independent of the TX_ON and PA_ON
  control signals.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_HDR_TX_SLEEP_COMPLETE event.  The function will attempt to give a
  best estimate of the time remaining in the return value.

  RF will be set to NO TRAFFIC state if this API call is sucessful

  @param device
  The transmit path to put to sleep.
 
  @param txlm_handle
  The Tx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param callback
  A function pointer that will be called when the entire sleep sequence
  is complete.  This function will be passed the RFM_HDR_TX_SLEEP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_hdr_sleep_tx()
  function return until the operation is complete. Returns a 0 on success (no
  wait time needed) and a negative value on failure
*/
rfm_wait_time_t
rfm_hdr_sleep_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_hdr_validate_t device_state; /* return value of device_state check */
  rf_time_tick_type sleep_time; /* time spent in API */
  rf_time_type sleep_time_val; /* time spent in API */
  boolean are_tasks_running = TRUE;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* mark start of function */
  sleep_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_hdr_sleep_tx, device, txlm_handle, &callback, &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_hdr_sleep_tx : START - Dev %d", device );

    /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_hdr_validate_state( device, RF_CDMA_STATE_RXTX );
  
    /* Proceed only if State is valid, i.e. device_state is non-negative  */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      ret_val = rf_hdr_mc_sleep_tx(device, txlm_handle);
  
      if ( ret_val >= 0 )
      {
        /* Flag Callback event as successful */
        cb_event = RFM_HDR_TX_SLEEP_COMPLETE;   
      }
      else
      {
        /* indicate that API has failed  */
        cb_event = RFM_EXECUTION_FAILURE;
      }
    }
    else
    {
      /* assign the current device state to return value of API */
      ret_val = device_state;
  
      /* indicate failure */
      cb_event = RFM_EXECUTION_FAILURE;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_sleep_tx: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* get time elapsed in this function */
  sleep_time_val = rf_time_get_elapsed( sleep_time, RF_USEC );

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_hdr_sleep_tx : [Status %d] - Dev %d, TxSleepTime %d", 
            ret_val, device, sleep_time_val );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_sleep_tx: API FAILED. Dev %d",
              device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_hdr_sleep_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Re-tune the HDR radio for a new band and or channel of operation, and
  configure single- or multi-carrier operation.

  @details
  Changes the operating frequency and number of active carriers for the
  HDR radio.  This function may only be called in Sleep or Rx state.

  If this API is called while the radio is in the Sleep state, the
  RF only prepares its internal state for the new frequency.  The
  radio will not be physically tuned unti the reciever is woken up.

  If this API is called while the radio is in the Rx state, the receiver
  (and any associated diversity receivers) will be re-tuned to operate
  in the new band and/or channel.

  To support multi-carrier 1xEV-DO revB, this API takes an array of
  band and channel structures, which specify the number and location
  of the carriers that should be tuned.  This API supports single
  carrier opeation by passing an array of length one.  The maximum
  number of carriers supported depends on the modem hardware, but 
  three carriers is the current maximum limit.

  HDR L1 must specify the exact carrier indicies it expects each channel
  to be assigned to through an array of carrier or sample RAM indicies.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_HDR_TUNE_COMPLETE event.  The function will attempt to give a
  best estimate of the time remaining in the return value.

  @param device
  The radio path to re-tune.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.
 
  @param num_band_chan
  The length of the band_chan parameter array.

  @param band_chan_array
  An array of band and channel structures for tuning multiple
  carriers in 1xEV-DO revB multi-carrier mode.

  @param carrier_index
  An array in integers that specify the carrier the corresponding channel
  should be placed on in the multi-carrier Rx front.  This carrier will
  deliver samples into the corresponding sample RAM for demodulation.

  @param callback
  A function pointer that will be called when the entire tuning sequence
  is complete.  This function will be passed the RFM_HDR_TUNE_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_hdr_tune()
  function return until the operation is complete, or an error code.
  If the API call succeeds the return value will be non-negative; all
  error codes will be negative.

  @retval RFM_HDR_TUNE_ERROR__INVALID_BAND
  One or more of the bands the user requested is not supported by the
  current chipset configuration.

  @retval RFM_HDR_TUNE_ERROR__INVALID_CHANNEL
  One or more of the channels the user requested is not supported by
  the current chipset configuration.

  @retval RFM_HDR_TUNE_ERROR__INVALID_MODE
  The radio was in an invalid mode and the radio could not be retuned.

  @retval RFM_HDR_TUNE_ERROR__SVDO_BANDWIDTH
  The radio is in simultaneous operation, and the channel that HDR has
  asked to tune to is incompatible with the current 1x channel due
  to bandwidth limitations.

  @retval RFM_HDR_TUNE_ERROR__INVALID_BANDS
  The MC-DO tune was for channels that were not all in the same band.

  @retval RFM_HDR_TUNE_ERROR__INVALID_CHANNELS
  The MC-DO tune was for channels that do not fit in the bandwidth
  supported by the current chipset configuration.
*/
rfm_wait_time_t
rfm_hdr_retune
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 num_band_chan,
  const sys_channel_type * const band_chan_array,
  const int32 * const carrier_index ,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rf_time_tick_type retune_time; /* Re-Tune Time */
  rf_time_type retune_time_val; /* Re-Tune Time */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */ 
  /* return value of device_state check for device */
  rfm_hdr_validate_t device_state_1 = RFM_CDMA_HEALTHY_STATE; /* return value of
                                                         device_state check */
  rfm_hdr_validate_t device_state_2 = RFM_CDMA_HEALTHY_STATE; /* return value of
                                                         device_state check */
  boolean are_tasks_running = TRUE;
  rfm_cdma_band_class_type band;/* Band  */
  rfm_cdma_chan_type chan_array[RFM_CDMA_CARRIER_CUMULATIVE] = {0,0,0};/* Channel array */ 
  uint32 c_idx; /*counter for channel index*/

  /* Number of bands is one */
  band = rf_cdma_sys_band_type_to_rf_band_type(band_chan_array[0].band);

  /* Populate the channel array*/
  for( c_idx = 0; c_idx < num_band_chan; c_idx++ )
  {
    chan_array[c_idx] = (rfm_cdma_chan_type)band_chan_array[c_idx].chan_num;
  }

  /* Start Profiling of Rx Re-tune */
  retune_time = rf_time_get_tick(); 

  RFM_DEBUG_EN_Q(&rfm_hdr_retune, device, rxlm_handle, &band_chan_array, 
                 *carrier_index);

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_hdr_retune : START - Dev %d, Band %d, Chan %d",
            device, band, chan_array[0] );

  /* If validity check passed, proceed. Else, send error msg and go to call back       
  */ 
  if ( ( rf_hdr_channel_list_is_supported( device, num_band_chan, 
                                         band_chan_array ) == TRUE ) &&
        ( num_band_chan <= RFM_CDMA_CARRIER_CUMULATIVE ) )  
  { 
    /* Acquire Critical Section Lock */
    rf_common_enter_critical_section(rf_cdma_get_lock_data());

    /* Query if all required tasks are running or not */
    are_tasks_running &= rf_cdma_required_tasks_running(device);
  
    /* Perform RF action, only if all required tasks are running */
    if ( are_tasks_running == TRUE )
    {
      /* Validate the Current State. Retune function can be called, only if RF
         is in Rx state. */
      device_state_1 = rfm_hdr_validate_state( device, RF_CDMA_STATE_RX );
      device_state_2 = rfm_hdr_validate_state( device, RF_CDMA_STATE_RXTX );
    
      /* Proceed only if State is valid */
      if ( ( device_state_1 == RFM_CDMA_HEALTHY_STATE ) ||
           ( device_state_2 == RFM_CDMA_HEALTHY_STATE ) )
      {  
        /* call MC API to retune */
        ret_val = rf_hdr_mc_retune( device, rxlm_handle, num_band_chan,
                                    band, chan_array, carrier_index,
                                    FALSE /*Not in IRAT mode*/ );
    
        /* declare success if ret_val >=0  */
        if ( ret_val >= RFM_CDMA_HEALTHY_STATE )
        {
          /* Flag Callback event as successful */
          cb_event = RFM_HDR_TUNE_COMPLETE;   
        }
      }
      else
      {
        /* indicate failure */
        ret_val = RFM_CDMA_ERROR_FAILURE;
      }
    } /* if ( are_tasks_running == TRUE ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rfm_hdr_retune: Dev %d - Tasks not running",
                device );
    } /* if !( are_tasks_running == TRUE ) */
  
    /* Release Critical Section Lock: Release lock before proceeding with the
    RxAGC convergence. Since there is no RF State variable update during RxAGC
    convergence, there is no need to hold the lock. This way other RF actions
    which requires CDMA lock can proceed. However the HDR callback function
    will be called only after RxAGC converges so that HDR has the
    guarantee from RF that RxAGC coneverges at the end of this function. */
    rf_common_leave_critical_section(rf_cdma_get_lock_data());
  
  } /*   if ( rf_hdr_channel_list_is_supported( device, num_band_chan, 
                                         band_chan_array ) == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, 
              "rfm_hdr_retune: Dev %d validity check failed", device);
    ret_val = RFM_CDMA_ERROR_INVALID_BAND_CHAN;
  } /*   if !( rf_hdr_channel_list_is_supported( device, num_band_chan, 
                                         band_chan_array ) == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* Record the execution time statistics for wakeup logging for this device*/
  retune_time_val = rf_time_get_elapsed( retune_time, RF_USEC );  

  RF_MSG_3( RF_HIGH, "rfm_hdr_retune: [Status %d] - Dev %d "
            "RetuneTime %d ", ret_val, device, retune_time_val  );
  
  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_2( RF_ERROR, "rfm_hdr_retune: API FAILED. Dev %d, num chans %d",
              device, num_band_chan );
  }
  
  /* Return the API Wait time */
  return ret_val;

} /* rfm_hdr_retune */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Associate a new receive device with a primary control device for
  diversity operation.

  @details
  This API links the HDR state of slave_device to the state of master_device.
  slave_device will track the operating frequency and Sleep/Rx state of the
  master.  HDR L1 need not make any calls to directy control slave_device
  until diversity mode is terminated with a call to
  rfm_hdr_disable_diversity().

  If master_device is in Rx or RxTx mode, slave is woken up if necessary,
  and linked to the operating frequency of master.  If master device is
  in sleep mode, slave remains in sleep mode and will be woken up
  when device1 is woken up.

  Any tuning opeations on device1 will be mirrored on device2 while
  they are linked in diversity mode.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_HDR_ENABLE_DIV_COMPLETE event.  The function will attempt to give
  a best estimate of the time remaining in the return value.

  @param master_device
  The radio device that will be the master.  Device2 is linked as a
  slave to this device.
 
  @param master_rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware for device1, if necessary.

  @param slave_device
  The radio device that will be slaved to master_device.

  @param slave_rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware for slave_device.
 
  @param callback
  A function pointer that will be called when the entire diversity enable
  sequence is complete.  This function will be passed the
  RFM_HDR_ENABLE_DIV_COMPLETE event when all actions associated with
  this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the
  rfm_hdr_enable_diversity() function return until the operation is
  complete.

  @todo
  Add documentation about possible errors when incompatible devices
  are passed, which cannot operation in diversity mode.
*/
rfm_wait_time_t
rfm_hdr_enable_diversity
(
  const rfm_device_enum_type master_device,
  const lm_handle_type master_rxlm_handle,
  const rfm_device_enum_type slave_device,
  const lm_handle_type slave_rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rf_time_tick_type enable_div_time; /* Time spent in function */
  rf_time_type enable_time; /* Time spent in function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_hdr_validate_t device_state; /* return value of device_state check */
  boolean are_tasks_running = TRUE;
  rf_hdr_mdsp_rx_agc_read_type rx_agc_read; /* RxAGC Read */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start to Time Measurement */
  enable_div_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_hdr_enable_diversity, master_device, master_rxlm_handle, 
                 slave_device, slave_rxlm_handle);

  /* Throw F3 Message */
  RF_MSG_2( RF_HIGH, "rfm_hdr_enable_diversity : START - mDev %d, sDev %d",
            master_device, slave_device );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(master_device);
  are_tasks_running &= rf_cdma_required_tasks_running(slave_device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State for Master Device. Diversity can be
    enabled when Master Device is either in Rx State ot RxTx State. If the
    expected state is atleast one of them, it is safe to procees */
    device_state = rfm_hdr_validate_state( master_device, RF_CDMA_STATE_RX ); 
  
    if ( device_state != RFM_CDMA_HEALTHY_STATE)
    {
      device_state = rfm_hdr_validate_state( master_device, RF_CDMA_STATE_RXTX );
    }
  
    /* Validate the current state for Slave Device */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      device_state = rfm_hdr_validate_state( slave_device, RF_CDMA_STATE_SLEEP );
    }
  
    /* Proceed only if both devices are in their expected states - i.e, 
       Master device is in (Rx or RxTx) state and slave device is in Sleep state.
   
       Checking ret_val against RFM_CDMA_HEALTHY_STATE, to make sure that none
       of the preliminary checks changed ret_val from its initial value of
       RFM_CDMA_HEALTHY_STATE
    */ 
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Call MC fucntion to Enable diversity */
      ret_val = rf_hdr_mc_enable_diversity( master_device, master_rxlm_handle,
                                            slave_device, slave_rxlm_handle );
        
      /* Based on Status of MC execution, set CB Event Flag */
      if ( ret_val >= 0 )
      {
        /* Flag Callback event as successful */
        cb_event = RFM_HDR_ENABLE_DIV_COMPLETE;
      }
    }
    else
    {
      /* return with error */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_2( RF_ERROR, "rfm_hdr_enable_diversity: mDev %d, sDev %d - Tasks "
                        "not running", master_device, slave_device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Wait for RxAGXC to converge */
  if ( ret_val ==  RFM_CDMA_HEALTHY_STATE )
  {
    logical_dev = rfc_cdma_get_logical_device_params( slave_device );

    /* check for NULL pointers */
    if ( logical_dev == NULL )
    {
       RF_MSG_1( RF_ERROR, "rfm_hdr_enable_diversity: NULL data for sDevice %d",
                 slave_device );
      ret_val = RFM_CDMA_ERROR_NULL_DATA;
    } /* if ( logical_dev == NULL ) */
    else
    {
      /* Wait for RxAGC to converge */
      rf_hdr_mc_rx_agc_convergence( slave_device );
  
      /* Get Final RxAGC/ LNA State */
      rf_hdr_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_read );
      
      rx_agc_read.rx_agc[0] = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                    rx_agc_read.rx_agc[0]);
      rx_agc_read.rx_agc[1] = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                    rx_agc_read.rx_agc[1]);
      rx_agc_read.rx_agc[2] = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                    rx_agc_read.rx_agc[2]);
      rx_agc_read.rx_agc[3] = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                                    rx_agc_read.rx_agc[3]);
      /* Report final Rx AGC and LNA state */
      RF_MSG_6( RF_MED, "rfm_hdr_enable_diversity : sDev %d, "
                "RxAGC0 %d (dBm10), RxAGC1 %d (dBm10), "
                "RxAGC2 %d (dBm10), RxAGCAll %d (dBm10), LNA %d",
                slave_device, rx_agc_read.rx_agc[0], rx_agc_read.rx_agc[1], 
                rx_agc_read.rx_agc[2], rx_agc_read.rx_agc[3], 
                rx_agc_read.lna_state );
    } /* if ! ( logical_dev == NULL ) */
  } /* if ( ret_val ==  RFM_CDMA_HEALTHY_STATE ) */
  

  /* Release Critical Section Lock: Release lock before proceeding with the
  RxAGC convergence. Since there is no RF State variable update during RxAGC
  convergence, there is no need to hold the lock. This way other RF actions
  which requires CDMA lock can proceed. However the HDR callback function
  will be called only after RxAGC converges so that HDR has the
  guarantee from RF that RxAGC coneverges at the end of this function. */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());
  
  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* End Time Measurement */
  enable_time = rf_time_get_elapsed( enable_div_time, RF_USEC );

  RF_MSG_4( RF_HIGH, "rfm_hdr_enable_diversity : [Status %d] - MasterDev %d, "
            "SlaveDev %d, EnableDivTime %d", ret_val, master_device, 
            slave_device, enable_time );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_2( RF_ERROR, "rfm_hdr_enable_diversity: API FAILED. MDev %d, SDev %d",
              master_device, slave_device );
  }

  return ret_val;
} /* rfm_hdr_enable_diversity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable receive diversity operation and put the device to sleep.

  @details
  This API unlinks the state of device2 from whatever device it was
  associated with in the rfm_hdr_enable_diversity() function.

  If device2 is in Rx or RxTx mode, device2 is put to sleep.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_HDR_DISABLE_DIV_COMPLETE event.  The function will attempt to give
  a best estimate of the time remaining in the return value.

  @param device2
  The radio device that will be disconnected from the diversity radio.
  This device must have previously been associated with a master device
  in a call to rfm_hdr_enable_diversity().
 
  @param rxlm_handle2
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware for device2.

  @param callback
  A function pointer that will be called when the entire diversity disable
  sequence is complete.  This function will be passed the
  RFM_HDR_DISABLE_DIV_COMPLETE event when all actions associated with
  this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the
  rfm_hdr_disable_diversity() function return until the operation is
  complete.

  @todo
  Add documentation about possible errors if device2 is not currently
  associated with a diversity master.
*/
rfm_wait_time_t
rfm_hdr_disable_diversity
(
  const rfm_device_enum_type slave_device,
  const lm_handle_type slave_rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rf_time_tick_type disable_div_time; /* time spent in function */
  rf_time_type disable_time; /* time spent in function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_hdr_validate_t device_state; /* return value of device_state check */
  boolean are_tasks_running = TRUE;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start time Measurement */
  disable_div_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_hdr_disable_diversity, slave_device, slave_rxlm_handle, 
                 &callback, &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_hdr_disable_diversity : START - sDev %d",
            slave_device );

    /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(slave_device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State for Slave Device */
    device_state = rfm_hdr_validate_state( slave_device, RF_CDMA_STATE_RXDIV );
  
    /* proceed only if device state is valid i.e. non-negative */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Call MC fucntion to disable diversity */
      ret_val = rf_hdr_mc_disable_diversity (slave_device, slave_rxlm_handle);
  
      /* check if MC API was successful */
      if ( ret_val >= 0 )
      {    
        /* Flag Callback event as successful */
        cb_event = RFM_HDR_DISABLE_DIV_COMPLETE;   
      }
    } /* if ( device_state == RFM_CDMA_HEALTHY_STATE ) */
    else
    {
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_disable_diversity: sDev %d - Tasks not "
                        "running", slave_device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* End Time Measurement */
  disable_time = rf_time_get_elapsed( disable_div_time, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_3( RF_HIGH, "rfm_hdr_disable_diversity : [Status %d] - SlaveDev %d, "
            "DisableDivTime %d", ret_val, slave_device, disable_time );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_disable_diversity: API FAILED. SDev %d",
              slave_device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_hdr_disable_diversity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API updates the properties of the reverse link (RL) carriers
 
  @details
  This API is called by HDR-L1 to instruct RF (and hence FW) to use SCDO or 
  MCDO max power limits. The decision to use SCDO or MCDO limits is based on the
  num_carriers argument - if num_carriers == 1, then RF updates FW with SCDO 
  power limits (even though the radio is in MCDO mode)
 
  This API may be called at any point of time after TX is established, and to 
  switch between SC- and MC-power limits
 
  @param device
  The radio device for which RL set update needs to be done. This device must 
  have previously been put in RXTX state.
 
  @param band_chan_array
  An array of band and channel structures for tuning multiple carriers in 
  1xEV-DO revB multi-carrier mode.

  @param num_band_chan
  The length of the band_chan parameter array.
 
  @return
  RFM_CDMA_HEALTHY_STATE on success and RFM_CDMA_ERROR_BAD_PARAM on failure
 
*/
rfm_wait_time_t
rfm_hdr_update_rev_link_carrier_set
(
  const rfm_device_enum_type device,
  const sys_channel_type * const band_chan_array,
  const uint8 num_carriers            
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_BAD_PARAM; /* Return value */
  rfm_hdr_validate_t device_state; /* return value of device_state check */
  boolean are_tasks_running = TRUE;
  rf_time_type update_rl_time;                 /* Warmup Time            */
  rf_time_tick_type tick_for_rl_time;          /* Warmup Tick            */

  RFM_DEBUG_EN_Q( &rfm_hdr_update_rev_link_carrier_set, device,
                  &band_chan_array, num_carriers, 0 /* dummy arg*/ );

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start Time Measurement */
  tick_for_rl_time = rf_time_get_tick();

  RF_MSG_2( RF_HIGH, "rfm_hdr_update_rev_link_carrier_set: device %d, "
                     "num_carriers %d", device, num_carriers );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running( device );

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* check for RXTX state */
    device_state = rfm_hdr_validate_state( device, RF_CDMA_STATE_RXTX ); 

    if (device_state != RFM_CDMA_HEALTHY_STATE )
    {
      RF_MSG_1 ( RF_ERROR, "rfm_hdr_update_rev_link_carrier_set: device %d not "
               "RXTX state" , device );
    }
    else if ( num_carriers > RFM_CDMA_CARRIER_CUMULATIVE )
    {
      RF_MSG_3 ( RF_ERROR, "rfm_hdr_update_rev_link_carrier_set: device %d "
               "num_carriers[%d] > RFM_CDMA_CARRIER_CUMULATIVE[%d] " ,
               device, num_carriers , RFM_CDMA_CARRIER_CUMULATIVE ) ;
    }
    else
    {
      int16 max_power_limit_selected_dB10 = -600 ;
      rfm_cdma_chan_type cdma_band_chan_array[3] = {
        band_chan_array[0].chan_num , band_chan_array[1].chan_num , 
        band_chan_array[2].chan_num } ;

      boolean success =
        rf_hdr_plim_set_max_pwr_per_modified_ccp ( device , 
                                                   cdma_band_chan_array ,
                                                   num_carriers , 
                                                   &max_power_limit_selected_dB10 ) ;
      /* all checks passed */
      if ( TRUE == success )
      {
        ret_val = RFM_CDMA_HEALTHY_STATE;
      }
      else
      {
        RF_MSG ( RF_ERROR , 
          "rfm_hdr_update_rev_link_carrier_set: Max power override failed " );
      }
    }
  } /* ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_update_rev_link_carrier_set: Dev %d - "
                        "Tasks not running", device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  /* Record the execution time statistics for wakeup logging for this device*/
  update_rl_time = rf_time_get_elapsed( tick_for_rl_time, RF_USEC );
  
  RF_MSG_3( RF_HIGH, "rfm_hdr_update_rev_link_carrier_set: [Status %d] Dev %d, "
            " time %d", ret_val, device, update_rl_time );

  return ret_val;

} /* rfm_hdr_update_rev_link_carrier_set */

/*! @} */

/*============================================================================*/
/*!
  @name HDR AGC Query Functions

  @brief
  Functions to query HDR Rx and Tx AGC
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current gain state of the receiver.

  @details
  This function returns the current gain state of the requested
  receiver path.  The gain state is common for all carriers
  on a receive path.

  @param device
  The receive path to query the Rx gain state.

  @return
  Receiver gain state on success and RFM_LNA_INVALID_GAIN_STATE on failure
*/
rfm_lna_gain_state_type
rfm_hdr_get_lna_gain_state
(
  const rfm_device_enum_type device
)
{
  rf_hdr_mdsp_rx_agc_read_type rx_agc_data; /* holds RxAGC data from FW */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  
  /* Parameter Validation  */
  if ( device > RFM_MAX_DEVICES )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_lna_gain_state(): Invalid device %d", 
              device );
    return RFM_LNA_INVALID_GAIN_STATE;
  }
  
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* Validate Rx mdsp path */
  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_hdr_get_lna_gain_state(): logical_dev is NULL." );
    return RFM_LNA_INVALID_GAIN_STATE;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_lna_gain_state(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_LNA_INVALID_GAIN_STATE;
  }

  /* Get Current HDR RxAGC from RF-MDSP */
  rf_hdr_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_data );

  return (rx_agc_data.lna_state);

} /* rfm_hdr_get_lna_gain_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current HDR Rx AGC

  @details
  This function returns the HDR Rx AGC for an individual carrier
  or the composite of all carriers taken together.  The AGC can
  be reported in a variety of formats, depending on the client's
  needs.

  The composite AGC is the value used to control the receiver
  gain stepping.  The individual carrier AGCs are the estimate
  of each specific carrier's receive power.

  The formats supported are
  - Raw
  - 1/256th dBm

  When returning dBm values, the value is a 2's complement signed
  number centered at 0dBm.

  @param device
  The receive path to query the Rx AGC.

  @param mc_id
  The specific carrier to query the AGC on, or the composite receiver
  AGC if the parameter is set to RFM_HDR_CARRIER_ID__CUMULATIVE.

  @param format
  The format to return the AGC in.

  @return
  Rx AGC in the requested format on success and 0xFFFF on failure
*/
int16
rfm_hdr_get_rx_agc
(
  const rfm_device_enum_type device,
  const rfm_cdma_carrier_type mc_id,
  const rfm_hdr_agc_format_type format
)
{
  int16 ret_val; /* return value */
  rf_hdr_mdsp_rx_agc_read_type rx_agc_data;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
 
  /* Parameter Validation  */
  if ( mc_id > RFM_CDMA_CARRIER_CUMULATIVE )
  {
    RF_MSG_1( RF_ERROR, "rf_hdr_get_rx_agc(): Invalid carrier ID %d",\
              mc_id );
    return 0xFFFF;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* Validate Rx mdsp path */
  if ( logical_dev == NULL )
  { 
    RF_MSG( RF_ERROR, "rfm_hdr_get_rx_agc(): logical_dev is NULL %d");
    return 0xFFFF;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_rx_agc(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_LNA_INVALID_GAIN_STATE;
  }

  /* Get Current HDR RxAGC from RF-MDSP */
  rf_hdr_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_data );

  /* Get RxAGC for requested Carrier in requested format */
  ret_val = rf_cdma_convert_rxagc( format, rx_agc_data.rx_agc[mc_id] );
  
  return ret_val;

} /* rfm_hdr_get_rx_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Capture the recent Rx AGC range and current Rx AGC value over
  the last tracking interval.

  @details
  This API returns the min, max, and current Rx AGC in raw units
  taken over a recent sampling interval.  This information is
  reset every time AGC tracking is started.

  @param device
  The radio path that is being queried.

  @param agc_info
  Out parameter containing a pointer to a structure that will hold
  the minimum, maximum, and current Rx AGC.

  @todo
  Get better information on what "tracking interval" means.
*/
void
rfm_hdr_capture_agc
(
  const rfm_device_enum_type device,
  rfm_hdr_agc_info_struct_type* const agc_info
)
{
  return;
} /* rfm_hdr_capture_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfm wrapper function to get L1SRAM Sample Memory 
  Address for CDMA HDR.

  @details
  Returns the L1SRAM Sample Memory Address for CDMA HDR.  

  @param device
  The radio path that is being queried.
  
  @param carrier
  When operating in multi-carrier mode, and as fimrware supports
  SRAM Base Adresses per-carrier basis for HDR, this parameter can be used to 
  get the SRAM mempool Base Adress for a specific carrier.  
  
  @return
  L1SRAM Sample Memory Base Address.

*/
 uint32
 rfm_hdr_get_mem_pool_base_addr
 (
   const rfm_device_enum_type device,
   const rfm_hdr_carrier_id_type carrier
 )
 { 
   const rfc_cdma_logical_device_params_type* logical_dev;
   
   if ( device >= RFM_MAX_DEVICES || carrier >= RFM_CDMA_CARRIER_CUMULATIVE )
   {
	 RF_MSG_2( RF_ERROR, "rfm_hdr_get_mem_pool_base_addr: Invalid device %d or carrier %d ", 
				   device, carrier);
	 return NULL;
   }
   /* Get and validate Logical Device */
   logical_dev = rfc_cdma_get_logical_device_params( device );
	   
   if ( logical_dev == NULL )
   {
	  RF_MSG_DBG_1 ( RF_ERROR, "rfm_1x_get_mem_pool_base_addr(): Invalid logical device "
							   "for device %d", device);
	  return NULL ;
	}
	   
	/* Validate Rx mdsp path */
	if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
	{
	  RF_MSG_1( RF_ERROR, "rfm_1x_get_mem_pool_base_addr(): Invalid rx_mdsp_path %d",
					logical_dev->rx_mdsp_path );
		  
	  return NULL ;
	}
   
   return (rf_hdr_mdsp_get_mem_pool_base_addr( carrier, logical_dev->rx_mdsp_path));

}/* rfm_hdr_get_mem_pool_base_addr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the Rx power filtered over the last hundred milliseconds.

  @details
  Returns Rx power reported by the FW Rx AGC filter.  This filter is intended
  to be a slowly varying record of recent Rx power, and is commonly used as
  an input to a multi-radio concurrency manager.

  The Rx power is filtered by the firmware with a time constant around 100ms,
  but this is not set by the RF driver, so it may change without notice.

  The API supports querying per-carrier filtered power as well as overall
  Rx power through the carrier parameter.  If the firmware Rx AGC does not
  support per-carrier Rx AGC filtering, then the function will print an
  RF error message in the F3 log, and return the overall filtered Rx
  power.  Overall filtered Rx power should always be valid.

  @param device
  The radio path that is being queried.

  @param carrier
  When operating in multi-carrier mode, and if the fimrware Rx AGC supports
  per-carrier filtering, this parameter can be used to get the filtered
  Rx of a specific carrier.  To get the overall filtered Rx
  power for all carriers, pass RFM_CDMA_CARRIER_CUMULATIVE.

  @param format
  The format the Rx power should be returned as.

  @return
  Filtered Rx power in the specified format.
*/
int16
rfm_hdr_get_filtered_rx_power
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type carrier,
  const rfm_hdr_agc_format_type format
)
{
  const int MIN_RX_POWER = -115 ;
  int16 filtered_power_dB640 [RFM_CDMA_CARRIER_CUMULATIVE] = { 
    RF_CDMA_MDSP_MIN_RX_PWR , RF_CDMA_MDSP_MIN_RX_PWR , 
    RF_CDMA_MDSP_MIN_RX_PWR } ; 
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 filtered_rx_power = MIN_RX_POWER ; /* holds the filtered rx_total power */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* validate carrier */
  if ( carrier >= RFM_CDMA_CARRIER_CUMULATIVE )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_filtered_rx_power(): Invalid carrier %d", 
               carrier);
    return filtered_rx_power;
  }

   /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_filtered_rx_power(): Invalid logical device "
                         "for device %d", device);
    return filtered_rx_power;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_filtered_rx_power(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return filtered_rx_power;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL != dev_status_r )
  {
    /* Get per carrier filtered Rx */
    rf_hdr_mdsp_get_filtered_rx_power( logical_dev->rx_mdsp_path, carrier, 
                                         &filtered_power_dB640[0] ); 
    filtered_rx_power = filtered_power_dB640[carrier] ;

    /* Currently Rx filtered power is not supported so the swicth statement is empty */
    /* validate format */
    switch ( format )
    {
      case RFM_CDMA_AGC_FORMAT__RAW:
        RF_MSG ( RF_LOW , "rfm_hdr_get_filtered_rx_power(): "
                 "Format: AGC Raw " ) ;
        break ;

      case RFM_CDMA_AGC_FORMAT__DBM10:
        filtered_rx_power = 
        rf_cdma_convert_rxagc ( RFM_CDMA_AGC_FORMAT__DBM10 , 
                                    filtered_rx_power ) ;
        break ;

      case RFM_CDMA_AGC_FORMAT__DBM256:
        RF_MSG ( RF_LOW , "rfm_hdr_get_filtered_rx_power(): "
                 "Format: AGC dBm256 " ) ;  
        break ;

      default: 
      {
        RF_MSG_1(RF_ERROR, "rfm_hdr_get_filtered_rx_power(): dB format %d unsupported", 
                 format) ;
      }
    }
  }
  else
  {
    RF_MSG ( RF_ERROR, "rfm_hdr_get_filtered_rx_power(): Invalid device pointer" ); 
  }

  return filtered_rx_power ;

} /* rfm_hdr_get_filtered_rx_power() */
/*! @} */
 
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the current HDR offset and and nominal power.

  @details
  This API returns a zero for TC and nominal power on error, and valid values 
  on success

  @param device
  The transmit path to query the offset and nominal power.

  @param turnaround_db
  Output parameter for turnaround constant as specifed by standard

  @param nominal_pwr_db
  Output parameter for over-ridden turn-around constant (aka nominal power)
 
*/
void
rfm_hdr_get_turnaround_info
(
  const rfm_device_enum_type device,
  int16* const turnaround_db,
  int16* const nominal_pwr_db
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status read Pointer */
  /* return values of device_state check */
  rfm_hdr_validate_t device_in_rx, device_in_rxtx; 

  /* initialize to 0's - this is the value to be returned on error */
  *turnaround_db = 0;
  *nominal_pwr_db = 0;

  /* Validate the current driver state - needs to be in Rx state atleast, for 
     band info to be valid */
  device_in_rx = rfm_hdr_validate_state( device, RF_CDMA_STATE_RX );

  device_in_rxtx = rfm_hdr_validate_state( device, RF_CDMA_STATE_RXTX );

  /* Throw error and return if neither in RX or RXTX state */
  if ( ( device_in_rx < 0 ) && ( device_in_rxtx < 0 ))
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_turnaround_info: Device %d neither in Rx"
              " of RxTx state - invalid state", device);
    return;   
  }

  /* Get readable Device Status */
  dev_status_r = rf_cdma_get_device_status( device );

  /* check for NULL pointers */
  if ( dev_status_r != NULL )
  {
    *turnaround_db = rf_cdma_get_turnaround_constant(dev_status_r->curr_band);
  
    *nominal_pwr_db = (int16)
                         (dev_status_r->data_hdr.hdr_nominal_pwr_dB256 / 256);
  }
  else
  {
    RF_MSG_1 (RF_ERROR, "rfm_hdr_get_turnaround_info: Could not get a valid "
                        "device pointer for device %d", device );
  }

  return;
} /* rfm_hdr_get_turnaround_info */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current closed loop power control adjustment for
  a given carrier.

  @details
  This function returns the closed loop Tx AGC adjustment in several
  possible formats.  The power control is applied per carrier.  There
  is no meaning for the composite power control, so this function
  may not be called with the RFM_HDR_CARRIER_ID__CUMULATIVE paramenter.

  The formats supported are
  - Raw
  - 1/256th dBm

  When returning dBm values, the value is a 2's complement signed
  number centered at 0dBm.

  @param device
  The transmit path to query the closed loop adjust.

  @param mc_id
  The specific carrier to query the Tx closed loop adjust AGC on.
  RFM_HDR_CARRIER_ID__CUMULATIVE is invalid to pass to this function.

  @param format
  The format to return the AGC in.

  @return 
  The Tx closed loop adjust in dB256 units on success and 0xFFFF on failure
*/
int16
rfm_hdr_get_tx_adjust
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type mc_id,
  const rfm_hdr_agc_format_type format
)
{
  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_data; /* Tx AGC Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 closed_loop_power; /* holds the tx_total power */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* validate carrier */
  if ( mc_id > RFM_CDMA_CARRIER_CUMULATIVE )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_adjust: Invalid carrier %d", mc_id );
    return 0xFFFF;
  }

  /* validate format */
  if ( format != RFM_CDMA_AGC_FORMAT__DBM256 )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_adjust: dB format %d unsupported", 
              format );
    return 0xFFFF;
  }

  /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_adjust: Invalid logical device "
                         "for device %d", device );
    return 0xFFFF;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_adjust: Invalid "
              "rx_mdsp_path %d", logical_dev->rx_mdsp_path );
    return 0xFFFF;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( dev_status_r != NULL )
  {   
    if ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX )
    {
      /* get Turnaround constant for current band in 256dB units */
      int16 K256 = 256*rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

      /* read closed loop power from FW */
      rf_hdr_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path ,
                                           rf_cdma_data_get_txlm_handle (device) ,
                                           mc_id , 
                                           &tx_agc_data ) ;
       
      /* tx_agc_data.closed_loop_power is in 1/512dB - convert to 1/256dB */
      closed_loop_power = ( tx_agc_data.closed_loop_power / 2 ) - 
                          ( dev_status_r->data_hdr.hdr_nominal_pwr_dB256 - K256);                         
    }
    else
    {
      RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_adjust: Device not in RXTX "
                           "state - current state %d ",  
                           dev_status_r->data_hdr.rf_state ); 
      closed_loop_power = 0xFFFF;
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_hdr_get_tx_adjust: Invalid device pointer");
    closed_loop_power = 0xFFFF;
  }

  return closed_loop_power;

} /* rfm_hdr_get_tx_adjust */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current open loop Tx AGC for a given carrier.

  @details
  This function returns the open loop Tx AGC value in several
  possible formats.  Transmit power is only reported on a per-
  carrier basis, passing RFM_HDR_CARRIER_ID__CUMULATIVE is
  invalid.

  The formats supported are
  - Raw
  - 1/256th dBm

  When returning dBm values, the value is a 2's complement signed
  number centered at 0dBm.

  @param device
  The transmit path to query the open loop Tx AGC.

  @param mc_id
  The specific carrier to query the Tx AGC on.
  RFM_HDR_CARRIER_ID__CUMULATIVE is invalid to pass to this function.

  @param format
  The format to return the AGC in.

  @return 
  The Tx open loop in dB256 on success and 0xFFFF on failure
*/
int16
rfm_hdr_get_tx_open_loop
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type mc_id,
  const rfm_hdr_agc_format_type format
)
{
  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_data; /* Tx AGC Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 open_loop_power; /* holds the open-loop power */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* validate carrier */
  if ( mc_id > RFM_CDMA_CARRIER_CUMULATIVE )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_open_loop(): Invalid carrier %d", 
              mc_id);
    return 0xFFFF;
  }

  /* validate format */
  if ( format != RFM_CDMA_AGC_FORMAT__DBM256 )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_open_loop(): dB format %d unsupported", 
              format);
    return 0xFFFF;
  }

   /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_open_loop(): Invalid logical device "
                         "for device %d", device);
    return 0xFFFF;
  }

  /* Validate Tx mdsp path */
  if ( logical_dev->tx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_open_loop(): Invalid tx_mdsp_path %d",
              logical_dev->tx_mdsp_path );
    return 0xFFFF;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL != dev_status_r )
  {
    if ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX )
    {
      /* Get TxAGC Data */
      rf_hdr_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path,
                                           rf_cdma_data_get_txlm_handle (device) ,
                                           mc_id, &tx_agc_data);
     
      open_loop_power = (( tx_agc_data.open_loop_power*256)/640)
                       + (-16333) /* (-63.8 * 256) */
                       - dev_status_r->data_hdr.hdr_nominal_pwr_dB256;
     
      open_loop_power = -open_loop_power;
    }
    else
    {
      RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_open_loop: Device not in RXTX "
                           "state - current state %d ", 
                           dev_status_r->data_hdr.rf_state ); 
      open_loop_power =  0xFFFF;
    }
  }
  else
  {
    RF_MSG ( RF_ERROR, "rfm_hdr_get_tx_open_loop: Invalid device pointer" );
    open_loop_power =  0xFFFF;
  }

  return open_loop_power;

} /* rfm_hdr_get_tx_open_loop */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current Tx pilot power for a given carrier.

  @details
  This function returns the Tx pilot power value in several
  possible formats.  Transmit power is only reported on a per-
  carrier basis, passing RFM_HDR_CARRIER_ID__CUMULATIVE is
  invalid.

  Tx pilot power is the sum of a carrier's open loop power
  and closed loop power control adjust.

  The formats supported are
  - Raw
  - 1/256th dBm

  When returning dBm values, the value is a 2's complement signed
  number centered at 0dBm.

  @param device
  The transmit path to query the Tx pilot power.

  @param mc_id
  The specific carrier to query the Tx pilot power on.
  RFM_HDR_CARRIER_ID__CUMULATIVE is invalid to pass to this function.

  @param format
  The format to return the Tx pilot power in.

  @return 
  The Tx pilot power in db256 on success and 0xFFFF on failure
*/
int16
rfm_hdr_get_tx_pilot_power
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type mc_id,
  const rfm_hdr_agc_format_type format
)
{
  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_data; /* Tx AGC Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 tx_pilot_power; /* holds the tx_total power */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* validate carrier */
  if ( mc_id > RFM_CDMA_CARRIER_CUMULATIVE )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_open_loop(): Invalid carrier %d", 
              mc_id);
    return 0xFFFF;
  }

  /* validate format */
  if ( format != RFM_CDMA_AGC_FORMAT__DBM256 )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_open_loop(): dB format %d unsupported", 
               format);
    return 0xFFFF;
  }

   /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_open_loop(): Invalid logical device "
                         "for device %d", device);
    return 0xFFFF;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->tx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_pilot_power(): Invalid tx_mdsp_path %d",
              logical_dev->tx_mdsp_path );
    return 0xFFFF;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL != dev_status_r )
  {
    if ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX )
    {
      /* get Turnaround constant for current band in 256dB units */
      int K256 = 256 * rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

      /* Get TxAGC Data */
      rf_hdr_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path,
                                           rf_cdma_data_get_txlm_handle (device),
                                           mc_id, 
                                           &tx_agc_data );
     
      tx_pilot_power = ((tx_agc_data.pilot_power * 256) / 640)
                       + ( -16333) /* (-63.8 * 256 ) */ - K256;
     
      /* Tx has -ve resolution compared to Rx pwr level based calculation above*/
      tx_pilot_power = -tx_pilot_power;
    }
    else
    {      
      RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_pilot_power: Device not in RXTX "
                           "state - current state %d ", 
                           dev_status_r->data_hdr.rf_state ); 
      tx_pilot_power = 0xFFFF;
    }
  } /* if ( NULL != dev_status_r ) */
  else
  {
    RF_MSG ( RF_ERROR, "rfm_hdr_get_tx_pilot_power: Invalid device pointer" ); 
    tx_pilot_power = 0xFFFF;
  }

  return tx_pilot_power;

} /* rfm_hdr_get_tx_pilot_power */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current total Tx power for a given carrier.

  @details
  This function returns the total Tx power value in several
  possible formats.  Transmit power is only reported on a per-
  carrier basis, passing RFM_HDR_CARRIER_ID__CUMULATIVE is
  invalid.

  Tx total power is the sum of a carrier's pilot power (open loop
  + closed loop adjust) and any additional data or control channels
  transmitted with the carrier.

  The formats supported are
  - Raw
  - 1/256th dBm
  - 1/10th dBm

  When returning dBm values, the value is a 2's complement signed
  number centered at 0dBm.

  @param device
  The transmit path to query the total Tx power.

  @param mc_id
  The specific carrier to query the total Tx power on.
  RFM_HDR_CARRIER_ID__CUMULATIVE is invalid to pass to this function.

  @param format
  The format to return the total Tx power in.

  @return 
  The total Tx power in the appropriate units and 0xFFFF on error
*/
int16
rfm_hdr_get_tx_total_power
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type carrier,
  const rfm_hdr_agc_format_type format
)
{
  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_data; /* Tx AGC Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 tx_total; /* holds the tx_total power */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* validate carrier */
  if ( carrier > RFM_CDMA_CARRIER_CUMULATIVE )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_total_power(): Invalid carrier %d", 
               carrier);
    return 0xFFFF;
  }

  /* validate format */
  if ( format != RFM_CDMA_AGC_FORMAT__DBM256 )
  {
    RF_MSG_1(RF_ERROR, "rfm_hdr_get_tx_total_power(): dB format %d unsupported", 
             format);
    return 0xFFFF;
  }

   /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_total_power(): Invalid logical device "
                         "for device %d", device);
    return 0xFFFF;
  }

  /* Validate Tx mdsp path */
  if ( logical_dev->tx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_total_power(): Invalid tx_mdsp_path %d",
              logical_dev->tx_mdsp_path );
    return 0xFFFF;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL != dev_status_r )
  {
    if ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX )
    {
      /* get Turnaround constant for current band in 256dB units */
      int K256 = 256 * rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

      /* Get TxAGC Data */
      rf_hdr_mdsp_get_tx_agc_logical_data(logical_dev->tx_mdsp_path, 
                                          rf_cdma_data_get_txlm_handle (device),
                                          carrier, 
                                          &tx_agc_data );
     
      if ( carrier == RFM_CDMA_CARRIER_CUMULATIVE )
      {
        tx_total = ((tx_agc_data.total_power * 256) / 640) + 
                    ( -16333 ) /* (-63.8 * 256 ) */ - K256;
      }
      else
      {
      tx_total = ((tx_agc_data.total_carrier_power * 256) / 640) + 
                  ( -16333 ) /* (-63.8 * 256 ) */ - K256;
      }
      /*Tx has -ve resolution compared to Rx pwr level based calculation above*/
      tx_total = -tx_total;
    } /* if ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX ) */
    else
    {
      RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_total_power: Device not in RXTX "
                           "state - current state %d ", 
                           dev_status_r->data_hdr.rf_state ); 
      tx_total = 0xFFFF;
    }
  } /* if ( NULL != dev_status_r ) */
  else
  {
    RF_MSG ( RF_ERROR, "rfm_hdr_get_tx_total_power: Invalid device pointer" ); 
    tx_total = 0xFFFF;
  }

  return tx_total;

} /* rfm_hdr_get_tx_total_power */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Query current Tx State parameter
 
  @details
  This interface can be used to used to query the current Tx state parameter
  of active RF radio. The Tx data return from this interface will be valid
  only when the radio is in Tx state.
 
  @param device
  RFM Device for which the Tx state parameter is queried
 
  @param tx_state_param
  Pointer to the structure containing the Tx State parameter. Calling
  software module will be responsible for allocating the memory
  pointed by the pointer.
 
  @return
  Flag indicating if the Tx State param populated in tx_state_param is valid
  or not.
*/
rfm_cdma_api_status_type
rfm_hdr_get_tx_agc_state_parameter
(
  const rfm_device_enum_type device,
  rfm_hdr_tx_agc_state_param_type *tx_state_param
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status read Pointer */
  rfm_hdr_carrier_id_type carrier;
  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_data; /* Tx AGC Data */
  rf_cdma_mdsp_tx_agc_physical_read_type tx_agc_phy;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 K256;

  /* Get readable Device Status */
  dev_status_r = rf_cdma_get_device_status( device );

  /* check for NULL pointers */
  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_agc_state_parameter: Dev %d - Data "
                        "Data", device );
    return RFM_CDMA_ERROR_NULL_DATA;
  } /* if ( dev_status_r == NULL ) */

  /* NULL pointer check on the Tx State param container */
  if ( tx_state_param == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_tx_agc_state_parameter: Dev %d - Invalid "
                        "container", device );
    return RFM_CDMA_ERROR_BAD_PARAM;
  } /* if ( tx_state_param == NULL ) */

  /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_tx_agc_state_parameter(): Dev %d - "
                         "Invalid logical device ", device);
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Check if the device is in Tx STate */
  if ( dev_status_r->data_hdr.rf_state != RF_CDMA_STATE_RXTX )
  {
    RF_MSG_2( RF_ERROR, "rfm_hdr_get_tx_agc_state_parameter: Dev %d - Bad RF "
                        "State %d", device, dev_status_r->data_hdr.rf_state );
    return RFM_CDMA_ERROR_INVALID_DEV_STATE;
  } /* if ( dev_status_r->rf_state != RF_CDMA_STATE_RXTX ) */

  /* get Turnaround constant for current band in 256dB units */
  K256 = 256*rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

  /* Query Carrier based TxState param */
  for ( carrier = RFM_CDMA_CARRIER_0; carrier < RFM_CDMA_CARRIER_NUM;
        carrier++ )
  {

    /* read closed loop power from FW */
    rf_hdr_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path, 
                                         rf_cdma_data_get_txlm_handle (device) ,
                                         carrier, 
                                         &tx_agc_data );

    /* Get Logical Side Data of TxAGC */
    rf_hdr_mdsp_get_tx_agc_physical_data( logical_dev->tx_mdsp_path, 
                                          rf_cdma_data_get_txlm_handle (device) ,
                                          (rfcom_multi_carrier_id_type)carrier,
                                          &tx_agc_phy ); 
    
    /* NOTE: Tx has -ve resolution compared to Rx power level */   

    /* tx_agc_data.closed_loop_power is in 1/512dB - convert to 1/256dB */
    tx_state_param->carrier_power[carrier].tx_adjust = 
                        ( tx_agc_data.closed_loop_power / 2 ) -
                        ( dev_status_r->data_hdr.hdr_nominal_pwr_dB256 - K256);

    /* Open Loop Power */
    tx_state_param->carrier_power[carrier].open_loop = 
                          -( (( tx_agc_data.open_loop_power*256)/640)
                             + (-16333) /* (-63.8 * 256) */
                             - dev_status_r->data_hdr.hdr_nominal_pwr_dB256 );
   
    /* Pilot Power */
    tx_state_param->carrier_power[carrier].pilot = 
                          -( ((tx_agc_data.pilot_power * 256) / 640)
                             + ( -16333) /* (-63.8 * 256 ) */ - K256 );

    /* Total Power */
    tx_state_param->carrier_power[carrier].total = 
                          -( ((tx_agc_data.total_carrier_power * 256) / 640) + 
                             ( -16333 ) /* (-63.8 * 256 ) */ - K256 );
       
  } /* for ( carrier = RFM_CDMA_CARRIER_0; carrier <= RFM_CDMA_CARRIER_NUM;
        carrier++ ) */

  tx_state_param->total_power = 
                      -( ((tx_agc_data.total_power * 256) / 640) + 
                         ( -16333 ) /* (-63.8 * 256 ) */ - K256 );
  /* Update PA State */
  tx_state_param->pa_state = tx_agc_data.pa_state;

  return RFM_CDMA_HEALTHY_STATE;

} /* rfm_hdr_get_tx_agc_state_parameter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the Tx power filtered over the last hundred milliseconds.

  @details
  Returns Tx power reported by the FW Tx AGC filter.  This filter is intended
  to be a slowly varying record of recent Tx power, and is commonly used as
  an input to a multi-radio concurrency manager.

  The Tx power is filtered by the firmware with a time constant around 100ms,
  but this is not set by the RF driver, so it may change without notice.

  The API supports querying per-carrier filtered power as well as overall
  Tx power through the carrier parameter.  If the firmware Tx AGC does not
  support per-carrier Tx AGC filtering, then the function will print an
  RF error message in the F3 log, and return the overall filtered Tx
  power.  Overall filtered Tx power should always be valid.

  @param device
  The radio path that is being queried.

  @param carrier Only supports overall filtered Tx AGC whether
  operating SCDO or MCDO mode

  @param format
  The format the Tx power should be returned as.

  @return
  Filtered Tx power in the specified format.
*/
int16
rfm_hdr_get_filtered_tx_power
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type carrier,
  const rfm_hdr_agc_format_type format
)
{
  int16 filtered_power_dB640 = RF_CDMA_MDSP_MIN_TX_PWR ; 
  const int MIN_TX_POWER = -60 ;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 filtered_tx_power = MIN_TX_POWER ; /* holds the filtered tx_total power */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

   /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_filtered_tx_power(): Invalid logical device "
                         "for device %d", device);
    return filtered_tx_power ;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->tx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_filtered_tx_power(): Invalid tx_mdsp_path %d",
              logical_dev->tx_mdsp_path );
    return filtered_tx_power ;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL != dev_status_r )
  {
    int tc = 0 ; /* Turnaround constant */
    int16 coex_tx_power ;
    /* Get per carrier filtered Rx */
    rf_hdr_mdsp_get_filtered_tx_power( logical_dev->tx_mdsp_path, 
                                       &filtered_power_dB640 ,
                                       &coex_tx_power ) ;
    filtered_tx_power = filtered_power_dB640 ;
    tc = rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

    /* validate format */
    switch ( format )
    {
      case RFM_CDMA_AGC_FORMAT__RAW:
      RF_MSG ( RF_LOW , "rfm_hdr_get_filtered_tx_power(): "
                 "Format: AGC Raw " ) ;  
        break ;

      case RFM_CDMA_AGC_FORMAT__DBM10:
        filtered_tx_power = 
          rf_cdma_convert_txagc ( RFM_CDMA_AGC_FORMAT__DBM10,
                                    tc, filtered_tx_power );
        break ;

      case RFM_CDMA_AGC_FORMAT__DBM256:
      {
        RF_MSG ( RF_LOW , "rfm_hdr_get_filtered_tx_power(): "
                 "Format: AGC dBm256 " ) ;  
        filtered_tx_power =
          rf_cdma_convert_txagc ( RFM_CDMA_AGC_FORMAT__DBM256 ,
                                      tc, filtered_tx_power );
        break ;
      }

      default: 
      {
        RF_MSG_1(RF_ERROR, "rfm_hdr_get_filtered_tx_power(): dB format %d unsupported", 
                 format) ;
        break ;
      }
    } /* switch */ 
  } /* if ( NULL != dev_status_r ) */
  else
  {
    RF_MSG ( RF_ERROR, "rfm_hdr_get_filtered_tx_power(): Invalid device pointer" ); 
    }

  return filtered_tx_power ;

} /* rfm_hdr_get_filtered_tx_power() */
/*! @} */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the Tx power filtered over the last hundred milliseconds.

  @details
  Returns Tx power reported by the FW Tx AGC filter.  This filter is intended
  to be a slowly varying record of recent Tx power, and is commonly used as
  an input to a multi-radio concurrency manager.

  The Tx power is filtered by the firmware with a time constant around 100ms,
  but this is not set by the RF driver, so it may change without notice.

  The API supports overall querying filtered Tx power.  Since 
  the firmware Tx AGC does not support per-carrier Tx AGC
  filtering, then the function will print an RF error message in
  the F3 log, and return the overall filtered Tx power.  Overall
  filtered Tx power should always be valid.

  @param device
  The radio path that is being queried.

  @param filter_type 
  The FW supports few types of Filter
  algorithms. This parameter slects the filter desired by higher
  layers of SW.

  @param format
  The format the Tx power should be returned as.

  @return
  Filtered Tx power in dBm10 format.
*/
int16
rfm_hdr_get_filtered_tx_power_v2
(
  const rfm_device_enum_type device , 
  const rfm_filter_type filter_type 
)
{
  const int MIN_TX_POWER = -60 ;
  int16 filtered_tx_power = 
    MIN_TX_POWER ; /* holds the filtered tx_total power */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_filtered_tx_power_v2(): Invalid logical device "
                         "for device %d", device);
    return filtered_tx_power ;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->tx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_get_filtered_tx_power_v2(): Invalid tx_mdsp_path %d",
              logical_dev->tx_mdsp_path );
    return filtered_tx_power ;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL != dev_status_r )
  {
    int tc = 0 ; /* Turnaround constant */
    int16 coex_tx_power_dB640 = RF_CDMA_MDSP_MIN_TX_PWR ;
    int16 filtered_power_dB640 = RF_CDMA_MDSP_MIN_TX_PWR ; 

    /* Get per carrier filtered Rx */
    rf_hdr_mdsp_get_filtered_tx_power( logical_dev->tx_mdsp_path ,
                                       &filtered_power_dB640 ,
                                       &coex_tx_power_dB640 ) ;

    tc = rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

    RF_MSG_3 ( RF_LOW , "rfm_hdr_get_filtered_tx_power_v2(): "  
               "| TC: %d | filtered_power_dB640 %d | CoEx_dB640 %d | " ,
               tc , filtered_power_dB640 , coex_tx_power_dB640 ) ;

    /* validate filter type */
    switch ( filter_type )
    {
      case RFM_TX_FILTER_RMS_POWER :
      {
        filtered_tx_power = filtered_power_dB640 ;
        RF_MSG ( RF_LOW , "rfm_hdr_get_filtered_tx_power_v2(): "
                 "Filter Type: RFM_TX_FILTER_RMS_POWER " ) ;  
        break ;
      }

      case RFM_TX_FILTER_POWER_WHEN_TX_ON:
      {
          filtered_tx_power = coex_tx_power_dB640 ;
        RF_MSG ( RF_LOW , "rfm_hdr_get_filtered_tx_power_v2(): "
                 "Filter Type: RFM_TX_FILTER_POWER_WHEN_TX_ON " ) ;  
        break ;
      }

      default: 
      {
        RF_MSG_1 ( RF_ERROR, "rfm_hdr_get_filtered_tx_power_v2(): "
                             "Unsupported filter type: %d ", filter_type ) ;
        break ;
      }
    } /* switch */ 
    filtered_tx_power =
      rf_cdma_convert_txagc ( RFM_CDMA_AGC_FORMAT__DBM10,
                              tc , filtered_tx_power ) ;
    RF_MSG_1 ( RF_LOW , "rfm_hdr_get_filtered_tx_power_v2(): "
                "filtered Tx power is: %d " , filtered_tx_power ) ;   
    
  } /* if ( NULL != dev_status_r ) */
  else
  {
    RF_MSG ( RF_ERROR, "rfm_hdr_get_filtered_tx_power_v2(): Invalid device pointer" ); 
  }

  return filtered_tx_power ;

} /* rfm_hdr_get_filtered_tx_power_v2_v2() */
/*! @} */

/*============================================================================*/
/*!
  @name HDR State Query Functions

  @brief
  Functions to query HDR state
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if a radio path is asleep.

  @param device
  The radio path to get the state from

  @return
  True if the radio is sleeping, otherwise false.
*/
boolean
rfm_hdr_is_sleeping
(
  const rfm_device_enum_type device
)
{
  rfm_mode_enum_type curr_mode; /* current Mode */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  boolean hdr_sleep; /* Flag indicating CDMA 1x Sleep State */

  /* Get Current Mode */
  curr_mode =  rfm_get_current_mode(device);

  /* Validate the current Mode */
  if ( curr_mode != RFM_1XEVDO_MODE )
  {
    RF_MSG_2( RF_ERROR, "rfm_hdr_is_sleeping: Bad Mode - %d on device %d ",
              curr_mode, device );
    return TRUE;   
  }

  /* Get the Device Status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( dev_status_r == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_is_sleeping(): Invalid device %d data",
               device);
    return TRUE;   
  }

  if ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_INVALID )
  {
    RF_MSG_2( RF_ERROR, "rfm_hdr_is_sleeping: Invalid State - %d on device %d ",
              curr_mode, device );
    return TRUE;   
  }

  /* Determine the Sleep Status */
  if ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_SLEEP )
  {
    hdr_sleep = TRUE; /* HDR is sleeping */
  }
  else
  {
    hdr_sleep = FALSE; /* HDR is not sleeping */
  }

  return hdr_sleep;

} /* rfm_hdr_is_sleeping */

/*! @} */

/*============================================================================*/
/*!
  @name HDR Dynamic Tx Control Functions

  @brief
  Functions to control the dynamic behavior or the HDR transmitter
  while in a call.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Inform the RF driver when the HDR reverse link configuration changes.

  @details
  The RF driver may need to change the transmitter configuration for
  the various reverse link configurations in HDR.  This function allows
  HDR RMAC to let the RF driver know when the reverse link configuration
  has changed, so the RF driver can update PA switch points, waveform
  compensations, max power limits, and any other parameters that depend
  on the reverse link channel configuration.

  This function may take into account other phone state such as the
  temperature and battery level when compensating for the reverse
  link channel configuration.

  @param device
  The radio path to configure.

  @param rate
  The current reverse link rate configuration.  This rate is the RRI in
  HDR Rev0, or RF_HDR_REVA_RATE_OFFSET if the radio is in RevA or higher
  mode.
*/
void
rfm_hdr_notify_data_rate
(
  const rfm_device_enum_type device,
  const uint8 rate
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status read Pointer */
  /* NV related variables */
  const rf_hdr_config_type *rf_hdr_config_ptr; /* Pointer to HDR RF NV data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rf_cdma_mdsp_pa_stm_config_type pa_stm_data; /* PA Params holder */

  dev_status_r = rf_cdma_get_device_status( device );

  if ( dev_status_r == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_hdr_notify_data_rate: Dev status ptr is NULL" );
    return;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev ==  NULL)
  {
    RF_MSG( RF_ERROR, "rfm_hdr_notify_data_rate: logical_dev is NULL." );
    return;   
  }

  /* ensure that NV is valid for given device and band */
  rf_hdr_config_ptr = rf_cdma_nv_get_hdr_data( 
                               logical_dev->tx_nv_path[dev_status_r->curr_band],
                               dev_status_r->curr_band);

  if ( rf_hdr_config_ptr ==  NULL)
  {
    RF_MSG( RF_ERROR, "rfm_hdr_notify_data_rate: rf_hdr_config_ptr is NULL." );
    return;   
  }

  /* update the RRI data structure only when all conditions are satisfied */
  if ( rate < RF_HDR_TX_AGC_RRI_NUM )
  {
    rf_cdma_data_set_hdr_rri( device, (rf_hdr_tx_agc_rri_type)rate );
  }
  else
  {
    rf_cdma_data_set_hdr_rri( device, RF_HDR_TX_AGC_9K6_BPS_RRI );
  }

  /* Reconfigure TxAGC based on Data rate notification if device is
  in RxTx state */
  if ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX )
  {
    rf_hdr_tx_agc_update_pa_params( device, dev_status_r->num_carriers,
                                    dev_status_r->curr_band, 
                                    dev_status_r->lo_tuned_to_chan,
                                    rf_hdr_config_ptr, &pa_stm_data );

    /* In this scenario we need to update PA switchpoints ONLY and not the Xpt
       config, because DPD config will be zero, that will force voice
       XPT type into APT type by default every time. data.
       We're only updating switchpoints, and not linearizer data */
    pa_stm_data.update_xpt_mode = FALSE;

    if ( FALSE == 
         rf_cdma_mdsp_config_pa_params( 
                             RFM_1XEVDO_MODE,
                             dev_status_r->txlm_handle,
                             rf_cdma_xpt_get_cfg_param(device, XPT_CFG),
                             &pa_stm_data ) )
    {
      RF_MSG( RF_ERROR, "rfm_hdr_notify_tx_config(): "
                        "rf_cdma_mdsp_config_pa_params() failed." ) ;
    }
    RF_MSG_1( RF_LOW, "rfm_hdr_notify_data_rate: Dev %d", device );
  }
} /* rfm_hdr_notify_data_rate */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Inform the RF driver if the HDR transmitter is in access mode, or in
  traffic mode.

  @details
  The RF driver may need to change the transmitter configuration for
  access and traffic modes in HDR.  This function allows HDR L1 to let
  the RF driver know when it is in access state, and when it is in
  traffic state.

  L1 Can use this API to set RF state to TRAFFIC.
  RF sets itself to Non-TRAFFIC state when rfm_sleep_tx() is successful.

  @param device
  The radio path to configure.

  @param is_traffic_state
  Indicates if the transmitter should be in traffic state.  If this
  parameter is true, the radio should be configured for traffic state.
  If this parameter is false, the radio should be configured for
  access state.
*/
void
rfm_hdr_set_pa_access_probe_config
(
  const rfm_device_enum_type device,
  const boolean is_traffic_state
)
{
  boolean success = TRUE; /* tracks state of API thru various calls */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  /* NV related variables */
  const rf_hdr_config_type *rf_hdr_config_ptr;   /* Pointer to HDR RF NV data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  /* Structure to hold PA State Machine Data */
  rf_cdma_mdsp_pa_stm_config_type pa_stm_data;
  
  dev_status_r = rf_cdma_get_device_status( device );
  dev_status_w = rf_cdma_get_mutable_device_status( device ); 
  
  /*Perform NULL pointer check*/
  if(  dev_status_r  == NULL || dev_status_w == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_set_pa_access_probe_config(): Invalid "
               "device %d data", device);
    success = FALSE;
  }
  else
  {
    /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( device );

    if ( logical_dev ==  NULL)
    {
      RF_MSG( RF_ERROR, "rfm_hdr_set_pa_access_probe_config: "
                        "logical_dev is NULL." );
      success = FALSE;   
    }
  }

  /* all pointers are valid */
  if ( success == TRUE )
  {
    /* Update the CDMA data structure indicating that Traffice State is
    enabled */
    rf_cdma_data_set_traffic_state( device, is_traffic_state );

    /* ensure that NV is valid for given device and band */
    rf_hdr_config_ptr = rf_cdma_nv_get_hdr_data( 
                              logical_dev->tx_nv_path[dev_status_r->curr_band],
                              dev_status_r->curr_band);

    if ( rf_hdr_config_ptr ==  NULL)
    {
      RF_MSG( RF_ERROR, "rfm_hdr_set_pa_access_probe_config: "
                        "rf_1x_config_ptr is NULL." );
      return;   
    }

    /* Reconfigure TxAGC based on Access/Traffic notification if device is
    in RxTx state */
    if ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX )
    {
      /* Update the PA Params based on device status */
      success &= rf_hdr_tx_agc_update_pa_params( device, 
                                                 dev_status_r->num_carriers,
                              dev_status_r->curr_band, 
                              dev_status_r->lo_tuned_to_chan,
                                                 rf_hdr_config_ptr, 
                                                 &pa_stm_data );


      /* We are transitioning from Access to Voice, in this scenario we need
      to update PA switchpoints ONLY and not the Xpt conffig, because for 
      Access switchpoints DPD config will be zero, that will force voice
      XPT type into APT type by default every time */
      pa_stm_data.update_xpt_mode = FALSE;

      /* Update PA state params to RFLM */
      success &= rf_cdma_mdsp_config_pa_params( 
                             RFM_1XEVDO_MODE,
                              dev_status_r->txlm_handle,
                             rf_cdma_xpt_get_cfg_param(device, XPT_CFG),
                             &pa_stm_data );

      if ( success == TRUE )
      {
      RF_MSG_2( RF_LOW, "rfm_hdr_set_pa_access_probe_config: "
                        "Dev %d, Traffic %d",
                device, is_traffic_state );
    }
      else
      {
        RF_MSG_2( RF_ERROR, "rfm_hdr_set_pa_access_probe_config: "
                            "Dev %d, Traffic %d. Failed",
                  device, is_traffic_state );
      }
    }
  } /* if ( success == TRUE ) */
  
  return;
} /* rfm_hdr_set_pa_access_probe_config */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the HDR Tx nominal power as specified by the AN
 
  @details
  As per the HDR standard, the Access Network (AN) may override the turnaround
  constant (TC) that the Access Terminal (AT) uses (73 and 76 for low and high 
  bands respectively).
 
  When the AN orders the AT to use a specific TC (sent as part of Access 
  Parameters message), the AT will need to use this new TC in its calculations.
 
  This API stores the new TC (in dB256, as required by L1) in the RF data 
  structures
 
  @param device
  The RF device for which nominal power is to be updated

  @param nominal_pwr
  Nominal power (turnaround constant) as set by the AN (dB)
 
*/
void
rfm_hdr_set_nominal_pwr
(
  const rfm_device_enum_type device,
  const int16 nominal_pwr
)
{
  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* AN tells the AT to use a specific turnaround constant - this needs to be 
     flipped in sign when the AT uses it, since (Rx + Tx = TC).  Convert to 
     db256 and then update the nominal power */ 
  rf_cdma_data_set_hdr_nominal_pwr( device, -nominal_pwr*256 );

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return;
} /* rfm_hdr_set_nominal_pwr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the HDR Tx closed loop power adjustment.

  @details
  This API allows HDR to set the desired closed loop power if the Tx AGC is in 
  override mode.

  This API does not set or clear the override mode status.

  @param device
  The radio path to configure.

  @param power_dbx2
  Tx power in 1/2 dB units
 
  @return
  TRUE on success and a FALSE on failure 

*/
boolean 
rfm_hdr_set_tx_power_adjust
(
  const rfm_device_enum_type device,
  const int16 power_dbx2
)
{
  boolean result; /* final result of API */

  /* device pointer needed to access members of data structure */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  dev_status_r = rf_cdma_get_device_status( device );

  if ( dev_status_r == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_hdr_set_tx_power_adjust(): Invalid "
               "device %d data", device);
    result = FALSE;   
  } /* if ( dev_status_r == NULL ) */
  else
  {
    /* Validate the current state */
    if ( rfm_hdr_validate_state( device, RF_CDMA_STATE_RXTX ) == 
         RFM_CDMA_HEALTHY_STATE )
    {
      rf_hdr_set_tx_power_adjust( device,dev_status_r->curr_band, power_dbx2, 
                                  dev_status_r->data_hdr.hdr_nominal_pwr_dB256);  
      result = TRUE;
    }
    else
    {
      result = FALSE;
    }
  } /* if ! ( dev_status_r == NULL ) */
  
  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return result;

} /* rfm_hdr_set_tx_power_adjust */

/*! @} */

/*============================================================================*/
/*!
  @name Thermal Backoff

  @brief
  Functions to be used for Thermal Backoff.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF interface to control Thermal Backoff state.

  @param device
  The device to configure thermal backoff
 
  @param thermal_backoff_state
  The thermal backoff state to configure
*/
void
rfm_hdr_set_thermal_state
(
  const rfm_device_enum_type device,
  rfm_hdr_thermal_state_type thermal_backoff_state
)
{
  /* Update max limit */
  rf_hdr_plim_update_thermal_backoff_limit( device, thermal_backoff_state );
}

/*! @} */ /* End of Thermal Backoff Section */


int16
rfm_hdr_get_highest_tx_max_power_limit_for_current_band
(
   rfcom_device_enum_type       const device,
   rfcom_agc_format_type        const format
) 
{ 
  int16 max_power_limit_in_band_dB10 = 0 ;
  switch ( format )
  {
    case RFCOM_AGC_FORMAT__DBM10 :
    {
      boolean success = 
      rf_hdr_get_max_tx_power_for_band ( device , &max_power_limit_in_band_dB10 ) ;
      if ( FALSE == success )
      {
        RF_MSG ( RF_ERROR , 
          "rfm_hdr_get_highest_tx_max_power_limit_for_current_band(): "
          "Call to rf_hdr_get_max_tx_power_for_band() failed " ) ;
       break ;
     }
    }
    default :
    {
        RF_MSG ( RF_ERROR , 
          "rfm_hdr_get_highest_tx_max_power_limit_for_current_band(): "
          "Unsupported Format " ) ;
        /* Redundant Break in case "default:" is not the case */
        break ;
    }
  } /* switch */
  return max_power_limit_in_band_dB10 ; 
}

boolean 
rfm_hdr_get_funneling_metrics
(
  rfm_device_enum_type device ,
  rfm_hdr_per_carrier_funneling_bias_type* funnel_bias_array_ptr
)
{
  return (rf_hdr_plim_get_funneling_metrics (device , funnel_bias_array_ptr) ) ;
}


/*============================================================================*/
/*!
  @name Antenna Tuner

  @brief
  This section includes functions to perform actions related to antenna tuner 
  algorithm.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Antenna Tuner action time indication
 
  @details
  This function shall be used by L1 to indicate the antenna tuner action time
  to HDR RF SW. All antenna tuner updates will be aligned with this action
  time invoked by L1. Based on the action time, RFSW will schedule the
  dynamic antenna tuner update to FW on the requested device and its associated
  devices. The dynamic update will be executed only for the case when the
  antenna tuner module has pending scenarios for the given device.
 
  @param device
  RFM Device for which dynamic antenna tuner update is to be done.
  
  @return
  Error code indicating the status of the execution of the action time update.
  Negative return value indicates a failure, while non-negative indicates
  success.
*/
int32
rfm_hdr_antenna_tuner_action_time
(
  const rfm_device_enum_type device
)
{
  int32 ret_val = RFM_CDMA_HEALTHY_STATE;
  boolean api_status = TRUE;
  static boolean first_action_time = TRUE;
  static rf_time_tick_type prof_tick;
  rf_time_type prof_time = 0; /* Time spent in function */
  rf_time_type time_bet_actions = 0; /* Time spent in function */
  const rf_cdma_data_status_type *m_dev_r;/* Dev Status Read Ptr */
  rfm_cdma_band_class_type band;
  const rfc_cdma_logical_device_params_type *m_logical_dev;

  void* tuner_event_buf;

  #ifdef FEATURE_RF_ASDIV
  /* If AsDiv switching is already in progress. Abort AOL update */
  if (rfcommon_asdiv_is_switch_in_progress() == TRUE) 
  {
   RF_MSG(RF_HIGH, "rfm_hdr_antenna_tuner_action_time: [ASDIV]Switch in progress, "
                   "Skip update");
      return RFM_CDMA_HEALTHY_STATE;
    }
  #endif

  /* Get Device Status */
  m_dev_r = rf_cdma_get_device_status( device );
  m_logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( m_dev_r == NULL || m_logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_antenna_tuner_action_time: Dev %d - NULL "
              "device or RFC data", device );
    return RFM_CDMA_ERROR_NULL_DATA;
  } /* if ( dev_status_r == NULL ) */

  if ( first_action_time == FALSE )
  {
    time_bet_actions = rf_time_get_elapsed( prof_tick, RF_USEC );
  } /* if ( first_action_time == FALSE ) */
  else
  {
    first_action_time = FALSE;
  } /* if ! ( first_action_time == FALSE ) */
  
  /* Start to Time Measurement */
  prof_tick = rf_time_get_tick();

  /* Try to acquire Critical Section Lock. If the lock cannot be obtained 
  right away, bailout and skipp rest of the opration. */
  if( rf_common_try_enter_critical_section(rf_cdma_get_lock_data()) == FALSE )
  {
    /* Throw F3 Message */
    RF_MSG( RF_HIGH, "rfm_hdr_antenna_tuner_action_time : Invalid RF state,"
                       "AOL update skipped" );
    ret_val = RFM_CDMA_ERROR_INVALID_DEV_STATE;
  }
  else
  {
    /* get band */
    band = m_dev_r->curr_band;

    if( m_dev_r->data_hdr.rf_state == RF_CDMA_STATE_RX || 
        m_dev_r->data_hdr.rf_state == RF_CDMA_STATE_RXDIV )
    {
      RF_MSG( RF_MED, "rfm_hdr_antenna_tuner_action_time: "
                      "Device in Rx only state. No tune code update " );
    }
    else if( m_dev_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX )
    {
      /* Update tune code for Primary Device. */
      if ( m_logical_dev->ant_tuner[band] != NULL )
      {
        rfcom_band_type_u rf_band;
        boolean is_rx_mode;
        uint16 tuner_mdsp_chain;

        rfcommon_tuner_update_cb_data_type* tuner_cb_data;

        rf_buffer_intf *tuner_script;
        rfcommon_mdsp_event_handle** tuner_event_handle_ptr;

        /* 1. Determin mdsp chain */
        rf_band.cdma_band = band;

        if ( m_dev_r->data_hdr.rf_state == RF_CDMA_STATE_RX )
      {
        is_rx_mode = TRUE;
          tuner_mdsp_chain = (uint16)m_logical_dev->rx_mdsp_path;;    
      }
      else
      {
        is_rx_mode = FALSE;
          tuner_mdsp_chain = (uint16)m_logical_dev->tx_mdsp_path;
        }

        if( tuner_mdsp_chain >= RF_TUNER_CHAIN_NUM )
        {
          RF_MSG_1( RF_ERROR, "rfm_hdr_antenna_tuner_action_time: "
                              "Invalid MDSP chain %d", tuner_mdsp_chain );
          return RFM_CDMA_ERROR_FAILURE;
        }

        /* 2. Create Tuner Update Event Call Back Data Object  */
        tuner_event_buf = rf_cdma_mc_get_aol_tx_dm_buffer ( device ) ; 

        tuner_cb_data = rfcommon_tuner_event_create_cb_data(tuner_event_buf, RFM_1X_MODE,
                                                            m_dev_r->txlm_handle);
        if( tuner_cb_data == NULL )
        {
          RF_MSG( RF_ERROR, "rfm_hdr_antenna_tuner_action_time: "
                            "CB Buffer Create failed" );
          return RFM_CDMA_ERROR_FAILURE;
       }

        /* 3. Create RF script object */
        tuner_script = rf_buffer_create ( 0, RF_CDMA_MAX_RFFE_SCRIPT_SIZE, 0 ) ;   

        tuner_event_handle_ptr = &(tuner_cb_data->tuner_ccs_event_handle);
      
        /* 5(1). Obtain tuner update RF script on Primary Device */

        #ifdef FEATURE_RF_ASDIV
        /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
        rfcommon_asdiv_manager_enter_lock();
        #endif
        api_status &=
          rf_cdma_atuner_update_tune_code(m_logical_dev->ant_tuner[band],
                                          RFM_1XEVDO_MODE,
                                          rf_band,
                                          m_dev_r->lo_tuned_to_chan,
                                          is_rx_mode,                                               
                                          tuner_mdsp_chain,
                                          RFDEVICE_CREATE_SCRIPT,
                                          tuner_script,
                                          0 ,
                                          (void*)tuner_cb_data );
        #ifdef FEATURE_RF_ASDIV
        /* Releae the ASDIV manager interface */
        rfcommon_asdiv_manager_release_lock();
        #endif

        /* 5(2). Also Update Tune Code for Diversity Device, if applicable  */
        if( api_status == TRUE && m_dev_r->assoc_dev != RFM_INVALID_DEVICE )
        {   
          const rfc_cdma_logical_device_params_type *s_logical_dev;
          const rf_cdma_data_status_type *s_dev_r;/* Dev Status Read Ptr */

          s_dev_r = rf_cdma_get_device_status( m_dev_r->assoc_dev );
          s_logical_dev = rfc_cdma_get_logical_device_params( m_dev_r->assoc_dev );

          if( s_logical_dev != NULL && s_dev_r != NULL)
          {
            #ifdef FEATURE_RF_ASDIV
            /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
            rfcommon_asdiv_manager_enter_lock();
            #endif
            api_status &=
              rf_cdma_atuner_update_tune_code(s_logical_dev->ant_tuner[band],
                                              RFM_1XEVDO_MODE,
                                              rf_band,
                                              s_dev_r->lo_tuned_to_chan,
                                              TRUE,                                                     
                                               RF_TUNER_CHAIN_1,
                                              RFDEVICE_CREATE_SCRIPT,
                                              tuner_script,
                                              0,
                                              (void*)tuner_cb_data );
            #ifdef FEATURE_RF_ASDIV
            /* Releae the ASDIV manager interface */
            rfcommon_asdiv_manager_release_lock();
            #endif
          }
          else
          {
            RF_MSG( RF_HIGH, "rfm_hdr_antenna_tuner_action_time: "
                              "NULL logical dev pointer for slave device " );
            api_status = FALSE;
          }
        }/* if( api_status == TRUE && dev_r->assoc_dev != RFM_INVALID_DEVICE ) */      

        /* Check for total number of writes in the script, if 0, skip Tuner AOL
        update*/
        if( rf_buffer_get_num_trans( tuner_script, RF_BUFFER_RFFE ) == 0 )
        {
          RF_MSG( RF_HIGH, "rfm_hdr_antenna_tuner_action_time: "
                           "Zero transactions in tune code script. "
                           "Skip AOL update " );
          api_status = FALSE;
        }

        /* 6. Update mdsp data and register callback for FW response.
        If master chain tuner token is not valid, skip and clean up. */
        if( api_status == TRUE )
        {
          *tuner_event_handle_ptr = NULL;
          api_status &= rfcommon_mdsp_tuner_open_loop_queue_script( tuner_event_buf,
                                                                    RF_CDMA_MDSP_CCS_EVENT_AOL_ID,
                                                                    tuner_script,
                                                                    tuner_event_handle_ptr,
                                                                    rfcommon_tuner_open_loop_response_handler,
                                                                    (void *)tuner_cb_data);
          }

        /* 7. If any previous API fails, clean up objects and free memory */
        if ( api_status == FALSE )
        {
          /* No need to continue, force cleaning up all allocated memory */
          rfcommon_tuner_event_cleanup( tuner_cb_data );
          ret_val = RFM_CDMA_ERROR_FAILURE;
        }

        /* 8. Always destroy RF buffer script object after MDSP call flow*/
        if( tuner_script != NULL )
        {
          rf_buffer_destroy( tuner_script );
        }

      }/* if ( logical_dev->ant_tuner[band] == NULL ) */
    }/* else if( RF_CDMA_STATE_RXTX ) */

    /* Release Critical Section Lock */
    rf_common_leave_critical_section(rf_cdma_get_lock_data());

  }
  /* End Time Measurement */
  prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );

  RF_MSG_4( RF_MED, "rfm_hdr_antenna_tuner_action_time: [Status %d] Dev %d, "
            "ActionTime %d us, Profile %d", ret_val, device, time_bet_actions,
            prof_time );

  return ret_val;
} /* rfm_hdr_antenna_tuner_action_time */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare RF to make it ready for impending QTA.

  @param device
  The device on which QTA will be performed.
 
  @param params
  QTA params that need to be configured.
 
  @return
  Status of function execution.
  TRUE: API Successful.
  FALSE: API will return error.
*/
boolean 
rfm_hdr_prep_qta 
( 
   const rfm_device_enum_type   device  , 
   rfm_hdr_qta_params*          params  
)
{
  boolean result = FALSE ; /* final result of API */

  rf_time_type qta_prep_time ; /* Time spent in function */
  rf_time_tick_type qta_prep_tick ; /* Start time of function */

  /* Start to Time Measurement */
  qta_prep_tick = rf_time_get_tick() ;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section ( rf_cdma_get_lock_data() ) ;

  /* This API will perform all pre-QTA preperations */     
  result =  rf_cdma_mc_prep_qta ( device , (rfm_cdma_qta_params*) params ) ;

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  /* End Time Measurement */
  qta_prep_time = rf_time_get_elapsed( qta_prep_tick, RF_USEC ) ;
  
  RF_MSG_3 ( RF_MED , "rfm_hdr_prep_qta: [Status %d] Dev %d, "
             "TimeSpent %d us ", result, device , qta_prep_time ) ;
  
  return result ;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tell RF that QTA has concluded so that RF can prepare for
  normal opecation.

  @param device
  The device on which QTA was performed.
 
  @return
  Status of function execution.
  TRUE: API Successful.
  FALSE: API will return error.
*/
boolean 
rfm_hdr_end_qta 
( 
  const rfm_device_enum_type   device  
)
{
  boolean result = FALSE ; /* final result of API */

  rf_time_type enable_time ; /* Time spent in function */
  rf_time_tick_type enable_tick ; /* Start time of function */

    /* Start to Time Measurement */
  enable_tick = rf_time_get_tick();

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* This API will perform all pre-QTA preperations */     
  result =  rf_cdma_mc_end_qta ( device ) ;

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  /* End Time Measurement */
  enable_time = rf_time_get_elapsed( enable_tick, RF_USEC ) ;
  
  RF_MSG_3 ( RF_MED , "rfm_hdr_end_qta: [Status %d] Dev %d, "
             "TimeSpent %d us ", result, device , enable_time ) ;
  
  return result ;
}

#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */

