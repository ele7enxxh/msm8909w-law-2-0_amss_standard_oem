#ifndef RFWCDMA_DATA_DEBUG_H
#define RFWCDMA_DATA_DEBUG_H

/*! 
  @file
  rfwcdma_data_debug.h
 
  @brief
  This file contains prototypes and definitions that can be exported to
  wcdma and other components for debug purposes.
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/10/14   vs      Added Flag to enable NBEE Cal debug info
11/17/14   rmb     Initial Revision.

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Data structure used to keep track of temporary debug variables */
typedef struct
{

  boolean enable_opt_wakeup;
  /*!< Flag to enable/disable debug optimization */

  boolean enable_agc_log_v2;
  /*!< Flag to enable WCDMA AGC log v2 */

   boolean enable_rf_nbr_operations;
  /*!< Flag to enable/disable NBR operations */

  boolean enable_script_based_tune;
  /*!< Flag to enable/ disable script based Rx Tune. 
    This lets RF control when to fall back to direct write tuning */

  boolean apply_notch_msm;
  /*!< Flag to apply notch to RxFE */

  boolean enable_nbee_cal_dbg;
  /*!< Flag to enable NBEE Cal debug info */

} rfwcdma_data_debug_flags_type;

/*----------------------------------------------------------------------------*/
rfwcdma_data_debug_flags_type*
rfwcdma_get_debug_flags
(
  void
);
#ifdef __cplusplus
}
#endif

#endif /* RFWCDMA_DATA_DEBUG_H */

