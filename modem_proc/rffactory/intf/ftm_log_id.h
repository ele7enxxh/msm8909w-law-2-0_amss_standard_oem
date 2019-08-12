#ifndef FTM_LOG_ID_H
#define FTM_LOG_ID_H

/*!
  @file
  Common FTM Log IDs

  @brief
  This module contains list of common FTM Log IDs.
*/

/*==============================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.

  Qualcomm Technologies Proprietary and Confidential.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rffactory/intf/ftm_log_id.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/14/15   ash     Added support for HORXD + CA logging
09/25/15   jmf     Add support for FBRx-based VSWR logging
01/27/15   dyc     Added support for ASDiv related logging
08/28/14   zbz     support for GSM enhanced AGC logging
08/26/14   aa      Added support for FBRX QRCT logging
10/08/13   aro     Updated copyright information
07/19/13   aro     Migrated Qtuner Log ID
07/10/13   aro     Removed header file
05/25/13   aro     Migrated FTM Log ID
05/25/13   aro     Initial Release

==============================================================================*/

#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/ 
/*! Enumeration of log IDs */
typedef enum 
{
  FTM_LOG_PRINTF          = 0x0,
  FTM_LOG_BT              = 0x1,
 
  FTM_LOG_DACC_ACCUM_C0   = 0x2,
  FTM_LOG_DACC_ACCUM_C1   = 0x3,
  
  FTM_LOG_1X_AGC          = 0x4,
  FTM_LOG_HDR_AGC         = 0x5,
  FTM_LOG_1X_AGC_C1       = 0x6,
  FTM_LOG_HDR_AGC_C1      = 0x7,

  //FTM_LOG_MODEM_STATUS    = 0x8000,
  FTM_LOG_IM2_DFT         = 0x8,
  FTM_LOG_FFT             = 0x9,
  FTM_LOG_AUTOCAL         = 0xA, 
  
  FTM_LOG_GSM_THERM_VBATT  = 0xB,

  FTM_LOG_TX_RX_SWEEP      = 0xB,
  FTM_LOG_INTELLICEIVER   = 0xB,

  FTM_LOG_HDET_TRACKING   = 0x10,

  FTM_LOG_SEQUENCER      = 0x11,

  FTM_LOG_ICI_CAL_DATA   = 0x20,

  FTM_LOG_ATUNER_RL_DATA       = 0x21,
  FTM_LOG_ATUNER_SCENARIO_DATA = 0x22,

  FTM_LOG_ASDIV_DATA       = 0x40,
  FTM_LOG_ASDIV_TUNER_DATA = 0x41,

  FTM_LOG_WCDMA_AGC       = 0x1004,
  FTM_LOG_GSM_BER         = 0x1005,
  FTM_LOG_WCDMA_BER       = 0x1006,
  FTM_LOG_WCDMA_AGC_C1    = 0x1007,
  FTM_LOG_WCDMA_TXPLIM    = 0x1008,
  FTM_LOG_SE_BER          = 0x1009,
  FTM_LOG_WCDMA_AGC_ENH   = 0x1010,

  FTM_LOG_LTE_TX_AGC            = 0x1111,
  FTM_LOG_LTE_RX_AGC            = 0x1112,
  FTM_LOG_LTE_TXPLIM            = 0x1113,
  FTM_LOG_LTE_TX_AGC_ENH        = 0x1114,
  FTM_LOG_LTE_RX_AGC_ENH        = 0x1115,
  FTM_LOG_LTE_CA_HORXD_RX_AGC   = 0x1116,
  
  FTM_LOG_TDSCDMA_RX_AGC    = 0x1120,
  FTM_LOG_TDSCDMA_TX_AGC    = 0x1121,
  FTM_LOG_TDSCDMA_BER       = 0x1122,
  FTM_LOG_TDSCDMA_TXPLIM    = 0x1123,

  FTM_LOG_GSM               = 0x1124,
  FTM_LOG_IRAT_TIMING       = 0x1125,
  FTM_LOG_FBRX_RESULTS      = 0x1126,
  FTM_LOG_GSM_ENH           = 0x1127,

  FTM_LOG_FBRX_VSWR_RESULTS = 0x1128,

  FTM_LOG_MAX

} ftm_log_id_type;

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_LOG_ID_H */



