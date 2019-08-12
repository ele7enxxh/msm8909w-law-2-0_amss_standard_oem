#ifndef RF_1X_RF_TASK_H
#define RF_1X_RF_TASK_H

/*! 
  @file
  rf_1x_rf_task.h
 
  @brief
  This file includes functions pertaining to Tasks used for 1x Operation
*/
 
/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_rf_task.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/22/12   aro     Changes to migrate to new precide time profiler
07/23/12   Saul    CDMA. Removed unnecessary header.
03/22/12   aro     Deleted unused types/functions
03/18/12   aro     Added UMID logging
03/13/12   aro     Moved FW State config and DAC cal response handling to RF
03/13/12   aro     Moved tech specific definitions to tech file
03/13/12   aro     Implemented function to de-initilize RF_TASK dispatch
                   for 1x
03/12/12   aro     Function to perform initialization of 1x dispatcher
                   for RF_TASK. Function does tech / module registration with
                   RF_TASK, msgr registration, and semaphore initialization
03/12/12   aro     Handler functions for 1x UMIDs 
03/12/12   aro     Added handler for 1x Temp Comp read 
03/08/12   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "msgr_types.h"
#include "rfm_async_types.h"
#include "rfm_device_types.h"
#include "rfcommon_time_profile.h"

/*----------------------------------------------------------------------------*/
boolean
rf_1x_rf_task_dispatch_init
(
  msgr_client_t *msgr_client, 
  msgr_id_t mailbox 
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_rf_task_dispatch_deinit
(
  msgr_client_t *msgr_client
);

#endif /* RF_1X_RF_TASK_H */


