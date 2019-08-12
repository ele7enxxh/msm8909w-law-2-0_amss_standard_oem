/*==============================================================================

                              ds_appsrv_task.h

GENERAL DESCRIPTION
  Utility functions for appsrv modules to send commands to dstask

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/05/14    ml     Created file/Initial version.
==============================================================================*/
#include "datamodem_variation.h"
#include "data_msg.h"
#include "ds_appsrv_task.h"

#include "ds_atp_api.h"
#include "ds_andsf_api.h"
#include "ds_andsf_query_data_manager.h"
#include "ds_http_api.h"
#include "ds_ssl_api.h"
#include "ds_http_data_mgr.h"
#include "msg.h"
extern "C"
{
  #include "dstask_v.h"
  #include "ds_appsrv_utils.h"
}


/*==============================================================================
                            Internal Object Types
==============================================================================*/
struct ds_appsrv_cmd_msg
{
  ds_appsrv_process_fcn process_fcn;
  uint32*               data_ptr;
};

/*==============================================================================
                          Static Function Declaration
==============================================================================*/
static void ds_appsrv_process_cmd(ds_cmd_type *cmd_ptr);

/*==============================================================================
                          Static Variable Declaration
==============================================================================*/
static ds_appsrv_process_fcn process_fcn_list[DS_APPSRV_MODULE_MAX];



/*==============================================================================
                                  Initializers
==============================================================================*/
void ds_appsrv_cmd_init()
{
  // register appsrv process function to dstask
  ds_cmd_process_register(DS_MODULE_APPSRV, &ds_appsrv_process_cmd);

  for(uint32 i = 0; i < DS_APPSRV_MODULE_MAX; ++i)
  {
    process_fcn_list[i] = NULL;
  }
}


void ds_andsf_rule_init()
{
  ds_andsf_rule_mgr_data_init();
}


void ds_appsrv_init()
{

  /* appsrv module init */
  ds_ssl_init(); /* ssl need to be initialized before http */
  ds_http_init();

  #ifdef FEATURE_DATA_ATP
  ds_atp_init();
  #endif /* FEATURE_DATA_ATP */

  #ifdef FEATURE_DATA_ANDSF
  ds_andsf_init();
  #endif /* FEATURE_DATA_ANDSF*/
}



/*==============================================================================
                          DSTask Command processing
==============================================================================*/
void ds_appsrv_register_process_fcn(
                                    ds_appsrv_module_type module_id,
                                    ds_appsrv_process_fcn process_fcn
                                    )
{
  if(DS_APPSRV_MODULE_MAX > module_id)
    process_fcn_list[module_id] = process_fcn;
}




boolean ds_appsrv_put_cmd(ds_appsrv_module_type module_id, uint32* msg_content_ptr)
{
  if(DS_APPSRV_MODULE_MAX <= module_id)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_appsrv_put_cmd - Invalid module ID");
    return FALSE;
  }

  if(NULL == process_fcn_list[module_id])
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_appsrv_put_cmd - process fcn not registered");
    return FALSE;
  }

  ds_cmd_type* cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_appsrv_cmd_msg));

  if (NULL == cmd_ptr)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_appsrv_put_cmd - alloc ds_cmd_type failed");
    return FALSE;
  }

  cmd_ptr->hdr.cmd_id           = DS_APPSRV_CMD;
  ds_appsrv_cmd_msg* appsrv_msg = (ds_appsrv_cmd_msg*)cmd_ptr->cmd_payload_ptr;
  appsrv_msg->process_fcn       = process_fcn_list[module_id];
  appsrv_msg->data_ptr          = msg_content_ptr;

  ds_put_cmd(cmd_ptr);
  return TRUE;
}




boolean ds_appsrv_put_cmd_ex(ds_appsrv_process_fcn process_fcn, uint32* msg_content_ptr)
{
  if(NULL == process_fcn)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_appsrv_put_cmd_ext - NULL process fcn");
    return FALSE;
  }

  ds_cmd_type* cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_appsrv_cmd_msg));

  if (NULL == cmd_ptr)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_appsrv_put_cmd_ext - alloc ds_cmd_type failed");
    return FALSE;
  }

  cmd_ptr->hdr.cmd_id           = DS_APPSRV_CMD;
  ds_appsrv_cmd_msg* appsrv_msg = (ds_appsrv_cmd_msg*)cmd_ptr->cmd_payload_ptr;
  appsrv_msg->process_fcn       = process_fcn;
  appsrv_msg->data_ptr          = msg_content_ptr;

  ds_put_cmd(cmd_ptr);
  return TRUE;
}




void ds_appsrv_process_cmd(ds_cmd_type *cmd_ptr)
{
  if (NULL == cmd_ptr)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_appsrv_process_cmd - NULL cmd_ptr");
    return;
  }

  if(DS_APPSRV_CMD != cmd_ptr->hdr.cmd_id)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_appsrv_process_cmd - Invalid cmd_id %d", cmd_ptr->hdr.cmd_id);
    return;
  }

  ds_appsrv_cmd_msg* appsrv_msg = (ds_appsrv_cmd_msg*)cmd_ptr->cmd_payload_ptr;

  if (NULL != appsrv_msg)
  {
    if(NULL != appsrv_msg->process_fcn)
    {
      (appsrv_msg->process_fcn)(appsrv_msg->data_ptr);
    }
    else
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_appsrv_process_cmd - NULL process fcn");
    }
  }
  else
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_appsrv_process_cmd - NULL cmd_payload_ptr");
  }
}


