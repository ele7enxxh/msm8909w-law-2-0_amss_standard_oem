/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 U I M   U S I M   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the ISIM protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_isim_command
    Processes a isim UIM server command

  uim_process_isim_response
    Processes the responses for a isim UIM server command

  uim_send_isim_err_rpt
    Processes an error condition for a isim UIM server command

  uim_isim_command
    This function contains the state machine that processes APDUs
    for isim commands.

  uim_isim_command_response
    This function processes the UIM responses from APDUs.

Copyright (c) 2001, 2005-2006, 2009-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimisim.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/14   ak      Feature flags cleanup
03/14/14   nmb     Correct Authentication states
02/10/14   sam     Fix for klocwork issues
01/29/14   sam     Usage of newer style diag macros
02/02/14   nmb     Directory Maintenance changes
01/09/14   na      Resetting of directory hierarchy after ADF selection
09/26/13   ak      Remove unnecessary feature flags
09/24/13   nmb     Remove extra uim_free param
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
07/10/13   js      For reporting error set length of data buffer to zero for
                   authentication commands
06/24/13   js      TSTS changes
05/16/13   na      Secure memory copy
02/12/13   js      General F3 macro cleanup
12/17/12   js      Fixed KW Errors
12/05/12   js      UIM Parallel Processing changes
10/26/12   akv     HW Enumeration changes (Feature removal)
08/13/12   js      Do SELECT on ISIM ADF before Auth
06/01/12   vv      Fix the output message
08/29/11   ssr     Fixed to add slot information for fetch command
08/11/11   nmb     Expanded Logical Channels
05/13/11   nmb     Added support for ISIM GBA Authentication
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/10/10   yt      Memory reduction changes
09/01/10   ps      Merge UIM server DSDS changes
02/18/10   vs      Updates for multimode UICC
10/07/09   yb      Klockwork fix
09/26/09   kp      ZI memory reduction changes
05/02/09   ps      Fix 8200 Warnings
04/03/06   pv      Lint Fixes
05/16/05   pv      Created module.  This module contains the ISIM state
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
#include "uimisim.h"
#include "uimutil.h"
#include <memory.h>
#include "uimglobals.h"


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
  I N T E R N A L   F U N C T I O N   P R O T O T Y P E S
===========================================================================*/
static void uim_process_isim_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
);

static void uim_isim_command
(
  uim_instance_global_type *uim_ptr
);

extern uim_cmd_status_type uim_update_last_sel_dir
(
  uim_rsp_buf_type const *rsp_ptr,
  word                    path,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_SEND_ISIM_ERR_RPT

DESCRIPTION
  This function sends an error report for a isim command. We pass cmd_ptr
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
void uim_send_isim_err_rpt
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
)
{
  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_send_isim_err_rpt cmd_ptr is NULL",0,0,0);
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
    case UIM_ISIM_AUTHENTICATE_F:

      if (cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_ISIM_AUTHENTICATE_R;

        uim_ptr->command.rpt_buf.rpt.isim_autn.data_length = 0;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        UIMDRV_MSG_MED_0(uim_ptr->id, "Reporting error for ISIM Authenticate cmd " );
      }

     break;

     default:
       UIMDRV_MSG_ERR_1(uim_ptr->id,
                        "Unknown command 0x%x in uim_send_isim_err_rpt", cmd_ptr->hdr.command );
       break;
  }

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
} /* uim_send_isim_err_rpt */


/*===========================================================================

FUNCTION UIM_PROCESS_ISIM_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a isim command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
static void uim_process_isim_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  uim_ptr->command.cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Set the status to UIM_PASS unless something failed */
  uim_ptr->command.rpt_buf.rpt_status = UIM_PASS;

  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_ISIM_AUTHENTICATE_F:           /* Authenticate for ISIM */
    {
      /* Report back that Access was completed, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_ISIM_AUTHENTICATE_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the number of bytes in the response that we got
          from UIM into the command */
        uim_ptr->command.rpt_buf.rpt.isim_autn.data_length = rsp_ptr->cmd_rsp_size;

        /* copy the data into the buffer for response */
        uim_memscpy(uim_ptr->command.rpt_buf.rpt.isim_autn.data,
               sizeof(uim_ptr->command.rpt_buf.rpt.isim_autn.data),
               rsp_ptr->rsp.data,
               rsp_ptr->cmd_rsp_size);

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        UIMDRV_MSG_MED_0(uim_ptr->id, "Reporting Authenticate for ISIM " );

      } /* (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL) */
    } /* end case - ISIM_AUTHENTICATE_F */
    break;
    default:
      UIMDRV_MSG_ERR_1(uim_ptr->id,
                    "Unknown command 0x%x in uim_process_isim_response",
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
} /* uim_process_isim_response */



/*===========================================================================

FUNCTION UIM_ISIM_COMMAND

DESCRIPTION
  This procedure is the main state machine for ISIM UIM command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/
static void uim_isim_command
(
  uim_instance_global_type *uim_ptr
)
{
  uim_ptr->command.isim_req_buf_ptr = &uim_ptr->command.req_buf;

  if(NULL == uim_ptr->command.cmd_ptr)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id, "uim_isim_command: pointer is NULL");
    return;
  }

  /* Set the protocol from the command to the UIM request buffer */
  uim_ptr->command.isim_req_buf_ptr->protocol = uim_ptr->command.cmd_ptr->hdr.protocol;

  switch(*uim_ptr->command.isim_state)
  {
    case UIM_ISIM_AUTHENTICATE_ST:              /* Authenticate for ISIM */
    {
      uim_ptr->command.isim_req_buf_ptr->apdu_hdr.uim_class =
        uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
      uim_ptr->command.isim_req_buf_ptr->apdu_hdr.instrn = ISIM_AUTHENTICATE;
      uim_ptr->command.isim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;

      /* Indicates that no info on the algorithm is given. The algo is
         implicitly known in the context of the selected application */
      uim_ptr->command.isim_req_buf_ptr->apdu_hdr.p1 = 0;

      /* b8 of p2 should always be 1 for ISIM authenticate contexts */
      uim_ptr->command.isim_req_buf_ptr->apdu_hdr.p2 = UIM_ISIM_AUTN_SPECIFIC_REF_DATA_P2;

      /* b1-b3:
         000 Reserved
         001 IMS AKA
         010 HTTP Digest
         100 GBA CONTEXT */
      uim_ptr->command.isim_req_buf_ptr->apdu_hdr.p2 |= ((byte) uim_ptr->command.cmd_ptr->isim_autn.ref_data_number &
                                            UIM_ISIM_AUTN_REF_DATA_NUMBER_MASK);

      uim_ptr->command.isim_req_buf_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->isim_autn.num_bytes;
                    /* bytes of data sent in the command */

      uim_ptr->command.isim_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      /* Copy the data from the command for the specificed number of bytes */
      uim_memscpy((byte*) uim_ptr->command.isim_req_buf_ptr->req.data,
              sizeof(uim_ptr->command.isim_req_buf_ptr->req.data),
              uim_ptr->command.cmd_ptr->isim_autn.data,
              uim_ptr->command.cmd_ptr->isim_autn.num_bytes);

      uim_ptr->command.isim_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.isim_req_buf_ptr, uim_ptr);
    }
    break;

    case UIM_ISIM_DONE_ST:                         /* Done State */
    {
      uim_process_isim_response(&uim_ptr->command.rsp_buf,uim_ptr);
    } /* end case - UIM_DONE_ST */
      break;

    default:
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"Invalid state in uim_isim_command %ld",
                        *uim_ptr->command.isim_state);
      break;
  } /* end of main switch */

  UIMDRV_MSG_LOW_1(uim_ptr->id,"UIM state in uim_isim_command %ld",
                   *uim_ptr->command.isim_state);

} /* uim_isim_command */


/*===========================================================================

FUNCTION UIM_PROCESS_ISIM_COMMAND

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
boolean uim_process_isim_command
(
  uim_instance_global_type *uim_ptr
)
{
  /* This indicates that the command is processed by the state machine */
  boolean status = TRUE;

  if(uim_ptr->command.cmd_ptr == NULL)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"cmd_ptr is NULL");
    return FALSE;
  }

  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = uim_ptr->command.cmd_ptr->hdr.user_data;

  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_ISIM_AUTHENTICATE_F:            /* Authenticate for ISIM */
    {
      UIMDRV_MSG_MED_0(uim_ptr->id, "Received ISIM Authenticate command ");

      /* Build the ISIM Authenticate list of states. */
      uim_ptr->mode.isim_states[0] = UIM_ISIM_AUTHENTICATE_ST;
      uim_ptr->mode.isim_states[1] = UIM_ISIM_DONE_ST;
      uim_ptr->command.isim_state  = uim_ptr->mode.isim_states;
    } /* end case - UIM_ISIM_AUTHENTICATE_F */
    break;

    default:
      status = FALSE;
      UIMDRV_MSG_ERR_1(uim_ptr->id,
                    "Unrecognized cmd recd in ISIM Module 0x%x",
                        uim_ptr->command.cmd_ptr->hdr.command);

      break;
  }

  if (status)
  {
    /* Call the state machine. */
    uim_isim_command (uim_ptr);
  }
  else
  {
    /* Send an error report to the client and release the buffer */
    uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
    uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;
    uim_send_isim_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
  }
  return(status);

} /* uim_process_isim_command() */


/*===========================================================================

FUNCTION UIM_ISIM_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a isim comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_isim_state.

===========================================================================*/

uim_cmd_status_type uim_isim_command_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  uim_cmd_status_type status;
  /* check to see what the status from the UIM is */

  uim_isim_state_type curr_uim_isim_state;
  /* Normally, the status when a command is completed is UIM_CMD_COMPLETED
     This is changed if there is a proactive command pending. */
  uim_cmd_status_type completed_status = UIM_CMD_COMPLETED;

  status = UIM_CMD_SUCCESS;

  /* Fill the status words into the report buffer. This could be used for
     both passed and failed reports. If the command is not sent to the
     card, then the status words are cleared before sending the report */

  uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type)rsp_ptr->sw1;
  uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type)rsp_ptr->sw2;

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
  curr_uim_isim_state = *uim_ptr->command.isim_state;

  switch (curr_uim_isim_state)
  {
    case UIM_ISIM_AUTHENTICATE_ST:        /* ISIM Authenticate state */
    {
      /* Return an error status if the command failed. */
      if (rsp_ptr->cmd_status != UIM_DONE)
      {
        status = UIM_CMD_ERR;
      }
      else
      {
        /* Skip to the next state. */
        ++uim_ptr->command.isim_state;
      }
    } /* end case - UIM_ISIM_AUTHENTICATE_ST */
    break;

    default:
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Invalid State in uim_isim_command_response " );
      status = UIM_CMD_ERR;
      break;
  }
  if (status == UIM_CMD_SUCCESS)
  {
    /* Process the next state of the command */
    uim_isim_command(uim_ptr);
  }

  if (UIM_ISIM_DONE_ST == *uim_ptr->command.isim_state)
  {
    /* Indicate that the command has been completed */
    return (completed_status);
  }

  return(status);
} /* uim_isim_command_response */


