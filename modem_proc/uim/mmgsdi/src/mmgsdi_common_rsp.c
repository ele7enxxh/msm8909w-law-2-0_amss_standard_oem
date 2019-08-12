/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                M M G S D I   I C C / U I C C  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the response processing support for MMGSDI.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_common_rsp.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/16   bcho    F3 frequency reduction by conditional logic
06/07/16   ar      Removing string prints from APDU and AUTH
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/17/16   tkl     F3 log prints cleanup
05/17/16   ar      Fix read/write of cache files when cache is not_init
12/09/14   ar      Replace mmgsdi_u/icc_rsp_srv_available with common func
10/16/14   tl      Set response type before building cnf data for sync resp
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
05/12/14   bcho    NULL check added before dereferencing extra_param_ptr
12/05/13   tkl     SFI Support and decouple uim file enums
01/21/14   yt      Remove duplicate code
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/16/13   tl      Prevent initalizing the cache with partial files
10/02/13   yt      Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_nv.h"
#include "mmgsdicache.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_cnf.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_INCREASE

   DESCRIPTION:
     This function will build the response to INCREASE request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_rsp_increase should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_increase (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_increase_cnf_type          ** mmgsdi_cnf_pptr,
  boolean                              synch,
  mmgsdi_protocol_enum_type            protocol
)
{
  mmgsdi_request_header_type            req_header;
  int32                                 index                 = 0;
  mmgsdi_return_enum_type               mmgsdi_status         = MMGSDI_SUCCESS;
  mmgsdi_increase_cnf_type            * cnf_ptr               = NULL;
  uim_cmd_type                        * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr       = NULL;
  mmgsdi_return_enum_type               report_status         = MMGSDI_SUCCESS;
  boolean                               sw_present            = FALSE;
  mmgsdi_data_type                      inc_rsp_data;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_cnf_pptr);
  }

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  memset(&inc_rsp_data, 0x00, sizeof(mmgsdi_data_type));

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

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type
  ---------------------------------------------------------------------------*/
  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  if (report_status == MMGSDI_SUCCESS)
  {
    /* Increased file needs to be cached as the increased data is placed
       in record 1 of the cyclic file. Trim the response data to cache only
       the record contents.*/
    if(extra_param_ptr->increase_data.data.data_len < uim_rsp_ptr->rsp_data.data_len)
    {
      inc_rsp_data.data_len = extra_param_ptr->increase_data.data.data_len;
    }
    else
    {
      inc_rsp_data.data_len = uim_rsp_ptr->rsp_data.data_len;
    }

    if(uim_rsp_ptr->rsp_data.data_ptr &&
       inc_rsp_data.data_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(inc_rsp_data.data_ptr,
                                         inc_rsp_data.data_len);

      /* In case of failed memory allocation, clear cache to avoid stale data */
      if(inc_rsp_data.data_ptr == NULL)
      {
        if(extra_param_ptr->increase_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
        {
          (void)mmgsdi_cache_delete_item(req_header.session_id,
                                         1,
                                         &extra_param_ptr->increase_data.access.file.file_enum);
        }
        else
        {
          UIM_MSG_HIGH_1("failed to delete cache item with access method: %x",
                         extra_param_ptr->increase_data.access.access_method);
        }
      }
      else
      {
        mmgsdi_data_from_enum_type data_from = MMGSDI_DATA_FROM_CARD_INCOMPLETE;

        if(extra_param_ptr->increase_data.data.data_len == 0)
        {
          data_from = MMGSDI_DATA_FROM_CARD_COMPLETE;
        }

        mmgsdi_memscpy(inc_rsp_data.data_ptr,
                       inc_rsp_data.data_len,
                       uim_rsp_ptr->rsp_data.data_ptr,
                       inc_rsp_data.data_len);

        mmgsdi_status = mmgsdi_cache_write(req_header.session_id,
                                           &extra_param_ptr->increase_data.access,
                                           inc_rsp_data,
                                           0,
                                           data_from);

        MMGSDIUTIL_TMC_MEM_FREE(inc_rsp_data.data_ptr);
      }
    }
  }
  else
  {
    /* Unsuccessful processing of command */
    if (extra_param_ptr->increase_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      if(report_status == MMGSDI_NOT_FOUND)
      {
        /* Update cache to indicate that file was not found on card */
        (void)mmgsdi_cache_write(req_header.session_id,
                                 &extra_param_ptr->increase_data.access,
                                 uim_rsp_ptr->rsp_data,
                                 0,
                                 MMGSDI_DATA_NOT_FOUND);
      }
    }
  } /* Parsing UIM Status */

  if (report_status == MMGSDI_SUCCESS)
  {
    /* if read from cache, and the operation is successful,
       the cmd_transacted will be FALSE.  if the operation is not successful,
       the command will go to uim */
    sw_present = TRUE;
  }
  else
  {
    sw_present = uim_rsp_ptr->cmd_transacted;
  }

  if (!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status, &req_header, extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      sw_present,
      uim_rsp_ptr->status_word);
  }
  else
  {
    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*mmgsdi_cnf_pptr),
      sizeof(mmgsdi_increase_cnf_type));
    if ((*mmgsdi_cnf_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    cnf_ptr = *mmgsdi_cnf_pptr;

    mmgsdi_status = mmgsdi_cnf_build_increase_data(
      cnf_ptr,
      report_status,
      extra_param_ptr,
      (mmgsdi_data_type*)&uim_rsp_ptr->rsp_data);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Populate Header */
      /* Header population should be done AFTER data populate
        because any pointer length will be assigned in the
        individual confirmation population and used in the
        populate_cnf_rsp_header for assigning the response
        length */
      mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                          &cnf_ptr->response_header,
                                          &req_header,
                                          MMGSDI_INCREASE_CNF,
                                          sw_present,
                                          uim_rsp_ptr->status_word);

      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
  }

  return mmgsdi_status;
} /* mmgsdi_common_rsp_increase */


/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_WRITE

   DESCRIPTION:
     This function will build the response to Write request

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_write (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_write_cnf_type                ** mmgsdi_cnf_pptr,
  boolean                                 synch,
  mmgsdi_protocol_enum_type               protocol
)
{
  mmgsdi_request_header_type             req_header;
  int32                                  index                = 0;
  mmgsdi_return_enum_type                mmgsdi_status        = MMGSDI_SUCCESS;
  uim_cmd_type                         * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type    * extra_param_ptr      = NULL;
  mmgsdi_return_enum_type                report_status        = MMGSDI_SUCCESS;
  mmgsdi_write_cnf_type                * cnf_ptr              = NULL;
  mmgsdi_data_type                       write_data_buf;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_cnf_pptr);
  }

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  memset(&write_data_buf, 0x00, sizeof(mmgsdi_data_type));

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

  if (extra_param_ptr == NULL ||
      extra_param_ptr->write_data.data.data_ptr == NULL ||
      extra_param_ptr->write_data.data.data_len <= 0)
  {
    UIM_MSG_ERR_0("No extra write response data");
    return MMGSDI_ERROR;
  }

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
         write to cache if item is in the cache
  ---------------------------------------------------------------------------*/
  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  /* copy rsp_data from uim_rsp_ptr so it can be modified by
     TMSI PLMN check during util write cache operation*/
  write_data_buf.data_len = extra_param_ptr->write_data.data.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(write_data_buf.data_ptr,
                                     write_data_buf.data_len);

  /* In case of failed memory allocation, clear cache to avoid stale data */
  if (write_data_buf.data_ptr == NULL)
  {
    if (extra_param_ptr->increase_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      (void)mmgsdi_cache_delete_item(req_header.session_id,
                                     1,
                                     &extra_param_ptr->increase_data.access.file.file_enum);
    }
  }
  else
  {
    mmgsdi_memscpy(write_data_buf.data_ptr,
                   int32touint32(write_data_buf.data_len),
                   extra_param_ptr->write_data.data.data_ptr,
                   int32touint32(write_data_buf.data_len));

    if (report_status == MMGSDI_SUCCESS)
    {
      mmgsdi_data_from_enum_type data_from = MMGSDI_DATA_FROM_CARD_INCOMPLETE;

      if(extra_param_ptr->write_data.data.data_len == 0 && 
         extra_param_ptr->write_data.offset == 0)
      {
        data_from = MMGSDI_DATA_FROM_CARD_COMPLETE;
      }

      /* Write to cache. If it is record based file,
         update only if it is 1st rec */
      if(!((extra_param_ptr->write_data.file_type == MMGSDI_LINEAR_FIXED_FILE) &&
           (extra_param_ptr->write_data.rec_num != 1)))
      {
        (void)mmgsdi_cache_write(
                          req_header.session_id,
                          &extra_param_ptr->write_data.access,
                          write_data_buf,
                          extra_param_ptr->write_data.offset,
                          data_from);
      }
      mmgsdi_status = MMGSDI_SUCCESS;
    }
    else
    {
      /* unsuccessful processing of command */
      if (extra_param_ptr->write_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
      {
        if(report_status == MMGSDI_NOT_FOUND)
        {
          if(!((extra_param_ptr->write_data.file_type == MMGSDI_LINEAR_FIXED_FILE) &&
               (extra_param_ptr->write_data.rec_num != 1)))
          {
            /* Update cache to indicate that file was not found on card */
            (void)mmgsdi_cache_write(
                    req_header.session_id,
                    &extra_param_ptr->write_data.access,
                    write_data_buf,
                    extra_param_ptr->write_data.offset,
                    MMGSDI_DATA_NOT_FOUND);
          }
        }
      }
    } /* Parsing UIM Status */

    MMGSDIUTIL_TMC_MEM_FREE(write_data_buf.data_ptr);
  }

  if(!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                               &req_header,
                                               extra_param_ptr,
                                               NULL,
                                               uim_rsp_ptr->cmd_transacted,
                                               /* status word required for write */
                                               uim_rsp_ptr->status_word);
  }
  else
  {
    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*mmgsdi_cnf_pptr),
      sizeof(mmgsdi_write_cnf_type));
    if ((*mmgsdi_cnf_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    cnf_ptr = *mmgsdi_cnf_pptr;

    mmgsdi_status = mmgsdi_cnf_build_write_data(cnf_ptr,
                                                report_status,
                                                extra_param_ptr);

    /* Populate Header */
    /* Header population should be done AFTER data populate because any pointer
       length will be assigned in the individual confirmation population and used in the
       populate_cnf_rsp_header for assigning the response   length
    */
    mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                        &cnf_ptr->response_header,
                                        &req_header,
                                        MMGSDI_WRITE_CNF,
                                        uim_rsp_ptr->cmd_transacted,
                                        /* Status word required for read */
                                        uim_rsp_ptr->status_word);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
    else
    {
      UIM_MSG_ERR_1("Failed to build write data: status 0x%x", mmgsdi_status);
      cnf_ptr->response_header.mmgsdi_status = mmgsdi_status;
    }
  }

  return mmgsdi_status;
} /* mmgsdi_common_rsp_write */


/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_ACTIVATE_ONCHIP

   DESCRIPTION:
     This function will build the response to ACTIVATE ONCHIP REQ

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_activate_onchip(
  mmgsdi_return_enum_type                     mmgsdi_status,
  const mmgsdi_activate_onchip_sim_req_type * onchip_sim_config_ptr
)
{
  mmgsdi_sw_status_type   status_word;

  UIM_MSG_HIGH_1("MMGSDI_PROCESS_ACTIVATE_ONCHIP_RSP status 0x%x", mmgsdi_status);

  /* Status word not applicable to Onchip command */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  MMGSDIUTIL_RETURN_IF_NULL(onchip_sim_config_ptr);

  return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                    &onchip_sim_config_ptr->request_header,
                                    NULL,
                                    NULL,
                                    FALSE,/* sw not required for onchip */
                                    status_word);
} /* mmgsdi_common_rsp_activate_onchip */


/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_COMPUTE_IP_AUTH

   DESCRIPTION:
     This function will build the response to a COMPUTE IP AUTH command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_compute_ip_auth (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
)
{
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

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
  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_3("UIM_FAIL sw1 %x sw2 %x status %d",
                   uim_rsp_ptr->status_word.sw1, uim_rsp_ptr->status_word.sw2,
                   report_status);
  } /* Parsing UIM Status */

  if (uim_rsp_ptr->rsp_data.data_len > 0 && uim_rsp_ptr->rsp_data.data_ptr != NULL)
  {
    UIM_MSG_HIGH_1("IP auth response first byte: 0x%x",
                   uim_rsp_ptr->rsp_data.data_ptr[0]);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      uim_rsp_ptr->cmd_transacted,
      uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_common_rsp_compute_ip_auth */


/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_RUN_CAVE

   DESCRIPTION:
     This function will build the response to a RUN CAVE command

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_run_cave (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }
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
  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_3("UIM_FAIL sw1 %x sw2 %x status %d",
                   uim_rsp_ptr->status_word.sw1, uim_rsp_ptr->status_word.sw2,
                   report_status);
  } /* Parsing UIM Status */

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      uim_rsp_ptr->cmd_transacted,
      uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_rsp_run_cave */


/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_CDMA_AKA_AUTH

   DESCRIPTION:
     This function will build the response to a CDMA AKA AUTH command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_cdma_aka_auth (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }
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
  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_3("UIM_FAIL sw1 %x sw2 %x status %d",
                   uim_rsp_ptr->status_word.sw1, uim_rsp_ptr->status_word.sw2,
                   report_status);
  } /* Parsing UIM Status */

  if (uim_rsp_ptr->rsp_data.data_len > 0 && uim_rsp_ptr->rsp_data.data_ptr != NULL)
  {
    UIM_MSG_HIGH_1("CDMA-AKA auth response first byte: 0x%x",
                   uim_rsp_ptr->rsp_data.data_ptr[0]);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      uim_rsp_ptr->cmd_transacted,
      uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_rsp_cdma_aka_auth */


/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_GENERATE_KEY_VPM

   DESCRIPTION:
     This function will build the response to a GENERATE KEY command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_generate_key_vpm (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;
  mmgsdi_data_type                    response_data;

  memset(&response_data, 0x00, sizeof(mmgsdi_data_type));

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }
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
  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_3("UIM_FAIL sw1 %x sw2 %x status %d",
                   uim_rsp_ptr->status_word.sw1, uim_rsp_ptr->status_word.sw2,
                   report_status);
  } /* Parsing UIM Status */

  /* Look back in the original command to see what the expected size of the
    response should be. The response should an 8-byte key, plus n VPM Octets
    where n is last - first + 1 according to IS-820-C */

  if((uim_cmd_ptr->gen_keys.vpm_first_octet != 0xFF) &&
     (uim_cmd_ptr->gen_keys.vpm_last_octet != 0xFF) &&
      (uim_cmd_ptr->gen_keys.vpm_last_octet >=
       uim_cmd_ptr->gen_keys.vpm_first_octet))
  {
    response_data.data_len = (uim_cmd_ptr->gen_keys.vpm_last_octet) -
      (uim_cmd_ptr->gen_keys.vpm_first_octet) + 1 + MMGSDI_KEY_SIZE;
    if(uim_rsp_ptr->rsp_data.data_len > UIM_MAX_CHARS)
    {
       UIM_MSG_HIGH_1("Invalid response length from GENERATE KEY command. Length: %d",
                      uim_rsp_ptr->rsp_data.data_len);
       return MMGSDI_ERROR;
    } /* end if Response is too long, something wrong. Limit to UIM_MAX_CHARS */
  } /* end if - the UIM Command data has valid first / last octet data */
  else
  {
    UIM_MSG_HIGH_0("Invalid input data for Generate Key. Response unpopulated");
    return MMGSDI_ERROR;
  } /* end if  - the UIM command Octet Data has invalid first / last info */

  /* Obtain memory for confirmation data */
  if (uim_rsp_ptr->rsp_data.data_ptr &&
      response_data.data_len > 0)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(response_data.data_ptr,
                                       response_data.data_len);
    if (response_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    mmgsdi_memscpy(response_data.data_ptr,
                   int32touint32(response_data.data_len),
                   uim_rsp_ptr->rsp_data.data_ptr,
                   int32touint32(response_data.data_len));
  }
  else
  {
    UIM_MSG_HIGH_0("No response data allocated");
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&response_data,
      FALSE, /* status word not required for generate key */
      uim_rsp_ptr->status_word);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(response_data.data_ptr);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_rsp_generate_key_vpm */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_BLOCK

  DESCRIPTION:
    This function will build the response to OTASP Block Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_block (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

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

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* unsuccessful processing of command */
    UIM_MSG_ERR_2("UIM Response parsing failed for OTASP 0x%x Block Request 0x%x",
                  extra_param_ptr->session_otasp_block_data.otasp_block_op_type,
                  report_status);
    /* -------------------------------------------------------------------
    Command processing returned Status words other than 0x90 0x00
    Log Error Message
    ----------------------------------------------------------------------*/
    UIM_MSG_ERR_3("UIM_FAIL sw1:0x%x sw2:0x%x status:0x%0x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for OTASP block operation */
                                             uim_rsp_ptr->status_word);

  UIM_MSG_HIGH_2("RSP - OTASP block 0x%x Request status is 0x%x",
                 extra_param_ptr->session_otasp_block_data.otasp_block_op_type,
                 mmgsdi_status);
  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_common_rsp_otasp_block */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_OTAPA

  DESCRIPTION:
    This function will build the response to OTASP OTAPA Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_otapa (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

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

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* -----------------------------------------------------------------------
    Unsuccessful processing of command, command processing returned
    Status words other than 0x90 0x00. Log Error Message
    --------------------------------------------------------------------------*/
    UIM_MSG_ERR_3("UIM Response parsing failed for OTAPA Request sw1:0x%x, sw2:0x%x report_status:0x%x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for OTA operation */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_common_rsp_otasp_otapa */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_COMMIT

  DESCRIPTION:
    This function will build the response to OTASP COMMIT Request

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_commit (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

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

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* -----------------------------------------------------------------------
    Unsuccessful processing of command, command processing returned
    Status words other than 0x90 0x00. Log Error Message
    --------------------------------------------------------------------------*/
    UIM_MSG_ERR_3("UIM Response parsing failed for OTASP Commit sw1:0x%x, sw2:0x%x report_status:0x%x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for OTASP Commit operation */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_common_rsp_otasp_commit */

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_SSPR_CONFIG

  DESCRIPTION:
    This function will build the response to OTASP SSPR Config Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_sspr_config (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

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

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* -----------------------------------------------------------------------
    Unsuccessful processing of command, command processing returned
    Status words other than 0x90 0x00. Log Error Message
    --------------------------------------------------------------------------*/
    UIM_MSG_ERR_3("UIM Response parsing failed for OTASP SSPR Config Request sw1:0x%x, sw2:0x%x report_status:0x%x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for OTASP SSPR Config operation */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_common_rsp_otasp_sspr_config */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_KEY_GEN

  DESCRIPTION:
    This function will build the response to OTASP Key Gen Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_key_gen (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

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

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* -----------------------------------------------------------------------
    Unsuccessful processing of command, command processing returned
    Status words other than 0x90 0x00. Log Error Message
    --------------------------------------------------------------------------*/
    UIM_MSG_ERR_3("UIM Response parsing failed for OTASP Key Gen Request sw1:0x%x, sw2:0x%x report_status:0x%x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for OTASP KEY GEN operation */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_common_rsp_otasp_key_gen */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_MS_KEY

  DESCRIPTION:
    This function will build the response to OTASP MS Key Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_ms_key (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }
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

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* -----------------------------------------------------------------------
    Unsuccessful processing of command, command processing returned
    Status words other than 0x90 0x00. Log Error Message
    --------------------------------------------------------------------------*/
    UIM_MSG_ERR_3("UIM Response parsing failed for OTASP MS Key Request sw1:0x%x, sw2:0x%x report_status:0x%x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for OTASP MS Key operation */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_common_rsp_otasp_ms_key */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_BS_CHAL

  DESCRIPTION:
    This function will build the response to BS CHAL Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_bs_chal (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }


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

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* -----------------------------------------------------------------------
    Unsuccessful processing of command, command processing returned
    Status words other than 0x90 0x00. Log Error Message
    --------------------------------------------------------------------------*/
    UIM_MSG_ERR_3("UIM Response parsing failed for BS CHAL Request sw1:0x%x, sw2:0x%x report_status:0x%x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for BS CHAL operation */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_common_rsp_bs_chal */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_SSD_CONFIRM

  DESCRIPTION:
    This function will build the response to SSD Confirm Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_ssd_confirm (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

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

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* -----------------------------------------------------------------------
    Unsuccessful processing of command, command processing returned
    Status words other than 0x90 0x00. Log Error Message
    --------------------------------------------------------------------------*/
    UIM_MSG_ERR_3("UIM Response parsing failed for SSD Confirm Request sw1:0x%x, sw2:0x%x report_status:0x%x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for SSD Confirm operation */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_common_rsp_ssd_confirm */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_SSD_UPDATE

  DESCRIPTION:
    This function will build the response to SSD Update Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_ssd_update (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

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

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* -----------------------------------------------------------------------
    Unsuccessful processing of command, command processing returned
    Status words other than 0x90 0x00. Log Error Message
    --------------------------------------------------------------------------*/
    UIM_MSG_ERR_3("UIM Response parsing failed for SSD Update Request sw1:0x%x, sw2:0x%x report_status:0x%x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for SSD Update operation */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_common_rsp_ssd_update */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_STORE_ESN

  DESCRIPTION:
    This function will build the response to Store ESN request

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_store_esn(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_session_store_esn_cnf_type    ** store_esn_cnf_pptr,
  boolean                                 synch,
  mmgsdi_protocol_enum_type               protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;
  mmgsdi_session_store_esn_cnf_type *cnf_ptr          = NULL;
  mmgsdi_slot_data_type             *slot_data_ptr    = NULL;

  UIM_MSG_HIGH_1("MMGSDI_COMMON_RSP_STORE_ESN, synch: 0x%x", synch);

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(store_esn_cnf_pptr);
  }

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

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  UIM_MSG_HIGH_1("Store ESN status 0x%x",report_status);

  /* Store the ESN ME Change Flag, LSB indicates if ESN has changed*/
  if ((report_status == MMGSDI_SUCCESS ) &&
      (uim_rsp_ptr->rsp_data.data_ptr != NULL) &&
      (uim_rsp_ptr->rsp_data.data_len == 1))
  {
    if(uim_rsp_ptr->rsp_data.data_ptr[0] & MMGSDI_JCDMA_ESN_ME_BIT)
    {
      slot_data_ptr->esn_me_change_flag = MMGSDI_ESN_ME_CHANGE;
    }
    else
    {
      slot_data_ptr->esn_me_change_flag = MMGSDI_ESN_ME_NO_CHANGE;
    }
    UIM_MSG_HIGH_1("ESN ME Change flag set to : 0x%x",
                   slot_data_ptr->esn_me_change_flag);
  }

  if (!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                               &req_header,
                                               NULL,
                                               NULL,
                                               FALSE, /* status word not required
                                                        for store esn */
                                               uim_rsp_ptr->status_word);
  }
  else
  {
    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      (*store_esn_cnf_pptr),
      sizeof(mmgsdi_session_store_esn_cnf_type));
    if (*store_esn_cnf_pptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    cnf_ptr = *store_esn_cnf_pptr;

    /* Populate Header */
    /* Header population should be done AFTER data populate
      because any pointer length will be assigned in the
      individual confirmation population and used in the
      populate_cnf_rsp_header for assigning the response
      length */
    mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                        &cnf_ptr->response_header,
                                        &req_header,
                                        MMGSDI_SESSION_STORE_ESN_CNF,
                                        FALSE, /* status word not
                                               required for gfa */
                                        uim_rsp_ptr->status_word);

    cnf_ptr->response_header.mmgsdi_status = report_status;
  }

  UIM_MSG_HIGH_1("RSP - STORE_ESN status is 0x%x", report_status);
  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  (void)store_esn_cnf_pptr;
  (void)synch;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_rsp_store_esn */


/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_SEARCH

   DESCRIPTION:
     This function will build the response to search request

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_search (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_search_cnf_type            ** mmgsdi_cnf_pptr,
  boolean                              synch,
  mmgsdi_protocol_enum_type            protocol
)
{
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                         *uim_cmd_ptr    = NULL;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr= NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;
  mmgsdi_search_cnf_type             * cnf_ptr         = NULL;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_cnf_pptr);
  }

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

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (protocol == MMGSDI_UICC)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }
  else
  {
    report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (report_status != MMGSDI_SUCCESS)
  {
    /* unsuccessful processing of command */
    UIM_MSG_ERR_0("UIM Response parsing failed for Search");
    mmgsdi_util_print_file_information(extra_param_ptr->search_data.access, TRUE);
    /* -------------------------------------------------------------------
    Command processing returned Status words other than 0x90 0x00
    Log Error Message
    ----------------------------------------------------------------------*/
    MMGSDIUTIL_DEBUG_MSG_ERROR_3("UIM_FAIL sw1:0x%x sw2:0x%x status:0x%x",
                                 uim_rsp_ptr->status_word.sw1,
                                 uim_rsp_ptr->status_word.sw2,
                                 report_status);
  }/* End of if (report_status == MMGSDI_SUCCESS) */

  if(!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                               &req_header,
                                               extra_param_ptr,
                                               (void*)&uim_rsp_ptr->rsp_data,
                                               uim_rsp_ptr->cmd_transacted,
                                               /* status word required for search */
                                               uim_rsp_ptr->status_word);

  }
  else
  {
    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*mmgsdi_cnf_pptr),
                                       sizeof(mmgsdi_search_cnf_type));
    if (*mmgsdi_cnf_pptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    cnf_ptr = *mmgsdi_cnf_pptr;

    mmgsdi_status = mmgsdi_cnf_build_search_data(cnf_ptr,
                                                 report_status,
                                                 extra_param_ptr,
                                                 (mmgsdi_data_type*)&uim_rsp_ptr->rsp_data);

    /* Populate Header */
    /* Header population should be done AFTER data populate because any pointer
     * length will be assigned in the individual confirmation population and used in the
     * populate_cnf_rsp_header for assigning the response   length
     */
    mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                        &cnf_ptr->response_header,
                                        &req_header,
                                        MMGSDI_SEARCH_CNF,
                                        uim_rsp_ptr->cmd_transacted,
                                        /* Status word required for read */
                                        uim_rsp_ptr->status_word);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
    else
    {
      UIM_MSG_ERR_1("Failed to build search data: status 0x%x", mmgsdi_status);
      cnf_ptr->response_header.mmgsdi_status = mmgsdi_status;
    }
  }

  UIM_MSG_HIGH_1("RSP - SEARCH status is 0x%x",mmgsdi_status);
  return mmgsdi_status;
}/* mmgsdi_common_rsp_search */


/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_SRV_AVAILABLE

   DESCRIPTION:
     This function will build the response for the service available command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_srv_available(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header;
  int32                               index            = 0;
  mmgsdi_return_enum_type             mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type                       *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type  *extra_param_ptr  = NULL;

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

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(!uim_rsp_ptr->is_uim_success)
  {
    mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                             &req_header,
                                             extra_param_ptr,
                                            (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* status word not required
                                                       for srv avail */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_common_rsp_srv_available*/

