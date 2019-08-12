/*!
  @file
  tfw_sw_intf_rx_tx.h

  @brief
  TD-SCDMA rx and tx FW-SW Interface Definitions 

*/

/*===========================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Confidential and Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_sw_intf_rx_tx.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
05/08/13   HK      Add data structure tfw_xpt_capture_cmd_t and tfw_xpt_tx_cmd_t
01/03/13   VK      Remove unused elements from APT config message structure
11/27/12   JP      Merge NikeL CL 2922049 Added sw/fw msg to enable/disable adc clock gating
09/14/12   IS      Re-enable IQ_CAPTURE CMD for Dime. CMD now expects a USTMR
12/03/12   BL      Modify iq_capture_cfg cmd/rsp to support RSB cal
09/14/12   IS      Re-enable IQ_CAPTURE CMD for Dime. CMD now expects a USTMR
                   delta value instead of a full WALLTIME value as a parameter.

===========================================================================*/

#ifndef TFW_SW_INTF_RX_TX_H
#define TFW_SW_INTF_RX_TX_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tfw_sw_intf_common.h"
#include "tfw_sw_intf_cell.h"
#include "tfw_rf_sm_intf.h"
#include "fw_rf_common_intf.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

  /*-----------------------------------------------------------------------*/
  /*     CONSTANTS                                                         */
  /*-----------------------------------------------------------------------*/

/*! \brief Activation of Rx chain */ 
#define  TFW_RX_CONFIG_ACTIVATE     1

/*! \brief Deactivation of Rx chain */ 
#define  TFW_RX_CONFIG_DEACTIVATE   0

/*! \brief Number of RF devices per band */
#define TFW_RFC_MAX_DEVICE_INSTANCES_PER_BAND 5
/*! \brief Number of writes per device */
#define TFW_RX_TX_MAX_NUM_WRITES_UPDATE 15

/**
    \defgroup cmdRxTx RF RX & TX Configuration */
/*\{*/

/*\}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR RX_CONFIG COMMAND                             */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdRxConfig RX_CONFIGURATION Command
   \ingroup cmdRxTx 
  @verbatim
  This command directs firmware to configure RXFE and points FW to the
  RF WTR script for "home" frequency operation.
 
  SW Usage:
    1) This command can only be sent to FW after FW is in ACQUISITION,
       TRACKING, or FTM state
    2) This command must be sent BEFORE any channel configuration command
       to ensure that the RF & RXFE is properly configured
    3) Upon receipt of this command, RXLM static and frequency dependent
       settings are applied immediately.
    4) NOTE: If another command is received while FW is active (eg in the
       middle of acquisition or channels configured), RXFE settings will
       be reapplied immediately and the index to the RF buffer updated
  @endverbatim
*/
           
 
/*! @brief RX_CONFIG command 
 *
 * This is a message definition for RX_CONFIG command
 *
 */

typedef struct
{
  /*! \brief RF Carrier for Primary frequency in Hz 
  */
  uint32 rfCarrierPrimaryHz;
  /*! \brief RF Carrier for Working frequency in Hz 
  */
  uint32 rfCarrierWorkingHz;
  /*! \brief Total frequency error in Hz (S32Q8)
  */
  int32  totalFreqErrorHz;
} tfw_rx_freq_dependent_settings_t;

typedef struct {
  /*! \brief RXFE frequency dependent settings.
      @verbatim
      These settings are applied in addition to the RXLM static settings
      @endverbatim
      */
  tfw_rx_freq_dependent_settings_t rxFreqSettings;
  /*! \brief Index to RXLM static settings
      @verbatim
      This index points to a buffer for the non-Interfreq mode.

      NOTE: Upon receipt of command, buffer is applied to the RXFE
      immediately.
      @endverbatim
      **/
  uint8 rxlmStaticBufIndex;
  /*! \brief Index to RXLM static settings for the diversity antenna */
  uint8 rxlmStaticBufIndexDiv;
  /*! \brief Antenna selection. Note that Ant_1 is for TESTING only  */
  tfw_ant_sel_e antSel;
  /*! \brief Index to RFSW-specified SSBI/RXAGC/GRFC buffer
      @verbatim
      This buffer - in rfRxFreqBuf - pointed to by this index is
      also known as "home" or "OFF-to-F1" tuning script when this
      structure is part of tfw_rx_config_cmd_t. The buffer pointed
      to by this index in IRAT step0, IRAT step1/step2 and RSCP
      commands will be tuning script for F2.

      NOTE: The script is NOT applied immediately upon receipt of this
      command; instead, they applied prior to active slots.
      @endverbatim
      */
  uint8 rfRxFreqBufIndex;
  tfw_rx_lna_t lnaInfo;  
    /*! \brief SW indication that FW is in LTA gap
  @verbatim 
  SW will set the LTA indication flag to true when state transition is requested due to LTA. 
  In all other Cases, SW should send the flag to false;
  @endverbatim 
  */
  boolean     ltaInd;
} tfw_rx_config_t;

/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief Body of the message */
  tfw_rx_config_t          cfg;
  } tfw_rx_config_cmd_t;
/*@}*/


/*! @brief rx chain release command 
 *
 * This is a message definition for rx chain release command
 *
 */

typedef struct{
  /*! \brief chain to release:
  TFW_ANT_0: to release chain0 only;
  TFW_ANT_1: to release chain1 only;
  TFW_ANT_BOTH: to release both chains
  */
  tfw_ant_sel_e antSel;
} tfw_rx_chain_rel_t;


/**
   \defgroup cmdRxChainImmRel RX_CHAIN_IMM_REL Command
   \ingroup cmdRxTx 
  @verbatim
  SW uses this cmd to ask FW to release one or both chains 
  immediately 
 
  SW Usage:
    1) This command can only be sent to FW when FW is in idle
    (no active chains)
    2) The command is applied immediately upon receipt
    3) RFSW uses this command in FTM mode
    4) L1SW isn't plan to use this command currently; please
    discuss with FW if it is going to be used
 
  FW Behavior:
    1) FW should ignore this cmd if the chains aren't enabled
    2) FW should assert if the chains are active once receiving
    this commnad
  @endverbatim

*/
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief Body of the message */
  tfw_rx_chain_rel_t          chainInfo;
} tfw_rx_chain_imm_rel_cmd_t;
/*@}*/


/*! @brief ARD command 
 *
 * This is a message definition for ARD command
 *
 */

typedef struct{

  /*! \brief Imbalance threshold 16Q10 */
  uint16 thIb;
  /*! \brief Weak SNR threshold for rx0; 16Q10 */
  uint16 thRx0;
  /*! \brief Weak SNR threshold for rx1; 16Q10 */
  uint16 thRx1;
  /*! \brief SNR IIR filter coefficient. It is a function of ThN; 16Q15 */
	uint16 gamma;
  /*! \brief Number of required measurements (#subframes) before 
             making any ARD decision 
  */
	uint16 thN;
  /*! \brief Measurement expiration time in subframes */
	uint16 tExp;
  /*! \brief Flag to enable (1) / disable (0) ARD */
  boolean ardEnable;
  /*! \brief Flag to reset Ns; 1: reset, 0: dont reset */
  boolean nsResetEnable;
} tfw_ard_cfg_t;

/*@{*/ 
typedef struct
{
	/*! \brief Message header */
	msgr_hdr_struct_type	 hdr;
	/*! \brief Body of the message */
	tfw_ard_cfg_t cfg;
} tfw_ard_cmd_t;
/*@}*/

/*! @brief ARD_RD_OFF Indication
 *
 *   This is the message definition for ARD_RD_OFF Indication
 */

/*@{*/ 
typedef struct 
{
	/*! \brief Message header */
	msgr_hdr_struct_type  hdr;
	/*! \brief ARD indication about recommendation for ant sel */
	tfw_ant_sel_e         antSel;
} tfw_ard_ind_t;					  
/*\}*/



  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR RX_CONFIG_UPDATE COMMAND                      */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdRxConfigUpdate RX_CONFIG_UPDATE Command 
   \ingroup cmdRxTx 
  @verbatim
 
  @endverbatim
*/
                  

/*! @brief RX_CONFIG_UPDATE command 
 *
 * This is a message definition for RX_CONFIG_UPDATE command
 *
 */
typedef struct {
  /*! \brief Flag to indicate if tracking mode switch-points have changed (R4 to HSDPA change).
      @verbatim
      This change can be asynchronous to FW operation, i.e.,
      can take place in parallel with DPCH demod.
      @verbatim
  */
  boolean newHomeRxAgcBuffer;

  /*! \brief Flag to indicate if inter-freq mode switch-points have changed.
      @verbatim
      if true, L1 sets the lna state of each inter-frequency as:
      a.	Use the gain state from the same frequency RSCP response,
          if this frequency was part of the earlier neighbor list as well.
      b.	Use default gain state (G1) if it is a new frequency.
      @verbatim
  */
  boolean newInterFreqRxAgcBuffer;
  /*! \brief number of inter-frequencies
      @verbatim
      only valid if newInterFreqRxAgcBuffer is true
      @verbatim
  */
  uint8 numInterFreqs;
  /*! \brief LNA state info
      @verbatim
      only valid if newInterFreqRxAgcBuffer is true
      @verbatim
  */
  tfw_rx_lna_t lnaInfo[TFW_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];

  /*! \brief Flag to indicate if antenna selection mode has changed  */
  boolean newAntSel;
  /*! \brief Antenna selection. Note that Ant_1 is for TESTING only
      @verbatim
      only valid if newAntSel is true
      @verbatim
  */
  tfw_ant_sel_e antSel;
  /*! \brief lna state of diversity chain
      @verbatim
      only valid if newAntSel is true && antSel is ANT_1 or ANT_BOTH
      @endverbatim 
  */
  tfw_lna_state_e lnaStateDiv;
  /*! \brief DVGA compensation (G(-1)) status of diversity chain
      @verbatim
      only valid if newAntSel is true && antSel is ANT_1 or ANT_BOTH
      0: OFF
      1: ON
      @endverbatim 
  */
  boolean dvgaCompensationDiv;
  /*! \brief Index to RXLM static settings for the diversity antenna
      @verbatim
      only valid if newAntSel is true && antSel is ANT_0 or ANT_BOTH
      @verbatim
  */
  uint8 rxlmStaticBufIndexDiv;

  /*! \brief Index for primary frequency 
      @verbatim
      1) If primaryFreqIndex == -1, primary frequency is not in inter frequency neighbour list.
      2) If primaryFreqIndex > -1 and primaryFreqIndex < TFW_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS, FW will latch it as the index for priamry frequency
      @verbatim
  */
  int8 primaryFreqIndex;


} tfw_rx_config_update_t;

/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief Sub-frame number */
  tfw_rx_config_update_t   cfg;
  } tfw_rx_config_update_cmd_t;
/*@}*/ 

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR TX_CONFIG COMMAND                             */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdTxConfig TX_CONFIG Command 
   \ingroup cmdRxTx 
  @verbatim
  This command directs firmware to configure TXC/TXR and points FW to the
  RF TX WTR script for "home" frequency operation.
 
  SW Usage:
    1) This command can only be sent to FW after FW is in TRACKING or FTM
       state
    2) This command must be sent BEFORE any channel configuration command
       to ensure that the TXR/TXC & WTR is properly configured
    3) Upon receipt of this command, TXLM static and frequency dependent
       settings are applied immediately.
    4) NOTE: If another command is received while FW is active, TXLM settings
       will be(re)applied immediately and the index to the RF buffer updated
  @endverbatim
*/
                  
typedef struct {
  /*! \brief flag tells FW to use txChain parameter and
   * download uK image to VPE0. 1 == download only, 
   * 0 == regular TX_CONFIG CMD */
  boolean downloadUk;
  /*! \brief Index to TXLM static settings
      @verbatim
      This index points to a buffer for the non-Interfrequency mode.

      NOTE: Upon receipt of command, buffer is applied to the TXC/TXR
      immediately.
      @endverbatim
      */
  uint8 txLmStaticBufIndex;
  /*! \brief Index to RFSW-specified SSBI/RXAGC/GRFC buffer
      @verbatim
      XXX ioei - Confirm necessity of this script. One is need to for
      interfrequency handover, but not sure about initial TX config
 
      This buffer - in rfTxHomeFreqBuf - pointed to by this index is
      also known as "home" or tuning script.

      NOTE: The script is NOT applied immediately upon receipt of this
      command; instead, they applied prior to active slots.
      @endverbatim
      */
  /*! \brief SSBI/TXAGC/GRFC buffer index */
  uint8 rfTxHomeFreqBufIndex;
  /*! \brief txChain reported early by SW before TXLM 
   * shared memory interface is populated. Only read by 
   * FW if 1 == downloadUk. txChain = [0..1]*/
  uint8 txChain;
} tfw_tx_config_t;

/*! @brief TX_CONFIG command 
 *
 * This is a message definition for TX_CONFIG command
 *
 */
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  /*! \brief Body of message */
  tfw_tx_config_t      cfg;
} tfw_tx_config_cmd_t ;

  /*-----------------------------------------------------------------------*/
  /*     Message data structures for TX_APT_CONFIG                     */
  /*-----------------------------------------------------------------------*/
/*!  @brief TX APT CONFIG command
 *
 *   Message data structure for TX APT Config                  
 *
 */

typedef struct {
  /*!  Flag to indicate if APT feature is enabled */
  boolean txAptFeatureEnable;  
  uint16  txAptPdmOvDrDur;
  uint16  txAptPdmIdleVal;
  /*! \brief APT track by power threshold */
  int16  txAptTrackByPwrThresh;
} tfw_tx_apt_config_t;

typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  /*! \brief Body of message */
  tfw_tx_apt_config_t      cfg;
} tfw_tx_apt_config_cmd_t ;
/*@}*/

 /*-----------------------------------------------------------------------*/
  /*     Message data structures for XPT_CAPTURE                   */
  /*-----------------------------------------------------------------------*/
/*! @brief XPT capture command. Carries info specific to xpt sample capture.
*/
typedef struct 
{
  /*! @brief Message header */
  msgr_hdr_struct_type hdr;

  /*! @ brief Specify sample capture parameters, including buffer ID's,
   * response sequence ID */
  fw_rf_capture_param_t captureParam;

} tfw_xpt_capture_cmd_t;


/*! @brief XPT TX BURST MODE definitions to decide what wave form to use */
typedef enum 
{
  TFW_XPT_TX_BURST_MODE_OFF = 0,
  TFW_XPT_TX_SPL_BURST_MODE,
  TFW_XPT_TX_DPD_BURST_MODE,
  TFW_XPT_TX_NUM_MODE
} tfw_xpt_tx_burst_mode_e;

/*! @brief XPT TX command. Trigger encoding of special wave form and
 * special burst tx in continuous mode.
 * 
*/
typedef struct 
{
  /*! @brief Message header */
  msgr_hdr_struct_type hdr;

  /*! @brief burst mode */
  tfw_xpt_tx_burst_mode_e burstMode;

} tfw_xpt_tx_cmd_t;

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR BATON_HANDOVER COMMAND                   */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdBatonHandover BATON_HANDOVER Command
   \ingroup  cmdRxTx
   @verbatim
   BATON_HANDOVER Command
 
   Usage:
     1) This command can only be sent to FW after FW is in Tracking state
     2) Firmware will send BATON_HANDOVER_IND upon START and END of Baton Handover
     2a) For 0ms BHO, Firmware will send only 1 indication
     2) In addition to this command, the following commands must be sent 1 frame before
        the START of Baton Handover
        - UL_DPCH_PWR_CONFIG_CMD
        - UL_DPCH_CONFIG_CMD
     3) The following commands must be sent 1 frame before the END of Baton Handover
        - DL_MIDAMBLE_CONFIG_TABLE_CMD
        - SERVING_CELL_JDCS_CMD
        - DL_CCTRCH_CONFIG_CMD
        - DL_DPCH_CONFIG_CMD
   @endverbatim 
*/

typedef struct {
  /*! \brief Rx Frequency, FE (RXLM) & RF configuration
  @verbatim 
   NOTE about RxCfg.rxFreqSettings.totalFreqErrorHz:
   If its not available for the target cell, the same freqOffset as the 
   serving cell (TBD: scaling needed if freq offset unit is Hz) shall be 
   provided. 
  @endverbatim 
 */
  tfw_rx_config_t rxCfg;

  /*! \brief Tx Frequency, TXC/TXR (TXLM) & RF configuration
  @verbatim
  @endverbatim
  */
  tfw_tx_config_t txCfg;

  /*! \brief Midamble Index of the Target serving cell */
  uint8 midambleIdx;

  /*! \brief Timing offset of the target serving cell (cx8)    
  @verbatim 
   Walltime offset of the subframe boundary for the target
   serving cell (cx8) 
  @endverbatim 
  */ 
  uint16 ts0StartPosition;
                                          
  /*! \brief 32-bit wall time offset including both cx8 part as well as the
     subframe number part */
  uint32 wallTimeOfs;
                                          
  /*! \brief Subframe time when BHO begins */
  uint16 subframeStart;

  /*! \brief Baton Handover duration in subframes
  @verbatim
  Valid values: 0 through 16, ie 0ms to 80ms.
  @endverbatim
  */
  uint8 subframeDuration;
  } tfw_baton_handover_cfg_t;


/*! @brief BATON_HANDOVER Command
 *
 *   This is the message definition for BATON_HANDOVER Command
 */
/*@{*/
  typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type               hdr;
  /*! \brief Baton handover config */
  tfw_baton_handover_cfg_t          cfg;
} tfw_baton_handover_cmd_t;             
/*@}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR BATON_HANDOVER RESPONSE                       */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdBatonHandover BATON_HANDOVER Response
   \ingroup cmdRxTx
   @verbatim
   BATON_HANDOVER Response
 
   FW sends this response message to SW after the start and end of Baton
   Handover. For 0ms BHO, only one response will be sent.
   @endverbatim 
*/

typedef struct {
  /*! \brief Subframe number when baton handover starts or ends 
   @verbatim
   When bhoStarted == TRUE, this field indicates the first subframe of BHO
   When bhoEnded == TRUE, this field indicates the subframe number after BHO
   When bhoStarted = bhoEnded == TRUE, this field indicates the subframe boundary
   for 0ms BHO.
   @endverbatim
    */
  uint16 bhoSubframe;
  
  /*! \brief Serving cell's timing offset, frequency error, and midamble index 
   @verbatim
   At the START of BHO, the CURRENT serving cell's info is reported
   At the END of BHO, the TARGET serving cell's info is reported
   @endverbatim 
   */

  tfw_cell_freq_timing_t servingCell;
  
  /*! \brief Boolean flag indicating the START of BHO */
  boolean bhoStarted;
  
  /*! \brief Boolean flag indicating the END of BHO 
   @verbatim
   NOTE: For 0 ms BHO, both bhoStarted and bhoEnded will be set
   @endverbatim
   */
  /*! \brief Message header */
  boolean bhoEnded;
} tfw_baton_handover_info_t;

/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type             hdr;
  /*! \brief Baton handover configuration config */
  tfw_baton_handover_info_t        info;
} tfw_baton_handover_rsp_t;    
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR RX_CALIBRATION COMMAND               */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdRxCalibration RX_CALIBRATION Command
   \ingroup cmdRxTx 
  @verbatim
  This command initiates firmware operation for RX calibration.
 
  SW Usage:
    1) This command can only be sent to FW after FW is in FTM state
    2) This command assumes that an RX_CONFIG command has been previously
       sent to configure the RF and RXFE
    2) The command is applied immediately upon receipt
 
  FW Behavior:
  - Power Estimation: FW enables the NB power estimator, accumulating over
      128 chips, and posts to power estimate variable in RF shared memory.
      The power estimate is updated every 128 chips until another command
      is sent to disable the measurements.
  @endverbatim

*/
   
typedef enum 
{
  TFW_FTM_CAL_MODE_DISABLE = 0,
  TFE_FTM_CAL_MODE_LEGACY,
  TFW_FTM_CAL_MODE_SELF,
  TFW_FTM_CAL_NUM_MODE
} tfw_ftm_cal_mode_e;               

/*! @brief RX_CALIBRATION command 
 */
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief NB Power Estimation enable: 1=enable, 0=disable */
  union
  {
    boolean                  pwrEstEnable;
    tfw_ftm_cal_mode_e       calMode;
  };

} tfw_rx_calibration_cmd_t ;
/*@}*/ 


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR TX_CALIBRATION COMMAND                        */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdTxCalibration  TX_CALIBRATION Command 
   \ingroup cmdRxTx 
  @verbatim
  This command initiates firmware operation for TX calibration.
 
  SW Usage:
    1) This command can only be sent to FW after FW is in FTM state
    2) This command assumes that a TX_CONFIG command has been previously
       sent to configure the RF and TXC/TXR
    2) The command is applied immediately upon receipt
 
  FW Behavior:
  - Continuous Transmission: FW enables the modulator to transmit the
      TD-SCDMA Special Burst sequence with 0 Gap Period. This achieves
      continuous transmission until another command is received to disable
      transmission.
 
      Transmission is at fixed digital gain of XXX dB. FW-based TxAGC does
      not run. FW does not issue any SSBI/GRFC writes to control analog gain
      or PA state.
 @endverbatim 
*/



/*! @brief TX_CALIBRATION command 
 */
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief Calibration control flag */
  union
  {
    boolean                  continuousSpecialBurstEnable;
    tfw_ftm_cal_mode_e       calMode;
  };
  
} tfw_tx_calibration_cmd_t ;
/*@}*/ 

                  

/**
   \defgroup cmdFastCalibration FAST_CALIBRATION Command
   \ingroup cmdRxTx 
  @verbatim
  This command initiates firmware operation for FAST RF calibration.
 
  SW Usage:
    1) This command can only be sent to FW after FW is in FTM state
    2) This command assumes that a RX_CONFIG & TX_CONFIG command have
       been previously sent to configure the RF, RXFE, TXC/TXR
    2) The command is applied immediately upon receipt
 
  FW Behavior:
  - XXX TBD
 @endverbatim
*/

/*! @brief FAST_CALIBRATION command 
 */
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief Fast Calibration enable
      @verbatim
      Fast calibration does the power estimate on TS0 and transmits the 
      UL signal with fast pace ramping up and down the levels. The power level
      is control by either index or implicit looping through the default tables.
 
      1 = Enable
      0 = Disable
      @endverbatim
     */
  boolean                  fastCalibEnable;
  /*! \brief
      @verbatim
      Shared Memory update with SSBI index to preload LUT (for fast calibration)
      @endverbatim
      */
  boolean                  useTxSsbiIndexTableEnable;
} tfw_fast_calibration_cmd_t ;
/*@}*/ 


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR RX_TX_SM_UPDATE COMMAND                       */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdRxTxSMUpdate RX_TX_SM_UPDATE Command 
   \ingroup cmdRxTx 
  @verbatim
  @endverbatim
*/
                  

/*! @brief RX_TX_SM_UPDATE command 
 *
 * This is a message definition for RX_TX_SM_UPDATE command
 *
 */
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief SM partition Indicator (the memory regions, likely enum-indexed are TBD) */
  uint8                   sharedMemoryBlockIndicator;
} tfw_rx_tx_sm_update_cmd_t ;
/*@}*/ 

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR IQMC_TEMP_COMPENSATION COMMAND                  */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdTempCompensation IQMC_TEMP_COMPENSATION Command 
   \ingroup cmdTempCompensation 
  @verbatim
  @endverbatim
*/
                  

/*! @brief IQMC_TEMP_COMPENSATION command 
 *
 * This is a message definition for IQMC_TEMP_COMPENSATION command
 *
 */
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief IQMC override enable */
  boolean iqmcAbOverrideFlag;
  /*! \brief IQMC AB: A - lower 16 bits, B - upper 16 bits  */
  uint32 iqmcAb;
  /*! \brief IQMC AB for diversity. Format the same as iqmcAb */
  uint32 iqmcAbDiv;
} tfw_iqmc_temp_compensation_cmd_t ;
/*@}*/ 

  /*-----------------------------------------------------------------------*/
  /*                  DATA STRUCTURES FOR Override MESSAGE                 */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdRxTxOverRide RX_TX_OVERRIDE_CONFIG Command
   \ingroup cmdRxTx
   @verbatim
   Override command configures a few key TX/RX parameters for mission mode.
   @endverbatim 
*/
typedef struct 
{
  /*! \brief Flag to indicate if LNA range should be overridden.
  @verbatim 
   1: override
   0: no override 
  @endverbatim 
  */
  boolean rxLnaStateOverrideFlag;
  /*! \brief LNA range when flag is true.
  @verbatim 
   LNA override state specified here. 
  @endverbatim 
  */
  uint8 rxLnaStateOverrideVal;
  /*! \brief Flag to determine whether to override TX Power value.
  @verbatim 
   1: override
   0: no override 
  @endverbatim 
  */
  boolean txPwrOverrideFlag;
  /*! \brief Specifies override value for TX Power in dBm512 format. 
  @verbatim 
   Override value in int16Q9 (dBm). 
  @endverbatim 
  */
  int16  txPwrOverrideVal;
  /*! \brief Flag to override rx/tx dynamic switching with the bitmap value 
  @verbatim 
   1: override
   0: no override 
  @endverbatim 
  */
  boolean rxTxSwchOverrideFlag;
  /*! \brief Specifies override value for RX/TX dynamic switching.  
  @verbatim 
   Bit0 to bit 7 are for DL time slot configuration.
   Bit8 to bit 15 are for UL time slot configuration. 
  @endverbatim 
  */
  uint16  rxTxSwchOverrideVal;
  /*! \brief Flag to determine whether to override TPC pattern.
  @verbatim 
   1: override, FW will follow RFSW's TPC pattern setting
   0: no override, FW will follow TFW own decoded TPC pattern
  @endverbatim 
  */
  boolean tpcPatternOverrideFlag;
  /*! \brief Specifies override value for TPC pattern. 
  @verbatim 
   1: up.
   0: down
  @endverbatim 
  */
  int16  tpcPatternOverrideVal;
} tfw_rx_tx_override_t;
/*! @brief RX_TX_OVERRIDE_CONFIG Command
 *
 *   This is the message definition for RX_TX_HDET Configuration
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type         hdr;
  /*! \brief Override configuration */
  tfw_rx_tx_override_t overRideConfig;
} tfw_rx_tx_override_config_cmd_t;                     
/*@}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR IQ CAPTURE COMMAND                            */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdRxTxIqCapture IQ_CAPTURE Command
   \ingroup cmdRxTx
   @verbatim
    This command triggers FW to log IQ samples and post to rf_log_buffer
    in FW_SMEM_COMMON_FW_RF region.
 
    NOTES:
    - This command may only be used in FTM state
    - FTM state sample format assumes Cx1, 10-bit PCFL - ie "acquisition mode"
    - Maximum number of samples captured in one command is 12800.
    - IQ CAPTURE assumes that RX_CONFIG command has been received to properly
    configure the RXFE & RF.

   @endverbatim 
*/


/*! @brief IQ_CAPTURE
 *
 *   This is the message definition for IQ_CAPTURE Configuration
 */
typedef struct {
  /*! \brief Number of samples to collect. Maximum value: 12800 if destAddr = 0 */
  uint32 numSamples;
  /*! \brief Start time to collect samples. If 0, collect immediately 
  @verbatim
  startTime = 0 -> immediately collect samples
            = UNIV_STMR delta value -> collect samples after the specified delta time.
 
  UNIV_STMR_MSTMR is a 32-bit Cx15 counter
  @endverbatim
  */
  uint32 startTime;
  /*! @brief Destination sample buffer address */
  uint32  destAddrPrx;
  uint32  destAddrDiv;
  /*! @brief Set to TRUE to receive IQ capture response message */
  boolean sendMsgrRsp;

  /*! \brief isCx2 to indicate cx2 or cx1, TRUE:Cx2, FALSE:Cx1 */
  boolean isCx2;

  /*! \brief  antSel indicate for diversity, TFW_ANT_0: chain 0 only, TFW_ANT_1 chain 1 only, TFW_ANT_BOTH: both chain*/
  tfw_ant_sel_e antSel;
  /*! @brief User data to be returned back in response message */
  uint32  userData;
} tfw_iq_capture_cfg_t;


/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  /*! \brief IQ Capture configuration */
  tfw_iq_capture_cfg_t cfg;
} tfw_iq_capture_cmd_t;                     

/*! @brief IQ_CAPTURE Response
 *
 *   This is the message definition for IQ_CAPTURE Response
 */
typedef enum {
  T_IQ_CAPTURE_FAILURE=0,
  T_IQ_CAPTURE_SUCCESS=0xFF
} tfw_iq_capture_status_e;

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  /*! \brief Status of IQ_CAPTURE command */
  tfw_iq_capture_status_e status;
  /*! \brief Selected antenna */
  tfw_ant_sel_e antSel;
  /*! \brief Number of samples captured */
  uint32  numSamples;
  /*! \brief Start time of IQ capture */
  uint32  startTime;
  /*! \brief Start address of IQ capture buffer in shared memory. */
  union {
    uint32  startAddress;
    uint32  startAddressPrx;
  };
  uint32  startAddressDiv;
  /*! @brief User data that was set in IQ capture command */
  uint32  userData;
} tfw_iq_capture_rsp_t;
/*@}*/


/*NSHEN: Get Multi Synth, FTM-RF*/
typedef struct {
	/*! \brief Message header */
	msgr_hdr_struct_type hdr;
	
	/*! \brief SSBI reading info for rx pll */
	//tfw_rd_ssbi_cfg_t   rx_pll;
	
	/*! \brief SSBI reading info for tx pll */
	//tfw_rd_ssbi_cfg_t   tx_pll;
}tfw_ftm_get_multi_synth_state_cmd_t;

typedef struct
{
	/*! \brief Message header */
	msgr_hdr_struct_type hdr;	
	
	/*! \brief 1 means FW successfully read the SSBI for Rx and Tx, 0 for anything wrong */	
	uint8 status;
	
	/*! \brief Indicate the status of Rx PLL, 0 means UNLOCKED, 1 means LOCKED  */
	uint8 rx_pll_status;
	
	/*! \brief Indicate the status of Tx PLL, 0 means UNLOCKED, 1 means LOCKED */
	uint8 tx_pll_status;
	
	/*! \brief Indicate the if the cmd is issued right after continous tx flag is on and before tx hw is really turned on */	
	uint8 is_cont_tx_valid;
}tfw_ftm_get_multi_synth_state_rsp_t;


/*End */



  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR FTM_TX COMMAND                                */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdFtmTx  FTM TX Command 
   \ingroup cmdRxTx 
  @verbatim
  This command controls firmware operation for FTM Tx mode.
 
  SW Usage:
    RFSW issues the cmd to start or stop the FTM Tx operation.
    This can also be used to reconfigure the ongoing FTM Tx.
 
  FW Behavior:
    FW starts/stops the FTM Tx within 5ms duration.
       
 @endverbatim 
*/

typedef struct 
{
  /** Physical Channel Present Indication. */
  /** Valid [0,1] :
      \li 0 : not present, the rest of parameters are ignored.
      \li 1 : present
       */
  uint16  valid : 1;
  /** OVSF Code. Valid [1..sf], sf=1,2,4,8,16 */
  uint16  chanCode : 5;
  /** Spreading factor for DPCH. Valid [1,2,4,8,16] */
  uint16  sf : 5;
  /*! \brief Modulation type: 0 (QPSK) or 1 (16QAM) */
  uint16  modType : 1;
  uint16  : 4;
  /*! \brief Channel desired TX power.
  Valid <TBD> [-50dBm..33dBm] in 1/16 dBm step */
  int16   chTxPwr; 
} tfw_ftm_tx_phy_ch_cfg_t;

typedef struct 
{
  /*! \brief Enable/disable FTM Tx mode operation
  @verbatim 
   TRUE: Enable FTM Tx mode operation
   FALSE: Disable it
   Note: if it is set to Ture, other variable will be processed.
   if set to false, other variable is not cared, just shut off tx directly.
  @endverbatim 
  */
  boolean enableFtmTx;
  /*! \brief N slots to transmit
  @verbatim 
   0-5 slots
   if set to 0, means ftm works in continuous slot mode
  @endverbatim 
  */
  uint32 nSlots : 3;
  /*! \brief Sub ID for the command
  @verbatim 
   0-2: based on the source of commands in RFSW.
  @endverbatim 
  */
  uint32 subID : 3;
  uint32       : 26;
  /*! \brief frequency error offset
  @verbatim 
   1-5 slots
  @endverbatim 
  */
  int32 freqOffset;
  /*! \brief tx frequency number (HZ)
  @verbatim 
   1-5 slots
  @endverbatim 
  */
  uint32 txfreq;
  
  /*! \brief phy channel config
  @verbatim 
   Phy channel config for up to 2 channels in a slot
  @endverbatim 
  */
  tfw_ftm_tx_phy_ch_cfg_t  phy[TFW_MAX_UL_PHY_PER_SLOT];
  /*! \brief Tx Frequency, TXC/TXR (TXLM) & RF configuration
  @verbatim 
  @endverbatim 
  */
  tfw_tx_config_t  txCfg;
} tfw_ftm_tx_config_t;

/*! @brief FTM_TX command 
 */
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief FTM Tx config */
  tfw_ftm_tx_config_t      cfg;
} tfw_ftm_tx_cmd_t ;
/*@}*/ 

/*! @brief FTM_TX command_response
*/
/*@{*/ 
typedef struct
{
  /*! \cmd status, TRUE: success, FALSE: fail*/
  boolean status;
  /*! \cmd status, TRUE: reset, FALSE: continue no reset happens;
    the variable is only valid when status==FALSE*/
  boolean reset;
  /*! \indicate this response is for which cmd SUB ID */
  uint32  subID;
} tfw_ftm_tx_cmd_status_t;
/*@}*/


/*! @brief FTM_TX command_response
*/
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  tfw_ftm_tx_cmd_status_t  rsp;
} tfw_ftm_tx_cmd_rsp_t;
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR FTM_RX COMMAND                                */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdFtmRx  FTM RX Command 
   \ingroup cmdRxTx 
  @verbatim
  This command controls firmware operation for FTM Rx mode.
 
  SW Usage:
    RF SW issues this cmd to start or stop the FTM Rx mode operation.    
 
  FW Behavior:
    SW starts/stops within 5ms duration.
 
      
 @endverbatim 
*/
typedef struct 
{
  /*! \brief enable/disable FTM Rx mode.
  @verbatim
    TRUE: enable FTM Rx
    FALSE: disable it
  @endverbatim 
  */
  boolean  enableFtmRx;
  /*! \brief enable/disable LNA state machine.
  @verbatim
    TRUE: enable LNA state machine handling
    FALSE: LNA state is fixed to the one given in Rx config below
  @endverbatim 
  */
  boolean  enableLnaStateMachine;
  /*! \brief N slots to measure
  @verbatim 
   1-6 slots
  @endverbatim 
  */
  uint8 nSlots;
  /*! \brief Rx Frequency, FE (RXLM) & RF configuration
  @verbatim 
  @endverbatim 
  */
  tfw_rx_config_t rxCfg;
} tfw_ftm_rx_config_t;
                  
                  
/*! @brief FTM_RX command 
 */
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief FTM Tx config */
  tfw_ftm_rx_config_t      cfg;
} tfw_ftm_rx_cmd_t ;
/*@}*/ 

/*! @brief FTM_RX command_response
*/
/*@{*/ 
typedef struct
{
  /*! \cmd status, TRUE: success, FALSE: fail*/
  boolean status;
} tfw_ftm_rx_cmd_status_t;
/*@}*/


/*! @brief FTM_RX command_response
*/
/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  tfw_ftm_rx_cmd_status_t  rsp;
} tfw_ftm_rx_cmd_rsp_t;
/*@}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR LINK RESPONSE                                 */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdRxTxLinkResponse LINK_RESPONSE Command
   \ingroup cmdRxTx
   @verbatim
      Link response returns frequence error from ftl to XO manager.
   @endverbatim 
*/
typedef struct 
{
  /*! \brief Total frequency error
  @verbatim
   TBA
  @endverbatim 
  */
  int32  total_freq_error;
  /*! \brief Rotator frequency error
  @verbatim 
   TBD 
  @endverbatim 
  */
  int32  rotator_freq_error;
  /*! \brief DPLL frequency error
  @verbatim 
   TBD 
  @endverbatim 
  */
  int32  dpll_freq_error;
} tfw_rx_tx_link_result_t;
/*! @brief RX_TX_LINK Response
 *
 *   This is the message definition for RX_TX_LINK Response
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type         hdr;
  /*! \brief Power scan response */
  tfw_rx_tx_link_result_t linkResult;
} tfw_rx_tx_link_rsp_t;                     
/*@}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR ADC CLOCK GATING                              */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdAdcClockGating ADC_CLOCK_GATING Command
   \ingroup cmdRxTx
   @verbatim
      Config message to enable/disable adc clock gating
   @endverbatim 
*/
typedef struct {
  /*! \brief flag to enable/disable adc clock gating.
  @verbatim
    TRUE : adc clock gating is enabled,
    FALSE: adc clock gating is disabled
  @endverbatim 
  */
  boolean                      gatingEnabled;
  /*! \brief subframe number by which gating configuration should take effect.
  @verbatim
    FW won't wait for the subframe number to apply the configuration;
    TBD action if current time has passed the sfn
  @endverbatim 
  */
  uint16                       sfn;
} tfw_adc_clock_gating_config_t;
/*! @brief ADC_CLOCK_GATING Configuration
 *
 *   This is the message definition for ADC_CLOCK_GATING Configuration
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type         hdr;
  /*! \brief adc clock gating config */
  tfw_adc_clock_gating_config_t cfg;
} tfw_adc_clock_gating_config_cmd_t;                     
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR TX_TEST_AGC COMMAND                           */
  /*-----------------------------------------------------------------------*/
/**
  XXX ioei - Remove this!
 
  This command is used for testing purposes only
*/
                  
/* TX_TEST_AGC command 
 *
 * This is a message definition for TX_TEST_AGC command
 *
 */
typedef struct
{
  /* Message header */
  msgr_hdr_struct_type     hdr;
  /* Number of sub-frame the txAgc test is conducted */
  uint8                    count;
} tfw_tx_test_agc_cmd_t ;


#endif /* TFW_SW_INTF_RX_TX_H */
