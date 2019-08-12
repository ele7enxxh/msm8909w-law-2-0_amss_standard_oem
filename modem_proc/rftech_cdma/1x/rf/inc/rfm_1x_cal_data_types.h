#ifndef RFM_1X_CAL_DATA_TYPES_H
#define RFM_1X_CAL_DATA_TYPES_H

/*!
  @file
  Definitions of types pertaining to RF Calibration Data.

  @details
  This file exports the definitions of types related to RF Calibration Data 
  Packet that are used in the RF and FTM.
*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rfm_1x_cal_data_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/21/14   spa     Use int32 for offset calculation (rfm_1x_lna_cal_data_type)
07/01/11   aro     Added channel to intelliceiver Cal data structure
05/16/11   aro     Documentation Update for Cal data units
04/19/11   cd      LNA gain calibration data is signed int16
02/01/11   aro     Promoted Power Mode types to RFM
01/31/11   aro     Initial Release.

==============================================================================*/

#include "rf_cdma_intelliceiver.h"
#include "rfdevice_cdma_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
#define RFM_1X_INTELLICEIVER_CAL_DATA_MAX 128 /*!< Maximum number of 
Intelliceiver Calibration Data Points */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data Type to hold LNA Calibration Data. This will be used to format the data 
  packet collected during LNA calibration.
*/
typedef struct
{

  uint8 power_mode_num;
  /*!< Numer of Power Modes */

  int32 calibration[RFM_CDMA_POWER_MODE_MAX];
  /*!< Cal Data for Power Modes in 1/640th of dB units * */

} rfm_1x_lna_cal_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to store the intelliceiver calibrated data.
*/
typedef PACK(struct)
{
  uint16 channel;
  int16 midpoint_power[RFM_CDMA_POWER_MODE_MAX][RFDEVICE_CDMA_IC_NUM_OF_DET];

} rfm_1x_ic_calibration_data_type;

#ifdef __cplusplus
}
#endif

#endif /* RFM_1X_CAL_DATA_TYPES_H */

