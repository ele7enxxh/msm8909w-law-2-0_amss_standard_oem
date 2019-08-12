/*!
  @file
  rfgsm_ftm_msg.h

  @brief
  Message router UMIDs header for RF GSM FTM subsystem. This file defines
  interfaces for the RF GSM FTM commands to be processed by the
  RF Task.

  @note
  This file must be named with a _msg.h suffix for the build environment
  to correctly process the messages defined within.

*/

/*===========================================================================

  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                     EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/gsm/rfgsm_ftm_msg.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $ 


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/08/14   ec      Add message IDs to set and get SAWless linearity mode
05/29/14   sb      IP2 cal bug fixes, updates
04/16/14   sb      IP2 cal Interface and functionality changes
03/26/14   sb      Add support for IP2 cal for Dime PM
07/30/13   ec      Add types for FTM IQ capture configuration using MSGR
04/18/13   sb      Add FTM hook for overriding PA scaling,smps bias
03/28/13   aka     GSM frame sweep feature changes.
02/15/13   sb      Added FTM hook to override tuner tune code 
11/27/12   tws     Add rfm device to all command/response structures
08/16/12   ec	     Cleanup of FTM set pa range implementation
08/01/12   ec      Modified parameters for set pa range, 
                   set linear rgi.
06/26/12   ggs     Typo fix
05/24/12   ec      Added structures and UMID definitions for set_band cmd/rsp
05/10/12   ec      Added structures and UMID definitions for set_tx_pow_dbm and
                   tune_tx commands
04/04/12   ggs     Struct definition name changes
03/19/12   ggs     Added more message struct definitions
03/12/12   ggs     Initial version based upon rf_lte_ftm_msg.h

===========================================================================*/

#ifndef RFGSM_FTM_MSG_H
#define RFGSM_FTM_MSG_H

/*===========================================================================
 
                      INCLUDE FILES

===========================================================================*/
#include "appmgr_msg.h"
#include "msgr.h"
#include "msgr_types.h"
#include "rfa_msgr.h"
#include "rfcom.h"
#include "rfgsm_core_types.h"
#include "rfgsm_core_handle.h"

/*===========================================================================

                      EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*-----------------------------------------------------------------------*/
/*     MSGR COMMAND ID'S                                             */
/*-----------------------------------------------------------------------*/

/*! @todo Most of these definitions are placeholders for possible future use */

/*! @brief ID for FTM to enter GSM mode of operation
 */
#define RFA_RF_GSM_FTM_MSG_ID_SET_MODE                          1

/*! @brief ID for FTM to turn Tx on */
#define RFA_RF_GSM_FTM_MSG_ID_SET_TX_ON                         2

/*! @brief ID for FTM to turn Tx off */
#define RFA_RF_GSM_FTM_MSG_ID_SET_TX_OFF                        3

/*! @brief ID for FTM to set LNA range */
#define RFA_RF_GSM_FTM_MSG_ID_SET_LNA_RANGE                     4

/*! @brief ID for FTM to set PDM */
#define RFA_RF_GSM_FTM_MSG_ID_SET_PDM                           5

/*! @brief ID for FTM to set channel */
#define RFA_RF_GSM_FTM_MSG_ID_SET_BAND                          6

/*! @brief ID for FTM to set Tx in continuous mode */
#define RFA_RF_GSM_FTM_MSG_ID_SET_TRANSMIT_CONT                 7

/*! @brief ID for FTM to set Tx in burst mode */
#define RFA_RF_GSM_FTM_MSG_ID_SET_TRANSMIT_BURST                8

/*! @brief ID for FTM to set Rx to burst mode */
#define RFA_RF_GSM_FTM_MSG_ID_SET_RX_BURST                      9

/*! @brief ID for FTM to get RSSI */
#define RFA_RF_GSM_FTM_MSG_ID_GET_RSSI                          10

/*! @brief ID for FTM to set PA start delta */
#define RFA_RF_GSM_FTM_MSG_ID_SET_PA_START_DELTA                11

/*! @brief ID for FTM to set PA stop delta */
#define RFA_RF_GSM_FTM_MSG_ID_SET_PA_STOP_DELTA                 12

/*! @brief ID for FTM to set PA DAC input */
#define RFA_RF_GSM_FTM_MSG_ID_SET_PA_DAC_INPUT                  13

/*! @brief ID for FTM to set Rx in continuous mode */
#define RFA_RF_GSM_FTM_MSG_ID_SET_RX_CONTINUOUS                 14

/*! @brief ID for FTM to set GSM Tx path delay */
#define RFA_RF_GSM_FTM_MSG_ID_SET_PATH_DELAY                    15

/*! @brief ID for FTM to set the Tx frame matrix */
#define RFA_RF_GSM_FTM_MSG_ID_SET_SLOT_OVERRIDE_FLAG            16

/*! @brief ID for FTM to perform Rx gain range calibration */
#define RFA_RF_GSM_FTM_MSG_ID_RX_GAIN_RANGE_CAL                 17

/*! @brief ID for FTM to perform Tx KVCO calibration */
#define RFA_RF_GSM_FTM_MSG_ID_TX_KV_CAL                         18

/*! @brief ID for FTM to perform Tx KVCO calibration version 2 algorithm (speed optimised) */
#define RFA_RF_GSM_FTM_MSG_ID_TX_KV_CAL_V2                      19

/*! @brief ID for FTM to set the PA gain range */
#define RFA_RF_GSM_FTM_MSG_ID_SET_LINEAR_PA_RANGE               20

/*! @brief ID for FTM to set the RGI (Radio Gain Index) value */
#define RFA_RF_GSM_FTM_MSG_ID_SET_LINEAR_RGI                    21

/*! @brief ID for FTM to capture GSM Rx IQ samples */
#define RFA_RF_GSM_FTM_MSG_ID_CAPTURE_IQ                        22

/*! @brief ID for FTM to adjust Rx timing */
#define RFA_RF_GSM_FTM_MSG_ID_SET_RX_TIMING                     23

/*! @brief ID for FTM to adjust Tx timing */
#define RFA_RF_GSM_FTM_MSG_ID_SET_TX_TIMING                     24

/*! @brief ID for FTM to perform Tx gain sweep for calibration */
#define RFA_RF_GSM_FTM_MSG_ID_SET_TX_GAIN_SWEEP                 25

/*! @brief ID for FTM to perform Tx envelope DC sweep for calibration */
#define RFA_RF_GSM_FTM_MSG_ID_TX_ENVDC_CS_SWEEP                 26

/*! @brief ID for FTM to perform AMAM sweep version 2 calibration */
#define RFA_RF_GSM_FTM_MSG_ID_CFG2_AMAM_SWEEP_V2                27

/*! @brief ID for FTM to set Rx multislot */
#define RFA_RF_GSM_FTM_MSG_ID_SET_RX_MULTISLOT                  28

/*! @brief ID for FTM to set Rx burst gain for the expected power level */
#define RFA_RF_GSM_FTM_MSG_ID_SET_RX_BURST_FOR_EXPECTED_PWR     29

/*! @brief ID for FTM to get the measured power for the previous Rx bursts */
#define RFA_RF_GSM_FTM_MSG_ID_GET_MEAS_PWR                      30

/*! @brief ID for FTM to set the tx power in dBm*/
#define RFA_RF_GSM_FTM_MSG_ID_SET_TX_POW_DBM                    31

/*! @brief ID for FTM to tune Tx */
#define RFA_RF_GSM_FTM_MSG_ID_TUNE_TX                           32

/*! @brief ID for FTM to override tuner code */
#define RFA_RF_GSM_FTM_MSG_ID_TUNER_OVERRIDE                    33


#define RFA_RF_GSM_FTM_MSG_ID_SET_IP2_CAL_PARAMS            34

#define RFA_RF_GSM_FTM_MSG_ID_GET_IP2_CAL_PARAMS            35

#define RFA_RF_GSM_FTM_MSG_ID_GET_IP2_CAL_RESULTS           36

#define RFA_RF_GSM_FTM_MSG_ID_SET_SAWLESS_LIN_MODE          37

#define RFA_RF_GSM_FTM_MSG_ID_GET_SAWLESS_LIN_MODE          38


/****************************************************************************
                      RF Task GSM messages
****************************************************************************/
/*! @brief GSM subsystem message IDs the RF task receives. 
   This enum is never called by name. It just defines symbols.
*/

/*! @todo Most of these command UMIDs are placeholders for possible future use */
enum
{
  /*! Command to enter GSM FTM mode, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_MODE, RFA_RF_GSM_FTM_MSG_ID_SET_MODE, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to turn Tx on, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_TX_ON, RFA_RF_GSM_FTM_MSG_ID_SET_TX_ON, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to turn Tx off, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_TX_OFF, RFA_RF_GSM_FTM_MSG_ID_SET_TX_OFF, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set LNA range, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_LNA_RANGE, RFA_RF_GSM_FTM_MSG_ID_SET_LNA_RANGE, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set PDM, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_PDM, RFA_RF_GSM_FTM_MSG_ID_SET_PDM, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set channel, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_BAND, RFA_RF_GSM_FTM_MSG_ID_SET_BAND, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set Tx in continuous mode, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_TRANSMIT_CONT, RFA_RF_GSM_FTM_MSG_ID_SET_TRANSMIT_CONT, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set Tx in burst mode , sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_TRANSMIT_BURST, RFA_RF_GSM_FTM_MSG_ID_SET_TRANSMIT_BURST, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set Rx to burst mode, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_RX_BURST, RFA_RF_GSM_FTM_MSG_ID_SET_RX_BURST, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to get RSSI, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, GET_RSSI, RFA_RF_GSM_FTM_MSG_ID_GET_RSSI, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set PA start delta, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_PA_START_DELTA, RFA_RF_GSM_FTM_MSG_ID_SET_PA_START_DELTA, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set PA stop delta, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_PA_STOP_DELTA, RFA_RF_GSM_FTM_MSG_ID_SET_PA_STOP_DELTA, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set PA DAC input, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_PA_DAC_INPUT, RFA_RF_GSM_FTM_MSG_ID_SET_PA_DAC_INPUT, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set Rx in continuous mode, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_RX_CONTINUOUS, RFA_RF_GSM_FTM_MSG_ID_SET_RX_CONTINUOUS, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set GSM Tx path delay, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_PATH_DELAY, RFA_RF_GSM_FTM_MSG_ID_SET_PATH_DELAY, rfa_rf_gsm_ftm_set_path_delay_type_cmd_s),

  /*! Command to set the slot_override_flag, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_SLOT_OVERRIDE_FLAG, RFA_RF_GSM_FTM_MSG_ID_SET_SLOT_OVERRIDE_FLAG, rfa_rf_gsm_ftm_set_slot_override_flag_type_cmd_s),

  /*! Command to perform Rx gain range calibration, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, RX_GAIN_RANGE_CAL, RFA_RF_GSM_FTM_MSG_ID_RX_GAIN_RANGE_CAL, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to perform Tx KVCO calibration, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, TX_KV_CAL, RFA_RF_GSM_FTM_MSG_ID_TX_KV_CAL, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to perform Tx KVCO calibration version 2 algorithm (speed optimised), sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, TX_KV_CAL_V2, RFA_RF_GSM_FTM_MSG_ID_TX_KV_CAL_V2, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set the PA gain range, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_LINEAR_PA_RANGE, RFA_RF_GSM_FTM_MSG_ID_SET_LINEAR_PA_RANGE, rfa_rf_gsm_ftm_set_pa_range_type_cmd_s),

  /*! Command to set the RGI (Radio Gain Index) value, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_LINEAR_RGI, RFA_RF_GSM_FTM_MSG_ID_SET_LINEAR_RGI, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to capture GSM Rx IQ samples, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, CAPTURE_IQ, RFA_RF_GSM_FTM_MSG_ID_CAPTURE_IQ, rfa_rf_gsm_ftm_capture_iq_type_cmd_s),

  /*! Command to adjust Rx timing, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_RX_TIMING, RFA_RF_GSM_FTM_MSG_ID_SET_RX_TIMING, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to adjust Tx timing, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_TX_TIMING, RFA_RF_GSM_FTM_MSG_ID_SET_TX_TIMING, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to perform Tx gain sweep for calibration, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_TX_GAIN_SWEEP, RFA_RF_GSM_FTM_MSG_ID_SET_TX_GAIN_SWEEP, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to perform Tx envelope DC sweep for calibration, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, TX_ENVDC_CS_SWEEP, RFA_RF_GSM_FTM_MSG_ID_TX_ENVDC_CS_SWEEP, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to perform AMAM sweep version 2 calibration, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, CFG2_AMAM_SWEEP_V2, RFA_RF_GSM_FTM_MSG_ID_CFG2_AMAM_SWEEP_V2, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set Rx multislot, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_RX_MULTISLOT, RFA_RF_GSM_FTM_MSG_ID_SET_RX_MULTISLOT, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set Rx burst gain for the expected power level, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_RX_BURST_FOR_EXPECTED_PWR, RFA_RF_GSM_FTM_MSG_ID_SET_RX_BURST_FOR_EXPECTED_PWR, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to get the measured power for the previous Rx bursts, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, GET_MEAS_PWR, RFA_RF_GSM_FTM_MSG_ID_GET_MEAS_PWR, rfa_rf_gsm_ftm_set_mode_type_cmd_s),

  /*! Command to set the Tx power in dBm for a specific slot index, sent from FTM task to the RF task*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_TX_POW_DBM, RFA_RF_GSM_FTM_MSG_ID_SET_TX_POW_DBM, rfa_rf_gsm_ftm_set_tx_pow_dbm_type_cmd_s),
  
  /*! Command to tune the RF to a specific arfcn, sent from FTM task to the RF task*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, TUNE_TX, RFA_RF_GSM_FTM_MSG_ID_TUNE_TX, rfa_rf_gsm_ftm_tune_tx_type_cmd_s),

  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, TUNER_TUNE_CODE_OVERRIDE, RFA_RF_GSM_FTM_MSG_ID_TUNER_OVERRIDE, rfa_rf_gsm_ftm_tuner_override_type_cmd_s),

  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_IP2_CAL_PARAMS, RFA_RF_GSM_FTM_MSG_ID_SET_IP2_CAL_PARAMS, rfa_rf_gsm_ftm_ip2_cal_type_cmd_s),

  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, GET_IP2_CAL_PARAMS, RFA_RF_GSM_FTM_MSG_ID_GET_IP2_CAL_PARAMS, rfa_rf_gsm_ftm_get_ip2_cal_info_cmd_s),

  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, GET_IP2_CAL_RESULTS, RFA_RF_GSM_FTM_MSG_ID_GET_IP2_CAL_RESULTS, rfa_rf_gsm_ftm_get_ip2_cal_results_cmd_s),

  /*! Override the Rx Linearity mode per ARFCN on a SAWless target */
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, SET_SAWLESS_LIN_MODE, RFA_RF_GSM_FTM_MSG_ID_SET_SAWLESS_LIN_MODE, rfa_rf_gsm_ftm_set_sawless_lin_mode_cmd_s),

  /*! Report current Rx Linearity mode per ARFCN on a SAWless target */
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, CMD, GET_SAWLESS_LIN_MODE, RFA_RF_GSM_FTM_MSG_ID_GET_SAWLESS_LIN_MODE, rfa_rf_gsm_ftm_get_sawless_lin_mode_cmd_s),
  
};

/*! @brief GSM subsystem message IDs the RF task sends as a response. 
   This enum is never called by name. It just defines symbols.
*/

/*! @todo Most of these response UMIDs are placeholders for possible future use */
enum
{
  /*! Response to enter GSM FTM mode, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_MODE, RFA_RF_GSM_FTM_MSG_ID_SET_MODE, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to turn Tx on, sent by the FTM task to the RF task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_TX_ON, RFA_RF_GSM_FTM_MSG_ID_SET_TX_ON, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to turn Tx off, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_TX_OFF, RFA_RF_GSM_FTM_MSG_ID_SET_TX_OFF, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set LNA range, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_LNA_RANGE, RFA_RF_GSM_FTM_MSG_ID_SET_LNA_RANGE, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set PDM, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_PDM, RFA_RF_GSM_FTM_MSG_ID_SET_PDM, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set channel, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_BAND, RFA_RF_GSM_FTM_MSG_ID_SET_BAND, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set Tx in continuous mode, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_TRANSMIT_CONT, RFA_RF_GSM_FTM_MSG_ID_SET_TRANSMIT_CONT, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set Tx in burst mode , sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_TRANSMIT_BURST, RFA_RF_GSM_FTM_MSG_ID_SET_TRANSMIT_BURST, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set Rx to burst mode, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_RX_BURST, RFA_RF_GSM_FTM_MSG_ID_SET_RX_BURST, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to get RSSI, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, GET_RSSI, RFA_RF_GSM_FTM_MSG_ID_GET_RSSI, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set PA start delta, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_PA_START_DELTA, RFA_RF_GSM_FTM_MSG_ID_SET_PA_START_DELTA, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set PA stop delta, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_PA_STOP_DELTA, RFA_RF_GSM_FTM_MSG_ID_SET_PA_STOP_DELTA, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set PA DAC input, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_PA_DAC_INPUT, RFA_RF_GSM_FTM_MSG_ID_SET_PA_DAC_INPUT, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set Rx in continuous mode, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_RX_CONTINUOUS, RFA_RF_GSM_FTM_MSG_ID_SET_RX_CONTINUOUS, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set GSM Tx path delay, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_PATH_DELAY, RFA_RF_GSM_FTM_MSG_ID_SET_PATH_DELAY, rfa_rf_gsm_ftm_set_path_delay_type_rsp_s),

  /*! Response to set the slot_override_flag, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_SLOT_OVERRIDE_FLAG, RFA_RF_GSM_FTM_MSG_ID_SET_SLOT_OVERRIDE_FLAG, rfa_rf_gsm_ftm_set_slot_override_flag_type_rsp_s),

  /*! Response to perform Rx gain range calibration, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, RX_GAIN_RANGE_CAL, RFA_RF_GSM_FTM_MSG_ID_RX_GAIN_RANGE_CAL, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to perform Tx KVCO calibration, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, TX_KV_CAL, RFA_RF_GSM_FTM_MSG_ID_TX_KV_CAL, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to perform Tx KVCO calibration version 2 algorithm (speed optimised), sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, TX_KV_CAL_V2, RFA_RF_GSM_FTM_MSG_ID_TX_KV_CAL_V2, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set the PA gain range, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_LINEAR_PA_RANGE, RFA_RF_GSM_FTM_MSG_ID_SET_LINEAR_PA_RANGE, rfa_rf_gsm_ftm_set_pa_range_type_rsp_s),

  /*! Response to set the RGI (Radio Gain Index) value, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_LINEAR_RGI, RFA_RF_GSM_FTM_MSG_ID_SET_LINEAR_RGI, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to capture GSM Rx IQ samples, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, CAPTURE_IQ, RFA_RF_GSM_FTM_MSG_ID_CAPTURE_IQ, rfa_rf_gsm_ftm_capture_iq_type_rsp_s),

  /*! Response to adjust Rx timing, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_RX_TIMING, RFA_RF_GSM_FTM_MSG_ID_SET_RX_TIMING, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to adjust Tx timing, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_TX_TIMING, RFA_RF_GSM_FTM_MSG_ID_SET_TX_TIMING, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to perform Tx gain sweep for calibration, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_TX_GAIN_SWEEP, RFA_RF_GSM_FTM_MSG_ID_SET_TX_GAIN_SWEEP, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to perform Tx envelope DC sweep for calibration, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, TX_ENVDC_CS_SWEEP, RFA_RF_GSM_FTM_MSG_ID_TX_ENVDC_CS_SWEEP, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to perform AMAM sweep version 2 calibration, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, CFG2_AMAM_SWEEP_V2, RFA_RF_GSM_FTM_MSG_ID_CFG2_AMAM_SWEEP_V2, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set Rx multislot, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_RX_MULTISLOT, RFA_RF_GSM_FTM_MSG_ID_SET_RX_MULTISLOT, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to set Rx burst gain for the expected power level, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_RX_BURST_FOR_EXPECTED_PWR, RFA_RF_GSM_FTM_MSG_ID_SET_RX_BURST_FOR_EXPECTED_PWR, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Response to get the measured power for the previous Rx bursts, sent by the RF task to the FTM task.*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, GET_MEAS_PWR, RFA_RF_GSM_FTM_MSG_ID_GET_MEAS_PWR, rfa_rf_gsm_ftm_set_mode_type_rsp_s),

  /*! Command to set the Tx power in dBm for a specific slot index, sent from FTM task to the RF task*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_TX_POW_DBM, RFA_RF_GSM_FTM_MSG_ID_SET_TX_POW_DBM, rfa_rf_gsm_ftm_set_tx_pow_dbm_type_rsp_s),

  /*! Command to tune the RF to a specific arfcn, sent from FTM task to the RF task*/
  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, TUNE_TX, RFA_RF_GSM_FTM_MSG_ID_TUNE_TX, rfa_rf_gsm_ftm_tune_tx_type_rsp_s),


  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, TUNER_TUNE_CODE_OVERRIDE, RFA_RF_GSM_FTM_MSG_ID_TUNER_OVERRIDE, rfa_rf_gsm_ftm_tuner_override_type_rsp_s),


  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_IP2_CAL_PARAMS, RFA_RF_GSM_FTM_MSG_ID_SET_IP2_CAL_PARAMS, rfa_rf_gsm_ftm_ip2_cal_type_rsp_s),


  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, GET_IP2_CAL_PARAMS, RFA_RF_GSM_FTM_MSG_ID_GET_IP2_CAL_PARAMS, rfa_rf_gsm_ftm_get_ip2_cal_info_rsp_s),


  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, GET_IP2_CAL_RESULTS, RFA_RF_GSM_FTM_MSG_ID_GET_IP2_CAL_RESULTS, rfa_rf_gsm_ftm_get_ip2_cal_results_rsp_s),


  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, SET_SAWLESS_LIN_MODE, RFA_RF_GSM_FTM_MSG_ID_SET_SAWLESS_LIN_MODE, rfa_rf_gsm_ftm_set_sawless_lin_mode_rsp_s),


  MSGR_DEFINE_UMID(RFA, RF_GSM_FTM, RSP, GET_SAWLESS_LIN_MODE, RFA_RF_GSM_FTM_MSG_ID_GET_SAWLESS_LIN_MODE, rfa_rf_gsm_ftm_get_sawless_lin_mode_rsp_s),

};

/*===========================================================================
 
              EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef enum
{
  RFA_RF_GSM_FTM_SUCCESS=1,
  RFA_RF_GSM_FTM_STATE_MACHINE_ERROR=2,
  RFA_RF_GSM_FTM_EXECUTION_ERROR=3,
  RFA_RF_GSM_FTM_CMD_OUTCOME_MAX=4,
} rfa_rf_gsm_ftm_cmd_outcome_t;
 

typedef enum
{
  RFGSM_FTM_DISPATCH_SUCCESSFUL=1,
  RFGSM_FTM_DISPATCH_UNSUCCESSFUL=2,
  RFGSM_FTM_DISPATCH_NULL_PTR=3,
  RFGSM_FTM_DISPATCH_INVALID_PARAM =4,
  RFGSM_FTM_DISPATCH_MAX
}rfgsm_ftm_dispatch_outcome_t;

/* Structures for the commands that FTM task sends to the RF task. */

typedef struct
{
  uint16                 num_slot;
  uint16                 pa_range;
  rfgsm_modulation_type  mod_type;
}rf_gsm_ftm_pa_range_type;


typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_pa_range_type                 pa_range_params;
} rfa_rf_gsm_ftm_set_pa_range_type_cmd_s;

/*@todo The remaining cmd structures are placeholders for future use  */

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  boolean                                  success;  
}rfa_rf_gsm_ftm_generic_message_type_cmd_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  uint16                                   mode;
} rfa_rf_gsm_ftm_set_mode_type_cmd_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  boolean                                  state;
} rfa_rf_gsm_ftm_set_tx_type_cmd_s;

typedef enum
{
  RF_GSM_FTM_LNA_RANGE_0,
  RF_GSM_FTM_LNA_RANGE_1,
  RF_GSM_FTM_LNA_RANGE_2,
  RF_GSM_FTM_LNA_RANGE_3,
  RF_GSM_FTM_LNA_RANGE_4,
  RF_GSM_FTM_LNA_RANGE_5,
  RF_GSM_FTM_LNA_RANGE_MAX
} rfa_rfgsm_ftm_lna_range_type;

typedef struct
{
  msgr_hdr_struct_type           hdr;
  rfm_device_enum_type           rfm_dev;
  rfa_rfgsm_ftm_lna_range_type   lna_range_val;
} rfa_rf_gsm_ftm_set_lna_range_type_cmd_s;

typedef struct
{
    word id;
    int2 val;
} rf_gsm_ftm_pdm_id_val_type;

typedef struct
{
  msgr_hdr_struct_type        hdr;
  rfm_device_enum_type        rfm_dev;
  rf_gsm_ftm_pdm_id_val_type  pdm_params;
} rfa_rf_gsm_ftm_set_pdm_type_cmd_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfm_device_enum_type   rfm_dev;
  int8                   band;
} rfa_rf_gsm_ftm_set_band_type_cmd_s;

typedef enum
{
  RF_GSM_FTM_TX_DATA_SOURCE_PSDRND,
  RF_GSM_FTM_TX_DATA_SOURCE_TONE,
  RF_GSM_FTM_TX_DATA_SOURCE_BUFFER,
  RF_GSM_FTM_TX_DATA_SOURCE_TWOTONE
} rf_gsm_ftm_enum_tx_data_source_type;

typedef struct
{
  uint8                                slot_num;
  rf_gsm_ftm_enum_tx_data_source_type  data_source;
  uint8                                tsc_index;
  uint32                               num_bursts;
  boolean                              infinite_duration;
} rf_gsm_ftm_tx_burst_type;

typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfm_device_enum_type      rfm_dev;
  rf_gsm_ftm_tx_burst_type  tx_burst_type;
} rfa_rf_gsm_ftm_set_tx_burst_type_cmd_s;

typedef enum
{
  RF_GSM_FTM_RX_FUNCTION_GET_RSSI,
  RF_GSM_FTM_RX_FUNCTION_GET_DC_OFFSETS,
  RF_GSM_FTM_RX_FUNCTION_STOP_CONTINOUS_RX,
  RF_GSM_FTM_RX_FUNCTION_RF_RX_HW_ON,
  RF_GSM_FTM_RX_FUNCTION_RF_RX_HW_OFF
} rf_gsm_ftm_enum_rx_function_type;

typedef struct
{
  uint8                             slot_num;
  uint8                             num_frames_to_avg;
  rf_gsm_ftm_enum_rx_function_type  rx_func;
} rf_gsm_ftm_rx_burst_type;

typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfm_device_enum_type      rfm_dev;
  rf_gsm_ftm_tx_burst_type  rx_burst_type;
} rfa_rf_gsm_ftm_set_rx_burst_type_cmd_s;

/*! @todo Add FTM_GET_RSSI */

typedef struct
{
  uint16                             pa_start_delta;
  uint16                             pa_stop_delta;
} rf_gsm_ftm_set_pa_start_stop_delta_type;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_set_pa_start_stop_delta_type  pa_start_stop_delta;
} rfa_rf_gsm_ftm_set_pa_delta_type_cmd_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  int16                                    delay_value;
} rfa_rf_gsm_ftm_set_path_delay_type_cmd_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  boolean                                  override_flag;
} rfa_rf_gsm_ftm_set_slot_override_flag_type_cmd_s;

typedef struct 
{
  uint8 gain_range_bitmask; 
  uint8 num_avgs;
  boolean toggle_rx_on;
  uint32 rssi_result[8];
}rf_gsm_ftm_rxcal_params_type;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_rxcal_params_type             rx_cal_params;
} rfa_rf_gsm_ftm_rxcal_params_type_cmd_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  ftm_iqcapture_type                       iq_capture_type;
} rfa_rf_gsm_ftm_capture_iq_type_cmd_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  uint16                                   generic_a_16bit;
  uint16                                   generic_b_16bit;
} rfa_rf_gsm_ftm_generic_2x16bit_params_type_cmd_s;

typedef struct
{
  int16                                    pow_dbm;
  int8                                     index;
} rf_gsm_ftm_set_tx_pow_dbm_params_type;

typedef struct
{
  msgr_hdr_struct_type                      hdr;
  rfm_device_enum_type                      rfm_dev;
  rf_gsm_ftm_set_tx_pow_dbm_params_type     set_tx_pow_dbm_params;
} rfa_rf_gsm_ftm_set_tx_pow_dbm_type_cmd_s;

#define RF_GSM_KV_CAL_V2_MAX_BANDS 4
#define RF_GSM_KV_CAL_V2_MAX_CHANNELS 8

typedef struct
{
   uint16 num_bands;                                   /* Number of Bands */
   uint16 num_tx_cycles;                               /* Number of Tx Cycles */
   uint16 num_meas;                                    /* Number of Measurements per cycle */
   uint16 rfmode_list[RF_GSM_KV_CAL_V2_MAX_BANDS];     /* GSM RF Mode list */
   uint16 kv_chan_code[RF_GSM_KV_CAL_V2_MAX_CHANNELS]; /* KV chan in input & code in resp */
 } rf_gsm_ftm_kv_cal_v2_type;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_kv_cal_v2_type                gsm_kv_cal_v2_params;
} rfa_rf_gsm_ftm_kv_cal_v2_params_type_cmd_s;


typedef struct
{
  uint16                 rgi;
  uint16                 env_gain; 
  rfgsm_modulation_type  mod_type_is_gsm;
  uint8                  slot_num;
  boolean                set_script;
  uint16                 pa_scaling;
  uint16                 smps_bias;   
}rf_gsm_ftm_linear_rgi_type;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_linear_rgi_type               linear_rgi_params;
} rfa_rf_gsm_ftm_set_linear_rgi_type_cmd_s;

/*! @todo Add SET_RX_MULTISLOT */
typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  int16                                    power_dbm16;
  uint8                                    average_bursts;
} rfa_rf_gsm_ftm_setup_rx_burst_for_expected_pwr_type_cmd_s;


typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  int16                                    power_dbm16;
} rfa_rf_gsm_ftm_get_meas_pwr_type_cmd_s;

typedef struct
{
  msgr_hdr_struct_type                    hdr;
  rfm_device_enum_type                    rfm_dev;
  uint16                                  arfcn;
} rfa_rf_gsm_ftm_tune_tx_type_cmd_s;

typedef struct
{
  uint8 override_flag;
  void* data_ptr;
  uint8* tuner_nv_ptr; 
  uint32 error_status;
  uint8* tuner_id_ptr;
}rf_gsm_ftm_set_tuner_override_params_type;

typedef struct
{
  msgr_hdr_struct_type                      hdr;
  rfm_device_enum_type                      rfm_dev;
  rf_gsm_ftm_set_tuner_override_params_type tuner_params;
}rfa_rf_gsm_ftm_tuner_override_type_cmd_s;

typedef struct
{
  int16 start_i_code;
  int16 start_q_code;
  //uint8 num_cal_points;  // Total number of IP2 cal nodes
  uint8 unit_step_size; // distance between adjacent cal points
  uint8 width;  // Number of cal points around the start I Q code
  uint16 arfcn;
  uint8 rfcom_band;
  uint8 tx_rgi;
  uint8 lna_range;
  uint16 step_duration_qs;
  uint16 dc_settling_time_qs;
  boolean use_default_params;
}
rf_gsm_ftm_ip2_cal_params_type;



typedef struct
{
  msgr_hdr_struct_type                      hdr;
  rfm_device_enum_type                      rfm_dev;
  rf_gsm_ftm_ip2_cal_params_type            ip2_cal_params;
  uint8                                     sub_id;
}rfa_rf_gsm_ftm_ip2_cal_type_cmd_s;


typedef struct
{ 
  msgr_hdr_struct_type                      hdr;
  rfm_device_enum_type                      rfm_dev;
  uint8                                     rfcom_band;
  uint8                                     sub_id;

}rfa_rf_gsm_ftm_get_ip2_cal_info_cmd_s;

typedef struct
{
  msgr_hdr_struct_type                      hdr;
  rfm_device_enum_type                      rfm_dev;
  uint8                                     sub_id;
  uint8                                     rfcom_band;
  uint8                                     flag_mask;
  boolean                                   ip2_cal_end;  /* Indicates the end of all the expected iterations of IP2 cal */
}
rfa_rf_gsm_ftm_get_ip2_cal_results_cmd_s;


typedef struct
{
  msgr_hdr_struct_type                      hdr;
  rfm_device_enum_type                      rfm_dev;
  uint8                                     band;
  uint16                                    arfcn;
  boolean                                   use_high_lin_mode;
}
rfa_rf_gsm_ftm_set_sawless_lin_mode_cmd_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  uint8                                    band;
  uint16                                   arfcn;
}
rfa_rf_gsm_ftm_get_sawless_lin_mode_cmd_s;

  /*! @todo Add more cmd structures here */

/* The structures for the responses that RF sends back to the FTM task */

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_pa_range_type                 pa_range_params;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_set_pa_range_type_rsp_s;

/*@todo The remaining rsp structures are placeholders for future use  */

typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfm_device_enum_type          rfm_dev;
  uint16                        mode;
  rfa_rf_gsm_ftm_cmd_outcome_t  cmd_result;
} rfa_rf_gsm_ftm_set_mode_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type            hdr;
  rfm_device_enum_type            rfm_dev;
  uint8                           status;  
} rfa_rf_gsm_ftm_generic_message_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfm_device_enum_type          rfm_dev;
  boolean                       state;
  rfa_rf_gsm_ftm_cmd_outcome_t  cmd_result;
} rfa_rf_gsm_ftm_set_tx_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type            hdr;
  rfm_device_enum_type            rfm_dev;
  rfa_rfgsm_ftm_lna_range_type    lna_range_val;
  rfa_rf_gsm_ftm_cmd_outcome_t    cmd_result;
} rfa_rf_gsm_ftm_set_lna_range_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type            hdr;
  rfm_device_enum_type            rfm_dev;
  rf_gsm_ftm_pdm_id_val_type      pdm_params;
  rfa_rf_gsm_ftm_cmd_outcome_t    cmd_result;
} rfa_rf_gsm_ftm_set_pdm_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type            hdr;
  rfm_device_enum_type            rfm_dev;
  int8                            band;
  rfa_rf_gsm_ftm_cmd_outcome_t    cmd_result;
} rfa_rf_gsm_ftm_set_band_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type            hdr;
  rfm_device_enum_type            rfm_dev;
  rf_gsm_ftm_tx_burst_type        tx_burst_type;
  rfa_rf_gsm_ftm_cmd_outcome_t    cmd_result;
} rfa_rf_gsm_ftm_set_tx_burst_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type            hdr;
  rfm_device_enum_type            rfm_dev;
  rf_gsm_ftm_tx_burst_type        rx_burst_type;
  rfa_rf_gsm_ftm_cmd_outcome_t    cmd_result;
} rfa_rf_gsm_ftm_set_rx_burst_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_set_pa_start_stop_delta_type  pa_start_stop_delta;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_set_pa_delta_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  int16                                    delay_value;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_set_path_delay_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  boolean                                  override_flag;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_set_slot_override_flag_type_rsp_s;


typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_rxcal_params_type             rx_cal_params;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_rxcal_params_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_set_tx_pow_dbm_params_type    set_tx_pow_dbm_params;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
}rfa_rf_gsm_ftm_set_tx_pow_dbm_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  uint16                                   generic_a_16bit;
  uint16                                   generic_b_16bit;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_generic_2x16bit_params_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_kv_cal_v2_type                gsm_kv_cal_v2_params;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_kv_cal_v2_params_type_rsp_s;


typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rf_gsm_ftm_linear_rgi_type               linear_rgi_params;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_set_linear_rgi_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  int16                                    power_dbm16;
  uint8                                    average_bursts;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_setup_rx_burst_for_expected_pwr_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  int16                                    power_dbm16;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_get_meas_pwr_type_rsp_s;

typedef struct
{
    msgr_hdr_struct_type                    hdr;
    rfm_device_enum_type                     rfm_dev;
    uint16                                  arfcn;
    rfa_rf_gsm_ftm_cmd_outcome_t            cmd_result;
} rfa_rf_gsm_ftm_tune_tx_type_rsp_s;

typedef struct
{
    msgr_hdr_struct_type                      hdr;
    rfm_device_enum_type                      rfm_dev;
    rf_gsm_ftm_set_tuner_override_params_type tuner_params;
    rfa_rf_gsm_ftm_cmd_outcome_t              cmd_result;
}rfa_rf_gsm_ftm_tuner_override_type_rsp_s;

typedef struct
{
    msgr_hdr_struct_type                     hdr;
    rfm_device_enum_type                     rfm_dev;
    rfgsm_core_iq_capture_type*              iq_capture;
    rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_capture_iq_type_rsp_s;


typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
} rfa_rf_gsm_ftm_ip2_cal_type_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
  uint8 width;  // Number of cal points around the start I Q code
  uint8 num_frames;
  uint16 step_duration_qs;
  uint16 dc_settling_time_qs;
}rfa_rf_gsm_ftm_get_ip2_cal_info_rsp_s;


typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
  /* i code for min IM2 point */
  int16 final_i_code;

  /* q code for min IM2 point*/
  int16 final_q_code;

  /* Min IM2 value */
  int32 min_im2_val;

  /* Pass or fail status of IP2 cal. True = pass */
  boolean status;
}rfa_rf_gsm_ftm_get_ip2_cal_results_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
}rfa_rf_gsm_ftm_set_sawless_lin_mode_rsp_s;

typedef struct
{
  msgr_hdr_struct_type                     hdr;
  rfm_device_enum_type                     rfm_dev;
  rfa_rf_gsm_ftm_cmd_outcome_t             cmd_result;
  uint8                                    band;
  uint16                                   arfcn;
  boolean                                  is_in_high_lin;
}rfa_rf_gsm_ftm_get_sawless_lin_mode_rsp_s;

  /*! @todo Add more response structures here */

typedef union
{
  rfa_rf_gsm_ftm_set_pa_range_type_cmd_s                           rfa_rf_gsm_ftm_set_pa_range_cmd;

  /*! @todo The remaining cmd structures are placeholders for future use  */
  rfa_rf_gsm_ftm_generic_message_type_cmd_s                        rfa_rf_gsm_ftm_generic_message_cmd;
  rfa_rf_gsm_ftm_set_mode_type_cmd_s                               rfa_rf_gsm_ftm_set_mode_cmd;
  rfa_rf_gsm_ftm_set_tx_type_cmd_s                                 rfa_rf_gsm_ftm_set_tx_cmd;
  rfa_rf_gsm_ftm_set_lna_range_type_cmd_s                          rfa_rf_gsm_ftm_set_lna_range_cmd;
  rfa_rf_gsm_ftm_set_pdm_type_cmd_s                                rfa_rf_gsm_ftm_set_pdm_cmd;
  rfa_rf_gsm_ftm_set_band_type_cmd_s                               rfa_rf_gsm_ftm_set_band_cmd;
  rfa_rf_gsm_ftm_set_tx_burst_type_cmd_s                           rfa_rf_gsm_ftm_set_tx_burst_cmd;
  rfa_rf_gsm_ftm_set_rx_burst_type_cmd_s                           rfa_rf_gsm_ftm_set_rx_burst_cmd;
  rfa_rf_gsm_ftm_set_pa_delta_type_cmd_s                           rfa_rf_gsm_ftm_set_pa_start_stop_delta_cmd;
  rfa_rf_gsm_ftm_set_path_delay_type_cmd_s                         rfa_rf_gsm_ftm_set_path_delay_cmd;
  rfa_rf_gsm_ftm_set_slot_override_flag_type_cmd_s                 rfa_rf_gsm_ftm_set_slot_override_flag_cmd;
  rfa_rf_gsm_ftm_rxcal_params_type_cmd_s                           rfa_rf_gsm_ftm_rxcal_cmd;
  rfa_rf_gsm_ftm_generic_2x16bit_params_type_cmd_s                 rfa_rf_gsm_ftm_2x16bit_cmd;
  rfa_rf_gsm_ftm_kv_cal_v2_params_type_cmd_s                       rfa_rf_gsm_ftm_kv_cal_v2_cmd;
  rfa_rf_gsm_ftm_set_linear_rgi_type_cmd_s                         rfa_rf_gsm_ftm_set_linear_rgi_cmd;
  rfa_rf_gsm_ftm_setup_rx_burst_for_expected_pwr_type_cmd_s        rfa_rf_gsm_ftm_setup_rx_burst_for_expected_pwr_cmd;
  rfa_rf_gsm_ftm_get_meas_pwr_type_cmd_s                           rfa_rf_gsm_ftm_get_meas_pwr_cmd;
  rfa_rf_gsm_ftm_set_tx_pow_dbm_type_cmd_s                         rfa_rf_gsm_ftm_set_tx_pow_dbm_cmd;
  rfa_rf_gsm_ftm_tune_tx_type_cmd_s                                rfa_rf_gsm_ftm_tune_tx_cmd;
  rfa_rf_gsm_ftm_tuner_override_type_cmd_s                         rfa_rf_gsm_ftm_tuner_override_cmd;
  rfa_rf_gsm_ftm_capture_iq_type_cmd_s                       	   rfa_rf_gsm_ftm_capture_iq_cmd;
  rfa_rf_gsm_ftm_ip2_cal_type_cmd_s                                rfa_rf_gsm_ftm_update_ip2_cal_params_cmd;
  rfa_rf_gsm_ftm_set_sawless_lin_mode_cmd_s                        rfa_rf_gsm_ftm_set_sawless_lin_mode_cmd;
  rfa_rf_gsm_ftm_get_sawless_lin_mode_cmd_s                        rfa_rf_gsm_ftm_get_sawless_lin_mode_cmd;
  /*! @todo Add more cmd structures here */

  rfa_rf_gsm_ftm_set_pa_range_type_rsp_s                           rfa_rf_gsm_ftm_set_pa_range_rsp;

  /*! @todo The remaining rsp structures are placeholders for future use  */
  rfa_rf_gsm_ftm_generic_message_type_rsp_s                        rfa_rf_gsm_ftm_generic_message_rsp;
  rfa_rf_gsm_ftm_set_mode_type_rsp_s                               rfa_rf_gsm_ftm_set_mode_rsp;
  rfa_rf_gsm_ftm_set_tx_type_rsp_s                                 rfa_rf_gsm_ftm_set_tx_rsp;
  rfa_rf_gsm_ftm_set_lna_range_type_rsp_s                          rfa_rf_gsm_ftm_set_lna_range_rsp;
  rfa_rf_gsm_ftm_set_pdm_type_rsp_s                                rfa_rf_gsm_ftm_set_pdm_rsp;
  rfa_rf_gsm_ftm_set_band_type_rsp_s                               rfa_rf_gsm_ftm_set_band_rsp;
  rfa_rf_gsm_ftm_set_tx_burst_type_rsp_s                           rfa_rf_gsm_ftm_set_tx_burst_rsp;
  rfa_rf_gsm_ftm_set_rx_burst_type_rsp_s                           rfa_rf_gsm_ftm_set_rx_burst_rsp;
  rfa_rf_gsm_ftm_set_pa_delta_type_rsp_s                           rfa_rf_gsm_ftm_set_pa_start_stop_rsp;
  rfa_rf_gsm_ftm_set_path_delay_type_rsp_s                         rfa_rf_gsm_ftm_set_path_delay_rsp;
  rfa_rf_gsm_ftm_set_slot_override_flag_type_rsp_s                 rfa_rf_gsm_ftm_set_slot_override_flag_rsp;
  rfa_rf_gsm_ftm_rxcal_params_type_rsp_s                           rfa_rf_gsm_ftm_rxcal_rsp;
  rfa_rf_gsm_ftm_generic_2x16bit_params_type_rsp_s                 rfa_rf_gsm_ftm_2x16bit_rsp;
  rfa_rf_gsm_ftm_kv_cal_v2_params_type_rsp_s                       rfa_rf_gsm_ftm_kv_cal_v2_rsp;
  rfa_rf_gsm_ftm_set_linear_rgi_type_rsp_s                         rfa_rf_gsm_ftm_set_linear_rgi_rsp;
  rfa_rf_gsm_ftm_setup_rx_burst_for_expected_pwr_type_rsp_s        rfa_rf_gsm_ftm_setup_rx_burst_for_expected_pwr_rsp;
  rfa_rf_gsm_ftm_get_meas_pwr_type_rsp_s                           rfa_rf_gsm_ftm_get_meas_pwr_rsp;
  rfa_rf_gsm_ftm_set_tx_pow_dbm_type_rsp_s                         rfa_rf_gsm_ftm_set_tx_pow_dbm_rsp; 
  rfa_rf_gsm_ftm_tune_tx_type_rsp_s                                rfa_rf_gsm_ftm_tune_tx_rsp;
  rfa_rf_gsm_ftm_tuner_override_type_rsp_s                         rfa_rf_gsm_ftm_tuner_override_rsp;
  rfa_rf_gsm_ftm_capture_iq_type_rsp_s                             rfa_rf_gsm_ftm_capture_iq_rsp;
  rfa_rf_gsm_ftm_ip2_cal_type_rsp_s                                rfa_rf_gsm_ftm_update_ip2_cal_scripts_rsp;
  rfa_rf_gsm_ftm_get_ip2_cal_info_rsp_s                            rfa_rf_gsm_get_ip2_cal_params_rsp;
  rfa_rf_gsm_ftm_get_ip2_cal_results_rsp_s                         rfa_rf_gsm_get_ip2_cal_results_rsp;
  rfa_rf_gsm_ftm_set_sawless_lin_mode_rsp_s                        rfa_rf_gsm_ftm_set_sawless_lin_mode_rsp;
  rfa_rf_gsm_ftm_get_sawless_lin_mode_rsp_s                        rfa_rf_gsm_ftm_get_sawless_lin_mode_rsp;

  /*! @todo Add more rsp structures here */

} rfgsm_ftm_msg_u;

#endif /* RFGSM_FTM_MSG_H */



