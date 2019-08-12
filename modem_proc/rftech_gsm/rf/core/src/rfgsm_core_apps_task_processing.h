#ifndef RFGSM_CORE_APPS_TASK_PROCESSING_H
#define RFGSM_CORE_APPS_TASK_PROCESSING_H

/*!
  @file
  rfgsm_core_apps_task_processing.h
 
  @brief
  RF_APPS_TASK_processing header file for GSM technology.

  @details
  This file exports the definitions and declartions to be used by RF_APPS_TASK
  for GSM technology.
*/

/*==============================================================================

  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_apps_task_processing.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/23/15   bp     Stop further processing of therm_read_rsp if temp. compensation is stopped through GSM Tx Disable.
04/09/13   tsr    Added RF APPS->RF Task MSGR framework for temp comp to make temp comp data thread safe
02/12/12   tsr    Removed Apis to send therm read cmd to GFW 
01/31/13   tsr    Added framework for sending therm read process cmd to GFW  
01/08/13   sr     Initial version

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

  /*GSM temp/therm read cmd*/
  RFGSM_CORE_APPS_TASK_THERM_READ_CMD,

  /*GSM temp comp cmd*/
  RFGSM_CORE_APPS_TASK_TEMP_COMP_CMD,

  /*GSM vbatt read cmd*/
  RFGSM_CORE_APPS_TASK_VBATT_READ_CMD,

  /*vbatt read and apply vbatt comp cmd*/
  RFGSM_CORE_APPS_TASK_VBATT_COMP_CMD,

  /* debug message log cmd*/
  RFGSM_CORE_APPS_TASK_MSG_LOG_CMD,

  /*GSM therm read event handler cleanup*/
  RFGSM_CORE_APPS_TASK_THERM_READ_CLEANUP_CMD,

  /* Add a new cmd below */

  /* End of adding new cmd*/

  RFGSM_CORE_APPS_TASK_MAX_CMD,

} rfgsm_ccore_apps_task_cmd_enum_type;

/*----------------------------------------------------------------------------*/
void rfgsm_core_apps_task_cmd_dispatch (rf_apps_cmd_type *cmd);

errno_enum_type rfgsm_core_apps_task_send_msgr_req( msgr_hdr_struct_type* msg_ptr, 
                                                    uint32 umid, 
                                                    uint32 size );

                                                           
#ifdef __cplusplus
}
#endif

#endif /* RFGSM_CORE_APPS_TASK_PROCESSING_H */
