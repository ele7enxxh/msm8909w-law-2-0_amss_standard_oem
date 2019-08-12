/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   C O M M A N D   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA functions which handles entry commands.

                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/15   tkl     Moved request_id up to callback function
03/27/15   tl      Add support for GBA cancel
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "queue.h"

#include "gba.h"
#include "gba_bootstrap.h"
#include "gba_cmd.h"
#include "gba_lib.h"
#include "gba_platform.h"
#include "gba_response.h"
#include "gba_state.h"
#include "gba_timer.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
static q_type                             gba_cmd_q;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_CMD_COMPARE_REQUEST_ID

DESCRIPTION
  Compares the request IDs values.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  It returns nonzero if the element is to be operated on
  otherwise, it returns 0.

SIDE EFFECTS
  None
===========================================================================*/
static int gba_cmd_compare_request_id (
  void * item_ptr,
  void * compare_val
)
{
  gba_cmd_type   * task_cmd_ptr   = (gba_cmd_type*)item_ptr;
  uint32         * request_id_ptr = (uint32*)compare_val;

  if (item_ptr == NULL ||
      compare_val == NULL)
  {
    return 0;
  }

  if (task_cmd_ptr->request_id == *request_id_ptr)
  {
    return 1;
  }

  return 0;
} /* gba_cmd_compare_request_id */


/*===========================================================================
FUNCTION GBA_CMD_SET_SIG

DESCRIPTION
  This function sets the GBA command signal.

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_cmd_set_sig (
  void
)
{
  if (q_cnt(&gba_cmd_q) > 0 &&
      gba_state_current() == GBA_STATE_IDLE)
  {
    /*  Items on the queue. Set the signal */
    (void) rex_set_sigs(rex_self(), GBA_CMD_SIG);
  }
} /* gba_cmd_set_sig */


/*===========================================================================
FUNCTION GBA_CMD_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in gba_main, is called to process the GBA command

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_cmd_handle_cmd_sig (
  void
)
{
  gba_cmd_type                   * task_cmd_ptr  = NULL;
  gba_result_enum_type             gba_status    = GBA_SUCCESS;

  (void) rex_clr_sigs(rex_self(), GBA_CMD_SIG);

  /* If GBA is not in a valid state to handle a new command then
     leave command in queue and continue processing. */
  if (gba_state_current() != GBA_STATE_IDLE)
  {
    return;
  }

  task_cmd_ptr = (gba_cmd_type*)(q_get(&gba_cmd_q));

  /* If there is a command on either of the queues */
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  GBA_MSG_HIGH_0("GBA_CMD_SIG received");

  /* Start overall sanity timer to make sure command does not get stuck */
  gba_set_timer();

  do
  {
    gba_status = gba_response_save_client_data(task_cmd_ptr->gba_cb_ptr,
                                               task_cmd_ptr->user_data_ptr);
    if (gba_status != GBA_SUCCESS)
    {
      break;
    }

    gba_status = gba_bootstrap_process_external_cmd(&task_cmd_ptr->gba_bootstrap,
                                                    task_cmd_ptr->request_id);
  } while(0);

  GBA_FREE(task_cmd_ptr);

  if (gba_status != GBA_SUCCESS)
  {
    gba_send_response(gba_status,
                      NULL);

    gba_reset();

    /* Set the signal if there are one or more commands still in queue
        and GBA is currently in IDLE state. */
    gba_cmd_set_sig();
  }
} /* gba_cmd_handle_cmd_sig */


/*===========================================================================
   FUNCTION:      GBA_QUEUE_CMD

   DESCRIPTION:
     This function allows client to queue a GBA command to be processed
     in GBA task context.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type  gba_queue_cmd (
  gba_cmd_type         * task_cmd_ptr
)
{
  gba_result_enum_type  gba_status  = GBA_SUCCESS;

  if (task_cmd_ptr == NULL )
  {
    GBA_MSG_ERR_0("Attempt to use NULL POINTER");
    return GBA_INCORRECT_PARAMS;
  }

  GBA_ENTER_TASK_STATE_CRIT_SECT;

  if (!gba_task_stopped)
  {
    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

    /* Put the message on the queue */
    q_put(&gba_cmd_q, &task_cmd_ptr->link);

    /* Set the command queue signal */
    (void) rex_set_sigs(UIM_GBA_TCB, GBA_CMD_SIG);
  }
  else
  {
    GBA_MSG_ERR_0("GBA task stopped, cannot queue command");
    gba_status = GBA_GENERIC_ERROR;
  }

  GBA_LEAVE_TASK_STATE_CRIT_SECT;
  return gba_status;
} /* gba_queue_cmd */


/*===========================================================================
FUNCTION GBA_CMD_CLEANUP

DESCRIPTION
  This function flushes the gba cmd queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_cmd_cleanup (
  void
)
{
  gba_cmd_type  * task_cmd_ptr = NULL;

  while (q_cnt(&gba_cmd_q) != 0)
  {
    task_cmd_ptr = (gba_cmd_type*)(q_get(&gba_cmd_q));

    if (task_cmd_ptr != NULL)
    {
      if (task_cmd_ptr->gba_cb_ptr != NULL)
      {
        GBA_MSG_LOW_0("Notify error to client");
        task_cmd_ptr->gba_cb_ptr(GBA_GENERIC_ERROR,
                                 task_cmd_ptr->request_id,
                                 NULL,
                                 task_cmd_ptr->user_data_ptr);
      }

      GBA_FREE(task_cmd_ptr);
    }
  }
} /* gba_cmd_cleanup */


/*===========================================================================
FUNCTION GBA_CMD_INIT

DESCRIPTION
  This function, called in gba_main, is called to initialize the command
  module.

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_cmd_init (
  void
)
{
  (void) q_init(&gba_cmd_q);
} /* gba_cmd_init */


/*===========================================================================
FUNCTION GBA_CMD_CANCEL

DESCRIPTION
  Command cancels a command currently running in the queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_cmd_cancel (
  uint32        request_id
)
{
  gba_cmd_type   * task_cmd_ptr   = NULL;

  task_cmd_ptr = q_linear_search(&gba_cmd_q,
                                 gba_cmd_compare_request_id,
                                 &request_id);

  if (task_cmd_ptr != NULL)
  {
    if (task_cmd_ptr->gba_cb_ptr)
    {
      task_cmd_ptr->gba_cb_ptr(GBA_CANCELLED,
                               request_id,
                               NULL,
                               task_cmd_ptr->user_data_ptr);
    }

    (void)q_delete_ext(&gba_cmd_q,
                       (void*)task_cmd_ptr);

    return GBA_SUCCESS;
  }

  return GBA_GENERIC_ERROR;
} /* gba_cmd_cancel */
