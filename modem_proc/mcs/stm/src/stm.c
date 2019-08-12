/*==========================================================================

                STM.C  State Machine Subsystem

Description:
  This file contains driver code necessary for state machine operations.

  STM is a framework that automates the generation of a group of state machines.
  A BNF grammar is used to specify the state machine group and the individual
  state machines and states themselves.  During compilation, a state machine
  compiler (smcomp.pl) generates the tables and definitions that describe (in C)
  the specified state machine group construct.

  A state machine group can contain an arbitrary number of concurrently active
  state machines.  Each state machine can contain an arbitrary number of states,
  of which only one is the current state.

  State machines are input driven.  Inputs may be any arbitrary command sent to
  a given state machine.  In the STM grammar, for each state in a state machine,
  a series of inputs are mapped to transition functions.  Also for each state
  there may be entry and/or exit functions specified.  When an input is received
  by a state machine, the transition function for the current state is called.
  The transition function then returns the next state the state machine should
  enter.  If the next state is different than the current state, the exit
  function of the current state is called prior to the entry function for the
  next state.  State machines themselves also have an entry and/or exit
  function.  Upon being started, a state machine's entry function is called.
  Upon being stopped, a state machine's exit function is called.  Similarly,
  state machine groups have an entry and/or exit function.  When a state machine
  group is started, the entry function is called.  When it is stopped, the exit
  function is called.

  State machine groups are composed of one or more state machines.  Each group
  may optionally have an internal command queue that is used to communicate
  between state machines.  There can be an entry and exit function for each
  state machine group and for each state machine.  State machines within an
  active state machine group may be individually activated and deactivated.

  stm_start_group() is used to start a group of state machines.  State machines
  within the group may be started as active or inactive.  Once the group is
  started, the user may optionally call stm_activate() or stm_deactivate() to
  activate or deactivate individual state machines within the group.
  stm_activate() allows the user to start the state machine in its last state or
  in it's initial state.  State machines may also call stm_put_internal_cmd() to
  send internal commands to each other. stm_stop_group() is called to deactivate
  the group and all state machines within the group.

Externalized Functions:
  stm_activate
  stm_group_is_active
  stm_is_active
  stm_deactivate
  stm_start_group
  stm_stop_group
  stm_alloc_msg_buf
  stm_copy_msg_buf
  stm_put_internal_cmd_payload
  stm_delay_internal_cmd_payload
  stm_set_debug
  stm_get_debug
  stm_get_message_index
  stm_process_group
  stm_delete_internal_cmds
  stm_print_group_sm_states

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/stm/src/stm.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/11/13   sr      Change memcpy to memscpy.
10/03/12   ag      Run global and sub group's proc_sig on same set of sigs
03/23/12   ag      Handle NULL tcb pointer. 
05/02/11   hh      Clear the signal if there are no more internal commands.
04/01/11   bb      Changes to support multiple commands posted in STM 
                   internal command queue
01/12/11   sg      Fixed compiler warnings
07/12/10   trc/hh  Added global group signals to wait signal.
01/21/10   ag      Moved the mutex to the command queue structure.
09/01/09   adw     Stop the group directly if stm_stop_group() is called for
                   a group that does not make use of the stm driver.
03/23/09   ag      Replaced INTLOCKS with Rex based critical section macros
09/18/07   bt/trc  Do pre-insert for delayed messages onto the internal queue
                   and set payload to NULL if payload size is 0 and a delayed
                   queue is not used.
07/10/07   bt      Only increment read_index once in stm_delete_internal_cmds.
04/06/07   bt      Always send a payload pointer stm_process_input even if 
                   size is 0.
03/29/07   trc     Update stm_get_internal_cmd() to copy the command queue item
                   so proc_int_cmd() need not worry about its future validity
01/10/07   bt      Added stm_copy_msg_buf, stm_delay_internal_cmd_payload and
                   implemented delayed internal commands.
12/05/06   rkc     In stop_group(), make sure state machine is active before
                   calling any exit funcs.
10/15/06   trc     Convert to STM service
10/13/06   trc     Changes for new STM grammar
08/21/06   rkc     Added stm_print_group_sm_states().
07/26/06   trc     Add special handling for ext. signals that are mapped by the
                   foreground group but need to be handled by the global group
07/19/06   trc     Decoupling STM from SRCH
07/11/06   trc     STM interface changes
06/01/06   tjc     Added stm_get_wait_sigs function
06/01/06   tjc     First pass at removing subsignals
01/16/06   awj     Moved to srch_stm interface, lint cleanup
10/26/05   awj     Lint compliance
09/27/05   ejv     Update statics.
09/26/05   awj     Warn when an input mapping is not present.
09/20/05   ejv     Removed stm_is_group_active.
09/08/05   sfm     Added a call to stm_process_group to process the global
                   group just before stopping the group in stm_driver().
08/18/05   awj     Set command payload fields to NULL after a free
08/09/05   awj     Added drain_int_cmd_queue.
08/08/05   pa      Call stop_group() in stm_process_group() when group does not
                   use driver, fix to not process pending commands after
                   stm_stop_group() is called.
08/08/05   awj     Moved STM_MSG_HDR_WATERMARK to stm.h
                   Throw errors and do not return buffers which have corrupted
                   watermarks or false "checked_out" fields.
                   Free buffers passed to NULL command queues.
07/29/05   pa      Set signal from within task lock in stm put internal cmd.
07/26/05   awj     Changed transition functions to take void * arguments
07/25/05   awj     Added new functionality for buffer reporting.
                   Changed stop_group() to declare the group inactive before
                   purging the int_cmd_queue.
07/14/05   pa      Added distinction between when a group is stopped and when
                   it becomes inactive.
07/11/05   awj     Added message payloads.
06/15/05   bt      Always include command signals into the wait mask.
06/15/05   bt      Don't execute invalid cmds in stm_process_input.
05/31/05   rng     Merged in from SHDR Sandbox.
01/18/05   sfm     Put getting external commands and clearing the command
                   queue signal into a critical section
01/10/05   kwo     Converted stm_debug to a mask
                   Added stm_group_is_active()
01/05/05   sfm     Added stm_delete_internal_cmds()
12/29/04   sfm     Updates for adding the global group to the STM group struct
12/27/04   kwo     Added next_state arg to stm exit functions
12/17/04   kwo     Added stm group to stm_put_internal_cmd(),
                   stm_deactivate(), stm_activate()
09/12/04   br      Changed mechanism for setting user subsignal wait mask.
                   Added stm_dummy_handler() and stm_dummy_subsigs().
09/03/04   kwo     Added more robust signal checking
08/16/04   jcm     stdio.h no longer needed
08/13/04   sst     Corrected index in msg at end of stm_process_input()
07/27/04   kwo     Converted sring messaging to use message service
                   Removed group field from stm_put_internal_cmd(),
                     stm_activate(), stm_deactivate()
                   Removed ambiguity between invalid int msg and empty queue
                   Added external command pointer as payload to trans function
                   Added external command error handling
07/07/04   ejv     Add T_WINCE.
06/22/04   sfm     Added the payload parameter to stm_debug_trace logging
06/01/04   sfm     Added a slow clock time stamp to stm_debug_trace logging
04/30/04   sfm     Added previous_state parameter to state entry functions
04/30/04   kwo     Process the state machine exit functions in reverse order
04/23/04   sfm     Added stm_debug()
04/07/04   ejv     Don't include msm_drv.h.  Changed MSM_LOCK/UNLOCK pair
                   to INTLOCK/FREE pair.
04/05/04   sfm     Added error logging if nothing is processed in stm_driver()
                   and changed stm_stop_group() to delay stop_group processing
                   until the current item being processed is done.
02/10/04   sfm     Modified for lint compliance
02/09/04   bt      Added stm_is_active.
11/19/03   jcm     Call entry functions of all stms in group when created
10/20/03   bt      Don't wait after group becomes inactive.
10/09/03   sfm     Updated for new state machine paradigm
09/18/03   sfm     Changed to use new state machine group paradigm
04/03/03   kwo     STM_DEBUG was renamed to STM_EXTENDED
02/18/03   bt      Added check for locked interrupts after state transition.
01/10/03   gs      Merged in Logic Analyzer Debug
11/25/02   gs      Make STM_DEBUG use consistent and clean up warnings.
10/21/02   gs      Merge in latest smcomp support
09/07/02   gs      Fix number of internal commands
08/18/02   gs      Some cleanup.
08/08/02   gs      Initial Revision
===========================================================================*/

/*===========================================================================
                INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "timetick.h"
#include "stringl.h"
#include "stm.h"

/*===========================================================================
                DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define SM_NOT_IN_GROUP       (-1)

#define INVALID_INT_CMD       ((uint32)~0)

/* STM macros which map to rex critical section calls */
#define STM_INIT_CRIT_SECT(lock)  \
{ \
  REX_DISABLE_PREMPTION(); \
  rex_init_crit_sect(&(lock)); \
  REX_ENABLE_PREMPTION(); \
}
#define STM_ENTER_CRIT_SECT(lock) REX_ISR_LOCK(&(lock));
#define STM_LEAVE_CRIT_SECT(lock) REX_ISR_UNLOCK(&(lock));


/*===========================================================================
                FUNCTION PROTOTYPES
===========================================================================*/
static void stm_free_msg_buf
(
  stm_group_type const  *group,        /* Group whose heap owns the buffer */
  void                  *payload       /* buffer to be freed */
);

/*===========================================================================
                FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================

FUNCTION STM_REQUEUE_DELAYED_CMDS

DESCRIPTION    This function puts messages on the delayed internal command
               queue back on to the internal command queue.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
static void stm_requeue_delayed_cmds
(
  stm_group_type          *group,
  stm_state_machine_type  *sm
)
{
  uint32 int_read_idx;
  uint32 dl_write_idx;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return;
  }

  if ( sm == NULL )
  {
    ERR( "Null state machine pointer", 0, 0, 0 );
    return;
  }

  /* Lock mutex for both the command queues - 1. delayed then 2. internal */
  /*lint -save -e578 Both macros map to INTLOCK() which declares same variable 
    'sav' giving rise to lint error 578. */
  STM_ENTER_CRIT_SECT(group->delayed_int_cmd_queue->stm_q_crit_sect);
  STM_ENTER_CRIT_SECT(group->int_cmd_queue->stm_q_crit_sect);
  /*lint -restore */

  /* In an effort to preserve chronological ordering, we want to prepend
     the delayed command queue to the internal one, while watching out
     for potential overflow of the internal command queue due to too many
     preexisting commands on it.  If there is nothing on the delayed
     command queue, the while() loop won't be entered. */
  while (group->delayed_int_cmd_queue->write_index !=
         group->delayed_int_cmd_queue->read_index)
  {
    /* Snapshot some often used values for readability and possible
       compiler optimization. */
    int_read_idx = group->int_cmd_queue->read_index;
    dl_write_idx = group->delayed_int_cmd_queue->write_index;

    /* Walk the internal command-queue read index backwards by one and
       check for reverse wraparound overflow.  It will now point to the
       queue entry corresponding to a prepend-insert. */
    int_read_idx = (int_read_idx > 0) ?
      (int_read_idx - 1) : (group->int_cmd_queue->q_len - 1);
    if (int_read_idx == group->int_cmd_queue->write_index)
    {
      /* This is bad - we stepped on our own tail - buffer too small! */
      MSG_SPRINTF_1(
        MSG_SSID_DFLT,MSG_LEGACY_ERROR,
        "Int cmd queue overflow due to delayed cmd requeue for group %s",
        group->name );
      ERR_LOG();
      ERR_FATAL("Int cmd queue overflow (delyd cmd requeue)", 0, 0, 0 );
    }

    /* Walk the delayed command-queue write index backwards by one.  The
       check above in the while() condition ensures that we will not
       overstep the read index.  It now will point to the last item
       written to the delayed command queue. */
    dl_write_idx = (dl_write_idx > 0) ?
      (dl_write_idx - 1) : (group->delayed_int_cmd_queue->q_len - 1);

    /* Copy the payload from the last delayed command queue item into
       the tip of the internal command queue (ie. prepend) */
    group->int_cmd_queue->queue[int_read_idx] =
      group->delayed_int_cmd_queue->queue[dl_write_idx];
   
    /* Update the real queue indices for the next go-around. */
    group->int_cmd_queue->read_index = int_read_idx;
    group->delayed_int_cmd_queue->write_index = dl_write_idx;
  }

  /* Unlock mutex for both command queues - 1. internal then 2. delayed  */
  STM_LEAVE_CRIT_SECT(group->int_cmd_queue->stm_q_crit_sect);
  STM_LEAVE_CRIT_SECT(group->delayed_int_cmd_queue->stm_q_crit_sect);

} /* stm_requeue_delayed_cmds */

/*===========================================================================

FUNCTION STM_DRAIN_INT_CMD_QUEUE

DESCRIPTION    Free any buffers currently checked out to a given internal
               command queue. It is possible to specify a state machine in
               which case only commands to that state machine will be drained.

               !!! Please note that only commands to an inactive state machine
                   are drained !!!

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
static void stm_drain_int_cmd_queue
(
  stm_group_type          *group,              /* pointer to group */
  stm_int_q_type          *cmd_q,              /* which int cmd_q to drain */
  stm_state_machine_type  *sm,                 /* for specific state machine
                                                  leave NULL, if all */
  boolean                  clear_int_cmd_sig   /* whether the internal cmd
                                                  sig needs to be cleard */
)
{

  stm_int_cmd_type *cmd;
  uint32            read_index;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return;
  }

  if(cmd_q == NULL)
  {
    ERR( "Null cmd_q pointer", 0, 0, 0 );
    return;
  }

  if(group->tcb == NULL)
  {
    ERR( "Null tcb pointer", 0, 0, 0 );
    return;
  }

  STM_ENTER_CRIT_SECT(cmd_q->stm_q_crit_sect);

  /* if there is a signal associated, clear it */
  if(clear_int_cmd_sig)
  {
    (void) rex_clr_sigs( group->tcb, group->int_cmd_sig );
  }

  for ( /* Start from the read index */
        read_index  = cmd_q->read_index;
        /* Proceed until read_index == write_index */
        read_index != cmd_q->write_index;
        /* Increment read_index, with wraparound for circular buffer */
        read_index  = ( ( read_index + 1 ) < cmd_q->q_len ) ?
                      ( read_index + 1 ) : 0
      )
  {
    cmd = &cmd_q->queue[read_index];

    /* Free internal commands for all state machines that are not active. */
    if ( (cmd->sm != NULL) && !cmd->sm->active )
    {
      /* if there is no state machine specified or the state machine specified
         matches the current command */
      if ( (sm == NULL) || (sm == cmd->sm) )
      {
        /* Free any non-NULL payload */
        if ( cmd->payload != NULL )
        {
          stm_free_msg_buf( group, cmd->payload );
          cmd->payload = NULL;
        }

        /* Mark the cmd as invalid and nullify the SM ptr */
        cmd->id = INVALID_INT_CMD;
        cmd->sm = NULL;
      }
    }
  }

  STM_LEAVE_CRIT_SECT(cmd_q->stm_q_crit_sect);

} /* stm_drain_int_cmd_queue */


/*===========================================================================

FUNCTION STM_QUEUE_INTERNAL_CMD_PAYLOAD

DESCRIPTION    This function queues an internal command on a circular
               buffer and sets the internal command task signal if specified.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
static void stm_queue_internal_cmd_payload
(
  stm_int_q_type         *cmd_q,
  void                   *payload,      /* internal command payload */
  boolean                 set_int_cmd_sig
)
{
  stm_msg_hdr            *header;      /* incoming info buffer */
  stm_state_machine_type *sm;          /* state machine command is for */
  stm_group_type         *group;       /* group that the state machine is in */
  uint32                  id;          /* internal command ID */
  uint32                  write_index; /* internal queue index to write at */
  uint32                  index;       /* temp var for input name lookup */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( payload == NULL )
  {
    ERR( "Null payload passed to stm_stm_queue_internal_cmd_payload, discarding msg",
         0, 0, 0 );
    return;
  }

  header = (stm_msg_hdr *)((uint32)payload - sizeof(stm_msg_hdr));
  if ( header == NULL ||
       header->watermark != STM_MSG_HDR_WATERMARK )
  {
    ERR( "Invalid int cmd payload message, discarding", 0, 0, 0 );
    return;
  }

  sm = header->sm;
  group = header->group;
  id = header->id;

  if ( group == NULL )
  {
    ERR( "Null group pointer in int cmd header, discarding msg (%d)", id, 0, 0 );
    return;
  }

  if ( sm == NULL )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Null SM pointer in int cmd header (group %s)",
      group->name );
    ERR_LOG();

    return;
  }

  if ( !sm->active )
  {
    /* if the sm is inactive, make sure we free the payload */
    stm_free_msg_buf(group, payload);

    MSG_SPRINTF_3(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Int cmd %ld sent to group %s inactive SM %s",
      id, group->name, sm->name );
    ERR_LOG();

    return;
  }

  if ( !group->active )
  {
    /* if the group is inactive, make sure we free the payload */
    stm_free_msg_buf(group, payload);

    MSG_SPRINTF_3(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Int cmd %ld sent to inactive group %s SM %s",
      id, group->name, sm->name );
    ERR_LOG();

    return;
  }

  if ( cmd_q == NULL )
  {
    /* Attempted to put an internal command into a null command queue */
    stm_free_msg_buf(group, payload);

    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "No int cmd queue for group %s",
      group->name );
    ERR_LOG();

    return;
  }

  if(group->tcb == NULL)
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "No tcb ptr for group %s",
      group->name );
    ERR_LOG();
    return;
  }

  STM_ENTER_CRIT_SECT(cmd_q->stm_q_crit_sect);

  /* put the command into the internal queue */
  write_index = cmd_q->write_index;
  cmd_q->queue[write_index].id = id;
  cmd_q->queue[write_index].sm = sm;
  cmd_q->queue[write_index].payload = payload;
  cmd_q->queue[write_index].input_name = NULL;

  /* Look up the input name and stash a pointer to it in the int_cmd_q */
  for ( index = 0; index < sm->number_of_inputs; index++ )
  {
    if ( sm->input_table[index].message == id )
    {
      cmd_q->queue[write_index].input_name =
          sm->input_table[index].input_name;
      break;
    }
  }

  if ( cmd_q->queue[write_index].input_name == NULL )
  {
    MSG_SPRINTF_3(
      MSG_SSID_DFLT, MSG_LEGACY_ERROR,
      "Invalid input %d in group %s SM %s",
      id, group->name, sm->name );
    ERR_LOG();
  }

  /* update the write index */
  if ( ++write_index == cmd_q->q_len )
  {
    write_index = 0;    /* wrap around */
  }

  if ( write_index == cmd_q->read_index )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Int cmd queue overflow for group %s",
      group->name );
    ERR_LOG();

    ERR_FATAL("Int cmd queue overflow", 0, 0, 0 );
  }

  cmd_q->write_index = write_index;

  if(set_int_cmd_sig)
  {
    /* inform the task that an internal command is pending */
    (void) rex_set_sigs( group->tcb, group->int_cmd_sig );
  }

  STM_LEAVE_CRIT_SECT(cmd_q->stm_q_crit_sect);

} /* stm_queue_internal_cmd_payload */


/*======================================================================

  Function:  stm_process_input

  Description:
    This function first translates the message to a given input (via a table
    included in the state machine).  It then calls the exit function of the
    current state followed by the transition function for the given state/input
    combination. Then it calls the entry function for the next state, and
    finally sets the current state to next state.

  Dependencies:
    None

  Return Value:
    TRUE - if input was processed
    FALSE - error happened and input was not processed

  Side Affects:
    None

======================================================================*/
static boolean stm_process_input
(
  stm_group_type                *group,          /* state machine group */
  stm_state_machine_type * const sm,             /* state machine */
  uint32                         message_id,     /* current msg id */
  void *                         payload,        /* message payload */
  boolean   process_input_required   /* is processing of the input required? */
)
{
  int                  input_index;              /* state machine input index */
  stm_state_type       next_state;               /* next state for SM */
  uint32               index;                    /* index into transition
                                                    function array */

  stm_transition_fn_type
                       trans_func;               /* transition function */
  state_exit_fn_type   exit_func;                /* state exit function */
  state_entry_fn_type  entry_func;               /* state entry function */

  user_debug_info_type debug_info;               /* Debug info for group
                                                    user debug function */

  boolean              input_processed = FALSE;  /* set to true if processed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check for a NULL group ptr */
  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return(input_processed);
  }

  /* Check for a NULL SM ptr */
  if ( sm == NULL )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Null SM pointer (but valid group %s)",
      group->name );
    ERR_LOG();

    return(input_processed);
  }

  /* Check for a NULL SM input table ptr */
  if ( sm->input_table == NULL )
  {
    MSG_SPRINTF_2(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Null input table for group %s SM %s",
      group->name,
      sm->name );
    ERR_LOG();

    return(input_processed);
  }

  /* first, store the current message in the state machine */
  sm->current_message_id = message_id;

  /* Map the message to an appropriate input.  Right now, its a linear search,
     but if these lists become big enough, we can sort & do binary search. */
  input_index = stm_get_message_index(sm,message_id);

  if ( input_index == STM_INVALID_INDEX )
  {
    if(process_input_required)
    {
      MSG_SPRINTF_4(
        MSG_SSID_DFLT,MSG_LEGACY_ERROR,
        "Invalid input, group %s SM %s state %s message_id %ld",
        group->name,
        sm->name,
        sm->state_table[sm->current_state].name,
        message_id
        );
      ERR_LOG();
    }

    return(input_processed);
  }

  sm->current_input_index = input_index;

  /* call the transition function */
  index = sm->current_state * sm->number_of_inputs;
  trans_func = sm->transition_table[index + (uint32)input_index];

  if ( trans_func == NULL )
  {
    MSG_SPRINTF_4(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Null transition fn, group %s SM %s state %s input %s",
      group->name,
      sm->name,
      sm->state_table[sm->current_state].name,
      sm->input_table[(uint32)input_index].input_name
      );
    ERR_LOG();

    next_state = STM_SAME_STATE;
  }
  else
  {
    next_state = trans_func( payload );
    input_processed = TRUE;
  }

  if ( sm->current_state == next_state )
  {
    next_state = STM_SAME_STATE;
  }

  if ( next_state != STM_SAME_STATE )
  {
    /* call the exit function for this state */
    exit_func = sm->state_table[sm->current_state].exit_f;
    if ( exit_func != NULL )
    {
      exit_func( payload, next_state );
    }

    /* call the entry function for the next state */
    entry_func = sm->state_table[next_state].entry_f;
    if ( entry_func != NULL )
    {
      entry_func( payload, sm->current_state );
    }
  }

  /* If there is a user debug function, call it now */
  if(group->user_debug_f != NULL)
  {
    /* Populate the debug info structure */
    debug_info.group_name = group->name;
    debug_info.message_id = message_id;
    debug_info.input_payload = payload;
    debug_info.state_machine_id = sm->id;
    debug_info.state_machine_name = sm->name;
    debug_info.current_state = sm->current_state;
    debug_info.debug_mask = sm->debug;
    debug_info.input_index = (uint32)input_index;
    debug_info.input_name = sm->input_table[(uint32)input_index].input_name;
    debug_info.current_state_name =
      sm->state_table[sm->current_state].name;

    /* If next state is STM_SAME_STATE, reflect that next == current so that
       the debug function doesn't need to know any STM internals
       (like STM_SAME_STATE) */
    if(next_state != STM_SAME_STATE)
    {
      debug_info.next_state = next_state;
      debug_info.next_state_name = sm->state_table[next_state].name;
    } else {
      debug_info.next_state = debug_info.current_state;
      debug_info.next_state_name = debug_info.current_state_name;
    }

    /* Call the user specified group debug function */
    group->user_debug_f(&debug_info);
  }

  /* update our current state. */
  if ( next_state != STM_SAME_STATE )
  {
    sm->current_state = next_state;

    /* now that we are in a new state, check whether there are any delayed
       messages that need to be re-queued */
    if(group->delayed_int_cmd_queue)
    {
      stm_requeue_delayed_cmds(group, sm);
    }
  }

  return(input_processed);

}  /* end of stm_process_input */


/*======================================================================

  Function:  sm_in_group

  Description:   Determines if a state machine is in a group

  Dependencies:  None

  Return Value:  index in init_sm struct of the state machine if the state
                 machine is in the group, SM_NOT_IN_GROUP (-1) if not

  Side Affects:  None

======================================================================*/
static int32 sm_in_group
(
  stm_state_machine_type const *sm,       /* the state machine */
  stm_group_type         const *group     /* the group */
)
{
  uint32 i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return SM_NOT_IN_GROUP;
  }

  if ( sm == NULL )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Null SM pointer (but valid group %s)",
      group->name );
    ERR_LOG();

    return SM_NOT_IN_GROUP;
  }

  for ( i = 0; i < group->num_sm; i++ )
  {
    if ( sm == group->sm_list[i] )
    {
      return (int32)i;
    }
  }

  return SM_NOT_IN_GROUP;

}  /* sm_in_group */


/*===========================================================================

FUNCTION STM_FREE_MSG_BUF

DESCRIPTION    This function frees a message buffer including header
               information.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
static void stm_free_msg_buf
(
  stm_group_type const  *group,        /* Group whose heap owns the buffer */
  void                  *payload       /* buffer to be freed */
)
{

  stm_msg_hdr  *hdr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR("Null group pointer, not freeing buffer",0,0,0);
    return;
  }

  if ( payload == NULL )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT, MSG_LEGACY_ERROR,
      "Null payload pointer (group %s), not freeing buffer",
      group->name);
    ERR_LOG();

    return;
  }

  /* Back up to compute where the header should reside */
  hdr = (stm_msg_hdr *)((uint32)payload - sizeof(stm_msg_hdr));

  /* Make sure this looks like a real header */
  if ( hdr->watermark == STM_MSG_HDR_WATERMARK )
  {
    if ( hdr->checked_out )
    {
      hdr->checked_out = FALSE;
      group->user_free_f((void *)hdr);
    }
    else
    {
      MSG_SPRINTF_3(
        MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Not freeing buffer id %d at 0x%x in group %s, not checked out",
        hdr->id, hdr, group->name );
      ERR_LOG();
    }
  }
  else
  {
    MSG_SPRINTF_2(
      MSG_SSID_DFLT, MSG_LEGACY_ERROR,
      "Not freeing buffer at 0x%x in group %s, invalid watermark",
      hdr, group->name );
    ERR_LOG();
  }

} /* stm_free_msg_buf */


/*======================================================================

  Function:  stm_activate

  Description:
    This function sets a state machine active

  Dependencies:
    Caller must have a valid state machine.

  Return Value:
    None

  Side Affects:
    None

======================================================================*/
void stm_activate
(
  stm_state_machine_type *sm,       /* the state machine */
  stm_group_type         *group,    /* the group the SM is in */
  boolean                 reset     /* reset to initial state? */
)
{
  state_entry_fn_type    state_entry_func;
  int32                  sm_index;        /* index of sm in init_sm */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    if ( sm == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "Null group, Null SM" );
      ERR_LOG();
      return;
    }

    MSG_SPRINTF_1( MSG_SSID_DFLT,MSG_LEGACY_ERROR,
                   "Null group pointer, unable to activate SM %s",
                   sm->name );
    ERR_LOG();

    return;
  }

  if ( sm == NULL )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Null SM pointer (but valid group %s)",
       group->name );
    ERR_LOG();

    return;
  }

  sm_index = sm_in_group( sm, group );
  if ( sm_index == SM_NOT_IN_GROUP )
  {
    MSG_SPRINTF_2(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "SM %s is not in group %s",
      sm->name, group->name );
    ERR_LOG();

    return;
  }

  if ( !sm->active )
  {
    sm->active = TRUE;

    /* if an entry function is defined for the state machine, call it */
    if ( sm->entry_f != NULL )
    {
      sm->entry_f( group );
    }

    /* reset the state to the initial state if requested */
    if ( reset )
    {
      sm->current_state = sm->initial_state;
    }

    /* if an entry function is defined for for the current state, call it */
    state_entry_func = sm->state_table[sm->current_state].entry_f;
    if ( state_entry_func != NULL )
    {
      state_entry_func( 0, STM_NO_STATE );
    }
  }

}  /* stm_activate */


/*===========================================================================

FUNCTION STM_GROUP_IS_ACTIVE

DESCRIPTION    This function returns the active status of a state machine group

DEPENDENCIES   None

RETURN VALUE   TRUE if the state machine is active, FALSE if not

SIDE EFFECTS   None

===========================================================================*/
boolean stm_group_is_active
(
  stm_group_type const *group
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR("Null group pointer",0,0,0);
    return(FALSE);
  }

  return(group->active);

} /* stm_group_is_active */


/*===========================================================================

FUNCTION STM_IS_ACTIVE

DESCRIPTION    This function returns the active status of the state machine

DEPENDENCIES   None

RETURN VALUE   TRUE if the state machine is active, FALSE if not

SIDE EFFECTS   None

===========================================================================*/
boolean stm_is_active
(
  stm_state_machine_type const *stm
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( stm == NULL )
  {
    ERR("Null SM pointer",0,0,0);
    return(FALSE);
  }

  return(stm->active);

} /* stm_is_active */


/*======================================================================

  Function:  stm_deactivate

  Description:
    This function deactivates a state machine

  Dependencies:
    Caller must have a valid state machine.

  Return Value:
    None

  Side Affects:
    None

======================================================================*/
void stm_deactivate
(
  stm_state_machine_type *sm,       /* the state machine */
  stm_group_type         *group     /* the group the SM is in */
)
{
  state_exit_fn_type   exit_func;  /* exit function of current state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    if ( sm == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "Null group pointer, Null SM pointer" );
      ERR_LOG();
      return;
    }

    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Null group pointer, unable to deactivate SM %s",
      sm->name );
    ERR_LOG();

    return;
  }

  if ( sm == NULL )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Null SM pointer (but valid group %s)",
      group->name );
    ERR_LOG();

    return;
  }

  if ( !group->active )
  {
    MSG_SPRINTF_2(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "SM %s is in inactive group %s",
      sm->name, group->name );
    ERR_LOG();

    return;
  }

  if ( sm_in_group( sm, group ) == SM_NOT_IN_GROUP )
  {
    MSG_SPRINTF_2(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "SM %s is not in group %s",
      sm->name, group->name );
    ERR_LOG();

    return;
  }

  if ( sm->active )
  {
    /* While executing the exit functions, the state machine should still be
     * active. Clearing of internal command queue should be done after the
     * exit functions are executed. */

    /* if an exit function is defined for the current state, call it */
    exit_func = sm->state_table[sm->current_state].exit_f;
    if ( exit_func != NULL )
    {
      exit_func( 0, STM_NO_STATE );    /* no payload on this call */
    }

    /* if an exit function is defined for the state machine, call it */
    if ( sm->exit_f != NULL )
    {
      sm->exit_f( group );
    }

    /* Set state machine to inactive */
    sm->active = FALSE;

    /* drain commands from the internal command queues */
    if (group->int_cmd_queue != NULL )
    {
      stm_drain_int_cmd_queue( group, group->int_cmd_queue, sm, FALSE );
    }

    if (group->delayed_int_cmd_queue != NULL )
    {
      stm_drain_int_cmd_queue( group, group->delayed_int_cmd_queue, sm, FALSE );
    }
  }

}  /* stm_deactivate */


/*===========================================================================

FUNCTION STM_ALLOC_MSG_BUF

DESCRIPTION    This function allocates a message buffer including header
               information for the state machine driver.

DEPENDENCIES   None

RETURN VALUE   Pointer to the payload field - header data will be in front,
               and clobbering it will have unsavory effects.

SIDE EFFECTS   None

===========================================================================*/
void *stm_alloc_msg_buf
(
 stm_state_machine_type *sm,       /* state machine command is for */
 stm_group_type         *group,    /* group that the state machine is in */
 uint32                  id,       /* internal command ID */
 uint32                  size      /* size of internal command payload */
)
{
  stm_msg_hdr          *message_header;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Handle a null input group */
  if(group == NULL)
  {
    return NULL;
  }

  /* allocate enough for the payload PLUS a header */
  message_header = (stm_msg_hdr *)
    group->user_malloc_f(sizeof(stm_msg_hdr) + size);

  /* save the message header info */
  message_header->sm          = sm;
  message_header->group       = group;
  message_header->id          = id;
  message_header->size        = size;
  message_header->checked_out = TRUE;
  message_header->time        = timetick_get();
  message_header->watermark   = STM_MSG_HDR_WATERMARK;

  /* return the pointer after this header info */
  return((void *)((uint32)message_header + sizeof(stm_msg_hdr)));

} /* stm_alloc_msg_buf */


/*===========================================================================

FUNCTION STM_COPY_MSG_BUF

DESCRIPTION    This function allocates and copies a given message based on
               a payload pointer of the message to be copied.

DEPENDENCIES   None

RETURN VALUE   Pointer to the payload field - header data will be in front,
               and clobbering it will have unsavory effects.

SIDE EFFECTS   None

===========================================================================*/
void *stm_copy_msg_buf
(
 void                   *payload   /* pointer to payload of msg to be copied */
 )
{
  stm_group_type       *group;
  stm_msg_hdr          *src_msg_header;
  stm_msg_hdr          *dst_msg_header;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Handle a null input group */
  if(payload == NULL)
  {
    return NULL;
  }

  src_msg_header = (stm_msg_hdr *)((uint32)payload - sizeof(stm_msg_hdr));

  group = src_msg_header->group;

  /* allocate enough for the payload PLUS a header */
  dst_msg_header = (stm_msg_hdr *)group->user_malloc_f(sizeof(stm_msg_hdr) +
                                                       src_msg_header->size);

  /* copy the message */
  memscpy(
    (void*)dst_msg_header, 
    sizeof(stm_msg_hdr) + src_msg_header->size,
    (void*)src_msg_header,
    sizeof(stm_msg_hdr) + src_msg_header->size
  );

  /* return the pointer after this header info */
  return((void *)((uint32)dst_msg_header + sizeof(stm_msg_hdr)));

} /* stm_copy_msg_buf */


/*===========================================================================

FUNCTION STM_PUT_INTERNAL_CMD_PAYLOAD

DESCRIPTION    This function places an internal command on a circular
               buffer and sets the internal command task signal.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void stm_put_internal_cmd_payload
(
  void                   *payload      /* internal command payload */
)
{
  stm_msg_hdr            *header;      /* incoming info buffer */
  uint32                  id;          /* internal command ID */
  stm_group_type         *group;       /* group that the state machine is in */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( payload == NULL )
  {
    ERR( "Null payload passed to stm_put_internal_cmd_payload, discarding msg",
         0, 0, 0 );
    return;
  }

  header = (stm_msg_hdr *)((uint32)payload - sizeof(stm_msg_hdr));
  if ( header == NULL ||
       header->watermark != STM_MSG_HDR_WATERMARK )
  {
    ERR( "Invalid int cmd payload message, discarding", 0, 0, 0 );
    return;
  }

  group = header->group;
  id = header->id;

  if ( group == NULL )
  {
    ERR( "Null group pointer in int cmd header, discarding msg (%d)", id, 0, 0 );
    return;
  }

  stm_queue_internal_cmd_payload(group->int_cmd_queue, payload, TRUE);

} /* stm_put_internal_cmd_payload */


/*===========================================================================

FUNCTION STM_GET_INTERNAL_CMD

DESCRIPTION    This function retrieves an internal command from a circular
               buffer.

DEPENDENCIES   None

RETURN VALUE   TRUE if internal command was retrieved, FALSE otherwise

SIDE EFFECTS   None

===========================================================================*/
static boolean stm_get_internal_cmd
(
  stm_group_type       *group,  /* group that the state machine is in */
  stm_int_cmd_type     *cmd     /* pointer to internal cmd structure to fill */
)
{
  boolean           status = FALSE; /* return status */
  stm_int_cmd_type  *result = NULL; /* queued internal command */
  uint32            read_index;     /* index to read at */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return status;
  }

  if ( cmd == NULL )
  {
    ERR( "Null cmd pointer", 0, 0, 0 );
    return status;
  }

  if ( group->int_cmd_queue == NULL )
  {
    MSG_SPRINTF_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                   "No int cmd queue for group %s",
                   group->name );
    ERR_LOG();

    return status;
  }

  if(group->tcb == NULL)
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "No tcb ptr for group %s",
      group->name );
    ERR_LOG();
    return status;
  }

  STM_ENTER_CRIT_SECT(group->int_cmd_queue->stm_q_crit_sect);

  /* Is there something on the queue? */
  read_index = group->int_cmd_queue->read_index;
  if ( read_index != group->int_cmd_queue->write_index )
  {
    result = &group->int_cmd_queue->queue[read_index];

    /* update the write index */
    if ( ++read_index == group->int_cmd_queue->q_len )
    {
      read_index = 0;    /* wrap around */
    }
    group->int_cmd_queue->read_index = read_index;

    /* make sure a valid command is sent to an active state machine */
    if (( result->id != INVALID_INT_CMD )  && ( !result->sm->active ))
    {
      MSG_SPRINTF_3(
        MSG_SSID_DFLT,MSG_LEGACY_ERROR,
        "Int cmd %s discarded: group %s, SM %s inactive",
        result->input_name, group->name, result->sm->name );
      ERR_LOG();

      result = NULL;
    }
  }

  /* If result points to an internal command, copy it back to our client */
  if( result != NULL )
  {
    /* implicit memcpy performed by struct assignment */
    *cmd = *result;

    /* clean up the fields in result, since they have now been copied over */
    result->id = INVALID_INT_CMD;
    result->input_name = NULL;
    result->payload = NULL;
    result->sm = NULL;

    /* mark it up as a successful 'get' */
    status = TRUE;
  }

  /* Clear the signal on task TCB if there are no more 
     pending internal command buffers on the queue */
  if ( read_index == group->int_cmd_queue->write_index )
  {
    /* clear the internal command signal */
    (void) rex_clr_sigs( group->tcb, group->int_cmd_sig );
  }

  STM_LEAVE_CRIT_SECT(group->int_cmd_queue->stm_q_crit_sect);

  return status;

} /* stm_get_internal_cmd */


/*===========================================================================

FUNCTION STM_DELAY_INTERNAL_CMD_PAYLOAD

DESCRIPTION    This function delays an internal command until the next
               state change (when it gets re-issued).

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void stm_delay_internal_cmd_payload
(
  void                 *payload
)
{

  stm_msg_hdr            *header;
  uint32                  id;          /* internal command ID */
  stm_group_type         *group;       /* group that the state machine is in */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( payload == NULL )
  {
    ERR( "Null payload passed to stm_delay_internal_msg",
         0, 0, 0 );
    return;
  }

  header = (stm_msg_hdr *)((uint32)payload - sizeof(stm_msg_hdr));

  if ( header == NULL ||
       header->watermark != STM_MSG_HDR_WATERMARK )
  {
    ERR( "Invalid int cmd payload message, discarding", 0, 0, 0 );
    return;
  }

  group = header->group;
  id = header->id;

  if ( group == NULL )
  {
    ERR( "Null group pointer in int cmd header, discarding msg (%d)", id, 0, 0 );
    return;
  }

  /* copy message and put it on the delayed command queue */
  stm_queue_internal_cmd_payload(group->delayed_int_cmd_queue,
                                 stm_copy_msg_buf(payload), FALSE);

} /* stm_delay_internal_cmd_payload */


/*===========================================================================

FUNCTION STOP_GROUP

DESCRIPTION    This function stops a group of state machines

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Active flag in group is cleared

===========================================================================*/
static void stop_group
(
  stm_group_type *group     /* group to stop */
)
{
  int32                    i;             /* loop counter */
  state_exit_fn_type       exit_func;     /* exit function of current state */
  stm_state_machine_type  *sm;            /* ptr to the current state machine */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return;
  }

  /* process all state machines in the group */
  for ( i = (int32)(group->num_sm - 1); i >= 0; i-- )
  {
    sm = group->sm_list[i];

    /* Make sure the state machine is active before calling any exit funcs */
    if (sm->active)
    {
      /* call the exit function for the current state if one is defined */
      exit_func = sm->state_table[sm->current_state].exit_f;
      if ( exit_func != NULL )
      {
        exit_func( 0, STM_NO_STATE );    /* no payload on this call */
      }

      /* if an exit function is defined for the state machine and the
         state machine is active, call the exit function */
      if ( sm->exit_f != NULL )
      {
        sm->exit_f( group );
      }

      sm->active = FALSE;    /* deactivate the state machine */
    }
  }

  /* if defined, call the group exit function */
  if ( group->exit_f != NULL )
  {
    group->exit_f();
  }

  /* All state machines have exited:  group can be set to false
     This needs to occur before the internal command queue is cleared to guard
     against internal commands being inserted by isrs. */
  group->active = FALSE;

  if (group->int_cmd_queue != NULL )
  {
    stm_drain_int_cmd_queue( group, group->int_cmd_queue, NULL, TRUE );
  }

  if (group->delayed_int_cmd_queue != NULL )
  {
    stm_drain_int_cmd_queue( group, group->delayed_int_cmd_queue, NULL, FALSE );
  }

  /* Call the group's heap destructor, if there is one. */
  if(group->user_heap_destroy_f != NULL)
  {
    group->user_heap_destroy_f(group);
  }

}  /* stop_group */


/*===========================================================================

FUNCTION PROC_INT_CMD

DESCRIPTION    This function processes an internal command.

DEPENDENCIES   None

RETURN VALUE   TRUE if the queue contained a command, FALSE otherwise,

SIDE EFFECTS   None

===========================================================================*/
static boolean proc_int_cmd
(
  stm_group_type *group,                 /* state machine group */
  boolean         process_cmd_required   /* is processing of the cmd required?*/
)
{
  stm_int_cmd_type  cmd;             /* current internal command */
  boolean           cmd_rxed = FALSE;/* internal command was received */
  stm_msg_hdr      *msg_hdr;         /* payload header */
  void *            payload;         /* payload for process_input */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*Initialize cmd to avoid warnings */
  cmd.id = 0;
  cmd.sm = NULL;
  cmd.payload = NULL;

  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return FALSE;
  }

  if ( group->int_cmd_queue != NULL )
  {
    cmd_rxed = stm_get_internal_cmd( group, &cmd );
    if ( cmd_rxed && (cmd.id != INVALID_INT_CMD) )
    {
      if (cmd.payload == NULL)
      {
        payload = NULL;
      }
      else
      {
        msg_hdr = (stm_msg_hdr *)((uint32) cmd.payload - sizeof(stm_msg_hdr));

        /* always send a payload if there is a delayed command queue */
        if((group->delayed_int_cmd_queue == NULL) && (msg_hdr->size == 0))
        {
          payload = NULL;
        }
        else
        {
          payload = cmd.payload;
        }
      }

      (void) stm_process_input( group, cmd.sm, cmd.id,
                                payload, process_cmd_required );

    }

    /* Free the payload (except NULL payload) */
    if ( cmd_rxed && (cmd.payload != NULL) )
    {
      stm_free_msg_buf( group, cmd.payload );
    }

  }

  return ( cmd_rxed );

}  /* proc_int_cmd */


/*===========================================================================

FUNCTION STM_GET_WAIT_SIGS

DESCRIPTION    This function returns the wait sigs for the specified group

DEPENDENCIES   None

RETURN VALUE   Group signals to wait on

SIDE EFFECTS   None

===========================================================================*/
static rex_sigs_type stm_get_wait_sigs
(
  stm_group_type const *group        /* state machine group */
)
{
  rex_sigs_type wait_sigs = 0;          /* signals to wait for */
  uint32        i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return FALSE;
  }

  for (i=0; i < group->num_sigs; i++)
  {
    wait_sigs |= group->sig_list[i];
  }

  /* add the command signals */
  wait_sigs    |= group->int_cmd_sig;
  /* add the user signals */

  if ( group->user_f_sig != NULL )
  {
    wait_sigs  |= group->user_f_sig();
  }

  /* If we have a global group, add it's command signals as well */
  if ( group->global_group != NULL )
  {
    wait_sigs    |= group->global_group->int_cmd_sig;

    for (i=0; i < group->global_group->num_sigs; i++)
    {
      wait_sigs |= group->global_group->sig_list[i];
    }

    /* add the global group's user signals */
    if(group->global_group->user_f_sig != NULL)
    {
      wait_sigs  |= group->global_group->user_f_sig();
    }
  }

  return wait_sigs;
} /* stm_get_wait_sigs */


/*===========================================================================

FUNCTION PROC_SIG

DESCRIPTION    This function processes a signal.

DEPENDENCIES   None

RETURN VALUE   TRUE if the queue contained a command, FALSE otherwise,

SIDE EFFECTS   None

===========================================================================*/
static boolean proc_sig
(
  stm_group_type       *group,                /* state machine group */
  boolean               process_sig_required, /* process sig required? */
  rex_sigs_type        sigs                   /* mask of sigs to prcoess */
)
{
  uint32             i;                /* loop counter */
  boolean            handled = FALSE;  /* handled flag */
  rex_sigs_type     *sig_list;
  rex_sigs_type      rec_sig;          /* received signal(s) */
  rex_sigs_type      filter_mask;      /* signals we care about */
  rex_sigs_type      user_sig;         /* user signal(s) */
  stm_sig_cmd_type   sig_cmd;          /* signal command info */
  stm_sig_cmd_type   gg_sig_cmd;       /* global group's signal command info */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure we have a valid group pointer */
  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return handled;
  }

  /* Bail now if this group doesn't process any signals */
  if ( group->num_sigs == 0 )
  {
    return handled;
  }

  if(group->tcb == NULL)
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "No tcb ptr for group %s",
      group->name );
    return handled;
  }

  /* Get our list of signals to handle */
  sig_list = group->sig_list;

  /* get the 'user' signals */
  user_sig = STM_NO_SIGS;
  if ( group->user_f_sig != NULL )
  {
    user_sig = group->user_f_sig();
  }

  /* Construct a filter mask of the signals STM uses, but are processed
     in other places.  In this case int_cmd_sig is handled with
     proc_int_cmd() and the user_sigs are processed with user_f() */
  filter_mask = (~group->int_cmd_sig) & (~user_sig);

  /* get task signals and filter down to the subset we care about */
  rec_sig = stm_get_wait_sigs(group) &  filter_mask & sigs;

  /* Loop over all possible signals.  Note the signals are processed in
     the order provided in the sig_list so there is implied priority */
  for ( i = 0; (i < group->num_sigs) && (rec_sig != 0); i++ )
  {
    /* Spin until we find a signal which is set */
    if ( !(rec_sig & sig_list[i]) )
    {
      continue;
    }

    /* Got one, clear the signal out of the local mask. */
    rec_sig &= ~sig_list[i];

    /* See if we have a NULL (ie. none) mapper function for this group */
    if( group->sig_mapper_f == NULL )
    {
      /* We have no mapper function.  If we _must_ process this signal,
         let's bail by claiming we handled it and clear it from the
         task's signal set. */
      if (process_sig_required)
      {
        (void) rex_clr_sigs( group->tcb, sig_list[i] );
        MSG_SPRINTF_2(
          MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "Group %s has NULL sig mapper fn, clearing signal 0x%x",
          group->name, sig_list[i] );
        ERR_LOG();

        /* return the signal was handled since we did a forced clear*/
        handled = TRUE;
      }

      /* Bail out now.  We either didn't handle the signal, or faked
         handling it. */
      break;
    }

    /* If we get here, we're ready to call the mapper and try processing
       the signal (as it instructs us.)
       Clearing of the task sig is moved to the mapper in case there
       is the need for an atomic operation such as queue magic and
       signal management.  Also if the signal needs to be forwarded
       to the global group we need to leave it set at this point, but
       remove it from our local processing mask */

    /* Run the mapper and handle the 'failure to map case' if it arises */
    if ( !(group->sig_mapper_f(group->tcb, sig_list[i], &sig_cmd)) )
    {
      /* This is the case, where the signal mapper was unable
         to resolve the signal to an input for whatever reason.  The
         mapper should have managed the signal (cleared it) so mark
         it as handled and bail out. */
      MSG_SPRINTF_2(
        MSG_SSID_DFLT, MSG_LEGACY_LOW,
        "Group %s sig mapper failed to map sig 0x%x",
        group->name, sig_list[i] );
      ERR_LOG();

      handled = TRUE;
      break;
    }

    /* Sanity check the SM given to us by the mapper */
    if( sig_cmd.sm == NULL )
    {
      MSG_SPRINTF_2(
        MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Group %s sig mapper provided NULL SM pointer for sig 0x%x",
        group->name, sig_list[i] );
      ERR_LOG();

      handled = FALSE;
      break;
    }

    /* If we get here, a valid map was found, so try to process it */
    if( stm_process_input( group, sig_cmd.sm, sig_cmd.input,
                           sig_cmd.payload, process_sig_required ))
    {
      /* we're done, input has been successfully processed */
      handled = TRUE;
      break;
    }

    /* We weren't able to process the input, but we _were_ able to map
       the signal in this group.  If we have a global group in this group,
       let's let it try to map the signal, as it might also be able to process
       it (for example, an external cmd signal.). */

    /* Make a copy of the group's sig_cmd info, so the global group mapper
       can muck around with it if it likes without disturbing sig_cmd. */
    gg_sig_cmd = sig_cmd;

    /* The global group's mapper function has to be smart enough to know that
       the foreground group's mapper may have already pulled a command off the
       external queue (or not) and potentially re-use the sig_cmd structure's
       contents.  This is done by passing NULL as the rex_tcb_type parameter. */
    if( (group->global_group != NULL) &&
        (group->global_group->sig_mapper_f != NULL) &&
        (group->global_group->sig_mapper_f(NULL,sig_list[i],&gg_sig_cmd)) )
    {
      /* OK - the global's group's mapper can also handle this signal.
         Call stm_process_input() with the global group's gg_sig_cmd info
         and return happily if it works. Note that we tell stm_process_input()
         that processing is _required_ since this is the last chance to handle
         a signal that is common to the foreground & global group. */
      if( stm_process_input( group->global_group,
                             gg_sig_cmd.sm,
                             gg_sig_cmd.input,
                             gg_sig_cmd.payload,
                             TRUE ) )
      {
        /* we're done, input has been successfully processed */
        handled = TRUE;
      } else {
        /* The global group also didn't handle the input, so we must allow
           the global group's error handler function (if it exists) to
           handle the failure case and finally declare that we handled the sig.
           Note that we are using gg_sig_cmd, which is originally a copy of
           sig_cmd, but the global group's mapper can override any parameter. */
        if ( gg_sig_cmd.sig_error_handler_f != NULL )
        {
          gg_sig_cmd.sig_error_handler_f(&gg_sig_cmd);
          handled = TRUE;
        }
      }

      /* Win or lose, we tried everything we could to process the signal
         that is common to both the foreground group and the global group.
         We should bail now, as we know there's nothing more that can be
         done in this case. */
      break;
    }

    /* We didn't process the signal, and the global group (if it existed)
       didn't claim/map it either.  If we were required to process it, though,
       there is one more chance to handle this signal. If the group's mapper
       provided us an error handler function for this sig, let's call it and
       bail indicating success. */
    if ( (process_sig_required) &&
         (sig_cmd.sig_error_handler_f != NULL) )
    {
      sig_cmd.sig_error_handler_f(&sig_cmd);
      handled = TRUE;
      break;
    }

    /* If we get here, we didn't process the input but weren't required
       to.  Let's bail now and give someone else a chance to handle this
       signal (preserving the 'priority' of the signal over any others that
       might still be set in our pending signal set, rec_sig. */
    break;

  } /* for loop over claimed sigs */

  /* Return with handled status from within the for() loop. */
  return(handled);

}  /* proc_sig */


/*===========================================================================

FUNCTION STM_PROCESS_GROUP

DESCRIPTION    This function processes all inputs for a state machine group.

DEPENDENCIES   None

RETURN VALUE   TRUE if at least one item was processed, FALSE otherwise

SIDE EFFECTS   None

===========================================================================*/
boolean stm_process_group
(
  stm_group_type *group     /* state machine group */
)
{
  boolean processed = FALSE;/* was a cmd/event/signal processed? */
  boolean has_global_group; /* no global group? */
  rex_sigs_type  sigs_set;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return processed;
  }

  if ( !group->active )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT, MSG_LEGACY_ERROR,
      "Attempt to process inactive group %s",
      group->name );
    ERR_LOG();

    return processed;
  }

  /* Is there a global group to fall back to or are the lower level STMs
     required to handle the inputs */
  has_global_group = (group->global_group != NULL);

  while ( !group->stopped )
  {
    /* Get the sigs which are set for the current task... */
    sigs_set = rex_get_sigs(rex_self());

    /* Process a signal for the group.  Note that if the signal can't be
       processed by the group, and the global group also handles the same
       signal, it is given a chance to process it as well. */
    if ( proc_sig( group, (boolean)(!has_global_group), sigs_set) )
    {
      processed = TRUE;
      continue;
    }

    /* Process a signal for the global group (if there is one) that was not
       handled normally by the foreground group.  This will only happen for the
       signals that the global group handles, but the foreground group does
       not. */
    if ( has_global_group &&
         proc_sig( group->global_group, TRUE, sigs_set ) )
    {
      processed = TRUE;
      continue;
    }

    /* process an internal command for the group */
    if ( proc_int_cmd( group, (boolean)(!has_global_group) ) )
    {
      processed = TRUE;
      continue;
    }

    /* Process an internal command for the global group.  Note that
       this will only do any work if the global group has a different
       internal command queue than the foreground group.  Otherwise, the
       above will empty the shared internal command queue. */
    if ( has_global_group &&
         proc_int_cmd( group->global_group, TRUE ))
    {
      processed = TRUE;
      continue;
    }

    /* call the user provided function (if any) for the group */
    if ( group->user_f != NULL )
    {
      if ( group->user_f() )
      {
        processed = TRUE;
      }
    }

    /* call the user provided function (if any) for the global group */
    if ( has_global_group &&
         (group->global_group->user_f != NULL) )
    {
      if ( group->global_group->user_f() )
      {
        processed = TRUE;
      }
    }

    /* when we get this far, everything is processed so leave the loop */
    break;
  }

  /* In the normal case, stop_group() is called from stm_driver when we are
   * exiting from the driver. If we are not using the driver and if
   * stm_stop_group() was called, we will call stop_group() here.
   * This will make sure that the exit functions have been executed and
   * the group is set to inactive. This code can be removed when all search
   * states use the stm driver. Also see note on STM_GROUP_TYPE in stm.h.
   */
  if ( !group->using_driver && group->stopped )
  {
    /* Call exit fns if we are not using driver and group was stopped */
    stop_group( group );
  }

  return processed;

}  /* stm_process_group */


/*===========================================================================

FUNCTION STM_DRIVER

DESCRIPTION    This is the driver for the state machines.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
static void stm_driver
(
  stm_group_type *group        /* state machine group */
)
{
  rex_sigs_type     wait_sigs;       /* signals to wait for */
  rex_sigs_type     sigs;            /* received signals */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR( "NULL group pointer", 0, 0, 0 );
    return;
  }

  /* Initialize the sigs */
  sigs = STM_NO_SIGS;

  /* See note for STM_GROUP_TYPE in stm.h */
  while ( !group->stopped )
  {

    /* wait for next signal/cmd */
    wait_sigs = stm_get_wait_sigs(group);

    if ( group->wait_f != NULL )
    {
      sigs = group->wait_f( wait_sigs );
    }

    /* process inputs for the group */
    if ( !stm_process_group( group ))
    {
      MSG_SPRINTF_3(
        MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "No cmd/sig processed for group %s: waitsig=0x%x rxsig=0x%x",
        group->name, wait_sigs, sigs );
      ERR_LOG();
    }
  }

  if ( group->global_group != NULL )
  {
    (void) stm_process_group( group->global_group );
  }

  stop_group( group );

}  /* stm_driver */


/*===========================================================================

FUNCTION STM_START_GROUP

DESCRIPTION    This function starts a group of state machines.

DEPENDENCIES   Group must not already be started

RETURN VALUE   None

SIDE EFFECTS   Active flag in group is set

===========================================================================*/
void stm_start_group
(
  stm_group_type *group,           /* group to start */
  boolean         start_driver     /* call stm_driver() on this group? */
)
{
  state_entry_fn_type     entry_f;
  stm_state_machine_type *sm = NULL;    /* state machine command is for */
  uint32 i;                             /* loop counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure we have a valid group ptr */
  if ( group == NULL )
  {
    ERR( "NULL group pointer", 0, 0, 0 );
    return;
  }

  /* Make sure we have > 0 state machines */
  if ( group->num_sm == 0 )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Empty group %s",
      group->name );
    ERR_LOG();

    return;
  }

  /* Make sure the group is NOT already active */
  if ( group->active )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Group %s already active",
      group->name );
    ERR_LOG();

    return;
  }

  /* Make sure the group has valid malloc/free function pointers. */
  if((group->user_malloc_f == NULL) ||
     (group->user_free_f == NULL))
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "NULL malloc() and/or free() fn ptrs in group %s",
      group->name );
    ERR_LOG();

    return;
  }

  /* If the group has signals, make sure there is a signal mapper */
  if ((group->num_sigs > 0) && (group->sig_mapper_f == NULL))
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "NULL signal mapper function for signals in group %s",
      group->name );
    ERR_LOG();

    return;
  }
  
  /* If the group doesnt have a tcb, assign it the current task's tcb */
  if (group->tcb == NULL)
  {
 	  group->tcb = rex_self();
	
	  MSG_SPRINTF_2(
      MSG_SSID_DFLT,MSG_LEGACY_HIGH,
      "NULL tcb for group %s. using caller tcb  0x%x",
      group->name, group->tcb );
  }

  /* Create the group's heap manager, if there is one. */
  if(group->user_heap_create_f != NULL)
  {
    group->user_heap_create_f(group);
  }

  /* Initialize the critical sections if not already */
  if (group->int_cmd_queue != NULL &&
      !group->int_cmd_queue->mutex_init)
  {
    STM_INIT_CRIT_SECT (group->int_cmd_queue->stm_q_crit_sect);
    group->int_cmd_queue->mutex_init = TRUE;
  }

  if ( group->delayed_int_cmd_queue != NULL && 
       !group->delayed_int_cmd_queue->mutex_init )
  {
    STM_INIT_CRIT_SECT (group->delayed_int_cmd_queue->stm_q_crit_sect);
    group->delayed_int_cmd_queue->mutex_init = TRUE;
  }

  /* initialize all state machines in the group */
  for ( i = 0; i < group->num_sm; i++ )
  {
    sm = group->sm_list[i];
    sm->active = sm->start_active;
    sm->current_state = sm->initial_state;
  }

  /* mark the group as active */
  group->active = TRUE;

  /* mark the group as NOT stopped */
  group->stopped = FALSE;

  /* mark whether or not we're using stm_driver() */
  group->using_driver = start_driver;

  /* if defined, call the group entry function */
  if ( group->entry_f != NULL )
  {
    group->entry_f();
  }

  /* if defined, call state machine entry functions for any initially active
     state machines in the group */
  for ( i = 0; i < group->num_sm; i++ )
  {
    sm = group->sm_list[i];
    if ( sm->start_active )
    {
      /* Call State Machine Entry Function if it exists */
      if (sm->entry_f != NULL )
      {
        /* SM entry function takes the group * as param */
        sm->entry_f( group );
      }

      /* Call State Entry Function if it exists */
      entry_f = sm->state_table[sm->initial_state].entry_f;
      if ( entry_f != NULL )
      {
        /* State entry function takes a payload and previous state as params */
        entry_f( NULL, STM_NO_STATE );
      }
    }
  }

  /* If the group is to be managed by the STM driver, call it.  Otherwise,
     we'll leave the details up to the caller and assume they know what they
     are doing. */
  if ( start_driver )
  {
    /* start the driver - doesn't return until stm_stop_group() is called */
    stm_driver( group );
  }

}  /* stm_start_group */


/*===========================================================================

FUNCTION STM_STOP_GROUP

DESCRIPTION    This function stops a group of state machines

DEPENDENCIES   Group must be started

RETURN VALUE   None

SIDE EFFECTS   Stopped flag in group is set

===========================================================================*/
void stm_stop_group
(
  stm_group_type *group     /* group to stop */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure we have a valid group ptr */
  if ( group == NULL )
  {
    ERR( "NULL group pointer", 0, 0, 0 );
    return;
  }

  /* Make sure the group is NOT already active */
  if ( !group->active )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,MSG_LEGACY_ERROR,
      "Group %s already inactive",
      group->name );
    ERR_LOG();

    return;
  }

  /* See note for STM_GROUP_TYPE in stm.h */
  group->stopped = TRUE;

  /* If we are not using the stm driver... */
  if ( !group->using_driver )
  {
    /* Call the exit functions directly as we are not using the stm driver
       and the group needs to be stopped now. */
    stop_group( group );
  }

}  /* stm_stop_group */


/*===========================================================================

FUNCTION STM_SET_DEBUG

DESCRIPTION    This function sets or clears the debug_on flag for a
               state machine

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void stm_set_debug
(
  stm_state_machine_type *sm,
  uint32                  mask
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( sm == NULL )
  {
    ERR( "Null SM pointer", 0, 0, 0 );
    return;
  }

  sm->debug = mask;

}  /* stm_set_debug */


/*===========================================================================

FUNCTION STM_GET_DEBUG

DESCRIPTION    This function retrieves the debug mask for a state machine

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
uint32 stm_get_debug
(
  stm_state_machine_type const *sm
)
{
  uint32 debug_mask = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( sm == NULL )
  {
    ERR( "Null SM pointer", 0, 0, 0 );
  } else {
    debug_mask = sm->debug;
  }

  return(debug_mask);

}  /* stm_get_debug */


/*===========================================================================

FUNCTION STM_GET_MESSAGE_INDEX

DESCRIPTION    This function returns the index of the message_id for
               a provided state machine.  This is useful for encoding
               various message ids into the debug bitmask.

DEPENDENCIES   None

RETURN VALUE   The index of the specified command, or STM_INVALID_INDEX
               if the message_id isn't handled by the state machine.

SIDE EFFECTS   None

===========================================================================*/
int stm_get_message_index
(
  stm_state_machine_type const *sm,
  stm_input_type                message_id
)
{
  uint32 i;
  int    index = STM_INVALID_INDEX;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( sm == NULL )
  {
    ERR( "Null SM pointer", 0, 0, 0 );
  } else {
    /* Look for the message_id in our SM structure */
    for(i=0; i < sm->number_of_inputs; i++)
    {
      if(sm->input_table[i].message == message_id)
      {
        /* Grab the index and bail */
        index = (int)i;
        break;
      }
    }
  }

  return(index);
}


/*===========================================================================

FUNCTION STM_DELETE_INTERNAL_CMDS

DESCRIPTION    This function deletes messages from the internal message queue
               of the specified state machine group.  The caller provides an
               array of state machine/command ID pairs to be removed.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void stm_delete_internal_cmds
(
  stm_group_type            *group,       /* state machine group */
  stm_int_cmd_id_type const *cmds,        /* array of commands to be deleted */
  uint32                     count        /* number of entries in cmds */
)
{
  stm_int_cmd_type* int_cmd_p;      /* ptr to an internal queue entry */
  uint32            read_index;     /* internal command queue read index */
  uint32            i;              /* loop counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR( "Null group pointer", 0, 0, 0 );
    return;
  }

  if ( cmds == NULL || count == 0 )
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT, MSG_LEGACY_ERROR,
      "No command/s given to delete in group %s",
      group->name );
    ERR_LOG();

    return;
  }

  STM_ENTER_CRIT_SECT(group->int_cmd_queue->stm_q_crit_sect);
  read_index = group->int_cmd_queue->read_index;

  /* loop over all commands in the internal command queue */
  while ( read_index != group->int_cmd_queue->write_index )
  {
    /* loop over all command IDs in the list of IDs to be deleted */
    for ( i = 0; i < count; i++ )
    {
      int_cmd_p = &group->int_cmd_queue->queue[ read_index ];
      if ( int_cmd_p->id == cmds[ i ].id &&
         ( int_cmd_p->sm == cmds[ i ].sm || cmds[ i ].sm == NULL ))
      {
        /* delete the command from the queue */
        int_cmd_p->id = INVALID_INT_CMD;
        if ( int_cmd_p->payload != NULL )
        {
          stm_free_msg_buf( group, int_cmd_p->payload );
          int_cmd_p->payload = NULL;
        }
        /* Command neutralized, break out of for() loop */
        break;
      }
    }

    if ( ++read_index == group->int_cmd_queue->q_len )
    {
      read_index = 0;    /* wrap around */
    }
  }
  STM_LEAVE_CRIT_SECT(group->int_cmd_queue->stm_q_crit_sect);

}  /* stm_delete_internal_cmds */


/*===========================================================================

FUNCTION STM_PRINT_GROUP_SM_STATES

DESCRIPTION    This function prints the diag state of all the state machines
               in a given group.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void stm_print_group_sm_states
(
  stm_group_type const *group
)
{
  stm_state_machine_type *sm = NULL;    /* state machine ptr */
  uint32 i;                             /* loop counter */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( group == NULL )
  {
    ERR("Null group pointer",0,0,0);
    return;
  }

  /* Print a group diag header for the messages to follow */
  MSG_SPRINTF_2(MSG_SSID_DFLT,MSG_LEGACY_HIGH,
                "STM group (%s) active state table (%d state machines):",
                group->name,
                group->num_sm);

  /* For each state machine in the group, print if it is active or not */
  for ( i = 0; i < group->num_sm; i++ )
  {
    sm = group->sm_list[i];
    if (sm->active)
    {
      MSG_SPRINTF_1(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"%22s = ACTIVE",
                    sm->name);
    }
    else
    {
      MSG_SPRINTF_1(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"%22s = inactive",
                    sm->name);
    }
  }

} /* stm_print_group_sm_states */

