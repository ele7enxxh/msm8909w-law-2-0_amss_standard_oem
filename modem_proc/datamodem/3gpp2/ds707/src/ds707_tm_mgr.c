/*===========================================================================

                         D S 3 G P P 2 _ T M _ M G R
GENERAL DESCRIPTION
  This file contains the congestion control manager functionality.
  Currently it runs in CFCM task context.
 
 Copyright (c) 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk.c_v   1.2   19 Nov 2002 19:23:50   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_tm_mgr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
when       who        what, where, why
--------   ---        -------------------------------------------------------
10/09/08   jc         First version of file.
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "task.h"
#include "queue.h"
#include "rex.h"
#include "ds707_ccmi.h"
#include "ds707_cback.h"
#include "ds707_ccm_log.h"
#include "ds707_pkt_mgr.h"
#include "rcinit_rex.h"
#include "ds3geventmgr.h"
#include "ds707_tm_mgr.h"
/*===========================================================================
                            VARIABLES
===========================================================================*/
/*--------------------------------------------------------------------------
  Variables for storing the current thermal mitigation level.
----------------------------------------------------------------------------*/
static ds3geventmgr_th_mit_action_type ds707_tm_mgr_current_action;

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS3GPP22_TM_MGR_POWERUP_INIT

DESCRIPTION   This function performs power-up initialization of 3GPP2 
              Thermal Mitigation Manager.This includes initialization of
              some parameters that are used to store current TM state and
              registers with DS3G for Thermal mitigation events.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_tm_mgr_powerup_init
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds707_tm_mgr_current_action = DS3GEVENTMGR_TH_MIT_MIN;
  
}/* ds707_tm_mgr_powerup_init() */

/*===========================================================================
FUNCTION      DS707_TM_MGR_PROCESS_THERM_MIT_CMD

DESCRIPTION   This function is called when processing the TM command posted 
              internally to 3GPP2 MH after receiving a call back from DS3G. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Static variable Current action is set to the new value 
              and current calls are torn down gracefully.
===========================================================================*/
void ds707_tm_mgr_process_therm_mit_cmd
(
   ds_cmd_type *ds_cmd_ptr
)
{
  ds707_TM_mgr_therm_mit_notify_cmd_info_type* cmd_info; 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("ds_cmd_ptr or cmd_payload_ptr is NULL");
    return; 
  }
   
  cmd_info = (ds707_TM_mgr_therm_mit_notify_cmd_info_type *)
                                            (ds_cmd_ptr->cmd_payload_ptr);
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "ds707_tm_mgr_process_therm_mit_cmd()"
                                 "Processing Thermal mitigation command"
                                 "Old mitigation action: %d , "
                                 "New Mitigation action: %d",
                                  ds707_tm_mgr_current_action,
                                  cmd_info->action);
  /*-------------------------------------------------------------------------- 
    Events DS3GEVENTMGR_TH_MIT_ALLOW_ONLY_IMS_CALLS and
    DS3GEVENTMGR_TH_MIT_TEAR_DOWN_BLOCK_ALL_CALLS are treated in the same way
    by 3GPP2 MH as per the design of system level Thermal mitigation.
  ---------------------------------------------------------------------------*/
  switch (cmd_info->action)
  {
    case DS3GEVENTMGR_TH_MIT_ALLOW_ALL_CALLS:   
      ds707_tm_mgr_current_action = DS3GEVENTMGR_TH_MIT_ALLOW_ALL_CALLS;
      break;

    case DS3GEVENTMGR_TH_MIT_ALLOW_ONLY_IMS_CALLS:
    case DS3GEVENTMGR_TH_MIT_TEAR_DOWN_BLOCK_ALL_CALLS:

      ds707_tm_mgr_current_action = cmd_info->action;
    /*------------------------------------------------------------------------ 
      All the existing calls needs to be torn down gracefull for both the
      cases. If in OOS, it is handled by DSSNET state machines.
    -------------------------------------------------------------------------*/
      ds707_pkt_mgr_graceful_ppp_termination(
                               PS_NET_DOWN_REASON_THERMAL_MITIGATION);

      break;

    default:
     break;
  }
} /* ds707_tm_mgr_process_therm_mit_cmd() */

/*===========================================================================
FUNCTION      DS3GPP22_TM_MGR_IS_CALL_ALLOWED

DESCRIPTION   This function is called during Routing and before Physlink 
              bringup to check if the call is allowed according to
              curent mitigation level.

DEPENDENCIES  None.

RETURN VALUE  TRUE if allowed 
              FALSE by default.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_tm_mgr_is_call_allowed
(
  ps_iface_apn_enum_type  apn_type
)
{
  boolean return_value = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------- 
    apn_type is not handled currently because 3GPP2 treats all the mitigation
    levels in the same way as per current design.
  ---------------------------------------------------------------------------*/
  if (ds707_tm_mgr_current_action == DS3GEVENTMGR_TH_MIT_ALLOW_ALL_CALLS)
  {    
    return_value = TRUE;
  }
  DATA_IS707_MSG2(MSG_LEGACY_LOW,
                  "ds707_TM_mgr_is_call_allowed():"
                  "Current mitigation level is %d,"
                  "returning %d",ds707_tm_mgr_current_action,return_value);
  return return_value;
}/* ds707_tm_mgr_process_therm_mit_cmd() */



