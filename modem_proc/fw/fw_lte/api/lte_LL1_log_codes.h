/*!
  @file
  lte_LL1_log_codes.h

  @brief
  This file contains LTE LL1 log code definitions.

*/

/*=============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_codes.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
=============================================================================*/

#ifndef LTE_LL1_LOG_CODES_H
#define LTE_LL1_LOG_CODES_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <log_codes.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/



/********************* START OF ML1 LOG CODE DEFINES **************************/
/*! @brief LTE LL1 log code base 
    LOG_LTE_BASE_C = 0xB010 
    LOG_LTE_LL1_BASE_C = 0xB010 + 0x100 = 0xB110
*/
#define LOG_LTE_LL1_BASE_C                      (0x100 + LOG_LTE_BASE_C)

// ----------------------------------------------------------------------------
// Req msgr msg logging for the fwsim
// ----------------------------------------------------------------------------
/*! @brief LTE_LL1_REQ_MSG_LOG log code */
#define LOG_LTE_LL1_REQ_MSG_LOG_C               (0x0 + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// Receiver Front End
// ----------------------------------------------------------------------------
/*! @brief LTE LL1 rx-FE AGC log code */
#define LOG_LTE_LL1_RXFE_AGC_LOG_C              (0x1 + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_RXFE_AGC_INT_LOG_C          (0x2 + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// Searcher and Measurements
// ----------------------------------------------------------------------------
/*! @brief LTE LL1 search PSS result log code */
#define LOG_LTE_LL1_SRCH_PSS_RESULT_LOG_C       (0x3 + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_SRCH_PSS_RESULT_INT_LOG_C   (0x4 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 search SSS result log code */
#define LOG_LTE_LL1_SRCH_SSS_RESULT_LOG_C       (0x5 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 serving cell detection result log code */
#define LOG_LTE_LL1_SRCH_SCELL_RESULT_LOG_C     (0x7 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 neighbor cell detection result log code */
#define LOG_LTE_LL1_SRCH_NCELL_RESULT_LOG_C     (0x8 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 serving cell frame timing log code */
#define LOG_LTE_LL1_SCELL_FRAME_TIMING_LOG_C    (0x4 + LOG_LTE_LL1_BASE_C)  //not used!

/*! @brief LTE LL1 serving cell measurements results log code */
#define LOG_LTE_LL1_SRCH_SERVING_CELL_MEASUREMENT_RESULT_INT_LOG_C (0x06 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 neighbor cell measurements & tracking results log code */
#define LOG_LTE_LL1_SRCH_NCELL_MEASUREMENT_AND_TRACKING_INT_LOG_C  (0x09 + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// PRS
// ----------------------------------------------------------------------------
/*! @brief LTE LL1 PRS log code */
#define LOG_LTE_LL1_PRS_FW_LOG_C       (0x0A + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 serving cell frequency tracking results log code */
#define LOG_LTE_LL1_SRCH_SERVING_CELL_FTL_RESULT_INT_LOG_C  (0x0B + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 serving cell time tracking results log code */
#define LOG_LTE_LL1_SRCH_SERVING_CELL_TTL_RESULT_INT_LOG_C  (0x0D + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// LTE profiling log packet 0xB11C
// ----------------------------------------------------------------------------
/*! @brief LTE LL1 unformated profiling log packet */
#define LOG_LTE_LL1_PROF_LOG_C    (0x0C + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// Debug log packet 0xB11E
// ----------------------------------------------------------------------------
/*! @brief LTE LL1 unformated recovery log packet */
#define LOG_LTE_LL1_RECOV_LOG_C    (0x0E + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// Debug log packet 0xB11F
// ----------------------------------------------------------------------------
/*! @brief LTE LL1 unformated debug packet */
#define LOG_LTE_LL1_DBG_LOG_C    (0x0F + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// Demodulator front end
// ----------------------------------------------------------------------------
/*! @brief LTE LL1 Antenna Correlation results log code */
#define LOG_LTE_LL1_ANT_CORR_RESULT_LOG_C (0x10 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 Demfront Serving Cell CER log code */
#define LOG_LTE_LL1_DEMFRONT_SERVING_CELL_CER_LOG_C (0x12 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 Demfront Neighbor Cell CER log code */
#define LOG_LTE_LL1_DEMFRONT_NEIGHBOR_CELL_CER_LOG_C (0x13 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 MBSFN whitened chest log code, similar to LOG_LTE_LL1_CSF_WHITENED_MATRICES_LOG_C */
#define LOG_LTE_LL1_MBSFN_WHITENED_MATRICES_LOG_C    (0x15 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 Demfront PDSCH Demapper Configuration log code */
#define LOG_LTE_LL1_DEMFRONT_PDSCH_DEMAPPER_CONFIGURATION_LOG_C (0x16 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 MBSFN eMBMS CER log code */
#define LOG_LTE_LL1_MBSFN_EMBMS_CER_LOG_C (0x17 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 RLM results log code */
#define LOG_LTE_LL1_RLM_RESULT_INT_LOG_C          (0x19 + LOG_LTE_LL1_BASE_C)


/*! @brief LTE LL1 Demfront Serving Cell RS log code */
#define LOG_LTE_LL1_DEMFRONT_SERVING_CELL_RS_LOG_C (0x24 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 Serving Cell COM loop log code */
#define LOG_LTE_LL1_SERVING_CELL_COM_LOOP_LOG_C (0x11 + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// Demodulator back end
// ----------------------------------------------------------------------------

/*! @brief LTE LL1 pdcch/pdsch log codes */
#define LOG_LTE_LL1_PCFICH_DECODE_RESULT_LOG_C    (0x1A + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_VPE_SCHEDULER_LOG_C           (0x1B + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_PHICH_DECODE_RESULT_LOG_C     (0x1C + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_PBCH_DECODE_RESULT_LOG_C      (0x1E + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_PDCCH_DECODE_RESULT_LOG_C     (0x20 + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_PDSCH_DECODE_RESULT_LOG_C     (0x22 + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_PDSCH_HARQ_OFFLOAD_LOG_C      (0x14 + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_PDSCH_DEBUG_LOG_C             (0x1F + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// Uplink
// ----------------------------------------------------------------------------

/*! @brief LTE LL1 PUSCH Tx report log code */
#define LOG_LTE_LL1_UL_PUSCH_TX_REPORT_LOG_C      (0x29 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 PUCCH Tx report log code */
#define LOG_LTE_LL1_UL_PUCCH_TX_REPORT_LOG_C      (0x2C + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 SRS Tx report log code */
#define LOG_LTE_LL1_UL_SRS_TX_REPORT_LOG_C        (0x30 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 CxM log packets */
#define LOG_LTE_LL1_CXM_CONFIG_LOG_C              (0x31 + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_CXM_STATUS_LOG_C              (0x32 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 PRACH Tx report log code */
#define LOG_LTE_LL1_UL_PRACH_TX_REPORT_LOG_C      (0x34 + LOG_LTE_LL1_BASE_C)

/*! @brief LTE LL1 PRACH Tx report log code */
#define LOG_LTE_LL1_UL_AGC_TX_REPORT_LOG_C        (0x36 + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_UL_AGC_TX_REPORT_INT_LOG_C    (0x37 + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// Multi-rat
// ----------------------------------------------------------------------------
/*! @brief LTE LL1 Multi-rat conflict report log code */
#define LOG_LTE_LL1_MULTI_RAT_DL_REPORT_LOG_C      (0x2A + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_MULTI_RAT_UL_REPORT_LOG_C      (0x2B + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// CSF
// ----------------------------------------------------------------------------

/*! @brief LTE LL1 CSF report log code */
#define LOG_LTE_LL1_CSF_SE_TXMODE456_R1_RESULT_LOG_C   (0x39 + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_CSF_SE_TXMODE4_R2_RESULT_LOG_C     (0x3A + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_CSF_SE_TXMODE127_3R1_RESULT_LOG_C  (0x3B + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_CSF_WHITENED_MATRICES_LOG_C        (0x3C + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_CSF_PUCCH_PERIODIC_REPORT_LOG_C    (0x3D + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_CSF_PUSCH_APERIODIC_REPORT_LOG_C   (0x3E + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_CSF_SE_TXMODE9_RESULT_LOG_C        (0x28 + LOG_LTE_LL1_BASE_C)
#define LOG_LTE_LL1_CSF_CSIRS_CHEST_TXMODE9_LOG_C      (0x38 + LOG_LTE_LL1_BASE_C)

// ----------------------------------------------------------------------------
// UERS
// ----------------------------------------------------------------------------
/*! @brief LTE LL1 UERS log code */
#define LOG_LTE_LL1_UERS_TONES_LOG_C        (0x3F + LOG_LTE_LL1_BASE_C)
// ----------------------------------------------------------------------------
// Transmit front end
// ----------------------------------------------------------------------------


#define LOG_LTE_LL1_MASK_OFS( code ) \
  ( (uint64) 1 << ( (code) - LOG_LTE_LL1_BASE_C ) )

// Mask of valid log codes
// ML1 needs to forward log masks to LL1, and this mask contains the offsets
// from LOG_LTE_LL1_BASE_C that have valid codes. It must be kept up to date
// as new codes are added.
#define LOG_LTE_LL1_VALID_MASK ( \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_REQ_MSG_LOG_C                   ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_RXFE_AGC_LOG_C                  ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_RXFE_AGC_INT_LOG_C              ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_SRCH_PSS_RESULT_LOG_C           ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_SRCH_PSS_RESULT_INT_LOG_C       ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_SRCH_SSS_RESULT_LOG_C           ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_SRCH_SCELL_RESULT_LOG_C         ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_SRCH_NCELL_RESULT_LOG_C         ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_ANT_CORR_RESULT_LOG_C           ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_DEMFRONT_SERVING_CELL_CER_LOG_C ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_DEMFRONT_NEIGHBOR_CELL_CER_LOG_C ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_MBSFN_WHITENED_MATRICES_LOG_C   ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_DEMFRONT_PDSCH_DEMAPPER_CONFIGURATION_LOG_C ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_MBSFN_EMBMS_CER_LOG_C           ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_RLM_RESULT_INT_LOG_C            ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_PCFICH_DECODE_RESULT_LOG_C      ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_VPE_SCHEDULER_LOG_C             ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_PHICH_DECODE_RESULT_LOG_C       ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_PBCH_DECODE_RESULT_LOG_C        ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_PDCCH_DECODE_RESULT_LOG_C       ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_PDSCH_DECODE_RESULT_LOG_C       ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_PDSCH_DEBUG_LOG_C               ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_PDSCH_HARQ_OFFLOAD_LOG_C        ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_DEMFRONT_SERVING_CELL_RS_LOG_C  ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_UL_PUSCH_TX_REPORT_LOG_C        ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_UL_PUCCH_TX_REPORT_LOG_C        ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_UL_SRS_TX_REPORT_LOG_C          ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_UL_PRACH_TX_REPORT_LOG_C        ) | \
  LOG_LTE_LL1_MASK_OFS( LOG_LTE_LL1_UL_AGC_TX_REPORT_LOG_C          ) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_CSF_SE_TXMODE456_R1_RESULT_LOG_C ) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_CSF_SE_TXMODE4_R2_RESULT_LOG_C   ) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_CSF_SE_TXMODE127_3R1_RESULT_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_CSF_WHITENED_MATRICES_LOG_C      ) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_CSF_PUCCH_PERIODIC_REPORT_LOG_C  ) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_CSF_PUSCH_APERIODIC_REPORT_LOG_C ) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_SRCH_SERVING_CELL_MEASUREMENT_RESULT_INT_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_SRCH_SERVING_CELL_FTL_RESULT_INT_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_SRCH_SERVING_CELL_TTL_RESULT_INT_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_SERVING_CELL_COM_LOOP_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_SRCH_NCELL_MEASUREMENT_AND_TRACKING_INT_LOG_C) |  \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_UERS_TONES_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_RECOV_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_PRS_FW_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_PROF_LOG_C)       | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_DBG_LOG_C)| \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_CXM_CONFIG_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_CXM_STATUS_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_MULTI_RAT_DL_REPORT_LOG_C) | \
  LOG_LTE_LL1_MASK_OFS(LOG_LTE_LL1_MULTI_RAT_UL_REPORT_LOG_C) \
  )

/*********************** END OF LL1 LOG CODE DEFINES **************************/



#endif /* LTE_LL1_LOG_CODES_H */

