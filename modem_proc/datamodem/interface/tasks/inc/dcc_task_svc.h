#ifndef DCC_TASK_SVC_H
#define DCC_TASK_SVC_H
/*===========================================================================

                        D C C _ T A S K _ S V C . H

DESCRIPTION

  Data Common Control Task services header file. All definitions, structures,
  and functions needed for performing DCC task specific services.
  
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

  dcc_get_nv_item()
    Retrive NV item in DCC task.

  dcc_put_nv_item
    Write to NV in DCC task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the DCC services can be used unless MC has created the DCC task
  and dcc_task() has been invoked.

Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/dcc_task_svc.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/11   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/15/10   ss      Added new APIs to access NV items from multiple SIMs.
09/24/10   pp      API dcc_send_cmd_ex introduced.
02/19/09   am      Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#ifdef __cplusplus
extern "C"
{
#endif

#ifndef FEATURE_WINCE
#include "rex.h"
#endif /* FEATURE_WINCE */
#include "nv.h"
#include "dcc_task_defs.h"

/*===========================================================================

                      EXTERN DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  NV item and status.
---------------------------------------------------------------------------*/
extern nv_item_type         dcc_nv_item;
extern nv_stat_enum_type    dcc_nv_status;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DCC_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the DCC task uses for rex_wait().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
dcc_enable_sig
(
  dcc_sig_enum_type sig                       /* Signal to be enabled      */
);

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
);

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
);

#if 1 /* TODO: until MHs have their own ps_mem bufs */

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
dcc_cmd_data_buf_type * dcc_get_cmd_data_buf
(
  void
);

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
);

#endif /* TODO: ... */

/*===========================================================================

FUNCTION DCC_SEND_CMD_EX()

DESCRIPTION
  This function posts a cmd for processing in DCC task context.  The cmd is
  processed by calling the registered cmd handler, if any.

  This API is mainly for clients which allocates  user_data_ptr from their
  own memory [not by dcc_get_cmd_buf]. Henceforth, if the client uses 
  this API, it must free the memory after processing the command.

  Eventually all clients MUST move to user this API.

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
);

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
  dcc_cmd_enum_type cmd,             /* Command name                      */
  dcc_cmd_handler_type cmd_handler   /* Cmd handler function pointer      */
);


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
);

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
  rex_sigs_type requested_mask              /* mask of signals to wait for */
);

#endif /* FEATURE_REX_SIGS_EXT */


/*===========================================================================

FUNCTION DCC_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged
  if status is other than:
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
);


/*===========================================================================
FUNCTION DCC_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.

DEPENDENCIES
  This function can only be called from DCC task.  Also it is not
  reentrant. Calls rex_wait() and doesn't exit until processing is finished.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged
  if status is other than:
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
);

/*===========================================================================

FUNCTION DCC_GET_NV_ITEM_EXT

DESCRIPTION
  This function retrieves the specified item from NV for the specified SIM.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged
  if status is other than:
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
);


/*===========================================================================
FUNCTION DCC_PUT_NV_ITEM_EXT

DESCRIPTION
  Write an item to NV memory of the specified SIM.  Wait until write is 
  completed.

DEPENDENCIES
  This function can only be called from DCC task.  Also it is not
  reentrant. Calls rex_wait() and doesn't exit until processing is finished.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged
  if status is other than:
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
);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DCC_TASK_SVC_H */
