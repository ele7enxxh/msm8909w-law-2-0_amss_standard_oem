/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   C M   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock call manager (CM) interaction
  functions.


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_cm.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/22/16   vdc     Remove F3 messages for memory allocation failure
12/17/14   vv      Set emergency mode if remote SFS is enabled, but not ready
11/10/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock.h"
#include "simlock_modem.h"
#include "simlock_modem_p.h"
#include "simlock_util.h"
#include "simlock_config.h"
#include "cm.h"

#include "queue.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* CM response queue */
static q_type              simlock_cm_rsp_q;

/* CM client id */
static cm_client_id_type   simlock_cm_client_id;

/* sanity timer (in ms) for response from CM */
#define SIMLOCK_CM_SANITY_TIMER_VALUE    80000

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_CM_SET_DEVICE_MODE_RSP_TYPE

   DESCRIPTION:
     The structure contains response data for a set device mode request
-------------------------------------------------------------------------------*/
typedef struct{
  cm_ac_cmd_e_type         ac_cmd;
  cm_ac_cmd_err_e_type     ac_cmd_err;
  void                   * user_data_ptr;
} simlock_cm_set_device_mode_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_CM_RSP_TYPE

   DESCRIPTION:
     The response header for response received from CM by SIMLOCK task
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                                  link;
  simlock_cm_set_device_mode_rsp_type          device_mode_rsp;
} simlock_cm_rsp_type;


/*===========================================================================

                        FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_CM_INIT

DESCRIPTION
  This function registers the simlock with CM, if it is not already
  registered. It uses the "GSDI" client type to obtain a client id.
  It also intializes the CM response queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void simlock_cm_init
(
  void
)
{
  /* indicates if simlock already initialized with CM */
  static boolean is_simlock_cm_init = FALSE;

  if(is_simlock_cm_init)
  {
    return;
  }

  if (cm_client_init(CM_CLIENT_TYPE_GSDI, &simlock_cm_client_id) != CM_CLIENT_OK)
  {
    SIMLOCK_MSG_ERR_0("Unable to get client id from CM");
    return;
  }

  (void)q_init(&simlock_cm_rsp_q);

  is_simlock_cm_init = TRUE;
} /* simlock_cm_init */


/*===========================================================================
FUNCTION SIMLOCK_CM_QUEUE_RSP

DESCRIPTION
  This function puts the CM response into the SIMLOCK response queue

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static simlock_result_enum_type simlock_cm_queue_rsp(
  simlock_cm_rsp_type     * task_rsp_ptr
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
    q_put(&simlock_cm_rsp_q, &task_rsp_ptr->link);

    /* Set the command queue signal */
    (void)rex_set_sigs(UIM_SIMLOCK_TCB, SIMLOCK_CM_RESPONSE_SIG);
  }
  else
  {
    SIMLOCK_MSG_ERR_0("SIMLOCK task stopped, cannot queue response");
    result = SIMLOCK_GENERIC_ERROR;
  }

  SIMLOCK_LEAVE_TASK_STATE_CRIT_SECT;
  return result;
} /* simlock_cm_queue_rsp */


/*===========================================================================
FUNCTION SIMLOCK_CM_RESPONSE_CB

DESCRIPTION
  The function is a callback, which builds the CM response to the simlock
  task. The callback is provided in the request API.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void simlock_cm_response_cb
(
  void                           *  data_block_ptr,
  cm_ac_cmd_e_type                  ac_cmd,
  cm_ac_cmd_err_e_type              ac_cmd_err
)
{
  simlock_result_enum_type           simlock_status = SIMLOCK_SUCCESS;
  simlock_cm_rsp_type             *  task_rsp_ptr   = NULL;

  SIMLOCK_CHECK_AND_MEM_MALLOC(task_rsp_ptr,
                               sizeof(simlock_cm_rsp_type));
  if (task_rsp_ptr == NULL)
  {
    return;
  }

  SIMLOCK_MSG_MED_0("simlock_cm_response_cb");

  memset(task_rsp_ptr, 0x00, sizeof(simlock_cm_rsp_type));

  task_rsp_ptr->device_mode_rsp.ac_cmd          = ac_cmd;
  task_rsp_ptr->device_mode_rsp.ac_cmd_err      = ac_cmd_err;
  task_rsp_ptr->device_mode_rsp.user_data_ptr   = data_block_ptr;

  /* Put on SIM Lock command queue */
  simlock_status = simlock_cm_queue_rsp(task_rsp_ptr);
  if (simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MEM_FREE(task_rsp_ptr->device_mode_rsp.user_data_ptr);
    SIMLOCK_MEM_FREE(task_rsp_ptr);
  }

  SIMLOCK_MSG_MED_1("queue of cm response, status 0x%x",
                    simlock_status);
} /* simlock_cm_response_cb */


/*===========================================================================
FUNCTION SIMLOCK_CM_PROCESS_RESPONSE

DESCRIPTION
  This function processes the response received from the CM.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static simlock_result_enum_type simlock_cm_process_response
(
  simlock_cm_set_device_mode_rsp_type    * resp_ptr,
  simlock_token_id_type                    request_token,
  boolean                                * token_match_ptr
)
{
  simlock_token_id_type       response_token  = 0;

  if((resp_ptr == NULL) ||
     (resp_ptr->user_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  response_token = *((simlock_token_id_type *)resp_ptr->user_data_ptr);

  /* if the tokens doesn't match return error */
  if(request_token != response_token)
  {
    *token_match_ptr = FALSE;
    SIMLOCK_MSG_ERR_2("token mis-match. Request token: 0x%x, response token: 0x%x",
                      request_token, response_token);
    return SIMLOCK_GENERIC_ERROR;
  }

  *token_match_ptr = TRUE;

  if(resp_ptr->ac_cmd_err != CM_AC_CMD_ERR_NOERR)
  {
    SIMLOCK_MSG_ERR_2("error in response, err: 0x%x for cmd: 0x%x",
                      resp_ptr->ac_cmd_err, resp_ptr->ac_cmd);
    return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_cm_process_response */


/*===========================================================================
FUNCTION SIMLOCK_CM_PROCESS_RSP_SIG

DESCRIPTION
  This function processes the response signal set for the CM response

DEPENDENCIES
  SIMLOCK task is in wait state

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_cm_process_rsp_sig (
  simlock_token_id_type       request_token,
  boolean                   * token_match_ptr
)
{
  simlock_cm_rsp_type        *    simlock_cm_rsp_ptr  = NULL;
  simlock_result_enum_type        simlock_status      = SIMLOCK_GENERIC_ERROR;

  if(token_match_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_cm_rsp_ptr = (simlock_cm_rsp_type*)(q_get(&simlock_cm_rsp_q));

  /* If there is a command on the response queue */
  if(simlock_cm_rsp_ptr != NULL)
  {
    SIMLOCK_MSG_HIGH_0("SIMLOCK_CM_RESPONSE_SIG received");

    simlock_status = simlock_cm_process_response(&simlock_cm_rsp_ptr->device_mode_rsp,
                                                 request_token, token_match_ptr);
    SIMLOCK_MEM_FREE(simlock_cm_rsp_ptr->device_mode_rsp.user_data_ptr);
    SIMLOCK_MEM_FREE(simlock_cm_rsp_ptr);
  }

  (void)rex_clr_sigs(rex_self(), SIMLOCK_CM_RESPONSE_SIG);

  /* in case there is a token mis-match, check if there
     are more responses in the queue */
  if(*token_match_ptr == FALSE)
  {
    /* Set the signal if there are one or more responses to look at */
    if (q_cnt(&simlock_cm_rsp_q) > 0)
    {
      /*  Items on the queue. Set the signal */
      (void)rex_set_sigs(rex_self(), SIMLOCK_CM_RESPONSE_SIG);
    }
  }

  return simlock_status;
} /* simlock_sfs_remote_process_rsp_sig */


/*===========================================================================
FUNCTION SIMLOCK_CM_CONVERT_DEVICE_MODE_TO_AC_CMD

DESCRIPTION
  This function converts simlock device mode to the access control module
  command type

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_cm_convert_device_mode_to_ac_cmd
(
  simlock_device_mode_enum_type      device_mode,
  cm_ac_cmd_e_type                *  ac_cmd_ptr
)
{
  if(ac_cmd_ptr ==  NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  switch (device_mode)
  {
    case SIMLOCK_DEVICE_NORMAL_OPERATION_MODE:
      *ac_cmd_ptr = CM_AC_CMD_NORMAL_OPERATION;
      break;

    case SIMLOCK_DEVICE_EMERGENCY_ONLY_MODE:
      *ac_cmd_ptr = CM_AC_CMD_EMERGENCY_ONLY;
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_cm_convert_device_mode_to_ac_cmd */


/*===========================================================================
FUNCTION SIMLOCK_SET_DEVICE_MODE_SYNC

DESCRIPTION
  This function sends a emergency or normal mode request to CM task.
  It enters a synchronous wait state, until a response is received or the
  sanity timer expires.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_device_mode_sync
(
  simlock_device_mode_enum_type   device_mode
)
{
  simlock_result_enum_type   simlock_status     = SIMLOCK_SUCCESS;
  cm_ac_cmd_e_type           cm_ac_cmd          = CM_AC_CMD_NORMAL_OPERATION;
  boolean                    is_token_match     = FALSE;
  simlock_token_id_type    * token_ptr          = NULL;
  rex_sigs_type              sigs_rcvd          = 0x00;
  rex_sigs_type              sigs               = SIMLOCK_CM_RESPONSE_SIG               |
                                                  SIMLOCK_CM_RESPONSE_SANITY_TIMER_SIG  |
                                                  SIMLOCK_TASK_STOP_SIG;
  rex_timer_type             simlock_cm_sanity_timer;

  (void)rex_clr_sigs(rex_self(), SIMLOCK_CM_RESPONSE_SIG);
  (void)rex_clr_sigs(rex_self(), SIMLOCK_CM_RESPONSE_SANITY_TIMER_SIG);

  /* requests a client id from cm and
     initializes the simlock_cm_rsp_q */
  simlock_cm_init();

  /* convert the device mode to ac_cmd */
  simlock_status = simlock_cm_convert_device_mode_to_ac_cmd(device_mode, &cm_ac_cmd);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(token_ptr,
                               sizeof(simlock_token_id_type));
  if (token_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* generate a request token */
  *token_ptr = simlock_util_generate_request_token();

  /* invoke the CM API. It sets the device in a mode defined
     by cm_ac_cmd */
  if(!cm_ac_cmd_control_nw_access(simlock_cm_response_cb, token_ptr,
                                  simlock_cm_client_id, cm_ac_cmd,
                                  CM_AC_REASON_SIM_LOCK, SYS_MODEM_AS_ID_1))
  {
    SIMLOCK_MSG_ERR_0("cm_ac_cmd_control_nw_access request failed");
    SIMLOCK_MEM_FREE(token_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* sanity timer used for the SIMLock - CM interface */
  rex_def_timer(&simlock_cm_sanity_timer, rex_self(), SIMLOCK_CM_RESPONSE_SANITY_TIMER_SIG);

  /* set the timer for the response */
  (void)rex_set_timer(&simlock_cm_sanity_timer, SIMLOCK_CM_SANITY_TIMER_VALUE);

  do
  {
    sigs_rcvd = simlock_wait(sigs);

    if(sigs_rcvd & sigs & SIMLOCK_CM_RESPONSE_SIG)
    {
      simlock_status = simlock_cm_process_rsp_sig(*token_ptr, &is_token_match);

      /* in case there is mis-match between request and response
         tokens, continue to wait */
      if(is_token_match)
      {
        break;
      }
    }
  } while ((sigs_rcvd & 
            (SIMLOCK_CM_RESPONSE_SANITY_TIMER_SIG  |
             SIMLOCK_TASK_STOP_SIG)) == 0 );

  /* clear the timer */
  (void)rex_clr_timer(&simlock_cm_sanity_timer);

  /* undef as the timer is no longer needed */
  (void)rex_undef_timer(&simlock_cm_sanity_timer);

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
    else if(sigs_rcvd & SIMLOCK_CM_RESPONSE_SANITY_TIMER_SIG)
    {
      (void)rex_clr_sigs(rex_self(), SIMLOCK_CM_RESPONSE_SANITY_TIMER_SIG);
      SIMLOCK_MSG_ERR_0("timer expired waiting for CM response");
    }
  }

  SIMLOCK_MSG_MED_1("simlock_set_device_mode_sync, status: 0x%x", simlock_status);

  return simlock_status;
} /* simlock_set_device_mode_sync */


/*===========================================================================
FUNCTION SIMLOCK_CM_IS_DEVICE_IN_EMERGENCY_MODE

DESCRIPTION
  This function checks if the device in the emergency only mode.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  TRUE  : If the device is in emergency mode
  FALSE : If otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean simlock_cm_is_device_in_emergency_mode
(
  void
)
{
  simlock_result_enum_type  simlock_status = SIMLOCK_SUCCESS;
  simlock_config_data_type  config_file_data;

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    /* if the config data read failed, for some reason, we
       are taking a conservative approach to set the device
       in the emergency mode */
    SIMLOCK_MSG_ERR_0("read config file failed");
    return TRUE;
  }

  /* check if the device is in emergency mode */
  return simlock_util_is_device_in_emergency_mode(&config_file_data);
} /* simlock_cm_is_device_in_emergency_mode */

#endif /* FEATURE_SIMLOCK */

