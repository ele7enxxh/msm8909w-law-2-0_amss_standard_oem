/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 U I M   G S M   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the gsm protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_gsm_command
    Processes a gsm UIM server command

  uim_process_gsm_response
    Processes the responses for a gsm UIM server command

  uim_send_gsm_err_rpt
    Processes an error condition for a gsm UIM server command

  uim_gsm_command
    This function contains the state machine that processes APDUs
    for gsm commands.

  uim_gsm_command_response
    This function processes the UIM responses from APDUs.

Copyright (c) 2001-2006, 2009-2010-2014 by QUALCOMM Technologies, Inc.
All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimgsm.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/16   gm      F3 message optimization
07/31/14   ak      Feature flags cleanup
01/29/14   sam     Usage of newer style diag macros
10/24/13   akv     Removed unnecessary feature flags
10/03/13   ssr     F3 log reduction
09/24/13   nmb     Remove extra uim_free param
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
07/10/13   js      For reporting error set length of data buffer to zero for
                   authentication commands
06/24/13   js      TSTS changes
05/16/13   na      Secure memory copy
12/05/12   js      UIM Parallel Processing changes
10/26/12   akv     HW Enumeration changes (without features)
08/29/11   ssr     Fixed to add slot information for fetch command
11/10/10   yt      Memory reduction changes
09/01/10   ps      Merge UIM server DSDS changes
09/26/09   kp      ZI memory reduction changes
05/08/09   ps      Fixed 8200 compiler warnings
04/03/06   pv      Lint Fixes
11/14/05   pv      Lint Fixes
04/28/05   jk      Added support to properly transfer the user data from the
                   incoming command to the report
02/24/05   tml     Populate user data for Run GSM Algorithm command
05/17/04   ck      Moved the call to uim_send_gsm_err_rpt() from default case
                   in the function uim_process_gsm_command() to the end of the
                   function based on tbe status flag.
02/20/04   ts      Fixed lint errors
03/28/03   ts      Replaced ME_TO_UIM and UIM_TO_ME with instruction case in
                   UIM driver commands.
02/02/03   ck      Lint changes.
10/24/02   ts      Remove response timeout processing as timeout is handled
                   in the driver.
12/18/01   ck      Added the include file memory.h
12/04/01   ts      Added support for UIM toolkit.
05/25/01   ck      Added support for Multi mode server.
01/29/01   ck      Created module.  This module contains the gsm state
                   machine.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "comdef.h"
#include "uimdrv_msg.h"
#include "err.h"
#include "uimdrv.h"
#include "uimi.h"
#include "uimgsm.h"
#include <memory.h>

#include "uimglobals.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
              F U N C T I O N   P R O T O T Y P E S
===========================================================================*/
static void uim_process_gsm_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
);

static void uim_gsm_command
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_SEND_GSM_ERR_RPT

DESCRIPTION
  This function sends an error report for a gsm command. We pass cmd_ptr
  explicitly here since we may be sending err report for am unprocessed
  command that was present in command queue while processing hotswap
  card removal or card error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_gsm_err_rpt
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
)
{
  if (cmd_ptr == NULL)
  {
    return;
  }
  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = cmd_ptr->hdr.user_data;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
  uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;

  switch(cmd_ptr->hdr.command)
  {
    case UIM_RUN_GSM_ALGO_F:

      if (cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_RUN_GSM_ALGO_R;

        uim_ptr->command.rpt_buf.rpt.run_gsm.data_length = 0;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        UIMDRV_MSG_MED_0(uim_ptr->id, "Reporting error for RUN GSM ALGO " );
      }

      break;

    default:
      UIMDRV_MSG_ERR_1(uim_ptr->id,
                    "Unknown command 0x%x in uim_send_gsm_err_rpt",
                       cmd_ptr->hdr.command );
      break;
  }

  /* We only allocate cmd_ptr memory for the external commands that have
     valid callback. Internal UIM commands that use static buffers
     have callback set to NULL. */

  if (cmd_ptr->hdr.rpt_function != NULL)
  {
    if (cmd_ptr == uim_ptr->command.cmd_ptr)
    {
      /* If cmd_ptr is same as current command in progress set global
         cmd_ptr to NULL as well */
      uim_ptr->command.cmd_ptr = NULL;
    }
    UIM_FREE(cmd_ptr);
  }
} /* uim_send_gsm_err_rpt */


/*===========================================================================

FUNCTION UIM_PROCESS_GSM_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a gsm command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/

static void uim_process_gsm_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  if (uim_ptr->command.cmd_ptr == NULL  || rsp_ptr == NULL)
  {
    UIM_MSG_ERR_0(" Invalid input in uim_process_gsm_response");
    return;
  }


  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  uim_ptr->command.cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Set the status to UIM_PASS unless something failed */
  uim_ptr->command.rpt_buf.rpt_status = UIM_PASS;

  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_RUN_GSM_ALGO_F:           /* RUN GSM Algorithm */
    {
      /* Report back that Access was completed, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_RUN_GSM_ALGO_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the number of bytes in the response that we got
          from UIM into the command */
        uim_ptr->command.rpt_buf.rpt.run_gsm.data_length = rsp_ptr->cmd_rsp_size;

        /* copy the data into the buffer for response */
        uim_memscpy(uim_ptr->command.rpt_buf.rpt.run_gsm.data,
               sizeof(uim_ptr->command.rpt_buf.rpt.run_gsm.data),
               rsp_ptr->rsp.data,
               rsp_ptr->cmd_rsp_size);

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        UIMDRV_MSG_MED_0(uim_ptr->id, "Reporting RUN GSM Algo " );

      } /* (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL) */
    } /* end case - UIM_RUN_GSM_ALGO_F */
    break;

    default:
      UIMDRV_MSG_ERR_1(uim_ptr->id,
                  "Unknown command 0x%x in uim_process_gsm_response",
                        uim_ptr->command.cmd_ptr->hdr.command );
      break;
  }

  /* We only allocate cmd_ptr memory for the external commands that have
     valid callback. Internal UIM commands that use static buffers
     have callback set to NULL. */
  if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
  {
    UIM_FREE(uim_ptr->command.cmd_ptr);
  }

} /* uim_process_gsm_response */


/*===========================================================================

FUNCTION UIM_GSM_COMMAND

DESCRIPTION
  This procedure is the main state machine for GSM UIM command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/
static void uim_gsm_command
(
  uim_instance_global_type *uim_ptr
)
{
  uim_ptr->command.gsm_req_buf_ptr = &uim_ptr->command.req_buf;

  if (uim_ptr->command.cmd_ptr == NULL)
  {
    return;
  }
  /* Set the protocol from the command to the UIM request buffer */
  uim_ptr->command.gsm_req_buf_ptr->protocol = uim_ptr->command.cmd_ptr->hdr.protocol;

  switch(*uim_ptr->command.gsm_state)
  {
    case UIM_RUN_GSM_ALGORITHM_ST:              /* RUN GSM Algorithm state */
    {
      uim_ptr->command.gsm_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
      uim_ptr->command.gsm_req_buf_ptr->apdu_hdr.instrn = RUN_GSM_ALGORITHM;
      uim_ptr->command.gsm_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.gsm_req_buf_ptr->apdu_hdr.p1 = 0;
      uim_ptr->command.gsm_req_buf_ptr->apdu_hdr.p2 = 0;
      uim_ptr->command.gsm_req_buf_ptr->apdu_hdr.p3 = UIM_P3_RUN_GSM_ALGORITHM;
                    /* bytes of data sent in the command */

      uim_ptr->command.gsm_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      /* Copy the 16 bytes of rand from the command */
      uim_memscpy((byte*) uim_ptr->command.gsm_req_buf_ptr->req.data,
              sizeof(uim_ptr->command.gsm_req_buf_ptr->req.data),
              uim_ptr->command.cmd_ptr->run_gsm.rand,
              UIM_P3_RUN_GSM_ALGORITHM);

      uim_ptr->command.gsm_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.gsm_req_buf_ptr, uim_ptr);
    }
    break;

    case UIM_GSM_DONE_ST:                         /* Done State */
    {
      uim_process_gsm_response(&uim_ptr->command.rsp_buf, uim_ptr);
    } /* end case - UIM_DONE_ST */
    break;

    default:
      break;

  } /* end of main switch */

  UIMDRV_MSG_LOW_1(uim_ptr->id, "UIM state in uim_gsm_command %ld",
                   uim_ptr->command.gsm_state);
} /* uim_gsm_command */


/*===========================================================================

FUNCTION UIM_PROCESS_GSM_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM state variable amongst others.

===========================================================================*/
boolean uim_process_gsm_command
(
  uim_instance_global_type *uim_ptr
)
{
  /* This indicates that the command is processed by the state machine */
  boolean status = TRUE;

  if (uim_ptr->command.cmd_ptr == NULL)
  {
    return FALSE;
  }

  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = uim_ptr->command.cmd_ptr->hdr.user_data;

  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_RUN_GSM_ALGO_F:            /* RUN GSM Algorithm */
    {
      UIMDRV_MSG_MED_0(uim_ptr->id, "Received RUN GSM ALGORITHM command " );

      /* Build the RUN GSM list of states. */
      uim_ptr->mode.gsm_states[0] = UIM_RUN_GSM_ALGORITHM_ST;
      uim_ptr->mode.gsm_states[1] = UIM_GSM_DONE_ST;
      uim_ptr->command.gsm_state = uim_ptr->mode.gsm_states;

    } /* end case - UIM_RUN_GSM_ALGORITHM_F */
    break;

    default:
      status = FALSE;
      break;
  }

  if (status)
  {
    /* Call the state machine. */
    uim_gsm_command (uim_ptr);
  }
  else
  {
    /* Send an error report to the client and release the buffer */
    uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
    uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;

    uim_send_gsm_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
  }

  return(status);
} /* uim_process_gsm_command() */


/*===========================================================================

FUNCTION UIM_GSM_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a gsm comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_ptr->command.gsm_state.

===========================================================================*/
uim_cmd_status_type uim_gsm_command_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  uim_cmd_status_type status;
  /* check to see what the status from the UIM is */

  uim_gsm_state_type curr_uim_gsm_state;
  /* Normally, the status when a command is completed is UIM_CMD_COMPLETED
     This is changed if there is a proactive command pending. */
  uim_cmd_status_type completed_status = UIM_CMD_COMPLETED;

  status = UIM_CMD_SUCCESS;

  if (rsp_ptr == NULL)
  {
    return UIM_CMD_ERR;	
  }

  /* Fill the status words into the report buffer. This could be used for
     both passed and failed reports. If the command is not sent to the
     card, then the status words are cleared before sending the report */

  uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) rsp_ptr->sw1;
  uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) rsp_ptr->sw2;

  /* Change fetch responses to normal end responses */
  if (SW1_END_FETCH == uim_ptr->command.rpt_buf.sw1)
  {
    uim_ptr->command.rpt_buf.sw1 = SW1_NORMAL_END;
    uim_ptr->command.rpt_buf.sw2 = SW2_NORMAL_END;
    /* If the command is completed, indicate there is a proactive command
       pending. */
    completed_status = UIM_CMD_FETCH;
    /* Set the protocol for the Internal FETCH command based on the last
       successful command */
    uim_ptr->command.static_cmd_buf.hdr.protocol = uim_ptr->command.cmd_ptr->hdr.protocol;

  } /* end if - fetch response */

  /* Necessary so that we do not change the uim state and switch into that
     case also */
  curr_uim_gsm_state = *uim_ptr->command.gsm_state;

  switch (curr_uim_gsm_state)
  {
    case UIM_RUN_GSM_ALGORITHM_ST:        /* Run GSM Algorithm state */
    {
      /* Return an error status if the command failed. */
      if (rsp_ptr->cmd_status != UIM_DONE)
      {
        status = UIM_CMD_ERR;
      }
      else
      {
        /* Skip to the next state. */
        ++uim_ptr->command.gsm_state;
      }
    } /* end case - UIM_RUN_GSM_ALGO_ST */
    break;

    default:
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Invalid State in uim_gsm_command_response " );
      status = UIM_CMD_ERR;
      break;
  }

  if (status == UIM_CMD_SUCCESS)
  {
    /* Process the next state of the command */
    uim_gsm_command(uim_ptr);
  }

  if (UIM_GSM_DONE_ST == *uim_ptr->command.gsm_state)
  {
    /* Indicate that the command has been completed */
    return (completed_status);
  }
  return(status);
} /* uim_gsm_command_response */
