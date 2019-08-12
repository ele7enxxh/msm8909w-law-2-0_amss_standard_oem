#ifndef FTM_CALIBRATION_V3_TYPES_H
#define FTM_CALIBRATION_V3_TYPES_H

/*!
  @file
  ftm_calibration_v3_types.h
 
  @brief
  Common Type definition to be used by calv4.
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


when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/13/14   kab     Add 3xCA support
04/16/13   aro     Variable/type/prototype cleanup
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/02/13   aro     Migrated to inline Math functions
04/02/13   aro     Command for enable debug level for calv3
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/28/13   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_mode_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Macro definition to perform size variable and pointer update based on size 
passed */
#define UPDATE_SIZE_N_PTR(size_var, ptr, size) \
  size_var += size; ptr += size;

/*----------------------------------------------------------------------------*/
/*! Maximum number of Techs supported by RF Calibration V3 */
#define FTM_CALIBRATION_NUM_OF_TECHS ( RFM_NUM_MODES + 1 )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type of Device Flag Mask for FTM Calibration Command
*/
typedef uint8 ftm_calibration_device_flag_mask_type;

/*----------------------------------------------------------------------------*/
/*! List of enumeration indicating the F3 debug level for calv3 */
typedef enum
{
  CAL_DBG_ALL = 0x1,
  /*!< indicates that all debug messages are enabled */

  CAL_DBG_ISR = 0x2,
  /*!< indicates that only debug messages in ISR callback are enabled */

} ftm_cal_debug_level_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of all valid Tx Measurement Mask used for RF Calibration.
*/
typedef enum
{

  DEVICE_FLAG_0    = 0x1,
  /*!< Device 0 Measurement */

  DEVICE_FLAG_1    = 0x2,
  /*!< Device 1 Measurement */

  DEVICE_FLAG_2    = 0x4,
  /*!< Device 2 Measurement */

  DEVICE_FLAG_3    = 0x8,
  /*!< Device 3 Measurement */

  DEVICE_FLAG_4    = 0x10,
  /*!< Device 4 Measurement */

  DEVICE_FLAG_5    = 0x20,
  /*!< Device 5 Measurement */

} ftm_calibration_device_flag_mask_enum_type;

#ifdef __cplusplus
}
#endif

#endif /* FTM_CALIBRATION_V3_TYPES_H */

