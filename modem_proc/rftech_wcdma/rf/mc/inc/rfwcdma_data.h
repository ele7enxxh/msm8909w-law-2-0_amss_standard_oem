#ifndef RFWCDMA_DATA_H
#define RFWCDMA_DATA_H

/*! 
  @file
  rfwcdma_data.h
 
  @brief
  This file contains prototypes and definitions to be used by Centralized 
  WCDMA Global Data.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/mc/inc/rfwcdma_data.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/17/14   rmb     Move the debug data flags to rfwcdma_data_debug.h
06/26/14   vbh     Add a dbg flag for using script based tune from FTM
06/19/14   aro/ak  Added Support for Prep/Exec Temp Comp Split
06/06/14   ac      NBR support
05/16/14   aro     Added structure to hold power limiting data
02/04/14   vbh     Initial revision on Bolt
10/01/13   aro     Flag to enable v3 AGC log
09/30/13   aro     Doxygen Update
09/30/13   aro     Add Data module initialization
09/30/13   aro     Variable to enable/disable wakeup optimization
09/13/13   aro     Added data structure for critical section lock
09/13/13   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_locks.h"
#include "rfwcdma_power_limiting.h"
#include "rfwcdma_core_temp_comp.h"
#include "rfwcdma_data_debug.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
/*! Enum to indicate the Max WCDMA device */
#define RFWCDMA_DATA_MAX_DEVICE (RFM_DEVICE_4 + 1)

/*----------------------------------------------------------------------------*/

/*! 
  @brief
  Structure Containing WCDMA radio status 
*/ 
typedef struct
{

  rfwcdma_temp_comp_data_type temp_comp;
  /*!< Data structur holding the temperature compensation data */

} rfwcdma_data_status_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by WCDMA Driver. There should not
  be stray global variables. If global variable is required, it must be 
  contained within this structure. 
*/ 
typedef struct
{
  rfwcdma_data_status_type radio_status[RFWCDMA_DATA_MAX_DEVICE];
  /*!< Structure Containing WCDMA radio status  */

  rf_lock_data_type *intf_crit_section;
  /*!< Global Mutex for Common WCDMA Driver */

  rf_lock_data_type tempcomp_crit_section;
  /*!< Global Mutex to avoid Temp Read and RF sleep */

  rfwcdma_data_debug_flags_type debug_flags;
  /*!< Data structure used to keep track of trmporary debug variables */

  rfwcdma_power_limiting_data_type plim_data;
  /*!< Structure to hold the power limiting data */

} rfwcdma_data_type;

/*----------------------------------------------------------------------------*/
void
rfwcdma_set_lock_data
(
  rf_lock_data_type *lock_data
);

/*----------------------------------------------------------------------------*/
rf_lock_data_type*
rfwcdma_get_lock_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfwcdma_power_limiting_data_type*
rfwcdma_get_power_limiting_data
(
  void
);

/*----------------------------------------------------------------------------*/
void
rfwcdma_init_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_lock_data_type*
rfwcdma_get_temp_comp_lock_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfwcdma_temp_comp_data_type*
rfwcdma_data_get_temp_comp
(
   rfm_device_enum_type device
);


#ifdef __cplusplus
}
#endif

#endif /* RFWCDMA_DATA_H */

