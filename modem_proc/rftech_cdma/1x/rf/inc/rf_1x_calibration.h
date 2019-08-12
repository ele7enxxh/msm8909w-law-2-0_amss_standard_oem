#ifndef RF_1X_CALIBRATION_H
#define RF_1X_CALIBRATION_H

/*! 
  @file
  rf_1x_calibration.h
 
  @brief
  This file defines interfaces to be used for RF Calibration
*/

/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_calibration.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/25/13   spa     Use int32 math for LNA offset calculation 
05/31/13   JJ      Change rf_1x_write_nv input band type 
04/02/12   aro     Removed unused 1x Cal v3 functions
03/28/12   aro     Added Current RF State as the parameter for ic calibration
                   to be used for PM override function to check if the state
                   has changed between the time when PM Ocerride command is
                   posted and handled
07/19/11   aro     Updated measure rx function to return status
07/01/11   aro     Added interface to perform Intelliceiver Calibration
05/16/11   aro     Removed DVGA Calibration interfaces
05/16/11   aro     Updated LNA Calibration interfaces to pass Gain State
05/16/11   aro     Added Rx Measurment config and query function
04/21/11   aro     Moved Get Calibration compensation function to Common
04/19/11   cd      Changed LNA offset return type to signed int16
02/17/11   aro     Moved LNA Gain Step definition to RFM
02/16/11   aro     Removed Power Mode Param from Measure LNA/DVGA interfaces
02/03/11   aro     Added interface to apply Compensation value
02/02/11   aro     Removed Power Mode Param from Measure DVGA/LNA
02/01/11   aro     Added support for NV Commit
02/01/11   aro     Added LNA Calibration Support
02/01/11   aro     Renamed function name
02/01/11   aro     Promoted Power Mode types to RFM
02/01/11   aro     Added DVGA Calibrate and Store
01/31/11   aro     Added LNA Calibration Support
01/31/11   aro     Initial Revision.

==============================================================================*/
#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "rf_cdma_lna.h"
#include "rfm_1x_cal_data_types.h"
#include "rfm_gain_state_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
int32
rf_1x_measure_lna_offset
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state,
  int32 agc_expected_val
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_calibrate_lna
(
  rfm_device_enum_type device,
  int16 power,
  rfm_lna_gain_state_type gain_state,
  rfm_1x_lna_cal_data_type *cal_data,
  int32 length
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_calibrate_intelliceiver
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_ic_data_type *ic_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type channel,
  rf_cdma_state_type curr_rf_state,
  int16 power,
  rfm_1x_ic_calibration_data_type *cal_data
);


/*----------------------------------------------------------------------------*/
boolean
rf_1x_write_nv
(
  rfm_cdma_band_class_type band
);

#ifdef __cplusplus
}
#endif

#endif /* RF_1X_CALIBRATION_H */
