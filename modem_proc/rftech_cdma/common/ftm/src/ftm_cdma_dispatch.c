/*!
  @file
  ftm_cdma_dispatch.c

  @brief
  This module contains all the 1xHDR FTM dispatches.

  @addtogroup RF_FTM_COMMON_DISPATCH
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

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/src/ftm_cdma_dispatch.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/08/15   rs      KW error fix
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
09/24/14   spa     Skip call to ftm_rfmode_exit in cdma non signaling calls
10/02/14   pk      Added FTM support for agc reload
09/29/14   zhw     Compiler warning fix
09/23/14   pk      compiler warning fix
08/20/14   ndb     Add Tuner_ID as part of the response in 
                   "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
07/15/14   pk      Compiler warning Fix
07/08/14   pk      Implementation of HDR FTM module
06/18/14   JJ      Added support to reduce Cal time
04/08/14   JJ      Add a new FTM command for timed sleep rx
03/20/14   spa     Added support for FTM_CDMA_FORCE_RX_ONOFF 
11/25/13   spa     Use int32 math for LNA offset calculation 
09/06/13   ars     Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
08/27/13   aka     Added support for 1x self-test commands 
08/21/13   dj      Added FTM_INT_CALV3_FEEDBACK_SETUP command
05/14/13   cc      Added FTM 1x IQ capture support 
03/14/13   aro     Deprecated Composite Calibration and TxSweep Commands
02/04/13   ndb/bmg Added FTM_SET_TUNER_TUNE_CODE_OVERRIDE support 
01/21/13   nrk     Changed input param type path to device for multi synth functions
12/13/12   hdz     Fixed potential issue in ftm_cdma_dispatch()
12/04/12   zhw     APT PA Q Current support
11/28/12   nrk     Added cleanup of RF FTM state before entering CDMA NS
11/26/12   nrk     Added GET MUTLI SYNTH and RESET MULTI SYNTH Command
11/14/12   cri     Move radio setup request to common 
11/07/12   aro     Migrated Radio Setup operation to FTM Task
08/16/12   ars/hdz Modifed FTM frequency adjust logic to apply correction using
                   XO manager API
07/24/12   hdz     Added FTM_SET_TX_PWR_LIMIT in ftm_cdma_dispatch()
06/01/12   sbo     Added missing break statement.
03/18/12   aro     Moved common radio commands to common module
03/08/12   hdz     Added FTM_SET_FREQ_ADJUST in ftm_cdma_dispatch()
01/30/12   sty     update command response before creating the rsp pkt
12/29/11   ars     Returning raw Rx_AGC value in 10-bit format instead of 
                   16-bit format.
12/21/11   aro     Added 1x Enable and disable radio functions
12/21/11   aro     Added HDR Enable and disable radio functions
12/02/11   sty     Implemented FTM_1X_RX_LEVEL_REQ in ftm_cdma_mc_dispatch() 
11/22/11   aro     Added interface to disable iC for debug purpose
11/12/11   aro     Moved CDMA common commands from 1x to CDMA module
11/12/11   aro     Added CDMA driver verification sandbox function
11/12/11   aro     Added command to configure debug mode for CDMA
11/12/11   aro     Added radio command to get current ic power mode
11/12/11   aro     Added radio command to clear ic power mode override
11/12/11   aro     Added CDMA PM override function
10/27/11   ars     Fixing KW Errors
10/17/11   ars     Added FTM_RF_GET_RX_LEVEL_DBM command
10/17/11   ars     Added FTM_SET_TX_POWER_DBM FTM Layer
10/14/11   aro     Updated Get RxAGC interface to return RxAGC in 1/10th dB
10/13/11   bmg     FTM RFM interface support to set SAR state.
10/13/11   aro     Added interface to enable and disable various debug mode for
                   various debug modules
09/16/11   Saul    HDR non-sig support.
09/09/11   aro     Renamed CDMA logging functions to reflect primary and
                   secondary chains
09/01/11   cd      Added interface to program APT bias val in FTm
08/29/11   aro     Created a Driver verification sandbox
08/12/11   aro     Added support for HDR LNA override clear
08/11/11   aro     Added support to query synth status
08/11/11   aro     Added support to second chain test call
08/11/11   aro     Moved Cal v3 configure rx to be done under ISR context
08/08/11   aro     Added commands to test calv3 sequence and calv3 config rx
08/03/11   aro     Added support for radio tear down for cal v3
08/03/11   aro     Added support for retune for cal v3
08/03/11   aro     Added support for FTM command to set OL Tx Power
07/30/11   aro     F3 Message Update
07/27/11   aro     Added CDMA Cal v3 commands to config, retune and tear
                   down radio
07/26/11   aro     Added HDET settling time query to RF Device
07/21/11   aro     Enabled ADC read command
07/12/11   aro     Added support to query intelliceiver power mode
07/01/11   aro     Intelliceiver Calibration Node Added
07/01/11   aro     Added Debug interface to override Power Mode
06/29/11   aro     Renamed intelliceiver function name
06/16/11   sar     Including internal 1x interface.
06/14/11   aro     F3 Message Update
06/13/11   sty     Added implementation for FTM_HDR_RADIO_DISABLE_DIV
06/13/11   aro     Added internal command to Set Mode, Tx On, and Set
                   Lowest Tx Pwr
06/13/11   aro     Added 1x Tx Override command
06/03/11   aro     Fixed LNA Override response packet
06/02/11   sty     Deleted Tx workaounds
06/01/11   aro     Fixed 1/10th dB conversion for DVGA offset
05/31/11   aro     Removed hardcoding of Set Mode for Device 1
05/27/11   aro     [1] Moved SV debug flag to main dispatch
                   [2] Removed the hardcoding of Dev0 from TX_enable
05/27/11   aro     Added SV Chain Calibration temp support
05/25/11   aro     Added debug code to enable SV bringup
05/16/11   aro     Removed bit shift operation to mathematical operation
05/16/11   aro     Added proper unit conversion for expected RxAGC and Offset values
05/06/11   aro     Added 1x disable diversity radio control
05/06/11   aro     Added 1x enable diversity radio control
05/05/11   aro     Removed Linearizer Sweep
05/05/11   sty     Added case for FTM_HDR_RADIO_ENABLE_DIV
05/02/11   sty     reverted previous changes as the APIs are needed for cal
04/29/11   sar     Merged 1X API cleanup changes.
04/27/11   sty     Added checks for cal_state in FTM_SET_MODE and FTM_SET_TX_ON
04/22/11   cd      KW fixes
04/14/11   sty     Renamed ftm_1x_set_diversity to ftm_1x_set_sec_chain_mode()
04/07/11   aro     Renamed HDET Read function
04/07/11   aro     Hooked up TxLinearizer Sweep to 1x Dispatch
04/04/11   aro     Added support for 1xNS
03/29/11   aro     Compiler Warning Fix
03/29/11   aro     Added commands to disable Tx and LogCommit for COmpCal under
                   FTM Task Context
03/29/11   sty     Use ftm_1x_disable_tx() to disable Tx
03/28/11   aro     Added Internal SET CHAN command for Composite Cal usage
03/29/11   sty     Added implementation for FTM_SET_SECONDARY_CHAIN
03/14/11   aro     Merged Set DVGA/LNA Offset to a unified interface
03/07/11   aro     Added support for HDR retune control from FTM
02/25/11   aro     Compiler Warning Fix
02/24/11   sty     Added implementation for FTM_GET_HDET_FROM_TX_SWEEP_CAL and
                   FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL
02/23/11   sty     Added implementation - FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2
02/23/11   aro     Compiler warning fix
02/22/11   sty     Added support for FTM_TX_SWEEP_CAL
02/18/11   sty     Changed API calls for FTM_SET_CDMA_CW_WAVEFORM and 
                   FTM_SET_TX_WAVEFORM
02/17/11   aro     Added FTM_RFM_C request packet definition documentation
02/17/11   sty     Fixed compiler warning
02/17/11   sty     Added support for FTM_SET_TX_SWEEP_STEP_SIZE, 
                   FTM_CONFIGURE_TX_SWEEP_CAL and FTM_SET_PA_STATE
02/17/11   aro     Moved LNA Gain Step definition to RFM
02/16/11   sty     Implemented FTM_SET_CDMA_CW_WAVEFORM, FTM_SET_TX_WAVEFORM, 
                   FTM_SET_PDM and FTM_SET_PA_RANGE
02/15/11   aro     Moved set DVGA and LNA offset interface to MDSPs
02/14/11   aro     Added helper functions to Clean up Dispatch
02/14/11   sty     Removed typecast of return for ftm_1x_set_tx()
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     [1] Added support for Get 1x and HDR TxAGC
                   [2] Moved the Get RxAGC Dispatch
02/12/11   aro     [1] Deleted Get LNA state interface to merge with Get RxAGC
                   [2] Radio Dispatch cleanup
02/09/11   sty     Fixed support for FTM_SET_TX_ON and FTM_SET_TX_OFF
02/09/11   sty     Added support for FTM_SET_TX_ON and FTM_SET_TX_OFF
02/08/11   aro     Added F3 Messages
02/08/11   aro     Moved HDR Radio Dispatches from HWTC to FTM
02/08/11   aro     Moved CDMA Radio Dispatch from HWTC to FTM
02/07/11   aro     Removed Dispatch Command Logging
02/02/11   aro     Added all commands to avoid crash due to Bug in RF 
                   Calibration tree (Tx being turned ON during Serial Rx Cal 
                   when Tx is disabled)
02/02/11   aro     Added F3 Messages
02/02/11   aro     Removed Power Mode Param from Measure DVGA/LNA
02/02/11   aro     Added Set Power Mode Interface
02/02/11   aro     Added support to query RxAGC and set LNA state
02/02/11   aro     Added FTM Command History Logging
02/01/11   aro     Added support for NV Commit
02/01/11   aro     Added LNA Calibration Support
02/01/11   aro     Renamed function name
02/01/11   aro     Renamed function name
02/01/11   aro     Added DVGA Calibrate and Store
01/31/11   aro     Added support to set LNA offset
01/31/11   aro     Added LNA Calibration Support
01/31/11   aro     Added support to set DVGA offset
01/31/11   aro     Added DVGA Calibration Support
01/15/11   aro     [1] Added Set Mode Command Support
                   [2] Doxygen Update
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
01/03/11   aro     Fixed Compiler Warning
12/06/10   aro     Initial Release : Dispatch Support

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#ifdef FEATURE_FACTORY_TESTMODE

#include "rfm_1x.h"
#include "rfm_internal.h"
#include "rfm_1x_ftm.h"
#include "rfm_cdma_ftm.h"
#include "ftm_cdma_dispatch.h"
#include "ftm_cdma_control.h"
#include "ftm_cdma_radio.h"
#include "ftm_cdma_data.h"
#include "ftm_cdma_cmd.h"
#include "ftm_rf_cmd.h"
#include "ftm_1x_control.h"
#include "ftm_1x_radio.h"
#include "ftm_1x_calibration.h"
#include "ftm_1x_log.h"
#include "ftm_hdr_radio.h"
#include "ftm_msg.h"
#include "rfcommon_math.h"
#include "onex_non_sig.h"
#include "rf_cdma_mdsp.h"
#include "ftm_calibration_v3_radio.h"
#include "rfcommon_core.h"
#include "rfm_cdma_ftm.h"
#include "rfm_1x_umid.h"

int16 ftm_1x_tx_power_dbm;

/*============================================================================*/
/*!
  @name CDMA Dispatch helper functions

  @brief
  This section contains all the helper functions associated with CDMA 
  Dispatch.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to perform DVGA measurement

  @details
  This function will call RFM function to perform DVGA Calibration.

  @param device
  RF Device on which DVGA calibration is done

  @param agc_expected_val
  Expected AGC value. This corresponds to the actual AGC value that is fed to 
  phone antenna. This is in 1/10th of dB units.

  @param dvga_offset
  Calibrated DVGA Offset Value
 
  @return
  Type of response packet for the function
*/
static ftm_cdma_rsp_type
ftm_1x_measure_dvga_offset
(
  rfm_device_enum_type device,
  int16 agc_expected_val,
  int16 *dvga_offset
)
{
  boolean ret_val; /* return Value */
  int32 dvga_offset_local;
  /* Convert to 1/640th dB units */
  agc_expected_val = agc_expected_val * 64;

  /* Call RFM function */
  ret_val = rfm_1x_ftm_measure_lna_offset( device, RFM_LNA_GAIN_STATE_0,
                    agc_expected_val, &dvga_offset_local );

 /* Convert to 1/10th dB units */
  *(dvga_offset) = (dvga_offset_local) / 64;

  FTM_MSG_3( FTM_LOW, "Measure DVGA : Device %d, Exp AGC %d, DVGA Offset %d",
             device, agc_expected_val, *dvga_offset );

  /* Return appropriate Response packet*/
  if ( ret_val == TRUE )
  {
    return FTM_CDMA_GOOD_RSP;
  }
  else
  {
    return FTM_CDMA_BAD_RSP;
  }

} /* ftm_1x_measure_dvga_offset */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function Set DVGA Offset

  @details
  This function will call RFM function to set DVGA offset.

  @param device
  RF Device on which DVGA offset is to be set

  @param dvga_offset_val
  DVGA Gain offset value to be set in 1/10th dB units

  @return
  Type of response packet for the function
*/
static ftm_cdma_rsp_type
ftm_1x_set_dvga_offset
(
  rfm_device_enum_type device,
  int16 dvga_offset_val
)
{
  boolean ret_val; /* return Value */

  /* Convert to 1/640th dB units */
  dvga_offset_val = dvga_offset_val * 64;

  /* Call RFM function */
  ret_val = rfm_1x_ftm_set_rx_gain( device, RFM_LNA_GAIN_STATE_0,
                                    dvga_offset_val );

  FTM_MSG_2( FTM_LOW, "Set DVGA Offset : Device %d, offset_val %d", device,
             dvga_offset_val );

  /* Return appropriate Response packet*/
  if ( ret_val == TRUE )
  {
    return FTM_CDMA_GOOD_RSP;
  }
  else
  {
    return FTM_CDMA_BAD_RSP;
  }

} /* rfm_1x_ftm_set_dvga_offset */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to perform LNA measurement

  @details
  This function will call RFM function to perform LNA Calibration.

  @param device
  RF Device on which LNA calibration is done

  @param step_index
  Corresponds to the LNA Step from one gain state (x) to next consecutive gain
  state (x+1). 

  step_index 0 : Gain 0 -> Gain 1
  step_index 1 : Gain 1 -> Gain 2
  step_index 2 : Gain 2 -> Gain 3
  step_index 3 : Gain 3 -> Gain 4

  @param agc_expected_val
  Expected AGC value. This corresponds to the actual AGC value that is fed to 
  phone antenna. This is in 1/10th dB units.

  @param *lna_offset
  Storage for Calibrated LNA Offset Value

  @return
  Type of response packet for the function
*/
static ftm_cdma_rsp_type
ftm_1x_measure_lna_offset
(
  rfm_device_enum_type device,
  rfm_lna_gain_step_type step_index,
  int16 agc_expected_val,
  int16 *lna_offset
)
{
  boolean ret_val; /* return Value */
  int32 lna_offset_local;
  /* Convert to 1/640th dB units for Tools */
  agc_expected_val = agc_expected_val * 64;

  /* Call RFM function */
  ret_val = rfm_1x_ftm_measure_lna_offset( device,
                   (rfm_lna_gain_state_type)(step_index + 1),
                    agc_expected_val, &lna_offset_local );

  /* Convert to 1/10th dB units for Tools */
  *(lna_offset) = (lna_offset_local) / 64;

  FTM_MSG_4( FTM_LOW, "Measure LNA : Device %d, Step %d, Exp AGC %d, "
             "DVGA Offset %d", device, step_index, agc_expected_val,
             *lna_offset );

  /* Return appropriate Response packet*/
  if ( ret_val == TRUE )
  {
    return FTM_CDMA_GOOD_RSP;
  }
  else
  {
    return FTM_CDMA_BAD_RSP;
  }

} /* ftm_1x_measure_lna_offset */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function Set LNA Offset

  @details
  This function will call RFM function to set LNA offset.

  @param device
  RF Device on which DVGA offset is to be set

  @param index
  Index "x" refering to Step Between two consecutive LNA Gain State
  
  @param lna_offset_val
  LNA offset value to be set. This is in 1/10th of dB units.

  @return
  Type of response packet for the function
*/
static ftm_cdma_rsp_type
ftm_1x_set_lna_offset
(
  rfm_device_enum_type device,
  rfm_lna_gain_step_type index,
  int16 lna_offset_val
)
{
  boolean ret_val; /* return Value */

  /* Convert to 1/640th dB units */
  lna_offset_val = lna_offset_val * 64;

  /* Call RFM function : LNA state is incremented by 1 to map it to
  LNA State */
  ret_val = rfm_1x_ftm_set_rx_gain( device,
                                    (rfm_lna_gain_state_type)(index + 1),
                                    lna_offset_val );

  FTM_MSG_3( FTM_LOW, "Set LNA Offset: Device %d, State %d, lna_offset_val %d",
             device, index+1, lna_offset_val );

  /* Return appropriate Response packet*/
  if ( ret_val == TRUE )
  {
    return FTM_CDMA_GOOD_RSP;
  }
  else
  {
    return FTM_CDMA_BAD_RSP;
  }

} /* rfm_1x_ftm_set_lna_offset */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function Read the current 1x Rx AGC.

  @details
  This function calls RFM interface to query RxAGC

  @param device The receive path to query the Rx AGC

  @param rx_agc Container for RxAGC Read

  @return
  Type of response packet for the function
*/
static ftm_cdma_rsp_type
ftm_1x_get_rx_agc
(
  const rfm_device_enum_type device,
  int16 *rx_agc
)
{
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;

  rfm_1x_ftm_get_rx_agc_data( device, &rx_agc_data,
                              RFM_CDMA_AGC_FORMAT__RAW );

  *rx_agc = (rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0])/64;

  FTM_MSG_2( FTM_LOW, "Get 1x RxAGC : Device %d, RxAGC %d", device, *rx_agc );

  return FTM_CDMA_GOOD_RSP;
} /* ftm_1x_get_rx_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to Set LNA decision.

  @details
  This function call RFM function to set LNA state
  
  @param device
  RF Device under consideration

  @param state
  Valid Gain States

  @return
  Type of response packet for the function
*/
static ftm_cdma_rsp_type
ftm_1x_set_rx_gain_state
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type state
)
{
  boolean ret_val; /* return Value */

  /* Call RFM function */
  ret_val = rfm_1x_ftm_set_rx_gain_state( device, TRUE, state );

  FTM_MSG_2( FTM_LOW, "Set 1x Rx Gain State : Device %d, state %d", device,
             state );

  /* Return appropriate Response packet*/
  if ( ret_val == TRUE )
  {
    return FTM_CDMA_GOOD_RSP;
  }
  else
  {
    return FTM_CDMA_BAD_RSP;
  }

} /* ftm_1x_set_rx_gain_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to Set Power Mode

  @details
  This function will call RFM interface to Override the current RF power mode 
  selection. The power mode parameter is RF target dependent.

  @param device
  RF Device whose power mode is to be set

  @param power_mode
  Power Mode value to be set

  @return
  Type of response packet for the function
*/
static ftm_cdma_rsp_type
ftm_cdma_set_power_mode
(
  rfm_device_enum_type device,
  rfm_cdma_power_mode_type power_mode
)
{

  boolean ret_val; /* return Value */

  /* Call RFM function */
  ret_val = rfm_cdma_ftm_power_mode_override( device, power_mode );

  FTM_MSG_2( FTM_LOW, "Set Power Mode : Device %d, power_mode %d", device,
             power_mode );

  /* Return appropriate Response packet*/
  if ( ret_val == TRUE )
  {
    return FTM_CDMA_GOOD_RSP;
  }
  else
  {
    return FTM_CDMA_BAD_RSP;
  }

} /* ftm_cdma_set_power_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to read HDET

  @details
  This function will call RFM function to read HDET value.

  @param device
  RF Device on which HDET is to be read
 
  @return
  Read HDET value
*/
static uint16
ftm_1x_read_hdet
(
  rfm_device_enum_type device
)
{
  uint16 hdet_read;

  /* Enable HDET */
  rfm_cdma_ftm_enable_hdet( device );

  DALSYS_BusyWait(rfm_cdma_ftm_get_hdet_settling_time(device));
  
  /* Call RFM function : LNA state is incremented by 1 to map it to
  LNA State */
  hdet_read = rfm_cdma_ftm_read_hdet( device );

  /* Disable HDET */
  rfm_cdma_ftm_disable_hdet( device );

  FTM_MSG_2( FTM_MED, "ftm_1x_read_hdet: Device %d, HDET %d", device,
             hdet_read );

  /* Return appropriate Response packet*/
  return hdet_read;

} /* hdet_read */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Open Loop Tx Power

  @details
  This function will set the requested Open Loop Tx power to the desired level,
  while setting the closed loop power to zero.

  @param device
  RF Device on which Tx Power is to be set
 
  @param enable_flag
  Flag indicating, if the Tx Power is to be overriden or not
 
  @param power_level
  Power Level to which Tx power is to be set in 1/10 dB units
 
  @return
  Status of set Tx power action
*/
static ftm_cdma_rsp_type
ftm_1x_set_tx_power_dbm
(
  rfm_device_enum_type device,
  boolean enable_flag,
  int16 power_level
)
{
  ftm_cdma_rsp_type ret_val;

  /* Define : Response Variable*/
  int32 rf_status;

  ftm_cdma_data_status_type dev_status;  /* points to ftm device status */

  /* Define : To Extract Useful Data */  
  int16 tx_pwr_mdsp_units;

  if( enable_flag == TRUE )
  {
    /* Get RF Device Status */
    ftm_cdma_data_get_device_status( device, &dev_status );
  
    /* Tx AGC (mdsp units) = -640*(Tx_dBm - K - 63.8)
                   = -64*(Tx_dBm10 - 10*K - 638) */
    tx_pwr_mdsp_units = rf_cdma_mdsp_dbm10_to_txagc(
                               power_level,
                               10*rfm_cdma_ftm_get_tc(rfm_cdma_rf_band_type_to_sys_band_type(dev_status.curr_band)) );

    FTM_MSG_2( FTM_LOW, "ftm_1x_set_tx_power_dbm:  TX_Raw %d, "
               "TxPwr(dBm10) %d", tx_pwr_mdsp_units, power_level );

    /* Call RFM Function to set CL power to Zero */
    rf_status = rfm_1x_ftm_set_tx_pwr( device, RFM_CDMA_TX_PWR_CLOSED_LOOP, 0 );

    /* Call RFM Function to set OL Tx power */
    rf_status = rfm_1x_ftm_set_tx_pwr( device, RFM_CDMA_TX_PWR_OPEN_LOOP, tx_pwr_mdsp_units );

    /* Check if the eveything in the function executed or not. If not send
    back a BAD response packet */
    if ( rf_status == TRUE  )
    {
      ret_val = FTM_CDMA_GOOD_RSP;
      FTM_MSG_3( FTM_LOW, "ftm_1x_set_tx_power_dbm: Device %d, Enable Flag %d, "
                 "TxPwr(dBm10) %d : [Done]", device, enable_flag, power_level );
    }
    else
    {
      ret_val = FTM_CDMA_BAD_RSP;
      FTM_MSG_3( FTM_ERROR, "ftm_1x_set_tx_power_dbm: Device %d, Enable "
                 "Flag %d, TxPwr(dBm10) %d : [Fail]", device, enable_flag, 
                 power_level );
    }

    return ret_val;
  }
  else
  {
      return FTM_CDMA_BAD_RSP;
  }

} /* ftm_1x_set_tx_power_dbm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to get Rx Power Level

  @details
  Returns the Rx level, for a specified receiver, in both Rx AGC and dBm units.

  @param device
  RF Device on which Rx Level will be measured
 
  @param rx_level_agc
  Stores the raw AGC data for the measured Rx_Level
 
  @param rx_level_dbm
  Stores the dbm10 equivalent for the measured Rx_Level
 
  @return
  RxAGC in raw and dBm10 units on success and False on failure
*/
static ftm_cdma_rsp_type
ftm_1x_get_rx_level_dbm
(
  rfm_device_enum_type device,
  int16 *rx_level_agc,
  int16 *rx_level_dbm
)
{
  /* Define : Response Variable*/
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;
  boolean rfm_res; /* result of RFM API call */
  ftm_cdma_rsp_type api_res; /* final result of API */
  
  /* Call RFM Function to query RxAGC Data */
  rfm_res = rfm_1x_ftm_get_rx_agc_data( device, &rx_agc_data, 
                                    RFM_CDMA_AGC_FORMAT__DBM10);

  if ( rfm_res == TRUE )
  {
    /* Update the Response Packet Field */
    /* Returning raw Rx_AGC value in 10-bit format */
    *rx_level_agc = (rx_agc_data.raw_rxagc/64);
    /* safe to assume RFM_CDMA_CARRIER_0 since this is 1x */
    *rx_level_dbm = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0];
    
    FTM_MSG_5( FTM_LOW, "ftm_1x_get_rx_level_dbm: Dev : %d, RxAGC : %d, RxPower_dBm : %d"
               "LNA : %d, LNA Word : %d", device, *rx_level_agc, *rx_level_dbm, 
               rx_agc_data.gain_state, rx_agc_data.gain_word );

    api_res = FTM_CDMA_GOOD_RSP;
  }
  else
  {
    api_res = FTM_CDMA_BAD_RSP;
    FTM_MSG_1 ( FTM_ERROR, "ftm_1x_get_rx_level_dbm: Failed to get RxAGC for "
               "device %d", device );
  }
  
  /* Return Response Packet */
  return api_res ;

} /* ftm_1x_get_rx_level_dbm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to get Rx Power Level for multiple receivers.

  @details
  This API returns the Rx level, for all the requested receiver chains associated
  with one carrier, in both Rx AGC and dBm units. This command is only valid after
  the mode (Technology/Band) is set using FTM_SET_MODE and FTM_SET_CALIBRATION_STATE
  has been used to deactivate calibration state or calibration state has not been 
  activated while the device was powered up. Also, its required to enable all 
  receiver chains and program them to correct LNA state  before requesting RxAGC 
  for a Rx chain (e.g. secondary chain should be enabled using FTM_SET_SECODNARY_CHAIN 
  before using this command).

  @param device
  RF Device on which Rx Level will be measured
 
  @param rx_level_agc
  Stores the raw AGC data for the measured Rx_Level
 
  @param rx_level_dbm
  Stores the dbm10 equivalent for the measured Rx_Level
 
  @return
  RxAGC in raw and dBm10 units on success and False on failure
*/
static ftm_cdma_rsp_type
ftm_1x_get_multiple_rx_level_dbm
(
  rfm_device_enum_type device,
  uint16 select_rx_chain,
  ftm_rsp_pkt_type *rsp_pkt
)
{  
  /* Get Rx Level DBM data */
  int16 ftm_cdma_rx_level_agc[FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER] ={0};
  int16 ftm_cdma_rx_level_dbm10[FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER] = {0};   
  /* Define : Response Variable*/  
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;
  /* result of RFM API call */
  boolean rfm_res; 
  /* final result of API */
  ftm_cdma_rsp_type api_res; 
  /* Local vairable to handle loop counter */
  uint8 i; 
  /* Local variable to maintain count for selected receivers */
  uint8 number_selected_rx_chains = 0; 
  /* Local variable to maintain FTM ErrorCode in the response packet */
  uint16 status = FTM_RX_LEVEL_DBM_TEST_SUCCESS;
  /* Local variable for Secondary Chain device */
  rfm_device_enum_type device_drx;
  /* points to ftm device status */
  ftm_cdma_data_status_type dev_status;  
  /* Number of bytes in the response packet that will hold the result for Rx 
  level measurement */
  uint16 payload_size = 0;  
  /* Get pointer to response packet */
  ftm_get_multiple_rx_level_dbm_rsp_pkt_type *rsp_pkt_paylaod;  

  for (i=0;i<FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER;i++)
  {    
    if ((select_rx_chain >> i) & (uint16)0x1)
    {
      if (status == FTM_RX_LEVEL_DBM_TEST_SUCCESS)
      {
        switch(i)
        {
           case 0:
             rfm_res = rfm_1x_ftm_get_rx_agc_data( device, 
                                                   &rx_agc_data, 
                                                   RFM_CDMA_AGC_FORMAT__DBM10);
             if ( rfm_res == TRUE )
             {
               /* Update raw Rx_AGC value in 10-bit format */
               ftm_cdma_rx_level_agc[number_selected_rx_chains] = (rx_agc_data.raw_rxagc/64);
               /* safe to assume RFM_CDMA_CARRIER_0 since this is 1x */
               ftm_cdma_rx_level_dbm10[number_selected_rx_chains] = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0];

               status = FTM_RX_LEVEL_DBM_TEST_SUCCESS;
               FTM_MSG_5( FTM_LOW, "ftm_1x_get_multiple_rx_level_dbm: Dev : %d, RxAGC : %d, RxPower_dBm : %d"
                          "LNA : %d, LNA Word : %d", device, 
                          ftm_cdma_rx_level_agc[number_selected_rx_chains],
                          ftm_cdma_rx_level_dbm10[number_selected_rx_chains], 
                          rx_agc_data.gain_state, 
                          rx_agc_data.gain_word );
             }
             else
             {
               status = FTM_RX_LEVEL_DBM_TEST_GENERAL_FAILURE;
               FTM_MSG_1 ( FTM_ERROR, "ftm_1x_get_multiple_rx_level_dbm: Failed to get RxAGC for " 
                           "device %d", device );
               ftm_cdma_rx_level_agc[number_selected_rx_chains] = ftm_cdma_rx_level_dbm10[number_selected_rx_chains] = 0;
             }                          
             break;
          case 1:
             /* Get secondary chain device ID that is associated with current primary device */ 
            ftm_cdma_data_get_device_status( device, &dev_status );
            device_drx = dev_status.assoc_dev; 
            ftm_cdma_data_get_device_status( device_drx, &dev_status );
             
             if (dev_status.chain_config == RECEIVE_DIVERSITY)
             {                             
               rfm_res = rfm_1x_ftm_get_rx_agc_data( device_drx, 
                                                     &rx_agc_data, 
                                                     RFM_CDMA_AGC_FORMAT__DBM10);
               if ( rfm_res == TRUE )
               {
                 /* Update raw Rx_AGC value in 10-bit format */
                 ftm_cdma_rx_level_agc[number_selected_rx_chains] = (rx_agc_data.raw_rxagc/64);
                 /* safe to assume RFM_CDMA_CARRIER_0 since this is 1x */
                 ftm_cdma_rx_level_dbm10[number_selected_rx_chains] = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0];

                 status = FTM_RX_LEVEL_DBM_TEST_SUCCESS;
                 FTM_MSG_5( FTM_LOW, "ftm_1x_get_multiple_rx_level_dbm: Dev : %d, RxAGC : %d, RxPower_dBm : %d"
                            "LNA : %d, LNA Word : %d", device_drx, 
                            ftm_cdma_rx_level_agc[number_selected_rx_chains], 
                            ftm_cdma_rx_level_dbm10[number_selected_rx_chains], 
                            rx_agc_data.gain_state, 
                            rx_agc_data.gain_word );
               }
               else
               {
                 status = FTM_RX_LEVEL_DBM_TEST_GENERAL_FAILURE;
                 FTM_MSG_1 ( FTM_ERROR, "ftm_1x_get_multiple_rx_level_dbm: Failed to get RxAGC for " 
                             "device %d", device_drx );
                 ftm_cdma_rx_level_agc[number_selected_rx_chains] = ftm_cdma_rx_level_dbm10[number_selected_rx_chains] = 0;
               }               
             }
             else
             {            
               status = FTM_RX_LEVEL_DBM_TEST_ATLEAST_ONE_RECEIVER_BAD_STATE;
               FTM_MSG_1(FTM_ERROR, "ftm_1x_get_multiple_rx_level_dbm: Secondary chain not enabled: Dev : %d", device_drx);   
               ftm_cdma_rx_level_agc[number_selected_rx_chains] = ftm_cdma_rx_level_dbm10[number_selected_rx_chains] = 0;
             }
             break;
           default:
             status = FTM_RX_LEVEL_DBM_TEST_INVALID_RX_CHAIN;
             FTM_MSG_1(FTM_ERROR, "ftm_1x_get_multiple_rx_level_dbm: Invalid receiver type selected (select_rx_chain - %4X)", select_rx_chain);   
             ftm_cdma_rx_level_agc[number_selected_rx_chains] = ftm_cdma_rx_level_dbm10[number_selected_rx_chains] = 0;
             break;
        } /* switch(i) */
      } /* if (status == FTM_RX_LEVEL_DBM_TEST_SUCCESS) */
      number_selected_rx_chains++;
    } /* if ((select_rx_chain >> i) & (uint16)0x1) */
  } /* for (i=0;i<FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER;i++)*/

  FTM_MSG(FTM_MED, "Preparing for the response packet - FTM_GET_MULTIPLE_RX_LEVEL_DBM");  

  rsp_pkt_paylaod = &(ftm_cdma_get_rsp_pkt_data_ptr()->ftm_cdma_get_multiple_rx_level_dbm_rsp_pkt);

  /***************** Fill up RSP packet with the data *****************/    
  // Fill in the constant fields of the response packet. Sample buffer will be filled after sanity check.
  rsp_pkt_paylaod->cmd_code = 75;  
  rsp_pkt_paylaod->subsys_id = FTM;
  rsp_pkt_paylaod->subsys_cmd_code = (device == RFM_DEVICE_0) ? FTM_1X_C0_C : FTM_1X_C1_C ; ;
  rsp_pkt_paylaod->ftm_rf_cmd = FTM_GET_MULTIPLE_RX_LEVEL_DBM;  

  // Start with sucess flag and change it if we fail any check conditions.
  rsp_pkt_paylaod->select_rx_chain = select_rx_chain;
  rsp_pkt_paylaod->ftm_error_code = status;
    
  payload_size = number_selected_rx_chains*(sizeof(ftm_cdma_rx_level_agc[0]) + sizeof(ftm_cdma_rx_level_dbm10[0]));
  rsp_pkt_paylaod->payload_size = payload_size;
     
  for (i=0; i<number_selected_rx_chains; i++)
  {
    if (status == FTM_RX_LEVEL_DBM_TEST_SUCCESS)
    {
      rsp_pkt_paylaod->results[2*i] = ftm_cdma_rx_level_agc[i];
      rsp_pkt_paylaod->results[2*i + 1] = ftm_cdma_rx_level_dbm10[i];
    }
    else
    {
      rsp_pkt_paylaod->results[2*i] = 0;
      rsp_pkt_paylaod->results[2*i + 1] = 0;
    }
  } /* for (i=0; i<number_selected_rx_chains; i++) */
     
  // Fill in the constant fields of the response packet.Size of the packet will be based on the requested sample size.
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;  
  rsp_pkt->pkt_payload = (void*)rsp_pkt_paylaod;
  rsp_pkt->delete_payload = FALSE;
  rsp_pkt->pkt_len =  6 + 2*sizeof(uint16) + sizeof(int16) + payload_size;
  
  api_res = FTM_CDMA_NEW_RSP_PKT;

  /* Return Response Packet */
  return api_res ;

} /* ftm_1x_get_multiple_rx_level_dbm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the Synthesizer lock status
 
  @details
  This function will query the Device driver to check if the synth on a given
  device is locked or not.
 
  @param device
  RF device whose synth lock status is queried
 
  @param synth_status
  Container to store the Synth lock status
 
  @return
  Flag indicating, if the synth is locked or not.
*/
static ftm_cdma_rsp_type
ftm_1x_get_synth_lock_state
(
  rfm_device_enum_type device,
  boolean *synth_status
)
{
  *synth_status = rfm_cdma_ftm_get_synth_lock_state(device);

  return FTM_CDMA_GOOD_RSP;

} /* ftm_1x_get_synth_lock_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  txrx agc reload

  @details
  txrx agc reload

  @param   
 
  @return
  Response for the command
*/

static ftm_cdma_rsp_type
ftm_cdma_txrx_agc_reload
(
  void
)

{
  errno_enum_type result;
  ftm_cdma_rsp_type status = FTM_CDMA_GOOD_RSP;
  rfa_1x_txrx_agc_reload_cmd_t msg;
  
  msgr_init_hdr(&msg.hdr, MSGR_RFA_RF_1X, RFA_RF_1X_TXRX_AGC_RELOAD_CMD );
  result = msgr_send (&msg.hdr, sizeof(msg));  
  
  if(E_SUCCESS != result)
  {
    status = FTM_CDMA_BAD_RSP;
  }
  return status;
} /*ftm_1x_txrx_agc_reload */

/*! \} */

/*============================================================================*/
/*!
  @name CDMA Calibration Dispatch

  @brief
  This section contains the dispatches used for generic RF Calibration. This
  dispatcher services the requested from tools side and it also dispataches
  the internal requets.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA Calibration Dispatch function.

  @details
  This function handles requests to run calibration APIs by dispatching the
  appropriate functions.

  @param device
  RF Device on which RF Calibration is to be done

  @param *ftm
  Request Data Packet for FTM Calibration

  @return
  Response packet to the diag after the 1x Calibration command is dispatched
  and executed.
*/
ftm_rsp_pkt_type
ftm_cdma_dispatch
(
  rfm_device_enum_type device,
  ftm_pkt_type *ftm
)
{
  /* Request Packet : Extract the FTM Command only */
  ftm_rf_factory_parms *rf_req; /* Shortened Request Packet */ 
  ftm_cdma_rsp_type status = FTM_CDMA_BAD_RSP; /* FTM Cmd execution status */

  /* Create Response Packet : Defaults to Good Packet */
  ftm_rsp_pkt_type ftm_rsp_data;
  memset( &ftm_rsp_data, 0, sizeof(ftm_rsp_pkt_type) );

  /* Shorten the Request Packet to ftm_rf_factory_parms field */
  rf_req = &( ftm->rf_params );

  /* Dispatch the FTM Comand */
  switch ( rf_req->ftm_rf_cmd )
  {
  case FTM_SET_MODE: /* Enter and Enable 1x */
    status = ftm_1x_set_mode( device, rf_req->ftm_rf_factory_data.mode );
    break;

  case FTM_SET_CHAN: /* Enable Rx and Tune to band and Channel */
    status = ftm_1x_set_channel( device, rf_req->ftm_rf_factory_data.chan );
    break;

  case FTM_GET_DVGA_OFFSET: /* Calibrate DVGA for the given expected RxAGC */
    status = ftm_1x_measure_dvga_offset( device,
                              rf_req->ftm_rf_factory_data.gen_w,
                              (int16*)(&(rf_req->ftm_rf_factory_data.gen_w)) );
    break;

  case FTM_SET_DVGA_OFFSET: /* Set DVGA Offset */
    status = ftm_1x_set_dvga_offset( device, rf_req->ftm_rf_factory_data.gen_w);
    break;

  case FTM_GET_LNA_OFFSET: /* Perform LNA Calibration */
    status = ftm_1x_measure_lna_offset( device,
                         rf_req->ftm_rf_factory_data.id_val.id,
                         rf_req->ftm_rf_factory_data.id_val.val,
                         &(rf_req->ftm_rf_factory_data.id_val.val) );
    break;

  case FTM_SET_LNA_OFFSET: /* Set LNA Offset */
    status = ftm_1x_set_lna_offset( device,
                                (byte)rf_req->ftm_rf_factory_data.id_val.id,
                                rf_req->ftm_rf_factory_data.id_val.val );
    break;

  case FTM_GET_CAGC_RX_AGC: /* Read Raw Rx AGC */
    status = ftm_1x_get_rx_agc( device,
                               (int16*)(&(rf_req->ftm_rf_factory_data.gen_w)));
    break;

  case FTM_SET_LNA_RANGE: 
  /* Set LNA Range. Interface assumes LNA/PA RANGE have same values */
    status = ftm_1x_set_rx_gain_state( device, 
                   (rfm_lna_gain_state_type)rf_req->ftm_rf_factory_data.range );
    break;

  case FTM_SET_RF_POWER_MODE: /* Set RF Power Mode */
    status = ftm_cdma_set_power_mode( device, 
                                      rf_req->ftm_rf_factory_data.gen_b); 
    break;

  case FTM_SET_TX_ON: /* Enable Tx */
    status = ftm_1x_enable_tx( device);
    break;

  case FTM_SET_TX_OFF: /* Disable Tx */
    status = ftm_1x_disable_tx( device);
    break;

  case FTM_SET_CDMA_CW_WAVEFORM: /* choose between CDMA or tone */
    status = ftm_1x_set_tx_waveform( device, 
             rf_req->ftm_rf_factory_data.ftm_tx_waveform_data.ftm_tx_waveform,
             0 /* zero-offset */);
    break;

  case FTM_SET_TX_WAVEFORM: /* set Tx waveform to CDMA or cW - with 0 offset*/
    status = ftm_1x_set_tx_waveform( device, 
             rf_req->ftm_rf_factory_data.ftm_tx_waveform_data.ftm_tx_waveform,              
             rf_req->ftm_rf_factory_data.ftm_tx_waveform_data.ftm_tx_waveform_attrib
             );
    break;

  case FTM_SET_PDM: /* Set Tx AVGA */
    status = ftm_1x_set_tx_avga( device, 
                                 rf_req->ftm_rf_factory_data.id_val.val);
    break;

  case FTM_SET_PA_RANGE: /* Override PA Range */
    status = ftm_1x_set_pa_range( device, 
                                  (byte)rf_req->ftm_rf_factory_data.range); 
    break;

  case FTM_SET_PA_STATE: /* Override PA State with this */
     status = ftm_1x_set_pa_state( device,
                (rfm_pa_gain_state_type)rf_req->ftm_rf_factory_data.gen_b);
    break;

  case FTM_SET_TX_SWEEP_STEP_SIZE: /* Configure the Tx Sweep size */
  case FTM_CONFIGURE_TX_SWEEP_CAL: /* Configure params for TX sweep Cal */
  case FTM_TX_SWEEP_CAL: /* execute the Tx Cal sweep */
  case FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2: /* retrieve HDET readings */
  case FTM_GET_HDET_FROM_TX_SWEEP_CAL: /* retrieve HDET reading for given idx */
  case FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL:
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_dispatch: TxSweep Command %d Deprecated",
               rf_req->ftm_rf_cmd );
    status = FTM_CDMA_BAD_RSP;
    break;

  case FTM_SET_SECONDARY_CHAIN:    
    status = ftm_1x_set_second_chain_mode( device, 
                                   rf_req->ftm_rf_factory_data.id_val.id);
  break;

  case FTM_GET_ADC_VAL:
    rf_req->ftm_rf_factory_data.id_val.val = ftm_1x_read_hdet(device);
    status = FTM_CDMA_GOOD_RSP;
    break;

  case FTM_SECOND_CHAIN_TEST_CALL:    
    status = ftm_1x_set_second_chain_test_call( device, 
                                            rf_req->ftm_rf_factory_data.gen_b);
    break;

  case FTM_GET_SYNTH_STATE:
    status = ftm_1x_get_synth_lock_state( device,
                                     &(rf_req->ftm_rf_factory_data.on_off));
    break;

  case FTM_GET_MULTI_SYNTH_STATE: /* Read synth lock status */
    ftm->rf_params.ftm_rf_factory_data.ftm_get_multi_synth_lock_rsp.status_flag = TRUE;
    status = ftm_1x_get_multisynth_status(device,
		&ftm->rf_params.ftm_rf_factory_data.ftm_get_multi_synth_lock_rsp.rx_synth_status,
		&ftm->rf_params.ftm_rf_factory_data.ftm_get_multi_synth_lock_rsp.tx_synth_status);
    break;	

  case FTM_RESET_MULTI_SYNTH_STATE: /* Reset synth lock status */
    ftm->rf_params.ftm_rf_factory_data.on_off = TRUE;
    status = ftm_1x_reset_multisynth_status(device);
    break;	

  case FTM_SET_TUNER_TUNE_CODE_OVERRIDE:
    ftm->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.result =
      (ftm_tuner_override_status_type)ftm_1x_set_tuner_tune_code_override(
         device,
         ftm->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.override_flag,
         &(ftm->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.config_data),
         (uint8*)&(ftm->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.nv_data),
         (uint8*)&(ftm->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.tuner_id));
    status = FTM_CDMA_GOOD_RSP;
    break;	

  case FTM_SET_TX_POWER_DBM:
    status = ftm_1x_set_tx_power_dbm( device,
      rf_req->ftm_rf_factory_data.tx_power_dbm_data.enable_flag,
      rf_req->ftm_rf_factory_data.tx_power_dbm_data.tx_power.tx_power_dbm_1x);
    ftm_1x_tx_power_dbm = rf_req->ftm_rf_factory_data.tx_power_dbm_data.tx_power.tx_power_dbm_1x;
    break;

  case FTM_GET_RX_LEVEL_DBM:
         status = ftm_1x_get_rx_level_dbm( device,
           &rf_req->ftm_rf_factory_data.rx_level_dbm_data.rx_level_agc.rx_level_agc_1x,
           &rf_req->ftm_rf_factory_data.rx_level_dbm_data.rx_level_dbm.rx_level_dbm_1x);
    break;

  case FTM_SET_SMPS_PA_BIAS_VAL:
    status = ftm_1x_set_tx_apt_smps_bias_val( 
                          device,
                          (uint16)rf_req->ftm_rf_factory_data.gen_w);
    break;

  case FTM_SET_FREQ_ADJUST:
    rf_req->ftm_rf_factory_data.ftm_set_freq_adjust.total_freq_adjust_hz.total_freq_adjust_hz_cdma = 
      ftm_1x_set_freq_adjust(
        device,
        rf_req->ftm_rf_factory_data.ftm_set_freq_adjust.user_freq_adjust_ppb.user_freq_adjust_ppb_cdma,
        rf_req->ftm_rf_factory_data.ftm_set_freq_adjust.center_frequency_hz.center_frequency_hz_cdma,
        rf_req->ftm_rf_factory_data.ftm_set_freq_adjust.enable_xo_freq_adjust,
        &status
      );
    break;

  case FTM_SET_TX_PWR_LIMIT:
    status = ftm_1x_set_tx_pwr_limit( device, 
      rf_req->ftm_rf_factory_data.tx_pwr_limit_data.tx_pwr_limit_override,
      rf_req->ftm_rf_factory_data.tx_pwr_limit_data.tx_pwr_limit_dbm );
    break;

  case FTM_GET_MULTIPLE_RX_LEVEL_DBM:
    status = ftm_1x_get_multiple_rx_level_dbm( device,
      rf_req->ftm_rf_factory_data.get_multiple_rx_level_dbm_data.select_rx_chain,
      &ftm_rsp_data);
    break;
  /*--------------------------------------------------------------------------*/
  /* The following set of commands are to be executed internally by RF Driver.
  Thus, the response packet should be flagged as FTM_CDMA_NO_RSP; so that
  nothing is sent to diag.*/

  case  FTM_INT_SET_MODE:
    (void)ftm_1x_set_mode( device, rf_req->ftm_rf_factory_data.mode );
    status = FTM_CDMA_NO_RSP;
    break;

  case FTM_INT_SET_CHAN: /* Enable Rx and Tune to band and Channel */
    (void)ftm_1x_set_channel( device, rf_req->ftm_rf_factory_data.chan );
    status = FTM_CDMA_NO_RSP;
    break;

  case FTM_INT_SET_TX_ON:
    (void)ftm_1x_enable_tx( device );
    status = FTM_CDMA_NO_RSP;
    break;
    
  case FTM_INT_SET_TX_OFF:
    (void)ftm_1x_disable_tx( device );
    status = FTM_CDMA_NO_RSP;
    break;

  case FTM_INT_SET_LOWEST_TX_PWR:
    (void)ftm_1x_set_tx_lowest_power( device );
    status = FTM_CDMA_NO_RSP;
    break;
    
  case FTM_INT_CALV3_RADIO_SETUP:
    (void)ftm_1x_calibration_radio_setup( 
       (ftm_cal_radio_setup_req_type*)ftm );
    status = FTM_CDMA_NO_RSP;
    break;

  case FTM_IQ_CAPTURE_GET_CONFIG:
    ftm->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_config.ftm_error_code =
    ftm_1x_iq_capture_get_config(&ftm->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_config);
    status = FTM_CDMA_GOOD_RSP;
    break;	

  case FTM_IQ_CAPTURE_ACQUIRE_SAMPLES:
    ftm->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_acquire_samples.ftm_error_code =
    ftm_1x_iq_capture_acquire_samples(
             device,
             &ftm->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_acquire_samples
            );
    status = FTM_CDMA_GOOD_RSP;
    break;	

  case FTM_IQ_CAPTURE_GET_SAMPLES: 
   /* Create response packet and fill in the basic feilds */
   ftm_1x_create_iq_capture_get_samples_rsp_diag_pkt(device,&ftm_rsp_data);
   /* Based on the request and sanity checks fill in the rest of the feilds of the response packet */
    ftm_1x_iq_capture_get_samples(
		    device,
		    &ftm->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_samples,
                    &ftm_rsp_data
                   );
    status = FTM_CDMA_NEW_RSP_PKT;
    break;	

   case FTM_INT_CALV3_FEEDBACK_SETUP:
    (void)ftm_1x_calibration_feedback_setup( 
       (ftm_cal_feedback_setup_req_type*)ftm );
    status = FTM_CDMA_NO_RSP;
    break;

   case FTM_IQ_CAPTURE_ANALYZE_SAMPLES:
    FTM_MSG_1(FTM_HIGH,"The cmd IQ CAPTURE ANALYSIS SAMPLES is yet to be implemented...",0); 
    status = FTM_CDMA_GOOD_RSP;
    break;

  /*--------------------------------------------------------------------------*/

  case FTM_RF_MEAS_TX_PARAMS:
     /* Create response packet and fill in the basic feilds */
     /* Based on the request and sanity checks fill in the rest of the feilds of the response packet */
     ftm_1x_fbrx_meas_tx_params(device,
                                ftm->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_tx_params.num_averages,
                                ftm->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_tx_params.capture_offset,
                                ftm->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_tx_params.tx_measurement_config,
                                &ftm_rsp_data);

     status = FTM_CDMA_NEW_RSP_PKT;
     break;

  case FTM_FBRX_IQ_CAPTURE_TRIGGER:
     /* Create response packet and fill in the basic feilds */
     /* Based on the request and sanity checks fill in the rest of the feilds of the response packet */
     ftm_1x_fbrx_iq_capture_trigger(device,
                                    ftm->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_trigger.sample_size,
                                    ftm->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_trigger.fbrx_chain,
                                    &ftm_rsp_data);

     status = FTM_CDMA_NEW_RSP_PKT;
     break;

  case FTM_FBRX_IQ_CAPTURE_GET_SAMPLES:
     /* Create response packet and fill in the basic feilds */
     /* Based on the request and sanity checks fill in the rest of the feilds of the response packet */
     ftm_1x_fbrx_iq_capture_get_samples(device,
                                        ftm->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_get_samples.sample_source,
                                        ftm->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_get_samples.sample_offset,
                                        ftm->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_get_samples.sample_size,
                                        &ftm_rsp_data);
     status = FTM_CDMA_NEW_RSP_PKT; 
    break;

   case FTM_INT_CALV3_XPT_IQ_CAPTURE:
     ftm_1x_calv3_xpt_iq_capture( (ftm_cal_xpt_instr_req_type*)ftm );
     status = FTM_CDMA_NO_RSP; 
   break;

  /*--------------------------------------------------------------------------*/

  default:
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_dispatch: Bad Cmd %d", rf_req->ftm_rf_cmd );
    status = FTM_CDMA_BAD_RSP;
    break;

  }

  /* Create Rsp Pkt if status is not a new RSP packek. For new RSP packet type */
  /* FTM API will create packet */
  if(status != FTM_CDMA_NEW_RSP_PKT)
  {
    ftm_rsp_data = ftm_cdma_create_rsp_pkt( (ftm_cdma_rsp_type)status );
  }

  FTM_MSG_1( FTM_MED, "ftm_cdma_dispatch: Command %d Completed",
             rf_req->ftm_rf_cmd );
  
  /* Return the Response Packet */
  return ftm_rsp_data;

} /* ftm_cdma_dispatch */

/*! \} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HDR FTM Dispatch function.

  @details
  This function handles requests to run calibration APIs by dispatching the
  appropriate functions.

  @param device
  RF Device on which FTM action is to be done

  @param *ftm
  Request Data Packet for FTM dispatch

  @return
  Response packet to the diag after the HDR FTM command is dispatched
  and executed.
*/
ftm_rsp_pkt_type
ftm_hdr_dispatch
(
  rfm_device_enum_type device,
  ftm_pkt_type *ftm
)
{
  /* Request Packet : Extract the FTM Command only */
  ftm_rf_factory_parms *rf_req; /* Shortened Request Packet */ 
  ftm_cdma_rsp_type status; /* FTM Cmd execution status */

  /* Create Response Packet : Defaults to Good Packet */
  ftm_rsp_pkt_type ftm_rsp_data;
  
  /* Shorten the Request Packet to ftm_rf_factory_parms field */
  rf_req = &( ftm->rf_params );

  /* Dispatch the FTM Comand */
  switch ( rf_req->ftm_rf_cmd )
  {
   case FTM_SET_MODE: /* Enter and Enable HDR */
    status = ftm_hdr_set_mode( device, rf_req->ftm_rf_factory_data.mode );
    break;

   case FTM_SET_CHAN: /* Enable Rx and Tune to band and Channel */
    status = ftm_hdr_set_channel( device, rf_req->ftm_rf_factory_data.num_band_chan, rf_req->ftm_rf_factory_data.chan_array );
    break;

   case FTM_SET_TX_ON:
    status = ftm_hdr_set_tx_on(device);
    break;

   case FTM_SET_TX_OFF:
    status = ftm_hdr_set_tx_off(device);
    break;

   case FTM_SET_SECONDARY_CHAIN:
    status = ftm_hdr_set_second_chain_mode(device, rf_req->ftm_rf_factory_data.id_val.id );
    break;

   case FTM_GET_RX_LEVEL_DBM:
    status = ftm_hdr_get_rx_level_dbm( device,
		rf_req->ftm_rf_factory_data.rx_level_dbm_data.rx_level_agc.rx_level_agc_hdr,
		rf_req->ftm_rf_factory_data.rx_level_dbm_data.rx_level_dbm.rx_level_dbm_hdr);
    break;

   default:
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_dispatch: Bad Cmd %d", rf_req->ftm_rf_cmd );
    status = FTM_CDMA_BAD_RSP;
    break;
  }

  ftm_rsp_data = ftm_cdma_create_rsp_pkt( (ftm_cdma_rsp_type)status );

  FTM_MSG_1( FTM_MED, "ftm_hdr_dispatch: Command %d Completed",
             rf_req->ftm_rf_cmd );
  
  /* Return the Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_dispatch */


/*============================================================================*/
/*!
  @name 1xHDR Cal v2 Dispatch

  @brief
  This section contains the Calibration version 2 Dispatches. Cal V2 disptach
  is used for LNA, DVA, IM2, and Intelliciever Calibration.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calibration V2 Dispatch function.

  @details
  This function handles requests to run tests and  other primitives
  by dispatching the appropriate functions.

  @param *req
  Request Data Packet for FTM Calibration
  
  @return
  Response packet to the diag after the FTM SVDO command is dispatched and
  executed.
*/
ftm_rsp_pkt_type
ftm_cdma_cal_v2_dispatch
(
  void* req
)
{
  /* Request Packet : Extract the FTM Command only */
  ftm_composite_cmd_header_type* header = req;

  /* Create Response Packet : Defaults to Bad Packet */
  ftm_rsp_pkt_type ftm_rsp_data = ftm_cdma_create_rsp_pkt( FTM_CDMA_BAD_RSP );

  /* Dispatch the FTM Comand */
  switch (header->ftm_hdr.cmd_id)
  {

  case FTM_CDMA_CAL_DVGA: /* Calibrate DVGA */
    ftm_rsp_data = ftm_1x_calibrate_dvga( req ); 
    break;

  case FTM_CDMA_CAL_LNA: /* Calibrate LNA */
    ftm_rsp_data = ftm_1x_calibrate_lna( req ); 
    break;

  case FTM_CDMA_CAL_INTELLICEIVER:
    ftm_rsp_data = ftm_1x_calibrate_intelliceiver( req );
    break;

  case FTM_CDMA_CAL_COMMIT_NV: /* Calibrate LNA */
    ftm_rsp_data = ftm_1x_commit_cal_nv( req ); 
    break;

  default: /* Invalid Command */
    FTM_MSG_1( FTM_ERROR, "Cal v2 Dispatch : Unknown FTM RF Command %d",
               header->ftm_hdr.cmd_id );
    break;

  }

  FTM_MSG_1( FTM_MED, "ftm_cdma_cal_v2_dispatch: Command %d Completed",
             header->ftm_hdr.cmd_id );

  /* Return Response packet */
  return ftm_rsp_data;


} /* ftm_cdma_cal_v2_dispatch */

/*! \} */

/*============================================================================*/
/*!
  @name 1xHDR Radio Control Dispatch

  @brief
  This section contains the dispatches used to control the SVDO Radio.

  @image html ftm_cdma_rfm_command.jpg
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  SVDO Dispatch function.

  @details
  This function handles requests to run tests and  other primitives
  by dispatching the appropriate functions.

  @param *ftm_req_data
  Request Data Packet

  @return
  Response packet to the diag after the FTM SVDO command is dispatched and
  executed.
*/
ftm_rsp_pkt_type
ftm_cdma_radio_dispatch
(
  void  * ftm_req_data
)
{
  /* Request Packet : extract the FTM Command only */
  ftm_composite_cmd_header_type* header =
    (ftm_composite_cmd_header_type*)ftm_req_data;

  /* Create Response Packet : Defaults to Bad Packet */
  ftm_rsp_pkt_type ftm_rsp_data = ftm_cdma_create_rsp_pkt( FTM_CDMA_BAD_RSP );

  /* Dispatch Commands */
  switch (header->ftm_hdr.cmd_id)
  {

  /*--------------------------------------------------------------------------*/
  /* 1x commands */
  /*--------------------------------------------------------------------------*/

  case FTM_1X_RADIO_ENTER:
    ftm_rsp_data = ftm_1x_radio_enter( ftm_req_data );
    break;

  case FTM_1X_RADIO_ENABLE:
    ftm_rsp_data = ftm_1x_radio_enable( ftm_req_data );
    break;

  case FTM_1X_RADIO_DISABLE:
    ftm_rsp_data = ftm_1x_radio_disable( ftm_req_data );
    break;

  case FTM_1X_RADIO_WAKEUP_RX:
    ftm_rsp_data = ftm_1x_radio_wakeup_rx( ftm_req_data );
    break;

  case FTM_1X_RADIO_SLEEP_RX:
    ftm_rsp_data = ftm_1x_radio_sleep_rx( ftm_req_data );
    break;

  case FTM_1X_RADIO_RETUNE:
    ftm_rsp_data = ftm_1x_radio_retune( ftm_req_data );
    break;

  case FTM_1X_RADIO_ENABLE_DIV:
    ftm_rsp_data = ftm_1x_radio_enable_diversity( ftm_req_data );
    break;

  case FTM_1X_RADIO_DISABLE_DIV:
    ftm_rsp_data = ftm_1x_radio_disable_diversity( ftm_req_data );
    break;

  case FTM_1X_RADIO_WAKEUP_TX:
    ftm_rsp_data = ftm_1x_radio_wakeup_tx( ftm_req_data );
    break;

  case FTM_1X_RADIO_SLEEP_TX:
    ftm_rsp_data = ftm_1x_radio_sleep_tx( ftm_req_data );
    break;

  case FTM_1X_RADIO_CLEAR_LNA_OVERRIDE:
    ftm_rsp_data = ftm_1x_radio_clear_lna_override(ftm_req_data);
    break;

  case FTM_1X_RADIO_SET_TX_OVERRIDE:
    ftm_rsp_data = ftm_1x_radio_set_tx_override(ftm_req_data);
    break;

  case FTM_1X_CAL_V3_CONFIG_RADIO:
    ftm_rsp_data = ftm_1x_calv3_configure_radio( ftm_req_data );
    break;

  case FTM_1X_CAL_V3_RETUNE_RADIO:
    ftm_rsp_data = ftm_1x_calv3_retune_radio( ftm_req_data );
    break;

  case FTM_1X_CAL_V3_TEAR_DOWN_RADIO:
    ftm_rsp_data = ftm_1x_calv3_tear_down_radio( ftm_req_data );
    break;

  case FTM_1X_RADIO_GET_RX_AGC:
    ftm_rsp_data = ftm_1x_radio_get_rx_agc( ftm_req_data );
    break;
     
  case FTM_1X_RADIO_GET_TX_AGC:
    ftm_rsp_data = ftm_1x_radio_get_tx_agc( ftm_req_data );
    break;

  case FTM_1X_RADIO_SET_TX_POWER:
    ftm_rsp_data = ftm_1x_radio_set_tx_power( ftm_req_data );
    break;

  case FTM_1X_RADIO_TIMED_SLEEP:
    ftm_rsp_data = ftm_1x_radio_timed_sleep( ftm_req_data );
    break;
  
  /*--------------------------------------------------------------------------*/
  /* HDR commands */
  /*--------------------------------------------------------------------------*/

  case FTM_HDR_RADIO_ENTER:
    ftm_rsp_data = ftm_hdr_radio_enter( ftm_req_data );
    break;

  case FTM_HDR_RADIO_ENABLE:
    ftm_rsp_data = ftm_hdr_radio_enable( ftm_req_data );
    break;

  case FTM_HDR_RADIO_DISABLE:
    ftm_rsp_data = ftm_hdr_radio_disable( ftm_req_data );
    break;

  case FTM_HDR_RADIO_WAKEUP_RX:
    ftm_rsp_data = ftm_hdr_radio_wakeup_rx( ftm_req_data );
    break;

  case FTM_HDR_RADIO_ENABLE_DIV:
     ftm_rsp_data = ftm_hdr_radio_enable_div( ftm_req_data );
     break;

  case FTM_HDR_RADIO_WAKEUP_TX:
    ftm_rsp_data = ftm_hdr_radio_wakeup_tx( ftm_req_data );
    break;

  case FTM_HDR_RADIO_SLEEP_TX:
     ftm_rsp_data = ftm_hdr_radio_sleep_tx( ftm_req_data );
     break;

  case FTM_HDR_RADIO_SLEEP_RX:
     ftm_rsp_data = ftm_hdr_radio_sleep_rx( ftm_req_data );
     break;

  case FTM_HDR_RADIO_DISABLE_DIV:
     ftm_rsp_data = ftm_hdr_radio_disable_div( ftm_req_data );
     break;

  case FTM_HDR_RADIO_RETUNE:
    ftm_rsp_data = ftm_hdr_radio_retune( ftm_req_data );
    break;
     
  case FTM_HDR_RADIO_GET_RX_AGC:
    ftm_rsp_data = ftm_hdr_radio_get_rx_agc( ftm_req_data );
    break;

  case FTM_HDR_RADIO_GET_TX_AGC:
    ftm_rsp_data = ftm_hdr_radio_get_tx_agc( ftm_req_data );
    break;

  case FTM_HDR_RADIO_SET_TX_POWER:
    ftm_rsp_data = ftm_hdr_radio_set_tx_power( ftm_req_data );
    break;

  case FTM_HDR_RADIO_CLEAR_LNA_OVERRIDE:
    ftm_rsp_data = ftm_hdr_radio_clear_lna_override( ftm_req_data );
    break;

  /*--------------------------------------------------------------------------*/
  /* CDMA commands */
  /*--------------------------------------------------------------------------*/

  case FTM_CDMA_RADIO_SET_PM_OVERRIDE:
  case FTM_1X_RADIO_SET_PM_OVERRIDE:
    ftm_rsp_data = ftm_cdma_radio_power_mode_override(ftm_req_data);
    break;

  case FTM_CDMA_RADIO_CLEAR_PM_OVERRIDE:
  case FTM_1X_RADIO_CLEAR_PM_OVERRIDE:
    ftm_rsp_data = ftm_cdma_radio_clear_power_mode_override(ftm_req_data);
    break;

  case FTM_CDMA_RADIO_GET_PM:
  case FTM_1X_RADIO_GET_PM:
    ftm_rsp_data = ftm_cdma_radio_get_power_mode(ftm_req_data);
    break;

  case FTM_CDMA_DRIVER_VER_SANDBOX:
    ftm_rsp_data = ftm_cdma_driver_verification_sandbox(ftm_req_data);
    break;

  case FTM_CDMA_RADIO_FORCE_RX_ONOFF:
    ftm_rsp_data = ftm_cdma_radio_test_rx_on_off( ftm_req_data );
    break;

  case FTM_CDMA_CONFIG_DEBUG_MODE:
    ftm_rsp_data = ftm_cdma_radio_config_debug_mode( ftm_req_data );
    break;

  case FTM_CDMA_CONFIG_IC_OPERATION:
    ftm_rsp_data = ftm_cdma_radio_configure_ic_operation( ftm_req_data );
    break;

  default:
    FTM_MSG_1( FTM_ERROR, "CDMA Radio Dispatch : Invalid Command - %d",
               header->ftm_hdr.cmd_id );
    break;

  }

  FTM_MSG_1( FTM_MED, "CDMA Radio Dispatch : Command - %d",
             header->ftm_hdr.cmd_id );

  /* Return Response packet */
  return ftm_rsp_data;

} /* ftm_cdma_radio_dispatch */

/*! \} */

/*============================================================================*/
/*!
  @name 1xHDR MC Dispatch

  @brief
  This section contains the dispatches used for generic 1xHDR Tests. This
  dispatcher services the requested from tools side and it also dispataches
  the internal requets.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Test Dispatch function.

  @details
  This function handles requests to run tests and  other primitives
  by dispatching the appropriate functions.

  @param *data
  Request Data Packet for HDR Test

  @return
  Response packet to the diag after the Test is done
*/
ftm_rsp_pkt_type
ftm_cdma_mc_dispatch
(
  ftm_mc_pkt_type *data
)
{
  ftm_rsp_pkt_type       rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  /* response for MC commands */
  ftm_mc_cmd_resp_type   ftm_mc_response_data; 

  /*--------------------------------------------------------------------------*/
  ftm_mc_response_data.diag_hdr = data->diag_hdr;
  ftm_mc_response_data.sub_cmd = data->ftm_hdr.cmd_id;
  ftm_mc_response_data.status = TEST_SUCCESS;
  
  FTM_MSG_1( FTM_HIGH, "ftm_cdma_mc_dispatch:FTM MC dispatching packet id: %d", 
             data->ftm_hdr.cmd_id);

  switch ( data->ftm_hdr.cmd_id )
  {
    case FTM_1X_RX_LEVEL_REQ:
    {
      /* this is the ONLY MC-level command that needs a data packet to be 
         returned. Since this is RxAGC related, the implementation was decided
         to be done within RF-FTM module.
      */

      ftm_rx_level_rsp_type ftm_rx_level_rsp; /* container for RxAgc response */
      rfm_cdma_ftm_rx_agc_data_type rx_agc_data; /* holds RxACG data from RFM */
      boolean rxagc_is_valid; /* result of RFM API */

      ftm_cmd_rx_level_param_type *param = &data->param.rx_level_req_params;

      /* assign correct values to ftm_rx_level_rsp packet */
      ftm_rx_level_rsp.diag_hdr = data->diag_hdr;
      ftm_rx_level_rsp.sub_cmd  = data->ftm_hdr.cmd_id;
      ftm_rx_level_rsp.ftm_rx_report.rx_chain = param->rx_chain;

      /* query to get RxAGC data */
      rxagc_is_valid = rfm_1x_ftm_get_rx_agc_data( param->rx_chain, 
                                     &rx_agc_data, RFM_CDMA_AGC_FORMAT__DBM10 );

      /* check to see if rfm call was successful */
      if ( rxagc_is_valid == TRUE )
      {
        /* Returning raw Rx_AGC value in 10-bit format */
        ftm_rx_level_rsp.ftm_rx_report.rssi = 
                                         (rx_agc_data.raw_rxagc/64);
        ftm_rx_level_rsp.ftm_rx_report.rx_pwr_dbm10 = 
                                         rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0];

        /* mark command as success */
        ftm_rx_level_rsp.status = TEST_SUCCESS; 

        /* send the data to module that invoked the FTM_1X_RX_LEVEL_REQ
         command */
        rsp_pkt = ftmdiag_create_new_pkt_with_data((byte*)&ftm_rx_level_rsp, 
                                      sizeof(ftm_rx_level_rsp), (byte*)NULL, 0);

      }
      else
      {
        /* flag a failure */
        ftm_rx_level_rsp.status = TEST_NO_SUCCESS; 
        rsp_pkt.cmd = FTM_RSP_BAD_CMD;

        FTM_MSG_1( FTM_ERROR, "ftm_cdma_mc_dispatch: Get RxAGC call to RFM for"
                              "device %d failed", param->rx_chain);
      }
    } /* case FTM_1X_RX_LEVEL_REQ: */
    break;
  
    case FTM_FCH_RESET_STATS_SC:
      onex_nonsig_reset_fch_stats();
      break;

    case FTM_FCH_GET_STATS_SC:
    { 
      /* response stats response */
      ftm_fch_stats_rsp_type ftm_fch_stats_rsp;

      /* container sent to 1x NonSig module */
      onex_nonsig_cmd_rxc_fer_ser_type onex_ser_data_capsule;

      /* result from NonSig API call */
      onex_nonsig_cmd_status_type cmd_res;

      /* assign the correct header and command_id */
      ftm_fch_stats_rsp.diag_hdr = data->diag_hdr;        
      ftm_fch_stats_rsp.sub_cmd = data->ftm_hdr.cmd_id;  
 
      cmd_res = onex_nonsig_get_fch_stats(&onex_ser_data_capsule);

      /* check for success */
      if ( cmd_res != NS1X_TEST_INVALID_CMD )
      {
        /* copy over the stats to ftm_fch_stats_rsp packet */
        ftm_fch_stats_rsp.ftm_rxc_fer_ser_report.fch_frame_total  = 
                                          onex_ser_data_capsule.fch_frame_total;    
        ftm_fch_stats_rsp.ftm_rxc_fer_ser_report.fch_frame_errors = 
                                         onex_ser_data_capsule.fch_frame_errors;    
        ftm_fch_stats_rsp.ftm_rxc_fer_ser_report.fch_sym_total    = 
                                            onex_ser_data_capsule.fch_sym_total;      
        ftm_fch_stats_rsp.ftm_rxc_fer_ser_report.fch_sym_errors = 
                                           onex_ser_data_capsule.fch_sym_errors;      

        /* mark command as success */
        ftm_fch_stats_rsp.status = TEST_SUCCESS;

        rsp_pkt = ftmdiag_create_new_pkt_with_data((byte*)&ftm_fch_stats_rsp, 
                                                   sizeof(ftm_fch_stats_rsp),
                                                   (byte*)NULL, 0);

      }
      else
      {
        /* flag a failure */
        ftm_fch_stats_rsp.status = TEST_NO_SUCCESS;
        rsp_pkt.cmd = FTM_RSP_BAD_CMD;
      }
    
    } /* case FTM_FCH_GET_STATS_SC: */
    break;

    default:
    /* Implement ALL other MC-level commands thru 1xL1 Non-Sig module */
    {
      /* Skip call to RFmode exit here, since "rx chain" is part of union
      it will point to the band, we will end up trying to disable 
      something thats not even valid, which will lead to crashes or bad
      access issues , L1 uses TRM to get device information so it should be
      safe to skip this.*/

      /* Cleanup any RF FTM state only at just before entering CDMA NS call
      if (data->ftm_hdr.cmd_id == FTM_FIRST_CMD)
      {    
        ftm_rfmode_exit(data->param.rx_level_req_params.rx_chain, FTM_STATE_PARK);
      }
      */
    
      ftm_mc_response_data.status =  
          onex_nonsig_queue_cmd( &data->param.onex, data->ftm_hdr.cmd_id );
      
      switch (ftm_mc_response_data.status)
      {
      case TEST_SUCCESS:
      case TEST_INVALID_STATE:
      case TEST_NO_SUCCESS:
        rsp_pkt = ftmdiag_create_new_pkt_with_data((byte*)&ftm_mc_response_data, 
                                                   sizeof(ftm_mc_response_data),
                                                   (byte*)NULL, 0);
      break;

      case TEST_INVALID_CMD:
      default:
        rsp_pkt.cmd = FTM_RSP_BAD_CMD;
      }
    
    } /* default case for "switch (ftm->ftm_hdr.cmd_id )" */
    break;

  } /* switch (data->ftm_hdr.cmd_id ) */
  
  return rsp_pkt;
  
} /* ftm_cdma_mc_dispatch */

/*! \} */

#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FEATURE_CDMA1X */
/*! @} */

