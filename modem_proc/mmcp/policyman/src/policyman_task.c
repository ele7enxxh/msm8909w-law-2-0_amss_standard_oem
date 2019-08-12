/**
  @file policyman_task.c

  @brief Policy Manager task code.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_task.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_task.h"
#include "policyman_dbg.h"
#include "policyman_diag.h"
#include "policyman_device_config.h"
#include "policyman_efs.h"
#include "policyman_msg.h"
#include "policyman_msgr_i.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_tech.h"
#include "policyman_util.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "policyman_refresh.h"
#endif

#include "modem_mem.h"
#include "err.h"
#include "atomic_ops.h"
#include "rcevt_rex.h"
#include "rex.h"

#include "dog_hb_rex.h"
#include <qurt_anysignal.h>
#include <qurt_timer.h>
#include "rcinit.h"
#include "rcevt_qurt.h"

#if defined(OLD_QSH_IFACE)
//#include "qsh.h"
#endif /* defined(OLD_QSH_IFACE) */

#define POLICYMAN_SIGNALS_MAX  32

void policyman_register_for_cm_events(void);
void policyman_register_for_uim_events(void);

policyman_crit_sect_t   policyman_item_crit_sect;
policyman_crit_sect_t   policyman_blocking_crit_sect;

STATIC policyman_tcb_t *policyman_tcb_ptr;
/*  Set used to store signals that are used to block client threads that call
 *  into policyman before it is initialized.
 */
STATIC policyman_set_t    *pBlockedSignals;

STATIC uint32             policyman_thread_id;


uint32 policyman_get_current_thread_id(
  void
)
{
  return policyman_tcb_ptr->thread_id;
}

/*****************************************************************************
******************************************************************************

  Signal pool

  The signal pool is a set of signals that can be allocated for blocking a
  thread calling into a policyman front-end API.  Once the thread is released
  after the API completes, the signal is returned to the signal pool.

  NOTE: target builds MUST use QuRT signals here, rather than rex signals,
  because we need true kernel signals in order to block and release multiple
  threads.  QuRT signals satisfy this, but rex signals are set/waited in the
  TCB of the calling thread, so are different for each client.

******************************************************************************
*****************************************************************************/


/*-----------------------------------------------------------------------------
  Define what the signal in a sigpool element is.
-----------------------------------------------------------------------------*/
typedef qurt_anysignal_t *  sigpool_signal_t;


/*-----------------------------------------------------------------------------
  A sigpool element is a structure that has an OS-specific signal type and
  a boolean that indicates whether it's available for allocation.

-----------------------------------------------------------------------------*/
typedef struct
{
  atomic_word_t     available;
  sigpool_signal_t  signal;
  qurt_anysignal_t  qurt_signal;
} sigpool_element_t;


/*-----------------------------------------------------------------------------
  The actual pool of signals to be used for commands.
-----------------------------------------------------------------------------*/
STATIC  sigpool_element_t policyman_sigpool[POLICYMAN_SIGNALS_MAX];



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Implementation of signal pool functions

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*-------- policyman_sigpool_init --------*/
/**
@brief  Initialize the policyman signal pool
*/
STATIC void policyman_sigpool_init(
  void
)
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(policyman_sigpool) ; ++i)
  {
    policyman_sigpool[i].signal = &policyman_sigpool[i].qurt_signal;
    qurt_anysignal_init(policyman_sigpool[i].signal);
    policyman_sigpool[i].available.value = TRUE;
  }
}

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

/******************************************************************************
******************************************************************************

  Client blocking signal functions

  The following functions allow policyman to block a client thread until
  policyman has completed its initialization.  They are used in
  policyman_get_items_block() and policyman_init().
  
******************************************************************************
******************************************************************************/

#define POLICYMAN_CLIENT_BLOCK_SIG 0x00000001
#define POLICYMAN_CLIENT_DBUPDATE_BLOCK_SIG 0x00000002

/*-------- policyman_client_block_signal_reset --------*/
STATIC void policyman_client_block_signal_reset(
  policyman_signal_t  signal,
  unsigned int        reason
)
{
  qurt_anysignal_t  *sig = (qurt_anysignal_t *) signal;

  if (sig != POLICYMAN_NULL_SIGNAL)
  {
    (void) qurt_anysignal_clear(sig, reason);
  }
}

/*-------- policyman_client_block_sig_return --------*/
STATIC void policyman_client_block_sig_return(
  policyman_signal_t  signal,
  unsigned int        reason
)
{
  size_t  i;

  if (signal != POLICYMAN_NULL_SIGNAL)
  {
    for (i = 0 ; i < ARR_SIZE(policyman_sigpool) ; ++i)
    {
      if (policyman_sigpool[i].signal == (sigpool_signal_t) signal)
      {
        policyman_client_block_signal_reset((policyman_signal_t) policyman_sigpool[i].signal, reason);
        policyman_sigpool[i].available.value = TRUE;
        break;
      }
    }
  }
}


/*-------- signal_wait --------*/
STATIC void signal_wait(
  policyman_signal_t  signal,
  unsigned int        reason

)
{
  if (signal != POLICYMAN_NULL_SIGNAL)
  {
    (void) qurt_anysignal_wait((qurt_anysignal_t *) signal, reason);
    policyman_client_block_sig_return(signal, reason);
  }
}

/*-------- policyman_client_block_signal_wait --------*/
void policyman_client_block_signal_wait(
  policyman_signal_t  signal
)
{
  signal_wait(signal, POLICYMAN_CLIENT_BLOCK_SIG);
}

/*-------- policyman_client_dbupdate_block_signal_wait --------*/
void policyman_client_dbupdate_block_signal_wait(
  policyman_signal_t  signal
)
{
  signal_wait(signal, POLICYMAN_CLIENT_DBUPDATE_BLOCK_SIG);
}

/*-------- signal_set --------*/
STATIC void signal_set(
  policyman_signal_t  signal,
  unsigned int        reason
)
{
  qurt_anysignal_t  *sig = (qurt_anysignal_t *) signal;

  if (sig != POLICYMAN_NULL_SIGNAL)
  {
    qurt_anysignal_set(sig, reason);
  }
}

/*-------- policyman_client_block_signal_set --------*/
void policyman_client_block_signal_set(
  policyman_signal_t  signal
)
{
  signal_set(signal, POLICYMAN_CLIENT_BLOCK_SIG);
}

/*-------- policyman_client_dbupdate_block_signal_set --------*/
void policyman_client_dbupdate_block_signal_set(
  policyman_signal_t  signal
)
{
  signal_set(signal, POLICYMAN_CLIENT_DBUPDATE_BLOCK_SIG);
}


/*-------- policyman_client_block_sig_get --------*/
policyman_signal_t policyman_client_block_sig_get(
  void
)
{
  size_t              i;
  policyman_signal_t  signal = POLICYMAN_NULL_SIGNAL;

  for (i = 0 ; i < ARR_SIZE(policyman_sigpool) ; ++i)
  {
    if (atomic_compare_and_set(&policyman_sigpool[i].available, TRUE, FALSE))
    {
      signal = (policyman_signal_t) policyman_sigpool[i].signal;
      break;
    }

  }

  if (signal == POLICYMAN_NULL_SIGNAL)
  {
    POLICYMAN_ERR_FATAL("Unable to allocate signal from policyman signal pool", 0, 0, 0);
  }

  return signal;
}


/******************************************************************************
******************************************************************************

  SignalSet

  The policyman_signalset functions abstract the ability to block a thread on
  multiple signals.  This is used on the PM task thread, since it requires
  blocking on command availability, watchdog timer, and stop signal.
  
******************************************************************************
******************************************************************************/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  QURT implementation of signalset functions.

  The PM task itself needs to wait on separate bits on the PM task thread.
  In this case, we use a single qurt_anysignal_t (referenced by the QURT
  implementation of policyman_signalset) and pass a mask to it.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*-------- policyman_signalset --------*/
/**
@brief  Definition of a "signal set" for REX.

In REX, this is just a bitmask in a policyman_signal.
*/
typedef struct
{
  unsigned int      set;
} policyman_signalset_t;


/*-------- policyman_signalset_clear --------*/
/**
@brief  Initialize a signal set to "empty".
*/
STATIC void policyman_signalset_clear(
  policyman_signalset_t  *sigset
)
{
  sigset->set = 0;
}

/*-------- policyman_signalset_init --------*/
/**
@brief  Do any necessary initialization of a signal set object
*/
STATIC void policyman_signalset_init(
  policyman_signalset_t  *sigset
)
{
  policyman_signalset_clear(sigset);
}

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

/*-------- policyman_signalset_add --------*/
/**
@brief  Add a signal to the signal set.
*/
STATIC void policyman_signalset_add(
  policyman_signalset_t  *sigset,
  policyman_signal_t     sig
)
{
  sigset->set |= sig;
}


/*-------- policyman_signalset_contains --------*/
/**
@brief  Return TRUE if a signal is in the signal set, FALSE if not.
*/
STATIC boolean policyman_signalset_contains(
  policyman_signalset_t  *sigset,
  policyman_signal_t     sig
)
{
  return (sigset->set & sig) != 0;
}

/*-------- policyman_signalset_wait --------*/
/**
@brief  Wait on the signals in a signal set.

This returns if any of the signals is set.  The signals that freed the wait are
returned in the 'firedsigs' signal set.
*/
STATIC void policyman_signalset_wait(
  policyman_signalset_t  *sigset,
  policyman_signalset_t  *firedsigs
)
{
  firedsigs->set = rex_wait(sigset->set);
  rex_clr_sigs(policyman_tcb_ptr, firedsigs->set);
}

/*-------- policyman_signalset_set --------*/
/**
@brief  Set a signal in a signal set.

Returns TRUE if the signal was set, FALSE otherwise.
*/
STATIC boolean policyman_signalset_set(
  policyman_signalset_t  *sigset,
  policyman_signal_t     sig
)
{
  if (!policyman_signalset_contains(sigset, sig))
  {
    return FALSE;
  }

  rex_set_sigs(policyman_tcb_ptr, sig);

  return TRUE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  End of implementation of signal functions.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*****************************************************************************
******************************************************************************

  Command functions

******************************************************************************
*****************************************************************************/

/*-------- policyman_cmd_dtor --------*/
/**
@brief  Command destructor

Called when the reference count of the command goes to zero.  This calls
the specific command's destructor, then cleans up all other resources
allocated to the command before freeing its memory.
*/
STATIC void policyman_cmd_dtor(
  void  *pobj
)
{
  policyman_cmd_t  *me = (policyman_cmd_t *) pobj;

  /* First call the destructor for the derived command.
  */
  if (me->dtor != NULL)
  {
    me->dtor(me);
  }

  /*  Free any signal allocated to this command.
   */
  policyman_client_block_sig_return(me->signal, POLICYMAN_CLIENT_BLOCK_SIG);

  /* Free the command memory.
  */
  policyman_mem_free(me);
}


/*-------- policyman_cmd_new --------*/
/**
@brief  Command constructor

Allocate a new command structure of the size passed in, then initialize
the base portion of the command (the POLICYMAN_CMD_HDR portion).  Note that this
may fail with a fatal error if there are no available signals in the 
PM signal pool.
*/
policyman_cmd_t * policyman_cmd_new(
  size_t              size,
  execute_fn_t        execute,
  dtor_fn_t           dtor,
  policyman_policy_t  *pPolicy
)
{
  /* Allocate the command
  */
  policyman_cmd_t  *pCmd = policyman_mem_alloc(size);

  /* Intialize reference counting on the command
  */
  ref_cnt_obj_init(pCmd, policyman_cmd_dtor);

  /* Fill in the command header
  */
  pCmd->execute           = execute;
  pCmd->dtor              = dtor;
  pCmd->signal            = POLICYMAN_NULL_SIGNAL;
  pCmd->pPolicy           = pPolicy;
  pCmd->pState            = policyman_state_get_state();
  pCmd->hook              = NULL;

  return pCmd;
}

/*-------- policyman_cmd_with_hook_new --------*/
policyman_cmd_t * policyman_cmd_with_hook_new(
  size_t               size,
  execute_fn_t         execute,
  dtor_fn_t            dtor,
  policyman_policy_t  *pPolicy,
  notification_hook    hook
)
{
  policyman_cmd_t  *pCmd; 

  pCmd = policyman_cmd_new( size,
                            execute,
                            dtor,
                            pPolicy );


  pCmd->hook = hook;

  return pCmd;
}


/*****************************************************************************
******************************************************************************

  Command queue

******************************************************************************
*****************************************************************************/

STATIC q_type                 policyman_cmd_q;
STATIC policyman_signal_t     policyman_cmd_signal;
STATIC policyman_signal_t     policyman_stop_signal;
STATIC policyman_signal_t     policyman_CM_ready_signal;
STATIC policyman_signal_t     policyman_MMGSDI_ready_signal;
STATIC policyman_signal_t     policyman_msgr_signal;
STATIC policyman_signal_t     policyman_done_init_signal;

STATIC policyman_signalset_t  policyman_waitsigs;


/*-------- policyman_queue_init --------*/
/**
@brief  Initialize the PM command queue.
*/
STATIC void policyman_queue_init(
  void
)
{
  (void) q_init(&policyman_cmd_q);
}

/*-------- policyman_queue_put_cmd --------*/
/**
@brief  Add a command to the command queue.

Add the command passed in to the command queue and set the command queue's
signal, unblocking the dispatch thread if it is waiting for a command to
become available.
*/
void policyman_queue_put_cmd(
  policyman_cmd_t  *cmd
)
{
  /*  Add a reference for the queue.
   */
  ref_cnt_obj_add_ref(cmd);
  
  q_put(&policyman_cmd_q, &cmd->link);

  policyman_signalset_set(&policyman_waitsigs, policyman_cmd_signal);
}

/*-------- policyman_queue_get_cmd --------*/
/**
@brief  Get a command from the command queue.

Get a command from the command queue, or block on the command queue's
signal if the queue is empty.
*/
STATIC policyman_cmd_t * policyman_queue_get_cmd(
  void
)
{
  void            *qlink = NULL;
  policyman_cmd_t *cmd;

  qlink = q_get(&policyman_cmd_q);

  if (qlink == NULL)
  {
    cmd = NULL;
  }
  else
  {
    cmd = (policyman_cmd_t *) (((byte *) qlink) - offsetof(policyman_cmd_t, link));
  }
  
  return cmd;
}


#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */


/*****************************************************************************
******************************************************************************

  PM task functions

******************************************************************************
*****************************************************************************/

STATIC policyman_signal_t  policyman_dog_signal;
STATIC dog_report_type  policyman_dog_rpt = 0;

STATIC void policyman_pet_dog(
  void
)
{
  dog_hb_report(policyman_dog_rpt);
}


/*-------- policyman_init_before_start --------*/
/**
@brief  Initialize the PM command queue and signals before the task thread is
        started.
*/
void policyman_init_before_start(
  void
)
{
  policyman_cmd_signal = (policyman_signal_t) 0x00000001;
  policyman_dog_signal = (policyman_signal_t) 0x00000002;
  policyman_stop_signal = (policyman_signal_t) 0x00000004;
  policyman_CM_ready_signal = (policyman_signal_t) 0x00000008;
  policyman_MMGSDI_ready_signal = (policyman_signal_t) 0x00000010;
  policyman_msgr_signal = (policyman_signal_t) 0x00000020;
  policyman_done_init_signal =    (policyman_signal_t) 0x00000040;

  policyman_init_crit_sect(&policyman_item_crit_sect);
  policyman_init_crit_sect(&policyman_blocking_crit_sect);
  policyman_queue_init();
}


/*-------- policyman_init_after_start --------*/
/**
@brief  Do the rest of PM initialization after the task thread is started.
*/
void policyman_init_after_start(
  void
)
{

  policyman_tcb_ptr = rex_self();

  policyman_signalset_set(&policyman_waitsigs, policyman_dog_signal);

  /* Register with watchdag HB API
  */
  policyman_dog_rpt = dog_hb_register_rex(policyman_dog_signal);
  
  policyman_pet_dog();

}


/*-------- policyman_process_cmds --------*/
/**
@brief  Process any commands that are in the command queue.
*/
void policyman_process_cmds(
  void
)
{
  policyman_cmd_t    *cmd;

  while ((cmd = policyman_queue_get_cmd()) != NULL)
  {
    cmd->execute(cmd);
    policyman_client_block_signal_set(cmd->signal);
    ref_cnt_obj_release(cmd);
  }
}


/*-------- policyman_rcinit --------*/
/**
@brief  Do initialization for the RCINIT framework.
*/
STATIC void policyman_rcinit(
  void
)
{
  /* In theory, tasks should register with RCINIT for shutdown signals.
   * However, some Policyman clients do not listen to the stop signal 
   * and continue to call Policyman -- leading to crashes if Policyman 
   * shuts down.
   * 
   * Registration for stop signals is only done in the test 
   * environment where the task needs to be cleanly stopped. 
   */
#if defined(TEST_FRAMEWORK)
  #error code not present
#endif /* +TEST_FRAMEWORK. */

  rcinit_handshake_startup();
  (void) rcevt_signal_name(POLICYMAN_RCEVT_READY);
}


#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

/*-------- policyman_unregister_for_task_sync --------*/
void policyman_unregister_for_task_sync(
  char const            *pEvtName,
  RCEVT_SIGEX_SIGREX   *pEvtInfo
)
{
  (void) rcevt_unregister_sigex_name(pEvtName, RCEVT_SIGEX_TYPE_SIGREX, pEvtInfo);
}

/*-------- policyman_register_for_task_sync --------*/
boolean policyman_register_for_task_sync(
  char const            *pEvtName,
  policyman_signal_t    sig,
  RCEVT_SIGEX_SIGREX    *pEvtInfo
)
{
  boolean taskReady;

  pEvtInfo->signal = policyman_tcb_ptr;
  pEvtInfo->mask   = sig;
  (void) rcevt_register_sigex_name(pEvtName, RCEVT_SIGEX_TYPE_SIGREX, pEvtInfo);

  taskReady = (rcevt_getcount_name(pEvtName) > 0);
  if (taskReady)
  {
    policyman_unregister_for_task_sync(pEvtName, pEvtInfo);
  }

  return taskReady;
}

#if defined(OLD_QSH_IFACE)
/*-------- policyman_task_qsh_cb --------*/
boolean policyman_task_qsh_cb(
  qsh_cb_params_s  *qsh_cb_params
)
{
  if (   (NULL != qsh_cb_params)
      && (QSH_ACTION_DUMP == qsh_cb_params->action) )
  {
    policyman_cfgitem_handle_dump(qsh_cb_params);
  }

  return TRUE;
}

/*-------- policyman_task_qsh_cb_done --------*/
void policyman_task_qsh_cb_done(
  uint32      context_id,
  size_t      dump_size_bytes,
  char       *alt_name,
  int         name_len
)
{
  qsh_client_cb_done_s    client_cb_done;

  qsh_client_cb_done_init(&client_cb_done);

  client_cb_done.client          = QSH_CLT_POLICYMAN;
  client_cb_done.context_id      = context_id;
  client_cb_done.dump_size_bytes = dump_size_bytes;

  if (NULL != alt_name)
  {
    memscpy( &client_cb_done.alt_client_name,
             sizeof(client_cb_done.alt_client_name),
             alt_name,
             name_len );
  }

  qsh_client_cb_done(&client_cb_done);
}

/*-------- policyman_task_qsh_register_client --------*/
void policyman_task_qsh_register_client(
  void
)
{
  qsh_client_reg_s client_reg;

  /* initialize reg params */
  qsh_client_reg_init(&client_reg);

  client_reg.client        = QSH_CLT_POLICYMAN;
  client_reg.client_cb_ptr = policyman_task_qsh_cb;
  client_reg.major_ver     = 1;
  client_reg.minor_ver     = 0;

  client_reg.dump_info[QSH_DUMP_TYPE_MINI].size_bytes = sizeof(LOG_PM_MINI_DUMP_INFO_type);
  client_reg.dump_info[QSH_DUMP_TYPE_FULL].size_bytes = sizeof(LOG_PM_DUMP_INFO_type);

  client_reg.action_support_mask = QSH_ACTION_DUMP;

  /* register client */
  qsh_client_reg(&client_reg);
}
#endif /* defined(OLD_QSH_IFACE) */

/*-------- policyman_get_tcb_pointer --------*/
/**
@brief  Returns rex_tcb_type pointer that can be accessed outside.
*/
rex_tcb_type *policyman_get_tcb_pointer(
  void
)
{
  return (rex_tcb_type *)policyman_tcb_ptr;
}

/*-------- policyman_get_thread_id --------*/
/**
@brief  Returns the policyman task's thread id
*/
int policyman_get_thread_id(
  void
)
{
  return (int)(policyman_get_tcb_pointer()->thread_id);
}

/*-------- policyman_get_caller_thread_id --------*/
/**
@brief  Returns the calling task's thread id
*/
int policyman_get_caller_thread_id(
  void
)
{
  return (int)(rex_self()->thread_id);
}

/*-------- policyman_caller_is_client --------*/
/**
@brief  Returns true if the calling task's thread id is not policyman's
*/
boolean policyman_caller_is_client(
  void
)
{
  return policyman_get_thread_id() != policyman_get_caller_thread_id();
}


/*-------- policyman_is_initializing --------*/
/**
@brief Check if Policyman is initializing.
*/
boolean policyman_is_initializing(
  void
)
{
  boolean result;

  policyman_enter_crit_sect(&policyman_blocking_crit_sect);
  result =  pBlockedSignals != NULL;
  policyman_leave_crit_sect(&policyman_blocking_crit_sect);

  return result;
}


/*-------- policyman_blocked_clients_init --------*/
/**
@brief Create a set for blocking clients during initialization.
*/
void policyman_blocked_clients_init(
  void
)
{
  policyman_enter_crit_sect(&policyman_blocking_crit_sect);
  pBlockedSignals = policyman_set_new(sizeof(policyman_signal_t), 5, POLICYMAN_SET_NO_MAX_SIZE, NULL, NULL);
  policyman_leave_crit_sect(&policyman_blocking_crit_sect);
}


/*-------- release_blocked_client --------*/
boolean release_blocked_client(
  void  *pElem,
  void  *pCtx
)
{
  policyman_signal_t  *pSignal = (policyman_signal_t *) pElem;

  if (*pSignal != POLICYMAN_NULL_SIGNAL)
  {
    policyman_client_block_signal_set(*pSignal);
  }

  return TRUE;
}


/*-------- policyman_blocked_clients_release --------*/
/**
@brief  Release the set used for blocked clients.
*/
void policyman_blocked_clients_release(
  void
)
{
  policyman_set_t    *pBlockedSignals_temp;

  /*Get the pointer to the set into a local variable and clear the global.
      This allows us to stop blocking clients while we relesase all currently
      blocked clients
   */
  policyman_enter_crit_sect(&policyman_blocking_crit_sect);
  pBlockedSignals_temp = pBlockedSignals;
  pBlockedSignals      = NULL;
  policyman_leave_crit_sect(&policyman_blocking_crit_sect);
  
  // Unblock clients
  mre_set_iterate(pBlockedSignals_temp, release_blocked_client, NULL);

  // Release the set
  ref_cnt_obj_release(pBlockedSignals_temp);
}


/*-------- policyman_blocked_clients_add --------*/
/**
@brief  Block client during initialization.  Private function to be used by PM task code.

@return
  None
*/
void policyman_blocked_clients_add(
  void
)
{
  policyman_signal_t signal;

  signal = policyman_client_block_sig_get();
  
  policyman_enter_crit_sect(&policyman_blocking_crit_sect);
  policyman_set_add(pBlockedSignals, &signal);
  policyman_leave_crit_sect(&policyman_blocking_crit_sect);
  policyman_client_block_signal_wait(signal);
}


/*-------- policyman_task --------*/
/**
@brief  Dispatch look of the PM task.
*/
void policyman_task(
  void
  )
{
  policyman_signalset_t  fired_sigs;

  boolean               taskReady;
  RCEVT_SIGEX_SIGREX    pm_rcevt_siginfo_cm;
  RCEVT_SIGEX_SIGREX    pm_rcevt_siginfo_mmgsdi;

  POLICYMAN_MSG_HIGH_0("PM task starting.");

  policyman_init_after_start();

  policyman_signalset_init(&fired_sigs);

  policyman_signalset_init(&policyman_waitsigs);
  policyman_signalset_add(&policyman_waitsigs, policyman_cmd_signal);
  policyman_signalset_add(&policyman_waitsigs, policyman_stop_signal);
  policyman_signalset_add(&policyman_waitsigs, policyman_dog_signal);
  policyman_signalset_add(&policyman_waitsigs, policyman_CM_ready_signal);
  policyman_signalset_add(&policyman_waitsigs, policyman_MMGSDI_ready_signal);
  policyman_signalset_add(&policyman_waitsigs, policyman_msgr_signal);

  /* Get the task's thread id so we can ERR_FATAL if a front-end API
  ** is called from that thread (which will deadlock the PM task and
  ** whatever task called the API that led to PM calling a front-end
  ** API from the task thread).
  */
  policyman_thread_id = policyman_get_current_thread_id();

  /*  Initialize the signal pool of signals used to block clients making API calls.
   */
  policyman_sigpool_init();
  
  policyman_tech_init();
  policyman_device_config_init();


  /*  Get a set in which to store any signals used to block threads which call
   *    us while we're initializing.
   */
  policyman_blocked_clients_init();

  /*  We're up enough to let others know we're alive.
   */
  policyman_rcinit();

  /*  Initialize our message router client.
   */
  policyman_msgr_init(policyman_msgr_signal, policyman_tcb_ptr);

  /*  Do main initialization, loading policies, etc.
   */
  policyman_init();

  /* Release the set for blocking clients
  */
  policyman_blocked_clients_release(); 
  
  /*  Register for synchronization with MMGSDI.
   */
  taskReady = policyman_register_for_task_sync("MM_GSDI: READY",
                                                policyman_MMGSDI_ready_signal,
                                                &pm_rcevt_siginfo_mmgsdi );
  if (taskReady)
  {
    POLICYMAN_MSG_HIGH_0("MMGSDI task is up...");
    policyman_register_for_uim_events();
  }

  /*  Register for synchronization with CM.
   */
  taskReady = policyman_register_for_task_sync("cm:ready",
                                                policyman_CM_ready_signal,
                                                &pm_rcevt_siginfo_cm );
  if (taskReady)
  {
    POLICYMAN_MSG_HIGH_0("CM task is up...");
    policyman_register_for_cm_events();
  }
  
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  policyman_register_for_mcfg_refresh();
#endif

#if defined(OLD_QSH_IFACE)
  policyman_task_qsh_register_client();
#endif /* defined(OLD_QSH_IFACE) */

  POLICYMAN_MSG_HIGH_0("PM task init complete, entering command phase");

  while (TRUE)
  {
    policyman_signalset_clear(&fired_sigs);
    policyman_signalset_wait(&policyman_waitsigs, &fired_sigs);

    if (policyman_signalset_contains(&fired_sigs, policyman_dog_signal))
    {
      policyman_pet_dog();
    }  

    if (policyman_signalset_contains(&fired_sigs, policyman_cmd_signal))
    {
      policyman_process_cmds();
    }

    if (policyman_signalset_contains(&fired_sigs, policyman_msgr_signal))
    {
      policyman_process_msgr();
    }
    
    if (policyman_signalset_contains(&fired_sigs, policyman_CM_ready_signal))
    {
      POLICYMAN_MSG_HIGH_0("CM task is up...");
      policyman_unregister_for_task_sync("cm:ready", &pm_rcevt_siginfo_cm);    
      policyman_register_for_cm_events();
    }

    if (policyman_signalset_contains(&fired_sigs, policyman_MMGSDI_ready_signal))
    {
      POLICYMAN_MSG_HIGH_0("MMGSDI task is up...");
      policyman_unregister_for_task_sync("MM_GSDI: READY", &pm_rcevt_siginfo_mmgsdi);    
      policyman_register_for_uim_events();
    }

#if defined(TEST_FRAMEWORK)
    #error code not present
#endif /* +TEST_FRAMEWORK. */

  }

  /* The loop above is infinite unless the test framework is enabled.
   * During testing, cleanup the task completely.
   */
#if defined(TEST_FRAMEWORK)
  #error code not present
#endif /* +TEST_FRAMEWORK. */
}
