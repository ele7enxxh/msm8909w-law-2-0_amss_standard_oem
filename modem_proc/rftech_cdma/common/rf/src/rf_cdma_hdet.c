/*! 
  @file
  rf_cdma_hdet.c
 
  @brief
  This file defines the common CDMA (1x and HDR) interfaces needed for HDET 
  operation

  @image html rf_cdma_hdet_state_machine.jpg
 
  @addtogroup RF_CDMA_COMMON_HDET
  @{
*/

/*==============================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_hdet.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/20/15   vr      Autopin check-in
04/16/15   vr/cd   HDET updates for power capping
11/24/14   zhw     Fix incorrect band check during HDET temp param update
11/20/14   spa     Make sure HDET data has valid band before updating temp param
11/12/14   spa     Change tx_nv_path to be per band per logical device
08/01/14   zhw     Check HDET state during HDET temp param update
05/28/14   zhw     FTM HDET support
05/13/14   spa     Check for NULL event handle before calling config devices
04/18/14   JJ      Early return in hdet_update_temperature_params api for bolt
04/14/14   zhw     Return TRUE for mdsp_get_hdet_data
04/07/14   zhw     FED based C2K HDET support
03/28/14   APU     Hook up Tx logging interfaces in SW driver to log data 
                   provided by RFLM.
09/23/13   zhw     Added Tuner CL corner case handling when FW times out
09/19/13   APU     MC changes to support DM.
09/09/13   shb     Fixed DRx non-sig call crash
08/26/13   shb     Removed legacy get_hdet_param() call from init(). Query from 
                   read_hdet() if needed.
08/23/13   pk      Fix for LPM HDET transition issue
08/01/13   zhw     Use #defined CDMA Tuner CL Time Out constant
07/10/13   zhw     Remove dev_status_w in mc layer call flow
03/07/13   pk      Added missing initialization in hdet init 
06/26/13   zhw     Tuner CL support. Remove HDET config msg.
06/21/13   zhw     Tuner CL support. Remove unused params
06/16/13   zhw     Turn off HDET debug flag
06/13/13   zhw     Tuner CL Return loss support using HDET mdsp polling
05/24/13   APU     Eliminated unwanted event information structures.
05/22/13   APU     Changed the HDET CCS event name to be more consistent with 
                   the rest if CDMA CCS events.
03/21/13   shb     Updated to supported gated(WTR1625) & non-gated(WTR1605) HDET
03/20/13   cd      XPT Temp comp support
03/01/13   zhw     Init hdet crit sect whether tx_device is present
02/25/13   zhw     Refactored CCS event cleanup API and corner case handling
02/21/13   Saul    CDMA. Added xpt_temp_comp_enabled debug flag.
02/15/13   spa     Remove code under featurization FEATURE_RF_HAS_QFE1510_HDET
02/11/13   spa     Fix for offtarget 
02/11/13   spa     Added call to HDET init in HDET start API
02/01/13   bmg     Moved all HDET calls to CDMA HDET object interface
01/12/13   cd      RxAGC LNA SM event configuration support
12/27/12   adk     Updated TDET code
12/20/12   sty     KW fix: deleted null pointer access
11/30/12   spa     Updated HDET for Dime
12/03/12   APU     Added a one time check for therm cal monotonicity. 
11/30/12   adk     Dime RFC updates to get hdet device instance
10/17/12   kai     Changed calling to QFE1510 HDET APIs
10/31/12   adk     Merged antenna tuner code from Nikel.
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/14/12   sty     Disable HDET for bringup
04/18/12   aro     Added callback function as parameter to override functions
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/14/12   shb     Removed HDET config message to fw in rf_cdma_hdet_start()
02/23/12   shb     Do not reset isHKADCbusy flag if rf_cdma_mdsp_polling_cb 
                   function timesout/aborts HDET operation.
02/21/12   cd      Moved LPM HDET NV message to med from error 
02/16/12   cd      Removed redundant NV valid check for LPM extract NV
02/11/12   shb     Enabled HDET F3 message for easy debug
02/11/12   shb     Check to avoid redundant HDET_reset messages to FW
02/02/12   cd      Added LPM HDET support 
                   - All functionality now supports two HDET power modes
                   - Update power mode based on max power limit and LPM
                     threshold
01/23/12   shb     Reset MDSP HDET state machine whenever RF SW stops HDET loop 
01/20/12   shb     Updated CDMA device interface to use generic 
                   rfdevice_ssbi_write_type and rfdevice_script_type instead of
                   CDMA specific structures
01/04/12   hdz     Use rf_cdma_obtain_hkadc_lock() to obtain HKADC lock 
12/22/11   shb     Simplified isHKADCbusy flag management in read_hdet_cb
12/22/11   shb     Always reset isHKADCbusy flag when HDET is stopped
12/22/11   shb     Query SSBI scripts from device one time in rf_cdma_hdet_start
11/15/11   aro     Added debug flag check to print F3 message
11/12/11   hdz     Klocwork fix
11/12/11   hdz     Fixed Klocwork error
11/12/11   hdz     Fixed klocwork error
11/10/11   hdz     Added control logic to HDET to co-exist with thermistor
10/18/11   cd      Replaced NV functionality to query data from multi lin NV
                   information
10/13/11   aro     Klocwork fix
10/13/11   aro     Added F3 messages for error cases
10/13/11   aro     Added interface to enable and disable various debug mode
09/21/11   aro     Added F3 messages
09/21/11   aro     Removed error condition for HDET read function when HDET is
                   enabled with no read
09/13/11   shb     Enabled online HDET operation and disabled F3s
09/07/11   shb     Added HDET state machine/tx agc units diagrams
09/07/11   shb     Added code to not run HDET for HDR if tx power is too low
09/07/11   shb     Reset state_count whenever HDET state changes
09/07/11   shb     Reset filtered, abs tx errors when HDET is re-started
09/07/11   shb     Enhanced F3 messages
08/31/11   shb     Added global flag to disable HDET operation - start/stop
                   calls will be ignored
08/30/11   shb     Added HDR power limiting module
08/29/11   shb     Handle redundant/repeat calls to rf_cdma_hdet_start()
08/29/11   shb     Handle null data in rf_cdma_hdet_mdsp_polling_cb
08/25/11   shb     Added mode/band arg in rf_cdma_read_hdet()
08/25/11   shb     Fixed compiler warning
08/25/11   shb     use exp hdet vs agc tbl start offset from NV - fixes 0.1dBm
                   err in tx agc estimates by hdet
08/25/11   shb     Updated code that converts hdet read to txagc to handle sign
                   in a more robust way
08/25/11   shb     Added API to read HDET: rf_cdma_read_hdet
08/25/11   shb     Fixed doxygen format issues
08/24/11   shb     Stop HDET loop if calibration mode is enabled
08/23/11   shb     Added NULL data checks in rf_cdma_hdet_start()
08/22/11   shb     Updates for new NV names
08/22/11   shb     Started using new cal tbl for Lim/HDET data
08/22/11   shb     Added helper function to get TX total power from MDSP
08/22/11   shb     Update HDET read cb func to not read HDET is TX power is too
                   low compared to min HDET value
08/19/11   shb     Updated bailout mechanism: firmware itself bails out with a 
                   read failure after 40ms
08/18/11   shb     Added device to hdet_data. Removed arg from start/stop APIs
08/18/11   shb     Update TX error from HDET to plim module
08/16/11   shb     Implemented rf_cdma_hdet_update_state()
08/16/11   shb     Added debugging_enabled flag to selectively print F3s
08/16/11   shb     Read MDSP read count everytime before sending a read request
08/15/11   shb     Renamed HDET operating "mode" to "state" to avoid confusion 
                   with 1x/DO mode.
08/12/11   shb     Fixed Klocwork warnings
08/11/11   shb     fixed ontarget compiler warning
08/11/11   shb     Updated rf_cdma_hdet_update_tx_err() to do HDET offset/span
                   checking
08/11/11   shb     Fixed macros that return filter err and polling period
08/11/11   shb     Updated rf_cdma_analyze_exp_hdet_vs_agc() to detect any 
                   railed value and not all zeros only
08/09/11   shb     Use existing MDSP TX min max power defns
08/08/11   shb     Updates to handle HDET reads outside expected hdet vs agc tbl
08/08/11   shb     Updated rf_cdma_hdet_filter_err to handle div by 0 and
                   rounding of decimal value
08/05/11   shb     Updated device interface to get HDET scripts for MDSP
08/05/11   shb     Added callback functions and timer hooks to read HDET 
                   periodically and poll MDSP SMEM
08/05/11   shb     Added hooks to MDSP interface for HDET config, trigger & read
08/05/11   shb     Added helper to filter TX error in integer arithmetic
08/03/11   shb     Fixed compiler warning due to unused local variable
08/03/11   shb     Implemented rf_cdma_hdet_init() and rf_cdma_hdet_start() APIs
08/03/11   shb     Query device dependent HDET params from device interface
08/03/11   shb     Added helper functions rf_cdma_convert_hdet_read_to_txagc(),
                   rf_cdma_analyze_exp_hdet_vs_agc(), 
                   rf_cdma_hdet_extract_nv_data()
07/26/11   aro     Updated HDET init to pass logical device
07/25/11   sty     Removed CS protection from rf_cdma_hdet_init - since CS data
                   is uninitialized at this point
07/25/11   sty     Added locks to APIs
06/25/11   sty     Initial version

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfcommon_msg.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_hdet.h"
#include "rfcommon_math.h" /* Interpolation functions */
#include "rf_cdma_utils_freq.h" /* Function to get TX carrier freq from chan */
#include "rf_cdma_nv.h" /* Get NV table */
#include "rf_1x_mdsp.h"
#include "rf_hdr_mdsp.h"
#include "rfm_internal.h" /* rfm_get_calibration_state() and debug flag */
#include "rf_1x_power_limiting.h"
#include "rf_hdr_power_limiting.h"
#include "rf_cdma_power_limiting.h"
#include "rf_cdma_mdsp.h" /* rf_cdma_mdsp_dbm10_to_txagc() */
#include "rf_cdma_data.h"
#include "rf_cdma_temp_comp.h"
#include "rf_cdma_constants.h"
#include "rfdevice_hdet_cdma_intf.h"
#include "rfdevice_hdet_cmn_intf.h"
#include "rf_cdma_mc.h"
#include "rflm_c2k_mc.h"
#include "ftm.h"

/*!
  @brief
  Gets HDET read period based on current state: acquisition or tracking
*/
#define HDET_READ_PERIOD( hdet_data ) \
  (hdet_data->state == RF_CDMA_HDET_ACQUISITION) ? \
  hdet_data->device_param.acq_read_period : \
  hdet_data->device_param.trk_read_period

/*!
  @brief
  Gets HDET error filter coefficient based on current state: acquisition or 
  tracking
*/
#define HDET_ERR_FILTER_COEFF( hdet_data ) \
  (hdet_data->state == RF_CDMA_HDET_ACQUISITION) ? \
  hdet_data->device_param.acq_err_filter_coeff : \
  hdet_data->device_param.trk_err_filter_coeff

/*============================================================================*/
/*!
  @name MDSP Interface

  @brief
  Functions used for MDSP interaction by HDET.

  @details
  These wrapper functions are created so that there is no 1x/hdr specific code
  in HDET functions. These functions take the mode as an arg and call the
  appropriate 1x/hdr function from MDSP
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Gets HDET read data from MDSP

  @details
  Technology independent wrapper that calls the appropriate MDSP API to get
  HDET read data for 1x or HDR

  @param chain
  MDSP chain to read SMEM from

  @param mode
  Technology or RFM mode (1x or HDR). Decides which MDSP interface API is 
  called by this generic API

  @param *mdsp_hdet_read_data
  Pointer to struct where data read from MDSP will be stored
  
  @retval TRUE
  Data query from MDSP was successful
  
  @retval FALSE
  Data query from MDSP failed
*/
boolean
rf_cdma_mdsp_get_hdet_read_data
(
  rfm_device_enum_type device,
  rfm_mode_enum_type mode,
  rf_cdma_mdsp_hdet_read_data_type* mdsp_hdet_read_data
)
{
  rflm_c2k_hdet_data_t* rflm_hdet_data;
  int16 current_tx_agc;

  if ( rf_cdma_data_get_rf_mode(device) == RFM_1X_MODE )
  {
    rflm_hdet_data = &(rflm_c2k_get_data()->onex_data.hdet_data);   
  }
  else
  {
    rflm_hdet_data = &(rflm_c2k_get_data()->hdr_data.hdet_data);   
  }

  current_tx_agc = rf_cdma_mdsp_dbm10_to_txagc(
			rflm_hdet_data->hdet_tx_power,
	                rf_cdma_get_turnaround_constant(rf_cdma_data_get_curr_band(device))*10 );

  mdsp_hdet_read_data->read_count = rflm_hdet_data->num_hdet_serviced;
  mdsp_hdet_read_data->read_success = rflm_hdet_data->hdet_success;
  mdsp_hdet_read_data->txagc_at_read = current_tx_agc;

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query current total tx power from MDSP for given mode
  
  @details
  Gets TX AGC parameters from 1x/HDR MDSP and returns Tx Total power from them
  
  @param chain
  MDSP chain to read SMEM from

  @param mode
  Technology or RFM mode (1x or HDR). Decides which MDSP interface API is 
  called by this generic API

  @return
  Current TX total power in raw FW TX AGC units (dB/640)
*/
int16
rf_cdma_mdsp_get_tx_total_power
(
  rf_cdma_mdsp_chain_enum_type chain,
  uint32 rflm_handle,
  rfm_mode_enum_type mode
)
{
  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_data;

  /* Set default total power to min MDSP tx agc value i.e. max possible power
  At this power, HDET reads would be done which should be the default 
  behavior */
  tx_agc_data.total_power = RF_CDMA_MDSP_MIN_TX_PWR;

  if ( mode == RFM_1X_MODE )
  {
    rf_1x_mdsp_get_tx_agc_logical_data( chain,
                                        rflm_handle,
                                        &tx_agc_data );
  }
  else if ( mode == RFM_1XEVDO_MODE )
  {
    /* Need new FW interface from firmware for HDR tx total. Carrier is a dont 
    care as we need total power not per carrier. */
    rf_hdr_mdsp_get_tx_agc_logical_data( chain,
                                         rflm_handle,
                                         RFM_CDMA_CARRIER_0,
                                         &tx_agc_data );
  }
  return tx_agc_data.total_power;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Triggers MDSP to schedule HDET read for given mode

  @details
  Technology independent wrapper that calls 1x or HDR MDSP interface API to
  trigger HDET read

  @param mode
  Technology or RFM mode (1x or HDR). Decides which MDSP interface API is 
  called by this generic API
*/
void
rf_cdma_mdsp_trigger_hdet_read
(
  rf_cdma_hdet_data_type* hdet_data,
  rfm_mode_enum_type mode
)
{
  rflm_c2k_hdet_data_t* hdet_event;

  boolean hdet_in_ftm_mode;

  if ( mode == RFM_1X_MODE )
  {
    hdet_event = &(rflm_c2k_get_data()->onex_data.hdet_data); 
  }
  else
  {
    hdet_event = &(rflm_c2k_get_data()->hdr_data.hdet_data);
  }    

  if ( ftm_get_mode() == FTM_MODE )
  {
    hdet_in_ftm_mode = TRUE;
  }
  else
  {
    hdet_in_ftm_mode = FALSE;
  }

  /* Set HDET FTM mode flag */
  hdet_event->hdet_in_ftm_mode = hdet_in_ftm_mode;

  /* Set HDET pending to TRUE to signal pending HDET event */
  hdet_event->hdet_pending = TRUE;
}
/*! @} */
/*============================================================================*/
/*!
  @name Helper Functions

  @brief
  Helper functions used for miscellaneous functionality needed by HDET 
  algorithm
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to validate Expected HDET vs AGC table

  @details
  Checks expected HDET vs AGC table for monotonicity and validity 
  (not all zeros).

  Initialize *exp_hdet_vs_agc ptr in hdet_data to point to the 
  *exp_hdet_vs_agc_tbl passed in. 

  Analyzes table to find low_zone and high_zone and store in hdet_data. Expected
  HDET vs AGC curve contains HDET reading for the current band, reference 
  channel at 1.6dB intervals in the top quarter of the TX AGC range. HDET 
  readings must increase with TX power monotonically for proper operation and 
  TX AGC estimation. Hence, the curve is monotonic. It is flat at the base as
  the HDET readings rail low and and at the top as they rail at the max value.

  We precompute the range of these flat areas on the curve to avoid searching
  thru them for HDET readings. The highest bin index with railed low value is
  called exp_hdet_vs_agc_low_zone and lowest bin index with railed high value
  is called exp_hdet_vs_agc_high_zone. Note that zone index is the first value
  in the bin irrespective of low or high bin.

  @param *hdet_data
  Pointer to HDET data of current device

  @param *exp_hdet_vs_agc_tbl
  Pointer to Expected HDET vs AGC table from NV for current band/device

  @param size
  Size of Expected HDET vs AGC table

  @param power_mode
  Current HDET power mode

  @retval TRUE
  Expected HDET vs AGC curve is valid. Low zone and high zone have been 
  computed successfully and stored in HDET data

  @retval FALSE
  Expected HDET vs AGC curve is invalid - either all zeros or non-monotonic.
  Or an invalid (NULL or zero size) table has been passed in.
*/
boolean
rf_cdma_analyze_exp_hdet_vs_agc
(
  rf_cdma_hdet_data_type* hdet_data,
  const uint16* exp_hdet_vs_agc_tbl,
  uint8 size,
  rfdevice_cdma_hdet_power_mode_type power_mode
)
{
  boolean high_zone_found = FALSE;
  uint8 index;

  if ( exp_hdet_vs_agc_tbl == NULL )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_analyze_exp_hdet_vs_agc: "
                        "Dev %d, HDET PM %d - ExpHDET vs "
              "AGC Table is invalid", 
              hdet_data->device, power_mode);
    return FALSE;
  }

  if ( size == 0 )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_analyze_exp_hdet_vs_agc: "
                        "Dev %d, HDET PM %d - Size 0",
              hdet_data->device, power_mode);
    return FALSE;
  }

  /* Store pointer to first element of array in HDET data structure */
  hdet_data->exp_hdet_vs_agc[power_mode] = exp_hdet_vs_agc_tbl;

  /* Initialize low_zone to first element of exp_hdet_vs_agc tbl */
  hdet_data->exp_hdet_vs_agc_low_zone[power_mode] = 0;

  /* Initialize high_zone to last element of exp_hdet_vs_agc tbl */
  hdet_data->exp_hdet_vs_agc_high_zone[power_mode] = size - 1;

  /* Check Exp HDET vs AGC for monotonicity. Loop thru table to find any index
  whose value is strictly less than previous index. Also look to calculate
  the low_zone (highest range with value same as first entry) and
  high_zone (lowest range with value same as last entry) while we are looping
  thru the table */
  for ( index = 1; index < size; index++ )
  {
    if ( exp_hdet_vs_agc_tbl[index] < exp_hdet_vs_agc_tbl[index-1] )
    {
      /* Found an index where exp hdet vs agc falls. Break! This is not a 
      valid table */
      break;
    }
    else 
    {
      /* Valid table entries so far. 
      Check if index lies in low_zone or high_zone */
      if ( exp_hdet_vs_agc_tbl[index] == exp_hdet_vs_agc_tbl[0] )
      {
        hdet_data->exp_hdet_vs_agc_low_zone[power_mode] = index;
      }
      else if ( (high_zone_found == FALSE) &&
                (exp_hdet_vs_agc_tbl[index] == exp_hdet_vs_agc_tbl[size-1]) )
      {
        hdet_data->exp_hdet_vs_agc_high_zone[power_mode] = index - 1;
        /* Set high_zone_found flag so that no more checks for high_zone are 
        done */
        high_zone_found = TRUE;
      }
    }
  }
  if ( (index < size) || /* Implies table is NOT monotonic */
       (exp_hdet_vs_agc_tbl[size-1] == exp_hdet_vs_agc_tbl[0]) 
       /* Last element is same as first element and array
       is monotonic. Implies all elements are same, cannot determine tx agc 
       from HDET reading in such a case */ 
     )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_analyze_exp_hdet_vs_agc: "
                        "Dev %d, HDET PM %d - Table not "
              "monotonic", hdet_data->device, power_mode);
    return FALSE;
  }

  /* If code flow has reached here, Exp HDET vs AGC table is monotonic and 
  non-zero. Assume it is valid if it meets these criteria. */

  return TRUE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Converts 16bit HDET reading to TX AGC in raw firmware units (signed db/640)

  @details
  Looks up the hdet_read in Expected HDET vs TX AGC table and computes TX AGC
  by interpolating linearly within the bin where the hdet reading falls. Lim
  vs freq compensation is applied from hdet_data.

  This function operators on signed db/640 TX AGC units. Hence TX AGC increases
  with decreasing power.

  @image html rf_cdma_hdet_tx_agc_units.jpg

  Note that this function relies on rf_cdma_hdet_start() setting up some 
  initial data and cannot be called by itself to convert any arbitrary HDET
  read to TX AGC. This is meant to be an internal helper function for the HDET
  loop only.
  
  @param *hdet_data
  Pointer to hdet_data of current device. Must be updated expected hdet vs agc
  table pointer, its low/high zones and interpolated lim_vs_freq for current 
  frequency prior to using this function for HDET read to TX AGC conversion

  @param hdet_read
  16-bit unsigned HDET read from the device.

  @param power_mode
  Current HDET power mode

  @return
  16bit signed tx agc value in raw firmware units of db/640 for given hdet_read
*/
int16
rf_cdma_convert_hdet_read_to_txagc
(
  rf_cdma_hdet_data_type* hdet_data,
  uint16 hdet_read,
  rfdevice_cdma_hdet_power_mode_type power_mode
)
{
  uint8 zone, i;
  uint8 low_zone = hdet_data->exp_hdet_vs_agc_low_zone[power_mode];
  uint8 high_zone = hdet_data->exp_hdet_vs_agc_high_zone[power_mode];
  uint16 delta, rise;
  int32 txagc_x640_signed;
  int16 txagc_x640_signed_limited, bin_offset;

  /* Check if HDET reading is railed below the curve */
  if( hdet_read <= hdet_data->exp_hdet_vs_agc[power_mode][low_zone] )
  {
    zone = low_zone;
  }
  /* Check if HDET reading is railed above the curve */
  else if( hdet_read >= hdet_data->exp_hdet_vs_agc[power_mode][high_zone] )
  {
    zone = high_zone;
  }
  /* Implies HDET reading is on the curve */
  else
  {
    /* Traverse thru the curve to find the zone where hdet_read 
    falls. */
    zone = low_zone;
    for ( i = low_zone; i <= high_zone; i++ )
    {
      if ( hdet_read >= hdet_data->exp_hdet_vs_agc[power_mode][i] )
      {
        zone = i;
      }
      else
        break;
    }
  }

  /* Calculate AGC offset for this bin from the first AGC value in 
  Exp HDET vs AGC table */
  bin_offset = zone * RF_CDMA_EXP_HDET_VS_AGC_BIN_SIZE_DBX640;

  /* Calculate the rise in HDET reading in this bin */
  rise = hdet_data->exp_hdet_vs_agc[power_mode][zone+1] - 
    hdet_data->exp_hdet_vs_agc[power_mode][zone];

  /* Calculate the delta from the lower edge of this bin */
  if ( hdet_read < hdet_data->exp_hdet_vs_agc[power_mode][zone] )
  {
    /* HDET read is actually lower than lowest value in bin. Delta = 0.
    Possible if in low_zone and hdet read is lower than the lowest value in
    exp hdet vs agc table */
    delta = 0;
  }
  else if ( hdet_read > hdet_data->exp_hdet_vs_agc[power_mode][zone+1] )
  {
    /* HDET read is greater than upper value in bin. Delta = rise.
    Possible if in high_zone and hdet read is higher than the highest value in
    exp hdet vs agc table */
    delta = rise;
  }
  else
  {
    /* Valid HDET reading within the bin */
    delta = hdet_read - hdet_data->exp_hdet_vs_agc[power_mode][zone];
  }

  /* Calculate the TX AGC for the first entry in the bin. This equals the AGC 
  value of the first element in expected hdet vs agc table plus the offset
  of the bin */
  txagc_x640_signed = hdet_data->exp_hdet_vs_agc_start_offset[power_mode]
                                                   + bin_offset;

  /* calculate tx agc delta within the bin from slope delta of hdet reading 
  from first entry in bin and the total rise in hdet reading across the bin.
  We subtract (because RF_CDMA_EXP_HDET_VS_AGC_BIN_SIZE_DBX640 is -ve) by 
  rise/2 prior to division for rounding */
  if ( rise != 0 )
  {
    txagc_x640_signed += 
      ( ((delta * RF_CDMA_EXP_HDET_VS_AGC_BIN_SIZE_DBX640) - rise/2) / rise );
  }

  /* Apply temperature compensation. Converting value from db/10 to db/640 
  units */
  txagc_x640_signed += (hdet_data->hdet_vs_temp << 6);

  /* Apply frequency compensation. Converting value from db/10 to db/640 
  units */
  txagc_x640_signed += (hdet_data->hdet_vs_freq << 6);


  /*! Saturate the result ensuring it is signed 16 bit after frequency 
  compensation */
  txagc_x640_signed_limited = (int16)rf_limit_s32( txagc_x640_signed, 
                                                   RF_CDMA_MDSP_MIN_TX_PWR,
                                                   RF_CDMA_MDSP_MAX_TX_PWR );

  if ( hdet_data->debugging_enabled == TRUE )
  {
    RF_MSG_6( RF_MED, "Convert HDET to AGC. Bin: %d, Bin offset: %d, Delta: %d, "
                      "Rise: %d, lim_vs_freq: %d, hdet_vs_temp: %d",
              zone, bin_offset, delta, rise, hdet_data->hdet_vs_freq,
              hdet_data->hdet_vs_temp );
  }

  return txagc_x640_signed_limited;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Extracts HDET related NV data from NV tables to HDET data structure for given
  band/chan

  @details
  Queries NV module for band dependent NVs and extracts Expected HDET vs AGC
  and Lim vs Freq table. Interpolated Lim vs Freq table to get lim_vs_freq 
  adjustment for current channel.

  Calls rf_cdma_analyze_exp_hdet_vs_agc() to analyze the expected HDET vs
  AGC table for validity (not all zeros) and monotonicity. This function 
  further computes the low zone and high zone for this table. This is generic
  one-time per band computation that does not depend on HDET reading.

  Extracts min and max valid HDET reading and corresponding TXAGC values from
  HDET offset and span NV
  
  @param *logical_dev
  Pointer to logical device param of current device of operation

  @param *hdet_data
  Pointer to HDET data of current device

  @param band
  Current CDMA band of operation used to obtain the correct NV tables

  @param chan
  Current CDMA channel of operation used to interpolate lim vs freq NV

  @param power_mode
  Current HDET power mode

  @retval TRUE
  NV tables for given band are not NULL. Expected HDET vs AGC NV is valid.
  NV extraction was successful and sufficient information has been extracted
  from NV to proceed with HDET operation

  @retval FALSE
  NV tables for band are NULL or Expected HDET vs AGC table is invalid 
  (all zeros or non-monotonic). It is not possible to convert HDET readings
  to TX AGC and hence NV load has failed. It is not advisable to run HDET
  loop at this point. Note that NV load does NOT fail if only lim_vs_freq
  is not available or invalid. Lim_vs_freq comp is assumed to be 0 in such a 
  case
*/
boolean
rf_cdma_hdet_extract_nv_data
(
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_hdet_data_type* hdet_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  rfdevice_cdma_hdet_power_mode_type power_mode
)
{
  boolean hdet_nv_status = TRUE;
  /* Get pointers to TX cal data from NV tables */
  const rf_cdma_tx_cal_type* tx_cal_nv_ptr = 
    rf_cdma_nv_get_tx_cal( logical_dev->tx_nv_path[band],
                           band );
  const rf_cdma_tx_lim_cal_type* tx_lim_cal_nv_ptr;
  const rf_cdma_tx_hdet_cal_type* tx_hdet_cal_nv_ptr;

  if ( tx_cal_nv_ptr == NULL )
  {
    RF_MSG_1( RF_ERROR, "CDMA HDET: Unable to extract NV data for band %d",
              band );
    hdet_data->enabled[power_mode] = FALSE;

    return FALSE;
  }

  /* Get Lim/HDET data ptr from tx cal data */
  tx_lim_cal_nv_ptr = &tx_cal_nv_ptr->tx_lim_cal;

  /* Fetch HDET NV data based on power mode */
  if ( power_mode == RFDEVICE_CDMA_HDET_HPM )
  {
    tx_hdet_cal_nv_ptr = &tx_lim_cal_nv_ptr->hpm_hdet;
  }
  else if ( power_mode == RFDEVICE_CDMA_HDET_LPM )
  {
    tx_hdet_cal_nv_ptr = &tx_lim_cal_nv_ptr->lpm_hdet;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "CDMA HDET: Invalid HDET PM %d, cannot extract NV",
              power_mode );
    hdet_data->enabled[power_mode] = FALSE;

    return FALSE;
  }

  /* Check whether NV for requested power mode is enabled or not */
  if ( tx_hdet_cal_nv_ptr->enabled == FALSE )
  {
    if ( power_mode == RFDEVICE_CDMA_HDET_HPM )
    {
      RF_MSG_2( RF_ERROR, "CDMA HDET: HPM HDET disabled by NV on device %d,"
                          "band %d",
                hdet_data->device, band );
    }
    else
    {
      RF_MSG_2( RF_MED, "CDMA HDET: LPM HDET disabled by NV on device %d,"
                          "band %d",
                hdet_data->device, band );
    }
    hdet_data->enabled[power_mode] = FALSE;

    return FALSE;
  }

  /* Extract the LPM threshold from lim cal NV */
  hdet_data->lpm_threshold_val = tx_lim_cal_nv_ptr->lpm_threshold;

  /* Extract HDET min and max trusted values from offset and span. 
  Store in hdet_data */
  hdet_data->min_hdet[power_mode] = tx_hdet_cal_nv_ptr->offset;
  hdet_data->max_hdet[power_mode] = 
    tx_hdet_cal_nv_ptr->offset + tx_hdet_cal_nv_ptr->span;

  if ( (tx_cal_nv_ptr->tx_multi_lin_cal.tx_cal_chan_size > 0) &&
       (tx_cal_nv_ptr->tx_multi_lin_cal.tx_cal_chan_size <= 
                                              RF_CDMA_NV_FREQ_LIST_SIZE) )
  {
    /* Ensure calibrated data is loaded. Assuming that is freq_list_size
    > 0, all other freq compensation NVs are valid. If not valid, this
    interpolation will not be done and lim_vs_freq will use the default
    value of 0 i.e. no freq error in hdet readings. Note that this is NOT
    flagged as a failure */
    hdet_data->hdet_vs_freq = 
      rf_lininterp_u32_s8( tx_cal_nv_ptr->tx_multi_lin_cal.tx_cal_freq,
                           tx_lim_cal_nv_ptr->hdet_vs_freq,
                           tx_cal_nv_ptr->tx_multi_lin_cal.tx_cal_chan_size,
                           rf_cdma_get_tx_carrier_freq(band,chan) );
  }

  /* Get Expected HDET vs AGC start power level in db640 from NV structure */
  hdet_data->exp_hdet_vs_agc_start_offset[power_mode] = 
    rf_cdma_mdsp_dbm10_to_txagc( tx_hdet_cal_nv_ptr->start_pwr_lvl,
                                 rf_cdma_get_turnaround_constant(band) * 10 );

  /* Analyze Expected HDET vs AGC table for monotonicity and validity
  (atleast 1 non-zero value). If all valid, this function will compute
  the low_zone and high_zone for future usage */
  hdet_nv_status &= 
    rf_cdma_analyze_exp_hdet_vs_agc( hdet_data,
                                     tx_hdet_cal_nv_ptr->exp_hdet_vs_agc,
                                     RF_CDMA_NV_EXP_HDET_VS_AGC_SIZE,
                                     power_mode );

  if ( hdet_nv_status == TRUE )
  {
    /* Since NV tables are valid, go ahead to update other NV dependent 
    state variables */
    hdet_data->txagc_at_min_hdet[power_mode] = 
      rf_cdma_convert_hdet_read_to_txagc( hdet_data, 
                                          hdet_data->min_hdet[power_mode],
                                          power_mode );

    hdet_data->txagc_at_max_hdet[power_mode] = 
      rf_cdma_convert_hdet_read_to_txagc( hdet_data, 
                                          hdet_data->max_hdet[power_mode],
                                          power_mode );
  }
  else
  {
    RF_MSG_2( RF_ERROR,"rf_cdma_hdet_extract_nv_data: "
                       "Dev %d, HDET PM %d - NV is invalid",
              hdet_data->device, power_mode );
  }
  
  /* Store the enabled status for this power mode */
  hdet_data->enabled[power_mode] = hdet_nv_status;

  return hdet_nv_status;
} /* rf_cdma_hdet_extract_nv_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unity gain, order-1, feedforward IIR filter

  @details
  Helper function to filter TX AGC error estimated by HDET loop. Use Q15 format
  for fractions

  @param curr_err
  Current error in int16 format to suit raw tx agc units (signed db/640)

  @param prev_err
  Previous error in int16 format to suit raw tx agc units (signed db/640)

  @param filter_coeff_inv
  IIR filter coefficient. This is a non zero integer. 1/filter_coeff_inv is the
  actual filter coefficient. Note that since this is a unit gain, order-1 
  feedforward IIR filter, implicitly the filter coeffs become:
  [1/filter_coeff_inv    1 - 1/filter_coeff_inv]

  @return
  Filtered Error processed by this IIR filter. Essentially returns:
  (1/filter_coeff_inv)*curr_err + (1-1/filter_coeff_inv)*prev_err

  0x8000 if filter_coeff_inv is invalid = 0
*/
int16
rf_cdma_hdet_filter_err
(
  int16 curr_err,
  int16 prev_err,
  uint8 filter_coeff_inv
)
{
  /* Filter Coeff 0 in Q15 format */
  uint16 filter_coeff0_q15;
  /* Filter Coeff 1 in Q15 format */
  uint16 filter_coeff1_q15;
  /* Filter output in Q15 format */
  int32 filtered_err_q15;
  /* 16-bit Result - largest negative number by default*/
  int16 filtered_err = -32768;

  if ( filter_coeff_inv != 0 )
  {
    /* Coeff 0 = 1 / input */
    filter_coeff0_q15 = (1 << 15) / filter_coeff_inv;

    /* Coeff 1 = 1 - Coeff 0. Since this is a unit gain filter */
    filter_coeff1_q15 = (1 << 15) - filter_coeff0_q15;

    /* Filter output = Coeff0 x err[n] + Coeff1 x err [n-1] */
    filtered_err_q15 = 
      (curr_err * filter_coeff0_q15) + (prev_err * filter_coeff1_q15);

    /* For Rounding, Add 0.5 in Q15 before discarding fractional bits */
    filtered_err_q15 += 0x4000;

    /* Divide by 2^15 to get back to normal format */
    filtered_err = (int16)(filtered_err_q15 >> 15);

  }

  return filtered_err;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  IIR Filter for HDET accumulated error

  @details
  Helper function to filter accumulated TX AGC error collected by HDET loop. 
  Use Q15 format for fractions

  @param curr_err
  Current error in int16 format to suit raw tx agc units (signed db/640)

  @param prev_accum_err
  Previous accumulated filtered error in int16 format to suit raw tx agc units 
  (signed db/640)

  @param filter_coeff_inv
  IIR filter coefficient. This is a non zero integer. 1/filter_coeff_inv is the
  actual filter coefficient.

  @return
  Filtered Accumulated Error processed by this IIR filter.

  0x8000 if filter_coeff_inv is invalid = 0
*/
int16
rf_cdma_hdet_filter_accum_err
(
  int16 curr_err,
  int16 prev_accum_err,
  uint8 filter_coeff_inv
)
{
  /* Filter Coeff 0 in Q15 format */
  uint16 filter_coeff0_q15;
  /* Filter output in Q15 format */
  int32 filtered_accum_err_q15;
  /* 16-bit Result - largest negative number by default*/
  int16 filtered_err = -32768;

  if ( filter_coeff_inv != 0 )
  {
    /* Coeff 0 = 1 / input */
    filter_coeff0_q15 = (1 << 15) / filter_coeff_inv;

    /* Filter accum = accum_err + Coeff0 x curr_err */
    filtered_accum_err_q15 = 
       (prev_accum_err << 15) + (curr_err * filter_coeff0_q15);

    /* For Rounding, Add 0.5 in Q15 before discarding fractional bits */
    filtered_accum_err_q15 += 0x4000;

    /* Divide by 2^15 to get back to normal format */
    filtered_err = (int16)(filtered_accum_err_q15 >> 15);

  }

  return filtered_err;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface to reset the HDET accumulated error, based on the current 
  temperature compensation data.

  @details
  This function is meant to reset the HDET accumulated error every time temp 
  comp runs, in order to accommodate for the error that has been currently 
  accumulated, and the compensation that statically characterized temp comp 
  provides. This ensures that temp comp is always applied whenever triggered, 
  and the remainder of the error is filtered into HDET, to continue with 
  dynamic correction.

  @param hdet_data
  Pointer to HDET data structure

  @param new_accum_err
  The new error in dB10, which HDET accumulator must be reset to.

  @return
  TRUE if the re-initialization of accumulated error was successful, else 
  FALSE.
*/
boolean
rf_cdma_hdet_set_accum_err_from_temp_comp
(
  rf_cdma_hdet_data_type* hdet_data,
  int16 new_accum_err
)
{
  boolean status;

  if ( hdet_data != NULL )
  {
    /* Convert the err to dB640 units before updating HDET
    data structure */
    hdet_data->accum_lin_tx_err = new_accum_err * -64;

    RF_MSG_2( RF_HIGH, "rf_cdma_hdet_set_accum_err_from_temp_comp: "
                       "Reset HDET accum error to %d(dB640) %d(dB10)",
              hdet_data->accum_lin_tx_err,
              new_accum_err );

    status = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_set_accum_err_from_temp_comp: "
                      "NULL HDET data" );
    status = FALSE;
  }

  return status;
} /* rf_cdma_hdet_set_accum_err_from_temp_comp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to clear HDET timers and state variables

  @details
  Used by rf_cdma_hdet_stop() and any other situations where HDET loop needs to
  be stopped and its state reset.

  @param *hdet_data
  Pointer to HDET data that needs to be reset
*/
static void
rf_cdma_hdet_clear_timers_reset_state
(
  rf_cdma_hdet_data_type* hdet_data
)
{
  if ( hdet_data->debugging_enabled == TRUE )
  {
    RF_MSG( RF_MED, "HDET stopped! Clear isHKADCbusy flag" );
  }
  
  /* Clear callback timers right away*/
  timer_clr( &hdet_data->hdet_read_timer, T_NONE); 
  timer_clr( &hdet_data->hdet_mdsp_polling_timer, T_NONE);

  /* Reset FW HDET state machine - we check for current state to ensure
  redundant resets are not sent to FW. If a hdet reset message is sent to FW
  while it is already in the process of resetting, FW crashes hence must
  be avoided. */
  if ( hdet_data->state != RF_CDMA_HDET_INACTIVE )
  {
    hdet_data->state = RF_CDMA_HDET_INACTIVE;
  }

  /* revert all HDET data for the device to initial state */
  hdet_data->state_count = 0;
  hdet_data->filtered_tx_err = 0;
  hdet_data->accum_lin_tx_err = 0;
  hdet_data->tx_estimate_err = 0;
  hdet_data->last_hdet_read = 0;
  hdet_data->last_txagc_at_hdet_read = 0;
  hdet_data->hdet_read_validity_meter = 0;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculates filtered TX error from HDET reading and TX AGC and applies update
  to power limiting module
  
  @details
  Based on HDET offset, span, expected HDET vs AGC table and lim_vs_freq value,
  converts a HDET reading to TX AGC if valid. Computes the TX error from this.
  Filters the error based on previous error. Updates the error in power 
  limiting module.

  hdet_data->filtered_tx_err and hdet_data->tx_estimate_err will be updated 
  by this function. Former is the filtered error in desired tx agc vs actual
  tx power computed by HDET. This is compensation that should be applied to 
  the max power limit. Latter is a metric to track the convergence of 
  filtered tx error to actual TX error in tx agc. It is the difference between
  error compensated tx agc based on current calculations and actual tx power
  based on current hdet reading.

  @param *hdet_data
  Pointer to HDET data for current device. Will use lim_vs_freq, min/max_hdet,
  exp_hdet_vs_agc tbl and params, state(acq or tracking), prev tx agc error.
  Must make sure these arguments are up to data for accurate TX AGC estimation

  @param txagc_at_read
  TX AGC when HDET reading was taken. 16bit signed value from MDSP in db/640

  @param hdet_reading
  16-bit HDET reading from device

  @param power_mode
  Current HDET power mode

  @param ept_enabled
  Indicate if EPT HDET algorithm must be executed
*/
void
rf_cdma_hdet_update_tx_err
(
  rf_cdma_hdet_data_type* hdet_data,
  int16 txagc_at_read,
  uint16 hdet_reading,
  rfdevice_cdma_hdet_power_mode_type power_mode,
  boolean ept_enabled
)
{
  int16 txagc_est, curr_tx_err;
  /* Based on previous error estimate, actual tx agc should be around this 
  value. */
  int16 exp_tx_based_on_prev_err = txagc_at_read + hdet_data->filtered_tx_err;
  boolean hdet_should_be_in_range = FALSE;

  if ( (exp_tx_based_on_prev_err >= hdet_data->txagc_at_max_hdet[power_mode]) 
       &&
       (exp_tx_based_on_prev_err <= hdet_data->txagc_at_min_hdet[power_mode]) )
  {
    /* Exp TX agc with error is within HDET operating range. Thus HDET
    reading "should" be within min, max value from offset/span */
    hdet_should_be_in_range = TRUE;
  }

  if ( (hdet_reading < hdet_data->min_hdet[power_mode]) ||
       (hdet_reading > hdet_data->max_hdet[power_mode]) )
  {
    /* HDET reading falls outside reliable operating range. Check if this was
    expected */
    if ( hdet_should_be_in_range == TRUE )
    {
      /* Based on TX AGC expectation, HDET reading should have been in range.
      Compensate based on min/max hdet read. Limit the read */
      hdet_reading = ( (hdet_reading < hdet_data->min_hdet[power_mode]) ? 
                       hdet_data->min_hdet[power_mode] :
                       hdet_data->max_hdet[power_mode] );
    }
    else
    {
      /* Nothing to do during this HDET read */
      if ( hdet_data->debugging_enabled == TRUE )
      {
        RF_MSG_7( RF_MED, "Outside HDET operating range. TX AGC at read: %d,"
                          "hdet_read: %d, min_hdet: %d, max_hdet: %d,"
                          "min_agc: %d, max_agc: %d, HDET PM %d",
                  txagc_at_read,
                  hdet_reading,
                  hdet_data->min_hdet[power_mode],
                  hdet_data->max_hdet[power_mode],
                  hdet_data->txagc_at_min_hdet[power_mode],
                  hdet_data->txagc_at_max_hdet[power_mode],
                  power_mode );
      }

      return;
    }
  }

  /* Convert HDET reading to TX AGC estimate */
  txagc_est = rf_cdma_convert_hdet_read_to_txagc( hdet_data, 
                                                  hdet_reading,
                                                  power_mode );

  /* Calculate error in TX AGC */
  curr_tx_err = txagc_est - txagc_at_read;

  /* Compute filtered error from current error and previous filtered error */
  hdet_data->filtered_tx_err = 
    rf_cdma_hdet_filter_err( curr_tx_err,
                             hdet_data->filtered_tx_err, 
                             HDET_ERR_FILTER_COEFF(hdet_data) );

  /* We update the max power limit provided to FED */
  /* Send error update to power limiting module converting to dB10 units */
  if ( hdet_data->mode == RFM_1X_MODE )
  {
    rf_1x_plim_update_tx_err_from_hdet( hdet_data->device,
                                        hdet_data->filtered_tx_err );
  }
  else
  {
    /* HDR Mode */
    rf_hdr_plim_update_tx_err_from_hdet( hdet_data->device,
                                         hdet_data->filtered_tx_err );
  }

  /* Compute error in tx estimate. This tracks the convergence of estimated 
  error to actual error and decides acq to track state transitions */
  hdet_data->tx_estimate_err = (txagc_at_read + hdet_data->filtered_tx_err - 
                                  txagc_est);
  
  //if ( hdet_data->debugging_enabled )
  {
    /* Update TX error in power limiting module */
    RF_MSG_9( RF_HIGH, "CDMA HDET. TX error updated: New filt TX err: %d "
                       "(%d db10), estimation err: %d, HDET read: %d, "
                       "TX AGC from HDET: %d (%d), "
                       "TX AGC from MDSP : %d (%d), "
                      "Raw current TX err (db640) %d",
              hdet_data->filtered_tx_err, 
              hdet_data->filtered_tx_err / 64,
              hdet_data->tx_estimate_err,
              hdet_reading,
              txagc_est,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(hdet_data->band),
                                    txagc_est),
              txagc_at_read,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(hdet_data->band),
                                    txagc_at_read),
              curr_tx_err );
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates operating state of HDET
  
  @details
  Based on error in TX error estimate and the number of times HDET reading has
  been done in current state, decides if the state needs to be changed.

  hdet_data->state will be updated with the next state based on this computation

  @param *hdet_data
  Pointer to HDET data based on which state will be decided.
*/
void
rf_cdma_hdet_update_state
(
  rf_cdma_hdet_data_type* hdet_data
)
{
  rf_cdma_hdet_state_type curr_state = hdet_data->state;
  rf_cdma_hdet_state_type next_state;

  switch ( curr_state )
  {
  case RF_CDMA_HDET_ACQUISITION:
    if ( (hdet_data->state_count < hdet_data->device_param.acq_mode_min_count) ||
         (abs(hdet_data->tx_estimate_err) > 
          (hdet_data->device_param.acq2trk_err_thresh_x10 * 64)) )
    {
      /* Error exceeds threshold or havent made enough reads in Acquisition 
      state. Continue operating in Acquisition state */
      next_state = RF_CDMA_HDET_ACQUISITION;
    }
    else
    {
      /* Satisfied min count requirement and error has converged within threshold.
      Switch to Tracking mode */
      RF_MSG_1( RF_LOW, "CDMA HDET: Transition from Acquisition to Tracking "
                        "state, HDET PM %d", hdet_data->power_mode );
      next_state = RF_CDMA_HDET_TRACKING;
    }
    break;

  case RF_CDMA_HDET_TRACKING:
    if ( abs(hdet_data->tx_estimate_err) < 
         (hdet_data->device_param.trk2acq_err_thresh_x10 * 64) )
    {
      /* Error within threshold. Continue in tracking state of operation */
      next_state = RF_CDMA_HDET_TRACKING;
    }
    else
    {
      /* Error has exceeded Track to Acq threshold. Switch to acquisition 
      state to converge it back */
      RF_MSG_1( RF_LOW, "CDMA HDET: Transition from Tracking to Acquisition "
                        "state, HDET PM %d", hdet_data->power_mode );
      next_state = RF_CDMA_HDET_ACQUISITION;
    }
    break;

  default:
    /* Invalid curr_state. Make next_state INACTIVE */
    next_state = RF_CDMA_HDET_INACTIVE;
    break;
  }

  if ( hdet_data->debugging_enabled == TRUE )
  {
    RF_MSG_4( RF_MED, "CDMA HDET. Curr state: %d, Next state: %d. "
                      "state count: %d, abs(tx estimate err): %d, ",
              curr_state,
              next_state,
              hdet_data->state_count,
              abs(hdet_data->tx_estimate_err) );
  }

  if ( hdet_data->state != next_state )
  {
    /* Update hdet_data with next state */
    hdet_data->state = next_state;
    hdet_data->state_count = 0;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates operating power mode of HDET
  
  @details
  Provide a wrapper for updating the HDET data structure with the 
  required power mode. The updated power mode is used for all HDET 
  functionality, including the setup updates sent to the device driver.

  @param *hdet_data
  Pointer to HDET data in which power mode must be updated
 
  @param power_mode
  New power mode to which HDET operation must transition
*/
void
rf_cdma_hdet_update_power_mode
(
  rf_cdma_hdet_data_type* hdet_data,
  rfdevice_cdma_hdet_power_mode_type power_mode
)
{
  /* Update the HDET state variable with the new power mode value,
  if needed */
  if ( hdet_data->power_mode != power_mode )
  {
    RF_MSG_2( RF_LOW, "CDMA HDET: Transition HDET Power Mode "
                      "PM %d -> PM %d",
              hdet_data->power_mode,
              power_mode );

    hdet_data->power_mode = power_mode;
  }

}/* rf_cdma_hdet_update_power_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates the read validity indicatory for HDET

  @details
  This function helps to update the current validity indicator for every HDET 
  read. This validity of HDET reads is used by the EPT temp comp algorithm 
  to decide whether it should propagate the error to HDET, or apply the 
  compensation itself without relying on HDET.

  @param hdet_data
  Pointer to HDET data structure

  @param curr_read_valid
  Indicates if the current read is valid or not
*/
void
rf_cdma_hdet_update_read_validity
(
  rf_cdma_hdet_data_type* hdet_data,
  boolean curr_read_valid
)
{
  if ( hdet_data != NULL )
  {
    hdet_data->hdet_read_validity_meter <<= 1;
    hdet_data->hdet_read_validity_meter |= curr_read_valid;
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_update_read_validity: "
                      "NULL HDET data" );
  }

  return;

} /* rf_cdma_hdet_update_read_validity */

/*! @} */
/*============================================================================*/
/*!
  @name Timer callback Functions

  @brief
  Functions called by timer cb in timer_task. These are functions called to
  trigger hdet reads and poll mdsp smem
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  HDET read callback function that will be called to schedule HDET reads
  
  @details
  Will be called periodicaly based on period for the current state - acquisition 
  or tracking. Instructs MDSP to schedule HDET read. Starts timer to poll
  MDSP for HDET read done status. All processing of HDET read will happen in
  this polling function rf_cdma_hdet_mdsp_polling_cb() when MDSP HDET read is
  done.

  Note that HDET must be in an active state - acquisition or tracking for this
  function to do anything.

  @param data
  Pointer to HDET data of current device cast to timer data type
*/
void
rf_cdma_hdet_read_cb
(
  timer_cb_data_type data
)
{
  rf_cdma_hdet_data_type* hdet_data = (rf_cdma_hdet_data_type*)data;
  rf_cdma_mdsp_hdet_read_data_type mdsp_read_data = {0,0};
  int16 curr_tx_total = 0 ;
  int16 curr_max_pwr_lim;
  /* Device Status Read Pointer */
  const rf_cdma_data_status_type *dev_status_r;

  if ( hdet_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_read_cb: Null data!" );
    return;
  }
  
  /*! Get the current device */ 
  rf_common_enter_critical_section( &hdet_data->crit_section );

  if ( (rfm_get_calibration_state() == FALSE) &&
       (hdet_data->state != RF_CDMA_HDET_INACTIVE) )
  {
    /* Determine the HDET operating power mode, if LPM is enabled */
    if ( hdet_data->enabled[RFDEVICE_CDMA_HDET_LPM] == TRUE )
    {
      /* Obtain the current max power limit */
      curr_max_pwr_lim = rf_cdma_plim_get_curr_max_power_limit( 
        hdet_data->device,
        hdet_data->mode );

      if ( curr_max_pwr_lim < hdet_data->lpm_threshold_val )
      {
        /* Transition to or stay in low power mode */
        rf_cdma_hdet_update_power_mode ( hdet_data, RFDEVICE_CDMA_HDET_LPM );

        if ( hdet_data->debugging_enabled )
        {
          RF_MSG_3( RF_MED, "CDMA HDET: Max power Lim %d(dBm10) lower than "
                            "LPM threshold %d(dBm10). Using PM %d",
                    curr_max_pwr_lim,
                    hdet_data->lpm_threshold_val,
                    hdet_data->power_mode );
        }
      }
      else
      {
        /* Transition to or stay in high power mode */
        rf_cdma_hdet_update_power_mode ( hdet_data, RFDEVICE_CDMA_HDET_HPM );

        if ( hdet_data->debugging_enabled )
        {
          RF_MSG_3( RF_MED, "CDMA HDET: Max power Lim %d(dBm10) higher than "
                            "LPM threshold %d(dBm10). Using PM %d",
                    curr_max_pwr_lim,
                    hdet_data->lpm_threshold_val,
                    hdet_data->power_mode );
        }
      }
    }

    dev_status_r = rf_cdma_get_device_status( hdet_data->device );
    
    if ( dev_status_r != NULL )
    {
    curr_tx_total = 
      rf_cdma_mdsp_get_tx_total_power(hdet_data->logical_device->tx_mdsp_path,
                                        rf_cdma_data_get_txlm_handle (hdet_data->device) ,
                                      hdet_data->mode);
    }
    else
    {
      RF_MSG_1( RF_ERROR,"rf_cdma_hdet_read_cb: NULL data for device %d",
                         hdet_data->device );
    }
    
    /* Check if current TX power is high enough to be in HDET operating
    range. If the tx power is lower than the tx power at min hdet by
    more than the max possible tx err, there is no point even enabling
    PDET and wasting current. We know that the HDET reading will NOT
    fall in the operating range. Note that we are using signed dB/640 
    TX AGC units for this computation. Hence the signs are inverted from
    actual TX power in dB */
    if ( (curr_tx_total - hdet_data->txagc_at_min_hdet[hdet_data->power_mode])
         < (RF_CDMA_HDET_MAX_TX_ERR_DB * 640) )
    {

      /* Get the current status of MDSP HDET read data, mainly the read_count.
      txagc_at_read is a dont care at this point. Also implicitly confirms that 
      MDSP SMEM is initialized and ready for HDET operation */
      if ( (rf_cdma_obtain_hkadc_lock( hdet_data->device ) == TRUE) &&
           (rf_cdma_mdsp_get_hdet_read_data(
             hdet_data->device,
             hdet_data->mode,
             &mdsp_read_data) == TRUE) )
      {
        if ( hdet_data->debugging_enabled )
        {
          RF_MSG( RF_MED, 
                  "rf_cdma_hdet_read_cb: HKADC/FW lock obtained for HDET!" );
        }

        /* We will be doing HDET reads via FW. Setup TX device for HDET reads */
        rfdevice_hdet_cdma_setup(hdet_data->logical_device->hdet_obj[hdet_data->band],
                                 NULL,
                                 RFDEVICE_EXECUTE_IMMEDIATE,
                                 hdet_data->power_mode );

        /* Set MDSP read count to what was read back from SMEM. This counter will
        be incremented by MDSP when a HDET read is completed. We can then compare
        the value with the last recorded value to determine if a fresh HDET read
        is ready to be processed. */
        hdet_data->mdsp_read_count = mdsp_read_data.read_count;
      
        /* Send Message to FW to trigger HDET read */
        rf_cdma_mdsp_trigger_hdet_read( hdet_data, hdet_data->mode );
        
        /* Reset MDSP polling count */
        hdet_data->mdsp_polling_count = 0;
        
        /* Set timer to poll FW shared memory for HDET read completion */
        timer_set( &hdet_data->hdet_mdsp_polling_timer, 
                   RF_CDMA_HDET_MDSP_POLLING_PERIOD_MS, 
                   0,
                   T_MSEC );
      } /* if ( rf_cdma_mdsp_get_hdet_read_data() == TRUE )*/
      else
      {

        /* Before any interaction with device or FW, check if HKADC is busy
        doing therm reads or available for HDET - get dev_status_w to
        get access to isHKADCbusy flag */
        dev_status_r = rf_cdma_get_device_status( hdet_data->device );

        if( dev_status_r == NULL )
        {
          RF_MSG_1( RF_ERROR,"rf_cdma_hdet_read_cb: NULL data for device %d",
                             hdet_data->device );
        }
        else
        {
          RF_MSG_1( RF_HIGH, "rf_cdma_hdet_read_cb: HKADC locked by Temp Comp!"
                             "isHKADCbusy=%d. Try again after read_period!", 
                             dev_status_r->isHKADCbusy );
        }

        timer_set( &hdet_data->hdet_read_timer, 
                   HDET_READ_PERIOD(hdet_data), 
                   0,
                   T_MSEC );
      }
    } /* if ( (curr_tx_total - hdet_data->txagc_at_min_hdet) < (RF_CDMA_HDET_MAX_TX_ERR_DB * 640) ) */
    else
    {
      /* TX pwr too low to even enable HDET. Re-schedule read after read_period */
      if ( hdet_data->debugging_enabled == TRUE )
      {
        RF_MSG_4( RF_MED, "rf_cdma_hdet_read_cb: TX power too low to try HDET reads."
                          "TX AGC: %d, TX AGC at min HDET: %d, max tx err: %d,"
                          "HDET PM %d",
                  curr_tx_total,
                  hdet_data->txagc_at_min_hdet[hdet_data->power_mode],
                  RF_CDMA_HDET_MAX_TX_ERR_DB * 640,
                  hdet_data->power_mode );
      }
      
      /* Invalid HDET read, update read indicator */
      rf_cdma_hdet_update_read_validity( hdet_data, FALSE );

      timer_set( &hdet_data->hdet_read_timer, 
                 HDET_READ_PERIOD(hdet_data), 
                 0,
                 T_MSEC );
    }
  } /* if ( (rfm_get_calibration_state() == FALSE) && 
    (hdet_data->state != RF_CDMA_HDET_INACTIVE) )*/
  else
  {
    /* Either in cal mode or hdet in inactive state. Stop HDET loop altogether
    by resetting timers */
    rf_cdma_hdet_clear_timers_reset_state(hdet_data);
    RF_MSG_2( RF_ERROR, "rf_cdma_hdet_read_cb: Stopping HDET loop. Cal mode: %d,"
                        " HDET state: %d",
              rfm_get_calibration_state(), hdet_data->state ) ;
  }

  rf_common_leave_critical_section( &hdet_data->crit_section );
}

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Polling callback function to poll MDSP for HDET read completion and process
  it when done
  
  @details
  Polls MDSP SMEM for HDET read completion. If MDSP indicates HDET read has
  been completed and successful, reads HDET from device. If HDET reading is 
  valid, uses it to estimate filter TX error and applies it to power limiting 
  module. If all is successful schedules timer for rf_cdma_hdet_read_cb() at 
  an interval dependent on current state of operation - acquisition or 
  tracking.

  If HDET read is not completed, schedules timer to itself to poll again after
  RF_CDMA_HDET_MDSP_POLLING_PERIOD_MS = 1ms. Keeps track of the number of times
  SMEM has been polled. 

  If MDSP increments HDET read_count but indicates read failure, assumes 
  something went wrong during this read and schedule rf_cdma_hdet_read_cb() to 
  retry. 

  If MDSP does not respond in 100ms, assumes something is drastically wrong with
  firmware and stops the HDET loop completely. This should never happen.

  Note that HDET must be in an active state - acquisition or tracking for this
  function to do anything.

  @param data
  Pointer to HDET data of current device cast to timer data type
*/
void
rf_cdma_hdet_mdsp_polling_cb
(
  timer_cb_data_type data
)
{
  rf_cdma_hdet_data_type* hdet_data = (rf_cdma_hdet_data_type*)data;
  rf_cdma_mdsp_hdet_read_data_type mdsp_hdet_data = {0,0};
  rfdevice_cdma_hdet_read_type hdet_read;
  /* Device Status Read Pointer */
  const rf_cdma_data_status_type *dev_status_r;
  rflm_c2k_data_t* rflm_cdma_data_ptr = rflm_c2k_get_data();

  if ( hdet_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_mdsp_polling_cb: NULL data! ");
    return;
  }
  
  /*! Get the current device */ 
  dev_status_r = rf_cdma_get_device_status( hdet_data->device );

  if ( dev_status_r == NULL)
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_mdsp_polling_cb: NULL data! ");
    return;
  }

  /* Enter critical section */
  rf_common_enter_critical_section( &hdet_data->crit_section );

  if ( hdet_data->state != RF_CDMA_HDET_INACTIVE )
  {
    /* Increment polling count to keep track of number of times we have
    polled MDSP to get a valid HDET read */
    hdet_data->mdsp_polling_count++;
    
    /* Read MDSP HDET data. Check if MDSP has performed the HDET read sequence and 
    incremented counter by 1. */
    if ( (rf_cdma_mdsp_get_hdet_read_data(hdet_data->device,
                                          hdet_data->mode,
                                          &mdsp_hdet_data)==TRUE) &&
         (mdsp_hdet_data.read_count == 
          (hdet_data->mdsp_read_count + 1)) )
    {
      if ( mdsp_hdet_data.read_success == TRUE )
      {
        /* HDET read SSBI sequence was successfully executed by firmware */
        if ( hdet_data->debugging_enabled == TRUE )
        {
          RF_MSG_2( RF_MED, "rf_cdma_hdet_mdsp_polling_cb: HDET read DONE in "
                            "%d polls, read_success: %d",
                    hdet_data->mdsp_polling_count, mdsp_hdet_data.read_success );
        }
        
        /* Read HDET from device */
        rfdevice_hdet_cdma_read_hdet( hdet_data->logical_device->hdet_obj[hdet_data->band],
                                      TRUE,
                                      &hdet_read );

        if (hdet_read.hdet_quality == TRUE)
        {
          /* Update members recording last hdet read and tx pwr at read */
          hdet_data->last_hdet_read = hdet_read.hdet_reading;
          hdet_data->last_txagc_at_hdet_read = mdsp_hdet_data.txagc_at_read;
    
          /* HDET reading is valid. Go ahead and execute algorithm to
          estimated TX AGC error and update to power limiting module
          or linearizer, based on EPT enabled status */
          rf_cdma_hdet_update_tx_err( hdet_data,
                                      mdsp_hdet_data.txagc_at_read,
                                      hdet_read.hdet_reading,
                                      hdet_data->power_mode,
                                      dev_status_r->ept_online_enabled  
                                    );
        
          /* Increment state_count */
          hdet_data->state_count++;
        } /* if (hdet_read.hdet_quality == TRUE) */
        else
        {
          /* HDET Quality not good. This HDET read cannot be processed. Try
           again after HDET_READ_PERIOD */
          RF_MSG_1( RF_HIGH, "rf_cdma_hdet_mdsp_polling_cb: Bad HDET read "
                             "quality from device. Ignore this read: %d",
                    hdet_read.hdet_reading );
        }
    
        /* Determine if state of operation needs to be updated, This function
        will update hdet_data with the next state */
        rf_cdma_hdet_update_state( hdet_data );
        
      } /* if ( mdsp_hdet_data.read_success == TRUE ) */
      else
      {
        /* read_success flag was not set but read_count was incremented. This
        implies MDSP has timed out for the current reading. Do NOT poll again. 
        Try again after HDET_READ_PERIOD. Also, state_count is not incremented
        as a valid read was not completed and hence state will not be updated */
        RF_MSG_2( RF_HIGH, "rf_cdma_hdet_mdsp_polling_cb: MDSP abandoned"
                           " read after %d polls, read_success: %d. Try again"
                           " later",
                  hdet_data->mdsp_polling_count, mdsp_hdet_data.read_success );
      }

      /* HKADC/FW use by HDET complete, release lock for next try*/
      rf_cdma_data_set_isHKADCbusy( hdet_data->device, FALSE );   

      if ( hdet_data->mode == RFM_1X_MODE )
      {
       rflm_cdma_data_ptr->onex_data.hdet_data.hdet_read_in_process = FALSE;
      }
      else
      {
       rflm_cdma_data_ptr->hdr_data.hdet_data.hdet_read_in_process = FALSE;
      }
	  

      if ( hdet_data->debugging_enabled == TRUE )
      {
        RF_MSG( RF_MED, "rf_cdma_hdet_mdsp_polling_cb: HKADC/FW lock released"
                        "by HDET!" );
      }
        
      /* Schedule new timer for HDET read depending on state */
      timer_set( &hdet_data->hdet_read_timer, 
                 HDET_READ_PERIOD(hdet_data), 
                 0,
                 T_MSEC );
    } /* if (mdsp_hdet_data.read_count == (hdet_data->mdsp_read_count + 1)) */
    else
    {
      /* HDET read sequence not yet completed by MDSP */
    
      if ( hdet_data->debugging_enabled == TRUE )
      {
        RF_MSG_3( RF_MED, "rf_cdma_hdet_mdsp_polling_cb: HDET read not done in "
                          "%d polls: mdsp_count = %d, last_count = %d",
                  hdet_data->mdsp_polling_count,
                  mdsp_hdet_data.read_count,
                  hdet_data->mdsp_read_count );
      }
      
      if( hdet_data->mdsp_polling_count < 100 )
      {
        /* Schedule timer to poll MDSP again */
        timer_set( &hdet_data->hdet_mdsp_polling_timer,
                   RF_CDMA_HDET_MDSP_POLLING_PERIOD_MS,
                   0,
                   T_MSEC );
      }
      else if ( hdet_data->mdsp_polling_count < 1000 )
      {
        /* Too many polls with no result. Continue polling at slower rate */
        timer_set( &hdet_data->hdet_mdsp_polling_timer,
                   10*RF_CDMA_HDET_MDSP_POLLING_PERIOD_MS,
                   0,
                   T_MSEC );
   
      } /* if ( hdet_data->mdsp_polling_count > 100 ) */
      else
      {
        /* Too many polls with no result. Something drastically wrong with MDSP.
        Stop HDET loop altogether. Will be re-started only when 
        rf_cdma_hdet_start() is called again. */
        RF_MSG_1( RF_ERROR, "rf_cdma_hdet_mdsp_polling_cb: HDET read not done in "
                            "%d polls. Stopping HDET loop completely.",
                            hdet_data->mdsp_polling_count );
    
        /* Reset all timers and state variables to put HDET in "Inactive" state.
        Release HKADC/FW lock */
        rf_cdma_hdet_clear_timers_reset_state( hdet_data );
      }
    }
  } /* if ( hdet_data->state != RF_CDMA_HDET_INACTIVE ) */
  else
  {
    /* MDSP polling cb with hdet in inactive state. Stop HDET loop altogether
    by resetting timers - release HKADC/FW lock */
    rf_cdma_hdet_clear_timers_reset_state(hdet_data);
    RF_MSG_2( RF_ERROR, "rf_cdma_hdet_mdsp_polling_cb: Stopping HDET loop. "
                        " HDET state: %d, device: %d",
              hdet_data->state, hdet_data->device ) ;
  }

  /* Leave critical section */
  rf_common_leave_critical_section( &hdet_data->crit_section );
}

/*! @} */

/*============================================================================*/
/*!
  @name HDET APIs

  @brief
  APIs to driver outside HDET module. This section contains the simple control
  interface to HDET module to initialize, start/stop it.
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Initializes the HDET state machine and state variables for a given device
    
  @details  
  This API only sets up internal data structures and state for HDET operation
  on the selected device. It does NOT do any communication with actual 
  hardware. 

  HDET algorithms and function rely on a well-defined initial state of its 
  data. Thus this function should be the first to be called on bootup via
  rfm_init() before any HDET operation is attempted.

  If this API is called again in the middle of HDET operation, it will reset
  the HDET state variables.

  @param *logical_dev
  Pointer to logical device param of device whose HDET has to be initialized   
    
  @param hdet_data  
  Pointer to the data structure that contains HDET info for a given device
 
  @return
  TRUE if HDET initialization was successful on this device, FALSE otherwise
*/
boolean 
rf_cdma_hdet_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_hdet_data_type* hdet_data
)
{
  boolean init_status = TRUE;

  if ( (hdet_data == NULL) || (logical_dev == NULL) )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_hdet_init: HDET cannot be initialized"
                        " for device %d - invalid data", device);
    return FALSE;
  }

  /* Initialize hdet_supported flag to FALSE and let TX device update it to 
  TRUE if it supports HDET */
  hdet_data->device_param.hdet_supported = FALSE;

  /* Initialize Mutex lock for HDET critical section - 1 per device */
  init_status &= rf_common_init_critical_section( &hdet_data->crit_section );

  if ( rfc_cdma_is_tx_device(device) == TRUE  )
  {
    /* Initialize HDET state variables */
    hdet_data->state_count = 0;
    hdet_data->state = RF_CDMA_HDET_INACTIVE;
    hdet_data->power_mode = RFDEVICE_CDMA_HDET_HPM;
    hdet_data->enabled[RFDEVICE_CDMA_HDET_HPM] = FALSE;
    hdet_data->enabled[RFDEVICE_CDMA_HDET_LPM] = FALSE;
    hdet_data->filtered_tx_err = 0;
    hdet_data->accum_lin_tx_err = 0;
    hdet_data->tx_estimate_err = 0;
    hdet_data->hdet_vs_freq = 0;
    hdet_data->hdet_vs_temp = 0;
    hdet_data->device = device;
    hdet_data->last_hdet_read = 0;
    hdet_data->last_txagc_at_hdet_read = 0;
    hdet_data->exp_hdet_vs_agc_start_offset[RFDEVICE_CDMA_HDET_HPM] = 
      RF_CDMA_EXP_HDET_VS_AGC_DEFAULT_OFFSET_DBX640;
    hdet_data->exp_hdet_vs_agc_start_offset[RFDEVICE_CDMA_HDET_LPM] = 
      RF_CDMA_EXP_HDET_VS_AGC_DEFAULT_OFFSET_DBX640;
    hdet_data->logical_device = logical_dev;
    hdet_data->hdet_read_validity_meter = 0;
    hdet_data->hdet_mod_range_hi = 0;
    hdet_data->hdet_mod_range_lo = 0;

    /* Enable F3s messages for debug. Should be disabled in mainline */
    hdet_data->debugging_enabled = FALSE;
    
    /* Define timer to schedule periodic HDET reads */
    timer_def( &hdet_data->hdet_read_timer,
               NULL, NULL, 0,
               rf_cdma_hdet_read_cb, 
               (timer_cb_data_type)hdet_data);

    /* Define timer to poll MDSP SMEM for HDET trigger status */
    timer_def( &hdet_data->hdet_mdsp_polling_timer,
               NULL, NULL, 0,
               rf_cdma_hdet_mdsp_polling_cb, 
               (timer_cb_data_type)hdet_data);    

  } /* if ( logical_dev->tx_device != NULL ) */
  else
  {
    /* TX Device is NULL, so HDET obviously cannot be supported on device */
    RF_MSG_1( RF_MED, "rf_cdma_hdet_init: HDET not supported on device %d ",
              device );
  }

  /* Initialization complete - set init_done flag. This should be the last
  parameter to be initialized */
  hdet_data->init_done = TRUE;

  RF_MSG_1( RF_MED, "rf_cdma_hdet_init: HDET initialized for device %d",device);

  return init_status;

} /* rf_cdma_hdet_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  API to query device driver for HDEt scripts and populate shared mem with it
 
  @details
  This function will query the device driver for HDET scripts needed for PDET 
  ON/HKADC conversion/PDET OFF, and populate a generic buffer interface with it
  Once the script is obtained, the information is stored in FW shared memory,
  every time an HDET read is requested, FW will directly read the contents of 
  this shared memory
 
  @param mode
  Technology or RFM mode (1x or HDR). Decides which MDSP interface API is 
  called by this generic API, also if required, can be used to get 1x/HDR
  specific timing informaiton for the devices
 
  @param *tx_device
  Pointer to Tx device for which Tx timing parameter is queried
 
  @return
  Status indicating success/failure of API
*/
static boolean
rf_cdma_hdet_set_hdet_smem
(
  rfm_device_enum_type device,
  rfm_mode_enum_type mode,
  rf_buffer_intf *hdet_settings
)
{
  boolean ret_val = TRUE;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get Logical Device Params */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_set_hdet_smem: "
                      "Null Logical Dev pointer");
    return FALSE;
  }

  /* Check if the HDET settings buffer is not NULL */
  if ( hdet_settings != NULL )
  {
    rfcommon_mdsp_event_handle** hdet_script_event_handle =
                  rf_cdma_data_get_event_handle( device, 
                                                 RF_CDMA_EVENT_HDET_CONFIG );
    /* Check for NULL event handle*/
    if ( hdet_script_event_handle == NULL )
    {
      RF_MSG( RF_ERROR, "rf_cdma_hdet_set_hdet_smem: "
              "Null HDET event handle");
      ret_val =  FALSE;
    }
    else
    {
      /* Push Analog frontend Script to MDSP */
      ret_val &= rf_cdma_mdsp_configure_devices( device ,
                                                 logical_dev->tx_mdsp_path,
                                                 mode,
                                                 RF_CDMA_MDSP_CCS_EVENT_RF_HDET_CONFIG_ID,
                                                 hdet_script_event_handle,
                                                 (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RF_HDET_CONFIG_ID)) ,
                                                 hdet_settings, 
                                                 0, /*zero, because this is a 
                                                      dynamic event and does not 
                                                      require a script index*/ 
                                                 rf_cdma_data_get_txlm_handle (device) ,
                                                 NULL, NULL );
    }/*if ! ( hdet_script_event_handle == NULL )*/
  }/*if ( hdet_settings != NULL )*/
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_set_hdet_smem: "
                      "HDET settings buffer is NUL!");
    ret_val = FALSE;

  }/*if ( hdet_settings == NULL )*/

  if ( ret_val == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_set_hdet_smem: "
                      "API failed!");
  }

  return ret_val;

} /* rf_cdma_hdet_set_hdet_smem */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Start HDET operation and state machine for the device 
 
  @details  
  This API is called when TX is enabled. Extracts HDET related NV for the
  current band/chan ensuring those required for basic operation are valid.

  If all NVs and data are valid, HDET is configured to start running in 
  acquisition state. 

  Initializes call back timer so that rf_cdma_hdet_update_max_pwr() is called  
  periodically. 
    
  @param device
  Logical device ( Used here to get CCS event handle )
 
  @param hdet_data  
  Pointer to the data structure that contains HDET info for a given device

  @param band
  Current band of operation. Needed to obtain correct HDET vs TX AGC and
  LIM vs FREQ table from NV

  @param chan
  Current channel of operation. Needed to compute Frequency error in HDET
  reading based on Lim vs Freq NV
 
  @return  
  TRUE on successful enabling of HDET loop. FALSE implies HDET loop could not
  be enabled thus HDET reads based max power limiting will not be performed.
  This does not imply TX is unoperational only that TX power at max limits
  is not guaranteed.
*/
boolean
rf_cdma_hdet_start
(
  rfm_device_enum_type device,
  rf_cdma_hdet_data_type* hdet_data,
  rfm_mode_enum_type mode,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
)
{
  boolean status = TRUE;
  /* Tx Freq, required for HDET init, initialize to zero*/
  uint32 tx_freq = 0;
  /* Pointer to generic script, to hold the HDET settings*/
  rf_buffer_intf *hdet_settings = NULL;
  int16 curr_max_pwr_lim;

  if ( rf_cdma_debug_flags.disable_hdet == TRUE )
  {
    RF_MSG( RF_LOW, "rf_cdma_hdet_start(): HDET algorithm not enabled " );
    return TRUE;
  }

  if ( (hdet_data == NULL) ||
       (logical_dev == NULL) ||
       (hdet_data->init_done == FALSE) )
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_hdet_start(): Unexpected NULL pointer or uninitialized"
            " data structure.  Aborting." );
    return FALSE;
  }

  /* Query HDET device to see if HDET is supported and get operating
     parameters. */
  status &= rfdevice_hdet_cdma_get_hdet_param( logical_dev->hdet_obj[band],
                                               &hdet_data->device_param );

  if ( (status != TRUE) ||
       (hdet_data->device_param.hdet_supported == FALSE) ||
       ((mode != RFM_1X_MODE) && (mode != RFM_1XEVDO_MODE)) )
  {
    RF_MSG_1( RF_ERROR, "Trying to start HDET on uninitialized or unsupported"
                        " device, rf mode %d", mode );
    return FALSE;
  }

  if ( rfm_get_calibration_state() == TRUE )
  {
    RF_MSG( RF_LOW, "Not starting HDET loop in calibration mode" );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &(hdet_data->crit_section) );

  if ( hdet_data->state == RF_CDMA_HDET_INACTIVE )
  {
    /* Extract HDET NV data for given band, chan. This function
    will extract and precompute compensation data dependent on band/chan as
    well as HDET power mode and store in hdet_data. This will be used for
    all subsequent HDET reads and TX AGC estimation for this band/chan. */
    status &= rf_cdma_hdet_extract_nv_data( logical_dev,
                                            hdet_data,
                                            band,
                                            chan,
                                            RFDEVICE_CDMA_HDET_HPM );
    if ( status == TRUE )
    {
      /* Load LPM HDET NV only if HPM is active and valid, else
      don't run HDET */
      rf_cdma_hdet_extract_nv_data( logical_dev,
                                    hdet_data,
                                    band,
                                    chan,
                                    RFDEVICE_CDMA_HDET_LPM );
    }

    /* Get Tx freq using current band and channel*/
    tx_freq = rf_cdma_get_tx_carrier_freq( band, 
                                           chan );

    /* Initializes HDET circuitry in CDMA mode */
    status &= rfdevice_hdet_cdma_init ( logical_dev->hdet_obj[band] , 
                                        NULL, RFDEVICE_EXECUTE_IMMEDIATE, 
                                        tx_freq );

    /* Create generic buffer interface object for SSBI/RFFE/GRFC,
       allocate memory to max script sizes, since the memory is released when 
       object is destroyed later on anyway */
    hdet_settings = rf_buffer_create ( RF_CDMA_MAX_SSBI_SCRIPT_SIZE , 
                                       RF_CDMA_MAX_RFFE_SCRIPT_SIZE , 
                                       RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;
    /* NUll pointer check */
    if ( hdet_settings!= NULL )
    {
      /* Get scripts from device needed by MDSP to turn PDET ON,
      trigger HKADC conversion and turn PDET OFF, with timing information */
      status &= rfdevice_hdet_cdma_get_fw_trigger_scripts( 
                                  logical_dev->hdet_obj[band], hdet_settings );
      
      /* API to populate FW shared mem , the FW shared mem needs to be 
      populated only once with the script informaiton, every time there is a 
      trigger for HDET, FW can pick up contents of this location*/ 
      status &= rf_cdma_hdet_set_hdet_smem( device,
                                            mode,
                                            hdet_settings );

      /* Destroy the buffer once used*/
      rf_buffer_destroy( hdet_settings );

    }/*if ( hdet_settings!= NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_hdet_start: "
                        "Unable to create buffer object ");
      status &= FALSE;
    }

    /* Schedule timer and start HDET loop in acquisition state only if
    NV extraction and MDSP read was successful. 

    rf_cdma_hdet_extract_nv_data()
    returns failure only if Exp HDET vs AGC table is invalid (non-monotonic or 
    all zeros) or NV table pointers are NULL. In these cases, it is impossible 
    to do any HDET to TX AGC conversion and hence any closed loop power limiting. 

    If MDSP HDET read was not successful we cannot do HDET reads at all.

    If we cannot retrieve SSBI scripts from device to give to MDSP, we cannot
    do HDET reads at all.

    Hence we return failure without scheduling hdet reads */

    if ( status == TRUE )
    {
      /* Initialize logical_dev pointer in HDET data for future use in 
      callbacks */
      hdet_data->logical_device = logical_dev;
    
      /* Set technology (rfm_mode) of operation */
      hdet_data->mode = mode;
      
      /* Always start in Acq state */
      hdet_data->state = RF_CDMA_HDET_ACQUISITION;
      
      /* Reset state_count at start of HDET loop */
      hdet_data->state_count = 0;
    
      /* Store current band as it will be needed for logging purposes later 
      (TC needed to convert raw txagc to dbm is band dependent) */
      hdet_data->band = band;

      /* Reset filtered tx error as HDET is re-started */
      hdet_data->filtered_tx_err = 0;

      /* Reset accumulated tx error as HDET is re-started */
      hdet_data->accum_lin_tx_err = 0;

      /* Reset TX estimate error as HDET is re-started */
      hdet_data->tx_estimate_err = 0;

      /* Reset last HDET read as HDET is re-started */
      hdet_data->last_hdet_read = 0;

      /* Reset AGC at last HDET read on re-start */
      hdet_data->last_txagc_at_hdet_read = 0;
    
      /* Reset HDET read validity meter on start */    
      hdet_data->hdet_read_validity_meter = 0;

      /* Schedule timer at acq polling period */
      timer_set( &hdet_data->hdet_read_timer, 
                 hdet_data->device_param.acq_read_period, 
                 0,
                 T_MSEC);

	  if ( hdet_data->enabled[RFDEVICE_CDMA_HDET_LPM] == TRUE )
	  {
	    /* Obtain the current max power limit */
		curr_max_pwr_lim = rf_cdma_plim_get_curr_max_power_limit( 
		hdet_data->device,
		hdet_data->mode );

		if ( curr_max_pwr_lim < hdet_data->lpm_threshold_val )
		{
		  /* Transition to or stay in low power mode */
		  rf_cdma_hdet_update_power_mode ( hdet_data, RFDEVICE_CDMA_HDET_LPM );

		  if ( hdet_data->debugging_enabled )
		  {
			RF_MSG_3( RF_MED, "CDMA HDET: Max power Lim %d(dBm10) lower than "
			 			"LPM threshold %d(dBm10). Using PM %d",
						curr_max_pwr_lim,
						hdet_data->lpm_threshold_val,
						hdet_data->power_mode );
		  }
		}
		else
		{
		  /* Transition to or stay in high power mode */
		  rf_cdma_hdet_update_power_mode ( hdet_data, RFDEVICE_CDMA_HDET_HPM );

		  if ( hdet_data->debugging_enabled )
		  {
			RF_MSG_3( RF_MED, "CDMA HDET: Max power Lim %d(dBm10) higher than "
						"LPM threshold %d(dBm10). Using PM %d",
						curr_max_pwr_lim,
						hdet_data->lpm_threshold_val,
						hdet_data->power_mode );
		  }
		}
	  }
	  else
	  {
	    /* Transition to or stay in high power mode */
		rf_cdma_hdet_update_power_mode ( hdet_data, RFDEVICE_CDMA_HDET_HPM );
	  }
	  
      RF_MSG_1( RF_MED, "rf_cdma_hdet_start: HDET operation started for "
                "device %d", hdet_data->device );

    } /* if ( status == TRUE ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_hdet_start: HDET failed to start for "
                "device %d", hdet_data->device );
    }
    
  } /*if ( hdet_data->state == RF_CDMA_HDET_INACTIVE ) */
  else
  {
    RF_MSG_2( RF_HIGH, "CDMA HDET: HDET cannot be started on device %d in "
                       "state %d. stop hdet first ",
              hdet_data->device,
              hdet_data->state );
    status = FALSE;
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &(hdet_data->crit_section) );
  
  return status;

} /* rf_cdma_hdet_start */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  API to stop the HDET operation for given device 
   
  @param device 
  RFM device 
   
  @param hdet_data  
  Pointer to the data structure that contains HDET info for a given device
 
  @return  
  TRUE on success and FALSE on failure 
*/
boolean
rf_cdma_hdet_stop
( 
  rfm_device_enum_type device,
  rf_cdma_hdet_data_type* hdet_data
)
{
  if ( rf_cdma_debug_flags.disable_hdet == TRUE )
  {
    RF_MSG( RF_LOW, "rf_cdma_hdet_stop(): HDET algorithm not enabled " );
    return TRUE;
  }
  /* enter critical Section */
  rf_common_enter_critical_section( &(hdet_data->crit_section) );

  /* Helper function to reset all timers and state variables */
  rf_cdma_hdet_clear_timers_reset_state( hdet_data );

  /* Always clean up HDET trig event smem and event handle */
  rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_HDET_CONFIG );

  /* Leave critical Section */
  rf_common_leave_critical_section( &(hdet_data->crit_section) );

  RF_MSG_1( RF_MED, "rf_cdma_hdet_stop: HDET operation stopped for device %d,",
            device);

  return TRUE;
} /* rf_cdma_hdet_stop */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Gets a recent HDET reading from device and TX power corresponding to the
  HDET read

  @details
  If HDET loop is currently running (hdet_data->state != RF_CDMA_HDET_INACTIVE),
  this function will not do a fresh HDET read to avoid risk of conflicting with
  MDSP triggered HDET reads. It will just return the most recent HDET read done
  by HDET algorithm stored in hdet_data.

  If HDET loop is not running, there is no risk of conflict and hence a fresh 
  HDET read will be done from device.
  
  @param *hdet_data
  "this" pointer to hdet data of device whose hdet must be read

  @param mode
  Current mode of operation - 1x or HDR. Helps determine which fw image should
  be queried for current tx total power if HDET is inactive. If HDET is active,
  this argument is unused as HDET is already operating on a specific mode.

  @param band
  Current band. Needed to get Turnaround constant to convert TX agc from fw to
  dbm10. If HDET is active, this argument is unused. HDET data has its own 
  internal "band" of operation which is used.

  @param *hdet_reading
  Current hdet reading - 16bit unsigned int. This is a return value. It may be
  an actual HDET read from device or the last valid read depending on whether
  HDET loop is running

  @param *txpwr_at_read_dbm10
  TX power in dbm x 10 at time of HDET read. If thru MDSP, it is the value 
  reported by MDSP when read was triggered. In this case, it exactly alligns
  with the HDET read. If a fresh read was done, MDSP is queried independently
  for the current TX total. In this case, the tx power will not correspond 
  precisely to the instance HDET read was made.
  
  @retval TRUE
  Valid hdet read and tx power populated in pointer arguments.

  @retval FALSE
  HDET read could not be performed. No valid data populated in pointer args. 
  The various failure scenarios are:
   - Invalid hdet_data ptr arg
   - Device does not support HDET
   - HDET loop inactive and HDET read from device failed (as indicated by 
     hdet_quality flag from device)
   - HDET loop active, but no valid HDET read has been by device so far 
*/
boolean
rf_cdma_read_hdet
(
  rf_cdma_hdet_data_type* hdet_data,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  uint16* hdet_reading,
  int16* txpwr_at_read_dbm10
)
{
  boolean status = TRUE;
  rfdevice_cdma_hdet_read_type hdet_result;
  rfdevice_cdma_hdet_param_type local_hdet_param;
  /* Device Status Read Pointer */
  const rf_cdma_data_status_type *dev_status_r;

  hdet_result.hdet_quality = FALSE;

  if ( hdet_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_read_hdet: Null hdet_data ptr!" );
    return FALSE;
  }

  if ( band >= RFM_CDMA_MAX_BAND )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_read_hdet: Invalid band %d", band );
    return FALSE;
  }

  if ( hdet_data->logical_device == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_read_hdet: Logical dev info not available" );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &(hdet_data->crit_section) );

  if ( hdet_data->state == RF_CDMA_HDET_INACTIVE )
  {
    /* HDET currently inactive. We can do a fresh HDET read from
    device without any concern of conflicts with normal HDET loop
    operation. This would be the case in calibration mode */

    /* If HDET supported flag is not set, query device driver again.
    If HDET is not yet started, this flag would not be updated */
    if ( hdet_data->device_param.hdet_supported == FALSE )
    {
      rfdevice_hdet_cdma_get_hdet_param( hdet_data->logical_device->hdet_obj[band],
                                         &local_hdet_param );
    }
    else
    {
      local_hdet_param.hdet_supported = TRUE;
    }

    if ( local_hdet_param.hdet_supported == TRUE )
    {
      rfdevice_hdet_cdma_read_hdet( hdet_data->logical_device->hdet_obj[band],
                                    FALSE, /* not a MDSP triggered read */
                                    &hdet_result );

      if ( hdet_result.hdet_quality == TRUE )
      {
        dev_status_r = rf_cdma_get_device_status( hdet_data->device );

        if ( dev_status_r != NULL )
        {
        *txpwr_at_read_dbm10 = 
          rf_cdma_convert_txagc( RFM_CDMA_AGC_FORMAT__DBM10,
                                 rf_cdma_get_turnaround_constant(band), 
                                   rf_cdma_mdsp_get_tx_total_power(
                                   hdet_data->logical_device->tx_mdsp_path,
                                   dev_status_r->txlm_handle,
                                  mode) );
        }
        else
        {
          RF_MSG_1( RF_ERROR,"rf_cdma_read_hdet: NULL data for device %d",
                             hdet_data->device );
        }
        *hdet_reading = hdet_result.hdet_reading;
      } /* if ( hdet_result.hdet_quality == TRUE ) */
      else
      {
        /* HDET read failed */
        status = FALSE;
        RF_MSG_1( RF_ERROR, "rf_cdma_read_hdet: Dev %d - Bad Read", 
                  hdet_data->device );
      }
    } /* if ( hdet_data->device_param.hdet_supported == TRUE ) */
    else
    {
      /* HDET not supported on this device */
      status = FALSE;
      RF_MSG_1( RF_HIGH, "rf_cdma_read_hdet: Dev %d - HDET Not supported", 
                hdet_data->device );
    }
  } /* if ( hdet_data->state == RF_CDMA_HDET_INACTIVE ) */
  else
  {
    /* Not in calibration mode and HDET is active. No need to do a fresh HDET 
    read. Just return the last valid read and tx power */
    *hdet_reading = hdet_data->last_hdet_read;
    *txpwr_at_read_dbm10 = 
      rf_cdma_convert_txagc( RFM_CDMA_AGC_FORMAT__DBM10,
                             rf_cdma_get_turnaround_constant(hdet_data->band),
                             hdet_data->last_txagc_at_hdet_read );
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &(hdet_data->crit_section) );

  return status;

} /* rf_cdma_read_hdet */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface API to fetch the current read validity list

  @details
  This API will provide the current validity of the HDET readings to any module
  outside of HDET. The function looks for at least one valid HDET reading 
  in the recent HDET "valid" readings window.

  @param hdet_data
  Pointer to HDET data structure

  @return
  TRUE if there was at least one valid reading in the read window, else FALSE.
*/
boolean
rf_cdma_hdet_get_recent_read_validity
(
  rf_cdma_hdet_data_type* hdet_data
)
{
  boolean reads_valid = FALSE;
  uint8 validity_mask;

  if ( hdet_data != NULL )
  {
    /* Check the number of lower bits equal to the validity
    window size */
    validity_mask = (1<<RF_CDMA_HDET_READ_VALIDITY_WINDOW)-1;
    reads_valid = ( hdet_data->hdet_read_validity_meter &
                    validity_mask );

    RF_MSG_3( RF_HIGH, "rf_cdma_hdet_get_recent_read_validity: "
                       "Current validity meter %d, validity mask %d, "
                       "Reads valid in window %d",
              hdet_data->hdet_read_validity_meter,
              validity_mask,
              reads_valid );
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_get_recent_read_validity: "
                      "NULL HDET data" );
  }

  return (reads_valid > 0);

} /* rf_cdma_hdet_get_recent_read_validity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates the temperature parameters corresponding to the HDET module
  
  @details
  This API uses the last valid temperature value from temperature compensation
  in order to update HDET_vs_Temp and HDET Mod range data, based on NV. 
  HDET_vs_Temp is used during Exp_HDET_vs_AGC conversion and HDET Mod Range 
  data is used by the EPT HDET algorithm for bounding HDET correction.

  @param *hdet_data
  Pointer to HDET data

  @param last_temp_sensor_read
  Last valid temperature read from temp comp 
*/
void
rf_cdma_hdet_update_temperature_params
(
  rf_cdma_hdet_data_type* hdet_data,
  uint16 last_temp_sensor_read
)
{
  /* Common Tx Static NV pointer */
  const rf_cdma_tx_static_type* tx_static;
  /* Common Tx Cal NV pointer */
  const rf_cdma_tx_cal_type* tx_cal;

  if ( rf_cdma_debug_flags.disable_hdet == TRUE )
  {
    RF_MSG( RF_LOW, "rf_cdma_hdet_update_temperature_params(): "
                    "HDET algorithm not enabled " );
    return ;
  }

  /* Check for NULL pointer */
  if ( hdet_data != NULL && hdet_data->logical_device != NULL && 
       hdet_data->band < RFM_CDMA_MAX_BAND )
  {
    if ( (rfm_get_calibration_state() == FALSE) &&
         (hdet_data->state != RF_CDMA_HDET_INACTIVE) )
    {
      tx_static = rf_cdma_nv_get_tx_static( 
                         hdet_data->logical_device->tx_nv_path[hdet_data->band],
                         hdet_data->band );
      tx_cal = rf_cdma_nv_get_tx_cal( 
                         hdet_data->logical_device->tx_nv_path[hdet_data->band],
                         hdet_data->band );
  
      if ( tx_static != NULL && tx_cal != NULL )
      {
        if ( tx_cal->therm_cal_data.is_therm_cal_nv_data_valid ) 
        {
        hdet_data->hdet_vs_temp = rf_lininterp_u16_s8( 
                                              tx_cal->therm_cal_data.tx_therm_cal,
                                            tx_static->hdet_vs_temp,
                                            RF_CDMA_NV_TEMP_LIMIT_SIZE,
                                            last_temp_sensor_read );
  
  
        /* Store the Hi and Lo thresholds in dB640 */
        hdet_data->hdet_mod_range_hi = 
              ( rf_lininterp_u16_s16( 
                      tx_cal->therm_cal_data.tx_therm_cal,
                    tx_static->hdet_mod_range[RF_CDMA_NV_HDET_MOD_RANGE_HI],
                    RF_CDMA_NV_TEMP_LIMIT_SIZE,
                    last_temp_sensor_read ) * -64 );
  
        hdet_data->hdet_mod_range_lo = 
              ( rf_lininterp_u16_s16( 
                      tx_cal->therm_cal_data.tx_therm_cal,
                    tx_static->hdet_mod_range[RF_CDMA_NV_HDET_MOD_RANGE_LO],
                    RF_CDMA_NV_TEMP_LIMIT_SIZE,
                    last_temp_sensor_read ) * -64 );
  
        RF_MSG_1( RF_MED, "rf_cdma_hdet_update_temperature_params: "
                          "HDET vs Temp val = %d",
                  hdet_data->hdet_vs_temp );
        RF_MSG_2( RF_MED, "rf_cdma_hdet_update_temperature_params: "
                          "HDET Mod Range Hi = %d (dB10), "
                          "HDET Mod Range Lo = %d (dB10)",
                  rf_divide_with_rounding(hdet_data->hdet_mod_range_hi,-64),
                  rf_divide_with_rounding(hdet_data->hdet_mod_range_lo,-64) );
      }
        else
        {
          /* Invalid therm cal case */
          hdet_data->hdet_vs_temp = 0 ;
          RF_MSG ( RF_HIGH , "rf_cdma_hdet_update_temperature_params(): " 
                   "Therm Cal NV is marked invalid " ) ;
        } /* ( tx_cal->therm_cal_data.is_therm_cal_nv_data_valid)  */
      } /* ( tx_static != NULL && tx_cal != NULL ) */
    } /* ( rfm_get_calibration_state() == FALSE ) */
  } /*  ( hdet_data != NULL && hdet_data->logical_device != NULL ) */
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_update_temperature_params: NULL "
                      "HDET pointer or invalid band" );
  }

  return;

}/* rf_cdma_hdet_update_temperature_params */

/*! @} */

/*============================================================================*/
/*!
  @name Debug

  @brief
  Functions used for HDET debugging
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Configure HDET debug Mode
 
  @details
  This function will set the HDET variables to debug mode,
 
  @param ic_data
  Pointer to HDET data for which debug mode is to be se
 
  @param debug_en
  Flag indicating, if the debug mode is to be enabled or not.
 
  @return
  Status indicating, if the debug mode is enabled or not.
*/
boolean
rf_cdma_hdet_configure_debug_mode
(
  rf_cdma_hdet_data_type* hdet_data,
  boolean debug_en
)
{
  boolean ret_val = FALSE;

  if ( hdet_data != NULL )
  {
    hdet_data->debugging_enabled = debug_en;
    RF_MSG_2( RF_LOW, "rf_cdma_hdet_configure_debug_mode: Dev %d - Debug "
              "Mode %d", hdet_data->device, debug_en );
    ret_val = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_hdet_configure_debug_mode: Failed to "
            "Configure debug mode" );
    ret_val = FALSE;
  }

  return ret_val;

} /* rf_cdma_hdet_configure_debug_mode */
#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */

