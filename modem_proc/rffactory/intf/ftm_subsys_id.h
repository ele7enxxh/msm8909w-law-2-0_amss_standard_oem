#ifndef FTM_SUBSYS_ID_H
#define FTM_SUBSYS_ID_H

/*!
  @file
  FTM Subsys IDs

  @brief
  This module contains list of  FTM Subsys IDs.
*/

/*==============================================================================

  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc.  All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rffactory/intf/ftm_subsys_id.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/26/16   jfc     Added a new FTM subsystem ID for Unified Query
05/12/16   ndb     Added a new FTM Subsystem for Tuner(FTM_RF_ANT_TUNER_C)
03/16/16   esw     Added Extra FTM enums to support TX device status read register on THOR
10/22/15   php     Add support for LTE 5DLCA FTM Test Framework
07/22/15   zhw     Added support for C2K C4/C5 FTM Cmd
06/16/15   tsr     Added support for GSM C4 and C5 dispatch commands for GSM Non-Signalling
06/03/15   zhw     Added support for DRX NPT test
06/01/15   zhw     Added support for NPT test with TRX split
05/13/15   sc      Add enh RF GSM dispatcher
05/04/15   ak      [TH.2.0 CRAT]Change to support 6 Rx path RF test
04/16/15   aro     Added support to enable FTM RF test
04/14/15   zbz     Added C4 and C5 for GSM
01/23/15   Saul    Added FTM_SECURITY_ID_SENSOR
11/11/14   jyu     Add FTM_TDSCDMA_C2_C and FTM_TDSCDMA_C3_C
07/08/14   pk      Implementation of HDR FTM module
05/13/14   kab     Adding FTM_LTE_CAR2_C and FTM_LTE_CAR2_RX_2_C
10/08/13   aro     Updated copyright information
09/04/13   aro     Added FTM_WP_C
08/06/13   sml     Supporting GSM RxD FTM IDs
07/10/13   aro     Removed header file
05/25/13   aro     Migrated FTM Subsys ID
05/25/13   aro     Initial Release

==============================================================================*/

#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/ 
/*! FTM Subsys IDs */
typedef enum 
{
  FTM_1X_C0_C           = 0,
  FTM_WCDMA_C           = 1,
  FTM_GSM_C0_C          = 2,
  FTM_1X_C1_C           = 3,
  FTM_BT_C              = 4,
  FTM_I2C_C             = 5,
  FTM_MC_C              = 7,
  FTM_HDR_C             = 8,
  FTM_LOG_C             = 9,
  FTM_AGPS_C            = 10,
  FTM_PMIC_C            = 11,
  FTM_GSM_BER_C0_C      = 13,
  FTM_AUDIO_C           = 14,
  FTM_CAMERA_C          = 15,
  FTM_WCDMA_BER_C       = 16,
  FTM_GSM_EXTENDED_C0_C = 17,
  FTM_1X_CAL_V2_C       = 18,
  FTM_MF_C              = 19,
  FTM_COMMON_C          = 20,
  FTM_WCDMA_RX_2_C      = 21,
  FTM_WLAN_C            = 22,
  FTM_DSPDIAG_C         = 23,
  FTM_QFUSE_C           = 24,
  FTM_MBP_C             = 25,
  FTM_MF_NS_C           = 26,
  FTM_SE_BER_C          = 27,
  FTM_FM_C              = 28,
  FTM_LTE_C             = 29,
  FTM_LTE_NS_C          = 30,
  FTM_INO_C             = 31,
  FTM_1X_C2_C           = 32,
  FTM_MEAS              = 33,
  FTM_GNSS_C            = 34,
  FTM_LTE_RX_2_C        = 35,
  FTM_RFNV_C            = 36,
  FTM_SEQ_C             = 37,
  FTM_SV_C              = 38,
  FTM_RFM_C             = 39,
  FTM_1X_C3_C           = 40,
  FTM_RFCA_C            = 41,
  FTM_TDSCDMA_C         = 42,
  FTM_TDSCDMA_RX_2_C    = 43,
  FTM_TDSCDMA_BER_C     = 44,
  FTM_1X_C4_C           = 45,
  FTM_LTE_RFM_C         = 46,
  FTM_LTE_CAR1_C        = 47,
  FTM_LTE_CAR1_RX_2_C   = 48,
  FTM_GSM_C2_C          = 49,
  FTM_GSM_EXTENDED_C2_C = 50,
  FTM_GSM_BER_C2_C      = 51,
  FTM_AP_C              = 52,
  FTM_FFBM_C            = 53,
  FTM_DEVICE_INFO_C     = 54,
  FTM_NFC_C             = 55,
  FTM_WCDMA_RX_3_C      = 56,
  FTM_WCDMA_RX_4_C      = 57,
  FTM_GSM_C1_C          = 58,
  FTM_GSM_EXTENDED_C1_C = 59,
  FTM_GSM_BER_C1_C      = 60,
  FTM_GSM_C3_C          = 61,
  FTM_GSM_EXTENDED_C3_C = 62,
  FTM_GSM_BER_C3_C      = 63,
  FTM_WP_C              = 64, /*!< Interface between Factory tool and MMI app 
                              used in factory test mode to do device 
                              provisioning, fuses status checking and
                              device locking, and so on. Commands are functional 
                              for Windows phone target only */ 
  FTM_LTE_CAR2_C        = 65,
  FTM_LTE_CAR2_RX_2_C   = 66,
  /*67 directly used by LTE*/
  FTM_WCDMA_RX_5_C      = 68,  /*Addition of 2 Rx path Sub SyS ID.*/
  FTM_WCDMA_RX_6_C      = 69,
  FTM_WCDMA_RX_7_C      = 70,
  FTM_WCDMA_RX_8_C      = 71,
  FTM_GSM_ENH_0_C       = 72, //internal use only
  FTM_GSM_ENH_1_C       = 73, //internal use only
  FTM_LTE_RX_3_C        = 74,
  FTM_LTE_RX_4_C        = 75,
  FTM_LTE_CAR1_RX_3_C   = 76,
  FTM_LTE_CAR1_RX_4_C   = 77,
  FTM_LTE_CAR2_RX_3_C   = 78,
  FTM_LTE_CAR2_RX_4_C   = 79,
  FTM_LTE_CAR3_C        = 80,
  FTM_LTE_CAR3_RX_2_C   = 81,
  FTM_LTE_CAR3_RX_3_C   = 82,
  FTM_LTE_CAR3_RX_4_C   = 83,
  FTM_LTE_CAR4_C        = 84,
  FTM_LTE_CAR4_RX_2_C   = 85,
  FTM_LTE_CAR4_RX_3_C   = 86,
  FTM_LTE_CAR4_RX_4_C   = 87,

  FTM_MODE_ID_ANT_C     = 94,
  FTM_HDR_C0_C          = 95,
  FTM_HDR_C1_C          = 96,
  FTM_HDR_C2_C          = 97,
  FTM_HDR_C3_C          = 98,
  FTM_TDSCDMA_C2_C      = 99,
  FTM_TDSCDMA_C3_C      = 100,
  FTM_GSM_C4_C          = 101,
  FTM_GSM_C5_C          = 102,
  FTM_GSM_EXTENDED_C4_C = 103,
  FTM_GSM_EXTENDED_C5_C = 104,
  FTM_RF_TEST_C         = 105,

  FTM_1X_DYNAMIC_CHAIN_C = 106,
  FTM_WCDMA_DYNAMIC_CHAIN_C = 107,
  FTM_GSM_DYNAMIC_CHAIN_C = 108,
  FTM_TDS_DYNAMIC_CHAIN_C = 109,
  FTM_GSM_EXTENDED_DYNAMIC_CHAIN_C = 110,

  FTM_1X_DYNAMIC_CHAIN_DRX_C = 111,
  FTM_WCDMA_DYNAMIC_CHAIN_DRX_C = 112,
  FTM_GSM_DYNAMIC_CHAIN_DRX_C = 113,
  FTM_TDS_DYNAMIC_CHAIN_DRX_C = 114,
  FTM_GSM_EXTENDED_DYNAMIC_CHAIN_DRX_C = 115,
  FTM_GSM_BER_C4_C      = 116,
  FTM_GSM_BER_C5_C      = 117,
  FTM_1X_C5_C           = 118,
  FTM_GSM_C6_C          = 119,
  FTM_GSM_C7_C          = 120,
  FTM_RF_ANT_TUNER_C    = 121,
  FTM_UNIFIED_QUERY_C   = 122,
  
  FTM_RESERVED1_C       = 251,
  FTM_RESERVED2_C       = 252,
  FTM_RESERVED3_C       = 253,
  FTM_RESERVED4_C       = 254,
  FTM_SECURITY_ID_SENSOR = 255,
  FTM_MODE_FACTORY_C     = 0x8000,
  FTM_RESERVED_C         = 0x8001,
  FTM_MODE_MAX_C
}ftm_subsys_id_type;

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_SUBSYS_ID_H */



