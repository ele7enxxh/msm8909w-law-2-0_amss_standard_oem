#ifndef RF_HDR_TEMP_COMP_H
#define RF_HDR_TEMP_COMP_H

/*!
  @file
  rf_hdr_temp_comp.h

  @details
  This file exports the APIs needed to operate 1x temperature compensation
  module.  
*/

/*==============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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
04/18/12   hdz     Changed interface of rf_hdr_temp_comp_read_handler() to
                   handle cmd from rf_apps_task
11/23/11   hdz     Initial version

==============================================================================*/

#include "rf_cdma_temp_comp.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
boolean
rf_hdr_temp_comp_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_temp_comp_data_type *temp_comp_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_temp_comp_start
(
  rf_cdma_temp_comp_data_type *temp_comp_data,
  rfm_mode_enum_type mode,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_temp_comp_stop
(
  rf_cdma_temp_comp_data_type *temp_comp_data
);

/*----------------------------------------------------------------------------*/
void 
rf_hdr_temp_comp_read_handler
( 
  rf_apps_cmd_type* cmd_ptr 
);


#ifdef __cplusplus
}
#endif

#endif  /* RF_1X_TEMP_COMP_H */

