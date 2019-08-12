/*!
  @file
  tfw_diag_log_codes.h

  @brief
  This file contains tfw diag log code definitions.

*/

/*===========================================================================

  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_diag_log_codes.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
===========================================================================*/

#ifndef __TFW_DIAG_LOG_CODES_H
#define __TFW_DIAG_LOG_CODES_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include "log_codes.h"
/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/



/********************* START OF LOG CODE DEFINES **************************/
/*! @brief TFW log code base 
    LOG_TDSCDMA_BASE_C = 0xD000 
    LOG_TFW_BASE_C = 0xD000 + 0x100 = 0xD100
*/
#define TFW_DIAG_LOG_BASE_C                             (0x100 + LOG_TDSCDMA_BASE_C)

/*! @brief Receiver front end (AGC as well as other blocks)
*/
#define TFW_DIAG_LOG_TRACKING_MODE_C                    (0 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_AGC_POWER_SCAN_C                   (1 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_AGC_GAP_DETECTION_C                (2 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_AGC_ACQ_STEP12_C                   (3 + TFW_DIAG_LOG_BASE_C)
 
/*! @brief Searcher and measurement
*/
#define TFW_DIAG_LOG_REQCQ_RESULTS_C                    (4 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_DL_SYNC_CODE_RESULTS_C             (5 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_TS0_MIDAMBLE_DETECTION_RESULTS_C   (6 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_TS_RSP_MEASUREMENT_RESULTS_C       (7 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_TRACKING_LOOP_RESULTS_C            (8 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_STEP3_RESULTS_C                    (9 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_SERVING_CELL_FRAME_TIMING_C        (0xA + TFW_DIAG_LOG_BASE_C)
 
/*! @brief Demodulator_front_end_packets
*/
#define TFW_DIAG_LOG_DEMFRONT_CFG_C                     (0xB + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_DEMFRONT_CELL_CFG_C                (0xC + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_CHANNEL_ESTIMATION_SHORT_C         (0xD + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_EQUALIZER_WEIGHT_COMPUTATION_C     (0xE + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_AE_RESULTS_C                       (0xF + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_SPECIAL_BURST_DETECTION_RESULTS_C  (0x10 + TFW_DIAG_LOG_BASE_C)
 
/*! @brief Demodulator_back_end
*/
#define TFW_DIAG_LOG_TPC_SS_DECODING_RESULTS_C          (0x11 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_TFCI_DECODING_RESULTS_C            (0x12 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_EHICH_DECODING_RESULTS_C           (0x13 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_EAGCH_DECODING_RESULTS_C           (0x14 + TFW_DIAG_LOG_BASE_C)
 
/*! @brief Uplink
*/
#define TFW_DIAG_LOG_UL_DPCH_ENCODING_RESULTS_C         (0x15 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_PRACH_ENCODING_RESULTS_C           (0x16 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_UPPCH_ENCODING_RESULTS_C           (0x17 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_HS_SICH_ENCODING_RESULTS_C         (0x18 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_EPUCH_ENCODING_RESULTS_C           (0x19 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_ERUCCH_ENCODING_RESULTS_C          (0x1A + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_UPLINK_REPORT_C                    (0x1B + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_TPC_REPORT_C                       (0x1C + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_UPLINK_TIMING_C                    (0x1D + TFW_DIAG_LOG_BASE_C)

/*! @brief new packets*/
#define TFW_DIAG_LOG_CHANNEL_ESTIMATION_LONG_C          (0x1E + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_NONHS_SIR_C                        (0x1F + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_MEMORY_CHUNK_C                     (0x20 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_CQI_C                              (0x21 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_MSG_C                              (0x22 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_DYNAMIC_SWITCH_C                   (0x23 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_HSSCCH_DECODING_RESULTS_C          (0x24 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_IRAT_T2X_C                         (0x25 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_TIMELINE_PROFILE_C                 (0x26 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_CE_DEMFRONT_PROFILE_C              (0x27 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_IRAT_X2T_C                         (0x28 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_ENHANCE_NONTS0_PRUNING_RESULTS_C   (0x29 + TFW_DIAG_LOG_BASE_C)

/*! @brief DSDx Related log packets*/
#define TFW_DIAG_LOG_DSDX_CXM_SLOT_STATUS_C             (0x2A + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_MULTI_SIM_CFG_MSG_C                (0x2B + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_DSDS_TG_QTA_GAP_C                  (0x2C + TFW_DIAG_LOG_BASE_C)

#define TFW_DIAG_LOG_RX_AGC_C                           (0x2D + TFW_DIAG_LOG_BASE_C)

#define TFW_DIAG_LOG_RF_SLOT_UPDATE_C                   (0x2E + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_RF_EVENTS_C                        (0x2F + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_EXCEPTIONAL_EVENTS_C               (0x30 + TFW_DIAG_LOG_BASE_C)
/*! @brief Dime vpe information used log packet*/
#define TFW_DIAG_LOG_VPE_INFORMATION_C             (0x31 + TFW_DIAG_LOG_BASE_C)

#define TFW_DIAG_LOG_DEBUG_C                            (0x33 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_COMMAND_C                          (0x34 + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_RESPONSE_C                         (0x35 + TFW_DIAG_LOG_BASE_C)
/*! @brief HS Result log packet*/
#define TFW_DIAG_LOG_HS_RESULT_C                        (0x38 + TFW_DIAG_LOG_BASE_C)

/*! @brief DSDA power scan log packets*/
#define TFW_DIAG_LOG_DSDA_TG_POWERSCAN_C                (0x39 + TFW_DIAG_LOG_BASE_C)

#define TFW_DIAG_LOG_FBRX_C                             (0x3A + TFW_DIAG_LOG_BASE_C)
#define TFW_DIAG_LOG_DSDA_ACQ_IRAT_C                    (0x3B + TFW_DIAG_LOG_BASE_C)

#endif /* __TFW_DIAG_LOG_CODES_H */

