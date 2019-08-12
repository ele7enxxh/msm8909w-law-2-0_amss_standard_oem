/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I M   R S P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UIM response processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2005, 2008 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_gen_rsp.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   ar      Review of macros used by MMGSDI
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/12/16   tkl     F3 log prints cleanup
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
01/29/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
06/30/14   vv      Remove Perso code
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
10/16/13   df      Removing unused functions
05/16/13   vdc     Replace memcpy with safer version memscpy
05/08/13   at      Supporting 62XX & 63XX warnings in APDU transactions
05/06/13   spo     Memory optimization for get response Cache
12/18/12   nmb     Relay SW1=63 to client with additional data for case 4 ins
10/11/12   spo     Removed unnecessary F3 messages
09/28/12   abg     Updated ERR to MSG_ERROR
08/24/12   abg     Use UIM response status while building the
                   Card Power Down CNF
05/31/12   bcho    Remove incorrect clean-up of app and pin data while handling
                   Card Power-down response
03/29/12   shr     Added support for Send APDU extension API
02/18/12   av      Cleaned up remaining ALWAYS OFF features
12/21/11   kk      Legacy GSDI Removal updates
10/24/11   shr     Send correct GET RESPONSE data from the MMGSDI cache
05/17/11   shr     Updates for Thread safety
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
10/29/10   tkl     Returned original mmgsdi_err_status in generic rsp
03/02/10   kk      Fixed get all available apps command handling
02/01/10   jk      EPRL Support
12/18/09   nb      Moved Dual Slot Changes
09/26/09   kp      ZI memory reduction changes
08/13/09   js      Clean up app pin info for mmgsdi card pup/pdown request
08/05/09   js      Clean up app pin info for mmgsdi card pup/pdown request
04/29/09   js      Included header mmgsdi_gen_rsp.h
02/24/09   nb      Perso Command Support in MMGSDI
12/17/08   nb      Common Modem Interface
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
07/17/08   kk      Added support for PKCS 15
06/04/08   tml     Added sw to cnf rsp header and message cleanup
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO Supports.
08/07/06   tml     Added ACL support
05/23/06   tml     BDN support
04/18/06   nk      Featurized and converted macro calls to function calls
12/14/05   tml     MMGSDI Cleanup
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
08/26/05   pv      Carry over the result from uim to the confirmation
08/25/05   pv      Initial Version

===========================================================================*/


#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "mmgsdi.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdi_sap_rsp.h"
#include "mmgsdi_uicc_rsp.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "rex.h"
#include "uim.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_gen_rsp.h"

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_GEN_RSP

   DESCRIPTION:
     This function will process the any generic response
     Note, for BDN and ACL, this function should only come called if it
     is a failed operation, if successful operation, should be handled by
     the get_file_attribute_response and write_response accordingly

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_gen_rsp(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_request_header_type         req_header;
  int32                              index            = 0;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;
  mmgsdi_sw_status_type              status_word;

  UIM_MSG_HIGH_0("MMGSDI PROCESS GEN RSP");

  /* status word is not applicable in gen response processing */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (uim_rsp_ptr->is_uim_success)
  {

    report_status = uim_rsp_ptr->mmgsdi_error_status;
  }
  else
  {
     report_status = MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status, &req_header,
                                             extra_param_ptr, NULL,
                                             FALSE, status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp */


/*=============================================================================
  FUNCTION: MMGSDI_GEN_RSP_GET_ATR

  DESCRIPTION:
    The response to the Get ATR command.  The ATR is internally stored in
    MMGSDI and the response is built internally in MMGSDI

  PARAMETERS:
    uim_rsp_ptr:    UIM response data.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
==============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_rsp_get_atr (
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
)
{
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type*                      uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;
  mmgsdi_sw_status_type              status_word;

  /* status word is not applicable to ATR */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  /*
   * Check input pointers.
   */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(uim_rsp_ptr->rsp_data_index, 0, MMGSDI_MAX_CLIENT_INFO-1);
  /*
   * Get the request header.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info( uim_rsp_ptr->rsp_data_index,
                                                             &req_header,
                                                             &uim_cmd_ptr,
                                                             &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Get the MMGSDI command processing status */
  report_status = uim_rsp_ptr->mmgsdi_error_status;

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE, /* sw not required for
                                                       atr */
                                             status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp_get_atr */


/*===========================================================================
FUNCTION MMGSDI_GEN_GET_RSP_GET_CACHE_INDEX

DESCRIPTION
  Simple function used to search the APDU GET RESPONSE Cache Table for
  the next Available Index.

DEPENDENCIES
  None

RETURN VALUE
  int32:   -1  :  Table is full.
          0-MAX:  Usable index


SIDE EFFECTS
  None

===========================================================================*/
static int32 mmgsdi_gen_get_rsp_get_next_cache_index(
  void
)
{
  int32 index = 0x00;

  while (index < MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES)
  {
    if (mmgsdi_apdu_get_rsp_table_ptr[index] == NULL )
    {
      /* Return the index to be used to store the next
      ** APDU Get Response Data
      */
      return index;
    }
    index++;
  }

  /* Indicate to the client the table is full */
  return MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES;
} /* mmgsdi_gen_get_rsp_get_next_cache_index */


/*===========================================================================
FUNCTION MMGSDI_GEN_RSP_TABLE_ADD

DESCRIPTION
  Function will add the APDU Response Data cached from the currently
  applicable APDU Exchange with the Card inserted.

DEPENDENCIES
  The APDU_PTR provided must contain a valid pointer and all memory must
  be appropriately allocated to match the data maintained in this table.

RETURN VALUE
  mmgsdi_return_enum_type:
  MMGSDI_SUCCESS:           Get Response Data populated in the apdu_ptr
                            provided.
  MMGSDI_NOT_FOUND:         Get Response Data for the CLIENT ID, SLOT ID,
                            CHANNEL ID is no longer present
  MMGSDI_INCORRECT_PARAMS:  Parameter check failure. NULL DATA PTR provided
                            or Get Response Value exceeds the maximum.
  MMGSDI_ERROR:             Could not find an index location to save the
                            GET APDU Transaction Data.

SIDE EFFECTS
  A Successful add will allocate memory from the head.  This memory
  allocation is freed when the next applicable APDU Exchange occurs.

===========================================================================*/
static mmgsdi_return_enum_type  mmgsdi_gen_get_rsp_table_add(
  mmgsdi_client_id_type      client_id,
  mmgsdi_slot_id_enum_type   slot_id,
  uint8                      channel_id,
  const mmgsdi_data_type *   apdu_ptr
)
{
  int32                       index           = 0x00;

  MMGSDIUTIL_RETURN_IF_NULL_2(apdu_ptr, apdu_ptr->data_ptr);

  if(apdu_ptr->data_len <= MMGSDI_GEN_SW_LEN)
  {
    UIM_MSG_ERR_1("Incorrect APDU length: 0x%x for caching data",
                  apdu_ptr->data_len);

    return MMGSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Retrieve an Index from the APDU Get Response Cache
     ----------------------------------------------------------------------- */
  index = mmgsdi_gen_get_rsp_get_next_cache_index();

  if ( index >= MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("GET APDU RSP Cache Table Full" );
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Allocate memory to store the data into and intialize it to 0x00 if
     successful.
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_apdu_get_rsp_table_ptr[index],
    sizeof(mmgsdi_apdu_data_get_rsp_cache_type));
  if (mmgsdi_apdu_get_rsp_table_ptr[index] == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_apdu_get_rsp_table_ptr[index]->apdu.data_ptr,
    apdu_ptr->data_len);

  if(mmgsdi_apdu_get_rsp_table_ptr[index]->apdu.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_apdu_get_rsp_table_ptr[index]);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* --------------------------------------------------------------------------
     Store the following into the table
      - Client ID
      - Slot   ID
      - Channel ID for this particular APDU Get Response Data
      - Implicit SW1 value
      - APDU Get Response Data
      - APDU Get Response Data Len
     ------------------------------------------------------------------------*/
  mmgsdi_apdu_get_rsp_table_ptr[index]->client_id     = client_id;
  mmgsdi_apdu_get_rsp_table_ptr[index]->slot_id       = slot_id;
  mmgsdi_apdu_get_rsp_table_ptr[index]->channel_id    = channel_id;
  mmgsdi_apdu_get_rsp_table_ptr[index]->implicit_sw1  =
    apdu_ptr->data_ptr[apdu_ptr->data_len -2];
  mmgsdi_apdu_get_rsp_table_ptr[index]->apdu.data_len = apdu_ptr->data_len;

  mmgsdi_memscpy(mmgsdi_apdu_get_rsp_table_ptr[index]->apdu.data_ptr,
          int32touint32(apdu_ptr->data_len),
          apdu_ptr->data_ptr,
         (size_t)(apdu_ptr->data_len - 2));  /* Only copy the data + status word and leave the
                                      implicit status word as 0x00 */

  UIM_MSG_HIGH_1("APDU Get Response Data Stored. Index 0x%x", index);
  return MMGSDI_SUCCESS;
} /* mmgsdi_gen_get_rsp_table_add */


/*===========================================================================
FUNCTION MMGSDI_GEN_RSP_TABLE_DELETE

DESCRIPTION
  Function shall delete the cached APDU Response Data corresponding to the
  requested Client ID, Slot ID, Channel ID.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
  MMGSDI_SUCCESS:           Get Response Data deleted from the cache
  MMGSDI_NOT_FOUND:         No Get Response Data cached for the
                            requested Client ID, Slot ID
  MMGSDI_INCORRECT_PARAMS:  Parameter check failure

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type  mmgsdi_gen_get_rsp_table_delete(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot_id,
  uint8                    channel_id
)
{
  uint8 tbl_index = 0;

  UIM_MSG_HIGH_2("Clear any APDU rsp data stored in cache for Client id_low = 0x%x, id_high = 0x%x",
                 (client_id & 0xFFFFFFFF), (client_id>>32 & 0xFFFFFFFF));

  UIM_MSG_HIGH_2("slot: 0x%x, channel: 0x%x", slot_id, channel_id);

  /* Search the APDU Response Cache data for data stored corresponding to the
     requested Client ID, Slot ID, Channel ID  */
  for (tbl_index = 0;
       tbl_index < MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES;
       tbl_index++)
  {
    if(mmgsdi_apdu_get_rsp_table_ptr[tbl_index] == NULL)
    {
      continue;
    }

    if (mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->client_id != client_id)
    {
      /* Client IDs did not match */
      continue;
    }

    if (mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->slot_id != slot_id)
    {
      /* Not the Slot ID for which APDU Data is requested.
         Client may be sending APDUs over various slots.
         Continue Searching */
      continue;
    }

    if (mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->channel_id!= channel_id)
    {
      /* Not the Channel ID for which APDU Data is requested.
         Client may be sending APDUs over various channels.
         Continue Searching */
      continue;
    }

    /* Match found. Break out of the for loop. */
    break;
  }

  if (tbl_index >= MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES)
  {
    /* No match found */
    UIM_MSG_HIGH_0("No APDU rsp data stored in cache for client");
    return MMGSDI_NOT_FOUND;
  }

  /* Clear the cache table entry */
  UIM_MSG_HIGH_1("Clear APDU rsp data stored at cache table index: 0x%x",
                 tbl_index);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_apdu_get_rsp_table_ptr[tbl_index]);

  return MMGSDI_SUCCESS;
} /* mmgsdi_gen_get_rsp_table_delete */


/*===========================================================================
FUNCTION MMGSDI_GEN_RSP_SEND_APDU_RSP

DESCRIPTION
  Function to handle the send APDU response received from UIM.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_send_apdu_rsp(
  const mmgsdi_uim_report_rsp_type*     uim_rsp_ptr
)
{
  mmgsdi_request_header_type         req_header;
  int32                              index            = 0;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type*                      uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;
  uint8                              sw1              = 0x00;
  uint8                              sw2              = 0x00;
  mmgsdi_data_type                   get_rsp_apdu;
  mmgsdi_data_type                   get_rsp_sw_apdu;
  void                             * cnf_add_data_ptr = NULL;
  mmgsdi_sw_status_type              status_word      = {0};
  uint8                              channel_id       = 0;

  /*lint -esym(613,mmgsdi_cnf_ptr)*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  memset(&get_rsp_sw_apdu, 0x00, sizeof(mmgsdi_data_type));

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);
  /*
   * Get the request header.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
      uim_rsp_ptr->rsp_data_index,
      &req_header,
      &uim_cmd_ptr,
      &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Retrieve the Channel ID from the CLA byte
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_channel_id_from_cla(extra_param_ptr->send_apdu_data.cla,
                                                      req_header.slot_id,
                                                      &channel_id);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Could not retrieve Channel ID from CLA");
    return mmgsdi_status;
  }

  /* Delete any previously stored Get Response data for the client on the
     requested slot, channel if the current command is not a Get Response.
     This ensures that valid Get Response data is returned only if it is
     executed by the client on the channel immediately after the command
     it is related to (ETSI TS 102 221, Section 12.1.1.1) */
  if(extra_param_ptr->send_apdu_data.ins != GET_RESPONSE)
  {
    (void)mmgsdi_gen_get_rsp_table_delete(req_header.client_id,
                                          req_header.slot_id,
                                          channel_id);
  }

  cnf_add_data_ptr = (void*)&uim_rsp_ptr->rsp_data;

  if (uim_rsp_ptr->is_uim_success)
  {
    /* --------------------------------------------------------------------------
      Peek into the APDU Data for an Implicit Status Word
      ----------------------------------------------------------------------- */
    sw1 = uim_rsp_ptr->rsp_data.data_ptr[uim_rsp_ptr->rsp_data.data_len - 2];
    sw2 = uim_rsp_ptr->rsp_data.data_ptr[uim_rsp_ptr->rsp_data.data_len - 1];

    /* status word of the command */
    status_word.sw1 = uim_rsp_ptr->status_word.sw1;
    status_word.sw2 = uim_rsp_ptr->status_word.sw2;

    if ((extra_param_ptr->send_apdu_data.ins != GET_RESPONSE) &&
        (sw1 == GSDI_SIM_SW1_PROCEDURE_BYTE_61 ||
         sw1 == GSDI_SW1_WARNINGS_PART_1 ||
         sw1 == GSDI_SW1_WARNINGS_PART_2 ||
         /* For an ICC Card 9F is valid only for an ICC card on channel 0 */
         ((sw1 == GSDI_SIM_SW1_NORM_END_W_SW2_LENGTH) &&
          (extra_param_ptr->send_apdu_data.cla == MMGSDI_CLA_VALUE_ICC_CH_0) &&
          (mmgsdi_util_determine_protocol(req_header.slot_id) == MMGSDI_ICC))))
    {
      /* If Implicit status words are present and have been requested by the
         client, save the corresponding Get Response data */
      if(extra_param_ptr->send_apdu_data.apdu_option ==
          MMGSDI_SEND_APDU_RETURN_IMPLICIT_STATUS_WORDS)
      {
        /* ------------------------------------------------------------------------
          Save the Data to the Cache
          --------------------------------------------------------------------- */
        get_rsp_apdu.data_len = (int32)uim_rsp_ptr->rsp_data.data_len;
        get_rsp_apdu.data_ptr = (uint8 *)uim_rsp_ptr->rsp_data.data_ptr;

        /* If total Get Response data length exceeds 0xFF, then return
           SW2 as 0 to indicate the same.
           In the case of 0x62XX & 0x63XX we want to preserve the SW2.  In this
           case sw2 does not represent the length and shouldn't be changed */
        if((uim_rsp_ptr->rsp_data.data_len >
            MMGSDI_GEN_MAX_GET_RESPONSE_REQ_LEN + MMGSDI_GEN_SW_LEN) &&
            (sw1 != GSDI_SW1_WARNINGS_PART_1) &&
            (sw1 != GSDI_SW1_WARNINGS_PART_2))
        {
          sw2 = 0;
        }

        mmgsdi_status = mmgsdi_gen_get_rsp_table_add(
          req_header.client_id,
          req_header.slot_id,
          channel_id,
          &get_rsp_apdu);

        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          get_rsp_sw_apdu.data_len = MMGSDI_GEN_SW_LEN;

          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(get_rsp_sw_apdu.data_ptr,
                                             get_rsp_sw_apdu.data_len);

          if(get_rsp_sw_apdu.data_ptr == NULL)
          {
            return MMGSDI_ERROR;
          }
          get_rsp_sw_apdu.data_ptr[0] = sw1;
          get_rsp_sw_apdu.data_ptr[1] = sw2;
          get_rsp_sw_apdu.data_ptr[2] = sw1;
          get_rsp_sw_apdu.data_ptr[3] = sw2;

          /* update with the implicit sw */
          status_word.sw1 = sw1;
          status_word.sw2 = sw2;

          cnf_add_data_ptr = (void*)&get_rsp_sw_apdu;
        }
      }
      /* Reset the bytes in the data buffer corresponding to the implicit
         status words, as this has not been requested */
      else
      {
        uim_rsp_ptr->rsp_data.data_ptr[uim_rsp_ptr->rsp_data.data_len - 2] = 0x0;
        uim_rsp_ptr->rsp_data.data_ptr[uim_rsp_ptr->rsp_data.data_len - 1] = 0x0;
      }
    }
  }
  else
  {
    report_status = MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             cnf_add_data_ptr,
                                             uim_rsp_ptr->cmd_transacted,
                                             status_word);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(get_rsp_sw_apdu.data_ptr);
  return mmgsdi_status;
  /*lint +esym(613,mmgsdi_cnf_ptr)*/
} /* mmgsdi_gen_rsp_send_apdu_rsp */ /*lint !e715 */


/* ============================================================================
FUNCTION:      MMGSDI_GEN_RSP_SESSION_GET_INFO

DESCRIPTION:
  This function will build get the session info and prepare the cnf

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS:     The cnf was successfully queued to cnf Queue
    MMGSDI_ERROR:       Failed in queuing cnf to cnf Queue

SIDE EFFECTS:
  NONE
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_session_get_info (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                       * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr = NULL;
  mmgsdi_sw_status_type                status_word;

  memset(&status_word, 0x00, sizeof(mmgsdi_sw_status_type));
  memset(&req_header, 0x00, sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info( index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (extra_param_ptr == NULL))
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE,
                                             status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp_session_get_info*/


/* ============================================================================
FUNCTION:      MMGSDI_GEN_RSP_SESSION_READ_PRL

DESCRIPTION:
  This function will build the read prl rsp and prepare the cnf

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS:     The cnf was successfully queued to cnf Queue
    MMGSDI_ERROR:       Failed in queuing cnf to cnf Queue

SIDE EFFECTS:
  NONE
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_session_read_prl (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                       * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr = NULL;
  mmgsdi_sw_status_type                status_word;

  memset(&status_word, 0x00, sizeof(mmgsdi_sw_status_type));
  memset(&req_header, 0x00, sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info( index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (extra_param_ptr == NULL))
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE,
                                             status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp_session_read_prl */


/* ============================================================================
FUNCTION:      MMGSDI_GEN_RSP_SESSION_PROV_APP_INIT_COMPLETE

DESCRIPTION:
  This function will build cnf for session prov app init complete command

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS:     The cnf was successfully queued to cnf Queue
    MMGSDI_ERROR:       Failed in queuing cnf to cnf Queue

SIDE EFFECTS:
  NONE
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_session_prov_app_init_complete (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                       * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr = NULL;
  mmgsdi_sw_status_type                status_word;

  memset(&status_word, 0x00, sizeof(mmgsdi_sw_status_type));
  memset(&req_header, 0x00, sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info( index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE,
                                             status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp_session_prov_app_init_complete*/


/* ============================================================================
FUNCTION:      MMGSDI_GEN_RSP_AVAILABLE_APPS_REQ

DESCRIPTION:
  This function will build cnf for get available apps command

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS:     The cnf was successfully queued to cnf Queue
    MMGSDI_ERROR:       Failed in queuing cnf to cnf Queue

SIDE EFFECTS:
  NONE
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_available_apps_req (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                       * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr = NULL;
  mmgsdi_sw_status_type                status_word;

  memset(&status_word, 0x00, sizeof(mmgsdi_sw_status_type));
  memset(&req_header, 0x00, sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info( index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE,
                                             status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp_available_apps_req */


/* ============================================================================
FUNCTION:      MMGSDI_GEN_RSP_GET_CPHS_INFO

DESCRIPTION:
  This function will build cnf for get cphs info command

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS:     The cnf was successfully queued to cnf Queue
    MMGSDI_ERROR:       Failed in queuing cnf to cnf Queue

SIDE EFFECTS:
  NONE
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_get_cphs_info (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                       * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr = NULL;
  mmgsdi_sw_status_type                status_word;

  memset(&status_word, 0x00, sizeof(mmgsdi_sw_status_type));
  memset(&req_header, 0x00, sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info( index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE,
                                             status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp_get_cphs_info */


/* ============================================================================
FUNCTION:      MMGSDI_GEN_RSP_GET_APP_CAPABILITIES

DESCRIPTION:
  This function will build cnf for get app capabilities command

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS: The cnf is posted to cnf queue
    MMGSDI_ERROR:   The cnf posting to cnf queue failed

SIDE EFFECTS:
  NONE
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_get_app_capabilities (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                       * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr = NULL;
  mmgsdi_sw_status_type                status_word;

  memset(&status_word, 0x00, sizeof(mmgsdi_sw_status_type));
  memset(&req_header, 0x00, sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info( index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE,
                                             status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp_get_app_capabilities */
