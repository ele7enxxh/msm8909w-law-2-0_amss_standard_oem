/*!
  @file
  tfw_sw_intf_log.h

  @brief
  TD-SCDMA diag log FW-SW Interface Definitions 

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_sw_intf_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
===========================================================================*/

#ifndef __TFW_SW_INTF_LOG_H
#define __TFW_SW_INTF_LOG_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tfw_const.h"
#include "msgr.h"
#include "tfw_diag_log_codes.h"


/*===========================================================================
  Log Mask
===========================================================================*/
/**
   \defgroup diagLogMsg Messages from SW
   \ingroup diagLog
*/

typedef struct {
  /*! \brief bitmask for tfw log codes*/
  uint64 mask;
} tfw_diag_log_mask_t;

/*! @brief Message for Log Mask
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  /*! \brief Log mask */
  tfw_diag_log_mask_t logMask;
} tfw_diag_log_mask_cmd_t;    
                 
#define TFW_DIAG_LOG_MASK_OFS( code ) \
  ( (uint64) 1 << ( (code) - TFW_DIAG_LOG_BASE_C ) )

/*inform SW which packets are available in FW*/
#define TFW_DIAG_LOG_VALID_MASK ( \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_TRACKING_MODE_C                    ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_AGC_POWER_SCAN_C                   ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_AGC_GAP_DETECTION_C                ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_AGC_ACQ_STEP12_C                   ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_REQCQ_RESULTS_C                    ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_DL_SYNC_CODE_RESULTS_C             ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_TS0_MIDAMBLE_DETECTION_RESULTS_C   ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_TS_RSP_MEASUREMENT_RESULTS_C       ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_TRACKING_LOOP_RESULTS_C            ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_STEP3_RESULTS_C                    ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_SERVING_CELL_FRAME_TIMING_C        ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_DEMFRONT_CFG_C                     ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_DEMFRONT_CELL_CFG_C                ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_CHANNEL_ESTIMATION_SHORT_C         ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_EQUALIZER_WEIGHT_COMPUTATION_C     ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_AE_RESULTS_C                       ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_SPECIAL_BURST_DETECTION_RESULTS_C  ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_TPC_SS_DECODING_RESULTS_C          ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_TFCI_DECODING_RESULTS_C            ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_EHICH_DECODING_RESULTS_C           ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_EAGCH_DECODING_RESULTS_C           ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_UL_DPCH_ENCODING_RESULTS_C         ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_PRACH_ENCODING_RESULTS_C           ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_UPPCH_ENCODING_RESULTS_C           ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_HS_SICH_ENCODING_RESULTS_C         ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_EPUCH_ENCODING_RESULTS_C           ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_ERUCCH_ENCODING_RESULTS_C          ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_UPLINK_REPORT_C                    ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_TPC_REPORT_C                       ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_UPLINK_TIMING_C                    ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_CHANNEL_ESTIMATION_LONG_C          ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_NONHS_SIR_C                        ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_MEMORY_CHUNK_C                     ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_CQI_C                              ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_MSG_C                              ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_DYNAMIC_SWITCH_C                   ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_HSSCCH_DECODING_RESULTS_C          ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_IRAT_T2X_C                         ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_TIMELINE_PROFILE_C                 ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_CE_DEMFRONT_PROFILE_C              ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_IRAT_X2T_C                         ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_ENHANCE_NONTS0_PRUNING_RESULTS_C   ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_DSDX_CXM_SLOT_STATUS_C             ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_MULTI_SIM_CFG_MSG_C                ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_DSDS_TG_QTA_GAP_C                  ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_DSDA_ACQ_IRAT_C                    ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_RX_AGC_C                           ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_RF_SLOT_UPDATE_C                   ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_RF_EVENTS_C                        ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_EXCEPTIONAL_EVENTS_C               ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_DSDA_TG_POWERSCAN_C                ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_VPE_INFORMATION_C                  ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_DEBUG_C                            ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_COMMAND_C                          ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_RESPONSE_C                         ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_HS_RESULT_C                        ) | \
  TFW_DIAG_LOG_MASK_OFS( TFW_DIAG_LOG_FBRX_C                             ) | \
  0)           

/*used by SW*/
#define TFW_DIAG_LOG_OFS_MAX       (1+TFW_DIAG_LOG_MASK_OFS(TFW_DIAG_LOG_FBRX_C))

#endif /* __TFW_SW_INTF_LOG_H */
