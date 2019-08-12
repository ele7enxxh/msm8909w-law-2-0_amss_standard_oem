#ifndef STM_H
#define STM_H
/*===========================================================================

                STM.H - State Machine  Header File

DESCRIPTION
  This file contains the state machine structures necessary for STM
  operation.

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

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/stm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/10   sb      Make stm_msg_hdr 64-bit aligned 
01/21/10   ag      Moved the mutex to the command queue structure.
03/23/09   ag      Added a stm_crit_sect field to stm_group structure
08/13/08   sst     Add include of timetick.h
09/18/07   bt      Added NULL payload check macro back in.
01/10/07   bt      Added stm_copy_msg_buf, stm_delay_internal_cmd_payload and 
                   implemented delayed internal commands.
10/15/06   trc     Convert to STM service
10/13/06   trc     Changes for new STM grammar
08/21/06   rkc     Added stm_print_group_sm_states().
07/19/06   trc     Decoupling STM from SRCH
07/11/06   trc     STM interface changes
06/01/06   tjc     Added stm_get_wait_sigs function
06/01/06   tjc     First pass at removing subsignals
01/16/06   awj     Moved to srch_stm interface, lint cleanup
11/14/05   awj     Lint compliance
10/03/05   pa      Added STM_DEL_INTERNAL_CMD and STM_ALLOC_PUT_INTERNAL_CMD.
10/03/05   awj     Corrected order of variable names in macro
09/20/05   ejv     Removed stm_is_group_active.
08/09/05   awj     Added STM_NULL_PAYLOAD_CHECK
08/08/05   pa      Added a boolean to remember how the group was started.
08/08/05   awj     Brought over STM_MSG_HDR_WATERMARK from stm.c
07/26/05   awj     Changed transition functions to take void * arguments
07/25/05   awj     Added new functionality for buffer reporting.
07/14/05   pa      Added distinction between when a group is stopped and when
                   it becomes inactive.
07/11/05   awj     Added message payloads.
04/25/05   kwo     Added STM_INPUT_TO_MASK()
01/10/05   kwo     Converted stm_debug to a mask
                   Added stm_group_is_active()
01/05/05   sfm     Added stm_delete_internal_cmds()
12/29/04   sfm     Updates for adding the global group to the STM group struct
12/27/04   kwo     Added next_state arg to stm exit functions
12/17/04   kwo     Added stm group to stm_put_internal_cmd(),
                    stm_deactivate(), stm_activate()
09/12/04   br      Changed mechanism for setting user subsignal wait mask.
                   Added stm_dummy_handler() and stm_dummy_subsigs() prototypes.
07/27/04   kwo     Interface changes to stm_put_internal_cmd(),
                   stm_activate(), stm_deactivate()
04/30/04   sfm     Added previous_state parameter to state entry function
04/23/04   sfm     Added stm_debug()
02/09/04   bt      Added stm_is_active.
10/28/03   ddh     Moved NOTUSED macro to srchi.h
10/09/03   sfm     Updated for new state machine paradigm
09/30/03   sfm     Changed to use new state machine group paradigm
04/03/03   kwo     STM_DEBUG was renamed to STM_EXTENDED
01/10/03   gs      Merge in state_machine_id field
11/25/02   gs      Make STM_DEBUG use consistent.
09/07/02   gs      Fix "number of internal commands"
08/18/02   gs      Some cleanup.
08/08/02   gs      Initial Revision
===========================================================================*/

/*===========================================================================
                INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "timetick.h"


/*===========================================================================
                DEFINITIONS AND DECLARATIONS
===========================================================================*/

/* Return value of transition function when no state change is to occur */
#define STM_SAME_STATE              ((stm_state_type)~0)

/* Used as a param for a state entry or exit function where there is no
   previous or next state (generally in the case of state machine activation or
   deactivation.) */
#define STM_NO_STATE                (STM_SAME_STATE - 1)

/* Watermark value used to separate an internal command header from the
   command payload.  Used internally to detect corruption, and can be used
   externally by memory manager to further monitor command buffer status. */
#define STM_MSG_HDR_WATERMARK       ((uint32)0xdeadbeef)

/* Used to indicated that the message index was not found when the function
   stm_get_message_index() is called for an unspecified message in a given
   state machine. */
#define STM_INVALID_INDEX           (-1)

/* A signal mask indicating NO signals are set. */
#define STM_NO_SIGS                 0


/*===========================================================================
    HELPER MACROS
===========================================================================*/

/* Deletes a single internal command corresponding to a given state machine */
#define STM_DEL_INTERNAL_CMD( sm, sm_group, cmd )                      \
  {                                                                    \
    stm_int_cmd_id_type _del_cmd_buf = { (sm), (uint32)(cmd) };        \
    stm_delete_internal_cmds( (sm_group), &_del_cmd_buf, 1 );          \
  }

/* Allocates a msg buf based on the type given, fills it with data and
   puts it into the internal command queue. Note that type should not be
   surrounded by parantheses while using this macro. */
#define STM_ALLOC_PUT_INTERNAL_CMD( sm, sm_group, cmd, type, data )     \
  {                                                                     \
    type *_payload;                                                     \
    _payload = (type *)                                                 \
      stm_alloc_msg_buf( (sm), (sm_group), (uint32)cmd, sizeof(type) ); \
    *_payload = (data);                                                 \
    stm_put_internal_cmd_payload( (void *)_payload );                   \
  }

/* Puts a msg buf in the internal queue with no payload other than the
   command itself */
#define STM_PUT_INTERNAL_CMD(sm_id, sm_group, cmd)                      \
  stm_put_internal_cmd_payload(                                         \
    stm_alloc_msg_buf(sm_id, sm_group, (uint32)cmd, 0) )

/* Allocate a message buffer for the user to fill with data.  Does not queue
   the command! */
#define STM_ALLOC_MSG_BUF( sm, group, cmd, size )                       \
  stm_alloc_msg_buf( sm, group, (uint32)cmd, size )

/* Helper function for use within a state for NULL payload checking.  Returns
   STM_SAME_STATE if payload == NULL. */
#define STM_NULL_PAYLOAD_CHECK( pload )                                    \
  if ( (pload) == NULL )                                                   \
  {                                                                          \
    MSG_ERROR("Null payload not expected in transition function", 0, 0, 0 ); \
    return STM_SAME_STATE;                                                   \
  }

/* Helper function for use within a state for NULL payload checking. */
#define STM_PAYLOAD_IS_EMPTY( pload )                                      \
  (((pload) == NULL ) ||                                                   \
   ((stm_msg_hdr *)((uint32) pload - sizeof(stm_msg_hdr)))->size == 0)

/* Helper function for use within a state for empty payload checking. */
#define STM_EMPTY_PAYLOAD_CHECK( pload )                                   \
  if(STM_PAYLOAD_IS_EMPTY( pload ))                                        \
  {                                                                          \
    MSG_ERROR("Null payload not expected in transition function", 0, 0, 0 ); \
  } 

/* Helper function for use within a state for empty payload checking. */
#define STM_EMPTY_PAYLOAD_CHECK_AND_RETURN( pload )                        \
  if(STM_PAYLOAD_IS_EMPTY( pload ))                                        \
  {                                                                          \
    MSG_ERROR("Null payload not expected in transition function", 0, 0, 0 ); \
    return(STM_SAME_STATE);                                                  \
  } 


/*===========================================================================
  STM_STATE_TYPE and STM_INPUT_TYPE

    These two typedefs are used in application code as variables for the
    various states and inputs.
===========================================================================*/
typedef uint32 stm_state_type;
typedef uint32 stm_input_type;

// State entry/exit function types
typedef void (*state_entry_fn_type)(void *,stm_state_type prev_state);
typedef void (*state_exit_fn_type)(void *,stm_state_type next_state);

// State array type
typedef struct
{
  const char * const  name;
  state_entry_fn_type entry_f;
  state_exit_fn_type  exit_f;
} stm_state_array_type;

// Input array type
typedef struct
{
  stm_input_type      message;
  const char * const  input_name;
} stm_input_array_type;

// Transition function type
typedef stm_state_type (*stm_transition_fn_type)(void *payload);


/*==========================================================================
  USER_DEBUG_INFO_TYPE
  USER_DEBUG_FN_TYPE

    Group debug function type and structure for use in monitoring
    the processing of each input for a group.

============================================================================*/
typedef struct {
  uint32          state_machine_id;   /* Unique SM id, generated by smcomp
                                         for TLA tracing */
  uint32          debug_mask;         /* Current SM's debug mask */
  uint32          message_id;         /* External message ID */
  uint32          input_index;        /* Internal input index */
  stm_state_type  current_state;      /* Current SM state */
  stm_state_type  next_state;         /* Next SM state (on transition) */
  const char     *group_name;         /* SM Group name */
  const char     *state_machine_name; /* SM name */
  const char     *input_name;         /* Input name */
  const char     *current_state_name; /* Current state name */
  const char     *next_state_name;    /* Next state name (on transition) */
  void           *input_payload;      /* Pointer to input's payload */
} user_debug_info_type;

typedef void  (*user_debug_fn_type)(user_debug_info_type *);

/*==========================================================================
  Pre-declare some group typedefs so they
  can be used within their own struct declarations.
============================================================================*/
struct stm_group;
typedef struct stm_group stm_group_type;



/*===========================================================================
  STATE_MACHINE_TYPE

    The state machine type contains all the relevant information about
    a given state machine, including the state table, input table, and
    transition table.  It provides a single access point to all state
    machine operations.

    The active flag is used to route a given signal to a set of
    state machines. Because a signal does not have a destination and
    because it is advantageous to be able to "route" a signal to more
    than one state machine at different times, we need to identify
    which of those state machines will be responsible for processing the
    signal.

============================================================================*/

// State machine entry/exit function types
typedef void (*sm_entry_fn_type)(stm_group_type *);
typedef void (*sm_exit_fn_type)(stm_group_type *);


typedef struct
{
  const char * const                    name;
  const uint32                          id;
  const sm_entry_fn_type                entry_f;
  const sm_exit_fn_type                 exit_f;
  const stm_state_type                  initial_state;
  const boolean                         start_active;
  const uint32                          number_of_states;
  const uint32                          number_of_inputs;
  const stm_state_array_type * const    state_table;
  const stm_input_array_type * const    input_table;
  const stm_transition_fn_type * const  transition_table;

  /* Private state machine data */
  stm_state_type                        current_state;
  int                                   current_input_index;
  uint32                                current_message_id;
  boolean                               active;
  uint32                                debug;
} stm_state_machine_type;

/*==========================================================================
  STM_SIG_CMD_TYPE

    Used by the signal mapper to map signals to external commands and
    a designated SM.  Also provides an error handler function if delivery
    fails.
============================================================================*/
typedef struct stm_sig_cmd
{
  rex_sigs_type           signal;    /* the signal being mapped */
  stm_state_machine_type *sm;        /* ptr to a state machine to handle sig */
  stm_input_type          input;     /* input to be sent to the state machine */
  void                   *payload;   /* ptr to the payload to pass, if any */
                                     /* function ptr to an error handler
                                        to call upon failure to process
                                        this signal (optional, can be NULL) */
  void                  (*sig_error_handler_f)(struct stm_sig_cmd *);
} stm_sig_cmd_type;

/*==========================================================================
  STM_SIG_MAP_TYPE

    Used to map external signals onto internal commands and provide
    optional error handling in the case that they're not handled by the
    designated SM.
============================================================================*/
typedef boolean (*sig_mapper_fn_type)(rex_tcb_type*,
                                      rex_sigs_type,
                                      stm_sig_cmd_type *);

/*==========================================================================
  STM_INT_CMD_TYPE

    Defines an entry on the internal command queue.  'payload' a void ptr
    that is interpreted based on 'id' by the receiving state machine as
    a 32 bit value or a pointer to some type of data.
============================================================================*/
typedef struct
{
  stm_state_machine_type *sm;              /* ptr to state machine */
  uint32                  id;              /* internal command ID */
  void                   *payload;         /* internal command payload */
  const char             *input_name;      /* string describing input name */

} stm_int_cmd_type;

/*==========================================================================
  STM_INT_Q_TYPE

    Internal message queue for communciations between the state machine that
    processes external commands and all other state machines in the group
============================================================================*/
typedef struct
{

  uint32               read_index;       /* index of next internal command */
  uint32               write_index;      /* index of last internal command */
  uint32               q_len;            /* length of queue in commands    */
  stm_int_cmd_type*    queue;            /* ptr to internal command queue  */
  rex_crit_sect_type   stm_q_crit_sect;  /* Critical section lock to protect
                                            queue from concurrent access   */
  boolean              mutex_init;       /* Flag to check if mutex has been 
                                            initialized.                   */

} stm_int_q_type;


/*==========================================================================
  USER_HEAP_FN_TYPE
  USER_MALLOC_FN_TYPE
  USER_FREE_FN_TYPE

    Group heap and memory allocation functions.

============================================================================*/
typedef void  (*user_heap_fn_type)(stm_group_type *);
typedef void *(*user_malloc_fn_type)(uint32);
typedef void  (*user_free_fn_type)(void *);

// Group entry/exit function types
typedef void (*group_entry_fn_type)(void);
typedef void (*group_exit_fn_type)(void);

// Group wait function type
typedef rex_sigs_type (*wait_fn_type)(rex_sigs_type sigs);

// Deprecated - user sig and handler fn types
typedef rex_sigs_type (*user_sigs_fn_type)(void);
typedef boolean (*user_sig_handler_fn_type)(void);

/*==========================================================================
  STM_GROUP_TYPE

    Used to define a group of state machines.  Exactly one state machine
    receives external commands and signals.  If internal messaging is not used,
    int_cmd_queue and int_cmd_tcb should be set to NULL.

    Ideally the group should be set to inactive after executing the exit
    functions of all the state machines and states of the group and not when
    stm_stop_group() is called. Otherwise, the following might happen.
    a) a function called by an exit function might return/print an error since
       group is inactive or
    b) a callback function that is cancelled by an exit function might now run
       and return/print an error since group is inactive.

    So a new boolean 'stopped' was added to control the stm_driver.
    It is set when stm_stop_group() is called and this stops the driver.
    The 'active' boolean is reset only after calling the exit functions
    in stop_group().

    When a group does not use stm driver, we need a way to call stop_group()
    after stm_stop_group() is called. Note that this cannot be done from within
    stm_stop_group() since the call to stm_stop_group() can occur in the
    middle of a transition function. So stop_group() is called from
    stm_process_group(). The boolean 'using_driver' is used to remember how
    the group was started.

============================================================================*/
struct stm_group
{
  const char * const      name;              /* name of the group */
  rex_sigs_type          *sig_list;          /* ptr to array of signals */
  uint32                  num_sigs;          /* number of signals? */
  sig_mapper_fn_type      sig_mapper_f;      /* func which maps signals */
  stm_state_machine_type **sm_list;          /* ptr to array of state machines
                                                ptrs in the group. */
  uint32                  num_sm;            /* number of state machines */
  wait_fn_type            wait_f;            /* wait function */
  rex_tcb_type           *tcb;               /* tcb of task that receives
                                                signals and commands */
  rex_sigs_type           int_cmd_sig;       /* internal command signal */
  stm_int_q_type*         int_cmd_queue;     /* internal command queue */
  stm_int_q_type*         delayed_int_cmd_queue; /* delayed int. cmd queue */
  group_entry_fn_type     entry_f;           /* called when group is started */
  group_exit_fn_type      exit_f;            /* called when group is stopped */
  user_debug_fn_type      user_debug_f;      /* Debug hook function, called upon
                                                input processing events. */
  user_heap_fn_type       user_heap_create_f;/* Called upon stm_start_group()
                                                 to create/initialize the group
                                                 heap for malloc/free usage. */
  user_heap_fn_type       user_heap_destroy_f;/* Called upon stm_stop_group()
                                                 to perform any group heap
                                                 destruction or examination
                                                 (memory leaks, etc.) */
  user_malloc_fn_type     user_malloc_f;     /* Group's malloc() function
                                                associated with group's heap. */
  user_free_fn_type       user_free_f;       /* Group's free() function
                                                associated with group's heap. */
  struct stm_group*       global_group;      /* ptr to the global group */
  user_sigs_fn_type       user_f_sig;        /* signal(s) waited on in the
                                                user supplied function */
  user_sig_handler_fn_type user_f;           /* user supplied function called
                                                inside of stm_driver() */

  /* Private data */
  /* Note: Add new public members before starting of private data.
   * The following members need not be initialized during declaration.
   * C assures that they will be automatically set to 0. Lint does not like
   * it though. */
  boolean                 active;            /* indicates if the group has
                                                been activated or not */
  boolean                 stopped;           /* indicates if the group driver
                                                is stopped or not. */
  boolean                 using_driver;      /* indicates if the stm_driver()
                                                is being used or not. */
};


/*==========================================================================
  STM_INT_CMD_ID_TYPE

  This struct is used by stm_delete_internal_cmds() to specify which
  commands to delete from an internal command queue.  id is the command
  identifier.  sm specifies the state machine the command is for.  If sm is
  NULL then all commands that match id will be deleted.
============================================================================*/
typedef struct
{
  stm_state_machine_type *sm;     /* ptr to state machine */
  uint32                  id;     /* internal command ID */
} stm_int_cmd_id_type;

/*==========================================================================
  STM_MSG_HDR

  This struct is used by the state machine to attach info describing where a
  payload is headed, what state machine it's part of, what it's doing, and how
  much data there is to the actual payload.  In the case of NULL payload
  (not uncommon) we still just send a pointer to NULL, as the payload is
  decalared NOT_USED() in the transition function.

  These fields are populated via stm_alloc_msg_buf().
============================================================================*/
typedef struct
{
  stm_state_machine_type  *sm;          /* state machine command is for */
  stm_group_type          *group;       /* group the state machine is in */
  uint32                   id;          /* internal command ID */
  uint32                   size;        /* size of internal command payload */
  boolean                  checked_out; /* true if the buffer is checked out */
  timetick_type            time;        /* timetick at allocation */
  uint32                   watermark;   /* should always be 0xdeadbeef */
  uint32                   reserved;    /* 64-bit alignment */  
} stm_msg_hdr;



/*===========================================================================
        FUNCTION DECLARATIONS
===========================================================================*/

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
extern void stm_activate
(
  stm_state_machine_type *sm,       /* the state machine */
  stm_group_type         *group,    /* the group the SM is in */
  boolean                 reset     /* reset to initial state? */
);

/*===========================================================================

FUNCTION STM_GROUP_IS_ACTIVE

DESCRIPTION    This function returns the active status of a state machine group

DEPENDENCIES   None

RETURN VALUE   TRUE if the state machine is active, FALSE if not

SIDE EFFECTS   None

===========================================================================*/
extern boolean stm_group_is_active
(
  stm_group_type const *group
);

/*===========================================================================

FUNCTION STM_IS_ACTIVE

DESCRIPTION    This function returns the active status of the state machine

DEPENDENCIES   None

RETURN VALUE   TRUE if the state machine is active, FALSE if not

SIDE EFFECTS   None

===========================================================================*/
extern boolean stm_is_active
(
  stm_state_machine_type const *stm
);

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
extern void stm_deactivate
(
  stm_state_machine_type *sm,       /* the state machine */
  stm_group_type         *group     /* the group the SM is in */
);

/*===========================================================================

FUNCTION STM_START_GROUP

DESCRIPTION    This function starts a group of state machines.

DEPENDENCIES   Group must not already be started

RETURN VALUE   None

SIDE EFFECTS   Active flag in group is set

===========================================================================*/
extern void stm_start_group
(
  stm_group_type *group,           /* group to start */
  boolean         start_driver     /* call stm_driver() on this group? */
);

/*===========================================================================

FUNCTION STM_STOP_GROUP

DESCRIPTION    This function stops a group of state machines

DEPENDENCIES   Group must be started

RETURN VALUE   None

SIDE EFFECTS   Active flag in group is cleared

===========================================================================*/
extern void stm_stop_group
(
  stm_group_type *group     /* group to stop */
);

/*===========================================================================

FUNCTION STM_ALLOC_MSG_BUF

DESCRIPTION    This function allocates a message buffer including header
               information for the state machine driver.

DEPENDENCIES   None

RETURN VALUE   Pointer to the payload field - header data will be in front,
               and clobbering it will have unsavory effects.

SIDE EFFECTS   None

===========================================================================*/
extern void *stm_alloc_msg_buf
(
 stm_state_machine_type *sm,       /* state machine command is for */
 stm_group_type         *group,    /* group that the state machine is in */
 uint32                 id,        /* internal command ID */
 uint32                 size       /* size of internal command payload */
);

/*===========================================================================

FUNCTION STM_COPY_MSG_BUF

DESCRIPTION    This function allocates and copies a given message based on 
               a payload pointer of the message to be copied.

DEPENDENCIES   None

RETURN VALUE   Pointer to the payload field - header data will be in front,
               and clobbering it will have unsavory effects.

SIDE EFFECTS   None

===========================================================================*/
extern void *stm_copy_msg_buf
(
 void                   *payload   /* pointer to payload of msg to be copied */
);

/*===========================================================================

FUNCTION STM_PUT_INTERNAL_CMD_PAYLOAD

DESCRIPTION    This function places an internal command on a circular
               buffer and sets the internal command task signal.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void stm_put_internal_cmd_payload
(
  void                 *payload    /* internal command payload */
);

/*===========================================================================

FUNCTION STM_DELAY_INTERNAL_CMD_PAYLOAD

DESCRIPTION    This function delays an internal command until the next 
               state change (when it gets re-issued).

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void stm_delay_internal_cmd_payload
(
  void                 *payload
);

/*===========================================================================

FUNCTION STM_SET_DEBUG

DESCRIPTION    This function sets or clears the debug_on flag for a
               state machine

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void stm_set_debug
(
  stm_state_machine_type *sm,
  uint32                  mask
);

/*===========================================================================

FUNCTION STM_GET_DEBUG

DESCRIPTION    This function retrieves the debug mask for a state machine

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern uint32 stm_get_debug
(
  stm_state_machine_type const *sm
);

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
extern int stm_get_message_index
(
  stm_state_machine_type const *sm,
  stm_input_type                message_id
);

/*===========================================================================

FUNCTION STM_PROCESS_GROUP

DESCRIPTION    This function processes all inputs for a state machine group.

DEPENDENCIES   None

RETURN VALUE   TRUE if at least one item was processed, FALSE otherwise

SIDE EFFECTS   None

===========================================================================*/
extern boolean stm_process_group
(
  stm_group_type *group         /* state machine group */
);

/*===========================================================================

FUNCTION STM_DELETE_INTERNAL_CMDS

DESCRIPTION    This function deletes messages from the internal message queue
               of the specified state machine group.  The caller provides an
               array of state machine/command ID pairs to be removed.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void stm_delete_internal_cmds
(
  stm_group_type            *group,     /* state machine group */
  stm_int_cmd_id_type const *cmds,      /* the commands to be deleted */
  uint32                     count      /* number of entries in cmds */
);

/*===========================================================================

FUNCTION STM_PRINT_GROUP_SM_STATES

DESCRIPTION    This function prints the diag state of all the state machines
               in a given group.
               
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void stm_print_group_sm_states
(
  stm_group_type const *group
);

#endif  /* STM_H */
