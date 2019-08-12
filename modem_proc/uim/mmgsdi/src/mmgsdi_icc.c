/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC protocol processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_icc.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   ar      Send FDN event source along with FDN event
06/08/16   ar      Send FDN event if FDN status changed
05/24/16   ar      Review of macros used by MMGSDI
05/23/16   dd      Skip Search Req for PB files
05/16/16   ar      F3 reduction
11/16/15   nr      Updating fdn status in FCN handling based on ADN activation status
10/26/15   nr      Disable MMGSDI PB cache for PBM module
04/17/15   nr      Phonebook record read optimization
01/09/15   ar      Cache file if req_len is more than file size
10/10/14   vdc     Expose NV for CDMA select before reading ADN under TELECOM
09/09/14   kk      Enhancements in NV handling
08/11/14   bcho    Optimize srv req by using cached srv table in client context
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/28/14   vv      Remove unused data types
07/15/14   yt      Support for NV refresh
06/26/14   vv      Fix for a possible memory leak
06/18/14   av      Validate cdmahome EF attributes during power up
05/27/14   av      Don't allow access if UST/CST can't be read(pin not verified)
04/08/14   ar      Skip sending event to clients if service status is same
03/27/14   yt      Ensure usage of correct slot id
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
02/10/14   ar      Replace session checks with utility functions
12/05/13   tkl     SFI Support and decouple uim file enums
01/21/14   yt      Remove duplicate code
01/16/14   df      Add new sim phase enumeration
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/04/13   lxu     Add slot id for card session in mmgsdi_icc_read
11/12/13   yt      Fixes for multi-SIM OnChip activation
11/06/13   tl      Remove TO-DO
10/22/13   vv      Fix to read GSM OPL/PNN files from cache
10/18/13   vv      Fix to ensure a response for ICC READ requests
10/04/13   rp      F3 message reduction
10/02/13   av      Do not activate onchip for non-test IMSIs
09/26/13   av      Enable OnChip on bootup based on EFS item
09/24/13   yt      Remove function mmgsdi_util_is_gsdi_task_context()
09/20/13   yt      Remove duplicate code
09/03/13   tl      Reduce On-chip SIM activation delay between stages
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/07/13   tl      Add CDMA AKA Authenticate functionality
07/26/13   av      Skip security attribs check during ICC READ requests
07/26/13   av      Do not allow disabling PIN1 if SST is not cached yet
07/09/13   vdc     Allocate memory for slot data while activating onchip ICC
06/24/13   vdc     Added support for triple SIM
05/30/13   akv     Heart beat mechanism for Watchdog
05/21/13   vv      Added support for session closed cause
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
04/25/13   vdc     Select DFCDMA before reading TELECOM ADN for special card
04/17/13   tl      Replace Featurization with NV
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
03/04/13   av      Allow card sessions to access cache for READ
12/30/12   kk      Fixed TP State check during imsi/loci rehab
12/05/12   vv      Added support for parallel processing in MMGSDI
10/12/12   abg     Do not send provisioning events to non-prov sessions
10/11/12   spo     Removed unnecessary F3 messages
10/01/12   vs      Added utility for context check including logic for SGLTE
09/28/12   abg     Updated ERR to MSG_ERROR
09/03/12   bcho    mmgsdi_icc_search modified to support sync operation,
                   Read record from OPL-PNN cache
05/16/12   bcho    When clients try to enable/disable a service and respective
                   serivce is already enabled/disbaled, do not free extra
                   param pointer.
05/15/12   vv      Perform IMSI/LOCI rehab after TP download is done
04/11/12   yt      Perform NULL check before dereferencing pointer
04/09/12   yt      Reject PIN operations for non-prov app in ICC mode
03/28/12   bcho    rex_sleep() is replaced with sleep mechanism based on
                   non-deferrable timer
03/16/12   yt      Modify references to gsdi_task_tcb
03/12/12   vv      Fixed klocwork errors
03/07/12   bcho    NULL check removed for mmgsdi_channel_info_table_ptr
03/01/12   bcho    Redundant session info extraction function call removed
02/23/12   av      Cleaned up reamining always ON features
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
01/14/12   bcho    Thread safe changes
01/10/12   vv      Perform IMSI or LOCI rehab depending on it's invalidity
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   kk      Legacy GSDI removal updates
10/28/11   yt      Added new files for cache-related utility functions
10/13/11   shr     Adding support for full recovery
09/28/11   vv      Allow initialization if card is missing mandatory EF-PHASE
08/01/11   vs      Updates for key ref based PIN operations
05/20/11   yt      Added support for INCREASE request
05/17/11   kk      Updates for Thread safety
05/17/11   shr     Updates for Thread safety
05/11/11   nb      Support for Async TP
04/07/11   at      Fix for handling card sessions in mmgsdi_icc_get_file_attr
02/21/11   ms      Implementation of Session based caching on App capabilities
01/31/11   ssr     Added file access check
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/26/10   kk      Free client request table entry if a SRV available fails
11/05/10   yt      Klocwork fixes
10/21/10   shr     Cleanup of Onchip USIM support
10/14/10   yt      Updated handling of client req table entries in case of error
09/17/10   yk      Added support dynamic registration on WDOG.
09/15/10   nmb     Allocate mmgsdi_client_id_reg_table dynamically
08/09/10   adp     IMSI should always be read from the cache, expect for the
                   first time.
08/05/10   yt      Check for permission to disable CHV1 based on app type
07/26/10   shr     Refresh events sent out as part of onchip activation
                   need to be session based. Also, sessions need to be
                   cleaned-up if already active before onchip activation
07/13/10   yt      Corrected sanity timer value for Onchip
06/25/10   shr     Clean-up of Onchip support
05/24/10   nmb     Get empty client request table spot before using table
05/19/10   nb      Updated call to mmgsdi_uim_icc_write_transparent
05/04/10   shr     Removed temporary Feature Flags
05/03/10   shr     Fixed incorrect decoding of EF-SST
04/26/10   ssr     Fixed Disable PIN for RUIM card
04/22/10   nb      Reintroduced service table check before sending down SELECT
04/07/10   yt      Fixed compilation warnings
12/18/09   nb      Moved Dual Slot Changes
12/16/09   rn      DF enum access support for mmgsdi_icc_get_file_attr
12/11/09   kk      Fixed IMSI rehab check
12/07/09   js      Fixed lint error
12/04/09   kp      Replacing mmgsdi_data_slot2 with mmgsdi_data_slot2_ptr
12/03/09   shr     Cache read process enhancement
12/02/09   js      Perform correct IMSI/LOCI rehab depending on FDN status
09/26/09   kp      ZI memory reduction changes
07/27/09   kk      Fixed Lint Error - Boolean within 'if' always evaluates
                   to False
07/20/09   shr     Block relevant PIN operations if PIN is
                   blocked/permanently blocked
07/08/09   shr     Fixed Lint warnings
06/10/09   kk      Added service table check before sending down SELECT
05/04/09   mib     Replaced gsdi_uim_sanity_timer_value after onchip.
03/26/09   mib     Integration of support for ONCHIP USIM
03/12/09   mib     Fixed issue during onchip procedure
03/02/09   mib     activate_onchip_sim: pass original refresh mode
09/25/08   js      Fixed Klockwork Error
09/22/08   js      Fixed Lint Errors
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/20/08   jk      Support for Generate Key API
08/20/08   kk      Calls in create_pkcs15_table made to async
07/31/08   kk      Fixing issues related to path length and path in pkcs table
07/23/08   sp      Initialize the data_ptr
07/20/08   sp      Update the cache properly for read on record based files
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/20/08   sp      Add data offset messages for read
03/03/08   nk      Updated call to mmgsdi_uim_icc_select and
                   mmgsdi_uim_icc_rehabilitate
02/14/08   vs      Added PIN events in mmgsdi_icc_activate_onchip_sim()
01/25/07   vs      Fixed compile error
12/17/07   vs      Featurized function mmgsdi_icc_run_cave()
08/28/07   tml     Fixed onchip sim support.  Ensure events are properly built
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
07/27/07   sun     Fixed Srv Available calculation
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
03/19/07   sun     Fixed Lint Error
03/10/07   sun     Added support for Disable with Replacement
03/08/07   tml     Compilation fix
03/02/07   sun     Check for file Attributes before proceeding to read/write
02/27/07   sun     Removed Global PIN from GSDI
02/22/07   tml     Fixed on chip sim not able to allow other command to
                   proceed issue
01/17/07   sp      Removed pin_just_verified flag
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access
10/17/06   tml     Check for disable CHV1 allowed or not before Disable CHV1
                   action is sent to the card
10/08/06   tml     Fixed compilation error
10/02/06   sp      Changed me supports fdn from gsdi to mmgsdi global data
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     Lint
05/23/06   tml     BDN, rehabilitate and invalidate supports
04/15/06   tml     moved onchip member to global data structure and compilation
                   warning fix
04/16/06   jar     Added ONCHIP SIM Support
02/15/06   tml     Used read_cache_len to get the cached item len in case
                   user passed in len of 0
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
09/29/05   tml     Greater than 512 byte Cache Read
08/30/05   jar     Added Support for sending Status Commands to the Card
07/26/05   sst     Fixed lint errors
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version

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
#include "mmgsdi_evt.h"
#include "mmgsdi_nv.h"
#include "mmgsdicache.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_file.h"
#include "mmgsdi_pb_cache.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* 1st byte in SST */
#define SST_PIN_DISABLING_OFFSET            0x00

#define SST_PIN_DISABLING_MASK              0x03



/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_READ

   DESCRIPTION:
     This function will provide read access to any file using the ICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read should be used

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_read (
  const mmgsdi_read_req_type      * req_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_data_type                    temp_data_holder;
  int32                               index                = 0;
  uim_cmd_type                      * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr      = NULL;
  uim_rpt_type                        report_ptr;
  uint32                              temp_data_len        = 0;
  mmgsdi_int_app_info_type          * prov_app_info_ptr    = NULL;
  mmgsdi_slot_id_enum_type            slot                 = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_info_type          * session_info_ptr     = NULL;
  mmgsdi_cache_init_enum_type         cache_state          = MMGSDI_CACHE_MAX_ENUM;
  boolean                             is_enum_imsi         = FALSE;
  mmgsdi_slot_data_type             * slot_data_ptr        = NULL;
  mmgsdi_onchip_sim_state_enum_type   onchip_state         = MMGSDI_ONCHIP_SIM_STATE_MAX_ENUM;
  boolean                             is_card_session      = FALSE;
  mmgsdi_session_type_enum_type       session_type         = MMGSDI_MAX_SESSION_TYPE_ENUM;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
                                              - MMGSDI_BY_PATH_ACCESS
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
                                           MMGSDI_ICC))
  {
    UIM_MSG_ERR_0(" File Access is not allowed  ");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    req_ptr->request_header.session_id,&session_info_ptr,NULL);

  if ((session_info_ptr == NULL) || (mmgsdi_status != MMGSDI_SUCCESS))
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("Error finding session and channel info");
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
      switch (session_type)
      {
        case MMGSDI_CARD_SESSION_SLOT_1:
          slot = MMGSDI_SLOT_1;
          is_card_session = TRUE;
          break;

        case MMGSDI_CARD_SESSION_SLOT_2:
          slot = MMGSDI_SLOT_2;
          is_card_session = TRUE;
          break;

        case MMGSDI_CARD_SESSION_SLOT_3:
          slot = MMGSDI_SLOT_3;
          is_card_session = TRUE;
          break;
        default:
          is_card_session = FALSE;
      }
    }
    prov_app_info_ptr = NULL;
  }

  /* Allow cache access either only if it is card or a prov session AND the
     access method is BY_ENUM. Check PB cache content first if it is phonebook EF */
  if (mmgsdi_util_is_ef_under_phonebook(&req_ptr->access) ||
      ((req_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS) &&
      (prov_app_info_ptr != NULL||
       is_card_session)))
  {
    /*---------------------------------------------------------------------------
      Check status of EF in cached SST or CST. For files under MF, skip service
      table check
    ---------------------------------------------------------------------------*/
    if(mmgsdi_file_is_ef_under_mf(&req_ptr->access) == FALSE &&
       prov_app_info_ptr != NULL)
    {
      mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
        req_ptr->request_header.client_id,
        req_ptr->request_header.session_id,
        prov_app_info_ptr->app_data.app_type,
        &req_ptr->access );
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
    }

    if((req_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE) &&
       (req_ptr->access.file.file_enum == MMGSDI_GSM_OPL ||
        req_ptr->access.file.file_enum == MMGSDI_GSM_PNN))
    {
      mmgsdi_status = mmgsdi_eons_opl_pnn_read_record(req_ptr->request_header.session_id,
                                                      req_ptr->access.file.file_enum,
                                                      req_ptr->data_len,
                                                      req_ptr->rec_num,
                                                      &temp_data_holder,
                                                      &cache_state);
    }
    /* Check the phonebook cache content if the EF is Telecom ADN (3F00 7F10 6F3A) 
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
                                                   &req_ptr->access,
                                                   &temp_data_holder,
                                                   req_ptr->request_header.session_id);
      }
      else
      {
        /* Updating mmgsdi_status to MMGSDI_ERROR explicitly to send to the card
           for the commands coming from PBM.*/
        mmgsdi_status = MMGSDI_ERROR;
      }
    }
    else if((req_ptr->file_type == MMGSDI_TRANSPARENT_FILE) ||
            (((req_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE)  ||
              (req_ptr->file_type == MMGSDI_CYCLIC_FILE))
             && (req_ptr->rec_num == 1)))
    {
      if (req_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS &&
          (req_ptr->access.file.file_enum == MMGSDI_CDMA_IMSI_M ||
           req_ptr->access.file.file_enum == MMGSDI_CDMA_IMSI_T ||
           req_ptr->access.file.file_enum == MMGSDI_GSM_IMSI))
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

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        if (!is_enum_imsi && (req_ptr->data_len != 0) &&
            (req_ptr->data_len < temp_data_holder.data_len))
        {
          temp_data_holder.data_len = req_ptr->data_len;
        }
        temp_data_len = int32touint32(temp_data_holder.data_len);

        /* 'temp_data_len' could possibly be zero here */
        if(temp_data_len == 0)
        {
          UIM_MSG_ERR_0("Can not allocate zero length memory to temp_data_holder.data_ptr");
          return MMGSDI_ERROR;
        }
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
      if(prov_app_info_ptr != NULL &&
         mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SKIP_UICC_SECURITY_ATTR_CHECK_BEFORE_READ, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
           == MMGSDI_FEATURE_DISABLED)
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

      UIM_MSG_HIGH_1("Sending down ICC READ command to MMGSDI UIM 0x%x",
                     req_ptr->access.file.file_enum);
      /*---------------------------------------------------------------------------
        Read from cache failed
        Send to the card
        ---------------------------------------------------------------------------*/
      switch (req_ptr->file_type)
      {
        case MMGSDI_LINEAR_FIXED_FILE:
        case MMGSDI_CYCLIC_FILE:
          return mmgsdi_uim_icc_read_record(req_ptr, FALSE, NULL);

        case MMGSDI_TRANSPARENT_FILE:
          return mmgsdi_uim_icc_read_transparent(req_ptr, FALSE, NULL);

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

    /* If onchip support is activated, or if app info ptr is NULL, do not check
       for file attributes */
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

    UIM_MSG_HIGH_1("Getting data from cache ICC READ command 0x%x",
                   req_ptr->access.file.file_enum);

    /* Data came from cache */
    mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Unable to get free client index");
      UIM_MSG_ERR_1("Unsuccessful ICC READ command from cache 0x%x",
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
    if (mmgsdi_status != MMGSDI_SUCCESS)
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
                                                uim_cmd_ptr,
                                                extra_param_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {

      UIM_MSG_ERR_1("Populate to client req table failed 0x%x", mmgsdi_status);
      UIM_MSG_ERR_1("Unsuccessful ICC READ command from cache 0x%x",
                    req_ptr->access.file.file_enum);

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

    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else if ((req_ptr->access.access_method == MMGSDI_BY_PATH_ACCESS)||
           mmgsdi_util_is_card_session(session_info_ptr->session_type) ||
           mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
  {
    UIM_MSG_HIGH_0("Sending down ICC READ command to MMGSDI UIM 0x%x");
    /*---------------------------------------------------------------------------
      EF accessed by PATH, do not go through cache and send to the card directly
    ---------------------------------------------------------------------------*/
    switch (req_ptr->file_type)
    {
      case MMGSDI_LINEAR_FIXED_FILE:
      case MMGSDI_CYCLIC_FILE:
        return mmgsdi_uim_icc_read_record(req_ptr, FALSE, NULL);

      case MMGSDI_TRANSPARENT_FILE:
        return mmgsdi_uim_icc_read_transparent(req_ptr, FALSE, NULL);

      default:
        return MMGSDI_INCORRECT_PARAMS;
    }
  }
  else
  {
    /* This ensures a response for the request */
    return MMGSDI_ERROR;
  }

  return mmgsdi_status;
} /* mmgsdi_icc_read */


/* ============================================================================
  FUNCTION:      MMGSDI_ICC_STATUS

  DESCRIPTION:
    This function will send a status command to card.  It will stay compliant
    to GSM 11.11 or IS820C

    TThis function returns information concerning the current directory.
    A current EF is not affected by the STATUS function. It is also used
    to give an opportunity for a pro active SIM to indicate that the
    SIM wants to issue a SIM Application Toolkit command to the ME.

    The information is provided asynchronously in the response cnf.
    Which will contain information containing:  file ID, total memory space
    available, CHV enabled/disabled indicator, CHV status and other
    GSM/CDMA specific data (identical to SELECT).

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
mmgsdi_return_enum_type  mmgsdi_icc_status (
  mmgsdi_status_req_type      * req_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  /* --------------------------------------------------------------------------
     The me_app_status and ret_data_struct types are not applicable to a
     GSM SIM.  Therfore, ignore the inputs.
     ------------------------------------------------------------------------*/
  if ( ( req_ptr->me_app_status   != MMGSDI_STATUS_APP_NO_INDICATION ) &&
       ( req_ptr->ret_data_struct != MMGSDI_STATUS_NONE ) )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Ignoring Inputs:  Invalid ME Status Indicator: 0x%x",
                                 req_ptr->me_app_status);

    /* Reset the values */
    req_ptr->me_app_status   = MMGSDI_STATUS_APP_NO_INDICATION;
    req_ptr->ret_data_struct = MMGSDI_STATUS_NONE;
  }

  /* ---------------------------------------------------------------------------
     Move into the MMGSDI and UIM Interface to Queue the status command onto
     the UIM Task Command Queue
     -------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_uim_icc_status(req_ptr);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Failed UIM ICC STATUS: 0%x", mmgsdi_status);
  }

  return mmgsdi_status;
}


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_PROCESS_ENABLE_SERVICE

  DESCRIPTION:
    This function will enable a service by doing a rehabiliate or
    invalidate to UIM.

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function

  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_process_enable_service (
  const mmgsdi_session_enable_service_req_type *req_ptr
)
{
  mmgsdi_return_enum_type                       mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_set_file_status_req_type               set_file_status_req;
  int32                                         index            = 0;
  mmgsdi_client_req_extra_info_type            *extra_param_ptr  = NULL;
  mmgsdi_int_app_info_type                     *app_info_ptr     = NULL;
  mmgsdi_session_enable_service_extra_info_type enable_ser_extra_info;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  memset(&enable_ser_extra_info,
          0x00,
          sizeof(mmgsdi_session_enable_service_extra_info_type));

  mmgsdi_status =  mmgsdi_util_get_prov_session_info(
                     req_ptr->request_header.session_id,
                     NULL,
                     NULL,
                     &app_info_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    UIM_MSG_HIGH_1("Could not get app info for session id 0x%x",
                   req_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

 /*---------------------------------------------------------------------------
    Allocate and Populate Extra Parameter
    - Free Request Table if error in Extra Parameter
  ---------------------------------------------------------------------------*/
  enable_ser_extra_info.service_type = req_ptr->service;
  enable_ser_extra_info.client_data  = req_ptr->request_header.client_data;
  enable_ser_extra_info.orig_callback = req_ptr->request_header.response_cb;

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                                    MMGSDI_SESSION_ENABLE_SERVICE_REQ,
                                    (void*)&enable_ser_extra_info,
                                    &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

 /*---------------------------------------------------------------------------
    Populate Client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                index,
                                                &req_ptr->request_header,
                                                NULL,
                                                extra_param_ptr );
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Populate to client req table failed 0x%x", mmgsdi_status);

    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been
                                    populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  if(req_ptr->service == MMGSDI_GSM_SRV_BDN)
  {
    if(app_info_ptr->app_capabilities.bdn_state == MMGSDI_SRV_ENABLE)
    {
      extra_param_ptr->enable_service_data.is_status_same = TRUE;
      mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                        index,
                        req_ptr->request_header.request_type,
                        MMGSDI_SUCCESS);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        mmgsdi_util_free_client_request_table_index(index);
      }

      UIM_MSG_HIGH_0("BDN service is already enabled");

      return mmgsdi_status;
    }

    mmgsdi_memscpy(&set_file_status_req.request_header,
                   sizeof(set_file_status_req.request_header),
                   &req_ptr->request_header,
                   sizeof(mmgsdi_request_header_type));

    /* overwrite the request type to rehabilitate */
    set_file_status_req.access.access_method        = MMGSDI_EF_ENUM_ACCESS;
    set_file_status_req.request_header.request_type = MMGSDI_REHABILITATE_REQ;
    set_file_status_req.access.file.file_enum       = MMGSDI_TELECOM_BDN;
    set_file_status_req.request_header.client_data  = int32touint32(index);
    set_file_status_req.request_header.response_cb  = mmgsdi_util_internal_cb;

    /* Send Rehabilitate request to the UIM server */
    mmgsdi_status = mmgsdi_uim_icc_rehabilitate(&set_file_status_req, FALSE, NULL);

  }
  else if(req_ptr->service == MMGSDI_GSM_SRV_FDN ||
          req_ptr->service == MMGSDI_CDMA_SRV_FDN)
  {
    if(app_info_ptr->app_capabilities.fdn_state == MMGSDI_SRV_ENABLE)
    {
      extra_param_ptr->enable_service_data.is_status_same = TRUE;
      mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                        index,
                        req_ptr->request_header.request_type,
                        MMGSDI_SUCCESS);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        mmgsdi_util_free_client_request_table_index(index);
      }

      UIM_MSG_HIGH_0("FDN service is already enabled");

      return mmgsdi_status;
    }

    if(app_info_ptr->pin2.status != MMGSDI_PIN_ENABLED_VERIFIED &&
       app_info_ptr->pin2.status != MMGSDI_PIN_DISABLED)
    {
      UIM_MSG_ERR_1("Cannot continue with FDN enable as PIN2 is not in verified or not in disable state 0x%x",
                    app_info_ptr->pin2.status);
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ACCESS_DENIED;
    }

    /* send out the invalidate ADN command */
    mmgsdi_memscpy(&set_file_status_req.request_header,
                   sizeof(set_file_status_req.request_header),
                   &req_ptr->request_header,
                   sizeof(mmgsdi_request_header_type));

    /* overwrite the request type to invalidate */
    set_file_status_req.request_header.client_data  = int32touint32(index);
    set_file_status_req.request_header.response_cb  = mmgsdi_util_internal_cb;
    set_file_status_req.access.access_method        = MMGSDI_EF_ENUM_ACCESS;
    set_file_status_req.request_header.request_type = MMGSDI_INVALIDATE_REQ;
    set_file_status_req.access.file.file_enum       = MMGSDI_TELECOM_ADN;

    /* Send invalidate request to the UIM server */
    mmgsdi_status = mmgsdi_uim_icc_invalidate(&set_file_status_req);
  }
  else
  {
    UIM_MSG_ERR_1("Enable service request failed for service: 0x%x",
                  req_ptr->service);
    mmgsdi_status = MMGSDI_ERROR;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    mmgsdi_util_free_client_request_table_index(index);
    UIM_MSG_HIGH_1("ERROR Enable service process status 0x%x", mmgsdi_status);
  }
  return mmgsdi_status;
} /* mmgsdi_icc_process_enable_service */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_PROCESS_DISABLE_SERVICE

  DESCRIPTION:
    This function will disable a service by doing a rehabiliate or
    invalidate to UIM.

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using
    this function

  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_process_disable_service (
  const mmgsdi_session_disable_service_req_type *req_ptr
)
{
  mmgsdi_return_enum_type                       mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_set_file_status_req_type               set_file_status_req;
  int32                                         index            = 0;
  mmgsdi_client_req_extra_info_type            *extra_param_ptr  = NULL;
  mmgsdi_int_app_info_type                     *app_info_ptr    = NULL;
  mmgsdi_session_enable_service_extra_info_type enable_srv_extra_info;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status =  mmgsdi_util_get_prov_session_info(
                     req_ptr->request_header.session_id,
                     NULL,
                     NULL,
                     &app_info_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    UIM_MSG_HIGH_1("Could not get app info for session id 0x%x",
                   req_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

  memset(&enable_srv_extra_info,
          0x00,
          sizeof(mmgsdi_session_enable_service_extra_info_type));

   /*---------------------------------------------------------------------------
    Allocate and Populate Extra Parameter
    - Free Request Table if error in Extra Parameter
  ---------------------------------------------------------------------------*/
  enable_srv_extra_info.service_type  = req_ptr->service;
  enable_srv_extra_info.client_data   = req_ptr->request_header.client_data;
  enable_srv_extra_info.orig_callback = req_ptr->request_header.response_cb;

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                                    MMGSDI_SESSION_DISABLE_SERVICE_REQ,
                                    (void*)&enable_srv_extra_info,
                                    &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Populate Client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                index,
                                                &req_ptr->request_header,
                                                NULL,
                                                extra_param_ptr );
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Populate to client req table failed 0x%x", mmgsdi_status);

    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer (Since client table has not been
                                    populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  if(req_ptr->service == MMGSDI_GSM_SRV_FDN ||
     req_ptr->service == MMGSDI_CDMA_SRV_FDN)
  {
    if(app_info_ptr->app_capabilities.fdn_state == MMGSDI_SRV_DISABLE)
    {
      extra_param_ptr->enable_service_data.is_status_same = TRUE;
      mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                        index,
                        req_ptr->request_header.request_type,
                        MMGSDI_SUCCESS);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        mmgsdi_util_free_client_request_table_index(index);
      }

      UIM_MSG_HIGH_0("FDN service is already disabled");

      return mmgsdi_status;
    }

    if(app_info_ptr->pin2.status != MMGSDI_PIN_ENABLED_VERIFIED &&
       app_info_ptr->pin2.status != MMGSDI_PIN_DISABLED)
    {
      UIM_MSG_ERR_1("Cannot continue with FDN disable as PIN2 is not in verified or not in disable state 0x%x",
                    app_info_ptr->pin2.status);
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ACCESS_DENIED;
    }

    mmgsdi_memscpy(&set_file_status_req.request_header,
                   sizeof(set_file_status_req.request_header),
                   &req_ptr->request_header,
                   sizeof(mmgsdi_request_header_type));

    /* overwrite the request type to rehabilitate */
    set_file_status_req.access.access_method        = MMGSDI_EF_ENUM_ACCESS;
    set_file_status_req.request_header.request_type = MMGSDI_REHABILITATE_REQ;
    set_file_status_req.access.file.file_enum       = MMGSDI_TELECOM_ADN;
    set_file_status_req.request_header.client_data  = int32touint32(index);
    set_file_status_req.request_header.response_cb  = mmgsdi_util_internal_cb;

    /* Send Rehabilitate request to the UIM server */
    mmgsdi_status = mmgsdi_uim_icc_rehabilitate(&set_file_status_req, FALSE, NULL);

  }
  else if(req_ptr->service == MMGSDI_GSM_SRV_BDN)
  {
    if(app_info_ptr->app_capabilities.bdn_state == MMGSDI_SRV_DISABLE)
    {
      extra_param_ptr->enable_service_data.is_status_same = TRUE;
      mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                        index,
                        req_ptr->request_header.request_type,
                        MMGSDI_SUCCESS);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        mmgsdi_util_free_client_request_table_index(index);
      }

      UIM_MSG_HIGH_0("BDN service is already disabled");

      return mmgsdi_status;
    }

    /* send out the invalidate BDN command */
    mmgsdi_memscpy(&set_file_status_req.request_header,
                   sizeof(set_file_status_req.request_header),
                   &req_ptr->request_header,
                   sizeof(mmgsdi_request_header_type));

    /* overwrite the request type to invalidate */
    set_file_status_req.request_header.client_data  = int32touint32(index);
    set_file_status_req.request_header.response_cb  = mmgsdi_util_internal_cb;
    set_file_status_req.access.access_method        = MMGSDI_EF_ENUM_ACCESS;
    set_file_status_req.request_header.request_type = MMGSDI_INVALIDATE_REQ;
    set_file_status_req.access.file.file_enum       = MMGSDI_TELECOM_BDN;

    /* Send invalidate request to the UIM server */
    mmgsdi_status = mmgsdi_uim_icc_invalidate(&set_file_status_req);
  }
  else
  {
    UIM_MSG_ERR_1("Disable service request failed for service: 0x%x",
                  req_ptr->service);
    mmgsdi_status = MMGSDI_ERROR;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    mmgsdi_util_free_client_request_table_index(index);
    UIM_MSG_HIGH_1("ERROR disable service process status 0x%x", mmgsdi_status);
  }
  return mmgsdi_status;

} /* mmgsdi_icc_process_disable_service */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_SET_FILE_STATUS

  DESCRIPTION:
    This function will rehabilitate or invalidate the EF


  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_set_file_status (
  const mmgsdi_set_file_status_req_type   *req_ptr,
  mmgsdi_cmd_enum_type                     req_type
)
{
  mmgsdi_return_enum_type          mmgsdi_status   = MMGSDI_SUCCESS;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

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

  /* Send rehabilitate request to the UIM server */
  if (req_type == MMGSDI_REHABILITATE_REQ)
  {
    mmgsdi_status = mmgsdi_uim_icc_rehabilitate(req_ptr, FALSE, NULL);
  }
  else if (req_type == MMGSDI_INVALIDATE_REQ)
  {
    mmgsdi_status = mmgsdi_uim_icc_invalidate(req_ptr);
  }
  else
  {
    mmgsdi_status = MMGSDI_ERROR;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    UIM_MSG_HIGH_2("ERROR processing request 0x%x status 0x%x",
                   req_type, mmgsdi_status);
  }
  return mmgsdi_status;
} /* mmgsdi_icc_set_file_status */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_IS_DISABLE_CHV1_ALLOWED

  DESCRIPTION:
    This function will check if disable CHV1 is allowed or not in the Service
    Table

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function

  LIMITATIONS:
    This function is limited to the use of ICC

  RETURN VALUE:
    TRUE/FALSE

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
boolean  mmgsdi_icc_is_disable_chv1_allowed (
  mmgsdi_session_id_type  session_id
)
{
  mmgsdi_data_type                    sst_data;
  uint8                               sst_data_byte   = 0;
  mmgsdi_access_type                  file_access;
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type            *sel_app_ptr    = NULL;

  sst_data.data_len = 1;
  sst_data.data_ptr = &sst_data_byte;

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    NULL,
                                                    &sel_app_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || sel_app_ptr == NULL)
  {
    return FALSE;
  }

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    file_access.file.file_enum = MMGSDI_CDMA_SVC_TABLE;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    file_access.file.file_enum = MMGSDI_GSM_SST;
  }
  else
  {
    UIM_MSG_ERR_1("Invalid app type: 0x%x", sel_app_ptr->app_data.app_type);
    return FALSE;
  }

  mmgsdi_status = mmgsdi_cache_read(session_id,
                                    &file_access,
                                    &sst_data,
                                    SST_PIN_DISABLING_OFFSET,
                                    NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Read from cache failed.
       We reached here probably because PIN is enabled and not verified and
       since SST read is PIN1 protected, the SST read did not go through
       rendering cache un-iniatialized for SST. Since bit1 of SST decides
       whether DISABLE PIN on PIN1 is allowed for the ICC card or not, we
       should be restrictive here and return an error for the DISABLE_PIN
       command on PIN1 for ICC cards, when EF-SST is not already cached */
    UIM_MSG_HIGH_0("Disable CHV1 Not Allowed: SST not cached yet");
    return FALSE;
  }

  if ( ( sst_data.data_ptr[0] & SST_PIN_DISABLING_MASK ) == SST_PIN_DISABLING_MASK )
  {
    UIM_MSG_HIGH_1("Disable CHV1 Allowed, byte value: 0x%x", sst_data.data_ptr[0]);
    return TRUE;
  }
  UIM_MSG_HIGH_1("Disable CHV1 Not Allowed, byte value: 0x%x",
                 sst_data.data_ptr[0]);
  return FALSE;
} /* mmgsdi_icc_is_disable_chv1_allowed */


/* ============================================================================
  FUNCTION:      MMGSDI_ICC_PIN_STATUS

  DESCRIPTION:
    This function will get the Pin Status from the cache or send the command to
    the card if the PIN Status in not available
  DEPENDENCIES:


  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_pin_status should be used.

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
mmgsdi_return_enum_type  mmgsdi_icc_pin_status (
  const mmgsdi_get_all_pin_status_req_type      * req_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_ERROR;
  int32                               index           = 0;
  mmgsdi_client_req_extra_info_type * extra_param_ptr = NULL;
  mmgsdi_session_type_enum_type       session_type    = MMGSDI_MAX_SESSION_TYPE_ENUM;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Block PIN operations for non-provisioning apps in ICC mode because
     MMGSDI has valid PIN info only for prov sessions. Streaming APDUs
     should be used to perform such PIN operations. */
  mmgsdi_status = mmgsdi_util_get_session_type(
                    req_ptr->request_header.session_id,
                    &session_type,
                    NULL);
  if(mmgsdi_status == MMGSDI_SUCCESS &&
     mmgsdi_util_is_non_prov_session(session_type))
  {
    UIM_MSG_ERR_1("GET_ALL_PIN_STATUS not supported for session type 0x%x in ICC mode",
                  session_type);
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param( MMGSDI_GET_ALL_PIN_STATUS_REQ,
                                                            (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
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
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, MMGSDI_GET_ALL_PIN_STATUS_REQ,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return mmgsdi_status;
}/* mmgsdi_icc_pin_status */


/* ============================================================================
  FUNCTION:      MMGSDI_ICC_CHK_SRV_AVAILABLE

  DESCRIPTION:
    This function checks the SRV AVAILABLE BIT in GSM/RUIM

  DEPENDENCIES:


  LIMITATIONS:
    There is no corresponding EST bit in SIM/RUIM

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_chk_srv_available(
  mmgsdi_session_id_type    session_id,
  mmgsdi_chk_srv_enum_type  check_type,
  mmgsdi_service_enum_type  srv_type,
  boolean                  *srv_available_ptr)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_access_type        access;
  mmgsdi_data_type          data;
  uint32                    offset          = 0x00;
  uint32                    mask            = 0x01;
  uint32                    shift           = 0x01;
  uint32                    service         = 0;
  boolean                   need_alloc      = TRUE;
  mmgsdi_slot_id_enum_type  slot_id         = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type *app_info_ptr    = NULL;
  mmgsdi_app_enum_type      app_type        = MMGSDI_APP_UNKNOWN;
  boolean                   is_gsdi_task    = TRUE;

  MMGSDIUTIL_RETURN_IF_NULL(srv_available_ptr);

  /* Check if function is invoked in GSDI task context */
  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    is_gsdi_task = FALSE;
  }

  memset (&data, 0x00, sizeof(mmgsdi_data_type));

  *srv_available_ptr = FALSE;

  access.access_method = MMGSDI_EF_ENUM_ACCESS;

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

    /* Get the App type  */
    app_type = app_info_ptr->app_data.app_type;

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

  service              = (uint32)srv_type;

  if(service >= MMGSDI_CDMA_SRV &&
     service <  MMGSDI_GSM_SRV)
  {
    access.file.file_enum = MMGSDI_CDMA_SVC_TABLE;

    /*Subtract MMGSDI_CDMA_SRV so that the service
      values are mapped directly to the services in the spec*/
    service = service - MMGSDI_CDMA_SRV;
  }
  else if(service >= MMGSDI_GSM_SRV &&
          service <  MMGSDI_USIM_SRV  )
  {
    access.file.file_enum = MMGSDI_GSM_SST;

    /*Subtract MMGSDI_GSM_SRV so that the service
      values are mapped directly to the services in the spec*/
    service = service - MMGSDI_GSM_SRV;
  }
  else
  {
    UIM_MSG_ERR_1("Invalid Service 0x%x", srv_type);
    return MMGSDI_ERROR;
  }

  /* Get the cache length */

  /* Protect access to client ID reg. table/App. table */
  if(!is_gsdi_task)
  {
    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
  }

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
    need_alloc = FALSE;
  }
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* not cached or failed to cache the service table  */
    UIM_MSG_ERR_1("Cache read Serve Table failed 0x%x", mmgsdi_status);
    return mmgsdi_status;
  }

  if (data.data_len == 0)
  {
    /* SST/CST size is zero */
    UIM_MSG_ERR_0("SST/CST Service Table size 0");
    if (!need_alloc)
    {
      /* Already allocated, need to free now */
      MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
    }
    return MMGSDI_ERROR;
  }

  if (need_alloc)
  {
    /* memory allocate the data with the data_len size */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data.data_ptr, data.data_len);
    if (data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    /* Protect access to client ID reg. table/App. table */
    if(!is_gsdi_task)
    {
      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
    }

    mmgsdi_status = mmgsdi_cache_read(
      session_id,
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
      UIM_MSG_ERR_0("SST/CST Service Table Not cached");
      return mmgsdi_status;
    }
  }

  /*
    First byte:
   b8   b7  b6  b5  b4  b3  b2  b1
    |   |   |   |   |   |   |____|Service n°1
                    |___|_________Service n°2
            |___|_________________Service n°3
    |___|_________________________Service n°4
  */

  offset = service/4;
  shift = service%4;

  mask = MMGSDI_ICC_SRV_MASK;

  /*Since the mask moves every two bits for every service, the shift
     should also be made accordingly. */

  shift = shift *2;

  mask = mask<<(shift);

  if(offset < int32touint32(data.data_len))
  {
    if(((data.data_ptr[offset])&mask) == mask)
    {
      *srv_available_ptr = TRUE;
    }
    else
    {
      *srv_available_ptr = FALSE;
    }
  }

  UIM_MSG_HIGH_2("Srv Type 0x%x, ICC SRV AVAILABLE - 0x%x",
                 srv_type, *srv_available_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
  return MMGSDI_SUCCESS;
}/*mmgsdi_icc_chk_srv_available*/


/*===========================================================================
FUNCTION MMGSDI_ICC_IS_SERVICE_AVAILABLE

DESCRIPTION
  This function checks to see if the service is available in SST/CST.

  DEPENDENCIES:


  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_is_service_available should be used.

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
mmgsdi_return_enum_type  mmgsdi_icc_is_service_available (
  const mmgsdi_srv_available_req_type      * req_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_ERROR;
  int32                               index                = 0;
  mmgsdi_client_req_extra_info_type * extra_param_ptr      = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param( MMGSDI_SRV_AVAILABLE_REQ,
                                                            (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
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
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_icc_chk_srv_available(req_ptr->request_header.session_id,
                                               MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                               req_ptr->service,
                                               &extra_param_ptr->srv_available_data.srv_available);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp( index, MMGSDI_SRV_AVAILABLE_REQ, MMGSDI_SUCCESS);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
      return mmgsdi_status;
    }
  }
  else
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
}/*mmgsdi_icc_is_service_available*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_INIT_REHAB_FILE

   DESCRIPTION:
     This function checks if passed in file is invalidated.  If so, it will
     rehabilitate the file.

   DEPENDENCIES:
     Should only be called if FDN is enabled and should only be called for
     initialization processing.

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     Input file is re-habilitated conditionally

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_init_rehab_file(
  boolean                    skip_get_file_attr,
  mmgsdi_session_id_type     session_id,
  mmgsdi_app_enum_type       app_type,
  mmgsdi_file_enum_type      sim_filename,
  mmgsdi_slot_id_enum_type   slot
)
{
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_rehabilitate_cnf_type      *rehab_cnf_ptr     = NULL;
  mmgsdi_set_file_status_req_type   *rehab_req_ptr     = NULL;
  mmgsdi_file_status_type            file_status;

  memset(&file_status, 0x00, sizeof(mmgsdi_file_status_type));

  UIM_MSG_HIGH_0("mmgsdi_icc_init_rehab_file");

  if(!skip_get_file_attr)
  {
    mmgsdi_status = mmgsdi_card_init_get_file_attr(session_id,
                                                   app_type,
                                                   slot,
                                                   sim_filename,
                                                   &file_status,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL);
    if ( mmgsdi_status != MMGSDI_SUCCESS )
    {
      UIM_MSG_ERR_1("mmgsdi_uim_icc_select returned with error status 0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
    }

    /* We have successfully got the file attributes for File*/
    if (!file_status.file_invalidated)
    {
      UIM_MSG_HIGH_1("File: 0x%x Not Invalidated", sim_filename);
      return MMGSDI_SUCCESS;
    }
    /* If the card is not provisioned properly for security condition,
       the rehabilitate will fail but that should not stop the normal power up
       of the card nevertheless.*/
    UIM_MSG_HIGH_1("File 0x%x Invalidated - Rehabilitating", sim_filename);
  }

  /* Rehabilitate File */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(rehab_req_ptr,
    sizeof(mmgsdi_set_file_status_req_type));
  if (rehab_req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  rehab_req_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  rehab_req_ptr->request_header.session_id        = session_id;
  rehab_req_ptr->request_header.request_type      = MMGSDI_REHABILITATE_REQ;
  rehab_req_ptr->request_header.orig_request_type = MMGSDI_REHABILITATE_REQ;
  rehab_req_ptr->request_header.request_len       =
    uint32toint32(sizeof(mmgsdi_read_req_type));
  rehab_req_ptr->request_header.payload_len        =
    rehab_req_ptr->request_header.request_len -
    uint32toint32(sizeof(mmgsdi_read_req_type));
  rehab_req_ptr->request_header.slot_id           = slot;
  rehab_req_ptr->request_header.client_data       = 0;
  rehab_req_ptr->request_header.response_cb       = NULL;
  rehab_req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  rehab_req_ptr->access.file.file_enum            = sim_filename;

  mmgsdi_status = mmgsdi_uim_icc_rehabilitate(rehab_req_ptr,
                                              TRUE,
                                              &rehab_cnf_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(rehab_req_ptr);

  /* Check the status if the rehabilitate command. */
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (rehab_cnf_ptr == NULL))
  {
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)rehab_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(rehab_cnf_ptr);
    return MMGSDI_ERROR;
  }

  /* Check SW1 and SW2 from card response if failure is because of auth
     failure then we have a card that does not support rehabiliation so
     continue */
  /* sw1=>90*/
  if (rehab_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("File 0x%x Rehabilitate Success", sim_filename);
  }
  /* sw1=>0x98 sw2=>0x04*/
  else if (rehab_cnf_ptr->response_header.mmgsdi_status ==
           MMGSDI_ACCESS_DENIED)
  {
    UIM_MSG_HIGH_1("File 0x%x Rehabilitate Failed, Access Condition Not Fulfilled",
                   sim_filename);
  }
  else
  {
    UIM_MSG_ERR_2("File 0x%x Rehabilitate Failed mmgsdi status: 0x%x",
                  sim_filename, rehab_cnf_ptr->response_header.mmgsdi_status);
    mmgsdi_status = rehab_cnf_ptr->response_header.mmgsdi_status;
  } /* End Check for Rehabilitate Command SW1 and SW2 */

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)rehab_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(rehab_cnf_ptr);

  return mmgsdi_status;
} /* mmgsdi_icc_init_rehab_file */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_GET_SIM_CAPABILITIES

  DESCRIPTION:
    This function will provide capabilities on fdn, bdn, acl and imsi
    rehabilitation procedure for SIM

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.
    MMGSDI_NOT_SUPPORTED    : When FEATURE_MMGSDI_GSM is not defined

  SIDE EFFECTS:
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_get_sim_capabilities (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot
)
{
#ifdef FEATURE_MMGSDI_3GPP
  boolean                         imsi_valid               = TRUE;
  boolean                         loci_valid               = TRUE;
  boolean                         bdn_valid                = TRUE;
  boolean                         bdn_avail                = FALSE;
  boolean                         cc_avail                 = FALSE;
  mmgsdi_return_enum_type         mmgsdi_status            = MMGSDI_SUCCESS;
  mmgsdi_file_status_type         file_status;
  mmgsdi_int_app_info_type       *app_info_ptr             = NULL;
  mmgsdi_rehab_info_type         *rehab_info_ptr           = NULL;
  mmgsdi_slot_data_type          *slot_data_ptr            = NULL;
  mmgsdi_nv_context_type          nv_context;

  UIM_MSG_HIGH_0("mmgsdi_icc_get_sim_capabilities");

  /* Get the slot data */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot);

  if (slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
  {
    UIM_MSG_HIGH_1("Onchip SIM active for slot: 0x%x", slot);
    return MMGSDI_SUCCESS;
  }

  memset(&file_status, 0x00,sizeof(mmgsdi_file_status_type));

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
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

  if (slot_data_ptr->sim_phase == MMGSDI_SIM_PHASE_NOT_INIT)
  {
    mmgsdi_status = mmgsdi_card_init_check_ef_phase(session_id,
                                                    slot,
                                                    MMGSDI_APP_SIM);
    if (mmgsdi_status == MMGSDI_NOT_FOUND)
    {
      /* File is not found, so check if it is mandatory or not */

      /* If mmgsdi feature absent_mandatory_files_init is enabled, file is not
         mandatory */
      if (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_ABSENT_MANDATORY_FILES_INIT, nv_context) !=
                      MMGSDI_FEATURE_ENABLED)
      {
        /* Update the session close cause. The session changed event
           broadcasts the cause */
        app_info_ptr->session_closed_cause =
                   MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PHASE;
        return MMGSDI_ERROR;
      }
    }
  }

  /* Phase 1 card */
  if (slot_data_ptr->sim_phase == MMGSDI_SIM_PHASE_1)
  {
    app_info_ptr->app_capabilities.fdn_state = MMGSDI_SRV_DISABLE;
    return MMGSDI_SUCCESS;
  }

  /* Procedure for phase 2 or phase 2+ card */

  /* Check BDN service  */
  mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                               MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                               MMGSDI_GSM_SRV_BDN,
                                               &bdn_avail);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Get Service available for BDN failed */
    return MMGSDI_ERROR;
  }

  /* Check Call Control service  */
  mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                               MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                               MMGSDI_GSM_SRV_CALL_CONTROL,
                                               &cc_avail);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Get Service available for Call Control failed */
    return MMGSDI_ERROR;
  }

  /* Check if IMSI is invalidated or not */
  mmgsdi_status = mmgsdi_card_init_get_file_attr(session_id,
                                                 MMGSDI_APP_SIM,
                                                 slot,
                                                 MMGSDI_GSM_IMSI,
                                                 &file_status,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  imsi_valid = !file_status.file_invalidated;

  /* Check if LOCI is invalidated or not */
  mmgsdi_status = mmgsdi_card_init_get_file_attr(session_id,
                                                 MMGSDI_APP_SIM,
                                                 slot,
                                                 MMGSDI_GSM_LOCI,
                                                 &file_status,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  loci_valid = !file_status.file_invalidated;

  /* Check if FDN is supported before rehabilitating */
  if ( mmgsdi_generic_data_ptr->me_capabilities.me_supports_fdn )
  {
    /* Check if TP is done */

    /* Spec 11.11
       Under Section 8.15
       If BDN is enabled (see subclause 11.5.1) then the REHABILITATE
       function shall not rehabilitate the invalidated EFIMSI and EFLOCI
       until the PROFILE DOWNLOAD procedure is performed indicating that
       the ME supports the "Call control by SIM" facility
    */
#ifdef FEATURE_GSTK
    if (app_info_ptr->app_tp_state == MMGSDI_TP_STATE_DONE ||
        app_info_ptr->app_tp_state == MMGSDI_TP_STATE_NOT_NEEDED ||
        app_info_ptr->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)
#endif /* FEATURE_GSTK */
    {
      rehab_info_ptr = &slot_data_ptr->rehab_info;

      /* Check if IMSI needs rehabilitation */
      if ( !imsi_valid )
      {
        /* Clear flag indicating IMSI rehabilitation status */
        rehab_info_ptr->imsi_rehabilitated = FALSE;

        /* We need to rehab IMSI immediately after SELECT hence
           set the first param (skip_get_file_attr) to false */
        mmgsdi_status = mmgsdi_icc_init_rehab_file(FALSE,
                                                   session_id,
                                                   MMGSDI_APP_SIM,
                                                   MMGSDI_GSM_IMSI,
                                                   slot);
        if ( mmgsdi_status == MMGSDI_SUCCESS )
        {
          /* IMSI rehabilitated */
          rehab_info_ptr->imsi_rehabilitated = TRUE;
        }
        else
        {
          /* Use global imsi_invalidated to indicate if IMSI
             rehab failure */
          app_info_ptr->app_capabilities.imsi_invalidated = TRUE;
          UIM_MSG_HIGH_0("IMSI is invalidated");
        }
      }

      /* Check if LOCI needs rehabilitation */
      if ( !loci_valid )
      {
        /* Clear flag indicating LOCI rehabilitation status */
        rehab_info_ptr->loci_rehabilitated = FALSE;

        /* We need to rehab LOCI immediately after SELECT hence
           set the first param (skip_get_file_attr) to false */
        mmgsdi_status = mmgsdi_icc_init_rehab_file(FALSE,
                                                   session_id,
                                                   MMGSDI_APP_SIM,
                                                   MMGSDI_GSM_LOCI,
                                                   slot);
        if ( mmgsdi_status == MMGSDI_SUCCESS )
        {
          /* LOCI Rehabilitated */
          rehab_info_ptr->loci_rehabilitated = TRUE;
        }
        else
        {
          /* global imsi_invalidated is used to indicate if LOCI
           rehab fails as well */
          app_info_ptr->app_capabilities.imsi_invalidated = TRUE;
          UIM_MSG_HIGH_0("IMSI is invalidated");
        }
      }
    } /* MMGSDI_TP_STATE_DONE */
#ifdef FEATURE_GSTK
    else
    {
      UIM_MSG_HIGH_1("TP state is 0x%x", app_info_ptr->app_tp_state);
      app_info_ptr->app_capabilities.imsi_invalidated = !imsi_valid;
    }
#endif /* FEATURE_GSTK */
  } /*  FDN is supported */

  /* Update FDN service */
  mmgsdi_status = mmgsdi_icc_update_fdn_status(session_id,
                                               slot,
                                               MMGSDI_FDN_SOURCE_CARD_INIT);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Reset fdn_state to deafult values as fdn_status updation is failed */
    app_info_ptr->app_capabilities.fdn_state = MMGSDI_SRV_NOT_INIT;
  }

  /* Spec 11.11
     Under Section for EF SST
     As the BDN service relies on the Call Control feature,
     service n°31 (BDN) should only be allocated and activated
     if service n°28 (Call control) is allocated and activated.
  */
  if (( bdn_avail ) && ( cc_avail ))
  {
    /* Get File Info for BDN */
    mmgsdi_status = mmgsdi_card_init_get_file_attr(session_id,
                                                   MMGSDI_APP_SIM,
                                                   slot,
                                                   MMGSDI_TELECOM_BDN,
                                                   &file_status,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_HIGH_1("SST Indicated BDN and CC support with EF BDN access failure 0x%x",
                     mmgsdi_status);
      app_info_ptr->app_capabilities.bdn_state = MMGSDI_SRV_DISABLE;
      UIM_MSG_HIGH_0("BDN service is disabled");
      /* Reset status to success */
      mmgsdi_status = MMGSDI_SUCCESS;
    }
    else
    {
      bdn_valid = !file_status.file_invalidated;
      if ( !bdn_valid )
      {
        app_info_ptr->app_capabilities.bdn_state = MMGSDI_SRV_DISABLE;
        UIM_MSG_HIGH_0("BDN service is disabled");
      }
      else
      {
        app_info_ptr->app_capabilities.bdn_state = MMGSDI_SRV_ENABLE;
        UIM_MSG_HIGH_0("BDN service is enabled");
      }
    }
  }
  else
  {
    UIM_MSG_HIGH_0("BDN not Enabled in SST");
    app_info_ptr->app_capabilities.bdn_state = MMGSDI_SRV_DISABLE;
  }

  /* If TP is not done, don't set all_srv_init.
     We might not have REHABILITATED the EFIMSI and EFLOCI */
#ifdef FEATURE_GSTK
  if (app_info_ptr->app_tp_state == MMGSDI_TP_STATE_DONE ||
      app_info_ptr->app_tp_state == MMGSDI_TP_STATE_NOT_NEEDED ||
      app_info_ptr->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)
#endif /* FEATURE_GSTK */
  {
    /* Set all_srv_init as TRUE */
    app_info_ptr->app_capabilities.all_srv_init = TRUE;
    UIM_MSG_HIGH_0("All services are initialized");
  }
  return mmgsdi_status;
#else
  (void)session_id;
  (void)slot;
  UIM_MSG_ERR_0("mmgsdi_icc_get_sim_capabilities Err: FEATURE_MMGSDI_3GPP not defined");
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP */
} /* mmgsdi_icc_get_sim_capabilities */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_GET_RUIM_CAPABILITIES

  DESCRIPTION:
    This function will provide capabilities on fdn, bdn, acl and imsi
    rehabilitation procedure for RUIM

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.
    MMGSDI_NOT_SUPPORTED    : When FEATURE_MMGSDI_3GPP2 is not defined

  SIDE EFFECTS:
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_get_ruim_capabilities (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_return_enum_type         mmgsdi_status             = MMGSDI_SUCCESS;
  mmgsdi_file_enum_type           fdn_filenames[MMGSDI_CDMA_FDN_FILES] =
                                  {MMGSDI_CDMA_IMSI_M, MMGSDI_CDMA_IMSI_T,
                                   MMGSDI_CDMA_TMSI};
  uint8                           index                     = 0;
  mmgsdi_data_type                mmgsdi_data_buf;
  mmgsdi_int_app_info_type       *app_info_ptr             = NULL;
  mmgsdi_rehab_info_type         *rehab_info_ptr           = NULL;
  mmgsdi_slot_data_type          *slot_data_ptr            = NULL;

  memset(&mmgsdi_data_buf, 0x00, sizeof(mmgsdi_data_type));

  UIM_MSG_HIGH_0("mmgsdi_icc_get_ruim_capabilities");

  mmgsdi_status =  mmgsdi_util_get_prov_session_info(session_id,
                                                     NULL,
                                                     NULL,
                                                     &app_info_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Nothing to do, no app ptr");
      return MMGSDI_ERROR;
  }

  /* Get the slot data */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(app_info_ptr->app_capabilities.all_srv_init == TRUE)
  {
    UIM_MSG_HIGH_0("All services are in init state");
    return MMGSDI_SUCCESS;
  }

  /* Reading RUM CDMA Phase file, It has 1 byte info.
     It is mandatory file and should be read
  */
  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                MMGSDI_APP_RUIM,
                                                slot,
                                                MMGSDI_CDMA_RUIM_PHASE,
                                                &mmgsdi_data_buf);
  /* Free up the SST Buffer allocated in this function */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);
  if ( mmgsdi_status == MMGSDI_NOT_FOUND)
  {
    UIM_MSG_ERR_1("mmgsdi_icc_get_ruim_capabilities failed for 0x%x",
                MMGSDI_CDMA_RUIM_PHASE);
    /* No need to return the GSDI_ERROR if this mandatory file is not present
       on the card. According to test requirement for Version Identification,
       ME should requests the R-UIM version (EFRevision) as part of the
       initialization, It does not say that initialization should be stop
       in case this file is not there.
    */
    mmgsdi_status = MMGSDI_SUCCESS;
  }

  app_info_ptr->app_capabilities.imsi_invalidated = FALSE;
  app_info_ptr->app_capabilities.fdn_state        = MMGSDI_SRV_DISABLE;

  /* Update FDN service */
  mmgsdi_status = mmgsdi_icc_update_fdn_status(session_id,
                                               slot,
                                               MMGSDI_FDN_SOURCE_CARD_INIT);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Reset fdn_state to deafult values as fdn_status updation is failed */
    app_info_ptr->app_capabilities.fdn_state = MMGSDI_SRV_NOT_INIT;
  }

  if (app_info_ptr->app_capabilities.fdn_state == MMGSDI_SRV_ENABLE)
  {
#ifdef FEATURE_GSTK
    if (app_info_ptr->app_tp_state == MMGSDI_TP_STATE_DONE ||
        app_info_ptr->app_tp_state == MMGSDI_TP_STATE_NOT_NEEDED ||
        app_info_ptr->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)
#endif /* FEATURE_GSTK */
    {
      rehab_info_ptr= &slot_data_ptr->rehab_info;

      /* Clear flags indicating CDMA FDN files rehabilitation status */
      memset(rehab_info_ptr->cdma_fdn_files_rehab,
             0x00,
             sizeof(rehab_info_ptr->cdma_fdn_files_rehab));


      for (index=0; index<MMGSDI_CDMA_FDN_FILES; index++)
      {
        mmgsdi_status = mmgsdi_icc_init_rehab_file(FALSE,
                                                   session_id,
                                                   MMGSDI_APP_RUIM,
                                                   fdn_filenames[index],
                                                   slot);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          return MMGSDI_ERROR;
        }
        /* File rehabilitated */
        rehab_info_ptr->cdma_fdn_files_rehab[index] = TRUE;
      }
    }
#ifdef FEATURE_GSTK
    else
    {
      UIM_MSG_HIGH_1("TP state is 0x%x", app_info_ptr->app_tp_state);
    }
#endif /* FEATURE_GSTK */
  } /* MMGSDI_SRV_ENABLE */

  /* If TP is not done, don't set all_srv_init.
     We might not have REHABILITATED the EFIMSIs */
#ifdef FEATURE_GSTK
  if (app_info_ptr->app_tp_state == MMGSDI_TP_STATE_DONE ||
      app_info_ptr->app_tp_state == MMGSDI_TP_STATE_NOT_NEEDED ||
      app_info_ptr->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)
#endif /* FEATURE_GSTK */
  {
    app_info_ptr->app_capabilities.all_srv_init = TRUE;
    UIM_MSG_HIGH_0("All services are initialized");
  }

  return MMGSDI_SUCCESS;
#else
  (void)session_id;
  (void)slot;
  UIM_MSG_ERR_0("mmgsdi_icc_get_ruim_capabilities Err: FEATURE_MMGSDI_3GPP2 not defined");
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_icc_get_ruim_capabilities */


#ifdef FEATURE_MMGSDI_3GPP
/*===========================================================================
  FUNCTION:      MMGSDI_ICC_ONCHIP_POPULATE_RUN_GSM_DATA

  DESCRIPTION
    Function used to populate RUN GSM Algorithm data into the buffer
    passed in, when the Onchip SIM feature is enabled and the
    Functionality is active.

  DEPENDENCIES
    None

  LIMITATIONS:
    This function is limited to the use of ICC card.

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_onchip_populate_run_gsm_data (
  const uint8              *rand_ptr,
  uint8                    *data_ptr,
  mmgsdi_slot_id_enum_type  slot_id
)
{
  uint8                 *sres_ptr      = NULL;
  uint8                 *kc_ptr        = NULL;
  int                    i             = 0;
  mmgsdi_slot_data_type *slot_data_ptr = NULL;
  if (rand_ptr == NULL || data_ptr == NULL)
  {
    UIM_MSG_HIGH_2("NULL rand_ptr: 0x%x or data_ptr: 0x%x", rand_ptr, data_ptr);
    return MMGSDI_ERROR;
  }

  /* Assign the rand_ptr, sres_ptr, and kc_ptr */
  sres_ptr = data_ptr;
  kc_ptr   = &data_ptr[MMGSDI_GSM_ALGO_SRES_LEN];

  UIM_MSG_HIGH_0("Onchip SIM Authentication with Rand");

  while ( i < MMGSDI_RAND_LEN)
  {
    UIM_MSG_HIGH_2("RAND[%d] = 0x%x", i, rand_ptr[i]);
    i++;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }


  /* RESET i */

  i=0;
  while ( i < MMGSDI_ONCHIP_SIM_AUTH_KEY_LEN)
  {
    UIM_MSG_HIGH_2("ONCHIP SIM KEY[%d] = 0x%x",
                   i, slot_data_ptr->onchip_sim_data.auth_key[i]);
    i++;
  }

  /* RESET i and Perform the Auth using an XOR Algorithm */
  i = 0;
  while (i < MMGSDI_GSM_ALGO_SRES_LEN + MMGSDI_GSM_ALGO_KC_LEN)
  {
    if ( i < MMGSDI_GSM_ALGO_SRES_LEN )
    {
      /* The first 4 bytes, index 0 to 3 are populated with
      ** the SRES Value
      */
      sres_ptr[i] = rand_ptr[i] ^ slot_data_ptr->onchip_sim_data.auth_key[i];
      UIM_MSG_HIGH_2("SRES[%d] = 0x%x", i, sres_ptr[i]);
    }
    else
    {
      /* The next 8 bytes are populated with the KC Value */
      kc_ptr[i] = rand_ptr[i] ^ slot_data_ptr->onchip_sim_data.auth_key[i];
      UIM_MSG_HIGH_2("KC[%d] = 0x%x", i, kc_ptr[i]);
    }
    i++;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_icc_onchip_populate_run_gsm_data */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_ONCHIP_RUN_GSM_ALGO

  DESCRIPTION:
    This function executes the onchip GSM algorithm

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of ICC card.

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_onchip_run_gsm_algo (
  const mmgsdi_session_run_gsm_algo_req_type   *req_ptr
)
{
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr = NULL;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  uim_rpt_type                       uim_report;
  mmgsdi_slot_data_type             *slot_data_ptr   = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  memset(&uim_report, 0x00, sizeof(uim_rpt_type));

  /* Get a free index */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Populate the extra param */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
        MMGSDI_SESSION_RUN_GSM_ALGO_REQ, (void*)req_ptr, &extra_param_ptr);
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
  if (slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
  {
    uim_report.sw1        = SW1_NORMAL_END;
    uim_report.sw2        = SW2_NORMAL_END;
    uim_report.rpt_status = UIM_PASS;

    mmgsdi_status = mmgsdi_icc_onchip_populate_run_gsm_data(
                      (uint8*)req_ptr->rand_val.data_ptr,
                      uim_report.rpt.run_gsm.data,
                      req_ptr->request_header.slot_id);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      uim_report.rpt.run_gsm.data_length =  MMGSDI_GSM_ALGO_SRES_LEN + MMGSDI_GSM_ALGO_KC_LEN;

      /* Set the other values in uim_report */
      uim_report.rpt_type            = UIM_RUN_GSM_ALGO_R;
      uim_report.cmd_transacted      = FALSE;
      uim_report.user_data           = int32touint32(index);

      /* Queue response */
      mmgsdi_uim_report(&uim_report);

      return mmgsdi_status;
    }
  }

  mmgsdi_util_free_client_request_table_index(index);
  return MMGSDI_ERROR;
} /* mmgsdi_icc_onchip_run_gsm_algo */
#endif  /* FEATURE_MMGSDI_3GPP */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_RUN_GSM_ALGO

  DESCRIPTION:
    This function will allow RUN GSM algorithm command to be sent to UIM.
    In case the onchip is activated, the onchip algorithm is executed.

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type ICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_run_gsm_algo (
  const mmgsdi_session_run_gsm_algo_req_type   *req_ptr
)
{
#ifdef FEATURE_MMGSDI_3GPP

  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
  {
    /* OnChip authenticate command */
    return mmgsdi_icc_onchip_run_gsm_algo(req_ptr);
  }

  /* Send RUN GSM algorithm command to UIM server */
  return mmgsdi_uim_icc_run_gsm_algo(req_ptr);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP */
} /* mmgsdi_icc_run_gsm_algo */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_IS_DCS1800_DF_PRESENT

  DESCRIPTION:
    This function checks if the DCS 1800 DF is present on the card.
    The DCS 1800 NV must be checked before calling this funciton.

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of ICC protocol

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_is_dcs1800_df_present (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type      *select_ptr    = NULL;
  mmgsdi_get_file_attr_cnf_type      *cnf_ptr       = NULL;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
    sizeof(mmgsdi_get_file_attr_req_type));
  if (select_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  select_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  select_ptr->request_header.session_id        = session_id;
  select_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.request_len       =
    uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
  select_ptr->request_header.payload_len       = 0;
  select_ptr->request_header.slot_id           = slot_id;
  select_ptr->request_header.client_data       = 0;
  select_ptr->request_header.response_cb       = NULL;
  select_ptr->access.access_method             = MMGSDI_DF_ENUM_ACCESS;
  select_ptr->access.file.df_enum              = MMGSDI_DCS1800_DIR;

  mmgsdi_status = mmgsdi_uim_icc_select(select_ptr,
                                        TRUE,
                                        &cnf_ptr);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_ptr);

  if (cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cnf Ptr Null after DCS1800 DF select");
    return MMGSDI_ERROR;
  }

  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_0("DCS1800 DF Select not successful");
    mmgsdi_status = MMGSDI_ERROR;
  }

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  return mmgsdi_status;
} /* mmgsdi_icc_is_dcs1800_df_present */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_UPDATE_FDN_STATUS

  DESCRIPTION:
    This function will update capabilities on fdn procedure for ICC
    based on ADN activation status.

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_update_fdn_status (
  mmgsdi_session_id_type          session_id,
  mmgsdi_slot_id_enum_type        slot,
  mmgsdi_fdn_evt_source_enum_type fdn_source
)
{
  boolean                         adn_valid                = TRUE;
  boolean                         fdn_avail                = FALSE;
  boolean                         adn_avail                = FALSE;
  mmgsdi_return_enum_type         mmgsdi_status            = MMGSDI_SUCCESS;
  boolean                         fdn_enabled              = FALSE;
  mmgsdi_file_status_type         file_status;
  mmgsdi_service_enum_type        fdn_srv                  = MMGSDI_NONE;
  mmgsdi_service_enum_type        adn_srv                  = MMGSDI_NONE;
  mmgsdi_int_app_info_type       *app_info_ptr             = NULL;

  memset(&file_status, 0x00,sizeof(mmgsdi_file_status_type));
  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    NULL,
                                                    &app_info_ptr);
  if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("Error obtaining Prov Session Info Status 0x%x Session ID 0x%x",
                   mmgsdi_status, session_id);
    return MMGSDI_ERROR;
  }

  switch(app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_SIM:
      fdn_srv = MMGSDI_GSM_SRV_FDN;
      adn_srv = MMGSDI_GSM_SRV_ADN;
      break;

    case MMGSDI_APP_RUIM:
      fdn_srv = MMGSDI_CDMA_SRV_FDN;
      adn_srv = MMGSDI_CDMA_SRV_LOCAL_PHONEBOOK;
      break;

    default:
      return MMGSDI_ERROR;
  }

  /* Check FDN service enabled or not */
  mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                               MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                               fdn_srv,
                                               &fdn_avail);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Get Service available for FDN failed */
    return MMGSDI_ERROR;
  }

  /* ADN service check and phonebook present determination */
  mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                               MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                               adn_srv,
                                               &adn_avail);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Get Service available for ADN failed */
    return MMGSDI_ERROR;
  }

  /* FDN Enabled in SST */
  if ( fdn_avail )
  {
    /* ADN enabled in SST */
    if ( adn_avail )
    {
      /* Get File Info for ADN */
      mmgsdi_status = mmgsdi_card_init_get_file_attr(session_id,
                                                     app_info_ptr->app_data.app_type,
                                                     slot,
                                                     MMGSDI_TELECOM_ADN,
                                                     &file_status,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     NULL);
      UIM_MSG_HIGH_2("mmgsdi_status: 0x%x file_status.file_invalidated: 0x%x",
                      mmgsdi_status, file_status.file_invalidated);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        /* Reset the mmgsdi_status - No MMGSDI_TELCOM_ADN does not mean
        ** we need to go to NO Card
        */
        mmgsdi_status = MMGSDI_SUCCESS;
        fdn_enabled = FALSE;
      }
      else
      {
        adn_valid = !file_status.file_invalidated;
        if ( !adn_valid )
        {
          /* FDN is enabled when the ADN is invalidated or not activated */
          fdn_enabled = TRUE;
        }
        else
        {
          fdn_enabled = FALSE;
        }/* Is ADN valid */
      } /* get file attr for ADN */
    }
    else
    {
      fdn_enabled = TRUE;
    }
  }
  else
  {
    fdn_enabled = FALSE;
  }

  /* Do not update fdn capability flag setting if IMSI invalidated */
  if ( !app_info_ptr->app_capabilities.imsi_invalidated )
  {
    mmgsdi_srv_state_enum_type  old_fdn_state = app_info_ptr->app_capabilities.fdn_state;

    if(fdn_enabled)
    {
      app_info_ptr->app_capabilities.fdn_state = MMGSDI_SRV_ENABLE;
    }
    else
    {
      app_info_ptr->app_capabilities.fdn_state = MMGSDI_SRV_DISABLE;
    }

    /* If the FDN state changed, notify clients by sending the FDN event */
    if (old_fdn_state != app_info_ptr->app_capabilities.fdn_state)
    {
      mmgsdi_evt_session_notify_type   notify_fdn;

      memset(&notify_fdn, 0x00, sizeof(mmgsdi_evt_session_notify_type));

      notify_fdn.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
      notify_fdn.slot_id     = slot;
      notify_fdn.session_id  = session_id;

      /* Source of event is MMGSDI as this is sent as part
         of refresh or initialization */
      mmgsdi_evt_build_and_send_fdn(notify_fdn,
                                    fdn_enabled,
                                    app_info_ptr,
                                    0,
                                    NULL,
                                    fdn_source);
    }
  }

  UIM_MSG_HIGH_3("mmgsdi_status: 0x%x FDN service status: 0x%x imsi_invalidated 0x%x",
                  mmgsdi_status,
                  app_info_ptr->app_capabilities.fdn_state,
                  app_info_ptr->app_capabilities.imsi_invalidated);
  return MMGSDI_SUCCESS;
} /* mmgsdi_icc_update_fdn_status */