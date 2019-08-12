/*!
  @file
  rfca_handler.h
 
  @brief
  Functions to transform RFCA messages to FTM messages.

  Most of these functions work in pairs in a manner that there is a send
  and receive function even thoug only send functions are listed here.
  Send functions take care of queueing needed FTM messages to FTM sequencer.
  Once sequence executes and FTM response messages have been received the
  receive function gets called. Receive function populates needed data to HAL 
  response message, this data includes the overall HAL error and possible
  return data read from from.

  @todo Move helper functions to some other header.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_handler.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/16/14   aki     Removed AddSweepRadioSetup
01/06/14   aki     Added TDD tx align
12/09/13   aki     Removed CreateRxSweepConfig, added GetMasterDeviceMaskFromRxPath,
                   GetSlaveIdFromRxPath and AddSweepRxMeasure
11/25/13   aki     Changed V3_SWEEP_ENABLE_F3 from #define to uint16
11/25/13   aki     AddSweepRxConfig
11/19/13   aki     LTEFDD and TDS TX align, helpoer functions for main and div
11/14/13   aki     Updated version of antenna switching
11/14/13   aki     Added GetEfsSyncAfterTunerSelfCal
11/14/13   aki     Added persistent TX mode
09/13/13   aki     Added ant switch support
07/31/13   aki     Add IQ functions
07/30/13   aki     Added GetFtmModeIdV2
07/16/13   aki     Changed return type of rfca_common_msm_get_hw_timer_periodicty
                   to int32 added HalTechToFtmTech and AddSweepRadioSetup
06/18/13   aki     Added SetActiveRfDevice
06/18/13   aki     Added rfca_common_msm_get_hw_timer_periodicty
06/10/13   aki     Mainline RFCA
05/20/13   aki     Updated GSM sweep code
05/02/13   aki     Added new WAIT_V2 functions
04/29/13   aki     New LTE functions
04/26/13   aki     Added GetGsmSweepConfig
02/08/13   aki     Removed unnecessary TX DBM configs
02/01/13   aki     Changed rfMode to band in AddSweepConfig
01/30/13   aki     Removed RX ALIGN timing config
01/16/13   aki     Added functions for V3 configuration
12/27/12   aki     Added pre-dist and DA timing config
10/31/12   aki     Added functions and data types for timing config handling
10/03/12   aki     Added pre-exec and teardown fields for timing struct
08/23/12   aki     Pack local struct
08/16/12   aki     Freq addjust, disable sweep F3
07/23/12   aki     More TDSCDMA functions
07/23/12   aki     Added V3_SWEEP_ENABLE_F3
07/13/12   aki     V3 helpers, default segment times, LTEFDD, removed gsm_tx_dac
05/25/12   aki     Added AddNoOpSegment and CreateRxSweepConfig functions
04/12/12   aki     Dual carrier, enh sweep support, LNA range
03/01/12   aki     Changed <> to "", HDET reading support
02/08/12   aki     Removed second comdef.h include
10/28/11   aki     Initial revision

==============================================================================*/



#ifndef _RFCA_TO_FTM_H_
#define _RFCA_TO_FTM_H_

#include "rfa_variation.h"
#include "comdef.h"

#include "rfca_defs.h"
#include "rfca_hal_msg_def.h"
#include "rfca_ftm_package_helper.h"
#include "ftm.h"

#ifdef __cplusplus
  extern "C" {
#endif

#ifdef _WIN32
  #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*----------------------------------------------------------------------------*/
/*! Flag to enable ENH sweep debug messages */
extern uint8 V3_SWEEP_ENABLE_F3;

/*----------------------------------------------------------------------------*/
extern uint32 iqSampleOffset;
extern uint32 iqSampleSize;

typedef PACK(struct) {
  signed char I;
  signed char Q;
} EightBitIqSample;

typedef PACK(struct) {
  signed short I;
  signed short Q;
} SixteenBitIqSample;


/*----------------------------------------------------------------------------*/
typedef struct{
  bool needToSwitch;
  uint8 newPosition;
} AsdivSwitchSetting;

/*----------------------------------------------------------------------------*/
uint8 GetNewAsdivSwitchingState();

/*----------------------------------------------------------------------------*/
void SetAsdivSwitchingState(bool switchingIsNeeded, uint8 newPosition);

/*----------------------------------------------------------------------------*/
bool GetEfsSyncAfterTunerSelfCal();

/*----------------------------------------------------------------------------*/
bool GetRxTxInteleavingFlag();

/*----------------------------------------------------------------------------*/
void SetRxTxInteleavingFlag(bool newState);

/*----------------------------------------------------------------------------*/
bool GetPersistentTxModeFlag();

/*----------------------------------------------------------------------------*/
void SetPersistentTxModeFlag(bool newState);

/*----------------------------------------------------------------------------*/
/*!
  @brief 
    Getter for HAL request id of HAL_RF_STOP command.

*/
uint16 GetStopRequestHalId();

/*----------------------------------------------------------------------------*/
/*!
 @brief Get the second channel to be used for dual carrier
  
 @return The channel numebr
*/
uint16 GetDualCarrierSecondChannel();

/*----------------------------------------------------------------------------*/
/*!
 @brief Set state if config file has been loaded already

 @details By default file is read once but RFIC_WRITE(HAL_RF_RELOAD_CONFIG_FILE)
          can be used to reload it.
 
 @param newState   true or false
 
*/
void SetConfigHasBeenLoaded(bool newState);

/*----------------------------------------------------------------------------*/
/*!
 @brief Get state if config file has been loaded already

 @details By default file is read once but RFIC_WRITE(HAL_RF_RELOAD_CONFIG_FILE)
          can be used to reload it.
  
 @return true = files has been loaded already, false = config not been loaded
*/
uint16 GetConfigHasBeenLoaded();

/*----------------------------------------------------------------------------*/
/*!
 @brief Get the active RF device

 @details Use RFIC_WRITE(HAL_RF_ACTIVE_DEVICE) to set the active device
  
 @return The active RF device
*/
RF_DEVICE_ID GetActiveRfDevice();

/*----------------------------------------------------------------------------*/
/*!
 @brief Set the active RF device

 @details Use RFIC_WRITE(HAL_RF_ACTIVE_DEVICE) to set the active device
 
 @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL

*/
uint16 SetActiveRfDevice(uint16 rfDevice);

/*----------------------------------------------------------------------------*/
/*!
 @brief Get the TX FLAG for sweep

 @details Use RFIC_WRITE(HAL_RF_ACTIVE_DEVICE) to set the active device.
   
  @return 1 for first RF IC and 4 for second RF IC, 0 if current device selection is invalid
*/
byte GetTxFlag();

/*----------------------------------------------------------------------------*/
/*!
 @brief Get the Master device mask

 @details Use RFIC_WRITE(HAL_RF_ACTIVE_DEVICE) to set the active device.

 @note Right now calls GetTXFlag() function as the values seem to go hand in hand
   
 @return see @GetTXFlag
*/
byte GetMasterDeviceMask();

/*----------------------------------------------------------------------------*/
/*!
  @brief Get the Master device mask from given RX path enum
 
  @param [in] rxPath One from HAL_RF_SIGNAL_PATH
 
  @return Master device mask or 0 if invalid rxPath is given
*/
byte GetMasterDeviceMaskFromRxPath(uint16 rxPath);

/*----------------------------------------------------------------------------*/
/*!
  @brief Get the Slave ID for sweep from given RX path enum

  @param [in] rxPath One from HAL_RF_SIGNAL_PATH
 
  @return The slave id based on rx path or 0xFFFFFFFF if invalid rxPath is given
*/
uint32 GetSlaveIdFromRxPath(uint16 rxPath);

/*----------------------------------------------------------------------------*/
/*!
 @brief Get the Slave ID for sweep

 @details Use RFIC_WRITE(HAL_RF_ACTIVE_DEVICE) to set the active device
  
 @return The slave id based on active device
*/
dword GetSlaveId(void);

/*----------------------------------------------------------------------------*/
/*!
 @brief Get the current power mode for CDMA RX

 @details Use RFIC_WRITE(HAL_RF_POWER_MODE) to set the mode.
          Valid values are 0 = low power and 1 = high power
  
 @return The active RF device
*/
uint16 GetPowerMode();


/*----------------------------------------------------------------------------*/
/*!
 @brief Convert HAL technology enum to FTM technology enum

 @return One of ftm_rf_technology_type
*/
ftm_rf_technology_type HalTechToFtmTech(HAL_RF_PROTOCOL_SYSTEM halTech);

/*----------------------------------------------------------------------------*/
/*!
 @brief Returns RFM device from RX signal path

 @param [in] signalPath Hal message
 
 @return One of rfm_device_enum_type, RFM_INVALID_DEVICE on error
 
*/
rfm_device_enum_type GetDeviceIdFromSignalPath(uint16 signalPath);

/*----------------------------------------------------------------------------*/
/*!
 @brief Returns RFM device from currently active RF device
 
 @note This is used for TX functions which do not have Rf Path as argument

 @return One of rfm_device_enum_type, RFM_INVALID_DEVICE on error

*/
rfm_device_enum_type GetDeviceFromActiveRfDevice();

/*----------------------------------------------------------------------------*/
/*!
 @brief Returns main path enum based on selected device
 
 @note This is used for TX functions which do not have Rf Path as argument

 @return One of main path items of HAL_RF_SIGNAL_PATH

*/
uint16 GetMainPathFromActiveRfDevice();

/*----------------------------------------------------------------------------*/
/*!
 @brief Returns diversity path enum based on selected device
 
 @note This is used for TX functions which do not have Rf Path as argument

 @return One of diversity path items of HAL_RF_SIGNAL_PATH

*/
uint16 GetDivPathFromActiveRfDevice();

/*----------------------------------------------------------------------------*/
/*!
 @brief Returns if secondary chain path is selected, use this with FTM SET
        SECONDARY CHAIN command

 @param [in] signalPath Hal message

*/
uint16 GetSecChainEnabledFromSignalPath(uint16 signalPath);

/*----------------------------------------------------------------------------*/
/*!
 @brief Set flag to indicate that APT sweep items have been queued. 

 @param [in] newState   true or false

*/
void SetAtpSweepConfigured(bool newState);

/*----------------------------------------------------------------------------*/
/*!
 @brief Query if ATP sweep has been configured.

 @return true or false

*/
bool GetAtpSweepConfigured();

/*----------------------------------------------------------------------------*/
/*!
  @brief 
    Setter to indicate new messages have been sent to FTM sequencer
 
*/
void NewMessagesBeenSentToFtmSequencer(bool value = true);

/*----------------------------------------------------------------------------*/
/*!
 @brief Initializes RFCA and FTM sequencer

 @details FTM messages to be sent:
            - DIAG_MODE_CONTROL_FTM

 @note This function must be called before any other RFCA function

 @param [in] data Hal message

*/
void RFCA_init_send(HAL_RF_INIT_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Executes FTM sequencer and terminates RFCA and resets state cache.

 @details FTM messages to be sent:
            - FTM_SET_TX_OFF

 @note Once this function has been called RFCA_init_send must be
       called before calling any other RFCA function.

 @param [in] data Hal message

*/
void RFCA_stop_send(HAL_RF_STOP_REQ *data);


/*----------------------------------------------------------------------------*/
/*!
 @brief Callback function for FTM seqeuncer completion.

 @details This is called by @ref RfcaRespDispatcher when FTM sequence has been completed.

*/
void RFCA_ftm_sequencer_completed(void);

/*----------------------------------------------------------------------------*/
/*!
 @brief Set up for TX sweep

 @details Performs TX sweep over given PA ranges and PDM values.

          FTM messages to be sent if needed:
            - FTM_SET_MODE
            - FTM_SET_CHAN
            - FTM_SET_TX_ON
            - FTM_CDMA_CW_WAVEFORM
            - FTM_SET_PA_RANGE
            - FTM_SET_PDM 
            - FTM_SEQ_ADD_WAIT

            To be able to set PA range and PDM caller must provide an array containing PA range, 
            PDM type and PDM value for each PDM step. Array of HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA 
            structures is stored into @ref HAL_RF_WCDMA_TX_DAC_REQ::tx_control_data. Number of array 
            items must match HAL_RF_WCDMA_TX_DAC_REQ::number_of_steps. It is acceptable to include
            multiple PA ranges into the array when calling this function.


  @param [in] data Hal message, see HAL_RF_WCDMA_TX_DAC_REQ

  @ignored HAL_RF_WCDMA_TX_DAC_REQ::spreading_factor is not used
  @ignored HAL_RF_WCDMA_TX_DAC_REQ::tx_data_size is not used in code

*/
void RFCA_wcdma_tx_dac_send(HAL_RF_WCDMA_TX_DAC_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Reports DVGA or LNA offset for WCDMA Rx calibration

 @details FTM messages to be sent:
            - FTM_SET_MODE
            - FTM_SET_CHAN
            - FTM_SET_SECONDARY_CHAIN
            - FTM_GET_DVGA_OFFSET
            - FTM_GET_LNA_OFFSET
            - FTM_SEQ_ADD_WAIT


 @param [in] data Hal message

*/
void RFCA_wcdma_rx_align_send(HAL_RF_WCDMA_RX_ALIGN_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_wcdma_rx_wait_send(HAL_RF_WCDMA_RX_WAIT_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above or fall below specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_wcdma_rx_wait_v2_send(HAL_RF_WCDMA_RX_WAIT_V2_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Gets RSSI as dBm

 
 @param [in] data Hal message

 */
void RFCA_wcdma_rx_rssi_send(HAL_RF_WCDMA_RX_RSSI_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Capture RX IQ

 @param [in] data Hal message

 */
void RFCA_wcdma_rx_iq_send(HAL_RF_WCDMA_RX_IQ_REQ* data);

/*----------------------------------------------------------------------------*/
/*!
  @brief Performs GSM Rx Gain Cal

  @details FTM messages to be sent:
            - FTM_SET_MODE
            - FTM_SET_SET_TX_OFF
            - FTM_SET_RX_BURST
            - FTM_SET_CHAN
            - FTM_RX_GAIN_RANGE_CAL
            - FTM_SEQ_ADD_WAIT

  @param [in] data Hal message
 
  @ignored SIGNAL_PATH is currently ignored!
  @ignored expected_level
  @todo duration is in uS right now.
 
*/
void RFCA_gsm_rx_align_send(HAL_RF_GSM_RX_ALIGN_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief RFCA to FTM translator

 @details 
          FTM messages to be sent:
            - FTM_SET_MODE
            - FTM_SET_CHAN
            - ...


 @param [in] data Hal message

 @todo SIGNAL_PATH is currently ignored!
 @todo Number of gain ranges are hard coded to 4 at the moment.
 @todo How do we define gain ranges 4 or 5, through API or hard code
*/
void RFCA_gsm_rx_align_old_send(HAL_RF_GSM_RX_ALIGN_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Refresh calibration values from NV

 @details Synchronously sends FTM_LOAD_RF_NV message to mobile to reload 
          calibration values from NV to memory. If TX is on it will be turned off. 

          FTM messages to be sent:
          - FTM_SET_TX_OFF
          - FTM_LOAD_RF_NV

 @param [in] data Hal message

*/
void RFCA_calibration_values_load_send(HAL_RF_CALIBRATION_VALUES_LOAD_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Stores calibration values in RAM to NV

 @note This function does not do anything right now and might be removed in future.

 @param [in] data Hal message

*/
void RFCA_calibration_values_store_send(HAL_RF_CALIBRATION_VALUES_STORE_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Reads various data from UUT

 @param [in] data Hal message

 @note HAL_RF_RFIC_READ_REQ.data_length is ignored

*/
void RFCA_rfic_read_send(HAL_RF_RFIC_READ_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Writes various data from UUT

 @param [in] data Hal message

*/
void RFCA_rfic_write_send(HAL_RF_RFIC_WRITE_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Informs UUT to use given data 

 @param [in] data Hal message

*/
void RFCA_inform_send(HAL_RF_INFORM_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Set up for TX sweep

 @details Performs TX sweep over given PA ranges and PDM values.

          FTM messages to be sent if needed:
            - FTM_SET_MODE
            - FTM_SET_CHAN
            - FTM_SET_TX_ON
            - FTM_CDMA_CW_WAVEFORM
            - FTM_SET_PA_RANGE
            - FTM_SET_PDM 
            - FTM_SEQ_ADD_WAIT

            To be able to set PA range and PDM caller must provide an array containing PA range, 
            PDM type and PDM value for each PDM step. Array of HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA 
            structures is stored into @ref HAL_RF_WCDMA_TX_DAC_REQ::tx_control_data. Number of array 
            items must match HAL_RF_WCDMA_TX_DAC_REQ::number_of_steps. It is acceptable to include
            multiple PA ranges into the array when calling this function.

  @code
            typedef struct {
              uint16 paRange;
              uint16 pdmType;
              uint16 pdmValue;
            } HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA;
  @endcode

  @param [in] data Hal message

*/
void RFCA_cdma2k_tx_dac_send(HAL_RF_CDMA2K_TX_DAC_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Set TX power as dBm units

 @param [in] data Hal message

 */
void RFCA_cdma2k_tx_dbm_send(HAL_RF_CDMA2K_TX_DBM_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Reports DVGA or LNA offset for CDMA Rx calibration

 @details FTM messages to be sent:
            - FTM_SET_MODE
            - FTM_SET_CHAN
            - FTM_SET_SECONDARY_CHAIN
            - FTM_GET_DVGA_OFFSET
            - FTM_GET_LNA_OFFSET
            - FTM_SEQ_ADD_WAIT


 @param [in] data Hal message

*/
void RFCA_cdma2k_rx_align_send(HAL_RF_CDMA2K_RX_ALIGN_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_cdma2k_rx_wait_send(HAL_RF_CDMA2K_RX_WAIT_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above or fall below specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_cdma2k_rx_wait_v2_send(HAL_RF_CDMA2K_RX_WAIT_V2_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Gets RX IQ

 
 @param [in] data Hal message

 */
void RFCA_cdma2k_rx_iq_send(HAL_RF_CDMA2K_RX_IQ_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Gets RSSI as dBm

 
 @param [in] data Hal message

 */
void RFCA_cdma2k_rx_rssi_send(HAL_RF_CDMA2K_RX_RSSI_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Performs one GSM TX align

 @details Function performs one of the GSM TX calibrations: Driver Amp (DA), 
          Predistortion or KV.

 @param [in] data Hal message

*/
void RFCA_wcdma_tx_align_send(HAL_RF_WCDMA_TX_ALIGN_REQ *data); 

/*----------------------------------------------------------------------------*/
/*!
 @brief Set TX power as dBm units

 @param [in] data Hal message

 */
void RFCA_wcdma_tx_dbm_send(HAL_RF_WCDMA_TX_DBM_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Set power level

 @param [in] data Hal message

*/
void RFCA_gsm_tx_pcl_send(HAL_RF_GSM_TX_PCL_REQ *data);


/*----------------------------------------------------------------------------*/
/*!
 @brief 

 @param [in] data Hal message

*/
void RFCA_gsm_tx_align_send(HAL_RF_GSM_TX_ALIGN_REQ *data);


/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_gsm_rx_wait_send(HAL_RF_GSM_RX_WAIT_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above or fall below specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_gsm_rx_wait_v2_send(HAL_RF_GSM_RX_WAIT_V2_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Gets IQ

 @param [in] data Hal message

*/
void RFCA_gsm_rx_iq_send(HAL_RF_GSM_RX_IQ_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Gets RSSI as dBm

 @param [in] data Hal message

*/
void RFCA_gsm_rx_rssi_send(HAL_RF_GSM_RX_RSSI_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Misc TX messages

 @details Perform TX operation which does not fall under any other HAL API function.
          HAL_RF_ARB_TX_REQ::config_pattern defines operation to be executed, for valid 
          options see HAL_RF_TX_ARB_CONFIG_PATTERN

          - GSM_DRIVER_AMP_CAL
              - Performing GSM Driver Ampm (DA) calibration HAL_RF_ARB_TX_REQ::data_array must contain an
                array of GSM_DRIVER_AMP_CAL_DATA items. The number of itmes in array must match
                the HAL_RF_ARB_TX_REQ::number_of_steps. See FTM_GSM_TX_GAIN_SWEEP for details specification.

          - GSM_PREDISTRORTION_CAL
              - When executing Predistotion clibration HAL_RF_ARB_TX_REQ::data_array must contain single
                GSM_PREDISTORTION_CAL_DATA item. See FTM_TX_CFG2_AMAM_SWEEP_V2 for details.



 @param [in] data Hal message
 
 @todo CL93-V5370-1 is not up to date for FTM_TX_CFG2_AMAM_SWEEP_V2 as it is missing waveform argument which QMLS does include.
 @todo Implement HAL_RF_TX_ARB_CONFIG_PATTERN::GSM_ENVELOPE_DC_CAL


*/
void RFCA_arb_tx_send(HAL_RF_ARB_TX_REQ *data);

  /*----------------------------------------------------------------------------*/
/*!
 @brief RX AGC for LTE Calibration

 @details Reads LNA offsets from UUT. 

 @param [in] data Hal message

*/
void RFCA_ltefdd_rx_align_send(HAL_RF_LTEFDD_RX_ALIGN_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Gets RSSI as dBm

 
 @param [in] data Hal message

 */
void RFCA_ltefdd_rx_rssi_send(HAL_RF_LTEFDD_RX_RSSI_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_ltefdd_rx_wait_send(HAL_RF_LTEFDD_RX_WAIT_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above or fall below specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_ltefdd_rx_wait_v2_send(HAL_RF_LTEFDD_RX_WAIT_V2_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief LTE TX Sweep

 @param [in] data Hal message

*/
void RFCA_ltefdd_tx_dac_send(HAL_RF_LTEFDD_TX_DAC_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Set TX power as dBm units

 @param [in] data Hal message

 */
void RFCA_ltefdd_tx_dbm_send(HAL_RF_LTEFDD_TX_DBM_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Perform TX align

 @param [in] data Hal message

 */
void RFCA_ltefdd_tx_align_send(HAL_RF_LTEFDD_TX_ALIGN_REQ* data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Capture RX IQ

 @param [in] data Hal message

 */
void RFCA_ltefdd_rx_iq_send(HAL_RF_LTEFDD_RX_IQ_REQ* data);

/*----------------------------------------------------------------------------*/
/*!
 @brief RX AGC for LTE Calibration

 @details Reads LNA offsets from UUT. 

 @param [in] data Hal message

*/
void RFCA_ltetdd_rx_align_send(HAL_RF_LTETDD_RX_ALIGN_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Gets RSSI as dBm

 
 @param [in] data Hal message

 */
void RFCA_ltetdd_rx_rssi_send(HAL_RF_LTETDD_RX_RSSI_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_ltetdd_rx_wait_send(HAL_RF_LTETDD_RX_WAIT_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief LTE TX Sweep

 @param [in] data Hal message

*/
void RFCA_ltetdd_tx_dac_send(HAL_RF_LTETDD_TX_DAC_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Set TX power as dBm units

 @param [in] data Hal message

 */
void RFCA_ltetdd_tx_dbm_send(HAL_RF_LTETDD_TX_DBM_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Capture RX IQ

 @param [in] data Hal message

 */
void RFCA_ltetdd_rx_iq_send(HAL_RF_LTETDD_RX_IQ_REQ* data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Perform TX align

 @param [in] data Hal message

 */
void RFCA_ltetdd_tx_align_send(HAL_RF_LTETDD_TX_ALIGN_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief RX AGC for TDSCDMA Calibration

 @details Reads LNA offsets from UUT. 

 @param [in] data Hal message

*/
void RFCA_tdscdma_rx_align_send(HAL_RF_TDSCDMA_RX_ALIGN_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief TDSCDMA TX Align

 @param [in] data Hal message

*/
void RFCA_tdscdma_tx_align_send(HAL_RF_TDSCDMA_TX_ALIGN_REQ* data);

/*----------------------------------------------------------------------------*/
/*!
 @brief TDSCDMA TX Sweep

 @param [in] data Hal message

*/
void RFCA_tdscdma_tx_dac_send(HAL_RF_TDSCDMA_TX_DAC_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief TDSCDMA dBm TX Sweep

 @param [in] data Hal message

*/
void RFCA_tdscdma_tx_dbm_send(HAL_RF_TDSCDMA_TX_DBM_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_tdscdma_rx_wait_send(HAL_RF_TDSCDMA_RX_WAIT_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Waits RSSI to rise above or fall below specified treshold or timeout to happen

 @param [in] data Hal message

*/
void RFCA_tdscdma_rx_wait_v2_send(HAL_RF_TDSCDMA_RX_WAIT_V2_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Reads RX IQ

 @param [in] data Hal message

*/
void RFCA_tdscdma_rx_iq_send(HAL_RF_TDSCDMA_RX_IQ_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Reads RSSI

 @param [in] data Hal message

*/
void RFCA_tdscdma_rx_rssi_send(HAL_RF_TDSCDMA_RX_RSSI_REQ *data);

/*----------------------------------------------------------------------------*/
/*!
 @brief Cleans up QMLS in PC RFCA, not needed in Embedded RFCA 

 @param [in] data Hal message

*/
void RFCA_finish_send(HAL_RF_FINISH_REQ *data); 

/*----------------------------------------------------------------------------*/
// Timings for Enhanced Sweep
/*----------------------------------------------------------------------------*/

extern HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG enhSweepSegmentLength;

extern HAL_RF_FREQ_ADJUST_DATA freqAdjustData;

extern uint16 atpTuningSegmentLength_Tx_Lte;
extern uint16 atpTuningSegmentLength_Tx_TdsCdma;

const double wcdmaTimeToUs = 3.84; // 3.84 MHz
const double cdma2kTimeToUs = 1.2288; // 1.2288 MHz
const double lteTimeToUs = 0.001; 
const double tdscdmaTimeToUs = 1.28; // 1.28 MHz

extern byte numberOfHdetAvgerages_wcdma;
extern byte numberOfHdetAvgerages_cdma;
extern byte numberOfHdetAvgerages_tdscdma;
extern byte numberOfHdetAvgerages_lte;

extern const uint16 defaultLnaRangeForXo;

// TODO Refactor
typedef PACK(struct) {
  uint16 segmentNumber;
  byte   resultSize;
  uint16 resultData[];
} HdetReading;

#define MAX_NUMBER_OF_LNA_RANGE_OFFSETS 5
/*----------------------------------------------------------------------------*/
/*!
 @brief DBm value in Q8 format versus Lna Range.

 @details Values in the array must be in increasing order.
  
*/
extern int16_q8 lnaRangeForRxLevel[MAX_NUMBER_OF_LNA_RANGE_OFFSETS]; 
/*----------------------------------------------------------------------------*/
/*!
 @brief Get lna range for given dBm value

 @details Lna ranges are stored from highest to lowes and lnaRangeForRxLevel
          is scanned in the same order. For example is lnaRangeForRxLevel has
          dBm values -60, -50, -40, coded as Q8, then all rx_level values less 
          or equal to -60 dBm return 0 as Lna range, all rx_levels between -60
          and -50 return Lna range 1 and so on.

 @param [in] system   Protocol system for getting LNA range
 @param [in] rx_level Rx level as dBm in Q8 format

 @return Lna range to set, or 0 if invalid system is used.
*/
uint16 GetLnaRangeForRxLevel( HAL_RF_PROTOCOL_SYSTEM system, int16_q8 rx_level);

/*----------------------------------------------------------------------------*/
/*!
 @brief Set LNA range if needed based on rx level

 @param [in] system      LNA range threshold table to use
 @param [in] opId        Hal operation id
 @param [in] ftmModeId   Ftm mode id
 @param [in] rxThreshold RX treshold in dBm to select Lna range

*/
void SetLnaRxRangeForRxLevel(HAL_RF_PROTOCOL_SYSTEM system, uint16 opId, uint16 ftmModeId, uint16 rxThreshold);

/*----------------------------------------------------------------------------*/
/*!
  @brief Add sweep list to FTM command queue
 
  @param [in]  halOpId        Hal operation id
  @param [in]  head           Current head of sweep list
  @param [in]  tail           Current tail of sweep list
  @param [in]  cleanList      Flag to free list items or not

  @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL
*/
int AddSweepToFtmCmdQueue(uint16 halOpId, FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, bool cleanList);

/*----------------------------------------------------------------------------*/
/*!
  @brief Add No-Op sweep item

  @deftails Adds 0 or more no-op sweep items until the totalTimeToDoNothing has been 
            consumed. 
  
  @param [in]  FTM_Cal_Sweep_Opcode_Req_List_Item  pointer pointer to the last item in the list
  @param [in]  totalTimeToDoNothing                Time to spend

  @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL 
*/
int AddNoOpSegment(FTM_Cal_Sweep_Opcode_Req_List_Item **tailOfList, int32 totalTimeToDoNothing);

/*----------------------------------------------------------------------------*/
/*!
  @brief Add Rf Config segment
  
  @param [in]  head             Current head of sweep list
  @param [in]  tail             Current tail of sweep list
  @param [in]  timeout          RX wait timeout in us
  @param [in]  pollingInterval  RX level polling interval in us
  @param [in]  threshold        RX level threshold in dBm10
 
  @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL 
*/
uint16 AddSweepRxWait(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint32 timeout, uint16 pollingInterval, int16 threshold);
 
/*----------------------------------------------------------------------------*/
/*!
  @brief Add Rx Config segment
  
  @param [in]  head             Current head of sweep list
  @param [in]  tail             Current tail of sweep list
  @param [in]  duration         Step duration
  @param [in]  rxCtrlData       HAL_RF_RX_SWEEP_STEP_CONFIG
 
  @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL 
*/
uint16 AddSweepRxConfig(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint16 duration, HAL_RF_RX_SWEEP_STEP_CONFIG rxCtrlData);

/*----------------------------------------------------------------------------*/
/*!
  @brief Add Rx Config and Measure segments
  
  @param [in]  head             Current head of sweep list
  @param [in]  tail             Current tail of sweep list
  @param [in]  duration         Step duration
  @param [in]  rxCtrlData       HAL_RF_RX_SWEEP_STEP_CONFIG
 
  @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL 
*/
uint16 AddSweepRxMeasure(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint16 duration, HAL_RF_RX_SWEEP_STEP_CONFIG rxCtrlData);

/*----------------------------------------------------------------------------*/
/*!
  @brief Add Rf Config segment
  
  @param [in]  head           Current head of sweep list
  @param [in]  tail           Current tail of sweep list
  @param [in]  band           Band
  @param [in]  channel        Channel
  @param [in]  channelOffset  Channel off-set for dual carrier, set 0 when not used
  @param [in]  txBW           TX bandwidth
  @param [in]  txBW           TX bandwidth
  @param [in]  rxPath         RX Path, main, div, 2nd main, 2nd div
  @param [in]  duration       Duration of the segment in system specific ticks

  @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL 
*/
uint16 AddSweepConfig(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint16 band, uint16 channel, uint16 channelOffset, uint16 txBW, uint16 rxBW, uint16 rxPath, uint16 duration);

/*----------------------------------------------------------------------------*/
/*!
  @brief Add Rf Tune segment
  
  @param [in]  head           Current head of sweep list
  @param [in]  tail           Current tail of sweep list
  @param [in]  channel        Channel
  @param [in]  channelOffset  Channel off-set for dual carrier, set 0 when not used
  @param [in]  duration       Duration of the segment in system specific ticks

  @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL 
*/
uint16 AddSweepTune(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint16 channel, uint16 channelOffset, uint16 duration);

/*----------------------------------------------------------------------------*/
/*!
  @brief Get FTM Mode Id

  @details Gets FTM Mode Id based on system, Rx path and device selection
  
  @param [in]  rfDevice       Device to be used, from RF_DEVICE_ID
  @param [in]  technology     Technology from HAL_RF_PROTOCOL_SYSTEM
  @param [in]  rxPath         Signal path from, use RF_SIGNAL_PATH_MAIN if using this
                              function for TX purposes
  @param [out]  ftmModeId     Returned FTM mode id from @ref FTM_Mode_Id_Enum
   
  @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL 
*/
uint16 GetFtmModeId(uint16 rfDevice, uint16 technology, uint16 rxPath, uint16 *ftmModeId);

/*----------------------------------------------------------------------------*/
/*!
  @brief Get FTM Mode Id

  @details Gets FTM Mode Id based on system, Rx path and device selection
  
  @param [in]  technology     Technology from HAL_RF_PROTOCOL_SYSTEM
  @param [in]  rxPath         Signal path, use RF_SIGNAL_PATH_MAIN or RF_SIGNAL_PATH_MAIN_2ND_TRANSCVR
                              if using this function for TX purposes
  @param [out]  ftmModeId     Returned FTM mode id from @ref FTM_Mode_Id_Enum
   
  @return RFCA_FUNCTION_SUCCESS or RFCA_FUNCTION_FAIL 
*/
uint16 GetFtmModeIdV2(uint16 technology, uint16 rxPath, uint16 *ftmModeId);


/*----------------------------------------------------------------------------*/
/*!
  @brief Gets Entended GSM Mode Id

  @details Gets Entended GSM FTM Mode Id based on FTM Mode Id
    
  @param [in]  ftmModeId     GSM mode id FTM_GSM_C0_C, etc.
   
  @return FTM_GSM_EXTENDED_C0_C or FTM_GSM_EXTENDED_C2_C 
*/
word GetGsmExtendedModeIdFromModeId(word ftmModeId);

/*----------------------------------------------------------------------------*/
/*!
  @brief Get flag if old FTM commands are used for RX

  @return true or false
*/
bool GetUseOldFtmForRxFlag();


/*----------------------------------------------------------------------------*/
/*!
  @brief Returns pointer to GSM sweep master configuration structure

  @return Pointer to master config structure
*/
ftm_gsm_frame_sweep_config_type* GetMasterGsmSweepConfig();

/*----------------------------------------------------------------------------*/
/*!
  @brief Frees memory allocated for the GSM sweep buffer in master config
*/
void ResetGsmSweepConfig();


/*----------------------------------------------------------------------------*/
/*!
  @brief Set the selection between old frame matrix or new sweep function for
         GSM TX PCL      
*/
void SetGsmPclUseSweep(bool newState);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get max periodicity of the hardware timer
 
  @details
  Return maximum periodicty duration in microseconds allowed for the timer.
 
  @return
  The maximum periodicity of O_STMR based timer in the units of microseconds.
  
*/
int32 rfca_common_msm_get_hw_timer_periodicty (void);

/*----------------------------------------------------------------------------*/
/*!
  @brief Data type to hold RFCA time configs

*/
typedef struct {
  uint32 rfConfig;      //<! Initial config time, SET_MODE, SET_TX_WAVEFORM, SET_SECONDARY_CHAIN, etc.
  uint32 rfTune;        //<! Rf tune aka channel change, FTM_SET_CHAN
  uint32 rfExec;        //<! Fixed exec time for functions which don't have duration argument
// Following are somewhat exceptions and only apply to some techs and functions
  uint32 rfPreExec;     //<! Time for after tune and before exec
  uint32 rfTeardown;    //<! Time for post actions
} RfcaTimeConfigData;

typedef struct {
  RfcaTimeConfigData RX_WAIT;
  RfcaTimeConfigData RX_RSSI;
  RfcaTimeConfigData RX_IQ;
} CdmaRfcaTimeConfigData;

typedef struct {
  RfcaTimeConfigData RX_WAIT;
  RfcaTimeConfigData RX_RSSI;
  RfcaTimeConfigData TX_DBM;
  RfcaTimeConfigData RX_IQ;
} LteRfcaTimeConfigData;

typedef struct {
  RfcaTimeConfigData RX_WAIT;
  RfcaTimeConfigData RX_RSSI;
  RfcaTimeConfigData TX_DBM;
  RfcaTimeConfigData RX_IQ;
} TdsCdmaRfcaTimeConfigData;

typedef struct {
  RfcaTimeConfigData RX_WAIT;
  RfcaTimeConfigData RX_RSSI;
  RfcaTimeConfigData RX_IQ;
} WcdmaRfcaTimeConfigData;

typedef struct {
  RfcaTimeConfigData RX_ALIGN;
  RfcaTimeConfigData RX_WAIT;
  RfcaTimeConfigData RX_RSSI;
  struct {
    RfcaTimeConfigData KVCO_CAL;
    //RfcaTimeConfigData SELF_CAL;
  } TX_ALIGN;
  RfcaTimeConfigData TX_PCL;
  struct
  {
    RfcaTimeConfigData DRIVER_AMP;
    RfcaTimeConfigData PRE_DIST;
  } ARB_TX;
  RfcaTimeConfigData RX_IQ;
} GsmRfcaTimeConfigData;

extern CdmaRfcaTimeConfigData CdmaRfcaTimeConfig;
extern LteRfcaTimeConfigData LteRfcaTimeConfig;
extern TdsCdmaRfcaTimeConfigData TdsCdmaRfcaTimeConfig;
extern WcdmaRfcaTimeConfigData WcdmaRfcaTimeConfig;
extern GsmRfcaTimeConfigData GsmRfcaTimeConfig;

#ifdef _WIN32
  #pragma pack(pop) // Restore
#endif

#ifdef __cplusplus
  }
#endif

#endif //_RFCA_TO_FTM_H_

