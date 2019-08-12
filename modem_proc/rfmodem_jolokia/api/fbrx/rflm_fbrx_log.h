
/*!
  @file
  rflm_fbrx_log.h

  @brief
  RF LOG API definitions for FBRX to interact with SW and LM
  
  @detail
  This file will contain all definitios and declarations to be
  shared with SW and LM for FBRX LM Logging
 
*/

/*==============================================================================

  Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/fbrx/rflm_fbrx_log.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/16/15   gh      Added support for flush log buffer for fbrx_log layer
03/12/15   gh      Fix FBRx logging issue
07/24/14   aa     [1] Added FBRX results log packet 0x1849 v3 revision changes
                  [2] Added support for new sample capture log packet 0x184A
06/06/14   aa      Added new log version 2 
04/14/14   aa      Update log structure for version control
04/03/14   ka      Update log structure
03/18/14   aa      Update exp gain data results into buffer
03/12/14   aa      Latest FBRx changes for ILPC correction
03/04/14   aa      Initial Version - Support for FBRX Logging


==============================================================================*/

#ifndef RFLM_FBRX_LOG_H
#define RFLM_FBRX_LOG_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "rflm.h"
#include "rflm_api_fbrx.h"
#include "rflm_fbrx.h"
#include "rflm_diag_log.h"
#include "log.h"

/*==============================================================================

                EXTERNAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/************************************************************/
/*                LOG PACKET IDENTIFICATION                 */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! @brief RFLM FBRx log packet ID */
#define RFLM_LOG_PACKET_FBRX_RESULTS_ID                 0x1849 
#define RFLM_LOG_PACKET_FBRX_CAPTURE_ID                 0x184A 

/*----------------------------------------------------------------------------*/
/*! @brief RFLM FBRx user ID */
#define RFLM_LOG_PACKET_RAT_FBRX_RESULTS                0
#define RFLM_LOG_PACKET_RAT_FBRX_CAPTURE                1

/*----------------------------------------------------------------------------*/
/*! @brief RFLM FBRx tag size */
#define RFLM_LOG_PACKET_FBRX_RESULTS_TAG_SIZE_WORDS     4 /* Should be multiple of 4 */
#define RFLM_LOG_PACKET_FBRX_CAPTURE_TAG_SIZE_WORDS     4 /* Should be multiple of 4 */

/*----------------------------------------------------------------------------*/

#define RFLM_FBRX_MAX_TX_SAMPLE_SIZE 391
#define RFLM_FBRX_MAX_RX_SAMPLE_SIZE 391

/************************************************************/
/*                   LOG PACKET VERSIONS                    */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*!@brief Enumeration indicating the FBRx Log Packet version */
typedef enum
{
  RFLM_FBRX_LOG_VER_1 = 1, /*!< FBRx Log Packet Version 1 */
  RFLM_FBRX_LOG_VER_2 = 2, /*!< FBRx Log Packet Version 2 */
  RFLM_FBRX_LOG_VER_3 = 3, /*!< FBRx Log Packet Version 3 */
  RFLM_FBRX_LOG_VER_4 = 4,
  RFLM_FBRX_LOG_VER_5 = 5  /*!< For DimePM */
} rflm_fbrx_log_version_type;

/*----------------------------------------------------------------------------*/

/*==============================================================================

              EXTERNAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/


/*==============================================================================

            EXTERNAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/************************************************************/
/*              LOG PACKET PAYLOAD DEFINITION               */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! @brief  FBRX Log packet structure Version 1 */

typedef struct
{
  uint8  rflm_tech;         /*!< rflm tech id ( 0=1X, 1=HDR, 2=LTE, 3=WCDMA, 4=TDSCDMA, 5=GSM ) */
  uint8  bandwidth;         /*!< rfcommon_fbrx_tx_cfg_type tech bandwidth */
  uint8  fbrx_mode;         /*!< flag indicating the fbrx mode (0,1,2 or 3) */
  uint8  gain_state;        /*!< flag indicating the fbrx gain state (0,1,2 or 3) */
  uint8  slot_idx;          /*!< flag indicating the slot index (0,1, or 2) per SF for current fbrx run */ 
  uint8  pa_idx;            /*!< flag indicating the PA State */ 
  uint8  rgi_idx;           /*!< flag indicating the RGI row index */ 
  int16  temp_comp;         /*!< Temp Comp value for current tech/band/chan/mode/gs */

  int16  exp_cal_gain;      /*!< FBRx Expected Cal Gain value */
  uint8  ubias_new;         /*!< Updated ubias value for current tech/band/chan/mode/gs */
  uint16 rxscale_new;       /*!< Updated Rx Scale value for current tech/band/chan/mode/gs */
  int16  gain_est;          /*!< Current Gain estimate result */
  int16  next_gain;         /*!< Next Gain estimate result */

  int16  last_txagc;        /*!< Current TxAGC value */
  int16  delta_prx;         /*!< Updated Tx power in dB10 for current tech/band/chan/mode/gs */
  int16  gain_error_db10;   /*!< The FBRx Gain Error in dB10 uint for current txagc */
  int16  filt_error_db10;   /*!< The FBRx Filtered Gain Error in dB10 uint for current txagc */

  uint32 cgain_est;         /*!< complex gain estimate result */
  uint32 ls_err;            /*!< complex least squared error estimate result */
  int32  model_a;           /*!< model A param estimate result */
  int32  model_b;           /*!< model B param estimate result */
  int32  model_c;           /*!< model C param estimate result */

  int32  loft;              /*!< Current LOFT compensation */
  int32  next_loft;         /*!< Next LOFT compensation estimate */
  int32  rsb;               /*!< Current Residual Side Band estimate */
  int32  next_rsb;          /*!< Next Residual Side Band estimate */
  int32  gain_imbalance;    /*!< Current Gain imbalance estimate */
  int32  phase_imbalance;   /*!< Current Phase imbalance estimate */
  uint16 xcorr_log[RFLM_FBRX_XCORR_LOG_SIZE];  /*!< Cross correlation results */

}rflm_fbrx_log_packet_v1;

/* Sort the data into 32bits field */
typedef struct
{
  uint8  rflm_tech;         /*!< rflm tech id ( 0=1X, 1=HDR, 2=LTE, 3=WCDMA, 4=TDSCDMA, 5=GSM ) */
  uint8  bandwidth;         /*!< rfcommon_fbrx_tx_cfg_type tech bandwidth */
  uint8  ftm_cal_mode;      /*!< flag indicating the RF Cal mode (0 =FALSE, 1=TRUE) */ 
  uint8  fbrx_mode;         /*!< flag indicating the fbrx mode (0,1,2 or 3) */

  uint8  gain_state;        /*!< flag indicating the fbrx gain state (0,1,2 or 3) */
  uint8  slot_idx;          /*!< flag indicating the slot index (0,1, or 2) per SF for current fbrx run */ 
  uint8  pa_idx;            /*!< flag indicating the PA State */ 
  uint8  rgi_idx;           /*!< flag indicating the RGI row index */ 

  int16  temp_comp;         /*!< Temp Comp value for current tech/band/chan/mode/gs */
  int16  exp_cal_gain;      /*!< FBRx Expected Cal Gain value */

  uint16 ubias_new;         /*!< Updated ubias value for current tech/band/chan/mode/gs */
  uint16 rxscale_new;       /*!< Updated Rx Scale value for current tech/band/chan/mode/gs */

  int16  gain_est;          /*!< Current Gain estimate result */
  int16  next_gain;         /*!< Next Gain estimate result */

  int16  txagc;             /*!< Current TxAGC value */
  int16  delta_prx;         /*!< Updated Tx power in dB10 for current tech/band/chan/mode/gs */

  int16  gain_error_db10;   /*!< The FBRx Gain Error in dB10 uint for current txagc */
  int16  filt_error_db10;   /*!< The FBRx Filtered Gain Error in dB10 uint for current txagc */

  uint32 cgain_est;         /*!< complex gain estimate result */

  uint32 ls_err;            /*!< complex least squared error estimate result */

  int32  model_a;           /*!< model A param estimate result */

  int32  model_b;           /*!< model B param estimate result */

  int32  model_c;           /*!< model C param estimate result */

  int32 loft;               /*!< Current LOFT compensation */

  int32 next_loft;          /*!< Next LOFT compensation estimate */

  int32 rsb;                /*!< Current Residual Side Band estimate */

  int32 next_rsb;           /*!< Next Residual Side Band estimate */

  int32 gain_imbalance;     /*!< Current Gain imbalance estimate */

  int32 phase_imbalance;    /*!< Current Phase imbalance estimate */

  uint16 xcorr_log[RFLM_FBRX_XCORR_LOG_SIZE];  /*!< Cross correlation results */

}rflm_fbrx_log_packet_v2;


/* Sort the data into 32bits field */
typedef struct
{
  int16  txagc;              /*!< Current TxAGC value in dB10 */
  int16  gain_est;           /*!< Current Gain estimate result */

  int16  gain_error_db10;    /*!< The FBRx Gain Error in dB10 uint for current txagc */
  int16  filt_error_db10;    /*!< The FBRx Filtered Gain Error in dB10 uint for current txagc */

  uint32 ls_err;             /*!< complex least squared error estimate result */

  int16  next_gain;          /*!< BB IQ gain or Env Scale from uK */
  uint16 current_gain;       /*!< BB IQ gain or Env Scale from tech TXAGC */

  uint16 threshold_gain;     /*!< Nominal BB IQ gain or Envelope scale value */
  int16  cal_pwr;            /*!< FBRx Cal Power for current gain state */
  int16  exp_cal_gain;       /*!< FBRx Expected Cal Gain value */
  int16  delta_prx;          /*!< The delta in Tx power in dB10 units for the current TxAGC from the FBRX Cal point or power for current gain state */  

  int16  temp_comp;          /*!< Temp Comp value for current tech/band/chan/mode/gs */
  uint16 ubias_new;          /*!< Updated ubias value for current tech/band/chan/mode/gs */
  uint16 rxscale_new;        /*!< Updated Rx Scale value for current tech/band/chan/mode/gs */
  uint16 txscale_new;        /*!< Updated Tx Scale value for current tech/band/chan/mode/gs */

  uint8  rflm_tech;          /*!< rflm tech id ( 0=1X, 1=HDR, 2=LTE, 3=WCDMA, 4=TDSCDMA, 5=GSM ) */
  uint8  bandwidth;          /*!< rfcommon_fbrx_tx_cfg_type tech bandwidth */
  uint8  fbrx_mode;          /*!< flag indicating the fbrx mode (0,1,2 or 3) */
  uint8  gain_state;         /*!< flag indicating the fbrx gain state (0,1,2 or 3) */

  uint8  slot_idx;           /*!< flag indicating the slot index (0,1, or 2) per SF for current fbrx run */ 
  uint8  pa_idx;             /*!< flag indicating the PA State */ 
  uint8  rgi_idx;            /*!< flag indicating the RGI row index */ 
  uint8  es_iq_sel;          /*!< flag indicating if IQ gain or Envelope scale */ 

  uint8  ftm_cal_mode;       /*!< flag indicating the RF Cal mode (0 =FALSE, 1=TRUE) */ 
  uint8  fbrx_band;          /*!< flag indicating the current Band of type (rfcom_band_type_u) */ 
  uint8  nv_control;         /*!< flag indicating the FBRX SYS common NV control value for current tech */ 
  uint8  fw_abort_status;    /*!< flag indicating if FW aborted FBRX due to timeline limitation */

  uint16 fbrx_skip_cnt;      /*!< Number of valid FBRX skips from bootup */
  uint16 bad_capture_idx;    /*!< flag indicating the bad sample capture index */ 

  uint32 last_ldo_time;      /*!< Time at which the FBRx LDO was last turned on */
  uint32 fbrx_start_time;    /*!< Time at which CMN FW starts the FBRx */
  uint32 fbrx_end_time;      /*!< Time at which CMN FW stops FBRX */

  uint32 cgain_est;          /*!< complex gain estimate result */
  int32  model_a;            /*!< model A param estimate result */
  int32  model_b;            /*!< model B param estimate result */
  int32  model_c;            /*!< model C param estimate result */
  int32  loft;               /*!< Current LOFT compensation */
  int32  next_loft;          /*!< Next LOFT compensation estimate */
  int32  rsb;                /*!< Current Residual Side Band estimate */
  int32  next_rsb;           /*!< Next Residual Side Band estimate */
  int32  gain_imbalance;     /*!< Current Gain imbalance estimate */
  int32  phase_imbalance;    /*!< Current Phase imbalance estimate */

  uint16 xcorr_log[RFLM_FBRX_XCORR_LOG_SIZE];  /*!< Cross correlation results */

}rflm_fbrx_log_packet_v3;

typedef rflm_fbrx_ilpc_log_t rflm_fbrx_log_packet_v5;

/*----------------------------------------------------------------------------*/

/*!  @brief FBRx log packet structure */
/// @log_id 0x1849
typedef struct
{
  uint8  log_version;           /*!< Log Packet Version */
  uint8  num_sub_packets;       /*!< Number of Sub-Packets in each log packet */   
  int16  reserved;              /*!< Reserved 16 bit field */

  /// @descriminator log_version
  union  rflm_fbrx_log_packet_versions
  {
    /// @condition 1
    rflm_fbrx_log_packet_v1 v1;   /*!< Log Packet Version 1 */
    /// @condition 2
    rflm_fbrx_log_packet_v2 v2;   /*!< Log Packet Version 2 */
    /// @condition 3
    rflm_fbrx_log_packet_v3 v3;   /*!< Log Packet Version 3 */
    /// @condition 5
    rflm_fbrx_log_packet_v5 v5;   /*!< Log Packet Version 5 */
  } versions;

}rflm_fbrx_log_packet_t;

/*----------------------------------------------------------------------------*/

/************************************************************/
/*              SAMPLE CAPTURE PACKET PAYLOAD DEFINITION               */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! @brief  FBRX sample_capture_packet packet structure Version 1 */

typedef struct
{
  uint32 bad_capture_idx;    /*!< flag indicating the bad sample capture index */ 
  uint32 tx_samples[RFLM_FBRX_MAX_TX_SAMPLE_SIZE];  /*!< TX Samples */
  uint32 rx_samples[RFLM_FBRX_MAX_RX_SAMPLE_SIZE];  /*!< RX samples */

}rflm_fbrx_sample_capture_packet_v1;

/*!  @brief FBRx Sample Captures structure */
/// @log_id 0x184A
typedef struct
{
  uint8  log_version;           /*!< Log Packet Version */
  uint8  num_sub_packets;       /*!< Number of Sub-Packets in each log packet */   
  int16  reserved;              /*!< Reserved 16 bit field */

  /// @descriminator log_version
  union  rflm_fbrx_sample_captures_versions
  {
    /// @condition 1
    rflm_fbrx_sample_capture_packet_v1 v1;   /*!< FBRX Sample Capture Packet Version 1 */

  } versions;

}rflm_fbrx_sample_captures_t;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
              API input/output function definitions -- FBRX 
==============================================================================*/

/************************************************************/
/*           DIAG LOG SUBPACKET API DEFINITIONS             */
/************************************************************/

/*----------------------------------------------------------------------------*/

void
rflm_fbrx_results_diag_log_subpacket
(
  rflm_fbrx_dm_template_t* fbrx_data_ptr, 
  uint8 run_idx 
);

void
rflm_fbrx_captures_diag_log_subpacket
(
  rflm_fbrx_dm_template_t* fbrx_data_ptr, 
  uint8 run_idx 
);

void
rflm_fbrx_flush_diag_log_subpacket
(
 rflm_fbrx_dm_template_t* fbrx_data_ptr,
 uint16 log_mask
);


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_FBRX_LOG_H */
