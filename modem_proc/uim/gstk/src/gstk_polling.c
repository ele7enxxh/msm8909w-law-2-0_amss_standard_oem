/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   P O L L I N G


GENERAL DESCRIPTION
  This source file contains functions to process polling proactive command


FUNCTIONS
  gstk_polling_req
    This function parses the polling Proactive Command from the card
    and sends it to the client.

  gstk_polling_cnf
    This function packs the polling terminal response and send
    the command to UIM server.

  gstk_uim_polling_report
    This is the callback function for polling response from UIM.
    UIM will call this function when it finishes it's polling operation

  gstk_polling_cnf_cleanup
    This function cleans up any allocated pointer on the stack.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_polling.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/12/15   vr      F3 Logging inprovements
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
06/30/14   gm      Remove unnecessary clear of timer
05/06/14   gs      GSTK Featurization enhancements and clean up
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/13/13   shr     Added support for BIP over WLAN
10/04/13   gs      F3 Message reduction
09/13/13   gm     Support for Recovery and hot-swap
01/08/13   hn      Fix critical Klocwork warning
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
03/16/12   yt      Remove tmc.h inclusion
07/15/11   xz      Fix unexpected duration TLV in POLLING OFF TR
06/06/11   sg      Add support for mandatory TLV for result 0x20
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/17/10   ps      UIM memory reduction changes
02/15/10   nb      Moved Dual Slot Changes
09/26/09   kp      ZI memory reduction changes
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/23/07   nk      Replaced memcpy with gstk_offset_memcpy
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
03/12/07   sk      Initialized poll duration
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/18/05   tml     Fixed potential data abort issue
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
08/13/04   tml     Centralize release memory condition in the request function
08/03/04   tml     Memory leak fix and changed to use pointer for addtiional
                   info in the terminal response
05/26/04   tml     Added More debug messages
05/17/04   tml     Changed MAX_POLL_DURATION to UIM_MAX_POLL_DURATION
04/16/04   tml     linted
07/08/03   tml     Remove unnecessary message
05/16/03   tml     linted
04/17/03   tml     lint
03/06/03   tml     Updated enum names
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

#include "err.h"
#include "string.h"
#include "uim.h"
#include "uim_msg.h"

/* Fixed Bug of #define using only 2 Bytes instead of
** 4 Bytes
*/
#define GSTK_POLL_CMD_DETAILS_REF_ID_MASK 0x0000FFFF
#define GSTK_POLL_CMD_NUM_MASK            (unsigned) 0xFFFF0000
#define GSTK_2_BYTES                      16 /* Bits */


/*===========================================================================
FUNCTION gstk_polling_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  polling_cnf_ptr:  [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_response_ptr: [Input] Pointer to the STK intermediate message

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
static void gstk_polling_cnf_cleanup(
  gstk_poll_interval_cnf_type              *polling_cnf_ptr,
  polling_interval_terminal_response_type  *STK_response_ptr)
{
  if(STK_response_ptr == NULL || polling_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_2("gstk_polling_cnf_cleanup ERR: STK_response_ptr 0x%x, polling_cnf_ptr 0x%x",
                  STK_response_ptr, polling_cnf_ptr);
    return;
  }

  /* free the polling pointer from STK_response and _cnf if needed */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)polling_cnf_ptr);/*lint !e826 area too small */
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
    STK_response_ptr->header.result.additional_info= NULL;
  }
} /* gstk_polling_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_uim_polling_report

DESCRIPTION

  This is the callback function for polling response from UIM.
  UIM will call this function when it finishes it's polling operation

PARAMETERS
  report_ptr: [Input] Pointer to the UIM polling report

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
void gstk_uim_polling_report (uim_rpt_type *report_ptr)
{
  gstk_task_cmd_type   * task_cmd_ptr  = NULL;
  uint32                 length        = 0;
  uint32                 index         = 0;

  UIM_MSG_HIGH_0("GSTK UIM polling report_ptr cb");

  if(report_ptr == NULL)
  {
    UIM_MSG_ERR_0("report_ptr ERR:NULL");
    return;
  }

  if (report_ptr->rpt_type != UIM_POLLING_INTERVAL_R) {
    // build error message to GSTK?????????????
    UIM_MSG_ERR_0("POLLING expected in CB");
    return;
  }

  /* get command buf */
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr != NULL ) { /* successfully obtained command buffer */
    /* build the terminal profile response */
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.message_header.sim_slot_id =
      (gstk_slot_id_enum_type)report_ptr->slot;
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.message_header.command_group =
      GSTK_TERMINAL_RSP;
    /* polling off */
    if(report_ptr->rpt.polling_interval.actual_uim_polling_interval
          == (dword)UIM_MAX_POLL_DURATION) {
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.message_header.command_id
                                               = (int)GSTK_POLLING_OFF_CNF;
    }
    else { /* polling interval */
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.message_header.command_id
                                               = (int)GSTK_POLL_INTERVAL_CNF;
    }
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.message_header.user_data
                                             = report_ptr->user_data;

    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.cmd_details_ref_id =
        report_ptr->user_data;

    index = GSTK_CMD_REF_TABLE_INDEX(task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.cmd_details_ref_id);

    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.command_number =
        command_details_ref_table_ptr[index].command_details.command_number;

    /* initialize additional_info to zero */
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = 0;
    task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

    if(report_ptr->rpt_status == UIM_PASS) {
      /* polling off */
      if(report_ptr->rpt.polling_interval.actual_uim_polling_interval
            == (dword)UIM_MAX_POLL_DURATION) {
        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.present = FALSE;
        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.length = 0;
      }
      else { /* polling interval */

        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.present = TRUE;
        length = report_ptr->rpt.polling_interval.actual_uim_polling_interval
                                    / GSTK_MILISECONDS_IN_A_MINUTE;
        if (length >= 1)  {
          task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.units
                                                   = GSTK_DURATION_MINUTES;
        }
        else { /* unit is less minute */
          length = report_ptr->rpt.polling_interval.actual_uim_polling_interval
                                   / GSTK_MILISECONDS_IN_A_SECOND;
          if (length >= 1) {
            task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.units
                                                     = GSTK_DURATION_SECONDS;
          }
          else { /* unit is in ms */
            task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.units
                                                     = GSTK_DURATION_TENTHS_OF_SECONDS;
            length = report_ptr->rpt.polling_interval.actual_uim_polling_interval;
          }
        }

        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.length = (uint8)length;
      } /*polling interval */

     index = GSTK_CMD_REF_TABLE_INDEX(task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.cmd_details_ref_id);

      if(index < GSTK_MAX_PROACTIVE_COMMAND) {
        if (command_details_ref_table_ptr[index].partial_comprehension) {
          task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.command_result =
            GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION;
        }
        else {
          task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.command_result =
            GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
        }
      }
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = 0;
    }
    else { /* polling fail */
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.present = FALSE;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.length = 0;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.duration.units = 0;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.command_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = 1;
      task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr =
        GSTK_CALLOC(task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length);
      if (task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = 0;
      }
      else {
        task_cmd_ptr->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr[0] = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
      }
    }

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
}/*lint !e818 *report_ptr suppression for externalized function */
/* gstk_uim_polling_report */

/*===========================================================================
FUNCTION gstk_polling_req

DESCRIPTION
  This function parses the Poll Interval Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
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
gstk_status_enum_type gstk_polling_req (gstk_proactive_cmd_type* command_ptr)
{
  uint8                              *cmd_data_ptr          = NULL;
  int32                               cmd_data_len          = 0;
  uint32                              cmd_details_ref_id    = 0;
  int32                               curr_offset           = 0;
  gstk_status_enum_type               gstk_status           = GSTK_SUCCESS;
  boolean                             error_already_raised  = FALSE;
  int32                               new_offset            = 0;
  gstk_additional_info_type           additional_info       = {0};
  gstk_terminal_rsp_extra_param_type  extra_param           = {0};
  boolean                             more_tlv              = FALSE;
  uim_cmd_type                        *uim_cmd_ptr          = NULL;
  gstk_duration_type                  poll_duration         = {0};
  uint32                              poll_ms               = 0;
  boolean                             is_polling_off        = FALSE;
  uint32                              index                 = 0;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  UIM_MSG_HIGH_0(" IN GSTK_polling_REQ ");

  if(command_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* initialize extra_param */
  extra_param.present = TRUE;

  /* Check and populate command details
  ** into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into
  ** the cmd_data_ptr, and specifies the length
  ** of the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );
  index = GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id);

  if(gstk_status == GSTK_SUCCESS)
  {
    if(command_details_ref_table_ptr[index].expected_cmd_rsp_type
        == GSTK_POLLING_OFF_CNF) {
      /* this is for polling off command */
      is_polling_off = TRUE;
      poll_duration.present = FALSE;
    }
    else { /* this is polling interval command */
      if (cmd_data_ptr == NULL) {
        error_already_raised = TRUE;
      }
      else {
        /* parse the proactive command to get the duration information */
        /* duration */
        gstk_status = gstk_parser_duration_tlv( cmd_data_ptr,
                                                cmd_data_len,
                                                curr_offset,
                                                &poll_duration,
                                                &new_offset,
                                                &more_tlv,
                                                GSTK_MANDATORY,
                                                &cmd_details_ref_id
                                                );
        if (gstk_status != GSTK_SUCCESS )
        {
          error_already_raised = TRUE;
        }
      }
    }
  } /* GSTK_SUCCESS from preprocess_command */
  else
  {
    error_already_raised = TRUE;
  }

  /* No error has been encountered */
  if(!error_already_raised)
  {
    /* further checking for polling interval */
    /* Build UIM Polling command */
    if(!is_polling_off) { /* polling interval */
      if(!poll_duration.present)
      {
        /* got here because the unit is unknown! */
        UIM_MSG_HIGH_0("No duration for poll");
        gstk_status = GSTK_ERROR;

        /* Send Terminal Response to Card */
        /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
        extra_param.present = FALSE;
        extra_param.extra_param.poll_duration.present = FALSE;

        gstk_status = gstk_send_terminal_response(
          gstk_curr_inst_ptr->client_id,       /* client_id */
          0,                                  /* user_data */
          cmd_details_ref_id,                 /* command_details_ref */
          command_details_ref_table_ptr[index].command_details.command_number,
                                              /* command_num */
          GSTK_POLL_INTERVAL_CNF,             /* command response */
          GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,  /* general_result */
          &additional_info,                   /* additional result */
          &extra_param                        /* extra param */
          );
        if(gstk_status != GSTK_SUCCESS) {
          UIM_MSG_ERR_1("TP fail, 0x%x", gstk_status);
        }
        error_already_raised = TRUE;
      } /* polling duration not present */
    } /* not polling off */
  } /* no error yet */

  if(!error_already_raised) /* no error still */
  {
    /* get a buffer for the request */
    uim_cmd_ptr = gstk_get_uim_buf();
    if (uim_cmd_ptr != NULL )
    {
      gstk_status = gstk_util_populate_uim_buf_hdr_info(
                                        UIM_POLLING_INTERVAL_F,
                                        &uim_cmd_ptr);
      if (gstk_status == GSTK_SUCCESS && uim_cmd_ptr)
      {
        uim_cmd_ptr->hdr.user_data  =
          command_details_ref_table_ptr[index].unique_ref_id;

        if(!is_polling_off) {
          /* populate the uim polling info */
          uim_cmd_ptr->polling.command_type = POLLING_INTERVAL;

          switch(poll_duration.units) {
          case GSTK_DURATION_MINUTES:
            poll_ms = poll_duration.length *
                      GSTK_MILISECONDS_IN_A_MINUTE;
            break;
          case GSTK_DURATION_SECONDS:
            poll_ms = poll_duration.length *
                    GSTK_MILISECONDS_IN_A_SECOND;
            break;
          case GSTK_DURATION_TENTHS_OF_SECONDS:
            poll_ms = poll_duration.length *
                    GSTK_MILISECONDS_IN_ONE_TENTH_SEC;
            break;
          default:
            UIM_MSG_HIGH_0("Bad Duration");
            gstk_status = GSTK_ERROR;
            break;
          }
          uim_cmd_ptr->polling.polling_interval_in_ms = poll_ms;
        }
        else { /* polling off */
          uim_cmd_ptr->polling.command_type = POLLING_OFF;
          uim_cmd_ptr->polling.polling_interval_in_ms = 0;
        }
      }
      else {
          UIM_MSG_HIGH_0("Populate UIM Hdr Err");
          gstk_status = GSTK_ERROR;
      }

      if (gstk_status != GSTK_SUCCESS) { /* we have already get a uim_buffer but encounter an error */
        if (uim_cmd_ptr != NULL) {
          /* uim_cmd_ptr can be equal to NULL because in the gstk_util_populate_uim_buf_hdr_info, if
          there is error encountered, the uim_cmd_ptr will be set to NULL */
          /* put the queue back to the free queue */
          gstk_free(uim_cmd_ptr);
        }

        /* Send Terminal Response to Card */
        /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
        UIM_MSG_ERR_0("Get polling TLV parsing");

        /* Send Terminal Response to Card */
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        extra_param.present = FALSE;
        extra_param.extra_param.poll_duration.present = FALSE;
        extra_param.present = FALSE;

        gstk_status = gstk_send_terminal_response(
                        gstk_curr_inst_ptr->client_id, /* client_id */
                        0,                             /* user_data */
                        cmd_details_ref_id,           /* command_details_ref */
                        command_details_ref_table_ptr[index].command_details.command_number,
                        /* command_num */
                        command_details_ref_table_ptr[index].expected_cmd_rsp_type,
                        /* command response: poll interval or polling off */
                        GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
                        &additional_info, /* additional result */
                        &extra_param /* extra param */ );
        if(gstk_status != GSTK_SUCCESS) {
          UIM_MSG_ERR_1("TP fail, %d", gstk_status);
        }

      }
      else { /* send to UIM */
        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
        /* Start Timer */
        gstk_util_start_client_response_timer(cmd_details_ref_id,
                                              GSTK_TERM_RSP_WAIT_TIME);
        /* reset gstk_status to SUCCESS since there has been no error*/
        /* This is needed since some of the TLVs are optional, which might
        ** cause the various parsing functions to return invalid_command */
        gstk_status = GSTK_SUCCESS;
      }
    } /* uim_cmd_ptr != NULL */
    else { /* can't get uim buffer */
      gstk_status = GSTK_MEMORY_ERROR;

      /* Send Terminal Response to Card */
      /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
      UIM_MSG_HIGH_0("Get polling TLV parsing");

      /* Send Terminal Response to Card */
      gstk_set_additional_info(&additional_info,GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      extra_param.present = FALSE;
      extra_param.extra_param.poll_duration.present = FALSE;
      gstk_status = gstk_send_terminal_response(
                      gstk_curr_inst_ptr->client_id, /* client_id */
                      0,              /* user_data */
                      cmd_details_ref_id, /* command_details_ref */
                      command_details_ref_table_ptr[index].command_details.command_number,
                      /* command_num */
                      command_details_ref_table_ptr[index].expected_cmd_rsp_type,
                      /* command response: poll interval or polling off */
                      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
                      &additional_info, /* additional result */
                      &extra_param /* extra param */ );
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_1("TP fail, 0x%x", gstk_status);
      }
    }
  } /* ! error raised from parsing */
  else { /* error from parsing */
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    extra_param.present = FALSE;
    extra_param.extra_param.poll_duration.present = FALSE;

    gstk_status = gstk_send_terminal_response(
                    gstk_curr_inst_ptr->client_id, /* client_id */
                    0,              /* user_data */
                    cmd_details_ref_id, /* command_details_ref */
                    command_details_ref_table_ptr[index].command_details.command_number,
                    /* command_num */
                    command_details_ref_table_ptr[index].expected_cmd_rsp_type,
                    /* command response: poll interval or polling off */
                    GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                    &additional_info, /* additional result */
                    &extra_param /* extra param */ );
    if(gstk_status != GSTK_SUCCESS) {
      UIM_MSG_ERR_1("TP fail, 0x%x", gstk_status);
    }
  }

  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
    cmd_data_ptr = NULL;
  }
  return gstk_status;
} /* gstk_polling_req */

/*===========================================================================
FUNCTION gstk_polling_cnf

DESCRIPTION
  This function packs the poll interval terminal response and send
  the command to UIM server.

PARAMETERS
  polling_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_polling_cnf(
  gstk_poll_interval_cnf_type* polling_cnf_ptr)
{

  gstk_status_enum_type                     gstk_status    = GSTK_SUCCESS;
  polling_interval_terminal_response_type   STK_response   = {0};
  uim_cmd_type                              *uim_cmd_ptr   = NULL;
  uint8                                     header_bytes   = 0;
  uint32                                    offset         = 0;
  uint32                                    ref_id         = 0;

  UIM_MSG_HIGH_0("IN GSTK_polling_cnf ");

  if(polling_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("polling_cnf_ptr ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  ref_id = polling_cnf_ptr->cmd_details_ref_id;
  if (command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(ref_id)].expected_cmd_rsp_type ==
        GSTK_POLLING_OFF_CNF)
  {
    polling_cnf_ptr->duration.present = FALSE;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(polling_interval_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      polling_cnf_ptr->cmd_details_ref_id,
      polling_cnf_ptr->message_header.command_id,
      polling_cnf_ptr->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_polling_cnf_cleanup(polling_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_polling_cnf_cleanup(polling_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      polling_cnf_ptr->command_result,
      polling_cnf_ptr->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_polling_cnf_cleanup(polling_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* duration */
  if (polling_cnf_ptr->duration.present) {
    gstk_status = gstk_packer_duration_tlv(
      &polling_cnf_ptr->duration,
      &STK_response.duration);
  }


  /* total data length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
                      + duration data + 2
  */
  header_bytes = STK_response.header.command_details.length +
                 STK_response.header.device_id.device_tag_length +
                 STK_response.header.result.result_tag_length +
                 6;
  if (polling_cnf_ptr->duration.present) {
    STK_response.num_bytes = header_bytes +
                          STK_response.duration.length +
                          2;
  }
  else {
    STK_response.num_bytes = header_bytes;
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      if (uim_cmd_ptr != NULL) {
        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(polling_cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = gstk_curr_inst_ptr->client_id;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        /* copy command details */
        if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                   &STK_response.header.command_details,
                                   offset,
                                   (size_t)(STK_response.header.command_details.length + 2),
                                   sizeof(uim_cmd_ptr->terminal_response.data),
                                   sizeof(gstk_command_details_tlv_type)) >
           (size_t)(STK_response.header.command_details.length + 2))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy device */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.header.device_id,
                                        offset +=
                                        (int)(STK_response.header.command_details.length + 2),
                                        (size_t)(STK_response.header.device_id.device_tag_length + 2),
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_device_identities_tag_tlv_type)) <
                (size_t)(STK_response.header.device_id.device_tag_length + 2))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy result */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.header.result,
                                        offset +=
                                        (int)(STK_response.header.device_id.device_tag_length + 2),
                                        3,  /* result tag +  length + general result */
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_result_tag_tlv_type)) < 3)
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }

        offset = offset + 3;

        if(gstk_status == GSTK_SUCCESS &&
           STK_response.header.result.result_tag_length > 1 ) { /* has additional info */
          if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                     STK_response.header.result.additional_info,
                                     offset,
                                     (size_t)(STK_response.header.result.result_tag_length - 1),
                                     sizeof(uim_cmd_ptr->terminal_response.data),
                                     ((STK_response.header.result.result_tag_length - 1) *
                                      sizeof(byte))) <
             (size_t)(STK_response.header.result.result_tag_length - 1))
          {
            UIM_MSG_ERR_0("gstk_polling_cnf(): gstk_byte_offset_memcpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
          offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
        }

        /* copy duration */
        if(gstk_status == GSTK_SUCCESS &&
           polling_cnf_ptr->duration.present) {
          if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                     &STK_response.duration,
                                     header_bytes,
                                     STK_response.duration.length + 2,
                                     sizeof(uim_cmd_ptr->terminal_response.data),
                                     sizeof(gstk_duration_tlv_type)) <
             (size_t)(STK_response.duration.length + 2))
          {
            gstk_status = GSTK_MEMORY_ERROR;
          }
        }

        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_util_dump_byte_array("Polling TR",
                                    uim_cmd_ptr->terminal_response.data,
                                    uim_cmd_ptr->terminal_response.num_bytes);

          gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
        }
        else
        {
          gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
          uim_cmd_ptr = NULL;
        }
      }
    }
  } /* uim_cmd_ptr != NULL */
  else {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_polling_cnf_cleanup(polling_cnf_ptr, &STK_response);
  return gstk_status;

} /* gstk_polling_cnf*/

