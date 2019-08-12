/*!
  @file
  ftm_1x_radio.c

  @brief
  This module contains FTM HWTC 1x Radio Control code which is used to 
  exclusively control Radio for test purposes.
*/

/*==============================================================================

  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/src/ftm_1x_radio.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/04/15   vc      Updated the Copyright information
01/24/15   vr      Init Tx Freq Error to zero during FTM wakeup Tx
11/19/14   spa     Added FTM concurrecny manager check,done in single enable API
09/22/14   spa     KW warning fixes
04/21/14   JJ      call get current rtc time api from ftm_cdma_msm.c
04/10/14   JJ      Pass fw_smem_ptr to enable modulator api
04/08/14   JJ      Add a new FTM api for timed sleep rx
12/06/13   JJ      Add check for enable modulator
11/14/13   kma     Added pin calibration support
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
05/06/13   spa     Use #def from QPOET driver for min Tx power in TxAGC override
04/24/13   sty     Added calls to ftm_1x_enable_baseband_tx
04/18/13   spa/fh  call enc_tx_enable() during retune
04/09/13   cri     Add init and deinit for cal data module
03/28/13   zhw     Get txagc F3 format correction
03/19/13   aro     Deleted ET config path
02/23/13   zhw     Skip modulator enable & ET call if wakeup_tx fails.
02/20/13   zhw     Remove 5ms delay as FW has stopped sending RF OFF at start
                   Config traffic state before FTM wakeup Tx
02/05/13   cd      Implicit xPT configuration is valid for radio wakeup tx 
                   as it does not affect APT cal right now
02/05/13   cd      Remove implicit FTM xPT configuration during wakeup Tx
01/22/13   zhw     Added 5ms delay between tx_start and modulator enable
01/18/13   aro     Removed nikel-XPT specific code
01/18/13   aro     Deleted nikel-XPT specific code
01/15/13   Saul    CDMA. KW Fix
01/04/13   Saul    CDMA. KW Fix
12/19/12   cd      Program ET Tx configuration only in FTM mode
12/17/12   sty     KW fixes
12/16/12   sty     KW fixes
12/04/12   zhw     APT PA Q Current support (interface cleanup)
11/28/12   Saul    Sample Capture. Temp features to FEATURE_RF_HAS_XPT_SUPPORT
11/19/12   zhw     Cal mode interface change support for APT in Dime
11/16/12   Saul    Sample Capture. Use common response.
11/08/12   cd      - Changed legacy override to FW override
10/30/12   spa     Removed refs to RFM_DEVICE_4 (not used in CDMA)
10/25/12   spa     Fixed miscalculation of slave device is calv3 config radio 
10/24/12   zhw     CDMA. Fix SV Chain Cal trying to enable diversity issue
10/19/12   Saul    CDMA. Removed tx_agc_override from sample capture.
10/12/12   Saul    CDMA Sample Capture. Changed PA bias
10/03/12   Saul    CDMA Sample Capture. Capture done message.
09/19/12   Saul    CDMA Sample Capture. Capture type, do leakage flag. 
                   Use PA state 1.
09/04/12   aro     Added band and channel to EPT FB config interface
08/27/12   Saul    CDMA. Added call to log xpt samples to EFS.
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/17/12   aro     Sequence change to perform DAC Stop followed by TX Stop
08/08/12   cvd     Replace timetick with rfcommon_time_profile interface
08/01/12   aro     Added EPT override Test function
07/02/12   cvd     Add Null pointer checks for ftm_cdma_data_get_device_ptr()
06/29/12   spa     Compiler warning fix 
04/13/12   aro     Fixed invalid error message
03/18/12   aro     Moved common radio commands to common module
03/15/12   spa     Added max device check for rf_cdma_get_device_status() 
03/09/12   spa     Changed ftm_cdma_data_update_band_chan arguments to cdma band     
                   and chan type
03/02/12   aro     F3 message Update to flag Cal v3
01/12/12   aro     Converted Tx flag and Rx flag to device flag
01/03/12   hdz     Force 1x to Traffic mode in ftm_1x_radio_wakeup_tx() 
12/21/11   aro     Added 1x Enable and disable radio functions
12/08/11   aro     Power Mode overriden to HL while doing tuning in cal v3
12/06/11   aro     Added infrastructure in CDMA Cal v3 to perform one PM cal
11/17/11   aro     Enabled FTM callback functions for RFM APIs
11/17/11   aro     Added new MCPM configure function with voting mechanism
11/15/11   aro     Added Cal v3 1x time profiling
11/15/11   cd      Get Tx AGC should return PA state instead of PA range
11/12/11   aro     Moved CDMA common commands from 1x to CDMA module
11/12/11   aro     Doxygen fix
11/07/11   cd      Update FTM CDMA data in retune function with current band 
                   chan info
11/04/11   aro     Added generic payload for CDMA Driver verification cmd
10/20/11   adk     Added support for SV diversity.
10/13/11   bmg     FTM RFM interface support to set SAR state.
10/13/11   aro     Added interface to enable and disable various debug mode for
                   various debug modules
09/22/11   aro     Added HDET disable and enable in Cal v3 retune function
09/22/11   cd      Added power mode and jammer state logging in Rx AGC response
                   packet
09/20/11   aro     Added new function to perform clear PA Range
                   override action
09/15/11   cd      Fixed conversion of set_tx_pwr value to Tx AGC units
09/01/11   cd      Remove PA range functionality, replace with PA state
08/31/11   aro     Updated cal v3 retune to set lowest RGI
08/29/11   aro     Created a Driver verification sandbox
08/24/11   aro     Updated Tx Data query function to have more Tx parameter
08/24/11   aro     Fixed Bad HDET read om second Cal v3 sweep
08/23/11   sty     Fixed KW errors
08/11/11   aro     [1] Moved Cal v3 configure rx to be done under ISR context
                   [2] Added time profiling for Radio config and retune func
08/11/11   sar     Made code review changes.
08/11/11   sar     Fixed KW errors.
08/08/11   aro     Fixed the slave device mask check causing val v3 to fail
                   when primary and diveristy radio is calibrated simultaneously
08/08/11   aro     Updated cal v3 sequence verification function to cover cases
                   with diversity enabled or disabled
08/08/11   aro     Added debug interface to test Cal v3 sequence
08/03/11   aro     Added support for radio tear down for cal v3
08/04/11   aro     Compiler warning fix
08/03/11   aro     Added support for retune for cal v3
07/28/11   aro     Compiler warning fix
07/27/11   aro     Compiler warning fix
07/27/11   aro     [1] Moved Enable/Disable combiner interrupts to FTM 1x
                   Enter and Exit
                   [2] Added function to configure radio for Cal v3
07/13/11   sty     Fixed warning
07/13/11   sty     Fixed compile error
07/13/11   sty     Added null pointer checks
07/12/11   aro     Added support to query intelliceiver power mode
07/07/11   sty     Added band info in arg list when API 
                   rfm_1x_ftm_set_tx_agc_override is invoked 
07/01/11   aro     Added Debug interface to override Power Mode
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/16/11   sar     Including internal 1x interface.
06/14/11   aro     Renamed TxAGC Override function + Signature to use PA State
06/14/11   sty     Changes to report all AGC in dBm10 units
06/13/11   sty     Changes needed for 1x diversity in FTM mode
06/13/11   aro     Added 1x Tx Override command
06/08/11   aro     Changed disable 1x modulator func signature to have device
06/07/11   aro     Changed 1x modulator function signature to have device
06/06/11   cd      Fixed get_tx_agc() interface to correctly divide with 
                   rounding
06/03/11   aro     Added 1x Tear down in 1x enter Mode
06/01/11   sty     Fixed minor bug in channel assignment
05/27/11   sty     Compile errors 
05/27/11   sty     Fixed functionality in ftm_1x_radio_enable_diversity()
05/25/11   sty     Enable combiner interrupts after prep_wakeup
05/17/11   cd      Changed get Tx AGC implementation to return value in dBm100
05/17/11   aro     Changed the order to configure Modulator after Tx Wakeup
05/16/11   aro     Fixed RxAGC dBm conversion formula
05/13/11   aro     [1] Updated Read 1x RxAGC to correspond to new FW RxAGC
                   [2] Added interface to configure combiner interrupts
05/06/11   aro     Added 1x disable diversity radio control
05/06/11   aro     Added 1x enable diversity radio control
05/05/11   aro     Replaced wakeup rx interface with Prep and execute function
04/25/11   aro     Renamed FTM RFMODE to FTM State
04/21/11   aro     Changed rfm_enter_mode() call to support RxLM Buffer
03/29/11   sty     Deleted debug flag sleep_clk_mod - modulator is now diabled 
                   when Rx is put to sleep 
03/24/11   aro     [1] Renamed FTM system control filename
                   [2] Compiler Warning Fix
03/24/11   aro     Defined unique RxAGC Read structure for 1x
03/24/11   aro     Renamed FTM Enter Mode function 
03/22/11   sty     Deleted APIs that enabled and disabled clks - this is taken 
                   care of by MCPM
03/22/11   sty     Update response in ftm_1x_radio_sleep_tx
03/21/11   sty     Disable Tx clks and modulator - using flag sleep_clk_mod to
                   NOT execute the disabling of Tx clks and modulator for now
                   Code cleanup
03/15/11   aro     Added consolidated TxAGC Read Interface
03/14/11   aro     Consolidated RxAGC Reads to one data structure
03/11/11   sty     FTM State data structure was not being updated for Rx and 
                   Tx operations
03/10/11   sty     Fixed debug message
03/09/11   sty     Changed ftm_1x_radio_set_tx_power() to accept TxAGC with
                   (1/10) dBm resolution
                   Added limiting function to catch over- and underflows in 
                   ftm_1x_radio_set_tx_power()
03/08/11   sty     Added implementation for ftm_1x_radio_set_tx_power
03/04/11   aro     Compiler Warning Fix
03/04/11   aro     Added Turnaround Query function
03/04/11   sty     Disabled debug flag
03/04/11   sty     Added debug flag in ftm_1x_radio_get_tx_agc() to debug 
                   txAGC calculation
03/04/11   sty     Modified ftm_1x_radio_get_tx_agc()
03/03/11   sty     Used new variable names in ftm_1x_radio_get_tx_agc
03/02/11   aro     Renamed FTM and FTM-NS mode enums
03/01/11   aro     Updated RxAGC (dBm) conversion formula
02/16/11   aro     Added support to get RxLM and TxLM handle from FTM Data str
02/16/11   aro     [1] Added support to do FTM init for Radio Dispatch
                   [2] Added support to send BAD response packet
02/15/11   aro     Added support to pass LM handle to RFM interface
02/15/11   sty     Call rfm_1x_wakeup_tx instead of rfm_1x_ftm_enable_tx
02/14/11   sty     Call rfm_1x_ftm_enable_tx() instead of rfm_1x_ftm_set_tx_on()
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     [1] Updated LNA query interface
                   [2] Added support for Get TxAGC
02/12/11   aro     [1] Deleted Get LNA state interface to merge with Get RxAGC
                   [2] F3 message update
02/10/11   sty     Added TxLM buffer in call to rfm_1x_wakeup_tx()
02/08/11   aro     [1] Added LaTeX for RxAGC formula
                   [2] Created QDART Doxygen group
02/08/11   aro     Cleaned up redundant header file
02/08/11   aro     Moved the 1x Radio control to FTM
02/04/11   sty     Added support for sleep Rx and Tx for 1x
02/04/11   sty     Deleted ftm_hwtc_1x_set_tx
02/03/11   sty     Split 1x tx into 2 steps - enable modulator and then 
                   enable Tx
02/01/11   sty     Renamed rf_1x_enable_modulator to 
                   rf_1x_enable_modulator_and_tx
02/01/11   aro     Moved malloc function to FTM Common
01/31/11   bmg     Moved modulator hardware config to HAL
01/30/11   sty     Added encoder and HWIO writes to ftm_hwtc_1x_start_tx
01/27/11   aro     Added support for 1x retune
01/24/11   aro     Added LNA Range Query Interfaces
01/24/11   sty     Added ftm_cdma_pilot_waveform and ftm_hwtc_1x_start_tx
01/20/11   aro     Clean-up and doxygen update
01/19/11   sty     Fixed bug in type-casting
01/18/11   sty     Added ftm_hwtc_1x_radio_wakeup_tx
01/15/11   aro     Updated RxAGC calculation
01/13/11   aro     Updated rfm_1x_wakeup_rx() to pass buffer index
01/12/11   aro     Added Enable Firmware function call
01/08/11   aro     Added RXF Debug function
01/08/11   aro     Changed @ingroup to @addtogroup
01/07/11   aro     Renamed function names to reflect 1x
01/06/11   aro     Added RxAGC Query function
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
01/05/11   aro     Added Response Packets for HWTC CDMA
01/05/10   aro     Added Debug interface for rfm_1x_enter()
12/30/10   aro     Initial Release : Based on SVDO Design

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_cdma_packet_types.h"
#include "ftm_1x_radio.h"
#include "ftm_cdma_control.h"
#include "ftm_1x_control.h"
#include "rfm_1x.h"
#include "rfm_1x_ftm.h"
#include "rfm_cdma_ftm.h"
#include "ftm_cdma_data.h"
#include "ftm_msg.h"
#include "ftm_common_control.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_cdma_state_types.h"
#include "rfcommon_math.h"
#include "ftm_1x_msm.h"
#include "rfm_internal.h"
#include "rf_cdma_data.h"
#include "rf_cdma_mdsp.h"
#include "rfcommon_time_profile.h"
#include "rfm_common_sar.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_mdsp.h"
#include "rfcommon_mc.h"
#include "ftm_calibration_data_module.h"
/* This is included to access QPOET #def for min bias voltage */
#include "rfdevice_papm_intf.h"
#include "ftm_cdma_msm.h"
/*============================================================================*/
/*!
  @addtogroup RF_FTM_1X_QDART
  @image html ftm_hwtc_cdma_rsp_pkt_structure.jpg
  @image html ftm_hwtc_cdma_req_pkt_structure.jpg
  @{
*/

/*============================================================================*/
/*!
  @name 1x Radio Control Request Packets

  @brief
  This block includes all the Request Packet definitions to be used to send
  FTM commands related to CDMA 1x Radio Control
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_ENTER Command
*/
typedef ftm_cdma_req_7_type ftm_1x_radio_enter_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_ENABLE Command
*/
typedef ftm_cdma_req_2_type ftm_1x_radio_enable_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_DISABLE Command
*/
typedef ftm_cdma_req_7_type ftm_1x_radio_disable_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_WAKEUP_RX Command
*/
typedef ftm_cdma_req_2_type ftm_1x_radio_wakeup_rx_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_SLEEP_RX Command
*/
typedef ftm_cdma_req_7_type ftm_1x_radio_sleep_rx_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_RETUNE Command
*/
typedef ftm_cdma_req_2_type ftm_1x_radio_tune_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_ENABLE_DIV Command
*/
typedef ftm_cdma_req_9_type ftm_1x_radio_enable_diversity_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_DISABLE_DIV Command
*/
typedef ftm_cdma_req_7_type ftm_1x_radio_disable_diversity_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_WAKEUP_TX Command
*/
typedef ftm_cdma_req_7_type ftm_1x_radio_wakeup_tx_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_SLEEP_TX Command
*/
typedef ftm_cdma_req_7_type ftm_1x_radio_sleep_tx_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_SET_TX_POWER Command
*/
typedef ftm_cdma_req_5_type ftm_1x_radio_set_tx_power_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_CLEAR_LNA_OVERRIDE Command
*/
typedef ftm_cdma_req_1_type ftm_1x_radio_clear_lna_override_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_SET_PM_OVERRIDE Command
*/
typedef ftm_cdma_req_11_type ftm_1x_radio_power_mode_override_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_CLEAR_PM_OVERRIDE Command
*/
typedef ftm_cdma_req_1_type ftm_1x_radio_clear_power_mode_override_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_GET_PM Command
*/
typedef ftm_cdma_req_1_type ftm_1x_radio_get_power_mode_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_SET_TX_OVERRIDE Command
*/
typedef ftm_cdma_req_10_type ftm_1x_radio_set_tx_override_req_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_GET_RX_AGC Command
*/
typedef ftm_cdma_req_1_type ftm_1x_radio_get_rx_agc_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_GET_TX_AGC Command
*/
typedef ftm_cdma_req_1_type ftm_1x_radio_get_tx_agc_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_DRIVER_VER_SANDBOX Command
*/
typedef ftm_cdma_req_13_type ftm_1x_radio_driver_ver_sandbox_req_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_CONFIG_DEBUG_MODE Command
*/
typedef ftm_cdma_req_12_type ftm_1x_radio_config_debug_mode_req_type;

/*! \} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_CDMA_RADIO_TIMED_SLEEP Command
*/
typedef ftm_cdma_req_16_type ftm_1x_radio_timed_sleep_req_type;

/*============================================================================*/
/*!
  @name 1x Radio Control Response Packets

  @brief
  This block includes all the Response Packet definitions to be used to receive
  reponses for FTM commands related to CDMA 1x Radio Control
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_ENTER Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_enter_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_ENABLE Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_enable_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_DISABLE Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_disable_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_WAKEUP_RX Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_wakeup_rx_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_SLEEP_RX Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_sleep_rx_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_RETUNE Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_retune_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_ENABLE_DIV Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_enable_diversity_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_DISABLE_DIV Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_disable_diversity_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_WAKEUP_TX Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_wakeup_tx_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_SLEEP_TX Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_sleep_tx_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_SET_TX_POWER Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_set_tx_power_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_CLEAR_LNA_OVERRIDE Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_clear_lna_override_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_SET_PM_OVERRIDE Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_power_mode_override_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_CLEAR_PM_OVERRIDE Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_clear_power_mode_override_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_GET_PM Command
*/
typedef ftm_cdma_rsp_5_type ftm_1x_radio_get_power_mode_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_RADIO_SET_TX_OVERRIDE Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_set_tx_override_rsp_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_GET_RX_AGC Command
*/
typedef ftm_cdma_rsp_2_type ftm_1x_radio_get_rx_agc_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_1X_RADIO_GET_TX_AGC Command
*/
typedef ftm_cdma_rsp_3_type ftm_1x_radio_get_tx_agc_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_DRIVER_VER_SANDBOX Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_driver_ver_sandbox_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_CDMA_RADIO_TIMED_SLEEP Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_timed_sleep_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_1X_CONFIG_DEBUG_MODE Command
*/
typedef ftm_cdma_rsp_1_type ftm_1x_radio_config_debug_mode_rsp_type;

/*! \} */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_FTM_1X_RADIO
  @{
*/
/*============================================================================*/
/*!
  @name CDMA Radio Control Functions

  @brief
  Functions for FTM to control the radio in CDMA Mode of Operation
*/
/*! @{ */

static void ftm_1x_radio_timed_sleep_cb
(
  rfm_cb_event_enum_type     event,         /* Reason for RF callback */
  void*                     data           /* unused argument */
)
{
  switch ( event )
  {
    case RFM_1X_RX_TIMED_SLEEP_COMPLETE:
    {
      /* timed sleep is succeesful */
      ftm_cdma_data_set_timed_sleep_state( TRUE );
      break;
    }

    case RFM_EXECUTION_FAILURE:
    case RFM_1X_RX_TIMED_SLEEP_FW_FAILED:
    {
      /* timed sleep is unsucceesful */
      ftm_cdma_data_set_timed_sleep_state( FALSE );
      break;
    }

    default:
    {
      FTM_MSG_1( FTM_ERROR, "Unexpected RF event %d", event);
      return;

    }
  }
} /* timed_sleep_cb */

/*============================================================================*/
/*!
  @name CDMA 1x Radio Control Functions

  @brief
  Functions for FTM to control the radio in 1x Mode of Operation
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn on the radio and tune to the given band and channel on 1x Mode.

  @details
  This function calls RFM API to turn on the radio and tune to given
  band and channel in 1x Mode.

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_1x_radio_enter
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_enter_req_type* header = \
    (ftm_1x_radio_enter_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_enter_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  boolean init_ftm_env = FALSE;
  ftm_cdma_data_status_type dev_state; /* Var to hold current state */
  boolean rfm_ret = TRUE; /* Return val from RFM */

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_enter_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_enter: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Set up FTM environment for 1x */
  init_ftm_env = ftm_rfmode_enter( device, FTM_STATE_1X );

  (void)ftm_cdma_data_get_device_status( device, &dev_state );

  /* Call RFM Function to Wakeup Rx */
  if ( init_ftm_env == TRUE )
  {
    /* Tear Down RF to enter 1x mode */  
    if ( !ftm_1x_tear_down(device) )
    {
       /* Enter 1x Mode */
      rfm_ret &= ( rfm_enter_mode( device, RFM_1X_MODE, NULL, NULL,
                                   dev_state.rxlm_handle) == 0 );
    }
  }
  else
  {
    rf_status = -1;
  }

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_enter_rsp_type*)ftm_rsp_data.pkt_payload;

  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_enter : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Send back a BAD response packet for failures */
  if ( ( rf_status < 0 ) || ( rfm_ret == FALSE ) )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_1( FTM_LOW, "ftm_1x_radio_enter: Dev %d : [Done]", device );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_enter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable 1x radio

  @details
  Prepares the radio for 1x operation on an initial band and channel.
  The 1x subsystem is initialized in the Sleep state with the requested
  band and channel.

  @param *ftm_req_data
  FTM Request Data
 
  @return
  Diag Response packet
*/
ftm_rsp_pkt_type
ftm_1x_radio_enable
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_enable_req_type* header = 
    (ftm_1x_radio_enable_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_enable_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  int32 rf_status = 0; /* Response Variable*/
  rfm_device_enum_type device; /* hols the master and slave device */  
  sys_channel_type band_chan; /* holds band_chan of master device */
  lm_handle_type rxlm_handle;
  ftm_1x_rfm_callback_data_type cb_payload;
  ftm_cdma_data_status_type dev_status;
  boolean rfm_ret;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_enable_rsp_type) ); 

  /* Extract master and slave devices from input data capsule */
  device = (rfm_device_enum_type)header->device;

  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_enable: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  band_chan.band = (sys_band_class_e_type)header->band;
  band_chan.chan_num = (sys_channel_num_type)header->chan;
  
  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );
  rxlm_handle = dev_status.rxlm_handle;

  /* Call RFM Functions to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */

  rfm_ret =  ftm_cdma_control_enable_tech( RFM_1X_MODE,
                                         device, 
                                         band_chan,
                                         rxlm_handle, 
                                         &ftm_1x_rfm_callback, 
                                         &cb_payload );

  if ( rfm_ret == FALSE )
  {
    rf_status = RFM_CDMA_ERROR_FAILURE;
  }
  
  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_enable_rsp_type*)ftm_rsp_data.pkt_payload;
  
  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_enable : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  
  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_2( FTM_ERROR, "ftm_1x_radio_enable : Failed enable 1x on" 
               "device %d : Status =%d", device, ftm_rsp_data.cmd );

  } /* if ( rf_status < 0 ) */

  FTM_MSG_1( FTM_LOW, "ftm_1x_radio_enable : Dev %d : [Done]", 
             device );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_enable */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable 1x radio

  @details
  Deactivates the 1x portion of the CDMA RF driver.  This API should
  only be called if 1x is in the Sleep state.  If the radio was
  operating in wideband mode, it is reconfigured for narrowband mode.

  @param *ftm_req_data
  FTM Request Data
 
  @return
  Diag Response packet
*/
ftm_rsp_pkt_type
ftm_1x_radio_disable
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_disable_req_type* header = 
    (ftm_1x_radio_disable_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_disable_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  int32 rf_status = 0; /* Response Variable*/
  rfm_device_enum_type device; /* hols the master and slave device */  
  lm_handle_type rxlm_handle;
  ftm_1x_rfm_callback_data_type cb_payload;
  ftm_cdma_data_status_type dev_status;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_disable_rsp_type) ); 

  /* Extract master and slave devices from input data capsule */
  device = (rfm_device_enum_type)header->device;

  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_disable: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }
  
  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );
  rxlm_handle = dev_status.rxlm_handle;

  /* Call RFM Functions to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */
  rf_status = rfm_1x_disable( device, rxlm_handle, &ftm_1x_rfm_callback, 
                               &cb_payload );
  
  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_disable_rsp_type*)ftm_rsp_data.pkt_payload;
  
  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_disable : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  
  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_2( FTM_ERROR, "ftm_1x_radio_disable : Failed enable 1x on" 
               "device %d : Status =%d", device, ftm_rsp_data.cmd );

  } /* if ( rf_status < 0 ) */

  FTM_MSG_1( FTM_LOW, "ftm_1x_radio_disable : Dev %d : [Done]", 
             device );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_disable */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn on the radio and tune to the given band and channel on 1x Mode.

  @details
  This function calls RFM API to turn on the radio and tune to given
  band and channel in 1x Mode.

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_1x_radio_wakeup_rx
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_wakeup_rx_req_type* header = \
    (ftm_1x_radio_wakeup_rx_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_wakeup_rx_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  sys_channel_type band_chan;
  lm_handle_type rxlm_handle;
  ftm_cdma_data_status_type dev_status;
  ftm_1x_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_wakeup_rx_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_wakeup_rx: Unsupported device %d", 
               device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  band_chan.band = (sys_band_class_e_type)header->band;
  band_chan.chan_num = (sys_channel_num_type)header->chan;

  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );
  rxlm_handle = dev_status.rxlm_handle;
  
  /* Call RFM Function to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */
  rf_status = rfm_1x_prep_wakeup_rx( device, rxlm_handle, band_chan, 
                                     &ftm_1x_rfm_callback, &cb_payload );

  cb_payload.device = device; /* Populate the CB Payload */
  rf_status = rfm_1x_exec_wakeup_rx( device, rxlm_handle, &ftm_1x_rfm_callback,
                                     &cb_payload );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_wakeup_rx_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_wakeup_rx : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  else
  {
    /* Update FTM Data Structure with current band and chan */
    ftm_cdma_data_update_band_chan( device, 
                          rf_cdma_sys_band_type_to_rf_band_type(band_chan.band),
                          (rfm_cdma_chan_type )band_chan.chan_num );

    ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_RX );
  }

  FTM_MSG_4( FTM_LOW, "ftm_1x_radio_wakeup_rx: Dev %d, Band %d, Chan %d, "\
             " RxLM %d : [Done]", device, band_chan.band, band_chan.chan_num,\
             rxlm_handle );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn OFF the radio and put baseband to sleep for 1x Mode

  @details
  This function calls RFM API which in turn puts the RTR and MSM to sleep for 
  the given device

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_1x_radio_sleep_rx
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_sleep_rx_req_type* header = \
    (ftm_1x_radio_sleep_rx_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_sleep_rx_rsp_type *cdma_rsp; /*Packet Payload for each cmd*/

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  lm_handle_type rxlm_handle;
  ftm_cdma_data_status_type dev_status;
  ftm_1x_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_sleep_rx_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_sleep_rx: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );
  rxlm_handle = dev_status.rxlm_handle;

  /* Call RFM Function to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */
  rf_status = rfm_1x_sleep_rx( device, rxlm_handle, &ftm_1x_rfm_callback,
                               &cb_payload );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_sleep_rx_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_sleep_rx : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  else
  {
    /* Update FTM Data Structure with current band and chan */
    ftm_cdma_data_update_band_chan( device, FTM_CDMA_INVALID_BAND,
                                    FTM_CDMA_INVALID_CHAN );    
    ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_SLEEP );
  }

  FTM_MSG_2( FTM_LOW, "ftm_1x_radio_sleep_rx: Dev %d, RxLM %d : [Done]",\
             device, rxlm_handle );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_sleep_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retune the radio to the given band and channel on 1x Mode.

  @details
  This function calls RFM API to retune the radio to the given band and channel
  on 1x Mode.

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_1x_radio_retune
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_tune_req_type* header = \
    (ftm_1x_radio_tune_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_retune_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  sys_channel_type band_chan;
  lm_handle_type rxlm_handle;
  ftm_cdma_data_status_type dev_status;
  ftm_1x_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_retune_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_retune: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  band_chan.band = (sys_band_class_e_type)header->band;
  band_chan.chan_num = (sys_channel_num_type)header->chan;

  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );
  rxlm_handle = dev_status.rxlm_handle;
  
  /* Call RFM Function to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */

  /* Before Retune call L1 API to set Frame valid to False, if in RxTx state */
  if ( (dev_status.rf_state == FTM_CDMA_STATE_RXTX)&& (rfm_get_calibration_state() != TRUE) )
  {
    ftm_1x_enable_baseband_tx(FALSE);
  }

  rf_status = rfm_1x_retune( device, rxlm_handle, band_chan, 
                             &ftm_1x_rfm_callback, &cb_payload );

  /* After Retune call L1 API to set Frame valid to TRUE , this will 
     trigger FW to run RF on/off scripts, if in RxTx state*/ 
  if ( (dev_status.rf_state == FTM_CDMA_STATE_RXTX)&& (rfm_get_calibration_state() != TRUE)  )
  {
    ftm_1x_enable_baseband_tx(TRUE);
  }

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_retune_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_retune : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  else
  {
    /* If the retune was successful, update the FTM data with new
    band-chan info */
    ftm_cdma_data_update_band_chan( device, 
                          rf_cdma_sys_band_type_to_rf_band_type(band_chan.band),
                          (rfm_cdma_chan_type )band_chan.chan_num );  
  }
  
  FTM_MSG_4( FTM_LOW, "ftm_1x_radio_retune: Dev %d, Band %d, Chan %d, "\
             " RxLM %d : [Done]", device, band_chan.band, band_chan.chan_num,\
             rxlm_handle );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_retune */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable receive diversity for given Master device.

  @details
  This function calls RFM API to enable the given slave device as a receive
  diversity chain for the already tuned master device.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the enable diversity action
*/
ftm_rsp_pkt_type
ftm_1x_radio_enable_diversity
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_enable_diversity_req_type* header = \
    (ftm_1x_radio_enable_diversity_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_enable_diversity_rsp_type *cdma_rsp; /*Pkt Payload for each cmd*/

  /* Request packet for enter_mode API */
  ftm_1x_radio_enter_req_type data_for_1x_enter_mode;
  
  /* response type enter_mode API */
  ftm_rsp_pkt_type rsp_for_1x_enter_mode;

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device_m, device_s;
  lm_handle_type rxlm_handle_m, rxlm_handle_s;
  ftm_cdma_data_status_type dev_status_m, dev_status_s;
  ftm_1x_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_enable_diversity_rsp_type) ); 

  /* Extract Useful Data */
  device_m = (rfm_device_enum_type)header->device_1;
  device_s = (rfm_device_enum_type)header->device_2;

  /* do a enter_mode on the diversity device since, slave device at this point
     is in parked_mode, and hence, LM handles are invalid 
  */
  data_for_1x_enter_mode.device = device_s;

  rsp_for_1x_enter_mode = ftm_1x_radio_enter(&data_for_1x_enter_mode);

  if (rsp_for_1x_enter_mode.cmd != FTM_RSP_BAD_PARM)
  {
     /* need to update the state of slave device - so get a pointer to it */
     ftm_cdma_data_status_type* slave_dev_ptr = 
                                        ftm_cdma_data_get_device_ptr(device_s);

     /* need to associate Master and slave devices - so get a pointer to it */
     ftm_cdma_data_status_type* master_dev_ptr = 
                                        ftm_cdma_data_get_device_ptr(device_m);

     /* check for valid device ptr */
     if (slave_dev_ptr == NULL)
     {
       FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_enable_diversity: Failed to get a "
                             "valid device pointer for device %d", device_s );

       /* enable_diversity failed - so report error */
       ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;

       return ftm_rsp_data;
     }

     /* check for valid device ptr */
     if (master_dev_ptr == NULL)
     {
       FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_enable_diversity: Failed to get a "
                             "valid device pointer for device %d", device_m );

       /* enable_diversity failed - so report error */
       ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;

       return ftm_rsp_data;
     }

    /* Get RF Device Status */
    ftm_cdma_data_get_device_status( device_m, &dev_status_m );
    ftm_cdma_data_get_device_status( device_s, &dev_status_s );

    rxlm_handle_m = dev_status_m.rxlm_handle;
    rxlm_handle_s = dev_status_s.rxlm_handle;
    
    /* Call RFM Function to Wakeup Rx */
    cb_payload.device = device_m; /* Populate the CB Payload */
    rf_status = rfm_1x_enable_diversity( device_m, rxlm_handle_m, device_s,
                                         rxlm_handle_s, &ftm_1x_rfm_callback,
                                         &cb_payload );
    
    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
    cdma_rsp =(ftm_1x_radio_enable_diversity_rsp_type*)ftm_rsp_data.pkt_payload;
    
    if( cdma_rsp != NULL )
    {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
    }
    else
    {
      FTM_MSG( FTM_ERROR, "ftm_1x_radio_enable_diversity : NULL cdma_rsp" );
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    }
    
    /* Check if the eveything in the function executed or not. If not send
    back a BAD response packet */
    if ( rf_status < 0 )
    {
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    }
    else
    {
      /* ALL operations succeeded - proceed to update ftm_cdma_data */
      slave_dev_ptr->assoc_dev = device_m;
      slave_dev_ptr->curr_band = dev_status_m.curr_band;
      slave_dev_ptr->curr_chan = dev_status_m.curr_chan;
      slave_dev_ptr->rf_state  = FTM_CDMA_STATE_RXDIV;

      master_dev_ptr->assoc_dev = device_s;

      FTM_MSG_4( FTM_LOW, "ftm_1x_radio_enable_diversity: DevM %d, RxLMm %d, "
                 " DevS %d, RxLMs %d : [Done]", device_m, dev_status_m.rxlm_handle, 
                 device_s, dev_status_s.rxlm_handle );

    }
  } /* (rsp_for_1x_enter_mode.cmd != FTM_RSP_BAD_PARM) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "ftm_1x_radio_enable_diversity : Failed enter mode on" 
               "device %d : Status =%d", device_s, ftm_rsp_data.cmd);

    /* enable_diversity failed - so report error */
    ftm_rsp_data.cmd = FTM_RSP_BAD_MODE;
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_enable_diversity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable receive diversity for given Master device.

  @details
  This function calls RFM API to disable the given slave device as a receive
  diversity chain for the already tuned master device.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the disable diversity action
*/
ftm_rsp_pkt_type
ftm_1x_radio_disable_diversity
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_disable_diversity_req_type* header = 
    (ftm_1x_radio_disable_diversity_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_disable_diversity_rsp_type *cdma_rsp; /*Pkt Payload */

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device_s;
  lm_handle_type rxlm_handle_s;
  ftm_cdma_data_status_type dev_status_s;
  ftm_1x_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_disable_diversity_rsp_type) ); 

  /* Extract Useful Data */
  device_s = (rfm_device_enum_type)header->device;
  if ( device_s >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_disable_diversity: Unsupported device %d", device_s );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device_s, &dev_status_s );
  rxlm_handle_s = dev_status_s.rxlm_handle;

  /* Call RFM Function to Wakeup Rx */
  cb_payload.device = device_s; /* Populate the CB Payload */
  rf_status = rfm_1x_disable_diversity( device_s, rxlm_handle_s, 
                                        &ftm_1x_rfm_callback, &cb_payload );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_disable_diversity_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_disable_diversity : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  
  FTM_MSG_2( FTM_LOW, "ftm_1x_radio_disable_diversity: "
             "DevS %d, RxLMs %d : [Done]", device_s, rxlm_handle_s );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_disable_diversity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn on the Tx path for a given device

  @details
  This function calls RFM API to enable Tx for a given device. The APIs for
  rfm_enter_mode and rfm_enable_rx should have already been executed for the
  device

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_1x_radio_wakeup_tx
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_wakeup_tx_req_type* header = \
    (ftm_1x_radio_wakeup_tx_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;                  /* Packet returned to Diag */
  ftm_1x_radio_wakeup_tx_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  int32 rf_status;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  ftm_1x_rfm_callback_data_type cb_payload;

  /* pointer to FTM data structure for CDMA */
  ftm_cdma_data_status_type device_status;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_wakeup_tx_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_wakeup_tx: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* get a pointer to ftm_cdma_data */
  if (ftm_cdma_data_get_device_status(device, &device_status))
  {
    /* Step 1: force 1x into traffic mode */
    rfm_1x_set_pa_access_probe_config( device, TRUE );

    /* Set frequency adjustment parameters to default for FTM 
    Tx wakeup. Freq error is initialized to zero */
    (void)rfm_1x_ftm_set_tx_freq_adjust( 0 );

    /* Step 2: configure and enable Tx - use 0 for callback and userdata */
    cb_payload.device = device; /* Populate the CB Payload */
    rf_status = rfm_1x_wakeup_tx( device, device_status.txlm_handle, 
                                  &ftm_1x_rfm_callback, &cb_payload );

    /* If wakeup_tx fails, skip call flow after it */
    if( rf_status >= 0 )
    {
      void* smem_addr = ftm_cdma_data_1x_get_fw_smem_addr();
      if (smem_addr != NULL) 
      {
        /* Step 3: configure and enable modulator for 1x */
        ftm_1x_enable_modulator( device, smem_addr );
      }
      else
      {
        rf_status = -1;
        FTM_MSG( FTM_ERROR, "ftm_1x_radio_wakeup_tx: fw shared memory "
                            "ptr is NULL" );
      }
    }
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_wakeup_tx: rfm_1x_wakeup_tx faiure %d",
                            rf_status );
    }

  }
  else
  {
    rf_status = -1;
  }

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_wakeup_tx_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_wakeup_tx : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  else
  {
    /* Update FTM Data Structure with current band and chan */
    ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_RXTX );
  }

  FTM_MSG_2( FTM_LOW, "ftm_1x_radio_wakeup_tx: Dev %d, TxLM %d : [Done]",\
             device, device_status.txlm_handle );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_wakeup_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn OFF the radio and put baseband to sleep for 1x Mode

  @details
  This function calls RFM API which in turn puts the RTR and MSM to sleep for 
  the given device

  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/
ftm_rsp_pkt_type
ftm_1x_radio_sleep_tx
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_sleep_tx_req_type* header = \
    (ftm_1x_radio_sleep_tx_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_sleep_tx_rsp_type *cdma_rsp; /*Packet Payload for each cmd*/

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;

  /* holds the device status */
  ftm_cdma_data_status_type dev_status;
  ftm_1x_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_sleep_tx_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_sleep_tx: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Get RF Device Status */
  if (ftm_cdma_data_get_device_status( device, &dev_status ))
  {
    /* Mimic the L1 sequence here  */

    /* Step 1: disable modulator for 1x */
    ftm_1x_disable_modulator( device );

    /* Step 2: disable Tx - use 0 for callback and userdata */
    cb_payload.device = device; /* Populate the CB Payload */
    rf_status = rfm_1x_sleep_tx( device, dev_status.txlm_handle, 
                                 &ftm_1x_rfm_callback, &cb_payload);

    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
    cdma_rsp = (ftm_1x_radio_sleep_tx_rsp_type*)ftm_rsp_data.pkt_payload;
    
  if( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_sleep_tx : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
    
    /* Check if the eveything in the function executed or not. If not send
    back a BAD response packet */
    if ( rf_status < 0 )
    {
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    }
    else
    {
      /* Update FTM Data Structure with current band and chan */
      ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_RX );
    }

    FTM_MSG_2( FTM_LOW, "ftm_1x_radio_sleep_tx: Dev %d, TxLM %d : [Done]",\
               device, dev_status.txlm_handle );

  } /* if (ftm_cdma_data_get_device_status( device, &dev_status )) */
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_sleep_tx: Error in device lookup");
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_sleep_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the Tx Power

  @details
  This function sets Open Loop or Closed Loop Transmit Power Level depending 
  on the parameter passed. This API takes OL or CL Tx Power in dBm.

  @param *ftm_req_data FTM Request Data
 
  @return
  Response Packet for the command
*/
ftm_rsp_pkt_type
ftm_1x_radio_set_tx_power
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_set_tx_power_req_type* header = \
    (ftm_1x_radio_set_tx_power_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_set_tx_power_rsp_type *cdma_rsp; /*Packet Payload for each cmd*/

  /* Define : Response Variable*/
  int32 rf_status = 0;

  ftm_cdma_data_status_type dev_status;  /* points to ftm device status */

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  rfm_cdma_tx_pwr_mode_type tx_pwr_mode;
  int16 tx_pwr_dBm10; /* Tx power in 1/10th of a dBm */
  int16 tx_pwr_mdsp_units;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_set_tx_power_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_set_tx_power: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  tx_pwr_mode = (rfm_cdma_tx_pwr_mode_type)header->tx_pwr_mode;
  tx_pwr_dBm10 = (int16)header->tx_pwr;
    
  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );

  /* Tx AGC (mdsp units) = -640*(Tx_dBm - K - 63.8)
                 = -64*(Tx_dBm10 - 10*K - 638) */
  tx_pwr_mdsp_units = rf_cdma_mdsp_dbm10_to_txagc(
                tx_pwr_dBm10,
                10*rfm_cdma_ftm_get_tc
                (rf_cdma_rf_band_type_to_sys_band_type(dev_status.curr_band)) );

  FTM_MSG_2( FTM_LOW, "ftm_1x_radio_set_tx_power:  TX_Raw %d, "
             "TxPwr(dBm10) %d", tx_pwr_mdsp_units, tx_pwr_dBm10 );
  if ( tx_pwr_mode == RFM_CDMA_TX_PWR_OPEN_LOOP )
  {
    FTM_MSG ( FTM_LOW, "ftm_1x_radio_set_tx_power(): PM is RFM_CDMA_TX_PWR_OPEN_LOOP" ) ;
  }
  else if ( tx_pwr_mode == RFM_CDMA_TX_PWR_CLOSED_LOOP )
  {
    FTM_MSG ( FTM_LOW, "ftm_1x_radio_set_tx_power(): PM is RFM_CDMA_TX_PWR_CLOSED_LOOP" ) ;
  }

  /* Call RFM Function to set Tx power */
  rf_status = rfm_1x_ftm_set_tx_pwr ( device, tx_pwr_mode, tx_pwr_mdsp_units ) ;

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_set_tx_power_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_set_tx_power : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status == TRUE  )
  {
    FTM_MSG_3( FTM_LOW, "ftm_1x_radio_set_tx_power: Dev %d, TxMode %d, "\
               "TxPwr(dBm10) %d : [Done]", device, tx_pwr_mode, tx_pwr_dBm10 );
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_3( FTM_ERROR, "ftm_1x_radio_set_tx_power: Dev %d, TxMode %d, "\
               "TxPwr(dBm10) %d : [Fail]", device, tx_pwr_mode, tx_pwr_dBm10 );
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_set_tx_power */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Tx Override

  @details
  This function sets Tx to override Mode. This will override the PA State,
  TxAVGA, PA Bias, and BetaP gain values.

  @param *ftm_req_data FTM Request Data
 
  @return
  Response Packet for the command
*/
ftm_rsp_pkt_type
ftm_1x_radio_set_tx_override
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_set_tx_override_req_type* header = \
    (ftm_1x_radio_set_tx_override_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_set_tx_override_rsp_type *cdma_rsp; /*Packet Payload*/

  /* Define : Response Variable*/
  int32 rf_status = 1;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  rfm_pa_gain_state_type pa_state;
  uint16 avga_idx;
  int16 betap_gain;
  uint16 pa_bias;
  uint16 pa_q_current;
  boolean pa_q_current_enable;

  ftm_cdma_rsp_type clear_pa_r_status;

  ftm_cdma_data_status_type* dev_ptr; /* ptr to device status */

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_set_tx_override_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_set_tx_override: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  pa_state = (rfm_pa_gain_state_type)header->pa_state;
  avga_idx = header->avga_index;
  betap_gain = header->betap_gain;
  pa_bias = header->pa_bias;
  pa_q_current = header->pa_q_current;
  pa_q_current_enable = header->pa_q_current_enable;

  dev_ptr = ftm_cdma_data_get_device_ptr(device);

  /* check for valid device ptr */
  if (dev_ptr == NULL)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_set_tx_override: Failed to get a valid "
                          "device pointer for device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* If PA Range is currently overriden, clear the override before
  executing Tx override. This will ensure that PA state is overriden as
  expected and maintains consistency */
  clear_pa_r_status = ftm_1x_clear_pa_range_override(device);

  /* Call RFM Function to set Tx power */
  if ( clear_pa_r_status == 1 )
  {
    rf_status = rfm_1x_ftm_set_fw_tx_agc_override( device, dev_ptr->curr_band,
                                                   pa_state, avga_idx,
                                                   betap_gain, pa_bias, 
                                                   pa_q_current, 
                                                   pa_q_current_enable );
  }

  /* Update FTM status information */
  dev_ptr->override_data.pa_state = pa_state;
  dev_ptr->override_data.tx_avga_idx = avga_idx;
  dev_ptr->override_data.apt_smps_bias_val = pa_bias;
  dev_ptr->override_data.apt_pa_q_current = pa_q_current;
  dev_ptr->override_data.apt_pa_q_current_enable = pa_q_current_enable;

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_set_tx_override_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_set_tx_override : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_1( FTM_LOW, "ftm_1x_radio_set_tx_override: Dev %d [Done]",device );

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_set_tx_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clear LNA State Override

  @details
  This function clears the LNA state machine from override Mode.

  @param *ftm_req_data FTM Request Data
 
  @return
  Response Packet for the command
*/
ftm_rsp_pkt_type
ftm_1x_radio_clear_lna_override
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_clear_lna_override_req_type* header = \
    (ftm_1x_radio_clear_lna_override_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_clear_lna_override_rsp_type *cdma_rsp; /*Packet Payload*/

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_clear_lna_override_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
    
  /* Call RFM Function to set Tx power */
  rf_status = rfm_1x_ftm_set_rx_gain_state( device, FALSE, 0 );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_clear_lna_override_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cdma_rsp != NULL )
  {
  /* Update the Response Packet Field */
  cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_clear_lna_override : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }


  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_1( FTM_LOW, "ftm_1x_radio_clear_lna_override: Dev %d [Done]",device );

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_clear_lna_override */

/*! \} */

/*============================================================================*/
/*!
  @name CDMA 1x Radio Query Functions

  @brief
  Functions for FTM to query 1x Data
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get 1x RxAGC Data

  @details
  This function calls RFM API to query the current RxAGC on given Device.
  Following formula is used to calculated the 1x RxAGC in dBm units.
 
  The returned CDMA Rx AGC setting is in 8 bit two's complement format 
  (-128...127). Rx signal strength is:
 
  \f[
    RxAGC_{dBm} = -63.248 + \frac{RxAGC_{raw} * 334}{1000}
  \f]
 
  That's -0.334dB per count referenced to -63.248dBm.  Since this returns
  int8, this is a numeric Rx signal strength from -106dBm to -20.83dBm.
  When 102 dB dynamic range is used the AGC value is scaled into the
  85 dB range.  AGC values for powers greater than -21 dBm are saturated
  to -21 dBm (127).  AGC values for powers less than -106 dBm are saturated
  to -106 dBm (-128).
 
  Beside RxAGC, this function also return the current LNA state and current
  LNA gain word that is programmed to RTR.
 
  @param *ftm_req_data FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
 
  @todo RxAGC Query : Formula Should be re-implemented for nikeL since RxAGC
  units may be different as 1x RxAGC has moved to FW in nikeL
*/
ftm_rsp_pkt_type
ftm_1x_radio_get_rx_agc
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_get_rx_agc_req_type* header = \
    (ftm_1x_radio_get_rx_agc_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_get_rx_agc_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;
  rfm_cdma_power_mode_type power_mode;
  rfm_cdma_power_mode_type jammer_state;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  boolean rfm_res;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_get_rx_agc_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  
  /* Call RFM Function to query LNA Data */
  rfm_res = rfm_1x_ftm_get_rx_agc_data( device, &rx_agc_data, RFM_CDMA_AGC_FORMAT__DBM10);
  
  if (rfm_res == TRUE)
  {
    /* Update linearity info */
    power_mode = rfm_cdma_ftm_get_power_mode( device );
    jammer_state = rfm_cdma_ftm_get_jammer_state( device );

    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
    cdma_rsp = (ftm_1x_radio_get_rx_agc_rsp_type*)ftm_rsp_data.pkt_payload;

    if( cdma_rsp != NULL )
    {
      /* Update the Response Packet Field */
      cdma_rsp->rx_agc = rx_agc_data.rx_agc[0];
      cdma_rsp->lna_state = (uint8)rx_agc_data.gain_state;
      cdma_rsp->lna_gain_word = (uint8)rx_agc_data.gain_word;
      cdma_rsp->ic_state = (uint8)power_mode;
      cdma_rsp->jammer_state = (uint8)jammer_state;
    
      FTM_MSG_6( FTM_LOW, "ftm_1x_radio_get_rx_agc: Dev %d, RxAGC %d, "
                 "LNA %d, LNA Word %d, PM %d, Jammer Det %d", 
                 device, rx_agc_data.rx_agc[0], 
                 rx_agc_data.gain_state, rx_agc_data.gain_word, 
                 power_mode, jammer_state );
    }
    else
    {
      FTM_MSG( FTM_ERROR, "ftm_1x_radio_get_rx_agc : NULL cdma_rsp" );
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    }
  } /* if (rfm_res == TRUE) */
  else
  {
      FTM_MSG( FTM_ERROR, "ftm_1x_radio_get_rx_agc : NULL cdma_rsp" );
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_get_rx_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get 1x TxAGC Data

  @details
  This function calls RFM API to query the current TxAGC data on given Device.
  Formula to convert raw TxAGC to dBm: 
  open-loop power(dBm) = K + 63.8 - (TxAGC)*102.4/65536
  K = turn-around constant for band
  TxAGC value is returned in dBm100

  @param *ftm_req_data FTM Request Data
 
  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_1x_radio_get_tx_agc
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_radio_get_tx_agc_req_type* header = 
    (ftm_1x_radio_get_tx_agc_req_type*)ftm_req_data;
 
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;              /* Packet returned to Diag */
  ftm_1x_radio_get_tx_agc_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  rfm_cdma_ftm_tx_agc_data_type txagc_data;   /* Define : Response Variable*/
  
  rfm_device_enum_type device; /* Define : To Extract Useful Data */  
  
  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_1x_radio_get_tx_agc_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  
  /* Call RFM Function to query TxAGC Data */
  rfm_1x_ftm_get_tx_agc_data( device, &txagc_data, 
                              RFM_CDMA_AGC_FORMAT__DBM10 );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_1x_radio_get_tx_agc_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cdma_rsp != NULL)
  {
  /* Update the Response Packet Field */
  cdma_rsp->avga_index    = txagc_data.avga_index;
  cdma_rsp->avga_val      = txagc_data.avga_val;
  cdma_rsp->betap_gain    = txagc_data.betap_gain;
  cdma_rsp->pa_state      = txagc_data.pa_state;
  cdma_rsp->apt_val       = txagc_data.apt_val;
  cdma_rsp->txagc_dBm10  = txagc_data.open_loop_power;

  FTM_MSG_7( FTM_LOW, "ftm_1x_radio_get_tx_agc: Dev %d AVGA idx %d, "
             "AVGA Val 0x%x, BetapGain %d, PA State %d, Apt Val %u, "
             "TxPwr(dBm10) %d",
             device, txagc_data.avga_index,
             txagc_data.avga_val, txagc_data.betap_gain,
             txagc_data.pa_state, txagc_data.apt_val, txagc_data.open_loop_power );
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_get_tx_agc : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
      
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_get_tx_agc */

/*! \} */

/*============================================================================*/
/*!
  @name CDMA 1x Cal v3 Functions

  @brief
  Functions for FTM Calibration v3
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure CDMA Radio for Calibration

  @details
  This function configures CDMA radio to RxTx state for calibration. Based on
  the enabled master devices list and their corresponding slave devices, radio
  will be configures to RxTx with diversity mode. However, this function sets
  the Tx power to the lowest possible to avoid false trigger to the calibration
  equipment. Therefore before enabling Tx, TxAGC output is overriden to lowest
  power possible.
 
  After Tx is enabled, HDET is also enabled. HDET will be disabled only at the
  end of the calibration sweep.
 
  Since this command will be sent by embedded side itself, the response
  packet will be created in such a way that no response is sent back to DIAG.

  @param *ftm_req_data
  FTM Request Data to configure radio
 
  @return
  Response packet for the command indicating that there is no response to DIAG.
*/
ftm_rsp_pkt_type
ftm_1x_calv3_configure_radio
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_cdma_cal_sweep_configure_radio_req_type* header = 
    (ftm_cdma_cal_sweep_configure_radio_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;

  rfm_device_enum_type m_device; /* Master Device */
  rfm_device_enum_type s_device = RFM_INVALID_DEVICE; /* Slave device */
  uint8 s_device_num; /* Slave device Number */
  sys_channel_type band_chan; /* Band and channel */
  ftm_cdma_data_status_type *m_dev_status = NULL; /* current state of master device*/
  ftm_cdma_data_status_type *s_dev_status = NULL;/* current state of slave device*/
  uint32 m_dev_mask = 0xFFFFFFFF; /* Variable to store extracted device Mask */
  uint32 m_dev_list; /* Variable to store the Master device list. This will be 
  modified in the while loop. */
  boolean rfm_ret = TRUE; /* Return val from RFM */
  uint32 extract_s_mask = 0xF; /* Mask to be used to extract the slave
  device for the given master device */
  uint8 rgi_min; /* Minimum AVGA Index */
  uint8 rgi_max; /* Maximum AVGA Index */
  boolean is_s_dev_configured = FALSE; /* Flag indicating if slave device
  is configured at the end of this function */
  ftm_1x_rfm_callback_data_type cb_payload;
  ftm_cdma_cal_v3_data_type *cal_v3_data;

  rf_time_tick_type exec_time;      /* Warmup Time */
  rf_time_type exec_time_val;

  /* Start Profiling of Rx Warm Up */
  exec_time = rf_time_get_tick();

  /* Since this funciton should be called internaly only, there should be
  no response packet. */
  ftm_rsp_data = ftm_cdma_create_rsp_pkt( FTM_CDMA_NO_RSP );

  /* Get the master device mask */
  m_dev_list = (uint8)header->device_master_mask;

  /* For the current unprocessed master device Flag . The first bit will be 
  from LSB. */
  m_dev_mask = rf_extract_first_one_bit_mask( m_dev_list );

  /* Continue only if the device mask is non-zero */
  if ( m_dev_mask != 0 )
  {
    /* Loop until, the master device Mask comes out to be zero */
    while( m_dev_mask != 0 )
    {
      /* Based on Master device mask, decide the master device  */
      switch( m_dev_mask )
      {
      case DEVICE_FLAG_0:
        m_device = RFM_DEVICE_0;
        break;
  
      case DEVICE_FLAG_1:
        m_device = RFM_DEVICE_1;
        break;
  
      case DEVICE_FLAG_2:
        m_device = RFM_DEVICE_2;
        break;
  
      case DEVICE_FLAG_3:
        m_device = RFM_DEVICE_3;
        break;
  
      default:
        m_device = RFM_INVALID_DEVICE;
        FTM_MSG_1( FTM_ERROR, "Cal v3 ftm_1x_calv3_configure_radio: "
                   "Invalid device Mask 0x%x", m_dev_mask );
        break;
      }
  
      /* If the device is not invalid perform tuning */
      if ( m_device != RFM_INVALID_DEVICE )
      {
        /* Query the current RF State Ptr for master device */
        m_dev_status = ftm_cdma_data_get_device_ptr(m_device);
  
        if( m_dev_status == NULL )
        {
          FTM_MSG_1(FTM_ERROR,"ftm_1x_calv3_configure_radio: NULL"
            "data for device %d",m_device);
        }
        else
        {
          /* Poupulate band and channel */
          band_chan.band = header->band;
          band_chan.chan_num = header->channel;
    
          /* Enter FTM to 1x Mode */
          rfm_ret &= ( ftm_rfmode_enter( m_device, FTM_STATE_1X ) == 1 );
    
          /* Tear Down RF to enter 1x mode */  
          if ( !ftm_1x_tear_down(m_device) )
          {
            /* Enter 1x Mode */
            rfm_ret &= ( rfm_enter_mode( m_device, RFM_1X_MODE, NULL, NULL,
                                         m_dev_status->rxlm_handle) == 0 );
            cb_payload.device = m_device; /* Populate the CB Payload */

            rfm_ret &= ftm_cdma_control_enable_tech( RFM_1X_MODE,
                                                   m_device, 
                                                   band_chan,
                                                   m_dev_status->rxlm_handle, 
                                                   &ftm_1x_rfm_callback, 
                                                   &cb_payload );
          }
    
          /* Enter 1x Mode */
          rfm_ret &= ( rfm_enter_mode( m_device, RFM_1X_MODE, NULL, NULL,
                                       m_dev_status->rxlm_handle) == 0 );

          cb_payload.device = m_device; /* Populate the CB Payload */
          
          rfm_ret &= ftm_cdma_control_enable_tech( RFM_1X_MODE,
                                                 m_device, 
                                                 band_chan,
                                                 m_dev_status->rxlm_handle, 
                                                 &ftm_1x_rfm_callback, 
                                                 &cb_payload );

          /* Make sure enable and enter mode went through before tune */
          if ( rfm_ret != FALSE )
          {
            /* Perform Wakeup Rx preparation */
            cb_payload.device = m_device; /* Populate the CB Payload */
            rfm_ret &= ( rfm_1x_prep_wakeup_rx( m_device, 
                                                m_dev_status->rxlm_handle,
                                                band_chan, &ftm_1x_rfm_callback, 
                                                &cb_payload ) >= 0 );
      
            /* Perform wakeup Rx execution */
            cb_payload.device = m_device; /* Populate the CB Payload */
            rfm_ret &= ( rfm_1x_exec_wakeup_rx( m_device, 
                                                m_dev_status->rxlm_handle,
                                                &ftm_1x_rfm_callback,
                                                &cb_payload ) >= 0 );
        
            /* Get min and max AVGA table index - this is a one-time thing */
            rfm_ret &= rfm_cdma_ftm_get_min_max_avga_idx( m_device, &rgi_min,
                                                          &rgi_max);
      
            /* Set lowest Tx AGC - set lowest index for AVGA to get the lowest
            possible Tx power */ 
            /* For min Tx power the bias voltage that should be sent should come 
            from the PA driver, this is the minimum possible bias voltage that can 
            be set by the QPOET */ 
            rfm_ret &= rfm_1x_ftm_set_fw_tx_agc_override( m_device, header->band,
                                             RFM_PA_GAIN_STATE_0,
                                             rgi_min, 0,/*betaP*/ 
                                             RFDEVICE_PAPM_MIN_TX_BIAS_VOLTAGE, 
                                             0/*quiscent current*/, 
                                             FALSE); 

            /* Set frequency adjustment parameters to default for FTM 
            Tx wakeup. Freq error is initialized to zero */
            rfm_ret &= rfm_1x_ftm_set_tx_freq_adjust( 0 );

            /* Perform wakeup Tx */
            cb_payload.device = m_device; /* Populate the CB Payload */
            rfm_ret &= ( rfm_1x_wakeup_tx( m_device, m_dev_status->txlm_handle, 
                                           &ftm_1x_rfm_callback, 
                                           &cb_payload ) >= 0 );
            
          /* Override the Intelliciever Power Mode to HL: This is done as a part
          of Rx Calibration time optimization. During the RF Config segment,
          PM will always be set to HL.
          - For one PM calibration, calibration will be done on HL and static
            offset will be applied to LL. Thus since we are overriding PM to HL
            in RF Config OpCode, there would be no need to set PM in Rx Config
            segment. This gives us a savings to 2ms of calibration time per
            Rx Segment. PM override will be skipped in Rx Config segment, if tools
            always sends same PM (in this case HL) nin Rx Config segment. Thus
            for one PM calibration, tools will always send PM as HL.
          - However, if both PM calibration is to be done, then tools will send
            appropriate PM in Rx Config segment. Thus if there is difference
            between last PM and new PM, then Rx Config segment will set the new
            PM. Thus there is a need to keep track of current PM in cal v3. This
            will be done by a variable in FTM CDMA CAL v3 data.
            */    
             rfm_cdma_ftm_power_mode_override( m_device, 
                                               RFM_CDMA_POWER_MODE_NORMAL );

             /* Get Pointer to Cal v3 data */
             cal_v3_data = ftm_cdma_data_get_cal_v3_adr( m_device );

             /* NULL pointer check */
             if ( cal_v3_data != NULL )
             {
               cal_v3_data->curr_power_mode = RFM_CDMA_POWER_MODE_NORMAL;
             }
             else
             {
               FTM_MSG_1( FTM_ERROR, "Cal v3 ftm_1x_calv3_configure_radio: "
                          "Invalid Cal data for dev %d", m_device );
             }

             if ( rfm_ret == TRUE )
             { 
               void* smem_addr = ftm_cdma_data_1x_get_fw_smem_addr();
               if (smem_addr != NULL) 
               {
                 /* Configure and enable modulator for 1x */
                 ftm_1x_enable_modulator( m_device, smem_addr );
               }
               else
               {
                 rfm_ret = FALSE;
                 FTM_MSG( FTM_ERROR, "ftm_1x_calv3_configure_radio: fw shared  "
                                     "memory ptr is NULL" );
               }

               /* Enable HDET */
               rfm_ret &= ( rfm_cdma_ftm_enable_hdet( m_device ) == 1 );

             }
             else
             {
               FTM_MSG_1( FTM_ERROR, "ftm_1x_calv3_configure_radio: Both "
                            "enable_modulator and enable_hdet are skipped for "
                            "mDev %d", m_device );
             }  
      
            /* Update the extract Mask */
            extract_s_mask = extract_s_mask << (m_device*4);
            
            /* Extract 4 bits from device_slave corresponding to the master 
            device*/
            s_device_num = 
              (uint8)((extract_s_mask & header->device_slave) >> (m_device*4));
            
            /* Perform diversity tuning only if slave device is not 0xFF or less
            than RFM_INVALID_DEVICE */
            if ( s_device_num != 0xF )
            {
              /* typecasr thr slave num to device */
              s_device = (rfm_device_enum_type)s_device_num;
      
              /* Query the current RF State Ptr for slave device */
              s_dev_status = ftm_cdma_data_get_device_ptr(s_device);
      
                if( s_dev_status == NULL )
                {
                  FTM_MSG_1(FTM_ERROR,"ftm_1x_calv3_configure_radio:"
                    "NULL data for device %d",s_device);
                }
                else
                {    
                  /* Enter FTM to 1x mode for sloave device */
                  rfm_ret &= ( ftm_rfmode_enter( s_device, FTM_STATE_1X ) == 1 );
      
                  /* Tear Down RF to enter 1x mode */  
                  if ( !ftm_1x_tear_down(s_device) )
                  {
                   /* Enter 1x Mode */
                   rfm_ret &= ( rfm_enter_mode( s_device, RFM_1X_MODE, NULL, 
                                                NULL,
                                             s_dev_status->rxlm_handle) == 0 );
                  }
              
                  /* Enable diversity */
                  cb_payload.device = m_device; /* Populate the CB Payload */
                  rfm_ret &= ( rfm_1x_enable_diversity( m_device,
                                   m_dev_status->rxlm_handle, s_device,
                                   s_dev_status->rxlm_handle, 
                                   &ftm_1x_rfm_callback,
                                   &cb_payload ) >= 0 );
      
                  is_s_dev_configured = TRUE;
      
                }  /*if( s_dev_status == NULL )*/
          
            } /*if ( s_device_num != 0xF ) */
          }/* if ( rfm_ret != FALSE ) */
          else
          {
            FTM_MSG_2( FTM_ERROR, "ftm_1x_calv3_configure_radio: Enter mode/"
                                  "enable mode failed for device:%d on band %d "
                                  , m_device ,band_chan.band );
          }
        } /*if( m_dev_status == NULL )*/

      } /* if ( m_device != RFM_INVALID_DEVICE ) */
  
      /* Update status based on RFM success */
      if ( rfm_ret == FALSE )
      {
        /* Since RFM failed, tear down RF for both devices */
        ftm_1x_tear_down( m_device );

        if ( is_s_dev_configured == TRUE )
        {
          ftm_1x_tear_down( s_device );
        } /* if ( is_s_dev_configured == TRUE ) */

        FTM_MSG( FTM_FATAL, "Cal v3 ftm_1x_calv3_configure_radio: RFM Failed" );
      } /* if ( rfm_ret == FALSE ) */
      else
      {
        if ( m_dev_status != NULL )
        {
          /* Update the device status for master device */
          m_dev_status->curr_band = band_chan.band;
          m_dev_status->curr_chan = band_chan.chan_num;
          m_dev_status->rf_state = FTM_CDMA_STATE_RXTX;

          if ( s_dev_status != NULL )
          {
            /* If slave device is configured, then update the device status for
            slave device as well */
            if ( is_s_dev_configured == TRUE )
            {
              m_dev_status->assoc_dev = s_device;
              s_dev_status->curr_band = band_chan.band;
              s_dev_status->curr_chan = band_chan.chan_num;
              s_dev_status->rf_state = FTM_CDMA_STATE_RXDIV;
              s_dev_status->assoc_dev = m_device;
            }
          } /* if ( s_dev_status != NULL ) */
        } /* if ( m_dev_status != NULL ) */
      } /* if ( rfm_ret != TRUE ) */

      /* Update Cal Action to set the processed bit to zero. */
      m_dev_list = m_dev_list & (~m_dev_mask);
  
      /* For the current unprocessed master device Flag . The first bit will be 
      from LSB. */
      m_dev_mask = rf_extract_first_one_bit_mask( m_dev_list );
      
    } /* while( m_dev_mask != 0 ) */

    FTM_MSG_4 ( FTM_MED, "Cal v3 ftm_1x_calv3_configure_radio: Radio "
                "Configured - mDev Mask 0x%x, Slave 0x%x, Band %d, Chan %d",
                header->device_master_mask, header->device_slave, header->band,
                header->channel );
  }
  else
  {
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_1x_calv3_configure_radio: Empty Device "
                        "Mask" );
  } /* if ( m_dev_mask != 0 ) */

  /* Record the execution time statistics for wakeup logging for this device*/
  exec_time_val = rf_time_get_elapsed( exec_time, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  FTM_MSG_2( FTM_HIGH, "Cal v3 ftm_1x_calv3_configure_radio: RF Success %d, "
            "Cal v3 Profile %d uS", rfm_ret, exec_time_val );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_calv3_configure_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retune CDMA Radio for Calibration

  @details
  This function retunes CDMA radio to new channel. Based on the enabled master 
  devices list, all master devices and their associated devices will be
  retuned to a new CDMA channel. This function sets the Tx power to the lowest 
  possible to avoid false trigger to the calibration equipment.
 
  HDET state will not be altered as HDET will be enabled till the end of the
  calibration sweep.
 
  Since this command will be sent by embedded side itself, the response
  packet will be created in such a way that no response is sent back to DIAG.

  @param *ftm_req_data
  FTM Request Data to retune radio
 
  @return
  Response packet for the command indicating that there is no response to DIAG.
*/
ftm_rsp_pkt_type
ftm_1x_calv3_retune_radio
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_cdma_cal_sweep_retune_radio_req_type* header = 
    (ftm_cdma_cal_sweep_retune_radio_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;

  rfm_device_enum_type m_device; /* Master Device */

  rfm_device_enum_type s_device = RFM_INVALID_DEVICE; /* Slave device */
  sys_channel_type band_chan; /* Band and channel */
  ftm_cdma_data_status_type *m_dev_status = NULL; /* current state of master device*/
  ftm_cdma_data_status_type *s_dev_status = NULL;/* current state of slave device*/
  uint32 m_dev_mask = 0xFFFFFFFF; /* Variable to store extracted device Mask */
  uint32 m_dev_list; /* Variable to store the Master device list. This will be 
  modified in the while loop. */
  boolean rfm_ret = TRUE; /* Return val from RFM */
  rf_time_tick_type exec_time;      /* Warmup Time */
  uint8 rgi_min; /* Minimum AVGA Index */
  uint8 rgi_max; /* Maximum AVGA Index */
  ftm_1x_rfm_callback_data_type cb_payload;
  ftm_cdma_cal_v3_data_type *cal_v3_data;
  rf_time_type exec_time_val;

  /* Start Profiling of Rx Warm Up */
  exec_time = rf_time_get_tick();

  band_chan.band = 0;
  band_chan.chan_num = 0;

  /* Since this funciton should be called internaly only, there should be
  no response packet. */
  ftm_rsp_data = ftm_cdma_create_rsp_pkt( FTM_CDMA_NO_RSP );

  /* Get the master device mask */
  m_dev_list = (uint8)header->device_master_mask;

  /* For the current unprocessed master device Flag . The first bit will be 
  from LSB. */
  m_dev_mask = rf_extract_first_one_bit_mask( m_dev_list );

  /* Continue only if the device mask is non-zero */
  if ( m_dev_mask != 0 )
  {
    /* Loop until, the master device Mask comes out to be zero */
    while( m_dev_mask != 0 )
    {
      /* Based on Master device mask, decide the master device  */
      switch( m_dev_mask )
      {
      case DEVICE_FLAG_0:
        m_device = RFM_DEVICE_0;
        break;
  
      case DEVICE_FLAG_1:
        m_device = RFM_DEVICE_1;
        break;
  
      case DEVICE_FLAG_2:
        m_device = RFM_DEVICE_2;
        break;
  
      case DEVICE_FLAG_3:
        m_device = RFM_DEVICE_3;
        break;
  
      default:
        m_device = RFM_INVALID_DEVICE;
        FTM_MSG_1( FTM_ERROR, "Cal v3 ftm_1x_calv3_retune_radio: Invalid "
                   "device Mask 0x%x", m_dev_mask );
        break;
      } /* switch( m_dev_mask ) */
  
      /* If the device is not invalid perform tuning */
      if ( m_device != RFM_INVALID_DEVICE )
      {
        /* Query the current RF State Ptr for master device */
        m_dev_status = ftm_cdma_data_get_device_ptr(m_device);
  
        if( m_dev_status == NULL )
        {
          FTM_MSG_1(FTM_ERROR,"ftm_1x_calv3_retune_radio: NULL data for device %d",m_device);
        }
        else
        {  
        /* Poupulate band and channel */
        band_chan.band = 
        rf_cdma_rf_band_type_to_sys_band_type(m_dev_status->curr_band);
        band_chan.chan_num = header->channel;
  
        /* Get min and max AVGA table index - this is a one-time thing */
        rfm_ret &= rfm_cdma_ftm_get_min_max_avga_idx( m_device, &rgi_min,
                                                      &rgi_max);
  
        /* Set lowest Tx AGC - set lowest index for AVGA to get the lowest
        possible Tx power */ 
        /* For min Tx power the bias voltage that should be sent should come 
        from the PA driver, this is the minimum possible bias voltage that can 
        be set by the QPOET */ 
        rfm_ret &= rfm_1x_ftm_set_fw_tx_agc_override( m_device,
                                                     m_dev_status->curr_band,
                                                     RFM_PA_GAIN_STATE_0,
                                                     rgi_min, 0,/*betaP*/
                                                     RFDEVICE_PAPM_MIN_TX_BIAS_VOLTAGE, 
                                                     0,/* Quiscent current*/ 
                                                     FALSE);

        /* Disable HDET */
        rfm_ret &= ( rfm_cdma_ftm_disable_hdet( m_device ) == 1 );

        cb_payload.device = m_device; /* Populate the CB Payload */
		
        /* Before Retune call L1 API to set Frame valid to False */
        if ((m_dev_status->rf_state == FTM_CDMA_STATE_RXTX)&&(rfm_get_calibration_state() != TRUE))
        {
          ftm_1x_enable_baseband_tx( FALSE );
        }

        rfm_ret &= (rfm_1x_retune( m_device, m_dev_status->rxlm_handle,
                                   band_chan, &ftm_1x_rfm_callback, 
                                   &cb_payload ) >= 0 );
  
         /* After Retune call L1 API to set Frame valid to TRUE , this will 
           trigger FW to run RF on/off scripts*/ 
        if ((m_dev_status->rf_state == FTM_CDMA_STATE_RXTX)&&(rfm_get_calibration_state() != TRUE))
        {
          ftm_1x_enable_baseband_tx(TRUE);
        }
        
        /* Override the Intelliciever Power Mode to HL: This is done as a part
        of Rx Calibration time optimization. During the RF Config segment,
        PM will always be set to HL.
        - For one PM calibration, calibration will be done on HL and static
          offset will be applied to LL. Thus since we are overriding PM to HL
          in RF Config OpCode, there would be no need to set PM in Rx Config
          segment. This gives us a savings to 2ms of calibration time per
          Rx Segment. PM override will be skipped in Rx Config segment, if tools
          always sends same PM (in this case HL) nin Rx Config segment. Thus
          for one PM calibration, tools will always send PM as HL.
        - However, if both PM calibration is to be done, then tools will send
          appropriate PM in Rx Config segment. Thus if there is difference
          between last PM and new PM, then Rx Config segment will set the new
          PM. Thus there is a need to keep track of current PM in cal v3. This
          will be done by a variable in FTM CDMA CAL v3 data.
        */    
         rfm_cdma_ftm_power_mode_override( m_device, 
                                           RFM_CDMA_POWER_MODE_NORMAL );

         /* Get Pointer to Cal v3 data */
         cal_v3_data = ftm_cdma_data_get_cal_v3_adr( m_device );

         /* NULL pointer check */
         if ( cal_v3_data != NULL )
         {
           cal_v3_data->curr_power_mode = RFM_CDMA_POWER_MODE_NORMAL;
         }
         else
         {
           FTM_MSG_1( FTM_ERROR, "Cal v3 ftm_1x_calv3_retune_radio: "
                                 "Invalid Cal data for dev %d", m_device );
         }
          
        /* Enable HDET */
        rfm_ret &= ( rfm_cdma_ftm_enable_hdet( m_device ) == 1 );
  
        } /*if( m_dev_status == NULL )*/
  
      } /* if ( m_device != RFM_INVALID_DEVICE ) */
  
      /* Update status based on RFM success */
      if ( rfm_ret == FALSE )
      {
        /* Since RFM failed, tear down RF for both devices */

        FTM_MSG( FTM_FATAL, "Cal v3 ftm_1x_calv3_retune_radio: RFM Failed" );
      } /* if ( rfm_ret == FALSE ) */
      else
      {
        if ( m_dev_status != NULL )
        {
          /* Update the device status for master device */
          m_dev_status->curr_band = band_chan.band;
          m_dev_status->curr_chan = band_chan.chan_num;

          if ( m_dev_status->assoc_dev != RFM_INVALID_DEVICE )
          {
            s_device = m_dev_status->assoc_dev;

            /* Query the current RF State Ptr for master device */
            s_dev_status = ftm_cdma_data_get_device_ptr(s_device);

            /* NULL Pointer check */
            if ( s_dev_status != NULL )
            {
              if ( s_dev_status->rf_state == FTM_CDMA_STATE_RXDIV )
              {
                s_dev_status->curr_band = band_chan.band;
                s_dev_status->curr_chan = band_chan.chan_num;
              } /* if ( s_dev_status->rf_state == FTM_CDMA_STATE_RXDIV ) */
            } /* if ( s_dev_status != NULL ) */
          } /* if ( m_dev_status->assoc_dev != RFM_INVALID_DEVICE ) */
        } /* if ( m_dev_status != NULL ) */

      } /* if ( rfm_ret != TRUE ) */

      /* Update Cal Action to set the processed bit to zero. */
      m_dev_list = m_dev_list & (~m_dev_mask);
  
      /* For the current unprocessed master device Flag . The first bit will be 
      from LSB. */
      m_dev_mask = rf_extract_first_one_bit_mask( m_dev_list );
      
    } /* while( m_dev_mask != 0 ) */
  
    FTM_MSG_2( FTM_MED, "Cal v3 ftm_1x_calv3_retune_radio: Radio Configured - "
               "mDev Mask 0x%x, Chan %d", header->device_master_mask, 
               header->channel ); }
  else
  {
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_1x_calv3_retune_radio: Empty Device Mask" );
  }

  /* Record the execution time statistics for wakeup logging for this device*/
  exec_time_val = rf_time_get_elapsed( exec_time, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  FTM_MSG_2( FTM_HIGH, "Cal v3 ftm_1x_calv3_retune_radio: RF Success %d, "
            "Cal v3 Profile %d uS", rfm_ret, exec_time_val );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_calv3_retune_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tear down CDMA Radio after Calibration

  @details
  This function tears down CDMA radio to SLEEP state. This function
  also disabled HDET at the end of tear down.
  
  Since this command will be sent by embedded side itself, the response
  packet will be created in such a way that no response is sent back to DIAG.

  @param *ftm_req_data
  FTM Request Data to tear down radio
 
  @return
  Response packet for the command indicating that there is no response to DIAG.
*/
ftm_rsp_pkt_type
ftm_1x_calv3_tear_down_radio
(
  void  *ftm_req_data
)
{
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;

  rfm_device_enum_type device_idx; /* Master Device */
  rfm_mode_enum_type curr_mode;
  ftm_cdma_data_status_type *ftm_dev_status = NULL; /* FTM Device Status */
  rf_time_tick_type exec_time;      /* Warmup Time */
  const rf_cdma_data_status_type *dev_status = NULL; /* RF Dev Status */
  ftm_cdma_cal_v3_data_type *cal_data;
  rf_time_type exec_time_val;

  /* Start Profiling of Rx Warm Up */
  exec_time = rf_time_get_tick();

  /* Since this funciton should be called internaly only, there should be
  no response packet. */
  ftm_rsp_data = ftm_cdma_create_rsp_pkt( FTM_CDMA_NO_RSP );
  
  for ( device_idx = RFM_DEVICE_0; device_idx < RFM_MAX_DEVICES; device_idx++ )
  {
    /* Get Current RFM Mode */
    curr_mode = rfm_get_current_mode( device_idx );

    /* Tear down only if the mode is 1x */
    if ( curr_mode == RFM_1X_MODE )
    {
      ftm_dev_status = ftm_cdma_data_get_device_ptr(device_idx);

      /* Get Calibration Data */
      cal_data = ftm_cdma_data_get_cal_v3_adr(device_idx);

      /* Query the current RF State Ptr for master device */
      if ( ftm_dev_status != NULL )
      {
        dev_status = rf_cdma_get_device_status(device_idx);
        
        if ( dev_status != NULL )
        {
          /* Disable HDET, if Radio is in RxTx state */
          if ( dev_status->data_1x.rf_state == RF_CDMA_STATE_RXTX )
          {
            rfm_cdma_ftm_disable_hdet(device_idx);
          }
        }
      } /* if ( ftm_dev_status != NULL ) */
      else
      {
        FTM_MSG_1( FTM_HIGH, "ftm_1x_calv3_tear_down_radio: Dev %d NULL Data",
                   device_idx );
      } /* if ! ( ftm_dev_status != NULL ) */

      /* Tear down Radio */
      ftm_1x_tear_down(device_idx);

      /* Invalidate Current Power Mode */
      cal_data->curr_power_mode = RFM_CDMA_POWER_MODE_INVALID;
    }
  } /*(device_idx = RFM_DEVICE_0; device_idx < RFM_MAX_DEVICES; device_idx++)*/

  /* Tear down the data module */
  ftm_cal_data_teardown_data_module();
  /* Tear down the ET delay table */
  ftm_cal_data_teardown_delay_map();
  /* Tear down the pin table */
  ftm_cal_data_teardown_pin_map();

  FTM_MSG( FTM_MED, "Cal v3 ftm_1x_calv3_tear_down_radio: CDMA radio torn "
                    "down" );
  /* Record the execution time statistics for wakeup logging for this device*/
  exec_time_val = rf_time_get_elapsed( exec_time, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  FTM_MSG_1( FTM_HIGH, "Cal v3 ftm_1x_calv3_tear_down_radio: Cal v3"
                       "Profile %d uS ", exec_time_val );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_calv3_tear_down_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Schedule timed rx sleep 

  @details
  This api calls rfm_1x_timed_sleep_rx to schedule timed rx sleep

  @param device
  Device on which requested mode needs to be enabled
 
  @param rtc_cx1_trigger
  scheduled sleep time in cx1 unit
 
  @return
  Response packet for the command
 
*/
ftm_rsp_pkt_type
ftm_1x_radio_timed_sleep
(
  void  *ftm_req_data
)
{
   boolean ret_val; /* return value */
   rfm_mode_enum_type curr_mode; /* Var to store current mode */
   rfm_wait_time_t rfm_1x_sleep_rx_ret_val; 
   uint32 rtc_cx1_trigger;

  /* Request Packet */
  ftm_1x_radio_timed_sleep_req_type* header =
    (ftm_1x_radio_timed_sleep_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_1x_radio_timed_sleep_rsp_type *cdma_rsp; /* Packet Payload for
                                                         each cmd */

  /* Extract info from payload */
  rfm_device_enum_type device = header->device;
  uint32 rtc_cx1_trigger_offset = header->rtc_cx1_trigger_offset;

  /* Device Status read Pointer*/
  ftm_cdma_data_status_type *dev_status = ftm_cdma_data_get_device_ptr(device); 
            
  /* Create a new reponse packet in the ftm heap. This will also update first
  10-bytes field in Packet Payload in the FTM response packet. These fields
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2),
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                         sizeof(ftm_1x_radio_timed_sleep_rsp_type) );

  if ( dev_status != NULL )
  {
    /* Get Current Mode */
    curr_mode = rfm_get_current_mode(device);

    if ( curr_mode == RFM_1X_MODE )
    {
      /* Device must be in Rx state */
      if ( dev_status->rf_state == FTM_CDMA_STATE_RX )
      {
        /* target rtc time to sleep in cx1 unit */
        rtc_cx1_trigger = ftm_cdma_msm_get_curr_rtc_cx1() 
                          + rtc_cx1_trigger_offset;

        /* Send command to turn off the receiver in future time */
        /* After FW receives timed rx stop msg, it has a small time window:
        [CDMA1X_TRIG_TIME_MARGIN0_CX1(64) CDMA1X_TRIG_TIME_MARGIN1_CX1(16384)], 
        and it takes about 300 cx1 from now to FW receiving rx_stop msg. In 
        order to make FW accept RF timed sleep request, the input timed 
        sleep offset should be in the range [64+300 16384+300]  */
        (void)rfm_1x_timed_sleep_rx( device, 
                                     dev_status->rxlm_handle, 
                                     rtc_cx1_trigger, 
                                     TRUE,
                                     &ftm_1x_radio_timed_sleep_cb,
                                     NULL );
        
        /* Check if FW accepts timed sleep request */
        if ( ftm_cdma_data_get_timed_sleep_state() == TRUE ) 
        {
          /* timed sleep is successful, set state */
          dev_status->rf_state = FTM_CDMA_STATE_SLEEP;
          ret_val = TRUE;
          FTM_MSG_1( FTM_MED, "ftm_1x_radio_timed_sleep: FW accepted timed" 
               " sleep with rtc_cx1_trigger: %x ", rtc_cx1_trigger );

          ret_val = rfm_1x_ftm_stop_oqpch_srch(device);

          if (ret_val == FALSE) 
           {
            ret_val = TRUE;
            FTM_MSG( FTM_ERROR, "ftm_1x_radio_timed_sleep: Failed to send OQPCH "
                                "SEARCH msg to FW ");
          }
        }
        else
        {
          /* timed sleep is unsuccessful, call regular sleep rx */
          rfm_1x_sleep_rx_ret_val = rfm_1x_sleep_rx( device, 
                                                     dev_status->rxlm_handle, 
                                                     NULL,
                                                     NULL );
          if (rfm_1x_sleep_rx_ret_val >= RFM_CDMA_HEALTHY_STATE) 
          {
            ret_val = TRUE;
            FTM_MSG_1( FTM_MED, "ftm_1x_radio_timed_sleep: FW refused timed" 
               " sleep with rtc_cx1_trigger: %x, regular sleep rx was executed "
               "successfully ", rtc_cx1_trigger );
            dev_status->rf_state = FTM_CDMA_STATE_SLEEP;
          }
          else
          {
            ret_val = FALSE;
            FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_timed_sleep: FW refused timed" 
               " sleep with rtc_cx1_trigger: %x, regular sleep rx failed too ",
               rtc_cx1_trigger );
          }
        }
      }
      else
      {
        FTM_MSG_2( FTM_ERROR, "ftm_1x_radio_timed_sleep: invalid device state "
                            " %d on device:%d",dev_status->rf_state, 
                           device );
        ret_val = FALSE;
      }
    }/* if ( dev_status_r->rf_mode == RFM_1X_MODE ) */
    else
    {
      FTM_MSG_2( FTM_ERROR, "ftm_1x_radio_timed_sleep: timed rx sleep is not "
                          "supported for tech %d on device:%d",
                          curr_mode, device );
      ret_val = FALSE;
    }
  }/* if ( dev_status != NULL )*/
  else
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_radio_timed_sleep: NULL data for "
                        "device:%d", device );
    ret_val = FALSE;
  }

 /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp =
      (ftm_1x_radio_timed_sleep_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Update the Response Packet Field */
  if(NULL != cdma_rsp)
  {
    cdma_rsp->rf_status = ret_val;
  }

  /* Send back a BAD response packet for failures */
  if ( ret_val == FALSE )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_1x_radio_timed_sleep: [Failed]" );
  }
  else
  {
    FTM_MSG( FTM_LOW, "ftm_cdma_1x_timed_sleep: [Success]" );
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_radio_timed_sleep */

#endif /* FEATURE_CDMA1X */
/*! \} */

#endif /*FEATURE_FACTORY_TESTMODE */

/*! @} */

