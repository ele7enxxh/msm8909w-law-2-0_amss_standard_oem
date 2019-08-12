#ifndef RFWCDMA_CORE_LOG_H
#define RFWCDMA_CORE_LOG_H
/*!
  @file
  rfwcdma_core_log.h

  @brief
  This file contains all declarations and definitions pertaining to WCDMA
  RF logging
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/inc/rfwcdma_core_log.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/01/14   rmb     Add flush API for the new logging 0x4176 version 2.
07/30/13   aro     Added function to log AGC once
07/24/13   aro     Added initial framework to support AGC logging for WCDMA
07/24/13   aro     Initial version

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
boolean 
rfwcdma_log_init
(
  void
);

/*----------------------------------------------------------------------------*/
void 
rfwcdma_core_log_agc_data
( 
  void 
);

/*----------------------------------------------------------------------------*/
void 
rfwcdma_core_log_agc_data_once
( 
  void 
);

/*----------------------------------------------------------------------------*/
void 
rfwcdma_core_log_agc_data_flush
( 
  void 
);

#ifdef __cplusplus
}
#endif

#endif /* RFWCDMA_CORE_LOG_H */





