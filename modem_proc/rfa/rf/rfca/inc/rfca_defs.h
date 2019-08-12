/*!
  @file
  rfca_defs.h
 
  @brief
  Misc defines for various IDs used in RFCA.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_defs.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/09/15   aki     Reduce log message buffer size
01/03/14   aki     Removed RFCA_ASSERT
08/09/13   aki     Added RX options
06/10/13   aki     Mainline RFCA 
06/08/13   hm      Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
05/02/13   aki     Added _FTM_RX_WAIT_DBM_V2
04/29/13   aki     Added LTE band 42
04/10/13   aki     Added GSM sweep cmd ids
04/10/13   aki     Added _FTM_LTE_SET_LNA_STATE_DBM
02/04/13   aki     Added _FTM_LTE_SET_TDD_PARAMS
12/10/12   aki     Remove old and new XO cal command id
07/23/12   aki     TDSCDMA frames, sweep F3 debug flag moved
07/13/12   aki     TDSCDMA, V3 default time for config
05/25/12   aki     Initial level for TX and log buffer size increase
04/23/12   aki     New defines, XO, ENH sweep, LTE
03/01/12   aki     Frame length defines, SELF CAL, removed XO magic numbers
02/17/12   aki     Added XO, THER, KV, FTM_CMD_ID_OFFSET and removed RFNV STATUS
01/25/12   aki     Diag V2 delayed response implementation
10/28/11   aki     Initial revision

==============================================================================*/



#ifndef _RFCA_DEFS_H_
#define _RFCA_DEFS_H_

#include "rfa_variation.h"
#include "comdef.h"

/*!
 @brief Maximum message size for queue items in task queues
 */
#define MAX_MESSAGE_SIZE 4096

#define DIAG_LOG_RESP_CMD_CODE         16   ///< DIAG command id for log messages
#define DIAG_BAD_COMMAND_CMD_CODE      19   ///< DIAG command
#define DIAG_MODE_CONTROL_CMD_CODE     41   ///< 
#define DIAG_MODE_CONTROL_FTM           3   ///< 


#define FTM_SEQ_LOG_CODE            0x117C  //!<' FTM Version 2 log file

#define FTM_SEQ_FTM_LOG_ID          0x0C    //!<' FTM log identifer for FTM_LOG_ID field
#define FTM_SEQ_DIAG_LOG_F          0x11    //!<' Diagnostic Log packet Request/Response command ID

#define GSM_FRAME_LENGTH_US         4615
#define GSM_SLOT_LENGTH_US          (GSM_FRAME_LENGTH_US / 8.0) 

#define LTE_FRAME_LENGTH_US         10000
#define LTE_SUBFRAME_LENGTH_US      (LTE_FRAME_LENGTH_US/10.0) 

#define WCDMA_FRAME_LENGTH_US       10000

#define CDMA2K_FRAME_LENGTH_US      20000

#define TDSCDMA_SUBFRAME_LENGTH_US   5000
#define TDSCDMA_NUM_OF_SUBFRAMES        7
// FTM TX function only takes 5 slots
#define TDSCDMA_NUM_OF_SUBFRAMES_FOR_FTM   5  



// Set the expected power to this dBm level before 
// turning on TX in TX DBM function
#define TX_DBM_INITIAL_LEVEL        -39

#define LTE_NETWORK_SIGNAL_VALUE 1

#define FTM_SEQ_QUEUE_COMMANDS 0

#define MAX_RFNV_ITEM_SIZE (RFCA_PACKET_HELPER_MAX_PACKET_SIZE - sizeof(ftm_rfnv_common_header_type))

#define FTM_MODE_ID_OFFSET 2
#define FTM_CMD_ID_OFFSET  4

#define FTM_COMMON_SUCCESS 0

#define RFCA_FUNCTION_SUCCESS 0
#define RFCA_FUNCTION_FAIL    1

#define FTM_DVGA_OFFSET       6
#define FTM_LNA_OFFSET        8

#define DELAY_AFTER_RF_CONFIG_US 5000 
                                        //   850          900          1800           1900
#define KVCO_DEFAULT_CHANNEL_TABLE    {{ 128,  251}, { 975,  124}, { 512,  885}, { 512,  810}}

#define MAX_HAL_LOG_BUFFER_SZIE 4096

// ------------------------------------------------------------------
#define FTM_XO_STATE_SIZE        2 
#define FTM_XO_NOM_COEF_SIZE    30
#define FTM_XO_CAL_COEF_SIZE    30
#define FTM_XO_CAL_INFO_SIZE    80

#define FTM_XO_STATE_OFFSET     10
#define FTM_XO_NOM_COEF_OFFSET  (FTM_XO_STATE_OFFSET + FTM_XO_STATE_SIZE)
#define FTM_XO_CAL_COEF_OFFSET  (FTM_XO_NOM_COEF_OFFSET + FTM_XO_NOM_COEF_SIZE + 2) 
#define FTM_XO_CAL_INFO_OFFSET  (FTM_XO_CAL_COEF_OFFSET + FTM_XO_CAL_COEF_SIZE + 2)

#define RFCA_XO_STATE_OFFSET     0
#define RFCA_XO_NOM_COEF_OFFSET  (RFCA_XO_STATE_OFFSET + FTM_XO_STATE_SIZE/2)
#define RFCA_XO_CAL_COEF_OFFSET  (RFCA_XO_NOM_COEF_OFFSET + FTM_XO_NOM_COEF_SIZE/2)
#define RFCA_XO_CAL_INFO_OFFSET  (RFCA_XO_CAL_COEF_OFFSET + FTM_XO_CAL_COEF_SIZE/2)

#define RFCA_XO_DATA_ARRAY_SIZE  ((FTM_XO_STATE_SIZE + FTM_XO_NOM_COEF_SIZE + FTM_XO_CAL_COEF_SIZE + FTM_XO_CAL_INFO_SIZE)/2) 

#define FTM_XO_COARSE_STATE_PASS   1
#define FTM_FT_COLLECT_SAMPLE_PASS 2
#define FTM_FT_CURVE_STATE_PASS    5

#define XO_VERSION 3

typedef enum {
  XO_TECH_1X,
  XO_TECH_WCDMA,
  XO_TECH_GSM,
  XO_TECH_LTE,
  XO_TECH_TDSCDMA
} XO_TECHNOLOGY;

typedef enum {
  UL_CHANNEL = 0,
  DL_CHANNEL = 1
} CHANNEL_TYPE;

/**
FTM Commands enumeration.

These are renamed with an _ prefix in order to avoid conflict with
the function names.

*/
typedef enum {
  _FTM_SET_PDM				      =   0,	//!< Sets a PDM to a value
  _FTM_SET_TX_ON            =   2,  //!< Sets the transmit circuitry ON
  _FTM_SET_TX_OFF           =   3,  //!< Sets the transmit circuitry OFF
  _FTM_SET_MODE	            =   7,  //!< Sets the phones operating mode
  _FTM_SET_CHAN	            =   8,  //!< Sets the phones current channel
  _FTM_CDMA_CW_WAVEFORM		  =  50,	//!< Turns on/off I/Q spreading of CDMA reverse	link
  _FTM_GET_RX_AGC				    =  53,	//!< Gets AGC value of Rx AGC loop
  _FTM_SET_PA_RANGE			    =  54,	//!< Sets PA range
  _FTM_SET_LNA_RANGE			  =  58,	//!< Sets LNA range
  _FTM_GET_ADC_VAL			    =  59,	//!< Gets ADC value
  _FTM_SET_LNA_OFFSET			  =  81,	//!< Sets LNA offset
  _FTM_SET_DVGA_OFFSET		  = 111,	//!< Sets DVGA offset
  _FTM_GET_DVGA_OFFSET		  = 117,	//!< Gets DVGA Offset
  _FTM_GET_LNA_OFFSET			  = 118,	//!< Gets LNA Offset
  _FTM_SET_SECONDARY_CHAIN	= 121,	//!< Set secondary Rx mode
  _FTM_SET_RF_POWER_MODE    = 128,
  _FTM_LOAD_RF_NV           = 131,  //!< load RF NV from NV
  _FTM_DO_INTERNAL_DEVICE_CAL = 132,
  _FTM_SET_CALIBRATION_STATE= 137,  //!<! Set the calibration state to active or inactive
  _FTM_GET_ENH_THERM        = 142,
  _FTM_SET_TX_POWER_DBM     = 176,  //!<' FTM RF command to program the PA range, LUT index and Digital Gain to reach programmed Tx Power Level
  _FTM_LTE_SET_NET_SIG_VALUE = 177,
  _FTM_GET_RX_LEVEL_DBM     = 179,
  _FTM_LTE_SET_LNA_STATE_DBM= 185,
  _FTM_SET_FREQ_ADJUST      = 186,
  _FTM_LTE_SET_TDD_PARAMS   = 187,
  _FTM_SET_TRANSMIT_BURST		= 217,
  _FTM_SET_RX_BURST			    = 225,
  _FTM_SET_TX_POWER_LEVEL		= 227,
  _FTM_GET_RSSI				      = 230,
  _FTM_SET_PATH_DELAY			  = 237,	//!< Set the path delay for AMAM/AMPM
  _FTM_SET_TX_FRAME_MATRIX	= 240,	//!< Set the TX frame matrix
  _FTM_RX_GAIN_RANGE_CAL		= 246,	//!< GSM Rx Gain Range Calibration (multiple gain states in one command)
  _FTM_GSM_SET_LINEAR_RGI   = 247,

  _FTM_TX_KV_CAL					      = 251, //!< Kvco Calibration
  _FTM_SET_GSM_LINEAR_PA_RANGE  = 252, //!< GSM PA Range
  _FTM_GSM_TX_KV_CAL_V2         = 254,
  _FTM_GSM_TX_ENVDC_CS_SWEEP    = 308, //!< GSM Envelope DC Carrier Suppression Sweep
  _FTM_GSM_TX_GAIN_SWEEP        = 310, //!< GSM Tx DA Calibration
  _FTM_TX_CFG2_AMAM_SWEEP_V2    = 324, //!< EDGE Tx Predistortion Calibration V2
  _FTM_DO_SELF_CAL              = 325, //!< For all the self calibration routines of RF Devices & MSM 
  _FTM_GSM_SETUP_RX_BURST_FOR_EXPECTED_PWR = 329, //!< This might change its name.... Aki
  _FTM_GSM_GET_MEAS_PWR = 330,

  /*LTE Command IDs*/
   _FTM_SET_TX_BANDWIDTH = 400,
   _FTM_SET_RX_BANDWIDTH,
   _FTM_SET_TX_GAIN_INDEX,
   _FTM_SET_LTE_WAVEFORM,
   _FTM_STOP_LTE_WAVEFORM,

  _FTM_LTE_SET_MODULATION_TYPE = 512,			// Set modulation type for LTE
  
  _FTM_SET_MULTI_CHAN       = 603,
  _FTM_RFNV_READ		        = 604,  //!< Read NV item via FTM API
  _FTM_RFNV_WRITE	          = 605,  //!< Write NV item via FTM API

  _FTM_CALIBRATION_SWEEP = 608,             /*!< Calibration Sweep */
  _FTM_CALIBRATION_SWEEP_GET_RESULTS = 609, /*!< Calibration Sweep Get results*/

  _FTM_RX_WAIT_DBM          = 611,

  _FTM_XO_CALIBRATION       = 624,
  _FTM_RX_WAIT_DBM_V2       = 634,

  _FTM_TDSCDMA_SET_BURST_TX_PARAMS = 712,
  _FTM_TDSCDMA_BURST_TX = 713,
  _FTM_TDSCDMA_BURST_RX = 714,

  _FTM_GSM_FRAME_SWEEP_CFG_CMD = 864,
  _FTM_GSM_FRAME_SWEEP_EXEC_CMD = 865,
  _FTM_GSM_FRAME_SWEEP_GET_RESULTS_CMD = 866,
  _FTM_GSM_FRAME_SWEEP_CLEANUP_CMD = 867, 
  _FTM_DO_TUNER_SELF_CAL = 904,
} FTM_Command_ID_Type;




typedef enum 
{
  RF_DEVICE_0 = 1,
  RF_DEVICE_1 = 2
} RF_DEVICE_ID;

/**
Diagnostic Subsystems

*/
typedef enum
{
  RANDOM_DATA = 0,
  TONE = 1,
  BUFFER_DATA =2
} GsmTxBurstSource;


// Modulation for frame matrix function
typedef enum {
      MCS1 = 0, 
      MCS2 = 1, 
      MCS9 = 8
    } GSM_MODULATION;

/*!
 @brief Enumeration of RF Mode ID's, from services\ftm\ftm.h
*/
typedef enum
{

   PHONE_MODE_FM=1,				//!<' FM
   PHONE_MODE_SLEEP=2,				//!<' Sleep Mode
   PHONE_MODE_GPS=3,				//!<' GPS
   PHONE_MODE_GPS_SINAD=4,			//!<' GPS SINAD
   PHONE_MODE_CDMA_800=5,			//!<' CDMA 800
   PHONE_MODE_CDMA_1900=6,			//!<' CDMA 1900
   PHONE_MODE_CDMA_1800=8,			//!<' CDMA 1800
   PHONE_MODE_J_CDMA=14,			//!<' JCDMA
   PHONE_MODE_CDMA_450=17,			//!<' CDMA 450
   PHONE_MODE_CDMA_IMT=19,			//!<' CDMA IMT
   PHONE_MODE_CDMA_1900_EXT=26,	//!<' Secndary CDMA 1900MHz Band, Band Class 14
   PHONE_MODE_CDMA_450_EXT=27,		//!<' CDMA BC 11 (450 Extension)
   PHONE_MODE_CDMA_800_SEC=33,		//!<' Secondary CDMA 800MHz Band, Band Class 10


   PHONE_MODE_WCDMA_IMT=9,			//!<' WCDMA IMT, Band I
   PHONE_MODE_GSM_900=10,			//!<' GSM 900
   PHONE_MODE_GSM_1800=11,			//!<' GSM 1800
   PHONE_MODE_GSM_1900=12,			//!<' GSM 1900
   PHONE_MODE_BLUETOOTH=13,		//!<' Bluetooth
   PHONE_MODE_WCDMA_1900A=15,		//!<' WCDMA 1900 A, Band II Add
   PHONE_MODE_WCDMA_1900B=16,		//!<' WCDMA 1900 B, Band II Gen
   PHONE_MODE_GSM_850=18,			//!<' GSM 850
   PHONE_MODE_WCDMA_800=22,		//!<' WCDMA 800, Band V Gen
   PHONE_MODE_WCDMA_800A=23,		//!<' WCDMA 800, Band V Add
   PHONE_MODE_WCDMA_1800=25,		//!<' WCDMA 1800, Band III
   PHONE_MODE_WCDMA_BC4=28,		//!<' WCDMA BC4-used for both Band IV Gen and Band IV Add
   PHONE_MODE_WCDMA_BC8=29,		//!<' WCDMA BC8, Band VIII

   PHONE_MODE_MF_700=30,			//!<' MediaFLO
   PHONE_MODE_WCDMA_BC9=31,		//!<' WCDMA BC9 (1750MHz & 1845MHz), Band IX
   PHONE_MODE_CDMA_BC15=32,		//!<' CDMA Band Class 15
   PHONE_MODE_LTE_B1=34,			//!<' LTE Band Class 1
   PHONE_MODE_LTE_B7=35,			//!<' LTE Band Class 7
   PHONE_MODE_LTE_B4=42,			//!<' LTE Band Class 4
   PHONE_MODE_LTE_B11=41,			//!<' LTE Band Class 11
   PHONE_MODE_LTE_B13=36, 		//!<' LTE Band Class 13
   PHONE_MODE_LTE_B17=37,  		//!<' LTE Band Class 17
   PHONE_MODE_LTE_B38=38, 		//!<' LTE Band Class 38
   PHONE_MODE_LTE_B40=39, 		//!<' LTE Band Class 40
   PHONE_MODE_WCDMA_1500=40,		//!<' WCDMA BC11 (1500MHz) Band XI

   PHONE_MODE_LTE_B2=43,
   PHONE_MODE_LTE_B3=44,
   PHONE_MODE_LTE_B5=45,
   PHONE_MODE_LTE_B6=46,
   PHONE_MODE_LTE_B8=47,
   PHONE_MODE_LTE_B9=48,
   PHONE_MODE_LTE_B10=49,
   PHONE_MODE_LTE_B12=50,
   PHONE_MODE_LTE_B14=51,
   PHONE_MODE_LTE_B15=52,
   PHONE_MODE_LTE_B16=53,
   PHONE_MODE_LTE_B18=54,
   PHONE_MODE_LTE_B19=55,
   PHONE_MODE_LTE_B20=56,
   PHONE_MODE_LTE_B21=57,
   PHONE_MODE_LTE_B22=58,
   PHONE_MODE_LTE_B23=59,
   PHONE_MODE_LTE_B24=60,
   PHONE_MODE_LTE_B25=61,
   PHONE_MODE_LTE_B26=62,
   PHONE_MODE_LTE_B27=63,
   PHONE_MODE_LTE_B28=64,
   PHONE_MODE_LTE_B29=65,
   PHONE_MODE_LTE_B30=66,
   PHONE_MODE_LTE_B31=67,
   PHONE_MODE_LTE_B32=68,
   PHONE_MODE_LTE_B33=69,
   PHONE_MODE_LTE_B34=70,
   PHONE_MODE_LTE_B35=71,
   PHONE_MODE_LTE_B36=72,
   PHONE_MODE_LTE_B37=73,
   PHONE_MODE_LTE_B39=74,

   PHONE_MODE_WCDMA_BC19=75,     //!<' WCDMA BC19 (subset of 800MHz) Band XIX
   PHONE_MODE_LTE_B41=76,
   PHONE_MODE_LTE_B42=77,

   /*TDSCDMA reserves 90 - 99*/
   PHONE_MODE_TDSCDMA_B34=90,
   PHONE_MODE_TDSCDMA_B39=91,
   PHONE_MODE_TDSCDMA_B40=92,

   /*
   QMSL Developers: please modify:
   - QLib.h  ->  QLIB_FTM_SET_MODE()
   - Diag_FTM.cpp  ->  Diag_FTM::FTM_SET_MODE()
   - QLIBFTMPhone.cpp when this list is changed.
   */
   PHONE_MODE_MAX=255				//!<' Maximum possible mode ID

} FTM_RF_Mode_Enum;

/**
  @brief Enumeration of Calibration API V2 (80-V2376-1) FTM Sub-command ID's
*/
typedef enum
{
   _FTM_CDMA_CAL_V2_COMMIT_NV			= 0,			//!<'Commit RF calibration to NV
   _FTM_CDMA_CAL_V2_CAL_DVGA			= 1,			//!<'Calibrate DVGA
   _FTM_CDMA_CAL_V2_CAL_LNA			= 2,			//!<'Calibrate LNA
   _FTM_CDMA_CAL_V2_CAL_IM2			= 3,			//!<'Calibrate IM2
   _FTM_CDMA_CAL_V2_INTELLICEIVER		= 4,				//!<'Calibrate Intelliceiver
   _FTM_CDMA_CAL_V2_INTELLICEIVER_RC_TUNE =5			//!<'Intelliceiver RC Tune calibration

}	FTM_CDMA_CAL_V2_Cmd_Id_Enum;


typedef enum
{
  RX_FUNCTION_GET_RSSI = 0,
  RX_FUNCTION_GET_DC_OFFSETS = 1,
  RX_FUNCTION_STOP_CONTINOUS_RX = 2,
  RX_FUNCTION_START_CONTINOUS_RX = 3,
  RX_FUNCTION_TURN_OFF_RECEIVER = 4
} GSM_RX_FUNCTION;

#define ENH_SWEEP_RESP_SIZE 11

#define TX_DAC_DEFAULT_SEGMENT_LENGTH_RF_CONFIG   60000
#define TX_DAC_DEFAULT_SEGMENT_LENGTH_RF_TUNE     6000
#define TX_DAC_DEFAULT_SEGMENT_LENGTH_WV_CONFIG   20000

typedef enum
{
  ENH_SWEEP_RX_ACTION_DVGA  = 0,
  ENH_SWEEP_RX_ACTION_LNA_0 = 0,
  ENH_SWEEP_RX_ACTION_LNA_1 = 1,
  ENH_SWEEP_RX_ACTION_LNA_2 = 2,
  ENH_SWEEP_RX_ACTION_LNA_3 = 4,
  ENH_SWEEP_RX_ACTION_LNA_4 = 8,
  ENH_SWEEP_RX_ACTION_LNA_5 = 16
} ENH_SWEEP_RX_ACTION;

/*!
 @brief Values to select PDM type for PDM SET 
 */
typedef enum {
  FTM_SET_PDM_TYPE_TX_AGC_ADJ = 2,
  FTM_SET_PDM_TYPE_TRK_LO_ADJ = 4,
} FTM_SET_PDM_TYPE;



// Note that this is product dependent thus might need to be changed in PC RFCA
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Available Power Modes for FTM_SET_RF_POWER_MODE 
*/
typedef enum
{

  CDMA_POWER_MODE_NORMAL,
  CDMA_POWER_MODE_LOW_LIN,
} FTM_SET_RF_POWER_MODES;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Available Power Modes for RX Sweep
*/
typedef enum
{

  SWEEP_RX_POWER_MODE_LOW,
  SWEEP_RX_POWER_MODE_HIGH,

} SWEEP_RX_POWER_MODES;

#define FTM_POWER_MODE_TO_SWEEP_POWER_MODE(pwr) ((CDMA_POWER_MODE_LOW_LIN==pwr)?SWEEP_RX_POWER_MODE_LOW:SWEEP_RX_POWER_MODE_HIGH)

#endif // _RFCA_DEFS_H_
