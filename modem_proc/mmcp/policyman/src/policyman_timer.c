/**
  @file policyman_timer.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_timer.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"

#include "policyman_dbg.h"
#include "policyman_efs.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_state.h"
#include "policyman_task.h"
#include "policyman_timer.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

#include <stringl/stringl.h>


/*-----------------------------------------------------------------------------
  States of timers
-----------------------------------------------------------------------------*/
typedef enum
{
  POLICYMAN_TIMER_STATE_STOPPED,
  POLICYMAN_TIMER_STATE_RUNNING,
  POLICYMAN_TIMER_STATE_EXPIRED
} policyman_timer_state_t;


/*-----------------------------------------------------------------------------
  Timer class
-----------------------------------------------------------------------------*/
typedef enum
{
  POLICYMAN_TIMER_CLASS_UNKNOWN,
  POLICYMAN_TIMER_CLASS_FIXED_INTERVAL,
  POLICYMAN_TIMER_CLASS_BACKOFF,
} policyman_timer_class_t;


/*=============================================================================
  The timer structure
=============================================================================*/

struct policyman_timer_t
{
  POLICYMAN_NAMED_OBJECT_BASE;

  policyman_timer_class_t      timerClass;
  timer_type                   timer;
  timetick_type                interval;
  policyman_timer_state_t      state;

  policyman_timer_expirefn_t   pfnOnExpire;  /*  Called from task thread only */
  uint32                       id;           /*  Timer ID known to policy.  Used to
                                             *  identify timer in debug statements,
                                             *  as we can't use strings.
                                             */
  sys_modem_as_id_e_type       asubs_id;     /* subscription from which timer is fired*/

  /*  Data for backoff timers
   */
  policyman_set_t             *pBackoffTable;
  size_t                       tableIndex;
  boolean                      persistIndex;
};


/* Timer group for making timers non-deferrable - by default all timer group are non-deferrable*/
static timer_group_type nonDeferrableTimerGroup;



/*=============================================================================
  Forward declarations
=============================================================================*/

static void policyman_timer_set_backoff_index(
  policyman_timer_t *pTimer,
  size_t             index
);



/*=============================================================================
  Timer utility functions
=============================================================================*/

/*-------- policyman_timer_get_timer --------*/
policyman_timer_t * policyman_timer_get_timer(
  char const  *pName
)
{
  return (policyman_timer_t *) mre_named_object_find_with_subs( pName,
                                                                POLICYMAN_NAMED_TIMER,
                                                                SYS_MODEM_AS_ID_1,
                                                                policyman_get_namespace_info() );
}

/*-------- policyman_timer_get_interval --------*/
timetick_type policyman_timer_get_interval(
  policyman_timer_t *pTimer
)
{
  return (pTimer == NULL) ? 0 : pTimer->interval;
}

/*-------- policyman_timer_modify_interval --------*/
void policyman_timer_modify_interval(
  policyman_timer_t *pTimer,
  timetick_type      newInterval
)
{
  if (pTimer != NULL)
  {
    pTimer->interval += newInterval;
    POLICYMAN_MSG_HIGH_4( SUBS_PREFIX "timer id %d on subs %d has new interval %d",
                          POLICY_SUBS_SAFE(pTimer->pPolicy),
                          pTimer->id,
                          pTimer->asubs_id,
                          pTimer->interval );
  }
}


/*-------- policyman_timer_get_policy --------*/
policyman_policy_t * policyman_timer_get_policy(
  policyman_timer_t *pTimer
)
{
  return (pTimer == NULL) ? NULL : (policyman_policy_t *) pTimer->pPolicy;
}

/*-------- policyman_timer_get_id --------*/
uint32 policyman_timer_get_id(
  policyman_timer_t *pTimer
)
{
  return (pTimer == NULL) ? 0 : pTimer->id;
}


/*-------- policyman_timer_is_stopped --------*/
boolean policyman_timer_is_stopped(
  policyman_timer_t *pTimer
)
{
  return    pTimer != NULL
         && pTimer->state == POLICYMAN_TIMER_STATE_STOPPED;
}


/*-------- policyman_timer_is_running --------*/
boolean policyman_timer_is_running(
  policyman_timer_t *pTimer
)
{
  return    pTimer != NULL
         && pTimer->state == POLICYMAN_TIMER_STATE_RUNNING;
}


/*-------- policyman_timer_is_expired --------*/
boolean policyman_timer_is_expired(
  policyman_timer_t *pTimer
)
{
  return    pTimer != NULL
         && pTimer->state == POLICYMAN_TIMER_STATE_EXPIRED;
}


/*-------- policyman_timer_start --------*/
void policyman_timer_start(
  policyman_timer_t *pTimer
)
{
  if (pTimer == NULL)
  {
    return;
  }

  pTimer->state = POLICYMAN_TIMER_STATE_RUNNING;

  if (!timer_is_active(&pTimer->timer))
  {
    timer_set(&pTimer->timer, pTimer->interval, 0, T_SEC);
    POLICYMAN_MSG_HIGH_5( LOG_SUBS_PREFIX "Starting timer %d on subs %d with duration %d secs",
                          LOG_SUBS(pTimer->asubs_id),
                          POLICY_SUBS_SAFE(pTimer->pPolicy),
                          pTimer->id,
                          pTimer->asubs_id,
                          pTimer->interval );
  }
}

/*-------- policyman_timer_stop --------*/
void policyman_timer_stop(
  policyman_timer_t *pTimer
)
{
  if (pTimer == NULL)
  {
    return;
  }

  if (timer_is_active(&pTimer->timer))
  {
    timer_clr(&pTimer->timer, T_SEC);               // stops the active timer
    POLICYMAN_MSG_HIGH_4( LOG_SUBS_PREFIX "Stopping timer %d on subs %d",
                          LOG_SUBS(pTimer->asubs_id),
                          POLICY_SUBS_SAFE(pTimer->pPolicy),
                          pTimer->id,
                          pTimer->asubs_id );
  }

  /*  Stopping a backoff timer resets its index to 0.
   */
  if (    pTimer->state == POLICYMAN_TIMER_STATE_RUNNING
      &&  pTimer->timerClass == POLICYMAN_TIMER_CLASS_BACKOFF
     )
  {
    policyman_timer_set_backoff_index(pTimer, 0);
  }

  pTimer->state = POLICYMAN_TIMER_STATE_STOPPED;
}

/*-------- policyman_timer_set_expirefn --------*/
void policyman_timer_set_expirefn(
  policyman_timer_t           *pTimer,
  policyman_timer_expirefn_t   pfnExpire
)
{
  if (pTimer != NULL)
  {
    pTimer->pfnOnExpire = pfnExpire;
  }
}

/*-------- policyman_timer_set_handled --------*/
void policyman_timer_set_handled(
  policyman_timer_t *pTimer
)
{
  if (policyman_timer_is_expired(pTimer))
  {
    pTimer->state = POLICYMAN_TIMER_STATE_STOPPED;
  }
}

/*-------- policyman_timer_set_subs --------*/
void policyman_timer_set_subs(
  policyman_timer_t      *pTimer,
  sys_modem_as_id_e_type  subs
)
{
  if (pTimer != NULL)
  {
    pTimer->asubs_id = subs;
  }
}

/*-------- policyman_timer_get_subs --------*/
sys_modem_as_id_e_type policyman_timer_get_subs(
  policyman_timer_t *pTimer
)
{
  return (pTimer == NULL) ? SYS_MODEM_AS_ID_1 : pTimer->asubs_id;
}


/*=============================================================================
  Timer handling functions
=============================================================================*/

typedef struct
{
  POLICYMAN_CMD_HDR;

  policyman_timer_t *pTimer;
} policyman_timer_cmd_t;


/*-------- get_backoff_timer_persist_path --------*/
static void get_backoff_timer_persist_path(
  policyman_timer_t *pTimer,
  char              *pFileName,
  size_t             bufSize
)
{
  size_t  copied;

  copied = strlcpy(pFileName, POLICYMAN_TIMERS_EFS_PATH_PREFIX, bufSize);
  pFileName += copied;
  bufSize -= copied;
  strlcpy(pFileName, pTimer->pName, bufSize);
}


/*-------- policyman_timer_persist_index --------*/
static void policyman_timer_persist_index(
  policyman_timer_t *pTimer
)
{
  char                    fileName[100];
  byte                    index     = (byte) pTimer->tableIndex;
  mre_efs_status_t        efsStatus;

  get_backoff_timer_persist_path(pTimer, fileName, sizeof(fileName));

  efsStatus = mre_efs_put_file( fileName,
                                &index,
                                sizeof(index),
                                TRUE,
                                SYS_MODEM_AS_ID_1 );

  if (efsStatus != MRE_EFS_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_1("persist timer index failed with error %d", efsStatus);
  }
}


/*-------- policyman_timer_set_backoff_index --------*/
STATIC void policyman_timer_set_backoff_index(
  policyman_timer_t *pTimer,
  size_t             index
)
{
  size_t  *pElem;
  size_t   tableSize;
  size_t   oldIndex = pTimer->tableIndex;

  /*  Adjust the index to fit the backoff table...
   */
  tableSize = policyman_set_num_elems(pTimer->pBackoffTable);
  index = MIN(index, tableSize - 1);
  pTimer->tableIndex = index;

  /*  Get the pointer to the interval in the table and set the timer interval
   *  to that value.
   */
  pElem = (size_t *) policyman_set_elem_ptr(pTimer->pBackoffTable, index);
  if (pElem == NULL)
  {
    /* This should never occur, because we reduced the index to fit the table... */
    POLICYMAN_ERR_FATAL("Internal error: backoff timer index larger than backoff table", 0, 0, 0);
  }
  pTimer->interval = *pElem;

  POLICYMAN_MSG_HIGH_2( "Updating backoff timer: new index = %d, new interval = %d",
                        pTimer->tableIndex,
                        pTimer->interval );

  /*  Persist the new index if necessary
   */
  if (index != oldIndex && pTimer->persistIndex)
  {
    policyman_timer_persist_index(pTimer);
  }
}


/*-------- policyman_timer_do_backoff --------*/
static void policyman_timer_do_backoff(
  policyman_timer_t *pTimer
)
{
  policyman_timer_set_backoff_index(pTimer, pTimer->tableIndex + 1);
}


/*-------- policyman_execute_process_timer_evt --------*/
void policyman_execute_process_timer_evt(
  policyman_cmd_t *pCmd
)
{
  policyman_timer_cmd_t *evt = (policyman_timer_cmd_t *) pCmd;
  policyman_timer_t     *pTimer = evt->pTimer;

  if (pTimer == NULL)
  {
    return;
  }

  POLICYMAN_MSG_HIGH_4( LOG_SUBS_PREFIX "Process expired timer %d for subs %d",
                        LOG_SUBS(pTimer->asubs_id),
                        POLICY_SUBS_SAFE(pTimer->pPolicy),
                        pTimer->id,
                        pTimer->asubs_id );

  pTimer->state = POLICYMAN_TIMER_STATE_EXPIRED;

  timer_clr(&pTimer->timer, T_SEC);     // stops the active timer

  /*  If a backoff timer expires, it advances to the next interval
   */
  if (pTimer->timerClass == POLICYMAN_TIMER_CLASS_BACKOFF)
  {
    policyman_timer_do_backoff(pTimer);
  }

  /*  If there's a function to call on timer expiration, do so.  Otherwise,
   *  run the rules.
   */
  if (pTimer->pfnOnExpire != NULL)
  {
    pTimer->pfnOnExpire(pTimer);
  }
  else
  {
    policyman_state_handle_update(pCmd->pState);
  }

#if defined(TEST_FRAMEWORK)
  #error code not present
#endif /* +TEST_FRAMEWORK. */
}


/*-------- policyman_timer_cb --------*/
/**
@brief  Callback function for timer expiration - queues a command to handle
        the expiration.
*/
void policyman_timer_cb(
  timer_cb_data_type  user_data
)
{
  policyman_timer_t     *pTimer = (policyman_timer_t *) user_data;
  policyman_timer_cmd_t *pCmd;

  POLICYMAN_MSG_HIGH_2( "policyman_timer_cb called: subs %d timer %d expired", pTimer->asubs_id, pTimer->id);
  POLICYMAN_MSG_HIGH_4( LOG_SUBS_PREFIX "policyman_timer_cb called: subs %d timer %d expired",
                        LOG_SUBS(pTimer->asubs_id),
                        POLICY_SUBS_SAFE(pTimer->pPolicy),
                        pTimer->asubs_id,
                        pTimer->id );

  pCmd = POLICYMAN_CMD_NEW( policyman_timer_cmd_t,
                            policyman_execute_process_timer_evt,
                            NULL,
                            (policyman_policy_t *) pTimer->pPolicy );

  pCmd->pTimer = pTimer;
  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);

  ref_cnt_obj_release(pCmd);
}


/*-------- policyman_timer_init --------*/
/**
@brief  Initialize the given timer, defining it in timer services.

@param
    pTimer:                  Pointer to timer object
    pfnOnExpire:           function to be called upon expiry
    id:                        timer id for the timer object
    timer_type:            timer type for making timer deferrable or non-deferrable

@return
    TRUE if timer is initialized properly, FALSE otherwise
*/
boolean policyman_timer_init(
  policyman_timer_t           *pTimer,
  policyman_timer_expirefn_t   pfnOnExpire,
  uint32                       id,
  policyman_timer_type_t       timer_type
)
{
  timer_group_type *pTimerGroup;

  boolean success = (pTimer != NULL && pTimer->interval > 0);

  if (success)
  {
    pTimer->pfnOnExpire = pfnOnExpire;
    pTimer->id = id;

    switch(timer_type)
    {
      case POLICYMAN_TIMER_TYPE_NONDEFERRABLE:
         pTimerGroup = &nonDeferrableTimerGroup;
         break;

      case POLICYMAN_TIMER_TYPE_DEFERRABLE:
        pTimerGroup = NULL;
        break;

      default:
        POLICYMAN_MSG_ERROR_1("timer type %d not handled", timer_type);
        pTimerGroup = NULL;
        break;
    }

    timer_def(
        &pTimer->timer,             // timer object
        pTimerGroup,                // timer group (needed if timer is critical and
                                    //    needs to expire while device is in sleep)
        NULL,                       // task to be called when timer expires (needed
                                    //    when signal parameter is provided, not
                                    //    needed with callback function)
        0,                          // signal to be generated when timer expires
        policyman_timer_cb,         // call back function
        (timer_cb_data_type) pTimer // callback will get a pointer to the expired timer
        );
  }

  return success;
}


/*=============================================================================
  Timer-related conditions.
=============================================================================*/

/*-----------------------------------------------------------------------------
  <timer_expired>
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_timer_t *pTimer;
  char const        *pTimerName;
} policyman_timer_expired_condition_t;


/*-------- evaluate_timer_expired --------*/
STATIC boolean evaluate_timer_expired(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
)
{
  boolean                              result = FALSE;
  policyman_timer_expired_condition_t *pCond = (policyman_timer_expired_condition_t *) pCondition;

  if (pCond->pTimer == NULL && pCond->pTimerName != NULL)
  {
    pCond->pTimer = (policyman_timer_t *) mre_named_object_find( pCond->pTimerName,
                                                                 POLICYMAN_NAMED_TIMER,
                                                                 pCond->pPolicy );


    /* Add reference count only for looked up timer object
      */
    if (pCond->pTimer != NULL)
    {
      ref_cnt_obj_add_ref(pCond->pTimer);
    }
  }

  POLICYMAN_MEM_FREE_IF(pCond->pTimerName);

  if (pCond->pTimer != NULL)
  {
    result = pCond->pTimer->state == POLICYMAN_TIMER_STATE_EXPIRED;
    POLICYMAN_MSG_HIGH_6( LOG_SUBS_PREFIX SRC_LINE "condition <timer_expired> for subs %d timer %d returns %d",
                          LOG_SUBS(pCond->pTimer->asubs_id),
                          POLICY_SUBS(pCond),
                          LINE_NUM(pCond),
                          pCond->pTimer->asubs_id,
                          pCond->pTimer->id,
                          result );
  }
  else
  {
    POLICYMAN_MSG_HIGH_3( SRC_LINE "condition <timer_expired> for no timer returns %d",
                          POLICY_SUBS(pCond),
                          LINE_NUM(pCond),
                          result );
  }
  return result;
}

/*-------- policyman_condition_timer_expired_evaluate --------*/
STATIC boolean policyman_condition_timer_expired_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_timer_expired);
}


/*-------- policyman_condition_time_expired_dtor --------*/
static void policyman_condition_time_expired_dtor(
  void  *pObj
)
{
  policyman_timer_expired_condition_t *pCondition = (policyman_timer_expired_condition_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pCondition->pTimer);
  POLICYMAN_MEM_FREE_IF(pCondition->pTimerName);
  policyman_condition_dtor(pCondition);
}


/*-------- policyman_condition_timer_expired_new --------*/
mre_status_t policyman_condition_timer_expired_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
)
{
  mre_status_t                         status = MRE_STATUS_ERR_INVALID_CONDITION;
  char const                          *pStr;
  policyman_timer_expired_condition_t *pCondition;

  /*  A name is required for this condition.
   */
  pStr = policyman_xml_get_attribute(pElem, "name");

  if (pStr == NULL)
  {
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "<time_expired> name required for condition", POLICY_SUBS(pPolicy));
    goto Done;
  }

  /*  Allocate the condition
   */
  pCondition = POLICYMAN_CONDITION_NEW( policyman_timer_expired_condition_t,
                                        pPolicy,
                                        policyman_condition_timer_expired_evaluate,
                                        policyman_condition_time_expired_dtor,
                                        SUBS_THIS,
                                        pElem );

  pCondition->pTimerName = policyman_str_dup(pStr);

  status = MRE_STATUS_SUCCESS;

  *ppCondition = (mre_condition_t *) pCondition;

Done:
  POLICYMAN_MSG_MED_2( SUBS_PREFIX "policyman_condition_timer_expired_new returned status %d",
                       POLICY_SUBS(pPolicy),
                       status );

  return status;
}


/*=============================================================================
  Timer-related actions
=============================================================================*/

/*-----------------------------------------------------------------------------
  <define_timer>
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_ACTION_BASE;

  char const              *pTimerName;   /* Name of the timer to create */
  uint32                   id;
  policyman_timer_class_t  timerClass;
  timetick_type            interval;
  policyman_set_t         *pBackoffTable;
  boolean                  persist;
} named_timer_action_t;


/*-------- policyman_named_timer_action_dtor --------*/
static void policyman_named_timer_action_dtor(
  void  *pObj
)
{
  named_timer_action_t  *pMe = (named_timer_action_t *) pObj;

  POLICYMAN_MEM_FREE_IF(pMe->pTimerName);
  REF_CNT_OBJ_RELEASE_IF(pMe->pBackoffTable);
  policyman_action_dtor(pMe);
}


/*-------- policyman_timer_dtor --------*/
static void policyman_timer_dtor(
  void  *pObj
)
{
  policyman_timer_t *pMe = (policyman_timer_t *) pObj;

  POLICYMAN_MEM_FREE_IF(pMe->pName);
  REF_CNT_OBJ_RELEASE_IF(pMe->pBackoffTable);

  timer_undef(&pMe->timer);
  policyman_mem_free(pMe);
}


/*-------- get_backoff_timer_index --------*/
STATIC size_t get_backoff_timer_index(
  policyman_timer_t     *pTimer
)
{
  size_t                  index         = 0;
  char                    fileName[100];
  byte                   *pIndex        = NULL;
  size_t                  indexSize;
  mre_efs_status_t        status;

  if (pTimer->persistIndex)
  {
    get_backoff_timer_persist_path(pTimer, fileName, sizeof(fileName));

    status = mre_efs_get_file( fileName,
                               (void **)&pIndex,
                               &indexSize,
                               SYS_MODEM_AS_ID_1 );

    if (status == MRE_EFS_STATUS_SUCCESS)
    {
      index = *pIndex;
    }
  }

  POLICYMAN_MEM_FREE_IF(pIndex);

  return index;
}


/*-------- execute_timer_new --------*/
STATIC boolean execute_timer_new(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  named_timer_action_t  *pTimerAction = (named_timer_action_t *) pAction;
  policyman_policy_t    *pPolicy      = (policyman_policy_t *) pAction->pPolicy;
  policyman_timer_t     *pTimer;
  boolean                succeeded;

  if (pPolicy == NULL)
  {
    POLICYMAN_MSG_ERROR_0("Internal error - executing new timer action with no policy");
    return FALSE;
  }

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "defining timer",
                        ACTION_SUBS(pTimerAction),
                        LINE_NUM(pTimerAction) );

  pTimer = (policyman_timer_t *) policyman_mem_alloc(sizeof(policyman_timer_t));
  ref_cnt_obj_init(pTimer, policyman_timer_dtor);

  policyman_named_object_init( (policyman_named_object_t *) pTimer,
                               pTimerAction->pTimerName,
                               POLICYMAN_NAMED_TIMER );

  pTimer->timerClass = pTimerAction->timerClass;
  pTimer->interval   = pTimerAction->interval;
  pTimer->id         = pTimerAction->id;
  pTimer->state      = POLICYMAN_TIMER_STATE_STOPPED;
  policyman_timer_set_subs(pTimer, pPolicy->subs);

  if (pTimer->timerClass == POLICYMAN_TIMER_CLASS_BACKOFF)
  {
    pTimer->persistIndex  = pTimerAction->persist;
    pTimer->pBackoffTable = pTimerAction->pBackoffTable;
    ref_cnt_obj_add_ref(pTimer->pBackoffTable);
    policyman_timer_set_backoff_index( pTimer,
                                       (pTimerAction->persist) ? get_backoff_timer_index(pTimer) : 0 );
  }

  (void) mre_named_object_add( (policyman_named_object_t *) pTimer,
                                MRE_NAMESPACE_SUBS,
                                (mre_policy_t *) pPolicy );

  succeeded = policyman_timer_init( pTimer,
                                    NULL,
                                    pTimer->id,
                                    POLICYMAN_TIMER_TYPE_NONDEFERRABLE );
  if (!succeeded)
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "Couldn't initialize timer %d",
                          POLICY_SUBS(pPolicy),
                          pTimer->id );

    POLICYMAN_UT_MSG_2( SUBS_PREFIX "Couldn't initialize timer %s",
                        POLICY_SUBS(pPolicy),
                        pTimer->pName );
  }

  /*  Release the local copy - the named object set still has a reference.
   */
  ref_cnt_obj_release(pTimer);

  return succeeded;
}

/*-------- policyman_timer_new_action_execute --------*/
STATIC boolean policyman_timer_new_action_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_timer_new);
}



/*-------- get_interval_multiplier --------*/
static size_t get_interval_multiplier(
  policyman_xml_element_t const *pElem
)
{
  char const  *pStr;
  size_t       multiplier = 0;

  pStr = policyman_xml_get_attribute(pElem, "units");

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_0("<define_timer>: needs a 'units' attribute");
    goto Done;
  }

  if      (strcmp(pStr, "min") == 0)
  {
    multiplier = 60;
  }
  else if (strcmp(pStr, "sec") == 0)
  {
    multiplier = 1;
  }
  else
  {
    POLICYMAN_UT_MSG_1("<define_timer>: invalid units for timer: %s", pStr);
    POLICYMAN_UT_MSG_0("Valid values are 'min' and 'sec'");
  }

Done:
  return multiplier;
}


/*-------- get_timer_class --------*/
static policyman_timer_class_t get_timer_class(
  policyman_xml_element_t const *pElem
)
{
  policyman_timer_class_t  timerClass = POLICYMAN_TIMER_CLASS_FIXED_INTERVAL;
  char const              *pStr;

  pStr = policyman_xml_get_attribute(pElem, "class");

  if (pStr == NULL)
  {
    goto Done;
  }

  if      (strcasecmp(pStr, "fixed") == 0)
  {
    timerClass = POLICYMAN_TIMER_CLASS_FIXED_INTERVAL;
  }
  else if (strcasecmp(pStr, "backoff") == 0)
  {
    timerClass = POLICYMAN_TIMER_CLASS_BACKOFF;
  }
  else
  {
    timerClass = POLICYMAN_TIMER_CLASS_UNKNOWN;
    POLICYMAN_UT_MSG_1("<define_timer>: invalid class for timer: %s", pStr);
    POLICYMAN_UT_MSG_0("Valid values are 'fixed' and 'backoff'");
  }

Done:
  return timerClass;
}


/*-------- get_timer_interval --------*/
static size_t get_timer_interval(
  policyman_xml_element_t const *pElem,
  size_t                         multiplier
)
{
  char const  *pStr;
  size_t       interval = 0;

  if (policyman_xml_get_text(pElem) != 0)
  {
    POLICYMAN_UT_MSG_0("<define_timer>: fixed interval timer must not have text");
    goto Done;
  }

  pStr = policyman_xml_get_attribute(pElem, "interval");

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_0("<define_timer>: fixed interval timer must have an 'interval' attribute");
    goto Done;
  }

  interval = (size_t) (atoi(pStr) * multiplier);

  if (interval == 0)
  {
    POLICYMAN_UT_MSG_1("<define_timer>: invalid interval for timer: %s", pStr);
  }

Done:
  return interval;
}


/*-------- get_timer_backoff_table --------*/
static policyman_set_t * get_timer_backoff_table(
  policyman_xml_element_t const *pElem,
  size_t                         multiplier
)
{
  char const      *pStr;
  policyman_set_t *pTable = NULL;
  char             token[20];
  size_t           interval;

  pStr = policyman_xml_get_attribute(pElem, "interval");

  if (pStr != NULL)
  {
    POLICYMAN_UT_MSG_0("<define_timer>: 'interval' attribute not allowed for backoff timer");
    goto Done;
  }

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_0("<define_timer>: backoff timer requires a table of intervals");
    goto Done;
  }

  pTable = policyman_set_new(sizeof(uint32), 5, POLICYMAN_SET_NO_MAX_SIZE, NULL, NULL);

  while (policyman_get_token(&pStr, token, sizeof(token)))
  {
    interval = (size_t) (atoi(token) * multiplier);
    if (interval == 0)
    {
      POLICYMAN_UT_MSG_1("<define_timer>: invalid interval for backoff interval: %s", token);
      ref_cnt_obj_release(pTable);
      pTable = NULL;
      break;
    }

    policyman_set_append(pTable, &interval);
  }

Done:
  return pTable;
}


/*-------- policyman_named_timer_action_new --------*/
mre_status_t policyman_named_timer_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  mre_status_t             status = MRE_STATUS_ERR_MALFORMED_XML;
  char const              *pName;
  char const              *pStr;
  size_t                   multiplier;
  uint32                   id;
  policyman_timer_class_t  timerClass;
  size_t                   interval = 0;
  policyman_set_t         *pBackoffTable = NULL;
  boolean                  persist = FALSE;
  named_timer_action_t    *pAction;

  /* Get the timer name
  */
  pName = policyman_xml_get_attribute(pElem, "name");

  if (pName == NULL)
  {
    POLICYMAN_UT_MSG_1(SRC_LINE "<define_timer>: needs a 'name' attribute", mre_xml_get_line_number(pElem));
    goto Done;
  }

  /*  Get the multiplier that converts the timer units to seconds.
   */
  multiplier = get_interval_multiplier(pElem);

  if (multiplier == 0)
  {
    goto Done;
  }

  /*  Get the ID for the timer - if not present, default to 1.
   */
  pStr = policyman_xml_get_attribute(pElem, "id");

  id = (pStr == NULL) ? 1 : atoi(pStr);

  /*  Get the type of timer.
   */
  timerClass = get_timer_class(pElem);

  if (timerClass == POLICYMAN_TIMER_CLASS_UNKNOWN)
  {
    goto Done;
  }

  /*  Get the information for the type of timer involved.
   */
  switch (timerClass)
  {
    case POLICYMAN_TIMER_CLASS_FIXED_INTERVAL:
      interval = get_timer_interval(pElem, multiplier);
      if (interval == 0)
      {
        goto Done;
      }
      break;

    case POLICYMAN_TIMER_CLASS_BACKOFF:
      pBackoffTable = get_timer_backoff_table(pElem, multiplier);
      if (pBackoffTable == NULL)
      {
        goto Done;
      }
      pStr = policyman_xml_get_attribute(pElem, "persist");
      persist = (pStr != NULL && strcasecmp(pStr, "true") == 0);
      break;

    default:
      goto Done;
  }

  /*  Allocate and initialize the timer create action.
   */
  pAction = POLICYMAN_ACTION_NEW( named_timer_action_t,
                                  pPolicy,
                                  policyman_timer_new_action_execute,
                                  policyman_named_timer_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  pAction->pTimerName    = policyman_str_dup(pName);
  pAction->timerClass    = timerClass;
  pAction->interval      = interval;
  pAction->pBackoffTable = pBackoffTable;
  pAction->persist       = persist;
  pAction->id            = id;

  *ppAction = (mre_action_t*)pAction;
  status = MRE_STATUS_SUCCESS;

Done:
  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "<define_timer> returned status %d",
                        POLICY_SUBS(pPolicy),
                        mre_xml_get_line_number(pElem),
                        status );
  return status;
}


/*=============================================================================
  Timer-related conditions and actions
=============================================================================*/


/*-----------------------------------------------------------------------------
  Generic timer action - many timer actions specialize this only by setting
  the specific execute action.
-----------------------------------------------------------------------------*/

#define POLICYMAN_TIMER_ACTION_BASE \
  POLICYMAN_ACTION_BASE;            \
  policyman_timer_t *pTimer;        \
  char const        *pTimerName


typedef struct
{
  POLICYMAN_TIMER_ACTION_BASE;
} timer_action_t;


/*-----------------------------------------------------------------------------
  Generic timer action functions, used by the specific timer actions.
-----------------------------------------------------------------------------*/

/*-------- policyman_timer_action_dtor --------*/
static void policyman_timer_action_dtor(
  void  *pObj
)
{
  timer_action_t *pAction = (timer_action_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pAction->pTimer);
  POLICYMAN_MEM_FREE_IF(pAction->pTimerName);
  policyman_action_dtor(pAction);
}


/*-------- policyman_timer_action_new --------*/
static mre_status_t policyman_timer_action_new(
  policyman_xml_element_t const  *pElem,
  policyman_policy_t             *pPolicy,
  mre_action_t                  **ppAction,
  pfn_execute_t                   pfnExecute,
  char const                     *pTag
)
{
  mre_status_t     status = MRE_STATUS_ERR_MALFORMED_XML;
  char const      *pName;
  timer_action_t  *pAction;

  pName = policyman_xml_get_attribute(pElem, "name");

  if (pName == NULL)
  {
    POLICYMAN_UT_MSG_1("<%s> needs a 'name' attribute", pTag);
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( timer_action_t,
                                  (mre_policy_t *)pPolicy,
                                  pfnExecute,
                                  policyman_timer_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  pAction->pTimerName = policyman_str_dup(pName);

  *ppAction = (mre_action_t*) pAction;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- policyman_timer_ensure --------*/
static policyman_timer_t * policyman_timer_ensure(
  timer_action_t  *pTimerAction
)
{
  policyman_timer_t               *pTimer;

  pTimer = pTimerAction->pTimer;

  if (pTimer == NULL && pTimerAction->pTimerName != NULL)
  {
    pTimer = (policyman_timer_t *) mre_named_object_find( pTimerAction->pTimerName,
                                                          POLICYMAN_NAMED_TIMER,
                                                          pTimerAction->pPolicy );

    if (pTimer != NULL)
    {
      ref_cnt_obj_add_ref(pTimer);
      pTimerAction->pTimer = pTimer;
    }
  }

  POLICYMAN_MEM_FREE_IF(pTimerAction->pTimerName);
  return pTimer;
}


/*-----------------------------------------------------------------------------
  <expired_timer_handled>
-----------------------------------------------------------------------------*/

/*-------- execute_expired_timer_handled --------*/
static boolean execute_expired_timer_handled(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  timer_action_t    *pTimerAction = (timer_action_t *) pAction;
  policyman_timer_t *pTimer;
  boolean            succeeded = FALSE;

  pTimer = policyman_timer_ensure(pTimerAction);

  if (pTimer != NULL)
  {
    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "executing <expired_timer_handled> on timer %d",
                          ACTION_SUBS(pTimerAction),
                          LINE_NUM(pTimerAction),
                          policyman_timer_get_id(pTimer) );

    policyman_timer_set_handled(pTimer);
    succeeded = TRUE;
  }
  else
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "No timer found for <expired_timer_handled>",
                          ACTION_SUBS(pTimerAction),
                          LINE_NUM(pTimerAction) );
  }

  return succeeded;
}

/*-------- policyman_expired_timer_handled_execute --------*/
static boolean policyman_expired_timer_handled_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_expired_timer_handled);
}


/*-------- policyman_expired_timer_handled_new --------*/
mre_status_t policyman_expired_timer_handled_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_timer_action_new( pElem,
                                     (policyman_policy_t *) pPolicy,
                                     ppAction,
                                      policyman_expired_timer_handled_execute,
                                     "expired_timer_handled" );
}


/*-----------------------------------------------------------------------------
  <timer_start>
-----------------------------------------------------------------------------*/

/*-------- execute_timer_start --------*/
STATIC boolean execute_timer_start(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  timer_action_t    *pTimerAction = (timer_action_t *) pAction;
  policyman_timer_t *pTimer;
  boolean            succeeded = FALSE;

  pTimer = policyman_timer_ensure(pTimerAction);

  if (pTimer != NULL)
  {
    POLICYMAN_MSG_HIGH_4( LOG_SUBS_PREFIX SRC_LINE "executing <timer_start> on timer %d",
                          LOG_SUBS(pTimer->asubs_id),
                          ACTION_SUBS(pTimerAction),
                          LINE_NUM(pTimerAction),
                          policyman_timer_get_id(pTimer) );

    policyman_timer_start(pTimer);
    succeeded = TRUE;
  }
  else
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "No timer found for <timer_start>",
                          ACTION_SUBS(pTimerAction),
                          LINE_NUM(pTimerAction) );
  }

  return succeeded;
}

/*-------- policyman_timer_start_execute --------*/
static boolean policyman_timer_start_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_timer_start);
}


/*-------- policyman_timer_start_new --------*/
mre_status_t policyman_timer_start_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_timer_action_new( pElem,
                                     (policyman_policy_t *) pPolicy,
                                     ppAction,
                                      policyman_timer_start_execute,
                                     "timer_start" );
}



/*-----------------------------------------------------------------------------
  <timer_stop>
-----------------------------------------------------------------------------*/

/*-------- execute_timer_stop --------*/
STATIC boolean execute_timer_stop(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  timer_action_t    *pTimerAction = (timer_action_t *) pAction;
  policyman_timer_t *pTimer;
  boolean            succeeded = FALSE;

  pTimer = policyman_timer_ensure(pTimerAction);

  if (pTimer != NULL)
  {
    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "executing <timer_stop> on timer %d",
                          ACTION_SUBS(pTimerAction),
                          LINE_NUM(pTimerAction),
                          policyman_timer_get_id(pTimer) );

    policyman_timer_stop(pTimer);
    succeeded = TRUE;
  }
  else
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX SRC_LINE "No timer found for <timer_stop>",
                          ACTION_SUBS(pTimerAction),
                          LINE_NUM(pTimerAction) );
  }


  return succeeded;
}

/*-------- policyman_timer_stop_execute --------*/
STATIC boolean policyman_timer_stop_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_timer_stop);
}


/*-------- policyman_timer_stop_new --------*/
mre_status_t policyman_timer_stop_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_timer_action_new( pElem,
                                     (policyman_policy_t *) pPolicy, ppAction,
                                      policyman_timer_stop_execute,
                                     "timer_stop" );
}

/*-------- policyman_timer_create_fixed_timer --------*/
policyman_timer_t * policyman_timer_create_fixed_timer(
  char const                  *pName,
  sys_modem_as_id_e_type       subsId,
  mre_policy_t                *pPolicy,
  uint32                       id,
  timetick_type                interval,
  policyman_timer_expirefn_t   pfnExpire
)
{
  policyman_timer_t     *pTimer;
  boolean                succeeded;

  pTimer = (policyman_timer_t *) policyman_mem_alloc(sizeof(policyman_timer_t));
  ref_cnt_obj_init(pTimer, policyman_timer_dtor);

  pTimer->pName      = policyman_str_dup(pName);
  pTimer->timerClass = POLICYMAN_TIMER_CLASS_FIXED_INTERVAL;
  pTimer->interval   = interval;
  pTimer->id         = id;
  pTimer->state      = POLICYMAN_TIMER_STATE_STOPPED;
  pTimer->asubs_id   = subsId;
  pTimer->pPolicy    = pPolicy;     // No policy will be associated with a netscan timer, runs for all policies

  succeeded = policyman_timer_init( pTimer,
                                    pfnExpire,
                                    pTimer->id,
                                    POLICYMAN_TIMER_TYPE_NONDEFERRABLE );

  if (!succeeded)
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "Couldn't initialize timer %d",
                          POLICY_SUBS_SAFE(pTimer->pPolicy),
                          pTimer->id );

    /*  Release the local copy.
     */
    REF_CNT_OBJ_RELEASE_IF(pTimer);
  }

  return pTimer;
}
