/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   U I M    I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC protocol processing support for MMGSDI
  before the command is being sent to UIM Module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uim_icc.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   ar      Review of macros used by MMGSDI
05/17/16   tkl     F3 log prints cleanup
05/17/16   ar      Fix read/write of cache files when cache is not_init
05/09/16   vdc     Reverting populate channel id before sending the APDU request
08/10/15   vdc     Retry read request for non spec compliant cards
07/14/15   ar      Cache complete data for req's with partial read/write data
12/03/14   vdc     Populate channel id before sending the APDU request
09/09/14   kk      Enhancements in NV handling
09/30/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
08/29/14   yt      Reject WRITE request if data length does not match file size
08/27/14   yt      Support for restricted SIM access via MMGSDI session APIs
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/30/14   ar      Revert FR 20469 changes to align with NAS check-in dates
07/25/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
07/15/14   yt      Support for NV refresh
04/29/14   nmb     Re-enable AT+CRSM after directory maintenance changes
04/24/14   am      Alloc extra params only if file info is available
04/22/14   av      Fix double free when handling icc pin operation response
02/10/14   ar      Replace session checks with utility functions
02/05/14   tkl     SFI Support and decouple uim file enums
01/21/14   yt      Remove duplicate code
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Remove Off Target featurization to cover functionality
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/11/13   df      Remove unused memory paging feature
10/04/13   spo     F3 message reduction
09/20/13   yt      Remove duplicate code
08/07/13   tl      Add CDMA AKA Authenticate functionality
07/22/13   df      Correction invalidate/rehabilitate to allow enum by path
06/24/13   vdc     Added support for triple SIM
06/18/13   vv      Fix copying the entire path
06/06/13   yt      Add check for index
05/16/13   vdc     Replace memcpy with safer version memscpy
04/16/13   tl      Clean up of radio access features from MMGSDI code
03/26/13   vv      Fixed EF access outside ADF on card slot sessions
03/04/13   vv      Allow EFs outside ADF to be accessed before app. selection
12/26/12   av      Do not allow writing to file that is being refreshed
12/05/12   vv      Added support for parallel processing in MMGSDI
10/01/12   abg     Removed Problem determining protocol type F3 message
09/03/12   bcho    mmgsdi_uim_icc_search_record modified for sync operation
03/29/12   shr     Enhancements to file access request methods
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   shr     Legacy GSDI removal updates
10/13/11   nb      Adding support for full recovery
05/17/11   yt      Added support for INCREASE request
02/01/11   ssr     Add extra_param_ptr NULL check for pin request
11/24/10   yt      Removed NULL check for extra_param_ptr for RUN_GSM_ALGO_REQ
11/05/10   yt      Klocwork fixes
07/14/10   js      Fixed compiler warnings
06/24/10   yt      Klocwork fixes
05/25/10   shr     Clean-up file access operations on Card Sessions
05/19/10   nb      Updated mmgsdi_uim_icc_write_transparent to
                   support sync writes
05/12/10   shr     File access operations need to be allowed on Card Sessions
                   irrespective of the provisioning status
05/04/10   yt      Lint fixes
03/12/10   kk      Fixed select command handling for card slot sessions
01/25/10   vs      Fixed compile error
12/18/09   nb      Moved Dual Slot Changes
12/15/09   ssr     Add check for RUIM APPS for cdma security commands
12/07/09   js      Fixed Lint Errors
10/21/09   kk      Fixed klocwork error - Possible Memory leak
09/26/09   kp      ZI memory reduction changes
09/08/09   rn      Fixed klocwork errors
04/29/09   js      Fixed compiler warning
09/17/08   ssr     Fixed memory free error from mmgsdi_uim_uicc_search_record
08/20/08   jk      Support for Generate Key API & Fixes for CAVE
08/08/08   ssr     Add more detailed messages for dispalyed in the log for
                   mmgsdi icc search
07/31/08   ssr     Update search for blocking the search directions belonging
                   to Record Pointer.
06/12/08   ssr     Support of MMGSDI Search functionality
06/20/08   sp      Add data offset messages for read
03/06/08   nk      Added synchronous support to mmgsdi_uim_icc_select and
                   mmgsdi_uim_icc_rehabilitate
01/10/08   nk      Fixed lint errors
12/17/07   vs      Featurized function mmgsdi_uim_icc_run_cave()
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access.  Add
                   support for cyclic and linear fixed write
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
10/26/06   tml     Get Key ref from client ID
08/07/06   sun     Store the PIN on any pin operation
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/08/06   tml     fixed featurized
05/23/06   tml     rehabilitate and invalidate supports
02/14/06   nk      Merged for ARR caching and by path changes
01/17/05   tml     MMGSDI cleanup
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/30/05   jar     Fix for not sending proper pointer to malloc function
08/30/05   jar     Added support for ICC Status Commands
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Channel ID support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   sk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_common_rsp.h"
#include "mmgsdi_file.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_SELECT

   DESCRIPTION:
     This function will format the get file attribute request in the form
     required by the UIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.

     NOTE that this function must make sure to free req_ptr->int_client_data
     for every error return path.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). This function is also limited to the use of EFs.


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
mmgsdi_return_enum_type mmgsdi_uim_icc_select (
  const mmgsdi_get_file_attr_req_type   *req_ptr,
  boolean                                sync_process,
  mmgsdi_get_file_attr_cnf_type        **cnf_pptr
)
{
  uim_cmd_type                          * uim_cmd_ptr      = NULL;
  uim_slot_type                           uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type                 mmgsdi_status    = MMGSDI_SUCCESS;
  uim_file_type                           type_of_file     = UIM_EF;
  uim_dir_type                            directory        = 0;
  int32                                   index            = 0;
  mmgsdi_client_req_extra_info_type     * extra_param_ptr  = NULL;
  mmgsdi_int_app_info_type              * sel_app_ptr      = NULL;
  mmgsdi_session_info_type              * session_info_ptr = NULL;
  mmgsdi_task_enum_type                   mmgsdi_task      = MMGSDI_TASK_MAX;

  UIM_MSG_HIGH_1("MMGSDI UIM ICC SELECT, sync process is 0x%x", sync_process);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  do
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                                             req_ptr->request_header.session_id,
                                             NULL,
                                             NULL,
                                             &sel_app_ptr,
                                             NULL,
                                             &session_info_ptr,
                                             NULL);
    if ((session_info_ptr != NULL) &&
        (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
    {
      /* If it is an EF/DF access outside of any ADF, it is not an error */
      if (mmgsdi_file_is_access_outside_adf(&req_ptr->access) == FALSE)
      {
        UIM_MSG_ERR_0("EF/DF access not allowed for Card Session");
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }
    }
    else if(mmgsdi_status != MMGSDI_SUCCESS || sel_app_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(req_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      type_of_file = UIM_EF;
    }
    else if(req_ptr->access.access_method == MMGSDI_DF_ENUM_ACCESS)
    {
      type_of_file = UIM_DF;
    }
    else if(req_ptr->access.access_method == MMGSDI_BY_PATH_ACCESS)
    {
      type_of_file = UIM_EF;
    }
    else
    {
      UIM_MSG_ERR_1("INVALID ACCESS METHOD %x", req_ptr->access.access_method);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }

    if((sel_app_ptr != NULL) &&
       (sel_app_ptr->app_data.app_type != MMGSDI_APP_SIM) &&
       (sel_app_ptr->app_data.app_type != MMGSDI_APP_RUIM))
    {
      UIM_MSG_ERR_1("INVALID APP TYPE %x", sel_app_ptr->app_data.app_type);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }

    /* Convert to UIM slot */
    mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                                 &uim_slot);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* Convert to UIM directory type */
    mmgsdi_status = mmgsdi_file_determine_uim_dir_type(&(req_ptr->access),
                                                       &directory);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* Allocate and populate extra parameter */
    mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
            MMGSDI_GET_FILE_ATTR_REQ, (void*)req_ptr, &extra_param_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
    {
      if (extra_param_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    /*---------------------------------------------------------------------------
      1) Get a UIM buffer for the request
      2) Check for if the buffer is NULL or not
    ---------------------------------------------------------------------------*/

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                       sizeof(uim_cmd_type));
    if (uim_cmd_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /*---------------------------------------------------------------------------
    Populate UIM Cmd pointer data
    ---------------------------------------------------------------------------*/
    uim_cmd_ptr->hdr.command      = UIM_SELECT_F;
    uim_cmd_ptr->select.file_type = type_of_file;
    uim_cmd_ptr->select.dir       = directory;
    uim_cmd_ptr->hdr.slot         = uim_slot;
    uim_cmd_ptr->hdr.channel      = UIM_CHANNEL0;
    uim_cmd_ptr->hdr.protocol     = UIM_ICC;

    mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                             &uim_cmd_ptr->select.path);


    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* This is a GSM select, so nil-out ADF specific parameters */
    uim_cmd_ptr->select.aid.aid_length = 0;

    if (req_ptr->activate_aid)
    {
      uim_cmd_ptr->select.session = UIM_ACTIVATION_OR_RESET;
    }
    else
    {
      uim_cmd_ptr->select.session = UIM_TERMINATION;
    }

    /*---------------------------------------------------------------------------
    Store Information into the client req info table
    1) Get the index
    2) Populate client request table
    ---------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  } while (0);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    if(req_ptr->int_client_data != 0)
    {
      mmgsdi_pin_extra_info_type* pin_extra_info_ptr;
      pin_extra_info_ptr = (mmgsdi_pin_extra_info_type*)req_ptr->int_client_data;
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(pin_extra_info_ptr);
    }

    /*---------------------------------------------------------------------------
      1) Release extra param pointer (Since client table has not been
                                      populated yet)
      2) Release uim cmd pointer  (Since client table has not been populated yet)
      ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been
                                    populated yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_2("Sending down SELECT command to UIM: 0x%x with user data: 0x%x",
                 req_ptr->access.file.file_enum, uim_cmd_ptr->hdr.user_data);

  if (sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      mmgsdi_task = mmgsdi_util_get_current_task();
      if (mmgsdi_task != MMGSDI_TASK_MAX &&
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data_index = index;
        mmgsdi_status = mmgsdi_icc_rsp_get_file_attr(
               mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE);
        if (mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
        }
      }
      else
      {
        UIM_MSG_ERR_2("Sync process for request at index 0x%x for file access 0x%x, has null mmgsdi_internal_synch_uim_rpt_rsp",
                      index, req_ptr->access.file.file_enum);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;

} /* mmgsdi_uim_icc_select */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_TRANSPARENT

   DESCRIPTION:
     This function package the Write Transparent command and send it to
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_transparent (
  const mmgsdi_write_req_type *  req_ptr,
  boolean                        sync_process,
  mmgsdi_write_cnf_type       ** cnf_pptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  mmgsdi_task_enum_type              mmgsdi_task      = MMGSDI_TASK_MAX;
  mmgsdi_len_type                    mmgsdi_write_len = 0;
  mmgsdi_len_type                    valid_fplmn_len  = 0;
  boolean                            is_ext_fplmn     = FALSE;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if(sync_process)
  {
    MMGSDIUTIL_RETURN_IF_NULL(cnf_pptr);
  }

  if (req_ptr->data.data_len <= 0)
    return MMGSDI_ERROR;

  /* Before sending the write request to uim, check if this file is currently
     being refreshed */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_BLOCK_WRITES_TO_REFRESH_FILES, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED &&
     mmgsdi_is_file_in_refresh_data_info_list(req_ptr->request_header.session_id,
                                              req_ptr->access))
  {
    UIM_MSG_ERR_0("Request temporarily not allowed as requested file is being refreshed");
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    &session_info_ptr,
                    NULL);
  /* Check if the write request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    if (mmgsdi_file_is_access_outside_adf(&req_ptr->access) == FALSE)
    {
      UIM_MSG_ERR_0("EF/DF access not allowed for Card Session");
      return MMGSDI_ERROR;
    }
  }
  else if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  /* Before sending write, make sure we are not trying to
     write more than the max file size */
  mmgsdi_write_len = req_ptr->data.data_len;
  mmgsdi_status = mmgsdi_file_check_file_info_and_update_len(
                    req_ptr->request_header.session_id,
                    req_ptr->request_header.slot_id,
                    req_ptr->access,
                    req_ptr->file_type,
                    req_ptr->offset,
                    &mmgsdi_write_len,
                    FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* If file is GSM_FPLMN and extended FPLMN feature is supported then
     data to be written can be more than file len as per attributes */
  if (req_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS &&
      req_ptr->access.file.file_enum == MMGSDI_GSM_FPLMN &&
      mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SUPPORT_EXTENDED_FPLMN_ICC,
        MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
  {
    is_ext_fplmn = TRUE;
  }
  else if(mmgsdi_write_len < req_ptr->data.data_len)
  {
    /* Data to be written is larger than the file size. Instead of truncating
       the data, return error here. Otherwise the client might think that 
       entire data in the request was written to the card. */
    return MMGSDI_INCORRECT_LENGTH;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_WRITE_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_WRITE_BIN_CMD_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) record access mode
    5) offset: based on file type, either record number or offset will be sent
    6) data len and data
    7) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_WRITE_BIN_CMD_F;
  uim_cmd_ptr->hdr.protocol        = UIM_ICC;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->write_bin.offset    = int32touint16(req_ptr->offset);
  uim_cmd_ptr->write_bin.len       = int32touint16(req_ptr->data.data_len);
  uim_cmd_ptr->write_bin.data      = extra_param_ptr->write_data.data.data_ptr;

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->write_bin.path);


  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer write data (Since client table has not been
                                    populated yet)
    2) Release extra param pointer (Since client table has not been populated
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer write data (Since client table has not been
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* If the file is GSM_FPLMN then write only last 4 valid FPLMNs (latest ones
     according to NAS) in the card and in our cache maintain the complete list
     of MMGSDI_EXTENDED_ICC_FPLMN_CACHE_LEN Bytes. In case if FPLMN data_len
     is upto card data size, write directly to card for backward compatibility.
     NAS will always give MMGSDI_EXTENDED_ICC_FPLMN_CACHE_LEN bytes data without
     any holes, with all invalid bytes set to 0xFF */
  if(is_ext_fplmn &&
     req_ptr->data.data_len > MMGSDI_GSM_FPLMN_SIZE)
  {
    if (req_ptr->data.data_len == MMGSDI_EXTENDED_ICC_FPLMN_CACHE_LEN)
    {
      /* Searching for number of valid FPLMNs(last 4 FPLMNs) in complete
         list which needs to be written to card */
      for (valid_fplmn_len = 0; valid_fplmn_len < req_ptr->data.data_len; valid_fplmn_len++)
      {
        if (*(req_ptr->data.data_ptr + valid_fplmn_len) == 0xFF)
        {
          break;
        }
      }
      if ((valid_fplmn_len % 3) != 0)
      {
        UIM_MSG_ERR_1("Incorrect FPLMN valid data: 0x%x", valid_fplmn_len);
        mmgsdi_util_free_client_request_table_index(index);
        return MMGSDI_ERROR;
      }
    }
    else
    {
      UIM_MSG_ERR_1("FPLMN data length recieved is incorrect 0x%x",
                    req_ptr->data.data_len);
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
    }

    UIM_MSG_HIGH_1("Support Extended FPLMN. Valid FPLMNs recieved: 0x%x",
                   (valid_fplmn_len/MMGSDI_PLMN_ID_SIZE));

    /* If valid list is more than card size write the last 4 FPLMNs */
    if (valid_fplmn_len > MMGSDI_GSM_FPLMN_SIZE)
    {
      uim_cmd_ptr->write_bin.data += (valid_fplmn_len - MMGSDI_GSM_FPLMN_SIZE);
    }
    uim_cmd_ptr->write_bin.len = MMGSDI_GSM_FPLMN_SIZE;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_1("Sending down WRITE command to UIM 0x%x",
                 req_ptr->access.file.file_enum);

  if(sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      mmgsdi_task = mmgsdi_util_get_current_task();
      if (mmgsdi_task != MMGSDI_TASK_MAX &&
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data_index = index;
        mmgsdi_status = mmgsdi_common_rsp_write(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE, MMGSDI_ICC);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
      }
      else
      {
        UIM_MSG_ERR_2("Sync process for request at index 0x%x for file access 0x%x, has null mmgsdi_internal_synch_uim_rpt_rsp",
                      index, req_ptr->access.file.file_enum);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }

  return mmgsdi_status;
} /* mmgsdi_uim_icc_write_transparent */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_RECORD

   DESCRIPTION:
     This function package the Write Record command and send it to
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_record (
  const mmgsdi_write_req_type *  req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  mmgsdi_len_type                    mmgsdi_write_len = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if (req_ptr->rec_num > MMGSDI_MAX_REC_NUM)
    return MMGSDI_ERROR;

  if (req_ptr->data.data_len <= 0)
    return MMGSDI_ERROR;

  /* Before sending the write request to uim, check if this file is currently
     being refreshed */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_BLOCK_WRITES_TO_REFRESH_FILES, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED &&
     mmgsdi_is_file_in_refresh_data_info_list(req_ptr->request_header.session_id,
                                              req_ptr->access))
  {
    UIM_MSG_ERR_0("Request temporarily not allowed as requested file is being refreshed");
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    &session_info_ptr,
                    NULL);
  /* Check if the write request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    if (mmgsdi_file_is_access_outside_adf(&req_ptr->access) == FALSE)
    {
      UIM_MSG_ERR_0("EF/DF access not allowed for Card Session");
      return MMGSDI_ERROR;
    }
  }
  else if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  /* Before sending write, make sure we are not trying to
     write anything other than the rec len */
  mmgsdi_write_len = req_ptr->data.data_len;
  mmgsdi_status = mmgsdi_file_check_file_info_and_update_len(
                    req_ptr->request_header.session_id,
                    req_ptr->request_header.slot_id,
                    req_ptr->access,
                    req_ptr->file_type,
                    req_ptr->offset,
                    &mmgsdi_write_len,
                    FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  if( mmgsdi_write_len < req_ptr->data.data_len)
  {
    /* Data to be written is larger than the file size. Instead of truncating
       the data, return error here. Otherwise the client might think that 
       entire data in the request was written to the card. */
    return MMGSDI_INCORRECT_LENGTH;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_WRITE_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_WRITE_REC_CMD_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) record access mode
    5) offset: based on file type, either record number or offset will be sent
    6) data len and data
    7) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_WRITE_REC_CMD_F;
  uim_cmd_ptr->hdr.protocol        = UIM_ICC;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->write_rec.rec_num   = int32touint8(req_ptr->rec_num);
  if (req_ptr->file_type == MMGSDI_CYCLIC_FILE)
  {
    uim_cmd_ptr->write_rec.recmode   = UIM_PREVIOUS;
  }
  else
  {
    uim_cmd_ptr->write_rec.recmode   = UIM_ABSOLUTE;
  }
  uim_cmd_ptr->write_rec.len       = int32touint16(mmgsdi_write_len);
  uim_cmd_ptr->write_rec.data      = extra_param_ptr->write_data.data.data_ptr;

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->write_rec.path);


  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer write data (Since client table has not been
                                    populated yet)
    2) Release extra param pointer (Since client table has not been populated
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer write data (Since client table has not been
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_1("Sending down WRITE command to UIM 0x%x",
                 req_ptr->access.file.file_enum);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_uim_icc_write_record */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_READ_TRANSPARENT

   DESCRIPTION:
     This function package the Read Transparent command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_read_transparent (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_pptr
)
{
  uim_cmd_type *                        uim_cmd_ptr      = NULL;
  uim_slot_type                         uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  int32                                 index            = 0;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr  = NULL;
  mmgsdi_session_info_type            * session_info_ptr = NULL;
  mmgsdi_task_enum_type                 mmgsdi_task      = MMGSDI_TASK_MAX;
  mmgsdi_len_type                       mmgsdi_read_len  = 0;
  mmgsdi_offset_type                    mmgsdi_offset    = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if (sync_process)
  {
    MMGSDIUTIL_RETURN_IF_NULL(cnf_pptr);
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
    req_ptr->request_header.session_id,
    NULL,
    NULL,
    NULL,
    NULL,
    &session_info_ptr,
    NULL);

  /* Check if the Read request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    if (mmgsdi_file_is_access_outside_adf(&req_ptr->access) == FALSE)
    {
      UIM_MSG_ERR_0("EF/DF access not allowed for Card Session");
      return MMGSDI_ERROR;
    }
  }
  else if ((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  /* Before sending read, make sure we are not trying to
     read more than the file size. Also, incase the data
     len passed in is zero, update that to the max file size
     because per the agreement between mmgsdi and drivers,
     mmgsdi must always send a non zero length. */
  mmgsdi_read_len = req_ptr->data_len;
  mmgsdi_status = mmgsdi_file_check_file_info_and_update_len(
                    req_ptr->request_header.session_id,
                    req_ptr->request_header.slot_id,
                    req_ptr->access,
                    req_ptr->file_type,
                    req_ptr->offset,
                    &mmgsdi_read_len,
                    FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* In case a partial file read, data_len might be updated to read the
     entire file so that it is cached by MMGSDI. In that case, offset
	 need to be adjust as well so that file is read from beginning. */
  mmgsdi_offset = req_ptr->offset;
  if(req_ptr->offset > 0 &&
     mmgsdi_read_len > req_ptr->data_len)
  {
    mmgsdi_offset = 0;
  }	

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_READ_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_READ_BIN_CMD_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) record access mode
    5) data len
    6) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_READ_BIN_CMD_F;
  uim_cmd_ptr->hdr.protocol        = UIM_ICC;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->read_bin.path);


  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  uim_cmd_ptr->read_bin.offset     = int32touint16(mmgsdi_offset);
  uim_cmd_ptr->read_bin.len        = int32touint16(mmgsdi_read_len);

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been populated
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_3("Sending down READ command to UIM 0x%x with file offset 0x%x and length 0x%x",
                 req_ptr->access.file.file_enum,
                 uim_cmd_ptr->read_bin.offset,
                 uim_cmd_ptr->read_bin.len);

  if (sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      mmgsdi_task = mmgsdi_util_get_current_task();
      if (mmgsdi_task != MMGSDI_TASK_MAX &&
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data_index = index;
        mmgsdi_status = mmgsdi_icc_rsp_read(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, sync_process);

        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
      }
      else
      {
        UIM_MSG_ERR_2("Sync process for request at index 0x%x for file access 0x%x, has null mmgsdi_internal_synch_uim_rpt_rsp",
                      index, req_ptr->access.file.file_enum);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }

  return mmgsdi_status;

} /* mmgsdi_uim_icc_read_transparent */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_READ_RECORD

   DESCRIPTION:
     This function package the Read Record command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_read_record (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_pptr
)
{
  uim_cmd_type *                        uim_cmd_ptr      = NULL;
  uim_slot_type                         uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  int32                                 index            = 0;
  mmgsdi_client_req_extra_info_type    *extra_param_ptr  = NULL;
  mmgsdi_session_info_type            * session_info_ptr = NULL;
  mmgsdi_task_enum_type                 mmgsdi_task      = MMGSDI_TASK_MAX;
  mmgsdi_len_type                       mmgsdi_read_len  = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if (sync_process)
  {
    MMGSDIUTIL_RETURN_IF_NULL(cnf_pptr);
  }
  if (req_ptr->rec_num > MMGSDI_MAX_REC_NUM)
  {
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
    req_ptr->request_header.session_id,
    NULL,
    NULL,
    NULL,
    NULL,
    &session_info_ptr,
    NULL);
  /* Check if the Read request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    if (mmgsdi_file_is_access_outside_adf(&req_ptr->access) == FALSE)
    {
      UIM_MSG_ERR_0("EF/DF access not allowed for Card Session");
      return MMGSDI_ERROR;
    }
  }
  else if ((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  /* Before sending read, make sure we are not trying to
     read more than the rec len. Also, make sure we pass
     the rec len down to the drivers because per the
     agreement between mmgsdi and drivers mmgsdi must
     always send a non zero length. */
  mmgsdi_read_len = 0;
  mmgsdi_status = mmgsdi_file_check_file_info_and_update_len(
                    req_ptr->request_header.session_id,
                    req_ptr->request_header.slot_id,
                    req_ptr->access,
                    req_ptr->file_type,
                    req_ptr->offset,
                    &mmgsdi_read_len,
                    FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_READ_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_READ_REC_CMD_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) record access mode
    5) data len
    6) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_READ_REC_CMD_F;
  uim_cmd_ptr->hdr.protocol        = UIM_ICC;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->read_rec.rec_num    = int32touint8(req_ptr->rec_num);
  uim_cmd_ptr->read_rec.recmode    = UIM_ABSOLUTE;
  uim_cmd_ptr->read_rec.rec_len    = int32touint8(mmgsdi_read_len);

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->read_rec.path);


  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been populated
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_2("Sending down READ command to UIM 0x%x record no 0x%x",
                 req_ptr->access.file.file_enum, uim_cmd_ptr->read_rec.rec_num);

  if (sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      mmgsdi_task = mmgsdi_util_get_current_task();
      if (mmgsdi_task != MMGSDI_TASK_MAX &&
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data_index = index;
        mmgsdi_status = mmgsdi_icc_rsp_read(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, sync_process);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
      }
      else
      {
        UIM_MSG_ERR_2("Sync process for request at index 0x%x for file access 0x%x, has null mmgsdi_internal_synch_uim_rpt_rsp",
                      index, req_ptr->access.file.file_enum);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }

  return mmgsdi_status;

} /* mmgsdi_uim_icc_read_record */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_INCREASE

   DESCRIPTION:
     This function package the Increase command and send it to
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_increase should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_increase (
  const mmgsdi_increase_req_type *  req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (req_ptr->data.data_len <= 0)
    return MMGSDI_ERROR;

  /* Before sending the increase request to uim, check if this file is currently
     being refreshed */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_BLOCK_WRITES_TO_REFRESH_FILES, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED &&
     mmgsdi_is_file_in_refresh_data_info_list(req_ptr->request_header.session_id,
                                              req_ptr->access))
  {
    UIM_MSG_ERR_0("Request temporarily not allowed as requested file is being refreshed");
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    &session_info_ptr,
                    NULL);
  /* Check if the increase request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    if (mmgsdi_file_is_access_outside_adf(&req_ptr->access) == FALSE)
    {
      UIM_MSG_ERR_0("EF/DF access not allowed for Card Session");
      return MMGSDI_ERROR;
    }
  }
  else if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_INCREASE_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->increase_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_INC_CMD_F
    2) protocol = UIM_ICC  [ GSM and CDMA share the same protocol ]
    3) slot
    4) record access mode
    5) offset: based on file type, either record number or offset will be sent
    6) data len and data
    7) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command  = UIM_INC_CMD_F;
  uim_cmd_ptr->hdr.protocol = UIM_ICC;
  uim_cmd_ptr->hdr.slot     = uim_slot;
  uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;
  uim_cmd_ptr->inc.len      = int32touint16(req_ptr->data.data_len);
  uim_cmd_ptr->inc.data     = extra_param_ptr->increase_data.data.data_ptr;

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->inc.path);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->increase_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer increase data (Since client table has not been
                                    populated yet)
    2) Release extra param pointer (Since client table has not been populated
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->increase_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer increase data (Since client table has not been
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->increase_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_1("Sending down INCREASE command to UIM 0x%x",
             req_ptr->access.file.file_enum);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_uim_icc_increase */

/*===========================================================================
FUNCTION:  MMGSDI_UIM_ICC_SEND_APDU

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_send_apdu(
  mmgsdi_send_apdu_req_type* req_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;
  int32                              index           = 0;
  boolean                            param_valid     = TRUE;

  /*
   * Parameter checks.
   */
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);
  /*
   * Get the protocol.
   */
  if (mmgsdi_util_determine_protocol(req_ptr->request_header.slot_id) == MMGSDI_NO_PROTOCOL)
  {
    return mmgsdi_status;
  }
  /*
   * Convert to UIM slot.
   */
  mmgsdi_status =
    mmgsdi_util_convert_uim_slot(
      req_ptr->request_header.slot_id, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*
   * Allocate and populate extra parameter.
   */
  mmgsdi_status =
    mmgsdi_util_alloc_and_populate_extra_param(
      req_ptr->request_header.request_type, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }
  /*
   * 1) Get a UIM buffer for the request.
   * 2) Check for if the buffer is NULL or not.
   */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  param_valid &=
    mmgsdi_util_is_slot_valid(req_ptr->request_header.slot_id);
  param_valid &=
    mmgsdi_chnl_mgt_is_data_len_valid( req_ptr->data.data_len);
  param_valid &=
    mmgsdi_chnl_mgt_is_data_buffer_valid( req_ptr->data.data_ptr);

  if (param_valid == FALSE)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_ERROR;
  }
  /*
    * Populate UIM information.
    */
  uim_cmd_ptr->hdr.command  = UIM_STREAM_ISO7816_APDU_F;
  uim_cmd_ptr->hdr.protocol = UIM_ICC;
  uim_cmd_ptr->hdr.slot     = uim_slot;
  /*
    * Get a new index from client req info table for response data.
    * Set the uim data pointer to the client request table data buffer.
    */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status =
    mmgsdi_util_populate_client_request_table_info(
      index,
      &req_ptr->request_header,
      uim_cmd_ptr,
      extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /*
    * Set UIM Stream APDU IS07816 Channel Command Options.
    */
  if ( req_ptr->data.data_len > MMGSDI_MAX_CHANNEL_DATA_CMD_LEN )
  {
    req_ptr->data.data_len = MMGSDI_MAX_CHANNEL_DATA_CMD_LEN;
  }
  uim_cmd_ptr->stream_iso7816_apdu.num_cmd_bytes =
    int32touint16(req_ptr->data.data_len);
  mmgsdi_memscpy(uim_cmd_ptr->stream_iso7816_apdu.cmd_data,
    sizeof(uim_cmd_ptr->stream_iso7816_apdu.cmd_data),
    req_ptr->data.data_ptr,
    int32touint32(req_ptr->data.data_len));
  /*
    * Assign uim_cmd_ptr's user data.
    */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);
  UIM_MSG_HIGH_0("Sending down send APDU command to UIM");
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /*lint !e715 */

/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_STATUS

   DESCRIPTION
     This function is called to Queue the command to the UIM for
     a Status command.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_status(
  const mmgsdi_status_req_type        *  req_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
    2. me_app_status    - Must be 0 if not overwrite
    3. ret_data_struct  - Must be 0 if not overwrite
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if (req_ptr->me_app_status   != MMGSDI_STATUS_APP_NO_INDICATION &&
      req_ptr->ret_data_struct != MMGSDI_STATUS_NONE)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_2("APP STATUS 0x%x or DATA STRUCT INFO 0x%x",
                                 req_ptr->me_app_status,
                                 req_ptr->ret_data_struct);
    return MMGSDI_ERROR;
  }

  /* ---------------------------------------------------------------------------
     Convert the slot from an MMGSDI SLOT ID to a UIM SLOT ID
     -------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("BAD SLOT ID: 0x%x",
                                 req_ptr->request_header.slot_id);
    return mmgsdi_status;
  }

  /* ---------------------------------------------------------------------------
     Get a UIM buffer for the request
     -------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* ----------------------------------------------------------------------------
     Set the uim_cmd_ptr paramaters accordingly
     --------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command        = UIM_STATUS_F;
  uim_cmd_ptr->hdr.protocol       = UIM_GSM;
  uim_cmd_ptr->hdr.slot           = uim_slot;

  /* ----------------------------------------------------------------------------
     Set the UIM Status Status accordingly
     --------------------------------------------------------------------------*/
  uim_cmd_ptr->status.status      = UIM_NO_INDICATION;

  /* -----------------------------------------------------------------------------
     Indicate to the card the expected return Data Structure.
     ---------------------------------------------------------------------------*/
  uim_cmd_ptr->status.return_data = UIM_RETURN_NONE;

  /* ---------------------------------------------------------------------------
    CLIENT REQUEST TABLE :- Store information
    1) Get the index
    2) If not able to get the index, free the uim_cmd_q, but keep the request
       ptr and the uim_rsp_data buffer so that it can be freed in the
       mmgsdi_uicc_rsp.c
    3) Populate client request table with req_ptr, data len and uim_rsp_data buffer
    ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Provide to the UIM the client ID Entry to be returned in the Status
     Report from the UIM Task
     ------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  /* --------------------------------------------------------------------------
     The memory allocated for the UIM will not be freed until
     processed by MMGSDI Task Process Response
     ------------------------------------------------------------------------*/

  return mmgsdi_status;

} /* mmgsdi_uim_icc_status */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_REHABILITATE

   DESCRIPTION:
     This function will rehabilitate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only).

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the rehabilitate
                              or invalidate the BDN file
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_rehabilitate (
  const mmgsdi_set_file_status_req_type  *req_ptr,
  boolean                                sync_process,
  mmgsdi_rehabilitate_cnf_type           **cnf_pptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_task_enum_type              mmgsdi_task      = MMGSDI_TASK_MAX;

  UIM_MSG_HIGH_1("MMGSDI_UIM_ICC_REHABILITATE, sync_process: 0x%x", sync_process);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_REHABILITATE_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_REHABILITATE_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_REHABILITATE_F;
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;

  mmgsdi_status             = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                                   &uim_cmd_ptr->rehab.path);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 (mmgsdi_request_header_type*)&req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been populated
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_1("Sending down REHABILITATE command to UIM 0x%x",
                 req_ptr->access.file.file_enum);

  if (sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      mmgsdi_task = mmgsdi_util_get_current_task();
      if (mmgsdi_task != MMGSDI_TASK_MAX &&
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data_index = index;
        mmgsdi_status = mmgsdi_icc_rsp_rehabitate(
               mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE);
        if (mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
        } /* free mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] */
      } /* if mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL*/
      else
      {
        UIM_MSG_ERR_2("Sync process for request at index 0x%x for file access 0x%x, has null mmgsdi_internal_synch_uim_rpt_rsp",
                      index, req_ptr->access.file.file_enum);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    } /* if synch_process */

  } /* else mmgsdi_status == SUCCESS cmd to uim*/

  return mmgsdi_status;
} /* mmgsdi_uim_icc_rehabilitate */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_INVALIDATE

   DESCRIPTION:
     This function will invalidate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only).

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the invalidate
                              command
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_invalidate (
  const mmgsdi_set_file_status_req_type *  req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_INVALIDATE_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = MMGSDI_INVALIDATE_REQ
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_INVALIDATE_F;
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;

  mmgsdi_status             = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                              &uim_cmd_ptr->invalidate.path);


  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 (mmgsdi_request_header_type*)&req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been populated
                                    yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_1("Sending down INVALIDATE command to UIM 0x%x",
                 req_ptr->access.file.file_enum);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_uim_icc_invalidate */


/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_SEARCH_RECORD

   DESCRIPTION:
     This function package the Search Record command and send it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_search_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.
     MMGSDI_NOT_SUPPORTED    : When Search Type is not valid for ICC Card.

   SIDE EFFECTS:

----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_search_record (
  const mmgsdi_search_req_type             * req_ptr,
  boolean                                    sync_process,
  mmgsdi_search_cnf_type                  ** cnf_pptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  int32                              index            = 0;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  mmgsdi_task_enum_type              mmgsdi_task      = MMGSDI_TASK_MAX;

  UIM_MSG_HIGH_0("MMGSDI_UIM_ICC_SEARCH_RECORD");

  /*Checking of Input Parameter*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  if(sync_process)
  {
    MMGSDIUTIL_RETURN_IF_NULL(cnf_pptr);
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid conversion of uim slot in UIM ICC Search 0x%x",
                  req_ptr->request_header.slot_id );
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
    req_ptr->request_header.session_id,
    NULL,
    NULL,
    NULL,
    NULL,
    &session_info_ptr,
    NULL);
  /* If the Search request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    if (mmgsdi_file_is_access_outside_adf(&req_ptr->access) == FALSE)
    {
      UIM_MSG_ERR_0("EF/DF access not allowed for Card Session");
      return MMGSDI_ERROR;
    }
  }
  else if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }


  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    MMGSDI_SEARCH_REQ,
                    (void*)req_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    UIM_MSG_ERR_0("Unable to allocate memory to extra param in UIM ICC Search");
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }
  /*-------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  --------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Memory Allcoation for uim cmd is failed in UIM ICC Search");
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);

   /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
 
  /*-------------------------------------------------
  hdr                            : Command header
   cmd_hdr                       : command header
    command                      : The command
    status                       : Status of the R-UIM
    options                      : Indicate command handling options
    protocol                     : Protocol of the command
    slot                         : Slot to execute the command
    uim_rpt_type                 : Pointer to Function to send report
    cmd_count                    :
    user_data                    : User defined data
    channel                      :
  item                           : EF to be searched
  srch_pattern[UIM_MAX_CHARS ]   : Search string
  num_bytes                      : Number of bytes in search pattern
  seek_type                      : Type 1 or Type 2 Seek
  mode                           : Search direction for seek
  aid                            : AID for USIM
  srch_mode                      : Search Type for UICC Search
  rec_number                     : Record Number for UICC Search
  srch_dir                       : Search Direction for UICC Search
  offset_type                    : offset as absolute position or character
                                   For Enhance UICC Search
  offset                         : One Byte Value for UICC Search
  srch_rec_mode_type             : Search Direction for UICC Search
 ----------------------------------------------------*/
  uim_cmd_ptr->hdr.command             = UIM_SEEK_F;
  uim_cmd_ptr->hdr.protocol            = UIM_GSM;
  uim_cmd_ptr->hdr.slot                = uim_slot;
  uim_cmd_ptr->hdr.channel             = UIM_CHANNEL0;

  /* Updating seek_type */
  uim_cmd_ptr->seek.seek_type          = UIM_SEEK_TYPE_2;

  UIM_MSG_HIGH_1("ICC SEEK DIRECTION: 0x%x", req_ptr->seek_direction);

  /* Updating mode */
  switch(req_ptr->seek_direction)
  {
    case MMGSDI_SEEK_BEGINNING_FORWARDS:
      uim_cmd_ptr->seek.mode           = UIM_SEEK_BEGINNING_FORWARDS;
      break;

    case MMGSDI_SEEK_END_BACKWARDS:
      uim_cmd_ptr->seek.mode           = UIM_SEEK_END_BACKWARDS;
      break;

    default:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      UIM_MSG_HIGH_1("MMGSDI SEEK DIRECTION IS NOT SUPPORTED in UIM ICC Search  0x%x",
                     req_ptr->seek_direction);
      return MMGSDI_INCORRECT_PARAMS;
  }/* end of switch(req_ptr->seek_direction)*/

  /* Updating rec_number: Not useful for seek command */
  uim_cmd_ptr->seek.rec_number         = 0x00;

  /* Updating srch_mode: Not useful for seek command */
  uim_cmd_ptr->seek.srch_mode          = UIM_NO_SUCH_SRCH_MODE;

  /* Updating srch_dir: Not useful for seek command */
  uim_cmd_ptr->seek.srch_dir           = UIM_SRCH_FORWARD;

  /* Updating offset_type: Not used for seek command */
  uim_cmd_ptr->seek.offset_type        = UIM_SRCH_ABSOLUTE;

  /*Updating offset: Not used for seek command */
  uim_cmd_ptr->seek.offset             = 0x00;

  /* Updating srch_rec_mode_type: Not used for seek command */
  uim_cmd_ptr->seek.srch_rec_mode_type = UIM_SRCH_P1_FORWARD;

  /* Updating num_bytes: Number of bytes in search pattern */
  if (req_ptr->data.data_len > MMGSDI_MAX_SEEK_PATTERN_LENGTH )
  {
    uim_cmd_ptr->seek.num_bytes        = MMGSDI_MAX_SEEK_PATTERN_LENGTH;
    UIM_MSG_HIGH_1("Change seek data length 0x%x to MMGSDI_MAX_SEEK_PATTERN_LENGTH[0x10]",
                   req_ptr->data.data_len);
    /* Maximum Length is bound to 16 byte */

  }
  else
  {
    uim_cmd_ptr->seek.num_bytes        = (byte)req_ptr->data.data_len;
  }

  /* Update srch_pattern[UIM_MAX_CHARS ]; Search string */
  mmgsdi_memscpy((byte *)uim_cmd_ptr->seek.srch_pattern,
                  sizeof(uim_cmd_ptr->seek.srch_pattern),
                  (byte *)req_ptr->data.data_ptr,
                  int32touint32(uim_cmd_ptr->seek.num_bytes));

  /* Print Search Pattern Length and Pattern */
  UIM_MSG_HIGH_1(" ICC SEARCH PATTERN LENGTH: 0x%x", uim_cmd_ptr->seek.num_bytes);
  mmgsdi_util_print_data_to_diag(" UICC SEARCH PATTERN:0x",
                                   uim_cmd_ptr->seek.srch_pattern,
                                   uim_cmd_ptr->seek.num_bytes);

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->seek.path);


  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    UIM_MSG_ERR_0("Convesion to UIM path is failed in UIM ICC Search ");

    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }
  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    UIM_MSG_ERR_0(" Finding free index in client request table is failed in UIM ICC Search");
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }/* End of if (mmgsdi_status != MMGSDI_SUCCESS)*/

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    uim_cmd_ptr,
                    extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been
                                    populated yet)
    3) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);

    UIM_MSG_ERR_0("Populating information from client request table is failed in UIM ICC Search");
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  /*Printing File Information */
  mmgsdi_util_print_file_information(req_ptr->access,FALSE);
  UIM_MSG_HIGH_0("Sending down seek command to UIM");
  if(sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) No need to release extra param pointer, It is already removed thru client Table
    3) No need to release uim cmd pointer, It is already removed thru client Table
  ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    UIM_MSG_ERR_0("Sending ICC Search Record command to UIM is failed");
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      mmgsdi_task = mmgsdi_util_get_current_task();
      if (mmgsdi_task != MMGSDI_TASK_MAX &&
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data_index = index;
        mmgsdi_status = mmgsdi_common_rsp_search(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE, MMGSDI_ICC);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
      }
      else
      {
        UIM_MSG_ERR_2("Sync process for request at index 0x%x for file access 0x%x, has null mmgsdi_internal_synch_uim_rpt_rsp",
                      index, req_ptr->access.file.file_enum);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  return mmgsdi_status;
}/* end of function mmgsdi_uim_icc_search_record */


/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_RUN_GSM_ALGO

   DESCRIPTION:
     This function package the Run GSM algorithm command and send it to UIM
     for further processing

   DEPENDENCIES:
     None

   LIMITATIONS:
     This function can be used only in case of ICC Card

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.

   SIDE EFFECTS:
     None
----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_run_gsm_algo (
  const mmgsdi_session_run_gsm_algo_req_type       * req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS || uim_slot == UIM_SLOT_NONE)
  {
    UIM_MSG_ERR_1("RUN_GSM_ALGO_REQ - Invalid slot id: 0x%x",
                  req_ptr->request_header.slot_id);
    return mmgsdi_status;
  }

  if (req_ptr->rand_val.data_len != MMGSDI_RAND_LEN ||
      req_ptr->rand_val.data_ptr == NULL)
  {
     UIM_MSG_ERR_2("RUN_GSM_ALGO_REQ - Invalid RAND length: 0x%x or pointer: 0x%x",
                   req_ptr->rand_val.data_len, req_ptr->rand_val.data_ptr);
     return MMGSDI_INCORRECT_PARAMS;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_SESSION_RUN_GSM_ALGO_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr,
                                     sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_ENABLE_CHV_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) pin data len and data
    5) user data: the req_ptr
    6) aid
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_RUN_GSM_ALGO_F;
  uim_cmd_ptr->hdr.protocol        = UIM_GSM;
  uim_cmd_ptr->hdr.slot            = uim_slot;

  mmgsdi_memscpy(uim_cmd_ptr->run_gsm.rand,
                 sizeof(uim_cmd_ptr->run_gsm.rand),
                 req_ptr->rand_val.data_ptr,
                 int32touint32(req_ptr->rand_val.data_len));

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
  /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);
  UIM_MSG_HIGH_0("Sending down RUN GSM ALGORITHM command to UIM ");

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_uim_icc_run_gsm_algo */

