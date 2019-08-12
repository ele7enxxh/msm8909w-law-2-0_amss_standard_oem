/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC protocol processing support for MMGSDI.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uicc.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/08/16   av      Send SELECT ISDR to determine if card is eUICC or not
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
06/19/16   bcho    F3 frequency reduction by conditional logic
05/24/16   ar      Review of macros used by MMGSDI
05/23/16   dd      Skip Search Req for PB files
05/12/16   tkl     F3 log prints cleanup
12/14/15   ar      Cache dummy UST only if app_state is beyond PIN state
10/26/15   nr      Disable MMGSDI PB cache for PBM module
10/06/15   rp      Fix usage of wrong size in memset
09/09/15   ar      Cache dummy EF-UST if face issue in caching from card
09/09/15   ar      Return service disabled when EST not accessible for buggy card
08/10/15   vdc     Retry read request for non spec compliant cards
04/17/15   nr      Phonebook record read optimization
01/09/15   ar      Cache file if req_len is more than file size
12/12/14   bcho    Return service status as not available if UST is not cached
11/03/14   yt      Return status words of original PIN OP
10/23/14   yt      Updates to session and app management
09/09/14   kk      Enhancements in NV handling
08/11/14   bcho    Optimize srv req by using cached srv table in client context
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/15/14   yt      Support for NV refresh
06/30/14   vv      Remove Perso code
06/23/14   yt      Send STATUS before app deactivation
06/03/14   tl      Remove additional parameter variables
05/27/14   av      Don't allow access if UST/CST can't be read(pin not verified)
04/22/14   yt      Block IMS auth on non-IMS apps
04/17/14   av      Enhance MMGSDI attributes cache
04/08/14   ar      Skip sending event to clients if service status is same
03/25/14   vdc     Added support for verifying PIN after PIN is changed
03/18/14   tl      Introduce new SIM Lock feature
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
02/10/14   ar      Replace session checks with utility functions
12/05/13   tkl     SFI Support and decouple uim file enums
01/21/14   yt      Remove duplicate code
01/14/14   tl      Return service disabled instead of error when EF-EST not present
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/01/13   df      Check UST for authenticate commands
11/12/13   yt      Fixes for multi-SIM OnChip activation
11/06/13   tl      Remove TO-DO
10/04/13   vdc     F3 message reduction
10/03/13   yt      Remove duplicate code
10/02/13   av      Do not activate onchip for non-test IMSIs
09/26/13   av      Enable OnChip on bootup based on EFS item
09/24/13   yt      Remove function mmgsdi_util_is_gsdi_task_context()
09/09/13   at      Update macros that are used while parsing EF DIR
09/03/13   tl      Reduce On-chip SIM activation delay between stages
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/07/13   tl      Add CDMA AKA Authenticate functionality
07/29/13   am      Added support to determine CSG support dynamically
07/10/13   at      Fix merge error for AID length check
07/09/13   vdc     Allocate memory for slot data while activating onchip UICC
07/03/13   at      Fix the length check for AID when parsing & storing AID
07/02/13   at      Validate AID & label data while parsing EF DIR records
06/24/13   vdc     Added support for triple SIM
05/30/13   akv     Heart beat mechanism for Watchdog
05/21/13   vv      Added support for session closed cause
05/21/13   vdc     Fix compilation errors for memory optimization
05/17/13   hn      Support for Alpha for Refresh
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
05/10/13   vv      Reject access to ActingHPLMN if EHPLMN srv is present
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/11/13   vv      Re-design handling of VPE response from the card
03/04/13   av      Allow card sessions to access cache for READ
02/07/13   yt      Support for activation/deactivation of EFs
01/20/13   vv      Added EONS support for the CSG
01/11/13   tl      Update est_file mmgsdi_uicc_process_enable_or_disable_srv()
12/17/12   tl      Check for service availability for enable/disable service
12/12/12   av      Fixed compiler critical warnings
12/05/12   vv      Added support for parallel processing in MMGSDI
11/05/12   av      Skipping checking security attributes before sending read
                   request down to the card or to the cache
10/12/12   abg     Do not send provisioning events to non-prov sessions
10/11/12   spo     Removed unnecessary F3 messages
10/01/12   vs      Added utility for context check including logic for SGLTE
09/28/12   abg     Updated ERR to MSG_ERROR
09/25/12   av      Skip reading EF-ARR for internal selects
09/13/12   av      Fixed compilation warning
09/05/12   av      Handling of VPE response from the card
09/03/12   bcho    mmgsdi_uicc_search modified to support sync operation,
                   Read record from OPL-PNN cache
07/19/12   tl      Increase number of applications in mmgsdi context to 20
05/16/12   bcho    Return MMGSDI_ERROR 1. when app ptr is NULL or
                   mmgsdi_util_get_prov_session_info() fails
                   2. EST service in not available while enable/disable operation
04/02/12   tl      Match entire RID and 3G App Code
03/28/12   bcho    rex_sleep() is replaced with sleep mechanism based on
                   non-deferrable timer
03/16/12   yt      Modify references to gsdi_task_tcb
03/07/12   bcho    NULL check removed for mmgsdi_channel_info_table_ptr
03/01/12   bcho    Redundant session info extraction function call removed
02/23/12   av      Clean up remaining always ON features
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
01/14/12   bcho    Thread safe changes
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   kk      Legacy GSDI removal updates
12/21/11   kk      Added USIM ECC caching support
10/28/11   yt      Added new files for cache-related utility functions
10/28/11   ssr     Fixed KW errors
10/19/11   js      Support to select ADF before PIN2 operation
09/21/11   ssr     Fixed code to send the Reset command immediately after
                   the EF-DIR failed or any read record failed.
08/01/11   vs      Updates for key ref based PIN operations
05/20/11   yt      Added support for INCREASE request
05/17/11   kk      Updates for Thread safety
05/17/11   shr     Updates for Thread safety
03/15/11   nb      Support for Async TP
03/11/11   ms      Clear the app capabilities in session deactivation
02/22/11   at      Fix for handling card sessions in mmgsdi_uicc_get_file_attr
02/21/11   ms      Implementation of Session based caching on App capabilities
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
01/31/11   ssr     Added file access check
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/26/10   shr     Do not build Phonebook lookup table if PBM is a session
                   based client
11/24/10   yt      Removed NULL check for extra_param_ptr for USIM_AUTH_REQ
11/05/10   yt      Klocwork fixes
10/21/10   shr     Cleanup of Onchip USIM support
10/14/10   yt      Updated handling of client req table entries in case of error
09/17/10   yk      Added support dynamic registration on WDOG.
09/09/10   nmb     Merge for USIM BDN/ACL state being overwritten
09/07/10   yt      Fix for enabling Universal PIN
08/31/10   vs      Clear client req table entry when SRV req is invalid
08/30/10   shr     Perso clean up for Sessions
07/15/10   adp     IMSI should always be read from the cache, expect for the
                   first time.
07/01/10   nmb     Updates for JCDMA CSIM support
06/10/10   shr     If PIN status is already cached, do not send PIN commands
                   to the card
06/06/10   kk      Fixed is service available handling - skipped EST check
05/26/10   yt      Fixed compiler warnings
05/04/10   shr     Removed temporary Feature Flags
04/27/10   shr     Added Hiddenkey support
04/22/10   nb      Reintroduced service table check before sending down SELECT
04/16/10   shr     Fixed onchip activation handling
04/07/10   yt      Fixed compilation warnings
04/05/10   mib     Added support for KEYS in onchip configuration
02/17/10   shr     Fixed Session activate/deactivate handling
02/09/10   kk      Removed "is default app" check for read/write cache
01/11/10   rm      Fix to read only MMGSDI_MAX_APP_INFO number of EF-DIR records
12/18/09   nb      Moved Dual Slot Changes
12/16/09   rn      DF enum access support for mmgsdi_uicc_get_file_attr
12/15/09   ssr     Removed session check for some cdma security commands
12/14/09   ssr     Removed MMGSDI_SELECT_AID_REQ
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
12/07/09   mib     Added RPLMNACT in onchip configuration
12/03/09   shr     Cache read process enhancement
09/26/09   kp      ZI memory reduction changes
09/08/09   rn      Fixed klocwork errors
07/29/09   shr     Fixed Lint Warnings
07/27/09   kk      Fixed Lint Error - Boolean within 'if' always evaluates
                   to False
07/20/09   shr     Block relevant PIN operations if PIN is
                   blocked/permanently blocked
07/20/09   mib     Fixed Lint warning: possible loss of data
                   Added support for XOR and milenage onchip authentication
                   Added support for EPSNSC and KEYSPS files in onchip usim
                   Replaced flag TEST_FRAMEWORK with FEATURE_UIM_TEST_FRAMEWORK
07/08/09   shr     Fixed Lint warnings
07/06/09   shr     Fixed Lint warnings
06/10/09   kk      Added service table check before sending down SELECT
05/12/09   kk      Fixed PIN cmd not sent to card in case UPIN is disabled
05/04/09   mib     Replaced gsdi_uim_sanity_timer_value after onchip.
04/29/09   js      Fixed compiler warnings
04/20/09   kk      Removed featurization which lead to path thrashing
04/06/09   mib     Added support for onchip USIM authenticate
03/26/09   mib     Integration of support for ONCHIP USIM
01/21/09   kk      Featurize function call which checks for EF init in
                   gsdi_usim_path_lookup_table for EF access
09/25/08   js      Fixed Klockwork Error
09/19/08   js      Fixed mmgsdi_uicc_read logic
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/20/08   kk      Calls in create_pkcs15_table made to async
08/06/08   tml     Fixed klocwork issue
07/31/08   kk      Fixing issues related to path length and path in pkcs table
07/20/08   sp      Update the cache properly for read on record based files
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/23/08   sp      Don't clean up the global that was populated as a result
                   of EF-DIR aid read during the reselection of an app.
06/20/08   sp      Add data offset messages for read
03/24/08   sun     Added support for OMA Bcast
03/13/08   tml     fixed memory leak
10/02/07   tml     Added proprietary app session support
07/27/07   sun     Fixed Service Available Calculation
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
03/19/07   sun     Fixed Lint Error
03/07/07   sun     Added support for Disable with Replacement
03/02/07   sun     Check for file Attributes before proceeding to read/write
02/01/07   tml     Null pointer check
01/16/07   tml     Reset mmgsdi status to MMGSDI_SUCCESS after
                   mmgsdi_util_access_path_lookup_table
01/16/07   sk      Return MMGSDI_NOT_FOUND if file is not init in path lookup
                   table.
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
10/20/06   tml     Check if default application before accessing the cache
08/20/06   tml     Fixed Select AID issue
08/07/06   tml     Added ACL support
08/03/06   tml     Passed in generic_data client ID for get pin info
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/07/06   tml     Fixed featurization issue
05/23/06   tml     bdn support
04/18/06   nk      Featurized code
02/15/06   tml     Used read_cache_len to get the cached item len in case
                   user passed in len of 0
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
09/29/05   tml     Greater than 512 byte Cache Read
08/30/05   jar     Added support to send a UICC Status Command
07/26/05   sst     Fixed lint errors
07/18/05   jk      Fixed Compile Issues for 1x Targets
06/16/05   tml     Prototype change for transparent and record access
02/24/05   tml     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_card_init.h"
#include "uim.h"
#include "mmgsdi_ruim_jcdma.h"
#include "mmgsdi_eons.h"
#include "mmgsdisessionlib_v.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_common.h"
#include "mmgsdi_file.h"
#include "mmgsdi_pb_cache.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Offsets & lengths used in AID check */
#define MMGSDI_EF_DIR_TEMPLATE_TAG_OFFSET          (0)
#define MMGSDI_EF_DIR_TEMPLATE_LEN_OFFSET          (1)
#define MMGSDI_EF_DIR_AID_TAG_OFFSET               (2)
#define MMGSDI_EF_DIR_AID_LEN_OFFSET               (3)
#define MMGSDI_EF_DIR_LABEL_TAG_OFFSET(aid_length)   ((aid_length) + 4)
#define MMGSDI_EF_DIR_LABEL_LEN_OFFSET(aid_length)   ((aid_length) + 5)
#define MMGSDI_EF_DIR_LABEL_VALUE_OFFSET(aid_length) ((aid_length) + 6)
#define MMGSDI_EF_DIR_APP_TEMPLATE_MIN_LEN         (3)
#define MMGSDI_EF_DIR_APP_TEMPLATE_ENTRY_MIN_LEN   (MMGSDI_EF_DIR_AID_LEN_OFFSET + 1)


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_READ

  DESCRIPTION:
    This function will provide read access to any file using the UICC
    protocol

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_read should be used

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                            within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                            but the service table indicates the card does not
                            have the support

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_read (
  mmgsdi_read_req_type      * req_ptr
  )
{
  mmgsdi_return_enum_type              mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_data_type                     temp_data_holder;
  int32                                index                = 0;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr      = NULL;
  uim_rpt_type                         report_ptr;
  uint32                               temp_data_len        = 0;
  mmgsdi_int_app_info_type            *prov_app_info_ptr    = NULL;
  mmgsdi_slot_id_enum_type             slot                 = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_cache_init_enum_type          cache_state          = MMGSDI_CACHE_MAX_ENUM;
  boolean                              is_enum_imsi         = FALSE;
  mmgsdi_slot_data_type              * slot_data_ptr        = NULL;
  mmgsdi_onchip_sim_state_enum_type    onchip_state         = MMGSDI_ONCHIP_SIM_STATE_MAX_ENUM;
  boolean                              is_card_session      = FALSE;
  mmgsdi_session_type_enum_type        session_type         = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_nv_context_type               efs_index            = MMGSDI_NV_CONTEXT_INVALID;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  /* initialize temp data */
  temp_data_holder.data_len = 0;
  temp_data_holder.data_ptr = NULL;

  /* Check for if it is EF access only */
  switch (req_ptr->access.access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
  case MMGSDI_BY_PATH_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid Access Type 0x%x",
                                 req_ptr->access.access_method);
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_check_file_access_data(
                                           req_ptr->request_header.session_id,
                                           req_ptr->access,
                                           MMGSDI_UICC))
  {
    UIM_MSG_ERR_0(" File Access is not allowed  ");
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_prov_session_info(req_ptr->request_header.session_id,
                                       NULL,
                                       &slot,
                                       &prov_app_info_ptr) != MMGSDI_SUCCESS)
  {
    /* Check if it is a card session - if so, allow reading of files at
       MF level from cache and from card */
    if(mmgsdi_util_get_session_type(req_ptr->request_header.session_id,
                                    &session_type,
                                    NULL) == MMGSDI_SUCCESS)
    {
      is_card_session = mmgsdi_util_is_card_session(session_type);
    }
    prov_app_info_ptr = NULL;
  }

  /* if it is not default app or if the access method is access by path
     read the data from the card.Check PB cache content first if it is phonebook EF */
  if ((prov_app_info_ptr == NULL &&
       is_card_session == FALSE) ||
      (req_ptr->access.access_method == MMGSDI_BY_PATH_ACCESS
      && mmgsdi_util_is_ef_under_phonebook(&req_ptr->access) == FALSE))
  {
    switch (req_ptr->file_type)
    {
      case MMGSDI_LINEAR_FIXED_FILE:
      case MMGSDI_CYCLIC_FILE:
        return mmgsdi_uim_uicc_read_record(req_ptr, FALSE, NULL);

      case MMGSDI_TRANSPARENT_FILE:
        return mmgsdi_uim_uicc_read_transparent(req_ptr, FALSE, NULL);

      default:
        return MMGSDI_INCORRECT_PARAMS;
    }
  }

  if(prov_app_info_ptr)
  {
    mmgsdi_util_check_usim_spec_version(&req_ptr->access,
                                        prov_app_info_ptr->usim_spec_version);
  }

  mmgsdi_status = MMGSDI_SUCCESS;

  /*---------------------------------------------------------------------------
    Check status of EF in cached UST. For files under MF, skip service table
    check
  ---------------------------------------------------------------------------*/
  if(mmgsdi_file_is_ef_under_mf(&req_ptr->access) == FALSE &&
     prov_app_info_ptr != NULL)
  {
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      prov_app_info_ptr->app_data.app_type,
                      &req_ptr->access );
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      if(req_ptr->access.file.file_enum == MMGSDI_USIM_7F66_PROP1_ACT_HPLMN)
      {
        mmgsdi_status = MMGSDI_ACCESS_DENIED;
      }
      return mmgsdi_status;
    }
  }

#ifdef FEATURE_MMGSDI_3GPP
  if((req_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE) &&
     (req_ptr->access.file.file_enum == MMGSDI_USIM_ECC))
  {
    mmgsdi_status = mmgsdi_cache_read_usim_ecc(req_ptr,&temp_data_holder);
  }
  else
#endif /* FEATURE_MMGSDI_3GPP */
  if((req_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE) &&
     (req_ptr->access.file.file_enum == MMGSDI_USIM_OPL ||
      req_ptr->access.file.file_enum == MMGSDI_USIM_PNN))
  {
    mmgsdi_status = mmgsdi_eons_opl_pnn_read_record(req_ptr->request_header.session_id,
                                     req_ptr->access.file.file_enum,
                                     req_ptr->data_len,
                                     req_ptr->rec_num,
                                     &temp_data_holder,
                                     &cache_state);
  }
  /* Check the phonebook cache content if it EF is under Phonebook directory (5F3A)
     to avoid sending directly to the card */
  else if((req_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE) &&
           mmgsdi_util_is_ef_under_phonebook(&req_ptr->access) &&
           !mmgsdi_util_is_ef_pbr(&req_ptr->access))
  {
    int32 client_index  = 0;

    mmgsdi_status = mmgsdi_util_get_client_id_index(req_ptr->request_header.client_id,
                                                    &client_index);

    /* Disable MMGSDI PB cache from client PBM to avoid unnecessary additional SEARCH commands
       performed by MMGSDI completely.*/
    if(mmgsdi_status == MMGSDI_SUCCESS && !MMGSDIUTIL_IS_PBM_CLIENT(client_index))
    {
      /* Get the phonebook record from cached data */
      mmgsdi_status = mmgsdi_pb_cache_get_record(req_ptr->request_header.slot_id,
                                                 req_ptr->rec_num,
                                                 &req_ptr->access, &temp_data_holder,
                                                 req_ptr->request_header.session_id);
    }
    else
    {
      /* Updating mmgsdi_status to MMGSDI_ERROR explicitly to send to the card
         for the commands coming from PBM.*/
      mmgsdi_status = MMGSDI_ERROR;
    }
  }
  else if ((req_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE) &&
           ((mmgsdi_util_get_efs_item_index(
               req_ptr->request_header.session_id, &efs_index) == MMGSDI_SUCCESS) &&
            (mmgsdi_nv_get_external_feature_status(
               MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG, efs_index) == MMGSDI_FEATURE_ENABLED)) &&
           ((req_ptr->access.file.file_enum == MMGSDI_USIM_ACSGL) ||
            (req_ptr->access.file.file_enum == MMGSDI_USIM_OCSGL) ||
            (req_ptr->access.file.file_enum == MMGSDI_USIM_CSGT)  ||
            (req_ptr->access.file.file_enum == MMGSDI_USIM_OCSGT) ||
            (req_ptr->access.file.file_enum == MMGSDI_USIM_HNBN)  ||
            (req_ptr->access.file.file_enum == MMGSDI_USIM_OHNBN)))
  {
    if(req_ptr->data_len == 0)
    {
      /* The entire record data will be provided */
      mmgsdi_status = mmgsdi_cache_get_record_len(
        req_ptr->request_header.session_id,
        req_ptr->access,
        req_ptr->rec_num,
        &temp_data_holder.data_len);
    }
    else
    {
      temp_data_holder.data_len = req_ptr->data_len;
    }

    /*---------------------------------------------------------------------------
     If successfully retreived file length from cache then
    - Allocate temporary buffer
    - Read from Cache
    ---------------------------------------------------------------------------*/

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      temp_data_len = int32touint32(temp_data_holder.data_len);

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(temp_data_holder.data_ptr,
                                         temp_data_len);
      if (temp_data_holder.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_status = mmgsdi_cache_get_record(req_ptr->request_header.session_id,
                                              req_ptr->access,
                                              req_ptr->rec_num,
                                              &temp_data_holder);
    }
  }
  else if ((req_ptr->file_type == MMGSDI_TRANSPARENT_FILE)  ||
           (((req_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE)  ||
             (req_ptr->file_type == MMGSDI_CYCLIC_FILE))       &&
             (req_ptr->rec_num   == 1)))
  {
    if (req_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS &&
        (req_ptr->access.file.file_enum == MMGSDI_CSIM_IMSI_M ||
         req_ptr->access.file.file_enum == MMGSDI_CSIM_IMSI_T ||
         req_ptr->access.file.file_enum == MMGSDI_USIM_IMSI))
    {
      is_enum_imsi = TRUE;
    }

    /* Check the length of file if cached */
    mmgsdi_status = mmgsdi_cache_read_len(
      req_ptr->request_header.session_id,
      &req_ptr->access,
      &temp_data_holder.data_len,
      req_ptr->offset,
      &cache_state);

    /*---------------------------------------------------------------------------
     If successfully retreived file length from cache then
    - Allocate temporary buffer
    - Read from Cache
    ---------------------------------------------------------------------------*/

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      if (!is_enum_imsi && (req_ptr->data_len != 0) &&
          (req_ptr->data_len < temp_data_holder.data_len))
      {
        temp_data_holder.data_len = req_ptr->data_len;
      }

      temp_data_len = int32touint32(temp_data_holder.data_len);

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(temp_data_holder.data_ptr,
                                         temp_data_len);
      if (temp_data_holder.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_status = mmgsdi_cache_read(req_ptr->request_header.session_id,
                                        &req_ptr->access,
                                        &temp_data_holder,
                                        req_ptr->offset,
                                        &cache_state);
    }
    /* Cache status indicates that the file is not present on the card */
    else if((mmgsdi_status == MMGSDI_NOT_FOUND) &&
            (cache_state == MMGSDI_CACHE_NOT_FOUND))
    {
      return mmgsdi_status;
    }
  }
  else
  {
    /* linear/cyclic file with rec # > 1 */
    mmgsdi_status = MMGSDI_ERROR;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);

    /* Cache status indicates that the file is not present on the card */
    if((mmgsdi_status == MMGSDI_NOT_FOUND) &&
       (cache_state == MMGSDI_CACHE_NOT_FOUND))
    {
      return mmgsdi_status;
    }

    if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SKIP_UICC_SECURITY_ATTR_CHECK_BEFORE_READ, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
         == MMGSDI_FEATURE_DISABLED &&
       prov_app_info_ptr != NULL)
    {
      /*Check to see if the security attributes allow this file to be read*/
      mmgsdi_status = mmgsdi_util_check_file_attr(
                        req_ptr->request_header.client_id,
                        req_ptr->request_header.session_id,
                        slot,
                        prov_app_info_ptr,
                        &req_ptr->access,
                        MMGSDI_READ_REQ,
                        FALSE);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_0("File Read not allowed");
        return mmgsdi_status;
      }
    }

    UIM_MSG_HIGH_1("Sending down UICC READ command to MMGSDI UIM 0x%x",
                   req_ptr->access.file.file_enum);
    /*---------------------------------------------------------------------------
      Read from cache failed
      Send to the card
      ---------------------------------------------------------------------------*/

    switch (req_ptr->file_type)
    {
    case MMGSDI_LINEAR_FIXED_FILE:
    case MMGSDI_CYCLIC_FILE:
      return mmgsdi_uim_uicc_read_record(req_ptr, FALSE, NULL);

    case MMGSDI_TRANSPARENT_FILE:
      return mmgsdi_uim_uicc_read_transparent(req_ptr, FALSE, NULL);

    default:
      return MMGSDI_INCORRECT_PARAMS;
    }
  } /* end if - data did not come from cache */

  /*---------------------------------------------------------------------------
    Read from cache succeed
      1) Check if security attributes allow file to be read
      2) get client request table index
      3) allocate extra parameter
      4) allocate uim cmd pointer
      5) queue report response
      ---------------------------------------------------------------------------*/

  /*Check to see if the security attributes allow this file to be read*/

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(slot_data_ptr != NULL)
  {
    onchip_state = slot_data_ptr->onchip_sim_data.state;
  }

  /* If onchip support is activated, or if the EFS item is set,
     do not check for file attributes */
  if(onchip_state != MMGSDI_ONCHIP_SIM_INIT &&
     prov_app_info_ptr != NULL &&
     mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SKIP_UICC_SECURITY_ATTR_CHECK_BEFORE_READ, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
       == MMGSDI_FEATURE_DISABLED)
  {
    mmgsdi_status = mmgsdi_util_check_file_attr(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      slot,
                      prov_app_info_ptr,
                      &req_ptr->access,
                      MMGSDI_READ_REQ,
                      FALSE);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("File Read not allowed");
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
      return mmgsdi_status;
    }
  }

  UIM_MSG_HIGH_1("Getting data from cache UICC READ command 0x%x",
                 req_ptr->access.file.file_enum);

  /* Data came from cache */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to get free client index, unsuccessful ICC READ command from cache,  file: 0x%x",
                  req_ptr->access.file.file_enum);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
    return mmgsdi_status;
  } /* not successful */

  /*---------------------------------------------------------------------------
    Allocate and Populate Extra Parameter
    - Free Request Table if error in Extra Parameter
    - Free temp data holder if error
  ---------------------------------------------------------------------------*/

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
    MMGSDI_READ_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release temp data  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate Client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                index,
                                                &req_ptr->request_header,
                                                NULL,
                                                extra_param_ptr );
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {

    UIM_MSG_ERR_2("Populate to client req table failed: 0x%x, unsuccessful UICC READ from cache, file: 0x%x",
                  mmgsdi_status, req_ptr->access.file.file_enum);

    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been
                                    populated yet)
    3) Release temp data  (Since client table has not been populated yet)
    4) Release uim cmd data  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/

    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
    return mmgsdi_status;
  }

  /* populate the necessary field in the uim_report_type */
  report_ptr.rpt.read.num_bytes_read =
    int32touint16(temp_data_holder.data_len);
  report_ptr.rpt.read.data           = temp_data_holder.data_ptr;
  report_ptr.rpt_status              = UIM_PASS;

  switch (req_ptr->file_type)
  {
  case MMGSDI_LINEAR_FIXED_FILE:
  case MMGSDI_CYCLIC_FILE:
    report_ptr.rpt_type            = UIM_READ_REC_CMD_R;
    break;

  case MMGSDI_TRANSPARENT_FILE:
    report_ptr.rpt_type            = UIM_READ_BIN_CMD_R;
    break;

  default:
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  report_ptr.sw1                     = SW1_NORMAL_END;
  report_ptr.sw2                     = SW2_NORMAL_END;
  report_ptr.cmd_transacted          = FALSE;
  report_ptr.user_data               = int32touint32(index);

  mmgsdi_uim_report(&report_ptr);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_data_holder.data_ptr);

  return MMGSDI_SUCCESS;
} /* mmgsdi_uicc_read */


/* ============================================================================
  FUNCTION:      MMGSDI_UICC_STATUS

  DESCRIPTION:
    This function will send a status command to card.  It will stay compliant
    to ETSI TS 102.221

    This function returns information concerning the current directory or current
    application.  In addition, according to the application specification,
    it may be used to indicate to the application in the UICC that its
    session activation procedure has been successfully executed or that
    its termination procedure will be executed.
    NOTE: These indications may be used to synchronize the applications in the
    terminal and in the UICC.

    The information is provided asynchronously in the response cnf.

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_status should be used.

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_status (
  const mmgsdi_status_req_type      * req_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  /* --------------------------------------------------------------------------
     Validate the ME Application Indications are valid
     ------------------------------------------------------------------------*/
  if ( ( req_ptr->me_app_status != MMGSDI_STATUS_APP_NO_INDICATION ) &&
       ( req_ptr->me_app_status != MMGSDI_STATUS_APP_INITIALIZED )   &&
       ( req_ptr->me_app_status != MMGSDI_STATUS_APP_TERMINATED ) )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid ME Status Indicator: 0x%x",
                                 req_ptr->me_app_status);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* ---------------------------------------------------------------------------
     Validate the type of return data requested
     -------------------------------------------------------------------------*/
  if ( ( req_ptr->ret_data_struct != MMGSDI_STATUS_DATA_SELECT ) &&
       ( req_ptr->ret_data_struct != MMGSDI_STATUS_TLV_DATA )    &&
       ( req_ptr->ret_data_struct != MMGSDI_STATUS_NO_DATA ) )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid ME requested Data Structure 0x%x",
                                 req_ptr->ret_data_struct);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* ---------------------------------------------------------------------------
     Move into the MMGSDI and UIM Interface to Queue the status command onto
     the UIM Task Command Queue
     -------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_uim_uicc_status(req_ptr, FALSE, NULL, NULL);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Failed UIM UICC STATUS: 0%x", mmgsdi_status);
  }

  return mmgsdi_status;

} /* mmgsdi_uicc_status */


/* ============================================================================
  FUNCTION:      MMGSDI_UICC_CHK_SRV_AVAILABLE

  DESCRIPTION:
    This function checks the SRV AVAILABLE BIT in USIM

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

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_chk_srv_available(
  mmgsdi_client_id_type     client_id,
  mmgsdi_session_id_type    session_id,
  mmgsdi_chk_srv_enum_type  check_type,
  mmgsdi_service_enum_type  srv_type,
  boolean                  *srv_available_ptr)
{
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_access_type                  access;
  mmgsdi_data_type                    data;
  uint32                              offset            = 0x00;
  uint32                              mask              = 0x01;
  uint32                              shift             = 0x01;
  mmgsdi_read_req_type               *req_ptr           = NULL;
  mmgsdi_read_cnf_type               *cnf_ptr           = NULL;
  uint32                              service           = 0;
  mmgsdi_slot_id_enum_type            slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type           *app_info_ptr      = NULL;
  boolean                             need_alloc        = TRUE;
  uint32                              data_len          = 0;
  mmgsdi_app_enum_type                app_type          = MMGSDI_APP_UNKNOWN;
  boolean                             app_selected      = FALSE;
  boolean                             is_gsdi_task      = TRUE;
  mmgsdi_app_state_enum_type          app_state         = MMGSDI_APP_STATE_UNKNOWN;

  MMGSDIUTIL_RETURN_IF_NULL(srv_available_ptr);

  memset(&data, 0x00, sizeof(mmgsdi_data_type));
  memset(&access, 0x00, sizeof(mmgsdi_access_type));

  /* Check if function is invoked in GSDI task context */
  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    is_gsdi_task = FALSE;
  }

  /* Protect read access to client ID reg. table/App. table (only required in non-task context) */
  if(!is_gsdi_task)
  {
    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
  }

  do
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                     &slot_id,
                                                     NULL,
                                                     &app_info_ptr,
                                                     NULL,
                                                     NULL,
                                                     NULL);

    if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
    {
      /* check service failed */
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    if ((slot_id != MMGSDI_SLOT_1) &&
        (slot_id != MMGSDI_SLOT_2) &&
        (slot_id != MMGSDI_SLOT_3))
    {
      UIM_MSG_ERR_1("Invalid slot id 0x%x", slot_id);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Get the App type and if the app is active */
    app_type = app_info_ptr->app_data.app_type;
    app_selected = app_info_ptr->app_selected;
    app_state    = app_info_ptr->app_state;

    /* If app is not selected & the request is to avoid card access,
       return error at this point */
    if((app_selected == FALSE) &&
       (check_type   == MMGSDI_CHK_SRV_FROM_CACHE_ONLY))
    {
      UIM_MSG_ERR_0("App not cached, cannot access card");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Do not use app_info_ptr from this point on */
    app_info_ptr = NULL;
  }while(0);

  if(!is_gsdi_task)
  {
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  }


  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  *srv_available_ptr = FALSE;

  service = (uint32)srv_type;

  access.access_method             = MMGSDI_EF_ENUM_ACCESS;

  switch (app_type)
  {
    case MMGSDI_APP_USIM:
      if ((service < MMGSDI_USIM_SRV) ||
          (service >= (uint32)(MMGSDI_CSIM_SRV)))
      {
        UIM_MSG_ERR_1("Invalid Service Request 0x%x", service);
        return MMGSDI_INCORRECT_PARAMS;
      }
      /*Subtract MMGSDI_USIM_SRV such that the "service" value matches
        directly with the services defeined in the spec
      */
      service = service - MMGSDI_USIM_SRV;
      access.file.file_enum = MMGSDI_USIM_UST;
      break;

    case MMGSDI_APP_CSIM:
      if ((service < MMGSDI_CSIM_SRV) ||
          (service >= (uint32)(MMGSDI_SRV_MAX)))
      {
        UIM_MSG_ERR_1("Invalid Service Request 0x%x", service);
        return MMGSDI_INCORRECT_PARAMS;
      }
      /*Subtract MMGSDI_CSIM_SRV such that the "service" value matches
        directly with the services defeined in the spec
      */
      service = service - MMGSDI_CSIM_SRV;
      access.file.file_enum = MMGSDI_CSIM_CST;
      break;

    case MMGSDI_APP_UNKNOWN:
      UIM_MSG_ERR_0("Unknown App type for UICC service table check");
      return MMGSDI_NOT_SUPPORTED;

    default:
      UIM_MSG_ERR_1("Invalid App type for UICC service table check 0x%x",
                    app_type);
      return MMGSDI_ERROR;
  }

  /* If the app is selected, try to read it from cache.
     Note that for a call that specifies no access to card, we come here
     only when the app is selected */
  if(app_selected)
  {
    /* Protect access to client ID reg. table/App. table */
    if(!is_gsdi_task)
    {
      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
    }

    /* Get the cache length */
    mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                          &access,
                                          &data.data_len,
                                          0,
                                          NULL);

    if(!is_gsdi_task)
    {
      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    }

    if ((mmgsdi_status == MMGSDI_NOT_INIT) &&
        (check_type    == MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE))
    {
      mmgsdi_status = mmgsdi_card_init_cache_binary(
                        session_id,
                        app_type,
                        slot_id,
                        access.file.file_enum,
                        &data);

      /* UST is a mandatory file and some modules might not work correctly if
         it's missing. For this reason, if there is any issue in caching UST,
         simulate its presence, with all the bits set to 0 but only when the
         app_state is beyond PIN state as EF-UST is PIN protected. */
      if (mmgsdi_status != MMGSDI_SUCCESS &&
          access.file.file_enum == MMGSDI_USIM_UST &&
          app_state >= MMGSDI_APP_STATE_READY_FOR_PERSO)
      {
        UIM_MSG_HIGH_0("Issue in caching EF-UST from card. Default caching of UST");
        data.data_len = 5;

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data.data_ptr, data.data_len);
        if (data.data_ptr == NULL)
        {
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }

        memset(data.data_ptr, 0x00, data.data_len);

        mmgsdi_status = mmgsdi_cache_write(session_id,
                                           &access,
                                           data,
                                           0,
                                           MMGSDI_DATA_FROM_CARD_COMPLETE);
      }

      need_alloc = FALSE;
    }

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* not cached or failed to cache */
      UIM_MSG_ERR_1("Cache read Service Table failed 0x%x", mmgsdi_status);

      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(data.data_ptr);
      /* Service status will be initialized during boot-up, return
         mmgsdi_status if the file is not INIT as it is too early */
      return mmgsdi_status;
    }

    if (data.data_len == 0)
    {
      /* Service Table size is zero */
      UIM_MSG_ERR_0("Service Table EF size 0");
      if (!need_alloc)
      {
        /* Already allocated, need to free now */
        MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
      }
      return MMGSDI_ERROR;
    }

    if (need_alloc)
    {
      data_len = int32touint32(data.data_len);

      /* memory allocate the data with the data_len size */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data.data_ptr, data_len);
      if (data.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      /* Protect access to client ID reg. table/App. table */
      if(!is_gsdi_task)
      {
        MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
      }

      mmgsdi_status = mmgsdi_cache_read(session_id,
                                        &access,
                                        &data,
                                        0,
                                        NULL);
      if(!is_gsdi_task)
      {
        MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
      }

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        /* not cached yet */
        MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
        return mmgsdi_status;
      }
    }
  }  /* if provisioning app */
  else
  {
    /* Commands can be sent to UIM in sync mode only in GSDI context */
    if(!is_gsdi_task)
    {
      UIM_MSG_HIGH_0("mmgsdi_uicc_chk_srv_available() invoked in non GSDI context");
      return MMGSDI_NOT_INIT;
    }

    /*Read the File From the Card*/

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr, sizeof(mmgsdi_read_req_type));
    if (req_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    req_ptr->request_header.client_id         = client_id;
    req_ptr->request_header.session_id        = session_id;
    req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
    req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
    req_ptr->request_header.request_len       = uint32toint32(sizeof(mmgsdi_read_req_type));
    req_ptr->request_header.payload_len       = req_ptr->request_header.request_len -
                                               uint32toint32(sizeof(mmgsdi_read_req_type));
    req_ptr->request_header.slot_id           = slot_id;
    req_ptr->request_header.client_data       = 0;
    req_ptr->request_header.response_cb       = NULL;
    mmgsdi_memscpy(&req_ptr->access,
                   sizeof(req_ptr->access),
                   &access,
                   sizeof(mmgsdi_access_type));
    req_ptr->rec_num                          = 0;
    req_ptr->offset                           = 0;
    req_ptr->file_type                        = MMGSDI_TRANSPARENT_FILE;
    req_ptr->data_len                         = 0;

    mmgsdi_status = mmgsdi_uim_uicc_read_transparent(req_ptr,
                                                     TRUE,
                                                     &cnf_ptr);

    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Cnf Ptr Null after read");
      return MMGSDI_ERROR;
    }
    if( (mmgsdi_status == MMGSDI_SUCCESS) &&
        (cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
    {
      data.data_len = cnf_ptr->read_data.data_len;
      data_len = int32touint32(data.data_len);
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data.data_ptr, data_len);
      if (data.data_ptr != NULL)
      {
        mmgsdi_memscpy(data.data_ptr,
                       data_len,
                       cnf_ptr->read_data.data_ptr,
                       int32touint32(cnf_ptr->read_data.data_len));
      }
      else
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)cnf_ptr));
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
  }

  /*   First byte:
        b8  b7  b6  b5  b4  b3  b2  b1
        |   |   |   |   |   |   |   |-- Service n°1
        |   |   |   |   |   |   |-------Service n°2
        |   |   |   |   |   |-----------Service n°3
        |   |   |   |   |---------------Service n°4
        |   |   |   |-------------------Service n°5
        |   |   |-----------------------Service n°6
        |   |---------------------------Service n°7
        |-------------------------------Service n°8
  */
  if(data.data_ptr != NULL)
  {
    offset = service/8;
    shift =  service%8;

    mask = mask << shift;

    if(offset < int32touint32(data.data_len))
    {
      if (((data.data_ptr[offset]) & mask) == mask)
      {
        *srv_available_ptr = TRUE;
      }
      else
      {
        *srv_available_ptr = FALSE;
      }
    }

    UIM_MSG_HIGH_2("Srv Type 0x%x, UICC SRV AVAILABLE - 0x%x",
               srv_type, *srv_available_ptr);

    MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  return mmgsdi_status;
} /* mmgsdi_uicc_chk_srv_available */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_PROCESS_ENABLE_OR_DISABLE_SERVICE

  DESCRIPTION:
    This function will handle enable or disable service request processing

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_process_enable_or_disable_srv (
  mmgsdi_request_header_type  request_header,
  mmgsdi_service_enum_type    service,
  mmgsdi_cmd_enum_type        cmd_type
)
{
  mmgsdi_return_enum_type                       mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_write_req_type                         write_req;
  mmgsdi_data_type                              data;
  boolean                                       srv_available       = FALSE;
  uint32                                        data_len            = 0;
  mmgsdi_session_type_enum_type                 session_type        = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_file_enum_type                         est_file            = MMGSDI_MAX_FILE_ENUM;
  int32                                         index               = 0;
  mmgsdi_client_req_extra_info_type            *extra_param_ptr     = NULL;
  mmgsdi_session_enable_service_extra_info_type enable_srv_extra_info;
  mmgsdi_cache_init_enum_type                   cache_state         = MMGSDI_CACHE_MAX_ENUM;
  mmgsdi_int_app_info_type                     *app_info_ptr        = NULL;

  memset(&data, 0x00, sizeof(mmgsdi_data_type));
  memset(&enable_srv_extra_info,
          0x00,
          sizeof(mmgsdi_session_enable_service_extra_info_type));

  mmgsdi_status = mmgsdi_util_get_session_type(request_header.session_id,
                                               &session_type, NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (!mmgsdi_util_is_prov_session(session_type)))
  {
    UIM_MSG_ERR_1("Not a valid session type: 0x%x", session_type);
    return MMGSDI_ERROR;
  }
  else if(((uint32)service & 0xFF00) == MMGSDI_USIM_SRV &&
    (session_type == MMGSDI_1X_PROV_PRI_SESSION ||
     session_type == MMGSDI_1X_PROV_SEC_SESSION ||
     session_type == MMGSDI_1X_PROV_TER_SESSION))
  {
    UIM_MSG_ERR_2("Error: Trying to enable/disable a service which is not allowed, session type: 0x%x, service: 0x%x",
                  session_type, service);
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if(((uint32)service & 0xFF00) == MMGSDI_CSIM_SRV &&
    (session_type == MMGSDI_GW_PROV_PRI_SESSION ||
     session_type == MMGSDI_GW_PROV_SEC_SESSION ||
     session_type == MMGSDI_GW_PROV_TER_SESSION))
  {
    UIM_MSG_ERR_2("Error: Trying to enable/disable a service which is not allowed, session type: 0x%x, service: 0x%x",
                  service, session_type);
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if ((service != MMGSDI_CSIM_SRV_FDN)&&(service != MMGSDI_USIM_SRV_FDN)&&
           (service != MMGSDI_USIM_SRV_ACL)&&(service != MMGSDI_USIM_SRV_BDN))
  {
    UIM_MSG_ERR_2("Trying to enable/disable a service which is not allowed, session type: 0x%x, service: 0x%x",
                  service, session_type);
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status =  mmgsdi_util_get_prov_session_info(request_header.session_id,
                                                     NULL,
                                                     NULL,
                                                     &app_info_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Nothing to do, no app ptr");
    return MMGSDI_ERROR;
  }

  /* Check if the service is enabled in the service table (EST) */
  mmgsdi_status = mmgsdi_uicc_chk_srv_available(request_header.client_id,
                                                request_header.session_id,
                                                MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                service,
                                                &srv_available);

  if (mmgsdi_status != MMGSDI_SUCCESS || srv_available == FALSE)
  {
    /* Error in enabling service, build command */
    UIM_MSG_ERR_1("Error: CST service check status: 0x%x", mmgsdi_status);
    return MMGSDI_NOT_SUPPORTED;
  }

  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_GW_PROV_TER_SESSION:
      est_file    = MMGSDI_USIM_EST;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION:
    case MMGSDI_1X_PROV_SEC_SESSION:
    case MMGSDI_1X_PROV_TER_SESSION:
      est_file    = MMGSDI_CSIM_EST;
      break;
    default:
      UIM_MSG_ERR_1("Error: Invalid session type: 0x%x", session_type);
      return MMGSDI_INCORRECT_PARAMS;
  }

  /* If the service type is FDN check if the corresponding PIN2 is verified */
  if(service == MMGSDI_USIM_SRV_FDN
    || service == MMGSDI_CSIM_SRV_FDN)
  {
    if(app_info_ptr->pin2.status != MMGSDI_PIN_ENABLED_VERIFIED &&
       app_info_ptr->pin2.status != MMGSDI_PIN_DISABLED)
    {
      UIM_MSG_ERR_1("Cannot continue as PIN2 is not in verified or not in disable state %d",
                    app_info_ptr->pin2.status);
      return MMGSDI_ACCESS_DENIED;
    }
  }

  /* Store the actual command info  */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  } /* not successful */

 /*---------------------------------------------------------------------------
    Allocate and Populate Extra Parameter
    Free Request Table if error in Extra Parameter
  ---------------------------------------------------------------------------*/
  enable_srv_extra_info.service_type  = service;
  enable_srv_extra_info.client_data   = request_header.client_data;
  enable_srv_extra_info.orig_callback = request_header.response_cb;

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                                    cmd_type,
                                    (void*)&enable_srv_extra_info,
                                    &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release temp data  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate Client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                index,
                                                &request_header,
                                                NULL,
                                                extra_param_ptr );
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Enable/Disable service request failed");

  /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been
                                    populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  switch(service)
  {
    case MMGSDI_USIM_SRV_BDN:
      if(((cmd_type == MMGSDI_SESSION_ENABLE_SERVICE_REQ) &&
          (app_info_ptr->app_capabilities.bdn_state == MMGSDI_SRV_ENABLE)) ||
         ((cmd_type == MMGSDI_SESSION_DISABLE_SERVICE_REQ) &&
          (app_info_ptr->app_capabilities.bdn_state == MMGSDI_SRV_DISABLE)))
      {
        extra_param_ptr->enable_service_data.is_status_same = TRUE;
      }
      break;
    case MMGSDI_USIM_SRV_ACL:
      if(((cmd_type == MMGSDI_SESSION_ENABLE_SERVICE_REQ) &&
          (app_info_ptr->app_capabilities.acl_state == MMGSDI_SRV_ENABLE)) ||
         ((cmd_type == MMGSDI_SESSION_DISABLE_SERVICE_REQ) &&
          (app_info_ptr->app_capabilities.acl_state == MMGSDI_SRV_DISABLE)))
      {
        extra_param_ptr->enable_service_data.is_status_same = TRUE;
      }
      break;
    case MMGSDI_USIM_SRV_FDN:
    case MMGSDI_CSIM_SRV_FDN:
      if(((cmd_type == MMGSDI_SESSION_ENABLE_SERVICE_REQ) &&
          (app_info_ptr->app_capabilities.fdn_state == MMGSDI_SRV_ENABLE)) ||
         ((cmd_type == MMGSDI_SESSION_DISABLE_SERVICE_REQ) &&
          (app_info_ptr->app_capabilities.fdn_state == MMGSDI_SRV_DISABLE)))
      {
        extra_param_ptr->enable_service_data.is_status_same = TRUE;
      }
      break;
    default:
      UIM_MSG_ERR_1("Service type 0x%x not expected", service);
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
  }

  if(extra_param_ptr->enable_service_data.is_status_same != FALSE)
  {
    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                      index,
                      request_header.request_type,
                      MMGSDI_SUCCESS);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
    }

    UIM_MSG_HIGH_2("Service type 0x%x is already in requested state with request 0x%x",
                   service, cmd_type);

    return mmgsdi_status;
  }

  mmgsdi_memscpy(&write_req.request_header,
                 sizeof(write_req.request_header),
                 &request_header,
                 sizeof(mmgsdi_request_header_type));

  write_req.request_header.response_cb  = mmgsdi_util_internal_cb;
  /* overwrite the request type to write */
  write_req.request_header.request_type = MMGSDI_WRITE_REQ;
  write_req.access.access_method        = MMGSDI_EF_ENUM_ACCESS;
  write_req.access.file.file_enum       = est_file;

  mmgsdi_status = mmgsdi_cache_read_len(
        write_req.request_header.session_id,
        &write_req.access,
        &data.data_len,
        0,
        &cache_state);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);

    /* Cache status indicates that the file is not present on the card */
    if((mmgsdi_status == MMGSDI_NOT_FOUND) &&
       (cache_state == MMGSDI_CACHE_NOT_FOUND))
    {
      return mmgsdi_status;
    }
    /* not cached yet */
    return MMGSDI_NOT_INIT;
  }

  data_len = int32touint32(data.data_len);
  if(data_len == 0)
  {
    mmgsdi_util_free_client_request_table_index(index);
    UIM_MSG_ERR_0("Can not allocate zero length memory to data.data_ptr");
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data.data_ptr, data_len);

  if(data.data_ptr == NULL)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  mmgsdi_status = mmgsdi_cache_read(
    write_req.request_header.session_id,
    &write_req.access,
    &data,
    0,
    &cache_state);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(data.data_ptr);

    /* Cache status indicates that the file is not present on the card */
    if((mmgsdi_status == MMGSDI_NOT_FOUND) &&
       (cache_state == MMGSDI_CACHE_NOT_FOUND))
    {
      return mmgsdi_status;
    }
    /* not cached yet */
    return MMGSDI_NOT_INIT;
  }

  switch(service)
  {
    case MMGSDI_USIM_SRV_BDN:
      if(cmd_type == MMGSDI_SESSION_ENABLE_SERVICE_REQ)
      {
        data.data_ptr[GSDI_EST_BDN_OFFSET] |= GSDI_EST_BDN_MASK;
      }
      else if(cmd_type == MMGSDI_SESSION_DISABLE_SERVICE_REQ)
      {
        data.data_ptr[GSDI_EST_BDN_OFFSET] &= ~GSDI_EST_BDN_MASK;
      }
      break;
    case MMGSDI_USIM_SRV_ACL:
      if(cmd_type == MMGSDI_SESSION_ENABLE_SERVICE_REQ)
      {
        data.data_ptr[GSDI_EST_ACL_OFFSET] |= GSDI_EST_ACL_MASK;
      }
      else if(cmd_type == MMGSDI_SESSION_DISABLE_SERVICE_REQ)
      {
        data.data_ptr[GSDI_EST_ACL_OFFSET] &= ~GSDI_EST_ACL_MASK;
      }
      break;
    case MMGSDI_USIM_SRV_FDN:
    case MMGSDI_CSIM_SRV_FDN:
      if(cmd_type == MMGSDI_SESSION_ENABLE_SERVICE_REQ)
      {
        data.data_ptr[GSDI_EST_FDN_OFFSET] |= GSDI_EST_FDN_MASK;
      }
      else if(cmd_type == MMGSDI_SESSION_DISABLE_SERVICE_REQ)
      {
        data.data_ptr[GSDI_EST_FDN_OFFSET] &= ~GSDI_EST_FDN_MASK;
      }
      break;
    default:
      break;
  }
  write_req.data.data_len              = data.data_len;
  write_req.data.data_ptr              = data.data_ptr;
  write_req.rec_num                    = 0;
  write_req.offset                     = 0;
  write_req.file_type                  = MMGSDI_TRANSPARENT_FILE;
  write_req.int_client_data            = 0;
  write_req.request_header.client_data = index;

  /* Send Write request to the UIM server */
  mmgsdi_status = mmgsdi_common_write(&write_req, MMGSDI_UICC);

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    mmgsdi_util_free_client_request_table_index(index);
    UIM_MSG_HIGH_1("ERROR Enable/Disable service status 0x%x", mmgsdi_status);
  }
  /* data will be copied over in the extra param with new memory
     allocation, so that this local variable
     does not have to keep track of the pointer allocated */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(data.data_ptr);
  return mmgsdi_status;
}/* mmgsdi_uicc_process_enable_or_disable_srv */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_HIDDENKEY_OPERATION

  DESCRIPTION:
    This function will handle the Hidden Key requests

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_hiddenkey_operation (
  const mmgsdi_pin_operation_req_type * req_ptr
)
{
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type   *extra_param_ptr  = NULL;
  int32                                client_cmd_index = 0;
  mmgsdi_int_app_info_type            *app_info_ptr     = NULL;

 /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  /* Get the app related to the session */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    NULL,
                    &app_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Could not get Prov app info");
    return MMGSDI_ERROR;
  }

  if(app_info_ptr->hiddenkey.status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
  {
    UIM_MSG_ERR_0("Hidden Key status not yet known");
    return MMGSDI_NOT_INIT;
  }
  else if(app_info_ptr->hiddenkey.status == MMGSDI_PIN_NOT_FOUND)
  {
    UIM_MSG_ERR_0("Hiddenkey not present on card");
    return MMGSDI_NOT_FOUND;
  }

  /* Check if the current Hiddenkey state allows the requested operation */
  switch(req_ptr->pin_op)
  {
    case MMGSDI_PIN_OP_VERIFY:
    case MMGSDI_PIN_OP_DISABLE:
    case MMGSDI_PIN_OP_CHANGE:
      if((app_info_ptr->hiddenkey.status != MMGSDI_PIN_ENABLED_NOT_VERIFIED) &&
         (app_info_ptr->hiddenkey.status != MMGSDI_PIN_ENABLED_VERIFIED))
      {
        UIM_MSG_ERR_2("Current Hiddenkey status 0x%x cannot support Hidden Key operation 0x%x",
                      app_info_ptr->hiddenkey.status, req_ptr->pin_op);
        return MMGSDI_ACCESS_DENIED;
      }
      break;
    case MMGSDI_PIN_OP_ENABLE:
      if(app_info_ptr->hiddenkey.status != MMGSDI_PIN_DISABLED)
      {
        UIM_MSG_ERR_2("Current Hiddenkey status 0x%x cannot support Hidden Key operation 0x%x",
                      app_info_ptr->hiddenkey.status, req_ptr->pin_op);
        return MMGSDI_ACCESS_DENIED;
      }
      break;
    default:
      UIM_MSG_ERR_1("Hiddenkey operation 0x%x not supported", req_ptr->pin_op);
      return MMGSDI_INCORRECT_PARAMS;
  }

  /* Verify the Hiddenkey data provided by the client against the card */
  if(req_ptr->pin_op != MMGSDI_PIN_OP_ENABLE)
  {
    /* The current Hiddenkey data passed in by the client is verified against
       the value in the cache */
    mmgsdi_status = mmgsdi_util_verify_hiddenkey(
                      req_ptr->request_header.session_id,
                      &req_ptr->pin_puk);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Hiddenkey verify failed, status 0x%x", mmgsdi_status);
      return MMGSDI_INCORRECT_CODE;
    }
  }

  /* build and Queue the response */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&client_cmd_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to find valid index, 0x%x", mmgsdi_status);
    return mmgsdi_status;
  }

  /*-------------------------------------------------------------------------
  allocate and build extra param ptr
  -------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    MMGSDI_PIN_OPERATION_REQ,
                    (void*)req_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    UIM_MSG_ERR_1("Unable to populate extra param 0x%x", mmgsdi_status);
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    client_cmd_index,
                    &req_ptr->request_header,
                    NULL,
                    extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to populate client request 0x%x", mmgsdi_status);
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                  client_cmd_index, MMGSDI_PIN_OPERATION_REQ, MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return mmgsdi_status;
  }

  return mmgsdi_status;
} /* mmgsdi_uicc_hiddenkey_operation */


/* ============================================================================
  FUNCTION:      MMGSDI_UICC_PIN_STATUS

  DESCRIPTION:
    This function will get the pin status either from the global data if the pin is already cached
    or from the card

  DEPENDENCIES:
  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_status should be used.

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_pin_status (
  const mmgsdi_get_all_pin_status_req_type      * req_ptr
)
{
  mmgsdi_int_app_info_type    *app_info_ptr  = NULL;
  mmgsdi_sw_status_type        status_word   = {0};
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_SUCCESS;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                   - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    NULL,
                    &app_info_ptr,
                    NULL, NULL, NULL);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("PIN status can be retrieved only for active Sessions");
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_is_pin_cached(app_info_ptr))
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,
                                               &req_ptr->request_header,
                                               NULL,
                                               NULL,
                                               FALSE,
                                               status_word);

    UIM_MSG_HIGH_1("PIN status is already cached, built confirmation, status 0x%x",
                   mmgsdi_status);

    return mmgsdi_status;
  }
  else
  {
  return mmgsdi_util_build_pin_req(&req_ptr->request_header,
                                   MMGSDI_PIN1,
                                   MMGSDI_PIN1,
                                   MMGSDI_PIN_OP_UNBLOCK,
                                   MMGSDI_PIN_OP_NONE,
                                   MMGSDI_UICC,
                                   NULL,
                                   NULL);
  }
} /* mmgsdi_uicc_pin_status */


/* ============================================================================
  FUNCTION:      MMGSDI_UICC_PARSE_AID_DATA

  DESCRIPTION:
    This function will parse the aid information to find USIM apps. For all other
    apps, the aid label is stored and the app type is set to MMGSDI_APP_UNKNOWN.

  DEPENDENCIES:
  LIMITATIONS:
    For ICC protocol access, AID is not applicable.

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
   None.

  CRITICAL SECTIONS:
    The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
    to slot data ptr.
============================================================================*/
static mmgsdi_return_enum_type mmgsdi_uicc_parse_aid_data(
  mmgsdi_slot_id_enum_type     slot_id,
  const mmgsdi_read_cnf_type  *read_cnf_ptr,
  uint32                       curr_aid_rec,
  int32                        dir_index)
{
  uint8                                  aid_length        = 0;
  static mmgsdi_key_ref_values_enum_type slot_pin2_key_ref = MMGSDI_SEC_PIN_APPL_1;
  mmgsdi_return_enum_type                mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_slot_data_type                 *slot_data_ptr     = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_2(read_cnf_ptr,
                              read_cnf_ptr->read_data.data_ptr);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Per Spec 102.221, the first 4 bytes & AID are mandatory */
  if (read_cnf_ptr->read_data.data_len < MMGSDI_EF_DIR_APP_TEMPLATE_ENTRY_MIN_LEN)
  {
    UIM_MSG_ERR_1("Invalid length, 0x%x", read_cnf_ptr->read_data.data_len);
    return MMGSDI_ERROR;
  }

  /* Ensure byte [0] = '61', byte [1] is at least 3 bytes &
     byte [2] = '4F' else flag error */
  if((read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_TEMPLATE_TAG_OFFSET] ==
        MMGSDI_APP_TEMPLATE_TAG) &&
     (read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_TEMPLATE_LEN_OFFSET] >=
        MMGSDI_EF_DIR_APP_TEMPLATE_MIN_LEN) &&
     (read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_TEMPLATE_LEN_OFFSET] <=
        (read_cnf_ptr->read_data.data_len - 1)) &&
     (read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_AID_TAG_OFFSET] ==
        MMGSDI_APP_ID_TAG))
  {
    aid_length = read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_AID_LEN_OFFSET];

    /* Validate the AID length present in the incoming data:
       bounds check on the total data & also length per Spec 102.221 */
    if ((aid_length == 0) ||
        (aid_length > UIM_AID_MAX_NAME_LENGTH) ||
        (aid_length > (read_cnf_ptr->read_data.data_len -
                       MMGSDI_EF_DIR_APP_TEMPLATE_ENTRY_MIN_LEN)) ||
        (aid_length > (read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_TEMPLATE_LEN_OFFSET] -
                       (MMGSDI_EF_DIR_AID_LEN_OFFSET - 1))))
    {
      UIM_MSG_ERR_1(" Invalid AID length, 0x%x", aid_length);
      return MMGSDI_ERROR;
    }

    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

    mmgsdi_status = mmgsdi_card_init_populate_app_pin_table(
      slot_id,
      curr_aid_rec,
      slot_pin2_key_ref);

    /* PIN2 key reference is incremented to point to next PIN2 for
       assigning this to next app */
    slot_pin2_key_ref++;

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
      return mmgsdi_status;
    }

    slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.aid.data_len   = aid_length;

    /* Copy the AID into the global data structure which holds the AID data */
    mmgsdi_memscpy(slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.aid.data_ptr,
      sizeof(slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.aid.data_ptr),
      &read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_AID_LEN_OFFSET+1],
      aid_length);

    slot_data_ptr->app_info_pptr[curr_aid_rec]->app_selected = FALSE;
    (void)mmgsdi_util_clear_app_capabilities(&(slot_data_ptr->app_info_pptr[curr_aid_rec]->app_capabilities));
    slot_data_ptr->app_info_pptr[curr_aid_rec]->dir_index = int32touint8(dir_index);

    /* Validate Application label DO before copying: tag, length & data buffer */
    if((read_cnf_ptr->read_data.data_len > (MMGSDI_EF_DIR_LABEL_VALUE_OFFSET(aid_length))) &&
       (read_cnf_ptr->read_data.data_len >=
         (read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_LABEL_LEN_OFFSET(aid_length)] +
          MMGSDI_EF_DIR_LABEL_VALUE_OFFSET(aid_length))) &&
       (read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_TEMPLATE_LEN_OFFSET] >
          MMGSDI_EF_DIR_LABEL_TAG_OFFSET(aid_length)) &&
       (read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_LABEL_TAG_OFFSET(aid_length)] ==
          MMGSDI_UICC_APP_LABEL_TAG) &&
       (read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_LABEL_LEN_OFFSET(aid_length)] <=
          MMGSDI_UICC_APP_LABEL_MAX_LENGTH))
    {
      slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.label.data_len =
              read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_LABEL_LEN_OFFSET(aid_length)];
      mmgsdi_memscpy(slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.label.data_ptr,
        sizeof(slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.label.data_ptr),
        &read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_LABEL_VALUE_OFFSET(aid_length)],
        int32touint32(slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.label.data_len));
    }

    if (mmgsdi_util_is_usim_aid(&read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_AID_LEN_OFFSET+1],
                                aid_length))
    {
      slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.app_type = MMGSDI_APP_USIM;
      slot_data_ptr->app_info_pptr[curr_aid_rec]->proprietary_app   = FALSE;
    }
    else if (mmgsdi_util_is_csim_aid(&read_cnf_ptr->read_data.data_ptr[MMGSDI_EF_DIR_AID_LEN_OFFSET+1],
                                     aid_length))
    {
      slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.app_type = MMGSDI_APP_CSIM;
      slot_data_ptr->app_info_pptr[curr_aid_rec]->proprietary_app   = FALSE;
      if (MMGSDI_FEATURE_ENABLED == mmgsdi_nv_get_jcdma_service_status(slot_id))
      {
        /* Determine if its a JCDMA CSIM App and if so copy over the AID */
        UIM_MSG_HIGH_0("JCDMA Service is enabled, copy the AID");
        (void)mmgsdi_jcdma_find_csim_app_for_color_and_lock(read_cnf_ptr);
      }
    }
    else /* The App is of an unknown type ergo a proprietary app */
    {
      slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.app_type = MMGSDI_APP_UNKNOWN;
      slot_data_ptr->app_info_pptr[curr_aid_rec]->proprietary_app   = TRUE;
    }
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  }
  else
  {
    UIM_MSG_ERR_0(" Invalid tag parsing for AID");
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_uicc_parse_aid_data */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_GET_AVAILABLE_AIDS

  DESCRIPTION:
    This function will read the EF-DIR to find out if there are any UICC
    Applications in the card. It will however return only the number of
    USIM applications back to the caller and also store the information for
    all the apps in the global mmgsdi_data_slot1. If EF-DIR is not found
    on the card, ef_dir_not_found_ptr is set to TRUE.

  DEPENDENCIES:

  LIMITATIONS:
    For ICC protocol access, this is not applicable.

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                            within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                            but the service table indicates the card does not
                            have the support

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_get_available_aids(
  mmgsdi_slot_id_enum_type     slot_id,
  mmgsdi_slot_data_type      * slot_data_ptr,
  int32                      * aid_count_ptr,
  boolean                    * fail_reason_card_error_ptr,
  boolean                    * ef_dir_not_found_ptr)
{
  mmgsdi_return_enum_type              mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_read_req_type               * read_req_ptr         = NULL;
  mmgsdi_read_cnf_type               * read_cnf_ptr         = NULL;
  int32                                current_rec          = 0;
  int32                                total_recs           = 0;
  uint32                               actual_recs          = 0;
  uint8                                slot_index           = MMGSDI_SLOT_1_INDEX;
  mmgsdi_access_type                   access;
  mmgsdi_attr_cache_element_type     * attr_cache_ptr       = NULL;
  mmgsdi_sw_status_enum_type           sw_present           = MMGSDI_STATUS_WORD_INVALID;

  UIM_MSG_HIGH_0("MMGSDI_UICC_GET_AVAILABLE_AIDS");

  MMGSDIUTIL_RETURN_IF_NULL_2(slot_data_ptr, aid_count_ptr);

  memset(&access, 0x00, sizeof(access));

  /* Initailize fail_reason_card_error to FALSE */
  if (fail_reason_card_error_ptr != NULL)
  {
    *fail_reason_card_error_ptr = FALSE;
  }

  /* Initailize ef_dir_not_found to FALSE */
  if (ef_dir_not_found_ptr != NULL)
  {
    *ef_dir_not_found_ptr = FALSE;
  }

  /*---------------------------------------------------------------------------
    1) Get number or records in EF-DIR
    3) Read the records
    4) Parse the record and extract the AID
    5) Store the AIDs in mmgsdi_data_slot1
  ---------------------------------------------------------------------------*/
  access.access_method   = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum  = MMGSDI_DIR;

  /* Instead of sending a SELECT to the card, see if the attribtues are present
     in the attributes cache already. If the items are not present in the
     attributes cache, this function call will send a SELECT to the card and
     upon success, will cache the attributes */
  mmgsdi_status = mmgsdi_cache_populate_attr_cache_if_needed(
                    mmgsdi_generic_data_ptr->client_id,
                    mmgsdi_generic_data_ptr->slot_session_id[slot_index],
                    slot_id,
                    TRUE,
                    FALSE,
                    &access,
                    &attr_cache_ptr,
                    &sw_present);

  /* If sw_present is absent, it means UIMDRV has given cmd_transacted as FALSE
     and so in that case we don't need to do reset with protocol switch.
     Also, if status is MMGSDI_UIM_CMD_TIMEOUT it means card has not given
     the response and in that case we don't need to switch protocol */
  if ((fail_reason_card_error_ptr != NULL) &&
      ((mmgsdi_status == MMGSDI_UIM_CMD_TIMEOUT)||
       (mmgsdi_status != MMGSDI_SUCCESS && sw_present == MMGSDI_STATUS_WORD_ABSENT)))
  {
    *fail_reason_card_error_ptr = TRUE;
  }

  if(mmgsdi_status == MMGSDI_NOT_FOUND &&
     ef_dir_not_found_ptr != NULL)
  {
    *ef_dir_not_found_ptr = TRUE;
  }

  if(attr_cache_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     attr_cache_ptr->file_type != MMGSDI_LINEAR_FIXED_FILE ||
     attr_cache_ptr->num_of_rec == 0)
  {
    UIM_MSG_ERR_0("Select EF DIR failed or no Record Found");
    mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
    return MMGSDI_ERROR;
  }

  /* Copy the number of records in EF-DIR */
  total_recs = attr_cache_ptr->num_of_rec;

  mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);

  /* No variable len to include */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
                                     sizeof(mmgsdi_read_req_type));

  if (read_req_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Limit reading only MMGSDI_MAX_EXT_APP_INFO number of AIDs */
  for(current_rec=1; (current_rec<=total_recs) &&
                     (current_rec <= MMGSDI_MAX_EXT_APP_INFO); current_rec++)
  {
    read_req_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
    read_req_ptr->request_header.session_id        = mmgsdi_generic_data_ptr->slot_session_id[slot_index];
    read_req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
    read_req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
    read_req_ptr->request_header.request_len       = sizeof(mmgsdi_read_req_type);
    read_req_ptr->request_header.payload_len       = read_req_ptr->request_header.request_len -
                                                       uint32toint32(sizeof(mmgsdi_read_req_type));
    read_req_ptr->request_header.slot_id           = slot_id;
    read_req_ptr->request_header.client_data       = 0;
    read_req_ptr->request_header.response_cb       = NULL;
    read_req_ptr->access.access_method   = MMGSDI_EF_ENUM_ACCESS;
    read_req_ptr->access.file.file_enum  = MMGSDI_DIR;
    read_req_ptr->file_type              = MMGSDI_LINEAR_FIXED_FILE;
    read_req_ptr->rec_num                = current_rec;
    read_req_ptr->data_len               = 0;/*Entire Record*/

    mmgsdi_status = mmgsdi_uim_uicc_read_record(read_req_ptr, TRUE, &read_cnf_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS ||
       read_cnf_ptr == NULL ||
       read_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Read EF DIR failed for record 0x%x", read_req_ptr->rec_num);

      /* status_word.present indicate the card status, if it is false that
         means cmd does not send to the card, it may be possible that card
         is removed and UIM is in error state. mmgsdi_state should be SUCCESS
         to confirm that read_cnf_ptr->response_header is correctly populated.
         In that case, there is no need to send RESET to card
         for switching protocol from UICC to ICC and so set
         fail_reason_card_error_ptr to TRUE.*/
      if ((fail_reason_card_error_ptr != NULL) &&
          (read_cnf_ptr != NULL) &&
          (read_cnf_ptr->response_header.status_word.present == FALSE) &&
          (mmgsdi_status == MMGSDI_SUCCESS))
      {
        UIM_MSG_ERR_0(" Card may be removed while reading EF-DIR Records");
        *fail_reason_card_error_ptr = TRUE;
      }
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_ptr);

      /* free the content of the cnf before freeing the cnf_ptr itself */
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);
      return MMGSDI_ERROR;
    }
    mmgsdi_status = mmgsdi_uicc_parse_aid_data(slot_id,
                                               read_cnf_ptr,
                                               actual_recs,
                                               current_rec);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_HIGH_0("Could not be parsed for AID record");
      mmgsdi_status = MMGSDI_SUCCESS;
    }
    else
    {
      actual_recs++;
    }

    /* free the content of the cnf before freeing the cnf_ptr itself */
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);
  }

  *aid_count_ptr = actual_recs;

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_ptr);
  return mmgsdi_status;
} /* mmgsdi_uicc_get_available_aids */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_IS_SERVICE_AVAILABLE

  DESCRIPTION:
    This function will find out if the service is enabled in UST

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_is_service_available (
  const mmgsdi_srv_available_req_type   *req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_ERROR;
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                   - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param( MMGSDI_SRV_AVAILABLE_REQ,
                                                            (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    UIM_MSG_ERR_0("Could not Malloc Extra Param Ptr");
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index,
                                                                  &req_ptr->request_header,
                                                                  NULL,
                                                                  extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                req_ptr->request_header.session_id,
                                                MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                req_ptr->service,
                                                &extra_param_ptr->srv_available_data.srv_available);


  /*Queue the Response*/
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp( index, MMGSDI_SRV_AVAILABLE_REQ,
                                                             MMGSDI_SUCCESS);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  else
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_uicc_is_service_available */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_ONCHIP_AUTH

  DESCRIPTION:
    This function executes the onchip usim authentication

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_onchip_auth (
  const mmgsdi_auth_req_type   *req_ptr
)
{
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr = NULL;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  uim_rpt_type                       uim_report;
  mmgsdi_slot_data_type             *slot_data_ptr   = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  memset(&uim_report, 0x00, sizeof(uim_rpt_type));

  /* Check context: in onchip mode, we support only GSM and 3G contexts */
  if (req_ptr->auth_context != MMGSDI_AUTN_USIM_GSM_CONTEXT &&
      req_ptr->auth_context != MMGSDI_AUTN_USIM_3G_CONTEXT)
  {
    UIM_MSG_ERR_1("Invalid context value: 0x%x", req_ptr->auth_context);
    return MMGSDI_ERROR;
  }

  /* Get a free index */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Unable to get free client index");
    return mmgsdi_status;
  }

  /* Populate the extra param */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
        MMGSDI_USIM_AUTH_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* Populate the request table */
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                  index,
                                  &req_ptr->request_header,
                                  NULL,
                                  extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* populate the necessary field in the uim_report_type */
  if (slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW1_OFFSET] !=
      (uint8)SW1_NO_STATUS)
  {
    /* Error response */
    uim_report.sw1 =
      (uim_sw1_type)slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW1_OFFSET];
    uim_report.sw2 =
      (uim_sw2_type)slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW2_OFFSET];
    uim_report.rpt_status = UIM_FAIL;
  }
  else if (slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_len > 0 &&
           slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_ptr != NULL)
  {
    /* AUTS response */
    uim_report.sw1        = SW1_NORMAL_END;
    uim_report.sw2        = SW2_NORMAL_END;
    uim_report.rpt_status = UIM_PASS;

    if (slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_len <= UIM_MAX_CHARS)
    {
      mmgsdi_memscpy(uim_report.rpt.autn.data,
        sizeof(uim_report.rpt.autn.data),
        slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_ptr,
        int32touint32(slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_len));
      uim_report.rpt.autn.data_length =
        int32touint16(slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_len);
    }
    else
    {
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
    }
  }
  else if (slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_len > 0 &&
           slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_ptr != NULL &&
           req_ptr->auth_context == MMGSDI_AUTN_USIM_3G_CONTEXT)
  {
    /* 3G security context */
    uim_report.sw1        = SW1_NORMAL_END;
    uim_report.sw2        = SW2_NORMAL_END;
    uim_report.rpt_status = UIM_PASS;

    if (slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_len <= UIM_MAX_CHARS)
    {
      mmgsdi_memscpy(uim_report.rpt.autn.data,
        sizeof(uim_report.rpt.autn.data),
        slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_ptr,
        int32touint32(slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_len));
      uim_report.rpt.autn.data_length =
        int32touint16(slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_len);
    }
    else
    {
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
    }
  }
  else if (slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_len > 0 &&
           slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_ptr != NULL &&
           req_ptr->auth_context == MMGSDI_AUTN_USIM_GSM_CONTEXT)
  {
    /* GSM security context */
    uim_report.sw1        = SW1_NORMAL_END;
    uim_report.sw2        = SW2_NORMAL_END;
    uim_report.rpt_status = UIM_PASS;

    if (slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_len <= UIM_MAX_CHARS)
    {
      mmgsdi_memscpy(uim_report.rpt.autn.data,
        sizeof(uim_report.rpt.autn.data),
        slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_ptr,
        int32touint32(slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_len));
      uim_report.rpt.autn.data_length =
        int32touint16(slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_len);
    }
    else
    {
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
    }
  }
  else
  {
    /* Dafault case */
    mmgsdi_util_free_client_request_table_index(index);
    return MMGSDI_ERROR;
  }

  /* Set the other values in uim_report */
  uim_report.rpt_type            = UIM_AUTHENTICATE_R;
  uim_report.cmd_transacted      = FALSE;
  uim_report.user_data           = int32touint32(index);

  /* Queue response */
  mmgsdi_uim_report(&uim_report);

  return mmgsdi_status;
} /* mmgsdi_uicc_onchip_auth */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_AUTH

  DESCRIPTION:
    This function will allow USIM Authentication command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application


  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_auth (
  const mmgsdi_auth_req_type   *req_ptr
)
{
  mmgsdi_slot_data_type         *slot_data_ptr           = NULL;
  mmgsdi_return_enum_type       mmgsdi_status            = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type session_type             = MMGSDI_MAX_SESSION_TYPE_ENUM;
  boolean                       service_avail            = FALSE;
  boolean                       sec_context_check_ok     = FALSE;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_type(req_ptr->request_header.session_id,
                                               &session_type, NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(session_type == MMGSDI_GW_PROV_PRI_SESSION ||
     session_type == MMGSDI_GW_PROV_SEC_SESSION ||
     session_type == MMGSDI_GW_PROV_TER_SESSION)
  {
    /* For GW prov sessions check if the relevant service in EF-UST is available
       for the security context of the request*/
    switch(req_ptr->auth_context)
    {
      case MMGSDI_AUTN_USIM_3G_CONTEXT:
        /* no service available check required for 3G Security Context */
        sec_context_check_ok = TRUE;
        break;
      case MMGSDI_AUTN_USIM_GSM_CONTEXT:
        /* Check MMGSDI_USIM_SRV_GSM_SECURITY_CONTEXT (service no.38) is available */
        mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                      req_ptr->request_header.session_id,
                                                      MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                      MMGSDI_USIM_SRV_GSM_SECURITY_CONTEXT,
                                                      &service_avail);
        if (mmgsdi_status == MMGSDI_SUCCESS && service_avail)
        {
          sec_context_check_ok = TRUE;
        }
        break;
      case MMGSDI_AUTN_USIM_VGCS_VBS_CONTEXT:
        /* Check both MMGSDI_USIM_SRV_VGCS (service no.57) &
           MMGSDI_USIM_SRV_VGCS_SECURITY (service no.64) are available
           or.. that both MMGSDI_USIM_SRV_VBS (service no.58) &
           MMGSDI_USIM_SRV_VBS_SECURITY (service no.65) are available */
        mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                      req_ptr->request_header.session_id,
                                                      MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                      MMGSDI_USIM_SRV_VGCS,
                                                      &service_avail);
        if (mmgsdi_status == MMGSDI_SUCCESS && service_avail)
        {
          mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                        req_ptr->request_header.session_id,
                                                        MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                        MMGSDI_USIM_SRV_VGCS_SECURITY,
                                                        &service_avail);
          if (mmgsdi_status == MMGSDI_SUCCESS && service_avail)
          {
            sec_context_check_ok = TRUE;
            break;
          }
        }

        /* first pair (service no.57 & no.64) failed,
           try second pair (service no.58 & no.65) */
        mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                      req_ptr->request_header.session_id,
                                                      MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                      MMGSDI_USIM_SRV_VBS,
                                                      &service_avail);
        if (mmgsdi_status == MMGSDI_SUCCESS && service_avail)
        {
          mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                        req_ptr->request_header.session_id,
                                                        MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                        MMGSDI_USIM_SRV_VBS_SECURITY,
                                                        &service_avail);
          if (mmgsdi_status != MMGSDI_SUCCESS && service_avail)
          {
            sec_context_check_ok = TRUE;
          }
        }
        break;
      case MMGSDI_AUTN_USIM_GBA_BOOTSTRAPPING_CONTEXT:
      case MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT:
        /* Check MMGSDI_USIM_SRV_GBA (service no.68) is available */
        mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                      req_ptr->request_header.session_id,
                                                      MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                      MMGSDI_USIM_SRV_GBA,
                                                      &service_avail);
        if (mmgsdi_status == MMGSDI_SUCCESS && service_avail)
        {
          sec_context_check_ok = TRUE;
        }
        break;
      case MMGSDI_AUTN_USIM_MBMS_MSK_UPDATE_CONTEXT:
      case MMGSDI_AUTN_USIM_MBMS_MTK_GEN_CONTEXT:
      case MMGSDI_AUTN_USIM_MBMS_MSK_DEL_CONTEXT:
      case MMGSDI_AUTN_USIM_MBMS_MUK_DEL_CONTEXT:
      case MMGSDI_AUTN_USIM_MBMS_SPE_DEL_CONTEXT:
      case MMGSDI_AUTN_USIM_MBMS_REC_DEL_CONTEXT:
        /* Check MMGSDI_USIM_SRV_MBMS_SECURITY (service no.69) is available */
        mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                      req_ptr->request_header.session_id,
                                                      MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                      MMGSDI_USIM_SRV_MBMS_SECURITY,
                                                      &service_avail);
        if (mmgsdi_status == MMGSDI_SUCCESS && service_avail)
        {
           sec_context_check_ok = TRUE;
        }
        break;
      default:
        UIM_MSG_ERR_1("Invalid Context Type: 0x%x", req_ptr->auth_context );
        return MMGSDI_ERROR;
    }

    if (sec_context_check_ok == FALSE)
    {
      UIM_MSG_ERR_1("Security Context check failed. auth_context: 0x%x",
                    req_ptr->auth_context );
      return MMGSDI_NOT_SUPPORTED;
    }
  }

  /* Security context check passed or was not required, so continue and get
     the slot data */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(
                    req_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
  {
    /* OnChip authenticate command */
    return mmgsdi_uicc_onchip_auth(req_ptr);
  }

  /* Send USIM authenticate command to UIM server */
  return mmgsdi_uim_uicc_auth(req_ptr);
} /* mmgsdi_uicc_auth */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_BCAST

  DESCRIPTION:
    This function will allow USIM BCAST command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application


  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_bcast (
  const mmgsdi_bcast_req_type   *req_ptr
)
{
  /* Send Bcast Command to the card */
  return mmgsdi_uim_uicc_bcast(req_ptr);

} /* mmgsdi_uicc_bcast */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_GET_CAPABILITIES

  DESCRIPTION:
    This function will provide capabilities for fdn, bdn, acl and phonebook
    pbr decoding procedure for UICC

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_get_capabilities (
  mmgsdi_session_id_type    session_id,
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_app_enum_type      app_type
)
{
  mmgsdi_return_enum_type   mmgsdi_status         = MMGSDI_SUCCESS;
  mmgsdi_service_enum_type  fdn_srv               = MMGSDI_NONE;
  mmgsdi_service_enum_type  bdn_srv               = MMGSDI_NONE;
  mmgsdi_service_enum_type  cc_srv                = MMGSDI_NONE;
  mmgsdi_service_enum_type  acl_srv               = MMGSDI_NONE;
  boolean                   fdn_avail             = FALSE;
  boolean                   bdn_avail             = FALSE;
  boolean                   cc_avail              = FALSE;
  mmgsdi_int_app_info_type *app_info_ptr          = NULL;
  boolean                   acl_enabled           = FALSE;

  UIM_MSG_HIGH_0("mmgsdi_uicc_get_capabilities");

  mmgsdi_status =  mmgsdi_util_get_prov_session_info(session_id,
                                                     NULL,
                                                     NULL,
                                                     &app_info_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Nothing to do, no app ptr");
    return MMGSDI_ERROR;
  }

  if(app_info_ptr->app_capabilities.all_srv_init == TRUE)
  {
    UIM_MSG_HIGH_0("All services are in init state");
    return MMGSDI_SUCCESS;
  }

  switch(app_type)
  {
    case MMGSDI_APP_USIM:
      fdn_srv        = MMGSDI_USIM_SRV_FDN;
      bdn_srv        = MMGSDI_USIM_SRV_BDN;
      cc_srv         = MMGSDI_USIM_SRV_CALL_CONTROL;
      acl_srv        = MMGSDI_USIM_SRV_ACL;
      break;
    case MMGSDI_APP_CSIM:
      fdn_srv        = MMGSDI_CSIM_SRV_FDN;
      cc_srv         = MMGSDI_CSIM_SRV_CALL_CONTROL;
      /* CSIM does not have BDN and ACL EF and does not have
         service table bits for these*/
      break;
    default:
      UIM_MSG_ERR_1("Invalid app type 0x%x for uicc get pbm cap", app_type);
      return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_uicc_chk_srv_enabled(mmgsdi_generic_data_ptr->client_id,
                                              session_id,
                                              fdn_srv,
                                              &fdn_avail);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if (fdn_avail)
  {
    /*Spec 31.102 - Section 5.1.1.3 - In case FDN is enabled, an ME which does not
      support FDN shall allow emergency calls but shall not allow MO-CS calls
      and MO-SMS.*/
    if ( !mmgsdi_generic_data_ptr->me_capabilities.me_supports_fdn )
    {
      UIM_MSG_ERR_1("App type 0x%x FDN Enabled and ME does not support FDN",
                    app_type);

      /* Update the session closed cause. The session changed event
             broadcasts the cause */
      app_info_ptr->session_closed_cause =
                 MMGSDI_SESSION_CLOSED_CAUSE_FDN_SRV_ENABLED_WITH_NO_FDN_SUPPORT;
      return MMGSDI_ERROR;
    }
  }

  UIM_MSG_HIGH_1("FDN service status: 0x%x", fdn_avail);

  if(fdn_avail == TRUE)
  {
    app_info_ptr->app_capabilities.fdn_state = MMGSDI_SRV_ENABLE;
  }
  else
  {
    app_info_ptr->app_capabilities.fdn_state = MMGSDI_SRV_DISABLE;
  }

  if (bdn_srv != MMGSDI_NONE)
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_enabled(mmgsdi_generic_data_ptr->client_id,
                                                session_id,
                                                bdn_srv,
                                                &bdn_avail);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }

    mmgsdi_status = mmgsdi_uicc_chk_srv_available(mmgsdi_generic_data_ptr->client_id,
                                                  session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  cc_srv,
                                                  &cc_avail);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }
    if ( bdn_avail && cc_avail )
    {
      /* Spec 31.102 Section 4.2.44: EF BDN:
         As the BDN service relies on the Call Control feature,
         BDN shall only be available if Call Control is available. */
      app_info_ptr->app_capabilities.bdn_state = MMGSDI_SRV_ENABLE;
      UIM_MSG_HIGH_0("BDN service is Enabled");
    }
    else
    {
      app_info_ptr->app_capabilities.bdn_state = MMGSDI_SRV_DISABLE;
      UIM_MSG_HIGH_0("BDN service is Disabled");
    }
  }

  /* Check ACL service.  Note, CSIM does not have ACL service bit */
  if (acl_srv != MMGSDI_NONE)
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_enabled(mmgsdi_generic_data_ptr->client_id,
                                                session_id,
                                                acl_srv,
                                                &acl_enabled);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }

    UIM_MSG_HIGH_1("ACL service status: 0x%x", acl_enabled);

    if(acl_enabled)
    {
      app_info_ptr->app_capabilities.acl_state = MMGSDI_SRV_ENABLE;
    }
    else
    {
      app_info_ptr->app_capabilities.acl_state = MMGSDI_SRV_DISABLE;
    }
  }

  app_info_ptr->app_capabilities.all_srv_init = TRUE;
  UIM_MSG_HIGH_0("All services are initialized");

  return MMGSDI_SUCCESS;
} /* mmgsdi_uicc_get_capabilities */


/* ============================================================================
  FUNCTION:      MMGSDI_UICC_CHK_SRV_ENABLED

  DESCRIPTION:
    This function checks the SRV ENABLED BIT in EST

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_chk_srv_enabled(
  mmgsdi_client_id_type     client_id,
  mmgsdi_session_id_type    session_id,
  mmgsdi_service_enum_type  srv_type,
  boolean                  *srv_enabled_ptr)
{
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_access_type                  access;
  mmgsdi_data_type                    data;
  uint32                              mask              = 0x01;
  mmgsdi_read_req_type               *req_ptr           = NULL;
  mmgsdi_read_cnf_type               *cnf_ptr           = NULL;
  mmgsdi_slot_id_enum_type            slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type           *app_info_ptr      = NULL;
  boolean                             need_alloc        = TRUE;
  uint32                              data_len          = 0;
  boolean                             srv_avail         = FALSE;

  MMGSDIUTIL_RETURN_IF_NULL(srv_enabled_ptr);

  memset(&data, 0x00, sizeof(mmgsdi_data_type));
  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  *srv_enabled_ptr = FALSE;

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot_id,
                                                   NULL,
                                                   &app_info_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* check service failed */
    return mmgsdi_status;
  }

  /* Make sure app_info_ptr received from the util is not null */
  MMGSDIUTIL_RETURN_IF_NULL(app_info_ptr);

  if ((slot_id != MMGSDI_SLOT_1) &&
      (slot_id != MMGSDI_SLOT_2) &&
      (slot_id != MMGSDI_SLOT_3))
  {
    UIM_MSG_ERR_1("Invalid slot id 0x%x", slot_id);
    return MMGSDI_ERROR;
  }

  access.access_method             = MMGSDI_EF_ENUM_ACCESS;

  switch (srv_type)
  {
    case MMGSDI_USIM_SRV_FDN:
      access.file.file_enum        = MMGSDI_USIM_EST;
      mask = GSDI_EST_FDN_MASK;
      break;
    case MMGSDI_CSIM_SRV_FDN:
      access.file.file_enum        = MMGSDI_CSIM_EST;
      mask = GSDI_EST_FDN_MASK;
      break;
    case MMGSDI_USIM_SRV_BDN:
      access.file.file_enum        = MMGSDI_USIM_EST;
      mask = GSDI_EST_BDN_MASK;
      break;
    case MMGSDI_USIM_SRV_ACL:
      access.file.file_enum        = MMGSDI_USIM_EST;
      mask = GSDI_EST_ACL_MASK;
      break;
    default:
      UIM_MSG_ERR_1("No entry in EST for service 0x%x", srv_type);
      return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                session_id,
                                                MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                srv_type,
                                                &srv_avail);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  if (srv_avail != TRUE)
  {
    /* EST not available hence per 31.102 and C.S0065 services are disabled */
    UIM_MSG_HIGH_0("Service not available. Hence cannot enable");
    return mmgsdi_status;
  }

  /* Check if the app is selected, if it is, that means we have
     the service table cached */
  if(app_info_ptr->app_selected)
  {
    /* Get the cache length */
    mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                          &access,
                                          &data.data_len,
                                          0,
                                          NULL);
    if (mmgsdi_status == MMGSDI_NOT_INIT)
    {
      mmgsdi_status = mmgsdi_card_init_cache_binary(
                        session_id,
                        app_info_ptr->app_data.app_type,
                        slot_id,
                        access.file.file_enum,
                        &data);
      need_alloc = FALSE;
    }

    /* If EF-EST is not found or access denied, just indicate that
       the service is disable and return success in case of
       misconfigured card */
    if (mmgsdi_status == MMGSDI_NOT_FOUND ||
        (mmgsdi_status == MMGSDI_ACCESS_DENIED &&
          app_info_ptr->app_state > MMGSDI_APP_STATE_PIN_EVT_SENT))
    {
      *srv_enabled_ptr = FALSE;
      return MMGSDI_SUCCESS;
    }


    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* not cached or failed to cache */
      UIM_MSG_ERR_1("Cache read Service Table failed 0x%x", mmgsdi_status);
      return mmgsdi_status;
    }

    if (data.data_len == 0)
    {
      /* Service Table size is zero */
      UIM_MSG_ERR_0("Service Table EF size 0");
      if (!need_alloc)
      {
        /* Already allocated, need to free now */
        MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
      }
      return MMGSDI_ERROR;
    }

    if (need_alloc)
    {
      data_len = int32touint32(data.data_len);

      /* memory allocate the data with the data_len size */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data.data_ptr, data_len);
      if (data.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_status = mmgsdi_cache_read(session_id,
                                        &access,
                                        &data,
                                        0,
                                        NULL);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        /* not cached yet */
        MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
        return mmgsdi_status;
      }
    }
  }  /* if provisioning app */
  else
  {
    /*Read the File From the Card*/

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr, sizeof(mmgsdi_read_req_type));
    if (req_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    req_ptr->request_header.client_id         = client_id;
    req_ptr->request_header.session_id        = session_id;
    req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
    req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
    req_ptr->request_header.request_len       = uint32toint32(sizeof(mmgsdi_read_req_type));
    req_ptr->request_header.payload_len       = req_ptr->request_header.request_len -
                                               uint32toint32(sizeof(mmgsdi_read_req_type));
    req_ptr->request_header.slot_id           = slot_id;
    req_ptr->request_header.client_data       = 0;
    req_ptr->request_header.response_cb       = NULL;
    mmgsdi_memscpy(&req_ptr->access,
                   sizeof(req_ptr->access),
                   &access,
                   sizeof(mmgsdi_access_type));
    req_ptr->rec_num                          = 0;
    req_ptr->offset                           = 0;
    req_ptr->file_type                        = MMGSDI_TRANSPARENT_FILE;
    req_ptr->data_len                         = 0;

    mmgsdi_status = mmgsdi_uim_uicc_read_transparent(req_ptr,
                                                     TRUE,
                                                     &cnf_ptr);

    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Cnf Ptr Null after read");
      return MMGSDI_ERROR;
    }

    /* If EF-EST is not found or access denied, just indicate that
       the service is disable and return success in case of
       misconfigured card */
    if ((mmgsdi_status == MMGSDI_SUCCESS) &&
        ((cnf_ptr->response_header.mmgsdi_status == MMGSDI_NOT_FOUND) ||
         (mmgsdi_status == MMGSDI_ACCESS_DENIED &&
           app_info_ptr->app_state > MMGSDI_APP_STATE_PIN_EVT_SENT)))
    {
      *srv_enabled_ptr = FALSE;
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)cnf_ptr));
      MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
      return MMGSDI_SUCCESS;
    }

    if( (mmgsdi_status == MMGSDI_SUCCESS) &&
        (cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
    {
      data.data_len = cnf_ptr->read_data.data_len;
      data_len = int32touint32(data.data_len);
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data.data_ptr, data_len);
      if (data.data_ptr != NULL)
      {
        mmgsdi_memscpy(data.data_ptr,
                       data_len,
                       cnf_ptr->read_data.data_ptr,
                       int32touint32(cnf_ptr->read_data.data_len));
      }
      else
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)cnf_ptr));
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
  }

  /*   First byte of EST:
        b8  b7  b6  b5  b4  b3  b2  b1
        |   |   |   |   |   |   |   |-- FDN
        |   |   |   |   |   |   |-------BDN
        |   |   |   |   |   |-----------ACL
        |   |   |   |   |---------------RFU
        |   |   |   |-------------------RFU
        |   |   |-----------------------RFU
        |   |---------------------------RFU
        |-------------------------------RFU
  */
  if(data.data_ptr != NULL)
  {
    if (((*data.data_ptr) & mask) == mask)
    {
      *srv_enabled_ptr = TRUE;
    }
    else
    {
      *srv_enabled_ptr = FALSE;
    }

    UIM_MSG_HIGH_2("Srv Type - 0x%x, UICC SRV Enabled - 0x%x",
                   srv_type, *srv_enabled_ptr);

    MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  return mmgsdi_status;
} /* mmgsdi_uicc_chk_srv_enabled */


/*===========================================================================
FUNCTION MMGSDI_UICC_GSTK_LOC_ENV_RSP_RECEIVED

DESCRIPTION
  This function, called in mmgsdi_process_command.
  If card has responded to VPE with an IMSI switch, an internal
  refresh will be triggered by MMGSDI if MMGSDI has a stale value
  of IMSI in its cache.

DEPENDENCIES
  None

LIMITATIONS
  This command is only supported for USIM

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_gstk_loc_env_rsp_received(
  const mmgsdi_gstk_loc_env_rsp_received_req_type *gstk_loc_env_rsp_received_req_ptr)
{
  mmgsdi_return_enum_type             mmgsdi_status              = MMGSDI_SUCCESS;
  mmgsdi_session_id_type              session_id;
  mmgsdi_client_id_type               client_id;
  mmgsdi_access_type                  access;
  mmgsdi_int_app_info_type           *app_info_ptr               = NULL;
  mmgsdi_data_type                    mmgsdi_data_from_cache     = {0, NULL};
  uint8                               imsi_data[MMGSDI_IMSI_LEN];
  mmgsdi_data_type                    mmgsdi_data_from_card      = {0, NULL};
  mmgsdi_read_req_type               *read_req_ptr               = NULL;
  mmgsdi_read_cnf_type               *read_cnf_ptr               = NULL;
  mmgsdi_data_type                    refresh_files              = {0, NULL};
  mmgsdi_option_type                  option;
  mmgsdi_slot_data_type              *slot_data_ptr              = NULL;
  int32                               client_cmd_index           = 0;
  boolean                             trigger_refresh            = TRUE;
  mmgsdi_client_req_extra_info_type  *extra_param_ptr            = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_2(gstk_loc_env_rsp_received_req_ptr,
                              mmgsdi_generic_data_ptr);

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  memset(&imsi_data, 0x00, sizeof(imsi_data));
  memset(&mmgsdi_data_from_cache, 0x00, sizeof(mmgsdi_data_type));
  memset(&mmgsdi_data_from_card, 0x00, sizeof(mmgsdi_data_type));
  memset(&option,0x00,sizeof(mmgsdi_option_type));

  /* Use mmgsdi's gw session id for all the procedures */
  client_id = gstk_loc_env_rsp_received_req_ptr->request_header.client_id;
  switch(gstk_loc_env_rsp_received_req_ptr->request_header.slot_id)
  {
    case MMGSDI_SLOT_1:
      session_id = mmgsdi_generic_data_ptr->pri_gw_session_id;
      break;
    case MMGSDI_SLOT_2:
      session_id = mmgsdi_generic_data_ptr->sec_gw_session_id;
      break;
    case MMGSDI_SLOT_3:
      session_id = mmgsdi_generic_data_ptr->ter_gw_session_id;
      break;
    default:
      UIM_MSG_ERR_1("mmgsdi_gstk_loc_env_response_received: Invalid SlotId=0x%x",
                    gstk_loc_env_rsp_received_req_ptr->request_header.slot_id);
      return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(gstk_loc_env_rsp_received_req_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /*----------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
   -----------------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&client_cmd_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ,
                    (void*)gstk_loc_env_rsp_received_req_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to populate extra param 0x%x", mmgsdi_status);
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    client_cmd_index,
                    &gstk_loc_env_rsp_received_req_ptr->request_header,
                    NULL,
                    extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /* Increment the gstk loc envelope (successful VPE) count */
  slot_data_ptr->gstk_loc_env_rsp_count ++;

  /* Proceed only if it is the first time the API is called by GSTK. */
  if(slot_data_ptr->gstk_loc_env_rsp_count != 1)
  {
    UIM_MSG_ERR_1("mmgsdi_gstk_loc_env_rsp_received: API called multiple times",
                  slot_data_ptr->gstk_loc_env_rsp_count);
    extra_param_ptr->gstk_loc_env_data.imsi_switched = FALSE;
    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                      client_cmd_index, MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ,
                      MMGSDI_SUCCESS);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(client_cmd_index);
      return mmgsdi_status;
    }
    return MMGSDI_SUCCESS;
  }

  /* Get the app corresponding to the session */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                    session_id,
                    NULL,
                    NULL,
                    &app_info_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return mmgsdi_status;
  }

  /* Get the appropriate IMSI EF based on the app type.
     Currently support only USIM app type. */
  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  switch(app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_USIM:
      access.file.file_enum = MMGSDI_USIM_IMSI;
      break;
    default:
      UIM_MSG_ERR_1("Invalid app_type=0x%x", app_info_ptr->app_data.app_type);
      mmgsdi_util_free_client_request_table_index(client_cmd_index);
      return MMGSDI_ERROR;
  }

  /* See if IMSI is currently read into the cache */
  /* Read current IMSI from MMGSI cache */
  mmgsdi_status = mmgsdi_session_read_cache_file_size(
                    session_id,
                    access.file.file_enum,
                    &mmgsdi_data_from_cache.data_len);
  if(mmgsdi_status == MMGSDI_NOT_FOUND)
  {
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    /* This is not a failure case as currently there's no IMSI in the cache */
    return MMGSDI_SUCCESS;
  }
  else if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return mmgsdi_status;
  }

  if(mmgsdi_data_from_cache.data_len > MMGSDI_IMSI_LEN)
  {
    UIM_MSG_ERR_1("Length of the IMSI file exceeded the MAX IMSI EF Length: 0x%x",
                  mmgsdi_data_from_cache.data_len);
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return MMGSDI_ERROR;
  }

  mmgsdi_data_from_cache.data_ptr = imsi_data;

  mmgsdi_status = mmgsdi_session_read_cache(
                    session_id,
                    access.file.file_enum,
                    mmgsdi_data_from_cache);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("read_cache for IMSI failed: 0x%x", mmgsdi_status);
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return mmgsdi_status;
  }

  /* At this point we have verified that an IMSI exists in the mmgsdi cache.
     Now we need to read EF IMSI from the card and compare it with the this
     cached IMSI. If they are not the same, we need to trigger INIT_FCN
     refresh. */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
    sizeof(mmgsdi_read_req_type));
  if (read_req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  read_req_ptr->request_header.client_id         = client_id;
  read_req_ptr->request_header.session_id        = session_id;
  read_req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
  read_req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
  read_req_ptr->request_header.request_len       = uint32toint32(sizeof(mmgsdi_read_req_type));
  read_req_ptr->request_header.payload_len       = read_req_ptr->request_header.request_len -
                                                     uint32toint32(sizeof(mmgsdi_request_header_type));
  read_req_ptr->request_header.slot_id           = gstk_loc_env_rsp_received_req_ptr->request_header.slot_id;
  read_req_ptr->request_header.client_data       = 0;
  read_req_ptr->request_header.response_cb       = NULL;
  read_req_ptr->rec_num                          = 0;
  read_req_ptr->offset                           = 0;
  read_req_ptr->file_type                        = MMGSDI_TRANSPARENT_FILE;
  read_req_ptr->data_len                         = 0;
  mmgsdi_memscpy(&read_req_ptr->access,
                 sizeof(read_req_ptr->access),
                 &access,
                 sizeof(mmgsdi_access_type));

  mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                   TRUE,
                                                   &read_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);

  if(read_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("cnf ptr invalid after read");
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return MMGSDI_ERROR;
  }

  if((read_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS) ||
     (mmgsdi_status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_1("read_transparent on EF IMSI failed: 0x%x", MMGSDI_ERROR);
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return MMGSDI_ERROR;
  }

  /* Lets copy from read_cnf_ptr the data received from the card */
  mmgsdi_data_from_card.data_len = read_cnf_ptr->read_data.data_len;
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_data_from_card.data_ptr,
                                     (mmgsdi_data_from_card.data_len));
  if(mmgsdi_data_from_card.data_ptr != NULL)
  {
    mmgsdi_memscpy(mmgsdi_data_from_card.data_ptr,
                   int32touint32(mmgsdi_data_from_card.data_len),
                   read_cnf_ptr->read_data.data_ptr,
                   int32touint32(read_cnf_ptr->read_data.data_len));
  }
  else
  {
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
    MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));

  /* Lets compare the IMSI with the IMSI that MMGSDI already has cached. */
  if(mmgsdi_data_from_card.data_len != mmgsdi_data_from_cache.data_len)
  {
    UIM_MSG_HIGH_2("The two IMSI lengths are different:Card=0x%x, Cached=0x%x",
                   mmgsdi_data_from_card.data_len,
                   mmgsdi_data_from_cache.data_len);
  }
  else
  {
    if(!memcmp(mmgsdi_data_from_card.data_ptr,
               mmgsdi_data_from_cache.data_ptr,
               mmgsdi_data_from_card.data_len))
    {
      UIM_MSG_HIGH_0("The two IMSIs match! Refresh not triggered");
      trigger_refresh = FALSE;
      extra_param_ptr->gstk_loc_env_data.imsi_switched = FALSE;
    }
  }

  if(trigger_refresh)
  {
    /* The two IMSIs are diferent. Lets trigger an internal INIT_FFCN refresh */
    UIM_MSG_HIGH_0("Two IMSIs are different! Triggering Internal INIT_FFCN refresh...");
    mmgsdi_status = mmgsdi_naa_refresh_ext(client_id,
                                           gstk_loc_env_rsp_received_req_ptr->request_header.slot_id,
                                           app_info_ptr->app_data,
                                           MMGSDI_REFRESH_NAA_INIT_FULL_FCN,
                                           refresh_files,
                                           0,
                                           NULL,
                                           option,
                                           MMGSDI_REFRESH_SOURCE_MMGSDI,
                                           mmgsdi_util_internal_cb, 0);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("mmgsdi_naa_refresh request for INIT_FCN failed: 0x%x",
                    mmgsdi_status);
      extra_param_ptr->gstk_loc_env_data.imsi_switched = FALSE;
    }
    else
    {
      extra_param_ptr->gstk_loc_env_data.imsi_switched = TRUE;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_from_card.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                  client_cmd_index, MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ, MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
    return mmgsdi_status;
  }

  return mmgsdi_status;
} /* mmgsdi_uicc_gstk_loc_env_rsp_received */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_SET_FILE_STATUS

  DESCRIPTION:
    This function will set the status of the EF to activated or deactivated

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using
    this function

  LIMITATIONS:
    This function is limited to the use of UICC.

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_set_file_status (
  const mmgsdi_set_file_status_req_type  *req_ptr,
  mmgsdi_cmd_enum_type                    req_type
)
{
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  /* Check for if it is EF access only */
  switch (req_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
    case MMGSDI_BY_PATH_ACCESS:
      break;

    default:
      MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid Access Type 0x%x",
                                   req_ptr->access.access_method);
      return MMGSDI_ERROR;
  }

  /* Send Invalidate request to the UIM server */
  return mmgsdi_uim_uicc_set_file_status(req_ptr,
                                         req_type);
} /* mmgsdi_uicc_set_file_status */

