#ifndef FTM_MODE_ID_H
#define FTM_MODE_ID_H

/*!
  @file
  FTM Mode IDs

  @brief
  This module contains list of  FTM Mode IDs.
*/

/*==============================================================================

  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rffactory/intf/ftm_mode_id.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/05/16   lhb     Add LTE B43 support
12/11/15   bmg     Added LTE B66 mode
12/15/14   kab     LTE B32, LTE-U 125,126,127 Support
06/20/14   qzh     LTE B45 Support
06/19/14   rp      Support for separate Tx NVs for LTE B39 filtered path.
11/20/13   kab     LTE B27 Support
10/08/13   aro     Updated copyright information
08/14/13   kab     LTE B30 Support
07/10/13   aro     Removed header file
05/25/13   aro     Migrated FTM mode ID
05/25/13   aro     Initial Release

==============================================================================*/

#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/ 
/*! FTM RF Mode IDs */
typedef enum 
{
  FTM_DB_RF_CDMA,
  FTM_DB_RF_CDMA_800,
  FTM_DB_RF_CDMA_1900,
  FTM_DB_RF_WCDMA_IMT,
  FTM_DB_RF_WCDMA_1900MHZ_A,
  FTM_DB_RF_WCDMA_1900MHZ_B,
  FTM_DB_RF_GSM_900,        
  FTM_DB_RF_GSM_1800,       
  FTM_DB_RF_GPS,            
  FTM_DB_RF_SLEEP,          
  FTM_DB_RF_IDLE,           
  FTM_DB_RF_GSM_850,        
  FTM_DB_RF_GSM_1900,       
  FTM_DB_RF_WCDMA_BC3,      
  FTM_DB_RF_WCDMA_BC4,      
  FTM_DB_RF_WCDMA_800MHZ,   
  FTM_DB_RF_WCDMA_BC8,      
  FTM_DB_RF_WCDMA_BC9,
  FTM_DB_RF_WCDMA_BC11,
  FTM_DB_RF_WCDMA_BC19,
  FTM_DB_RF_LTE_BC1,   
  FTM_DB_RF_LTE_BC2,   
  FTM_DB_RF_LTE_BC4,   
  FTM_DB_RF_LTE_BC7,   
  FTM_DB_RF_LTE_BC11,  
  FTM_DB_RF_LTE_BC13,  
  FTM_DB_RF_LTE_BC17,  
  FTM_DB_RF_LTE_BC38,  
  FTM_DB_RF_LTE_BC40,  
  FTM_DB_RF_LTE_BC3,   
  FTM_DB_RF_LTE_BC20,  
  FTM_DB_RF_LTE_BC5,   
  FTM_DB_RF_LTE_BC8,   
  FTM_DB_RF_LTE_BC18,  
  FTM_DB_RF_HDR, 
  FTM_DB_RF_TDSCDMA_B34,
  FTM_DB_RF_TDSCDMA_B39,
  FTM_DB_RF_TDSCDMA_B40,
  FTM_DB_RF_LTE_BC25,
  FTM_DB_RF_LTE_BC12,   
  FTM_DB_RF_LTE_BC41,   
  FTM_DB_RF_LTE_BC9,    
  FTM_DB_RF_LTE_BC34,   
  FTM_DB_RF_LTE_BC39,   
  FTM_DB_RF_LTE_BC6,    
  FTM_DB_RF_LTE_BC10,   
  FTM_DB_RF_LTE_BC14,   
  FTM_DB_RF_LTE_BC23,   
  FTM_DB_RF_LTE_BC26,   
  FTM_DB_RF_LTE_BC42,   
  FTM_DB_RF_LTE_BC28,   
  FTM_DB_RF_LTE_BC29,   
  FTM_DB_RF_LTE_BC30,  
  FTM_DB_RF_LTE_BC27,  
  FTM_DB_RF_LTE_BC45,
  FTM_DB_RF_LTE_BC32,
  FTM_DB_RF_LTE_BC125,
  FTM_DB_RF_LTE_BC126,
  FTM_DB_RF_LTE_BC127,
  FTM_DB_RF_LTE_BC252,
  FTM_DB_RF_LTE_BC253,
  FTM_DB_RF_LTE_BC254,
  FTM_DB_RF_LTE_BC255,
  FTM_DB_RF_MAX
} ftm_rf_mode_type;

/*----------------------------------------------------------------------------*/ 
/*! FTM Mode IDs */
typedef enum 
{
  FTM_PHONE_MODE_CDMA = 0,
  FTM_PHONE_MODE_SLEEP =2,
  FTM_PHONE_MODE_GPS,
  FTM_PHONE_MODE_CDMA_800 = 5,
  FTM_PHONE_MODE_CDMA_1900,
  FTM_PHONE_MODE_HDR,
  FTM_PHONE_MODE_CDMA_1800,
  FTM_PHONE_MODE_WCDMA_IMT,
  FTM_PHONE_MODE_GSM_900,
  FTM_PHONE_MODE_GSM_1800,
  FTM_PHONE_MODE_GSM_1900,
  FTM_PHONE_MODE_BLUETOOTH,
  FTM_PHONE_MODE_JCDMA = 14,
  FTM_PHONE_MODE_WCDMA_1900A,
  FTM_PHONE_MODE_WCDMA_1900B,
  FTM_PHONE_MODE_CDMA_450,
  FTM_PHONE_MODE_GSM_850,
  FTM_PHONE_MODE_IMT,
  FTM_PHONE_MODE_HDR_800,
  FTM_PHONE_MODE_HDR_1900,
  FTM_PHONE_MODE_WCDMA_800,
  FTM_PHONE_MODE_WCDMA_BC3 = 25,
  FTM_PHONE_MODE_CDMA_BC14 = 26,
  FTM_PHONE_MODE_CDMA_BC11 = 27,
  FTM_PHONE_MODE_WCDMA_BC4 = 28,
  FTM_PHONE_MODE_WCDMA_BC8 = 29,
  FTM_PHONE_MODE_MF_700 = 30,
  FTM_PHONE_MODE_WCDMA_BC9 = 31,
  FTM_PHONE_MODE_CDMA_BC15 = 32,
  FTM_PHONE_MODE_CDMA_BC10 = 33,

  FTM_PHONE_MODE_LTE_B1 = 34,
  FTM_PHONE_MODE_LTE_B7 = 35,
  FTM_PHONE_MODE_LTE_B13 = 36,
  FTM_PHONE_MODE_LTE_B17 = 37,
  FTM_PHONE_MODE_LTE_B38 = 38,
  FTM_PHONE_MODE_LTE_B40 = 39,
  FTM_PHONE_MODE_WCDMA_BC11 = 40,
  FTM_PHONE_MODE_LTE_B11 = 41,
  FTM_PHONE_MODE_LTE_B4 = 42,


  FTM_PHONE_MODE_LTE_B2 = 43,
  FTM_PHONE_MODE_LTE_B3 = 44,
  FTM_PHONE_MODE_LTE_B5 = 45,
  FTM_PHONE_MODE_LTE_B6 = 46,
  FTM_PHONE_MODE_LTE_B8 = 47,
  FTM_PHONE_MODE_LTE_B9 = 48,
  FTM_PHONE_MODE_LTE_B10 = 49,
  FTM_PHONE_MODE_LTE_B12 = 50,
  FTM_PHONE_MODE_LTE_B14 = 51,
  FTM_PHONE_MODE_LTE_B15 = 52,
  FTM_PHONE_MODE_LTE_B16 = 53,
  FTM_PHONE_MODE_LTE_B18 = 54,
  FTM_PHONE_MODE_LTE_B19 = 55,
  FTM_PHONE_MODE_LTE_B20 = 56,
  FTM_PHONE_MODE_LTE_B21 = 57,
  FTM_PHONE_MODE_LTE_B22 = 58,
  FTM_PHONE_MODE_LTE_B23 = 59,
  FTM_PHONE_MODE_LTE_B24 = 60,
  FTM_PHONE_MODE_LTE_B25 = 61,
  FTM_PHONE_MODE_LTE_B26 = 62,
  FTM_PHONE_MODE_LTE_B27 = 63,
  FTM_PHONE_MODE_LTE_B28 = 64,
  FTM_PHONE_MODE_LTE_B29 = 65,
  FTM_PHONE_MODE_LTE_B30 = 66,
  FTM_PHONE_MODE_LTE_B31 = 67,
  FTM_PHONE_MODE_LTE_B32 = 68,
  FTM_PHONE_MODE_LTE_B33 = 69,
  FTM_PHONE_MODE_LTE_B34 = 70,
  FTM_PHONE_MODE_LTE_B35 = 71,
  FTM_PHONE_MODE_LTE_B36 = 72,
  FTM_PHONE_MODE_LTE_B37 = 73,
  FTM_PHONE_MODE_LTE_B39 = 74,
  FTM_PHONE_MODE_WCDMA_BC19 = 75,
  FTM_PHONE_MODE_LTE_B41 = 76,
  FTM_PHONE_MODE_LTE_B42 = 77,
  FTM_PHONE_MODE_LTE_B39_B = 78,
  FTM_PHONE_MODE_LTE_B45 = 79,
  FTM_PHONE_MODE_LTE_B125 = 80,
  FTM_PHONE_MODE_LTE_B126 = 81,
  FTM_PHONE_MODE_LTE_B127 = 82,
  FTM_PHONE_MODE_LTE_B66 = 83,
  FTM_PHONE_MODE_LTE_B252 = 84,
  FTM_PHONE_MODE_LTE_B253 = 85,
  FTM_PHONE_MODE_LTE_B254 = 86,
  FTM_PHONE_MODE_LTE_B255 = 87,
  FTM_PHONE_MODE_LTE_B43 = 88,
  FTM_PHONE_MODE_LTE_B46 = 89,
  /*TDSCDMA reserves 90 - 99*/
  FTM_PHONE_MODE_TDSCDMA_B34 = 90,
  FTM_PHONE_MODE_TDSCDMA_B39 = 91,
  FTM_PHONE_MODE_TDSCDMA_B40 = 92,
  FTM_PHONE_MODE_TDSCDMA_B40_B = 93,
  FTM_PHONE_MODE_MAX

} ftm_mode_id_type;

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_MODE_ID_H */

