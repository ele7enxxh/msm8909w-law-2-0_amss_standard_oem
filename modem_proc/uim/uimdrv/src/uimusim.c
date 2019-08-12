/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 U I M   U S I M   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the USIM protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_usim_command
    Processes a usim UIM server command

  uim_process_usim_response
    Processes the responses for a usim UIM server command

  uim_send_usim_err_rpt
    Processes an error condition for a usim UIM server command

  uim_usim_command
    This function contains the state machine that processes APDUs
    for usim commands.

  uim_usim_command_response
    This function processes the UIM responses from APDUs.

Copyright (c) 2001-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimusim.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/16   na      Removed F3 traces
08/25/14   ll      Fix compile warnings medium
07/31/14   ak      Feature flags cleanup
02/18/14   sam     Usage of newer style diag macros
02/02/14   nmb     Directory Maintenance Changes
09/26/13   ak      Remove unnecessary feature flags
09/24/13   nmb     Remove extra uim_free param
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
07/10/13   js      For reporting error set length of data buffer to zero for
                   authentication commands
06/24/13   js      TSTS changes
06/17/13   ks      Removal of F3 messages as part of memory optimization
05/16/13   na      Secure memory copy
02/12/13   js      General F3 macro cleanup
12/17/12   js      Fixed KW error
12/05/12   js      UIM Parallel Processing changes
10/26/12   akv     HW Enumeration changes (without features)
03/30/12   av      Replace feature flags with NVITEM
03/15/12   rm      Changes for DIME
10/12/11   ms      Fixed to add slot information for fetch command
08/11/11   nmb     Expanded Logical Channels
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/10/10   yt      Memory reduction changes
09/01/10   ps      Merge UIM server DSDS changes
22/03/10   kp      Replaced UIM_UICC_SELECT_BY_DF_NAME with
                   UIM_USIM_SELECT_BY_DF_NAME
02/18/10   vs      Updates for multimode UICC
16/02/10   kp      Replaced UIM_USIM_SELECT_BY_DF_NAME with
                   UIM_UICC_SELECT_BY_DF_NAME
10/09/09   yb      Klockwork fix
10/07/09   kp      Fix for the error fatal
09/26/09   kp      ZI memory reduction changes
09/16/09   yb      Klockwork FIX
05/02/09   ps      Fix 8200 Warnings
01/12/09   sun     Added a new state - UIM_SELECT_ADF_ST before ODD INS
                   auth
09/18/08   nb      Lint Fixes
03/03/08   sun     Added support for OMA BCAST
02/25/08   sun     Do not change state if the command fails.
08/14/07   sun     Fixed Report Type for ODD_INS
07/05/07   sp      Send out a status command before each USIM authenticate.
07/02/07   sun     Added support for ODD INS Class
04/03/06   pv      Lint Fixes
05/10/05   sst     Lint fixes
04/28/05   jk      Added support to properly transfer the user data from the
                   incoming command to the report
02/24/05   tml     Populate user data for Authentication command
05/17/04   ck      Moved the call to uim_send_usim_err_rpt() from default case
                   in the function uim_process_usim_command() to the end of the
                   function based on tbe status flag.
03/28/03   ts      Replaced ME_TO_UIM and UIM_TO_ME with instruction case in
                   UIM driver commands.
02/02/03   ck      Lint mods.
01/17/03   ck      Included comdef.h and memory.h to avoid compiler warnings
10/24/02   ts      Removed response timeout processing as timeouts are
                   handled in the driver.
02/29/01   ck      Created module.  This module contains the USIM state
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
#include "assert.h"
#include "uimdrv.h"
#include "uimi.h"
#include "uimusim.h"
#include "uimgen.h"
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
static void uim_process_usim_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
);

static void uim_usim_command
(
  uim_instance_global_type *uim_ptr
);

static boolean uim_alloc_set_auth_info
(
  word                      num_bytes_to_alloc,
  uim_instance_global_type *uim_ptr
);



/*===========================================================================

FUNCTION UIM_FREE_RESET_AUTH_INFO

DESCRIPTION
  Frees the auth info memory

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_free_reset_auth_info
(
  uim_instance_global_type *uim_ptr
)
{

  if ( uim_ptr->command.auth_info.alloc_mem != NULL )
  {
    UIM_FREE(uim_ptr->command.auth_info.alloc_mem);
  }

  uim_ptr->command.auth_info.data_ptr      = NULL;
  uim_ptr->command.auth_info.num_bytes_rsp = 0;
  uim_ptr->command.auth_info.num_bytes     = 0;
  uim_ptr->command.auth_info.offset        = 0;

}/* uim_free_reset_auth_info */



/*===========================================================================

FUNCTION UIM_SEND_USIM_ERR_RPT

DESCRIPTION
  This function sends an error report for a usim command. We pass cmd_ptr
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
void uim_send_usim_err_rpt
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
)
{
  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_send_usim_err_rpt cmd_ptr is null",0,0,0);
  }
  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = cmd_ptr->hdr.user_data;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
  uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;
  UIMDRV_MSG_HIGH_1(uim_ptr->id, "Reporting error for USIM command 0x%x ",
                    cmd_ptr->hdr.command );

  switch(cmd_ptr->hdr.command)
  {
    case UIM_AUTHENTICATE_F:

      if (cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_AUTHENTICATE_R;

        uim_ptr->command.rpt_buf.rpt.autn.data_length = 0;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      }

      break;

     case UIM_AUTHENTICATE_ODD_INS_F:
       if (cmd_ptr->hdr.rpt_function != NULL)
       {
         uim_ptr->command.rpt_buf.rpt_type = UIM_AUTHENTICATE_ODD_INS_R;

         /* Fill in necessary fields of command block */
         uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

         uim_ptr->command.rpt_buf.rpt.autn_odd_ins.len = uim_ptr->command.auth_info.num_bytes_rsp;

         uim_ptr->command.rpt_buf.rpt.autn_odd_ins.data = uim_ptr->command.auth_info.alloc_mem;

         /* Report to requesting task */
         (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

         uim_free_reset_auth_info(uim_ptr);
       }
       break;

     case UIM_BCAST_F:
       if (cmd_ptr->hdr.rpt_function != NULL)
       {
         uim_ptr->command.rpt_buf.rpt_type = UIM_BCAST_R;

         /* Fill in necessary fields of command block */
         uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

         uim_ptr->command.rpt_buf.rpt.bcast.len = uim_ptr->command.auth_info.num_bytes_rsp;

         uim_ptr->command.rpt_buf.rpt.bcast.data = uim_ptr->command.auth_info.alloc_mem;

         /* Report to requesting task */
         (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

          uim_free_reset_auth_info(uim_ptr);
        }
        break;

     default:
       UIMDRV_MSG_ERR_1(uim_ptr->id,
                     "Unknown command 0x%x in uim_send_usim_err_rpt",
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
} /* uim_send_usim_err_rpt */


/*===========================================================================

FUNCTION UIM_PROCESS_USIM_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a usim command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
static void uim_process_usim_response
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

  UIMDRV_MSG_MED_1(uim_ptr->id, "Reporting Authenticate for command 0x%x",
                   uim_ptr->command.cmd_ptr->hdr.command );

  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_AUTHENTICATE_F:           /* Authenticate for USIM */
    {
      /* Report back that Access was completed, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_AUTHENTICATE_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the number of bytes in the response that we got
          from UIM into the command */
        uim_ptr->command.rpt_buf.rpt.autn.data_length = rsp_ptr->cmd_rsp_size;

        /* copy the data into the buffer for response */
        uim_memscpy(uim_ptr->command.rpt_buf.rpt.autn.data,
               sizeof(uim_ptr->command.rpt_buf.rpt.autn.data),
               rsp_ptr->rsp.data,
               rsp_ptr->cmd_rsp_size);

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

      } /* (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL) */
    } /* end case - USIM_AUTHENTICATE_F */
    break;

    case UIM_AUTHENTICATE_ODD_INS_F:           /* Authenticate for ODD INS */
    {
      /* Report back that Access was completed, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_AUTHENTICATE_ODD_INS_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the number of bytes in the response that we got
          from UIM into the command */
        uim_ptr->command.rpt_buf.rpt.autn_odd_ins.len = uim_ptr->command.auth_info.num_bytes_rsp;

        /* copy the data into the buffer for response */
        uim_ptr->command.rpt_buf.rpt.autn_odd_ins.data = uim_ptr->command.auth_info.alloc_mem;

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

        /* Release the allocated memory for auth commands */
        uim_free_reset_auth_info(uim_ptr);

      } /* (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL) */
    } /* end case - UIM_AUTHENTICATE_ODD_INS_F */
    break;

    case UIM_BCAST_F:           /* BCAST for SCP */
    {
      /* Report back that Bcast Cmd was completed */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_BCAST_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the number of bytes in the response that we got
          from UIM into the command */
        uim_ptr->command.rpt_buf.rpt.bcast.len = uim_ptr->command.auth_info.num_bytes_rsp;

        /* copy the data into the buffer for response */
        uim_ptr->command.rpt_buf.rpt.bcast.data = uim_ptr->command.auth_info.alloc_mem;

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

        /* Release the allocated memory for auth commands */
        uim_free_reset_auth_info(uim_ptr);
      } /* (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL) */
    } /* end case - UIM_BCAST_F */
    break;

    default:
       UIMDRV_MSG_ERR_1(uim_ptr->id, "Unknown command 0x%x in uim_process_usim_response",
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
} /* uim_process_usim_response */



/*===========================================================================

FUNCTION UIM_USIM_COMMAND

DESCRIPTION
  This procedure is the main state machine for USIM UIM command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/
static void uim_usim_command
(
  uim_instance_global_type *uim_ptr
)
{
  if(NULL == uim_ptr->command.cmd_ptr)
  {
    return;
  }

  uim_ptr->command.usim_req_buf_ptr = &uim_ptr->command.req_buf;

  /* Set the protocol from the command to the UIM request buffer */
  uim_ptr->command.usim_req_buf_ptr->protocol = uim_ptr->command.cmd_ptr->hdr.protocol;

  switch(*uim_ptr->command.usim_state)
  {
    case UIM_AUTHENTICATE_ST:              /* Authenticate for USIM */
    {
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.uim_class =
        uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.instrn = AUTHENTICATE;
      uim_ptr->command.usim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;

      /* Indicates that no info on the algorithm is given. The algo is
         implicitly known in the context of the selected application */
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p1 = 0;

      /* Clear P2 to begin with*/
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = 0;

      if (uim_ptr->command.cmd_ptr->autn.ref_data_type == UIM_AUTN_GLOBAL_REF_DATA)
      {
         uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = UIM_AUTN_GLOBAL_REF_DATA_P2;
      }
      else if (uim_ptr->command.cmd_ptr->autn.ref_data_type == UIM_AUTN_SPECIFIC_REF_DATA)
      {
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = UIM_AUTN_SPECIFIC_REF_DATA_P2;
      }

      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 |= (uim_ptr->command.cmd_ptr->autn.ref_data_number &
                                            UIM_AUTN_REF_DATA_NUMBER_MASK);

      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->autn.num_bytes;
                    /* bytes of data sent in the command */

      uim_ptr->command.usim_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      /* Copy the data from the command for the specificed number of bytes */
      uim_memscpy(uim_ptr->command.usim_req_buf_ptr->req.data,
                  sizeof(uim_ptr->command.usim_req_buf_ptr->req.data),
                  uim_ptr->command.cmd_ptr->autn.data,
                  uim_ptr->command.cmd_ptr->autn.num_bytes);

      uim_ptr->command.usim_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.usim_req_buf_ptr, uim_ptr);
    }
    break;

    case UIM_AUTHENTICATE_ODD_INS_ST:
    {
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.uim_class =
         uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
      uim_ptr->command.usim_req_buf_ptr->instrn_case        = UIM_INSTRN_CASE_3;
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.instrn    = AUTHENTICATE_ODD_INS;

      /* Indicates that no info on the algorithm is given. The algo is
         implicitly known in the context of the selected application */
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p1 = 0;

      if(uim_ptr->command.auth_info.offset == 0)
      {
        /*This is the first block of data*/
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p1 = UIM_AUTH_FIRST_CMD_BLOCK;
      }

      if (UIM_MAX_TRANSACTION_SIZE <= uim_ptr->command.auth_info.num_bytes)
      {
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_TRANS_SIZE_OUT;
      }
      else
      {
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3 = WORD_LO (uim_ptr->command.auth_info.num_bytes);
      }

      /* Copy the data from the command for the specificed number of bytes */
      uim_memscpy(uim_ptr->command.usim_req_buf_ptr->req.data,
                  sizeof(uim_ptr->command.usim_req_buf_ptr->req.data),
                  uim_ptr->command.cmd_ptr->autn_odd_ins.data,
                  uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3);

      /* Clear P2 to begin with*/
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = 0;

      if (uim_ptr->command.cmd_ptr->autn_odd_ins.ref_data_type == UIM_AUTN_GLOBAL_REF_DATA)
      {
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = UIM_AUTN_GLOBAL_REF_DATA_P2;
      }
      else if (uim_ptr->command.cmd_ptr->autn_odd_ins.ref_data_type == UIM_AUTN_SPECIFIC_REF_DATA)
      {
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = UIM_AUTN_SPECIFIC_REF_DATA_P2;
      }
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 |= (uim_ptr->command.cmd_ptr->autn_odd_ins.ref_data_number &
                                           UIM_AUTN_REF_DATA_NUMBER_MASK);

      uim_ptr->command.usim_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.usim_req_buf_ptr, uim_ptr);
    }
    break;

    case UIM_AUTHENTICATE_ODD_INS_RSP_ST:
    {
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.uim_class =
        uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
      uim_ptr->command.usim_req_buf_ptr->instrn_case        = UIM_INSTRN_CASE_2;
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.instrn    = AUTHENTICATE_ODD_INS;

      /* Indicates that no info on the algorithm is given. The algo is
         implicitly known in the context of the selected application */
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p1 = 0;

      if(uim_ptr->command.auth_info.offset == 0)
      {
        /*This is the first block of data*/
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p1 = UIM_AUTH_FIRST_RSP_BLOCK;
      }
      else
      {
        /*This is the next block of data*/
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p1 = UIM_AUTH_NEXT_RSP_BLOCK;
      }

      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3 = WORD_LO(uim_ptr->command.auth_info.num_bytes);
      /* bytes of data sent in the command */

      /* Clear P2 to begin with*/
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = 0;

      if (uim_ptr->command.cmd_ptr->autn_odd_ins.ref_data_type == UIM_AUTN_GLOBAL_REF_DATA)
      {
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = UIM_AUTN_GLOBAL_REF_DATA_P2;
      }
      else if (uim_ptr->command.cmd_ptr->autn_odd_ins.ref_data_type == UIM_AUTN_SPECIFIC_REF_DATA)
      {
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = UIM_AUTN_SPECIFIC_REF_DATA_P2;
      }

      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 |= (uim_ptr->command.cmd_ptr->autn_odd_ins.ref_data_number &
                                            UIM_AUTN_REF_DATA_NUMBER_MASK);

      uim_ptr->command.usim_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_ptr->command.usim_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.usim_req_buf_ptr, uim_ptr);
    }
    break;

    case UIM_BCAST_ST:
    {
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.uim_class =
         uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
      uim_ptr->command.usim_req_buf_ptr->instrn_case        = UIM_INSTRN_CASE_3;

      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.instrn    = BCAST_INS;

      /* Indicates that no info on the algorithm is given. The algo is
         implicitly known in the context of the selected application */
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p1 = 0;

      /*for UIM_BCAST_SPE_SIG_MODE, P1 is always set to 0*/
      if(uim_ptr->command.auth_info.offset == 0 &&
         uim_ptr->command.cmd_ptr->bcast.ref_data_number != UIM_BCAST_SPE_SIG_MODE)
      {
        /*This is the first block of data*/
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p1 = UIM_AUTH_FIRST_CMD_BLOCK;
      }

      if (UIM_MAX_TRANSACTION_SIZE <= uim_ptr->command.auth_info.num_bytes)
      {
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_TRANS_SIZE_OUT;
      }
      else
      {
        uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3 = WORD_LO (uim_ptr->command.auth_info.num_bytes);
      }

      uim_memscpy(uim_ptr->command.usim_req_buf_ptr->req.data,
                  sizeof(uim_ptr->command.usim_req_buf_ptr->req.data),
                  uim_ptr->command.cmd_ptr->bcast.data,
                  uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3);

      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2 = uim_ptr->command.cmd_ptr->bcast.ref_data_number;
      uim_ptr->command.usim_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;
      uim_ptr->command.usim_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;
      uim_send_command(uim_ptr->command.usim_req_buf_ptr, uim_ptr);
    }
    break;

    case UIM_STATUS_BEFORE_USIM_AUTH_ST:
    {
      if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_SEND_STATUS_BEFORE_AUTH,
                                   uim_ptr) == FALSE)
      {
        break;
      }

      /* Get the status (last selected DF) of the UIM card */
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.uim_class =
        uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.instrn    = STATUS;
      uim_ptr->command.usim_req_buf_ptr->instrn_case        = UIM_INSTRN_CASE_2;

      /* CAN CHOOSE TO HARD CODE OTHER VALUES FOR P1 INSTEAD */
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p1        = UIM_STATUS_P1_NO_INDICATION;

      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p2        = UIM_STATUS_P2_RETURN_NONE;

      /* IF P2 IS SET TO OTHER VALUES, P3 NEEDS TO BE 255 */
      uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3        = 0;
      uim_ptr->command.usim_req_buf_ptr->rsp_ptr            = &uim_ptr->command.rsp_buf;
      uim_send_command(uim_ptr->command.usim_req_buf_ptr, uim_ptr);
    } /* case UIM_STATUS_BEFORE_USIM_AUTH_ST */
    break;

    case UIM_USIM_DONE_ST:                         /* Done State */
    {
      uim_process_usim_response(&uim_ptr->command.rsp_buf, uim_ptr);
    } /* end case - UIM_DONE_ST */
      break;

    default:
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"Invalid state in uim_usim_command %ld",
                        *uim_ptr->command.usim_state);
      break;

  } /* end of main switch */

  UIMDRV_MSG_LOW_1(uim_ptr->id,"UIM state in uim_usim_command %ld",
                   *uim_ptr->command.usim_state);
} /* uim_usim_command */


/*===========================================================================

FUNCTION UIM_ALLOC_SET_AUTH_INFO

DESCRIPTION
  Allocates memory for auth info and frees any from before

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean uim_alloc_set_auth_info
(
  word                      num_bytes_to_alloc,
  uim_instance_global_type *uim_ptr
)
{
  uim_free_reset_auth_info(uim_ptr);

  if (num_bytes_to_alloc == 0 )
  {
    return FALSE;
  }

  uim_ptr->command.auth_info.alloc_mem =
    uim_malloc(num_bytes_to_alloc);

  uim_ptr->command.auth_info.data_ptr =
    uim_ptr->command.auth_info.alloc_mem;

  uim_ptr->command.auth_info.num_bytes =
    num_bytes_to_alloc;

  return ( uim_ptr->command.auth_info.alloc_mem != NULL );
}/* uim_alloc_set_auth_info */


/*===========================================================================

FUNCTION UIM_PROCESS_USIM_COMMAND

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
boolean uim_process_usim_command
(
  uim_instance_global_type *uim_ptr
)
{
  /* This indicates that the command is processed by the state machine */
  boolean status = TRUE;

  if(NULL == uim_ptr->command.cmd_ptr)
  {
    ERR_FATAL("uim_process_usim_command:pointer is NULL",0,0,0);
  }

  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = uim_ptr->command.cmd_ptr->hdr.user_data;
  UIMDRV_MSG_HIGH_1(uim_ptr->id, "Received USIM command 0x%x ",
                        uim_ptr->command.cmd_ptr->hdr.command );

  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_AUTHENTICATE_F:            /* Authenticate for USIM */
    {
      /* Build the USIM Authenticate list of states. */
      if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_SEND_STATUS_BEFORE_AUTH,
                                   uim_ptr) == TRUE)
      {
        uim_ptr->mode.usim_states[0] = UIM_STATUS_BEFORE_USIM_AUTH_ST;
        uim_ptr->mode.usim_states[1] = UIM_AUTHENTICATE_ST;
        uim_ptr->mode.usim_states[2] = UIM_USIM_DONE_ST;
        uim_ptr->command.usim_state = uim_ptr->mode.usim_states;
      }
      else
      {
        uim_ptr->mode.usim_states[0] = UIM_AUTHENTICATE_ST;
        uim_ptr->mode.usim_states[1] = UIM_USIM_DONE_ST;
        uim_ptr->command.usim_state = uim_ptr->mode.usim_states;
      }
    } /* end case - UIM_AUTHENTICATE_F */
    break;

    case UIM_AUTHENTICATE_ODD_INS_F:
    {
      /* Build the USIM Authenticate list of states. */
      uim_ptr->mode.usim_states[1] = UIM_AUTHENTICATE_ODD_INS_ST;
      uim_ptr->mode.usim_states[2] = UIM_USIM_DONE_ST;
      uim_ptr->command.usim_state = uim_ptr->mode.usim_states;

      /* Try to allocate mem and reset all the variables */
      if ( !uim_alloc_set_auth_info(uim_ptr->command.cmd_ptr->autn_odd_ins.len, uim_ptr) )
      {
        uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
        status = FALSE;
        UIMDRV_MSG_ERR_1(uim_ptr->id,"Unable to allocate memory for data 0x%x",
                         uim_ptr->command.auth_info.num_bytes);
        break;
      }

    } /* end case - UIM_AUTHENTICATE_ODD_INS_F */
    break;

    case UIM_BCAST_F:
    {
      UIMDRV_MSG_HIGH_1(uim_ptr->id, "Received BCAST command 0x%x ",
                        uim_ptr->command.cmd_ptr->hdr.command );

      /* Build the USIM Authenticate list of states. */
      uim_ptr->mode.usim_states[0] = UIM_BCAST_ST;
      uim_ptr->mode.usim_states[1] = UIM_USIM_DONE_ST;
      uim_ptr->command.usim_state = uim_ptr->mode.usim_states;

      /* Try to allocate mem and reset all the variables */
      if ( !uim_alloc_set_auth_info(uim_ptr->command.cmd_ptr->bcast.len, uim_ptr) )
      {
        uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
        status = FALSE;
        UIMDRV_MSG_ERR_1(uim_ptr->id,"Unable to allocate memory for data 0x%x",
                         uim_ptr->command.auth_info.num_bytes);
        break;
      }

    } /* end case - UIM_BCAST_F */
    break;

    default:
      status = FALSE;
      UIMDRV_MSG_ERR_1(uim_ptr->id,
                    "Unrecognized cmd recd in USIM Module 0x%x",
                       uim_ptr->command.cmd_ptr->hdr.command);

      break;
  }

  if (status)
  {
    /* Call the state machine. */
    uim_usim_command (uim_ptr);
  }
  else
  {
    /* Send an error report to the client and release the buffer */
    uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
    uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;

    uim_send_usim_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
  }

  return(status);

} /* uim_process_usim_command() */


/*===========================================================================

FUNCTION UIM_USIM_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a usim comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_ptr->command.usim_state.

===========================================================================*/
uim_cmd_status_type uim_usim_command_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  uim_cmd_status_type status;
  /* check to see what the status from the UIM is */

  uim_usim_state_type curr_uim_usim_state;
  /* Normally, the status when a command is completed is UIM_CMD_COMPLETED
     This is changed if there is a proactive command pending. */
  uim_cmd_status_type completed_status = UIM_CMD_COMPLETED;

  status = UIM_CMD_SUCCESS;

  /* Check for NULL pointer */
  if(rsp_ptr == NULL)
  {
    return UIM_CMD_ERR;
  }
  uim_ptr->command.usim_req_buf_ptr = &uim_ptr->command.req_buf;

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
  curr_uim_usim_state = *uim_ptr->command.usim_state;

  switch (curr_uim_usim_state)
  {
    case UIM_AUTHENTICATE_ST:        /* USIM Authenticate state */
    {
      /* Return an error status if the command failed. */
      if (rsp_ptr->cmd_status != UIM_DONE)
      {
        status = UIM_CMD_ERR;
      }
      else
      {
        /* Skip to the next state. */
        ++uim_ptr->command.usim_state;
      }
    } /* end case - UIM_AUTHENTICATE_ST */
      break;

    case UIM_BCAST_ST:
    {
      if(uim_ptr->command.rpt_buf.sw1 == SW1_WARNINGS1 && uim_ptr->command.rpt_buf.sw2 == SW2_MORE_DATA)
      {
        /*More Data to send*/

        /* decrement the number of bytes left to read by the number read*/
        uim_ptr->command.auth_info.num_bytes -= (word) uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3;


        /* store the total number of bytes read up-to now */
        uim_ptr->command.auth_info.num_bytes_rsp += (word) uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3;

        /* Increment the offset so that next read would get
         * the right data
         */
        uim_ptr->command.auth_info.offset += (word) uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3;

        /* Increment the pointer to point to the next
         * location to copy the data
         */
        uim_ptr->command.auth_info.data_ptr = uim_ptr->command.auth_info.data_ptr +
          (word) uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3;

        break;
      }
      /*Card is now ready to send the response, send new command*/
      else if( uim_ptr->command.rpt_buf.sw1 == SW1_NORMAL_END && uim_ptr->command.rpt_buf.sw2 == SW2_NORMAL_END)
      {
        /* Build the USIM Authenticate list of states. */
        uim_ptr->mode.usim_states[0] = UIM_BCAST_RSP_ST;
        uim_ptr->mode.usim_states[1] = UIM_USIM_DONE_ST;
        uim_ptr->command.usim_state = uim_ptr->mode.usim_states;

        if( !uim_alloc_set_auth_info((UIM_MAX_TRANSACTION_SIZE * 4), uim_ptr))
        {
          UIMDRV_MSG_ERR_1(uim_ptr->id,"Unable to allocate memory for data 0x%x",
                           uim_ptr->command.auth_info.num_bytes);
        }
        break;
      }
      /* Return an error status if the command failed. */
      if (rsp_ptr->cmd_status != UIM_DONE)
      {
        status = UIM_CMD_ERR;
        break;
      }
      else
      {
        /* Skip to the next state. */
        ++uim_ptr->command.usim_state;
      }
    }
    break;

    case UIM_AUTHENTICATE_ODD_INS_ST:
    {
      if(uim_ptr->command.rpt_buf.sw1 == SW1_WARNINGS2 && uim_ptr->command.rpt_buf.sw2 == SW2_MORE_DATA)
      {
        /*More Data to send*/

        /* decrement the number of bytes left to read by the number read*/
        uim_ptr->command.auth_info.num_bytes -= (word) uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3;


        /* store the total number of bytes read up-to now */
        uim_ptr->command.auth_info.num_bytes_rsp += (word) uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3;

        /* Increment the offset so that next read would get
         * the right data
         */
        uim_ptr->command.auth_info.offset += (word) uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3;

        /* Increment the pointer to point to the next
         * location to copy the data
         */
        uim_ptr->command.auth_info.data_ptr = uim_ptr->command.auth_info.data_ptr +
          (word) uim_ptr->command.usim_req_buf_ptr->apdu_hdr.p3;

        break;
      }
      /*Card is now ready to send the response, send new command*/
      else if(uim_ptr->command.rpt_buf.sw1 == SW1_WARNINGS1 && uim_ptr->command.rpt_buf.sw2 == SW2_AUTH_RSP_DATA)
      {
        /* Build the USIM Authenticate list of states. */
        uim_ptr->mode.usim_states[0] = UIM_AUTHENTICATE_ODD_INS_RSP_ST;
        uim_ptr->mode.usim_states[1] = UIM_USIM_DONE_ST;
        uim_ptr->command.usim_state = uim_ptr->mode.usim_states;

        if( !uim_alloc_set_auth_info((UIM_MAX_TRANSACTION_SIZE * 4), uim_ptr))
        {
          UIMDRV_MSG_ERR_1(uim_ptr->id,"Unable to allocate memory for data 0x%x",
                           uim_ptr->command.auth_info.num_bytes);
        }
        break;
      }
      /* Return an error status if the command failed. */
      if (rsp_ptr->cmd_status != UIM_DONE)
      {
        status = UIM_CMD_ERR;
        break;
      }
      else
      {
        /* Skip to the next state. */
        ++uim_ptr->command.usim_state;
      }
    } /* end case - UIM_AUTHENTICATE_ODD_INS_ST */
    break;

    case UIM_AUTHENTICATE_ODD_INS_RSP_ST:
    case UIM_BCAST_RSP_ST:
    {
      if((rsp_ptr->cmd_status == UIM_DONE) ||
         ((uim_ptr->command.rpt_buf.sw1 == SW1_WARNINGS1) &&
          (uim_ptr->command.rpt_buf.sw2 == SW2_MORE_DATA)))
      {
        /*More Data to Receive*/

        /* store the total number of bytes read up-to now */
        uim_ptr->command.auth_info.num_bytes_rsp += (word) rsp_ptr->cmd_rsp_size;

        /* Increment the offset so that next read would get
         * the right data
         */
        uim_ptr->command.auth_info.offset += (word) rsp_ptr->cmd_rsp_size;

        /* Copy over the data to the buffer provided each time we have
         * a peice of data returned
         */

        uim_memscpy(uim_ptr->command.auth_info.data_ptr,
               rsp_ptr->cmd_rsp_size,
               rsp_ptr->rsp.data,
               (word) rsp_ptr->cmd_rsp_size);

        /* Increment the pointer to point to the next
         * location to copy the data
         */
        uim_ptr->command.auth_info.data_ptr = uim_ptr->command.auth_info.data_ptr +
          (word) rsp_ptr->cmd_rsp_size;

      }
      if (rsp_ptr->cmd_status != UIM_DONE)
      {
        status = UIM_CMD_ERR;
        break;
      }

      if (uim_ptr->command.rpt_buf.sw1 == SW1_WARNINGS1 &&
          uim_ptr->command.rpt_buf.sw2 == SW2_MORE_DATA )
      {
        break;
      }
      else
      {
        /* Skip to the next state. */
        ++uim_ptr->command.usim_state;
      }
    }
    break;

    case UIM_STATUS_BEFORE_USIM_AUTH_ST:
    {
      if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_SEND_STATUS_BEFORE_AUTH,
                                   uim_ptr) == FALSE)
      {
        break;
      }

      if (rsp_ptr->cmd_status != UIM_DONE)
      {
        UIMDRV_MSG_ERR_3(uim_ptr->id,"STATUS before USIM Authenticate failed 0x%x,0x%x,0x%x",
                         rsp_ptr->cmd_status,
                         rsp_ptr->sw1,
                         rsp_ptr->sw2);

        /* SET STATUS TO UIM_CMD_ERR HERE IF WOULD NOT LIKE TO
         * CONTINUE DUE TO STATUS COMMAND FAILURE FOR ANY REASON.
         * PLEASE NOTE THAT IF AN ERROR IS RETURNED, THE AUTHENTICATION
         * COMMAND WOULD NOT BE SENT AND ALSO, THIS COMMAND WOULD NOT BE
         * RE-TRIED.
         */
        //status = UIM_CMD_ERR;
      }
      ++uim_ptr->command.usim_state;
    }/* case - UIM_STATUS_BEFORE_USIM_AUTH_ST */
    break;

    default:
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Invalid State in uim_usim_command_response " );
      status = UIM_CMD_ERR;
      break;
  }
  if (status == UIM_CMD_SUCCESS)
  {
    /* Process the next state of the command */
    uim_usim_command(uim_ptr);
  }

  if (UIM_USIM_DONE_ST == *uim_ptr->command.usim_state)
  {
    /* Indicate that the command has been completed */
    return (completed_status);
  }

  return(status);
} /* uim_usim_command_response */


