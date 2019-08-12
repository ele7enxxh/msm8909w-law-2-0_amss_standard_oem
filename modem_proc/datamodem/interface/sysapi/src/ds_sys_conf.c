/******************************************************************************
  @file    ds_sys_conf.c
  @brief   Data Service System Configuration implementation

  DESCRIPTION
  This file contains implementation of Data Service System Configuration API.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2011-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ds_sys_conf.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ 

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/11   bvd     Created module
===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/
#include "ds_sysi.h"
#include "ds_sys_conf.h"
#include "ps_sys_conf.h"
#include "dserrno.h"
#include "ds_Utils_DebugMsg.h"

int16 ds_sys_conf_get
(
  ds_sys_tech_enum_type              tech_type,
  ds_sys_conf_enum_type              conf_name,
  void                             * argval_ptr,
  int16                            * dss_errno
)
{
  return ds_sys_conf_get_ex(tech_type,
                            conf_name,
                            DS_SYS_PRIMARY_SUBS,
                            argval_ptr,
                            dss_errno);
} /* ds_sys_conf_get() */

int16 ds_sys_conf_get_ex
(
  ds_sys_tech_enum_type              tech_type,
  ds_sys_conf_enum_type              conf_name,
  ds_sys_subscription_enum_type      subscription_id,
  void                             * argval_ptr,
  int16                            * dss_errno
)
{
  uint16 ret;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  void *caller_parent_ptr = DS_UTILS_CALLER_ADDRESS(1);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_4("ds_sys_conf_get_ex(): "
    "conf_name:%d, argval_ptr:0x%p, caller 0x%p, parent 0x%p",
    conf_name, argval_ptr, caller_ptr, caller_parent_ptr);

  if (TRUE == ds_sysi_is_modem_reset_in_progress())
  {
    *dss_errno = DS_EINVAL;
    LOG_MSG_INVALID_INPUT_1("ds_sys_conf_get_ex() Modem restart in progress."
                            " conf_name:%d", conf_name);
    return -1;
  }

  if (dss_errno == NULL)
  {
    LOG_MSG_INVALID_INPUT_0("ds_sys_conf_get_ex(): Invalid param dss_errno");
    return -1;
  }

  if (tech_type >= DS_SYS_TECH_MAX || conf_name >= DS_SYS_CONF_MAX ||
     argval_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_0("ds_sys_conf_get_ex(): Invalid parameter passed");
    return -1;
  }
  
  if (subscription_id >= DS_SYS_SUBS_MAX || 
      subscription_id < DS_SYS_DEFAULT_SUBS)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("ds_sys_conf_get_ex(): Invalid subscription_id %d",
                            subscription_id);
    return -1;
  }  

  if (DS_SYS_DEFAULT_SUBS == subscription_id )
  {
    subscription_id = (ds_sys_subscription_enum_type)ps_sys_get_default_data_subscription();
  }

  ret = ps_sys_conf_get_ex((ps_sys_tech_enum_type)tech_type, 
                           (ps_sys_conf_enum_type)conf_name, 
                           (ps_sys_subscription_enum_type)subscription_id,
                           argval_ptr, 
                           dss_errno);

  return ret;
} /* ds_sys_conf_get_ex() */

int16 ds_sys_conf_set
(
  ds_sys_tech_enum_type              tech_type,
  ds_sys_conf_enum_type              conf_name,
  void                             * argval_ptr,
  int16                            * dss_errno
)
{

  return ds_sys_conf_set_ex(tech_type,
                            conf_name,
                            DS_SYS_PRIMARY_SUBS,
                            argval_ptr,
                            dss_errno);
} /* ds_sys_conf_set() */

int16 ds_sys_conf_set_ex
(
  ds_sys_tech_enum_type              tech_type,
  ds_sys_conf_enum_type              conf_name,
  ds_sys_subscription_enum_type      subscription_id,
  void                             * argval_ptr,
  int16                            * dss_errno
)
{
  uint16 ret;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  void *caller_parent_ptr = DS_UTILS_CALLER_ADDRESS(1);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_5("ds_sys_conf_set_ex(): "
    "tech %d, conf %d  arg 0x%p, caller 0x%p, parent 0x%p",
    tech_type, conf_name, argval_ptr, caller_ptr,
    caller_parent_ptr);

  if (dss_errno == NULL)
  {
    LOG_MSG_INVALID_INPUT_0("ds_sys_conf_set_ex(): Invalid parameter dss_errno");
    return -1;
  }

  if (tech_type >= DS_SYS_TECH_MAX || conf_name >= DS_SYS_CONF_MAX ||
     argval_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_0("ds_sys_conf_set_ex(): Invalid parameter passed");
    return -1;
  }

  if (subscription_id >= DS_SYS_SUBS_MAX || 
      subscription_id < DS_SYS_DEFAULT_SUBS)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("ds_sys_conf_set_ex(): Invalid subscription_id %d",
                            subscription_id);
    return -1;
  }

  if (DS_SYS_DEFAULT_SUBS == subscription_id )
  {
    subscription_id = (ds_sys_subscription_enum_type)ps_sys_get_default_data_subscription();
  }

  if ((conf_name == DS_SYS_CONF_3GPP_ROAMING_DISALLOWED_INFO) ||
      (conf_name == DS_SYS_CONF_SYSTEM_STATUS) ||
      (conf_name == DS_SYS_CONF_SYSTEM_STATUS_EX) ||
      (conf_name == DS_SYS_CONF_ROAMING_STATUS_INFO))
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("ds_sys_conf_set_ex(): Set operation for conf %d"
                            "is not supported",
                            conf_name);
    return -1;
  }

  ret = ps_sys_conf_set_ex((ps_sys_tech_enum_type)tech_type, 
                           (ps_sys_conf_enum_type)conf_name, 
                           (ps_sys_subscription_enum_type)subscription_id,
                           argval_ptr, 
                           dss_errno);
  return ret;
} /* ds_sys_conf_set_ex() */
