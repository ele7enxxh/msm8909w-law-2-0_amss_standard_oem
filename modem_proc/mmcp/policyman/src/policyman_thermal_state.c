/**
  @file policyman_thermal_state.c

  @brief  Module to handle thermal emergencies.
*/
	/*
		Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
		All Rights Reserved.
		Qualcomm Technologies Confidential and Proprietary
	
	  Export of this technology or software is regulated by the U.S. Government.
	  Diversion contrary to U.S. law prohibited.
	
	  All ideas, data and information contained in or disclosed by this
	  document are confidential and proprietary information of
	  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
	  By accepting this material the recipient agrees that this material
	  and the information contained therein are held in confidence and in
	  trust and will not be used, copied, reproduced in whole or in part,
	  nor its contents revealed in any manner to others without the express
	  written permission of QUALCOMM Technologies Incorporated.
	
	  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_thermal_state.c#1 $
	  $DateTime: 2016/12/13 08:00:05 $
	  $Author: mplcsds1 $
	*/

#include "policyman_task.h"
#include "policyman_policies.h"


typedef struct
{
  POLICYMAN_CMD_HDR;

  cfcm_cmd_type_s cfcm_cmd;
} policyman_cmd_report_thermal_state_t;

/*-------- policyman_compute_thermal_emerg_level --------*/
static policyman_thermal_level_t policyman_compute_thermal_emerg_level(
  uint32     monitor_mask,  
  cfcm_cmd_e fc_cmd
)
{
  policyman_thermal_level_t thermalLevel = POLICYMAN_THERM_LEVEL_0;
  size_t                    i;
  
  typedef struct
  {
    uint32                    mask;
    cfcm_cmd_e                cmd;
    policyman_thermal_level_t level;
  } monitor_cmd_level_map_elem;

  // Do not change below order
  static monitor_cmd_level_map_elem levels_map[] =
  {
    {CFCM_MONITOR_MASK_THERMAL_PA_EX,      CFCM_CMD_UP,        POLICYMAN_THERM_LEVEL_0},
    {CFCM_MONITOR_MASK_THERMAL_RUNAWAY_EX, CFCM_CMD_DOWN,      POLICYMAN_THERM_LEVEL_2},
    {CFCM_MONITOR_MASK_THERMAL_PA_EX,      CFCM_CMD_DOWN,      POLICYMAN_THERM_LEVEL_0},
    {CFCM_MONITOR_MASK_THERMAL_RUNAWAY_EX, CFCM_CMD_SHUT_DOWN, POLICYMAN_THERM_LEVEL_3},
    {CFCM_MONITOR_MASK_THERMAL_PA_EM,      CFCM_CMD_SHUT_DOWN, POLICYMAN_THERM_LEVEL_3},
  };

  
  POLICYMAN_MSG_HIGH_2("policyman_compute_thermal_emerg_level: monitor_mask %d cmd %d", monitor_mask, fc_cmd);

  for (i = 0 ; i < ARR_SIZE(levels_map) ; ++i)
  {
    monitor_cmd_level_map_elem  *pElem;
	pElem = &levels_map[i];

    if (pElem->mask & monitor_mask && pElem->cmd == fc_cmd)
    {
      thermalLevel = pElem->level;
      break;
    }
  }
  
  POLICYMAN_MSG_HIGH_1("policyman_compute_thermal_emerg_level: level %d", thermalLevel);
  
  return thermalLevel;
}

/*-------- policyman_compute_thermal_level --------*/
static boolean policyman_compute_thermal_level(
  policyman_state_t         *pState,
  cfcm_cmd_type_s           *pCmd
)
{
  policyman_thermal_level_t  thermalLevel;
  static uint32              masks_arr[] =
  {
    SYS_SYS_MODE_MASK_NONE,
    SYS_SYS_MODE_MASK_NONE,
    SYS_SYS_MODE_MASK_LTE,
    SYS_SYS_MODE_MASK_NONE,
  };

  // Compute the level corresponding to new monitor mask and command
  thermalLevel = policyman_compute_thermal_emerg_level( pCmd->monitors_active,
                                                        pCmd->cmd );
  
  policyman_state_update_thermal_level(thermalLevel);

  return TRUE;
}

/*-------- policyman_thermal_update_notification_hook --------*/
/**
@brief  Hook function for notification of the result of subs updates.

This creates another MSGR message (not sent by the MSGR) that is stored in the
command that initiated the notification.  The signal to unblock the thread
that sent the update is then set, which releases it and returns the message
to the caller of policyman_report_preference_change().
*/
STATIC void policyman_thermal_update_notification_hook(
  void              *pContext,
  size_t             nItems,
  policyman_item_t  **ppItems
)
{
  policyman_cmd_t *pCmd;

  /*  Create a new MSGR message and store it in the command.
   */
  pCmd = (policyman_cmd_t *) pContext;
  pCmd->pMsg = policyman_cfgitem_notify_msg_msim_new(ppItems, nItems, NULL);

  /*  Unblock the thread that issued the command.
   */
  policyman_client_block_signal_set(pCmd->signal);
}


/*-------- policyman_report_thermal_monitor_status_execute --------*/
static void policyman_report_thermal_state_execute(
  policyman_cmd_t *pCommand
  )
{ 
  policyman_state_t                    *pStateInfo;
  policyman_cmd_report_thermal_state_t *pCmd;

  pStateInfo = policyman_state_get_state();
  pCmd = (policyman_cmd_report_thermal_state_t *)pCommand;

  if (policyman_compute_thermal_level(pStateInfo, &pCmd->cfcm_cmd))
  {
    policyman_set_t   *pItemSet = policyman_itemset_new();
    
    // Run a policy check on the multimode subs.
    policyman_policies_run_policy_check(policyman_state_get_state(), pItemSet);

    //pCmd->pMsg = policyman_cfgitem_get_change_msg_hdr(pItemSet);

    // Update cfg items and notify clients if there is a change
    policyman_cfgitem_update_items(pItemSet, pCommand);
    REF_CNT_OBJ_RELEASE_IF(pItemSet);
  }
  else
  {
    /*  Unblock the thread that issued the command.
     */
    policyman_client_block_signal_set(pCmd->signal);
  }
    
}


/*-------- policyman_report_thermal_monitor_status --------*/
msgr_hdr_s *policyman_report_thermal_state(
  cfcm_cmd_type_s const *pCfcm_cmd
  )
{
  policyman_cmd_report_thermal_state_t *pCmd;
  msgr_hdr_s                           *pMsg = NULL;

  /*  If we should block the calling thread to do this update, get a signal on
   *  which to block for command completion, and put it in the command.
   *  Queue the command and wait for completion.
   */
  pCmd = POLICYMAN_CMD_WITH_HOOK_NEW( policyman_cmd_report_thermal_state_t,
                                      policyman_report_thermal_state_execute,
                                      NULL,
                                      NULL,
                                      policyman_cfgitem_update_notification_hook);

  pCmd->cfcm_cmd = *pCfcm_cmd;
  pCmd->pMsg     = NULL;

  POLICYMAN_MSG_HIGH_0("blocking caller of policyman_report_thermal_state()");
  pCmd->signal = policyman_client_block_sig_get();
  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  
  /* Block the caller only when policy is not suspended
  */
  if (!policyman_is_suspended())
  {
    policyman_client_block_signal_wait(pCmd->signal);
  }

  /*  Get any message to be returned and then release the command.
   */
  pMsg = pCmd->pMsg;
  ref_cnt_obj_release(pCmd);
  POLICYMAN_MSG_HIGH_0("returning to caller of policyman_report_thermal_state()");
  return pMsg;
  
}

