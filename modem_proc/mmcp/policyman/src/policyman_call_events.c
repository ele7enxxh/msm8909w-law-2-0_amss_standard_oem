/**
  @file policyman_call_events.c

  @brief
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_call_events.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/


#include "policyman_call_events.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_diag.h"
#include "policyman_policies.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_xml.h"

#include "sys.h"
#include "modem_mem.h"
#include <stringl/stringl.h>


/*=============================================================================
 Data Structure for CM CALL Info
==============================================================================*/
struct policyman_cmcall_info_s
{
  cm_call_event_e_type      call_event;       // Indicates the current call event.
  cm_call_type_e_type       call_type;        // Indicates the call type
};

/*=============================================================================
  Call Event callback definition.
=============================================================================*/
typedef struct
{
  POLICYMAN_CMD_HDR;
  cm_call_event_e_type    evt;
  cm_mm_call_info_s_type  info;
  uint32                  timestamp;
} policyman_cmcall_evt_cb_t;

/*=============================================================================
  Call Event History definitions
=============================================================================*/
struct policyman_cmcall_history_entry_s {
    size_t                  next;
    uint32                  timestamp;
    sys_modem_as_id_e_type  subs_id;
    cm_call_event_e_type    call_event;
    cm_call_type_e_type     call_type;
};

struct policyman_cmcall_history_s {
  size_t                            index;
  policyman_cmcall_history_entry_t  h[PM_DUMP_HISTORY_DEPTH];
};

/*=============================================================================
  APIs for CM Phone Event information
=============================================================================*/
/*-------- policyman_cmcall_init --------*/
boolean policyman_cmcall_init(
  policyman_cmcall_info_t **ppCallInfo,
  sys_modem_as_id_e_type    asubs_id
)
{
  policyman_cmcall_info_t *pInfo;

  pInfo = (policyman_cmcall_info_t *)policyman_mem_alloc(sizeof(policyman_cmcall_info_t));

  pInfo->call_event = CM_CALL_EVENT_NONE;

  *ppCallInfo = pInfo;
  return (*ppCallInfo != NULL);  // NOTE: ALWAYS true
}

/*-------- policyman_cmcall_deinit --------*/
void policyman_cmcall_deinit(
  policyman_cmcall_info_t *pInfo
  )
{
  POLICYMAN_MEM_FREE_IF(pInfo);
}

/*-------- policyman_cmcall_init_history --------*/
void policyman_cmcall_init_history(
  policyman_state_t  *pState
)
{
  if (NULL == pState->pCmcallHistory)
  {
    int   idx;

    pState->pCmcallHistory = (policyman_cmcall_history_t *)policyman_mem_alloc(sizeof(policyman_cmcall_history_t));

    for (idx = 0; idx < (PM_DUMP_HISTORY_DEPTH); ++idx)
    {
      pState->pCmcallHistory->h[idx].next       = idx + 1;
      pState->pCmcallHistory->h[idx].timestamp  = 0xC0FFEE;
      pState->pCmcallHistory->h[idx].call_event = CM_CALL_EVENT_NONE;
      pState->pCmcallHistory->h[idx].call_type  = CM_CALL_TYPE_NONE;
    }

    pState->pCmcallHistory->h[PM_DUMP_HISTORY_DEPTH - 1].next = 0;
  }
}

/*-------- policyman_cmcall_deinit_history --------*/
void policyman_cmcall_deinit_history(
  policyman_state_t  *pState
)
{
  POLICYMAN_MEM_FREE_IF(pState->pCmcallHistory);
}

/*-------- policyman_cmcall_update_history --------*/
void policyman_cmcall_update_history(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subs_id,
  cm_call_event_e_type    call_event,
  cm_call_type_e_type     call_type,
  uint32                  timestamp
)
{
  if (NULL != pState->pCmcallHistory)
  {
    pState->pCmcallHistory->h[pState->pCmcallHistory->index].timestamp  = timestamp;
    pState->pCmcallHistory->h[pState->pCmcallHistory->index].subs_id    = subs_id;
    pState->pCmcallHistory->h[pState->pCmcallHistory->index].call_event = call_event;
    pState->pCmcallHistory->h[pState->pCmcallHistory->index].call_type  = call_type;

    pState->pCmcallHistory->index = pState->pCmcallHistory->h[pState->pCmcallHistory->index].next;
  }
}

/*-------- policyman_cmcall_fill_diag_call_info --------*/
void policyman_cmcall_fill_diag_call_info(
  sys_modem_as_id_e_type     asubs_id,
  policyman_cmcall_info_t   *subs_cmcall_info,
  pm_dump_cm_call_info_t    *dump_cmcall_info
)
{
  dump_cmcall_info->asubs_id    = asubs_id;
  dump_cmcall_info->call_event  = subs_cmcall_info->call_event;
  dump_cmcall_info->call_type   = subs_cmcall_info->call_type;
}

/*-------- policyman_cmcall_history_entry_to_dump --------*/
STATIC void policyman_cmcall_history_entry_to_dump(
  policyman_cmcall_history_entry_t *hist_entry,
  pm_dump_cm_call_info_history_t   *dump_entry
)
{
  dump_entry->timestamp                = hist_entry->timestamp;

  dump_entry->cm_call_info.asubs_id    = hist_entry->subs_id;
  dump_entry->cm_call_info.call_event  = hist_entry->call_event;
  dump_entry->cm_call_info.call_type   = hist_entry->call_type;
}

/*-------- policyman_cmcall_get_history --------*/
size_t policyman_cmcall_get_history(
  pm_dump_cm_call_info_history_t *h_buffer,
  size_t                          h_len
)
{
  policyman_state_t      *pState;
  int                     oldest_hist_idx;
  int                     start_idx;
  int                     hist_idx;
  int                     offset;

  pState = policyman_state_get_state();

  // If given length larger than history, get only max history available
  if (h_len > PM_DUMP_HISTORY_DEPTH)
  {
    h_len = PM_DUMP_HISTORY_DEPTH;
  }

  oldest_hist_idx = pState->pCmcallHistory->index;  // points to oldest history record

  // Calculate the index to start with for the most recent h_len history entries
  start_idx = (oldest_hist_idx + (PM_DUMP_HISTORY_DEPTH - h_len)) % PM_DUMP_HISTORY_DEPTH;

  hist_idx = start_idx;

  for (offset = 0; offset < h_len; offset++ )
  {
    policyman_cmcall_history_entry_to_dump( &pState->pCmcallHistory->h[hist_idx],
                                            &h_buffer[offset] );

    hist_idx = pState->pCmcallHistory->h[hist_idx].next;
  }

  return h_len;
}

/*-------- policyman_cmcall_info_deep_copy --------*/
policyman_cmcall_info_t * policyman_cmcall_info_deep_copy(
  policyman_cmcall_info_t  *pCmCallState
)
{
  policyman_cmcall_info_t *pInfo;

  pInfo = (policyman_cmcall_info_t *)policyman_mem_alloc(sizeof(policyman_cmcall_info_t));

  *pInfo = *pCmCallState;

  return pInfo;
}


/*-------- policyman_cmcall_get_call_event --------*/
cm_call_event_e_type policyman_cmcall_get_call_event(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_cmcall_info_t *pCallInfo;

  pCallInfo = policyman_state_get_call_info(pState, subsId);

  return pCallInfo->call_event;
}


/*-------- policyman_cmcall_get_call_type --------*/
cm_call_type_e_type policyman_cmcall_get_call_type(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_cmcall_info_t *pCallInfo;

  pCallInfo = policyman_state_get_call_info(pState, subsId);

  return pCallInfo->call_type;
}


/*-------- policyman_cmcall_update_state --------*/
boolean policyman_cmcall_update_state(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId,
  cm_call_event_e_type    call_event,
  cm_call_type_e_type     type,
  uint32                  timestamp
)
{
  boolean                  changed   = FALSE;
  policyman_cmcall_info_t *pCallInfo;

  pCallInfo = policyman_state_get_call_info(pState, subsId);

  /* For MO call, handle call start/end events. For MT calls, handle call incoming/end events
        All other states are intermediate to start-end and they should be treated as in Call.
   */
  if (
       (   call_event == CM_CALL_EVENT_ORIG
        || call_event == CM_CALL_EVENT_CONNECT
        || call_event == CM_CALL_EVENT_INCOM
        || call_event == CM_CALL_EVENT_END
       )
       &&
       call_event != pCallInfo->call_event)
  {
    pCallInfo->call_event = call_event;
    pCallInfo->call_type  = type;
    changed = TRUE;

    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "updated state; call_event %d, call_type %d",
                          subsId,
                          call_event,
                          type );
  }

  policyman_cmcall_update_history( pState,
                                   subsId,
                                   call_event,
                                   type,
                                   timestamp );

  return changed;
}

/*===========================================================================
  FUNCTION POLICYMAN_EXECUTE_PROCESS_CM_CALL_EVT()

  DESCRIPTION
    Execute CM Call Set update update and policy check

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void policyman_execute_process_cmcall_evt(
  policyman_cmd_t *pCmd
)
{
  policyman_cmcall_evt_cb_t  *call_evt = (policyman_cmcall_evt_cb_t *)pCmd;

  if (NULL != call_evt)
  {
    boolean call_state_changed = FALSE;

    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "process cmcall event; call_event %d, call_type %d",
                          call_evt->info.asubs_id,
                          call_evt->evt,
                          call_evt->info.call_type );

    policyman_state_enter_crit_section();
    // No need to call this, can call policyman_cmcall_update_state() directly
    call_state_changed = policyman_cmcall_update_state( call_evt->pState,
                                                        call_evt->info.asubs_id,
                                                        call_evt->evt,
                                                        call_evt->info.call_type,
                                                        call_evt->timestamp );

    policyman_state_leave_crit_section();

    if (call_state_changed)
    {
      policyman_state_handle_update(call_evt->pState);
    }
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_CMCALL_EVENT_CB()

  DESCRIPTION
    Handle CM Call event callback

  PARAMETERS
    evt     : CM Call EVENT type
    p_info : actual payload of CM Call EVENT

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void policyman_cmcall_event_cb(
  cm_call_event_e_type           evt,
  const cm_mm_call_info_s_type  *p_info
)
{
  policyman_cmcall_evt_cb_t *pCmd = NULL;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "received cmcall evt %d",
                        p_info->asubs_id,
                        evt );

  pCmd = POLICYMAN_CMD_NEW( policyman_cmcall_evt_cb_t,
                            policyman_execute_process_cmcall_evt,
                            NULL,
                            NULL ); // No policy associated with CM Call Event

  pCmd->evt       = evt;
  pCmd->info      = *p_info;
  pCmd->timestamp = mre_util_get_time();

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);
}




/*=============================================================================
  Call-related conditions
=============================================================================*/

/*-----------------------------------------------------------------------------
  Call Event
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;
} policyman_call_event_condition_t;


/*-------- evaluate_in_call --------*/
static boolean evaluate_in_call(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  boolean                        result = FALSE;
  policy_execute_ctx_t *pContext  = (policy_execute_ctx_t *)pCtx;
  cm_call_event_e_type  call_event;
  cm_call_type_e_type   call_type;

  call_event = policyman_cmcall_get_call_event(pContext->pState, subsId);
  call_type  = policyman_cmcall_get_call_type(pContext->pState, subsId);

  /* Return TRUE only when Voice or E911 call is going on
   */
  result = (   call_type == CM_CALL_TYPE_EMERGENCY
            || call_type == CM_CALL_TYPE_VOICE
           )
           &&
           (   call_event == CM_CALL_EVENT_ORIG
            || call_event == CM_CALL_EVENT_INCOM
            || call_event == CM_CALL_EVENT_CONNECT
           );

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX SRC_LINE "condition <in_call> (call_type %d, call_event %d) returns %d",
                        subsId,
                        LINE_NUM(pCondition),
                        call_type,
                        call_event,
                        result );

  return result;
}


/*-------- policyman_condition_call_event_evaluate --------*/
static boolean policyman_condition_call_event_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_in_call);
}


/*-------- policyman_condition_call_event_new --------*/
mre_status_t policyman_condition_call_event_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                      status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_call_event_condition_t  *pCondition;
  policyman_subs_t                  pm_subs;

  /* Does this action indicate the subscription
   *  Default is subscription from the policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_call_event_condition_t,
                                        pPolicy,
                                        policyman_condition_call_event_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

  *ppCondition = (mre_condition_t *) pCondition;

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}
