#ifndef RFWCDMA_CORE_ASDIV_H
#define RFWCDMA_CORE_ASDIV_H
/*!
  @file
  rfwcdma_core_asdiv.h

  @brief
  This file contains all declarations and definitions necessary to use the RF
  Antenna Switch Diversity 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/inc/rfwcdma_core_asdiv.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/12/14   ag      Added API to abort the pending AsDiv events
10/04/14   ac      xsw script to be appended to tune, featurised for now
07/19/13   aro     Featurized ASDIV
07/17/13   aro     Moved the L1 callback definition to rfm.c
07/15/13   aro/kr  Added initial support for WCDMA ASDiv
07/13/13   aro     Initial version

============================================================================*/

#include "comdef.h"
#include "rfcommon_asdiv_manager.h"
#include "rfcom.h"
#include "rfm_device_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FEATURE_RF_ASDIV

/*----------------------------------------------------------------------------*/
/*! Structure to hold the data to be used by the callback function used to 
create ASM script
*/
typedef struct
{
  rfm_device_enum_type device;
  /*!<  RFM device for which GRFC switch script is to be created */

  rfcom_wcdma_band_type band;
  /*!< WCDMA band for which the script is to be created */
  boolean immediate;
  /*!< Flag indicating if a script is to be created or immediate action is to
  be done */

} rfwcdma_asdiv_asm_script_data;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Create GRFC Switch script
 
  @details
  Based on the immediate flag, this function will create the GRFC switch script
  or will perform the immediate GRFC config.
 
  @param ant_sw_pos
  New Antenna Switch Position
 
  @param rf_buffer
  Pointer to buffer to store script
 
  @param cb_data
  Pointer to callback data

  @return
  Status indicating the success of the function execution  
*/
boolean 
rfwcdma_asdiv_create_asm_script
(

  rfcommon_asdiv_position_type ant_sw_pos, /*!< New Antenna Position */
  rf_buffer_intf *rf_buffer, /*!< pointer to buffer to store Script */
  void *cb_data /*!< Callback data to be passed to the function */);
/*----------------------------------------------------------------------------*/
boolean 
rfwcdma_asdiv_handle_set_antenna
(
  rfm_device_enum_type device,
  rfcommon_asdiv_position_type ant_sw_pos,
  rfm_l1_cb_type cb_func,
  void *cb_data,
  boolean immediate
);
/*----------------------------------------------------------------------------*/

void rfwcdma_core_asdiv_abort (uint32 handle);


#endif /* FEATURE_RF_ASDIV */

#ifdef __cplusplus
}
#endif

#endif /* RFWCDMA_CORE_ASDIV_H */



