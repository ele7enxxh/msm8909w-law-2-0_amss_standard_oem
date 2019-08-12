/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   S F S   R E M O T E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock remote functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS 


                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_sfs_remote.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/10/14   vv      Add support for setting emergency mode for all the RATs
10/16/14   vv      Fix a merge error
10/10/14   vv      Add support for remote simlock configuration
09/29/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_sfs_remote.h"
#include "simlock.h"
#include "simlock_modem.h"
#include "simlock_modem_p.h"
#include "simlock_util.h"

#include "queue.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* SFS remote response queue */
static q_type   simlock_sfs_remote_rsp_q;

/* callback for remote SFS requests */
static simlock_remote_sfs_request_cb_type     simlock_remote_sfs_req_callback = NULL;

#ifndef FEATURE_QMI_UIM_SECURE_CHANNEL_TEST
/* sanity timer (in ms) for response from QMI */
#define SIMLOCK_SANITY_TIMER_VALUE    5000
#else
#error code not present
#endif /* FEATURE_QMI_UIM_SECURE_CHANNEL_TEST */

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_SFS_REMOTE_OPERATION_RSP_TYPE

   DESCRIPTION:
     The structure contains response data for a remote operation request
-------------------------------------------------------------------------------*/
typedef struct{
  simlock_result_enum_type          status;
  simlock_token_id_type             response_token;
  simlock_data_type                 simlock_data;
} simlock_sfs_remote_operation_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_SFS_REMOTE_RSP_TYPE

   DESCRIPTION:
     The response header for response received by SIMLOCK task
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                                  link;
  simlock_sfs_remote_operation_rsp_type        remote_operation_rsp;
} simlock_sfs_remote_rsp_type;


/*===========================================================================

                        FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_REGISTER_REMOTE_CB

DESCRIPTION
  This function registers the callback used in remote SFS operations

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_sfs_remote_register_remote_cb
(
  simlock_remote_sfs_request_cb_type     remote_cb
)
{
  if(remote_cb == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(simlock_remote_sfs_req_callback != NULL)
  {
    SIMLOCK_MSG_ERR_0("Client already registered for remote SFS operations");
    return SIMLOCK_INVALID_STATE;
  }

  simlock_remote_sfs_req_callback = remote_cb;

  return SIMLOCK_SUCCESS;
} /* simlock_sfs_remote_register_remote_cb */


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_DEREGISTER_REMOTE_CB

DESCRIPTION
  This function deregisters the callback used in remote SFS operations

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_sfs_remote_deregister_remote_cb
(
  void
)
{
  simlock_remote_sfs_req_callback = NULL;

  return SIMLOCK_SUCCESS;
} /* simlock_sfs_remote_deregister_remote_cb */


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_RESPONSE_Q_INIT

DESCRIPTION
  This function intializes the SFS remote response queue, if it is not already
  initialized. It updates a global value.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void simlock_sfs_remote_response_q_init
(
  void
)
{
  /* indicates if response queue is initialized */
  static boolean simlock_sfs_is_remote_rsp_q_init = FALSE;

  if(simlock_sfs_is_remote_rsp_q_init)
  {
    return;
  }

  (void)q_init(&simlock_sfs_remote_rsp_q);
  simlock_sfs_is_remote_rsp_q_init = TRUE;
} /* simlock_sfs_remote_response_q_init */


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_QUEUE_RSP

DESCRIPTION
  This function puts the remote SFS operation response into the SIMLOCK remote
  response queue

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static simlock_result_enum_type simlock_sfs_remote_queue_rsp(
  simlock_sfs_remote_rsp_type   * task_rsp_ptr
)
{
  simlock_result_enum_type result = SIMLOCK_SUCCESS;

  if (task_rsp_ptr == NULL )
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_ENTER_TASK_STATE_CRIT_SECT;

  /* Do not queue response if SIMLOCK task has been stopped */
  if(!simlock_task_stopped)
  {
    (void)q_link(task_rsp_ptr, &task_rsp_ptr->link);

    /* Put the message on the queue */
    q_put(&simlock_sfs_remote_rsp_q, &task_rsp_ptr->link);

    /* Set the command queue signal */
    (void)rex_set_sigs(UIM_SIMLOCK_TCB, SIMLOCK_REMOTE_SFS_RESPONSE_SIG);
  }
  else
  {
    UIM_MSG_ERR_0("SIMLOCK task stopped, cannot queue response");
    result = SIMLOCK_GENERIC_ERROR;
  }

  SIMLOCK_LEAVE_TASK_STATE_CRIT_SECT;
  return result;
} /* simlock_sfs_remote_queue_rsp */


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_OPERATION_RESPONSE_CB

DESCRIPTION
  The function is a callback, which builds the remote operation response
  to the simlock task. The callback is provided in the request for a
  read or write operation on the simlock data.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void simlock_sfs_remote_operation_response_cb
(
  simlock_result_enum_type          status,
  simlock_token_id_type             token,
  const simlock_data_type         * simlock_data_ptr
)
{
  simlock_result_enum_type           simlock_status = SIMLOCK_SUCCESS;
  simlock_sfs_remote_rsp_type     *  task_rsp_ptr   = NULL;

  SIMLOCK_CHECK_AND_MEM_MALLOC(task_rsp_ptr,
                               sizeof(simlock_sfs_remote_rsp_type));
  if (task_rsp_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("Failed to allocate request structure");
    return;
  }

  SIMLOCK_MSG_MED_0("simlock_sfs_remote_operation_response_cb");

  memset(task_rsp_ptr, 0x00, sizeof(simlock_sfs_remote_rsp_type));

  task_rsp_ptr->remote_operation_rsp.status                 = status;
  task_rsp_ptr->remote_operation_rsp.response_token         = token;

  /* It can be NULL for a write operation */
  if(simlock_data_ptr != NULL)
  {
    task_rsp_ptr->remote_operation_rsp.simlock_data.data_len  = simlock_data_ptr->data_len;

    if((simlock_data_ptr->data_len > 0) &&
       (simlock_data_ptr->data_ptr != NULL))
    {
      simlock_util_deep_copy(
                       (void **)&task_rsp_ptr->remote_operation_rsp.simlock_data.data_ptr,
                       (void *)simlock_data_ptr->data_ptr,
                       simlock_data_ptr->data_len);
    }
  }

  /* Put on SIM Lock command queue */
  simlock_status = simlock_sfs_remote_queue_rsp(task_rsp_ptr);
  if (simlock_status != SIMLOCK_SUCCESS)
  {
    /* Free the pointer since the task_rsp_ptr has not been put
       onto the response queue */
    SIMLOCK_MEM_FREE(task_rsp_ptr->remote_operation_rsp.simlock_data.data_ptr);
    SIMLOCK_MEM_FREE(task_rsp_ptr);
  }

  SIMLOCK_MSG_MED_1("queue of remote operation response, status 0x%x",
                    simlock_status);
} /* simlock_sfs_remote_operation_response_cb */


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_PROCESS_RESPONSE

DESCRIPTION
  This function checks if the response token received matches the request
  token. If it matches, for a read operation request, then it allocates memory
  and copies the data received. Caller is responsible to de-allocate the
  memory. If the response token doesn't match the request toekn, it returns
  an error.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static simlock_result_enum_type simlock_sfs_remote_process_response
(
  simlock_sfs_remote_operation_rsp_type       * resp_ptr,
  simlock_file_operation_enum_type              operation_type,
  simlock_token_id_type                         request_token,
  boolean                                     * token_match_ptr,
  simlock_data_type                           * out_data_ptr
)
{
  if((resp_ptr == NULL) ||
     (out_data_ptr == NULL) ||
     (token_match_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if the tokens doesn't match return error */
  if(request_token != resp_ptr->response_token)
  {
    *token_match_ptr = FALSE;
    SIMLOCK_MSG_ERR_2("token mis-match. Request token: 0x%x, response token: 0x%x",
                      request_token, resp_ptr->response_token);
    return SIMLOCK_GENERIC_ERROR;
  }

  *token_match_ptr = TRUE;

  if(resp_ptr->status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("error in response, status: 0x%x", resp_ptr->status);
    return SIMLOCK_GENERIC_ERROR;
  }

  switch(operation_type)
  {
    case SIMLOCK_FILE_OPERATION_READ:
      /* copy data if read */
      if((resp_ptr->simlock_data.data_len > 0) &&
         (resp_ptr->simlock_data.data_ptr != NULL))
      {
        out_data_ptr->data_len = resp_ptr->simlock_data.data_len;
        out_data_ptr->data_ptr = resp_ptr->simlock_data.data_ptr;

        /* this memory is passed on to data_ptr, which
         is freed by the caller of the sync function */
        resp_ptr->simlock_data.data_ptr = NULL;
      }
      else
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      break;

    case SIMLOCK_FILE_OPERATION_WRITE:
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_sfs_remote_process_response */


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_PROCESS_RSP_SIG

DESCRIPTION
  This function processes the response signal set for the remote command

DEPENDENCIES
  SIMLOCK task is in wait state

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_sfs_remote_process_rsp_sig (
  simlock_file_operation_enum_type   operation_type,
  simlock_token_id_type              request_token,
  boolean                          * token_match_ptr,
  simlock_data_type                * data_ptr
)
{
  simlock_sfs_remote_rsp_type *   simlock_rsp_ptr  = NULL;
  simlock_result_enum_type        simlock_status   = SIMLOCK_GENERIC_ERROR;

  if((data_ptr == NULL) ||
     (token_match_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_rsp_ptr = (simlock_sfs_remote_rsp_type*)(q_get(&simlock_sfs_remote_rsp_q));

  /* If there is a command on the response queue */
  if(simlock_rsp_ptr != NULL)
  {
    UIM_MSG_HIGH_0("SIMLOCK_REMOTE_SFS_RESPONSE_SIG received");

    simlock_status = simlock_sfs_remote_process_response(&simlock_rsp_ptr->remote_operation_rsp,
                                                         operation_type,
                                                         request_token,
                                                         token_match_ptr,
                                                         data_ptr);

    SIMLOCK_MEM_FREE(simlock_rsp_ptr->remote_operation_rsp.simlock_data.data_ptr);
    SIMLOCK_MEM_FREE(simlock_rsp_ptr);
  }

  (void)rex_clr_sigs(rex_self(), SIMLOCK_REMOTE_SFS_RESPONSE_SIG);

  /* in case there is a token mis-match, check if there
     are more responses in the queue */
  if(*token_match_ptr == FALSE)
  {
    /* Set the signal if there are one or more responses to look at */
    if (q_cnt(&simlock_sfs_remote_rsp_q) > 0)
    {
      /*  Items on the queue. Set the signal */
      (void)rex_set_sigs(rex_self(), SIMLOCK_REMOTE_SFS_RESPONSE_SIG);
    }
  }

  return simlock_status;
} /* simlock_sfs_remote_process_rsp_sig */


/*===========================================================================
FUNCTION SIMLOCK_MODEM_INVOKE_REMOTE_CALLBACK

DESCRIPTION
  This function invokes the callback registered to perform the remote
  read/write operations

DEPENDENCIES
  QMI UIM must have registered for simlock remote cb

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_modem_invoke_remote_callback
(
  simlock_file_operation_enum_type         operation_type,
  simlock_file_type                        file,
  simlock_token_id_type                    token,
  simlock_data_type                      * data_ptr,
  simlock_remote_sfs_response_cb_type      simlock_cb
)
{
  simlock_result_enum_type simlock_status = SIMLOCK_SUCCESS;

  if(simlock_remote_sfs_req_callback == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_remote_sfs_req_callback(operation_type,
                                                   file,
                                                   token,
                                                   data_ptr,
                                                   simlock_cb);

  return simlock_status;
} /* simlock_modem_invoke_remote_callback */


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_SEND_OPERATION_REQUEST_SYNC

DESCRIPTION
  This function sends a read/write request to registered client (QMI UIM)
  to fetch or write data from or to a remote SFS. It enters a synchronous
  wait state, until a response is received or the sanity timer expires.

DEPENDENCIES
  QMI UIM must have registered for simlock remote SFS cb

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_sfs_remote_send_operation_request_sync
(
  simlock_file_operation_enum_type      operation_type,
  simlock_file_type                     file,
  simlock_data_type                   * data_ptr
)
{
  simlock_result_enum_type   simlock_status     = SIMLOCK_SUCCESS;
  simlock_token_id_type      token              = 0;
  boolean                    is_token_match     = FALSE;
  rex_sigs_type              sigs_rcvd          = 0x00;
  rex_sigs_type              sigs               = SIMLOCK_REMOTE_SFS_RESPONSE_SIG      |
                                                  SIMLOCK_REMOTE_SFS_SANITY_TIMER_SIG  |
                                                  SIMLOCK_TASK_STOP_SIG;
  rex_timer_type             simlock_sanity_timer;

  if(data_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  (void)rex_clr_sigs(rex_self(), SIMLOCK_REMOTE_SFS_RESPONSE_SIG);
  (void)rex_clr_sigs(rex_self(), SIMLOCK_REMOTE_SFS_SANITY_TIMER_SIG);

  /* initializes the simlock_remote_sfs_rsp_q,
     if not already done */
  simlock_sfs_remote_response_q_init();

  /* generate a request token */
  token = simlock_util_generate_request_token();

  /* invoke the registered callback */
  simlock_status = simlock_modem_invoke_remote_callback(operation_type, file,
                                                        token, data_ptr,
                                                        simlock_sfs_remote_operation_response_cb);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* Sanity Timer used for the SIMLock - QMI UIM interface */
  rex_def_timer(&simlock_sanity_timer, rex_self(), SIMLOCK_REMOTE_SFS_SANITY_TIMER_SIG);

  /* set the timer for the response */
  (void)rex_set_timer(&simlock_sanity_timer, SIMLOCK_SANITY_TIMER_VALUE);

  do
  {
    sigs_rcvd = simlock_wait(sigs);

    if(sigs_rcvd & sigs & SIMLOCK_REMOTE_SFS_RESPONSE_SIG)
    {
      simlock_status = simlock_sfs_remote_process_rsp_sig(operation_type, token, &is_token_match, data_ptr);

      /* in case there is mis-match between request and response
         tokens, continue to wait */
      if(is_token_match)
      {
        break;
      }
    }
  } while ((sigs_rcvd & 
            (SIMLOCK_REMOTE_SFS_SANITY_TIMER_SIG  |
             SIMLOCK_TASK_STOP_SIG)) == 0 );

  /* clear the timer */
  (void)rex_clr_timer(&simlock_sanity_timer);

  /* undef as the timer is no longer needed */
  (void)rex_undef_timer(&simlock_sanity_timer);

  /* in case wait() state was exited due to timer expiry or stop sig,
     update the return status here. In case the RSP_Q_SIG and TIMER_SIG
     or STOP_SIG are simultaneously set, the wait() state will be
     exited as result of processing the RSP_Q_SIG. So do not update
     the return status */
  if(!is_token_match)
  {
    simlock_status = SIMLOCK_GENERIC_ERROR;
    if (sigs_rcvd & SIMLOCK_TASK_STOP_SIG)
    {
      /* let the main loop handle it */
    }
    else if(sigs_rcvd & SIMLOCK_REMOTE_SFS_SANITY_TIMER_SIG)
    {
      (void)rex_clr_sigs(rex_self(), SIMLOCK_REMOTE_SFS_SANITY_TIMER_SIG);
      SIMLOCK_MSG_ERR_0("timer expired waiting for remote response");
    }
  }

  return simlock_status;
} /* simlock_sfs_remote_send_operation_request_sync */


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_READ_FILE

DESCRIPTION
  This function reads the data from the requested file by calling the remote
  SFS function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_remote_read_file(
  simlock_file_type                     file,
  simlock_data_type                   * data_ptr
)
{
  if(data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_sfs_remote_read_file");

  return simlock_sfs_remote_send_operation_request_sync(SIMLOCK_FILE_OPERATION_READ,
                                                        file,
                                                        data_ptr);
} /* simlock_sfs_remote_read_file */


/*===========================================================================
FUNCTION SIMLOCK_SFS_REMOTE_WRITE_FILE

DESCRIPTION
  This function writes the data to the requested file by calling the remote
  SFS function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_remote_write_file(
  simlock_file_type                     file,
  simlock_data_type                   * data_ptr)
{
  if(data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_sfs_remote_write_file");

  return simlock_sfs_remote_send_operation_request_sync(SIMLOCK_FILE_OPERATION_WRITE,
                                                        file,
                                                        data_ptr);
} /* simlock_sfs_remote_write_file */

#endif /* FEATURE_SIMLOCK */

