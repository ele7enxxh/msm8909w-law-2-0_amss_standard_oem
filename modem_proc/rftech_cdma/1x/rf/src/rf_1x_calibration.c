/*! 
  @file
  rf_1x_calibration.c
 
  @brief
  This file defines interfaces to be used for RF Calibration
 
  @addtogroup RF_CDMA_1X_CALIBRAION
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rf_1x_calibration.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/29/14   zhw     Compiler warning fix
03/06/14   sty     KW fix - check for null pointer
02/20/14   spa     Calculate LNA offsets relative to previos gain state
01/20/14   spa     Added support for immediate LNA gain state writes
11/25/13   spa     Calculate offsets relative to LNA state 0
11/25/13   spa     Use int32 math for LNA offset calculation 
11/11/13   spa     Send in rxlm handle when calling LNA override
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
05/31/13   JJ      Change rf_1x_write_nv input band type
05/28/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
01/17/12   sty     Changed agc_acc_val from int16 to int32 to account for 
                   overflow
12/26/12   shb     Updates for different physical rx_device per band
12/20/12   sty     Added band check in rf_1x_calibrate_lna()
06/29/12   spa     Compiler warning fix 
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
06/14/12   cd      NV cleanup. Removed usage of old im2 and intelliceiver 
                   nv structures 
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
04/02/12   aro     Removed unused 1x Cal v3 functions
03/28/12   aro     Added Current RF State as the parameter for ic calibration
                   to be used for PM override function to check if the state
                   has changed between the time when PM Ocerride command is
                   posted and handled
03/28/12   aro     Added check in LNA cal and ic Cal functions to make sure
                   that teh SW is in calibration mode before proceeding
03/22/12   spa     Added max device checks for rf_cdma_get_cal_data() and 
                   rf_cdma_get_mutable_cal_data() 
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/15/12   spa     Added max device check for rf_cdma_get_device_status() 
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
11/14/11   aro     Mainlined the debug flags
09/14/11   sar     Fixed KW errors. 
08/10/11   aro     Fixed the LNA offset calculation for Cal v3
08/08/11   aro     Added cal v3 rx measurement funciton to pass RxAgc in
                   1/10th dB unit
07/30/11   aro     Removed old NV access from iC calibration function
07/25/11   cd      Null pointer handling for NV get APIs
07/19/11   aro     [1] Updated measure rx function to return status
                   [2] Updated Cal data query function to be done per device to
                   support simultaneous multiple Rx Cal on Cal v3
07/18/11   aro     Added intelliceiver HDR support
07/15/11   aro     Pushed JD code analysis inside RTR driver
07/13/11   aro     Updated intelliceiver interfaces not to have device param
07/13/11   aro     Updated "Get Starting PM" interface to send iC data
07/07/11   aro     Added code to update NV table with iC cal data at the
                   end of iC calibration
07/06/11   aro     [1] Fixed iC Calibration for Far Jammer
                   [2] Updated Override PM interface to return status
07/05/11   aro     Added code to break FOR loop for cal failure
07/05/11   aro     Moved the check of iC support to MC
07/01/11   aro     Added interface to perform Intelliceiver Calibration
06/29/11   aro     [1] Changed Set Power Mode interface to pass logical dev
                   and cal data from upper layer
                   [2] Renamed intelliceiver function name
06/15/11   aro     Removed RxAGC Sampling time debug variable
06/14/11   bmg     Fix array bounds and general pointer wonkyness
06/07/11   Saul    Updates for multi-carrier driver.
06/06/11   cd      rf_cdma_nv_add_cal_chan() takes nv_path parameter
06/06/11   cd      Changed cal items map to array of structs
06/02/11   cd      Modified cal items map struct type
06/02/11   aro     Removed debug flag
06/01/11   aro     Added debug flag for wait tiem for RxAGC sampling
06/01/11   aro     Removed extra wait for RxAGC sampling
05/31/11   aro     Added NULL pointer check for Logical Device param
05/27/11   cd      - Added new LRU chan list variable
                   - Added support to write SV path items
05/24/11   aro     Added Logical Device Mapping to MDSP Chain / NV for SV
05/24/11   aro     Added Logical Device Mapping to MDSP Chain for SV support
05/16/11   aro     Compiler Warning fixed
05/16/11   aro     Documentation Update
05/16/11   aro     [1] Added debug flags to bypass NV
                   [2] Added proper unit conversion for expected RxAGC and Offset values
05/16/11   aro     [1] Added Rx Measurment config and query function
                   [2] Removed the AGC settling time ftom AGC Sampling function
                   to be handled in calling function
04/22/11   cd      Replaced old Rx cal channel list with new data structure
04/22/11   aro     Compiler Warning Fix
04/21/11   cd      Removed old NV structure LNA offsets
04/21/11   aro     Updated CDMA Data interface for RF Calibraiton/Init
04/21/11   aro     Updated CDMA Device Data to have read and mutable interfaces
04/21/11   aro     [1] Moved Get Calibration compensation function to Common
                   [2] Moved ADC Clock Manager to HAL
04/20/11   cd      - Write NV uses new data structures for G1-G4
                   - Renamed rf_cdma_compute_gain_freq_nv to
                     rf_1x_compute_gain_freq_nv and added error handling
04/19/11   cd      Update new NV data structures with LNA offset calibrated val
04/19/11   cd      - Digital gain compensation is subtracted from LNA offsets 
                     during cal, to reflect update in Rx AGC programming
                   - Changed LNA offset return type to int16
03/30/11   aro     Increased Pre-RxAGC Sampling wait time to 2 mS
03/30/11   aro     Reduced AGC Sample num and wait time
03/24/11   aro     Defined unique RxAGC Read structure for 1x
03/24/11   aro     Removed get Highest LNA gain state function
03/22/11   bmg     Replaced old NV structure DVGA offset with new NV structure
                   gain calibration.
                   Generalized rf_1x_compute_dvga_freq_nv() to all gain states
03/19/11   bmg     Added IM2 cal NV write to rf_1x_write_nv()
03/18/11   aro     Moved code to store offset values into DVGA/LNA measurement
                   function to accomodate CompositeCal to use same core API
03/18/11   aro     Changed the RxAGC samples to 6 and wait to 0.5 ms
03/16/11   aro     Changed Device Param to MDSP Chain
03/14/11   aro     Consolidated RxAGC Reads to one data structure
03/14/11   aro     Merged Set DVGA/LNA Offset to a unified interface
03/01/11   aro     Moved wait at the start of RxAGC measurement
02/24/11   aro     Renamed type name to rf_cdma_nv_band_items_type
02/17/11   aro     Renamed intelliceiver function
02/17/11   aro     Moved LNA Gain Step definition to RFM
02/16/11   aro     Removed Power Mode Param from Measure LNA/DVGA interfaces
02/15/11   aro     Changed the Get Cal Comp interface to take LM buffer
02/15/11   aro     Moved set DVGA and LNA offset interface to MDSP
02/07/11   aro     Update not to store DVGA offset for MAX power Mode
02/03/11   aro     Added interface to apply Compensation value
02/02/11   aro     F3 Update
02/02/11   aro     Removed Power Mode Param from Measure DVGA/LNA
02/01/11   aro     Added support for NV Commit
02/01/11   aro     Added LNA Calibration Support
02/01/11   aro     Added support to perfrom DVGA Calibration
02/01/11   aro     Renamed function name
02/01/11   aro     Promoted Power Mode types to RFM
02/01/11   aro     Added DVGA Calibrate and Store
01/31/11   aro     Added LNA Calibration Support
01/31/11   aro     Initial Revision.

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_cdma_carrier_types.h"
#include "rf_1x_calibration.h"
#include "rf_1x_mdsp.h"
#include "rf_cdma_intelliceiver.h"
#include "rf_cdma_data.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_lna.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "rfdevice_cdma_interface.h"
#include "rfcommon_msg.h"
#include "DALSys.h"
#include "rfcommon_math.h"
#include "rfm_internal.h"
#include "rf_cdma_mc.h"

/*----------------------------------------------------------------------------*/
/*! Number of RxAGC Reads to be performed */
#define RF_1X_RXAGC_RD_SAMPLES 4

/*----------------------------------------------------------------------------*/
/*!< Wait Interval between two consecutive RxAGC Read. Unit is Microseconds */
#define RF_1X_RXAGC_RD_WAIT 200

/*----------------------------------------------------------------------------*/
/*!< RxAGC settling time before sampling starts. Unit is Microseconds */
#define RF_1X_PRE_RXAGC_RD_WAIT 2000

/*----------------------------------------------------------------------------*/
#define RF_1X_DVGA_VS_FREQ_10TH_DB_RES 1 /*!< Check */

/*============================================================================*/
/*!
  @name Helper functions

  @brief
  Helper functions used by RF Calibration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sample Rx AGC

  @details
  This function measures specified number of Rx AGC sample and returns the 
  average value.

  @param device
  RF Device on which Rx AGC is sampled

  @param num_of_samples
  Number of Rx AGC samples to be taken.

  @return
  RxAGC value averaged over num_of_samples. The units is 1/640th of dB.
*/
static int32
rf_1x_sample_rx_agc
(
  rfm_device_enum_type device,
  uint8 num_of_samples
)
{
  uint8 readx; /* Counter variable for FOR Loop */
  int16 agc_read_val = 0; /* AGC Read Value */
  int32 agc_acc_val = 0; /* AGC Accumulated value */
  rf_1x_mdsp_rx_agc_read_type rx_agc_data; /* Rx AGC Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
    
    /* Take AGC_READ_SAMPLE_NUM samples of AGC read and average it out  */
    for ( readx = 0; readx < num_of_samples; readx++ )
    {
  
      /* Wait for RF_1X_RXAGC_RD_WAIT mS before taking another read */
      DALSYS_BusyWait( RF_1X_RXAGC_RD_WAIT );
  
      /* Read 10 Bit AGC value */
      rf_1x_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_data );
  
      agc_read_val = rx_agc_data.rx_agc;

      /* Perform Sign Bit Extension for Negative AGC value */
      /* 512 - 1023 correspond to Negative AGC Read. Thus need to sign extend. */
     // if ( agc_read_val > 32767 )
     // {
     //   agc_read_val = agc_read_val - 65536;
     // }
      
      RF_MSG_3( RF_LOW, "RxAGC Sampling - Dev %d, AGC %d for (Itr %d)",
                device, agc_read_val, readx );
      
      /* Accumulate all AGC Samples */
      agc_acc_val += agc_read_val;
      
    }
  
    /* Average Out the Accumulated AGC Read value */
    if( agc_acc_val > 0 ) /* For proper rounding */
    {
      agc_acc_val = ( agc_acc_val + num_of_samples/2 ) / num_of_samples;
    }
    else
    {
      agc_acc_val = ( agc_acc_val - num_of_samples/2 ) / num_of_samples;
    }
  }
  else
  {
    agc_acc_val = 0xFFFF; /* Invalid AGC */
    RF_MSG( RF_ERROR, "rf_1x_sample_rx_agc : Invalid Device Param Ptr" );
  }

  return agc_acc_val;

} /* rf_1x_sample_rx_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute Rx gain vs. freq cal NV

  @details
  This function converts full representation RF NV gain calibration vs. 
  frequency values into NV compatible base + offset vs. frequency data. The
  integer pointed to by base is updated with the average of the min and
  max calibration values across the band and power modes.  The integer array
  pointed to by freq is updated with the corresponding calibration value
  minus the base.

  @param cdma_cfg_ptr
  CDMA configuration data (NV) pointer for the path and band that are
  being computed.

  @param gain_state
  The gain state (G0..G4) to compute.

  @param channel_count
  Number of Calibration Channels

  @param power_mode_count
  Number of Power modes

  @param *base
  Base Offset

  @param freq
  Variable to store offset_vs_freq
*/
static void
rf_1x_compute_gain_freq_nv
(
  rf_cdma_config_type* cdma_cfg_ptr,
  rfm_lna_gain_state_type gain_state,
  const uint32 channel_count,
  const uint8 power_mode_count,
  int16 *base,
  int8 freq[][NV_FREQ_TABLE_SIZ]
)
{
  uint32 i;
  int16 min = 32767;
  int16 max = -32768;
  uint8 j; 
  int16 local_base = 0;

  if ( gain_state >= RFM_LNA_GAIN_STATE_NUM )
  {
    ERR_FATAL( "Invalid gain state %d", gain_state, 0, 0 );
  }
  if ( channel_count > NV_FREQ_TABLE_SIZ )
  {
    ERR_FATAL( "Channel count %d is bigger than NV Table size", channel_count, 0, 0 );
  }
  if ( power_mode_count > RFM_CDMA_POWER_MODE_MAX )
  {
    ERR_FATAL( "Power mode count %d is bigger than Max Number of Power Modes", power_mode_count, 0, 0 );
  }

  for (j = 0; j < power_mode_count; j++)
  {
    for (i = 0; i < channel_count; i++)
    {
      if ( cdma_cfg_ptr->rx_cal.rx_gain_calibration[j][gain_state][i] < min )
      {
        min = cdma_cfg_ptr->rx_cal.rx_gain_calibration[j][gain_state][i];
      }
      if ( cdma_cfg_ptr->rx_cal.rx_gain_calibration[j][gain_state][i] > max )
      {
        max = cdma_cfg_ptr->rx_cal.rx_gain_calibration[j][gain_state][i];
      }
    }
  }

  local_base = (min + max)/2;
  *base = local_base;

  for (j = 0; j < NV_RF_POWER_MODE_SIZ; j++)
  {
    for (i = 0; i < channel_count; i++)
    {
      freq[j][i] =
        (int8)rf_limit_s32( cdma_cfg_ptr->rx_cal.rx_gain_calibration[j][gain_state][i] - local_base,
                            -128, 127 );
    }
    for (i = i; i < NV_FREQ_TABLE_SIZ; i++)
    {
      freq[j][i] = 0;
    }
  }
} /* rf_cdma_compute_gain_freq_nv */

/*! @} */

/*============================================================================*/
/*!
  @name Low Noise Amplifier

  @brief
  Functions to perform Receiver Calibration

  @details
  These function can only be used during FTM mode for calibration purpose.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform LNA Measurement

  @details
  This function shall be used to compute proper LNA compensation value for 
  desired AGC value on a particular Device with given LNA Step (x). This API 
  shall perform one instance of calibration. Following are the assumptions
  used for the LNA calibration.

  Lower the LNA Gain number corresponds to higher Gain, viz G(x) > G(x+1), 
  where G refers to Gain. LNA step corresponds to the Step from one gain 
  state (x) to next consecutive gain state (x+1). 

  Following is formula used to compute LNA offset.

  [ LNA Offset G(n) = Expected_AGC_val - Current_Avg_AGC_val + offset for 
                                                                        G(n-1)],
  where "offset for G(n)" refers to the LNA offset corresponding to Gain state
  (n) which is computed in previous run.
  There are 2 ways of calculating the offset, either calculate it relative
  to the gain state 0, or calculate it relative to the previous gain state
  Before making measurement for the nth gain state we apply the offset for 
  previos gain state as well. The computed LNA Offset shall be stored in 
  rf_lna_offset_val[] for use in consecutive runs.

  This will calibrate the target with the current power mode without altering 
  it.

  @param device
  RF Device on which LNA calibration is done

  @param gain_state
  Corresponds to the LNA State

  @param agc_expected_val
  Expected AGC value. This corresponds to the actual AGC value that is fed to 
  phone antenna. This is in units of 1/640th dB.
 
  @return
  Calibrated LNA Offset Value in 1/640th dB units
*/
int32
rf_1x_measure_lna_offset
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state,
  int32 agc_expected_val
)
{
  int32 agc_acc_val = 0; /* AGC Accumulated value */
  int32 lna_gain = 0; /* LNA Offset Value */
  int32 lna_gain_prev_state; /* LNA Gain Value From previous State. */
  rf_cdma_data_calibration_type *cal_data_w; /* RF Cal Data write Pointer */
  const rf_cdma_data_calibration_type *cal_data_r; /*RF Cal Data read Pointer */
  rfm_cdma_power_mode_type power_mode; /* Current Power Mode */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device*/
  rf_cdma_data_status_type *dev_status_w; /* Device Status */

  /* Perform Sign Bit Extension for Negative AGC value */
  /* 512 - 1023 correspond to Negative AGC Read. Thus need to sign extend. */
  if ( agc_expected_val > 32767 )
  {
    agc_expected_val = agc_expected_val - 65536;
  }

  /* Get Read-only and writable RF Cal Data Pointer */
  cal_data_r = rf_cdma_get_cal_data(device);
  cal_data_w = rf_cdma_get_mutable_cal_data(device);
  
  /*NULL pointer check*/
  if( ( cal_data_r == NULL ) || ( cal_data_w == NULL ) )
  {
    RF_MSG_1( RF_ERROR,"rf_1x_measure_lna_offset: NULL data for device %d"
              ,device );
    lna_gain = 0xFFFF; /* Invalid LNA Gain */
  }/*if( ( cal_data_r == NULL ) || ( cal_data_w == NULL ) )*/
  else
  {
    /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( device );
    
    /* Get writeable Device Status */
    dev_status_w = rf_cdma_get_mutable_device_status( device );
     
    /* NULL pointer check*/
    if(  dev_status_w == NULL )
    {
        RF_MSG_1( RF_ERROR,"rf_1x_measure_lna_offset: NULL data for device %d"
                  ,device );
        lna_gain = 0xFFFF; /* Invalid LNA Gain */
    }/* if(  dev_status_w == NULL ) */
  
    else
    {
      /* Verify that conditions are valid */
      if (    gain_state < RFM_LNA_GAIN_STATE_NUM
           && logical_dev != NULL )
      {
        lm_handle_type rx_handle;
        /* get Current power mode */
        power_mode = rf_cdma_ic_get_current_power_mode( 
                                              &(dev_status_w->intelliceiver) );
      
        /* Populate lna_offset_prev value reading the global variable  */
        if ( gain_state == RFM_LNA_GAIN_STATE_0 )
        {
          /* Force previous step LNA offset to be used for step index 0 to be zero*/
          lna_gain_prev_state = 0;
        }
        else
        {
          /* Otherwise use the offset from LNA state zero. */
          lna_gain_prev_state = 
                      cal_data_r->lna_offset[power_mode][gain_state-1];
        }
       
        /* Set to desired LNA Gain State G(x+1) */
        rf_1x_mdsp_lna_state_override( logical_dev->rx_mdsp_path,
                                       dev_status_w->rxlm_handle,
                                       gain_state, NULL, NULL, NULL );
      
        /* get rx handle for the device */
        rx_handle = rf_cdma_data_get_rxlm_handle(device);

        /* Set zero LNA offset for corresponding to LNA Gain State G(x) */
        (void)rf_1x_mdsp_rx_gain_override( rx_handle,
                                           logical_dev->rx_mdsp_path,
                                           gain_state, lna_gain_prev_state, 
                                           NULL, NULL, NULL );

        /* If flag to do immediate LNA writes during cal is set to TRUE
        query WTR driver for settings and write to the WTR LNA gain state 
        register immediately, If this is done, there should be no LNA state
        update scheduled on RFLM side */ 
        if ( rf_cdma_debug_flags.imm_lna_write_cal == TRUE )
        {
          /* Call API for immediate LNA state write */
          rf_cdma_mc_immediate_lna_write(device, gain_state);

        }/* if ( rf_cdma_debug_flags.imm_lna_write_cal == TRUE ) */

        /* Wait for RF_1X_PRE_RXAGC_RD_WAIT mS before starting RxAGC read */
        DALSYS_BusyWait( RF_1X_PRE_RXAGC_RD_WAIT );
    
        /* Take RF_1X_RXAGC_RD_SAMPLES samples of AGC read and average it out  */
        agc_acc_val = rf_1x_sample_rx_agc( device, RF_1X_RXAGC_RD_SAMPLES );
      
        /* LNA offset is the difference between the actual AGC value and
        expected AGC value with addition of LNA offset for previous LNA 
        gain state. LNA offset for previous gain state is applied on 
        RFLM side before making the measurement, so all computaitons are 
        relative to the previous LNA gain state */ 
        lna_gain = agc_acc_val - agc_expected_val + lna_gain_prev_state;

        /* Store LNA offset in rf_lna_offset_val[] for future GET LNA OFFSET cmds */
        cal_data_w->lna_offset[ power_mode ][ gain_state ] = lna_gain;
        
        RF_MSG_5( RF_MED,
                  "LNA Measure: lna%d Gain = (act - exp + prev) = (%d-%d+%d)=%d",
                  gain_state, agc_acc_val, agc_expected_val,
                  lna_gain_prev_state, 
                  lna_gain );
      }
      else
      {
        lna_gain = 0xFFFF; /* Invalid LNA Gain */
        RF_MSG( RF_ERROR, "rf_1x_measure_lna_offset : Invalid inputs" );
      }   
    }/* if !(  dev_status_w == NULL ) */
  }/*if !( ( cal_data_r == NULL ) || ( cal_data_w == NULL ) )*/
    /* Return the Current LNA offset value */
 
  return lna_gain;

} /* rf_1x_measure_lna_offset */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform LNA Calibration and Store on RFNV Structure

  @details
  This function performs LNA calibration on all available power modes. If the
  intelliceiver is not enabled for the system, then LNA calibration will be
  done for Normal Power Mode only. The calibrated values are stores in NV
  table.

  @param device
  RF Device on which LNA calibration is to be done

  @param power
  Expected RxAGC Power to perform LNA calibration. This is in units of
  1/640th dB.

  @param gain_state
  LNA State for LNA Calibration

  @param *cal_data
  Data Structure to store the calibrate values. This structure will hold the
  number of RF Power Modes on which LNA calibration was done. Along with this,
  it will also hold the LNA Calibration Offset value measured for each
  RF Power Mode

  @param length
  Size of the Expected Data

  @return
  Status of LNA Calibration. TRUE for success and FALSE for failure
*/
boolean
rf_1x_calibrate_lna
(
  rfm_device_enum_type device,
  int16 power,
  rfm_lna_gain_state_type gain_state,
  rfm_1x_lna_cal_data_type *cal_data,
  int32 length
)
{
  uint32 chan_idx = 0; /* Channel Index */
  rfm_1x_lna_cal_data_type *data_ptr; /* LNA Calibration Data */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  rfm_cdma_power_mode_type power_mode; /* RF Power Mode for LOOP */
  rfm_cdma_power_mode_type power_mode_max; /* Max RF Power Mode for the system */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = TRUE; /* Return Value */
 
  /* Return FALSE if the length passed smaller than the size of the local data
  container for LNA Calibration */
  if ( sizeof(rfm_1x_lna_cal_data_type) > length )
  {
    return FALSE;
  }

  /* This function should be called only in calibration mode */
  if ( rfm_get_calibration_state() == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_calibrate_lna : Dev %d - Not in CAL mode",
              device );
    return FALSE;
  } /* if ( rfm_get_calibration_state() == FALSE ) */

  /* Pointing LNA Cal Data to data packet in argument */
  data_ptr = (rfm_1x_lna_cal_data_type*) cal_data;

  /* Get Current Device Status for the given Device and check whether the data 
  returned is valid. */
  dev_status_r = rf_cdma_get_device_status( device );
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Perform NULL Pointer check */
  if ( ( dev_status_r == NULL ) || ( dev_status_w == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_calibrate_lna: NULL data for device %d",
              device );
    return FALSE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
    rfm_cdma_band_class_type band; /* Band  */
    rfm_cdma_chan_type chan; /* Channel */

    /* Populate Band and Chan */
    band = dev_status_r->curr_band;
    if (band >= RFM_CDMA_MAX_BAND)
    {
      RF_MSG_1 (RF_ERROR,"rf_1x_calibrate_lna: Invalid band %d", band);
      return FALSE;
    }
    chan = dev_status_r->curr_chans[0];
  
    /* Add calibration channel to the internal RF NV data structures */
    chan_idx = rf_cdma_nv_add_cal_chan( logical_dev->rx_nv_path[band], band, 
                                        chan );
  
    /* Determine the Max Number of Power Modes */
    power_mode_max = rf_cdma_ic_get_lowest_power_mode( 
                                            &(dev_status_w->intelliceiver) );
  
    if( power_mode_max >= RFM_CDMA_POWER_MODE_MAX )
    {
      RF_MSG_1( RF_ERROR, "rf_1x_calibrate_lna: Out-of-Bounds p"
                          "ower_mode_max - %d", power_mode_max );
      return FALSE;
    }

    /* Store Number of Power Modes in Cal Data Packet */
    data_ptr->power_mode_num = power_mode_max + 1;
  
    /* Channel Index should be less NV_FREQ_TABLE_SIZ */
    if ( chan_idx >= NV_FREQ_TABLE_SIZ )
    {
      RF_MSG_1( RF_ERROR, "LNA Calibration: Out-of-Bounds Element Access - %d",
                chan_idx );
  
      /* Return Error */
      return FALSE;
    }
  
    /* Loop through all RF Power Modes available */
    for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
          power_mode <= power_mode_max; power_mode++ )
    {
  
      /* Set the Power Mode if the flag is TRUE */
      if ( dev_status_w->intelliceiver.ic_supported )
      {
        /* Since we reach this point, we are in calibraiton mode. No
        iC will be running in calibraiton mode. Therefore, there is no
        need to PAUSE and RESUME intelliceiver during Power Mode
        override. */
        ret_val &= rf_cdma_ic_power_mode_override( 
                       &(dev_status_w->intelliceiver), logical_dev, 1,
                       dev_status_r->curr_band,
                       &(dev_status_r->curr_chans[RFM_CDMA_CARRIER_0]),
                       dev_status_r->data_1x.rf_state, RF_CDMA_STATE_SLEEP,
                       power_mode );
      } /* if ( dev_status_w->intelliceiver.ic_supported ) */
  
      /* Perform LNA Calibation for the given Power Mode */
      data_ptr->calibration[power_mode] = 
        rf_1x_measure_lna_offset( device, gain_state, power );
  
      /* Add in digital gain compensation to the offset value.
      Compensation will be applied to all gain states as opposed to only G0 on 
      previous targets */
      data_ptr->calibration[power_mode] -=
        rf_cdma_get_calibration_compensation( device, band, chan );
  
      /* Store the calibrated Value in the NV Table for the
         corresponding gain state */
      rf_cdma_nv_update_gain_cal( logical_dev->rx_nv_path[band],
                                  band,
                                  chan, power_mode, gain_state,
                                  data_ptr->calibration[power_mode] );
  
      RF_MSG_4( RF_MED, "LNA Calibration: Chan=%d, PwrMode=%d, LNA_Offset=%d, "
                "Chan Idex=%d",
                chan, power_mode,
                data_ptr->calibration[power_mode], chan_idx );
    }
  } /* if ( logical_dev != NULL ) */
  else
  {
    ret_val &= FALSE;
    RF_MSG( RF_ERROR, "rf_1x_calibrate_lna : Invalid Device Param Ptr" );
  }

  /* Return Success */
  return ret_val;

} /* rf_1x_calibrate_lna */

/*! @} */

/*============================================================================*/
/*!
  @name Intelliceiver

  @brief
  Functions used to perform intelliceiver calibration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Perform intelliceiver calibration
 
  @details
  This function will call device drive to perform intelliceive calibration for
  all power modes. Device driver will search for converged JD code. While
  performing the calibration, intelliceiver calibration is overriden, LNA state
  is overridden to Highest Gain state. Returned JD code will be adjusted with 
  the Cell IOR at which the calibration is performed.
 
  Calibration will be considered as PASS, if the returned JD code for
  all power modes are be converged. If not, error code will be sent back 
  indicating calibration failure.
 
  @param device
  RF Device for which intelliceiver calibraiton is to be done
 
  @param logical_dev
  Pointer to logical device parameter mapping for the given device
 
  @param ic_data
  Pointer to Intelliceiver Data to be used for calibration
 
  @param band
  RF Band at which intelliceiver calibration is done
 
  @param channel
  RF Channel at which intelliceiber is done
 
  @param power
  Cell IOR set during RF intelliceiver calibration
 
  @param cal_data
  Container where the calibration data is stored.
 
  @return
  Flag indicating if the calibration is successful.
 
  @retval TRUE
  Indicates that the calibration is successful and the JD Code is a converged
  value.
 
  @retval FALSE
  Indicates that JD Cide returned is not converged; thus the calibration
  failure is seen
*/
boolean
rf_1x_calibrate_intelliceiver
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_ic_data_type *ic_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type channel,
  rf_cdma_state_type curr_rf_state,
  int16 power,
  rfm_1x_ic_calibration_data_type *cal_data
)
{
  rfm_cdma_power_mode_type power_mode; /* Power mode index */
  rfdevice_cdma_ic_cal_data_type ic_cal; /* Container to hold returned CalData*/
  boolean ret_val = TRUE; /* Return value */
  rf_cdma_data_status_type *dev_status_w; /* Device Status */

  /* Get writeable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if ( dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_calibrate_intelliceiver: Null pointer for "
                        "device !", device );
    return FALSE;
  }

  /* This function should be called only in calibration mode */
  if ( rfm_get_calibration_state() == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_calibrate_intelliceiver : Dev %d - "
                        "Not in CAL mode", device );
    return FALSE;
  } /* if ( rfm_get_calibration_state() == FALSE ) */

  /* Override the Intelliceiver Power Mode */
  if ( ic_data->ic_supported != TRUE )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_calibrate_intelliceiver: No Support on Dev %d",
              device );
    return FALSE;
  }
  
  /* Make sure that RF is in calibration Mode */
  if ( rfm_get_calibration_state() == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_calibrate_intelliceiver: Cal Mode Required "
                        "on Dev %d", device );
    return FALSE;
  }

  /* Perform Calibration on all power Modes. */
  for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
        power_mode <= rf_cdma_ic_get_lowest_power_mode(ic_data);
        power_mode++ )
  {

    /* Override the Intelliceiver Power Mode */
    /* Since we reach this point, we are in calibraiton mode. No
    iC will be running in calibraiton mode. Therefore, there is no
    need to PAUSE and RESUME intelliceiver during Power Mode
    override. */
    ret_val &= rf_cdma_ic_power_mode_override( ic_data, logical_dev, 1,
                                               band, &channel, 
                                               curr_rf_state, RF_CDMA_STATE_SLEEP,
                                               power_mode);

    /* Override LNA state to G0 */
    rf_1x_mdsp_lna_state_override( logical_dev->rx_mdsp_path,
                                   dev_status_w->rxlm_handle,
                                   RFM_LNA_GAIN_STATE_0, NULL, NULL, NULL );

    /* Call Device Driver to perform intelliceiver calibration. This will
    return a JD Code along with thatm it will send two flags indicating
    if the JD Code search converged or not. */
    ic_cal = rfdevice_cdma_ic_calibration( logical_dev->rx_device[band], power );

    /* Populate the return container with the Calibration Data obtained from
    RTR Driver */
    cal_data->midpoint_power[power_mode][0] = ic_cal.cal_data;
    cal_data->midpoint_power[power_mode][1] = 
                             cal_data->midpoint_power[power_mode][0];

    /* Check if the JD code search converged or not. If the search is not
    converged, then return Error return value. */
    if ( ic_cal.cal_status == FALSE )
    {
       RF_MSG( RF_ERROR, "rf_1x_calibrate_intelliceiver: Failed" );
       ret_val &= FALSE;

       /* If a failure is seen, break the FOR loop, no need to proceed */
       break;
    }
  }

  /* Populate Channel */
  cal_data->channel = channel;

  /* Return */
  return ret_val;

} /* rf_1x_calibrate_intelliceiver */

/*! @} */

/*============================================================================*/
/*!
  @name NV Push

  @brief
  Functions related to RF Calibration NV write 
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Write all RF calibration

  @details
  This function will write all RF calibration items for a particular band to NV.

  @param band
  RF Band for which NV write is done.

  @return
  NV Write Status. If all items are written sucessfully, TRUE is returned,
  otherwise the return code of the first write error is returned.
*/
boolean
rf_1x_write_nv
(
  rfm_cdma_band_class_type band
)
{
  rf_cdma_config_type* cdma_cfg_ptr; /* Pointer to common CDMA config data */
  rf_cdma_cal_nv_items_map_type *cal_nv_tbl = NULL; /* Pointer to Cal NV */
  rf_cdma_nv_path_type nv_path; /* Variable to store NV Path */
  boolean write_status = FALSE; /* NV Write Status */
  int16 base_offset; /* Base Offset */
  int8 offset_vs_freq[NV_RF_POWER_MODE_SIZ][NV_FREQ_TABLE_SIZ];

  /* Loop Through all Paths */
  for ( nv_path = RF_CDMA_NV_PATH_0; nv_path < RF_CDMA_NV_PATH_NUM; nv_path++ )
  {
    uint32 channel_count; /* Counter for Channel */
    uint8 power_mode_count; /* Counter for Power modes */
    const rf_1x_config_type* rf_1x_config_ptr;

    /* Get Calibration NV Table */
    cal_nv_tbl = rf_cdma_nv_get_calibration_table( band, nv_path );
    if ( cal_nv_tbl == NULL )
    {
      RF_MSG_2( RF_ERROR,
                "rf_1x_write_nv: Cal NV data structure error for band %d, path %d",
                band, nv_path );
      return FALSE;
    }

    rf_1x_config_ptr = rf_cdma_nv_get_1x_data( nv_path, band );

    if ( rf_1x_config_ptr == NULL )
    {
      RF_MSG_2( RF_HIGH,
                "rf_1x_write_nv: Skipping band %d, path %d due to inactive config structure",
                band, nv_path );
      continue;
    }

    cdma_cfg_ptr = rf_1x_config_ptr->cdma_common_ptr;

    if ( cdma_cfg_ptr == NULL )
    {
      RF_MSG_2( RF_ERROR,
                "rf_1x_write_nv: Config data structure error for band %d, path %d",
                band, nv_path );
      return FALSE;
    }

    /* Write Calibraiton Channel List */
    write_status = rf_cdma_nv_write( cal_nv_tbl->rx_cal_chan_list,
                                     cdma_cfg_ptr->rx_cal.channel_list );
    if (write_status != TRUE)
    {
      return write_status;
    }

    /* Write Least Recently Used Channel List */
    write_status = rf_cdma_nv_write( cal_nv_tbl->rx_cal_chan_lru_list,
                                     cdma_cfg_ptr->rx_cal.lru_channel_list );
    if (write_status != TRUE)
    {
      return write_status;
    }
    
    /* Compute the num of channels to consider when calculating base/offset */
    channel_count = 0;

    while (channel_count < NV_FREQ_TABLE_SIZ &&
           cdma_cfg_ptr->rx_cal.channel_list[channel_count] != 0)
    {
      channel_count++;
    }

    /* Count number of Power modes */
    //power_mode_count = rf_cdma_ic_get_lowest_power_mode(nv_path)+1;
    power_mode_count = RFM_CDMA_POWER_MODE_NORMAL + 1;

    if ( channel_count > 0 )
    {
      /* Compute G0 Base and Offset */ 
      rf_1x_compute_gain_freq_nv( cdma_cfg_ptr,
                                  RFM_LNA_GAIN_STATE_0,
                                  channel_count,
                                  power_mode_count,
                                  &base_offset, offset_vs_freq );
      /* Write DVGA Cal Data */
      write_status = rf_cdma_nv_write(cal_nv_tbl->dvga,
                                      &base_offset);
      if (write_status != TRUE)
      {
        return write_status;
      }

      /* Write DVGA Cal Data */
      write_status = rf_cdma_nv_write(cal_nv_tbl->dvga_vs_freq,
                                      offset_vs_freq);
      if (write_status != TRUE)
      {
        return write_status;
      }

      /* Compute LNA Base and Offset */ 
      rf_1x_compute_gain_freq_nv( cdma_cfg_ptr,
                                  RFM_LNA_GAIN_STATE_1,
                                  channel_count,
                                  power_mode_count,
                                  &base_offset, offset_vs_freq );

      /* Write LNA Cal Data */
      write_status = rf_cdma_nv_write(cal_nv_tbl->lna_1,
                                      &base_offset);
      if (write_status != TRUE)
      {
        return write_status;
      }

      /* Write LNA Cal Data */
      write_status = rf_cdma_nv_write(cal_nv_tbl->lna_1_vs_freq,
                                      offset_vs_freq);
      if (write_status != TRUE)
      {
        return write_status;
      }

      /* Compute LNA Base and Offset */
      rf_1x_compute_gain_freq_nv( cdma_cfg_ptr,
                                  RFM_LNA_GAIN_STATE_2,
                                  channel_count,
                                  power_mode_count,
                                  &base_offset, offset_vs_freq );

      /* Write LNA Cal Data */
      write_status = rf_cdma_nv_write(cal_nv_tbl->lna_2,
                                      &base_offset);
      if (write_status != TRUE)
      {
        return write_status;
      }

      /* Write LNA Cal Data */
      write_status = rf_cdma_nv_write(cal_nv_tbl->lna_2_vs_freq,
                                      offset_vs_freq);
      if (write_status != TRUE)
      {
        return write_status;
      }

      /* Compute LNA Base and Offset */
      rf_1x_compute_gain_freq_nv( cdma_cfg_ptr,
                                  RFM_LNA_GAIN_STATE_3,
                                  channel_count,
                                  power_mode_count,
                                  &base_offset, offset_vs_freq );

      write_status = rf_cdma_nv_write(cal_nv_tbl->lna_3,
                                      &base_offset);
      if (write_status != TRUE)
      {
        return write_status;
      }

      /* Write LNA Cal Data */
      write_status = rf_cdma_nv_write(cal_nv_tbl->lna_3_vs_freq,
                                      offset_vs_freq);
      if (write_status != TRUE)
      {
        return write_status;
      }

      /* Compute LNA Base and Offset */
      rf_1x_compute_gain_freq_nv( cdma_cfg_ptr,
                                  RFM_LNA_GAIN_STATE_4,
                                  channel_count,
                                  power_mode_count,
                                  &base_offset, offset_vs_freq );

      /* Write LNA Cal Data */
      write_status = rf_cdma_nv_write(cal_nv_tbl->lna_4,
                                      &base_offset);
      if (write_status != TRUE)
      {
        return write_status;
      }

      /* Write LNA Cal Data */
      write_status = rf_cdma_nv_write(cal_nv_tbl->lna_4_vs_freq,
                                      offset_vs_freq);
      if (write_status != TRUE)
      {
        return write_status;
      }
    } /* if ( channel_count > 0 ) */
  } /* for ( nv_path = RF_CDMA_NV_PATH_0; nv_path < RF_CDMA_NV_PATH_NUM; nv_path++ )*/

  return TRUE;
} /* rf_1x_write_nv */
#endif /* FEATURE_CDMA1X */

/*! @} */

/*! @} */
