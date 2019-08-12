/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          D C C _ T A S K _ S V C . C

DESCRIPTION
  Data Common Control Task's services provided to its modules. Contained
  herein are the functions needed for signal notification to DCC task,
  accessing NV items and sending commands to be processed in DCC task context


EXTERNALIZED FUNCTIONS
  dcc_enable_sig()
    Adds specific signal for DCC task to process.

  dcc_disable_sig()
    Removes specific signal from DCC task processing.

  dcc_set_sig_handler()
    Sets signal handler for specific signal processed in DCC task.

  dcc_get_cmd_data_buf()
    Allocs a new command buffer to be used by caller.

  dcc_free_cmd_data_buf()
    Frees the DCC command buffer.

  dcc_set_cmd_handler()
    Sets command handler for command handled in DCC task.

  dcc_wait()
    Make DCC task wait for requested signals.

  dcc_get_nv_item()
    Retrieve a NV item

  dcc_put_nv_item()
    Write an item to the NV

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the DCC services can be used unless MC has created the DCC task
  and dcc_task() has been invoked.

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/dcc_task_svc.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/02/13    pkp    Dog heart beat implementation.
11/13/12    wc     Fix incorrect memory free in dcc_send_cmd()
03/15/12    sa     Fix to avoid memory leak in case of command buffer
                   exhaustion.
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/15/10    ss     Added new APIs to access NV items from multiple SIMs.
09/27/10    ss     DYNAMIC WDOG support for DATA tasks.
09/24/10    pp     API dcc_send_cmd_ex introduced.
11/05/09    ar     QTF support changes.
02/19/09    am     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "nv.h"
#include "msg.h"
#include "err.h"
#include "dog_hb_rex.h"
#include "rex.h"

#include "ps_mem.h"
#include "dcc_task_defs.h"
#include "dcc_taski.h"
#include "dcc_task_svc.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include "rcinit_rex.h"

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

nv_item_type         dcc_nv_item;
nv_stat_enum_type    dcc_nv_status;

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Command item to NV.
---------------------------------------------------------------------------*/
static nv_cmd_type dcc_nv_cmd_buf;

/*===========================================================================

                  EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================

FUNCTION DCC_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the DCC task uses for rex_wait().

DEPENDENCIES
  A valid signal handler should be registered with DCC before enabling the
  signal.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
dcc_enable_sig
(
  dcc_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_dcc_crit_section);

  if ( dcci_sig_handler[sig].sig_handler == dcc_default_sig_handler )
  {
    LOG_MSG_ERROR_1("dcc_enable_sig(): "
                    "Signal %d enabled without a handler", sig );
  }

#ifdef FEATURE_REX_SIGS_EXT
  DCC_SET_SIG_MASK(dcci_enabled_sig_mask, sig);
#else
  dcci_enabled_sig_mask |= ( (rex_sigs_type) 1 << (rex_sigs_type) sig );
#endif /* FEATURE_REX_SIGS_EXT */

  PS_LEAVE_CRIT_SECTION(&global_dcc_crit_section);

  DCC_SET_SIGNAL( DCC_SIG_MASK_CHANGE_SIGNAL );
} /* dcc_enable_sig() */

/*===========================================================================

FUNCTION DCC_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
dcc_disable_sig
(
  dcc_sig_enum_type sig                       /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_dcc_crit_section);

#ifdef FEATURE_REX_SIGS_EXT
  DCC_CLR_SIG_MASK(dcci_enabled_sig_mask, sig);
#else
  dcci_enabled_sig_mask &= ~( (rex_sigs_type) 1 << (rex_sigs_type) sig );
#endif /*FEATURE_REX_SIGS_EXT */

  PS_LEAVE_CRIT_SECTION(&global_dcc_crit_section);

} /* dcc_disable_sig() */


/*===========================================================================

FUNCTION DCC_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal.

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

dcc_sig_handler_type 
dcc_set_sig_handler
(
  dcc_sig_enum_type sig,             /* Signal assocaited with handler     */
  dcc_sig_handler_type sig_handler,  /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  dcc_sig_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_dcc_crit_section);

  old_handler = dcci_sig_handler[sig].sig_handler;

  dcci_sig_handler[sig].sig_handler = ( sig_handler != NULL) ?
                                        sig_handler :
                                        dcc_default_sig_handler;

  dcci_sig_handler[sig].user_data_ptr = user_data_ptr;

  PS_LEAVE_CRIT_SECTION(&global_dcc_crit_section);

  return old_handler;
} /* dcc_set_sig_handler() */

/*===========================================================================

FUNCTION DCC_GET_CMD_DATA_BUF()

DESCRIPTION
  Allocates a new command buffer to be used by caller to post commands to
  DCC task.
  
DEPENDENCIES
  None.

RETURN VALUE
  Ptr to new command data buffer.

SIDE EFFECTS
  None.
===========================================================================*/

dcc_cmd_data_buf_type *
dcc_get_cmd_data_buf
(
  void
)
{
  dcc_cmd_data_buf_type *new_cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  new_cmd_ptr = (dcc_cmd_data_buf_type *)
                ps_mem_get_buf(PS_MEM_DCC_CMD_DATA_BUF_TYPE);
  if (NULL == new_cmd_ptr)
  {
    LOG_MSG_ERROR_0("dcc_get_cmd_data_buf(): "
                    "DCC cmd data mem alloc error");
  }

  return new_cmd_ptr;

} /* dcc_get_cmd_data_buf */

/*===========================================================================

FUNCTION DCC_FREE_CMD_DATA_BUF()

DESCRIPTION
  Frees DCC task command buffer.
  
DEPENDENCIES
  Command buffer should have been allocated by a prior call to
  dcc_get_cmd_data_buf().

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void
dcc_free_cmd_data_buf
(
  void *cmd_ptr                  /* Command buffer to be freed             */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_MEM_FREE(cmd_ptr);

} /* dcc_get_cmd_data_buf */

/*===========================================================================

FUNCTION DCC_SEND_CMD_EX()

DESCRIPTION
  This function posts a cmd for processing in DCC task context.  The cmd is
  processed by calling the registered cmd handler, if any.

  This API is mainly for clients which allocates  user_data_ptr from their
  own memory [not by dcc_get_cmd_buf]. Henceforth, if the client uses this
  API, it must free the memory after processing the command.

  Eventually all clients must move to this configuration.

  NOTE: The passed command will be copied to a DCC task
  command buffer local to the DCC Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
dcc_send_cmd_ex
(
  dcc_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
  dcci_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links
    available, else copy buffer to cmd queue and signal DCC task
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ( dcci_cmd_type * ) q_get( &dcci_cmd_free_q ) ) == NULL )
  {
    PS_SYSTEM_HEAP_MEM_FREE(user_data_ptr);
    DATA_ERR_FATAL("dcc_send_cmd_ex(): DCC task cmd buffers unavailable" );
  }
  else
  {
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;
    cmd_ptr->user_allocated = TRUE;

    /*-----------------------------------------------------------------------
      Making the operation atomic so that race conditions do not occur
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION(&global_dcc_crit_section);
    q_put( &dcci_cmd_q, &cmd_ptr->link );
    DCC_SET_SIGNAL( DCC_CMD_Q_SIGNAL );
    PS_LEAVE_CRIT_SECTION(&global_dcc_crit_section);
	

  }
} /* dcc_send_cmd_ex() */

/*===========================================================================

FUNCTION DCC_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  The cmd handling
  can be disabled by setting a NULL handler

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/

dcc_cmd_handler_type 
dcc_set_cmd_handler
(
  dcc_cmd_enum_type cmd,              /* Command name                      */
  dcc_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
)
{
  dcc_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_dcc_crit_section);

  old_handler = dcci_cmd_handler[cmd];

  dcci_cmd_handler[cmd] = (cmd_handler != NULL) ?
                            cmd_handler : dcc_default_cmd_handler;

  PS_LEAVE_CRIT_SECTION(&global_dcc_crit_section);

  return old_handler;
} /* dcc_set_cmd_handler() */

#ifdef FEATURE_REX_SIGS_EXT
/*===========================================================================

FUNCTION DCCI_WAIT()

DESCRIPTION
  This function performs the DCC Task waiting function. This function will
  not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

===========================================================================*/

rex_sigs_type *
dcc_wait
(
  rex_sigs_type *requested_mask             /* mask of signals to wait for */
)
{
  rex_sigs_type set_sigs[DCC_SIG_ARR_LEN];        /* Currently set signals */
  boolean       sig_result =  FALSE;
  uint8         i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    First check to see if one of the signals in the mask is already set.  If
    yes, then clear those signals and return. Otherwise enter into the wait
    only if no requested signal events have occurred
  -------------------------------------------------------------------------*/
  for(i = 0; i < DCC_SIG_ARR_LEN; i++)
  {
    set_sigs[i] = 0;
  }

  sig_result = rex_get_sigs_ext(&dcc_tcb, set_sigs);

  if(sig_result == FALSE)
  {
    LOG_MSG_ERROR("dcc_wait(): Failed to obtain the signals from the DCC TCB."
              , 0,0,0);
    ASSERT(0);
    return NULL;
  }

  while(TRUE)
  {
    sig_result = FALSE;
    for(i = 0; i < DCC_SIG_ARR_LEN; i++)
    {
      if(set_sigs[i] & requested_mask[i])
      {
        sig_result = TRUE;
        break;
      }
    }

    if( FALSE == sig_result )
    {
      DCC_SET_SIG_MASK(requested_mask, DCC_DOG_HB_REPORT_SIGNAL);
      sig_result = rex_wait_ext(requested_mask, set_sigs);
      if( FALSE == sig_result )
      {
        LOG_MSG_ERROR_0("dcc_wait():rex_wait_ext() failed.");
        ASSERT(0);
        return NULL;
      }

      if(DCC_IS_SIG_MASK_SET(set_sigs, DCC_DOG_HB_REPORT_SIGNAL))
      {
        DCC_CLR_SIGNAL(DCC_DOG_HB_REPORT_SIGNAL);
        dog_hb_report( dcc_dog_rpt_id ); /* Play ping<->pong */
        DCC_CLR_SIG_MASK(requested_mask, DCC_DOG_HB_REPORT_SIGNAL);
      }
    }
    else
    {
      break;
    }
  }

  for(i = 0; i < DCC_SIG_ARR_LEN; i++)
  {
    requested_mask[i] &= set_sigs[i];
  }

  sig_result = rex_clr_sigs_ext(&dcc_tcb, requested_mask);
  if(sig_result == FALSE)
  {
    LOG_MSG_ERROR_0("dcc_wait():rex_clr_sigs_ext() failed.");
    ASSERT(0);
    return NULL;
  }

  return requested_mask;

} /* dcc_wait() */

#else
/*===========================================================================

FUNCTION DCCI_WAIT()

DESCRIPTION
  This function performs the DCC Task waiting function. This function will
  not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

===========================================================================*/

rex_sigs_type
dcc_wait
(
  rex_sigs_type requested_mask             /* mask of signals to wait for */
)
{
  rex_sigs_type set_sigs;                         /* Currently set signals */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    First check to see if one of the signals in the mask is already set.  If
    yes, then clear those signals and return. Otherwise enter into the wait
    only if no requested signal events have occurred
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2  
  set_sigs = rex_get_sigs( rex_dcc_tcb );
#else
  set_sigs = rex_get_sigs( &dcc_tcb );
#endif

  while ( (set_sigs & requested_mask) == 0 )
  {
    set_sigs = rex_wait( requested_mask |
                         (  1 << (rex_sigs_type)DCC_DOG_HB_REPORT_SIGNAL ) );

    /*-----------------------------------------------------------------------
      If dog heart beat is received, ack the beat. Play ping<->pong.
    -----------------------------------------------------------------------*/
    if( set_sigs & ( 1 << (rex_sigs_type)DCC_DOG_HB_REPORT_SIGNAL ) )
    {
#ifdef FEATURE_MODEM_RCINIT_PHASE2  
      (void)rex_clr_sigs( rex_dcc_tcb,
                    (  1 << (rex_sigs_type)DCC_DOG_HB_REPORT_SIGNAL ) );
#else
      (void)rex_clr_sigs( &dcc_tcb,
                    (  1 << (rex_sigs_type)DCC_DOG_HB_REPORT_SIGNAL ) );
#endif
      dog_hb_report( dcc_dog_rpt_id );
    }
  }

#ifdef FEATURE_MODEM_RCINIT_PHASE2  
  (void)rex_clr_sigs( rex_dcc_tcb, ( set_sigs & requested_mask ) );
#else
  (void)rex_clr_sigs( &dcc_tcb, ( set_sigs & requested_mask ) );
#endif

  return ( set_sigs & requested_mask );
} /* dcci_wait() */

#endif /* FEATURE_REX_SIGS_EXT */

/*===========================================================================

FUNCTION DCC_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other DCC task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/

nv_stat_enum_type 
dcc_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
)
{
#if defined( FEATURE_REX_SIGS_EXT ) && !defined( TEST_FRAMEWORK )
  rex_sigs_type req_mask[DCC_SIG_ARR_LEN];
#endif /* FEATURE_REX_SIGS_EXT */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef TEST_FRAMEWORK
#error code not present
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2    
    if(rex_self() != rex_dcc_tcb)
  #else
    if(rex_self() != &dcc_tcb)
  #endif
    {
    DATA_ERR_FATAL("dcc_get_nv_item() not called in dcc task context");
      return NV_BADTG_S;
    }

#ifdef FEATURE_REX_SIGS_EXT
  memset(req_mask, 0, DCC_SIG_ARR_LEN);
#endif /* FEATURE_REX_SIGS_EXT */

  /*-------------------------------------------------------------------------
    Prepare command buffer to get the item from NV.
  -------------------------------------------------------------------------*/
  dcc_nv_cmd_buf.cmd        = NV_READ_F;             /* Read request        */
#ifdef FEATURE_MODEM_RCINIT_PHASE2  
  dcc_nv_cmd_buf.tcb_ptr    = rex_dcc_tcb;           /* Notify this task    */
#else
  dcc_nv_cmd_buf.tcb_ptr    = &dcc_tcb;              /* Notify this task    */
#endif
  dcc_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)DCC_NV_CMD_SIGNAL; /* With this signal    */
  dcc_nv_cmd_buf.done_q_ptr = NULL;             /* Do not enqueue when done */
  dcc_nv_cmd_buf.item       = item_code;             /* Item to get         */
  dcc_nv_cmd_buf.data_ptr   = data_ptr;              /* Where to return it  */

  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  DCC_CLR_SIGNAL( DCC_NV_CMD_SIGNAL );             /* Clear signal for NV  */
  nv_cmd( &dcc_nv_cmd_buf );                       /* Issue the request    */
#ifdef FEATURE_REX_SIGS_EXT
  DCC_SET_SIG_MASK(req_mask, DCC_NV_CMD_SIGNAL);
  (void)dcc_wait(req_mask); /* Wait for completion*/
#else
  (void)dcc_wait((rex_sigs_type) 1 <<
                       (rex_sigs_type)DCC_NV_CMD_SIGNAL );/* Wait for completion*/
#endif /* FEATURE_REX_SIGS_EXT */

  if( dcc_nv_cmd_buf.status != NV_DONE_S &&
      dcc_nv_cmd_buf.status != NV_NOTACTIVE_S )
  {
    LOG_MSG_ERROR_2( "NV Read Failed Item %d Code %d",
                     dcc_nv_cmd_buf.item, dcc_nv_cmd_buf.status );
  }
  return( dcc_nv_cmd_buf.status );

#endif /* TEST_FRAMEWORK */
} /* dcc_get_nv_item() */

/*===========================================================================
FUNCTION DCC_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.

DEPENDENCIES
  This function can only be called from DCC task.  Also it is not
  reentrant. Calls rex_wait() and doesn't exit until processing is finished.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other DCC task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.

===========================================================================*/

nv_stat_enum_type 
dcc_put_nv_item(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
)
{
#ifdef FEATURE_REX_SIGS_EXT
  rex_sigs_type req_mask[DCC_SIG_ARR_LEN];
#endif /* FEATURE_REX_SIGS_EXT */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef TEST_FRAMEWORK
  #error code not present
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    if(rex_self() != rex_dcc_tcb)
  #else
    if(rex_self() != &dcc_tcb)
  #endif
    {
      DATA_ERR_FATAL("dcc_put_nv_item() not called in dcc task context");
      return NV_BADTG_S;
    }

#ifdef FEATURE_REX_SIGS_EXT
  memset(req_mask, 0, DCC_SIG_ARR_LEN);
#endif /* FEATURE_REX_SIGS_EXT */

  /*-------------------------------------------------------------------------
    Prepare command buffer to write the item to NV.
    TODO:NV team to be notified to change the nv_cmd_buf.sigs to an array
  -------------------------------------------------------------------------*/
  dcc_nv_cmd_buf.cmd        = NV_WRITE_F;           /* Write request       */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  dcc_nv_cmd_buf.tcb_ptr    = rex_dcc_tcb;          /* Notify this task    */
#else
  dcc_nv_cmd_buf.tcb_ptr    = &dcc_tcb;              /* Notify this task    */
#endif
  dcc_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)DCC_NV_CMD_SIGNAL; /* With this signal    */
  dcc_nv_cmd_buf.done_q_ptr = NULL;            /* Do not enqueue when done */
  dcc_nv_cmd_buf.item       = item_code;            /* Item to put         */
  dcc_nv_cmd_buf.data_ptr   = data_ptr;             /* Data to write       */


  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  DCC_CLR_SIGNAL( DCC_NV_CMD_SIGNAL );
  nv_cmd( &dcc_nv_cmd_buf );
#ifdef FEATURE_REX_SIGS_EXT
  DCC_SET_SIG_MASK(req_mask, DCC_NV_CMD_SIGNAL);
  (void)dcc_wait(req_mask); /* Wait for completion*/
#else
  (void)dcc_wait( (rex_sigs_type) 1 <<
                 (rex_sigs_type)DCC_NV_CMD_SIGNAL ); /* Wait for completion*/
#endif /* FEATURE_REX_SIGS_EXT */

  if( dcc_nv_cmd_buf.status != NV_DONE_S )
  {
    LOG_MSG_ERROR_2( "NV Write Failed Item %d Code %d",
                     dcc_nv_cmd_buf.item, dcc_nv_cmd_buf.status );
  }

  return( dcc_nv_cmd_buf.status );

#endif /* TEST_FRAMEWORK */
} /* dcc_put_nv_item() */

/*===========================================================================

FUNCTION DCC_GET_NV_ITEM_EXT

DESCRIPTION
  This function retrieves the specified item from NV for the specified SIM.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other DCC task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/

nv_stat_enum_type 
dcc_get_nv_item_ext
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type       *data_ptr,        /* Pointer where to put the item    */
  uint16              context          /* Context specifying the SIM       */
)
{
#if defined( FEATURE_REX_SIGS_EXT ) && !defined( TEST_FRAMEWORK )
  rex_sigs_type req_mask[DCC_SIG_ARR_LEN];
#endif /* FEATURE_REX_SIGS_EXT */
  nv_cmd_ext_type dcc_nv_cmd_ext_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    To avoid lint warning.
  -------------------------------------------------------------------------*/
  memset(&dcc_nv_cmd_ext_buf, 0, sizeof(nv_cmd_ext_type));
  (void)dcc_nv_cmd_ext_buf;

#ifdef TEST_FRAMEWORK
  #error code not present
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    if(rex_self() != rex_dcc_tcb)
  #else
    if(rex_self() != &dcc_tcb)
  #endif
    {
    DATA_ERR_FATAL("dcc_get_nv_item_ext() not called in dcc task context");
      return NV_BADTG_S;
    }

#ifdef FEATURE_REX_SIGS_EXT
  memset(req_mask, 0, DCC_SIG_ARR_LEN);
#endif /* FEATURE_REX_SIGS_EXT */

  /*-------------------------------------------------------------------------
    Prepare command buffer to get the item from NV.
  -------------------------------------------------------------------------*/
  dcc_nv_cmd_buf.cmd        = NV_READ_F;             /* Read request        */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  dcc_nv_cmd_buf.tcb_ptr    = rex_dcc_tcb;           /* Notify this task    */
#else
  dcc_nv_cmd_buf.tcb_ptr    = &dcc_tcb;              /* Notify this task    */
#endif
  dcc_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)DCC_NV_CMD_SIGNAL; /* With this signal    */
  dcc_nv_cmd_buf.done_q_ptr = NULL;             /* Do not enqueue when done */
  dcc_nv_cmd_buf.item       = item_code;             /* Item to get         */
  dcc_nv_cmd_buf.data_ptr   = data_ptr;              /* Where to return it  */

  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  DCC_CLR_SIGNAL( DCC_NV_CMD_SIGNAL );             /* Clear signal for NV  */
  
  dcc_nv_cmd_ext_buf.nvcmd   = &dcc_nv_cmd_buf;
  dcc_nv_cmd_ext_buf.context = context;  /* Populate NV request parameters */
  
  nv_cmd_ext( &dcc_nv_cmd_ext_buf );               /* Issue the request    */
#ifdef FEATURE_REX_SIGS_EXT
  DCC_SET_SIG_MASK(req_mask, DCC_NV_CMD_SIGNAL);
  (void)dcc_wait(req_mask); /* Wait for completion*/
#else
  (void)dcc_wait((rex_sigs_type) 1 <<
                       (rex_sigs_type)DCC_NV_CMD_SIGNAL );/* Wait for completion*/
#endif /* FEATURE_REX_SIGS_EXT */

  if( dcc_nv_cmd_buf.status != NV_DONE_S &&
      dcc_nv_cmd_buf.status != NV_NOTACTIVE_S )
  {
    LOG_MSG_ERROR_2( "NV Read Failed Item %d Code %d",
                     dcc_nv_cmd_buf.item, dcc_nv_cmd_buf.status );
  }
  return( dcc_nv_cmd_buf.status );

#endif /* TEST_FRAMEWORK */
} /* dcc_get_nv_item() */

/*===========================================================================
FUNCTION DCC_PUT_NV_ITEM_EXT

DESCRIPTION
  Write an item to NV memory of the specified SIM.  Wait until write is 
  completed.

DEPENDENCIES
  This function can only be called from DCC task.  Also it is not
  reentrant. Calls rex_wait() and doesn't exit until processing is finished.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other DCC task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.

===========================================================================*/

nv_stat_enum_type 
dcc_put_nv_item_ext
(
  nv_items_enum_type item_code,              /* which item                 */
  nv_item_type      *data_ptr,               /* pointer to data for item   */
  uint16             context                 /* Context specifying the SIM */
)
{
#ifdef FEATURE_REX_SIGS_EXT
  rex_sigs_type req_mask[DCC_SIG_ARR_LEN];
#endif /* FEATURE_REX_SIGS_EXT */
  nv_cmd_ext_type dcc_nv_cmd_ext_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    To avoid lint warning. 
  -------------------------------------------------------------------------*/
  memset(&dcc_nv_cmd_ext_buf, 0, sizeof(nv_cmd_ext_type));
  (void)dcc_nv_cmd_ext_buf;

#ifdef TEST_FRAMEWORK
  #error code not present
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    if(rex_self() != rex_dcc_tcb)
  #else
    if(rex_self() != &dcc_tcb)
  #endif
    {
      DATA_ERR_FATAL("dcc_put_nv_item_ext() not called in dcc task context");
      return NV_BADTG_S;
    }

#ifdef FEATURE_REX_SIGS_EXT
  memset(req_mask, 0, DCC_SIG_ARR_LEN);
#endif /* FEATURE_REX_SIGS_EXT */

  /*-------------------------------------------------------------------------
    Prepare command buffer to write the item to NV.
    TODO:NV team to be notified to change the nv_cmd_buf.sigs to an array
  -------------------------------------------------------------------------*/
  dcc_nv_cmd_buf.cmd        = NV_WRITE_F;           /* Write request       */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  dcc_nv_cmd_buf.tcb_ptr    = rex_dcc_tcb;          /* Notify this task    */
#else
  dcc_nv_cmd_buf.tcb_ptr    = &dcc_tcb;              /* Notify this task    */
#endif  
  dcc_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)DCC_NV_CMD_SIGNAL; /* With this signal    */
  dcc_nv_cmd_buf.done_q_ptr = NULL;            /* Do not enqueue when done */
  dcc_nv_cmd_buf.item       = item_code;            /* Item to put         */
  dcc_nv_cmd_buf.data_ptr   = data_ptr;             /* Data to write       */


  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  DCC_CLR_SIGNAL( DCC_NV_CMD_SIGNAL );             /* Clear signal for NV  */
  
  dcc_nv_cmd_ext_buf.nvcmd   = &dcc_nv_cmd_buf; 
  dcc_nv_cmd_ext_buf.context = context;  /* Populate NV request parameters */
  
  nv_cmd_ext( &dcc_nv_cmd_ext_buf );               /* Issue the request    */
#ifdef FEATURE_REX_SIGS_EXT
  DCC_SET_SIG_MASK(req_mask, DCC_NV_CMD_SIGNAL);
  (void)dcc_wait(req_mask); /* Wait for completion*/
#else
  (void)dcc_wait( (rex_sigs_type) 1 <<
                 (rex_sigs_type)DCC_NV_CMD_SIGNAL ); /* Wait for completion*/
#endif /* FEATURE_REX_SIGS_EXT */

  if( dcc_nv_cmd_buf.status != NV_DONE_S )
  {
    LOG_MSG_ERROR_2( "NV Write Failed Item %d Code %d",
                     dcc_nv_cmd_buf.item, dcc_nv_cmd_buf.status );
  }

  return( dcc_nv_cmd_buf.status );

#endif /* TEST_FRAMEWORK */
} /* dcc_put_nv_item() */
