/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


              U I M   C D M A   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the CDMA protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_cdma_command
    Processes a generic UIM server command

  uim_send_cdma_err_rpt
    Processes an error condition for a cdma UIM server command

  uim_cdma_command_response
    This function processes the UIM responses from cdma APDUs.

Copyright (c) 2001-2009, 2011 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimcdma.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/16   na      Removed F3 traces
02/28/15   ks      Explicit MF selection logic revisited
01/23/15   ks      Reselect MF after Store ESN commands
07/31/14   ak      Feature flags cleanup
01/29/14   sam     Usage of newer style diag macros
10/24/13   ks      secure memory optimization to cater destination buffer overflow
10/03/13   sam     F3 log reduction
09/26/13   ak      Remove unnecessary feature flags
09/24/13   nmb     Remove extra uim_free param
08/13/31   js      UIMDRV Memory De-Couple changes
08/07/13   tl      Add CDMA AKA Authenticate functionality
08/07/13   tl      Add CDMA AKA Authenticate functionality
07/12/13   nmb     Global Reorganization
07/10/13   js      For reporting error set length of data buffer to zero for
                   authentication commands
06/24/13   js      TSTS changes
06/17/13   ks      Removal of F3 messages as part of memory optimization
05/31/13   na      missed memcpy replaced with uim_memscpy
05/16/13   na      Secure memory copy
02/12/13   js      General F3 macro cleanup
12/17/12   js      Fixed KW error
12/05/12   js      UIM Parallel Processing changes
10/26/12   akv     HW Enumeration changes (without features)
10/09/12   tl      Change static data length copy to dynamic data length
02/08/12   js      Fixed compilation error for mob
02/07/12   js      Fixed uim compilation warnings
01/16/12   nmb     Removal of ALWAYS ON FEATURE_AUTH
12/23/11   shr     Removed featurization for ALWAYS ON features
10/06/11   av      Removed features that are always OFF
08/29/11   ssr     Fixed to add slot information for fetch command
08/11/11   nmb     Expanded Logical Channels
11/10/10   yt      Memory reduction changes
10/26/10   yb      Fixed klocwork error
09/10/10   ps      Merge UIM server DSDS changes
09/06/10   yb      Fixed Lint error
08/18/10   ps      Fixed Klocwork error
07/15/10   nmb     Updated OTAPA Request P1 to comply with spec
06/22/10   ssr     Fixed Lint errors
06/21/10   vs      Fixed class byte in compute IP auth command for ICC
03/29/10   ssr     Removed FEATURE_DS_MOBILE_IP
02/18/10   vs      Merge for CSIM functionality
12/07/09   js      Lint fixes
09/26/09   kp      ZI memory reduction changes
06/16/09   ssr     Fixed MN-AAA Auth
03/04/09   ssr     Fixed compilation warnings
11/10/08   sun     Fixed Featurization
10/05/07   vs/nk   Added EUIMID feature support
08/17/06   nk      Fixed casting in processing SSPR Config Request
04/03/06   pv      Lint Fixes
03/08/06   pv      Load the status words to the report buffer even if we
                   errored out because of un-known status words
11/14/05   pv      Lint Fixes
11/04/05   pv      Fixed compilation warning
04/28/05   jk      Added support to properly transfer the user data from the
                   incoming command to the report
08/27/04   jk      Added support for HRPD AN Authentication services in R-UIM
05/17/04   ck      Moved the call to uim_send_cdma_err_rpt() from default case
                   in the function uim_process_cdma_command() to the end of the
                   function based on tbe status flag.
05/13/04   ts      Fixed a bad reference for MN-AAA authentication processing.
02/20/04   ts      Fixed lint errors.
12/19/03   jk      Added processing for R-UIM 3GPD Services.
03/28/03   ts      Replaced ME_TO_UIM and UIM_TO_ME with instruction case in
                   UIM driver commands.
02/06/03   ck      Removed the duplicate signalling of UIM_RECEIVED_S to client
02/02/03   ck      Lint changes.
01/17/03   ck      Included comdef.h outside of FEATURE_UIM_RUIM to avoid
                   compiler warning
10/24/02   ts      Removed timeout processing.  Timeouts are handled in the
                   driver.
09/05/02   wli     Changed P3 to 1 for COMMIT command APDU.
12/13/01   ts      Cleaned up the status word assignment and included memcpy
                   prototype.
                   Featurized Toolkit code.
12/04/01   ts      Added support for UIM application toolkit.
07/27/01   ts      Cleaned up the code.
07/23/01   ck      Added caching of response to store_esn_me command.
06/26/01   ts      Changed CAVE ESN debug feature switch name to have "FEATURE_"
                   prepended.
01/16/01   ts      Created module.  This module contains the generic state
                   machine.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "intconv.h"

/*lint -esym(766,..\..\services\utils\comdef.h)
  comdef.h was previously included to get rid of compiler warning about
  no external translation unit.  The best thing to do would be
  to not compile uimcdma.c based on a rule..... but till then.....
  let us calm down the lint
*/
#include "bit.h"
#include "uimdrv_msg.h"
#include "err.h"
#include "uimi.h"
#include "uimdrv.h"
#include "uimcdma.h"
#include "uimutil.h"
#include <memory.h>
#include "uimglobals.h"


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static void uim_cdma_command(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_SEND_CDMA_ERR_RPT

DESCRIPTION
  This function sends an error report for a generic command that is being
  requested. We pass cmd_ptr explicitly here since we may be sending err
  report for am unprocessed command that was present in command queue while
  processing hotswap card removal or card error.

DEPENDENCIES
  This function queues an error report onto a client queue.

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_cdma_err_rpt
(
  uim_cmd_type             *cmd_ptr, /* the command to queue up */
  uim_instance_global_type *uim_ptr
)
{
  if (cmd_ptr == NULL)
  {
    return;
  }

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = cmd_ptr->hdr.user_data;
  /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
  uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;

  /* Check if the client task has supplied a report back function. */
  if (cmd_ptr->hdr.rpt_function != NULL)
  {
     UIMDRV_MSG_HIGH_1(uim_ptr->id, "Reporting error for CDMA command 0x%x ",
                       cmd_ptr->hdr.command );
    /* Process the error report based on the command that failed. */
    switch(cmd_ptr->hdr.command)
    {
      case UIM_STORE_ESN_ME_F:              /* Store ESN_ME command */
      {
        /* Report back that store ESN_ME was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STORE_ESN_ME_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_STORE_ESN_ME_F */
      break;

      case UIM_SSD_UPDATE_F:                /* Begin process to update SSD */
      {
        /* Report back that store ESN_ME was not completed */
       if (cmd_ptr->hdr.rpt_function != NULL)
       {
         uim_ptr->command.rpt_buf.rpt_type = UIM_SSD_UPDATE_R;

         /* Fill in necessary fields of command block */
         uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

         /* Report to requesting task */
         (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_SSD_UPDATE_F */
      break;

      case UIM_CONFIRM_SSD_F:         /* Finish process to update SSD */
      {
        /* Did the client give a reportback functiom. */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_CONFIRM_SSD_R;
          /* Indicate the SSD update failed. */
          uim_ptr->command.rpt_buf.rpt.ssd_update_ok = FALSE;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* The client gave a reportback function. */
      } /* end case - UIM_CONFIRM_SSD_F */
      break;

      case UIM_RUN_CAVE_F:                 /* Perform auth signature */
      {
        /* Report back auth signature, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_RUN_CAVE_R;
          uim_ptr->command.rpt_buf.rpt.run_cave.auth = 0;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
         (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end if - reportback function is provided by client. */
      } /* end case - UIM_RUN_CAVE_F */
      break;

      case UIM_AKA_AUTH_F:          /* Perform AKA authentication */
      {
        /* Report back AKA Auth if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_AKA_AUTH_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end if - reportback function is provided by client. */
      } /* end case - UIM_AKA_AUTH_F */
      break;

      case UIM_GENERATE_KEYS_F:              /* Generate CMEA key and VPM */
      {
        /* Report back the result, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_GENERATE_KEYS_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
      } /* end case - UIM_GENERATE_KEYS_F */
      break;

      case UIM_BS_CHAL_F:                   /* Do a Base station Challenge */
      {
        /* Report back the result, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_BS_CHAL_R;
          uim_ptr->command.rpt_buf.rpt.bs_chal.randbs = 0;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
      } /* end case - UIM_BS_CHAL_F */
      break;

      case UIM_COMMIT_F:              /* Commit Request */
      {
        /* Report back that Commit was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_COMMIT_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_COMMIT_F */
      break;

      case UIM_MS_KEY_REQ_F:          /* MS Key Request */
      {
        /* Report back that MS Key Request was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_MS_KEY_REQ_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_MS_KEY_REQ_F */
      break;

      case UIM_KEY_GEN_REQ_F:         /* Key Generation Request */
      {
        /* Report back that Key Generation Request was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_KEY_GEN_REQ_R;

          uim_ptr->command.rpt_buf.rpt.key_gen.ms_result_len = 0;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_KEY_GEN_REQ_F */
      break;

      case UIM_SERV_KEY_GEN_REQ_F:         /* Service Key Generation Request */
      {
        /* Report back that Service Key Generation Request was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_SERV_KEY_GEN_REQ_R;
          uim_ptr->command.rpt_buf.rpt.serv_key_gen.result = 0;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_SERV_KEY_GEN_REQ_F */
      break;

      case UIM_VALIDATE_F:            /* Validation Request */
      {
        /* Report back that Validation Request was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_VALIDATE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_VALIDATE_F */
      break;

      case UIM_CONFIG_REQ_F:             /* Configuration Request */
      {
        /* Report back that Config Request was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_CONFIG_REQ_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill the block id in the response from the command */
          uim_ptr->command.rpt_buf.rpt.configuration.block_id =
            cmd_ptr->configuration.block_id;

          uim_ptr->command.rpt_buf.rpt.configuration.block_length = 0;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_CONFIG_REQ_F */
      break;

      case UIM_DOWNLOAD_REQ_F:           /* Download Request */
      {
        /* Report back that Download Request was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_DOWNLOAD_REQ_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_DOWNLOAD_REQ_F */
      break;

      case UIM_SSPR_CONFIG_REQ_F:        /* SSPR Configuration Request */
      {
        /* Report back that SSPR Config Request was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_SSPR_CONFIG_REQ_R;

          uim_ptr->command.rpt_buf.rpt.sspr_config.block_length = 0;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_SSPR_CONFIG_REQ_F */
      break;

      case UIM_SSPR_DOWNLOAD_REQ_F:      /* SSPR Download Reqeust */
      {
        /* Report back that SSPR Download Request was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
         uim_ptr->command.rpt_buf.rpt_type = UIM_SSPR_DOWNLOAD_REQ_R;

         /* Fill in necessary fields of command block */
         uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

         /* Report to requesting task */
         (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_SSPR_DOWNLOAD_REQ_F */
      break;

      case UIM_OTAPA_REQ_F:           /* Otapa Request */
      {
        /* Report back that OTAPA Request was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_OTAPA_REQ_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_OTAPA_REQ_F */
      break;

      case UIM_COMPUTE_IP_AUTH_F:
      {
        /* Report back that Compute IP Auth was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_COMPUTE_IP_AUTH_R;

          uim_ptr->command.rpt_buf.rpt.compute_ip_auth.data_len = 0;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_COMPUTE_IP_AUTH_F */
      break;

      case UIM_STORE_ESN_MEID_ME_F:              /* Store ESN_MEID_ME command */
      {
        /* Report back that store ESN_MEID_ME was not completed */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STORE_ESN_MEID_ME_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }/* The client provided a reportback function */
      } /* end case - UIM_STORE_ESN_MEID_ME_F */
      break;

      default:
        UIMDRV_MSG_ERR_1(uim_ptr->id,
                      "Unknown command 0x%x in uim_send_cdma_err_rpt",
                         cmd_ptr->hdr.command );
        break;

    }  /* switch */

    /* We only allocate cmd_ptr memory for the external commands that have
       valid callback. Internal UIM commands that use static buffers
       have callback set to NULL. */
    if (cmd_ptr == uim_ptr->command.cmd_ptr)
    {
      /* If cmd_ptr is same as current command in progress set global
         cmd_ptr to NULL as well */
      uim_ptr->command.cmd_ptr = NULL;
    }
    UIM_FREE(cmd_ptr);

  } /* end if - the client has supplied a report back function. */
} /* uim_send_cdma_err_rpt */


/*===========================================================================

FUNCTION UIM_PROCESS_CDMA_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM_CDMA state variable amongst others.

===========================================================================*/
boolean uim_process_cdma_command
(
  uim_instance_global_type *uim_ptr
)
{
  /* This indicates that the command is processed by the state machine */
  boolean status = TRUE;

  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = uim_ptr->command.cmd_ptr->hdr.user_data;

  UIMDRV_MSG_HIGH_1(uim_ptr->id, "Received CDMA command 0x%x ",
                        uim_ptr->command.cmd_ptr->hdr.command );

  /* Process the generic command. */
  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_STORE_ESN_ME_F:              /* Store ESN_ME command */
    {
      /* Store ESN causes some cards to lose its FP.  Selecting MF after all
         Store ESN commands restores the FP and ensures successful selects */
      uim_ptr->state.mf_selection.required = TRUE;
      uim_ptr->state.mf_selection.channel  = uim_ptr->command.cmd_ptr->hdr.channel;

      /* Build the Store ESN list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_STORE_ESN_ME_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_STORE_ESN_ME_F */
    break;

    case UIM_SSD_UPDATE_F:                /* Begin process to update SSD */
    {
      /* Build the Start SSD Update list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_UPDATE_SSD_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end case - UIM_SSD_UPDATE_F */
    break;

    case UIM_CONFIRM_SSD_F:         /* Finish process to update SSD */
    {
      /* Build the Finish SSD Update list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_CONFIRM_SSD_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end case - UIM_CONFIRM_SSD_F */
    break;

    case UIM_RUN_CAVE_F:             /* Perform Run Cave algorithm */
    {
      /* Build the Auth Signature list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_RUN_CAVE_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end case - UIM_RUN_CAVE_F */
    break;

    case UIM_AKA_AUTH_F:         /* Perform AKA Authentication */
    {
      /* Build the AKA Auth list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_AKA_AUTH_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end case - UIM_AKA_AUTH_F */
    break;

    case UIM_GENERATE_KEYS_F:              /* Generate CMEA key and VPM */
    {
      /* Build the Generate Key list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_GENERATE_KEY_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end case - UIM_GENERATE_KEYS_F */
    break;

    case UIM_BS_CHAL_F:                   /* Do a Base station Challenge */
    {
      /* Build the BS Challenge list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_BS_CHALLENGE_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end case - UIM_BS_CHAL_F */
    break;

    case UIM_COMMIT_F:              /* Commit Request */
    {
      /* Build the OTASP Commit list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_COMMIT_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_COMMIT_F */
    break;

    case UIM_MS_KEY_REQ_F:          /* MS Key Request */
    {
      /* Build the MS Key Request list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_MS_KEY_REQ_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_MS_KEY_REQ_F */
    break;

    case UIM_KEY_GEN_REQ_F:         /* Key Generation Request */
    {
      /* Build the Key Generation list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_KEY_GEN_REQ_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_KEY_GEN_REQ_F */
    break;

    case UIM_SERV_KEY_GEN_REQ_F:     /* Service Key Generation Request */
    {
      /* Build the Key Generation list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_SERV_KEY_GEN_REQ_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_KEY_GEN_REQ_F */
    break;

    case UIM_VALIDATE_F:            /* Validation Request */
    {
      /* Build the OTASP Validation list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_VALIDATE_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_VALIDATE_F */
    break;

    case UIM_CONFIG_REQ_F:             /* Configuration Request */
    {
      /* Build the OTASP COnfig Request list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_CFG_REQ_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_CONFIG_REQ_F */
    break;

    case UIM_DOWNLOAD_REQ_F:           /* Download Request */
    {
      /* Build the OTASP Download Request list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_DLOAD_REQ_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_DOWNLOAD_REQ_F */
    break;

    case UIM_SSPR_CONFIG_REQ_F:        /* SSPR Configuration Request */
    {
      /* Build the OTASP SSPR Config Request list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_SSPR_CFG_REQ_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_SSPR_CONFIG_REQ_F */
    break;

    case UIM_SSPR_DOWNLOAD_REQ_F:      /* SSPR Download Reqeust */
    {
      /* Build the OTASP SSPR Download Request list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_SSPR_DLOAD_REQ_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_SSPR_DOWNLOAD_REQ_F */
    break;

    case UIM_OTAPA_REQ_F:           /* Otapa Request */
    {
      /* Build the OTASP OTAPA Request list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_OTAPA_REQ_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_OTAPA_REQ_F */
    break;

    case UIM_COMPUTE_IP_AUTH_F:           /* Compute IP Authentication */
    {
      /* Initialize variable to keep track of number of blocks sent in a
        chained command */
      uim_ptr->command.compute_ip_auth_num_blocks_sent = 0;
      uim_ptr->command.compute_ip_auth_more_blocks_req = FALSE;
      /* Build the Compute IP Auth list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_COMPUTE_IP_AUTH_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_COMPUTE_IP_AUTH_F */
    break;

    case UIM_STORE_ESN_MEID_ME_F:              /* Store ESN_MEID_ME command */
    {
      /* Store ESN causes some cards to lose its FP.  Selecting MF after all
         Store ESN commands restores the FP and ensures successful selects */
      uim_ptr->state.mf_selection.required = TRUE;
      uim_ptr->state.mf_selection.channel  = uim_ptr->command.cmd_ptr->hdr.channel;

      /* Build the Store ESN MEID list of states. */
      uim_ptr->mode.cdma_states[0] = UIM_CDMA_STORE_ESN_MEID_ME_ST;
      uim_ptr->mode.cdma_states[1] = UIM_CDMA_DONE_ST;
      uim_ptr->command.cdma_state_ptr = uim_ptr->mode.cdma_states;
    } /* end if - UIM_STORE_ESN_MEID_ME_F */
    break;

    default:
    {
      status = FALSE;
      UIMDRV_MSG_ERR_1(uim_ptr->id,
                    "Unrecognized cmd recd in CDMA Module 0x%x",
                        uim_ptr->command.cmd_ptr->hdr.command);
    }
    break;
  } /* end switch (command_type) */

  if (status)
  {
    /* Call the state machine. */
    uim_cdma_command (uim_ptr);
  }
  else
  {
    /* Send an error report to the client and release the buffer */
    uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
    uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;

    uim_send_cdma_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
  }

  return(status);

} /* uim_process_cdma_command() */


/*===========================================================================

FUNCTION UIM_PROCESS_CDMA_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a generic command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
static void uim_process_cdma_response
(
  uim_rsp_buf_type        *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  word  pos       = 0;   /* packed block position  */
  byte* temp_ptr  = NULL;
  byte  index     = 0;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  uim_ptr->command.cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Set the status to UIM_PASS unless something failed */
  uim_ptr->command.rpt_buf.rpt_status = UIM_PASS;

  UIMDRV_MSG_HIGH_1(uim_ptr->id, "Reporting result for CDMA command 0x%x",
                    uim_ptr->command.cmd_ptr->hdr.command );

  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_STORE_ESN_ME_F:              /* Store ESN_ME command */
    {
      /* Report back that store ESN_ME was completed */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_STORE_ESN_ME_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        uim_ptr->command.rpt_buf.rpt.store_esn.usage_ind_conf =
                   rsp_ptr->rsp.store_esn.usage_ind_conf;

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_STORE_ESN_ME_F */
    break;

    case UIM_SSD_UPDATE_F:                /* Begin process to update SSD */
    {
      /* Report back that store ESN_ME was completed */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_SSD_UPDATE_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_SSD_UPDATE_F */
    break;

    case UIM_CONFIRM_SSD_F:         /* Finish process to update SSD */
    {
      /* -------------------------------------
      ** Now, if required, report back whether
      ** or not SSD update was successful.
      ** ------------------------------------- */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_CONFIRM_SSD_R;
        /* Indicate the SSD update passed. */
        uim_ptr->command.rpt_buf.rpt.ssd_update_ok = TRUE;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      }
    } /* end case - UIM_CONFIRM_SSD_F */
      break;

    case UIM_RUN_CAVE_F:                 /* Run CAVE algorithm */
    {
      /* Report back auth signature, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_RUN_CAVE_R;
        uim_ptr->command.rpt_buf.rpt.run_cave.auth = b_unpackd(rsp_ptr->rsp.data, 0,
              FSIZ(uim_run_cave_rsp_siz_type, auth_sig));

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_RUN_CAVE_F */
    break;

    case UIM_AKA_AUTH_F:         /* Process AKA Auth Response */
    {
      /* Report back AKA Auth, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt.aka_auth.sync_tag = rsp_ptr->rsp.aka_auth.sync_tag;
        uim_ptr->command.rpt_buf.rpt_type = UIM_AKA_AUTH_R;
        if (uim_ptr->command.rpt_buf.rpt.aka_auth.sync_tag == UIM_AKA_AUTH_VALID_SEQ_NUM)
        {
          temp_ptr = uim_ptr->command.rpt_buf.rpt.aka_auth.aka_data.aka_keys.cipher_key;
          pos = 0;
          for (index=0;index<UIM_AKA_AUTH_KEY_LEN;index++)
          {
            *temp_ptr = b_unpackb((byte *)rsp_ptr->rsp.aka_auth.data,pos,8);
            temp_ptr++;
            pos += 8;
          }

          temp_ptr = uim_ptr->command.rpt_buf.rpt.aka_auth.aka_data.aka_keys.integrity_key;
          for (index=0;index<UIM_AKA_AUTH_KEY_LEN;index++)
          {
            *temp_ptr = b_unpackb((byte *)rsp_ptr->rsp.aka_auth.data,pos,8);
            temp_ptr++;
            pos += 8;
          }

          temp_ptr = (byte*)&(uim_ptr->command.rpt_buf.rpt.aka_auth.aka_data.aka_keys.res_length);
          *temp_ptr = b_unpackb((byte *)rsp_ptr->rsp.aka_auth.data,pos,8);
          pos += 8;

          temp_ptr = uim_ptr->command.rpt_buf.rpt.aka_auth.aka_data.aka_keys.res_data;
          for (index=0;
               index<uim_ptr->command.rpt_buf.rpt.aka_auth.aka_data.aka_keys.res_length;
               index++)
          {
            *temp_ptr = b_unpackb((byte *)rsp_ptr->rsp.aka_auth.data,pos,8);
            temp_ptr++;
            pos += 8;
          }
        }
        else if (uim_ptr->command.rpt_buf.rpt.aka_auth.sync_tag ==
                   UIM_AKA_AUTH_INVALID_SEQ_NUM)
        {
          temp_ptr = uim_ptr->command.rpt_buf.rpt.aka_auth.aka_data.auts;
          pos = 0;
          for (index=0;index<UIM_AKA_AUTH_AUTS_LEN;index++)
          {
            *temp_ptr = b_unpackb((byte *)rsp_ptr->rsp.aka_auth.data,pos,8);
            temp_ptr++;
            pos += 8;
          }
        }
        else
        {
          UIMDRV_MSG_ERR_1(uim_ptr->id, "Bad sync tag value from card %d",
                           uim_ptr->command.rpt_buf.rpt.aka_auth.sync_tag);
          break;
        }

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_AKA_AUTH_F */
    break;

    case UIM_GENERATE_KEYS_F:              /* Generate CMEA key and VPM */
    {
      /* Report back that keys are generated, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_GENERATE_KEYS_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Copy only the bytes received from the UIM. */
        uim_memscpy(uim_ptr->command.rpt_buf.rpt.generate_keys.data,
              sizeof(uim_ptr->command.rpt_buf.rpt.generate_keys.data),
              rsp_ptr->rsp.data,
              rsp_ptr->cmd_rsp_size);

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_GENERATE_KEYS_F */
    break;

    case UIM_BS_CHAL_F:                   /* Do a Base station Challenge */
    {
      /* Report back that keys are generated, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_BS_CHAL_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Copy only the bytes received from the UIM. */
        uim_ptr->command.rpt_buf.rpt.bs_chal.randbs = b_unpackd(rsp_ptr->rsp.data,0,32);

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_BS_CHAL_F */
    break;

    case UIM_COMMIT_F:              /* Commit Request */
    {
      /* Report back the commit results, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_COMMIT_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the result code in the response */
        uim_ptr->command.rpt_buf.rpt.commit.result = rsp_ptr->rsp.commit.result;

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_COMMIT_F */
    break;

    case UIM_MS_KEY_REQ_F:          /* MS Key Request */
    {
      /* Report back the MS key results, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_MS_KEY_REQ_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the result code in the response */
        uim_ptr->command.rpt_buf.rpt.ms_key.result = rsp_ptr->rsp.ms_key.result;

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_MS_KEY_REQ_F */
    break;

    case UIM_KEY_GEN_REQ_F:         /* Key Generation Request */
    {
      /* Report back the MS key results, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_KEY_GEN_REQ_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the result code in the response */
        uim_ptr->command.rpt_buf.rpt.key_gen.result = rsp_ptr->rsp.key_gen.result;

        /* Fill the length of the MS result */
        uim_ptr->command.rpt_buf.rpt.key_gen.ms_result_len =
                             rsp_ptr->rsp.key_gen.ms_result_len;

        /* Fill the MS result */
        uim_memscpy(uim_ptr->command.rpt_buf.rpt.key_gen.ms_result,
                sizeof(uim_ptr->command.rpt_buf.rpt.key_gen.ms_result),
                (byte *) rsp_ptr->rsp.key_gen.ms_result,
                rsp_ptr->rsp.key_gen.ms_result_len);

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_KEY_GEN_REQ_F */
    break;

    case UIM_SERV_KEY_GEN_REQ_F:       /* Service Key Gen Request */
    {
      /* Report back the Service Key Gen results if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_SERV_KEY_GEN_REQ_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the result code in the response */
        uim_ptr->command.rpt_buf.rpt.serv_key_gen.result
         = rsp_ptr->rsp.serv_key_gen.result;

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_SERV_KEY_GEN_REQ_F */
    break;

    case UIM_VALIDATE_F:            /* Validation Request */
    {
      /* Report back the result of the Validation Request */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_VALIDATE_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill the block id in the response */
        uim_ptr->command.rpt_buf.rpt.validate.block_id = rsp_ptr->rsp.validate.block_id;

        /* Fill in the result code in the response */
        uim_ptr->command.rpt_buf.rpt.validate.result = rsp_ptr->rsp.validate.result_code;

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_VALIDATE_F */
    break;

    case UIM_CONFIG_REQ_F:             /* Configuration Request */
    {
      /* Report back the result of Config Request */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_CONFIG_REQ_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill the block id in the response */
        uim_ptr->command.rpt_buf.rpt.configuration.block_id =
         rsp_ptr->rsp.configuration.block_id;

        /* Fill in the block length in the response */
        uim_ptr->command.rpt_buf.rpt.configuration.block_length =
         rsp_ptr->rsp.configuration.block_length;

        /* Fill in the result code in the response */
        uim_ptr->command.rpt_buf.rpt.configuration.result =
         rsp_ptr->rsp.configuration.result_code;

        /* Fill in the data of the Configuration Response */
        /* Skip the first 3 bytes */
        uim_memscpy(uim_ptr->command.rpt_buf.rpt.configuration.data,
              sizeof(uim_ptr->command.rpt_buf.rpt.configuration.data),
              (byte *)rsp_ptr->rsp.configuration.data ,
              rsp_ptr->rsp.configuration.block_length);

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_CONFIG_REQ_F */
    break;

    case UIM_DOWNLOAD_REQ_F:           /* Download Request */
    {
      /* Report back the result of Download Request */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_DOWNLOAD_REQ_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill the block in the response */
        uim_ptr->command.rpt_buf.rpt.download.block_id = rsp_ptr->rsp.download.block_id;

        /* Fill in the result code in the response */
        uim_ptr->command.rpt_buf.rpt.download.result = rsp_ptr->rsp.download.result_code;

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_DOWNLOAD_REQ_F */
    break;

    case UIM_SSPR_CONFIG_REQ_F:        /* SSPR Configuration Request */
    {
      /* Report the result of SSPR Config Request */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_SSPR_CONFIG_REQ_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill the block id in the response */
        uim_ptr->command.rpt_buf.rpt.sspr_config.block_id =
         rsp_ptr->rsp.sspr_config.block_id;

        /* Fill in the result code in the response */
        uim_ptr->command.rpt_buf.rpt.sspr_config.result =
         rsp_ptr->rsp.sspr_config.result_code;

        /* Fill in the block length */
        uim_ptr->command.rpt_buf.rpt.sspr_config.block_length =
           rsp_ptr->rsp.sspr_config.block_length;

        /* Fill in the data of the SSPR Configuration Response */
        uim_memscpy(uim_ptr->command.rpt_buf.rpt.sspr_config.data,
             sizeof(uim_ptr->command.rpt_buf.rpt.sspr_config.data),
             (byte*) rsp_ptr->rsp.sspr_config.data,
              rsp_ptr->rsp.sspr_config.block_length);

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_SSPR_CONFIG_REQ_F */
    break;

    case UIM_SSPR_DOWNLOAD_REQ_F:      /* SSPR Download Reqeust */
    {
      /* Report back the result of SSPR Download Request */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_SSPR_DOWNLOAD_REQ_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill the block id in the response */
        uim_ptr->command.rpt_buf.rpt.sspr_download.block_id =
                        rsp_ptr->rsp.sspr_download.block_id;

        /* Fill the result code in the response */
        uim_ptr->command.rpt_buf.rpt.sspr_download.result =
                        rsp_ptr->rsp.sspr_download.result_code;

        /* Fill the segment offset from the command */
        uim_ptr->command.rpt_buf.rpt.sspr_download.segment_offset =
                  (word) FLIPW(rsp_ptr->rsp.sspr_download.segment_offset);

        /* Fill the segment size from the command */
        uim_ptr->command.rpt_buf.rpt.sspr_download.segment_size =
                        rsp_ptr->rsp.sspr_download.segment_size;

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_SSPR_DOWNLOAD_REQ_F */
    break;

    case UIM_OTAPA_REQ_F:           /* Otapa Request */
    {
      /* Report back the OTAPA results, if requested */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_OTAPA_REQ_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Fill in the result code in the response */
        uim_ptr->command.rpt_buf.rpt.otapa.result = rsp_ptr->rsp.otapa.result;

        /* Fill tbe NAM Lock Indicator */
        uim_ptr->command.rpt_buf.rpt.otapa.nam_lock_ind =
                           rsp_ptr->rsp.otapa.nam_lock_ind;

        /* get the rand otapa as a dword from the data stream */
        uim_ptr->command.rpt_buf.rpt.otapa.rand_otapa =
              b_unpackd( (byte*) rsp_ptr->rsp.otapa.rand_otapa, 0, 32 );

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_OTAPA_REQ_F */
    break;

    case UIM_COMPUTE_IP_AUTH_F:             /* Configuration Request */
    {
      /* Report back the result of Config Request */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_COMPUTE_IP_AUTH_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        if (rsp_ptr->cmd_rsp_size <= UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH)
        {
          uim_ptr->command.rpt_buf.rpt.compute_ip_auth.data_len = rsp_ptr->cmd_rsp_size;
        }
        else
        {
          uim_ptr->command.rpt_buf.rpt.compute_ip_auth.data_len =
           UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH;
        }

        if (uim_ptr->command.rpt_buf.rpt.compute_ip_auth.data_len > 0)
        {
          /* Copy only the bytes received from the UIM. */
          uim_memscpy(uim_ptr->command.rpt_buf.rpt.compute_ip_auth.data,
                 sizeof(uim_ptr->command.rpt_buf.rpt.compute_ip_auth.data),
                 rsp_ptr->rsp.data,
                 uim_ptr->command.rpt_buf.rpt.compute_ip_auth.data_len);
        }

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - client provided a reportback function. */
    } /* end case - UIM_COMPUTE_IP_AUTH_F */
    break;

    case UIM_STORE_ESN_MEID_ME_F:         /* Store ESN_MEID_ME command */
      {
        /* Report back that store ESN_ME was completed */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STORE_ESN_MEID_ME_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          uim_ptr->command.rpt_buf.rpt.store_esn.usage_ind_conf =
                     rsp_ptr->rsp.store_esn.usage_ind_conf;

          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end if - client provided a reportback function. */
      } /* end case - UIM_STORE_ESN_MEID_ME_F */
      break;

    default:
      UIMDRV_MSG_ERR_1(uim_ptr->id,
                    "Unknown command 0x%x in uim_process_cdma_response",
                       uim_ptr->command.cmd_ptr->hdr.command );
      break;

  } /* end switch - uim_ptr->command.cmd_ptr->hdr.command */

  /* We only allocate cmd_ptr memory for the external commands that have
     valid callback. Internal UIM commands that use static buffers
     have callback set to NULL. */
  if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
  {
    UIM_FREE(uim_ptr->command.cmd_ptr);
  }
} /* uim_process_cdma_response */


/*===========================================================================

FUNCTION UIM_CDMA_COMMAND

DESCRIPTION
  This procedure is the main state machine for CDMA UIM command processing.

DEPENDENCIES
  This function uses the UIM CDMA state pointer to determine what to process.

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/

void uim_cdma_command
(
  uim_instance_global_type *uim_ptr
)
{
  byte i                    = 0;   /* index variable */
  word pos                  = 0;   /* packed block position  */
  word index                = 0;
  word number_bytes_left    = 0;
  word number_bytes_sent    = 0;
  byte *compute_ip_data_ptr = NULL;
  word preceding_rrq_length = 0;
  word offset               = 0;  /* start of 237 bytes of challenge    */
  word challenge_len        = 0;
  boolean flag_bad_protocol = FALSE;

  uim_ptr->command.cdma_req_buf_ptr = &uim_ptr->command.req_buf;

  /* Set the protocol from the command to the UIM request buffer */
  uim_ptr->command.cdma_req_buf_ptr->protocol = uim_ptr->command.cmd_ptr->hdr.protocol;

  /* Build an APDU based on the UIM generic state */
  switch(*uim_ptr->command.cdma_state_ptr)
  {
    case UIM_CDMA_STORE_ESN_ME_ST:                 /* Store ESN_ME state */
    case UIM_CDMA_STORE_ESN_MEID_ME_ST:                 /* Store ESN_MEID_ME state */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = STORE_ESN_ME;
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;

      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x08;

     /* If the uim state is to send store_esn_meid command then check if
        service 9 (meid) is allocated and activated.  If it is then the value
        of p1 is to be set to 1 and the meid needs to be sent instead of the
        esn */
      if (*uim_ptr->command.cdma_state_ptr == UIM_CDMA_STORE_ESN_MEID_ME_ST)
      {
        if (uim_ptr->command.cmd_ptr->store_meid.meid_available)
        {
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x01;
          for(i=0; i<UIM_RUIM_UIMID_MAX_SIZE; i++)
          {
            uim_ptr->command.cdma_req_buf_ptr->req.store_meid.meid[i] =
            QW_BYTE(uim_ptr->command.cmd_ptr->store_meid.meid ,i);
          }
          uim_ptr->command.cdma_req_buf_ptr->req.store_meid.len_and_usage =
                                 uim_ptr->command.cmd_ptr->store_meid.len_and_usage;
        }
        else
        {
          uim_ptr->command.cdma_req_buf_ptr->req.store_esn.len_and_usage =
                                 uim_ptr->command.cmd_ptr->store_esn.len_and_usage;
          for ( i = UIM_RUIM_UIMID_OFFSET; i < UIM_RUIM_UIMID_MAX_SIZE; i++)
          {
            uim_ptr->command.cdma_req_buf_ptr->req.store_esn.esn[i] = 0;
          }
          uim_memscpy( (byte*) uim_ptr->command.cdma_req_buf_ptr->req.store_esn.esn ,
                  sizeof(uim_ptr->command.cdma_req_buf_ptr->req.store_esn.esn),
                  (byte*) &(uim_ptr->command.cmd_ptr->store_esn.esn),
                  UIM_RUIM_UIMID_SIZE);
        }
      }
      else
      {
        uim_ptr->command.cdma_req_buf_ptr->req.store_esn.len_and_usage =
                                 uim_ptr->command.cmd_ptr->store_esn.len_and_usage;
        for ( i = UIM_RUIM_UIMID_OFFSET; i < UIM_RUIM_UIMID_MAX_SIZE; i++)
        {
          uim_ptr->command.cdma_req_buf_ptr->req.store_esn.esn[i] = 0;
        }
        uim_memscpy( (byte*) uim_ptr->command.cdma_req_buf_ptr->req.store_esn.esn ,
                sizeof(uim_ptr->command.cdma_req_buf_ptr->req.store_esn.esn),
                (byte*) &(uim_ptr->command.cmd_ptr->store_esn.esn), UIM_RUIM_UIMID_SIZE);
      }


      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);
    }
    break;

    case UIM_CDMA_UPDATE_SSD_ST:                   /* Update SSD State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = UPDATE_SSD;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn    = UIM_MANAGE_SSD;
      }
      else
      {
	flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;

      /* form the data stream */
      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      b_packq(uim_ptr->command.cmd_ptr->ssd_update.randssd,
              uim_ptr->command.cdma_req_buf_ptr->req.data,
              pos,
              FSIZ(uim_update_ssd_req_siz_type, randssd));

      pos+= FSIZ(uim_update_ssd_req_siz_type, randssd);

      b_packb(uim_ptr->command.cmd_ptr->ssd_update.process_control,
              uim_ptr->command.cdma_req_buf_ptr->req.data,
              pos,
              FSIZ(uim_update_ssd_req_siz_type, proc_control));

      pos+= FSIZ(uim_update_ssd_req_siz_type, proc_control);

      if(((pos/8)+4) < UIM_MAX_TRANS_SIZE_OUT)
      {
        uim_memscpy( uim_ptr->command.cdma_req_buf_ptr->req.data + (pos/8),
                (sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data) - (pos/8)),
                (byte*) &(uim_ptr->command.cmd_ptr->ssd_update.esn), 4);
      }
      else
      {
        UIMDRV_MSG_ERR_1(uim_ptr->id,"Value out of bounds- pos=0x%x", pos);
      }

      pos+= FSIZ(uim_update_ssd_req_siz_type, esn);

      /* Pad 2 bytes with zeroes */

      b_packw( 0, uim_ptr->command.cdma_req_buf_ptr->req.data, pos,
             FSIZ(uim_update_ssd_req_siz_type, esn_pad1));

      pos+= FSIZ(uim_update_ssd_req_siz_type, esn_pad1);

      /* Pad 1 byte with zero */
      b_packb( 0, uim_ptr->command.cdma_req_buf_ptr->req.data, pos,
             FSIZ(uim_update_ssd_req_siz_type, esn_pad2));

      pos+= FSIZ(uim_update_ssd_req_siz_type, esn_pad2);

      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = (byte) pos / 8;
      /* bytes of data sent in the command */

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_UPDATE_SSD_ST */
    break;

    case UIM_CDMA_CONFIRM_SSD_ST:                  /* Confirm SSD State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = CONFIRM_SSD;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2        = 0x00;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn    = UIM_MANAGE_SSD;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2        = 0x01;
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;

      /* form the data stream */
      b_packd(uim_ptr->command.cmd_ptr->confirm_ssd.authbs, uim_ptr->command.cdma_req_buf_ptr->req.data,
              pos, FSIZ(uim_conf_ssd_req_siz_type, authbs));
      pos+= FSIZ(uim_conf_ssd_req_siz_type, authbs);

      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = (byte) pos/8;
      /* bytes of data sent in the command */

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_CONFIRM_SSD_ST */
    break;

    case UIM_CDMA_GENERATE_KEY_ST:                 /* Generate KEY State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = GEN_KEY_VPM;
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      /* bytes of data sent in the command */
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x02;

      uim_ptr->command.cdma_req_buf_ptr->req.data[0] = uim_ptr->command.cmd_ptr->gen_keys.vpm_first_octet;
      uim_ptr->command.cdma_req_buf_ptr->req.data[1] = uim_ptr->command.cmd_ptr->gen_keys.vpm_last_octet;

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_GENERATE_KEY_ST */
    break;

    case UIM_CDMA_RUN_CAVE_ST:                     /* Run CAVE State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x80;
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = RUN_CAVE;
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      /* bytes of data sent in the command */
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x11;

      /* form the data stream */
      b_packb(uim_ptr->command.cmd_ptr->run_cave.rand_type,
              uim_ptr->command.cdma_req_buf_ptr->req.data,
              pos,
              FSIZ(uim_run_cave_req_siz_type, rand_type));

      pos+= FSIZ(uim_run_cave_req_siz_type, rand_type);

      if (uim_ptr->command.cmd_ptr->run_cave.rand_type == UIM_UNIQUE_RAND_CHAL)
      {
        pos += 8;

        b_packd(uim_ptr->command.cmd_ptr->run_cave.rand_chal,
                uim_ptr->command.cdma_req_buf_ptr->req.data,
                pos,
                FSIZ(uim_run_cave_req_siz_type, randu));

        pos+= FSIZ(uim_run_cave_req_siz_type, randu);
      }
      else
      {
        b_packd(uim_ptr->command.cmd_ptr->run_cave.rand_chal,
                uim_ptr->command.cdma_req_buf_ptr->req.data,
                pos,
                FSIZ(uim_run_cave_req_siz_type, rand));
        pos+= FSIZ(uim_run_cave_req_siz_type, rand);
      }

      b_packb(uim_ptr->command.cmd_ptr->run_cave.dig_len, uim_ptr->command.cdma_req_buf_ptr->req.data, pos,
              FSIZ(uim_run_cave_req_siz_type, dig_len));

      pos+= FSIZ(uim_run_cave_req_siz_type, dig_len);

      for (i = 0; i < UIM_RUN_CAVE_DIGITS ; i++)
      {
        b_packb(uim_ptr->command.cmd_ptr->run_cave.digits[i],
                uim_ptr->command.cdma_req_buf_ptr->req.data,
                pos,
                FSIZ(uim_run_cave_req_siz_type, digits));
        pos += FSIZ(uim_run_cave_req_siz_type, digits);
      }

      b_packb(uim_ptr->command.cmd_ptr->run_cave.process_control,
              uim_ptr->command.cdma_req_buf_ptr->req.data,
              pos,
              FSIZ(uim_run_cave_req_siz_type, proc_control));

      pos+= FSIZ(uim_run_cave_req_siz_type, proc_control);

      uim_memscpy( uim_ptr->command.cdma_req_buf_ptr->req.data + (pos/8),
                  (sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data) - (pos/8)),
                  (byte*) (uim_ptr->command.cmd_ptr->run_cave.esn), 4);

      pos+= FSIZ(uim_run_cave_req_siz_type, esn);

      /* Pad 2 bytes with zeroes */
      b_packw( 0, uim_ptr->command.cdma_req_buf_ptr->req.data, pos,
             FSIZ(uim_run_cave_req_siz_type, esn_pad1));

      pos+= FSIZ(uim_run_cave_req_siz_type, esn_pad1);

      /* Pad 1 byte with zeroes */
      b_packb( 0, uim_ptr->command.cdma_req_buf_ptr->req.data, pos,
             FSIZ(uim_run_cave_req_siz_type, esn_pad2));

      pos+= FSIZ(uim_run_cave_req_siz_type, esn_pad2);
      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;
      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_RUN_CAVE_ST */
    break;

    case UIM_CDMA_AKA_AUTH_ST:             /* AKA Auth State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x01;
        if (uim_ptr->command.cmd_ptr->aka_auth.op_type == UIM_AKA_AUTH_3G)
        {
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x01;
        }
        else if (uim_ptr->command.cmd_ptr->aka_auth.op_type == UIM_AKA_AUTH_WLAN)
        {
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x02;
        }
        else
        {
          UIMDRV_MSG_ERR_1 (uim_ptr->id,"Received unknown Auth command op type %d",
                             uim_ptr->command.cmd_ptr->aka_auth.op_type);
          break;
        }
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
        if (uim_ptr->command.cmd_ptr->aka_auth.op_type == UIM_AKA_AUTH_3G)
        {
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x81;
        }
        else if (uim_ptr->command.cmd_ptr->aka_auth.op_type == UIM_AKA_AUTH_WLAN)
        {
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x82;
        }
        else
        {
          UIMDRV_MSG_ERR_1(uim_ptr->id,"Received unknown Auth command op type %d",
                           uim_ptr->command.cmd_ptr->aka_auth.op_type);
          break;
        }
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = UIM_CDMA_AKA_AUTH;
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;

      /* bytes of data sent in the command */
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = UIM_AKA_AUTH_RANDA_LEN +
        uim_ptr->command.cmd_ptr->aka_auth.autn_len + 1; /* Add 1 for autn length paramenter */

      /* Form the data stream. Pack the RANDA, AUTN length and AUTN into
         the request payload */
      for (index=0;index<UIM_AKA_AUTH_RANDA_LEN;index++)
      {
        b_packb(uim_ptr->command.cmd_ptr->aka_auth.randa[index],
                (byte *)&uim_ptr->command.cdma_req_buf_ptr->req.aka_auth.randa[index],
                0,
                8);
      }

      uim_ptr->command.cdma_req_buf_ptr->req.aka_auth.autn_len =
        uim_ptr->command.cmd_ptr->aka_auth.autn_len;

      for (index=0;
           (index<uim_ptr->command.cmd_ptr->aka_auth.autn_len) && (index < UIM_MAX_CHARS);
           index++)
      {
        b_packb(uim_ptr->command.cmd_ptr->aka_auth.autn[index],
                (byte *)&uim_ptr->command.cdma_req_buf_ptr->req.aka_auth.autn[index],
                0,
                8);
      }

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_AKA_AUTH_ST */
    break;

    case UIM_CDMA_BS_CHALLENGE_ST:                 /* BS Challenge State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = BS_CHALLENGE;
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;

      /* form the data stream */
      b_packd(uim_ptr->command.cmd_ptr->bs_chal.randseed, uim_ptr->command.cdma_req_buf_ptr->req.data, pos,
              FSIZ(uim_bs_chal_req_siz_type, rands));

      pos+= FSIZ(uim_bs_chal_req_siz_type, rands);

      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = (byte) pos/8;
      /* bytes of data sent in the command */

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_BS_CHALLENGE_ST */
    break;

    case UIM_CDMA_MS_KEY_REQ_ST:                   /* MS Key Request State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = MS_KEY_REQUEST;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = UIM_GENERIC_KEY_GEN_REQ;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }

      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x6B;

      uim_memscpy((byte *)uim_ptr->command.cdma_req_buf_ptr->req.ms_key.randseed,
              sizeof(uim_ptr->command.cdma_req_buf_ptr->req.ms_key.randseed),
              uim_ptr->command.cmd_ptr->ms_key_req.randseed, UIM_RAND_SEED);

      uim_ptr->command.cdma_req_buf_ptr->req.ms_key.a_key_p_rev =
        uim_ptr->command.cmd_ptr->ms_key_req.a_key_p_rev;

      uim_ptr->command.cdma_req_buf_ptr->req.ms_key.param_p_len =
        uim_ptr->command.cmd_ptr->ms_key_req.param_p_len;

      uim_ptr->command.cdma_req_buf_ptr->req.ms_key.param_g_len =
        uim_ptr->command.cmd_ptr->ms_key_req.param_g_len;

      uim_memscpy((byte *) uim_ptr->command.cdma_req_buf_ptr->req.ms_key.param_p,
              sizeof(uim_ptr->command.cdma_req_buf_ptr->req.ms_key.param_p),
              uim_ptr->command.cmd_ptr->ms_key_req.param_p, UIM_PARAM_P);

      uim_memscpy((byte *) uim_ptr->command.cdma_req_buf_ptr->req.ms_key.param_g,
              sizeof(uim_ptr->command.cdma_req_buf_ptr->req.ms_key.param_g),
              uim_ptr->command.cmd_ptr->ms_key_req.param_g, UIM_PARAM_G);

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_MS_KEY_REQ_ST */
    break;

    case UIM_CDMA_KEY_GEN_REQ_ST:  /* Key Generation Request State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = KEY_GEN_REQUEST;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = UIM_GENERIC_KEY_GEN_REQ;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x01;
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }

      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->key_gen.bs_result_len + 1;

      uim_ptr->command.cdma_req_buf_ptr->req.key_gen.bs_result_len =
                                   uim_ptr->command.cmd_ptr->key_gen.bs_result_len;

      uim_memscpy((byte *) uim_ptr->command.cdma_req_buf_ptr->req.key_gen.bs_result,
              sizeof(uim_ptr->command.cdma_req_buf_ptr->req.key_gen.bs_result),
              uim_ptr->command.cmd_ptr->key_gen.bs_result, uim_ptr->command.cmd_ptr->key_gen.bs_result_len);

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_KEY_GEN_REQ_ST */
    break;

    case UIM_CDMA_SERV_KEY_GEN_REQ_ST:  /* Service Key Generation Request State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = KEY_GEN_REQUEST;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = UIM_GENERIC_KEY_GEN_REQ;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x02;
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }

      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = UIM_SERV_KEY_GEN_ID_SIZE;

      for (index=0;index<UIM_SERV_KEY_GEN_ID_SIZE;index++)
      {
        b_packb(uim_ptr->command.cmd_ptr->serv_key_gen.key_id[index],
               (byte *)&uim_ptr->command.cdma_req_buf_ptr->req.serv_key_gen.key_id[index],
                0,
                8);
      }

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_KEY_GEN_REQ_ST */
    break;

    case UIM_CDMA_CFG_REQ_ST:  /* Configuration Request State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = CONFIG_REQUEST;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = UIM_GENERIC_CONFIG_REQ;
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }

      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x01;

      uim_ptr->command.cdma_req_buf_ptr->req.config_req.block_id =
        uim_ptr->command.cmd_ptr->configuration.block_id;

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_CFG_REQ_ST */
    break;

    case UIM_CDMA_DLOAD_REQ_ST:                    /* Download Request State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = DOWNLOAD_REQUEST;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = UIM_GENERIC_DOWNLOAD_REQ;
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->download.block_len + 2;

      uim_ptr->command.cdma_req_buf_ptr->req.download_req.block_id =
        uim_ptr->command.cmd_ptr->download.block_id;

      uim_ptr->command.cdma_req_buf_ptr->req.download_req.block_len =
        (byte)uim_ptr->command.cmd_ptr->download.block_len;

      uim_memscpy((byte *) uim_ptr->command.cdma_req_buf_ptr->req.download_req.data,
              sizeof(uim_ptr->command.cdma_req_buf_ptr->req.download_req.data),
              uim_ptr->command.cmd_ptr->download.data,
              uim_ptr->command.cmd_ptr->download.block_len);

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_DLOAD_REQ_ST */
    break;

    case UIM_CDMA_SSPR_CFG_REQ_ST:  /* SSPR Config Request State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = SSPR_CONFIG_REQUEST;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = UIM_GENERIC_CONFIG_REQ;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x01;
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }

      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x04;

      uim_ptr->command.cdma_req_buf_ptr->req.sspr_cfg_req.block_id =
                          uim_ptr->command.cmd_ptr->sspr_config.block_id;

      FLOPW (uim_ptr->command.cdma_req_buf_ptr->req.sspr_cfg_req.req_offset ,
                          uim_ptr->command.cmd_ptr->sspr_config.req_offset );

      uim_ptr->command.cdma_req_buf_ptr->req.sspr_cfg_req.req_max_size =
                          uim_ptr->command.cmd_ptr->sspr_config.req_max_size;

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_SSPR_CFG_REQ_ST */
    break;

    case UIM_CDMA_SSPR_DLOAD_REQ_ST:  /* SSPR Download Request State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = SSPR_DOWNLOAD_REQUEST;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = UIM_GENERIC_DOWNLOAD_REQ;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x01;
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;

      uim_ptr->command.cdma_req_buf_ptr->req.sspr_download_req.block_id =
                        uim_ptr->command.cmd_ptr->sspr_download.block_id;

      uim_ptr->command.cdma_req_buf_ptr->req.sspr_download_req.block_len =
                        uim_ptr->command.cmd_ptr->sspr_download.block_len;

      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 =
                        uim_ptr->command.cmd_ptr->sspr_download.block_len + 2 ;

      /* Copy the contents of the data from the command */
      uim_memscpy((byte *) uim_ptr->command.cdma_req_buf_ptr->req.sspr_download_req.data,
             sizeof(uim_ptr->command.cdma_req_buf_ptr->req.sspr_download_req.data),
             uim_ptr->command.cmd_ptr->sspr_download.data, uim_ptr->command.cmd_ptr->sspr_download.block_len);

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_SSPR_DLOAD_REQ_ST */
    break;

    case UIM_CDMA_OTAPA_REQ_ST:                    /* OTAPA Request State */
    {
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = OTAPA_REQUEST;
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;

      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

        if (uim_ptr->command.cmd_ptr->otapa_req.meid_available)
        {
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x01;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x0C;

          uim_ptr->command.cdma_req_buf_ptr->req.otapa_req_esn.start_stop =
            uim_ptr->command.cmd_ptr->otapa_req.start_stop;

          uim_ptr->command.cdma_req_buf_ptr->req.otapa_req_esn.rand_seed =
            uim_ptr->command.cmd_ptr->otapa_req.randseed;

          uim_memscpy((byte*)uim_ptr->command.cdma_req_buf_ptr->req.otapa_req_esn.esn,
                 sizeof(uim_ptr->command.cdma_req_buf_ptr->req.otapa_req_esn.esn),
                 (byte*)&(uim_ptr->command.cmd_ptr->otapa_req.esn),sizeof(dword));
        }
        else
        {
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x05;

          uim_ptr->command.cdma_req_buf_ptr->req.otapa_req.start_stop =
             uim_ptr->command.cmd_ptr->otapa_req.start_stop;

          uim_ptr->command.cdma_req_buf_ptr->req.otapa_req.rand_seed =
            uim_ptr->command.cmd_ptr->otapa_req.randseed;
        }
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);

        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x0C;

        uim_ptr->command.cdma_req_buf_ptr->req.otapa_req_esn.start_stop =
          uim_ptr->command.cmd_ptr->otapa_req.start_stop;

        uim_ptr->command.cdma_req_buf_ptr->req.otapa_req_esn.rand_seed =
          uim_ptr->command.cmd_ptr->otapa_req.randseed;

        uim_memscpy((byte*)uim_ptr->command.cdma_req_buf_ptr->req.otapa_req_esn.esn,
               sizeof(uim_ptr->command.cdma_req_buf_ptr->req.otapa_req_esn.esn),
               (byte*)&(uim_ptr->command.cmd_ptr->otapa_req.esn),sizeof(dword));
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_OTAPA_REQ_ST */
    break;

    case UIM_CDMA_COMMIT_ST:                       /* Commit State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = COMMIT;
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = 0x01;

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_COMMIT_ST */
    break;

    case UIM_CDMA_VALIDATE_ST:                     /* Validation State */
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = VALIDATE;
      uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->validate.block_length + 2;

      uim_ptr->command.cdma_req_buf_ptr->req.validate.block_id =
        uim_ptr->command.cmd_ptr->validate.block_id;

      uim_ptr->command.cdma_req_buf_ptr->req.validate.block_len =
        uim_ptr->command.cmd_ptr->validate.block_length;

      uim_memscpy((byte *) uim_ptr->command.cdma_req_buf_ptr->req.validate.data,
             sizeof(uim_ptr->command.cdma_req_buf_ptr->req.validate.data),
             uim_ptr->command.cmd_ptr->validate.data, uim_ptr->command.cmd_ptr->validate.block_length);

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_VALIDATE_ST */
    break;

    case UIM_CDMA_COMPUTE_IP_AUTH_ST:
    {
      if(uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_ICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS2;
      }
      else if (uim_ptr->command.cdma_req_buf_ptr->protocol == UIM_UICC)
      {
        uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
      }
      else
      {
        flag_bad_protocol = TRUE;
        break;
      }
      uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.instrn = COMPUTE_IP_AUTHENTICATION;

      switch(uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_operation)
      {
        case UIM_CMPT_IP_CHAP:
        {
          uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_CHAP;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 =
                uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.
                         chap_challenge_length + 2;
          uim_ptr->command.cdma_req_buf_ptr->req.data[0] =
                uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_id;
          uim_ptr->command.cdma_req_buf_ptr->req.data[1] =
                uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.nai_entry_index;
          uim_memscpy(&(uim_ptr->command.cdma_req_buf_ptr->req.data[2]),
            /* minus is because data is being copied at offset 2*/
            (sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data) - 2),
            uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge,
            uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge_length);
        }  /* end case - UIM_CMPT_IP_CHAP */
        break;

        case UIM_CMPT_IP_MN_HA_AUTH:
        {
          number_bytes_sent = uim_ptr->command.compute_ip_auth_num_blocks_sent *
                                UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_MN_HA;


          if(uim_ptr->command.compute_ip_auth_num_blocks_sent == 0)
          {
            /* This is the first or only block for this command */
            if(uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                registration_data_length > UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE)
            {
              /* Multiple (chained) blocks are required */
              uim_ptr->command.compute_ip_auth_more_blocks_req = TRUE;
              uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_FIRST_BLK;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 =
                                          UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE+1;
              uim_ptr->command.cdma_req_buf_ptr->req.data[0] =
               uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.nai_entry_index;
              uim_memscpy(&(uim_ptr->command.cdma_req_buf_ptr->req.data[1]),
                          (sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data) - 1),
                 uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                          registration_data,
                          UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE);
            } /* end if - Multiple APDUs are required */
            else
            {
              /* Only one APDU is required */
              uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_SNGL_BLK;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 =
                uint16touint8(uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                              registration_data_length + 1);
              uim_ptr->command.cdma_req_buf_ptr->req.data[0] =
               uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.nai_entry_index;

              uim_memscpy(&(uim_ptr->command.cdma_req_buf_ptr->req.data[1]),
                 (sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data) - 1),
                 uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                          registration_data,
                 uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                          registration_data_length);
            } /* end else - only one APDU required */
          } /* end if - It's the first or only block to be sent */
          else
          {
            /* This is an intermediate or last block of the command */
            number_bytes_left=uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                                registration_data_length - number_bytes_sent;
            if (number_bytes_sent > UIM_MAX_3GPD_MN_HA_REG_DATA_LENGTH)
            {
              ERR_FATAL( "Incorrect length to registration_data", 0, 0, 0 );
            }
            else
            {
              compute_ip_data_ptr =
                  &(uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.
                             registration_data[number_bytes_sent]);
            }
            if(number_bytes_left > UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE)
            {
              /* This is not the last block - it is an intermediate block */
              uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_NEXT_BLK;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 =
                                           UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE;
              /*lint -e{670} , data buffer size is greater than BLK size */
              uim_memscpy(uim_ptr->command.cdma_req_buf_ptr->req.data,
                          sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data),
                          compute_ip_data_ptr,
                          UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE);
              /*lint -restore */
            } /* end if - Intermediate Block */
            else if(number_bytes_left > 0)
            {
              /* This is the last block */
              uim_ptr->command.compute_ip_auth_more_blocks_req = FALSE;
              uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_LAST_BLK;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = uint16touint8(number_bytes_left);
              uim_memscpy(uim_ptr->command.cdma_req_buf_ptr->req.data,
                          sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data),
                          compute_ip_data_ptr,
                          number_bytes_left);
            } /* end else if - last block */
            else {
              /* Should not get here - No more blocks needed for MN-HA */
              uim_ptr->command.compute_ip_auth_more_blocks_req = FALSE;
              UIMDRV_MSG_HIGH_0(uim_ptr->id,"Err-No more blks to send in CDMA_CMP_IP_AUTH ST");
            } /* end else - no more blocks to send */
          } /* end else - intermediate or last block */
        } /* end case - UIM_CMPT_IP_MN_HA_AUTH */
        break;

        case UIM_CMPT_IP_MIP_RRQ:
        {
          preceding_rrq_length = uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.
              mip_rrq_data.rrq_data_length - UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH;
          number_bytes_sent = uim_ptr->command.compute_ip_auth_num_blocks_sent *
                    UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE;
          uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_MIP_RRQ;

          /* In the MIP-RRQ command, the data length can be up to 255 bytes for
             each block.  The last 8 bytes of the last block must be the MN-AAA
             Extention header.  In order to simplify this software, and to
             ensure that the MN-AAA Extention header does not get split up, each
             block only sends at most 247 bytes of registration data.  The last
             block can then insert the 8 byte MN-AAA Extension header after the
             registration data without fear that there will not be room for the
             entire header */

          if(uim_ptr->command.compute_ip_auth_num_blocks_sent == 0)
          {
            /* This is the first or only block for this command */
            if(preceding_rrq_length > UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE)
            {
              /* Multiple (chained) blocks are required */
              uim_ptr->command.compute_ip_auth_more_blocks_req = TRUE;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_FIRST_BLK;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 =
                                             UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE;

              uim_memscpy(uim_ptr->command.cdma_req_buf_ptr->req.data,
                          sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data),
                          uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data,
                          UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE);

            } /* end if - Multiple APDUs are required */
            else
            {
              /* Only one APDU (block) is required */
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_SNGL_BLK;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = uint16touint8(preceding_rrq_length +
                                             UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH);

              uim_memscpy(uim_ptr->command.cdma_req_buf_ptr->req.data,
                          sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data),
                          uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data,
                          uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.
                          rrq_data_length);
            } /* end else - only one APDU required */
          } /* end if - It's the first or only block to be sent */
          else
          {
            /* This is an intermediate or last block of the command */
            number_bytes_left = preceding_rrq_length - number_bytes_sent;
            if (number_bytes_sent > UIM_MAX_3GPD_HASH_RRQ_DATA_LENGTH)
            {
              ERR_FATAL( "Incorrect length to rrq_data", 0, 0, 0 );
            }
            else
            {
              compute_ip_data_ptr =
                  &(uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.
                             rrq_data[number_bytes_sent]);
            }
            if(number_bytes_left > UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE)
            {
              /* This is not the last block - it is an intermediate block */
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_NEXT_BLK;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 =
                                            UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE;
              /*lint -e{670} , data buffer size is greater than BLK size */
              uim_memscpy(uim_ptr->command.cdma_req_buf_ptr->req.data,
                          sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data),
                          compute_ip_data_ptr,
                          UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE);
              /*lint -restore */
            } /* end if - Intermediate Block */
            else if(number_bytes_left > 0)
            {
              /* This is the last block */
              uim_ptr->command.compute_ip_auth_more_blocks_req = FALSE;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = UIM_CMPT_IP_AUTH_P2_LAST_BLK;
              uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = uint16touint8(number_bytes_left +
                                             UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH);
              /*lint -e{670} , data buffer size is greater than BLK size */
              uim_memscpy(uim_ptr->command.cdma_req_buf_ptr->req.data,
                          sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data),
                          compute_ip_data_ptr,
                          number_bytes_left + UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH);
              /*lint -restore */
            } /* end else if - last block */
            else {
              /* Should not get here - No more blocks needed for MIP-RRQ HASH */
              uim_ptr->command.compute_ip_auth_more_blocks_req = FALSE;
              UIMDRV_MSG_HIGH_0(uim_ptr->id,"Err-No more blks to send in CDMA_CMP_IP_AUTH ST");
            } /* end else - no more blocks to send */
          } /* end else - intermediate or last block */
        }/* end case - UIM_CMPT_IP_MIP_RRQ */
        break;
        case UIM_CMPT_IP_MN_AAA_AUTH:
        {
          /*--------------------------------------------------------------------
          Calculate the byte offset into challenge where the 237
          least-significant bytes begin
          --------------------------------------------------------------------*/

          challenge_len = uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.
                                   challenge_length;
          UIMDRV_MSG_LOW_1(uim_ptr->id,"Challenge len = 0x%x", challenge_len);
          if (challenge_len > UIM_CMPT_IP_AUTH_MAX_MN_AAA_CHALLENGE_SIZE -1 )
          {
            uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 =
                                 UIM_CMPT_IP_AUTH_MAX_MN_AAA_CHALLENGE_SIZE + 1;
            offset = challenge_len -
                               (UIM_CMPT_IP_AUTH_MAX_MN_AAA_CHALLENGE_SIZE - 1);
          }
          else
          {
            uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 = uint16touint8(challenge_len + 1);
            offset = 0;
          }
          uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_MN_AAA;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;

          uim_ptr->command.cdma_req_buf_ptr->req.data[0] =
              uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.nai_entry_index;

          if(challenge_len != 0)
          {
            if(offset != 0)
            {
              uim_ptr->command.cdma_req_buf_ptr->req.data[1] =
                uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge[0];
              uim_memscpy(&(uim_ptr->command.cdma_req_buf_ptr->req.data[2]),
                          sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data),
                         &(uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.
                         mn_aaa_data.challenge[offset]), challenge_len - offset);
            }
            else
            {
              uim_memscpy(&(uim_ptr->command.cdma_req_buf_ptr->req.data[1]),
                          sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data),
                          uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge,
                          challenge_len);
            }
          }
        }/* end case - UIM_CMPT_IP_MN_AAA_AUTH */
        break;
        case UIM_CMPT_IP_HRPD_AN_CHAP:
        {
          uim_ptr->command.cdma_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p1 = UIM_CMPT_IP_AUTH_P1_HRPD;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p2 = 0x00;
          uim_ptr->command.cdma_req_buf_ptr->apdu_hdr.p3 =
                uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.
                         chap_challenge_length + 1;
          uim_ptr->command.cdma_req_buf_ptr->req.data[0] =
                uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_id;

          uim_memscpy(&(uim_ptr->command.cdma_req_buf_ptr->req.data[1]),
                      sizeof(uim_ptr->command.cdma_req_buf_ptr->req.data),
                      uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge,
                      uim_ptr->command.cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.
                      chap_challenge_length);
        }  /* end case - UIM_CMPT_IP_HRPD_AN_CHAP */
        break;
      } /* end of Compute IP Auth operation type switch */

      uim_ptr->command.cdma_req_buf_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;
      ++uim_ptr->command.compute_ip_auth_num_blocks_sent;
      uim_send_command(uim_ptr->command.cdma_req_buf_ptr, uim_ptr);

    } /* end case - UIM_CDMA_COMPUTE_IP_AUTH_ST */
    break;

    case UIM_CDMA_DONE_ST:                         /* Done State */
    {
      /* Process the response of the command. */
      uim_process_cdma_response(&uim_ptr->command.rsp_buf, uim_ptr);
    }
    break;

    default:
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"Invalid state in uim_cdma_command %ld",
                        *uim_ptr->command.cdma_state_ptr);
      break;
  } /* end of main switch */

  if ( flag_bad_protocol == TRUE )
  {
    
    UIMDRV_MSG_ERR_2(uim_ptr->id,"State %d not processed due to bad protocol %d",
                      *uim_ptr->command.cdma_state_ptr,
                      uim_ptr->command.cdma_req_buf_ptr->protocol);
  }
  
  UIMDRV_MSG_LOW_1(uim_ptr->id,"UIM state in uim_command %ld",
                   *uim_ptr->command.cdma_state_ptr);

} /* uim_cdma_command */


/*===========================================================================

FUNCTION UIM_CDMA_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a generic comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_ptr->command.cdma_state_ptr.

===========================================================================*/

uim_cmd_status_type uim_cdma_command_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  uim_cmd_status_type status; /* Status of response processing. */
  uim_cdma_state_type curr_uim_cdma_state; /* Holds initial cdma state */
  /* Normally, the status when a command is completed is UIM_CMD_COMPLETED
     This is changed if there is a proactive command pending. */
  uim_cmd_status_type completed_status = UIM_CMD_COMPLETED;

  /* Assume the command response is proper. */
  status = UIM_CMD_SUCCESS;

  /* Fill the status words into the report buffer. This could be sent for
     both passed and failed reports. If the command is not sent to the
     card, then the status words are cleared before sending the report */

  uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) rsp_ptr->sw1;
  uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) rsp_ptr->sw2;

  /* UIM_PARITY is used to indicate an unknown response from the UIM. */
  if ( rsp_ptr->cmd_status == UIM_PARITY )
  {
     /* If we get a parity error, we would like to treat it as a command
        error */
     return(UIM_CMD_ERR);
  } /* end if - cmd_status == UIM_PARITY */

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
    /* Set the slot for the command based on the slot in which the status
       words were received */
    uim_ptr->command.static_cmd_buf.hdr.slot = uim_ptr->command.cmd_ptr->hdr.slot;

  } /* end if - fetch response */

  /* Necessary so that we do not change the uim state and switch into that
     case also */
  curr_uim_cdma_state = *uim_ptr->command.cdma_state_ptr;

  switch (curr_uim_cdma_state)
  {
    case UIM_CDMA_STORE_ESN_ME_ST:        /* Store ESN_ME state */
    case UIM_CDMA_STORE_ESN_MEID_ME_ST:   /* Store ESN_MEID_ME state */
    case UIM_CDMA_UPDATE_SSD_ST:          /* Update SSD State */
    case UIM_CDMA_CONFIRM_SSD_ST:         /* Confirm SSD State */
    case UIM_CDMA_GENERATE_KEY_ST:        /* Generate KEY State */
    case UIM_CDMA_RUN_CAVE_ST:            /* Run CAVE State */
    case UIM_CDMA_AKA_AUTH_ST:            /* AKA Auth State */
    case UIM_CDMA_BS_CHALLENGE_ST:        /* BS Challenge State */
    case UIM_CDMA_MS_KEY_REQ_ST:          /* MS Key Request State */
    case UIM_CDMA_KEY_GEN_REQ_ST:         /* Key Generation Request State */
    case UIM_CDMA_SERV_KEY_GEN_REQ_ST:    /* Key Generation Request State */
    case UIM_CDMA_CFG_REQ_ST:             /* Configuration Request State */
    case UIM_CDMA_DLOAD_REQ_ST:           /* Download Request State */
    case UIM_CDMA_SSPR_CFG_REQ_ST:        /* SSPR Config Request State */
    case UIM_CDMA_SSPR_DLOAD_REQ_ST:      /* SSPR Download Request State */
    case UIM_CDMA_OTAPA_REQ_ST:           /* OTAPA Request State */
    case UIM_CDMA_COMMIT_ST:              /* Commit State */
    case UIM_CDMA_VALIDATE_ST:            /* Validation State */
    {
      /* Determine if the UIM command responded with a problem. */
      if (rsp_ptr->cmd_status != UIM_DONE)
      {
        /* Indicate a problem with the command. */
        return (UIM_CMD_ERR);
      }
      else
      /* The command completed successfully */
      {
        /* Get the next state. */
        ++uim_ptr->command.cdma_state_ptr;
      } /* end if - the UIM command responded with a problem. */
    } /* end case */
    break;

    case UIM_CDMA_COMPUTE_IP_AUTH_ST:
    {
      /* Determine if the UIM command responded with a problem. */
      if (rsp_ptr->cmd_status != UIM_DONE)
      {
        /* Indicate a problem with the command. */
        return (UIM_CMD_ERR);
      }
      else if (!uim_ptr->command.compute_ip_auth_more_blocks_req)
      /* The command completed successfully and is complete.
         Otherwise, the command requires another block, repeat the state */
      {
        /* Get the next state. */
        ++uim_ptr->command.cdma_state_ptr;
      } /* end if - the UIM command responded with a problem. */
    } /* end case */
    break;

    case UIM_CDMA_DONE_ST:                         /* Done State */
      break;

    default:

      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Invalid State in uim_cdma_command_response ");
      status = UIM_CMD_ERR;
      break;

  } /* end switch - curr_uim_cdma_state */

  /* Continue processing the command only if the response indicates success. */
  if (status == UIM_CMD_SUCCESS)
  {
    /* Process the next state of this command. */
    uim_cdma_command(uim_ptr);
  } /* end if - command is still in progress */

  /* Determine if the command has completed */
  if (UIM_CDMA_DONE_ST == *uim_ptr->command.cdma_state_ptr)
  {
    /* Indicate the command has completed successfully. */
    return (completed_status);
  } /* end if - uim_cdma_state_ptr is UIM_CDMA_DONE_ST */

  return(status);
} /* uim_cdma_command_response */

