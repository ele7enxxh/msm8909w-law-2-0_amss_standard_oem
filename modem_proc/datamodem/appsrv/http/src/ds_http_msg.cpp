/*==============================================================================

                              ds_http_msg.cpp

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
#include "ds_http_msg.h"

#include "ds_appsrv_task.h"
/*
struct ds_http_msg_type
{
  http_msg_handler msg_handler;
  uint32*          data_ptr;

  ds_http_msg_type(http_msg_handler handler, uint32* data)
  : msg_handler(handler), data_ptr(data) { }
};


static void ds_http_msg_process(uint32* data_ptr);
*/

void ds_http_init_msg()
{
/*  ds_appsrv_register_process_fcn(DS_APPSRV_MODULE_HTTP, &ds_http_msg_process);*/
}


bool ds_http_msg_post(http_msg_handler handler, uint32* data)
{
  return ds_appsrv_put_cmd_ex(handler, data);
  /*
  ds_http_msg_type* msg_content_ptr = new ds_http_msg_type(handler, data);
  if(NULL == msg_content_ptr)
  {
    MSG_ERROR("ds_http_msg_post: msg_content_ptr failed", 0,0,0);
    return false;
  }

  if(!ds_appsrv_put_cmd(DS_APPSRV_MODULE_HTTP, (uint32*)msg_content_ptr))
  {
    MSG_ERROR("ds_http_msg_post: Failed to put cmd to dstask", 0,0,0);
    delete msg_content_ptr;
    return false;
  }
  return true;
  */
}


/*
void ds_http_msg_process(uint32* data_ptr)
{
  ds_http_msg_type* msg_ptr = (ds_http_msg_type*)data_ptr;

  if (NULL == msg_ptr)
  {
    MSG_ERROR("ds_http_msg_process, illegal msg_ptr", 0,0,0);
    return;
  }

  (msg_ptr->msg_handler)(msg_ptr->data_ptr);
  delete msg_ptr;
}
*/



