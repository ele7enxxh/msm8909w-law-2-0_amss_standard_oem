/*!
  @file
  rf_cdma_apps_task.c
 
  @brief
  RF Driver's apps task for cdma technology.

  @details
  This file defines the APIs used for RF_APPS_TASK for cdma technology.
 
  @addtogroup RF_CDMA_APPS_TASK_GROUP
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_apps_task.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/11/12   hdz     Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
/*==============================================================================
   common header file needed for rf_apps_task
==============================================================================*/
#include "rfcommon_msg.h"   
#include "rf_cdma_apps_task.h"

/*==============================================================================
  header files needed for RFA_APPS_1X_TEMP_COMP_READ_CMD
==============================================================================*/
#include "rf_1x_temp_comp.h"

/*==============================================================================
  header files needed for RFA_APPS_HDR_TEMP_COMP_READ_CMD
==============================================================================*/
#include "rf_hdr_temp_comp.h"

/*============================================================================*/
/*!
  @name RF APPS dispatcher

  @brief
  This is the RF APPS Task dispatcher.
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  This is RF APPS task dispatch
 
  @detail
  This dispatch is for RF APPS task within CDMA technology

  @param cmd
  rf apps cmd
 
  @return void
*/ 
void
rf_cdma_apps_dispatch
(
  rf_apps_cmd_type *cmd
)
{
  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR,"rf_cdma_apps_dispatch: Invalid cmd!");

    return;
  }

  switch ( cmd->msgr_hdr.id )
  {
    case RFA_APPS_1X_TEMP_COMP_READ_CMD:
      rf_1x_temp_comp_read_handler( cmd );
      break;

    case RFA_APPS_HDR_TEMP_COMP_READ_CMD:
      rf_hdr_temp_comp_read_handler( cmd );
      break;

    default:
      RF_MSG( RF_ERROR,"rf_cdma_apps_dispatch: Invalid cmd!");
  }

  return;
}
#endif /* FEATURE_CDMA1X */
/*! @} */

