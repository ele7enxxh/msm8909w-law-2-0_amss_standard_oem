/******************************************************************************
  @file:  loc_task.h
  @brief: Location Middleware task

  DESCRIPTION
  This module handles loc middleware taks startup and processes the command queue
  for the loc mw task

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ------------------------------------------------------- 
02/08/13   ssu      Move to the DOG Heartbeat mechanism on DIME and beyond.  
04/12/10   ns       change loc_mw task to dynamically register with dog
04/07/10   ns       Set the loc_mw_task_started flag to indicate that 
                    loc mw task is up

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_task.h#1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/

#ifndef LOC_TASK_H

#define LOC_TASK_H

#include "queue.h"

typedef enum
{  
   LOC_CMD_TYPE_PD_SCHEDULE,
   LOC_CMD_TYPE_REPORT_POSITION,
   LOC_CMD_TYPE_NOTIFY_CLIENT,
   LOC_CMD_TYPE_INJECT_XTRA_DATA,
   LOC_CMD_TYPE_SILENTLY_DENY_NI_REQUEST,
   LOC_CMD_TYPE_SERIALIZE_IND,
   LOC_CMD_TYPE_INIT_QMI_LOC
} loc_cmd_type_e_type;

typedef struct
{
   loc_event_mask_type        event_type;
   loc_event_payload_u_type   event_data; 
} loc_cmd_nty_client_s_type;

typedef struct loc_cmd_s  
{
   q_link_type                link;
   loc_client_handle_type     client_handle;
   loc_cmd_type_e_type        cmd_type;

   // data must be allocated with loc_calloc and freed by loc_free
   // if not freed and set to NULL explicitly by the receiver, 
   // loc_middleware_proc in loc_task would call loc_free for it
   void                       *ptr_data;
} loc_cmd_s_type;

extern boolean loc_middleware_queue_cmd(const loc_cmd_s_type  * const ptr_cmd);

boolean loc_middleware_initialized (void);

void * loc_calloc (const size_t block_size);
void loc_free (void * const ptr);

/*===========================================================================
FUNCTION loc_middleware_queue_ipc

DESCRIPTION
   This funciton is used to queue the IPC requests to be sent to the LOC_TASK
   from the different LOC_MW modules.   
   
DEPENDENCIES
   N/A

RETURN VALUE
   TRUE   : successful
   FALSE  : failed

SIDE EFFECTS
   N/A
===========================================================================*/
boolean loc_middleware_queue_ipc(loc_client_handle_type client_handle_type, 
                                 loc_cmd_type_e_type cmd_type,
                                 void *ptr_client_data);

#endif // LOC_TASK_H
