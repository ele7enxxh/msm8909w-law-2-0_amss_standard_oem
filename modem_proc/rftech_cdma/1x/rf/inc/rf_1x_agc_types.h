#ifndef RF_1X_AGC_TYPES_H
#define RF_1X_AGC_TYPES_H

/*!
  @file
  Definitions of types pertaining to RF 1x RxAGC.

  @details
  This file exports the definitions of types that are used in the RF
  for 1x RxAGC.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_agc_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/19/11   Saul    IRAT update. Use 1x Rx AGC Acq Duration of 2 ms for IRAT.
07/18/11   aro     Initial version

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  1x AGC modes
*/
typedef enum
{
  RF_1X_AGC_MODE_NORMAL,
  /*!< 1x RxAGC Mode in which RxAGC will be started normally with all 
  AGC accumulators reset to zero */

  RF_1X_AGC_MODE_RESUME_WITH_CURR,
  /*!< 1x RxAGC Mode in which RxAGC will be resumed normally without all 
  AGC accumulators reset to zero. The AGC accum values used by FW will be 
  internally fetched by FW. This will be used for cases like intelliceiver 
  Update  */ 

} rf_1x_agc_mode_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Object to specify what AGC acquistion mode to use during 
  1x Wakeup Rx, and any parameters that go with the requested
  mode.
*/
typedef struct
{

  rf_1x_agc_mode_type mode;
  /*!< The AGC acquisition mode to use during wakeup. */

  uint32 acq_duration;
  /*!< The AGC acquisition duration to use. Units: microsec */

} rf_1x_agc_param_type;



#ifdef __cplusplus
}
#endif

#endif /* RF_1X_AGC_TYPES_H */


