/*!
  @file 
  rf_hal_grfc.h

  @brief
  header file for all grfc typedef and functions that are made available to
  external mododule

*/

/*===========================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rf_hal_grfc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/13   ra      Support CCS portability 
07/26/12   jfc     Mainline FEATURE_DIME_MODEM
06/26/12   jfc     Remove temporary legacy definitions
06/08/12   kb      Temporary fix to the redefinition compilation errors.
05/17/12   jfc     Add prototypes of RF HAL GRFC functions
05/15/12   jfc     Add some common definitions
04/27/12   jfc     Add some legacy defs for Dime to get past compilation
04/26/12   jfc     Include rfd_grfc.h instead of putting the definitions here

===========================================================================*/
#ifndef RF_HAL_GRFC_H
#define RF_HAL_GRFC_H

/* Common Definitions */
#include "rf_hal_bus_types.h"

typedef rf_hal_bus_result_type rfhal_grfc_result_type;
typedef uint8 grfc_enum_type;

/*! @brief There are only 32 GRFC numbers on Dime */
#define RF_HAL_GRFC_MAX_NUM  32

/*! @brief Set a single GRFC signal to high or low 
*/
rf_hal_bus_result_type rfhal_grfc_write_mask
(
  uint32 mask, 
  boolean level, 
  rf_hal_bus_resource_script_settings_type* settings
);

/*! @brief Set a group of GRFC signal to high or low 
*/
rf_hal_bus_result_type rfhal_grfc_write_single
(
  uint8 grfc_num,
  boolean level,
  rf_hal_bus_resource_script_settings_type* settings
);

/*! @brief Read a single GRFC signal level
*/
rf_hal_bus_result_type rfhal_grfc_read_mask
(
  uint32 mask,
  uint32* levels
);

/*! @brief Read a group of single GRFC signal levels
*/
rf_hal_bus_result_type rfhal_grfc_read_single
(
  uint8 grfc_num,
  boolean* level,
  rf_hal_bus_resource_script_settings_type* settings
);

#endif  /* RF_HAL_GRFC_H */
