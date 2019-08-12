/*===========================================================================
FILE:  estk_sups.c


GENERAL DESCRIPTION : ENHANCED STK layer Send USSD and Send SS support

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2008 - 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_sups.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/16   vr      Allocating memory to additional info 2 ptr
05/30/16   gs      F3 Logging inprovements
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
09/09/14   bd      Features clean up
08/26/14   vr      Fix memory leaks
07/21/14   sw      Wait for display confirmation before sending terminal response
07/11/14   shr     Add Support for NV Refresh
05/27/14   bd      Changes to hold null alpha and no alpha indications
05/08/14   dy      Fix KW errors
05/06/14   gs      Removed the ESTK featurization
04/11/14   dy      Fix misspelling of send_ussd_pro_cmd_req as
                   send_ss_pro_cmd_req
04/03/14   vr      Do not send Alpha to client when IGNORE_ALPHA is enabled
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
02/26/14   vr      Send error TR if call_control_result is not MODIFIED_TO_VOICE
01/30/14   vr      SIM initiated call control handling
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/25/13   sw      Send USSD TR when REL_USS_IND is received
10/17/13   vr      Fix for SEND USSD/SS - Call Control Not Allowed wrong TR
09/13/13   gm      Support for Recovery and hot-swap
08/06/13   vr      Centralized Call Control changes
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/22/13   kb      Fixed wrong TR issue of SEND USSD/SS
10/19/12   hk      Memory Optimization in ESTK through F3 removal
09/27/12   hk      Fixed NULL pointer dereference
02/18/12   av      Cleaned up remaining ALWAYF OFF feature flags
02/06/12   xz      Fixed wrong TR issue of SEND USSD/SS
01/30/12   bd      Removed read access to global variable in client context
                   and fixed incorrect result object2 TLV, KW and LINT issues
12/23/11   bd      Support for call control by card modifying the proactive
                   command SETUP CALL into a USSD
12/22/11   bd      Fixed NULL pointer dereference
10/18/11   bd      Fix for the case of USSD transaction terminated by user
10/17/11   bd      Fixed incorrect use of pointers in CM’s context
08/17/11   dd      Copy the SS TR when saving it
06/13/11   sg      Move FEATURE_ESTK_IGNORE_ALPHA to NV
06/06/11   js      Fixed issue for inccorect TR for USSD with alpha but no icon
03/28/11   dd      Wait for icon_disp_rsp for network commands
03/10/11   sg      Convert USSD text string from CBS to SMS DCS
02/02/11   dd      Treat CC_CAUSE_INFO as USSD failure
09/10/10   dd      Fixing USSD and SS null alpha
07/19/10   dd      Add tag to the raw and decoded hdr for Set Up Call
06/30/10   dd      Icon display confirmation
03/15/10   xz      Claim CM client ownership when process SEND SS/USSD
02/19/10   xz      Fix compilation error
02/15/10   nb      Moved Dual Slot Changes
01/20/09   xz      Fix issue of showing SS alpha
11/03/08   sk      Featurized ESTK based on external interface support
09/16/08   sk      Fixed include header file name
                   Added lint fixes
09/10/08   sk/alg  Added fixes for Send SS
09/08/08   tml     Fixed auto header version generation and add Send SS
                   support
08/26/08   tml     Initial Support

=============================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>
#include "uim_msg.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif

#if defined(FEATURE_ESTK)
#include "estk_cm.h"

#include <string.h>
#include "estk_ssparser.h"
#include "intconv.h"

/*===========================================================================

                             DECLARATIONS

===========================================================================*/

#define ESTK_ERROR_CODE_TAG               0x02
#define ESTK_RETURN_RESULT_PROBLEM        0x82
#define ESTK_SUPS_GSM_7_BIT_SMS_DCS       0x00
#define ESTK_SUPS_8_BIT_SMS_DCS           0x04
#define ESTK_SUPS_UCS2_16_BIT_SMS_DCS     0x08

static void estk_sups_send_err_terminal_response (
  boolean                       err_info_present,
  uint16                        err_code_tag,
  uint16                        err_code
);

/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION: estk_sups_alloc_populate_priv_data

DESCRIPTION:
  This function allocates memory for the sups_info private estk data and
  populate the information as needed

PARAMETERS:
  cmd_ref_id   : [Input] command reference id for the GSTK command
  ss_operation : [Input] type of USSD or SS operation

DEPENDENCIES:
  None

RETURN VALUE:
  Pointer to the newly allocated sups info data structure

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_sups_info_type* estk_sups_alloc_populate_priv_data(
  uint32                           cmd_ref_id,
  estk_sups_ss_operation_enum_type ss_operation
)
{
  static uint8 curr_sups_req_id = 0x01;

  UIM_MSG_HIGH_1("estk_sups_alloc_populate_priv_data, curr_sups_req_id 0x%x",
                 curr_sups_req_id);

  /* Check for null private sups info ptr, if not null, free the existing
     data prior to allocating memory in the next step */
  if (estk_shared_info.sups_info_ptr != NULL)
  {
    UIM_MSG_ERR_1("Pending priv.sups_info_ptr, 0x%x, free memory",
                  estk_shared_info.sups_info_ptr);
    gstk_free(estk_shared_info.sups_info_ptr);
    estk_shared_info.sups_info_ptr = NULL;
  }

  /* Allocate memory for the private data */
  estk_shared_info.sups_info_ptr = GSTK_CALLOC(sizeof(estk_sups_info_type));

  if (estk_shared_info.sups_info_ptr == NULL)
  {
    return NULL;
  }

  /* Assign the private data sups info data structure members */
  estk_shared_info.sups_info_ptr->cmd_ref_id   = cmd_ref_id;
  estk_shared_info.sups_info_ptr->ss_operation = ss_operation;
  estk_shared_info.sups_info_ptr->req_id       = curr_sups_req_id;

  /* Update the curr_sups_req_id to the next value for the next
     supps usage */
  if (curr_sups_req_id == 0xFF)
  {
    curr_sups_req_id = 0x01;
  }
  else
  {
    curr_sups_req_id++;
  }

  return estk_shared_info.sups_info_ptr;

} /* estk_sups_alloc_populate_priv_data */

/*===========================================================================
FUNCTION: estk_ss_init

DESCRIPTION:
  The function initializes the SS Parser

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_ss_init(void)
{
  static boolean initialised = FALSE;

  UIM_MSG_HIGH_0("estk_ss_init");

  if (!initialised)
  {
    initialised = TRUE;
    ssparser_init();
  }

  return ESTK_SUCCESS;

} /* estk_ss_init */

/*===========================================================================

FUNCTION: estk_sups_send_ussd_cbs_to_sms_convert

DESCRIPTION:
  Convert the Send USSD TR from CBS to SMS format

PARAMETERS:
  cbs_to_sms_text: [Output] SMS format text string
  sups_info_ptr : [Input] CM USSD conf information

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_sups_send_ussd_cbs_to_sms_convert(
  gstk_string_type                  **cbs_to_sms_text_ptr,
  const cm_sups_info_s_type         *sups_info_ptr)
{
  uint8                 input_alphabet_string[MAX_DISPLAY_TEXT_LEN];

  /* Check input arguments */
  if(cbs_to_sms_text_ptr == NULL
     || *cbs_to_sms_text_ptr == NULL
     || sups_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Input pointer is NULL");
    return ESTK_BAD_INPUT_PARAM;
  }

  memset(input_alphabet_string, 0, sizeof(input_alphabet_string));

  /* Set to GSM-7 bit default - we will change if applicable later */
  (*cbs_to_sms_text_ptr)->dcs = ESTK_SUPS_GSM_7_BIT_SMS_DCS;

  if(sups_info_ptr->uss_data_type == CM_USS_PHASE1_TYPE)
  {
    UIM_MSG_HIGH_0("CM USSD response in IA5 format");
    (*cbs_to_sms_text_ptr)->length = cm_util_ascii_to_gsm_alphabet(
                               input_alphabet_string,
                               sups_info_ptr->uss_data.ussData,
                               sups_info_ptr->uss_data.size);
  }
  else
  {
    (*cbs_to_sms_text_ptr)->length = sups_info_ptr->uss_data.size;
     if(gstk_memcpy(input_alphabet_string,
                    sups_info_ptr->uss_data.ussData,
                    (*cbs_to_sms_text_ptr)->length,
                    (*cbs_to_sms_text_ptr)->length,
                    sups_info_ptr->uss_data.size) <
        (*cbs_to_sms_text_ptr)->length)
     {
       return ESTK_ERROR;
     }
  }

  /* Convert DCS from CBS to SMS */
  /* Convert from CBS to SMS coding scheme - based on 3gpp 23.038 */
  (*cbs_to_sms_text_ptr)->text =
    (uint8 *)GSTK_CALLOC((*cbs_to_sms_text_ptr)->length);

  if ((*cbs_to_sms_text_ptr)->text == NULL)
  {
    return ESTK_ERROR;
  }

  if(gstk_memcpy((*cbs_to_sms_text_ptr)->text,
                 input_alphabet_string,
                 (*cbs_to_sms_text_ptr)->length,
                 (*cbs_to_sms_text_ptr)->length,
                 MAX_DISPLAY_TEXT_LEN) <
     (*cbs_to_sms_text_ptr)->length)
  {
    gstk_free((*cbs_to_sms_text_ptr)->text);
    return ESTK_ERROR;
  }

  /* Lets say DCS is a byte represented by b7 b6 b5 b4 b3 b2 b1 b0 */
  /* Coding group is represented by b7 b6 b5 b4 */
  UIM_MSG_HIGH_1("CBS coding scheme is 0x%x",
                 sups_info_ptr->uss_data.uss_data_coding_scheme);

  /* Coding group - b7b6b5b4 -- 0001 => Message preceeded by 2 bytes for
     language indication - strip this out as it is not applicable in SMS */
  if((sups_info_ptr->uss_data.uss_data_coding_scheme == 0x10)
    || (sups_info_ptr->uss_data.uss_data_coding_scheme == 0x11))
  {
    /* Fix DCS if b3b2b1b0 -- 0001 - UCS2 else GSM 7 bit */
    if((sups_info_ptr->uss_data.uss_data_coding_scheme & 0x01) == 0x01)
    {
      (*cbs_to_sms_text_ptr)->dcs = ESTK_SUPS_UCS2_16_BIT_SMS_DCS;
    }

    /* Need to alter the text string - strip out the first two bytes */
    gstk_free((*cbs_to_sms_text_ptr)->text);
    (*cbs_to_sms_text_ptr)->length = ((*cbs_to_sms_text_ptr)->length) - 2;
    (*cbs_to_sms_text_ptr)->text =
      (uint8 *)GSTK_CALLOC((*cbs_to_sms_text_ptr)->length);
    if ((*cbs_to_sms_text_ptr)->text == NULL)
    {
      return ESTK_ERROR;
    }
    if(gstk_memcpy((*cbs_to_sms_text_ptr)->text,
                   sups_info_ptr->uss_data.ussData + 2,
                   (*cbs_to_sms_text_ptr)->length,
                   (*cbs_to_sms_text_ptr)->length,
                   (*cbs_to_sms_text_ptr)->length) <
       (*cbs_to_sms_text_ptr)->length)
    {
      gstk_free((*cbs_to_sms_text_ptr)->text);
      return ESTK_ERROR;
    }
    return ESTK_SUCCESS;
  }

  UIM_MSG_HIGH_1("CBS Text length is %d", (*cbs_to_sms_text_ptr)->length);
  /* Coding group - b7b6 -- 01 or b7b6b5b4 -- 1001*/
  if((((sups_info_ptr->uss_data.uss_data_coding_scheme >> 6) & 0x03) == 0x01)
     || (((sups_info_ptr->uss_data.uss_data_coding_scheme >> 4) & 0x0F) == 0x09))
  {
    /* If b3b2 == 01 - 8 bit data */
    if((sups_info_ptr->uss_data.uss_data_coding_scheme & 0x0C)
       == ESTK_SUPS_8_BIT_SMS_DCS)
    {
      (*cbs_to_sms_text_ptr)->dcs = ESTK_SUPS_8_BIT_SMS_DCS;
      return ESTK_SUCCESS;
    }
    /* If b3b2 == 10 - 16 bit UCS2 data */
    if((sups_info_ptr->uss_data.uss_data_coding_scheme & 0x0C)
        == ESTK_SUPS_UCS2_16_BIT_SMS_DCS)
    {
      (*cbs_to_sms_text_ptr)->dcs = ESTK_SUPS_UCS2_16_BIT_SMS_DCS;
      return ESTK_SUCCESS;
    }
  }

  /* Coding group b7 b6 b5 b4 == 1111 */
  if((sups_info_ptr->uss_data.uss_data_coding_scheme & 0xF0) == 0xF0)
  {
    /* If b2 is 1 then 8 bit data */
    if((sups_info_ptr->uss_data.uss_data_coding_scheme & 0x04)
       == ESTK_SUPS_8_BIT_SMS_DCS)
    {
      (*cbs_to_sms_text_ptr)->dcs = ESTK_SUPS_8_BIT_SMS_DCS;
    }
  }
  return ESTK_SUCCESS;
} /* estk_sups_send_ussd_cbs_to_sms_convert */

/*===========================================================================

FUNCTION: estk_sups_send_ss_success_terminal_response

DESCRIPTION:
  Processes the Send SS Toolkit Response from the Network and builds the
  TR to the card

PARAMETERS:
  sups_info_ptr   : [Input] CM supplementary service conf information

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_sups_send_ss_success_terminal_response (
  const cm_sups_info_s_type    *sups_info_ptr
)
{
  uint32                         cmd_ref_id                  = 0x00;
  gstk_additional_info_ptr_type  additional_result           = {0x00};
  uint8                          default_additonal_result[2] = {0x00, 0x00};

  UIM_MSG_HIGH_0("estk_sups_send_ss_success_terminal_response");

  if (sups_info_ptr == NULL || estk_shared_info.sups_info_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null Pointer sups_info_ptr = 0x%x,estk_shared_info.sups_info_ptr = 0x%x ",
                  		sups_info_ptr, estk_shared_info.sups_info_ptr);
    return;
  }
  /* Populate L3 data if it is Register or Interrogate SS */
  switch(estk_shared_info.sups_info_ptr->ss_operation)
  {
    case ESTK_INTERROGATE_SS:
    case ESTK_REGISTER_SS:
      UIM_MSG_HIGH_1("L3 present with size 0x%x",
                     sups_info_ptr->sups_unparsed_l3_data.size );

      if ( sups_info_ptr->sups_unparsed_l3_data.present &&
         sups_info_ptr->sups_unparsed_l3_data.size > 0)
      {
        additional_result.additional_info_ptr =
          (uint8*)sups_info_ptr->sups_unparsed_l3_data.data;
        additional_result.length =
          uint16touint8(sups_info_ptr->sups_unparsed_l3_data.size);
      } /* L3 data present */
      break;

    /* If feature not defined, fall through */
    default:
      UIM_MSG_HIGH_3("Building SS with ss_code 0x%x, estk ss_op 0x%x, cm ss_op 0x%x",
                     sups_info_ptr->ss_code,
                     estk_shared_info.sups_info_ptr->ss_operation,
                     sups_info_ptr->ss_operation);

      default_additonal_result[0] = sups_info_ptr->ss_code;
      default_additonal_result[1] = sups_info_ptr->ss_operation;
      additional_result.additional_info_ptr = default_additonal_result;
      additional_result.length = 2;
      break;
  }

  cmd_ref_id = estk_shared_info.sups_info_ptr->cmd_ref_id;

  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
  {
    UIM_MSG_HIGH_0("Saving SEND SS TR");
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
    estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
    estk_cache_additional_info(&additional_result);
#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */      
    
  }
  else
  {
    (void) estk_send_terminal_response(
              cmd_ref_id,                          /* command_details_ref */
              GSTK_SEND_SS_CNF,                    /* command response    */
              GSTK_COMMAND_PERFORMED_SUCCESSFULLY, /* general_result      */
              &additional_result,                  /* additional result   */
              NULL);                               /* extra param         */
  }

} /* estk_sups_send_ss_success_terminal_response */

/*===========================================================================

FUNCTION: estk_sups_send_cc_modified_ss_terminal_response

DESCRIPTION:
  Processes the  Toolkit Response from the Network for call control modified
  to SS and builds the TR to the card

PARAMETERS:
  sups_info_ptr   : [Input] CM supplementary service conf information

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_sups_send_cc_modified_ss_terminal_response (
  const cm_sups_info_s_type    *sups_info_ptr
)
{
  gstk_additional_info_ptr_type                additional_result           = {0x00, NULL};
  uint8                                        default_additonal_result[2] = {0x00, 0x00};

  UIM_MSG_HIGH_0("estk_sups_send_cc_modified_ss_terminal_response");

  if (sups_info_ptr == NULL || estk_shared_info.sups_info_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null Pointer sups_info_ptr = 0x%x,estk_shared_info.sups_info_ptr = 0x%x ",
                  		sups_info_ptr, estk_shared_info.sups_info_ptr);
    return;
  }
  /* Populate L3 data if it is Register or Interrogate SS */
  switch(estk_shared_info.sups_info_ptr->ss_operation)
  {
    case ESTK_INTERROGATE_SS:
    case ESTK_REGISTER_SS:
      UIM_MSG_HIGH_1("L3 present with size 0x%x",
                     sups_info_ptr->sups_unparsed_l3_data.size );

      if ( sups_info_ptr->sups_unparsed_l3_data.present &&
         sups_info_ptr->sups_unparsed_l3_data.size > 0)
      {
        additional_result.additional_info_ptr = 
          (uint8*)GSTK_CALLOC(uint16touint8(sups_info_ptr->sups_unparsed_l3_data.size));
        if (additional_result.additional_info_ptr != NULL)
        {
          (void)gstk_memcpy(additional_result.additional_info_ptr,
                    (uint8*)sups_info_ptr->sups_unparsed_l3_data.data,
                    uint16touint8(sups_info_ptr->sups_unparsed_l3_data.size),
                    uint16touint8(sups_info_ptr->sups_unparsed_l3_data.size),
                    uint16touint8(sups_info_ptr->sups_unparsed_l3_data.size));
          additional_result.length =
            uint16touint8(sups_info_ptr->sups_unparsed_l3_data.size);
        }
      } /* L3 data present */
      break;

    /* If feature not defined, fall through */
    default:
      UIM_MSG_HIGH_3("Building SS with ss_code 0x%x, estk ss_op 0x%x, cm ss_op 0x%x",
                     sups_info_ptr->ss_code,
                     estk_shared_info.sups_info_ptr->ss_operation,
                     sups_info_ptr->ss_operation);

      default_additonal_result[0] = sups_info_ptr->ss_code;
      default_additonal_result[1] = sups_info_ptr->ss_operation;
      additional_result.additional_info_ptr = 
        (uint8*)GSTK_CALLOC(sizeof(default_additonal_result));
      if (additional_result.additional_info_ptr != NULL)
      {
        (void)gstk_memcpy(additional_result.additional_info_ptr,
                          default_additonal_result,
                          sizeof(default_additonal_result),
                          sizeof(default_additonal_result),
                          sizeof(default_additonal_result));
        additional_result.length = sizeof(default_additonal_result);
      }
      break;
  }

  if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_USSD_REQ)
  {
    estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.text = NULL;
    estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.command_result2 =
                                                                               GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
    estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr
                                                                                = additional_result.additional_info_ptr;
    estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.additional_info2_ptr.length
                                                                                = additional_result.length;
  }
  else
  {
    estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.text_string2.text = NULL;
    estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.command_result2 =
                                                                               GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
    estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.additional_info2_ptr.additional_info_ptr
                                                                                = additional_result.additional_info_ptr;
    estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.additional_info2_ptr.length
                                                                                = additional_result.length;
  }

  /* Reset the additional result to avoid dangling pointer */
  additional_result.length = 0;
  additional_result.additional_info_ptr = NULL;

  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)

  {
    UIM_MSG_HIGH_0("Saving TR");
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
  }
  else
  {
    (void)estk_send_terminal_response(
                estk_shared_info.sups_info_ptr->cmd_ref_id, /* command_details_ref */
                estk_curr_inst_ptr->extra_param.rsp_type,   /* command response    */
                GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM,  /* general_result      */
                NULL,                                       /* additional result   */
                &(estk_curr_inst_ptr->extra_param));        /* extra param         */
  }

} /* estk_sups_send_ss_success_terminal_response */


/*===========================================================================

FUNCTION: estk_sups_send_cc_modified_ussd_terminal_response

DESCRIPTION:
  Processes the Terminal Response from the SUPS ussd events from
  Network and builds the TR to the card

PARAMETERS:
  sups_info_ptr   : [Input] CM supplementary service conf information

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_sups_send_cc_modified_ussd_terminal_response (
  const cm_sups_info_s_type* sups_info_ptr)
{
  gstk_string_type*                   cbs_to_sms_text_ptr = NULL;
  gstk_call_ss_rsp_extra_param_type*  extra_param_ptr     = NULL;

  UIM_MSG_HIGH_2("In estk_sups_send_cc_modified_ussd_terminal_response: "
                 "sups_info_ptr=0x%x, "
                 "estk_shared_info.sups_info_ptr=0x%x",
                 sups_info_ptr, estk_shared_info.sups_info_ptr);

  /* Parameters check */
  if ((NULL == sups_info_ptr) || (NULL == estk_shared_info.sups_info_ptr))
  {
    return;
  }

  cbs_to_sms_text_ptr = (gstk_string_type *) GSTK_CALLOC (sizeof(gstk_string_type));

  if(cbs_to_sms_text_ptr == NULL)
  {
    estk_sups_send_err_terminal_response(FALSE, 0, 0 );
    return;
  }

  /* initialize cbs_to_sms_text */
  memset (cbs_to_sms_text_ptr, 0x00, sizeof(gstk_string_type));

  if (0 < sups_info_ptr->uss_data.size)
  {
    if( estk_sups_send_ussd_cbs_to_sms_convert (
          &cbs_to_sms_text_ptr,
          sups_info_ptr) != ESTK_SUCCESS)
    {
      UIM_MSG_ERR_0("estk_sups_send_ussd_cbs_to_sms_convert failed");
      estk_sups_send_err_terminal_response (FALSE, 0, 0 );
      if(cbs_to_sms_text_ptr->text != NULL)
      {
        gstk_free(cbs_to_sms_text_ptr->text);
        cbs_to_sms_text_ptr->text = NULL;
      }
      if(cbs_to_sms_text_ptr != NULL)
      {
        gstk_free(cbs_to_sms_text_ptr);
        cbs_to_sms_text_ptr = NULL;
      }
      return;
    }
    if (TRUE == estk_curr_inst_ptr->extra_param.present)
    {
      if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_SS_REQ)
      {
        extra_param_ptr = &(estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param);
      }
      else
      {
      extra_param_ptr = &(estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param);
      }
      extra_param_ptr->command_result2 = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      extra_param_ptr->text_string2.dcs = cbs_to_sms_text_ptr->dcs;
      extra_param_ptr->text_string2.length = cbs_to_sms_text_ptr->length;
      extra_param_ptr->text_string2.text = NULL;
      extra_param_ptr->text_string2.text = (uint8 *)GSTK_CALLOC(cbs_to_sms_text_ptr->length);
      if (extra_param_ptr->text_string2.text == NULL)
      {
        if(cbs_to_sms_text_ptr->text != NULL)
        {
          gstk_free(cbs_to_sms_text_ptr->text);
          cbs_to_sms_text_ptr->text = NULL;
        }
        if(cbs_to_sms_text_ptr != NULL)
        {
          gstk_free(cbs_to_sms_text_ptr);
          cbs_to_sms_text_ptr = NULL;
        }
        return;
      }
      (void)gstk_memcpy(extra_param_ptr->text_string2.text ,
                     cbs_to_sms_text_ptr->text,
                     cbs_to_sms_text_ptr->length,
                     cbs_to_sms_text_ptr->length,
                     cbs_to_sms_text_ptr->length);

      UIM_MSG_HIGH_2("text_string2 dcs 0x%x, ussd len 0x%x",
                     extra_param_ptr->text_string2.dcs,
                     extra_param_ptr->text_string2.length);
    }
    else
    {
      UIM_MSG_ERR_1 ("Err: extra_param.present = 0x%x",
                     estk_curr_inst_ptr->extra_param.present);
    }
  }

  /* Check whether there is a need to wait for Alpha confirmation from QMI */
  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)

  {
    UIM_MSG_HIGH_0 ("Going to pending TR state");
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
    estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
  }
  else
  {
    (void)estk_send_terminal_response(
                estk_shared_info.sups_info_ptr->cmd_ref_id, /* command_details_ref */
                estk_curr_inst_ptr->extra_param.rsp_type,   /* command response    */
                GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM,  /* general_result      */
                NULL,                                       /* additional result   */
                &(estk_curr_inst_ptr->extra_param));        /* extra param         */
  }

  if(cbs_to_sms_text_ptr->text != NULL)
  {
    gstk_free(cbs_to_sms_text_ptr->text);
    cbs_to_sms_text_ptr->text = NULL;
  }
  if(cbs_to_sms_text_ptr != NULL)
  {
    gstk_free(cbs_to_sms_text_ptr);
    cbs_to_sms_text_ptr = NULL;
  }
}

/*===========================================================================

FUNCTION: estk_sups_send_send_ussd_success_terminal_response

DESCRIPTION:
  Processes the Send USSD Toolkit Response from the Network and builds the
  TR to the card

PARAMETERS:
  sups_info_ptr : [Input] CM USSD conf information

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_sups_send_send_ussd_success_terminal_response (
  const cm_sups_info_s_type    *sups_info_ptr
)
{
  gstk_terminal_rsp_extra_param_type  extra_param         = {0x00};
  uint32                              cmd_ref_id  = 0x00;
  gstk_string_type                    *cbs_to_sms_text_ptr = NULL;

  UIM_MSG_HIGH_2("In estk_sups_send_send_ussd_success_terminal_response: "
                 "sups_info_ptr=0x%x, "
                 "estk_shared_info.sups_info_ptr=0x%x",
                 sups_info_ptr, estk_shared_info.sups_info_ptr);

  /* Parameters check */
  if ((sups_info_ptr == NULL) || (estk_shared_info.sups_info_ptr == NULL))
  {
    return;
  }

  if (estk_shared_info.sups_info_ptr->ss_operation !=
      ESTK_PROCESS_USSD_DATA)
  {
    UIM_MSG_ERR_1("req_index operation is not USSD_REQ 0x%x",
                  estk_shared_info.sups_info_ptr->ss_operation);
    return;
  }

  cbs_to_sms_text_ptr = (gstk_string_type *)GSTK_CALLOC(sizeof(gstk_string_type));

  if(cbs_to_sms_text_ptr == NULL)
  {
    estk_sups_send_err_terminal_response( FALSE, 0, 0 );
    return;
  }

  /* initialize cbs_to_sms_text */
  memset (cbs_to_sms_text_ptr, 0x00, sizeof(gstk_string_type));

  cmd_ref_id = estk_shared_info.sups_info_ptr->cmd_ref_id;

  /* CC related parameters is not supported yet */
  extra_param.present  = TRUE;
  extra_param.rsp_type = GSTK_SEND_USSD_CNF;

  if (sups_info_ptr->uss_data.size > 0)
  {
    if(estk_sups_send_ussd_cbs_to_sms_convert(
        &cbs_to_sms_text_ptr,
        sups_info_ptr) != ESTK_SUCCESS)
    {
      UIM_MSG_ERR_0 ("estk_sups_send_ussd_cbs_to_sms_convert failed");
      estk_sups_send_err_terminal_response( FALSE, 0, 0 );
      if(cbs_to_sms_text_ptr->text != NULL)
      {
        gstk_free (cbs_to_sms_text_ptr->text);
        cbs_to_sms_text_ptr->text = NULL;
      }
      if(cbs_to_sms_text_ptr != NULL)
      {
        gstk_free (cbs_to_sms_text_ptr);
        cbs_to_sms_text_ptr = NULL;
      }
      return;
    }

    extra_param.extra_param.send_ussd_extra_param.data.dcs =
      cbs_to_sms_text_ptr->dcs;
    extra_param.extra_param.send_ussd_extra_param.data.length =
      cbs_to_sms_text_ptr->length;
    extra_param.extra_param.send_ussd_extra_param.data.text =
      (uint8*)cbs_to_sms_text_ptr->text;

    UIM_MSG_HIGH_2("extra param present,ussd dcs 0x%x, ussd len 0x%x",
                   extra_param.extra_param.send_ussd_extra_param.data.dcs,
                   extra_param.extra_param.send_ussd_extra_param.data.length);
  }
  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)

  {
    UIM_MSG_HIGH_0("Saving USSD TR");
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
    estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;

    estk_curr_inst_ptr->extra_param.present = TRUE;
    estk_curr_inst_ptr->extra_param.rsp_type = GSTK_SEND_USSD_CNF;
    if (sups_info_ptr->uss_data.size > 0)
    {
      estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.dcs =
      extra_param.extra_param.send_ussd_extra_param.data.dcs;

      estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.length =
        extra_param.extra_param.send_ussd_extra_param.data.length;
      /* Memory will be freed in cleanup */
      estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.text =
         (uint8 *)GSTK_CALLOC(estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.length);
      if (estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.text == NULL)
      {
        if(cbs_to_sms_text_ptr->text != NULL)
        {
          gstk_free(cbs_to_sms_text_ptr->text);
          cbs_to_sms_text_ptr->text = NULL;
        }
        if(cbs_to_sms_text_ptr != NULL)
        {
          gstk_free(cbs_to_sms_text_ptr);
          cbs_to_sms_text_ptr = NULL;
        }
        return;
      }
      (void)gstk_memcpy(estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.text,
                     extra_param.extra_param.send_ussd_extra_param.data.text,
                     estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.length,
                     estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.length,
                     extra_param.extra_param.send_ussd_extra_param.data.length);
    }
  }
  else
  {
    (void)estk_send_terminal_response(
                cmd_ref_id,                                   /* command_details_ref */
                GSTK_SEND_USSD_CNF,                           /* command response    */
                GSTK_COMMAND_PERFORMED_SUCCESSFULLY,          /* general_result      */
                NULL,                                         /* additional result   */
                &extra_param);                                /* extra param         */
  }
  if(cbs_to_sms_text_ptr->text != NULL)
  {
    gstk_free(cbs_to_sms_text_ptr->text);
    cbs_to_sms_text_ptr->text = NULL;
  }
  if(cbs_to_sms_text_ptr != NULL)
  {
    gstk_free(cbs_to_sms_text_ptr);
    cbs_to_sms_text_ptr = NULL;
  }
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */   
} /* estk_sups_send_send_ussd_success_terminal_response */

/*===========================================================================

FUNCTION: estk_sups_send_success_terminal_response

DESCRIPTION:
  Processes the Toolkit Response from the Network and builds the
  TR to the card

PARAMETERS:
  sups_info_ptr : [Input] CM USSD conf information

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_sups_send_success_terminal_response (
  const cm_sups_info_s_type*  sups_info_ptr )
{
  UIM_MSG_HIGH_2("In estk_sups_send_success_terminal_response: "
                 "sups_info_ptr=0x%x, "
                 "estk_shared_info.sups_info_ptr=0x%x",
                 sups_info_ptr, estk_shared_info.sups_info_ptr);

  /* Parameters check */
  if (sups_info_ptr == NULL || estk_curr_inst_ptr == NULL)
  {
    return;
  }

  switch (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
    case GSTK_SEND_SS_REQ:
      if( estk_shared_info.sups_info_ptr->ss_operation == ESTK_PROCESS_USSD_DATA)
      {
        estk_sups_send_cc_modified_ussd_terminal_response(
          sups_info_ptr);
      }
      else
      {
      estk_sups_send_ss_success_terminal_response (
        sups_info_ptr);
      }
      break;

    case GSTK_SEND_USSD_REQ:
      if( estk_shared_info.sups_info_ptr->ss_operation == ESTK_PROCESS_USSD_DATA)
      {
      estk_sups_send_send_ussd_success_terminal_response (
        sups_info_ptr);
      }
      else
      {
        estk_sups_send_cc_modified_ss_terminal_response (
          sups_info_ptr);
      }
      break;

    case GSTK_SETUP_CALL_REQ:
      if( estk_shared_info.sups_info_ptr->ss_operation == ESTK_PROCESS_USSD_DATA)
      {
        estk_sups_send_cc_modified_ussd_terminal_response(
          sups_info_ptr);
      }
      else
      {
        estk_sups_send_cc_modified_ss_terminal_response (
        sups_info_ptr);
      }
      break;

    default:
      UIM_MSG_HIGH_1 ("Call Control: 0x%x -> USSD is not supported !",
                      estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
      break;
  }
} /* estk_sups_send_success_terminal_response */

/*===========================================================================

FUNCTION: estk_sups_send_err_terminal_response

DESCRIPTION:
  Processes the Send USSD and Send SS Error Toolkit Response from the Network
  and builds the TR to the card

PARAMETERS:
  err_info_present : [Input] Whether error info (i.e., the next 2 parameters
                             are valid or not)
  err_code_tag     : [Input] Error tag (valid only if err_info_present = TRUE)
  err_code         : [Input] Error code (valid only if err_info_present = TRUE)

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_sups_send_err_terminal_response (
  boolean                       err_info_present,
  uint16                        err_code_tag,
  uint16                        err_code
)
{
  gstk_additional_info_ptr_type        additional_result      = {0x00, NULL};
  uint8                                additional_result_data = 0;
  gstk_call_ss_rsp_extra_param_type*   extra_param_ptr = NULL;

  UIM_MSG_HIGH_1("estk_sups_send_err_terminal_response: "
                 "estk_shared_info.sups_info_ptr=0x%x",
                 estk_shared_info.sups_info_ptr);

  if (estk_shared_info.sups_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("priv.sups_info_ptr Null");
    return;
  }

  /* Populate error code info if available */
  if (err_info_present)
  {
    if (((err_code_tag & 0x00FF) == ESTK_ERROR_CODE_TAG) ||
        ((err_code_tag & 0x00FF) == ESTK_RETURN_RESULT_PROBLEM))
    {
      additional_result_data = (uint8)(err_code & 0x00FF);
    }

    UIM_MSG_HIGH_1("err_info_present, additional_result data = 0x%x ",
                   additional_result_data);
  }
  additional_result.additional_info_ptr = &additional_result_data;
  additional_result.length = sizeof(additional_result_data);

  switch (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
    case GSTK_SEND_SS_REQ:
      if (TRUE == estk_curr_inst_ptr->extra_param.present)
      {
        UIM_MSG_HIGH_0 ("extra_param.present = TRUE");
        extra_param_ptr = &(estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param);
        extra_param_ptr->command_result2 = GSTK_USSD_RETURN_ERROR;

        if(additional_result.additional_info_ptr != NULL &&
           additional_result.length > 0)
        {
          extra_param_ptr->additional_info2_ptr.additional_info_ptr = (uint8*)GSTK_CALLOC(additional_result.length);
        }
        if (extra_param_ptr->additional_info2_ptr.additional_info_ptr != NULL)
        {
          (void)gstk_memcpy(extra_param_ptr->additional_info2_ptr.additional_info_ptr,
                 additional_result.additional_info_ptr,
                 additional_result.length,
                 additional_result.length,
                 additional_result.length);
          extra_param_ptr->additional_info2_ptr.length = additional_result.length;
        }
        if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
        {
           UIM_MSG_HIGH_0("Saving SEND SS Error TR");
           estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
           estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
        }
        else
        {        
          (void)estk_send_terminal_response(
                  estk_shared_info.sups_info_ptr->cmd_ref_id,     /* command_details_ref */
                  GSTK_SEND_SS_CNF,                            /* command response */
                  GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM,   /* general_result */
                  NULL,                                           /* additional result */
                  &(estk_curr_inst_ptr->extra_param));            /* extra param */
        }
        break;
      }
      switch (estk_shared_info.sups_info_ptr->ss_operation)
      {
        case ESTK_NULL_SS_OPERATION:
        case ESTK_REGISTER_SS:
        case ESTK_ERASE_SS:
        case ESTK_ACTIVATE_SS:
        case ESTK_DEACTIVATE_SS:
        case ESTK_INTERROGATE_SS:
          if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
          {
            estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
            estk_curr_inst_ptr->result = GSTK_SS_RETURN_ERROR;
            estk_cache_additional_info(&additional_result);
          }
          else
          {
          (void)estk_send_terminal_response(
                  estk_shared_info.sups_info_ptr->cmd_ref_id,     /* command_details_ref */
                  GSTK_SEND_SS_CNF,                               /* command response */
                  GSTK_SS_RETURN_ERROR,                           /* general_result */
                  &additional_result,                             /* additional result */
                  NULL);                                          /* extra param */
          }
          break;
        default:
          UIM_MSG_HIGH_2 ("Unexpected ss_operation 0x%x command 0x%x",
                          estk_shared_info.sups_info_ptr->ss_operation,
                          estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
          break;
      } /* switch (estk_shared_info.sups_info_ptr->ss_operation) */
      break;

    case GSTK_SEND_USSD_REQ:
      if (TRUE == estk_curr_inst_ptr->extra_param.present)
      {
        UIM_MSG_HIGH_0 ("extra_param.present = TRUE");
        switch(estk_shared_info.sups_info_ptr->ss_operation)
        {
          case ESTK_NULL_SS_OPERATION:
          case ESTK_REGISTER_SS:
          case ESTK_ERASE_SS:
          case ESTK_ACTIVATE_SS:
          case ESTK_DEACTIVATE_SS:
          case ESTK_INTERROGATE_SS:
            estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.command_result2 = GSTK_SS_RETURN_ERROR;
            break;
          default:
            break;
        }
        if (additional_result.additional_info_ptr != NULL &&
            additional_result.length > 0)
        {
          estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr
                                                                      = (uint8*)GSTK_CALLOC(additional_result.length);
        }
        if (estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr != NULL)
        {
          (void)gstk_memcpy(estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr,
                 additional_result.additional_info_ptr,
                 additional_result.length,
                 additional_result.length,
                 additional_result.length);
          estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.additional_info2_ptr.length = additional_result.length;
        }
        if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
        {
          UIM_MSG_HIGH_0("Saving USSD Error TR");
          estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
          estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
        }
        else
        {
          (void)estk_send_terminal_response(
                   estk_shared_info.sups_info_ptr->cmd_ref_id,     /* command_details_ref */
                   GSTK_SEND_USSD_CNF,                            /* command response */
                   GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM,   /* general_result */
                   NULL,                                           /* additional result */
                   &(estk_curr_inst_ptr->extra_param));            /* extra param */
        }
      }
      else if(estk_shared_info.sups_info_ptr->ss_operation == ESTK_PROCESS_USSD_DATA)
      {
         if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
         {
           UIM_MSG_HIGH_0("Saving USSD Error TR");
           estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
           estk_curr_inst_ptr->result = GSTK_USSD_RETURN_ERROR;
           estk_cache_additional_info(&additional_result);
         }
         else
         {
           (void)estk_send_terminal_response(
                  estk_shared_info.sups_info_ptr->cmd_ref_id,     /* command_details_ref */
                  GSTK_SEND_USSD_CNF,                             /* command response */
                  GSTK_USSD_RETURN_ERROR,                         /* general_result */
                  &additional_result,                             /* additional result */
                  NULL);                                          /* extra param */
         }
      }
      else
      {
        UIM_MSG_HIGH_2 ("Unexpected ss_operation 0x%x command 0x%x",
                        estk_shared_info.sups_info_ptr->ss_operation,
                        estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
      }
      break;

    case GSTK_SETUP_CALL_REQ:
      if (TRUE == estk_curr_inst_ptr->extra_param.present)
      {
        UIM_MSG_HIGH_0 ("extra_param.present = TRUE");
        extra_param_ptr = &(estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param);
        switch(estk_shared_info.sups_info_ptr->ss_operation)
        {
          case ESTK_PROCESS_USSD_DATA:
            extra_param_ptr->command_result2 = GSTK_USSD_RETURN_ERROR;
            break;
          case ESTK_NULL_SS_OPERATION:
          case ESTK_REGISTER_SS:
          case ESTK_ERASE_SS:
          case ESTK_ACTIVATE_SS:
          case ESTK_DEACTIVATE_SS:
          case ESTK_INTERROGATE_SS:
            extra_param_ptr->command_result2 = GSTK_SS_RETURN_ERROR;
            break;
          default:
            break;
        }

        if (additional_result.additional_info_ptr != NULL &&
            additional_result.length > 0)
        {
          extra_param_ptr->additional_info2_ptr.additional_info_ptr = (uint8*)GSTK_CALLOC(additional_result.length);
        }
        if (extra_param_ptr->additional_info2_ptr.additional_info_ptr != NULL)
        {
          (void)gstk_memcpy(extra_param_ptr->additional_info2_ptr.additional_info_ptr,
                 additional_result.additional_info_ptr,
                 additional_result.length,
                 additional_result.length,
                 additional_result.length);
          extra_param_ptr->additional_info2_ptr.length = additional_result.length;
        }
        if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
        {
           UIM_MSG_HIGH_0("Saving Setup Call Error TR");
           estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
           estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
        }
        else
        {
          (void)estk_send_terminal_response(
                  estk_shared_info.sups_info_ptr->cmd_ref_id,     /* command_details_ref */
                  GSTK_SETUP_CALL_CNF,                            /* command response */
                  GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM,   /* general_result */
                  NULL,                                           /* additional result */
                  &(estk_curr_inst_ptr->extra_param));            /* extra param */
        }
      }
      else
      {
        UIM_MSG_HIGH_1 ("Incorrect extra_param.present. Commmand 0x%x",
                        estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
      }
      break;

    default:
      UIM_MSG_HIGH_1 ("Unexpected command 0x%x",
                      estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
      break;
  } /* switch (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id) */

  /* Reset additional_result to avoid dangling pointers */
  additional_result.additional_info_ptr = NULL;
  additional_result.length              = 0;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */    
} /* estk_ss_send_err_terminal_response */

/*===========================================================================
FUNCTION: estk_sups_process_cm_ss_evt

DESCRIPTION:
  This function takes the information from the CM sups info pointer and
  operates on it.
  Base on whether a successful SS terminal response should be sent or not,
  the function will call the corresponding functions to send a
  successful or failure terminal response.

PARAMETERS:
  sups_info_ptr   : [Input] CM supplementary service conf information

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_sups_process_cm_ss_evt(
  const cm_sups_info_s_type    *sups_info_ptr
)
{
  UIM_MSG_HIGH_2("estk_sups_process_cm_ss_evt: "
                 "sups_info_ptr=0x%x, "
                 "estk_shared_info.sups_info_ptr=0x%x",
                 sups_info_ptr, estk_shared_info.sups_info_ptr);
  if (sups_info_ptr == NULL || estk_shared_info.sups_info_ptr == NULL)
  {
    return;
  }
  if (estk_shared_info.sups_info_ptr->req_id != sups_info_ptr->ss_ref)
  {
    UIM_MSG_ERR_2("priv.sups_info_ptr req_id 0x%x matches rsp_ptr req_id 0x%x",
                  estk_shared_info.sups_info_ptr->req_id,
                  sups_info_ptr->ss_ref);
    return;
  }

  if (sups_info_ptr->ss_success)
  {
    /* SS error detected from NW */
    switch (sups_info_ptr->conf_type)
    {
      case SS_ERROR_INFO:
        /* Failed SS Response from Network */
        estk_sups_send_err_terminal_response(
          sups_info_ptr->ss_error.present,
          sups_info_ptr->ss_error.error_code_tag,
          sups_info_ptr->ss_error.error_code);
        break;

      default:
        /* Successful SS Response from Network */
        estk_sups_send_success_terminal_response(
          sups_info_ptr);
        break;
    } /* switch sups_info_ptr->conf_type */
  } /* if ss_success */
  else
  {
    /* Failed SS response */
    estk_sups_send_err_terminal_response(
      FALSE,
      0,     /* error tag  */
      0 );   /* error code */
  } /* if !ss_success */

} /* estk_sups_process_cm_ss_evt */

/*===========================================================================
FUNCTION: estk_sups_process_cm_ussd_evt

DESCRIPTION:
  This function takes the information from the CM USSD sups info pointer and
  operates on it.
  Base on whether a successful USSD terminal response should be sent or not,
  the function will call the corresponding functions to send a
  successful or failure terminal response.

PARAMETERS:
  sups_info_ptr   : [Input] CM USSD conf information

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_sups_process_cm_ussd_evt(
  const cm_sups_info_s_type    *sups_info_ptr
)
{
  UIM_MSG_HIGH_3("In estk_sups_process_cm_ussd_evt: "
                 "sups_info_ptr=0x%x, "
                 "estk_curr_inst_ptr=0x%x, "
                 "estk_shared_info.sups_info_ptr=0x%x",
                 sups_info_ptr,
                 estk_curr_inst_ptr,
                 estk_shared_info.sups_info_ptr);

  /* Input parameter null pointer check */
  if (sups_info_ptr == NULL ||
      estk_curr_inst_ptr == NULL ||
      estk_shared_info.sups_info_ptr == NULL)
  {
    return;
  }

  if (sups_info_ptr->ss_success)
  {
    switch (sups_info_ptr->conf_type)
    {
      case USS_DATA_INFO: /* Successful USSD responses from Network */
        estk_sups_send_success_terminal_response(
          sups_info_ptr);
        break;

      case SS_ERROR_INFO:
        /* Failed USSD response from Network */
        estk_sups_send_err_terminal_response(
          sups_info_ptr->ss_error.present,
          sups_info_ptr->ss_error.error_code_tag,
          sups_info_ptr->ss_error.error_code);
        break;

      case CC_CAUSE_INFO:
        /* Treat as USSD failure */
        estk_sups_send_err_terminal_response(
          FALSE,
          0,
          0);
        break;

    default:
      UIM_MSG_ERR_1("Unhandled conf_type 0x%x", sups_info_ptr->conf_type);
      return;
    } /* switch sups_info_ptr->conf_type */
  } /* if ss_success */
  else
  {
    /* Failed USSD response */
    estk_sups_send_err_terminal_response(
      FALSE,
      0,     /* error tag  */
      0 );   /* error code */
  } /* if !ss_success */

} /* estk_sups_process_cm_ussd_evt */

/*===========================================================================
FUNCTION: estk_cm_sups_event_cb

DESCRIPTION:
  This is the event callback function registered with CM for supplementary
  service events

PARAMETERS:
  sups_event    : [Input] CM supplementary service event
  sups_info_ptr : [Input] CM supplementary service data info

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_sups_event_cb(
  cm_sups_event_e_type          sups_event,
  const cm_sups_info_s_type    *sups_info_ptr
)
{
  gstk_slot_id_enum_type   slot         = GSTK_SLOT_1;
  estk_cmd_type           *task_cmd_ptr = NULL;

  UIM_MSG_HIGH_2("Receive ESTK CM Supps Event 0x%x, sups_info_ptr=0x%x",
                 sups_event, sups_info_ptr);

  /* Input parameters and private data pointers check */
  if (sups_info_ptr == NULL)
  {
    return;
  }

  if (gstk_io_ctrl(
         GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
         &sups_info_ptr->asubs_id,
         &slot) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get slot for as_id 0x%x!",
                  sups_info_ptr->asubs_id);
    return;
  }


  if (!ESTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return;
  }

  if (!estk_instances_ptr[(uint8)slot - 1]->curr_cmd.hdr_cmd.command_id)
  {
    UIM_MSG_ERR_0("estk_cm_sups_event_cb: No SUPS cmd");
  }

  task_cmd_ptr = estk_get_cmd_buf();
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  task_cmd_ptr->hdr.cmd                       = ESTK_CM_SUPS_EVENT_CMD;
  task_cmd_ptr->hdr.sim_slot_id               = slot;
  task_cmd_ptr->hdr.cmd_ref_id                =
    estk_instances_ptr[(uint8)slot - 1]->curr_cmd.hdr_cmd.cmd_detail_reference;
  task_cmd_ptr->hdr.cli_user_data             = 0;
  task_cmd_ptr->result                        = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.sups_info.evt_id     = sups_event;
  (void)gstk_memcpy(&task_cmd_ptr->cmd_data.sups_info.evt_info,
                 sups_info_ptr,
                 sizeof(task_cmd_ptr->cmd_data.sups_info.evt_info),
                 sizeof(cm_sups_info_s_type),
                 sizeof(cm_sups_info_s_type));

  estk_put_cmd(task_cmd_ptr);

} /* estk_cm_sups_event_cb */

/*===========================================================================
FUNCTION: estk_cm_sups_cmd_cb

DESCRIPTION:
  This is the command callback function for the CM sups event processing
  procedure.

PARAMETERS:
  data_block_ptr : [Input] User data
  cmd            : [Input] CM supplementary service command
  cmd_err        : [Input] command result
  err_cause      : [Input] error cause
  alpha          : [Input] alpha string

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_cm_sups_cmd_cb(
  void*                         data_block_ptr,
  cm_sups_cmd_e_type            cmd,
  cm_sups_cmd_err_e_type        cmd_err,
  cm_sups_cmd_err_cause_e_type  err_cause,
  cm_alpha_s_type               alpha
)
{
  estk_sups_ss_operation_enum_type estk_ss_operation    = ESTK_MAX_SS_OPERATION;
  gstk_cmd_enum_type               cnf_type             = GSTK_CMD_ENUM_NOT_USE;
  uint32                           cmd_ref_id           = 0x00;
  gstk_general_result_enum_type    result               = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  gstk_additional_info_ptr_type    additional_result    = {0x00};
  uint8                            additional_info_data = 0;
  gstk_terminal_rsp_extra_param_type *extra_param_ptr      = NULL;
  gstk_terminal_rsp_extra_param_type  extra_param;

  /* If CM notifies error in the command callback, this means
     the SS or USSD requests were not able to be sent to the network */
  if (cmd_err != CM_SUPS_CMD_ERR_NOERR)
  {
    UIM_MSG_ERR_2("CM Sups Call Cmd Failed 0x%x for Cmd type 0x%x",
                  cmd_err, cmd);

    UIM_MSG_HIGH_2("estk_cm_sups_cmd_cb: data_block_ptr=0x%x, "
                   "estk_shared_info.sups_info_ptr=0x%x",
                   data_block_ptr, estk_shared_info.sups_info_ptr);

    if ((!estk_shared_info.sups_info_ptr)
        || (!data_block_ptr)
        || ((uint32)data_block_ptr != (uint32)estk_shared_info.sups_info_ptr))
    {
      return;
    }

    if (!ESTK_IS_VALID_SLOT_ID(estk_shared_info.slot_owns_cm_cli))
    {
      UIM_MSG_ERR_1("estk_cm_sups_cmd_cb: invalid slot id 0x%x!",
                    estk_shared_info.slot_owns_cm_cli);
      return;
    }

    switch(cmd_err)
    {
      case CM_SUPS_CMD_ERR_CALL_CONTROL_REJECTED:
        result = GSTK_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM;
        additional_result.length = 1;
        additional_info_data = int32touint8((int32)GSTK_ACTION_NOT_ALLOWED);
        additional_result.additional_info_ptr = &additional_info_data;
        break;
      case CM_SUPS_CMD_ERR_CALL_CONTROL_FAILURE:
        result = GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM;
        break;
      default:
        result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        break;
    }

    switch(cmd)
    {
    case CM_SUPS_CMD_REGISTER:
      /* Register Supplementary Service Request */
      estk_ss_operation = ESTK_REGISTER_SS;
      cnf_type = GSTK_SEND_SS_CNF;
      break;
    case CM_SUPS_CMD_ERASE:
      /* Erase Supplementary Service Request */
      estk_ss_operation = ESTK_ERASE_SS;
      cnf_type = GSTK_SEND_SS_CNF;
      break;
    case CM_SUPS_CMD_ACTIVATE:
      /* Activate Supplementary Service Request */
      estk_ss_operation = ESTK_ACTIVATE_SS;
      cnf_type = GSTK_SEND_SS_CNF;
      break;
    case CM_SUPS_CMD_DEACTIVATE:
      /* Deactivate Supplementary Service Request */
      estk_ss_operation = ESTK_DEACTIVATE_SS;
      cnf_type = GSTK_SEND_SS_CNF;
      break;
    case CM_SUPS_CMD_INTERROGATE:
      /* Interrogate Supplementary Service Request */
      estk_ss_operation = ESTK_INTERROGATE_SS;
      cnf_type = GSTK_SEND_SS_CNF;
      break;
    case CM_SUPS_CMD_PROCESS_USS:
      estk_ss_operation = ESTK_PROCESS_USSD_DATA;
      cnf_type = GSTK_SEND_USSD_CNF;
      break;
    default:
      UIM_MSG_HIGH_1(" cm sups cmd not handled by estk 0x%x", cmd);
      return;
    }

    if( (estk_instances_ptr[estk_shared_info.slot_owns_cm_cli -1]->curr_cmd.hdr_cmd.command_id
               == GSTK_SEND_USSD_REQ && estk_ss_operation != ESTK_PROCESS_USSD_DATA) ||
          (estk_instances_ptr[estk_shared_info.slot_owns_cm_cli -1]->curr_cmd.hdr_cmd.command_id
               == GSTK_SEND_SS_REQ &&  estk_ss_operation == ESTK_PROCESS_USSD_DATA))
    {
      memset(&extra_param, 0,sizeof(extra_param));
      /* SEND USSD is modified to SS type or SEND SS MODIFIED TO USSD*/
      if(estk_ss_operation == ESTK_PROCESS_USSD_DATA)
      {
         //SEND SS modified to USSD
         cnf_type = GSTK_SEND_SS_CNF;
         extra_param.rsp_type = GSTK_SEND_SS_CNF;
         extra_param.extra_param.send_ss_extra_param.command_result2
                 = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
         extra_param.extra_param.send_ss_extra_param.has_cc_type_modification = TRUE;
      }
      else
      {
         // SEND USSD to SS
         cnf_type = GSTK_SEND_USSD_CNF;
         extra_param.rsp_type = GSTK_SEND_USSD_CNF;
         extra_param.extra_param.send_ussd_extra_param.command_result2
                 = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
         extra_param.extra_param.send_ussd_extra_param.has_cc_type_modification = TRUE;
      }
      extra_param.present = TRUE;
      extra_param_ptr = &extra_param;
      result = GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM;
    }

    cmd_ref_id = estk_shared_info.sups_info_ptr->cmd_ref_id;
    if(estk_instances_ptr[estk_shared_info.slot_owns_cm_cli -1]->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving USSD/SS Error TR");
      estk_instances_ptr[estk_shared_info.slot_owns_cm_cli -1]->state = ESTK_PENDING_TR_ST;
      estk_instances_ptr[estk_shared_info.slot_owns_cm_cli -1]->result = result;
      estk_cache_additional_info(&additional_result);
    }
    else
    {
      (void) estk_send_terminal_response(
               cmd_ref_id,         /* command_details_ref */
               cnf_type,           /* command response */
               result,             /* general_result */
               &additional_result, /* additional result */
               extra_param_ptr);   /* extra param */
    }
  }
} /* estk_cm_sups_cmd_cb*/

/*===========================================================================

FUNCTION: estk_sups_process_send_ussd_req

DESCRIPTION:
  Process the Send USSD Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] request pointer for USSD in gstk format

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_sups_process_send_ussd_req (
   gstk_cmd_from_card_type *gstk_req_ptr
)
{
  cm_uss_params_s_type      *uss_cmd_params_ptr = NULL;
  estk_sups_info_type       *priv_sups_ptr      = NULL;
  sys_modem_as_id_e_type     as_id              = SYS_MODEM_AS_ID_1;
  cm_num_s_type              dialed_digits;
  gstk_additional_info_ptr_type   additional_result      = {0x00};
  uint8                           additional_result_data = 0;
  boolean                         send_tr                = FALSE;

  UIM_MSG_HIGH_1("estk_shared_info.slot_owns_cm_cli=0x%x",
                 estk_shared_info.slot_owns_cm_cli);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);
  
  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  /* Input parameters check for Send USSD data validity */
  if (gstk_req_ptr == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  if(estk_curr_inst_ptr->ussd_transaction)
  {
     additional_result_data = (uint8)GSTK_ME_CURRENTLY_BUSY_ON_USSD_TRANSACTION;
     send_tr = TRUE;
  }
  if(estk_curr_inst_ptr->ss_transaction)
  {
     additional_result_data = (uint8)GSTK_ME_CURRENTLY_BUSY_ON_SS_TRANSACTION;
     send_tr = TRUE;
  }
  if(send_tr)
  {
     additional_result.length = 1;
     additional_result.additional_info_ptr = &additional_result_data;
     (void)estk_send_terminal_response(
            gstk_req_ptr->hdr_cmd.cmd_detail_reference,
            GSTK_SEND_USSD_CNF,
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
            &additional_result,
            NULL);

     return ESTK_SUCCESS;
  }

  if (estk_shared_info.slot_owns_cm_cli != GSTK_SLOT_ID_MAX)
  {
    return ESTK_ERROR;
  }
  else
  {
    estk_shared_info.slot_owns_cm_cli = gstk_req_ptr->hdr_cmd.sim_slot_id;
  }

  if (gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length >
      sizeof(uss_cmd_params_ptr->uss_data.ussData))
  {
    /* Toolkit USSD String is too long */
    UIM_MSG_ERR_1("USSD String too long 0x%x",
                  gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length);
    return ESTK_ERROR;
  }

  /* set all the fields of the Process USS request */
  priv_sups_ptr = estk_sups_alloc_populate_priv_data(
            gstk_req_ptr->hdr_cmd.cmd_detail_reference,
            ESTK_PROCESS_USSD_DATA);
  if (priv_sups_ptr == NULL)
  {
    /* Cannot allocate memory for the private data*/
    return ESTK_ERROR;
  }

  /* USSD Data */
  /* length check */
  if (gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length == 0)
  {
    /* Invalid input parameter */
    /* priv data allocated is freed when a TR is sent or
       when a End Proactive Session is received */
    UIM_MSG_ERR_0("USSD String 0 length");
    return ESTK_ERROR;
  }

  /* USSD string null pointer check */
  if ((gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length > 0) &&
      (gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.text == NULL))
  {
    /* Invalid input parameter */
    /* priv data allocated is freed when a TR is sent or
       when a End Proactive Session is received */
    UIM_MSG_ERR_0("USSD String Null Text Pointer");
    return ESTK_ERROR;
  }

  /* Check Alpha availability */
  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == FALSE)
  {
    /* Even in case of null data object in ALPHA (i.e. length = '00' and no value part),
       GSTK make sures at least NULL char is present in alpha text and length includes
       that NULL character. So the min length is one and hence we need to check for
       length more than one */
    if(((gstk_req_ptr->cmd.send_ussd_pro_cmd_req.alpha.length) > 1 &&
       (gstk_req_ptr->cmd.send_ussd_pro_cmd_req.alpha.text != NULL )) ||
       (gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
           GSTK_SLOT_ID_MAX) == TRUE))
    {
      if (estk_send_alpha_to_display_client(gstk_req_ptr, TRUE) != ESTK_SUCCESS)
      {
        UIM_MSG_ERR_0("Alpha Display failed");
        /* priv data allocated is freed when a TR is sent or
           when a End Proactive Session is received */
        return ESTK_ERROR;
      }
    }
  }

  /* Populate CM USSD parameter */
  uss_cmd_params_ptr =
    (cm_uss_params_s_type*)GSTK_CALLOC(sizeof(cm_uss_params_s_type));

  if(uss_cmd_params_ptr == NULL)
  {
    /* priv data allocated is freed when a TR is sent or
       when a End Proactive Session is received */
    return ESTK_ERROR;
  }

  /* Check to ensure USSD string length will not exceed the limit
     imposed by CM buffer */
  if (gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length >
       sizeof(uss_cmd_params_ptr->uss_data.ussData))
  {
    UIM_MSG_ERR_2("GSTK USSD String 0x%x > CM capability 0x%x",
                  gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length,
                  sizeof(uss_cmd_params_ptr->uss_data.ussData));
    gstk_free(uss_cmd_params_ptr);
    /* priv data allocated is freed when a TR is sent or
       when a End Proactive Session is received */
    return ESTK_ERROR;
  }

  /* ss_ref is the req_id from the private data in ESTK's sups info */
  uss_cmd_params_ptr->ss_ref = estk_shared_info.sups_info_ptr->req_id;

  /* Copy USSD string into CM structure */
  uss_cmd_params_ptr->uss_data.present = TRUE;

  uss_cmd_params_ptr->uss_data.uss_data_coding_scheme =
    gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.orig_dcs_from_sim;

  (void)memscpy(uss_cmd_params_ptr->uss_data.ussData,
  	 sizeof(uss_cmd_params_ptr->uss_data.ussData),
         gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.text,
         gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length);

  uss_cmd_params_ptr->uss_data.size =
    gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length;

  dialed_digits.digit_mode = CM_DIGIT_MODE_8BIT_ASCII;

  (void)memscpy(dialed_digits.buf, sizeof(dialed_digits.buf),
  	             gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.text,
  	             gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length);

  dialed_digits.len = gstk_req_ptr->cmd.send_ussd_pro_cmd_req.ussd_string.length;

  UIM_MSG_HIGH_2("Calling cm_sups_cmd_process_uss with ussd_string len 0x%x, ss_ref 0x%x",
                 uss_cmd_params_ptr->uss_data.size,
                 uss_cmd_params_ptr->ss_ref);

  if (gstk_io_ctrl(
        GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
        &estk_shared_info.slot_owns_cm_cli,
        &as_id) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                  estk_shared_info.slot_owns_cm_cli);
    gstk_free(uss_cmd_params_ptr);
    return ESTK_ERROR;
  }


  if( cm_sups_cmd_process_uss_exten_per_subs (estk_cm_sups_cmd_cb,
                                              (void*)priv_sups_ptr,
                                              estk_shared_info.cm_client_id,
                                              uss_cmd_params_ptr,
                                              &dialed_digits,
                                              CM_ORIG_CLIENT_CATEG_CARD,
                                              FALSE,
                                              as_id) == FALSE)
  {
    UIM_MSG_ERR_0("cm_sups_cmd_process_uss failed");
    gstk_free(uss_cmd_params_ptr);
    /* priv data allocated is freed when a TR is sent or
       when a End Proactive Session is received */
    return ESTK_ERROR;
  }

  gstk_free(uss_cmd_params_ptr);
  return ESTK_SUCCESS;
} /* estk_sups_process_send_ussd_req*/

/*===========================================================================
FUNCTION: estk_ss_activate

DESCRIPTION:
  The function processes the SS Activation request and sends the request
  to CM

PARAMETERS:
  ss_string_ptr      : [Input] SS Activation string pointer
  estk_sups_info_ptr : [Input] ESTK sups info pointer
  dialed_digits      : [Input] Dialed digits

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_ss_activate(
  const ssparser_ss_string_info_type *ss_string_ptr,
  estk_sups_info_type                *estk_sups_info_ptr,
  cm_num_s_type                      *dialed_digits
)
{
  estk_result_enum_type   estk_result = ESTK_SUCCESS;
  cm_act_ss_params_s_type act_params;
  sys_modem_as_id_e_type  as_id       = SYS_MODEM_AS_ID_1;

  UIM_MSG_HIGH_2("estk_ss_activate: ss_string_ptr=0x%x, estk_sups_info_ptr=0x%x",
                 ss_string_ptr, estk_sups_info_ptr);


  if ((ss_string_ptr == NULL) || (estk_sups_info_ptr == NULL))
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  memset(&act_params, 0x00, sizeof(act_params));
  act_params.ss_code = int32touint8((int32)ss_string_ptr->sc);
  act_params.ss_ref  = estk_sups_info_ptr->req_id;

  /* SIB only ever denotes a Basic Service */
  if (ssparser_parse_basic_service(ss_string_ptr->sib_ptr,
                                   ss_string_ptr->sib_length,
                                   &act_params.basic_service.code_type,
                                   &act_params.basic_service.bs_code))
  {
    act_params.basic_service.present = TRUE;
  }


  if (gstk_io_ctrl(
        GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
        &estk_shared_info.slot_owns_cm_cli,
        &as_id) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                  estk_shared_info.slot_owns_cm_cli);
    return ESTK_ERROR;
  }

  if (!cm_sups_cmd_activate_cc_exten_per_subs(estk_cm_sups_cmd_cb,
                                              estk_sups_info_ptr,
                                              estk_shared_info.cm_client_id,
                                              &act_params,
                                              dialed_digits,
                                              CM_ORIG_CLIENT_CATEG_CARD,
                                              FALSE,
                                              as_id))
  {
    UIM_MSG_ERR_0("cm_sups_cmd_activate failed");
    estk_result = ESTK_ERROR;
  }

  return estk_result;

} /* estk_ss_activate */

/*===========================================================================
FUNCTION: estk_ss_deactivate

DESCRIPTION:
  The function processes the SS Deactivation request and sends the request
  to CM

PARAMETERS:
  ss_string_ptr      : [Input] SS Deactivation string pointer
  estk_sups_info_ptr : [Input] ESTK sups info pointer
  dialed_digits      : [Input] Dialed digits

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None


===========================================================================*/
static estk_result_enum_type estk_ss_deactivate(
  const ssparser_ss_string_info_type *ss_string_ptr,
  estk_sups_info_type                *estk_sups_info_ptr,
  cm_num_s_type                      *dialed_digits
)
{
  estk_result_enum_type     estk_result = ESTK_SUCCESS;
  cm_deact_ss_params_s_type deact_params;
  sys_modem_as_id_e_type    as_id       = SYS_MODEM_AS_ID_1;

  UIM_MSG_HIGH_2("estk_ss_deactivate: ss_string_ptr=0x%x, estk_sups_info_ptr=0x%x",
                 ss_string_ptr, estk_sups_info_ptr);

  if ((ss_string_ptr == NULL) || (estk_sups_info_ptr == NULL))
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  memset(&deact_params, 0x00, sizeof(deact_params));
  deact_params.ss_code = int32touint8((int32)ss_string_ptr->sc);
  deact_params.ss_ref  = estk_sups_info_ptr->req_id;

  /* SIB only ever denotes a Basic Service */
  if (ssparser_parse_basic_service(ss_string_ptr->sib_ptr,
                                   ss_string_ptr->sib_length,
                                   &deact_params.basic_service.code_type,
                                   &deact_params.basic_service.bs_code))
  {
    deact_params.basic_service.present = TRUE;
  }

  /* SIA may be an index value, for CCBS */
  if (ss_string_ptr->sia_type == SSPARSER_SI_BYTE)
  {
    if (!ssparser_parse_uint8(ss_string_ptr->sia_ptr,
                              ss_string_ptr->sia_length,
                              &deact_params.ccbs_indx))
    {
      deact_params.ccbs_indx = CM_CCBS_INDX_INVALID;
    }
  }

  if (gstk_io_ctrl(
        GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
        &estk_shared_info.slot_owns_cm_cli,
        &as_id) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                  estk_shared_info.slot_owns_cm_cli);
    return ESTK_ERROR;
  }

  if (!cm_sups_cmd_deactivate_cc_exten_per_subs(estk_cm_sups_cmd_cb,
                                                estk_sups_info_ptr,
                                                estk_shared_info.cm_client_id,
                                                &deact_params,
                                                dialed_digits,
                                                CM_ORIG_CLIENT_CATEG_CARD,
                                                FALSE,
                                                as_id))
  {
    UIM_MSG_ERR_0("cm_sups_cmd_deactivate failed");
    estk_result = ESTK_ERROR;
  }

  return estk_result;

} /* estk_ss_deactivate */

/*===========================================================================
FUNCTION: estk_ss_interrogate

DESCRIPTION:
  The function processes the SS Interrogate request and sends the request
  to CM

PARAMETERS:
  ss_string_ptr      : [Input] SS Interrogate string pointer
  estk_sups_info_ptr : [Input] ESTK sups info pointer
  dialed_digits      : [Input] Dialed digits

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_ss_interrogate(
  const ssparser_ss_string_info_type *ss_string_ptr,
  estk_sups_info_type                *estk_sups_info_ptr,
  cm_num_s_type                      *dialed_digits
)
{
  estk_result_enum_type      estk_result     = ESTK_SUCCESS;
  cm_interr_ss_params_s_type interrog_params;
  sys_modem_as_id_e_type     as_id           = SYS_MODEM_AS_ID_1;

  UIM_MSG_HIGH_2("estk_ss_interrogate: ss_string_ptr=0x%x, estk_sups_info_ptr=0x%x",
                 ss_string_ptr, estk_sups_info_ptr);

  if ((ss_string_ptr == NULL) || (estk_sups_info_ptr == NULL))
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  memset(&interrog_params, 0x00, sizeof(interrog_params));
  interrog_params.ss_code = int32touint8((int32)ss_string_ptr->sc);
  interrog_params.ss_ref  = estk_sups_info_ptr->req_id;

  /* SIB only ever denotes a Basic Service */
  if (ssparser_parse_basic_service(ss_string_ptr->sib_ptr,
                                   ss_string_ptr->sib_length,
                                   &interrog_params.basic_service.code_type,
                                   &interrog_params.basic_service.bs_code))
  {
    interrog_params.basic_service.present = TRUE;
  }

  /* SIA may be an index value, for CCBS */
  if (ss_string_ptr->sia_type == SSPARSER_SI_BYTE)
  {
    if (!ssparser_parse_uint8(ss_string_ptr->sia_ptr,
                              ss_string_ptr->sia_length,
                              &interrog_params.ccbs_indx))
    {
      interrog_params.ccbs_indx = CM_CCBS_INDX_INVALID;
    }
  }

  if (gstk_io_ctrl(
        GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
        &estk_shared_info.slot_owns_cm_cli,
        &as_id) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                  estk_shared_info.slot_owns_cm_cli);
    return ESTK_ERROR;
  }

  if (!cm_sups_cmd_interrogate_cc_exten_per_subs(estk_cm_sups_cmd_cb,
                                                 estk_sups_info_ptr,
                                                 estk_shared_info.cm_client_id,
                                                 &interrog_params,
                                                 dialed_digits,
                                                 CM_ORIG_CLIENT_CATEG_CARD,
                                                 FALSE,
                                                 as_id))
  {
    UIM_MSG_ERR_0("cm_sups_cmd_interrogate failed");
    estk_result = ESTK_ERROR;
  }

  return estk_result;

} /* estk_ss_interrogate */

/*===========================================================================
FUNCTION: estk_ss_register

DESCRIPTION:
  The function processes the SS Register request and sends the request
  to CM

PARAMETERS:
  ss_string_ptr      : [Input] SS Register string pointer
  estk_sups_info_ptr : [Input] ESTK sups info pointer
  dialed_digits      : [Input] Dialed digits

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_ss_register(
  const ssparser_ss_string_info_type *ss_string_ptr,
  estk_sups_info_type                *estk_sups_info_ptr,
  cm_num_s_type                      *dialed_digits
)
{
  estk_result_enum_type   estk_result = ESTK_SUCCESS;
  cm_reg_ss_params_s_type reg_params;
  sys_modem_as_id_e_type  as_id       = SYS_MODEM_AS_ID_1;

  UIM_MSG_HIGH_2("estk_ss_register: ss_string_ptr=0x%x, estk_sups_info_ptr=0x%x",
                 ss_string_ptr, estk_sups_info_ptr);

  if ((ss_string_ptr == NULL) || (estk_sups_info_ptr == NULL))
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  memset(&reg_params, 0x00, sizeof(reg_params));
  reg_params.ss_code = int32touint8((int32)ss_string_ptr->sc);
  reg_params.ss_ref  = estk_sups_info_ptr->req_id;

  /* SIB only ever denotes a Basic Service */
  if (ssparser_parse_basic_service(ss_string_ptr->sib_ptr,
                                   ss_string_ptr->sib_length,
                                   &reg_params.basic_service.code_type,
                                   &reg_params.basic_service.bs_code))
  {
    reg_params.basic_service.present = TRUE;
  }

  /* SIA may take a number of types, depending on the service */
  switch (ss_string_ptr->sia_type)
  {
    case SSPARSER_SI_TELNUM:
      if (ssparser_parse_bcd_telnum(ss_string_ptr->sia_ptr,
                                    ss_string_ptr->sia_length,
                                    ss_string_ptr->ton,
                                    ss_string_ptr->npi,
                                    reg_params.fwd_to_num.data,
                                    sizeof(reg_params.fwd_to_num.data),
                                    &reg_params.fwd_to_num.length))
      {
        reg_params.fwd_to_num.present = TRUE;
      }
      break;

    case SSPARSER_SI_PWD:
      /* Not handled yet */
      UIM_MSG_HIGH_0("SSPARSER_SI_PWD Not handled");
      break;

    default:
      /* Unknown */
      UIM_MSG_ERR_1("Unknown SIA type 0x%x", ss_string_ptr->sia_type);
      return ESTK_ERROR;
  }

  /* SIC only ever denotes a call forwarding timer interval */
  if (ssparser_parse_uint8(ss_string_ptr->sic_ptr,
                           ss_string_ptr->sic_length,
                           &reg_params.nr_timer.timer_value))
  {
    reg_params.nr_timer.present = TRUE;
  }

  if (gstk_io_ctrl(
        GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
        &estk_shared_info.slot_owns_cm_cli,
        &as_id) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                  estk_shared_info.slot_owns_cm_cli);
    return ESTK_ERROR;
  }

  if (!cm_sups_cmd_register_cc_exten_per_subs(estk_cm_sups_cmd_cb,
                                              estk_sups_info_ptr,
                                              estk_shared_info.cm_client_id,
                                              &reg_params,
                                              dialed_digits,
                                              CM_ORIG_CLIENT_CATEG_CARD,
                                              FALSE,
                                              as_id))
  {
    UIM_MSG_ERR_0("cm_sups_cmd_register failed");
    estk_result = ESTK_ERROR;
  }

  return estk_result;

} /* estk_ss_register */

/*===========================================================================
FUNCTION: estk_ss_erase

DESCRIPTION:
  The function processes the SS Erase request and sends the request
  to CM

PARAMETERS:
  ss_string_ptr      : [Input] SS Erase string pointer
  estk_sups_info_ptr : [Input] ESTK sups info pointer
  dialed_digits      : [Input] Dialed digits

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_ss_erase(
  const ssparser_ss_string_info_type *ss_string_ptr,
  estk_sups_info_type                *estk_sups_info_ptr,
  cm_num_s_type                      *dialed_digits
)
{
  estk_result_enum_type     estk_result = ESTK_SUCCESS;
  cm_erase_ss_params_s_type erase_params;
  sys_modem_as_id_e_type    as_id       = SYS_MODEM_AS_ID_1;

  UIM_MSG_HIGH_2("estk_ss_erase: ss_string_ptr=0x%x, estk_sups_info_ptr=0x%x",
                 ss_string_ptr, estk_sups_info_ptr);

  if ((ss_string_ptr == NULL) || (estk_sups_info_ptr == NULL))
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  memset(&erase_params, 0x00, sizeof(erase_params));
  erase_params.ss_code = int32touint8((int32)ss_string_ptr->sc);
  erase_params.ss_ref  = estk_sups_info_ptr->req_id;

  /* SIB only ever denotes a Basic Service */
  if (ssparser_parse_basic_service(ss_string_ptr->sib_ptr,
                                   ss_string_ptr->sib_length,
                                   &erase_params.basic_service.code_type,
                                   &erase_params.basic_service.bs_code))
  {
    erase_params.basic_service.present = TRUE;
  }

  if (gstk_io_ctrl(
        GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
        &estk_shared_info.slot_owns_cm_cli,
        &as_id) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                  estk_shared_info.slot_owns_cm_cli);
    return ESTK_ERROR;
  }

  if (!cm_sups_cmd_erase_cc_exten_per_subs(estk_cm_sups_cmd_cb,
                                           estk_sups_info_ptr,
                                           estk_shared_info.cm_client_id,
                                           &erase_params,
                                           dialed_digits,
                                           CM_ORIG_CLIENT_CATEG_CARD,
                                           FALSE,
                                           as_id))
  {
    UIM_MSG_ERR_0("cm_sups_cmd_erase failed");
    estk_result = ESTK_ERROR;
  }

  return estk_result;

} /* estk_ss_erase */

/*===========================================================================
FUNCTION: estk_process_send_ss_req

DESCRIPTION:
  Process the Send SS Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] request pointer for SS in gstk format

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_ss_req (
   gstk_cmd_from_card_type *gstk_req_ptr
)
{
  estk_result_enum_type         result         = ESTK_SUCCESS;
  estk_sups_info_type          *priv_sups_ptr  = NULL;
  ssparser_ss_string_info_type  ss_string_info;
  cm_num_s_type                 dialed_digits;
  /* Number buf going to CM.    */
  uint8                        *digit_ptr     = NULL;
  uint32                        i             = 0;
  gstk_additional_info_ptr_type  additional_result      = {0x00};
  uint8                          additional_result_data = 0;
  boolean                        send_tr                = FALSE;

  ESTK_RETURN_ERROR_IF_NULL_PTR(gstk_req_ptr, ESTK_ERROR);
  
  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  if(estk_curr_inst_ptr->ussd_transaction)
  {
     additional_result_data = (uint8)GSTK_ME_CURRENTLY_BUSY_ON_USSD_TRANSACTION;
     send_tr = TRUE;
  }
  if(estk_curr_inst_ptr->ss_transaction)
  {
     additional_result_data = (uint8)GSTK_ME_CURRENTLY_BUSY_ON_SS_TRANSACTION;
     send_tr = TRUE;
  }
  if(send_tr)
  {
     additional_result.length = 1;
     additional_result.additional_info_ptr = &additional_result_data;
     (void)estk_send_terminal_response(
            gstk_req_ptr->hdr_cmd.cmd_detail_reference,
            GSTK_SEND_SS_CNF,
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
            &additional_result,
            NULL);

     return ESTK_SUCCESS;
  }

  UIM_MSG_HIGH_1("estk_process_send_ss_req: estk_shared_info.slot_owns_cm_cli=0x%x",
                 estk_shared_info.slot_owns_cm_cli);

  if (estk_shared_info.slot_owns_cm_cli != GSTK_SLOT_ID_MAX)
  {
    return ESTK_ERROR;
  }
  else
  {
    estk_shared_info.slot_owns_cm_cli = gstk_req_ptr->hdr_cmd.sim_slot_id;
  }

  if (estk_ss_init() != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Unable to initialise ESTK SS module");
    return ESTK_ERROR;
  }

  if (&ss_string_info != ssparser_parse(gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.address,
                                        gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.length,
                                        gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.TON,
                                        gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.NPI,
                                        &ss_string_info))
  {
    UIM_MSG_ERR_0("Unable to parse SS string");
    return ESTK_ERROR;
  }

  priv_sups_ptr = estk_sups_alloc_populate_priv_data(gstk_req_ptr->hdr_cmd.cmd_detail_reference,
                                                     ss_string_info.op);
  if (priv_sups_ptr == NULL)
  {
    UIM_MSG_ERR_0("Unable to allocate ESTK data for SS operation");
    return ESTK_ERROR;
  }

  /* Check Alpha availability */
  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == FALSE)
  {
    /* Even in case of null data object in ALPHA (i.e. length = '00' and no value part),
       GSTK make sures at least NULL char is present in alpha text and length includes
       that NULL character. So the min length is one and hence we need to check for
       length more than one */
    if(((gstk_req_ptr->cmd.send_ss_pro_cmd_req.alpha.length) > 1 &&
       (gstk_req_ptr->cmd.send_ss_pro_cmd_req.alpha.text != NULL )) ||
       (gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
           GSTK_SLOT_ID_MAX) == TRUE))
    {
      if (estk_send_alpha_to_display_client(gstk_req_ptr, TRUE) != ESTK_SUCCESS)
      {
        UIM_MSG_ERR_0("Alpha Display failed");
        /* priv data allocated is freed when a TR is sent or
           when a End Proactive Session is received */
        return ESTK_ERROR;
      }
    }
  }

  dialed_digits.digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
  /* Seperate the number from the dtmf digits and build the number */
  for (i = 0, digit_ptr = &dialed_digits.buf[0];
       (i < CM_MAX_NUMBER_CHARS) &&
       (i < gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.length);
       i++)
  {
    switch (gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.address[i])
    {
      /* Digits are treated as ASCII, */
      case 0x00: case 0x01: case 0x02: case 0x03: case 0x04:
      case 0x05: case 0x06: case 0x07: case 0x08: case 0x09:
        *digit_ptr++ = '0' + gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.address[i];
        break;

      /* 0x0a is the * key */
      case 0x0A:
        *digit_ptr++ = '*';
        break;

      /* 0x0b is the # key */
      case 0x0B:
        *digit_ptr++ = '#';
        break;

      default:
        UIM_MSG_ERR_2("Unexpected value 0x%x in dial string at offset 0x%x.",
                      gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.address[i],
                      i);
        break;
    } /* End of switch */
  }/* End of for */

  dialed_digits.len = gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.length;

  if(gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.length > CM_MAX_NUMBER_CHARS)
  {
     dialed_digits.len = CM_MAX_NUMBER_CHARS;
  }

  dialed_digits.number_type = gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.TON;
  dialed_digits.number_plan = gstk_req_ptr->cmd.send_ss_pro_cmd_req.ss_string.NPI;

  switch (ss_string_info.op)
  {
    case SSPARSER_OP_ACTIVATE:
      result = estk_ss_activate(&ss_string_info, priv_sups_ptr, &dialed_digits);
      break;

    case SSPARSER_OP_DEACTIVATE:
      result = estk_ss_deactivate(&ss_string_info, priv_sups_ptr, &dialed_digits);
      break;

    case SSPARSER_OP_INTERROGATE:
      result = estk_ss_interrogate(&ss_string_info, priv_sups_ptr, &dialed_digits);
      break;

    case SSPARSER_OP_REGISTER:
      result = estk_ss_register(&ss_string_info, priv_sups_ptr, &dialed_digits);
      break;

    case SSPARSER_OP_ERASE:
      result = estk_ss_erase(&ss_string_info, priv_sups_ptr, &dialed_digits);
      break;

    default:
      UIM_MSG_ERR_0("Invalid SS operation");
      return ESTK_ERROR;
  }
  return result;
} /* estk_process_send_ss_req */

/*===========================================================================

FUNCTION: estk_sups_cleanup

DESCRIPTION:
  Clear the Sups related private data

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_sups_cleanup( void )
{
  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);
  /* Free sups info ptr private ESTK data */
  if(estk_shared_info.sups_info_ptr)
  {
    UIM_MSG_HIGH_0("estk_sups_cleanup, estk_shared_info.sups_info_ptr != NULL ");
    gstk_free((void *)estk_shared_info.sups_info_ptr);
    estk_shared_info.sups_info_ptr = NULL;
  }

  UIM_MSG_HIGH_1("estk_sups_cleanup, current command id 0x%x",
                                               estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
  switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
    case GSTK_SEND_USSD_REQ:
      if(!estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.has_cc_type_modification)
      {
        if(estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.text != NULL)
        {
          UIM_MSG_HIGH_0("estk_sups_cleanup, send_ussd_extra_param.data.text != NULL");
          gstk_free((void*)estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.text);
          estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.data.text = NULL;
        }
      }
      break;
    case GSTK_SEND_SS_REQ:
      if(estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param.text_string2.text != NULL)
      {
        gstk_free(estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param.text_string2.text);
        estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param.text_string2.text = NULL;
      }
      break;
    default:
      break;
  }

  estk_curr_inst_ptr->result = GSTK_GENERAL_RESULT_MAX;

  memset (&estk_curr_inst_ptr->extra_param, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));

  UIM_MSG_HIGH_0("Clean up of saved TR");
  if(estk_curr_inst_ptr->additional_result.additional_info_ptr != NULL)
  {
    gstk_free(estk_curr_inst_ptr->additional_result.additional_info_ptr);
    estk_curr_inst_ptr->additional_result.additional_info_ptr = NULL;
  }
  memset(&estk_curr_inst_ptr->additional_result, 0x00, sizeof(gstk_additional_info_ptr_type));
  memset(&estk_curr_inst_ptr->extra_param, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));
} /* estk_sups_cleanup */

/*===========================================================================

FUNCTION: estk_process_cm_sups_event_cmd

DESCRIPTION :
  This function will be called by ESTK to process CM events
  related to supplimentary services

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing of call event is successful
    ESTK_ERROR   : If processing results in error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_sups_event_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  gstk_slot_id_enum_type        slot          = GSTK_SLOT_ID_MAX;
  const cm_sups_info_s_type    *sups_info_ptr = NULL;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);
  sups_info_ptr = &(cmd_ptr->cmd_data.sups_info.evt_info);

  if(cmd_ptr == NULL || estk_shared_info.sups_info_ptr == NULL ||
  	      sups_info_ptr == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }
  slot = cmd_ptr->hdr.sim_slot_id;
  if (!ESTK_IS_VALID_SLOT_ID(slot))
  {
    return ESTK_ERROR;
  }

  switch(cmd_ptr->cmd_data.sups_info.evt_id)
  {
    /*--------- Supps Completed Event ---------*/
    case CM_SUPS_EVENT_REGISTER_CONF:
    case CM_SUPS_EVENT_ACTIVATE_CONF:
    case CM_SUPS_EVENT_DEACTIVATE_CONF:
    case CM_SUPS_EVENT_ERASE_CONF:
    case CM_SUPS_EVENT_INTERROGATE_CONF:
      estk_curr_inst_ptr->ss_transaction = FALSE;
      /* Check for reference number to make sure it is the matching with the
         one originated from ESTK only */
      if (sups_info_ptr->ss_ref == estk_shared_info.sups_info_ptr->req_id)
      {
        if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_USSD_REQ)
        {
          memset(&estk_curr_inst_ptr->extra_param, 0,sizeof(estk_curr_inst_ptr->extra_param));
          estk_curr_inst_ptr->extra_param.present = TRUE;
          estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.has_cc_type_modification = TRUE;
          estk_curr_inst_ptr->ussd_transaction = FALSE;
          estk_curr_inst_ptr->extra_param.rsp_type = GSTK_SEND_USSD_CNF;
        }
        switch(cmd_ptr->cmd_data.sups_info.evt_id)
        {
          case CM_SUPS_EVENT_REGISTER_CONF:
            estk_shared_info.sups_info_ptr->ss_operation = ESTK_REGISTER_SS;
            break;
          case CM_SUPS_EVENT_ACTIVATE_CONF:
            estk_shared_info.sups_info_ptr->ss_operation = ESTK_ACTIVATE_SS;
            break;
          case CM_SUPS_EVENT_DEACTIVATE_CONF:
            estk_shared_info.sups_info_ptr->ss_operation = ESTK_DEACTIVATE_SS;
            break;
          case CM_SUPS_EVENT_ERASE_CONF:
            estk_shared_info.sups_info_ptr->ss_operation = ESTK_ERASE_SS;
            break;
          case CM_SUPS_EVENT_INTERROGATE_CONF:
            estk_shared_info.sups_info_ptr->ss_operation = ESTK_INTERROGATE_SS;
            break;
          default:
            break;
        }
        estk_sups_process_cm_ss_evt(sups_info_ptr);
      }
      break;

    case CM_SUPS_EVENT_PROCESS_USS_CONF:
      estk_curr_inst_ptr->ussd_transaction = FALSE;
      /* Check for reference number to make sure it is the matching with the
         one originated from ESTK only */
      if (sups_info_ptr->ss_ref == estk_shared_info.sups_info_ptr->req_id)
      {
        if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_SS_REQ)
        {
          memset(&estk_curr_inst_ptr->extra_param, 0,sizeof(estk_curr_inst_ptr->extra_param));
          estk_curr_inst_ptr->extra_param.present = TRUE;
          estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param.has_cc_type_modification = TRUE;
          estk_curr_inst_ptr->ss_transaction = FALSE;
          estk_curr_inst_ptr->extra_param.rsp_type = GSTK_SEND_SS_CNF;
          estk_shared_info.sups_info_ptr->ss_operation = ESTK_PROCESS_USSD_DATA;
        }
        estk_sups_process_cm_ussd_evt(sups_info_ptr);
      }
      break;

    /*--------- Start Event - Not Applicable to Toolkit ---------*/
    case CM_SUPS_EVENT_REGISTER:
    case CM_SUPS_EVENT_ACTIVATE:
    case CM_SUPS_EVENT_DEACTIVATE:
    case CM_SUPS_EVENT_ERASE:
    case CM_SUPS_EVENT_INTERROGATE:
    case CM_SUPS_EVENT_REG_PASSWORD:
      estk_curr_inst_ptr->ss_transaction = TRUE;
      break;
    case CM_SUPS_EVENT_PROCESS_USS:
      estk_curr_inst_ptr->ussd_transaction = TRUE;
      break;

    /*--------- Network Initiated Event - Not Applicable to Toolkit  ---------*/
    case CM_SUPS_EVENT_GET_PASSWORD_IND:
    case CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_IND:
    case CM_SUPS_EVENT_LCS_MOLR:
      estk_curr_inst_ptr->ss_transaction = TRUE;
      break;

    case CM_SUPS_EVENT_USS_IND:
      estk_curr_inst_ptr->ussd_transaction = TRUE;
      break;

    case CM_SUPS_EVENT_RELEASE_USS_IND:
      estk_curr_inst_ptr->ussd_transaction = FALSE;
      if (sups_info_ptr->ss_ref == estk_shared_info.sups_info_ptr->req_id)
      {
        gstk_cmd_enum_type cnf_typ = GSTK_SEND_USSD_CNF;
        gstk_general_result_enum_type result = GSTK_USSD_SS_TERMINATED_BY_THE_USER;
        gstk_terminal_rsp_extra_param_type *extra_param_ptr = NULL;
        if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_SS_REQ)
        {
          memset(&estk_curr_inst_ptr->extra_param, 0,sizeof(estk_curr_inst_ptr->extra_param));
          estk_curr_inst_ptr->extra_param.present = TRUE;
          estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.has_cc_type_modification = TRUE;
          estk_curr_inst_ptr->ss_transaction = FALSE;
          estk_curr_inst_ptr->extra_param.rsp_type = GSTK_SEND_SS_CNF;
          estk_shared_info.sups_info_ptr->ss_operation = ESTK_PROCESS_USSD_DATA;
        }
        if(estk_curr_inst_ptr->extra_param.present)
        {
          extra_param_ptr = &(estk_curr_inst_ptr->extra_param);
          switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
          {
            case GSTK_SEND_SS_REQ:
              cnf_typ = GSTK_SEND_SS_CNF;
              extra_param_ptr->extra_param.send_ss_extra_param.command_result2 = result;
              result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
              break;
            case GSTK_SETUP_CALL_REQ:
              cnf_typ = GSTK_SETUP_CALL_CNF;
              extra_param_ptr->extra_param.setup_call_extra_param.command_result2 = result;
              result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
              break;
            case GSTK_SEND_USSD_REQ:
              cnf_typ = GSTK_SEND_USSD_CNF;
              break;
            default:
              break;
          }
          estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
        }        
        if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
        {
          UIM_MSG_HIGH_0("Saving USSD TR");
          estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
          estk_curr_inst_ptr->result = GSTK_USSD_SS_TERMINATED_BY_THE_USER;
        }
        else
        {
          UIM_MSG_HIGH_0 ("User terminated USSD transaction");
          (void)estk_send_terminal_response(
                  estk_shared_info.sups_info_ptr->cmd_ref_id,     /* command_details_ref */
                  cnf_typ,                                        /* command response */
                  result,                                         /* general_result */
                  NULL,                                           /* additional result */
                  extra_param_ptr);                               /* extra param */
        }
      }
      break;

    /*--------- Response to Network Event - Not Applicable to Toolkit ---------*/
    case CM_SUPS_EVENT_USS_NOTIFY_RES:
    case CM_SUPS_EVENT_USS_RES:
      estk_curr_inst_ptr->ussd_transaction = FALSE;
      break;

    case CM_SUPS_EVENT_REG_PASSWORD_CONF:
    case CM_SUPS_EVENT_GET_PASSWORD_RES:
    case CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_RES:
    case CM_SUPS_EVENT_LCS_MOLR_COMPLETED:
    case CM_SUPS_EVENT_LCS_MOLR_CONF:
      estk_curr_inst_ptr->ss_transaction = FALSE;
      break;

    case CM_SUPS_EVENT_SS_MOD_TO_ORIG:
      if (sups_info_ptr->ss_ref == estk_shared_info.sups_info_ptr->req_id)
      {
        memset(&estk_curr_inst_ptr->extra_param, 0,sizeof(estk_curr_inst_ptr->extra_param));
        estk_curr_inst_ptr->extra_param.present = TRUE;
        estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;

        switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
        {
          case GSTK_SEND_USSD_REQ:
            estk_curr_inst_ptr->extra_param.rsp_type = GSTK_SEND_USSD_CNF;
            estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.has_cc_type_modification = TRUE;
            break;
          case GSTK_SEND_SS_REQ:
            estk_curr_inst_ptr->extra_param.rsp_type = GSTK_SEND_SS_CNF;
            estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param.has_cc_type_modification = TRUE;
            break;
          default:
            estk_curr_inst_ptr->extra_param.present = FALSE;
            break;
        }
        if(sups_info_ptr->mod_ss_params.call_control_result == CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE)
        {
          estk_shared_info.call_info.call_id = sups_info_ptr->mod_ss_params.call_id;
        }
        else
        {
          if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_USSD_REQ)
          {
            estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.command_result2 =
                                                          GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;

          }
          else
          {
            estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param.command_result2 =
                                                           GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          }
          (void)estk_send_terminal_response(
                  estk_shared_info.sups_info_ptr->cmd_ref_id,                    /* command_details_ref */
                  estk_curr_inst_ptr->extra_param.rsp_type,                      /* command response */
                  GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM,   /* general_result */
                  NULL,                                                          /* additional result */
                  &(estk_curr_inst_ptr->extra_param));                           /* extra param */
        }
        gstk_free((void *)estk_shared_info.sups_info_ptr);
        estk_shared_info.sups_info_ptr = NULL;
      }
      break;

    default:
      break;
    } /* end switch */
    return ESTK_SUCCESS;
}

#endif /* FEATURE_ESTK */

