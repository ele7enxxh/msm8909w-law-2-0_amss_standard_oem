#ifndef FTM_HDR_LOG_H
#define FTM_HDR_LOG_H

/*!
  @file
  ftm_hdr_log.h

  @brief
  This module defines the function prototypes and defintions used by
  HDR RF Logging.
*/

/*==============================================================================

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/inc/ftm_hdr_log.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/09/11   aro     Renamed CDMA logging functions to reflect primary and
                   secondary chains
09/07/11   aro     Added functions to initialize, de-initialize and push the
                   RFM_DEVICE_0 AGC logging data
09/07/11   aro     Added data structure for HDR logging
12/13/10   aro     Removed FTM_HAS_LOGGING Feature
12/07/10   aro     Initial Revision : Common Calibration

==============================================================================*/

#include "comdef.h"
#include "ftm_cdma_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Data structure containing CDMA 1x logging information */
typedef struct
{

  ftm_cdma_c0_agc_type *c0_agc;
  /*!< Pointer to data structure containing Chain 0 AGC log information */

  ftm_cdma_c1_agc_type *c1_agc;
  /*!< Pointer to data structure containing Chain 1 AGC log information */

} ftm_hdr_log_data_type;

/*----------------------------------------------------------------------------*/
void
ftm_hdr_log_primary_agc_init_func
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_log_primary_agc_exit_func
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_log_primary_agc_cb
(
  int32 interval
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_log_diversity_agc_init_func
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_log_diversity_agc_exit_func
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_log_diveristy_agc_cb
(
  int32 interval
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_HDR_LOG_H */
