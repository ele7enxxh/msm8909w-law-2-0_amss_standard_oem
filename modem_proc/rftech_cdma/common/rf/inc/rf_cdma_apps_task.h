#ifndef RF_CDMA_APPS_TASK_H
#define RF_CDMA_APPS_TASK_H

/*!
  @file
  rf_cdma_apps_task.h
 
  @brief
  RF CDMA APPS TASK header file for CDMA technology.

  @details
  This file exports the definitions and declartions to be used by RF_APPS_TASK
  for CDMA technology.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_apps_task.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/11/12   hdz     Initial version

==============================================================================*/

#include "rf_apps_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*
  @brief
  This enum defines all cmds that need rf_apps_task to dispatch. To add a new
  cmd to rf_apps_task, one need to add a new cmd in this enum, and then add
  the corresponding cmd handler in rf_apps_cmd_dispatch().
*/
typedef enum
{
  RF_APPS_MIN_CMD,
 
  /*1x temp comp cmd*/
  RFA_APPS_1X_TEMP_COMP_READ_CMD,

  /*hdr temp comp cmd*/
  RFA_APPS_HDR_TEMP_COMP_READ_CMD,

  /* Add a new cmd below */

  /* End of adding new cmd*/

  RF_APPS_1XHDR_MAX_CMD,

} rf_apps_cdma_cmd_enum_type;

/*----------------------------------------------------------------------------*/
void
rf_cdma_apps_dispatch
(
  rf_apps_cmd_type *cmd
);

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_APPS_TASK_H */

