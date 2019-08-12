/*===========================================================================

                           S T A T E   M A C H I N E   M A N A G E R

                                        C   F I L E


DESCRIPTION
  This file contains data and function definitions for STATE MACHINE MANAGER
  implementation.    

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/sm_mgr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/11   sn      Globals cleanup for free floating of tasks.
04/10/09   sch     Created SM MGR header file
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "data_msg.h"
#include "err.h"
#include "assert.h"
#include "sm_mgr.h"
#include "modem_mem.h"
#include "amssassert.h"

/*===========================================================================

                      PUBLIC DATA DEFINITIONS

===========================================================================*/

/* state machine instances table implementation. */
static sm_inst_tbl_type sm_inst_tbl[SM_INST_MAX];

/* Pointers to entryexit cum transition tables of 
   different types of state machines */
static sm_entryexit_fns_type *entryexit_trans_tbl[SM_TYPE_MAX] = {NULL};

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION sm_powerup_init

DESCRIPTION
  This primitive is used to initialize state m/c params. 
  This is called during UE powerup.

PARAMETERS


DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void sm_powerup_init( void )
{
  uint8  i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Initialize sm_inst_tbl[] */
  for(i=0; i<SM_INST_MAX; i++)
  {
    sm_inst_tbl[i].valid = FALSE;
    sm_inst_tbl[i].cb_data = SM_INVALID_CB_DATA;
    sm_inst_tbl[i].type = SM_TYPE_MAX;
    sm_inst_tbl[i].sm_fn_tbl = NULL;
    sm_inst_tbl[i].state = SM_STATE_DOWN;
  }

  return;
} /* sm_powerup_init */

/*===========================================================================
FUNCTION sm_get_handle

DESCRIPTION
  This function is called to create a state machine instance 
  and return a handle to it. 

PARAMETERS
  type        - this is an enum value for type of state machine
                   i.e.(compression or csohs)
  max_states  - maximum states for the given state machine type
  cb_data        - this is the argument that entry and exit functions for 
                   this state machine will take. 
  handle         - handle value is returned here. This will be used to 
                   to refer to this state machine instance in subsequent
                   function calls.
DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if success, FALSE if error

SIDE EFFECTS
  None

===========================================================================*/
boolean sm_get_handle
(
  sm_type          type,
  uint32           max_states,
  uint32           cb_data,
  sm_handle_type  *handle
)
{
  sm_entryexit_fns_type  *sm_fn_tbl_ptr = NULL;
  uint8                   i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(!VALIDATE_SM_TYPE(type))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Handle Requested for Invalid SM Type : %d", type);
    return FALSE;   
  }

  /* Allocate and Initialize the entryexit_trans_tbl[] */
  entryexit_trans_tbl[type] = (sm_entryexit_fns_type*)
          modem_mem_alloc(sizeof(sm_entryexit_fns_type) * max_states,
                           MODEM_MEM_CLIENT_DATA);

  if(entryexit_trans_tbl[type] == NULL)
  {
    ASSERT(0); 
    return (FALSE);
  }

  else
  {
    memset(entryexit_trans_tbl[type], 0, sizeof(sm_entryexit_fns_type));

    sm_fn_tbl_ptr = entryexit_trans_tbl[type];
 
    for(i = 0; i<SM_INST_MAX; i++)
    {
      if(sm_inst_tbl[i].valid == FALSE)
      {
        sm_inst_tbl[i].valid = TRUE;
        sm_inst_tbl[i].sm_fn_tbl = sm_fn_tbl_ptr;
        sm_inst_tbl[i].type = type;
        sm_inst_tbl[i].state = SM_STATE_DOWN;
        sm_inst_tbl[i].cb_data = cb_data;      
        *handle = i;
        return TRUE;
      }
    }
    DATA_MSG0(MSG_LEGACY_LOW,
              "Cannot allocate a SM handle since max nbr of handles already "
              "allocated");
    return FALSE;
  }
} /* sm_get_handle */

/*===========================================================================
FUNCTION sm_set_state_hdlr

DESCRIPTION
  This function sets up the entry, exit events and cmd handlers for a given 
  state of the State machine. 

PARAMETERS
  type                 - Type of State Machine
  state                - this is the state for which event handlers are
                         being set up. 
  trans_tbl            - this is a pointer to the table of valid transitions
                         that can happen in this state.
  num_entries_trans_tbl- length of trans_tbl
  entry_func_ptr       - this is pointer to fn called upon entry to the state
  exit_func_ptr        - this is pointer to fn called upon exit from the state
  cmd_handler          - pointer to fn to handle cmds

DEPENDENCIES
  This function expects the client does state validity check before calling 
  this API.

RETURN VALUE
  void 

SIDE EFFECTS
  None

===========================================================================*/
void sm_set_state_hdlr
(
  sm_type                     type,
  uint32                      state,
  sm_event_state_trans_type  *trans_tbl,
  uint32                      num_entries_trans_tbl,
  sm_func_ptr_type            entry_func_ptr,
  sm_func_ptr_type            exit_func_ptr,
  sm_cmd_hdlr_ptr_type        cmd_handler
)
{
  uint8  i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(!VALIDATE_SM_TYPE(type))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Handle Requested for Invalid SM Type :%d", type);
    return;   
  }
  if(entryexit_trans_tbl[type] == NULL)
  {
    ASSERT(0);
  }
  entryexit_trans_tbl[type][state].entry_func_ptr = entry_func_ptr;
  entryexit_trans_tbl[type][state].exit_func_ptr  = exit_func_ptr;
  entryexit_trans_tbl[type][state].cmd_handler = cmd_handler;
  entryexit_trans_tbl[type][state].num_entries_trans_tbl = num_entries_trans_tbl;

  entryexit_trans_tbl[type][state].trans_tbl = (sm_event_state_trans_type*)
      modem_mem_alloc(sizeof(sm_event_state_trans_type)* num_entries_trans_tbl,
                      MODEM_MEM_CLIENT_DATA);
  if(entryexit_trans_tbl[type][state].trans_tbl == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Can not allocate trans_tbl for the given state");
    ASSERT(0); 
    return;           
  }
  for (i = 0; i < num_entries_trans_tbl; i++)
  {    
    entryexit_trans_tbl[type][state].trans_tbl[i] = trans_tbl[i];       
  }
  return;
} /* sm_set_state_hdlr */

void sm_reset_state_hdlr
(
  uint32  type,
  uint32  state
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(!VALIDATE_SM_TYPE(type))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Handle Requested for Invalid SM Type : %d", type);
    return;   
  }
  if(entryexit_trans_tbl[type] == NULL)
  {
    ASSERT(0);
  }
  entryexit_trans_tbl[type][state].entry_func_ptr = NULL;
  entryexit_trans_tbl[type][state].exit_func_ptr  = NULL;
  entryexit_trans_tbl[type][state].cmd_handler = NULL;
  entryexit_trans_tbl[type][state].num_entries_trans_tbl = 0;

  modem_mem_free(entryexit_trans_tbl[type][state].trans_tbl,
                 MODEM_MEM_CLIENT_DATA);
  
  return;
} /* sm_reset_state_hdlr */

/*===========================================================================
FUNCTION sm_process_event

DESCRIPTION
  This function is called to process an event posted to a state machine 
  instance. 

PARAMETERS
  handle              - this is handle of instance of state machine to which 
                        event was posted. 
  event               - the event that needs to be processed.
  cur_state           - the state in which this state m/c is at present.
  next_state          - the state in which state m/c will transition.
  ev_data             - this is event data 
DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static void sm_process_event
(
  sm_handle_type  handle,
  uint32          event,
  uint32          cur_state,
  uint32          next_state,
  void           *ev_data
)
{
  sm_entryexit_fns_type  *entryexit_fns_tbl = sm_inst_tbl[handle].sm_fn_tbl;
  uint32                  cb_data = sm_inst_tbl[handle].cb_data;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(next_state != SM_STATE_INVALID)
  {
    if(cur_state != next_state)
    {
      DATA_MSG1(MSG_LEGACY_LOW, "Calling exit_func for curr_state : %d",cur_state);
      if(entryexit_fns_tbl[cur_state].exit_func_ptr != NULL)
      {
        entryexit_fns_tbl[cur_state].exit_func_ptr(cb_data, ev_data, event);
      }
      else
      {
        sm_default_handler(SM_INVALID_CB_DATA, NULL);
      }

      DATA_MSG2(MSG_LEGACY_LOW,"Transitioning from curr_state: %d to next_state: %d",
                cur_state, next_state);
      sm_inst_tbl[handle].state = next_state;

      DATA_MSG1(MSG_LEGACY_LOW,"Calling entry_func for next_state : %d",next_state);
      if(entryexit_fns_tbl[next_state].entry_func_ptr != NULL)
      {
        entryexit_fns_tbl[next_state].entry_func_ptr(cb_data, ev_data, event); 
      }
      else
      {
        sm_default_handler(SM_INVALID_CB_DATA, NULL);
      }
    }
    else
    {
      if(entryexit_fns_tbl[cur_state].cmd_handler != NULL)
      {
        entryexit_fns_tbl[cur_state].cmd_handler(cb_data, event, ev_data); 
      }
      else
      {
        sm_default_handler(SM_INVALID_CB_DATA, NULL);
      }
    }
  }
} /* sm_process_event */

/*===========================================================================
FUNCTION sm_post_event

DESCRIPTION
  This function posts the event to the  state machine.  

PARAMETERS
  sm_handle_type - this is handle to the state machine for which  
                        state event handlers are being set up.
  event               - the event that needs to be posted. 
  ev_data             - data needed to process event  
 
DEPENDENCIES
  This function assumes that the event posted is a valid event.

RETURN VALUE
  void 

SIDE EFFECTS
  None

===========================================================================*/
void sm_post_event
(
  sm_handle_type  handle,
  uint32          event,
  void           *ev_data
)
{
  uint32                      cur_state;
  uint32                      next_state = SM_STATE_INVALID;
  sm_entryexit_fns_type      *sm_entryexit_fns_tbl;
  sm_event_state_trans_type  *trans_tbl;
  uint32                      i, num_entries_trans_tbl;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(handle >= SM_INST_MAX)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "Invalid  State Machine Handle");
    return;
  }
  if(sm_inst_tbl[handle].valid == FALSE)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "Event posted to Invalid SM Instance, Ignoring");
    return;   
  }
  cur_state = sm_inst_tbl[handle].state;
  sm_entryexit_fns_tbl = sm_inst_tbl[handle].sm_fn_tbl;  
  if((trans_tbl = sm_entryexit_fns_tbl[cur_state].trans_tbl) == NULL)
  {
    DATA_MSG1(MSG_LEGACY_LOW," Transition Table for this state : %d is NULL ",
              cur_state);
    ASSERT(0);
  }  
  num_entries_trans_tbl = sm_entryexit_fns_tbl[cur_state].num_entries_trans_tbl;
  for(i= 0; i < num_entries_trans_tbl; i++)
  { 
    if(trans_tbl->event == event)
    {
      next_state = trans_tbl->next_state;
      break;
    }
    trans_tbl++;
  }
  sm_process_event(handle, event, cur_state, next_state, ev_data);
} /* sm_post_event */

/*===========================================================================
FUNCTION sm_release_handle

DESCRIPTION
  This function is called to free State machine instance 
  and release its handle. 

PARAMETERS
  sm_handle_type - the handle that was used to refer to 
                   the given instance of state machine   

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void sm_release_handle
(
  sm_handle_type  handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(handle >= SM_INST_MAX)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "Invalid State Machine Handle");
    return;
  }

  modem_mem_free(entryexit_trans_tbl[sm_inst_tbl[handle].type], 
                 MODEM_MEM_CLIENT_DATA);

  /* reset sm_inst_tbl[handle] */
  sm_inst_tbl[handle].valid = FALSE;
  sm_inst_tbl[handle].cb_data = SM_INVALID_CB_DATA;
  sm_inst_tbl[handle].type = SM_TYPE_MAX;
  sm_inst_tbl[handle].sm_fn_tbl = NULL;
  sm_inst_tbl[handle].state = SM_STATE_DOWN;
} /* sm_release_handle */

/*===========================================================================
FUNCTION sm_default_handler

DESCRIPTION
  This function is a default handler function, when any of entry, exit, or 
  command handler function are NULL.

PARAMETERS
  cb_data 
  ev_data

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void sm_default_handler
(
  uint32  cb_data,
  void   *ev_data
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG0(MSG_LEGACY_LOW, "sm_default_handler");
} /* sm_default_handler */

/*===========================================================================
FUNCTION sm_set_cb_data

DESCRIPTION
  This function is used to set the cb data information for a particular
  state machine instance.

PARAMETERS
  SM Instance Handle
  Cb Data

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void sm_set_cb_data
(
  sm_handle_type  handle,
  uint32          cb_data
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  sm_inst_tbl[handle].cb_data = cb_data;
} /* sm_set_cb_data */

uint32 sm_get_current_state
(
  sm_handle_type  handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return sm_inst_tbl[handle].state;
} /* sm_get_current_state */
