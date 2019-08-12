#ifndef RF_CDMA_MDSP_TYPES_H
#define RF_CDMA_MDSP_TYPES_H

/*! 
  @file
  rf_cdma_mdsp_types.h
 
  @brief
  This module defines the 1xHDR defintions used by 1XHDR RF-MDSP
  Internal(RF) interface.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_mdsp_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/01/15   vr      Synchronized Therm Read with TxAGC event
01/16/15   vr     CDMA predictive clipping NV based power thresh support
11/10/14   zhw     Dynamic Predictive Clipping support
08/01/14   cd      Add support for FTM AGC logging with FED and common tx AGC 
                   functionality
07/21/14   cd      Add predictive clipper support for 1x, DO rev0, DO revA
05/02/14   cd      8-PA state functionality updates
04/16/14   cd      Cleanup unused MDSP interfaces and Tx AGC functionality
03/13/14   spa     Add flag for update_xpt_mode in pa_stm_data
01/20/14   sty     remove deprecated FW vars 
01/17/14   JJ      Deleted Settling Time for 1x & HDR, FW gets it from FED now
01/08/14   cd      Provide xPT type to RFLM per PA state
01/06/14   cd      Linearizer and Tx AGC updates for loading xPT information to 
                   RFLM
12/26/13   sar/hdz Remove et_txlm_hanlde from Bolt c2k code, and use one single 
                   TxLm handle allocation.
12/06/13   cd      Load nominal baseband gains to RFLM
12/03/13   sar/hdz Remove et_txlm_hanlde from Bolt c2k code, and use one single 
                   TxLm handle allocation.
10/16/13   cc      Add new ET logging for FTM AGC logs
10/03/13   cd      Hook up PA switchpoint loading to RFLM
06/24/13   zhw     Temporarily featurize template code for Triton
06/07/13   zhw     Define max no. of writes using unified FW interface 
04/28/13   APU     Added CCS event-info point to mdsp data.
04/23/13   APU     Removed old unused iqgain_params and incoporated CC comments.
04/08/13   zhw     Update max number of QPOET writes supported
03/28/13   zhw     Clean up obsolete pa_range and apt_val FW interface
03/27/13   ry      Added new structure rf_cdma_mdsp_jd_config_type
03/07/13   sr      changed the "pwr_tracker" name to "papm".
02/22/13   Saul    CDMA. Config xpt mode based on dpd info in linearizer.
01/30/13   APU     Brought over Enhanced PA backoff code from Nikel.
01/22/13   aro     Added common XPT capture type
01/16/13   Saul    CDMA. Enabled EPT/ET tempcomp.
01/14/13   Saul    CDMA. 1x EPT/ET call support.
12/20/12   zhw     Integrate GRFC PA device scripting to pa_state_script callflow
                   #defines that remap FW template type to RF CDMA MDSP type
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/11/12   zhw     Support for GRFC PA scripting in TxAGC CFG
12/05/12   spa     Add support for QPOET bias settings in linearizer table
11/29/12   zhw     Remove reference to tx_chain in tx_start/stop msg
11/12/12   zhw     Load FW mdsp interface with PA Q current RFFE writes
11/15/12   Saul    CDMA. TX AGC XPT.
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type.
11/02/12   zhw     Remove deprecated pa stm data members 
10/29/12   zhw     Support for query device_config from RFC and device driver
10/29/12   spa     Changed GRFC reset bitmask to a single value and not array 
10/23/12   spa     Added GRFC set/reset bitmasks to pa stm data
10/16/12   sty     Removed all refs to FEATURE_RF_HAS_QFE1320
10/16/12   zhw     TxAGC Timing info type cast correction
09/19/12   Saul    CDMA Sample Capture. Capture type
09/17/12   APU     Take neq power in deciding the HDET power mode. 
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/22/12   cri     EPT support for CDMA FW interfaces
08/17/12   aro     Added memory address param in rf_cdma_mdsp_ept_config_type
08/01/12   spa     Changed definition to RF_CDMA_MDSP_TYPES_H
07/16/12   sty     Include rf_cdma_mdsp_data.h instead of rf_cdma_mdsp.h
07/12/12   cri     CDMA interface changes for EPT
07/09/12   cd      Added IQ gain offset field to FW interface for EPT temp comp
06/11/12   aro     Added MDSP path/device to EPT configure function
06/05/12   anr     Initial EPT Support
05/10/12   zw      Renamed rtr_handle to script_index 
05/02/12   aro     Support for unique sequence number generation to be used for
                   RF-FW message synchronization
04/16/12   Saul    QFE. Fix strip/pack errors.
04/13/12   Saul    QFE. Renamed rfqfe_cdma_X to qfe_cdma_X
04/11/12   Saul    QFE. Initial support.
03/08/12   cd      Data structures to configure Tx AGC with GTR Thresh 
                   information
03/02/12   Saul    MCDO. Rx AGC freq comp and device droop compensation framework.
02/29/12   cd      Fixed pa_hyst_timer interface type. Should be uint16
01/04/12   aro     Added interface to query LNA phase offsets for current
                   configuration
12/20/11   aro     Documentation update for APT value
12/14/11   aro     Migrated CDMA MDSP data to CDMA Data module
12/01/11   cd      Added reverse link IQ gain params type for 1x HDR
11/10/11   Saul    SV Limiting. Initial support.
11/04/11   cd      Add enum to indicate FW Tx mode of operation
10/24/11   hdz     Added rf_cdma_mdsp_therm_read_data_type 
09/07/11   aro     Updated RX_STOP interface to pass the RTC time to stop Rx
09/07/11   shb     Reworked TX AGC Logical data to have carrier and cumulative 
                   total power
09/06/11   aro     Added Update Intelliceiver data type
09/01/11   cd      Add APT support
                   - Provide FW interface definitions
08/19/11   shb     Added #define for const to convert time in us to cx1
08/19/11   shb     Added read_success flag in MDSP hdet read data.
08/05/11   shb     Added struct to store HDET read data from MDSP - 
                   common for 1x/HDR
07/21/11   cd      Added definitions for max/min Rx power in RxAGC units
06/06/11   aro     Removed hardcoding of PA Range Map
05/24/11   aro     Updated interface to send Tx Chain and Rx Chain to MDSP
05/21/11   aro     Added Invalid MDSP invalid chain
05/18/11   sty     Added more members to rf_cdma_mdsp_pa_stm_config_type
05/16/11   aro     Changed the type of LNA rise and fall to int16
04/26/11   cd      RF_CDMA_MDSP_TX_LIN_TBL_SIZ now maps to the same value as 
                   NV_CDMA_TX_PDM_LIN_TYPE
04/25/11   aro     Added Tx Start and Tx Stop Messages
04/11/11   aro     Added structure for Rx Stop Configuration 
04/11/11   aro     Renamed Start rx interface
04/08/11   aro     Added RxAGC Start Config type
03/31/11   aro     Added Tx Pilot Power in Read Interface
03/31/11   aro     [1] Compiler Error Fix
                   [2] Mainlined SVDO feature
03/31/11   aro     Removed unused types
03/31/11   aro     Added TxAVGA Index Read Interface
03/24/11   aro     Defined unique RxAGC Read structure for 1x
03/25/11   aro     Compiler Warning Fix
03/24/11   aro     Added BTF in PA STM configuration Structure
03/16/11   aro     Added Rx SBI Bus Param in Configure RxAGC
03/16/11   aro     Changed Device Param to MDSP Chain
03/16/11   aro     Renamed enum to RF_CDMA_TX_LINEARIZER_NUM
03/16/11   aro     Moved function prototype to new file
03/15/11   aro     Added consolidated TxAGC Read Interface
03/15/11   sty     Deleted unused #define RF_CDMA_MDSP_MIN_TX_PWR_DBM10
03/15/11   aro     Added support for consolidated Tx Linearizer
03/15/11   aro     Consolidated Tx configuration to PA STM and TX
                   Linearizer interfaces
03/14/11   aro     Added RxAGC Read structure
03/14/11   aro     Merged the DVGA and LNA offset to Gain Calibration Data
03/14/11   sty     Changed RF_CDMA_MDSP_INVALID_TX_MIN_PWR to 
                   RF_CDMA_MDSP_MIN_TX_PWR (same for MAX)
                   Modified doxygen comments
03/13/11   sty     Renamed #define to RF_CDMA_MDSP_MIN_TX_PWR_DBM10
                   Changed arg &return type:rf_common_conv_txpwr_to_txagc_units
03/10/11   sty     Added prototype for rf_common_conv_pwr_to_mdsp_units
                   Added mdsp-related #defines
03/02/11   aro     Re-structured the RxAGC Param type
03/03/11   sty     Added C++ protection
03/02/11   sty     Renamed rf1xhdr_* to rf_cdma_*
03/02/11   sty     Renamed rf1xhdr_mdsp_pa_param_type to 
                   rf_cdma_mdsp_pa_param_type
02/25/11   aro     Added support for DVGA offset
01/19/11   aro     Added LNA Param type
01/18/11   sty     Renamed rf1xhdr_mdsp_tx_sbi_param_type to 
                   rf_cdma_mdsp_tx_sbi_param_type
12/10/10   aro     Enabling SVDO Flavor
07/20/10   zg      Rotator API implementation.
07/16/10   zg      Added support for SSMA.
05/13/10   aro     Removed unused TypeDef
05/04/10   aro     Renamed PA SMPS Variable Name
04/09/10   aro     Initial Revision.

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_wb_rxf.h"
#include "rfm_gain_state_types.h"
#include "lm_types.h"
#include "rf_1x_mdsp_data.h"
#include "rf_hdr_mdsp_data.h"
#include "rf_cdma_mdsp_data.h"
#include "rfm_cdma_carrier_types.h"
#include "rfcommon_core_xpt.h"
#include "rf_cdma_mdsp_ccs_events.h"
#include "rfcommon_core_txlin.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the min raw TxAGC that can be programmed to mdsp
*/
#define RF_CDMA_MDSP_MIN_TX_PWR                                           -32768

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  This number represents the max raw TxAGC that can be programmed to mdsp
*/
#define RF_CDMA_MDSP_MAX_TX_PWR                                            32767

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  This number represents the min raw RxAGC value that can be programmed to mdsp
*/
#define RF_CDMA_MDSP_MIN_RX_PWR                                           -32768

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  This number represents the max raw RxAGC value that can be programmed to mdsp
*/
#define RF_CDMA_MDSP_MAX_RX_PWR                                            32767

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  This number represents the max value that can be programmed to
  FW APT SMPS bias val
*/
#define RF_CDMA_MDSP_MAX_APT_SMPS_BIAS_VAL                                  1023

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  This number represents the min signed 16-bit value that can be programmed to
  FW APT SMPS bias val
*/
#define RF_CDMA_MDSP_MIN_APT_SMPS_BIAS_VAL                                     0

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  This number is used as a terminator in the Power table that is read from NV. 
  An entry in the power table with -32768 (0x8000) indicates that the entry is 
  invalid and should not be loaded to FW
*/
#define RF_CDMA_MDSP_NV_PWR_DELIMITER                                     -32768

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  This number is used as a terminator in the LUT table that is read from NV. 
  An entry in the power table with 32767 indicates that the entry is invalid 
  and should not be loaded to FW
*/
#define RF_CDMA_MDSP_NV_LUT_DELIMITER                                      32767

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Defines the size of linearizer tables in mdsp
*/
#define RF_CDMA_MDSP_TX_LIN_TBL_SIZ                      NV_CDMA_TX_PDM_LIN_TYPE
                                                                 
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Maximum Number of Multi-carriers supported 
*/
#define RF_CDMA_MDSP_MAX_NUM_OF_CARRIERS                                       3 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Multiply by this constant to convert microsec to cx1 (x256) units

  @details
  (round)(1.2288 << 8). After multiplying by this number, divide by 256 and 
  convert to int to get approximate translation of a value in microseconds to
  cx1
*/
#define RF_CDMA_MDSP_CX1_MICROSEC_CONV_X256                                  315

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Number of reverse link configurations supported for IQ gain updates in 1x
  mode
*/
#define RF_CDMA_MDSP_TX_IQGAIN_1X_NUM            RF_CDMA_NV_1X_TX_IQGAIN_RC_NUM

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Number of reverse link configurations supported for IQ gain updates in HDR
  mode
*/
#define RF_CDMA_MDSP_TX_IQGAIN_HDR_NUM          RF_CDMA_NV_HDR_TX_IQGAIN_RC_NUM

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Per Carrier Frequency Offset Parameter
*/
typedef enum
{

  RF_CDMA_MDSP_CHAIN_0, /*!< MDSP Chain 0 */

  RF_CDMA_MDSP_CHAIN_1, /*!< MDSP Chain 1 */

  RF_CDMA_MDSP_CHAIN_NUM, /*!< Maximum number of MDSP Chain */

  RF_CDMA_MDSP_CHAIN_INVALID /*!< Invalid number of MDSP Chain */


} rf_cdma_mdsp_chain_enum_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Define Tx Override Mode type provided to FW
*/
typedef enum
{
  RF_CDMA_MDSP_TX_MODE_NORMAL, /*!< Normal Tx operation */

  RF_CDMA_MDSP_TX_MODE_CAL_OVERRIDE, /*!< Calibration Override Mode */

  RF_CDMA_MDSP_TX_MODE_INVALID /*!< Invalid FW Tx Mode */
} rf_cdma_mdsp_tx_mode_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to used to return TxAGC Data reads on Logical Side
*/
typedef struct
{

  int16 open_loop_power;
  /*!< Tx Open Loop Power. The uint of value read from the interface -1/640dB
  in 102.4 dB Mode. This will be of range -32768 to +32767 */

  int16 closed_loop_power;
  /*!< Tx Closed Loop Power */

  int16 rate_adjust;
  /*!< Tx Rate Adjust */

  int16 total_carrier_power;
  /*!< Tx Total Power for a given carrier */

  int16 total_power;
  /*!< Tx Total Power (across all carriers for HDR) */

  int16 current_max_total_pwr ;
  /*!< Tx Total Power (across all carriers for HDR) 
    while in funnel mode of operation */

  int16 pilot_power;
  /*!< Tx Pilot Power */

  uint8 pa_state;
  /*!< PA State in the STM */

  int16 tx_min;
  /*!< Tx Min Limit */

  int16 tx_max;
  /*!< Tx Max Limit */

  int16 total_tx_filtered_power;
  /*!< Total Tx Filtered Power */

} rf_cdma_mdsp_tx_agc_logical_read_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to used to return TxAGC Data reads on Physical Side
*/
typedef struct
{

  uint32 tx_avga;
  /*!< Tx AVGA value */

  uint16 cal_bias_rgi_index;
  /*!< Calibrated linearizer RGI Index */

  uint16 full_bias_rgi_index;
  /*!< Full bias linearizer RGI Index */

  int16 betap;
  /*!< BetaPrime Value */

  uint8 xpt_mode;
  /*!< XPT mode 0: APT 1: EPT 2: ET */

  int16 env_scale_gain;
  /*!< envelop scale gain read from HW */

  int16 iq_gain;
  /*!< IQ gain read from HW */

  int32 xpt_delay;
  /*!< envelop delay */

} rf_cdma_mdsp_tx_agc_physical_read_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold CDMA RxAGC Param
*/
typedef struct
{
  uint8 wideband_index;
  /*!< Wideband index for which the RxAGC is to be configured */

  uint8 sbi_bus;
  /*!< Rx SBI Bus */

  uint8 sbi_addr;
  /*!< Address of SBI Register to which LNA State info is to be written */

  uint8 gain_word[RFM_LNA_GAIN_STATE_NUM];
  /*!< LNA Gain word per Gain State to be written to sbi_addr */

  int16 rise_val[RFM_LNA_GAIN_STEP_NUM];
  /*!< LNA Rise Value for each LNA Gain Step. */

  int16 fall_val[RFM_LNA_GAIN_STEP_NUM];
  /*!< LNA Fall Value for each LNA Gain Step. */

  int16 calibrated_gain_val[RFM_CDMA_CARRIER_NUM][RFM_LNA_GAIN_STATE_NUM];
  /*!< Calibrated Gain values per carrier and for all Gain States */

  int16 bypass_timer_val[RFM_LNA_GAIN_STEP_NUM];
  /*!< LNA Bypass Timer Value per LNA Gain Step. Units is in microseconds */

  int16 nonbypass_timer_val[RFM_LNA_GAIN_STEP_NUM];
  /*!< LNA non-Bypass Timer Value per LNA Gain Step. Units is in microseconds */

  uint32 lna_phase_offset[RFM_LNA_GAIN_STATE_NUM];
  /*!< LNA Phases offset in modem specific hardware units to be programmed */

} rf_cdma_mdsp_rx_agc_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold WBAGC threshold Param
*/
typedef struct
{
  uint32 wbagc_jamdet_thresh[RFM_LNA_GAIN_STEP_NUM];
  /*!< Wideband Energy Estimate Jammer Detect Threshold in linear value */

  uint32 jd_thresh_minus_hyst[RFM_LNA_GAIN_STEP_NUM];
  /*!< WB EE linear value at which JD flag trips back to default */

  uint16 JamDetEnable;
  /*!< Wideband Energy Estimate Jammer Detect algo enable flag */

}rf_cdma_mdsp_wbagc_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold CDMA Rx Start Configuration Param
*/
typedef struct
{
  uint32 script_index;
  /*!< RF script index. This will be 0xFFFFFFFF (msb=1) to indicate firmware 
  should not program any RF scripts. In this mode firmware still counts the 
  settling time. */

  lm_handle_type rxlm_handle;
  /*!< RxLM Buffer Index */

} rf_cdma_mdsp_rx_start_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold CDMA Rx Stop Configuration Param
*/
typedef struct
{
  uint32 script_index;
  /*!< RF script index */

  lm_handle_type rxlm_handle;
  /*!< RxLM Buffer Index */

  uint32 rtc_cx1_trigger;
  /*!< RTC Time at which the Rx will be stopped. The units is in Cx1 */

} rf_cdma_mdsp_rx_stop_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold CDMA Intelliceiver Update Param
*/
typedef struct
{
  uint32 script_index;
  /*!< RF script index. This will be 0xFFFFFFFF (msb=1) to indicate firmware 
  should not program any RF scripts. In this mode firmware still counts the 
  settling time. */

  lm_handle_type rxlm_handle;
  /*!< RxLM Buffer Index */

  uint32 settling_time;
  /*!< Cx1 delta between start message and start RxAGC. This includes the time 
  for both RTR Script program time and PLL settling time. */

} rf_cdma_mdsp_ic_update_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold CDMA Tx Start Configuration Param
*/
typedef struct
{
  uint32 script_index;
  /*!< RF script index. This will be 0xFFFFFFFF (msb=1) to indicate firmware 
  should not program any RF scripts. In this mode firmware still counts the 
  settling time. */

  lm_handle_type txlm_handle;
  /*!< TxLM Buffer Index */

} rf_cdma_mdsp_tx_start_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold CDMA Tx Stop Configuration Param
*/
typedef struct
{
  uint32 script_index;
  /*!< RF script index */

  lm_handle_type txlm_handle;
  /*!< RxLM Buffer Index */

} rf_cdma_mdsp_tx_stop_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold CDMA PA State Machine Configuration Param
*/
typedef struct
{
  int16 rise_val[RFM_PA_GAIN_STATE_NUM];
  /*!< PA Rise Value */

  int16 fall_val[RFM_PA_GAIN_STATE_NUM];
  /*!< PA Fall Value */

  boolean active_pa_state[RFM_PA_GAIN_STATE_NUM];

  tx_linearizer_aux_data_type dpd_type[RFM_PA_GAIN_STATE_NUM];
  /*!< DPD type per PA state */

  uint16 pa_hyst_timer;
  /*!< PA Hysteresis timer. If PA Hysterisis Timer is Zero, PA hysteris will be
  disabled */

  uint32 pa_l2h_delay;
  /*!< Delay between PA State change and AVGA Update, when PA State changes
  from Low to High */
  
  uint32 pa_h2l_delay;
  /*!< Delay between PA State change and AVGA Update, when PA State changes
  from High to Low */

  uint32 avga_l2h_delay;
  /*!< Delay between DVGA Update and AVGA Update, when AVGA update results in
  higher power */

  uint32 avga_h2l_delay;
  /*!< Delay between DVGA Update and AVGA Update, when AVGA update results in
  lower power */

  uint16 btf;
  /*!< "Back to Future" : Value indicating the adjustment for the Tx Time
  Error*/

  boolean update_xpt_mode;
  /*!< Flag that indicates whether XPT mode needs to be updated or not, 
    in scenarios where we are going from access to voice switchpoints, we
    do not need to update XPT mode*/
  
  int16 pred_clip_en_thresh;
  /*!< Predictive clipper power threshold in dBm10 */

  int16 pred_clip_dis_thresh;
  /*!< Predictive clipper power threshold in dBm10 */

} rf_cdma_mdsp_pa_stm_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to return 1x/HDR HDET Data read from MDSP
*/
typedef struct
{
  uint32 read_count;
  /*!< Count of last HDET read performed. MDSP increments this counter when it
  completes an HDET trigger sequence. RF SW should poll this variable to check
  if it has incremented from the previous value. */

  boolean read_success;
  /*!< Boolean flag indicates if MDSP was able to trigger an HDET read or not.
  Software is expected to poll read_count for MDSP response. When it receives
  a response, it should check read_success to determine if a HDET read was
  triggered or MDSP timed out. */

  int16 txagc_at_read;
  /*!< TX AGC recorded by MDSP when last HDET read was triggered by MDSP */

} rf_cdma_mdsp_hdet_read_data_type;



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to return 1x/HDR HDET Data read from MDSP
*/
typedef struct
{
  uint32 read_count;
  /*!< Count of last THERM read performed. MDSP increments this counter when it
  completes an THERM trigger sequence. RF SW should poll this variable to check
  if it has incremented from the previous value. */

  boolean therm_read_scheduled;
  /*!< Boolean flag indicates if MDSP was able to trigger an THERM read or not.
  Software is expected to poll read_count for MDSP response. When it receives
  a response, it should check read_success to determine if a THERM read was
  triggered or MDSP timed out. */

} rf_cdma_mdsp_therm_config_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to return 1x/HDR Data read from MDSP
*/
typedef struct
{
  uint32 read_count;
  /*!< Count of last therm read performed. MDSP increments this counter when it
  completes an Therm trigger sequence. RF SW should poll this variable to check
  if it has incremented from the previous value. */

  boolean read_success;
  /*!< Boolean flag indicates if MDSP was able to trigger an therm read or not.
  Software is expected to poll read_count for MDSP response. When it receives
  a response, it should check read_success to determine if a therm read was
  triggered or MDSP timed out. */
 
} rf_cdma_mdsp_therm_read_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition used to store Tx IQ gain data for different 1x and
  HDR reverse link configurations, used for APT backoff
*/
typedef struct
{
  uint16 iq_gain_scale_factor;
  /*!< IQ gain scale in 2Q14 HW units. Store the baseband gain adjust value 
  specific to the reverse link configuration. This is computed based on 
  NV data */

  int16  lutDbOffset;
  /*!< Linearizer power offset in dBx10 units. This will be used 
  by FW to offset the txTotal in order to ensure that the linearizer is 
  shifted correctly to compensate for IQ gain changes. */ 

} rf_cdma_mdsp_tx_iq_gain_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition used to store Tx Threshold indicator data as
  obtained from NV
*/
typedef struct
{
  boolean feature_enabled;
  /*!< Flag indicating whether the feature is enabled via NV or not */

  int16  rise_value;
  /*!< Tx Threshold rise value in Tx AGC units. FW will toggle the 
  Tx Thresh GRFC if Tx total is equal to or below this rise_value */

  int16  fall_value;
  /*!< Tx Threshold rise value in Tx AGC units. FW will toggle the 
  Tx Thresh GRFC if Tx total is above this fall_value */

} rf_cdma_mdsp_tx_gtr_thresh_type;

/*----------------------------------------------------------------------------*/
/*! Data structure containing the CDMA MDSP data */
typedef struct
{

  rf_1x_mdsp_data_type onex_mdsp;
  /*!< 
    @brief
    RF Variable to store all parameters related to 1x MDSP Interface
   
    @details
    This static variable will store all parameter related to 1x MDSP interface.
    The content of this variable is accessible only through accessor function.
  */

  rf_hdr_mdsp_data_type hdr_mdsp;
  /*!< 
    @brief
    RF Variable to store all parameters related to HDR MDSP Interface
   
    @details
    This static variable will store all parameter related to HDR MDSP interface.
    The content of this variable is accessible only through accessor function.
  */

  rf_cdma_mdsp_sum_data_type snum_data;
  /*!< Structure to hold data related to sequence number generation for 
  SW-FW communication */


  rf_cdma_ccs_event_descriptor_type* ccs_event_descriptor ;
   /*!< Structure-pointer to hold data related to CDMA CCS events. 
     */

} rf_cdma_mdsp_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold XPT Configuration Params
*/
typedef struct
{
  boolean ept_enable;
  /*!< Flag to indicate if EPT functionality must be enabled. Actual EPT operation
  is triggered through switchpoints update */
  boolean et_enable;
  /*!< Flag to indicate if ET functionality must be enabled. Actual ET operation
  is triggered through switchpoints update */
  int32 et_env_delay;
  /*!< ET envelope path delay value */
  int32 apt_iq_gain_nominal;
  /*!< Nominal IQ gain value for APT operation */
  int32 ept_iq_gain_nominal;
  /*!< Nominal IQ gain value for EPT operation */
  int32 et_iq_gain_nominal;
  /*!< Nominal IQ gain value for ET operation */
  int32 ept_env_scale_nominal;
  /*!< Nominal envelope scale value for EPT operation */
  int32 et_env_scale_nominal;
  /*!< Nominal envelope scale value for ET operation */
  lm_handle_type prim_txlm_handle;
  /*!< Primary TxLM handle */
  uint32 prim_tx_chain;
  /*!< Primary Tx chain */
  uint32 cdma1x_pred_clip_thresh;
  /*!< 1x Predictive clipper threshold */
  uint32 dor0_pred_clip_thresh;
  /*!< DOr0 Predictive clipper threshold */
  uint32 dorA_pred_clip_thresh;
  /*!< DOrA Predictive clipper threshold */
  int16 pred_clip_on;
  /*!< FW UNIT power threshold for enabling PC. If Tx > pred_clip_on, enable.*/
  int16 pred_clip_off;
  /*!< FW UNIT power threshold for disabling PC. If Tx < pred_clip_off, disable.*/
} rf_cdma_mdsp_xpt_config_type;

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_MDSP_TYPES_H */
