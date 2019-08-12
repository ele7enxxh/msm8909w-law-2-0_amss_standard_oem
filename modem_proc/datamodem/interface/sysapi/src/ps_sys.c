/******************************************************************************
  @file    ps_sys.c
  @brief   

  DESCRIPTION
  This file defines function, variables and data structures common to all
  PS System API module

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ps_sys.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/11   bvd     Created module
===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/
#include "ps_sys.h"
#include "ps_svc.h"
#include "ps_sys_eventi.h"
#include "ps_sys_confi.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_sys_conf_parser.h"
#include "ps_sys_ioctl.h"

ps_crit_sect_type ps_sys_crit_section;

/*Cache for storing DDS*/
static struct
{
  ps_sys_subscription_enum_type   default_data_subs;
} ps_sys_global = {0,};

void ps_sys_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize global event queue
  -------------------------------------------------------------------------*/
  (void) q_init(&ps_sys_eventi_q);

  /*-------------------------------------------------------------------------
    Set the command handlers for sys event and ioct tech callbacks
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler(PS_SYS_EVENT_CMD_HANDLER,
                            ps_sys_eventi_cback_cmd_handler);
  
  /*-------------------------------------------------------------------------
    Allocate memory for temp storage for config data. It would be removed
    once xml implementation is done
  -------------------------------------------------------------------------*/
  ps_sys_confi_alloc_temp_data();
  /*-------------------------------------------------------------------------
    Read persistent configuration settings from EFS and update conf cache
  -------------------------------------------------------------------------*/
  ps_sys_conf_parser_update_cfg_cache();

  /*------------------------------------------------------------------------
    Initialize the global PS critical section
  -------------------------------------------------------------------------*/
  PS_INIT_CRIT_SECTION(&ps_sys_crit_section);

  /*-------------------------------------------------------------------------
    Register for the TECH_ALL ioctls that are handled in the framework
  -------------------------------------------------------------------------*/  
  ps_sys_ioctl_register_common_cbacks();
}

ps_sys_tech_enum_type ps_sys_get_tech_from_rat
(
  ps_sys_rat_ex_enum_type  rat
)
{
  if(rat >= PS_SYS_RAT_EX_3GPP_WCDMA && rat < PS_SYS_RAT_EX_3GPP_MAX)
    return PS_SYS_TECH_3GPP;
  
  if(rat >= PS_SYS_RAT_EX_3GPP2_1X && rat < PS_SYS_RAT_EX_3GPP2_MAX)
    return PS_SYS_TECH_3GPP2;

  if(rat >= PS_SYS_RAT_EX_WLAN && rat < PS_SYS_RAT_EX_WLAN_MAX)
    return PS_SYS_TECH_WLAN;

  return PS_SYS_TECH_ALL;     
} /* ps_sys_get_tech_from_rat */

/** 
  @brief This function sets the DDS(Default data subscription) 
         in the system. Only DS3g module sets this parameter.
*/
void ps_sys_set_default_data_subscription
(
  ps_sys_subscription_enum_type  def_data_subs
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(def_data_subs < PS_SYS_PRIMARY_SUBS ||
     def_data_subs >= PS_SYS_SUBS_MAX)
  {
    ASSERT(0);
  }

  LOG_MSG_INFO1_1("ps_sys_set_default_data_subscription() subs_id %d",
                  def_data_subs);

  PS_ENTER_CRIT_SECTION(&ps_sys_crit_section);
  ps_sys_global.default_data_subs = def_data_subs;
  PS_LEAVE_CRIT_SECTION(&ps_sys_crit_section);
} /* ps_sys_set_default_data_subscription */

/** 
  @brief This function gets the DDS(Default data subscription) 
         in the system
*/
ps_sys_subscription_enum_type ps_sys_get_default_data_subscription
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_ENTER_CRIT_SECTION(&ps_sys_crit_section);
  if(PS_SYS_DEFAULT_SUBS == ps_sys_global.default_data_subs)
  {
    //Can happen only at boot up
    PS_LEAVE_CRIT_SECTION(&ps_sys_crit_section);
    return PS_SYS_PRIMARY_SUBS;
  }

  PS_LEAVE_CRIT_SECTION(&ps_sys_crit_section);
  return ps_sys_global.default_data_subs;
} /* ps_sys_get_default_data_subscription */
 