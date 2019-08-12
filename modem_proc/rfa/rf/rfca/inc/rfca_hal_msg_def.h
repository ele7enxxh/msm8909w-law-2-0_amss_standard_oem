/*!
  @file
  rfca_hal_msg_def.h
 
  @brief
  Defines Hal messages structures.

  @talkabout Is it intentional that in the spec some message IDs are RX intead of RF

*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_hal_msg_def.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/18/14   aki     Added support for compressed sweep payload 
05/19/14   aki     Added HAL_RF_GET_FLEXCAL_SWEEP_RESULT 
04/16/14   aki     Deprectaed FLEX_CAL for ARB config 
03/20/14   aki     Added DUPLEXER_CAL and HAL_RF_SET_SECONDARY_CHAIN
03/03/14   aki     Changed 'halfSpeed' to 'fullSpeed'
03/03/14   aki     Added 'halfSpeed' to RFCA_RFFE_READ_CONFIG 
01/24/14   aki     Added HAL_RF_EN/DISABLE_DEBUG_MESSAGES
01/03/14   aki     Added NO_EXIT_REQ
01/03/14   svi     LTE B27 Support for BOLT
12/11/13   aki     Swapped channel and address for RFFE struct
12/09/13   aki     Added RFFE read support for RFIC
12/05/13   aki     Added HAL_RF_TX_ALIGN_TXLO_RSB_OPTIONS 
11/25/13   aki     Added HAL_RF_SWEEP_DEBUG_MESSAGES, removed HAL_RF_SKIP_SWEEP_CONFIG
11/19/13   aki     Added LTEFDD and TDS TX_ALIGN structs 
11/14/13   aki     Updated version of antenna switching, LTE bands 22-31, fixed typos
11/14/13   aki     Added EFS_SYNC_AFTER_TUNER_CAL info items
11/14/13   aki     Added persistent TX mode to hal_inform
09/13/13   aki     Added ant switch to hal inform
08/13/13   aki     HAL_RF_GSM_TX_ALIGN_CONFIG_TUNER_SELF_CAL updated to 4 
08/09/13   aki     HAL_RF_RX_LNA_RANGE_THRESHOLDS_GSM
08/09/13   aki     IQ offset and size for RFIC 
08/02/13   aki     Removed HAL_RF_SET_PATH_DELAY and slight cleanup
07/31/13   aki     Add IQ functions 
07/31/13   aki     Remove old LTE APIs
07/30/13   aki     Rename LTE_CA to SKIP_STATE_CACHING, add new 2ND device paths
07/16/13   aki     Added FLEX_CAL and FLEX_CAL_V2 to tx arb 
06/29/13   hm      Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
06/24/13   aki     Removed  HAL_RF_x_TX_ARB_LTE_DAC_REQ_TX_CONTROL_DATA_APT, 
                   Added HAL_RF_ARB_TX_TDD_LTE_PARM and LTE_BW_MAX_HAL
06/10/13   aki     Mainline RFCA 
05/30/13   aki     Add support for STEP_TIME_10X align config pattern
05/21/13   aki     Added support for LTTDD DAC via ARB TX
05/01/13   aki     Added new RX WAIT functions and HAL_RF_STATUS_RX_WAIT_TIMEOUT
04/29/13   aki     Updated HAL_RF_UPDATE_CONFIG comment
04/29/13   aki     New LTE FDD and TDD messages and new LTE bands
04/25/13   aki     Added new error codes and GSM sweep RFIC id
04/13/13   aki     Added HAL_RF_STATUS_SWEEP_SEGMENT_OVERRUN to sweep result query
03/26/13   aki     Removed USE_RX_SWEEP and USE_CDMA_V2_API from 
                   HAL_RF_x_RX_ALIGN_CONFIG and added HAL_RF_CA_QCT_VERSION
                   to RFIC table
03/25/13   aki     Added new error code HAL_RF_STATUS_ARGUMENT_ERROR and 
                   new RFIC id HAL_RF_UPDATE_CONFIG
02/04/13   aki     Added HAL_RF_STATUS_INVALID_SF_ACTIVITY
02/01/13   aki     Added polling interval to sweep config
01/16/13   aki     Added RFIC WRITE id to enable old RX
12/10/12   aki     Updated XO 
10/31/12   aki     Added HAL_RF_RELOAD_CONFIG_FILE
09/04/12   aki     Added HAL_RF_STATUS_TIMING_OVERRUN error code
08/16/12   aki     FTM_SET_FREQ_ADJUST related additions, version date
07/23/12   aki     TDSCDMA TX DBM and RX RSSI, WCDMA RSSI param fix to int16_q8
07/13/12   aki     LTEFDD, TDSCDMA, new RFIC ids
05/25/12   aki     GSM Rx burst, Cdma API2, Rx Sweep, removed ol
04/23/12   aki     Added cal state, Lna range and dual carrier for rfic write,
				   Added XO and dual carries for rx align, waveform removed from amam
03/01/12   aki     Changed <> to "", remoed IDC, changed TX DAC config
02/17/12   aki     Added XO, THER and KV support
02/08/12   aki     All variable len arrays to 0 size
10/28/11   aki     Initial revision

==============================================================================*/


#ifndef _RFCA_HAL_MESSAGE_DEFINITION_H_
#define _RFCA_HAL_MESSAGE_DEFINITION_H_

#include "rfa_variation.h"
#include "comdef.h"

#include "rfca_custom_types.h"

#ifdef _MSC_VER
  #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
  #pragma warning( push )
  #pragma warning( disable : 4200 ) // Remove warning 'nonstandard extension used : zero-sized array in struct/union'
#endif


///< HAL Chipset API version supported by this implementation
#define HAL_RF_CHIPSET_API_VERSION 5.4
#define HAL_RF_RFCA_VERSION        "2014.06.17"

/*!
 @brief Hal request and response message identifiers
 */
typedef enum{
  HAL_RF_INIT_REQ_ID = 0x0001,
  HAL_RF_INIT_RESP_ID = 0x0002,
  HAL_RF_GSM_RX_RSSI_REQ_ID = 0x0003,
  HAL_RF_GSM_RX_RSSI_RESP_ID = 0x0004,
  HAL_RF_GSM_RX_WAIT_REQ_ID = 0x0005,
  HAL_RF_GSM_RX_WAIT_RESP_ID = 0x0006,
  HAL_RF_GSM_RX_IQ_REQ_ID = 0x0007,
  HAL_RF_GSM_RX_IQ_RESP_ID = 0x0008,
  HAL_RF_GSM_TX_PCL_REQ_ID = 0x0009,
  HAL_RF_GSM_TX_PCL_RESP_ID = 0x000A,
  HAL_RF_GSM_TX_DAC_REQ_ID = 0x000B,
  HAL_RF_GSM_TX_DAC_RESP_ID = 0x000C,
  HAL_RF_WCDMA_RX_RSSI_REQ_ID = 0x000D,
  HAL_RF_WCDMA_RX_RSSI_RESP_ID = 0x000E,
  HAL_RF_WCDMA_RX_WAIT_REQ_ID = 0x000F,
  HAL_RF_WCDMA_RX_WAIT_RESP_ID = 0x0010,
  HAL_RF_WCDMA_RX_IQ_REQ_ID = 0x0011,
  HAL_RF_WCDMA_RX_IQ_RESP_ID = 0x0012,
  HAL_RF_WCDMA_TX_DBM_REQ_ID = 0x0013,
  HAL_RF_WCDMA_TX_DBM_RESP_ID = 0x0014,
  HAL_RF_WCDMA_TX_DAC_REQ_ID = 0x0015,
  HAL_RF_WCDMA_TX_DAC_RESP_ID = 0x0016,
  HAL_RF_TDSCDMA_RX_RSSI_REQ_ID = 0x0017,
  HAL_RF_TDSCDMA_RX_RSSI_RESP_ID = 0x0018,
  HAL_RF_TDSCDMA_RX_WAIT_REQ_ID = 0x0019,
  HAL_RF_TDSCDMA_RX_WAIT_RESP_ID = 0x001A,
  HAL_RF_TDSCDMA_RX_IQ_REQ_ID = 0x001B,
  HAL_RF_TDSCDMA_RX_IQ_RESP_ID = 0x001C,
  HAL_RF_TDSCDMA_TX_DBM_REQ_ID = 0x001D,
  HAL_RF_TDSCDMA_TX_DBM_RESP_ID = 0x001E,
  HAL_RF_TDSCDMA_TX_DAC_REQ_ID = 0x001F,
  HAL_RF_TDSCDMA_TX_DAC_RESP_ID = 0x0020,
  HAL_RF_STOP_REQ_ID = 0x0021,
  HAL_RF_STOP_RESP_ID = 0x0022,
  HAL_RF_FTC_ADJUST_REQ_ID = 0x0023,
  HAL_RF_FTC_ADJUST_RESP_ID = 0x0024,
  HAL_RF_CALIBRATION_VALUES_LOAD_REQ_ID = 0x0025,
  HAL_RF_CALIBRATION_VALUES_LOAD_RESP_ID = 0x0026,
  HAL_RF_CALIBRATION_VALUES_STORE_REQ_ID = 0x0027,
  HAL_RF_CALIBRATION_VALUES_STORE_RESP_ID = 0x0028,
  HAL_RF_RFIC_READ_REQ_ID = 0x0029,
  HAL_RF_RFIC_READ_RESP_ID = 0x002A,
  HAL_RF_RFIC_WRITE_REQ_ID = 0x002B,
  HAL_RF_RFIC_WRITE_RESP_ID = 0x002C,
  HAL_RF_INFORM_REQ_ID = 0x002D,
  HAL_RF_INFORM_RESP_ID = 0x002E,
  HAL_RF_ARB_TX_REQ_ID = 0x003B,
  HAL_RF_ARB_TX_RESP_ID = 0x003C,
  HAL_RF_GSM_RX_ALIGN_REQ_ID = 0x003D,
  HAL_RF_GSM_RX_ALIGN_RESP_ID = 0x003E,
  HAL_RF_GSM_TX_ALIGN_REQ_ID = 0x003F,
  HAL_RF_GSM_TX_ALIGN_RESP_ID = 0x0040,
  HAL_RF_WCDMA_RX_ALIGN_REQ_ID = 0x0041,
  HAL_RF_WCDMA_RX_ALIGN_RESP_ID = 0x0042,
  HAL_RF_WCDMA_TX_ALIGN_REQ_ID = 0x0043,
  HAL_RF_WCDMA_TX_ALIGN_RESP_ID = 0x0044,
  HAL_RF_TDSCDMA_RX_ALIGN_REQ_ID = 0x0045,
  HAL_RF_TDSCDMA_RX_ALIGN_RESP_ID = 0x0046,
  HAL_RF_TDSCDMA_TX_ALIGN_REQ_ID = 0x0047,
  HAL_RF_TDSCDMA_TX_ALIGN_RESP_ID = 0x0048,
  HAL_RF_CDMA2K_RX_RSSI_REQ_ID = 0x004D,
  HAL_RF_CDMA2K_RX_RSSI_RESP_ID = 0x004E,
  HAL_RF_CDMA2K_RX_WAIT_REQ_ID = 0x004F,
  HAL_RF_CDMA2K_RX_WAIT_RESP_ID = 0x0050,
  HAL_RF_CDMA2K_RX_IQ_REQ_ID = 0x0051,
  HAL_RF_CDMA2K_RX_IQ_RESP_ID = 0x0052,
  HAL_RF_CDMA2K_TX_DBM_REQ_ID = 0x0053,
  HAL_RF_CDMA2K_TX_DBM_RESP_ID = 0x0054,
  HAL_RF_CDMA2K_TX_DAC_REQ_ID = 0x0055,
  HAL_RF_CDMA2K_TX_DAC_RESP_ID = 0x0056,
  HAL_RF_CDMA2K_RX_ALIGN_REQ_ID = 0x0057,
  HAL_RF_CDMA2K_RX_ALIGN_RESP_ID = 0x0058,
  HAL_RF_CDMA2K_TX_ALIGN_REQ_ID = 0x0059,
  HAL_RF_CDMA2K_TX_ALIGN_RESP_ID = 0x005A,
  HAL_RF_LTEFDD_RX_RSSI_REQ_ID = 0x005B,
  HAL_RF_LTEFDD_RX_RSSI_RESP_ID = 0x005C,
  HAL_RF_LTEFDD_RX_WAIT_REQ_ID = 0x005D,
  HAL_RF_LTEFDD_RX_WAIT_RESP_ID = 0x005E,
  HAL_RF_LTEFDD_RX_IQ_REQ_ID = 0x005F,
  HAL_RF_LTEFDD_RX_IQ_RESP_ID = 0x0060,
  HAL_RF_LTEFDD_TX_DBM_REQ_ID = 0x0061,
  HAL_RF_LTEFDD_TX_DBM_RESP_ID = 0x0062,
  HAL_RF_LTEFDD_TX_DAC_REQ_ID = 0x0063,
  HAL_RF_LTEFDD_TX_DAC_RESP_ID = 0x0064,
  HAL_RF_LTEFDD_RX_ALIGN_REQ_ID = 0x0065,
  HAL_RF_LTEFDD_RX_ALIGN_RESP_ID = 0x0066,
  HAL_RF_LTEFDD_TX_ALIGN_REQ_ID = 0x0067,
  HAL_RF_LTEFDD_TX_ALIGN_RESP_ID = 0x0068,
  HAL_RF_LTETDD_RX_RSSI_REQ_ID = 0x0069,
  HAL_RF_LTETDD_RX_RSSI_RESP_ID = 0x006A,
  HAL_RF_LTETDD_RX_WAIT_REQ_ID = 0x006B,
  HAL_RF_LTETDD_RX_WAIT_RESP_ID = 0x006C,
  HAL_RF_LTETDD_RX_IQ_REQ_ID = 0x006D,
  HAL_RF_LTETDD_RX_IQ_RESP_ID = 0x006E,
  HAL_RF_LTETDD_TX_DBM_REQ_ID = 0x006F,
  HAL_RF_LTETDD_TX_DBM_RESP_ID = 0x0070,
  HAL_RF_LTETDD_TX_DAC_REQ_ID = 0x0071,
  HAL_RF_LTETDD_TX_DAC_RESP_ID = 0x0072,
  HAL_RF_LTETDD_RX_ALIGN_REQ_ID = 0x0073,
  HAL_RF_LTETDD_RX_ALIGN_RESP_ID = 0x0074,
  HAL_RF_LTETDD_TX_ALIGN_REQ_ID = 0x0075,
  HAL_RF_LTETDD_TX_ALIGN_RESP_ID = 0x0076,
  HAL_RF_GSM_RX_WAIT_V2_REQ_ID = 0x0077,
  HAL_RF_GSM_RX_WAIT_V2_RESP_ID = 0x0078,
  HAL_RF_WCDMA_RX_WAIT_V2_REQ_ID = 0x0079,
  HAL_RF_WCDMA_RX_WAIT_V2_RESP_ID = 0x007A,
  HAL_RF_TDSCDMA_RX_WAIT_V2_REQ_ID = 0x007B,
  HAL_RF_TDSCDMA_RX_WAIT_V2_RESP_ID = 0x007C,
  HAL_RF_LTEFDD_RX_WAIT_V2_REQ_ID = 0x007D,
  HAL_RF_LTEFDD_RX_WAIT_V2_RESP_ID = 0x007E,
  HAL_RF_CDMA2K_RX_WAIT_V2_REQ_ID = 0x007F,
  HAL_RF_CDMA2K_RX_WAIT_V2_RESP_ID = 0x0080,

  // Additional messages to do cleanup
  HAL_RF_FINISH_REQ_ID                    = 0x1000,
} HAL_MESSAGE_ID;

/*!
 @brief RF path values. 
 */
typedef enum {
  HAL_RF_PATH_RX        = 0x0000, // Enables the RX branch
  HAL_RF_PATH_TX        = 0x0001, // Enables the TX branch
  HAL_RF_PATH_RX_AND_TX = 0x0002  // Enables RX and TX branches
} HAL_RF_PATH;

/*!
 @brief Protocol values.
 */
typedef enum {
  HAL_RF_PROTOCOL_SYSTEM_GSM      = 0x0000, // GSM protocol to be used
  HAL_RF_PROTOCOL_SYSTEM_WCDMA    = 0x0001, // WCDMA protocol to be used
  HAL_RF_PROTOCOL_SYSTEM_TD_SCDMA = 0x0002, // TD-SCDMA protocol to be used
  HAL_RF_PROTOCOL_SYSTEM_LTE      = 0x0003,  // LTE Protocol to be used
  HAL_RF_PROTOCOL_SYSTEM_CDMA     = 0x0004,  // CDMA
  HAL_RF_PROTOCOL_SYSTEM_MAX 
} HAL_RF_PROTOCOL_SYSTEM;

/*!
 @brief GSM band values. 
 */
typedef enum { 
  HAL_RF_BAND_GSM_850   = 0x0000,
  HAL_RF_BAND_GSM_900   = 0x0001,
  HAL_RF_BAND_GSM_1800  = 0x0002,
  HAL_RF_BAND_GSM_1900  = 0x0003,
} HAL_RF_GSM_BAND;

/*!
 @brief WCDMA band values. 
 */
typedef enum {
  HAL_RF_BAND_WCDMA_I     = 0x0000,
  HAL_RF_BAND_WCDMA_II    = 0x0001,
  HAL_RF_BAND_WCDMA_III   = 0x0002,
  HAL_RF_BAND_WCDMA_IV    = 0x0003,
  HAL_RF_BAND_WCDMA_V     = 0x0004,
  HAL_RF_BAND_WCDMA_VI    = 0x0005,
  HAL_RF_BAND_WCDMA_VII   = 0x0006,
  HAL_RF_BAND_WCDMA_VIII  = 0x0007,
  HAL_RF_BAND_WCDMA_IX    = 0x0008,
  HAL_RF_BAND_WCDMA_X     = 0x0009
} HAL_RF_WCDMA_BAND;

/*!
 @brief LTE band values. 
 */
typedef enum {
  HAL_RF_BAND_LTE_1  = 0x0000,
  HAL_RF_BAND_LTE_2  = 0x0001,
  HAL_RF_BAND_LTE_3  = 0x0002,
  HAL_RF_BAND_LTE_4  = 0x0003,
  HAL_RF_BAND_LTE_5  = 0x0004,
  HAL_RF_BAND_LTE_7  = 0x0006,
  HAL_RF_BAND_LTE_8  = 0x0007,
  HAL_RF_BAND_LTE_9  = 0x0008,
  HAL_RF_BAND_LTE_10 = 0x0009,
  HAL_RF_BAND_LTE_11 = 0x000A,
  HAL_RF_BAND_LTE_12 = 0x000B,
  HAL_RF_BAND_LTE_13 = 0x000C,
  HAL_RF_BAND_LTE_14 = 0x000D,
  HAL_RF_BAND_LTE_17 = 0x0010,
  HAL_RF_BAND_LTE_18 = 0x0011,
  HAL_RF_BAND_LTE_19 = 0x0012,
  HAL_RF_BAND_LTE_20 = 0x0013,
  HAL_RF_BAND_LTE_21 = 0x0014,
  HAL_RF_BAND_LTE_22 = 0x0015,
  HAL_RF_BAND_LTE_23 = 0x0016,
  HAL_RF_BAND_LTE_24 = 0x0017,
  HAL_RF_BAND_LTE_25 = 0x0018,
  HAL_RF_BAND_LTE_26 = 0x0019,
  HAL_RF_BAND_LTE_27 = 0x001A,
  HAL_RF_BAND_LTE_28 = 0x001B,
  HAL_RF_BAND_LTE_29 = 0x001C,
  HAL_RF_BAND_LTE_30 = 0x001D,
  HAL_RF_BAND_LTE_31 = 0x001E,
  //HAL_RF_BAND_LTE_32 = 0x001F,
  HAL_RF_BAND_LTE_33 = 0x0020,
  HAL_RF_BAND_LTE_34 = 0x0021,
  HAL_RF_BAND_LTE_35 = 0x0022,
  HAL_RF_BAND_LTE_36 = 0x0023,
  HAL_RF_BAND_LTE_37 = 0x0024,
  HAL_RF_BAND_LTE_38 = 0x0025,
  HAL_RF_BAND_LTE_39 = 0x0026,
  HAL_RF_BAND_LTE_40 = 0x0027,
  HAL_RF_BAND_LTE_41 = 0x0028,
  HAL_RF_BAND_LTE_42 = 0x0029,
  HAL_RF_BAND_LTE_43 = 0x002A,
  HAL_RF_BAND_LTE_44 = 0x002B

} HAL_RF_LTE_BAND;

/*!
 @brief CDMA2K band values. 
 */
typedef enum {
  HAL_RF_BAND_CDMA2K_0  = 0x0000,
  HAL_RF_BAND_CDMA2K_1  = 0x0001,
  HAL_RF_BAND_CDMA2K_2  = 0x0002,
  HAL_RF_BAND_CDMA2K_3  = 0x0003,
  HAL_RF_BAND_CDMA2K_4  = 0x0004,
  HAL_RF_BAND_CDMA2K_5  = 0x0005,
  HAL_RF_BAND_CDMA2K_6  = 0x0006,
  HAL_RF_BAND_CDMA2K_7  = 0x0007,
  HAL_RF_BAND_CDMA2K_8  = 0x0008,
  HAL_RF_BAND_CDMA2K_9  = 0x0009,
  HAL_RF_BAND_CDMA2K_10 = 0x000A,
  HAL_RF_BAND_CDMA2K_11 = 0x000B,
  HAL_RF_BAND_CDMA2K_12 = 0x000C,
  HAL_RF_BAND_CDMA2K_13 = 0x000D,
  HAL_RF_BAND_CDMA2K_14 = 0x000E,
  HAL_RF_BAND_CDMA2K_15 = 0x000F,
  HAL_RF_BAND_CDMA2K_16 = 0x0010,
  HAL_RF_BAND_CDMA2K_18 = 0x0012,
  HAL_RF_BAND_CDMA2K_19 = 0x0013,
  HAL_RF_BAND_CDMA2K_20 = 0x0014,
  HAL_RF_BAND_CDMA2K_21 = 0x0015,
} HAL_RF_CDMA2K_BAND;


/*!
 @brief TDSCDMA band values. 
 */
typedef enum {
  HAL_RF_BAND_TDSCDMA_I   = 0x0000,
  HAL_RF_BAND_TDSCDMA_II  = 0x0001,
  HAL_RF_BAND_TDSCDMA_III = 0x0002,
  HAL_RF_BAND_TDSCDMA_IV  = 0x0003,
  HAL_RF_BAND_TDSCDMA_V   = 0x0004,
  HAL_RF_BAND_TDSCDMA_VI  = 0x0005,
} HAL_RF_TDSCDMA_BAND;

/*!
 @brief TDSCDMA band values. 
 */
typedef enum {
  SELECT_RISE = 0x0000, //<! Triggers on the rising edge
  SELECT_FALL = 0x0001  //<!Triggers on the falling edge
} HAL_RF_RX_WAIT_TRIGGER_MODE;

typedef enum {
  HAL_RF_IQ_PATH_SELECT_IQ     = 0x0000,
  HAL_RF_IQ_PATH_SELECT_I      = 0x0001,
  HAL_RF_IQ_PATH_SELECT_Q      = 0x0002,
} HAL_RF_IQ_PATH;

typedef enum {
  DPDT_MAIN_DIV  = 0,
  GSM_COEX       = 1,
  DUPLEXER_CAL   = 2
} HAL_RF_ANT_SWITCH_PATH;

typedef PACK(struct) {
  uint16 path;      // HAL_RF_ANT_SWITCH_PATH
  uint16 state;     
  uint16 duration;  // In milliseconds
  uint16 system;    // According to HAL spec
  uint16 band;      // According to HAL spec
} HAL_RF_RFIC_ANT_SWITCH_CFG;

typedef PACK(struct) {
  uint16 size;         //!< Size of data in units of 8-bit integers
  uint8 data[];        //!< NV item data
} HAL_RF_RFIC_ID_NV_ITEM_CFG;

typedef PACK(struct) {
  uint16 band;        //!< Any band numeration. This must align with system selected in hal_rf_init
  uint16 channel;     //!< Valid channel for selected system and band
  uint16 txState;     //!< 0 - set TX OFF, anything else - set TX ON
} HAL_RF_RFIC_ID_CONFIG_TX_CFG;

typedef enum {
  HAL_RF_VENDOR_ID                        = 0x0000,
  HAL_RF_RF_LATENCY                       = 0x0001,
  HAL_RF_VENDOR_ID_LIST                   = 0x0004,
  HAL_RF_BUFFER_SIZE                      = 0x0005,
  HAL_RF_CSAPI_VERSION                    = 0x0006,
  HAL_RF_CA_QCT_VERSION                   = 0x0007,
  HAL_RF_NV_ITEM                          = 0x0100,   //!< Read/Write NV items, use data_offset for item ID
  HAL_CALIBRATION_STATE                   = 0x0101,   //!< Toggle calibration state, data_size =, data_array[0] = (0 = disable, 1 = enable)
  HAL_RF_ADC_READ                         = 0x0102,   //!< Read ADC, use data_offset for channel
  HAL_RF_RFIC_READ                        = 0x0103,   //!< Access RFIC ???
  HAL_RF_PDM_WRITE                        = 0x0104,   //!< Write PDM, use data_offset for PDM  

  HAL_RF_CONFIG_TX                        = 0x0106,   //!< Config phone band, channel and TX state to on or off. Set 0 in HAL_RF_RFIC_WRITE_REQ::data_array to HAL_RF_RFIC_ID_CONFIG_TX_CFG
  HAL_RF_FTM_SEQ_TIMING_MARKER            = 0x0108,   //!< Add timing marker, next SEQ wait is measured from this point. There are no arguments for this command.
  HAL_RF_FTM_SEQ_EXEC                     = 0x0109,   //!< Executes current FTM seqeuncer queue
  HAL_RF_FTM_SEQ_WAIT                     = 0x010A,   //!< Adds wait to sequence. Set wait time as DWORD in HAL_RF_RFIC_WRITE_REQ::data_array[0] and HAL_RF_RFIC_WRITE_REQ::data_size to 2
  HAL_RF_PARM_XO                          = 0x010B,   //!< See @ref HAL_RF_RFIC_XO_PARAM_OFFSET
  HAL_RF_PARM_KVCO                        = 0x010C,   //<! See @ref HAL_RF_RFIC_KV_PARAM_OFFSET
  HAL_RF_THERMISTOR                       = 0x010D,
  HAL_RF_SWEEP_RESULTS                    = 0x010E,
  HAL_RF_DUAL_CARRIER_CHANNEL             = 0x010F,   //<! Set second channel for dual carrier
  HAL_RF_RX_LNA_RANGE_THRESHOLDS_WCDMA    = 0x0110, //<! Pass in dBm treshold to set LNA Ranges for RSSI and WAIT
  HAL_RF_RX_LNA_RANGE_THRESHOLDS_LTE      = 0x0111, //<! Pass in dBm treshold to set LNA Ranges for RSSI and WAIT
  HAL_RF_RX_LNA_RANGE_THRESHOLDS_CDMA     = 0x0112, //<! Pass in dBm treshold to set LNA Ranges for RSSI and WAIT
  HAL_RF_SET_GSM_RX_BURST                 = 0x0113, //<! Sets GSM Rx burst, data_offset = slot number, data_array[0] action (  RX_FUNCTION_GET_RSSI = 0, 	RX_FUNCTION_GET_DC_OFFSETS = 1, 	RX_FUNCTION_STOP_CONTINOUS_RX = 2)
  HAL_RF_ACTIVE_DEVICE                    = 0x0114, //<! Selects active RF device 1 = first, 2 = seconds, 4 = third, etc.
  HAL_RF_POWER_MODE                       = 0x0115, //<! Power mode selection for CDMA Rx
  HAL_RF_TX_DAC_SEGMENT_DURATION          = 0x0116, //<! Sets the segment durations for TX Dac functions, there's only one set of values which are common to all systems
  //HAL_RF_SKIP_SWEEP_CONFIG                = 0x0117, //<! Sets the flag to skip or not to skip the sweep config, 0 = not skip (default), 1 = skip
  HAL_RF_RX_LNA_RANGE_THRESHOLDS_TDSCDMA  = 0x0118, //<! OBSOLETE! Pass in dBm treshold to set LNA Ranges for RSSI and WAIT
  HAL_RF_FTM_SET_MODE                     = 0x0119,
  HAL_RF_FTM_PASS_THROUGH                 = 0x011A,
  HAL_RF_TX_DAC_NUM_OF_HDET_AVG           = 0x011B, 
  HAL_RF_SET_FREQ_ADJUST_DATA             = 0x011C, //<! See @ref HAL_RF_FREQ_ADJUST_DATA_OFFSET for offsets
  //HAL_RF_DIAG_MODE_CHANGE                 = 0x011D, //<! PC RFCA ONLY Changes phone mode, data_offset = 0, data_length = 1, data_array[0] = new mode, see 80-V1294-1 for valid modes
  HAL_RF_TDSCDMA_RX_BURST_OFF             = 0x011E, //<! Calls FTM_TDSCDMA_BURST_RX to set BURST MODE OFF, has fixed execution time of 20 ms which can be changed by setting data_length = 1, data_array[0] = new_time_in_ms
  HAL_RF_RELOAD_CONFIG_FILE               = 0x011F,
  HAL_RF_USE_OLD_FTM_FOR_RX               = 0x0120,
  HAL_RF_UPDATE_CONFIG                    = 0x0121, //<! Takes one arguments data_array = new config data as 0-terminated string
  HAL_RF_GSM_FRAME_SWEEP_RESULTS          = 0x0122,
  HAL_RF_USE_GSM_FRAME_SWEEP_FOR_PCL      = 0x0123, //<! data_offset = 0, data_length = 1, data_array[0] = ( 0 = use frame matrix, 1 = use sweep)
  HAL_RF_ARB_TX_TDD_LTE_PARM              = 0x0124, //<! Set LTE TDD sweep created via TX ARB, see HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET for values to set
  HAL_RF_SWEEP_RESULTS_V2                 = 0x0125, //<! Return the raw FTM result array
  HAL_RF_IQ_SIZE_AND_OFFSET               = 0x0126, //<! data_offset = 0, data_length = 2, data_array[0] = num_of_samples, data_array[1] = offset
  HAL_RF_RX_LNA_RANGE_THRESHOLDS_GSM      = 0x0127, //<! Pass in dBm treshold to set LNA Ranges for IQ
  HAL_RF_PATH_CTRL                        = 0x0128, //<! Pass in HAL_RF_RFIC_ANT_SWITCH_CFG
  HAL_RF_SWEEP_DEBUG_MESSAGES             = 0x0129, //<! data[0] enable = 1, disable = 0   
  HAL_RF_RFFE_READ                        = 0x012A, //<! Reads RFFE, data_off set is coded as RFCA_RFFE_READ_CONFIG
  HAL_RF_SET_SECONDARY_CHAIN              = 0x012B,
  HAL_RF_GET_FLEXCAL_SWEEP_RESULT         = 0x0200  // Get full FTM sweep response from 'status' field onward, see 80-VA888-1
} HAL_RF_RFIC_ID;

typedef PACK(struct) 
{
  unsigned int address     : 7;
  unsigned int fullSpeed   : 1;
  unsigned int slave       : 4;
  unsigned int channel     : 4;
} RFCA_RFFE_READ_CONFIG;

typedef enum {
  HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_BANDWIDTH = 0,
  HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_RBS,
  HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_START_RB,
  HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_MAX
} HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET;

typedef PACK(struct) {
  uint16 rfConfig;
  uint16 rfTune;
  uint16 wvConfig; //<! This is for LTE
  uint16 pollingInterval_us;
} HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG;

typedef enum {
  HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG_OFFSET_RF_CONFIG = 0,
  HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG_OFFSET_RF_TUNE,
  HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG_OFFSET_WV_CONFIG,
  HAL_RF_TX_DAC_SEGMENT_DURATION_LTE_TX,
  HAL_RF_TX_DAC_SEGMENT_DURATION_TDSCDMA_TX,
  HAL_RF_TX_DAC_SEGMENT_DURATION_POLLING_INTERVAL,
  HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG_OFFSET_MAX
} HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG_OFFSET;

typedef enum {
  HAL_RF_TX_DAC_NUM_OF_HDET_AVG_WCDMA = 0,
  HAL_RF_TX_DAC_NUM_OF_HDET_AVG_CDMA,
  HAL_RF_TX_DAC_NUM_OF_HDET_AVG_TDSCDMA,
  HAL_RF_TX_DAC_NUM_OF_HDET_AVG_LTE
} HAL_RF_TX_DAC_NUM_OF_HDET_AVG_OFFSET;

typedef enum {
  HAL_RF_XO_PARAM_FREQUENCY_OFFSET_KHZ    = 0x0000,
  HAL_RF_XO_PARAM_TEMP_SPAN               = 0x0001,
  HAL_RF_XO_PARAM_MIN_FT_SAMPLE           = 0x0002,  
  HAL_RF_XO_PARAM_MAX_FT_SAMPLE           = 0x0003,
  HAL_RF_XO_PARAM_SUBSTAGE_TIME_MS        = 0x0004,
  HAL_RF_XO_PARAM_TIMEOUT_MS              = 0x0005,
  HAL_RF_XO_PARAM_WRITE_NV                = 0x0006,
  HAL_RF_XO_PARAM_TEMP_GRADIENT_TIMEOUT   = 0x0007,
  HAL_RF_XO_PARAM_XTAL                    = 0x0008
} HAL_RF_RFIC_XO_PARAM_OFFSET;     

typedef enum {
  HAL_RF_KV_PARAM_NO_OF_TX_CYCLES     = 0x0000,
  HAL_RF_KV_PARAM_NO_OF_MEASUREMENTS  = 0x0001,
  HAL_RF_KV_PARAM_BAND_SELECT         = 0x0002
} HAL_RF_RFIC_KV_PARAM_OFFSET;     

typedef enum {
  HAL_RF_RFIC_KV_BAND_SELECT_850      = 0x0001,
  HAL_RF_RFIC_KV_BAND_SELECT_900      = 0x0002,
  HAL_RF_RFIC_KV_BAND_SELECT_1800     = 0x0004,
  HAL_RF_RFIC_KV_BAND_SELECT_1900     = 0x0008
} HAL_RF_RFIC_KV_BAND_SELECT_PARAM;     

typedef enum {
  HAL_RF_FOLD_SLIDE_MECH_CLOSED              = 0x0000,
  HAL_RF_FOLD_SLIDE_MECH_OPEN                = 0x0001,
  HAL_RF_DVBH_INACTIVE                       = 0x0002,
  HAL_RF_DVBH_ACTIVE                         = 0x0003,
  HAL_RF_PROXIMITY_DETECTOR_FAR              = 0x0004,
  HAL_RF_PROXIMITY_DETECTOR_NEAR             = 0x0005,
  HAL_RF_GPS_INACTIVE                        = 0x0006,
  HAL_RF_GPS_ACTIVE                          = 0x0007,
  HAL_RF_ANTENNA_INACTIVE                    = 0x0008,
  HAL_RF_ANTENNA_ACTIVE                      = 0x0009,
  HAL_RF_RF_COMPENSATIONS_INACTIVE           = 0x000A,
  HAL_RF_RF_COMPENSATIONS_ACTIVE             = 0x000B,
  HAL_RF_DISABLE_EFS_SYNC_AFTER_TUNER_CAL    = 0x000E, 
  HAL_RF_ENABLE_EFS_SYNC_AFTER_TUNER_CAL     = 0x000F,
  HAL_RF_ENABLE_PERSISTENT_TX_MODE           = 0x0010,
  HAL_RF_DISABLE_PERSISTENT_TX_MODE          = 0x0011,
  HAL_RF_ENABLE_RX_TX_INTERLEAVING           = 0x0012,
  HAL_RF_DISABLE_RX_TX_INTERLEAVING          = 0x0013,
  HAL_RF_ENABLE_DEBUG_MESSAGES               = 0x0014,
  HAL_RF_DISABLE_DEBUG_MESSAGES              = 0x0015
} HAL_RF_INFORMATION_ID;

typedef enum {
  SF_NO_ACTIVITY = 0x0000,
  SF_LTE_0       = 0x0001,
  SF_LTE_1       = 0x0002,
  SF_LTE_2       = 0x0004,
  SF_LTE_3       = 0x0008,
  SF_LTE_4       = 0x0010,
  SF_LTE_5       = 0x0020,
  SF_LTE_6       = 0x0040,
  SF_LTE_7       = 0x0080,
  SF_LTE_8       = 0x0100,
  SF_LTE_9       = 0x0200,
} HAL_RF_LTE_SUBFRAME;

typedef enum {
  LTE_BW_1M4  = 0x0000,	// Set BW to 1.4 MHZ
  LTE_BW_3M   = 0x0001,	// Set BW to 3 MHZ
  LTE_BW_5M	  = 0x0002,	// Set BW to 5 MHZ
  LTE_BW_10M  = 0x0003,	// Set BW to 10 MHZ
  LTE_BW_15M  = 0x0004,	// Set BW to 15 MHZ
  LTE_BW_20M  = 0x0005,	// Set BW to 20 MHZ
  LTE_BW_MAX_HAL
} HAL_RF_LTE_BANDWIDTH;

typedef enum {
  LTE_CP_NORMAL   = 0x0000, //Normal length CP
  LTE_CP_EXTENDED = 0x0001, //Extended length CP
} HAL_RF_LTE_CP;

typedef enum {
  MODULATION_QPSK   = 0x0000, // QPSK modulation
  MODULATION_16_QAM = 0x0001, // 16-QAM modulation
  MODULATION_64_QAM = 0x0002, // 64-QAM modulation
} HAL_RF_LTE_MODULATION;

typedef enum {
  CALIBRATED_VALUES = 0x0000,
  DEFAULT_VALUES    = 0x0001
} HAL_RF_DATA_SELECT;

typedef enum {
  HAL_RF_STATUS_OK                                  = 0x0000, //!< Service OK
  HAL_RF_STATUS_FAILED                              = 0x0001, //!< Generic error
  HAL_RF_STATUS_SYSTEM_MISMATCH                     = 0x0100, //!< System specified in init does not match the request
  HAL_RF_STATUS_INVALID_RF_DEVICE                   = 0x0101, //!< Most likley init used wrong system
  HAL_RF_STATUS_TIMING_OVERRUN                      = 0x0102, //!< FTM command spent more time than budgeted
  HAL_RF_STATUS_NOT_IN_TEST_MODE                    = 0x0103, //!< Init has not been called 
  HAL_RF_STATUS_INVALID_SF_ACTIVITY                 = 0x0104, //!< Non-supported sf-activity (Not used in PC RFCA)
  HAL_RF_STATUS_MS_COM_FAIL                         = 0x0105, //!< In PC RFCA only
  HAL_RF_STATUS_CONNECTION_OPEN_FAIL                = 0x0106, //!< In PC RFCA only
  HAL_RF_STATUS_COMMUNICATION_FAIL                  = 0x0107, //!< In PC RFCA only
  HAL_RF_STATUS_FTM_SEQUENCER_INIT_FAIL             = 0x0108, //!< 
  HAL_RF_STATUS_ARGUMENT_ERROR                      = 0x0109, //!< Bad input argument, see F3 for details
  HAL_RF_STATUS_QLIB_EXCEPTION_ERROR                = 0x010A, //!< Exception from QLIB/QPST
  HAL_RF_STATUS_SEGMENT_LENGTH_TOO_SHORT            = 0x010B, //!< Segment is too short because some mandatory delays
  HAL_RF_STATUS_TOO_MANY_ITEMS_IN_SWEEP             = 0x010C, //!< 
  HAL_RF_STATUS_MEMORY_ALLOCATION_FAIL              = 0x010D, //!< 
  HAL_RF_STATUS_SWEEP_SEGMENT_OVERRUN               = 0x0120, //!< One or more sweep segments overran
  HAL_RF_STATUS_RX_WAIT_TIMEOUT                     = 0x0121, //!< Rx Wait Timedout
  HAL_RF_STATUS_RX_IQ_GENERAL_FAILURE               = 0x0122,
  HAL_RF_STATUS_RX_IQ_SAMPLES_UNAVAILABLE_ON_DEV    = 0x0123,
  HAL_RF_STATUS_RX_IQ_SAMPLES_UNAVAILABLE           = 0x0124,
  HAL_RF_STATUS_RX_IQ_ILLEGAL_COMBIMATION           = 0x0125,
  HAL_RF_STATUS_RSB_IQ_CAP_FAILURE                  = 0x0126,
  HAL_RF_STATUS_RSB_LM_CON_FAILURE                  = 0x0127,
  HAL_RF_STATUS_TXLO_IQ_CAP_FAILURE                 = 0x0128,
  HAL_RF_STATUS_TXLO_LM_CON_FAILURE                 = 0x0129,
  HAL_RF_STATUS_UNSUPPORTED_BAND                    = 0x012A
} HAL_RF_STATUS;


typedef enum {
  RF_SIGNAL_PATH_MAIN               = 0x0000,
  RF_SIGNAL_DIVERSITY               = 0x0001,
  RF_SIGNAL_PATH_MAIN_2ND_TRANSCVR  = 0x0002,
  RF_SIGNAL_DIVERSITY_2ND_TRANSCVR  = 0x0003,
  RF_SIGNAL_PATH_MAX                
} HAL_RF_SIGNAL_PATH;

typedef enum {
  TS_GSM_NO_ACTIVITY = 0x0000, // No activity in any of the timeslots.
  TS_GSM_0 = 0x0001, // Select timeslot 0
  TS_GSM_1 = 0x0002, // Select timeslot 1
  TS_GSM_2 = 0x0004, // Select timeslot 2
  TS_GSM_3 = 0x0008, // Select timeslot 3
  TS_GSM_4 = 0x0010, // Select timeslot 4
  TS_GSM_5 = 0x0020, // Select timeslot 5
  TS_GSM_6 = 0x0040, // Select timeslot 6
  TS_GSM_7 = 0x0080, // Select timeslot 7
} HAL_RF_GSM_TIMESLOT;

typedef enum  
{
  MODULATION_GMSK = 0x0000, // GMSK modulation
  MODULATION_8PSK = 0x0001, // 8PSK modulation
} HAL_RF_GSM_MODULATION;

typedef enum  
{
  BURST_LENGTH_NORMAL = 0x0000, // Normal bursts length
  BURST_LENGTH_RACH = 0x0001, // RACH (Access) bursts length
} HAL_RF_GSM_BURST_LENGTH;

typedef enum  
{
  TEST_ALL_0  = 0x0000, // ‘0’ bits, no TSC
  TEST_ALL_1  = 0x0001, // ‘1’ bits, no TSC
  TEST_PSEUDO = 0x0002, // Pseudo random, TSC 0
  TEST_STATIC = 0x0003, // Static data, TSC 0
  TEST_DUMMY  = 0x0004, // GSM SPEC DUMMY burst, special TSC
} HAL_RF_TX_DATA_TYPE;

/******************************
  HAL_RF_TX_ALIGN_CONFIG_PATTERN
******************************/
typedef enum {
  HAL_RF_TX_ALIGN_CONFIG_TXLO_RSB       = 1  
} HAL_RF_TX_ALIGN_CONFIG_PATTERN;

/******************************
  HAL_RF_TX_ALIGN_TXLO_RSB_OPTIONS
******************************/
typedef enum {
  RSB_NO_DIV         =   256,   // bit 8 
  NO_EXIT_REQ        =   512,   // bit 9
  //STEP_TIME_10X      = 32768    // bit 15: multiply step time by 10
} HAL_RF_TX_ALIGN_TXLO_RSB_OPTIONS;

typedef PACK(struct) 
{
  uint16 operation_id;
} HAL_RF_COMMON_HEADER;

/******************************
  HAL_RF_INIT_REQ
******************************/
typedef PACK(struct)
{
  HAL_RF_COMMON_HEADER hdr;
  uint16 system;  // HAL_RF_PROTOCOL_SYSTEM
  uint16 path;    // HAL_RF_PATH
} HAL_RF_INIT_REQ;

/******************************
  HAL_RF_INIT_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
} HAL_RF_INIT_RESP;

/******************************
  HAL_RF_STOP_REQ
******************************/
typedef PACK(struct)
{
  HAL_RF_COMMON_HEADER hdr;
} HAL_RF_STOP_REQ;

/******************************
  HAL_RF_STOP_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
} HAL_RF_STOP_RESP;

/******************************
  HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA_APT
******************************/
typedef PACK(struct) {
  byte paState;       // Bit 7 = HDET reading, bit 6 = LP HDET
  byte rgi;
  dword paBias;  
} HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA_APT;

/******************************
  HAL_RF_SWEEP_SEGMENT_CONFIG_DATA
******************************/
typedef PACK(struct) {
  uint16 version;              // can be used for version control as proposed
  uint16 numberOfSegments;     // *(1 NEW: will inform the RFCA how many “segments” are in the field
  uint16 resultSize;           // result size for the response of all segments  
  uint16 segmentDataSize;      // total size of segmentData[] in bytes
  byte segmentData[];          // *(2 raw message style data back to back
} HAL_RF_SWEEP_SEGMENT_CONFIG_DATA;

/******************************
  HAL_RF_WCDMA_TX_DAC_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           // HAL_RF_WCDMA_BAND
  uint16 channel;
  uint16 spreading_factor;
  uint16 step_duration;
  uint16 tx_data_size;
  uint16 number_of_steps;
  uint16 tx_control_data[]; // uint16 * tx_data_size * number_of_steps
}HAL_RF_WCDMA_TX_DAC_REQ;


/******************************
  HAL_RF_WCDMA_TX_DAC_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
}HAL_RF_WCDMA_TX_DAC_RESP;

/******************************
  HAL_RF_WCDMA_RX_RSSI_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           // HAL_RF_WCDMA_BAND
  uint16 channel;
  uint16 rf_signal_path;
  uint16 duration;
  uint16 iq_select;         
  int16_q8 expected_level;
} HAL_RF_WCDMA_RX_RSSI_REQ; 

/******************************
  HAL_RF_WCDMA_RX_RSSI_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;             // HAL_RF_WCDMA_BAND
  uint16 channel;
  int16_q8 rssi_value;
} HAL_RF_WCDMA_RX_RSSI_RESP;

/******************************
  HAL_RF_TDSCDMA_RX_RSSI_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           // HAL_RF_TDSCDMA_BAND
  uint16 channel;
  uint16 rf_signal_path;
  uint16 ts_activity;
  uint16 iq_select;         
  int16_q8 expected_level;
} HAL_RF_TDSCDMA_RX_RSSI_REQ; 

/******************************
  HAL_RF_TDSCDMA_RX_RSSI_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;             // HAL_RF_TDSCDMA_BAND
  uint16 channel;
  int16_q8 rssi_ts0;
  int16_q8 rssi_ts1;
  int16_q8 rssi_ts2;
  int16_q8 rssi_ts3;
  int16_q8 rssi_ts4;
  int16_q8 rssi_ts5;
  int16_q8 rssi_ts6;
} HAL_RF_TDSCDMA_RX_RSSI_RESP;

/******************************
  HAL_RF_CALIBRATION_VALUES_LOAD_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 config;
  uint16 data_select;               // HAL_RF_DATA_SELECT
} HAL_RF_CALIBRATION_VALUES_LOAD_REQ; 

/******************************
  HAL_RF_CALIBRATION_VALUES_LOAD_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
} HAL_RF_CALIBRATION_VALUES_LOAD_RESP;

/******************************
  HAL_RF_CALIBRATION_VALUES_STORE_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 config;
} HAL_RF_CALIBRATION_VALUES_STORE_REQ; 

/******************************
  HAL_RF_CALIBRATION_VALUES_STORE_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
} HAL_RF_CALIBRATION_VALUES_STORE_RESP;

/******************************
  HAL_RF_RFIC_READ_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 identifier;
  uint16 data_offset;
  uint16 data_length;
} HAL_RF_RFIC_READ_REQ; 

/******************************
  HAL_RF_RFIC_READ_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;
  uint16 data_array[];        
} HAL_RF_RFIC_READ_RESP;

/******************************
  HAL_RF_RFIC_WRITE_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 identifier;
  uint16 data_offset;
  uint16 data_length;
  uint16 data_array[];        
} HAL_RF_RFIC_WRITE_REQ; 

/******************************
  HAL_RF_RFIC_WRITE_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
} HAL_RF_RFIC_WRITE_RESP;

/******************************
  HAL_RF_INFORM_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 information;
} HAL_RF_INFORM_REQ; 

/******************************
  HAL_RF_INFORM_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
} HAL_RF_INFORM_RESP;

/******************************
  HAL_RF_FINISH_REQ
******************************/
typedef PACK(struct)
{
  HAL_RF_COMMON_HEADER hdr;
} HAL_RF_FINISH_REQ;

/******************************
  HAL_RF_FINISH_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
} HAL_RF_FINISH_RESP;


/******************************
  HAL_RF_x_RX_ALIGN_CONFIG
******************************/
typedef enum {
  LNA_OFFSET_0       =     0,
  LNA_OFFSET_1       =     1,
  LNA_OFFSET_2       =     2,
  LNA_OFFSET_3       =     3,
  LNA_OFFSET_4       =     4,
  LNA_OFFSET_5       =     5,
  DVGA_OFFSET        =   100,
  INTELLICEIVER      =   101,
  DUAL_CARRIER       =   128, // bit  7: Use dual carrier, second channel is from RFIC WRITE
  XO_CAL_ENABLE      =   256, // bit  8: 1 = Perform XO
  XO_CAL_SELECT      =   512, // bit  9: 0 = Coarse XO, 1 = Fine XO
  XO_CAL_COLLECT     =  1024, // bit 10: 1 = Collect samples
  SKIP_STATE_CACHING =  2048, // bit 11: Skips RF config
  STEP_TIME_10X      = 32768  // bit 15: multiply align time by 10
} HAL_RF_x_RX_ALIGN_CONFIG;

/******************************
  HAL_RF_WCDMA_RX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 expected_level;         
  uint16 duration;
  uint16 config_pattern;          //!< @see HAL_RF_x_RX_ALIGN_CONFIG
} HAL_RF_WCDMA_RX_ALIGN_REQ; 

/******************************
  HAL_RF_WCDMA_RX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_WCDMA_RX_ALIGN_RESP;


/******************************
  HAL_RF_GSM_RX_ALIGN_CONFIG
******************************/
typedef PACK(struct) {  
  byte gainRangeBitMask;            
  byte numberOfAverages;
} HAL_RF_GSM_RX_ALIGN_CONFIG;

/******************************
  HAL_RF_GSM_RX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 expected_level;         
  uint16 duration;
  uint16 config_pattern;        //!< @see HAL_RF_GSM_RX_ALIGN_CONFIG
} HAL_RF_GSM_RX_ALIGN_REQ; 

/******************************
  HAL_RF_GSM_RX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];          //!< Array of uint32[8], each index had RSSI for corresponding gain range
} HAL_RF_GSM_RX_ALIGN_RESP;

/******************************
  HAL_RF_GSM_RX_RSSI_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;  
  uint16 ts_activity;
  uint16 iq_select;
  int16_q8 expected_level;         
} HAL_RF_GSM_RX_RSSI_REQ; 

/******************************
  HAL_RF_GSM_RX_RSSI_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;            
  uint16 channel;
  int16_q8 rssi_ts0;
  int16_q8 rssi_ts1;
  int16_q8 rssi_ts2;
  int16_q8 rssi_ts3;
  int16_q8 rssi_ts4;
  int16_q8 rssi_ts5;
  int16_q8 rssi_ts6;
  int16_q8 rssi_ts7;
} HAL_RF_GSM_RX_RSSI_RESP;

/******************************
  HAL_RF_GSM_RX_WAIT_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 trigger_level;         
  uint16 timeout;
} HAL_RF_GSM_RX_WAIT_REQ; 

/******************************
  HAL_RF_GSM_RX_WAIT_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_GSM_RX_WAIT_RESP;

/******************************
  HAL_RF_WCDMA_RX_WAIT_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 trigger_level;         
  uint16 timeout;
} HAL_RF_WCDMA_RX_WAIT_REQ; 

/******************************
  HAL_RF_WCDMA_RX_WAIT_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_WCDMA_RX_WAIT_RESP;

/******************************
  HAL_RF_TDSCDMA_TX_DBM_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 ts_activity;  
  uint16 modulation;
  uint16 scrambling_code;         
  uint16 spreading_factor;
  uint16 sync_ul_id;
  int16_q8 power[7];       
} HAL_RF_TDSCDMA_TX_DBM_REQ; 

/******************************
  HAL_RF_TDSCDMA_TX_DBM_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
} HAL_RF_TDSCDMA_TX_DBM_RESP;

/******************************
  HAL_RF_WCDMA_TX_DBM_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 spreading_factor;            
  uint16 step_duration;         
  uint16 number_of_steps;   // m
  int16_q8 power[];         // m * 2
} HAL_RF_WCDMA_TX_DBM_REQ; 

/******************************
  HAL_RF_WCDMA_TX_DBM_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
} HAL_RF_WCDMA_TX_DBM_RESP;

/******************************
  HAL_RF_CDMA2K_RX_WAIT_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 trigger_level;         
  uint16 timeout;
} HAL_RF_CDMA2K_RX_WAIT_REQ; 

/******************************
  HAL_RF_CDMA2K_RX_WAIT_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_CDMA2K_RX_WAIT_RESP;

/******************************
  HAL_RF_TDSCDMA_RX_WAIT_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 trigger_level;         
  uint16 timeout;
} HAL_RF_TDSCDMA_RX_WAIT_REQ; 

/******************************
  HAL_RF_TDSCDMA_RX_WAIT_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_TDSCDMA_RX_WAIT_RESP;

/******************************
  HAL_RF_WCDMA_TX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 duration;
  uint16 config_pattern;
} HAL_RF_WCDMA_TX_ALIGN_REQ; 

/******************************
  HAL_RF_WCDMA_TX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_WCDMA_TX_ALIGN_RESP;


/******************************
  HAL_RF_GSM_TX_PCL_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 ts_activity;
  uint16 modulation;
  uint16 burst_length;
  uint16 tx_data_type;
  uint16 pcl[8];
} HAL_RF_GSM_TX_PCL_REQ;


/******************************
  HAL_RF_GSM_TX_PCL_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
} HAL_RF_GSM_TX_PCL_RESP;

/******************************
  HAL_RF_GSM_TX_DAC_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 ts_activity;
  uint16 modulation;
  uint16 burst_length;
  uint16 tx_data_type;
  uint16 tx_data_size;
  uint16 tx_control_data[];
} HAL_RF_GSM_TX_DAC_REQ;

/******************************
  HAL_RF_GSM_TX_DAC_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
} HAL_RF_GSM_TX_DAC_RESP;

/******************************
  HAL_RF_GSM_TX_ALIGN_CONFIG_PATTERN
******************************/
typedef enum {
  HAL_RF_GSM_TX_ALIGN_CONFIG_KVCO_CAL       = 1,
  HAL_RF_GSM_TX_ALIGN_CONFIG_SELF_CAL       = 2,
  HAL_RF_GSM_TX_ALIGN_CONFIG_TUNER_SELF_CAL	= 4,
} HAL_RF_GSM_TX_ALIGN_CONFIG_PATTERN;

/******************************
  HAL_RF_GSM_TX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 duration;
  uint16 config_pattern;          //!< See @ref HAL_RF_GSM_TX_ALIGN_CONFIG_PATTERN
} HAL_RF_GSM_TX_ALIGN_REQ; 

/******************************
  HAL_RF_GSM_TX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_GSM_TX_ALIGN_RESP;


/******************************
  HAL_RF_LTE_RX_ALIGN_REQ
*****************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 expected_level;         
  uint16 duration;
  uint16 config_pattern;          //!< HAL_RF_LTE_RX_ALIGN_REQ_TX_CONFIG_PATTERN
} HAL_RF_LTE_RX_ALIGN_REQ; 

/******************************
  HAL_RF_LTE_RX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_LTE_RX_ALIGN_RESP;

/*******************************
  HAL_RF_LTE_RX_RSSI_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 bandwidth;
  uint16 sf_activity;  
  uint16 iq_select;
  int16_q8 expected_level;
} HAL_RF_LTE_RX_RSSI_REQ;

/*******************************
  HAL_RF_LTE_RX_RSSI_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
  int16_q8 rssi_sf0;
  int16_q8 rssi_sf1;
  int16_q8 rssi_sf2;
  int16_q8 rssi_sf3;
  int16_q8 rssi_sf4;
  int16_q8 rssi_sf5;
  int16_q8 rssi_sf6;
  int16_q8 rssi_sf7;
  int16_q8 rssi_sf8;
  int16_q8 rssi_sf9;
} HAL_RF_LTE_RX_RSSI_RESP;

/******************************
  HAL_RF_LTE_RX_WAIT_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  uint16 bandwidth;
  uint16 iq_select;
  int16_q8 trigger_level;         
  uint16 timeout;
} HAL_RF_LTE_RX_WAIT_REQ; 

/******************************
  HAL_RF_LTE_RX_WAIT_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_LTE_RX_WAIT_RESP;

/******************************
  HAL_RF_LTE_TX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 duration;
  uint16 config_pattern;
} HAL_RF_LTE_TX_ALIGN_REQ; 

/******************************
  HAL_RF_LTE_TX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_LTE_TX_ALIGN_RESP;

/******************************
  HAL_RF_LTE_TX_DAC_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 sf_activity;
  uint16 bandwidth;
  uint16 resource_blocks;
  uint16 resource_blocks_start_index;
  uint16 cyclic_prefix;
  uint16 modulation;
  uint16 tx_data_size;
  uint16 tx_control_data[];      //!< @see HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA
} HAL_RF_LTE_TX_DAC_REQ;

/******************************
  HAL_RF_LTE_TX_DAC_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
} HAL_RF_LTE_TX_DAC_RESP;

/******************************
  HAL_RF_LTE_TX_DBM_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 sf_activity;
  uint16 bandwidth;
  uint16 resource_blocks;
  uint16 resource_blocks_start_index;
  uint16 cyclic_prefix;
  uint16 modulation;
  int16_q8 power[10];      
} HAL_RF_LTE_TX_DBM_REQ;

/******************************
  HAL_RF_LTE_TX_DBM_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
} HAL_RF_LTE_TX_DBM_RESP;

/* ---------------------------------- FDD ---------------------------------- */

/******************************
  HAL_RF_LTEFDD_RX_ALIGN_REQ
*****************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 expected_level;         
  uint16 duration;
  uint16 config_pattern;
} HAL_RF_LTEFDD_RX_ALIGN_REQ; 

/******************************
  HAL_RF_LTEFDD_RX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_LTEFDD_RX_ALIGN_RESP;

/*******************************
  HAL_RF_LTEFDD_RX_RSSI_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 bandwidth;
  uint16 duration;  
  uint16 iq_select;
  int16_q8 expected_level;
} HAL_RF_LTEFDD_RX_RSSI_REQ;

/*******************************
  HAL_RF_LTEFDD_RX_RSSI_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
  int16_q8 rssi_value;
} HAL_RF_LTEFDD_RX_RSSI_RESP;

/******************************
  HAL_RF_LTEFDD_RX_WAIT_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  uint16 bandwidth;
  uint16 iq_select;
  int16_q8 trigger_level;         
  uint16 timeout;
} HAL_RF_LTEFDD_RX_WAIT_REQ; 

/******************************
  HAL_RF_LTEFDD_RX_WAIT_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_LTEFDD_RX_WAIT_RESP;

/******************************
  HAL_RF_LTEFDD_TX_DAC_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 bandwidth;
  uint16 resource_blocks;
  uint16 resource_blocks_start_index;
  uint16 cyclic_prefix;
  uint16 modulation;
  uint16 step_duration;
  uint16 tx_data_size;
  uint16 number_of_steps;
  uint16 tx_control_data[];      //!< @see HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA
} HAL_RF_LTEFDD_TX_DAC_REQ;

/******************************
  HAL_RF_LTEFDD_TX_DAC_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
} HAL_RF_LTEFDD_TX_DAC_RESP;

/******************************
  HAL_RF_LTEFDD_TX_DBM_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 bandwidth;
  uint16 resource_blocks;
  uint16 resource_blocks_start_index;
  uint16 cyclic_prefix;
  uint16 modulation;
  uint16 step_duration;
  uint16 number_of_steps;
  int16_q8 power[];      
} HAL_RF_LTEFDD_TX_DBM_REQ;

/******************************
  HAL_RF_LTEFDD_TX_DBM_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
} HAL_RF_LTEFDD_TX_DBM_RESP;

/******************************
  HAL_RF_LTEFDD_TX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 duration;
  uint16 config_pattern;
} HAL_RF_LTEFDD_TX_ALIGN_REQ; 

/******************************
  HAL_RF_LTEFDD_TX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_LTEFDD_TX_ALIGN_RESP;

/* ---------------------------------- TDD ---------------------------------- */

/******************************
  HAL_RF_LTETDD_RX_ALIGN_REQ
*****************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 expected_level;         
  uint16 duration;
  uint16 config_pattern;
} HAL_RF_LTETDD_RX_ALIGN_REQ; 

/******************************
  HAL_RF_LTETDD_RX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_LTETDD_RX_ALIGN_RESP;

/*******************************
  HAL_RF_LTETDD_RX_RSSI_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 bandwidth;
  uint16 radioframe_activity;  
  uint16 iq_select;
  int16_q8 expected_level;
} HAL_RF_LTETDD_RX_RSSI_REQ;

/*******************************
  HAL_RF_LTETDD_RX_RSSI_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
  int16_q8 rssi_sf0;
  int16_q8 rssi_sf1;
  int16_q8 rssi_sf2;
  int16_q8 rssi_sf3;
  int16_q8 rssi_sf4;
  int16_q8 rssi_sf5;
  int16_q8 rssi_sf6;
  int16_q8 rssi_sf7;
  int16_q8 rssi_sf8;
  int16_q8 rssi_sf9;
} HAL_RF_LTETDD_RX_RSSI_RESP;

/******************************
  HAL_RF_LTETDD_RX_WAIT_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  uint16 bandwidth;
  uint16 iq_select;
  int16_q8 trigger_level;
  uint16 trigger_mode;
  uint16 timeout;
} HAL_RF_LTETDD_RX_WAIT_REQ; 

/******************************
  HAL_RF_LTETDD_RX_WAIT_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_LTETDD_RX_WAIT_RESP;

/******************************
  HAL_RF_LTETDD_TX_DAC_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 radioframe_activity;
  uint16 bandwidth;
  uint16 resource_blocks;
  uint16 resource_blocks_start_index;
  uint16 cyclic_prefix;
  uint16 modulation;
  /*uint16 step_duration;*/
  uint16 tx_data_size;
  /*uint16 number_of_steps;*/
  uint16 tx_control_data[];      //!< @see HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA
} HAL_RF_LTETDD_TX_DAC_REQ;

/******************************
  HAL_RF_LTETDD_TX_DAC_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
} HAL_RF_LTETDD_TX_DAC_RESP;

/******************************
  HAL_RF_LTETDD_TX_DBM_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 radioframe_activity;
  uint16 bandwidth;
  uint16 resource_blocks;
  uint16 resource_blocks_start_index;
  uint16 cyclic_prefix;
  uint16 modulation;
  /*uint16 step_duration;
  uint16 number_of_steps;*/
  int16_q8 power[10];      
} HAL_RF_LTETDD_TX_DBM_REQ;

/******************************
  HAL_RF_LTETDD_TX_DBM_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
} HAL_RF_LTETDD_TX_DBM_RESP;

/******************************
  HAL_RF_LTETDD_TX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 duration;
  uint16 config_pattern;
} HAL_RF_LTETDD_TX_ALIGN_REQ; 

/******************************
  HAL_RF_LTETDD_TX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_LTETDD_TX_ALIGN_RESP;

/* --------------------------------- CDMA2K -------------------------------- */

/******************************
  HAL_RF_CDMA2K_RX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 expected_level;         
  uint16 duration;
  uint16 config_pattern;          
} HAL_RF_CDMA2K_RX_ALIGN_REQ; 

/******************************
  HAL_RF_CDMA2K_RX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_CDMA2K_RX_ALIGN_RESP;

/******************************
  HAL_RF_CDMA2K_TX_DAC_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 spreading_factor;
  uint16 radio_config;     // 1 - 4 Radio Configuration, 1 = RC1, 2 = RC2, 3 = RC3, 4 = RC4
  uint16 step_duration;
  uint16 tx_data_size;
  uint16 number_of_steps;
  uint16 tx_control_data[]; //!<  uint16 * tx_data_size * number_of_steps
}HAL_RF_CDMA2K_TX_DAC_REQ;

/******************************
  HAL_RF_CDMA2K_TX_DAC_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
}HAL_RF_CDMA2K_TX_DAC_RESP;

/******************************
  HAL_RF_CDMA2K_TX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 duration;
  uint16 config_pattern;
} HAL_RF_CDMA2K_TX_ALIGN_REQ; 

/******************************
  HAL_RF_CDMA2K_TX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_CDMA2K_TX_ALIGN_RESP;

/******************************
  HAL_RF_CDMA2K_TX_DBM_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 spreading_factor;  
  uint16 radio_config;     // 1 - 4 Radio Configuration, 1 = RC1, 2 = RC2, 3 = RC3, 4 = RC4
  uint16 step_duration;         
  uint16 number_of_steps;   // m
  int16_q8 power[];         // m * 2
} HAL_RF_CDMA2K_TX_DBM_REQ; 

/******************************
  HAL_RF_CDMA2K_TX_DBM_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
} HAL_RF_CDMA2K_TX_DBM_RESP;


/******************************
  HAL_RF_CDMA2K_RX_RSSI_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           // HAL_RF_CDMA2K_BAND
  uint16 channel;
  uint16 rf_signal_path;
  uint16 duration;
  uint16 iq_select;         
  uint16 expected_level;
} HAL_RF_CDMA2K_RX_RSSI_REQ; 

/******************************
  HAL_RF_CDMA2K_RX_RSSI_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;             
  uint16 channel;
  uint16 rssi_value;
} HAL_RF_CDMA2K_RX_RSSI_RESP;

/******************************
  HAL_RF_ARB_TX_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 system;  //!<  HAL_RF_PROTOCOL_SYSTEM
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;    
  uint16 step_duration;
  uint16 number_of_steps;
  uint16 config_pattern;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_ARB_TX_REQ; 

/******************************
  HAL_RF_ARB_TX_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_ARB_TX_RESP;

/******************************
  HAL_RF_RX_SWEEP_STEP_CONFIG
******************************/
typedef PACK(struct) {
  uint16 actions;
  int16 expectedRxAgc;
  byte   powerMode;
  byte   rxFlag;
} HAL_RF_RX_SWEEP_STEP_CONFIG;

/******************************
  HAL_RF_TDSCDMA_RX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 expected_level;         
  uint16 duration;
  uint16 config_pattern;          //!< @see HAL_RF_x_RX_ALIGN_CONFIG
} HAL_RF_TDSCDMA_RX_ALIGN_REQ; 

/******************************
  HAL_RF_TDSCDMA_RX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_TDSCDMA_RX_ALIGN_RESP;

/******************************
  HAL_RF_TDSCDMA_TX_ALIGN_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 duration;
  uint16 config_pattern;
} HAL_RF_TDSCDMA_TX_ALIGN_REQ; 

/******************************
  HAL_RF_TDSCDMA_TX_ALIGN_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 data_length;            
  uint16 data_array[];
} HAL_RF_TDSCDMA_TX_ALIGN_RESP;

/******************************
  HAL_RF_TDSCDMA_TX_DAC_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 ts_activity;
  uint16 modulation;
  uint16 scrambling_code;
  uint16 spreading_factor;
  uint16 sync_ul_id;
  uint16 tx_data_size;
  uint16 tx_control_data[];      //!< @see HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA
} HAL_RF_TDSCDMA_TX_DAC_REQ;

/******************************
  HAL_RF_TDSCDMA_TX_DAC_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
} HAL_RF_TDSCDMA_TX_DAC_RESP;

/******************************
  HAL_RF_TD_SCDMA_TIMESLOT
******************************/
typedef enum {
  TS_TDSCDMA_NO_ACTIVITY     = 0x0000,
  TS_TDSCDMA_0       = 0x0001,
  TS_TDSCDMA_1       = 0x0002,
  TS_TDSCDMA_2       = 0x0004,
  TS_TDSCDMA_3       = 0x0008,
  TS_TDSCDMA_4       = 0x0010,
  TS_TDSCDMA_5       = 0x0020,
  TS_TDSCDMA_6       = 0x0040
} HAL_RF_TD_SCDMA_TIMESLOT;

/******************************
  HAL_RF_TD_SCDMA_MODULATION
******************************/
typedef enum {
  HAL_RF_TD_SCDMA_MODULATION_QPSK    = 0x0000, // QPSK modulation
  HAL_RF_TD_SCDMA_MODULATION_8PSK    = 0x0001, // 8-PSK modulation
  HAL_RF_TD_SCDMA_MODULATION_16QAM   = 0x0002  // 16QAM modulati
} HAL_RF_TD_SCDMA_MODULATION;

/******************************
  GSM_KV_CAL_DATA
******************************/
typedef PACK(struct) {
  uint16 lowChannel; 
  uint16 highChannel;
} GSM_KV_CAL_DATA;

/******************************
  GSM_DRIVER_AMP_CAL_DATA
******************************/
typedef PACK(struct) {
  /*uint16 segmentLength; //!<  Duration of segment. Unit 3.692uS */
  byte rgi;             //!<  Rgi of segment
  byte gsmMode;         //!<  Mode of segment, GSM mode - 1, EDGE Mode - 0
  uint16 paRange;         //!<  PA Range of segment
} GSM_DRIVER_AMP_CAL_DATA;

/******************************
  GSM_PREDISTORTION_CAL_DATA
******************************/
typedef PACK(struct) {
  uint16 preDcDuration; //!<  Duration of the DC waveform (Units : Quarter Symbols)
  uint16 edgeDuration;  //!<  Duration of the calibrated EDGE waveform (Units : Quarter Symbols)
  uint16 calRgi;        //!<  Calibration RGI (Units : None)
} GSM_PREDISTORTION_CAL_DATA;

/******************************
  GSM_ENVDC_CS_CAL_DATA
******************************/
typedef PACK(struct) {
  uint16 stepDuration;  //!<  Duration of step in unit symbols
  uint16 envelopeDc;    //!<  Envelope DC value for step
} GSM_ENVDC_CS_CAL_DATA;

/******************************
  HAL_RF_TX_ARB_CONFIG_PATTERN
******************************/
typedef enum  
{
  GSM_DRIVER_AMP_CAL      = 0x0001, 
  GSM_PREDISTRORTION_CAL  = 0x0002, 
  TDSCDMA_TX_CAL          = 0x0003,
  LTETDD_TX_CAL           = 0x0004,
  FLEX_CAL_V2             = 0x0006,
  FLEX_CAL_V2_COMPRESSED  = 0x0007
} HAL_RF_TX_ARB_CONFIG_PATTERN;

/******************************
  HAL_RF_RADIO_CONFIG
******************************/
typedef enum {
  HAL_RF_RC1 = 1, 
  HAL_RF_RC2 = 2,
  HAL_RF_RC3 = 3,
  HAL_RF_RC4 = 4
} HAL_RF_RADIO_CONFIG;

/******************************
  HAL_RF_FREQ_ADJUST_DATA
******************************/
typedef PACK(struct) {
  bool  freqAdjEnabled;     //<! Flag to indicate if FREQ ADJ FTM message is use at all
  int32 userFreqAdjustPpb;  //<! Amount of adjustement in ppb
  uint32 centerFreqHz;       //<! Center frequency for channel
  byte enableXoFreqAdjust;  //<! 0 - only user ppb, 1 - user and XO ppb (default)
} HAL_RF_FREQ_ADJUST_DATA;

typedef enum {
  HAL_RF_FREQ_ADJUST_DATA_OFFSET_FREQ_ADJUST_ENABLED = 0,
  HAL_RF_FREQ_ADJUST_DATA_OFFSET_USER_FREQ_ADJUST_PPB_LW, // DWORD split to this and next item, you must give both
  HAL_RF_FREQ_ADJUST_DATA_OFFSET_USER_FREQ_ADJUST_PPB_HW,
  HAL_RF_FREQ_ADJUST_DATA_OFFSET_CENTER_FREQ_LW,          // DWORD split to this and next item, you must give both
  HAL_RF_FREQ_ADJUST_DATA_OFFSET_CENTER_FREQ_HW,
  HAL_RF_FREQ_ADJUST_DATA_OFFSET_ENABLE_XO_FREQ_ADJUST,
} HAL_RF_FREQ_ADJUST_DATA_OFFSET;

/******************************
  HAL_RF_GSM_RX_WAIT_V2_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;
  int16_q8 trigger_level;
  uint16 trigger_mode;
  uint16 timeout;
} HAL_RF_GSM_RX_WAIT_V2_REQ; 

/******************************
  HAL_RF_GSM_RX_WAIT_V2_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_GSM_RX_WAIT_V2_RESP;

/******************************
  HAL_RF_WCDMA_RX_WAIT_V2_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 trigger_level;     
  uint16 trigger_mode;
  uint16 timeout;
} HAL_RF_WCDMA_RX_WAIT_V2_REQ; 

/******************************
  HAL_RF_WCDMA_RX_WAIT_V2_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_WCDMA_RX_WAIT_V2_RESP;

/******************************
  HAL_RF_CDMA2K_RX_WAIT_V2_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 trigger_level;         
  uint16 trigger_mode;
  uint16 timeout;
} HAL_RF_CDMA2K_RX_WAIT_V2_REQ; 

/******************************
  HAL_RF_CDMA2K_RX_WAIT_V2_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_CDMA2K_RX_WAIT_V2_RESP;


/******************************
  HAL_RF_TDSCDMA_RX_WAIT_V2_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  int16_q8 trigger_level;         
  uint16 trigger_mode;
  uint16 timeout;
} HAL_RF_TDSCDMA_RX_WAIT_V2_REQ; 

/******************************
  HAL_RF_TDSCDMA_RX_WAIT_V2_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_TDSCDMA_RX_WAIT_V2_RESP;

/******************************
  HAL_RF_LTEFDD_RX_WAIT_V2_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  uint16 bandwidth;
  uint16 iq_select;
  int16_q8 trigger_level;         
  uint16 trigger_mode;
  uint16 timeout;
} HAL_RF_LTEFDD_RX_WAIT_V2_REQ; 

/******************************
  HAL_RF_LTEFDD_RX_WAIT_V2_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;       
  int16_q8 rssi_value;
} HAL_RF_LTEFDD_RX_WAIT_V2_RESP;

/******************************
  HAL_RF_LTEFDD_RX_IQ_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  uint16 bandwidth;
  uint16 duration;
  uint16 iq_select;
  int16_q8 expected_level;         
} HAL_RF_LTEFDD_RX_IQ_REQ; 

/******************************
  HAL_RF_LTEFDD_RX_IQ_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;   
  uint16 iq_select;
  uint16 vector_length;
  uint16 iq_data[]; // Just a place holder to point to
  /* I sample vector */
  /* Q sample vector */
} HAL_RF_LTEFDD_RX_IQ_RESP;

/******************************
  HAL_RF_LTETDD_RX_IQ_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  uint16 bandwidth;
  uint16 radioframe_activity;
  uint16 iq_select;
  int16_q8 expected_level;         
} HAL_RF_LTETDD_RX_IQ_REQ; 

/******************************
  HAL_RF_LTETDD_RX_IQ_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
  uint16 radioframe_activity;
  uint16 iq_select;
  uint16 vector_length;
  uint16 iq_data[]; // Just a place holder to point to
  /* SF0 I sample vector */
  /* SF0 Q sample vector */
  /* SF1 I sample vector */
  /* SF1 Q sample vector */
  /* SF2 I sample vector */
  /* SF2 Q sample vector */
  /* SF3 I sample vector */
  /* SF3 Q sample vector */
  /* SF4 I sample vector */
  /* SF4 Q sample vector */
  /* SF5 I sample vector */
  /* SF5 Q sample vector */
  /* SF6 I sample vector */
  /* SF6 Q sample vector */
  /* SF7 I sample vector */
  /* SF7 Q sample vector */
  /* SF8 I sample vector */
  /* SF8 Q sample vector */
  /* SF9 I sample vector */
  /* SF9 Q sample vector */
} HAL_RF_LTETDD_RX_IQ_RESP;

/******************************
  HAL_RF_WCDMA_RX_IQ_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  uint16 duration;
  uint16 iq_select;
  int16_q8 expected_level;         
} HAL_RF_WCDMA_RX_IQ_REQ; 

/******************************
  HAL_RF_WCDMA_RX_IQ_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;   
  uint16 iq_select;
  uint16 vector_length;
  uint16 iq_data[]; // Just a place holder to point to
  /* I sample vector */
  /* Q sample vector */
} HAL_RF_WCDMA_RX_IQ_RESP;

/******************************
  HAL_RF_CDMA2K_RX_IQ_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  uint16 duration;
  uint16 iq_select;
  int16_q8 expected_level;         
} HAL_RF_CDMA2K_RX_IQ_REQ; 

/******************************
  HAL_RF_CDMA2K_RX_IQ_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;   
  uint16 iq_select;
  uint16 vector_length;
  uint16 iq_data[]; // Just a place holder to point to
  /* I sample vector */
  /* Q sample vector */
} HAL_RF_CDMA2K_RX_IQ_RESP;

/******************************
  HAL_RF_GSM_RX_IQ_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;
  uint16 channel;
  uint16 rf_signal_path;
  uint16 ts_activity;
  uint16 iq_select;
  int16_q8 expected_level;         
} HAL_RF_GSM_RX_IQ_REQ; 

/******************************
  HAL_RF_GSM_RX_IQ_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
  uint16 ts_activity;
  uint16 iq_select;
  uint16 vector_length;
  uint16 iq_data[]; // Just a place holder to point to
  /* TS1 I sample vector */
  /* TS1 Q sample vector */
  /* TS2 I sample vector */
  /* TS2 Q sample vector */
  /* TS3 I sample vector */
  /* TS3 Q sample vector */
  /* TS4 I sample vector */
  /* TS4 Q sample vector */
  /* TS5 I sample vector */
  /* TS5 Q sample vector */
  /* TS6 I sample vector */
  /* TS6 Q sample vector */
  /* TS7 I sample vector */
  /* TS7 Q sample vector */
  /* TS8 I sample vector */
  /* TS8 Q sample vector */  
} HAL_RF_GSM_RX_IQ_RESP;


/******************************
  HAL_RF_TDSCDMA_RX_IQ_REQ
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 rf_band;           
  uint16 channel;
  uint16 rf_signal_path;            
  uint16 ts_activity;
  uint16 iq_select;
  int16_q8 expected_level;         
} HAL_RF_TDSCDMA_RX_IQ_REQ; 

/******************************
  HAL_RF_TDSCDMA_RX_IQ_RESP
******************************/
typedef PACK(struct) {
  HAL_RF_COMMON_HEADER hdr;
  uint16 status;
  uint16 rf_band;
  uint16 channel;
  uint16 ts_activity;
  uint16 iq_select;
  uint16 vector_length;
  uint16 iq_data[]; // Just a place holder to point to
  /* SF1 I sample vector */
  /* SF1 Q sample vector */
  /* SF2 I sample vector */
  /* SF2 Q sample vector */
  /* SF3 I sample vector */
  /* SF3 Q sample vector */
  /* SF4 I sample vector */
  /* SF4 Q sample vector */
  /* SF5 I sample vector */
  /* SF5 Q sample vector */
  /* SF6 I sample vector */
  /* SF6 Q sample vector */
  /* SF7 I sample vector */
  /* SF7 Q sample vector */
} HAL_RF_TDSCDMA_RX_IQ_RESP;

#ifdef _MSC_VER
  #pragma warning( pop )
  #pragma pack(pop) // Restore
#endif

#endif //_RFCA_HAL_MESSAGE_DEFINITION_H_
