/*==============================================================================

                              ds_http_msg.h

GENERAL DESCRIPTION
  ds_task manager for ds_http

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef __DS_HTTP_MSG_H__
#define __DS_HTTP_MSG_H__

#include "comdef.h"

typedef void (*http_msg_handler)(uint32* data_ptr);

void ds_http_init_msg();

/*==============================================================================
FUNCTION ds_http_msg_post

DESCRIPTION
  Creates and sends a ds_cmd_type to the ds_task queue

PARAMETERS
  [In] handler - Callback function to call
  [In] data    - Data to send to ds_task

RETURN VALUE
  true on success, else false

DEPENDENCIES
  None
==============================================================================*/
bool ds_http_msg_post(http_msg_handler handler, uint32* data);

#endif
