#ifndef SMMGR_H
#define SMMGR_H
/*===========================================================================

                           S T A T E   M A C H I N E   M A N A G E R

                           H E A D E R   F I L E


DESCRIPTION
  This file contains data declarations and function prototypes for 
  STATE MACHINE MANAGER implementation.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010-2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/sm_mgr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

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
#include "sys.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define SM_INVALID_CB_DATA    0xFFFF
#define SM_STATE_INVALID      0xFF
#define SM_STATE_DOWN         0

typedef enum
{
  SM_TYPE_RMSM_PROXY = 0,
  SM_TYPE_MAX
} sm_type;

#define VALIDATE_SM_TYPE(type) \
                        ((type < SM_TYPE_MAX)?TRUE:FALSE)

#define SM_RMSM_PROXY_INST 1

#define SM_INST_MAX SM_RMSM_PROXY_INST

/* sm_handle_type will be used to index into sm_inst_tbl to retrieve 
   current state of state m/c and pointer to entry&exit function table*/
typedef uint32 sm_handle_type;

/* Generic function pointer for Entry and Exit functions for a state. 
   first argument is the callback data, second is event data, third is the
   actual event */
typedef void (* sm_func_ptr_type) (uint32, void *, uint32); 

/* Generic function pointer for command handler functions for a state 
   first argument is the callback data and second is event, third is event data */ 
typedef void (* sm_cmd_hdlr_ptr_type) (uint32, uint32, void *); 

/* An element in Event-State Transition Table */
typedef struct {
  uint32 event;
  uint32 next_state;
} sm_event_state_trans_type;

/* An element in EntryExit Functions Table */
typedef struct {
  sm_func_ptr_type entry_func_ptr;
  sm_func_ptr_type exit_func_ptr;
  sm_cmd_hdlr_ptr_type cmd_handler;
  uint32 num_entries_trans_tbl;
  sm_event_state_trans_type *trans_tbl;
} sm_entryexit_fns_type;

/* A record in state machine instances table. This contains current 
   state of state m/c and (void*) pointer to Entry And Exit Functions Table */
typedef struct {
  boolean valid;
  uint32 state;
  uint32 cb_data; /* This will keep in track cb_data for corresponding SM handle */
  sm_type type;
  sm_entryexit_fns_type *sm_fn_tbl; 
} sm_inst_tbl_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION sm_powerup_init

DESCRIPTION
  This primitive is used by the client to initialize state m/c params. 
  This is called during UE powerup while its client is being 
  powerup-initialized.

PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void sm_powerup_init 
(
  void
);

/*===========================================================================
FUNCTION sm_get_handle

DESCRIPTION
  This function is called to create a state machine instance 
  and return a handle to it. 

PARAMETERS
  sm             - this is an enum value for type of state machine
                   i.e.(compression or csohs)
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
  sm_type type, 
  uint32 max_states,
  uint32 cb_data,
  sm_handle_type *handle
);

/*===========================================================================
FUNCTION sm_set_state_hdlr

DESCRIPTION
  This function sets up the entry, exit events and cmd handlers for a given 
  state of the State machine. 

PARAMETERS
  type              - Type of State Machine
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
  sm_type type, 
  uint32 state, 
  sm_event_state_trans_type *trans_tbl,
  uint32 num_entries_trans_tbl,
  sm_func_ptr_type entry_func_ptr,
  sm_func_ptr_type exit_func_ptr,
  sm_cmd_hdlr_ptr_type cmd_handler
);

void sm_reset_state_hdlr
(
  uint32 type, 
  uint32 state
);

/*===========================================================================
FUNCTION sm_post_event

DESCRIPTION
  This function posts the event to State machine.  

PARAMETERS
  sm_handle_type - this is handle to the state machine for which  
                        event is being posted
  event               - this is the event that is causing an action to 
                        be taken in state machine
  ev_data             - pointer to event data 

DEPENDENCIES
  None

RETURN VALUE
  void 

SIDE EFFECTS
  None

===========================================================================*/
void sm_post_event
(
  sm_handle_type handle,
  uint32 event,
  void *ev_data
);

/*===========================================================================
FUNCTION sm_release_handle

DESCRIPTION
  This function is called to free State machine instance 
  and release its handle. 

PARAMETERS
  sm_handle_type - this is handle that was used to refer 
                        to the given instance of state machine   

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void sm_release_handle
(
  sm_handle_type handle
);

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
  uint32 cb_data, 
  void *ev_data
);

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
  sm_handle_type handle,
  uint32         cb_data
);

uint32 sm_get_current_state
(
  sm_handle_type handle
);

#endif /*SMMGR_H*/
