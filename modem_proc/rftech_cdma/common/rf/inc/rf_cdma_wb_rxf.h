#ifndef RF_CDMA_WB_RXF_H
#define RF_CDMA_WB_RXF_H

/*!
  @file
  rf_cdma_wb_rxf.h

  @details
  This file exports the definitions and declartions pertaining to Wideband Rx
  Front End.
*/

/*==============================================================================

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_wb_rxf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/09/14   spa     Notch filter support for bolt
06/15/12   adk     Corrected BB spur mitigation for WTR1605
02/17/12   sty     Added RF_MAX_NUM_CARRIERS_PER_PATH
12/10/11   sty     var name changed
12/06/11   sty     Changed num of Notch filters to 6 
12/08/10   aro     Fixing preprocessor Macros
12/06/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! max number of carriers allowed for a given path */
#define RF_MAX_NUM_CARRIERS_PER_PATH                                           3

/*----------------------------------------------------------------------------*/
/*! Maximum number of notch filters that can be set, cumulative for all carriers 
    - this needs to come from MSM HWIO file */
#define RF_MAX_NUM_NOTCH_FILTERS_AVAIL                                         4

/*----------------------------------------------------------------------------*/
/*! Number of notch gain states available for each notch filter */
#define RF_MAX_NUM_NOTCH_FILTER_STATES                                         3

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure for describing notch filter parameters
*/
typedef struct
{
  
  int32 spur_offset;
  /*!< Notch center freq in Hz in baseband */

  uint8 notch_depth[RF_MAX_NUM_NOTCH_FILTER_STATES];
  /*!< The notch depth (value of K in the notch FSM) */

  int32 rise_threshold[RF_MAX_NUM_NOTCH_FILTER_STATES - 1];
  /*!< Rise thresholds (in RxAGC units) for all states except the lowest one */

  int32 rise_fall_hysteresis_dB10;
  /*!< Hysteresis (in dB/10 units) to be applied to rise thresholds to compute 
  fall thresholds for all states except the highest one */

} rxf_notch_filter_config_type;

#ifdef __cplusplus
}
#endif

#endif  /* RF_CDMA_WB_RXF_H */

