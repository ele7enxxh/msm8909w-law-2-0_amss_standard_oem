/******************************************************************************
  @file    ds_sys_ioctl.c
  @brief   

  DESCRIPTION
  This file defines functions, enums and structs used for two modehandlers
  to communicate with each other through ioctls

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ds_sys_ioctl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ 
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/11   bvd     Created module
===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/
#include "comdef.h"
#include "queue.h"
#include "dserrno.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_sys_ioctl.h"

int16 ds_sys_ioctl
(
  ds_sys_tech_enum_type           tech_type,
  ds_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  int16                         * dss_errno
)
{
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  LOG_MSG_FUNCTION_ENTRY_1("ds_sys_ioctl(): "
    "caller 0x%p", caller_ptr);
  return ds_sys_ioctl_ex (tech_type,
                          ioctl_name,
                          DS_SYS_DEFAULT_SUBS,
                          arg_val_ptr,
                          dss_errno);
}

int16 ds_sys_ioctl_ex
(
  ds_sys_tech_enum_type           tech_type,
  ds_sys_ioctl_enum_type          ioctl_name,
  ds_sys_subscription_enum_type   subscription_id,
  void                          * arg_val_ptr,
  int16                         * dss_errno
)
{
  int16 result;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_4("ds_sys_ioctl_ex(): "
    "tech %d ioctl %d arg_val 0x%p, caller 0x%p",
    tech_type, ioctl_name, arg_val_ptr, caller_ptr);

  if (subscription_id >= DS_SYS_SUBS_MAX || 
      subscription_id < DS_SYS_DEFAULT_SUBS)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("ds_sys_ioctl_ex(): Invalid subscription_id %d",
                           subscription_id);
    return -1;
  }                           

  if (DS_SYS_DEFAULT_SUBS == subscription_id )
  {
    subscription_id = (ds_sys_subscription_enum_type)ps_sys_get_default_data_subscription();
  }
  
  if (ioctl_name < DS_SYS_IOCTL_COMMON_MIN ||
      ioctl_name >= DS_SYS_IOCTL_COMMON_MAX)
  {
    switch (tech_type)
    {
      case DS_SYS_TECH_3GPP:
      {  
        if (ioctl_name >= DS_SYS_IOCTL_3GPP_MAX || 
            ioctl_name < DS_SYS_IOCTL_3GPP_MIN)
        {
          LOG_MSG_INVALID_INPUT_1("ds_sys_ioctl_ex(): 3gpp ioctl %d", ioctl_name);
          goto bail;
        }
        break;
      }

      case DS_SYS_TECH_3GPP2:
      {  
        if (ioctl_name >= DS_SYS_IOCTL_3GPP2_MAX || 
            ioctl_name < DS_SYS_IOCTL_3GPP2_MIN)
        {
          LOG_MSG_INVALID_INPUT_1("ds_sys_ioctl_ex(): 3gpp2 ioctl %d",
                                  ioctl_name);
          goto bail;
        }
        break;
      }  

      case DS_SYS_TECH_WLAN:
      {  
        if (ioctl_name >= DS_SYS_IOCTL_WLAN_MAX || 
            ioctl_name < DS_SYS_IOCTL_WLAN_MIN)
        {
          LOG_MSG_INVALID_INPUT_1("ds_sys_ioctl_ex(): wlan ioctl %d",
                                  ioctl_name);
          goto bail;
        }
        break;
      }  

      case DS_SYS_TECH_ALL:
      {  
        if (ioctl_name >= DS_SYS_IOCTL_COMMON_MAX || 
            ioctl_name < DS_SYS_IOCTL_COMMON_MIN)
        {
          LOG_MSG_INVALID_INPUT_1("ds_sys_ioctl_ex(): comm ioctl %d", ioctl_name);
          goto bail;
        }
        break;
      }

      default:
      {     
        LOG_MSG_INVALID_INPUT_1("ds_sys_ioctl_ex(): tech %d", tech_type);
        goto bail;
      }
    }
  }

  result =  ps_sys_ioctl_ex((ps_sys_tech_enum_type)tech_type,
                         (ps_sys_ioctl_enum_type)ioctl_name,
                         (ps_sys_subscription_enum_type)subscription_id,
                         arg_val_ptr,
                         dss_errno);

  return result;

bail:
  *dss_errno = DS_EFAULT;
  return -1;
}
