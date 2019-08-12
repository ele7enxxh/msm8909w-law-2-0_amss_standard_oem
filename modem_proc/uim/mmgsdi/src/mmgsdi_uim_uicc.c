/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   U I M   U I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC protocol processing support for MMGSDI
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uim_uicc.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/16   bcho    F3 frequency reduction by conditional logic
06/07/16   ar      Removing string prints from APDU and AUTH
05/24/16   ar      Review of macros used by MMGSDI
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/17/16   tkl     F3 log prints cleanup
05/17/16   ar      Fix read/write of cache files when cache is not_init
05/16/16   ar      F3 reduction
05/12/16   lm      Remove excessive logging
05/09/16   vdc     Revert populating channel id before sending APDU request
01/07/16   kv      UIM PSM handling updates
12/25/15   kk      Send Card and session events based on SFS data in PSM mode
09/25/15   vr      Added support for ERA-GLONASS
08/10/15   vdc     Retry read request for non spec compliant cards
07/14/15   ar      Cache complete data for req's with partial read/write data
06/18/15   bcho    Close channel if session is activated on another slot
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
12/03/14   vdc     Populate channel id before sending the APDU request
10/23/14   yt      Updates to session and app management
09/09/14   kk      Enhancements in NV handling
08/29/14   hh      Fix compliation warnings
08/29/14   yt      Reject WRITE request if data length does not match file size
08/27/14   yt      Support for restricted SIM access via MMGSDI session APIs
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/25/14   vv      Added support to indicate app is initialized in recovery
07/15/14   yt      Support for NV refresh
06/23/14   yt      Send STATUS before app deactivation
06/03/14   tl      Remove additional parameter variables
05/07/14   yt      Explicitly set session control parameter for SELECT AID
04/29/14   nmb     Re-enable AT+CRSM after directory maintenance changes
04/24/14   am      Alloc extra params only if file info is available
04/22/14   df      Block IMS auth on non-IMS apps
03/13/14   av      Enhance MMGSDI attributes cache
02/25/14   yt      Support for selection mode in SELECT AID request
02/10/14   ar      Replace session checks with utility functions
12/05/13   tkl     SFI Support and decouple uim file enums
01/21/14   yt      Remove duplicate code
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/16/13   yt      Support for ISIM AUTH in HTTP digest security context
12/11/13   df      Remove unused memory paging feature
10/04/13   spo     F3 message reduction
09/20/13   yt      Remove duplicate code
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/07/13   tl      Add CDMA AKA Authenticate functionality
08/05/13   ar      Corrected typo in log message
07/23/13   df      Correcting comments
06/24/13   vdc     Added support for triple SIM
06/18/13   vv      Fix copying the entire path
06/06/13   yt      Add check for index
05/16/13   vdc     Replace memcpy with safer version memscpy
04/16/13   tl      Clean up of radio access features from MMGSDI code
03/04/13   vv      Allow EFs outside ADF to be accessed before app. selection
02/07/13   yt      Support for activation/deactivation of EFs
01/04/13   tl      Free data for write transparent
12/26/12   av      Do not allow writing to file that is being refreshed
12/12/12   at      Fixed critical KW warnings
12/05/12   vv      Added support for parallel processing in MMGSDI
10/01/12   abg     Removed Problem determining protocol type F3 message
09/11/12   tl      Fix KW errors
09/03/12   bcho    mmgsdi_uim_uicc_search_record modified for sync operation
08/21/12   av      Send down STORE_ESN_MEID_ME only if MEID is available
03/30/12   av      Replaced feature flags with NVITEM
03/29/12   shr     Enhancements to file access request methods, added
                   support for non-prov. app selection using zero length and
                   partial AID, support for Send APDU extension API
03/20/12   yt      Add request type for session_open_with_select_rsp
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   shr     Legacy GSDI removal updates
10/13/11   shr     Adding support for full recovery
08/18/11   shr     Do not send AID to UIM as part of file access request
                   only when the file requested is outside of all ADFs
08/01/11   vs      Added support for session open with select response
06/08/11   nmb     Corrected ISIM reference control number
05/20/11   yt      Added support for INCREASE request
05/17/11   nmb     Fixed lint error
05/17/11   shr     Updates for Thread safety
05/13/11   nmb     Added support for ISIM GBA Authentication
03/22/11   ea      Remove check for AID in EF-DIR
02/01/11   ssr     Add extra_param_ptr NULL check for pin request
12/06/10   yt      Klocwork fix
11/05/10   yt      Klocwork fixes
10/14/10   yt      Updated handling of client req table entries in case of error
09/27/10   yb      Added support to send aid data in status command to uim
08/25/10   yt      Added condition for MF file type in SELECT request
06/24/10   yt      Klocwork fixes
05/25/10   shr     Clean-up file access operations on Card Sessions
05/21/10   shr     Fixed KW error
05/21/10   shr     Reverted changes to allow all file accesses before
                   provisioning
05/12/10   shr     File access operations need to be allowed on Card Sessions
                   irrespective of the provisioning status
03/24/10   kk      Fixed close channel handling
03/05/10   mib     Fixed compilation error
02/17/10   shr     Fixed Session activate/deactivate handling
01/15/10   shr     Fixed incorrect access of freed memory locations
12/18/09   nb      Moved Dual Slot Changes
12/16/09   rn      DF enum access support for mmgsdi_uim_uicc_select
12/15/09   ssr     Add check for CSIM APPS for cdma security commands
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
12/07/09   js      Fixed lint error
12/02/09   js      Fixed session open issue for non-prov type sessions with
                   exclusive channel
10/21/09   kk      Fixed klocwork error - Possible Memory leak
09/26/09   kp      ZI memory reduction changes
09/08/09   rn      Fixed klocwork errors
07/27/09   kk      Fixed Lint Error - Boolean within 'if' always evaluates
                   to False
04/29/09   js      Fixed compiler warning
12/17/08   nb      Common Modem Interface
09/17/08   ssr     Fixed memory free error from mmgsdi_uim_uicc_search_record
22/08/08   ssr     Removing compilation warning from mmgsdi_uim_uicc_search_record.
08/08/08   ssr     Add more detailed messages for dispalyed in the log for
                   mmgsdi uicc search
07/31/08   ssr     Update search for blocking the search directions belonging
                   to Record Pointer.
07/16/08   jk      Fixed compile errors
06/12/08   ssr     Support of MMGSDI Search functionality
06/20/08   sp      Add data offset messages for read
06/04/08   tml     Fixed warning on LTK
03/03/08   sun     Added support for OMA Bcast
08/14/07   sun     Added MUK_DEL Context
07/30/07   sun     Return Error for invalid context
07/27/07   sun     Fixed passing of pointer
07/02/07   sun     Fixed USIM Authenticate and implemented odd ins class
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
03/07/07   sun     Added support for Disable with Replacement
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access.  Add
                   support for cyclic and linear fixed write
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
10/26/06   tml     Get Key ref from client ID
08/14/06   tml     Allow select AID to go through on non default channel
08/07/06   sun     Store the PIN on any pin operation
08/03/06   tml     use channel from the client table for pin function
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/08/06   tml     Fixed featurization
04/18/06   nk      Featurized code
02/14/06   nk      Merged for ARR caching and by path changes
01/17/05   tml     MMGSDI cleanup
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/30/05   jar     Fix for sending wrong pointer to malloc function
08/30/05   jar     Added support for UICC Status Commands
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_session.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_common_rsp.h"
#include "mmgsdi_file.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SELECT

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

     MMGSDI_DF_ENUM_ACCESS: Will get information on the characteristics of
                            DFs.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of UICC. This function is also
     limited to the use of EFs & DFs.


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
mmgsdi_return_enum_type mmgsdi_uim_uicc_select (
  mmgsdi_get_file_attr_req_type             *req_ptr,
  boolean                                    sync_process,
  mmgsdi_get_file_attr_cnf_type            **cnf_pptr,
  boolean                                    extra_param_req
)
{
  uim_cmd_type                      * uim_cmd_ptr                    = NULL;
  uim_slot_type                       uim_slot                       = UIM_SLOT_NONE;
  mmgsdi_return_enum_type             mmgsdi_status                  = MMGSDI_SUCCESS;
  uim_dir_type                        directory                      = 0;
  int32                               index                          = 0;
  mmgsdi_channel_id_type              channel_id                     = 0;
  mmgsdi_client_req_extra_info_type * extra_param_ptr                = NULL;
  uim_channel_type                    uim_channel_id                 = UIM_MAX_CHANNELS;
  mmgsdi_static_data_type           * aid_ptr                        = NULL;
  mmgsdi_int_app_info_type          * app_info_ptr                   = NULL;
  mmgsdi_uim_report_rsp_type        * internal_synch_uim_rpt_rsp_ptr = NULL;
  mmgsdi_session_info_type          * session_info_ptr               = NULL;
  boolean                             card_session                   = FALSE;
  mmgsdi_task_enum_type               mmgsdi_task                    = MMGSDI_TASK_MAX;

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

  switch(req_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
    case MMGSDI_DF_ENUM_ACCESS:
    case MMGSDI_BY_APP_ID_ACCESS:
    case MMGSDI_BY_PATH_ACCESS:
      break;
    default:
      UIM_MSG_ERR_1("INVALID ACCESS METHOD %x", req_ptr->access.access_method);
      return MMGSDI_INCORRECT_PARAMS;
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
                    &channel_id,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    /* If it is an EF/DF access outside of any ADF, it is not an error */
    if (mmgsdi_file_is_access_outside_adf(&req_ptr->access))
    {
      /* Card session accesses always go on channel 0 */
      uim_channel_id = UIM_CHANNEL0;
      card_session = TRUE;
    }
    else
    {
      UIM_MSG_ERR_0("EF/DF access not allowed for Card Session");
      return MMGSDI_ERROR;
    }
  }
  else if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    /* For non-prov. Sessions opened through Select with MF, no app shall be
       tied to the Session. In such cases, allow requests for EF/DF outside
       of ADFs on the channel corresponding to the Session */
    if((session_info_ptr != NULL) &&
       mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
    {
      if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
      {
        UIM_MSG_ERR_0("Channel info needs to be valid for non-prov. session");
        return MMGSDI_ERROR;
      }

      /* Block reads for files under ADF */
      if ((req_ptr->access.access_method != MMGSDI_BY_APP_ID_ACCESS) &&
          !mmgsdi_file_is_access_outside_adf(
             &req_ptr->access))
      {
        return MMGSDI_ERROR;
      }

      /* Set uim_channel_id back to UIM_MAX_CHANNELS, so that the command is
         sent on the actual logical channel corresponding to the Session */
      uim_channel_id = UIM_MAX_CHANNELS;
    }
    /* check if it is EF under MF, if so, not an error */
    else if (mmgsdi_file_is_ef_under_mf(&req_ptr->access))
    {
      /* All DFs under MF can be mapped to channel 0, revert to default */
      uim_channel_id = UIM_CHANNEL0;
    }
    else if (req_ptr->access.access_method == MMGSDI_BY_APP_ID_ACCESS)
    {
      /* If this is an APP that is being selected do nothing */
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }

  if (uim_channel_id == UIM_MAX_CHANNELS)
  {
    /* Session can be disconnected from the channel due to app deactivation.
       Try to find the channel id using AID and slot id combination. */
    if (mmgsdi_status != MMGSDI_SUCCESS &&
        req_ptr->access.access_method == MMGSDI_BY_APP_ID_ACCESS)
    {
      mmgsdi_aid_type  app_data;
      memset(&app_data, 0x0, sizeof(mmgsdi_aid_type));

      mmgsdi_memscpy(&app_data.aid, sizeof(mmgsdi_static_data_type),
                     &req_ptr->access.file.app_id, sizeof(mmgsdi_static_data_type));

      /* Ignore error in finding the channel id as it is possible that channel id
         was found above but the app info was not found in mmgsdi_app_pin_table.
         In such a case (e.g., non-prov session open on an AID not present in
         EF-DIR) mmgsdi_util_get_session_app_info would return the channel id
         and error status as the app info could not be found. */
      (void)mmgsdi_util_check_app_termination_skipped(req_ptr->request_header.slot_id,
                                                      &app_data,
                                                      &channel_id);
    }

    /* channel has not been assigned, this means we must have a good
       session_id which returns a valid channel ID */
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id,
                                                    &uim_channel_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /* AID data not required for file access on a card session or
     for an EF under MF or for a DF under MF */
  if ((!card_session) &&
      (!mmgsdi_file_is_ef_under_mf(&req_ptr->access)) &&
      (!mmgsdi_file_is_df_under_mf(&req_ptr->access)))
  {
    /* Assign the AID */
    if(req_ptr->access.access_method != MMGSDI_BY_APP_ID_ACCESS)
    {
      if(app_info_ptr != NULL)
      {
        aid_ptr = &app_info_ptr->app_data.aid;

        if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
            (aid_ptr->data_len < 0))
        {
          UIM_MSG_ERR_1("Invalid UIM ISIM Auth AID size 0x%x",
                        aid_ptr->data_len);
          return MMGSDI_ERROR;
        }
      }
    }
    else
    {
      /* a select AID request is received */
      aid_ptr = &req_ptr->access.file.app_id;
    }
  }

  /* Convert to UIM directory type */
  mmgsdi_status = mmgsdi_file_determine_uim_dir_type(&(req_ptr->access),
                                                     &directory);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(extra_param_req == TRUE)
  {
    /* Allocate and populate extra parameter */
    mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
            MMGSDI_GET_FILE_ATTR_REQ, (void*)req_ptr, &extra_param_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
    {
      return mmgsdi_status;
    }
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
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command      = UIM_SELECT_F;
  uim_cmd_ptr->hdr.protocol     = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot         = uim_slot;
  uim_cmd_ptr->hdr.channel      = uim_channel_id;
  uim_cmd_ptr->select.dir       = directory;

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->select.path);


  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  if (req_ptr->access.access_method == MMGSDI_BY_APP_ID_ACCESS)
  {
    uim_cmd_ptr->select.file_type = UIM_ADF;
    switch (req_ptr->aid_select_mode)
    {
      case MMGSDI_SELECT_MODE_FIRST_OR_ONLY:
      case MMGSDI_SELECT_MODE_UNKNOWN:
        uim_cmd_ptr->select.adf_selection = UIM_FIRST_OR_ONLY_OCCURRENCE;
        break;
      case MMGSDI_SELECT_MODE_LAST:
        uim_cmd_ptr->select.adf_selection = UIM_LAST_OCCURRENCE;
        break;
      case MMGSDI_SELECT_MODE_NEXT:
        uim_cmd_ptr->select.adf_selection = UIM_NEXT_OCCURRENCE;
        break;
      case MMGSDI_SELECT_MODE_PREVIOUS:
        uim_cmd_ptr->select.adf_selection = UIM_PREVIOUS_OCCURRENCE;
        break;
    }
  }
  else if (req_ptr->access.access_method == MMGSDI_DF_ENUM_ACCESS)
  {
    if (req_ptr->access.file.df_enum == MMGSDI_MF_DIR)
    {
      uim_cmd_ptr->select.file_type = UIM_MF;
    }
    else
    {
      uim_cmd_ptr->select.file_type = UIM_DF;
    }
  }
  else
  {
    uim_cmd_ptr->select.file_type = UIM_EF;
  }

  if(aid_ptr != NULL)
  {
    mmgsdi_memscpy(uim_cmd_ptr->select.aid.data,
                   sizeof(uim_cmd_ptr->select.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->select.aid.aid_length = int32touint8(aid_ptr->data_len);
  }

  if (req_ptr->activate_aid)
  {
    if (req_ptr->request_header.orig_request_type == MMGSDI_SESSION_OPEN_REQ ||
        req_ptr->request_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ ||
        req_ptr->request_header.orig_request_type == MMGSDI_SESSION_SELECT_AID_REQ)
    {
      switch (req_ptr->select_rsp_type)
      {
        case MMGSDI_SELECT_RSP_NO_DATA:
          uim_cmd_ptr->select.session = UIM_ACTIVATION_WITH_NO_DATA;
          break;
        case MMGSDI_SELECT_RSP_FMD:
          uim_cmd_ptr->select.session = UIM_ACTIVATION_WITH_FMD;
          break;
        case MMGSDI_SELECT_RSP_FCI:
          uim_cmd_ptr->select.session = UIM_ACTIVATION_WITH_FCI;
          break;
        case MMGSDI_SELECT_RSP_FCI_WITH_INTERFACES:
          uim_cmd_ptr->select.session = UIM_ACTIVATION_WITH_FCI_INTERFACES;
          break;
        case MMGSDI_SELECT_RSP_FCP:
        default:
          uim_cmd_ptr->select.session = UIM_ACTIVATION_OR_RESET;
          break;
      }
    }
    else
    {
      uim_cmd_ptr->select.session = UIM_ACTIVATION_OR_RESET;
    }
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
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been
                                    populated yet)
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

  UIM_MSG_HIGH_1("Sending down SELECT command to UIM 0x%x",
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
        /* Copy the UIM response */
        mmgsdi_status = mmgsdi_util_alloc_copy_uim_rsp(
                          &internal_synch_uim_rpt_rsp_ptr,
                          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);

        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          mmgsdi_util_free_client_request_table_index(index);
          if (internal_synch_uim_rpt_rsp_ptr != NULL)
          {
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(internal_synch_uim_rpt_rsp_ptr->rsp_data.data_ptr);
            MMGSDIUTIL_TMC_MEM_FREE(internal_synch_uim_rpt_rsp_ptr);
          }
          UIM_MSG_ERR_0("Could not copy UIM Response");
          return mmgsdi_status;
        }

        internal_synch_uim_rpt_rsp_ptr->rsp_data_index = index;
        mmgsdi_status = mmgsdi_uicc_rsp_get_file_attr(
               internal_synch_uim_rpt_rsp_ptr, cnf_pptr, TRUE);

        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          internal_synch_uim_rpt_rsp_ptr->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(internal_synch_uim_rpt_rsp_ptr);
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
} /* mmgsdi_uim_uicc_select */

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_WRITE_TRANSPARENT

   DESCRIPTION:
     This function package the Write Transparent command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_write_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_write_transparent (
  const mmgsdi_write_req_type             * req_ptr,
  boolean                                   sync_process,
  mmgsdi_write_cnf_type                   **cnf_pptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_channel_id_type             channel_id       = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_channel_type                   uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  boolean                            card_session     = FALSE;
  mmgsdi_task_enum_type              mmgsdi_task      = MMGSDI_TASK_MAX;
  mmgsdi_len_type                    mmgsdi_write_len = 0;

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
                    &channel_id,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  /* If the Write request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    /* Irrespective of app selection, allow write request for
       files outside ADF */

    /* Block write for files under ADF */
    if (!mmgsdi_file_is_access_outside_adf(
           &req_ptr->access))
    {
      UIM_MSG_ERR_0("Cannot write to file in ADF, on card session");
      return MMGSDI_ERROR;
    }
    else
    {
      /* Card Session accesses always go on channel 0 */
      uim_channel_id = UIM_CHANNEL0;
      card_session = TRUE;
    }
  }
  else if (mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    /* For non-prov. Sessions opened through Select with MF, no app shall be
       tied to the Session. In such cases, allow requests for EFs outside
       of ADFs on the channel corresponding to the Session */
    if((session_info_ptr != NULL) &&
       mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
    {
      if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
      {
        UIM_MSG_ERR_0("Channel info needs to be valid for non-prov. session");
        return MMGSDI_ERROR;
      }

      /* Block reads for files under ADF */
      if (!mmgsdi_file_is_access_outside_adf(
             &req_ptr->access))
      {
        return MMGSDI_ERROR;
      }

      /* Set uim_channel_id back to UIM_MAX_CHANNELS, so that the command is
         sent on the actual logical channel corresponding to the Session */
      uim_channel_id = UIM_MAX_CHANNELS;
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }

  mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* AID data not required for file access on a Card Session or
     for an EF under MF */
  if((!card_session) &&
     (!mmgsdi_file_is_ef_under_mf(&req_ptr->access)))
  {
    if(app_info_ptr != NULL)
    {
      aid_ptr = &app_info_ptr->app_data.aid;

      if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
          (aid_ptr->data_len < 0))
      {
        UIM_MSG_ERR_1("Invalid UIM ISIM Auth AID size 0x%x", aid_ptr->data_len);
        return MMGSDI_ERROR;
      }
    }
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
    1) command = UIM_WRITE_BIN_CMD_F
    2) protocol = UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) item
    5) path
    6) offset
    7) data len and data
    8) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_WRITE_BIN_CMD_F;
  uim_cmd_ptr->hdr.protocol        = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->write_bin.path);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  (void)mmgsdi_file_set_uim_sfi(req_ptr->request_header.session_id,
                                req_ptr->request_header.slot_id,
                                &(req_ptr->access),
                                &uim_cmd_ptr->write_bin.sfi);

  /* Assign uim_cmd_ptr's data len and data buffer info */
  uim_cmd_ptr->write_bin.offset    = int32touint16(req_ptr->offset);
  uim_cmd_ptr->write_bin.len       = int32touint16(req_ptr->data.data_len);
  uim_cmd_ptr->write_bin.data      = extra_param_ptr->write_data.data.data_ptr;

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

  /* aid */
  if(aid_ptr != NULL)
  {
    mmgsdi_memscpy(uim_cmd_ptr->write_bin.aid.data,
                   sizeof(uim_cmd_ptr->write_bin.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));

    uim_cmd_ptr->write_bin.aid.aid_length = int32touint8(aid_ptr->data_len);
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
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE, MMGSDI_UICC);
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

} /* mmgsdi_uim_uicc_write_transparent */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_WRITE_RECORD

   DESCRIPTION:
     This function package the Write Record command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_write_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_write_record (
  const mmgsdi_write_req_type             * req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_channel_id_type             channel_id       = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_channel_type                   uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  boolean                            card_session     = FALSE;
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
                    &channel_id,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  /* If the Write request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    /* Irrespective of app selection, allow write request for
       files outside ADF */

    /* Block write for files under ADF */
    if (!mmgsdi_file_is_access_outside_adf(
           &req_ptr->access))
    {
      UIM_MSG_ERR_0("Cannot write to file in ADF, on card session");
      return MMGSDI_ERROR;
    }
    else
    {
      /* Card Session accesses always go on channel 0 */
      uim_channel_id = UIM_CHANNEL0;
      card_session = TRUE;
    }
  }
  else if (mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    /* For non-prov. Sessions opened through Select with MF, no app shall be
       tied to the Session. In such cases, allow requests for EFs outside
       of ADFs on the channel corresponding to the Session */
    if((session_info_ptr != NULL) &&
       mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
    {
      if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
      {
        UIM_MSG_ERR_0("Channel info needs to be valid for non-prov. session");
        return MMGSDI_ERROR;
      }

      /* Block reads for files under ADF */
      if (!mmgsdi_file_is_access_outside_adf(
             &req_ptr->access))
      {
        return MMGSDI_ERROR;
      }

      /* Set uim_channel_id back to UIM_MAX_CHANNELS, so that the command is
         sent on the actual logical channel corresponding to the Session */
      uim_channel_id = UIM_MAX_CHANNELS;
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }

  mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* AID data not required for file access on a card session or
     for an EF under MF */
  if((!card_session) &&
     (!mmgsdi_file_is_ef_under_mf(&req_ptr->access)))
  {
    if(app_info_ptr != NULL)
    {
      aid_ptr = &app_info_ptr->app_data.aid;

      if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
          (aid_ptr->data_len < 0))
      {
        UIM_MSG_ERR_1("Invalid UIM ISIM Auth AID size 0x%x", aid_ptr->data_len);
        return MMGSDI_ERROR;
      }
    }
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
    4) path
    5) record access mode
    6) record number
    7) data len and data
    8) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_WRITE_REC_CMD_F;
  uim_cmd_ptr->hdr.protocol        = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;
  uim_cmd_ptr->write_rec.rec_num   = int32touint8(req_ptr->rec_num);
  if (req_ptr->file_type == MMGSDI_CYCLIC_FILE)
  {
    uim_cmd_ptr->write_rec.recmode   = UIM_PREVIOUS;
  }
  else
  {
    uim_cmd_ptr->write_rec.recmode   = UIM_ABSOLUTE;
  }

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->write_rec.path);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->write_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  (void)mmgsdi_file_set_uim_sfi(req_ptr->request_header.session_id,
                                req_ptr->request_header.slot_id,
                                &(req_ptr->access),
                                &uim_cmd_ptr->write_rec.sfi);

  /* Assign uim_cmd_ptr's data len and data buffer info */
  uim_cmd_ptr->write_rec.len       = int32touint16(mmgsdi_write_len);
  uim_cmd_ptr->write_rec.data      = extra_param_ptr->write_data.data.data_ptr;

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

  /* aid */
  if (aid_ptr != NULL )
  {
    mmgsdi_memscpy(uim_cmd_ptr->write_rec.aid.data,
                   sizeof(uim_cmd_ptr->write_rec.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->write_rec.aid.aid_length = int32touint8(aid_ptr->data_len);
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

} /* mmgsdi_uim_uicc_write_record */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_READ_TRANSPARENT

   DESCRIPTION:
     This function package the Read Transparent command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_read_transparent (
  const mmgsdi_read_req_type              * req_ptr,
  boolean                                   sync_process,
  mmgsdi_read_cnf_type                    **cnf_pptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_channel_id_type             channel_id       = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_channel_type                   uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  boolean                            card_session     = FALSE;
  mmgsdi_task_enum_type              mmgsdi_task      = MMGSDI_TASK_MAX;
  mmgsdi_len_type                    mmgsdi_read_len  = 0;
  mmgsdi_offset_type                 mmgsdi_offset    = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(sync_process)
  {
    MMGSDIUTIL_RETURN_IF_NULL(cnf_pptr);
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    &channel_id,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  /* If the Read request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    /* Irrespective of app selection, allow read request for
       files outside ADF */

    /* Block reads for files under ADF */
    if (!mmgsdi_file_is_access_outside_adf(
           &req_ptr->access))
    {
      UIM_MSG_ERR_0("Cannot read from file in ADF, on card session");
      return MMGSDI_ERROR;
    }
    else
    {
      /* Card Session accesses always go on channel 0 */
      uim_channel_id = UIM_CHANNEL0;
      card_session = TRUE;
    }
  }
  else if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    /* For non-prov. Sessions opened through Select with MF, no app shall be
       tied to the Session. In such cases, allow requests for EFs outside
       of ADFs on the channel corresponding to the Session */
    if((session_info_ptr != NULL) &&
       mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
    {
      if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
      {
        UIM_MSG_ERR_0("Channel info needs to be valid for non-prov. session");
        return MMGSDI_ERROR;
      }

      /* Block reads for files under ADF */
      if (!mmgsdi_file_is_access_outside_adf(
             &req_ptr->access))
      {
        return MMGSDI_ERROR;
      }

      /* Set uim_channel_id back to UIM_MAX_CHANNELS, so that the command is
         sent on the actual logical channel corresponding to the Session */
      uim_channel_id = UIM_MAX_CHANNELS;
    }
    /* check if it is EF under MF, if so, not an error */
    else if (mmgsdi_file_is_ef_under_mf(&req_ptr->access))
    {
      /* may be the first select or read during initiailzation,
         default everything to Channel 0 */
      uim_channel_id = UIM_CHANNEL0;
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }

  if (uim_channel_id == UIM_MAX_CHANNELS)
  {
    /* channel has not been assigned, this means we must have a good
       session_id which returns a valid channel ID */
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id,
                                                    &uim_channel_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /* AID data not required for file access on a Card Session or
     for an EF under MF */
  if ((!card_session) &&
      (!mmgsdi_file_is_ef_under_mf(&req_ptr->access)))
  {
    if(app_info_ptr != NULL)
    {
      aid_ptr = &app_info_ptr->app_data.aid;

      if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
          (aid_ptr->data_len < 0))
      {
        UIM_MSG_ERR_1("Invalid UIM AID size 0x%x", aid_ptr->data_len);
        return MMGSDI_ERROR;
      }
    }
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Before sending write, make sure we are not trying to
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
                    req_ptr->select_before_read);

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
    4) path
    5) offset
    6) record access mode
    7) offset: based on file type, either record number or offset will be sent
    8) data len
    9) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_READ_BIN_CMD_F;
  uim_cmd_ptr->hdr.protocol        = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->read_bin.path);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  (void)mmgsdi_file_set_uim_sfi(req_ptr->request_header.session_id,
                                req_ptr->request_header.slot_id,
                                &(req_ptr->access),
                                &uim_cmd_ptr->read_bin.sfi);

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

  /* Populate AID if EF is not under MF */
  if (aid_ptr != NULL)
  {
    mmgsdi_memscpy(uim_cmd_ptr->read_bin.aid.data,
                   sizeof(uim_cmd_ptr->read_bin.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));

    uim_cmd_ptr->read_bin.aid.aid_length = int32touint8(aid_ptr->data_len);
  }

  UIM_MSG_HIGH_3("Sending down READ command to UIM file 0x%x with offset 0x%x and length 0x%x",
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
        mmgsdi_status = mmgsdi_uicc_rsp_read(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE);
        /* It is possible that the global mmgsdi_internal_synch_uim_rpt_rsp
           is freed up already inside mmgsdi_uicc_rsp_read above...
           That would happen incase of a recursive call to this function as
           a result of read retry with explicit SELECT... so, free
           it only if it is not freed already */
        if (mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
          MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
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

  return mmgsdi_status;

} /* mmgsdi_uim_uicc_read_transparent */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_READ_RECORD

   DESCRIPTION:
     This function package the Read Record command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_read_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_read_record (
  const mmgsdi_read_req_type              * req_ptr,
  boolean                                   sync_process,
  mmgsdi_read_cnf_type                    **cnf_pptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_channel_id_type             channel_id       = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_channel_type                   uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  boolean                            card_session     = FALSE;
  mmgsdi_task_enum_type              mmgsdi_task      = MMGSDI_TASK_MAX;
  mmgsdi_len_type                    mmgsdi_read_len  = 0;

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
                    &channel_id,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  /* If the Read request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    /* Irrespective of app selection, allow read request for
       files outside ADF */

    /* Block reads for files under ADF */
    if (!mmgsdi_file_is_access_outside_adf(
           &req_ptr->access))
    {
      UIM_MSG_ERR_0("Cannot read from file in ADF, on card session");
      return MMGSDI_ERROR;
    }
    else
    {
      /* Card Session accesses always go on channel 0 */
      uim_channel_id = UIM_CHANNEL0;
      card_session = TRUE;
    }
  }
  else if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    /* For non-prov. Sessions opened through Select with MF, no app shall be
       tied to the Session. In such cases, allow requests for EFs outside
       of ADFs on the channel corresponding to the Session */
    if((session_info_ptr != NULL) &&
       mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
    {
      if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
      {
        UIM_MSG_ERR_0("Channel info needs to be valid for non-prov. session");
        return MMGSDI_ERROR;
      }

      /* Block reads for files under ADF */
      if (!mmgsdi_file_is_access_outside_adf(
             &req_ptr->access))
      {
        return MMGSDI_ERROR;
      }

      /* Set uim_channel_id back to UIM_MAX_CHANNELS, so that the command is
         sent on the actual logical channel corresponding to the Session */
      uim_channel_id = UIM_MAX_CHANNELS;
    }
    /* check if it is EF under MF, if so, not an error */
    else if (mmgsdi_file_is_ef_under_mf(&req_ptr->access))
    {
      /* may be the first select or read during initiailzation,
         default everything to Channel 0 */
      uim_channel_id = UIM_CHANNEL0;
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }

  if (uim_channel_id == UIM_MAX_CHANNELS)
  {
    /* channel has not been assigned, this means we must have a good
       session_id which returns a valid channel ID */
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id,
                                                    &uim_channel_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /* AID data not required for file access on a card session or
     for an EF under MF */
  if ((!card_session) &&
      (!mmgsdi_file_is_ef_under_mf(&req_ptr->access)))
  {
    if(app_info_ptr != NULL)
    {
      aid_ptr = &app_info_ptr->app_data.aid;

      if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
          (aid_ptr->data_len < 0))
      {
        UIM_MSG_ERR_1("Invalid UIM AID size 0x%x", aid_ptr->data_len);
        return MMGSDI_ERROR;
      }
    }
  }

  /* Before sending write, make sure we are not trying to
     read more than the rec len. Also, make sure we pass
     the rec len down to the drivers because per the
     agreement between mmgsdi and drivers mmgsdi must
     always send a non zero length. */
  mmgsdi_read_len = 0;
  mmgsdi_status = mmgsdi_file_check_file_info_and_update_len
                   (req_ptr->request_header.session_id,
                    req_ptr->request_header.slot_id,
                    req_ptr->access,
                    req_ptr->file_type,
                    req_ptr->offset,
                    &mmgsdi_read_len,
                    req_ptr->select_before_read);

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
    4) path
    5) record access mode
    6) record number
    7) data len
    8) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_READ_REC_CMD_F;
  uim_cmd_ptr->hdr.protocol        = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;
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

  (void)mmgsdi_file_set_uim_sfi(req_ptr->request_header.session_id,
                                req_ptr->request_header.slot_id,
                                &(req_ptr->access),
                                &uim_cmd_ptr->read_rec.sfi);

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

  /* aid */
  if (aid_ptr != NULL)
  {
    mmgsdi_memscpy(uim_cmd_ptr->read_rec.aid.data,
                   sizeof(uim_cmd_ptr->read_rec.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->read_rec.aid.aid_length = int32touint8(aid_ptr->data_len);
  }

  UIM_MSG_HIGH_2("Sending down READ command to UIM 0x%x with record no 0x%x",
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
        mmgsdi_status = mmgsdi_uicc_rsp_read(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE);
        /* It is possible that the global mmgsdi_internal_synch_uim_rpt_rsp
           is freed up already inside mmgsdi_uicc_rsp_read above...
           That would happen incase of a recursive call to this function as
           a result of read retry with explicit SELECT... so, free
           it only if it is not freed already */
        if (mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
          MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
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

  return mmgsdi_status;

} /* mmgsdi_uim_uicc_read_record */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_INCREASE

   DESCRIPTION:
     This function packages the Increase command and sends it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_uim_icc_increase should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_increase (
  const mmgsdi_increase_req_type          * req_ptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_channel_id_type             channel_id       = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_channel_type                   uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  boolean                            card_session     = FALSE;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

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
                    &channel_id,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  /* If the Write request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    /* Irrespective of app selection, allow increase request for
       files outside ADF */

    /* Block write for files under ADF */
    if (!mmgsdi_file_is_access_outside_adf(
           &req_ptr->access))
    {
      UIM_MSG_ERR_0("Cannot increment file in ADF on card session");
      return MMGSDI_ERROR;
    }
    else
    {
      /* Card Session accesses always go on channel 0 */
      uim_channel_id = UIM_CHANNEL0;
      card_session = TRUE;
    }
  }
  else if (mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    /* For non-prov. Sessions opened through Select with MF, no app shall be
       tied to the Session. In such cases, allow requests for EFs outside
       of ADFs on the channel corresponding to the Session */
    if((session_info_ptr != NULL) &&
       mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
    {
      if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
      {
        UIM_MSG_ERR_0("Channel info needs to be valid for non-prov. session");
        return MMGSDI_ERROR;
      }

      /* Block reads for files under ADF */
      if (!mmgsdi_file_is_access_outside_adf(
             &req_ptr->access))
      {
        return MMGSDI_ERROR;
      }

      /* Set uim_channel_id back to UIM_MAX_CHANNELS, so that the command is
         sent on the actual logical channel corresponding to the Session */
      uim_channel_id = UIM_MAX_CHANNELS;
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }

  mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* AID data not required for file access on a card session */
  if((!card_session) &&
     (!mmgsdi_file_is_ef_under_mf(&req_ptr->access)))
  {
    if(app_info_ptr != NULL)
    {
      aid_ptr = &app_info_ptr->app_data.aid;

      if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
          (aid_ptr->data_len < 0))
      {
        UIM_MSG_ERR_1("Invalid UIM ISIM Auth AID size 0x%x", aid_ptr->data_len);
        return MMGSDI_ERROR;
      }
    }
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
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_INC_CMD_F
    2) protocol = UIM_UICC
    3) slot
    4) path
    5) record access mode
    6) record number
    7) data len and data
    8) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command  = UIM_INC_CMD_F;
  uim_cmd_ptr->hdr.protocol = UIM_UICC;
  uim_cmd_ptr->hdr.slot     = uim_slot;
  uim_cmd_ptr->hdr.channel  = uim_channel_id;

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->inc.path);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->increase_data.data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  (void)mmgsdi_file_set_uim_sfi(req_ptr->request_header.session_id,
                                req_ptr->request_header.slot_id,
                                &(req_ptr->access),
                                &uim_cmd_ptr->inc.sfi);

  /* Assign uim_cmd_ptr's data len and data buffer info */
  uim_cmd_ptr->inc.len       = int32touint16(req_ptr->data.data_len);
  uim_cmd_ptr->inc.data      = extra_param_ptr->increase_data.data.data_ptr;

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

  /* aid */
  if (aid_ptr != NULL )
  {
    mmgsdi_memscpy(uim_cmd_ptr->inc.aid.data,
                   sizeof(uim_cmd_ptr->inc.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->inc.aid.aid_length = int32touint8(aid_ptr->data_len);
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

} /* mmgsdi_uim_uicc_increase */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_MANAGE_CHANNEL_OP

   DESCRIPTION
     This function sends MANAGE_CHANNEL_F to UIMDRV

   DEPENDENCIES:
     The function is applicable only for UICC cards

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_manage_channel_op(
  const mmgsdi_request_header_type         *req_header_ptr,
  mmgsdi_channel_id_type                    op_close_channel,
  uim_mc_action_type                        mc_action,
  mmgsdi_client_req_extra_info_type        *extra_param_ptr,
  boolean                                   sync_process,
  mmgsdi_session_deactivate_cnf_type      **cnf_pptr,
  mmgsdi_channel_id_type                   *out_channel_id_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_SUCCESS;
  uim_slot_type                       uim_slot      = UIM_SLOT_NONE;
  uim_cmd_type*                       uim_cmd_ptr   = NULL;
  int32                               index         = 0;
  mmgsdi_task_enum_type               mmgsdi_task   = MMGSDI_TASK_MAX;

  UIM_MSG_HIGH_0("MMGSDI_UIM_UICC_MANAGE_CHANNEL_OP");

  do
  {
    /* Parameter checks */
    if (NULL == req_header_ptr ||
        (!sync_process && NULL == req_header_ptr->response_cb))
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }

    /* Convert to UIM slot */
    mmgsdi_status = mmgsdi_util_convert_uim_slot(req_header_ptr->slot_id,
                                                 &uim_slot);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* Get a UIM buffer for the request */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
    if (NULL == uim_cmd_ptr)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    }
    uim_cmd_ptr->hdr.command                     = UIM_MANAGE_CHANNEL_F;
    uim_cmd_ptr->hdr.protocol                    = UIM_WCDMA;
    uim_cmd_ptr->hdr.slot                        = uim_slot;
    uim_cmd_ptr->hdr.channel                     = req_header_ptr->channel_id;
    uim_cmd_ptr->manage_channel.channel          = req_header_ptr->channel_id;
    uim_cmd_ptr->manage_channel.mc_action        = mc_action;
    uim_cmd_ptr->manage_channel.op_close_channel = op_close_channel;

    /*---------------------------------------------------------------------------
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
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      break;
    }

    mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                      index,
                      req_header_ptr,
                      (void*)uim_cmd_ptr,
                      extra_param_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      break;
    }
  }while(0);

  /* In case of errors, free extra_param_ptr before returning */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);
  UIM_MSG_HIGH_0("Sending down MANAGE CHANNEL command to UIM");
  if (!sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  else if (sync_process)
  {
    /* Synchronous internal processing, retrieve the confirmation data
       Currently only mmgsdi_uim_uicc_close_channel request should have
       sync processs
     */
    mmgsdi_task = mmgsdi_util_get_current_task();
    if (mmgsdi_task != MMGSDI_TASK_MAX &&
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
    {
      mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data_index = index;
      mmgsdi_status = mmgsdi_uicc_rsp_manage_channel_op_sync(
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, 
        out_channel_id_ptr, mc_action);

      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
    }
    else
    {
      UIM_MSG_ERR_1("Sync process for request at index 0x%x for CLOSE CHANNEL, has null mmgsdi_internal_synch_uim_rpt_rsp",
                    index);
      mmgsdi_status = MMGSDI_ERROR;
    }
    mmgsdi_util_free_client_request_table_index(index);
  }

  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
} /* mmgsdi_uim_uicc_manage_channel_op */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_MANAGE_CHANNEL_REQ

   DESCRIPTION
     This function handles MANAGE CHANNEL request

   DEPENDENCIES:
     The function is applicable only for UICC cards

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_manage_channel_req(
  const mmgsdi_manage_channel_req_type  *req_ptr
)
{
  return mmgsdi_uim_uicc_manage_channel_op(&req_ptr->request_header,
                                           req_ptr->op_close_channel,
                                           req_ptr->mc_action,
                                           NULL,
                                           FALSE,
                                           NULL,
                                           NULL);
}/* mmgsdi_uim_uicc_manage_channel_req */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_OPEN_CHANNEL

   DESCRIPTION
     This function is called from the Channel Management Open Channel
     function to build a UIM Manage Channel Request to open a new
     communication channel with the Card presently inserted.

   DEPENDENCIES:
     The function is applicable only for UICC cards. The manage channel
     command shall always be sent on channel 0.

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_open_channel(
  const mmgsdi_request_header_type *  req_header_ptr,
  mmgsdi_aid_type                  *  aid_ptr,
  boolean                             exclusive_channel,
  boolean                             select_rsp_requested,
  mmgsdi_select_rsp_enum_type         select_rsp_type
)
{
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type   *extra_param_ptr  = NULL;
  mmgsdi_session_open_extra_info_type  channel_info     = {{0}};

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  /*---------------------------------------------------------------------------
      Allocate and Populate Extra Parameter
      - Free Request Table if error in Extra Parameter
      - Free temp data holder if error
    ---------------------------------------------------------------------------*/
  if(aid_ptr != NULL)
  {
    mmgsdi_memscpy((void*)&channel_info.aid,
                   sizeof(channel_info.aid),
                   aid_ptr,
                   sizeof(mmgsdi_aid_type));
  }
  channel_info.exclusive_channel    = exclusive_channel;
  channel_info.select_rsp_requested = select_rsp_requested;
  channel_info.select_rsp_type      = select_rsp_type;

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                              req_header_ptr->orig_request_type,
                              (void*)&channel_info,
                              &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  return mmgsdi_uim_uicc_manage_channel_op(req_header_ptr,
                                           UIM_CHANNEL0,
                                           UIM_MC_OPEN,
                                           extra_param_ptr,
                                           FALSE,
                                           NULL,
                                           NULL);
} /* mmgsdi_uim_uicc_open_channel */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_CLOSE_CHANNEL

   DESCRIPTION
     This function is called from the Channel Management Close Channel
     function to build a UIM Manage Channel Request to close a new
     communication channel with the Card presently inserted.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_close_channel(
  const mmgsdi_request_header_type    * req_header_ptr,
  mmgsdi_client_req_extra_info_type   * extra_param_ptr,
  boolean                               sync_process,
  mmgsdi_session_deactivate_cnf_type ** cnf_pptr,
  const mmgsdi_aid_type               * app_data_ptr,
  mmgsdi_channel_id_type                channel_id
)
{
  mmgsdi_return_enum_type  mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_channel_id_type   op_close_channel = UIM_MAX_CHANNELS;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  /* Channel id is given explicitly in input parameter if it is not associated
     with any session */
  if(channel_id == MMGSDI_MAX_CHANNELS)
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(req_header_ptr->session_id,
                                                     NULL,
                                                     &channel_id,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     NULL);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Session can be disconnected from the channel due to app deactivation.
         Try to find the channel id using AID and slot id combination. */
      if (app_data_ptr == NULL ||
          (mmgsdi_util_check_app_termination_skipped(req_header_ptr->slot_id,
                                                    app_data_ptr,
                                                    &channel_id) == FALSE))
      {
        return mmgsdi_status;
      }
    }
  }

  if(channel_id == MMGSDI_DEFAULT_CHANNEL)
  {
    UIM_MSG_ERR_0("Can not perform close channel for default channel");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id,
                                                  &op_close_channel);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  return mmgsdi_uim_uicc_manage_channel_op(req_header_ptr,
                                           op_close_channel,
                                           UIM_MC_CLOSE,
                                           extra_param_ptr,
                                           sync_process,
                                           cnf_pptr,
                                           NULL);

  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
} /* mmgsdi_uim_uicc_close_channel */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SESSION_DEACTIVATE

   DESCRIPTION
     This function is called for Status command.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_session_deactivate(
  const mmgsdi_status_req_type        *  req_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
  mmgsdi_channel_id_type     channel_id       = 0;
  uim_channel_type           uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_int_app_info_type  *app_info_ptr     = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    &channel_id,
                    &app_info_ptr,
                    NULL,
                    NULL,
                    NULL);
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  uim_cmd_ptr->hdr.command               = UIM_STATUS_F;
  uim_cmd_ptr->hdr.protocol              = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot                  = uim_slot;
  uim_cmd_ptr->hdr.channel               = uim_channel_id;

  /* ------------------------------------------------------------------------
      Set UIM Manage Channel Command Options
    ----------------------------------------------------------------------*/
  uim_cmd_ptr->status.status                   = UIM_TERMINATION_OF_CURR_APP;
  uim_cmd_ptr->status.return_data              = UIM_RETURN_NONE;

  /*---------------------------------------------------------------------------
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
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
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

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
} /* mmgsdi_uim_uicc_session_deactivate */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_ISIM_AUTH

   DESCRIPTION
     This function is called for ISIM Authentication request.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_isim_auth(
  const mmgsdi_auth_req_type        *  req_ptr
)
{
  return mmgsdi_uim_uicc_auth(req_ptr);
} /* mmgsdi_uim_uicc_isim_auth */


/*===========================================================================
FUNCTION:  MMGSDI_UIM_UICC_SEND_APDU

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_send_apdu(
  mmgsdi_send_apdu_req_type* req_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_protocol_enum_type          protocol        = MMGSDI_NO_PROTOCOL;
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
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_ALLOW_SEND_APDU_ONLY, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED
     && (req_ptr->request_header.orig_request_type == MMGSDI_SEND_APDU_REQ
         || req_ptr->request_header.orig_request_type == MMGSDI_SEND_APDU_EXT_REQ))
  {
    protocol = MMGSDI_UICC;
  }
  else
  {
    protocol = mmgsdi_util_determine_protocol(req_ptr->request_header.slot_id);
    if (protocol == MMGSDI_NO_PROTOCOL)
    {
      return MMGSDI_ERROR;
    }
  }
  
  /*
   * Convert to UIM slot.
   */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*
   * Allocate and populate extra parameter.
   */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
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
       mmgsdi_chnl_mgt_is_data_len_valid(req_ptr->data.data_len);
  param_valid &=
       mmgsdi_chnl_mgt_is_data_buffer_valid(req_ptr->data.data_ptr);

  if (param_valid == FALSE)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_ERROR;
  }

  /*
   * Populate UIM information.
   */
  uim_cmd_ptr->hdr.command  = UIM_STREAM_ISO7816_APDU_F;
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_ALLOW_SEND_APDU_ONLY, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED
     && (req_ptr->request_header.orig_request_type == MMGSDI_SEND_APDU_REQ
         || req_ptr->request_header.orig_request_type == MMGSDI_SEND_APDU_EXT_REQ))
  {
    uim_cmd_ptr->hdr.protocol = UIM_WCDMA;
  }
  else
  {
    uim_cmd_ptr->hdr.protocol = UIM_UICC;
  }
  uim_cmd_ptr->hdr.slot     = uim_slot;
  /*
   * Get a new index from client req info table for response data.
   * Set the uim data pointer to the client request table data buffer.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
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
  uim_cmd_ptr->stream_iso7816_apdu.num_cmd_bytes = int32touint16(req_ptr->data.data_len);

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
} /* mmgsdi_uim_uicc_send_apdu */


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_UICC_STATUS

   DESCRIPTION
     This function builds the STATUS command and sends it to UIMDRV for
     further processing.

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
mmgsdi_return_enum_type mmgsdi_uim_uicc_status(
  const mmgsdi_status_req_type        *  req_ptr,
  boolean                                sync_process,
  mmgsdi_status_cnf_type              ** cnf_pptr,
  const mmgsdi_aid_type               *  app_data_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
  uim_channel_type           uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_int_app_info_type  *app_info_ptr     = NULL;
  mmgsdi_static_data_type   *aid_ptr          = NULL;
  mmgsdi_client_req_extra_info_type  *  extra_param_ptr  = NULL;
  mmgsdi_channel_id_type     channel_id       = 0;
  mmgsdi_task_enum_type      mmgsdi_task      = MMGSDI_TASK_MAX;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* ---------------------------------------------------------------------------
     Convert the slot from an MMGSDI SLOT ID to a UIM SLOT ID
     -------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(req_ptr->request_header.orig_request_type == MMGSDI_SET_SIM_PROFILE_REQ)
  {
    uim_channel_id = UIM_CHANNEL0;
  }
  else
  {
    /* Get channel ID corresponding to Session */
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      req_ptr->request_header.session_id,
                      NULL,
                      &channel_id,
                      NULL,
                      NULL,
                      NULL,
                      NULL);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Session can be disconnected from the channel due to app deactivation.
         Try to find the channel id using AID and slot id combination. */
      if (app_data_ptr == NULL ||
          (mmgsdi_util_check_app_termination_skipped(req_ptr->request_header.slot_id,
                                                     app_data_ptr,
                                                     &channel_id) == FALSE))
      {
        UIM_MSG_ERR_2("Failed to get channel id.session_id=0x%x status=0x%x",
                      req_ptr->request_header.session_id, mmgsdi_status);
        return mmgsdi_status;
      }
    }

    /* Convert to UIM Channel ID */
    mmgsdi_status = mmgsdi_util_convert_uim_channel(
                      channel_id, &uim_channel_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
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
  uim_cmd_ptr->hdr.command               = UIM_STATUS_F;
  uim_cmd_ptr->hdr.protocol              = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot                  = uim_slot;
  uim_cmd_ptr->hdr.channel               = uim_channel_id;

  /* ----------------------------------------------------------------------------
     Set the UIM Status Status accordingly
     --------------------------------------------------------------------------*/
  switch ( req_ptr->me_app_status )
  {
    case MMGSDI_STATUS_APP_NO_INDICATION:
      uim_cmd_ptr->status.status = UIM_NO_INDICATION;
      break;

    case MMGSDI_STATUS_APP_INITIALIZED:
      uim_cmd_ptr->status.status = UIM_CURR_APP_INITIALIZED;
      break;

    case MMGSDI_STATUS_APP_TERMINATED:
      uim_cmd_ptr->status.status = UIM_TERMINATION_OF_CURR_APP;
      break;

    case MMGSDI_STATUS_APP_NONE:
    case MMGSDI_STATUS_APP_MAX:
    default:
      MMGSDIUTIL_DEBUG_MSG_ERROR_1("BAD APP STATUS: 0x%x",
                                   req_ptr->me_app_status);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return MMGSDI_ERROR;
  }

  /* -----------------------------------------------------------------------------
     Indicate to the card the expected return Data Structure.
     ---------------------------------------------------------------------------*/
  switch ( req_ptr->ret_data_struct )
  {
    case MMGSDI_STATUS_DATA_SELECT:
      uim_cmd_ptr->status.return_data = UIM_RETURN_RSP_IDENTICAL_TO_SELECT;
      break;

    case MMGSDI_STATUS_TLV_DATA:
      uim_cmd_ptr->status.return_data = UIM_RETURN_DF_NAME_OF_CURR_APP;
      break;

    case MMGSDI_STATUS_NO_DATA:
      uim_cmd_ptr->status.return_data = UIM_RETURN_NONE;
      break;

    case MMGSDI_STATUS_NONE:
    case MMGSDI_STATUS_MAX:
    default:
      MMGSDIUTIL_DEBUG_MSG_ERROR_1("BAD RET DATA STRUCTURE STATUS: 0x%x",
                                   req_ptr->me_app_status);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return MMGSDI_ERROR;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                            req_ptr->request_header.request_type,
                            (void*)req_ptr, &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

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
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     If the expected return data is the DF name TLV,
     then pass in the AID data in the request
     ------------------------------------------------------------------------*/
  if(req_ptr->ret_data_struct == MMGSDI_STATUS_TLV_DATA)
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
      req_ptr->request_header.session_id,
      NULL,
      NULL,
      &app_info_ptr,
      NULL,
      NULL,
      NULL);

    if((mmgsdi_status == MMGSDI_SUCCESS) &&
       (app_info_ptr != NULL))
    {
      aid_ptr = &app_info_ptr->app_data.aid;

      if ((aid_ptr->data_len > 0) &&
          (aid_ptr->data_len <= UIM_AID_MAX_NAME_LENGTH))
      {
        mmgsdi_memscpy(uim_cmd_ptr->status.aid.data,
                       sizeof(uim_cmd_ptr->status.aid.data),
                       aid_ptr->data_ptr,
                       int32touint32(aid_ptr->data_len));
        uim_cmd_ptr->status.aid.aid_length = int32touint8(aid_ptr->data_len);
      }
    }
  }

  /* --------------------------------------------------------------------------
     Provide to the UIM the client ID Entry to be returned in the Status
     Report from the UIM Task
     ------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  if (!sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
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
        mmgsdi_status = mmgsdi_uicc_rsp_status(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
      }
      else
      {
        UIM_MSG_ERR_1("Sync process for request at index 0x%x for STATUS, has null mmgsdi_internal_synch_uim_rpt_rsp",
                      index);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }

  return mmgsdi_status;
} /* mmgsdi_uim_uicc_status */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_AUTH

   DESCRIPTION
     This function is called for USIM Authentication request.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_auth(
  const mmgsdi_auth_req_type        *  req_ptr
)
{
  mmgsdi_return_enum_type        mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *                 uim_cmd_ptr      = NULL;
  uim_slot_type                  uim_slot         = UIM_SLOT_NONE;
  int32                          index            = 0;
  mmgsdi_channel_id_type         channel_id       = 0;
  mmgsdi_static_data_type       *aid_ptr          = NULL;
  uim_channel_type               uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_int_app_info_type      *app_info_ptr     = NULL;
  mmgsdi_session_info_type      *session_info_ptr = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (req_ptr->auth_req.data_len > UIM_MAX_CHARS)
  {
    UIM_MSG_ERR_1("Exceed UIM IUICC Auth Array size 0x%x",
                req_ptr->auth_req.data_len);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                 req_ptr->request_header.session_id,
                 NULL,
                 &channel_id,
                 &app_info_ptr,
                 NULL,
                 &session_info_ptr,
                 NULL);
  /*Channel may not be assigned yet, do not check channel Id*/
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  aid_ptr = &app_info_ptr->app_data.aid;
  if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
      (aid_ptr->data_len == 0))
  {
    UIM_MSG_ERR_1("Invalid UIM USIM Auth AID size 0x%x", aid_ptr->data_len);
    return MMGSDI_ERROR;
  }

  if(req_ptr->auth_context == MMGSDI_AUTN_ISIM_GBA_BOOTSTRAPPING_CONTEXT ||
     req_ptr->auth_context == MMGSDI_AUTN_ISIM_GBA_NAF_DERIVATION_CONTEXT ||
     req_ptr->auth_context == MMGSDI_AUTN_ISIM_CONTEXT ||
     req_ptr->auth_context == MMGSDI_AUTN_ISIM_HTTP_DIGEST_CONTEXT)
  {
    /* ISIM auth is only allowed on non-prov sessions with ISIM AID */
    if (!mmgsdi_util_is_isim_aid(app_info_ptr->app_data.aid.data_ptr,
                                 app_info_ptr->app_data.aid.data_len) ||
        !((session_info_ptr != NULL)&&
          mmgsdi_util_is_non_prov_session(session_info_ptr->session_type)))
    {
      return MMGSDI_INCORRECT_PARAMS;
    }
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  uim_cmd_ptr->hdr.protocol              = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot                  = uim_slot;
  uim_cmd_ptr->hdr.channel               = uim_channel_id;

  /* ------------------------------------------------------------------------
      Set USIM Authentication data
    ----------------------------------------------------------------------*/
  uim_cmd_ptr->autn.aid.aid_length = int32touint8(aid_ptr->data_len);
  mmgsdi_memscpy(uim_cmd_ptr->autn.aid.data,
                 sizeof(uim_cmd_ptr->autn.aid.data),
                 aid_ptr->data_ptr,
                 int32touint32(aid_ptr->data_len));

  uim_cmd_ptr->hdr.command = UIM_AUTHENTICATE_F;

  switch(req_ptr->auth_context)
  {
    case MMGSDI_AUTN_USIM_GSM_CONTEXT:
    case MMGSDI_AUTN_USIM_3G_CONTEXT:
    case MMGSDI_AUTN_USIM_VGCS_VBS_CONTEXT:
      /*-----------------------------------------------------------------------
      Format :
         Byte(s)                Description              Length
         1                      Length of RAND           1
         2 to (L1+1)            RAND                     L1
         (L1+2)                 Length of AUTN (L2)      1
         (L1+3) to (L1+L2+2)    AUTN                     L2
      -----------------------------------------------------------------------*/

      /* P2 parameters */
      uim_cmd_ptr->autn.ref_data_type = UIM_AUTN_SPECIFIC_REF_DATA;
      uim_cmd_ptr->autn.ref_data_number = (byte)req_ptr->auth_context >> 4;

      uim_cmd_ptr->autn.num_bytes = int32touint8(req_ptr->auth_req.data_len);

      /* Length of rand and optional autn */
      mmgsdi_memscpy (uim_cmd_ptr->autn.data,
                      sizeof(uim_cmd_ptr->autn.data),
                      req_ptr->auth_req.data_ptr,
                      (uint32)uim_cmd_ptr->autn.num_bytes);

      break;
    case MMGSDI_AUTN_ISIM_GBA_BOOTSTRAPPING_CONTEXT:
      uim_cmd_ptr->hdr.command = UIM_ISIM_AUTHENTICATE_F;
      /* Fall through for generic GBA Bootstrapping handling */
    case MMGSDI_AUTN_USIM_GBA_BOOTSTRAPPING_CONTEXT:

      /*-----------------------------------------------------------------------
      Format :
         Byte(s)                Description              Length
         1                      “GBA Security Context
                                Bootstrapping Mode”
                                tag = ‘DD’               1
         2                      Length of RAND           1
         3 to (L1+2)            RAND                     L1
         (L1+3)                 Length of AUTN (L2)      1
         (L1+4) to (L1+L2+3)    AUTN                     L2
      -----------------------------------------------------------------------*/

      /* P2 parameters */
      uim_cmd_ptr->autn.ref_data_type = UIM_AUTN_SPECIFIC_REF_DATA;
      uim_cmd_ptr->autn.ref_data_number = (byte)req_ptr->auth_context >> 4;

      /*Add 1 Byte for the Tag*/
      uim_cmd_ptr->autn.num_bytes = int32touint8(req_ptr->auth_req.data_len + 1);

      uim_cmd_ptr->autn.data[0] = MMGSDI_GBA_BS_TAG;

      /* Length of rand and optional autn */
      if(uim_cmd_ptr->autn.num_bytes < UIM_MAX_AUTH_BYTES)
      {
        mmgsdi_memscpy(&uim_cmd_ptr->autn.data[1],
                       sizeof(uim_cmd_ptr->autn.data) - 1,
                       req_ptr->auth_req.data_ptr,
                      (uint32)uim_cmd_ptr->autn.num_bytes);
      }
      break;

    case MMGSDI_AUTN_ISIM_GBA_NAF_DERIVATION_CONTEXT:
      uim_cmd_ptr->hdr.command = UIM_ISIM_AUTHENTICATE_F;
      /* Fall through for generic GBA NAF Derivation handling */
    case MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT:

      /*-----------------------------------------------------------------------
      Format :
         Byte(s)                Description              Length
         1                      “GBA Security Context
                                NAF Derivation Mode”
                                tag = ‘DE’               1
         2                      Length of RAND           1
         3 to (L1+2)            RAND                     L1
         (L1+3)                 Length of AUTN (L2)      1
         (L1+4) to (L1+L2+3)    AUTN                     L2
      -----------------------------------------------------------------------*/

      /* P2 parameters */
      uim_cmd_ptr->autn.ref_data_type = UIM_AUTN_SPECIFIC_REF_DATA;
      uim_cmd_ptr->autn.ref_data_number = (byte)req_ptr->auth_context >> 4;

      /*Add 1 Byte for the Tag*/
      uim_cmd_ptr->autn.num_bytes = int32touint8(req_ptr->auth_req.data_len + 1);

      uim_cmd_ptr->autn.data[0] = MMGSDI_GBA_NAF_DERIVATION_TAG;

      /* Length of rand and optional autn */
      if(uim_cmd_ptr->autn.num_bytes < UIM_MAX_AUTH_BYTES)
      {
        mmgsdi_memscpy(&uim_cmd_ptr->autn.data[1],
                       sizeof(uim_cmd_ptr->autn.data) - 1,
                       req_ptr->auth_req.data_ptr,
                      (uint32)uim_cmd_ptr->autn.num_bytes);
      }
      break;

    case MMGSDI_AUTN_USIM_MBMS_MSK_UPDATE_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MTK_GEN_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MSK_DEL_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MUK_DEL_CONTEXT:
      {
      /*-----------------------------------------------------------------------
      Format :
         Byte(s)                Description              Length
         1                      MBMS Data Object tag
                                (‘53’)                      1
         2 to 1+A               MBMS Data Object length     A
         A+2                    MBMS Security Context Mode  1
         A+3 to (A+L1+1)        MIKEY                       L1-1
      -----------------------------------------------------------------------*/
        uint32 ber_len             = int32touint32(req_ptr->auth_req.data_len +1); /*1 for the Tag*/
        uint8  buff_offset         = 0;
        uint8  ber_data[4];

        /* P2 parameters */
        uim_cmd_ptr->autn_odd_ins.ref_data_type = UIM_AUTN_SPECIFIC_REF_DATA;
        uim_cmd_ptr->autn_odd_ins.ref_data_number = (byte)req_ptr->auth_context >> 4;

        uim_cmd_ptr->hdr.command  = UIM_AUTHENTICATE_ODD_INS_F;

        uim_cmd_ptr->autn_odd_ins.len = uint32touint16(mmgsdi_util_get_ber_length(
                                                       ber_len,
                                                       int32touint32(req_ptr->auth_req.data_len),
                                                       ber_data));

        ber_len = uim_cmd_ptr->autn_odd_ins.len;

        /*Add Length for Data , Tag and  Context Mode, new Length*/
        uim_cmd_ptr->autn_odd_ins.len += int32touint16(req_ptr->auth_req.data_len + 2);

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr->autn_odd_ins.data,
                                           uim_cmd_ptr->autn_odd_ins.len);

        if (uim_cmd_ptr->autn_odd_ins.data == NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }

        /*Tag*/
        uim_cmd_ptr->autn_odd_ins.data[buff_offset++]= 0x53;

        /*Length*/
        mmgsdi_memscpy(&uim_cmd_ptr->autn_odd_ins.data[buff_offset],
                       uim_cmd_ptr->autn_odd_ins.len - buff_offset,
                       ber_data,
                       ber_len);

        buff_offset += uint32touint8(ber_len);

        /*Security Context*/
        uim_cmd_ptr->autn_odd_ins.data[buff_offset++] = (int)req_ptr->auth_context & 0x0F;

        /*Data*/
        mmgsdi_memscpy(&uim_cmd_ptr->autn_odd_ins.data[buff_offset],
                       uim_cmd_ptr->autn_odd_ins.len - buff_offset,
                       req_ptr->auth_req.data_ptr,
                       int32touint32(req_ptr->auth_req.data_len));
      }
      break;
    case MMGSDI_AUTN_USIM_MBMS_SPE_DEL_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_REC_DEL_CONTEXT:
      {
      /*-----------------------------------------------------------------------
      Format :
         Byte(s)        Description              Length
         1              MBMS Data Object tag
                        (‘73’)                      1
         2 to 1+A       MBMS Data Object length     A
         A+2            OMA BCAST Operation TLV
                        tag('AE')                   1
         A+3            OMA BCAST Operation TLV
                        Data Object Length          1
         A+4            OMA BCAST Operation Mode
             Tag('90')                   1
         A+5            OMA BCAST Operation Mode
             Data Object Length          1
         A+6            OMA BCAST Operation Mode    1
             Data Object
         A+7 to (A+7+L) Data in TLV                 L
      -----------------------------------------------------------------------*/
        uint32 total_ber_len        = int32touint32(req_ptr->auth_req.data_len +
                                     MMGSDI_TLV_TAG_LEN + /*MBMS Data Object tag*/
                                                          /*Skip the length for now*/
                                     MMGSDI_TLV_TAG_LEN + /*OMA BCAST Operation TLV Tag*/
                                     MMGSDI_TLV_LEN_LEN + /*OMA BCAST Operation TLV Length*/
                                     MMGSDI_TLV_TAG_LEN + /*OMA BCAST Operation Mode tag*/
                                     MMGSDI_TLV_LEN_LEN + /*OMA BCAST Operation Mode Len*/
                                     MMGSDI_TLV_LEN_LEN);  /*OMA BCAST Operation Mode Value*/
        uint32 ber_len             = total_ber_len;
        uint8  buff_offset         = 0;
        uint8  ber_data[4];

        /* P2 parameters */
        uim_cmd_ptr->autn_odd_ins.ref_data_type = UIM_AUTN_SPECIFIC_REF_DATA;
        uim_cmd_ptr->autn_odd_ins.ref_data_number = (byte)((int)(req_ptr->auth_context) >> 8);

        uim_cmd_ptr->hdr.command  = UIM_AUTHENTICATE_ODD_INS_F;
        uim_cmd_ptr->autn_odd_ins.len = 0;

        uim_cmd_ptr->autn_odd_ins.len = uint32touint16(mmgsdi_util_get_ber_length(ber_len,
                                          int32touint32(req_ptr->auth_req.data_len),
                                          ber_data));

        ber_len = uim_cmd_ptr->autn_odd_ins.len;

        /*Add Length for Data , Tag and  Context Mode, new Length*/
        uim_cmd_ptr->autn_odd_ins.len += uint32touint16(total_ber_len);

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr->autn_odd_ins.data,
                                           uim_cmd_ptr->autn_odd_ins.len);

        if (uim_cmd_ptr->autn_odd_ins.data == NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }

        /*MBMS Data Object tag*/
        uim_cmd_ptr->autn_odd_ins.data[buff_offset++]= MMGSDI_MBMS_BER_TLV_TAG;

        /*MBMS Data Object length*/
        mmgsdi_memscpy(&uim_cmd_ptr->autn_odd_ins.data[buff_offset],
                       uim_cmd_ptr->autn_odd_ins.len - buff_offset,
                       ber_data,
                       ber_len);

        buff_offset += uint32touint8(ber_len);

        /*OMA BCAST Operation TLV Tag*/
        uim_cmd_ptr->autn_odd_ins.data[buff_offset++] = MMGSDI_BCAST_TAG;

        /*OMA BCAST Operation TLV Length*/
        uim_cmd_ptr->autn_odd_ins.data[buff_offset++] = int32touint8
         (MMGSDI_TLV_TAG_LEN         /*Mode Tag*/
        + MMGSDI_TLV_LEN_LEN         /*Mode Length*/
        + MMGSDI_TLV_TAG_LEN         /*Mode Value*/
        + req_ptr->auth_req.data_len);/*Data*/

        /*OMA BCAST Operation Mode tag*/
        uim_cmd_ptr->autn_odd_ins.data[buff_offset++] = MMGSDI_BCAST_MODE_TAG;

        /*OMA BCAST Operation Mode Length*/
        uim_cmd_ptr->autn_odd_ins.data[buff_offset++] = 1;

        /*OMA BCAST Operation Mode Value*/
        uim_cmd_ptr->autn_odd_ins.data[buff_offset++] = (byte)((int)req_ptr->auth_context << 8) & 0x0F;

        /*Data*/
        mmgsdi_memscpy(&uim_cmd_ptr->autn_odd_ins.data[buff_offset],
                       uim_cmd_ptr->autn_odd_ins.len - buff_offset,
                       req_ptr->auth_req.data_ptr,
                       int32touint32(req_ptr->auth_req.data_len));
      }
      break;

    case MMGSDI_AUTN_ISIM_CONTEXT:
    case MMGSDI_AUTN_ISIM_HTTP_DIGEST_CONTEXT:
      uim_cmd_ptr->hdr.command               = UIM_ISIM_AUTHENTICATE_F;
      /* ------------------------------------------------------------------------
         Set ISIM Authentication data
         ----------------------------------------------------------------------*/
      uim_cmd_ptr->isim_autn.aid.aid_length = int32touint8(aid_ptr->data_len);
      mmgsdi_memscpy(uim_cmd_ptr->isim_autn.aid.data,
                     sizeof(uim_cmd_ptr->isim_autn.aid.data),
                     aid_ptr->data_ptr,
                     int32touint32(aid_ptr->data_len));

      /* Reference data number for ISIM AKA is 0x01 and for HTTP Digest
         it is 0x02.*/
      if (req_ptr->auth_context == MMGSDI_AUTN_ISIM_CONTEXT)
      {
        uim_cmd_ptr->isim_autn.ref_data_number = UIM_AUTN_ISIM_AKA;
      }
      else
      {
        uim_cmd_ptr->isim_autn.ref_data_number = UIM_AUTN_ISIM_HTTP_DIGEST;
      }
      uim_cmd_ptr->isim_autn.ref_data_type   = UIM_AUTN_SPECIFIC_REF_DATA;

      uim_cmd_ptr->isim_autn.num_bytes = int32touint8(req_ptr->auth_req.data_len);
      mmgsdi_memscpy(uim_cmd_ptr->isim_autn.data,
                     sizeof(uim_cmd_ptr->isim_autn.data),
                     req_ptr->auth_req.data_ptr,
                     uim_cmd_ptr->isim_autn.num_bytes);
      break;

    default:
      UIM_MSG_ERR_1("Invalid Context Type 0x%x", req_ptr->auth_context );
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return MMGSDI_ERROR;
  }

  /*---------------------------------------------------------------------------
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
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
} /* mmgsdi_uim_uicc_auth */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_BCAST

   DESCRIPTION
     This function is called for USIM BCAST request.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_bcast(
  const mmgsdi_bcast_req_type        *  req_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
  mmgsdi_channel_id_type     channel_id       = 0;
  mmgsdi_static_data_type   *aid_ptr          = NULL;
  uim_channel_type           uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_int_app_info_type  *app_info_ptr     = NULL;

  UIM_MSG_HIGH_0("MMGSDI UIM UICC BCAST");

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                 req_ptr->request_header.session_id,
                 NULL,
                 &channel_id,
                 &app_info_ptr,
                 NULL,
                 NULL,
                 NULL);
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  aid_ptr = &app_info_ptr->app_data.aid;
  if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
      (aid_ptr->data_len == 0))
  {
    UIM_MSG_ERR_1("Invalid UIM USIM BCAST AID size 0x%x", aid_ptr->data_len);
    return MMGSDI_ERROR;
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  uim_cmd_ptr->hdr.protocol              = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot                  = uim_slot;
  uim_cmd_ptr->hdr.channel               = uim_channel_id;

  /* ------------------------------------------------------------------------
      Set USIM BCAST data
    ----------------------------------------------------------------------*/
  uim_cmd_ptr->bcast.aid.aid_length = int32touint8(aid_ptr->data_len);
  mmgsdi_memscpy(uim_cmd_ptr->bcast.aid.data,
                 sizeof(uim_cmd_ptr->bcast.aid.data),
                 aid_ptr->data_ptr,
                 int32touint32(aid_ptr->data_len));

  switch(req_ptr->bcast_mode)
  {
    case MMGSDI_BCAST_SPE_AUDIT_MODE:
    case MMGSDI_BCAST_SPE_SIG_MODE:
    case MMGSDI_BCAST_REC_AUDIT_MODE:
    {
      /*-----------------------------------------------------------------------
      Format :
         Byte(s)                Description              Length
         1                      MBMS Data Object tag
                                (‘73’)                      1
         2 to 1+A               MBMS Data Object length     A
         A+2 to (A+L1+2)        Data                        L1
      -----------------------------------------------------------------------*/
        uint32 ber_len             = int32touint32(req_ptr->bcast_req.data_len + 1); /*1 for the Tag*/
        uint8  buff_offset         = 0;
        uint8  ber_data[4];

        /* P2 parameters */
        uim_cmd_ptr->bcast.ref_data_number = (byte)req_ptr->bcast_mode;

        uim_cmd_ptr->hdr.command  = UIM_BCAST_F;
        uim_cmd_ptr->bcast.len = 0;

        uim_cmd_ptr->bcast.len = uint32touint16(mmgsdi_util_get_ber_length(ber_len,
                                   int32touint32(req_ptr->bcast_req.data_len),
                                   ber_data));

        ber_len = uim_cmd_ptr->bcast.len;


        /*Add Length for Data , Tag and  Context Mode, new Length*/
        uim_cmd_ptr->bcast.len += int32touint16(req_ptr->bcast_req.data_len + 2);

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr->bcast.data,
                                           uim_cmd_ptr->bcast.len);

        if (uim_cmd_ptr->bcast.data == NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }

        /*Tag*/
        uim_cmd_ptr->bcast.data[buff_offset++]= MMGSDI_MBMS_BER_TLV_TAG;

        /*Length*/
        mmgsdi_memscpy(&uim_cmd_ptr->bcast.data[buff_offset],
                       uim_cmd_ptr->bcast.len - buff_offset,
                       ber_data,
                       ber_len);

        buff_offset += uint32touint8(ber_len);

        /*Data*/
        mmgsdi_memscpy(&uim_cmd_ptr->bcast.data[buff_offset],
                       uim_cmd_ptr->bcast.len - buff_offset,
                       req_ptr->bcast_req.data_ptr,
                       int32touint32(req_ptr->bcast_req.data_len));
      }
      break;

    default:
      UIM_MSG_ERR_1("Invalid Mode Type 0x%x", req_ptr->bcast_mode );
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return MMGSDI_ERROR;
  }

  /*---------------------------------------------------------------------------
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
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    UIM_MSG_ERR_0("No Client Request Entry Available");
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
}


/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SEARCH_RECORD

   DESCRIPTION:
     This function package the Search Record command and send it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_search_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.
     MMGSDI_NOT_SUPPORTED    : When Search Type is not valid for UICC Card.

   SIDE EFFECTS:

----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_search_record (
  const mmgsdi_search_req_type             * req_ptr,
  boolean                                    sync_process,
  mmgsdi_search_cnf_type                  ** cnf_pptr
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_channel_id_type             channel_id       = 0;
  uim_channel_type                   uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  int32                              index            = 0;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  boolean                            card_session     = FALSE;
  mmgsdi_task_enum_type              mmgsdi_task      = MMGSDI_TASK_MAX;

  /*Checking of Input Parameter*/
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
                    &channel_id,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  /* If the Search request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    /* Irrespective of app selection, allow search request for
       files outside ADF */

    /* Block search for files under ADF */
    if (!mmgsdi_file_is_access_outside_adf(
           &req_ptr->access))
    {
      UIM_MSG_ERR_0("Cannot search file in ADF, on card session");
      return MMGSDI_ERROR;
    }
    else
    {
      /* Card Session accesses always go on channel 0 */
      uim_channel_id = UIM_CHANNEL0;
      card_session = TRUE;
    }
  }
  else if (mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    /* For non-prov. Sessions opened through Select with MF, no app shall be
       tied to the Session. In such cases, allow requests for EFs outside
       of ADFs on the channel corresponding to the Session */
    if((session_info_ptr != NULL) &&
       mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
    {
      if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
      {
        UIM_MSG_ERR_0("Channel info needs to be valid for non-prov. session");
        return MMGSDI_ERROR;
      }

      /* Block reads for files under ADF */
      if (!mmgsdi_file_is_access_outside_adf(
             &req_ptr->access))
      {
        return MMGSDI_ERROR;
      }

      /* Set uim_channel_id back to UIM_MAX_CHANNELS, so that the command is
         sent on the actual logical channel corresponding to the Session */
      uim_channel_id = UIM_MAX_CHANNELS;
    }
    /* check if it is EF under MF, if so, not an error */
    else if (mmgsdi_file_is_ef_under_mf(&req_ptr->access))
    {
      /* may be the first select or read during initiailzation,
         default everything to Channel 0 */
      uim_channel_id = UIM_CHANNEL0;
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }

  /* Convert to UIM Channel ID */
  mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* AID data not required for file access on a card session or
     for an EF under MF */
  if((!card_session) &&
     (!mmgsdi_file_is_ef_under_mf(&req_ptr->access)))
  {
    if(app_info_ptr != NULL)
    {
      aid_ptr = &app_info_ptr->app_data.aid;

      if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
          (aid_ptr->data_len < 0))
      {
        UIM_MSG_ERR_1("Invalid UIM AID size 0x%x", aid_ptr->data_len);
        return MMGSDI_ERROR;
      }
    }
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(MMGSDI_SEARCH_REQ,
                                                             (void*)req_ptr,
                                                             &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    UIM_MSG_ERR_0("Population of extra parameter is failed in UIM UICC Search");
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }

  /*-------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  --------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    /* For Search, Extra Parameter Data Struncture contains only file access
       information. And File accesss information does not have any pointer
       that needs to free before deallocation extra parameter pointer*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*-------------------------------------------------
  hdr                            : Command header
  cmd_hdr                        : command header
  command                        : The command
  status                         : Status of the R-UIM
  options                        : Indicate command handling options
  protocol                       : Protocol of the command
  slot                           : Slot to execute the command
  uim_rpt_type                   : Pointer to Function to send report
  cmd_count                    :
  user_data                      : User defined data
  channel                        :
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
  uim_cmd_ptr->hdr.command                 = UIM_SEEK_F;
  uim_cmd_ptr->hdr.protocol                = UIM_WCDMA;
  uim_cmd_ptr->hdr.slot                    = uim_slot;
  uim_cmd_ptr->hdr.channel                 = uim_channel_id;

  /* Updating seek_type: Not used for Search Record  */
  uim_cmd_ptr->seek.seek_type              = UIM_SEEK_TYPE_1;

  /* Updating mode: Not used for UICC Search Record */
  uim_cmd_ptr->seek.mode                   = UIM_SEEK_BEGINNING_FORWARDS;

  /* Updating rec_number: Record Number */
  uim_cmd_ptr->seek.rec_number             = int32touint8(req_ptr->rec_num);

  UIM_MSG_HIGH_3("UICC SEARCH RECORD NUMBER: 0x%x, SEARCH TYPE: 0x%x, SEARCH DIRECTION: 0x%x",
                 uim_cmd_ptr->seek.rec_number,
                 req_ptr->search_type,
                 req_ptr->search_direction);

  /* Updating srch_mode: Simple/enhanced Search */
  switch(req_ptr->search_type)
  {
    case MMGSDI_UICC_SIMPLE_SEARCH:
      uim_cmd_ptr->seek.srch_mode          = UIM_SIMPLE_SRCH;
      break;

    case MMGSDI_UICC_ENHANCED_SEARCH:
      uim_cmd_ptr->seek.srch_mode          = UIM_ENHANCED_SRCH;
      break;

    default:
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return MMGSDI_NOT_SUPPORTED;
  }/* end of switch(req_ptr->search_type) */

  /* Updating srch_dir: Search direction */
  switch(req_ptr->search_direction)
  {
    case MMGSDI_SEARCH_FORWARD_FROM_REC_NUM:
      /* For Simple UICC Search */
      uim_cmd_ptr->seek.srch_dir           = UIM_SRCH_FORWARD;
      /* For Enhance Search */
      uim_cmd_ptr->seek.srch_rec_mode_type = UIM_SRCH_P1_FORWARD;
      break;

    case MMGSDI_SEARCH_BACKWARD_FROM_REC_NUM:
      /* For Simple UICC Search */
      uim_cmd_ptr->seek.srch_dir           = UIM_SRCH_BACKWARD;
      /* For Enhance Search */
      uim_cmd_ptr->seek.srch_rec_mode_type = UIM_SRCH_P1_BACKWARD;
      break;

    default:
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return MMGSDI_INCORRECT_PARAMS;

  }/* end of  switch(req_ptr->search_direction) */

  /* Updating offset_type: offset as absolute position or character */
  if (MMGSDI_UICC_ENHANCED_SEARCH == req_ptr->search_type)
  {
    /* Updating offset: Offset within the record to begin search from */
    uim_cmd_ptr->seek.offset =
      uint32touint8(req_ptr->enhanced_search_offset.offset_data);

    UIM_MSG_HIGH_2("UICC ENHANCED SEARCH OFFSET DATA: 0x%x, OFFSET TYPE: 0x%x",
                   uim_cmd_ptr->seek.offset,
                   req_ptr->enhanced_search_offset.offset_type);

    switch(req_ptr->enhanced_search_offset.offset_type)
    {
      /* Offset is only used for Enhanced Search */
      case MMGSDI_SEARCH_BY_OFFSET:
        uim_cmd_ptr->seek.offset_type      = UIM_SRCH_ABSOLUTE;
        break;

      case MMGSDI_SEARCH_BY_CHAR:
        uim_cmd_ptr->seek.offset_type      = UIM_SRCH_CHARACTER;
        break;

      default:
        MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
        return MMGSDI_INCORRECT_PARAMS;
    }/*end of  switch(req_ptr->offset.offset_type) */
  }/*end of if (MMGSDI_UICC_ENHANCED_SEARCH ==req_ptr->search_type) */
  else
  {
    uim_cmd_ptr->seek.offset_type = UIM_SRCH_ABSOLUTE;
    uim_cmd_ptr->seek.offset      = 0x00;
    UIM_MSG_HIGH_0("UICC ENHANCED SEARCH : NOT APPLICABLE ");
  }

  /* Updating num_bytes: Number of bytes in search pattern */
  if (req_ptr->data.data_len > 0xFF )
  {
    UIM_MSG_HIGH_1("Change Search data length 0x%x to UIM_MAX_CHARS ",
                   req_ptr->data.data_len);
    uim_cmd_ptr->seek.num_bytes            = 0xFF;
    /* Maximum Length is bound to 255 */
  }
  else
  {
    uim_cmd_ptr->seek.num_bytes            = (byte)req_ptr->data.data_len;
  }

  /* Update srch_pattern[UIM_MAX_CHARS ]; Search string */
  mmgsdi_memscpy((byte *)uim_cmd_ptr->seek.srch_pattern,
                 sizeof(uim_cmd_ptr->seek.srch_pattern),
                 (byte *)req_ptr->data.data_ptr,
                 int32touint32(uim_cmd_ptr->seek.num_bytes));

  mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                           &uim_cmd_ptr->seek.path);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    UIM_MSG_ERR_0("Convesion to UIM path is failed in UIM UICC Search ");
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }

  (void)mmgsdi_file_set_uim_sfi(req_ptr->request_header.session_id,
                                req_ptr->request_header.slot_id,
                                &(req_ptr->access),
                                &uim_cmd_ptr->seek.sfi);

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
    /*Printing File Information */
    mmgsdi_util_print_file_information(req_ptr->access,TRUE);
    return mmgsdi_status;
  }
  /* Updating aid:  AID for USIM */
  if(aid_ptr != NULL)
  {
    mmgsdi_memscpy(uim_cmd_ptr->seek.aid.data,
                   sizeof(uim_cmd_ptr->seek.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->seek.aid.aid_length = int32touint8(aid_ptr->data_len);
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);
  UIM_MSG_HIGH_0("Sending down UICC SEARCH RECORD command to UIM");

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
    1) Release the client table
    2) No need Release extra param pointer, It is already removed thru client Table
    3) No need Release uim cmd pointer, It is already removed thru client Table
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
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
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE, MMGSDI_UICC);
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
}/* end of function mmgsdi_uim_uicc_search_record */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SET_FILE_STATUS

   DESCRIPTION:
     This function will activate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of UICC.

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
mmgsdi_return_enum_type mmgsdi_uim_uicc_set_file_status (
  const mmgsdi_set_file_status_req_type * req_ptr,
  mmgsdi_cmd_enum_type                    req_type
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;
  mmgsdi_channel_id_type             channel_id       = MMGSDI_DEFAULT_CHANNEL;
  uim_channel_type                   uim_channel_id   = UIM_MAX_CHANNELS;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  boolean                            card_session     = FALSE;

  UIM_MSG_HIGH_1("MMGSDI_UIM_UICC_SET_FILE_STATUS request type 0x%x", req_type);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. access method  - Should be EF Enum access
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (req_type != MMGSDI_REHABILITATE_REQ && req_type != MMGSDI_INVALIDATE_REQ)
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
                    &channel_id,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

    /* If the request is on a Card Session */
  if ((session_info_ptr != NULL) &&
      (mmgsdi_util_is_card_session(session_info_ptr->session_type)))
  {
    /* Provisioning has not yet happened on channel 0 */
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
    {
      /* check if it is EF under MF, if so, not an error */
      if(mmgsdi_file_is_ef_under_mf(&req_ptr->access))
      {
        uim_channel_id = UIM_CHANNEL0;
        card_session = TRUE;
      }
      else
      {
        UIM_MSG_ERR_0("Cannot read file on Card Session before provisioning");
        return MMGSDI_ERROR;
      }
    }
    else
    {
      /* Block reads for files under ADF */
      if (!mmgsdi_file_is_access_outside_adf(
             &req_ptr->access))
      {
        return MMGSDI_ERROR;
      }
      else
      {
        /* Card Session accesses always go on channel 0 */
        uim_channel_id = UIM_CHANNEL0;
        card_session = TRUE;
      }
    }
  }
  else if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return mmgsdi_status;
  }

  if (uim_channel_id == UIM_MAX_CHANNELS)
  {
    /* channel has not been assigned, this means we must have a good
       session_id which returns a valid channel ID */
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id,
                                                    &uim_channel_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /* AID data not required for file access on a Card Session or
     for an EF under MF */
  if ((!card_session) &&
      (!mmgsdi_file_is_ef_under_mf(&req_ptr->access)))
  {
    if(app_info_ptr != NULL)
    {
      aid_ptr = &app_info_ptr->app_data.aid;

      if ((aid_ptr->data_len > UIM_AID_MAX_NAME_LENGTH) ||
          (aid_ptr->data_len < 0))
      {
        UIM_MSG_ERR_1("Invalid UIM AID size 0x%x", aid_ptr->data_len);
        return MMGSDI_ERROR;
      }
    }
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          req_type, (void*)req_ptr, &extra_param_ptr);
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

  uim_cmd_ptr->hdr.protocol        = UIM_UICC;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;
  if (req_type == MMGSDI_REHABILITATE_REQ)
  {
    uim_cmd_ptr->hdr.command         = UIM_REHABILITATE_F;
    mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                             &uim_cmd_ptr->rehab.path);
  }
  else
  {
    uim_cmd_ptr->hdr.command         = UIM_INVALIDATE_F;
    mmgsdi_status = mmgsdi_file_set_uim_path(&(req_ptr->access),
                                             &uim_cmd_ptr->invalidate.path);
  }

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

  /* Populate AID if EF is not under MF */
  if (aid_ptr != NULL)
  {
    if (req_type == MMGSDI_REHABILITATE_REQ)
    {
      mmgsdi_memscpy(uim_cmd_ptr->rehab.aid.data,
                     sizeof(uim_cmd_ptr->rehab.aid.data),
                     aid_ptr->data_ptr,
                     int32touint32(aid_ptr->data_len));

      uim_cmd_ptr->rehab.aid.aid_length = int32touint8(aid_ptr->data_len);
    }
    else
    {
      mmgsdi_memscpy(uim_cmd_ptr->invalidate.aid.data,
                     sizeof(uim_cmd_ptr->invalidate.aid.data),
                     aid_ptr->data_ptr,
                     int32touint32(aid_ptr->data_len));

      uim_cmd_ptr->invalidate.aid.aid_length = int32touint8(aid_ptr->data_len);
    }
  }

  UIM_MSG_HIGH_2("Sending down command 0x%x to UIM 0x%x",
                 req_type, req_ptr->access.file.file_enum);

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
} /* mmgsdi_uim_uicc_set_file_status */
