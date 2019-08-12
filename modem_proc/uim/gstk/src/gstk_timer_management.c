/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           G S T K  T I M E R  M A N A G E M E N T


GENERAL DESCRIPTION
  This source file contains functions to process timer management
  proactive command


FUNCTIONS
  gstk_send_timer_management_terminal_rsp
    This function populate the gstk_timer_manage_cnf_type and put the
    terminal response confirm type to the GSTK queue.

  gstk_build_timer_expire_envelope
    This function build an envelope command when the timer was started and
    expired

  gstk_rex_timer_elapsed_expire_cb
    This function will add the elapsed time value to the time field of the
    timer management table and restart the timer_elapsed

  gstk_rex_timer_expire_cb
    This function builds the timer expiration envelope command to be sent
    to the card

  gstk_timer_action
    This function process the timer management proactive command, start
    the timer, deactivate or get the current time information

  gstk_timer_management_req
    This function parses the timer management Proactive Command from the card
    and sends it to the client.

  gstk_timer_management_cnf
    This function packs the timer management terminal response and send
    the command to UIM server.

  gstk_timer_management_cnf_cleanup
    This function cleans up any allocated pointer on the stack.


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004-2007, 2009 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_timer_management.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/12/15   vr      F3 Logging inprovements
07/13/15   gm      Fixed multi slot issue
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
04/03/14   dy      Replace gstk_malloc() with macro
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Updated GSTK_NULL_CHECK_PTR macro
12/11/13   df      Remove unused memory paging feature
10/04/13   gs      F3 Message reduction
09/13/13   gm      Support for Recovery and hot-swap
04/09/13   vr      Use safer versions of memcpy() and memmove()
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
08/24/11   xz      Fix compiler warnings and LINT/KW errors
06/06/11   sg      Add support for mandatory TLV for result 0x20
05/14/11   bd      Thread safe changes - timer management
04/28/11   nk      Fix compiler warning by removing iclude tmc.h
12/30/10   ssr     Dynamically allocate command buffer
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
03/15/10   xz      Fix issue of wrong timer expiration envelope
02/15/10   nb      Moved Dual Slot Changes
12/22/09   xz      Complete fix of the invalid timer ID issue
11/19/09   xz      Send error TR when timer ID is invalid
09/26/09   kp      ZI memory reduction changes
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
03/09/09   gg      Fixed elapsed timer value in elapsed timer callback
09/08/08   sk      Fixed indentation issues
04/27/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
08/13/04   tml     Fixed indentation
08/03/04   tml     Memory leak fix, changed to use pointer for additional info
                   in terminal response
02/12/04   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

#include "err.h"
#include "string.h"
#include <stringl/stringl.h>
#include "rex.h"
#include "uim_msg.h"

typedef enum
{
  GSTK_TIMER_START     = 0x00,
  GSTK_TIMER_END       = 0x01,
  GSTK_TIMER_GET_VALUE = 0x02
}
gstk_timer_action_enum_type;




/*===========================================================================
FUNCTION gstk_timer_management_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  timer_manage_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_response: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_timer_management_cnf_cleanup(
  gstk_timer_manage_cnf_type               * timer_manage_cnf,
  timer_management_terminal_response_type  * STK_response)
{

  if(timer_manage_cnf == NULL || STK_response == NULL)
  {
    UIM_MSG_ERR_2("In gstk_timer_management_cnf_cleanup()"
                  "timer_manage_cnf 0x%x, STK_response 0x%x ",
                  timer_manage_cnf, STK_response);
    return;
  }

  /* free timer management cnf and tlv */
  if (STK_response->header.result.additional_info != NULL) {
    gstk_free(STK_response->header.result.additional_info);
    STK_response->header.result.additional_info = NULL;
  }
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)timer_manage_cnf);/*lint !e826 area too small */
} /* gstk_timer_management_cnf_cleanup */


/*===========================================================================
FUNCTION gstk_rex_timer_elapsed_expire_cb

DESCRIPTION
  This function will add the elapsed time value to the time field of the
  timer management table and restart the timer_elapsed

PARAMETERS
  param: [Input] Specifies which timer has expired

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_rex_timer_elapsed_expire_cb( unsigned long param)
{
  uint8                inst_idx     = uint32touint8((param & 0x0000FF00) >> 8);
  uint8                timer_idx    = uint32touint8(param & 0x000000FF);
  gstk_task_cmd_type*  task_cmd_ptr = NULL;

  UIM_MSG_HIGH_0("gstk_rex_timer_elapsed_expire_cb()");
  if (!GSTK_IS_VALID_SLOT_ID(inst_idx + 1))
  {
    UIM_MSG_ERR_1("invalid slot id: 0x%x", inst_idx + 1);
    return;
  }
  if (timer_idx >= GSTK_MAX_TIMER_SUPPORTED)
  {
    UIM_MSG_ERR_1("invalid timer idx: 0x%x", timer_idx);
    return;
  }
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr != NULL)
  {
    task_cmd_ptr->cmd.timer_msg.message_header.sim_slot_id =
      (gstk_slot_id_enum_type)(inst_idx + 1);
    task_cmd_ptr->cmd.timer_msg.message_header.command_group =
      GSTK_TIMER_EXP_CMD;
    task_cmd_ptr->cmd.timer_msg.message_header.command_id =
      GSTK_TIMER_MGMT_TIMER;
    task_cmd_ptr->cmd.timer_msg.message_header.user_data = timer_idx;
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
} /* gstk_rex_timer_elapsed_expire_cb */

/*===========================================================================
FUNCTION gstk_rex_timer_expire_cb

DESCRIPTION
  This function trigger the timer expiration event envelope command.
  This will only build an envelope command when the timer was started and
  expired

PARAMETERS
  param: [Input] Specifies which timer has expired

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_rex_timer_expire_cb ( unsigned long param)
{
  uint8 inst_idx  = uint32touint8((param & 0x0000FF00) >> 8);
  uint8 timer_idx = uint32touint8(param & 0x000000FF);

  if (!GSTK_IS_VALID_SLOT_ID(inst_idx + 1))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", inst_idx + 1);
    return;
  }

  GSTK_RETURN_IF_NULL_PTR(gstk_instances_ptr[inst_idx]);

  if (timer_idx >= GSTK_MAX_TIMER_SUPPORTED)
  {
    UIM_MSG_ERR_1("invalid timer idx 0x%x", timer_idx);
    return;
  }

  if (gstk_instances_ptr[inst_idx]->gstk_timer_management[timer_idx].timer_started)
  {
    (void)rex_set_timer(
      &(gstk_instances_ptr[inst_idx]->gstk_timer_management[timer_idx].timer_elapsed),
      GSTK_TIMER_ELAPSED);
  }
  gstk_build_timer_expire_envelope((gstk_slot_id_enum_type)(inst_idx + 1),
                                   timer_idx);
} /* gstk_rex_timer_expire_cb */

/*===========================================================================
FUNCTION gstk_build_timer_expire_envelope

DESCRIPTION
  This function build an envelope command when the timer was started and
  expired

PARAMETERS
  slot:  [Input] The SIM slot where the envelope command to be sent
  param: [Input] Specifies which timer has expired

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_build_timer_expire_envelope(
  gstk_slot_id_enum_type slot,
  uint8                  param
)
{
  gstk_task_cmd_type    * task_cmd;
  uint32                  user_data;
  uint32                  time_elapsed;
  uint32                  time_left;

  UIM_MSG_HIGH_0("timer expire dl");

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return;
  }

  /* Timer expire, needs to send timer envelope */
  if ((gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[param].timer_started) ||
      (gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[param].pending))
  {
    /* set user_data as the timer index */
    user_data = param;

    /* get command buf */
    task_cmd = gstk_task_get_cmd_buf();
    if ( task_cmd != NULL )
    {
      /* successfully obtained command buffer - build the envelope command */
      task_cmd->cmd.timer_expire_envelope_ind.message_header.client_id     = gstk_instances_ptr[(uint32)slot-1]->client_id;
      task_cmd->cmd.timer_expire_envelope_ind.message_header.sim_slot_id   = slot;
      task_cmd->cmd.timer_expire_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      task_cmd->cmd.timer_expire_envelope_ind.message_header.command_id    = (int)GSTK_TIMER_EXPIRE_IND;
      task_cmd->cmd.timer_expire_envelope_ind.message_header.user_data     = user_data;

      /* get the current time elapsed */
      time_elapsed = rex_get_timer(
        &(gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[param].timer_elapsed));
      time_elapsed = GSTK_TIMER_ELAPSED - time_elapsed;

      /* timer info */
      task_cmd->cmd.timer_expire_envelope_ind.timer_info.timer_id = uint32touint8(param+1);
      task_cmd->cmd.timer_expire_envelope_ind.timer_info.time_value.hour =
          uint32touint8((gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[param].time + time_elapsed) / GSTK_MILISECONDS_IN_AN_HOUR);
      time_left = (gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[param].time + time_elapsed) % GSTK_MILISECONDS_IN_AN_HOUR;
      task_cmd->cmd.timer_expire_envelope_ind.timer_info.time_value.minute =
          uint32touint8(time_left / GSTK_MILISECONDS_IN_A_MINUTE);
      time_left = time_left % GSTK_MILISECONDS_IN_A_MINUTE;
      task_cmd->cmd.timer_expire_envelope_ind.timer_info.time_value.second =
          uint32touint8(time_left / GSTK_MILISECONDS_IN_A_SECOND);

      /* reset timer_start boolean */
      gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[param].timer_started = FALSE;
      gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[param].pending       = TRUE;
      gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[param].waiting_rsp   = TRUE;
      /* set GSTK_CMD_Q_SIG and put on GSTK queue */
      gstk_task_put_cmd_buf(task_cmd);
    }
    else
    {
      return;
    }
  }
  else
  { /* timer wasn't started! */
    UIM_MSG_HIGH_1("Timer[%d] was not running", param);
  }
} /* gstk_build_timer_expire_envelope */

/*===========================================================================
FUNCTION gstk_send_timer_management_terminal_rsp

DESCRIPTION
  This function populate the gstk_timer_manage_cnf_type and put the
  terminal response confirm type to the GSTK queue.

PARAMETERS
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
  cmd_num: [Input] This is the same value as that was being passed to
                   the client in the corresponding proactive command request
  general_result: [Input] Specifies the result of the proactive command
                          operation
  additional_result: [Input] Specifies the optional additional result for the
                             proactive command, the length field in the
                             gstk_additonal_info_type specifies number of
                             additional_info in the structure
  other_info_present: [Input] Other info present or not
  other_info: [Input] Pointer to the gstk_timer_info_type

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type  gstk_send_timer_management_terminal_rsp(
  uint32                                  cmd_details_ref_id,
  uint8                                   cmd_num,
  gstk_general_result_enum_type           general_result,
  const gstk_additional_info_type         *additional_result,
  boolean                                 other_info_present,
  const gstk_timer_info_type              *other_info)
{
  gstk_task_cmd_type              *command;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if (general_result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY)
  {
    if (command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].partial_comprehension) {
      general_result = GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION;
    }
  }

  if(additional_result == NULL)
  {
    UIM_MSG_ERR_0("additional_result ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_2("TIMER MANAGE rsp: result=0x%x, additional info len=0x%x",
                 general_result, additional_result->length);
  /* get command buf */
  command = gstk_task_get_cmd_buf();
  if ( command != NULL ) { /* successfully obtained command buffer */
    /* build the terminal response */
    command->cmd.timer_manage_term_rsp_cnf.message_header.sim_slot_id =
      gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
    command->cmd.timer_manage_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
    command->cmd.timer_manage_term_rsp_cnf.message_header.command_id = (uint32)GSTK_TIMER_MANAGEMENT_CNF;
    command->cmd.timer_manage_term_rsp_cnf.message_header.user_data = 0;
    command->cmd.timer_manage_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
    command->cmd.timer_manage_term_rsp_cnf.command_number = cmd_num;
    command->cmd.timer_manage_term_rsp_cnf.command_result = general_result;
    /* initialize additional info length to zero */
    command->cmd.timer_manage_term_rsp_cnf.result_additional_info.length = 0;
    command->cmd.timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

    if (additional_result->length != 0) {
      command->cmd.timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr =
        GSTK_CALLOC(command->cmd.timer_manage_term_rsp_cnf.result_additional_info.length);
      if (command->cmd.timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
        /* put the queue back to the free queue */
        gstk_task_free_cmd_buf(command);
        return GSTK_MEMORY_ERROR;
      }
      (void)memscpy(command->cmd.timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr,
             additional_result->length,
             additional_result->additional_info,
             additional_result->length);
      command->cmd.timer_manage_term_rsp_cnf.result_additional_info.length = additional_result->length;
    }
    /* initialize the input data field present to FALSE */
    command->cmd.timer_manage_term_rsp_cnf.timer_info_present = FALSE;
    if (other_info_present && (other_info != NULL)) {
      command->cmd.timer_manage_term_rsp_cnf.timer_info_present = TRUE;
      (void)memscpy(&command->cmd.timer_manage_term_rsp_cnf.time_info,
              sizeof(gstk_timer_info_type),
              other_info, sizeof(gstk_timer_info_type));
    }

    /* command performed successfully */
    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(command);
    return GSTK_SUCCESS;
  }
  else {
    // build error message to GSTK?????????????
    return GSTK_MEMORY_ERROR;
  }
} /* gstk_send_timer_management_terminal_rsp */

/*===========================================================================
FUNCTION gstk_timer_action

DESCRIPTION
  This function performs the corresponding timer action

PARAMETERS
  action: [Input] Specifies whether the timer is to be started, end or
                  get the current value of the timer
  timer_info: [Input/Output] Pointer that contains the timer information,
                             In case of start, this contains the length of
                             of the timer
                             In case of end, this will be populated with
                             the remaining time for the timer
                             In case of get current timer value, this
                             will be populated with the current time of the
                             timer

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_timer_action(
  gstk_timer_action_enum_type action,
  gstk_timer_info_type        *timer_info)
{
  uint32                current_time = 0;
  uint32                current_time_left = 0;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if (timer_info == NULL) {
    UIM_MSG_ERR_0("Null Input Param");
    return GSTK_ERROR;
  }

  UIM_MSG_HIGH_1("In gstk_timer_action(), Action Type is 0x%x ", action);
  switch (action)
  {
  case GSTK_TIMER_START:
    /* calculate the time in milliseconds */
    current_time = timer_info->time_value.hour * GSTK_MILISECONDS_IN_AN_HOUR;
    current_time += timer_info->time_value.minute * GSTK_MILISECONDS_IN_A_MINUTE;
    current_time += timer_info->time_value.second * GSTK_MILISECONDS_IN_A_SECOND;
    gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].time = current_time;

    /* start the timer now */
    (void)rex_set_timer(&(gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].timer),
                  gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].time);
    gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].timer_started = TRUE;
    gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].pending = FALSE;
    break;

  case GSTK_TIMER_END:
    if (gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].timer_started) {
      current_time = rex_clr_timer(&(gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].timer));
      gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].timer_started = FALSE;
      gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].pending = FALSE;
      gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].time = 0;

      /* find out the remaining time left on the timer */
      timer_info->time_value.hour = uint32touint8(current_time / GSTK_MILISECONDS_IN_AN_HOUR);
      current_time_left = current_time % GSTK_MILISECONDS_IN_AN_HOUR;
      timer_info->time_value.minute = uint32touint8(current_time_left / GSTK_MILISECONDS_IN_A_MINUTE);
      current_time_left = current_time_left % GSTK_MILISECONDS_IN_A_MINUTE;
      timer_info->time_value.second = uint32touint8(current_time_left / GSTK_MILISECONDS_IN_A_SECOND);
    }
    else { /* timer already ended, send TR with timer action contradiction */
      UIM_MSG_HIGH_0("Timer hasn't been started");
      return GSTK_CONTRADICTING_TIMER_REQUEST;
    }
    break;

  case GSTK_TIMER_GET_VALUE:
    if (gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].timer_started) {
      current_time = rex_get_timer(&(gstk_curr_inst_ptr->gstk_timer_management[timer_info->timer_id - 1].timer));

      /* calculate the current time */
      timer_info->time_value.hour = uint32touint8(current_time / GSTK_MILISECONDS_IN_AN_HOUR);
      current_time_left = current_time % GSTK_MILISECONDS_IN_AN_HOUR;
      timer_info->time_value.minute = uint32touint8(current_time_left / GSTK_MILISECONDS_IN_A_MINUTE);
      current_time_left = current_time % GSTK_MILISECONDS_IN_A_MINUTE;
      timer_info->time_value.second = uint32touint8(current_time_left / GSTK_MILISECONDS_IN_A_SECOND);
    }
    else { /* timer already ended, send TR with timer action contradiction */
      UIM_MSG_HIGH_0("Timer hasn't been started");
      return GSTK_CONTRADICTING_TIMER_REQUEST;
    }
    break;

  }
  return GSTK_SUCCESS;
} /* gstk_timer_action */


/*===========================================================================
FUNCTION gstk_timer_management_req

DESCRIPTION
  This function parses the Timer Management Proactive Command from the card
  and get the information from the various module and send the terminal
  response to the Card.
  In case of any parsing errors or when GSTK can't set up the timer,
  it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_timer_management_req (gstk_proactive_cmd_type* command)
{
  uint8                               *cmd_data = NULL;
  int32                               cmd_data_len;
  uint32                              cmd_details_ref_id;
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
  gstk_additional_info_type           additional_info;
  gstk_general_result_enum_type       general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  boolean                             more_tlv = TRUE;
  int32                               curr_offset = 0;
  int32                               new_offset = 0;
  gstk_timer_info_type                timer_info;
  boolean                             timer_info_present = FALSE;

  UIM_MSG_HIGH_0(" IN GSTK_TIMER_MANAGEMENT_REQ ");

  if(command == NULL)
  {
    UIM_MSG_ERR_0("Command ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  /* initialize additional_info's length to 0 */
  additional_info.length = 0;
  /* initialize extra param present to FALSE */
  timer_info_present = FALSE;
  /* initialize timer value to 0 */
  memset(&timer_info, 0x00, sizeof(gstk_timer_info_type));

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data, and specifies the length of
  ** the cmd_data in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command,
                                              &cmd_data,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );


  /* GSTK_SUCCESS from preprocess_command */
  if((gstk_status == GSTK_SUCCESS) && (cmd_data != NULL))
  {
    /* Timer ID */
    gstk_status = gstk_parser_timer_id_tlv(cmd_data,
                                    cmd_data_len,
                                    curr_offset,
                                    &(timer_info.timer_id),
                                    &new_offset,
                                    &more_tlv,
                                    GSTK_MANDATORY,
                                    &cmd_details_ref_id);
    if (gstk_status != GSTK_SUCCESS) {
      general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    }
    else if (timer_info.timer_id > GSTK_MAX_TIMER_SUPPORTED) {
      UIM_MSG_ERR_2("timer_id is out of boundary! 0x%x v.s. 0x%x",
                    timer_info.timer_id, GSTK_MAX_TIMER_SUPPORTED);
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
    }
    else {
      /* timer id is good, continue... */
      switch (command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.qualifier)
      {
      case 0x00: /* Start */
        if (more_tlv) {
          curr_offset = new_offset;
          /* get timer value */
          gstk_status = gstk_parser_timer_value_tlv(cmd_data,
                                    cmd_data_len,
                                    curr_offset,
                                    &(timer_info.time_value),
                                    &new_offset,
                                    &more_tlv,
                                    GSTK_OPTIONAL,
                                    &cmd_details_ref_id);
          if (gstk_status != GSTK_SUCCESS)
          {
            general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
          }
          else
          {
            /* Start the timer */
            timer_info_present = TRUE;
            gstk_status = gstk_timer_action(GSTK_TIMER_START, &timer_info);
            if (gstk_status != GSTK_SUCCESS) {
              if (gstk_status == GSTK_CONTRADICTING_TIMER_REQUEST) {
                general_result = GSTK_ACTION_IN_CONTRADICTION_WITH_THE_CURRENT_TIMER_STATE;
              }
              else { /* other error */
                general_result =  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
                gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
                timer_info_present = FALSE;
              }
            }
          }
        }
        else {
          general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
        }
        break;
      case 0x01: /* Deactivate */
        if (more_tlv) {
          UIM_MSG_HIGH_0("Ignore Card indicates more info...");
        }
        /* Deactivate the timer */
        timer_info_present = TRUE;
        gstk_status = gstk_timer_action(GSTK_TIMER_END, &timer_info);
        if (gstk_status != GSTK_SUCCESS) {
          if (gstk_status == GSTK_CONTRADICTING_TIMER_REQUEST) {
            general_result = GSTK_ACTION_IN_CONTRADICTION_WITH_THE_CURRENT_TIMER_STATE;
          }
          else { /* other error */
            general_result =  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
            timer_info_present = FALSE;
          }
        }
        /* deactivate the timer */
        break;
      case 0x02: /* Get current time */
        if (more_tlv) {
          UIM_MSG_HIGH_0("Ignore Card indicates more info...");
        }
        /* Get current value */
        timer_info_present = TRUE;
        gstk_status = gstk_timer_action(GSTK_TIMER_GET_VALUE, &timer_info);
        if (gstk_status != GSTK_SUCCESS) {
          if (gstk_status == GSTK_CONTRADICTING_TIMER_REQUEST) {
            general_result = GSTK_ACTION_IN_CONTRADICTION_WITH_THE_CURRENT_TIMER_STATE;
          }
          else { /* other error */
            general_result =  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
            timer_info_present = FALSE;
          }
        }
        /* get the timer value */
        break;
      default:
          general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
          UIM_MSG_ERR_0("Unknown qualifier for timer management");
          break;
      }
    }

    /* No error has been encountered in parsing */
    /* general_result default to success at the beginning of this function */

  } /* GSTK_SUCCESS from preprocess_command */
  else { /*  preprocess command parsing error */

    UIM_MSG_ERR_0("Timer Management TLV parsing err");
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
  } /* if no error from preprocess command */

  /* send the Terminal Response */
  gstk_status = gstk_send_timer_management_terminal_rsp(
                              cmd_details_ref_id, /* command_details_ref */
                              command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,
                              /* command_num */
                              general_result, /* general_result */
                              &additional_info, /* additional result */
                              timer_info_present,
                              &timer_info /* timer info pointer */ );
  if (gstk_status != GSTK_SUCCESS) {
    UIM_MSG_HIGH_0("Can't send term rsp");
  }

  /* nothing to release in memory location */
  if(cmd_data != NULL) {
    gstk_free(cmd_data);
    cmd_data = NULL;
  }

  return gstk_status;
} /* gstk_timer_management_req */

/*===========================================================================
FUNCTION gstk_timer_management_cnf

DESCRIPTION
  This function packs the timer management terminal response and send
  the command to UIM server.

PARAMETERS
  timer_manage_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_timer_management_cnf(
  gstk_timer_manage_cnf_type* timer_manage_cnf)
{

  gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;
  timer_management_terminal_response_type       STK_response;
  uim_cmd_type                                  *uim_cmd_ptr;
  uint32                                        offset = 0;

  UIM_MSG_HIGH_0("IN GSTK timer management cnf ");

  if(timer_manage_cnf == NULL)
  {
    UIM_MSG_ERR_0("timer_manage_cnf ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0, sizeof(timer_management_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      timer_manage_cnf->cmd_details_ref_id,
      timer_manage_cnf->message_header.command_id,
      timer_manage_cnf->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      timer_manage_cnf->command_result,
      timer_manage_cnf->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
    return gstk_status;
  }

  /* header length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  STK_response.num_bytes = STK_response.header.command_details.length +
                          STK_response.header.device_id.device_tag_length +
                          STK_response.header.result.result_tag_length +
                          6;

  /* Timer ID */
  if (timer_manage_cnf->timer_info_present) {
    gstk_status = gstk_packer_timer_id_tlv(
                      &timer_manage_cnf->time_info.timer_id,
                      &STK_response.timer_id);
    if (gstk_status != GSTK_SUCCESS) {
      gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
      return gstk_status;
    }

    /* header length: original data length + timer ID + 2 */
    STK_response.num_bytes += STK_response.timer_id.length + 2;

    /* check if it is a start timer cmd */
    /* check if it is success */
    if ((STK_response.header.command_details.qualifier != 0x00) &&
        ((STK_response.header.result.result == (int)GSTK_COMMAND_PERFORMED_SUCCESSFULLY) ||
         (STK_response.header.result.result == (int)GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION))) {
      /* mandatory timer value */
      gstk_status = gstk_packer_timer_value_tlv(
          &timer_manage_cnf->time_info.time_value,
          &STK_response.timer_value);
      if (gstk_status != GSTK_SUCCESS) {
        gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
        return gstk_status;
      }
      /* header length: original data length + timer value + 2 */
      STK_response.num_bytes += STK_response.timer_value.length + 2;
    }
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
      /* clear the command detail table reference slot before since we are about to reply to
        the card */
      gstk_util_cmd_details_ref_table_free_slot(timer_manage_cnf->cmd_details_ref_id);

      /* set user_data */
      uim_cmd_ptr->hdr.user_data               = gstk_curr_inst_ptr->client_id;
      /* populate the terminal_response info */
      uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

      /* copy command details */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                              &STK_response.header.command_details,
                              offset,
                              (STK_response.header.command_details.length +
                               GSTK_TAG_LENGTH_LEN ),
                              sizeof(uim_cmd_ptr->terminal_response.data),
                              sizeof(gstk_command_details_tlv_type));
      offset = (int)(offset + STK_response.header.command_details.length +
                     GSTK_TAG_LENGTH_LEN );
      /* copy device */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                             &STK_response.header.device_id,
                             offset,
                             (STK_response.header.device_id.device_tag_length +
                              GSTK_TAG_LENGTH_LEN ),
                             sizeof(uim_cmd_ptr->terminal_response.data),
                             sizeof(gstk_device_identities_tag_tlv_type));
      offset = (int)(offset + STK_response.header.device_id.device_tag_length +
                     GSTK_TAG_LENGTH_LEN );
      /* copy result */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                              &STK_response.header.result,
                              offset,
                              3, /* result tag +  length + general result */
                              sizeof(uim_cmd_ptr->terminal_response.data),
                              sizeof(gstk_result_tag_tlv_type));
      offset = offset + 3;
      if(STK_response.header.result.result_tag_length > 1 ) { /* has additional info */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                            STK_response.header.result.additional_info,
                            offset,
                            (STK_response.header.result.result_tag_length - 1),
                            sizeof(uim_cmd_ptr->terminal_response.data),
                            ((STK_response.header.result.result_tag_length - 1) *
                             sizeof(byte)));
        offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
      }

      /* time info */
      if (offset < STK_response.num_bytes) { /* has timer info */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                &STK_response.timer_id,
                                offset,
                                (STK_response.timer_id.length + 2 ),
                                sizeof(uim_cmd_ptr->terminal_response.data),
                                sizeof(gstk_timer_id_tlv_type));
        offset = (int)(offset + STK_response.timer_id.length + 2);

        if (offset < STK_response.num_bytes) {
          gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                  &STK_response.timer_value,
                                  offset,
                                  (STK_response.timer_value.length + 2 ),
                                  sizeof(uim_cmd_ptr->terminal_response.data),
                                  sizeof(gstk_timer_value_tlv_type));
          offset = (int)(offset + STK_response.timer_value.length + 2);
        }
      }

      gstk_util_dump_byte_array("Timer Management: TR",
                                uim_cmd_ptr->terminal_response.data,
                                uim_cmd_ptr->terminal_response.num_bytes);

      gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
    }
  } /* uim_cmd_ptr != NULL */
  else {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
  return gstk_status;
} /* gstk_timer_manage_cnf */
