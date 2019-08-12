/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  SHORT  MESSAGE


GENERAL DESCRIPTION : ENHANCED STK layer REFRESH support

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_refresh.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/16   gs      F3 Logging inprovements
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
09/09/14   bd      Features clean up
07/21/14   sw      Wait for display confirmation before sending terminal response
07/11/14   shr     Add Support for NV Refresh
05/27/14   bd      Changes to hold null alpha and no alpha indications
04/03/14   vr       Do not send Alpha to client when IGNORE_ALPHA is enabled
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
12/26/13   sw      Removed error print for refresh success result codes
10/04/13   sw      Reduced F3 messages
09/13/13   gm      Support for Recovery and hot-swap
07/01/13   hn      Send command_number, additional_info and other_info to ESTK
03/29/13   hn      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "intconv.h"
#include "uim_msg.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif

#ifdef FEATURE_ESTK
#include "estk_refresh.h"

/*===========================================================================
FUNCTION: estk_process_refresh_cnf

DESCRIPTION:
  GSTK calls this ESTK function to process the REFRESH CNF.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS: - When REFRESH is successfully handled
    ESTK_ERROR:   - When there is failure in handling REFRESH

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_refresh_cnf(
  estk_cmd_type *cmd_ptr
)
{
  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  if (!ESTK_IS_VALID_SLOT_ID(cmd_ptr->hdr.sim_slot_id)
      || cmd_ptr->hdr.sim_slot_id == GSTK_SLOT_ID_MAX)
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", cmd_ptr->hdr.sim_slot_id);
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_1("REFRESH result %d received",
            cmd_ptr->cmd_data.refresh_cnf.refresh_result);

  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
  {
    UIM_MSG_HIGH_0("Pending display alpha response");
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;    
    estk_curr_inst_ptr->result = cmd_ptr->cmd_data.refresh_cnf.refresh_result;
    estk_cache_additional_info(cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr);
    if(cmd_ptr->cmd_data.refresh_cnf.other_info_ptr != NULL)
    {
      gstk_memcpy(&(estk_curr_inst_ptr->extra_param),
                  cmd_ptr->cmd_data.refresh_cnf.other_info_ptr,
                  sizeof(gstk_terminal_rsp_extra_param_type),
                  sizeof(gstk_terminal_rsp_extra_param_type),
                  sizeof(gstk_terminal_rsp_extra_param_type));
    }
  }
  else
  {
    UIM_MSG_HIGH_0("Sending REFRESH TR");
    (void)estk_send_terminal_response(
          cmd_ptr->hdr.cmd_ref_id,                      /* command_details_ref */
          GSTK_REFRESH_CNF,                             /* command response    */
          cmd_ptr->cmd_data.refresh_cnf.refresh_result, /* general result      */
          cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr, /* additional result   */
          cmd_ptr->cmd_data.refresh_cnf.other_info_ptr);/* extra param         */
  }  

  if (cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr)
  {
    if (cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->additional_info_ptr)
    {
      gstk_free(cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->additional_info_ptr);
      cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr->additional_info_ptr = NULL;
    }
    gstk_free(cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr);
    cmd_ptr->cmd_data.refresh_cnf.additional_result_ptr = NULL;
  }

  if (cmd_ptr->cmd_data.refresh_cnf.other_info_ptr)
  {
    gstk_free(cmd_ptr->cmd_data.refresh_cnf.other_info_ptr);
    cmd_ptr->cmd_data.refresh_cnf.other_info_ptr = NULL;
  }

  return ESTK_SUCCESS;
} /* estk_process_refresh_cnf */


/*===========================================================================
FUNCTION: estk_process_refresh_req

DESCRIPTION:
  GSTK calls this ESTK function to process the REFRESH request.

PARAMETERS:
  gstk_req_ptr : [Input] gstk REFRESH cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS: - When REFRESH is successfully handled
    ESTK_ERROR:   - When there is failure in handling REFRESH

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_refresh_req(
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  /* Check input parameter */
  if (gstk_req_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  UIM_MSG_HIGH_2("In estk_process_refresh_req(): alpha_length=%d, alpha_text=%s",
                 gstk_req_ptr->cmd.refresh_pro_cmd_req.alpha.length,
                 gstk_req_ptr->cmd.refresh_pro_cmd_req.alpha.text);
  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == FALSE)
  {
    /* Check Alpha availability */
    /* Even in case of null data object in ALPHA (i.e. length = '00' and no value part),
       GSTK make sures at least NULL char is present in alpha text and length includes
       that NULL character. So the min length is one and hence we need to check for
       length more than one */
    if(((gstk_req_ptr->cmd.refresh_pro_cmd_req.alpha.length) > 1 &&
        (gstk_req_ptr->cmd.refresh_pro_cmd_req.alpha.text)) ||
       (gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
           GSTK_SLOT_ID_MAX) == TRUE))
    {
      if (estk_send_alpha_to_display_client(gstk_req_ptr, TRUE) != ESTK_SUCCESS)
      {
        UIM_MSG_ERR_0("Send Refresh Alpha Display failed");
        /* priv data allocated is freed when a TR is sent or
                    when a End Proactive Session is received */
        return ESTK_ERROR;
      }
    }
  }

  return ESTK_SUCCESS;
} /* estk_process_refresh_req */

#endif /* FEATURE_ESTK */

