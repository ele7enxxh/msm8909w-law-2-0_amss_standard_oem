/*!
  @file
  tfw_rf_sm_intf.h

  @brief
  TD-SCDMA FW header file for RF SW shared memory interface definitions

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_rf_sm_intf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/14/14   VK      Add support for inter-freq tuner update
08/06/13   VK      API changes for DPD for EPT
07/17/13   SL      Increase Rx Tuning buffer since Rx SSBIs have increased.
05/07/13   SL      Double buffer to fix racing condition crash.
02/27/13   VK      BHO startup script, IFREQ_TX script, RFFE PA state info,
                   support for home/ target freq for APT and PA state update
02/07/13   SL      Added priority field.
12/10/12   VK      Interface for APT. Rename PA state update structure entries
12/04/12   VK      Interface for dynamic PA state update
06/20/11   WS      Created the RF shared memory interface definitions. 
07/25/11   WS      Revised the SSBI and GRFC designs. 
08/15/11   IO      Wholesale revision for clarity and Doxygen support 
===========================================================================*/

#ifndef TFW_RF_SM_INTF_H
#define TFW_RF_SM_INTF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tfw_sw_intf_common.h"
#include "tfw_const.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------*/
/*                        TFW RF Calibration Results                         */
/*---------------------------------------------------------------------------*/
/**
    \defgroup rfSmiCal RF Calibration Results
    \ingroup rfSharedMemoryInterface
    @verbatim
    This section describes the results structures for RF calibration
    @endverbatim
    */
/*\{*/
typedef struct {
  /*! \brief Index number for the next index to the results[] */
  uint8 index;
  /*! \brief Measurement result in S16Q8 */
  int16  dlPowerLevel; 
  /*! \brief Measurement result in S16Q8 for diversity antenna*/
  int16  dlPowerLevelDiv; 
} tfw_rf_sm_ftm_slow_cal_results_t;
/*\}*/

/*---------------------------------------------------------------------------*/
/*                        TFW RF AGC Log                                     */
/*---------------------------------------------------------------------------*/
/**
    \defgroup rfAgcLog RF AGC LOG
    \ingroup rfSharedMemoryInterface
    @verbatim
    This section describes the RF AGC log
    @endverbatim
    */
/*\{*/
typedef struct
{
  /*! \brief LNA state 0~3 */
  uint16 lnaState;
  /*! \brief DVGA gain */
  int16 rxfeDvgaGain;
  /*! \brief Narrow-band received power*/
  uint32 nbRxPower;
  /*! \brief Wide-band received power*/
  uint32 wbRxPower;
  /*! \brief Received power in dBm */
  int32 rxPowerdBm;
} tfw_rx_agc_result_per_chain_t;

typedef struct
{
  /*! \brief flag to indicate whether this slot is active */
  uint32 active              : 1;
  /*! \brief flag to indicate if the operation frequency is an inter-frequency */
  uint32 isInterfreq         : 1;
  /*! \brief antenna selection, 0:ant0, 1:ant1, 2:both */
  uint32 antennaSelect       : 2;
  /*! \brief reserved */
  uint32                     : 28;
  /*! \brief operation frequency, Hz */
  uint32 operationFreq;
  /*! \brief per chain info */
  tfw_rx_agc_result_per_chain_t chain[2];
} tfw_rx_agc_result_per_slot_t;

typedef struct
{
  /*! \brief subframe number */
  uint16 sfn;
  /*! \brief frequency error, 32Q8, Hz */
  int32 freqError;
  /*! \brief rotator adjust */
  int32 rotatorVal;
  /*! \brief per slot info: TS0~TS6 */
  tfw_rx_agc_result_per_slot_t slot[TFW_SLOTS_PER_SUBFRAME];
} tfw_rx_agc_result_buf_t;


typedef struct
{
  /*! \brief  modulation type
  @verbatim 
      0 QPSK,
      1 16QAM,
  @endverbatim
  */
  uint16   modType                                  : 1;
  /*! \brief  Spreading factor   
  @verbatim 
      1/2/4/8/16
  @endverbatim
  */
  uint16   sf                                        : 5;
  /*! \brief  OVSF code number  
  @verbatim 
      1-SF depending upon the SF. 
  @endverbatim
  */
  uint16   codeNum                                   : 4;
  /*! \brief  channel type
  @verbatim 
    Formula  :  
      0 NONE,
      1 UPPCH,
      2 PRACH,
      3 DPCH,
      4 ERUCCH,
      5 EUCCH,
      6 EPUCH,
      7 HSSICH
  @endverbatim
  */
  uint16   chanType                                  : 6;
  /*! \brief Desired power from SW*/
  int16   desiredTxPwrSW;
  /*! \brief actual TX power*/
  int16 actualTxChanPower;
  /*! \brief computed TX power*/
  int16 computedTxChanPower;
  /*! \brief  beta gain for data part of the physical channel*/
  int16    betaData;
  /*! \brief  beta gain for midamble*/
  int16    betaMid;
  /*! \brief PA backoff*/
  uint16   paBackoff;
} tfw_tx_agc_result_per_chan_t;

typedef struct
{
  /*! \brief flag to indicate whether this slot is active*/
  uint16 active;
  /*! \brief Total TX power*/
  int16 txOutputPower;
  /*! \brief maximum power ratio*/
  int16 mprValue;
  /*! \brief analog power*/
  int16 analogPwr;
  /*! \brief backoff at DAC*/
  int16 dacBackoff;
  /*! \brief power compensation of temperature and frequency*/
  int16 tempFreqPwrComp;
  /*! \brief MTPL, Max transmit power limit*/
  uint16 mtpl;
  /*! \brief Tx gain entry, 0-63*/
  uint16 rfaGainLutIndex                             : 10;
  /*! \brief PA Gain State, 0-3*/
  uint16 paGainState                                 : 2;
  /*! \brief Flag indicating min power limits reached*/
  boolean minPwrLimited                              : 1;
  /*! \brief Flag indicating max power limits reached*/
  boolean maxPwrLimited                              : 1;
  /*! \brief Flag indicating transmit power override*/
  boolean txPwrOverrideFlag                          : 1;
    /* TPC override flag */
  boolean tpcPatternOverrideFlag                     : 1;
  /*! \brief Transmit power override value (Q4)*/
  int16 txPwrOverrideValue;
  /* TPC override value 1 up, -1 down */
  int16  tpcPatternOverrideVal;
  /*! \brief transmission frequency*/
  uint32 frequency;
  /*! \brief pathloss (Q4)*/
  uint32 pathloss;

  /*! \brief per chan info*/
  tfw_tx_agc_result_per_chan_t chan[TFW_MAX_UL_PHY_PER_SLOT];
} tfw_tx_agc_result_per_slot_t;

typedef struct
{
  /*! \brief subframe number */
  uint16 sfn;
  /*! \brief rotator adjust */
  int32 rotatorVal;
  /*! \brief time advance */
  int32 TimingAdvance;

  /*! \brief per slot info, UpPTS and TS1-5*/
  tfw_tx_agc_result_per_slot_t slot[TFW_MAX_UL_SLOT+1];
} tfw_tx_agc_result_buf_t;

typedef struct
{
  tfw_rx_agc_result_buf_t rx;
  tfw_tx_agc_result_buf_t tx;
} tfw_agc_result_buf_t;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* TFW_RF_SM_INTF_H */
