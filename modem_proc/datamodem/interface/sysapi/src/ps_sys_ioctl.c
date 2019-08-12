/******************************************************************************
  @file    ps_sys_ioctl.c
  @brief   

  DESCRIPTION
  This file defines functions, enums and structs used for two modehandlers
  to communicate with each other through ioctls

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2011-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ps_sys_ioctl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ 
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/11   bvd     Created module
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "dserrno.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_sysi.h"
#include "ps_sys_ioctl.h"
#include "ps_dpm.h"


/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/
#define PS_SYS_COMMON_IOCTL_MIN            0
#define PS_SYS_COMMON_IOCTL_EXTERNAL_MAX   25
#define PS_SYS_COMMON_IOCTL_INTERNAL_MIN   PS_SYS_COMMON_IOCTL_EXTERNAL_MAX
#define PS_SYS_COMMON_IOCTL_INTERNAL_MAX   PS_SYS_COMMON_IOCTL_INTERNAL_MIN + 25

static ps_sys_tech_func_type 
          ps_sys_ioctl_tech_func_type[PS_SYS_SUBS_MAX -1][PS_SYS_TECH_MAX];

/* The Common IOCTL handler table is split into 2 ranges.
   From PS_SYS_COMMON_IOCTL_MIN to < PS_SYS_COMMON_IOCTL_EXTERNAL_MAX is for
   external IOCTLs
   From PS_SYS_COMMON_IOCTL_INTERNAL_MIN to < PS_SYS_COMMON_IOCTL_INTERNAL_MAX is for
   internal IOCTLs
   If the number of common IOCTLs exceeds this range, then the macros will have
   be redefined
 */
static ps_sys_tech_func_type 
          ps_sys_common_ioctl_hdlr[PS_SYS_SUBS_MAX -1][PS_SYS_COMMON_IOCTL_INTERNAL_MAX];

int16 ps_sys_ioctl_registeri_cback
(
  ps_sys_tech_enum_type                    tech_type,
  ps_sys_ioctl_tech_cback_f_ptr_type       tech_cback_f_ptr,
  ps_sys_ioctl_tech_ex_cback_f_ptr_type    tech_cback_ex_f_ptr,
  ps_sys_subscription_enum_type            subscription_id,  
  int16                                  * ps_errno
);

/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
uint16 ps_sys_ioctl_get_common_table_index
(
  ps_sys_ioctl_enum_type          ioctl_name
)
{
  uint16 index = 0;
  if ((ioctl_name >= PS_SYS_IOCTL_COMMON_MAX) ||
      (ioctl_name < PS_SYS_IOCTL_COMMON_MIN))
  {
    ASSERT(0);
    return 0;
  }
  /* If external COMMON IOCTL, calculate the index and verify it does not exceed range */
  if (ioctl_name < PS_SYS_IOCTL_COMMON_INTERNAL_MIN)
  {
    index = ioctl_name - PS_SYS_IOCTL_COMMON_MIN;
    if (index >= PS_SYS_COMMON_IOCTL_EXTERNAL_MAX )
    {
      ASSERT(0);
      return 0;
    }
  }
  /* If internal COMMON IOCTL, calculate the index and verify it does not exceed range */
  else
  {
    index = ioctl_name - PS_SYS_IOCTL_COMMON_INTERNAL_MIN + PS_SYS_COMMON_IOCTL_INTERNAL_MIN;
    if (index >= PS_SYS_COMMON_IOCTL_INTERNAL_MAX )
    {
      ASSERT(0);
      return 0;
    }
  }
  return index;
}

static int16 ps_sys_ioctl_common_hdlr
(
  ps_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  ps_sys_subscription_enum_type   subscription_id,
  int16                         * ps_errno
)
{
  ps_sys_ioctl_clat_support_type * is_clat_supported_ptr;
  int16                                 ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(ioctl_name)
  {
    case PS_SYS_IOCTL_IS_CLAT_SUPPORTED_BY_PROFILE:
    {  
       is_clat_supported_ptr = 
         (ps_sys_ioctl_clat_support_type *)arg_val_ptr;

       if (0 == ps_sys_ioctl_ex(PS_SYS_TECH_3GPP, 
                                ioctl_name, 
                                subscription_id, 
                                arg_val_ptr, 
                                ps_errno))
       {
         if (TRUE == is_clat_supported_ptr->is_clat_supported)
         {
           return 0;
         }
       }       

       is_clat_supported_ptr = 
         (ps_sys_ioctl_clat_support_type *)arg_val_ptr;  
         
       if (0 == ps_sys_ioctl_ex(PS_SYS_TECH_3GPP2, 
                                ioctl_name, 
                                subscription_id, 
                                arg_val_ptr, 
                                ps_errno))
       {
         if (TRUE == is_clat_supported_ptr->is_clat_supported)
         {
           return 0;
         }
       }

       is_clat_supported_ptr = 
         (ps_sys_ioctl_clat_support_type *)arg_val_ptr;  
         
       if (0 == ps_sys_ioctl_ex(PS_SYS_TECH_WLAN, 
                                ioctl_name, 
                                subscription_id, 
                                arg_val_ptr, 
                                ps_errno))
       {
         if (TRUE == is_clat_supported_ptr->is_clat_supported)
         {
           return 0;
         }
       }   
       
       is_clat_supported_ptr->is_clat_supported = FALSE;
       ret_val = 0;
       break;
    }   
    
    default:
    {
      LOG_MSG_ERROR_0("IOCTL not handled");
      break;
    }
  }

  return ret_val;
} /* ps_sys_ioctl_common_hdlr */

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
void ps_sys_ioctl_register_common_cbacks
(
  void
)
{
  ps_sys_subscription_enum_type       subscription;
  int16                               ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*--------------------------------------------------------------------
    Register for all the tech all ioctl callbacks that are handled in 
    framework
  ---------------------------------------------------------------------*/   
  for(subscription = 0; subscription < PS_SYS_SUBS_MAX; subscription++)
  {
    ps_sys_ioctl_register_ioctl_cback_ex(PS_SYS_IOCTL_IS_CLAT_SUPPORTED_BY_PROFILE,
                                         ps_sys_ioctl_common_hdlr,
                                         subscription,
                                         &ps_errno);
  }                                       
                                       
} /* ps_sys_ioctl_register_common_cbacks */

int16 ps_sys_ioctl
(
  ps_sys_tech_enum_type           tech_type,
  ps_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  int16                         * ps_errno
)
{
  return ps_sys_ioctl_ex (tech_type,
                          ioctl_name,
                          PS_SYS_DEFAULT_SUBS,
                          arg_val_ptr,
                          ps_errno);
} /* ps_sys_ioctl() */


int16 ps_sys_ioctl_ex
(
  ps_sys_tech_enum_type           tech_type,
  ps_sys_ioctl_enum_type          ioctl_name,
  ps_sys_subscription_enum_type   subscription_id,
  void                          * arg_val_ptr,
  int16                         * ps_errno
)
{
  int16 result;
  int16 ret_err;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("ps_sys_ioctl_ex(): tech %d ioctl %d argval_ptr 0x%p",
                           tech_type, ioctl_name, arg_val_ptr);


  if (ps_errno == NULL)
  {
    LOG_MSG_INVALID_INPUT_0("ps_sys_ioctl_ex(): Invalid argument ps_errno");
    return -1;
  }
  
  if (TRUE == ds_sysi_is_modem_reset_in_progress())
  {
    ret_err = DS_EINVAL;
    LOG_MSG_ERROR_2("ps_sys_ioctl_ex(): Modem restart in progress ioctl %d tech %d",
                    ioctl_name, tech_type);
    goto bail;
  }
  
  if (subscription_id >= PS_SYS_SUBS_MAX || 
      subscription_id < PS_SYS_DEFAULT_SUBS)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("ps_sys_ioctl_ex(): Invalid subscription_id %d",
                           subscription_id);
    return -1;
  }

  if (PS_SYS_DEFAULT_SUBS == subscription_id )
  {
    subscription_id = ps_sys_get_default_data_subscription();
  }

  if(ioctl_name >= PS_SYS_IOCTL_MAX)
  {
    ret_err = DS_EINVAL;
    LOG_MSG_INVALID_INPUT_2("ps_sys_ioctl_ex(): Invalid ioctl %d for tech %d",
                            ioctl_name, tech_type);
    goto bail;
  }
  
  if(tech_type >= PS_SYS_TECH_MAX || tech_type < PS_SYS_TECH_MIN)
  {
    ret_err = DS_EINVAL;
    LOG_MSG_INVALID_INPUT_2("ps_sys_ioctl_ex(): Invalid tech type %d",
                            ioctl_name, tech_type);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Make sure that IOCTL is valid
  -------------------------------------------------------------------------*/
  if (ioctl_name < PS_SYS_IOCTL_COMMON_MIN ||
      ioctl_name >= PS_SYS_IOCTL_COMMON_MAX)
  {
    switch (tech_type)
    {
      case PS_SYS_TECH_3GPP:
      {  
        if (ioctl_name >= PS_SYS_IOCTL_3GPP_MAX || 
            ioctl_name < PS_SYS_IOCTL_3GPP_MIN)
        {
          ret_err = DS_EINVAL;
          LOG_MSG_INVALID_INPUT_1("ps_sys_ioctl_ex(): 3gpp ioctl %d", ioctl_name);
          goto bail;
        }
        break;
      }

      case PS_SYS_TECH_3GPP2:
      {  
        if (ioctl_name >= PS_SYS_IOCTL_3GPP2_MAX || 
            ioctl_name < PS_SYS_IOCTL_3GPP2_MIN)
        {
          ret_err = DS_EINVAL;
          LOG_MSG_INVALID_INPUT_1("ps_sys_ioctl_ex(): 3gpp2 ioctl %d",
                                  ioctl_name);
          goto bail;
        }
        break;
      }  

      case PS_SYS_TECH_WLAN:
      {
        if (ioctl_name >= PS_SYS_IOCTL_WLAN_MAX ||
            ioctl_name < PS_SYS_IOCTL_WLAN_MIN)
        {
          ret_err = DS_EINVAL;
          LOG_MSG_INVALID_INPUT_1("ps_sys_ioctl_ex(): wlan ioctl %d",
                                  ioctl_name);
          goto bail;
        }
        break;
      }

      case PS_SYS_TECH_ALL:
      {  
        if (ioctl_name >= PS_SYS_IOCTL_COMMON_MAX || 
            ioctl_name < PS_SYS_IOCTL_COMMON_MIN)
        {
          ret_err = DS_EINVAL;
          LOG_MSG_INVALID_INPUT_1("ps_sys_ioctl_ex(): comm ioctl %d", ioctl_name);
          goto bail;
        }
        break;
      }

      default:
      {     
        ret_err = DS_EINVAL;    
        LOG_MSG_INVALID_INPUT_1("ps_sys_ioctl_ex(): tech %d", tech_type);
        goto bail;
      }
    }
  }
  
  if( (ioctl_name == PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_SW) || 
      (ioctl_name == PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_HW) )
  {
  
    if (FALSE == ps_dpm_data_path_switch_allowed (PS_DPM_DISABLE_HW_PATH_SYS_IOCTL))
    {
      ret_err = DS_EINVAL;
      goto bail; 
    }
    
    tech_type = PS_SYS_TECH_3GPP;
  }
  
  switch(ioctl_name)
  {
    case PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_SW:
    {
      ps_dpm_suspend_hw_path();
      return 0;
    }
    
    case PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_HW:
    {
      ps_dpm_resume_hw_path();
      return 0;
    }
    
    default:
      break;
  }
  
  if ((ioctl_name >= PS_SYS_IOCTL_COMMON_MIN ) &&
      (ioctl_name < PS_SYS_IOCTL_COMMON_MAX ) &&
      (tech_type == PS_SYS_TECH_ALL)) 
  {
    if (ps_sys_common_ioctl_hdlr[subscription_id - 1]
    [ps_sys_ioctl_get_common_table_index(ioctl_name)].tech_ex_f_ptr != NULL)
    {
      result = ps_sys_common_ioctl_hdlr[subscription_id - 1]
       [ps_sys_ioctl_get_common_table_index(ioctl_name)].tech_ex_f_ptr
        ( ioctl_name, arg_val_ptr, subscription_id, ps_errno );
      return result;
    }
    else
    {
      ret_err = DS_EOPNOTSUPP;
      goto bail;
    }
  }
  
  if (ps_sys_ioctl_tech_func_type[subscription_id - 1][tech_type]. \
           tech_ex_f_ptr != NULL)
  {    
    /* Route it to specific modehandler. MH should use this extended callback
       to register */
  result =  
    ps_sys_ioctl_tech_func_type[subscription_id - 1][tech_type].tech_ex_f_ptr 
    ( ioctl_name, arg_val_ptr, subscription_id, ps_errno );
  }
  else
  {
    /* Need not check tech_f_ptr is valid since without subs id the callback
       behavior is undefined for multi sim scenarios.*/
    ret_err = DS_EOPNOTSUPP;   
    LOG_MSG_ERROR_1("ps_sys_ioctl_ex(): Tech %d has not registered function",
                    tech_type);
    goto bail;
  }
  return result;

bail:
  *ps_errno = ret_err;
  return -1;
} /* ps_sys_ioctl() */

int16 ps_sys_ioctl_register_tech_cback
(
  ps_sys_tech_enum_type                 tech_type,
  ps_sys_ioctl_tech_cback_f_ptr_type    tech_cback_f_ptr,
  int16                               * ps_errno
)
{
  return ps_sys_ioctl_registeri_cback(tech_type,
                                      tech_cback_f_ptr,
                                      NULL,
                                      PS_SYS_DEFAULT_SUBS,
                                      ps_errno);
                                              
} /* ps_sys_ioctl_register_tech_cback() */

int16 ps_sys_ioctl_register_tech_cback_ex
(
  ps_sys_tech_enum_type                    tech_type,
  ps_sys_ioctl_tech_ex_cback_f_ptr_type    tech_cback_ex_f_ptr,
  ps_sys_subscription_enum_type            subscription_id,  
  int16                                  * ps_errno
)
{
  return ps_sys_ioctl_registeri_cback(tech_type,
                                      NULL,
                                      tech_cback_ex_f_ptr,
                                      subscription_id,
                                      ps_errno);

} /* ps_sys_ioctl_register_tech_cback_ex() */

int16 ps_sys_ioctl_registeri_cback
(
  ps_sys_tech_enum_type                    tech_type,
  ps_sys_ioctl_tech_cback_f_ptr_type       tech_cback_f_ptr,
  ps_sys_ioctl_tech_ex_cback_f_ptr_type    tech_cback_ex_f_ptr,
  ps_sys_subscription_enum_type            subscription_id,  
  int16                                  * ps_errno
)
{
  LOG_MSG_FUNCTION_ENTRY("ps_sys_ioctl_registeri_cback(): Tech %d, "
                         "CB 0x%p", tech_type, tech_cback_f_ptr, 0 );

  if (TRUE == ds_sysi_is_modem_reset_in_progress())
  {
    *ps_errno = DS_EINVAL;
    LOG_MSG_ERROR_2("ps_sys_ioctl_registeri_cback():Modem restart in progress"
                    " tech %d cback_f_ptr 0x%p", tech_type, tech_cback_f_ptr);
    return -1;
  }

  if (ps_errno == NULL)
  {
    LOG_MSG_INVALID_INPUT_0("ps_sys_ioctl_registeri_cback(): Invalid arg "
                            "ps_errno");
    return -1;
  }

  if (subscription_id >= PS_SYS_SUBS_MAX || 
      subscription_id < PS_SYS_DEFAULT_SUBS)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("ps_sys_ioctl_registeri_cback(): Invalid"
                            " subscription_id %d",subscription_id);
    return -1;
  }

  if (PS_SYS_DEFAULT_SUBS == subscription_id )
  {
    subscription_id = ps_sys_get_default_data_subscription();
  }

  if (tech_type >= PS_SYS_TECH_MAX || tech_type == PS_SYS_TECH_ALL )
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_0("ps_sys_ioctl_registeri_cback(): Invalid arg "
                            "passed");
    return -1;
  }

  if ( (tech_cback_f_ptr != NULL && tech_cback_ex_f_ptr != NULL) ||
       (tech_cback_f_ptr == NULL && tech_cback_ex_f_ptr == NULL))
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_0("ps_sys_ioctl_registeri_cback(): No cback "
                            "or both cback cannot be resgistered");
    return -1;

  }

  if (tech_cback_f_ptr != NULL )
  {
    ps_sys_ioctl_tech_func_type[subscription_id - 1][tech_type].tech_f_ptr = 
      tech_cback_f_ptr;
  }
  else 
  {
    ps_sys_ioctl_tech_func_type[subscription_id - 1][tech_type].tech_ex_f_ptr = 
      tech_cback_ex_f_ptr;
  }

  return 0;

} /* ps_sys_ioctl_registeri_cback */

int16 ps_sys_ioctl_register_ioctl_cback_ex
(
  ps_sys_ioctl_enum_type                   ioctl_name,
  ps_sys_ioctl_tech_ex_cback_f_ptr_type    tech_cback_ex_f_ptr,
  ps_sys_subscription_enum_type            subscription_id,  
  int16                                  * ps_errno
)
{
  LOG_MSG_FUNCTION_ENTRY("ps_sys_ioctl_register_ioctl_cback_ex(): Subscription %d, "
                         "CB 0x%p IOCTL %d", subscription_id, tech_cback_ex_f_ptr, ioctl_name );

  if (TRUE == ds_sysi_is_modem_reset_in_progress())
  {
    *ps_errno = DS_EINVAL;
    LOG_MSG_ERROR_0("ps_sys_ioctl_register_ioctl_cback_ex():Modem restart in progress");
    return -1;
  }

  if (ps_errno == NULL)
  {
    LOG_MSG_INVALID_INPUT_0("ps_sys_ioctl_register_ioctl_cback_ex(): Invalid arg "
                            "ps_errno");
    return -1;
  }

  if (subscription_id >= PS_SYS_SUBS_MAX || 
      subscription_id < PS_SYS_DEFAULT_SUBS)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("ps_sys_ioctl_register_ioctl_cback_ex(): Invalid"
                            " subscription_id %d",subscription_id);
    return -1;
  }

  if (ioctl_name >= PS_SYS_IOCTL_COMMON_MAX || 
      ioctl_name < PS_SYS_IOCTL_COMMON_MIN)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_1("IOCTL not in COMMON range %d",ioctl_name);
    return -1;
  }

  if (PS_SYS_DEFAULT_SUBS == subscription_id )
  {
    subscription_id = ps_sys_get_default_data_subscription();
  }

  if ( tech_cback_ex_f_ptr == NULL)
  {
    *ps_errno = DS_EFAULT;
    LOG_MSG_INVALID_INPUT_0("cback ptr is NULL");
    return -1;

  }

  ps_sys_common_ioctl_hdlr[subscription_id - 1]
    [ps_sys_ioctl_get_common_table_index(ioctl_name)].tech_ex_f_ptr = 
      tech_cback_ex_f_ptr;

  return 0;

} /* ps_sys_ioctl_register_ioctl_cback_ex */

