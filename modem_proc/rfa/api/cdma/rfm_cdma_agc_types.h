#ifndef RFM_CDMA_AGC_TYPES_H
#define RFM_CDMA_AGC_TYPES_H

/*!
  @file
  rf_cdma_agc_types.h

  @details
  This file exports the definitions and declarations to be used by CDMA (1x
  and HDR) for RxAGC related operations
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_cdma_agc_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/15/11   sty     Doxygen update
06/14/11   sty     Doxygen update
06/14/11   sty     Initial Release

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Enum to use to indicate channel preference to 
rf_convert_rx_freq_khz_to_chan() API */ 
typedef enum
{
  RFM_CDMA_AGC_FORMAT__RAW,    /*!< AGC units as returned by FW */

  RFM_CDMA_AGC_FORMAT__DBM256, /*!< dBm*256 */

  RFM_CDMA_AGC_FORMAT__DBM10,   /*!< dBm*10 */

  /*!< deprecated - used only by HDR L1*/
  RFM_HDR_AGC_FORMAT__RAW = RFM_CDMA_AGC_FORMAT__RAW, 

  /*!< deprecated - used only by HDR L1*/
  RFM_HDR_AGC_FORMAT__DBM256 = RFM_CDMA_AGC_FORMAT__DBM256,

  /*!< deprecated - used only by HDR L1*/
  RFM_HDR_AGC_FORMAT__DBM10 = RFM_CDMA_AGC_FORMAT__DBM10 

} rf_cdma_agc_format_type;

typedef rf_cdma_agc_format_type rfm_hdr_agc_format_type;

#ifdef __cplusplus
}
#endif

#endif  /* RFM_CDMA_AGC_TYPES_H */

