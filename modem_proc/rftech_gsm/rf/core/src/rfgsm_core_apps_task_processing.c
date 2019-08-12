/*!
  @file
  rfgsm_core_apps_task_processing.c
 
  @brief
  RF GSM Driver's apps task processing.

  @details
  This file defines the APIs used for RF_APPS_TASK for GSM technology.
 
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_apps_task_processing.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/23/15   bp     Stop further processing of therm_read_rsp if temp. compensation is stopped through GSM Tx Disable.
02/05/13   par    Few F3 cleanup
04/09/13   tsr    Added RF APPS->RF Task MSGR framework for temp comp to make temp comp data thread safe
02/12/12   tsr    Removed Apis to send therm read cmd to GFW 
01/31/13   tsr    Added framework for sending therm read process cmd to GFW 
01/08/13   sr     Initial version

==============================================================================*/

/*==============================================================================
   common header file needed for rf_apps_task
==============================================================================*/
#include "rfcommon_msg.h"  
#include "rfgsm_core_apps_task_processing.h"
#include "rfgsm_core.h"
#include "rfgsm_msg.h"
#include "rfgsm_core_temp_comp.h"
#include "rfgsm_core_vbatt_comp.h"


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
  This dispatch is for RF APPS task within GSM technology

  @param cmd
  rf apps cmd
 
  @return void
*/ 
void rfgsm_core_apps_task_cmd_dispatch(rf_apps_cmd_type *cmd)
{
  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR,"rfgsm_core_apps_task_dispatch: Invalid cmd!");
    return;
  }

  RF_MSG_1( RF_LOW,"rfgsm_core_apps_task_dispatch: cmd_id = %d", cmd->msgr_hdr.id);

  switch ( cmd->msgr_hdr.id )
  {
    case RFGSM_CORE_APPS_TASK_THERM_READ_CMD:
      rfgsm_core_therm_read_handler( cmd );
      break;

    case RFGSM_CORE_APPS_TASK_TEMP_COMP_CMD:
      rfgsm_core_temp_comp_handler( cmd );
      break;

    case RFGSM_CORE_APPS_TASK_MSG_LOG_CMD:
      rfgsm_core_msg_log_handler( cmd );
      break;

    case RFGSM_CORE_APPS_TASK_VBATT_READ_CMD:
      rfgsm_core_vbatt_read_handler( cmd );
      break;

    case RFGSM_CORE_APPS_TASK_VBATT_COMP_CMD:
      rfgsm_core_vbatt_comp_handler( cmd );
      break;

    case RFGSM_CORE_APPS_TASK_THERM_READ_CLEANUP_CMD:
      rfgsm_core_therm_read_event_cleanup_handler( cmd );
      break;

    default:
      RF_MSG_1( RF_ERROR,"rfgsm_core_apps_task_dispatch: Invalid cmd(0x%x)!", cmd->msgr_hdr.id);
  }

  return;
}

/*===========================================================================
  FUNCTION:  rfgsm_core_apps_task_send_msgr_req()
===========================================================================*/
/*! @fn rfgsm_core_apps_task_send_msgr_req(msgr_hdr_struct_type* msg_ptr, uint32 umid, uint32 size){}
 *  @brief This function is called by any function that requires to send a 
 *  REQ MSG from RF apps task to RF task.
 *  @param msg_ptr a pointer to message router header structure type
 *  @param umid a uint32 of the Unique Message ID
 *  @param size the payload size
 *  @return errno_enum_type.
 * 
 */
errno_enum_type rfgsm_core_apps_task_send_msgr_req( msgr_hdr_struct_type* msg_ptr, 
                                     uint32 umid, 
                                     uint32 size )
{
  errno_enum_type msgr_send_status = E_SUCCESS;
  
  if( msg_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "MSGR Send NULL Message UMID 0x%X, error_no %d",umid);
    return (E_BAD_ADDRESS);
  }

  /* Send the CNF MSG back here */
  msgr_init_hdr( msg_ptr, MSGR_RFA_RF_GSM, umid );
    
  msgr_send_status = msgr_send( msg_ptr, size );
  return msgr_send_status;
}

/*! @} */
