/*============================================================================
  FILE:       sdtask.c

  OVERVIEW:

              Copyright (c) 2012 Qualcomm Technologies Incorporated.
              All Rights Reserved.
              Qualcomm Confidential and Proprietary
============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdtask.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/16/13   jvo     Added sd_is_init_complete() and sd_task_info.
??                 First version
===========================================================================*/

/*=============================================================================
  IMPORTANT:

  Do not directly use OS signal primitives in SD task code.  These have been
  abstracted out in a way that the same code can be run on multiple OSes - in
  particular, they will run in PC unit tests *and* on target.  If you need
  signals, use the sdtask_signal or sdtask_signalset functions.
=============================================================================*/


#include "sdtask.h"
#include "modem_mem.h"
#include "err.h"
#include "queue.h"
#include "atomic_ops.h"
#include "sddbg.h"
#include "sdss.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#include "sdcmd.h"
#endif

/*-----------------------------------------------------------------------------
  Map the various Windows defines to the single WIN32.
-----------------------------------------------------------------------------*/
#if !defined(WIN32) && (defined(_WIN32) || defined(WINNT))
  #define WIN32
#endif

/*-----------------------------------------------------------------------------
  Map the various target defines to the single TARGET_BUILD
-----------------------------------------------------------------------------*/
#if !defined(WIN32) && (defined(FEATURE_QDSP6) || defined(IMAGE_QDSP6_PROC))
  #define TARGET_BUILD
#endif


#if defined(WIN32)

#include "windows.h"

/*-----------------------------------------------------------------------------
  This is the signal that will be used to synchronize SUTE with the SD task
  thread.
-----------------------------------------------------------------------------*/
HANDLE  sd_start_signal;

#elif defined(TARGET_BUILD)

#include "dog_hb_qurt.h"                              /*Interface for new Watchdog services*/
#include <qurt_anysignal.h>
#include <qurt_timer.h>
#include "rcinit.h"
#include "rcevt_qurt.h"

static rex_tcb_type     *sd_tcb_ptr;

#endif


static sd_signal_t  sd_stop_signal;
static uint32       sdtask_thread_id;



#define SD_SIGNALS_MAX  32




#if defined(WIN32)

uint32 sdtask_get_current_thread_id(void)
{
  return GetCurrentThreadId();
}

#elif defined(TARGET_BUILD)

uint32 sdtask_get_current_thread_id(void)
{
  rex_tcb_type  *tcb = rex_self();
  return tcb->thread_id;
}

#endif



/*****************************************************************************
******************************************************************************

  Signal

  The sdtask_signal functions abstract the underlying OS signals in a way
  that these functions can be used both on Windows (for unit tests) and on
  target.  These are used in cases in which we need to wait only a single
  signal (as we do on the thread calling an API).  For waiting on multiple
  signals (as we do on the SD task thread), use sdtask_signalset.

******************************************************************************
*****************************************************************************/

#define NULL_SIGNAL ((sd_signal_t) 0)


#if defined(WIN32)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Windows implementation of signal functions.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*----------------------------------------------------------------------------
  sdtask_signal_wait
    Block the calling thread until the signal passed in is set.
----------------------------------------------------------------------------*/
static sd_signal_t sdtask_signal_wait(
  sd_signal_t signal
  )
{
  if (signal != NULL_SIGNAL)
  {
    WaitForSingleObject((HANDLE) signal, INFINITE);
  }

  return signal;
}

/*----------------------------------------------------------------------------
  sdtask_signal_set
    Set the signal that is passed in, releasing any thread that is waiting
    in sdtask_signal_wait().
----------------------------------------------------------------------------*/
static void        sdtask_signal_set(
  sd_signal_t signal
  )
{
  if (signal != NULL_SIGNAL)
  {
    SetEvent((HANDLE) signal);
  }
}



#elif defined(TARGET_BUILD)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  QURT implementation of signal functions (for target).

  Since multiple threads cannot wait on different signal bits within a given
  qurt_anysignal_t, signals used to block threads while waiting for commands
  need to be distinct qurt_anysignal_t objects.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* This is the bit used in each of the signals used by commands */
#define SDTASK_QURT_SIG_BIT 0x00000001

static sd_signal_t sdtask_signal_wait(
  sd_signal_t signal
  )
{
  qurt_anysignal_t  *sig = (qurt_anysignal_t *) signal;

  if (signal != NULL_SIGNAL)
  {
    (void) qurt_anysignal_wait(sig, SDTASK_QURT_SIG_BIT);
    (void) qurt_anysignal_clear(sig, SDTASK_QURT_SIG_BIT);
  }

  return SDTASK_QURT_SIG_BIT;
}

static void        sdtask_signal_set(
  sd_signal_t signal
  )
{
  qurt_anysignal_t  *sig = (qurt_anysignal_t *) signal;

  if (signal != NULL_SIGNAL)
  {
    qurt_anysignal_set(sig, SDTASK_QURT_SIG_BIT);
  }
}


#endif
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  End of target-specific implementation of signal functions.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/******************************************************************************
******************************************************************************

  SignalSet

  The sdtask_signalset functions abstract the ability to block a thread on
  multiple signals.  This is used on the SD task thread, since it requires
  blocking on command availability, watchdog timer, and stop signal.

******************************************************************************
******************************************************************************/


#if defined(WIN32)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Windows implementation of signalset functions (for unit tests).

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*-----------------------------------------------------------------------------
  sd_signalset_t

  Define a "signal set", i.e. a set of signals that can be waited on.
  In Windows, WaitForMultipleObjects takes an array of handles and the size
  of the array, so we will keep this information.
-----------------------------------------------------------------------------*/
typedef struct
{
  size_t      count;
  sd_signal_t set[SD_SIGNALS_MAX];
} sd_signalset_t;


/*-----------------------------------------------------------------------------
  sdtask_signalset_init

  Initialize a signal set to "empty".
-----------------------------------------------------------------------------*/
static void sdtask_signalset_init(
  sd_signalset_t  *sigset
  )
{
  sigset->count = 0;
}

/*-----------------------------------------------------------------------------
  sdtask_signalset_dtor

  Destructor for an sd_signalset.
-----------------------------------------------------------------------------*/
static void sdtask_signalset_dtor(
  sd_signalset_t  *sigset
  )
{
  size_t  i;

  for (i = 0 ; i < sigset->count ; ++i)
  {
    sd_signal_t sig = sigset->set[i];
    sigset->set[i] = NULL_SIGNAL;
    CloseHandle((HANDLE) sig);
  }
}

/*-----------------------------------------------------------------------------
  sdtask_signalset_add

  Add a signal to the signal set.
-----------------------------------------------------------------------------*/
static void sdtask_signalset_add(
  sd_signalset_t  *sigset,
  sd_signal_t     sig
  )
{
  if (sigset->count < SD_SIGNALS_MAX - 1)
  {
    sigset->set[sigset->count++] = sig;
  }
}



/*-----------------------------------------------------------------------------
  sdtask_signalset_contains

  Return TRUE if a signal is in the signal set, FALSE if not.
-----------------------------------------------------------------------------*/
static boolean sdtask_signalset_contains(
  sd_signalset_t  *sigset,
  sd_signal_t     sig
  )
{
  size_t  i;

  for (i = 0 ; i < sigset->count ; ++i)
  {
    if (sigset->set[i] == sig)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*-----------------------------------------------------------------------------
  sdtask_signalset_wait

  Wait on the signals in a signal set.  This returns if any of the signals is
  set.  The signals that freed the wait are returned in the 'firedsigs'
  signal set.

  Note that at the moment on Windows we only return the lowest-index signal from
  those on which we waited, because that is all WaitForMultipleObjects returns.
-----------------------------------------------------------------------------*/
static void sdtask_signalset_wait(
  sd_signalset_t  *sigset,
  sd_signalset_t  *firedsigs
  )
{
  uint32  result;

//  SD_MSG_HIGH_0("Wait on signalset");

  if (sigset->count == 0)
  {
    firedsigs->count = 0;
    return;
  }

  result = WaitForMultipleObjects(
              sigset->count,
              (HANDLE *) sigset->set,
              FALSE,
              INFINITE
              );

  SD_ASSERT(    result < WAIT_ABANDONED_0
            &&  (result - WAIT_OBJECT_0) < sigset->count
           );

  firedsigs->count = 1;
  firedsigs->set[0] = sigset->set[result - WAIT_OBJECT_0];
}


/*-----------------------------------------------------------------------------
  sdtask_signalset_set

  Set a signal in a signal set.  Returns TRUE if the signal was set, FALSE
  otherwise.
-----------------------------------------------------------------------------*/
static boolean sdtask_signalset_set(
  sd_signalset_t  *sigset,
  sd_signal_t     sig
  )
{

  if (!sdtask_signalset_contains(sigset, sig))
  {
    return FALSE;
  }

  SetEvent((HANDLE) sig);
  return TRUE;
}



#elif defined(TARGET_BUILD)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  QURT implementation of signalset functions (for target).

  The SD task itself needs to wait on separate bits on the SD task thread.
  In this case, we use a single qurt_anysignal_t (referenced by the QURT
  implementation of sdtask_signalset) and pass a mask to it.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*-----------------------------------------------------------------------------
  sd_signalset_t

  Define a "signal set", i.e. a set of signals that can be waited on.
  In QURT, this is just a bitmask in an sd_signal_t.  This is used to wait on
  a singleton qurt_anysignal_t.
-----------------------------------------------------------------------------*/
typedef struct
{
  qurt_anysignal_t  signal;
  unsigned int      set;
  boolean           initialized;
} sd_signalset_t;


/*-----------------------------------------------------------------------------
  sdtask_signalset_init

  Initialize a signal set to "empty".
-----------------------------------------------------------------------------*/
static void sdtask_signalset_init(
  sd_signalset_t  *sigset
  )
{
  if (!sigset->initialized)
  {
    qurt_anysignal_init(&sigset->signal);
    sigset->initialized = TRUE;
  }

  sigset->set = 0;
}

/*-----------------------------------------------------------------------------
  sdtask_signalset_dtor

  Destructor for an sd_signalset.
-----------------------------------------------------------------------------*/
static void sdtask_signalset_dtor(
  sd_signalset_t  *sigset
  )
{
  qurt_anysignal_destroy(&sigset->signal);
  sigset->initialized = FALSE;
}

/*-----------------------------------------------------------------------------
  sdtask_signalset_add

  Add a signal to the signal set.
-----------------------------------------------------------------------------*/
static void sdtask_signalset_add(
  sd_signalset_t  *sigset,
  sd_signal_t     sig
  )
{
  sigset->set |= sig;
}


/*-----------------------------------------------------------------------------
  sdtask_signalset_contains

  Return TRUE if a signal is in the signal set, FALSE if not.
-----------------------------------------------------------------------------*/
static boolean sdtask_signalset_contains(
  sd_signalset_t  *sigset,
  sd_signal_t     sig
  )
{
  return (sigset->set & sig) != 0;
}

/*-----------------------------------------------------------------------------
  sdtask_signalset_wait

  Wait on the signals in a signal set.  This returns if any of the signals is
  set.  The signals that freed the wait are returned in the 'firedsigs'
  signal set.
-----------------------------------------------------------------------------*/
static void sdtask_signalset_wait(
  sd_signalset_t  *sigset,
  sd_signalset_t  *firedsigs
  )
{
  firedsigs->set = qurt_anysignal_wait(&sigset->signal, sigset->set);
  qurt_anysignal_clear(&sigset->signal, firedsigs->set);
}

/*-----------------------------------------------------------------------------
  sdtask_signalset_set

  Set a signal in a signal set.  Returns TRUE if the signal was set, FALSE
  otherwise.
-----------------------------------------------------------------------------*/
static boolean sdtask_signalset_set(
  sd_signalset_t  *sigset,
  sd_signal_t     sig
  )
{
  if (!sdtask_signalset_contains(sigset, sig))
  {
    return FALSE;
  }

  qurt_anysignal_set(&sigset->signal, sig);
  return TRUE;
}


#endif
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  End of target-specific implementation of signal functions.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/




/*****************************************************************************
******************************************************************************

  Signal pool

  The signal pool is a set of signals that can be allocated for blocking a
  thread calling into an SD front-end API.  Once the thread is released after
  the API completes, the signal is returned to the signal pool.

******************************************************************************
*****************************************************************************/


/*-----------------------------------------------------------------------------
  Define what the signal in a sigpool element is.
-----------------------------------------------------------------------------*/
#if defined(WIN32)
typedef HANDLE              sigpool_signal_t;
#elif defined(TARGET_BUILD)
typedef qurt_anysignal_t *  sigpool_signal_t;
#endif

/*-----------------------------------------------------------------------------
  A sigpool element is a structure that has an OS-specific signal type and
  a boolean that indicates whether it's available for allocation.
  On target, we also include the actual signal in the element; on Windows,
  we have to call CreateEvent to get the signal, and we just store its handle
  in the element.
-----------------------------------------------------------------------------*/
typedef struct
{
  atomic_word_t     available;
  sigpool_signal_t  signal;

#if defined(TARGET_BUILD)
  /*---------------------------------------------------------------------------
    On target builds, a pointer to this signal will be put into the signal
    field.  This allows the same code to work on both Windows and target,
    because we just use the signal variable in both cases.
  ---------------------------------------------------------------------------*/
  qurt_anysignal_t  qurt_signal;
#endif

} sd_sigpool_element_t;


/*-----------------------------------------------------------------------------
  The actual pool of signals to be used for commands.
-----------------------------------------------------------------------------*/
static  sd_sigpool_element_t sd_sigpool[SD_SIGNALS_MAX];


/*****************************************************************************
******************************************************************************

  SD task info object

******************************************************************************
*****************************************************************************/

/* Type declaration */

/*-----------------------------------------------------------------------------
  sdtask_info_s_type
    A type that associates the SD Task generic info other than state info.
-----------------------------------------------------------------------------*/
typedef struct
{
  /*-------------Start debug information section--------------*/

  /* Ptr to the task pointer */
  uint32                   *thread_id;                 /*= &sdtask_thread_id*/

  /* Ptr to the signal pool */
  sd_sigpool_element_t     (*sigpool)[SD_SIGNALS_MAX]; /*= &sd_sigpool*/
  /* Note the unusual syntax --^
  ** 'sigpool' is a pointer to an array of size SD_SIGNALS_MAX,
  ** where each element is an sd_sigpool_element_t
  */

  /*-------------End debug information section--------------*/

  /* Flag used to check if a task from a lower RC INIT group like RR is
  ** calling SD utility functions during boot up, before SD is ready.
  */
  boolean is_sd_init_complete;                         /*= FALSE*/

} sdtask_info_s_type;

/* Declare and init the info struct (this is the object itself) */

static sdtask_info_s_type sd_task_info = {
  /*-------------Start debug information section--------------*/

  /* Ptr to the task pointer */
  &sdtask_thread_id,                             /* .thread_id */

  /* Ptr to the signal pool */
  &sd_sigpool,                                   /* .sigpool */

  /*-------------End debug information section--------------*/

  /* Flag used to check if a task from a lower RC INIT group like RR is
  ** calling SD utility functions during boot up, before SD is ready.
  */
  FALSE                                          /* .is_sd_init_complete */
};


#if defined(WIN32)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Windows implementation of signal pool functions (for unit tests).

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*----------------------------------------------------------------------------
  sdtask_sigpool_init
    Initialize the signal pool to be used by the SD command dispatch mechanism
----------------------------------------------------------------------------*/
static void        sdtask_sigpool_init(
  void
  )
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(sd_sigpool) ; ++i)
  {
    sd_sigpool[i].signal = CreateEvent(NULL, FALSE, FALSE, NULL);
    sd_sigpool[i].available.value = TRUE;
  }
}

/*----------------------------------------------------------------------------
  sdtask_sigpool_dtor
    Clean up the SD signal pool
----------------------------------------------------------------------------*/
static void        sdtask_sigpool_dtor(
  void
  )
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(sd_sigpool) ; ++i)
  {
    sd_sigpool[i].available.value = FALSE;
    if (sd_sigpool[i].signal == NULL_SIGNAL)
    {
      CloseHandle((HANDLE) sd_sigpool[i].signal);
    }
  }
}

#elif defined(TARGET_BUILD)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  QURT implementation of signal pool functions (for target).

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*----------------------------------------------------------------------------
  sdtask_sigpool_init
    Initialize the signal pool to be used by the SD command dispatch mechanism
----------------------------------------------------------------------------*/
static void        sdtask_sigpool_init(
  void
  )
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(sd_sigpool) ; ++i)
  {
    sd_sigpool[i].signal = &sd_sigpool[i].qurt_signal;
    qurt_anysignal_init(sd_sigpool[i].signal);
    sd_sigpool[i].available.value = TRUE;
  }
}

/*----------------------------------------------------------------------------
  sdtask_sigpool_dtor
    Clean up the SD signal pool
----------------------------------------------------------------------------*/
static void        sdtask_sigpool_dtor(
  void
  )
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(sd_sigpool) ; ++i)
  {
    sd_sigpool[i].available.value = FALSE;
    qurt_anysignal_destroy(sd_sigpool[i].signal);
  }

}

#endif
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  End of target-specific implementation of signal pool functions.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/*----------------------------------------------------------------------------
  sdtask_sigpool_get_sig
    Return an available signal from the signal pool.  It is a fatal error
    to have no available signals.
----------------------------------------------------------------------------*/
static sd_signal_t sdtask_sigpool_get_sig(
  void
  )
{
  size_t      i;
  sd_signal_t signal = NULL_SIGNAL;

  for (i = 0 ; i < ARR_SIZE(sd_sigpool) ; ++i)
  {
    if (atomic_compare_and_set(&sd_sigpool[i].available, TRUE, FALSE))
    {
      signal = (sd_signal_t) sd_sigpool[i].signal;
      break;
    }

  }

  if (signal == NULL_SIGNAL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  return signal;
}

/*----------------------------------------------------------------------------
  sdtask_sigpool_return_sig
    Return a signal to the signal pool, making it available to others again.
----------------------------------------------------------------------------*/
static void        sdtask_sigpool_return_sig(
  sd_signal_t signal
  )
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(sd_sigpool) ; ++i)
  {
    if (sd_sigpool[i].signal == (sigpool_signal_t) signal)
    {
      sd_sigpool[i].available.value = TRUE;
      break;
    }
  }
}


/*****************************************************************************
******************************************************************************

  Command functions

******************************************************************************
*****************************************************************************/

/*----------------------------------------------------------------------------
  sdtask_sd_cmd_dtor
    Called when the reference count of the command goes to zero.  This calls
    the specific command's destructor, then cleans up all other resources
    allocated to the command before freeing its memory.
----------------------------------------------------------------------------*/
static void      sdtask_sd_cmd_dtor(
  void  *pobj
  )
{
  sd_cmd_t  *me = (sd_cmd_t *) pobj;

  /* First call the destructor for the derived command.
  */
  if (me->dtor != NULL)
  {
    me->dtor(me);
  }

  /* Return the signal to the signal pool
  */
  sdtask_sigpool_return_sig(me->signal);

  /* Free the command memory.
  */
  modem_mem_free(me, MODEM_MEM_CLIENT_MMODE);
}


/*----------------------------------------------------------------------------
  sdtask_sd_cmd_new
    Allocate a new command structure of the size passed in, then initialize
    the base portion of the command (the SD_CMD_HDR portion).  Note that this
    may fail with a fatal error if there are no available signals in the
    SD signal pool.
----------------------------------------------------------------------------*/
sd_cmd_t  *sdtask_sd_cmd_new(
  size_t            size,
  execute_fn        execute,
  sd_ss_act_s_type  *act_ptr,
  dtor_fn           dtor
  )
{
  /* Allocate the command
  */
  sd_cmd_t  *cmd = modem_mem_alloc(size, MODEM_MEM_CLIENT_MMODE);
  if (cmd == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  /* intialize the memory allocated
  */
  memset(cmd, 0, sizeof(sd_cmd_t));

  /* Intialize reference counting on the command
  */
  ref_cnt_obj_init(cmd, sdtask_sd_cmd_dtor);

  /* Fill in the command header
  */
  cmd->execute = execute;
  cmd->ss = SD_SS_MAIN;
  cmd->dtor = dtor;
  cmd->cb = NULL;
  cmd->context = NULL;
  cmd->act = SD_SS_ACT_MAX;
  cmd->rtrn_act_ptr = act_ptr;

  return cmd;
}


/*----------------------------------------------------------------------------
  sdtask_sd_cmd_wait
    Wait for the command in question to complete.
----------------------------------------------------------------------------*/
static void      sdtask_sd_cmd_wait(
  sd_cmd_t *me
  )
{
  if (me->signal != NULL_SIGNAL)
  {
    sdtask_signal_wait(me->signal);
  }
}

/*----------------------------------------------------------------------------
  sdtask_sd_cmd_complete
    The command in question has completed execution.  Unblock any thread
    waiting for it to complete.
----------------------------------------------------------------------------*/
static void      sdtask_sd_cmd_complete(
  sd_cmd_t *me
  )
{
  if (me->signal != NULL_SIGNAL)
  {
    sdtask_signal_set(me->signal);
  }

  if (me->cb != NULL)
  {
    me->cb(me->context);
  }

  ref_cnt_obj_release(me);
}
/*****************************************************************************
******************************************************************************

  MCFG refresh related 

******************************************************************************
*****************************************************************************/
#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*=========================================================================== 
 
FUNCTION sdtask_mcfg_refresh_cb

DESCRIPTION
  This is a callback function used by MCFG for NV/EFS refresh purpose
  This function puts the received refresh info into SD command queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean sdtask_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info)
{
  sd_ss_mcfg_refresh_cmd *cmd;
  SD_MSG_HIGH_4("MCFGrefresh: type %d slot %d sub %d, ref_id %d",
                           p_info->type, p_info->slot_index,
  	                   p_info->sub_index, p_info->reference_id);

   if(p_info->type != MCFG_REFRESH_TYPE_SUBS )
   {
     mcfg_refresh_done(p_info->reference_id);
     return FALSE;
   }

    cmd = (sd_ss_mcfg_refresh_cmd *) sdtask_sd_cmd_new(
                                                sizeof(sd_ss_mcfg_refresh_cmd),
                                                (execute_fn) sdcmd_mcfg_refresh,
                                                NULL,
                                                NULL
                                                );

    cmd->as_id  = (sys_modem_as_id_e_type)p_info->sub_index;
    cmd->reference_id = p_info->reference_id;

    #ifdef SD_DEBUG
    #error code not present
#else
    (void)sdtask_queue_put_cmd_no_wait((sd_cmd_t *)cmd, NULL, NULL);
    #endif
     ref_cnt_obj_release(cmd);   

    return TRUE;

}

/*===========================================================================
FUNCTION SDTASK_MCFG_REFRESH_REGISTER

DESCRIPTION
This function call registers SD Task with MCFG for NV/EFS refresh purpose
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void sdtask_mcfg_refresh_register(void)
{
  mcfg_refresh_registration_s_type mcfg_reg;

  memset((void *)&mcfg_reg,0x00,sizeof(mcfg_refresh_registration_s_type));

  mcfg_reg.order = MCFG_REFRESH_ORDER_50;  
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  mcfg_reg.sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_reg.group = 0;
  mcfg_reg.cb = sdtask_mcfg_refresh_cb;

  if(mcfg_refresh_register(&mcfg_reg) != TRUE)
  {
     SD_ERR_0("MCFG Registration failed"); 
  }
}

#endif //FEATURE_MODEM_CONFIG_REFRESH
/*****************************************************************************
******************************************************************************

  Command queue

******************************************************************************
*****************************************************************************/

static q_type         sd_cmd_q;
static sd_signal_t    sd_cmd_signal;
static sd_signalset_t sdtask_waitsigs;

/*----------------------------------------------------------------------------
  sdtask_queue_init
    Initialize the SD command queue.
----------------------------------------------------------------------------*/
static void sdtask_queue_init(
  void
  )
{
  (void) q_init(&sd_cmd_q);
}

/*----------------------------------------------------------------------------
  sdtask_queue_put_cmd_wait
    Add the command passed in to the command queue and set the command queue's
    signal, unblocking the dispatch thread if it is waiting for a command to
    become available.

    This call blocks the calling thread until command execution is complete.
----------------------------------------------------------------------------*/
sd_ss_act_e_type sdtask_queue_put_cmd_wait(
  sd_cmd_t  *cmd
  )
{
  /* If we try to call a front-end API from the SD task, we'll deadlock.
  ** Crash the phone in this case so we can look at the callstack to find
  ** the culprit.
  */
  if (sdtask_get_current_thread_id() == sdtask_thread_id)
  {
    SD_ERR_FATAL_0("Front-end API called on the task thread.  Look at SD task callstack to find culprit.");
  }

  /* Get a signal for the command.
  */
  cmd->signal = sdtask_sigpool_get_sig();
  cmd->tcb_ptr = rex_self();

  /* Add the command to the queue by calling the asynchronous put function,
  ** then wait for it to complete.
  */
  sdtask_queue_put_cmd_no_wait(cmd, NULL, NULL);
  sdtask_sd_cmd_wait(cmd);

  return cmd->act;
}

/*----------------------------------------------------------------------------
  sdtask_queue_put_cmd_no_wait
    Add the command passed in to the command queue and set the command queue's
    signal, unblocking the dispatch thread if it is waiting for a command to
    become available.

    This call returns immediately and does not block the calling thread.
----------------------------------------------------------------------------*/
void sdtask_queue_put_cmd_no_wait(
  sd_cmd_t  *cmd,
  cmd_cb    cb,
  void      *context
  )
{
  cmd->cb = cb;
  cmd->context = context;

  ref_cnt_obj_add_ref(cmd);

  q_put(&sd_cmd_q, &cmd->link);

  sdtask_signalset_set(&sdtask_waitsigs, sd_cmd_signal);
}

/*----------------------------------------------------------------------------
  sdtask_queue_get_cmd
    Get a command from the command queue, or block on the command queue's
    signal if the queue is empty.
----------------------------------------------------------------------------*/
static sd_cmd_t *sdtask_queue_get_cmd(
  void
  )
{
  void      *qlink = NULL;
  sd_cmd_t  *cmd;

  qlink = q_get(&sd_cmd_q);

  if (qlink == NULL)
  {
    cmd = NULL;
  }
  else
  {
    cmd = (sd_cmd_t *) (((byte *) qlink) - offsetof(sd_cmd_t, link));
  }

  return cmd;
}

/*----------------------------------------------------------------------------
  sdtask_queue_dtor
    Destroy the command queue.  This is only used in a unit-test environment,
    as on target the task runs forever.
----------------------------------------------------------------------------*/
static void    sdtask_queue_dtor(
  void
  )
{
  if (sd_cmd_signal != NULL_SIGNAL)
  {
    sd_signal_t sig = sd_cmd_signal;
    sd_cmd_signal = NULL_SIGNAL;
    sdtask_signalset_set(&sdtask_waitsigs, sig);
  }
}


/*****************************************************************************
******************************************************************************

  SD task functions

******************************************************************************
*****************************************************************************/

static sd_signal_t  sd_dog_signal;


#if defined(TARGET_BUILD)

static dog_report_type  sd_dog_rpt = 0;

#endif

static void sdtask_pet_dog( void )
{
#if defined(TARGET_BUILD)

   dog_hb_report(sd_dog_rpt);

#endif
}

/*----------------------------------------------------------------------------
  sd_is_init_complete
    Returns whether SD task has completed initialization.
----------------------------------------------------------------------------*/
boolean sdtask_is_init_complete(
  void
  )
{
  return sd_task_info.is_sd_init_complete;
}

/*----------------------------------------------------------------------------
  sdtask_init_before_start
    Initialize the SD command queue and signals
----------------------------------------------------------------------------*/
void sdtask_init_before_start(
  void
  )
{

  sdtask_sigpool_init();

#if defined(WIN32)
  sd_cmd_signal = (sd_signal_t) CreateEvent(NULL, FALSE, FALSE, NULL);
  sd_dog_signal = (sd_signal_t) CreateEvent(NULL, FALSE, FALSE, NULL);
  sd_stop_signal = (sd_signal_t) CreateEvent(NULL, FALSE, FALSE, NULL);

  /*---------------------------------------------------------------------------
    This is the signal that will be used to synchronize SUTE with the SD task
    thread.
  ---------------------------------------------------------------------------*/
  sd_start_signal = CreateEvent(NULL, FALSE, FALSE, NULL);

#elif defined(TARGET_BUILD)
  sd_cmd_signal = (sd_signal_t) 0x00000001;
  sd_dog_signal = (sd_signal_t) 0x00000002;
  sd_stop_signal = (sd_signal_t) 0x00000004;
#endif

  sdtask_queue_init();
}


/*----------------------------------------------------------------------------
  sdtask_init_after_start
    Initialize after SD task is started.
----------------------------------------------------------------------------*/
void  sdtask_init_after_start(void)
{

#if defined(TARGET_BUILD)

   sdtask_signalset_set(&sdtask_waitsigs, sd_dog_signal);

  sd_tcb_ptr = rex_self();

  /* Register with watchdog HB API */
  sd_dog_rpt = dog_hb_register_qurt( &sdtask_waitsigs.signal, sd_dog_signal );

  sdtask_pet_dog();

#endif

  sdss_read_policyman_config();

}


static void sdtask_process_cmds(
  void
  )
{
  sd_cmd_t    *cmd;

  while ((cmd = sdtask_queue_get_cmd()) != NULL)
  {
    cmd->act = cmd->execute(cmd);
    sdtask_sd_cmd_complete(cmd);
  }
}

static void sdtask_rcinit(void)
{
#if defined(WIN32)

  SetEvent(sd_start_signal);

#elif defined(TARGET_BUILD) && defined(FEATURE_MODEM_RCINIT)

/*-----------------------------------------------------------------------------
  None of the SD clients register for the RCINIT termination signal, so we
  also do not register.  SD needs to stay running until power-down.
-----------------------------------------------------------------------------*/
#if 0
  RCEVT_SIGEX_SIGQURT sdtask_term_sig;
  RCINIT_GROUP        sdtask_group = RCINIT_GROUP_NONE;

  sdtask_term_sig.signal = &sdtask_waitsigs.signal;;
  sdtask_term_sig.mask = sd_stop_signal;

  /* Lookup for sequence group */
  sdtask_group = rcinit_lookup_group_rextask(rex_self());

  /* Registering for TASK_STOP_SIG with rcinit */
  if(sdtask_group != RCINIT_GROUP_NONE )
  {
    rcinit_register_term_group(sdtask_group,
                             RCEVT_SIGEX_TYPE_SIGQURT,
                             &sdtask_term_sig);
  }
  else
  {
    SD_ERR_0( "Invalid Sequence group rcinit_lookup_group_rextask failed ");
  }
#endif

  rcinit_handshake_startup();
  (void)rcevt_signal_name(SD_TASK_RCEVT_READY);

#endif
}

/*----------------------------------------------------------------------------
  sdtask_process_stop
    This function handles TASK_STOP_SIG received by the sdtask
----------------------------------------------------------------------------*/

static void sdtask_process_stop(void)
{

#if defined(WIN32)

  CloseHandle(sd_start_signal);

#elif 0
/*-----------------------------------------------------------------------------
  None of the SD clients register for the RCINIT termination signal, so we
  also do not register.  SD needs to stay running until power-down.  The
  termination code here is commented out because it will never be hit.
-----------------------------------------------------------------------------*/
/* #elif defined(TARGET_BUILD) && defined(FEATURE_MODEM_RCINIT) */

  RCEVT_SIGEX_SIGQURT term_sig;
  RCINIT_GROUP        sdtask_group = RCINIT_GROUP_NONE;

  SD_MSG_MED_0("SDTask received stop sig ");
  term_sig.signal = &sdtask_waitsigs.signal;
  term_sig.mask = sd_stop_signal;

  /* Lookup for sequence group */
  sdtask_group = rcinit_lookup_group_rextask(rex_self());
  rcinit_unregister_term_group(sdtask_group, RCEVT_SIGEX_TYPE_SIGQURT, &term_sig);

  rcinit_handshake_term();

#endif

}


/*----------------------------------------------------------------------------
  sdtask_exit
    Exit the SD task.  Only called in a unit-test environment - the task
    runs forever on target.
----------------------------------------------------------------------------*/
void  sdtask_exit(void)
{
#if defined(WIN32)
  SetEvent((HANDLE) sd_stop_signal);
#endif
}


#if defined(WIN32)

/*-----------------------------------------------------------------------------
  sdtask_wait_for_task_start
-----------------------------------------------------------------------------*/
void  sdtask_wait_for_task_start(void)
{
  WaitForSingleObject(sd_start_signal, INFINITE);
}

#endif


/*----------------------------------------------------------------------------
  sd_task
    The dispatch loop of the SD task.
----------------------------------------------------------------------------*/
void  sd_task(void)
{
  sd_signalset_t  fired_sigs;

  SD_MSG_HIGH_0("SD task starting.");

  memset(&fired_sigs, 0, sizeof(sd_signalset_t));

  sdtask_signalset_init(&sdtask_waitsigs);
  sdtask_signalset_add(&sdtask_waitsigs, sd_cmd_signal);
  sdtask_signalset_add(&sdtask_waitsigs, sd_stop_signal);;
  sdtask_signalset_add(&sdtask_waitsigs, sd_dog_signal);

  /* Get the task's thread id so we can ERR_FATAL if a front-end API
  ** is called from that thread (which will deadlock the SD task and
  ** whatever task called the API that led to SD calling a front-end
  ** API from the task thread).
  */
  sdtask_thread_id = sdtask_get_current_thread_id();

  sdtask_rcinit();

  sdtask_init_after_start();

  /* Mark that we've completed init, so we can return valid values for
  ** utility functions called by other modules.
  */
  sd_task_info.is_sd_init_complete = TRUE;

  while (TRUE)
  {
    sdtask_signalset_init(&fired_sigs);
    sdtask_signalset_wait(&sdtask_waitsigs, &fired_sigs);

    if (sdtask_signalset_contains(&fired_sigs, sd_dog_signal))
    {
      sdtask_pet_dog();
    }

    if (sdtask_signalset_contains(&fired_sigs, sd_cmd_signal))
    {
      sdtask_process_cmds();
    }

    if (sdtask_signalset_contains(&fired_sigs, sd_stop_signal))
    {
      break;
    }
  }

  sdtask_signalset_dtor(&sdtask_waitsigs);
  sdtask_queue_dtor();
  sdtask_sigpool_dtor();

  sdtask_process_stop();
}

