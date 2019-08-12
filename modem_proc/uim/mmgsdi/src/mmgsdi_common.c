/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                M M G S D I   I C C / U I C C  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the command processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_common.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/16   bcho    F3 frequency reduction by conditional logic
05/24/16   ar      Review of macros used by MMGSDI
05/20/16   vdc     Remove F3 messages for memory allocation failure
05/10/16   tkl     F3 log prints cleanup
05/10/16   sp      Delete PB cache for the file that is being written currently
05/02/16   sp      Adding ONCHIP support for KCGPRS files
09/08/15   bcho    Run SIMLOCK Algo in Onchip SIM 
05/13/15   stv     Check cache status while handling MMGSDI_GET_FILE_ATTR_REQ
11/14/14   hh      Fix compilation warnings
10/27/14   hh      Fix KW error 
10/15/14   kk      Fixed onchip config for USIM EF KC
09/29/14   tkl     Added support for persistent cache
08/11/14   bcho    Optimize srv req by using cached srv table in client context
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
06/30/14   vv      Remove Perso code
06/18/14   av      Fix out of bound access in mmgsdi_util_find_app_pin_index
06/03/14   tl      Remove additional parameter variables
05/27/14   av      Don't allow access if UST/CST can't be read(pin not verified)
04/02/14   hh      Correct range check in mmgsdi_common_activate_onchip
03/27/14   yt      Ensure usage of correct slot id
03/18/14   tl      Introduce new SIM Lock feature
02/25/14   hh      Purge temporary mobile identities during UICC Reset Refresh
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
12/05/13   tkl     SFI Support and decouple uim file enums
01/24/14   yt      Correctly initialize additional parameters
01/21/14   yt      Remove duplicate code
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
12/16/13   tl      Prevent initalizing the cache with partial files
11/06/13   yt      Fixes for multi-SIM OnChip activation
10/03/13   yt      Multi-SIM support for OnChip
10/02/13   yt      Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi.h"
#include "mmgsdi_evt.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_common_rsp.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_uim_common.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_pb_cache.h"
#ifdef FEATURE_SIMLOCK
#include "mmgsdi_simlock.h"
#endif /* FEATURE_SIMLOCK */

#ifdef FEATURE_LTE
#include "lte_aka_wrap.h"
#endif /* FEATURE_LTE */


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Macros to set and check if a tag for USIM authenticate is present
   to avoid an invalid configuration in the onchip TLV */
#define MMGSDI_SET_AUTH_TAG_PRESENT(var, tag)    { (var) |= (uint32)(1 << (tag)); }
#define MMGSDI_IS_AUTH_TAG_PRESENT(var, tag)     ( (var) & (uint32)(1 << (tag)) )

/* Default value for the onchip USIM authenticate
   in case of GSM security context */
mmgsdi_len_type default_gsm_auth_rsp_len   = 14;
uint8           default_gsm_auth_rsp_ptr[] = { 0x04, 0x2B, 0x4D, 0x56,
                                               0x5D, 0x08, 0x02, 0x0B,
                                               0x01, 0x01, 0x19, 0x00,
                                               0x77, 0x67 };

/* Default value for the onchip USIM authenticate
   in case of 3G security context */
mmgsdi_len_type default_3g_auth_rsp_len    = 61;
uint8           default_3g_auth_rsp_ptr[]  = { 0xDB, 0x10, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x10, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x10,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x08, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00 };


/*===========================================================================
   FUNCTION:      MMGSDI_COMMON_GET_FILE_ATTR

   DESCRIPTION:
     This function process the GET_FILE_ATTR command for ICC or UICC.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.
     MASTER FILE:       Will get information on the Master File
     DEDICATED FILE:    Will get information on the Dedicated File

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_common_get_file_attr(
  mmgsdi_get_file_attr_req_type * req_ptr,
  mmgsdi_protocol_enum_type       protocol
)
{
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_int_app_info_type          * prov_app_info_ptr    = NULL;
  mmgsdi_len_type                     cache_len            = 0;
  mmgsdi_cache_init_enum_type         cache_state          = MMGSDI_CACHE_MAX_ENUM;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  switch (req_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      UIM_MSG_HIGH_1("Sending SELECT to UIM for MMGSDI_EF_ENUM_ACCESS 0x%x",
                     req_ptr->access.file.file_enum);
      break;

    case MMGSDI_DF_ENUM_ACCESS:
      UIM_MSG_HIGH_1("Sending SELECT to UIM for MMGSDI_DF_ENUM_ACCESS 0x%x",
                     req_ptr->access.file.df_enum);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      UIM_MSG_HIGH_0("Sending SELECT to UIM for MMGSDI_BY_PATH_ACCESS");
      break;

    default:
      MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid Access Type 0x%x",
                                   req_ptr->access.access_method);
      return MMGSDI_INCORRECT_PARAMS;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_check_file_access_data(
                                           req_ptr->request_header.session_id,
                                           req_ptr->access,
                                           protocol))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_prov_session_info(req_ptr->request_header.session_id,
                                       NULL,
                                       NULL,
                                       &prov_app_info_ptr) != MMGSDI_SUCCESS)
  {
    prov_app_info_ptr = NULL;
  }

  if (prov_app_info_ptr != NULL)
  {
    if (protocol == MMGSDI_UICC)
    {
      mmgsdi_util_check_usim_spec_version(&req_ptr->access,
                                          prov_app_info_ptr->usim_spec_version);
    }

    /* Check status of EF in cached SST/CST/UST */
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

      mmgsdi_util_print_file_information(req_ptr->access, TRUE);
      return mmgsdi_status;
    }
  }

  /* Check if MMGSDI has tried to cache this file and status is NOT FOUND. If
     yes, there is no reason to try to access the same file again and we can
     avoid sending the SELECT command */
  mmgsdi_status = mmgsdi_cache_read_len(req_ptr->request_header.session_id,
                                        &req_ptr->access,
                                        &cache_len,
                                        0,
                                        &cache_state);

  if((mmgsdi_status == MMGSDI_NOT_FOUND) &&
     (cache_state == MMGSDI_CACHE_NOT_FOUND))
  {
    return mmgsdi_status;
  }

  UIM_MSG_HIGH_0("Sending down SELECT command to MMGSDI UIM");

  /* Send select request to the UIM server */
  if (protocol == MMGSDI_ICC)
  {
    mmgsdi_status = mmgsdi_uim_icc_select(req_ptr, FALSE, NULL);
  }
  else if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uim_uicc_select(req_ptr, FALSE, NULL, TRUE);
  }

  return mmgsdi_status;
} /* mmgsdi_common_get_file_attr */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_WRITE

  DESCRIPTION:
    This function will process the write command to any file

  DEPENDENCIES:


  LIMITATIONS:
    None

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
mmgsdi_return_enum_type  mmgsdi_common_write (
  mmgsdi_write_req_type     * req_ptr,
  mmgsdi_protocol_enum_type   protocol
)
{
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type          slot                 = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type         *prov_app_info_ptr    = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  /* Check for protocol */
  if ((protocol != MMGSDI_UICC) && (protocol != MMGSDI_ICC))
  {
    return MMGSDI_ERROR;
  }

  /* Check for access method only */
  switch (req_ptr->access.access_method)
  {
  case MMGSDI_BY_PATH_ACCESS:
  case MMGSDI_EF_ENUM_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid Access Type 0x%x",
                                 req_ptr->access.access_method);
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_check_file_access_data(
                                            req_ptr->request_header.session_id,
                                            req_ptr->access,
                                            protocol))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_prov_session_info(req_ptr->request_header.session_id,
                                       NULL,
                                       &slot,
                                       &prov_app_info_ptr) != MMGSDI_SUCCESS)
  {
    prov_app_info_ptr = NULL;
  }

  if (prov_app_info_ptr != NULL)
  {
    /* Check if requested file is being purged */
    if ((MMGSDI_EF_ENUM_ACCESS   == req_ptr->access.access_method)      &&
        ((MMGSDI_GSM_LOCI        == req_ptr->access.file.file_enum) ||
         (MMGSDI_USIM_LOCI       == req_ptr->access.file.file_enum) ||
         (MMGSDI_USIM_PSLOCI     == req_ptr->access.file.file_enum) ||
         (MMGSDI_USIM_EPSLOCI    == req_ptr->access.file.file_enum))    &&
        (mmgsdi_util_is_purge_temp_identities_in_progress(slot))
        )
    {
      UIM_MSG_ERR_1("Request not allowed, file_enum=0x%x is being purged",
                    req_ptr->access.file.file_enum);
      return MMGSDI_ERROR;
    } /* XXLOCI file being purged */

    /*Check to see if the security attributes allow this file to be updated*/
    mmgsdi_status = mmgsdi_util_check_file_attr(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      slot,
                      prov_app_info_ptr,
                      &req_ptr->access,
                      MMGSDI_WRITE_REQ,
                      FALSE);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("File Update not allowed");
      return mmgsdi_status;
    }

    if (protocol == MMGSDI_UICC)
    {
      mmgsdi_util_check_usim_spec_version(&req_ptr->access,
                                          prov_app_info_ptr->usim_spec_version);
    }

    /* Check status of EF in cached UST/SST/CST */
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      prov_app_info_ptr->app_data.app_type,
                      &req_ptr->access );
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_print_file_information(req_ptr->access,TRUE);
      UIM_MSG_ERR_1("SERVICE not allowed: error 0x%x", mmgsdi_status);
      return mmgsdi_status;
    }
  }

  UIM_MSG_HIGH_1("Sending down WRITE command to MMGSDI UIM 0x%x",
                 req_ptr->access.file.file_enum);
  switch (req_ptr->file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
    case MMGSDI_CYCLIC_FILE:
      /* Delete file attributes and search data also as the content may change as part of Write */
      mmgsdi_pb_free_ef_cache (req_ptr->access.file.path_type,
                               req_ptr->request_header.session_id,
                               req_ptr->request_header.slot_id);

      if (protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_uim_icc_write_record(req_ptr);
      }
      else
      {
        mmgsdi_status = mmgsdi_uim_uicc_write_record(req_ptr);
      }
      break;

    case MMGSDI_TRANSPARENT_FILE:
      if (protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_uim_icc_write_transparent(req_ptr, FALSE, NULL);
      }
      else
      {
        /* note: with SFI changes, no need to use access lookup table */
        mmgsdi_status = mmgsdi_uim_uicc_write_transparent(req_ptr, FALSE, NULL);
      }
      break;

    default:
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
  }

  return mmgsdi_status;
} /* mmgsdi_common_write */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_INCREASE

  DESCRIPTION:
    This function will provide increase access to any file

  DEPENDENCIES:


  LIMITATIONS:
    None

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
mmgsdi_return_enum_type  mmgsdi_common_increase (
  mmgsdi_increase_req_type     * req_ptr,
  mmgsdi_protocol_enum_type      protocol
)
{
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type          slot                 = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type         *prov_app_info_ptr    = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  /* Check for access method only */
  switch (req_ptr->access.access_method)
  {
    case MMGSDI_BY_PATH_ACCESS:
    case MMGSDI_EF_ENUM_ACCESS:
      break;

    default:
      MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid Access Type 0x%x",
                                   req_ptr->access.access_method);
      return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_check_file_access_data(
                                            req_ptr->request_header.session_id,
                                            req_ptr->access,
                                            protocol))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_prov_session_info(req_ptr->request_header.session_id,
                                       NULL,
                                       &slot,
                                       &prov_app_info_ptr) != MMGSDI_SUCCESS)
  {
    prov_app_info_ptr = NULL;
  }

  if (prov_app_info_ptr != NULL)
  {
    /*Check to see if the security attributes allow this file to be updated*/
    mmgsdi_status = mmgsdi_util_check_file_attr(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      slot,
                      prov_app_info_ptr,
                      &req_ptr->access,
                      MMGSDI_INCREASE_REQ,
                      FALSE);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("File Update not allowed");
      return mmgsdi_status;
    }

    /* Check status of EF in cached UST */
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      prov_app_info_ptr->app_data.app_type,
                      &req_ptr->access );
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_print_file_information(req_ptr->access,TRUE);
      UIM_MSG_ERR_1("SERVICE not allowed: error 0x%x", mmgsdi_status);
      return mmgsdi_status;
    }
  }

  UIM_MSG_HIGH_1("Sending down INCREASE command to MMGSDI UIM 0x%x",
                 req_ptr->access.file.file_enum);
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uim_uicc_increase(req_ptr);
  }
  else if (protocol == MMGSDI_ICC)
  {
    mmgsdi_status = mmgsdi_uim_icc_increase(req_ptr);
  }

  return mmgsdi_status;
} /* mmgsdi_common_increase */


/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_CLEAR_ONCHIP_USIM_AUTHENTICATE

   DESCRIPTION:
     This function clears the authenticate responses for onchip

   DEPENDENCIES:
     None

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     None

   SIDE EFFECTS:

============================================================================*/
static void mmgsdi_common_clear_onchip_usim_authenticate(
   mmgsdi_slot_id_enum_type              slot_id
)
{
  mmgsdi_data_type * auth_rsp_ptr = NULL;
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("slot_data_ptr is NULL");
    return;
  }

  /* Clear data for authenticate algorithm */
  auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

  auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

  auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

    auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_algo_root_key;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

  auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_milenage_op_data;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

  slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW1_OFFSET] =
    (uint8)SW1_NO_STATUS;
  slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW2_OFFSET] =
    (uint8)SW2_NORMAL_END;

  slot_data_ptr->onchip_sim_data.usim_auth_algo =
    MMGSDI_ONCHIP_AUTH_ALGORITHM_FIXED;
  slot_data_ptr->onchip_sim_data.usim_auth_algo_res_length = 0;
  slot_data_ptr->onchip_sim_data.usim_auth_milenage_seq = 0;
} /* mmgsdi_common_clear_onchip_usim_authenticate */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_POPULATE_ONCHIP_CACHE

  DESCRIPTION:
    This function is called to populate the USIM Cache with the configuration
    data provided in the ACTIVATE_ONCHIP_SIM Command.

  DEPENDENCIES:
    Can only be called after the mmgsdi_util_validate_onchip_config()

  LIMITATIONS:

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
static mmgsdi_return_enum_type mmgsdi_common_populate_onchip_cache(
  const mmgsdi_onchip_sim_config_data_type * onchip_config_data_ptr,
  mmgsdi_slot_id_enum_type                   slot_id,
  mmgsdi_session_type_enum_type              session_type,
  mmgsdi_onchip_mode_enum_type               onchip_mode
)
{
  uint16                  index            = 0;
  uint8                   len              = 0;
  uint8                   tag              = 0;
  uint16                  tlv_len          = 0;
  uint8                   tag_size         = 1;
  uint8                   len_size         = 1;
  uint8                 * tlv_data_ptr     = NULL;
  mmgsdi_file_enum_type   file_name        = MMGSDI_NO_FILE_ENUM;
  mmgsdi_data_type      * auth_rsp_ptr     = NULL;
  uint32                  auth_tag_present = 0;
  mmgsdi_return_enum_type mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type  *slot_data_ptr    = NULL;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(onchip_config_data_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(onchip_config_data_ptr->data_ptr);

  /* --------------------------------------------------------------------------
     Assign the Starting Point for the Data
     ------------------------------------------------------------------------*/
  tlv_data_ptr = onchip_config_data_ptr->data_ptr;

  /* --------------------------------------------------------------------------
     Increment the data to get the TLV LEN which is broken into two bytes
     ------------------------------------------------------------------------*/
  tlv_data_ptr++;
  tlv_len = *tlv_data_ptr;
  tlv_data_ptr++;
  tlv_len += *tlv_data_ptr * 0x100;

  /* --------------------------------------------------------------------------
     Move the pointer to the next Tag and start populating the Cache
     ------------------------------------------------------------------------*/
  tlv_data_ptr++;
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("slot_data_ptr is NULL");
    return MMGSDI_ERROR;
  }

  while ( index < tlv_len )
  {
    /* Check TLV size*/
    if ( index + tag_size + len_size > tlv_len )
    {
      MMGSDI_DEBUG_MSG_ERROR_0("Invalid TLV data");
      return MMGSDI_ERROR;
    }

    /* Copy the tag and move the pointer to the len */
    tag = *tlv_data_ptr;
    tlv_data_ptr = tlv_data_ptr + tag_size;

    /* Copy the length and move the pointer to the data (if exists)*/
    len = *tlv_data_ptr;
    tlv_data_ptr = tlv_data_ptr + len_size;

    /* Check TLV size*/
    if ( index + tag_size + len_size + len > tlv_len )
    {
      MMGSDI_DEBUG_MSG_ERROR_0("Invalid TLV data");
      return MMGSDI_ERROR;
    }

    switch ( tag )
    {
      case MMGSDI_ACT_ONCHIP_AUTH_KEY_TAG:
        if (len > 0 && len <= MMGSDI_ONCHIP_SIM_AUTH_KEY_LEN)
        {
          mmgsdi_memscpy(slot_data_ptr->onchip_sim_data.auth_key,
            sizeof(slot_data_ptr->onchip_sim_data.auth_key),
            tlv_data_ptr,
            (uint32)len);

          /* Move the TLV Data Pointer Past the Auth Key to
          ** the next TAG
          */
          tlv_data_ptr = tlv_data_ptr + len;
        }
        else if (len > MMGSDI_ONCHIP_SIM_AUTH_KEY_LEN)
        {
          MMGSDI_DEBUG_MSG_ERROR_2("INVALID AUTH KEY LEN 0x%x > 0x%x",
                                   len, MMGSDI_ONCHIP_USIM_AUTH_KEY_LEN);
          return MMGSDI_ERROR;
        }

        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_MILENAGE_OP_TAG:
        if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG);
        }
        else if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG);
        }
        else if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG);
        }
        else if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_algo_root_key;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG);
        }
        else if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_MILENAGE_OP_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_milenage_op_data;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_MILENAGE_OP_TAG);
        }

        /* Zero old value */
        auth_rsp_ptr->data_len = 0;
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

        if ( len > 0 )
        {
          /* Not validating the payload because invalid responses might
             be used for adverse scenario test cases */

          /* Allocate buffer */
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(auth_rsp_ptr->data_ptr,
                                             (uint32)len);

          if (auth_rsp_ptr->data_ptr == NULL)
          {
            mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
            break;
          }

          /* Copy response in buffer */
          auth_rsp_ptr->data_len = len;
          mmgsdi_memscpy(auth_rsp_ptr->data_ptr,
                         len,
                         tlv_data_ptr,
                         (uint32)len);

          /* Move the TLV Data Pointer to the next TAG */
          tlv_data_ptr = tlv_data_ptr + len;
        }

        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG:
        if(len == sizeof(slot_data_ptr->onchip_sim_data.usim_auth_error_rsp))
        {
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG);

          mmgsdi_memscpy(slot_data_ptr->onchip_sim_data.usim_auth_error_rsp,
            sizeof(slot_data_ptr->onchip_sim_data.usim_auth_error_rsp),
            tlv_data_ptr,
            (uint32)len);

          /* Move the TLV Data Pointer to the next TAG */
          tlv_data_ptr = tlv_data_ptr + len;
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR_1("INVALID AUTH ERR LEN 0x%x", len);
          return MMGSDI_ERROR;
        }
        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ALGO_TAG:
        if ( len == sizeof(uint8) )
        {
          uint8 algorithm = 0;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ALGO_TAG);

          algorithm = *tlv_data_ptr;

          if (algorithm == (uint8)MMGSDI_ONCHIP_AUTH_ALGORITHM_FIXED ||
              algorithm == (uint8)MMGSDI_ONCHIP_AUTH_ALGORITHM_XOR   ||
              algorithm == (uint8)MMGSDI_ONCHIP_AUTH_ALGORITHM_MILENAGE)
          {
            slot_data_ptr->onchip_sim_data.usim_auth_algo =
              (mmgsdi_onchip_auth_algo_enum_type)algorithm;

            /* Move the TLV Data Pointer to the next TAG */
            tlv_data_ptr = tlv_data_ptr + len;
          }
          else
          {
            MMGSDI_DEBUG_MSG_ERROR_1("INVALID ALGORITHM 0x%x", algorithm);
            return MMGSDI_ERROR;
          }
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR_1("INVALID ALGO LEN 0x%x", len);
          return MMGSDI_ERROR;
        }

        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_AUTH_RES_LEN_TAG:
        if ( len == sizeof(uint8) )
        {
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_RES_LEN_TAG);

          slot_data_ptr->onchip_sim_data.usim_auth_algo_res_length = *tlv_data_ptr;

          /* Move the TLV Data Pointer to the next TAG */
          tlv_data_ptr = tlv_data_ptr + len;
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR_1("INVALID RES LEN 0x%x", len);
          return MMGSDI_ERROR;
        }

        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_MILENAGE_SEQ_TAG:
        if ( len == sizeof(uint64) )
        {
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_MILENAGE_SEQ_TAG);

          slot_data_ptr->onchip_sim_data.usim_auth_milenage_seq = (uint64) *tlv_data_ptr;

          /* Move the TLV Data Pointer to the next TAG */
          tlv_data_ptr = tlv_data_ptr + len;
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR_1("INVALID SEQ LEN 0x%x", len);
          return MMGSDI_ERROR;
        }

        index += (tag_size + len + len_size);

        continue;

      case MMGSDI_ACT_ONCHIP_IMSI_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_IMSI : MMGSDI_GSM_IMSI;
        if(len > 0)
        {
          /* Make sure that IMSI is one of the test IMSIs. For all other IMSIs,
             return ERROR so that we do not end up activating onchip for fake
             IMSIs (to avoid a situation where someone can for example make an
             emergency call using this fake IMSI) */
          mmgsdi_data_type        tlv_imsi_data;
          mmgsdi_imsi_data_type   imsi             = {{0}};

          memset(&tlv_imsi_data, 0, sizeof(mmgsdi_data_type));

          tlv_imsi_data.data_len = tlv_data_ptr[0];
          tlv_imsi_data.data_ptr = tlv_data_ptr;

          mmgsdi_status = mmgsdi_util_convert_to_imsi_data_type(&tlv_imsi_data,
                                                                &imsi);
          if(mmgsdi_status == MMGSDI_SUCCESS &&
             FALSE == mmgsdi_card_init_is_imsi_test_imsi(imsi))
          {
            mmgsdi_status = MMGSDI_ERROR;
          }
        }
        break;

      case MMGSDI_ACT_ONCHIP_LOCI_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_LOCI : MMGSDI_GSM_LOCI;
        break;

      case MMGSDI_ACT_ONCHIP_ACC_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_ACC : MMGSDI_GSM_ACC;
        break;

      case MMGSDI_ACT_ONCHIP_FPLMN_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_FPLMN : MMGSDI_GSM_FPLMN;
        break;

      case MMGSDI_ACT_ONCHIP_AD_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_AD : MMGSDI_GSM_AD;
        break;

      case MMGSDI_ACT_ONCHIP_EPSLOCI_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_EPSLOCI : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_HPLMNWACT_TAG:         /* HPLMN selector with access tech   */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_HPLMNWACT : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_PLMNWACT_TAG:          /* User Controlled PLMN Selector     */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_PLMNWACT : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_UST_TAG:               /* USIM SERVICE TABLE                */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_UST : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_EST_TAG:               /* USIM ENABLE SERVICE TABLE         */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_EST : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_EPSNSC_TAG:            /* EPS NAS Security Context          */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_EPSNSC : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_KEYSPS_TAG:            /* C and I keys for PS domain        */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_KEYSPS : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_KEYS_TAG:              /* C and I keys                      */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_KEYS : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_RPLMNACT_TAG:          /* RPLMN with access tech            */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_RPLMNACT : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_KC_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_KC : MMGSDI_USIM_KC;
        break;

      case MMGSDI_ACT_ONCHIP_PLMN_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_PLMN : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_SST_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_SST : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_PHASE_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_PHASE : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_PLMNSEL_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_HPLMN : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_ICCID_TAG:             /* ICCID                             */
        file_name = MMGSDI_ICCID;
        break;

      case MMGSDI_ACT_ONCHIP_KCGPRS_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_KCGPRS : MMGSDI_USIM_KCGPRS;
        break;

      default:
        MMGSDI_DEBUG_MSG_ERROR_1("Unhandled TAG 0x%x", *tlv_data_ptr);
        mmgsdi_status = MMGSDI_ERROR;
        break;
    }

    if ( mmgsdi_status != MMGSDI_SUCCESS )
    {
      break;
    }

    if ( len > 0 )
    {
      if (file_name == MMGSDI_NO_FILE_ENUM)
      {
        return MMGSDI_INCORRECT_PARAMS;
      }

      mmgsdi_status = mmgsdi_cache_write_item(file_name,
                                              (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                                                MMGSDI_APP_USIM : MMGSDI_APP_SIM,
                                              session_type,
                                              len,
                                              0,
                                              tlv_data_ptr,
                                              MMGSDI_DATA_FROM_CARD_COMPLETE);

      if ( mmgsdi_status != MMGSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR_0("Failed to Write Cache Data: 0x%x");
        break;
      }

      /* Move to the next Tag */
      tlv_data_ptr += len;
    }

    /* Increase the index by the size of the TAG, LEN SIZE, and DATA LEN */
    index += (tag_size + len + len_size);
  }

  if (onchip_mode == MMGSDI_ONCHIP_MODE_SIM)
  {
    /* Nothing further to be done if the onchip mode is SIM */
    return mmgsdi_status;
  }

  /* Check on the onchip usim authenticate tags
      - If error is present, no other tag should be present
      - If AUTS is present, no other tag should be present
      - It is possible to have 3G context and GSM context at same time */
  if (
      (MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG) &&
       (MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG) ||
        MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG) ||
        MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG)))
      ||
      (MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG) &&
       (MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG) ||
        MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG)))
     )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Too many onchip authenticate tags");
    mmgsdi_status = MMGSDI_ERROR;
  }
/* Check on the onchip usim authenticate tags for milenage and xor
      - Root key must be present */
  if (
      (slot_data_ptr->onchip_sim_data.usim_auth_algo == MMGSDI_ONCHIP_AUTH_ALGORITHM_XOR ||
       slot_data_ptr->onchip_sim_data.usim_auth_algo == MMGSDI_ONCHIP_AUTH_ALGORITHM_MILENAGE)
      &&
      (!MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG))
     )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Root key is missing");
    mmgsdi_status = MMGSDI_ERROR;
  }

  /* Default values for USIM authenticate
     They are used only if:
      - error and AUTS are not present in the TLV
      - values for GSM context or 3G context were not present in the TLV */
  if (mmgsdi_status == MMGSDI_SUCCESS &&
      slot_data_ptr->onchip_sim_data.usim_auth_algo == MMGSDI_ONCHIP_AUTH_ALGORITHM_FIXED &&
      !MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG) &&
      !MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG))
  {
    if (!MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG))
    {
      uint32 default_gsm_auth_rsp_len_uint32 = int32touint32(default_gsm_auth_rsp_len);
      auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(auth_rsp_ptr->data_ptr,
        default_gsm_auth_rsp_len_uint32);
      if (auth_rsp_ptr->data_ptr != NULL)
      {
        /* Copy response in buffer */
        auth_rsp_ptr->data_len = default_gsm_auth_rsp_len;
        mmgsdi_memscpy(auth_rsp_ptr->data_ptr,
          default_gsm_auth_rsp_len_uint32,
          default_gsm_auth_rsp_ptr,
          int32touint32(default_gsm_auth_rsp_len));
      }
      else
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
    if (!MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG))
    {
      uint32 default_3g_auth_rsp_len_uint32 = int32touint32(default_3g_auth_rsp_len);
      auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(auth_rsp_ptr->data_ptr,
                                         default_3g_auth_rsp_len_uint32);
      if (auth_rsp_ptr->data_ptr != NULL)
      {
        /* Copy response in buffer */
        auth_rsp_ptr->data_len = default_3g_auth_rsp_len;
        mmgsdi_memscpy(auth_rsp_ptr->data_ptr,
          default_3g_auth_rsp_len_uint32,
          default_3g_auth_rsp_ptr,
          int32touint32(default_3g_auth_rsp_len));
      }
      else
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Clear data for authenticate algorithm */
    mmgsdi_common_clear_onchip_usim_authenticate(slot_id);
  }

  return mmgsdi_status;
} /* mmgsdi_common_populate_usim_cache */


/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_ACTIVATE_ONCHIP

  DESCRIPTION:
    This function will be used to activate the ONCHIP USIM or SIM.  It will
    configure the USIM/SIM Cache.

  DEPENDENCIES:


  LIMITATIONS:
    USIM Authentication in USIM mode is not supported.

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
    to slot data ptr, channel info ptr, pin info ptr and app info ptr.
============================================================================*/
mmgsdi_return_enum_type mmgsdi_common_activate_onchip(
  mmgsdi_activate_onchip_sim_req_type * onchip_sim_config_ptr,
  mmgsdi_onchip_mode_enum_type          onchip_mode
)
{
  mmgsdi_return_enum_type          mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_event_data_type          *evt_info_ptr       = NULL;
  mmgsdi_int_app_info_type        *app_info_ptr       = NULL;
  mmgsdi_int_app_info_type       **app_info_pptr      = NULL;
  mmgsdi_app_pin_info_type        *pin1_ptr           = NULL;
  mmgsdi_slot_data_type           *slot_data_ptr      = NULL;
  mmgsdi_session_info_type        *session_info_ptr   = NULL;
  uint32                           i                  = 0;
  uint32                           j                  = 0;
  mmgsdi_task_enum_type            mmgsdi_task        = MMGSDI_TASK_MAX;
  mmgsdi_evt_session_notify_type   notify_type;
  uint8                            app_index          = 0;
  int32                            channel_info_index = MMGSDI_MAX_CHANNEL_INFO;
  uint8                            slot_index         = 0;
  mmgsdi_session_id_type           session_id         = 0;
  uint8                            app_sel_index      = 0;
  mmgsdi_session_type_enum_type    session_type       = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_channel_info_type        *channel_info_ptr   = NULL;
  mmgsdi_slot_id_enum_type         slot_id            = MMGSDI_SLOT_1;
  const uint8                      usim_aid_value[]   = { MMGSDI_3GPP_APP_ID_RID_BYTE_1,
                                                          MMGSDI_3GPP_APP_ID_RID_BYTE_2,
                                                          MMGSDI_3GPP_APP_ID_RID_BYTE_3,
                                                          MMGSDI_3GPP_APP_ID_RID_BYTE_4,
                                                          MMGSDI_3GPP_APP_ID_RID_BYTE_5,
                                                          MMGSDI_USIM_APP_CODE_BYTE_1,
                                                          MMGSDI_USIM_APP_CODE_BYTE_2 };

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the current task is undetermined,
       default to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }

  /* --------------------------------------------------------------------------
     Validate the input Parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(onchip_sim_config_ptr);
  if ( onchip_sim_config_ptr->onchip_sim.data_len > 0 )
  {
    MMGSDIUTIL_RETURN_IF_NULL(onchip_sim_config_ptr->onchip_sim.data_ptr);
  }
  MMGSDIUTIL_RETURN_IF_EXCEEDS(onchip_sim_config_ptr->onchip_sim.data_len,
                           MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE);

  slot_id = onchip_sim_config_ptr->request_header.slot_id;
  if (mmgsdi_util_get_slot_index(slot_id, &slot_index) != MMGSDI_SUCCESS)
  {
    return mmgsdi_common_rsp_activate_onchip(MMGSDI_ERROR,onchip_sim_config_ptr);
  }

  /* Set app select index and session id depending on slot. For simplicity,
     primary session is mapped to slot 1, secondary to slot 2 and so on. */
  if (slot_id == MMGSDI_SLOT_1)
  {
    session_id = mmgsdi_generic_data_ptr->pri_gw_session_id;
    app_sel_index = MMGSDI_PRI_GW_PROV_APP_SEL_INDEX;
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }
  else if (slot_id == MMGSDI_SLOT_2)
  {
    session_id = mmgsdi_generic_data_ptr->sec_gw_session_id;
    app_sel_index = MMGSDI_SEC_GW_PROV_APP_SEL_INDEX;
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  {
    session_id = mmgsdi_generic_data_ptr->ter_gw_session_id;
    app_sel_index = MMGSDI_TER_GW_PROV_APP_SEL_INDEX;
    session_type = MMGSDI_GW_PROV_TER_SESSION;
  }

  /* Proceed only if the session id is inactive or active on the same slot on
     which OnChip is being activated. */
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    session_id,
                    NULL,
                    &channel_info_ptr);
  if (mmgsdi_status == MMGSDI_SUCCESS &&
      channel_info_ptr != NULL &&
      channel_info_ptr->slot_id != slot_id)
  {
    UIM_MSG_ERR_2("Session type 0x%x active on slot 0x%x; cannot proceed with OnChip",
                  session_type, channel_info_ptr->slot_id);
    return mmgsdi_common_rsp_activate_onchip(MMGSDI_ERROR, onchip_sim_config_ptr);
  }

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  /* --------------------------------------------------------------------------
     Validate the TLV Data
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_validate_onchip_config(
                       &onchip_sim_config_ptr->onchip_sim,
                       onchip_mode);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Validate ONCHIP SIM TLV Failure");
    /* No external client...directly generate a response */
    return mmgsdi_common_rsp_activate_onchip(mmgsdi_status,onchip_sim_config_ptr);
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if (evt_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cannot alloc evt_info_ptr, error");
    /* Directly generate a response */
    return mmgsdi_common_rsp_activate_onchip(mmgsdi_status,onchip_sim_config_ptr);
  }

  /* --------------------------------------------------------------------------
   Use the REFRESH SIM RESET Command to trigger the UI to get into
   the correct state so that a GSDI_SIM_INSERTED can be handled
   ------------------------------------------------------------------------*/
  UIM_MSG_HIGH_0("Sending out MMGSDI_REFRESH_STAGE_IN_PROGRESS");
  evt_info_ptr->evt                    = MMGSDI_REFRESH_EVT;
  evt_info_ptr->data.refresh.slot      = slot_id;
  evt_info_ptr->data.refresh.stage     = MMGSDI_REFRESH_STAGE_START;
  evt_info_ptr->data.refresh.mode      = MMGSDI_REFRESH_RESET;
  evt_info_ptr->data.refresh.orig_mode = MMGSDI_REFRESH_RESET;

  /* Send out the REFRESH start event */
  mmgsdi_util_send_refresh_evt_for_onchip_act(evt_info_ptr, slot_id);

  /* Allocating memory  for Slot data*/
  if(mmgsdi_data_slot_ptr[slot_index] == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_data_slot_ptr[slot_index],
      sizeof(mmgsdi_slot_data_type));

    if(mmgsdi_data_slot_ptr[slot_index] == NULL)
    {
      return MMGSDI_ERROR;
    }
  }

  /* Reset session and application data */
  mmgsdi_util_preinit_mmgsdi(0,
                             MMGSDI_APP_NONE,
                             MMGSDI_REFRESH_RESET,
                             slot_id,
                             FALSE);

  mmgsdi_state_update_card_error_state(slot_id);

  mmgsdi_util_timed_sleep(200);

  UIM_MSG_HIGH_0("Sending out MMGSDI_REFRESH_STAGE_END_SUCCESS");
  evt_info_ptr->data.refresh.stage = MMGSDI_REFRESH_STAGE_END_SUCCESS;

  /* Send out the REFRESH END_SUCCESS event */
  mmgsdi_util_send_refresh_evt_for_onchip_act(evt_info_ptr, slot_id);

  /* Allocating memory  for application*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_app_pin_table_ptr[slot_index][app_index],
    sizeof(mmgsdi_int_app_info_type));
  if(mmgsdi_app_pin_table_ptr[slot_index][app_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Allocating memory  for PIN1 info*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_app_pin1_table_ptr[slot_index][app_index],
    sizeof(mmgsdi_app_info_type));
  if(mmgsdi_app_pin1_table_ptr[slot_index][app_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_data_slot_ptr[slot_index];
  app_info_ptr = mmgsdi_app_pin_table_ptr[slot_index][app_index];
  app_info_pptr = mmgsdi_app_pin_table_ptr[slot_index];
  pin1_ptr = mmgsdi_app_pin1_table_ptr[slot_index][app_index];

  slot_data_ptr->onchip_sim_data.state = MMGSDI_ONCHIP_SIM_NOT_VALID;
  slot_data_ptr->mmgsdi_uim_sanity_timer_value = MMGSDI_UIM_SANITY_ONCHIP_TIMER_VALUE;
  if (onchip_mode == MMGSDI_ONCHIP_MODE_USIM)
  {
    slot_data_ptr->protocol = MMGSDI_UICC;
  }
  else
  {
    slot_data_ptr->protocol = MMGSDI_ICC;
  }

  /* --------------------------------------------------------------------------
     Now Set the MMGSDI Global Data Variables
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* Set select index to the first location */
  slot_data_ptr->prov_app_indices[app_sel_index] =
    MMGSDI_PROV_INDEX_VALIDITY_MASK | 0;

  /* Assign the first index location for the app info global table */
  app_info_ptr->app_selected = TRUE;

  /* Assign the first pin1 from the pin1 table to the app */
  app_info_ptr->pin1_ptr = pin1_ptr;

  /* populate pin1 info */
  pin1_ptr->key_ref             = MMGSDI_CHV1_KEY;
  pin1_ptr->pin_id              = MMGSDI_PIN1;
  pin1_ptr->status              = MMGSDI_PIN_DISABLED;
  pin1_ptr->pin_replacement     = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;
  pin1_ptr->num_retries         = 3;
  pin1_ptr->num_unblock_retries = 10;
  pin1_ptr->cached              = TRUE;

  /* populate pin2 info */
  app_info_ptr->pin2.key_ref             = MMGSDI_CHV2_KEY;
  app_info_ptr->pin2.pin_id              = MMGSDI_PIN2;
  app_info_ptr->pin2.status              = MMGSDI_PIN_DISABLED;
  app_info_ptr->pin2.pin_replacement     = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;
  app_info_ptr->pin2.num_retries         = 3;
  app_info_ptr->pin2.num_unblock_retries = 10;
  app_info_ptr->pin2.cached = TRUE;

  if (onchip_mode == MMGSDI_ONCHIP_MODE_USIM)
  {
    app_info_ptr->app_data.app_type = MMGSDI_APP_USIM;

    /* Set length to max len 16, it should not be 0 for USIM app */
    app_info_ptr->app_data.aid.data_len = MMGSDI_MAX_APP_ID_LEN;

    /* Set first seven byte of AID data to 3GPP USIM App identifier */
    memscpy(app_info_ptr->app_data.aid.data_ptr,
            sizeof(app_info_ptr->app_data.aid.data_ptr),
            usim_aid_value,
            sizeof(usim_aid_value));
  }
  else
  {
    app_info_ptr->app_data.app_type = MMGSDI_APP_SIM;
    app_info_ptr->app_data.aid.data_len = 0;
  }
  app_info_ptr->app_state         = MMGSDI_APP_STATE_DETECTED;

  /* assign the global data to the slot1 global data */
  slot_data_ptr->app_info_pptr    = app_info_pptr;
  slot_data_ptr->mmgsdi_tp_state = MMGSDI_TP_STATE_DONE;

  mmgsdi_generic_data_ptr->me_capabilities.me_supports_fdn = TRUE;
  mmgsdi_generic_data_ptr->me_capabilities.me_supports_bdn = TRUE;
  mmgsdi_generic_data_ptr->me_capabilities.me_supports_acl = TRUE;

  /* Get a free index from channel info table to avoid using a channel that is
     being used. */
  mmgsdi_status = mmgsdi_util_get_free_channel_index(&channel_info_index);
  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if (mmgsdi_status      != MMGSDI_SUCCESS ||
      channel_info_index >= MMGSDI_MAX_CHANNEL_INFO)
  {
    UIM_MSG_ERR_0("Could not find free channel index");
    /* free evt_info_ptr */
    mmgsdi_evt_free_data(evt_info_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    session_id,
                    &session_info_ptr,
                    NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) ||  (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Could not get GW_PRI Session info");

    /* free evt_info_ptr */
    mmgsdi_evt_free_data(evt_info_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
  do
  {
    if (mmgsdi_channel_info_ptr_table[channel_info_index] == NULL)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        mmgsdi_channel_info_ptr_table[channel_info_index],
        sizeof(mmgsdi_channel_info_type));
    }

    if (mmgsdi_channel_info_ptr_table[channel_info_index] == NULL)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    }

    memset(mmgsdi_channel_info_ptr_table[channel_info_index], 0x00,
           sizeof(mmgsdi_channel_info_type));

    session_info_ptr->channel_info_index = (uint8)channel_info_index;

    mmgsdi_channel_info_ptr_table[channel_info_index]->num_connections++;
    mmgsdi_channel_info_ptr_table[channel_info_index]->channel_id = MMGSDI_DEFAULT_CHANNEL;
    mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id = slot_id;
    mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr = app_info_ptr;

    for(i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
    {
      if (mmgsdi_client_id_reg_table[i] == NULL)
      {
        continue;
      }
      for(j = 0; j < MMGSDI_MAX_SESSION_INFO; j++)
      {
        uint8 curr_channel_info_index = 0;

        if (mmgsdi_client_id_reg_table[i]->session_info_ptr[j] == NULL)
        {
          continue;
        }

        curr_channel_info_index = mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index;
        if(curr_channel_info_index != channel_info_index)
        {
          if(/* Update channel info for sessions of provisioning type */
             (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
              session_info_ptr->session_type)
                                               ||
             /* Update channel info for sessions of card slot type */
             (
              (curr_channel_info_index < MMGSDI_MAX_CHANNEL_INFO)    &&
              mmgsdi_channel_info_ptr_table[curr_channel_info_index] &&
              (mmgsdi_channel_info_ptr_table[curr_channel_info_index]->channel_id ==
                MMGSDI_DEFAULT_CHANNEL) &&
              (
               /* Update channel info for sessions of card slot1 type */
               ((mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id == MMGSDI_SLOT_1) &&
                (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                 MMGSDI_CARD_SESSION_SLOT_1))
                                               ||
               /* Update channel info for sessions of card slot2 type */
               ((mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id == MMGSDI_SLOT_2) &&
                (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                 MMGSDI_CARD_SESSION_SLOT_2))
                                               ||
               /* Update channel info for sessions of card slot3 type */
               ((mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id == MMGSDI_SLOT_3) &&
                (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                 MMGSDI_CARD_SESSION_SLOT_3))
               )
              )
             )
          {
            UIM_MSG_HIGH_1("Channel mapping updated for session_id=0x%x",
                           mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_id);
            if((curr_channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
               (mmgsdi_channel_info_ptr_table[curr_channel_info_index] != NULL) &&
               (mmgsdi_channel_info_ptr_table[curr_channel_info_index]->num_connections > 0))
            {
              mmgsdi_channel_info_ptr_table[curr_channel_info_index]->num_connections--;
            }
            mmgsdi_channel_info_ptr_table[channel_info_index]->num_connections++;
            mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index =
              (uint8)channel_info_index;
          }
        }
      } /* for j<MAX_SESSION_INFO */
    } /* for i<MAX_CLIENT_INFO */
  }
  while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* free evt_info_ptr */
    mmgsdi_evt_free_data(evt_info_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);
    return MMGSDI_ERROR;
  }

  if (onchip_mode == MMGSDI_ONCHIP_MODE_USIM)
  {
    /* Clear data for authenticate algorithm */
    mmgsdi_common_clear_onchip_usim_authenticate(slot_id);
  }

  /* Set the appropriate flag to indicate an activate ONCHIP SIM */
  slot_data_ptr->onchip_sim_data.state = MMGSDI_ONCHIP_SIM_INIT;

  /* --------------------------------------------------------------------------
     Now Notify the tasks that we have a SIM Inserted.
     ------------------------------------------------------------------------*/
  UIM_MSG_HIGH_0("MMGSDI_CARD_INSERTED_EVT");

  mmgsdi_evt_build_and_send_card_inserted(TRUE, 0, slot_id);

  /* Sleep for another 100 ms while the SIM INSERTED propogates */
  mmgsdi_util_timed_sleep(100);

  UIM_MSG_HIGH_0("MMGSDI_SESSION_CHANGED_EVT");

  /* Assign session notify type info */
  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = slot_id;

  mmgsdi_evt_build_and_send_session_changed(
    notify_type, TRUE, (const mmgsdi_int_app_info_type*)app_info_ptr);

  /* Sleep for another 100 ms while the SESSION CHANGED propogates */
  mmgsdi_util_timed_sleep(100);

  /* --------------------------------------------------------------------------
     Now populate the USIM or SIM cache
     ------------------------------------------------------------------------*/
  if ( onchip_sim_config_ptr->onchip_sim.data_len > 0 )
  {
    mmgsdi_status = mmgsdi_common_populate_onchip_cache(&onchip_sim_config_ptr->onchip_sim,
                                                        slot_id,
                                                        session_type,
                                                        onchip_mode);
  }
  /* --------------------------------------------------------------------------
     Now Notify the tasks that we have a SIM Inserted.
     With an immediate notification that the SIM Init has Completed.
     ------------------------------------------------------------------------*/
  if ( mmgsdi_status == MMGSDI_SUCCESS )
  {
#ifdef FEATURE_LTE
    /* Initialize AKA algorithm if required */
    if (onchip_mode == MMGSDI_ONCHIP_MODE_USIM &&
        slot_data_ptr->onchip_sim_data.usim_auth_algo ==
          MMGSDI_ONCHIP_AUTH_ALGORITHM_MILENAGE)
    {
      lte_init_milenage_aka(
        slot_data_ptr->onchip_sim_data.usim_auth_algo_root_key.data_ptr,
        int32touint8(slot_data_ptr->onchip_sim_data.usim_auth_algo_root_key.data_len),
        slot_data_ptr->onchip_sim_data.usim_auth_milenage_op_data.data_ptr,
        int32touint8(slot_data_ptr->onchip_sim_data.usim_auth_milenage_op_data.data_len),
        slot_data_ptr->onchip_sim_data.usim_auth_milenage_seq);
    }
#endif /* FEATURE_LTE */

    UIM_MSG_HIGH_0("MMGSDI_PIN1_DISABLED_EVT");
    (void)mmgsdi_evt_build_pin(session_id,
            MMGSDI_PIN1, MMGSDI_PIN_DISABLED, evt_info_ptr);

    mmgsdi_evt_notify_session(evt_info_ptr,
                              slot_id,
                              (const mmgsdi_int_app_info_type*)app_info_ptr,
                              TRUE);

    /* Sleep for another 500 ms while the PIN1 Disabled propogates */
    mmgsdi_util_timed_sleep(100);

    UIM_MSG_HIGH_0("MMGSDI_PIN2_DISABLED_EVT");
    (void)mmgsdi_evt_build_pin(session_id,
            MMGSDI_PIN2, MMGSDI_PIN_DISABLED, evt_info_ptr);

    mmgsdi_evt_notify_session(evt_info_ptr,
                              slot_id,
                              (const mmgsdi_int_app_info_type*)app_info_ptr,
                              TRUE);

    /* Sleep for another 100 ms while the PIN2 Disabled propogates */
    mmgsdi_util_timed_sleep(100);

    app_info_ptr->perso_state = MMGSDI_SIMLOCK_VERIFIED;
    app_info_ptr->pre_perso_files_cached = TRUE;

#ifdef FEATURE_SIMLOCK
    mmgsdi_simlock_build_and_run_algo(TRUE);

    /* Sleep for another 100 ms while SIMLOCK module updates valid IMSI list */
    mmgsdi_util_timed_sleep(100);

    UIM_MSG_HIGH_0("MMGSDI_PERSO_EVT");

    /* Assign session notify type info */
    notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
    notify_type.slot_id     = slot_id;

    mmgsdi_evt_build_and_send_perso(
      notify_type,
      (const mmgsdi_int_app_info_type*)app_info_ptr,
      MMGSDI_PERSO_STATUS_DONE,
      MMGSDI_MAX_PERSO_FEATURE_ENUM,
      0,
      0);
#endif /* FEATURE_SIMLOCK */

    UIM_MSG_HIGH_0("MMGSDI_SUBSCRIPTION_READY_EVT");

    mmgsdi_evt_build_and_send_subscription_ready(
      notify_type, (const mmgsdi_int_app_info_type*)app_info_ptr);

    app_info_ptr->app_state = MMGSDI_APP_STATE_READY;

    /* Sleep for another 100 ms while the SUBS_READY propogates */
    mmgsdi_util_timed_sleep(100);
  }
  else
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Failed to populate USIM CACHE");
    slot_data_ptr->onchip_sim_data.state = MMGSDI_ONCHIP_SIM_NOT_VALID;
    mmgsdi_status = MMGSDI_ERROR;
  }

  /* free evt_info_ptr */
  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);

  /* Directly generate a response */
  return mmgsdi_common_rsp_activate_onchip(mmgsdi_status,
                                           onchip_sim_config_ptr);
} /* mmgsdi_common_activate_onchip */


/*============================================================================
   FUNCTION:      MMGSDI_COMMON_COMPUTE_IP_AUTH

   DESCRIPTION
     This function is called for a Compute IP Authentication request.

   DEPENDENCIES:
     None

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
mmgsdi_return_enum_type mmgsdi_common_compute_ip_auth(
  const mmgsdi_compute_ip_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                  protocol
)
{
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if ((protocol != MMGSDI_ICC) && (protocol != MMGSDI_UICC))
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  return mmgsdi_uim_common_compute_ip_auth(req_ptr, protocol);
}/* mmgsdi_common_compute_ip_auth */


/*============================================================================
   FUNCTION:      MMGSDI_COMMON_RUN_CAVE

   DESCRIPTION
     This function is called for a RUN CAVE ALGORITHM request.

   DEPENDENCIES:
     None

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
mmgsdi_return_enum_type mmgsdi_common_run_cave(
  const mmgsdi_run_cave_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
)
{
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (protocol != MMGSDI_ICC && protocol != MMGSDI_UICC)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  return mmgsdi_uim_common_run_cave(req_ptr, protocol);
} /* mmgsdi_common_run_cave */


/*============================================================================
   FUNCTION:      MMGSDI_COMMON_CDMA_AKA_AUTH

   DESCRIPTION
     This function is called for a CDMA AKA Authenticate request.

   DEPENDENCIES:
     None

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
mmgsdi_return_enum_type mmgsdi_common_cdma_aka_auth(
  const mmgsdi_cdma_aka_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  boolean                        cdma_aka_avail = FALSE;
  mmgsdi_return_enum_type        mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type  session_type   = MMGSDI_MAX_SESSION_TYPE_ENUM;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol != MMGSDI_ICC && protocol != MMGSDI_UICC)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_get_session_type(req_ptr->request_header.session_id,
                                               &session_type, NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     (session_type != MMGSDI_1X_PROV_PRI_SESSION &&
      session_type != MMGSDI_1X_PROV_SEC_SESSION &&
      session_type != MMGSDI_1X_PROV_TER_SESSION ))
  {
    UIM_MSG_ERR_1("Invalid session type: %d",session_type);
    return MMGSDI_ERROR;
  }

  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                  req_ptr->request_header.session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_CSIM_SRV_AKA,
                                                  &cdma_aka_avail);
  }
  else
  {
    mmgsdi_status = mmgsdi_icc_chk_srv_available(req_ptr->request_header.session_id,
                                                 MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                 MMGSDI_CDMA_SRV_AKA,
                                                 &cdma_aka_avail);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS || cdma_aka_avail == FALSE)
  {
    UIM_MSG_ERR_2("CDMA AKA AUTH service check failed. status: %d, srv avail: %d",
                  mmgsdi_status,cdma_aka_avail);
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_cdma_aka_auth(req_ptr, protocol);
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_cdma_aka_auth */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_GENERATE_KEY_VPM

  DESCRIPTION:
    This function will allow GENERATE KEY / VPM command to be sent to UIM

  DEPENDENCIES:


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
mmgsdi_return_enum_type mmgsdi_common_generate_key_vpm (
  const mmgsdi_generate_key_req_type   *req_ptr,
  mmgsdi_protocol_enum_type             protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol != MMGSDI_ICC && protocol != MMGSDI_UICC)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  return mmgsdi_uim_common_generate_key_vpm(req_ptr, protocol);
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_generate_key_vpm */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_BLOCK

  DESCRIPTION:
    This function will allow OTASP Block Request command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application


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
mmgsdi_return_enum_type  mmgsdi_common_otasp_block (
  const mmgsdi_session_otasp_block_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                      protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  if (protocol != MMGSDI_ICC && protocol != MMGSDI_UICC)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  return mmgsdi_uim_common_otasp_block(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_block */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_OTAPA

  DESCRIPTION:
    This function will allow OTAPA Request command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_otapa (
  const mmgsdi_session_otasp_otapa_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                      protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  /* No need to check callback,
     already checked when preparing mmgsdi command*/
  /* No need to check randseed,
     already checked when preparing mmgsdi command */
  if (protocol == MMGSDI_UICC && 0 == req_ptr->esn)
  {
    UIM_MSG_ERR_0("ESN value should not be 0x00 for UICC OTAPA Request");
    return MMGSDI_ERROR;
  }
  
  return mmgsdi_uim_common_otasp_otapa(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_otapa */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_COMMIT

  DESCRIPTION:
    This function will allow OTASP Commit Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_commit (
  const mmgsdi_session_otasp_commit_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                       protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_otasp_commit(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_commit */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_SSPR_CONFIG

  DESCRIPTION:
    This function will allow OTASP SSPR Config Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_sspr_config (
  const mmgsdi_session_otasp_sspr_config_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_otasp_sspr_config(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_sspr_config */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_KEY_GEN

  DESCRIPTION:
    This function will allow OTASP KEY GEN Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_key_gen (
  const mmgsdi_session_otasp_key_gen_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                        protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_otasp_key_gen(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_key_gen */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_MS_KEY

  DESCRIPTION:
    This function will allow OTASP MS Key Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_ms_key (
  const mmgsdi_session_otasp_ms_key_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                       protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_otasp_ms_key(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_ms_key */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_BS_CHAL

  DESCRIPTION:
    This function will allow BS CHAL Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_bs_chal (
  const mmgsdi_session_bs_chal_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                  protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_bs_chal(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_bs_chal */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_SSD_CONFIRM

  DESCRIPTION:
    This function will allow SSD Confirm Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_ssd_confirm (
  const mmgsdi_session_ssd_confirm_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                      protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_ssd_confirm(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_ssd_confirm */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_SSD_UPDATE

  DESCRIPTION:
    This function will allow SSD Update Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_ssd_update (
  const mmgsdi_session_ssd_update_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                     protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_ssd_update(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_ssd_update */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_PIN_OPERATION

  DESCRIPTION:
    This function will perform pin operations

  DEPENDENCIES:

  LIMITATIONS:

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
mmgsdi_return_enum_type  mmgsdi_common_pin_operation (
  const mmgsdi_pin_operation_req_type     * req_ptr,
  mmgsdi_protocol_enum_type                 protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_int_app_info_type  *sel_app_ptr       = NULL;
  uint32                     pin_index         = 0;
  mmgsdi_app_pin_info_type **app_pin1_tbl_pptr = NULL;
  uint8                      app_pin1_tbl_size = 0;
  mmgsdi_session_info_type  *session_info_ptr  = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol == MMGSDI_ICC &&
      (req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_1 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_2 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_3 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_4 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_5 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_6 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_7 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_8 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_1 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_2 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_3 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_4 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_5 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_6 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_7 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_8))
  {
    UIM_MSG_ERR_1("Key ref based PIN input 0x%x not supported for ICC mode",
                  req_ptr->orig_pin_id);
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(req_ptr->request_header.session_id,
                                                   NULL,
                                                   NULL,
                                                   &sel_app_ptr,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS || sel_app_ptr == NULL ||
      session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Block PIN operations for non-provisioning apps in ICC mode because
     MMGSDI has valid PIN info only for prov sessions. Streaming APDUs
     should be used to perform such PIN operations. */
  if(protocol == MMGSDI_ICC &&
     mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
  {
    UIM_MSG_ERR_1("PIN OP not supported for session type 0x%x in non-UICC mode",
                  session_info_ptr->session_type);
    return MMGSDI_NOT_SUPPORTED;
  }

  if(protocol == MMGSDI_UICC &&
     req_ptr->pin_id == MMGSDI_UNIVERSAL_PIN &&
     sel_app_ptr->universal_pin_ptr != NULL &&
     sel_app_ptr->universal_pin_ptr->status == MMGSDI_PIN_DISABLED &&
     req_ptr->pin_op != MMGSDI_PIN_OP_ENABLE)
  {
    UIM_MSG_ERR_0("UPIN is not in use or disabled - PIN operation skipped");
    return MMGSDI_ERROR;
  }

  if (protocol == MMGSDI_UICC &&
      sel_app_ptr->fcp_pin_data_absent &&
      (req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_1 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_2 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_3 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_4 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_5 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_6 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_7 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_8))
  {
    mmgsdi_key_ref_values_enum_type   input_key_ref = MMGSDI_PIN_APPL_1;

    switch (req_ptr->orig_pin_id)
    {
      case MMGSDI_KEY_REF_PIN_APPL_1:
        input_key_ref = MMGSDI_PIN_APPL_1;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_2:
        input_key_ref = MMGSDI_PIN_APPL_2;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_3:
        input_key_ref = MMGSDI_PIN_APPL_3;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_4:
        input_key_ref = MMGSDI_PIN_APPL_4;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_5:
        input_key_ref = MMGSDI_PIN_APPL_5;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_6:
        input_key_ref = MMGSDI_PIN_APPL_6;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_7:
        input_key_ref = MMGSDI_PIN_APPL_7;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_8:
        input_key_ref = MMGSDI_PIN_APPL_8;
        break;
      default:
        break;
    }
    if (sel_app_ptr->pin1_ptr->key_ref != input_key_ref)
    {
      UIM_MSG_HIGH_0("PIN1 keyref switch as req keyref mismatch with curr keyref");

      app_pin1_tbl_pptr = mmgsdi_util_get_app_pin1_info_tbl_ptr(&app_pin1_tbl_size,
                                                                req_ptr->request_header.slot_id);
      if(app_pin1_tbl_pptr == NULL ||
         app_pin1_tbl_size == 0)
      {
        UIM_MSG_ERR_0("Could not get PIN1 table");
        return MMGSDI_ERROR;
      }

      /* Ignoring return value because app pin table is fixed and pin index for
         a particular key ref will always be found */
      (void)mmgsdi_util_find_app_pin_index((const mmgsdi_app_pin_info_type **)app_pin1_tbl_pptr,
                                           app_pin1_tbl_size,
                                           input_key_ref,
                                           &pin_index);
      sel_app_ptr->pin1_ptr = app_pin1_tbl_pptr[pin_index];
    }
  }

  if (protocol == MMGSDI_UICC &&
      sel_app_ptr->fcp_pin_data_absent &&
      (req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_1 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_2 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_3 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_4 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_5 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_6 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_7 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_8))
  {
    mmgsdi_key_ref_values_enum_type   input_key_ref = MMGSDI_SEC_PIN_APPL_1;
    switch (req_ptr->orig_pin_id)
    {
      case MMGSDI_KEY_REF_SEC_PIN_APPL_1:
        input_key_ref = MMGSDI_SEC_PIN_APPL_1;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_2:
        input_key_ref = MMGSDI_SEC_PIN_APPL_2;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_3:
        input_key_ref = MMGSDI_SEC_PIN_APPL_3;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_4:
        input_key_ref = MMGSDI_SEC_PIN_APPL_4;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_5:
        input_key_ref = MMGSDI_SEC_PIN_APPL_5;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_6:
        input_key_ref = MMGSDI_SEC_PIN_APPL_6;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_7:
        input_key_ref = MMGSDI_SEC_PIN_APPL_7;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_8:
        input_key_ref = MMGSDI_SEC_PIN_APPL_8;
        break;
      default:
        break;
    }

    if (sel_app_ptr->pin2.key_ref != input_key_ref)
    {
      UIM_MSG_HIGH_0("PIN2 keyref switch as req keyref mismatch with curr keyref");
      sel_app_ptr->pin2.key_ref = input_key_ref;
    }
  }

  if(protocol == MMGSDI_UICC && req_ptr->pin_id == MMGSDI_PIN2)
  {
    if(MMGSDI_SUCCESS == mmgsdi_util_select_adf_before_pin2_op(
          req_ptr->request_header.session_id))
    {
      UIM_MSG_HIGH_0("Selected ADF successfully prior to PIN2 operation");
    }
  }

  /* Check for access method only */
  switch(req_ptr->pin_op)
  {
    case MMGSDI_PIN_OP_VERIFY:
      mmgsdi_status = mmgsdi_uim_common_verify_pin(req_ptr,
                                                   MMGSDI_RETRY_NONE,
                                                   protocol);
      break;
    case MMGSDI_PIN_OP_ENABLE:
      mmgsdi_status = mmgsdi_uim_common_enable_pin(req_ptr, protocol);
      break;
    case MMGSDI_PIN_OP_DISABLE_AND_REPLACE:
    case MMGSDI_PIN_OP_DISABLE:
      if (protocol == MMGSDI_ICC)
      {
        if (req_ptr->pin_op == MMGSDI_PIN_OP_DISABLE_AND_REPLACE)
        {
          return MMGSDI_INCORRECT_PARAMS;
        }

        if (mmgsdi_icc_is_disable_chv1_allowed(
              req_ptr->request_header.session_id) == FALSE)
        {
          return MMGSDI_NOT_SUPPORTED;
        }
      }
      mmgsdi_status = mmgsdi_uim_common_disable_pin(req_ptr, protocol);
      break;
    case MMGSDI_PIN_OP_CHANGE:
      mmgsdi_status = mmgsdi_uim_common_change_pin(req_ptr, protocol);
      break;
    case MMGSDI_PIN_OP_UNBLOCK:
      mmgsdi_status = mmgsdi_uim_common_unblock_pin(req_ptr,
                                                    MMGSDI_RETRY_NONE,
                                                    protocol);
      break;
    default:
      return MMGSDI_INCORRECT_PARAMS;
  }
  return mmgsdi_status;
} /* mmgsdi_common_pin_operation */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_SEARCH

  DESCRIPTION:
    This function will provide search access to record-based file

  DEPENDENCIES:


  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.
    MMGSDI_NOT_SUPPORTED    : When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support.
                              When the Search Type is MMGSDI_ICC_SEARCH.

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_search (
  mmgsdi_search_req_type      * req_ptr,
  mmgsdi_protocol_enum_type     protocol
)
{
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type          slot                 = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type         *prov_app_info_ptr    = NULL;

  /*---------------------------------------------------------------------------
   FUNCTION PARAMETER CHECKS
   1. req_ptr                                  - Cannot be NULL
   2. req_ptr->request_header.response_cb      - Cannot be NULL
   3. req_ptr->search_type                     - can not be MMGSDI_ICC_SEARCH
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  /* Checking of search type */
  if((protocol == MMGSDI_UICC && MMGSDI_ICC_SEARCH == req_ptr->search_type) ||
     (protocol == MMGSDI_ICC && MMGSDI_ICC_SEARCH != req_ptr->search_type))
  {
    UIM_MSG_ERR_2("Search type 0x%x is not supported for protocol 0x%x",
                  req_ptr->search_type, protocol);
    return MMGSDI_NOT_SUPPORTED;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_check_file_access_data(
                                           req_ptr->request_header.session_id,
                                           req_ptr->access,
                                           protocol))
  {
    UIM_MSG_ERR_0("File Access is not allowed");
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_prov_session_info(req_ptr->request_header.session_id,
                                       NULL,
                                       &slot,
                                       &prov_app_info_ptr) != MMGSDI_SUCCESS)
  {
    prov_app_info_ptr = NULL;
  }

  if (prov_app_info_ptr != NULL)
  {
    /*Check to see if the security attributes allow this file to be Searched*/
    mmgsdi_status = mmgsdi_util_check_file_attr(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      slot,
                      prov_app_info_ptr,
                      &req_ptr->access,
                      MMGSDI_SEARCH_REQ,
                      FALSE);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("File Searching is  not allowed");
      return mmgsdi_status;
    }

    if (protocol == MMGSDI_UICC)
    {
      mmgsdi_util_check_usim_spec_version(&req_ptr->access,
                                          prov_app_info_ptr->usim_spec_version);
    }

    /* Check status of EF in cached UST/SST */
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      prov_app_info_ptr->app_data.app_type,
                      &req_ptr->access);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_print_file_information(req_ptr->access,TRUE);
      UIM_MSG_ERR_1("SERVICE not allowed: error 0x%x", mmgsdi_status);
      return mmgsdi_status;
    }
  }

  UIM_MSG_HIGH_0("Sending down SEARCH command to UIM");
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uim_uicc_search_record(req_ptr, FALSE, NULL);
  }
  else
  {
    mmgsdi_status = mmgsdi_uim_icc_search_record(req_ptr, FALSE, NULL);
  }

  return mmgsdi_status;
}/* mmgsdi_common_search */
