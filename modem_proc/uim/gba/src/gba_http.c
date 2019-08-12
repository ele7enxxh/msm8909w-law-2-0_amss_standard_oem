/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   H T T P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA http functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2015-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gba/src/gba_http.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/16   vdc     Remove F3 messages for memory allocation failure
03/28/16   tkl     Added support of DSS network down reason in GBA response
10/26/15   vdc     Commit and end ds profile transaction after APN update
10/26/15   vdc     Fixed APN availability check
10/13/15   tkl     Fixed auts len check for sync failure
09/30/15   vdc     Remove duplicate APN check
09/22/15   tkl     Update use of iface type DS_HTTP_IFACE_ANY
09/22/15   lm      fix KW errors
08/19/15   vdc     Update IMEI header NV struct
08/12/15   ar      Make Server header as optional in HTTP response
08/12/15   ar      Added WLAN support
07/20/15   tkl     Added HTTP Cookies support
07/03/15   tkl     Added support for custom fields
06/16/15   tkl     Fixed race condition during ds_http_open_session
06/10/15   tkl     Add support of IPv6 BSF address
06/04/15   tkl     Support next nonce in auth resp header
06/04/15   vdc     Do not queue boostrap cmd if DS session open has failed
05/20/15   tkl     Fix compile warnings
05/14/15   tkl     Update DS http interface for rsp mismatch and timeout fix
05/12/15   tkl     Return GBA_SERVER_ERROR for BSF failure
05/05/15   lxu     Fixed various issues
05/01/15   tkl     Fix parsing with XML without tag
04/30/15   vdc     Added support for APN for bootstrapping
04/15/15   tkl     Add TMPI support
04/03/15   lxu     HTTPs support over Ub
11/07/14   tkl     Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "ds_http_api.h"
#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include "dssocket.h"
#include "mre_xml.h"
#include <stringl/stringl.h>
#include "string.h"
#include "fs_public.h"

#include "gba_bootstrap.h"
#include "gba_http.h"
#include "gba_http_cookie.h"
#include "gba_nv.h"
#include "gba_platform.h"
#include "gba_util.h"
#include "gba_lib.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define GBA_HTTP_USER_AGENT_STR               "Bootstrapping Client Agent; 3gpp-gba-tmpi"
                                              /* TS 124 109 Annex A.3 for "Bootstrapping Client Agent" */
                                              /* 33 220 Annex M6.3 for "3gpp-gba-tmpi" */
#define GBA_HTTP_CUSTOM_USER_AGENT_STR        "Bootstrapping Client Agent; 3gpp-gba-uicc"
#define GBA_HTTP_AUTH_STR                     "Authorization"
#define GBA_HTTP_AUTH_INFO_STR                "Authentication-Info"
#define GBA_HTTP_SERVER_STR                   "Server"
#define GBA_HTTP_SERVER_TMPI_STR              "3gpp-gba-tmpi"
#define GBA_HTTP_SERVER_CUSTOM_TMPI_STR       "3gpp-gba-uicc"
#define GBA_HTTP_SERVER_BOOTSTRAP_SERVER_TAG  "Bootstrapping Server"
#define GBA_HTTP_BS_CONTENT_TAG               "BootstrappingInfo"
#define GBA_HTTP_DIGEST_TAG                   "Digest"
#define GBA_HTTP_QOP_AUTH_INT_TAG             "auth-int"
#define GBA_HTTP_QOP_AUTH_TAG                 "auth"
#define GBA_HTTP_ALGOR_TAG                    "AKAv1-MD5"
#define GBA_HTTP_AUTS_TAG                     "auts"
#define GBA_HTTP_BTID_CONTENT_TAG             "btid"
#define GBA_HTTP_LIFETIME_CONTENT_TAG         "lifetime"
#define GBA_HTTP_URL_TAG                      "http://"
#define GBA_HTTPS_URL_TAG                     "https://"
#define GBA_HTTP_FRWD_SLASH_STR               "/"
#define GBA_HTTP_EMPTY_STR                    ""
#define GBA_HTTP_EQUAL_STR                    "="
#define GBA_HTTP_DOUBLE_QUOTE_STR             "\""

#define GBA_HTTP_DOUBLE_QUOTES_LEN            2
#define GBA_HTTP_EQUAL_LEN                    1
#define GBA_HTTP_STATUS_200OK                 200
#define GBA_HTTP_STATUS_403FORBIDDEN          403
#define GBA_HTTP_DEFAULT_PROFILE              0
#define GBA_MAX_NUM_CUSTOM_TOKEN              10
#define GBA_MAX_QOP_RETRY                     3

static const char GBA_HTTP_COLON_CHAR         = ':';
static const char GBA_HTTP_SEMI_COLON_CHAR    = ';';
static const char GBA_HTTP_DOUBLE_QUOTE_CHAR  = '\"';
static const char GBA_HTTP_SPACE_CHAR         = ' ';
static const char GBA_HTTP_COMMA_CHAR         = ',';
static const char GBA_HTTP_EQUAL_CHAR         = '=';
static const char GBA_HTTP_NULL_CHAR          = '\0';

typedef enum
{
  GBA_HTTP_USER_AGENT_HEADER,
  GBA_HTTP_AUTHORIZATION_HEADER,
  GAB_HTTP_MAX_HEADER    = 0xFF
} gba_http_header_enum_type;

typedef enum
{
  GBA_HTTP_DIGEST_TOKEN,
  GBA_HTTP_USERNAME_TOKEN,
  GBA_HTTP_REALM_TOKEN,
  GBA_HTTP_DOMAIN_TOKEN,
  GBA_HTTP_URI_TOKEN,
  GBA_HTTP_NONCE_TOKEN,
  GBA_HTTP_RESPONSE_TOKEN,
  GBA_HTTP_NC_TOKEN,
  GBA_HTTP_CNONCE_TOKEN,
  GBA_HTTP_ALGOR_TOKEN,
  GBA_HTTP_OPAQUE_TOKEN,
  GBA_HTTP_QOP_TOKEN,
  GBA_HTTP_RSPAUTH_TOKEN,
  GBA_HTTP_NEXTNONCE_TOKEN,
  GBA_HTTP_STALE_TOKEN,
  GAB_HTTP_MAX_TOKEN          = 0xFF
} gba_http_token_enum_type;

typedef enum
{
  GBA_HTTP_PENDING_CMD_NONE,
  GBA_HTTP_PENDING_GET_REQUEST,
  GBA_HTTP_PENDING_401_UNAUTH,
  GBA_HTTP_PENDING_RESOLVE_AUTH,
  GBA_HTTP_PENDING_200_OK,
  GBA_HTTP_MAX_PENDING_CMD    = 0xFF
} gba_http_pending_cmd_enum_type;

typedef enum {
  GBA_HTTP_VALIDATE_SUCCESS,
  GBA_HTTP_VALIDATE_FAIL,
  GBA_HTTP_VALIDATE_RETRY,
} gba_http_result_enum_type;

typedef struct
{
  gba_http_token_enum_type   token_id;
  char                     * name_ptr;
}gba_http_token_index_type;

const static gba_http_token_index_type  gba_http_token_index_table[] =
{
  {GBA_HTTP_DIGEST_TOKEN,      "Digest"},
  {GBA_HTTP_USERNAME_TOKEN,    "username"},
  {GBA_HTTP_REALM_TOKEN,       "realm"},
  {GBA_HTTP_DOMAIN_TOKEN,      "domain"},
  {GBA_HTTP_URI_TOKEN,         "uri"},
  {GBA_HTTP_NONCE_TOKEN,       "nonce"},
  {GBA_HTTP_RESPONSE_TOKEN,    "response"},
  {GBA_HTTP_NC_TOKEN,          "nc"},
  {GBA_HTTP_CNONCE_TOKEN,      "cnonce"},
  {GBA_HTTP_ALGOR_TOKEN,       "algorithm"},
  {GBA_HTTP_OPAQUE_TOKEN,      "opaque"},
  {GBA_HTTP_QOP_TOKEN,         "qop"},
  {GBA_HTTP_NEXTNONCE_TOKEN,   "nextnonce"},
  {GBA_HTTP_STALE_TOKEN,       "stale"},
  {GBA_HTTP_RSPAUTH_TOKEN,     "rspauth"}
};

typedef enum {
  GBA_HTTP_FALSE,
  GBA_HTTP_TRUE,
  GBA_HTTP_OPTIONAL
} gba_http_state_enum_type;

typedef struct
{
  gba_http_token_enum_type          token_id;
  boolean                           is_optional;
  gba_http_state_enum_type          has_quotes;
  char                            * value_ptr;
} gba_http_token_value_type;

typedef struct
{
  gba_http_header_enum_type         header_id;
  char                            * str_data_ptr;   /* string value of http header,
                                                       if header id = GBA_HTTP_AUTHORIZATION_HEADER,
                                                       then this is NULL. If it's for cutom header
                                                       then this is the name of custom header */
  gba_http_token_value_type       * token_table_ptr;/* token table for custom header */
  uint8                             table_size;     /* token table size for custom header */
} gba_http_req_header_type;

typedef struct
{
  uint32                            request_id;
  uint32                            reference_id;
  gba_http_header_value_type        impi;
  gba_session_type                  session_type;
  gba_http_header_value_type        username;
  gba_http_header_value_type        realm;
  gba_http_header_value_type        bsf;
  gba_apn_type                      apn_info;
  gba_bootstrap_callback_type       callback;
  gba_http_pending_cmd_enum_type    pending_cmd;
  uint8                             qop_retry_count;
} gba_http_ref_data_type;

typedef struct
{
  uint32                            session_id;
  sint15                            session_status;
} gba_http_session_id_type;

static gba_http_ref_data_type     * gba_http_global_data_ptr;

static q_type                       gba_http_cmd_q;

static gba_http_session_id_type     gba_http_session_id = {DS_HTTP_ERROR, DS_HTTP_ERROR_HTTP_STATUS};

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION GBA_HTTP_SET_HEADER_VALUE

DESCRIPTION
  This function construct set the input available_buf_ptr with input str_ptr value.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_http_set_header_value (
  char                      * available_buf_ptr,
  uint16                      available_buf_size,
  const char                * str_ptr,
  uint16                      str_len
)
{
  if(str_ptr == NULL || available_buf_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  if(str_len > available_buf_size)
  {
    return GBA_GENERIC_ERROR;
  }

  strlcpy(available_buf_ptr, str_ptr, available_buf_size + 1);

  return GBA_SUCCESS;
} /* gba_http_set_header_value */


/*===========================================================================
FUNCTION GBA_HTTP_CONSTRUCT_TOKEN

DESCRIPTION
  This function construct a string in a format where str1="str2" and put
  it in data_ptr string buffer. input data_buf should be have buffer
  size >= available_buf_size. output data_len returns the lenght of
  output string. all input strings are expected to be null terminated.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_http_construct_token (
  const char                * str1_ptr,
  const char                * str2_ptr,
  boolean                     add_quotes,
  uint16                    * available_buf_size_ptr,
  char                      * data_ptr,
  uint16                      data_len
)
{
  if(str1_ptr == NULL || str2_ptr == NULL ||
     data_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  if(data_len > *available_buf_size_ptr)
  {
    return GBA_GENERIC_ERROR;
  }

  memset(data_ptr, 0x00, *available_buf_size_ptr + 1);
  strlcpy(data_ptr, str1_ptr, *available_buf_size_ptr + 1);
  strlcat(data_ptr, GBA_HTTP_EQUAL_STR, *available_buf_size_ptr + 1);

  if(add_quotes)
  {
    strlcat(data_ptr, GBA_HTTP_DOUBLE_QUOTE_STR, *available_buf_size_ptr + 1);
  }

  strlcat(data_ptr, str2_ptr, *available_buf_size_ptr + 1);

  if(add_quotes)
  {
    strlcat(data_ptr, GBA_HTTP_DOUBLE_QUOTE_STR, *available_buf_size_ptr + 1);
  }

  *available_buf_size_ptr = *available_buf_size_ptr - data_len;

  return GBA_SUCCESS;
} /* gba_http_construct_token */


/*===========================================================================
   FUNCTION:      GBA_HTTP_FREE_TASK_CMD_PTR

   DESCRIPTION:
     This function performs deep free of task command pointer

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void gba_http_free_task_cmd_ptr (
  gba_http_cmd_type    * task_cmd_ptr
)
{
  if (task_cmd_ptr == NULL)
  {
    GBA_MSG_ERR_0("Attempt to use NULL POINTER");
    return;
  }

  switch(task_cmd_ptr->cmd_type)
  {
     case GBA_HTTP_RSP:
       GBA_FREE(task_cmd_ptr->data.rsp_data.content_ptr);
       GBA_FREE(task_cmd_ptr->data.rsp_data.header_info_ptr);
       GBA_FREE(task_cmd_ptr->data.rsp_data.cookie_ptr);
       break;
     case GBA_HTTP_BLOCK_RSP:
       GBA_FREE(task_cmd_ptr->data.block_rsp_data.auth_data.realm_ptr);
       GBA_FREE(task_cmd_ptr->data.block_rsp_data.auth_data.nonce_ptr);
       GBA_FREE(task_cmd_ptr->data.block_rsp_data.auth_data.auth_response_header_ptr);
       GBA_FREE(task_cmd_ptr->data.block_rsp_data.auth_data.cookie_ptr);
       break;
     default:
       break;
  }

  GBA_FREE(task_cmd_ptr);

} /* gba_http_free_task_cmd_ptr */


/*===========================================================================
   FUNCTION:      GBA_HTTP_QUEUE_RSP_CMD

   DESCRIPTION:
     This function allows to queue HTTP responses to GBA task to be
     processed.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static gba_result_enum_type  gba_http_queue_rsp_cmd (
  gba_http_cmd_type    * task_cmd_ptr
)
{
  if (task_cmd_ptr == NULL)
  {
    GBA_MSG_ERR_0("Attempt to use NULL POINTER");
    return GBA_INCORRECT_PARAMS;
  }

  GBA_ENTER_TASK_STATE_CRIT_SECT;

  /* Do not queue command if GBA task has been stopped */
  if(gba_task_stopped)
  {
    GBA_MSG_ERR_0("GBA task stopped, cannot queue command");
    GBA_LEAVE_TASK_STATE_CRIT_SECT;
    return GBA_GENERIC_ERROR;
  }

  (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

  /* Put the message on the queue */
  q_put(&gba_http_cmd_q, &task_cmd_ptr->link);

  /* Set the command queue signal */
  (void) rex_set_sigs(UIM_GBA_TCB, GBA_HTTP_CMD_SIG);

  GBA_LEAVE_TASK_STATE_CRIT_SECT;
  return GBA_SUCCESS;
} /* gba_http_queue_rsp_cmd */


/*===========================================================================
FUNCTION GBA_HTTP_COPY_DS_COOKIE_DATA_LIST

DESCRIPTION
  This function copy DS cookie data list to GBA HTTP cookie data list

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_http_cookie_data_type *

SIDE EFFECTS
  None
===========================================================================*/
static gba_http_cookie_data_type * gba_http_copy_ds_cookie_data_list (
  const ds_http_cookie_info_s_type  * ds_cookie_list_ptr,
  uint16                              ds_num_cookies,
  uint16                            * gba_http_num_cookies_ptr
)
{
  gba_http_cookie_data_type  * gba_http_cookie_list_ptr = NULL;
  uint16                       i                        = 0;

  if(ds_cookie_list_ptr != NULL && ds_num_cookies > 0 &&
     gba_http_num_cookies_ptr != NULL)
  {
    *gba_http_num_cookies_ptr = 0;
    gba_http_cookie_list_ptr = GBA_MALLOC(ds_num_cookies * sizeof(gba_http_cookie_data_type));

    /* ok to continue processing callback even if failed to copy cookies */
    if(gba_http_cookie_list_ptr != NULL)
    {
      *gba_http_num_cookies_ptr = ds_num_cookies;

      for (i = 0;
           i < *gba_http_num_cookies_ptr;
           i++)
      {
        gba_http_cookie_list_ptr[i].name_value = ds_cookie_list_ptr[i].name_value;
        gba_http_cookie_list_ptr[i].expiration_set = ds_cookie_list_ptr[i].expiration_set;

        if (gba_http_cookie_list_ptr[i].expiration_set)
        {
          gba_http_cookie_list_ptr[i].expiration = ds_cookie_list_ptr[i].expiration;
        }

        /* http_only, security, domain, path and unrecognized attributes
           are not copied as they are not applicable and to reduce cookie data size,
           will be added in future if there's a need */
      }
    }
  }

  return gba_http_cookie_list_ptr;
} /* gba_http_copy_ds_cookie_data_list */


/*===========================================================================
FUNCTION: GBA_HTTP_GET_PROFILE_NUM

DESCRIPTION :
  This function iterates through the list of profiles and checks if the input
  APN matches with any of the APNs of the profile. If the APN matches
  then pass the corresponding profile id to open the HTTP session.
  Otherwise, create a new profile and pass the new profile id

PARAMETERS:
  profile_num_ptr : Pointer to profile id

DEPENDENCIES:
  None

RETURN VALUE:
  gba_result_enum_type
    GBA_SUCCESS
    GBA_GENERIC_ERROR

SIDE EFFECTS:
  None
===========================================================================*/
static gba_result_enum_type gba_http_get_profile_num (
  ds_profile_num_type  * profile_num_ptr
)
{
  ds_profile_list_info_type  profile_list_info                                   = {0, NULL};
  ds_profile_status_etype    ds_status                                           = DS_PROFILE_REG_RESULT_SUCCESS;
  ds_profile_hndl_type       profile_hndl                                        = NULL;
  ds_profile_info_type       profile_info                                        = {NULL, 0};
  ds_profile_info_type       profile_list_name                                   = {NULL, 0};
  uint8                      apn_length                                          = 0;
  char                       profile_name[DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN]  = {'\0'};
  char                       profile_apn[DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1] = {'\0'};
  ds_profile_itr_type        profile_itr                                         = NULL;
  ds_profile_list_type       profile_list;
  ds_profile_config_type     profile_cfg;

  if(gba_http_global_data_ptr == NULL || profile_num_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  apn_length = (uint8)strnlen(gba_http_global_data_ptr->apn_info.apn, GBA_MAX_APN_LEN);
  if(apn_length == 0)
  {
    UIM_MSG_MED_0("gba_http_global_data_ptr->apn_info.apn with 0 length");
    return GBA_GENERIC_ERROR;
  }

  memset(&profile_list, 0x00, sizeof(ds_profile_list_type));
  memset(&profile_cfg, 0x00, sizeof(ds_profile_config_type));

  profile_list.dfn = DS_PROFILE_LIST_ALL_PROFILES;

  /* Get an iterator first */
  ds_status = ds_profile_get_list_itr(DS_PROFILE_TECH_3GPP,
                                      &profile_list,
                                      &profile_itr);

  if(ds_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_1("get profile list iterator failed: 0x%x", ds_status);
    return GBA_GENERIC_ERROR;
  }

  profile_list_info.name = &(profile_list_name);
  profile_list_info.name->buf = profile_name;
  profile_info.buf = profile_apn;

  do
  {
    memset((void *)profile_apn, 0x00, sizeof(profile_apn));

    profile_list_info.name->len = sizeof(profile_name);
    profile_info.len = sizeof(profile_apn);

    /* Get the value and iterate */
    ds_status = ds_profile_get_info_by_itr(profile_itr,
                                           &profile_list_info);
    if(ds_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_1("Profile info iterator failed: 0x%x", ds_status);
      break;
    }

    /* Get handle on the profile */
    ds_status = ds_profile_begin_transaction(DS_PROFILE_TRN_READ,
                                             DS_PROFILE_TECH_3GPP,
                                             profile_list_info.num,
                                             &profile_hndl);
    if(ds_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_1("Get profile handle failed: 0x%x", ds_status);
      break;
    }

    /* Get the profile APN parameter */
    ds_status = ds_profile_get_param(profile_hndl,
                                     DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
                                     &profile_info);
    if(ds_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_1("Get APN from profile info failed: 0x%x", ds_status);

      /* Release Handle */
      (void)ds_profile_end_transaction(profile_hndl,
                                       DS_PROFILE_ACTION_CANCEL);
      break;
    }

    /* Release handle */
    if(ds_profile_end_transaction(profile_hndl,
                                  DS_PROFILE_ACTION_CANCEL) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("Failed to commit profile");
      break;
    }

    /*Check if the APN matches*/
    if(strncasecmp((char *)profile_info.buf,
                   gba_http_global_data_ptr->apn_info.apn,
                   profile_info.len) == 0)
    {
      UIM_MSG_HIGH_1("Found matching profile number: 0x%x", profile_list_info.num);
      *profile_num_ptr = profile_list_info.num;

      (void)ds_profile_itr_destroy(profile_itr);
      return GBA_SUCCESS;
    }

    /* Iterate on the iterator */
    ds_status = ds_profile_itr_next(profile_itr);
    if(ds_status != DS_PROFILE_REG_RESULT_SUCCESS &&
       ds_status != DS_PROFILE_REG_RESULT_LIST_END)
    {
      UIM_MSG_ERR_1("Failed to goto next profile using iterator: 0x%x", ds_status);
      break;
    }
  }while(ds_status != DS_PROFILE_REG_RESULT_LIST_END);

  /* Release iterator */
  (void)ds_profile_itr_destroy(profile_itr);

  UIM_MSG_HIGH_1("Creating new profile as no profile has matched the APN: %s",
                 gba_http_global_data_ptr->apn_info.apn);

  /* There are 3 configs possible with the mask:
     1) profile id: Give the profile id in input
     2) subs id: Tie the profile to a subscription
     3) persistence: Whether we would require to have this profile across reboots.

     As the aim here is to get a profile id & not tie to a subscription, do not
     provide profile id & subs id config. Also, we do not want to have this new
     profile across reboots. So, provide the config mask as
     DS_PROFILE_CONFIG_MASK_PERSISTENCE with is_persistence set to FALSE. */
  profile_cfg.config_mask = DS_PROFILE_CONFIG_MASK_PERSISTENCE;
  profile_cfg.is_persistent = FALSE;

  /* Create a profile as the existing profiles does not have APN match */
  if(ds_profile_create_ex(DS_PROFILE_TECH_3GPP,
                          &profile_cfg,
                          profile_num_ptr)     != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to create profile");
    return GBA_GENERIC_ERROR;
  }

  if (ds_profile_begin_transaction(DS_PROFILE_TRN_RW,
                                   DS_PROFILE_TECH_3GPP,
                                   *profile_num_ptr,
                                   &profile_hndl)        != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to modify profile");
    (void)ds_profile_delete(DS_PROFILE_TECH_3GPP, *profile_num_ptr);
    return GBA_GENERIC_ERROR;
  }

  /* Set APN */
    profile_info.len = apn_length;
    profile_info.buf = &gba_http_global_data_ptr->apn_info.apn;
    if (ds_profile_set_param(
          profile_hndl,
          DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
          &profile_info)                                 != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("Failed to set APN, delete the created profile");
    (void)ds_profile_end_transaction(profile_hndl, DS_PROFILE_ACTION_CANCEL);
     (void)ds_profile_delete(DS_PROFILE_TECH_3GPP, *profile_num_ptr);
      return GBA_GENERIC_ERROR;
    }

  /* Release handle */
  if(ds_profile_end_transaction(profile_hndl,
                                DS_PROFILE_ACTION_COMMIT) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to end ds profile transaction");
	(void)ds_profile_delete(DS_PROFILE_TECH_3GPP, *profile_num_ptr);
    return GBA_GENERIC_ERROR;
  }

  return GBA_SUCCESS;
} /* gba_http_get_profile_num */


/*===========================================================================
FUNCTION GBA_HTTP_CB_FCN

DESCRIPTION
  This is the http callback function for GBA request,
  the http respose is post back to GBA HTTP and process.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_http_cb_fcn (
  uint32                              session_id,
  uint32                              request_id,
  sint15                              error,
  const ds_http_response_info_s_type* response_info_ptr
)
{
  /* callback for http session open status update, error http response,
     200OK data response for BTID and lifetime */
  gba_http_cmd_type         *gba_http_cmd_ptr = NULL;
  uint16                     i                = 0;

  if (gba_http_global_data_ptr == NULL )
  {
    return;
  }

  /* post back response data to HTTP queue */
  gba_http_cmd_ptr = GBA_MALLOC(sizeof(gba_http_cmd_type));
  if (gba_http_cmd_ptr == NULL)
  {
    return;
  }

  memset(gba_http_cmd_ptr, 0x00, sizeof(gba_http_cmd_type));
  gba_http_cmd_ptr->cmd_type = GBA_HTTP_RSP;
  gba_http_cmd_ptr->data.rsp_data.session_id = session_id;
  gba_http_cmd_ptr->data.rsp_data.request_id = request_id;
  gba_http_cmd_ptr->data.rsp_data.error = error;

  if (NULL == response_info_ptr)
  {
    (void)gba_http_queue_rsp_cmd(gba_http_cmd_ptr);
    return;
  }

  gba_http_cmd_ptr->data.rsp_data.http_status = response_info_ptr->http_status;
  gba_http_cmd_ptr->data.rsp_data.content_size = response_info_ptr->content_size;
  gba_http_cmd_ptr->data.rsp_data.num_headers = response_info_ptr->num_headers;

  if(response_info_ptr->content_size > 0)
  {
    gba_http_cmd_ptr->data.rsp_data.content_ptr =
                GBA_MALLOC(sizeof(uint8) * (response_info_ptr->content_size + 1));

    if (gba_http_cmd_ptr->data.rsp_data.content_ptr == NULL)
    {
      gba_http_free_task_cmd_ptr(gba_http_cmd_ptr);
      return;
    }

    memscpy(gba_http_cmd_ptr->data.rsp_data.content_ptr,
            response_info_ptr->content_size,
            (uint8 *)response_info_ptr->content,
            response_info_ptr->content_size);
  }

  if(response_info_ptr->num_headers > 0)
  {
    gba_http_cmd_ptr->data.rsp_data.header_info_ptr =
       GBA_MALLOC(sizeof(gba_http_response_header_type) * response_info_ptr->num_headers);

    if (gba_http_cmd_ptr->data.rsp_data.header_info_ptr == NULL)
    {
      gba_http_free_task_cmd_ptr(gba_http_cmd_ptr);
      return;
    }

    for(i = 0; i < response_info_ptr->num_headers; i++)
    {
      (void)strlcpy(gba_http_cmd_ptr->data.rsp_data.header_info_ptr[i].name,
                    response_info_ptr->header_info[i].name,
                    sizeof(gba_http_cmd_ptr->data.rsp_data.header_info_ptr[i].name));
      (void)strlcpy(gba_http_cmd_ptr->data.rsp_data.header_info_ptr[i].value,
                    response_info_ptr->header_info[i].value,
                    sizeof(gba_http_cmd_ptr->data.rsp_data.header_info_ptr[i].value));
    }
  }

  /* copy ds cookie data */
  gba_http_cmd_ptr->data.rsp_data.cookie_ptr =
      gba_http_copy_ds_cookie_data_list(response_info_ptr->cookie_info,
                                        response_info_ptr->num_cookies,
                                        &gba_http_cmd_ptr->data.rsp_data.num_cookies);

  (void)gba_http_queue_rsp_cmd(gba_http_cmd_ptr);
} /* gba_http_cb_fcn */


/*===========================================================================
FUNCTION GBA_HTTP_BLOCK_CB_FCN

DESCRIPTION
  Callback function to notify block events that need input from the client.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static void gba_http_block_cb_fcn (
  uint32                         session_id,
  uint32                         request_id,
  ds_http_block_event_type       event_type,
  ds_http_block_event_info_type* event_info_ptr
)
{
  gba_http_cmd_type              *gba_http_cmd_ptr = NULL;

  if (gba_http_global_data_ptr == NULL || event_info_ptr == NULL)
  {
    return;
  }

  GBA_MSG_MED_3("gba_http_block_cb_fcn: session id : 0x%x; request id 0x%x; event 0x%x",
                session_id, request_id, event_type);

  /* post back response data to HTTP queue */
  gba_http_cmd_ptr = GBA_MALLOC(sizeof(gba_http_cmd_type));
  if (gba_http_cmd_ptr == NULL)
  {
    return;
  }

  memset(gba_http_cmd_ptr, 0x00, sizeof(gba_http_cmd_type));
  gba_http_cmd_ptr->cmd_type = GBA_HTTP_BLOCK_RSP;
  gba_http_cmd_ptr->data.block_rsp_data.session_id = session_id;
  gba_http_cmd_ptr->data.block_rsp_data.request_id = request_id;

  switch (event_type)
  {
    case DS_HTTP_BLOCK_SSL_CERTIFICATE_ALERT:
      gba_http_cmd_ptr->data.block_rsp_data.event_type = GBA_HTTP_BLOCK_SSL_CERTIFICATE_ALERT;
      break;

    case DS_HTTP_BLOCK_AUTHENTICATION:
      gba_http_cmd_ptr->data.block_rsp_data.event_type = GBA_HTTP_BLOCK_AUTHENTICATION;

      gba_http_cmd_ptr->data.block_rsp_data.auth_data.realm_ptr =
          GBA_MALLOC(sizeof(gba_http_header_value_type));
      gba_http_cmd_ptr->data.block_rsp_data.auth_data.nonce_ptr =
          GBA_MALLOC(sizeof(gba_http_header_value_type));
      gba_http_cmd_ptr->data.block_rsp_data.auth_data.auth_response_header_ptr =
          GBA_MALLOC(sizeof(gba_http_header_value_type));

      if(gba_http_cmd_ptr->data.block_rsp_data.auth_data.realm_ptr == NULL ||
         gba_http_cmd_ptr->data.block_rsp_data.auth_data.nonce_ptr == NULL ||
         gba_http_cmd_ptr->data.block_rsp_data.auth_data.auth_response_header_ptr == NULL)
      {
        gba_http_free_task_cmd_ptr(gba_http_cmd_ptr);
        return;
      }

      if(event_info_ptr->auth_event_info.realm != NULL)
      {
        (void)strlcpy(gba_http_cmd_ptr->data.block_rsp_data.auth_data.realm_ptr,
                      (const char*)event_info_ptr->auth_event_info.realm,
                      sizeof(gba_http_header_value_type));
      }

      if(event_info_ptr->auth_event_info.nonce != NULL)
      {
        (void)strlcpy(gba_http_cmd_ptr->data.block_rsp_data.auth_data.nonce_ptr,
                      (const char*)event_info_ptr->auth_event_info.nonce,
                      sizeof(gba_http_header_value_type));
      }

      if(event_info_ptr->auth_event_info.auth_response_header != NULL)
      {
        (void)strlcpy(gba_http_cmd_ptr->data.block_rsp_data.auth_data.auth_response_header_ptr,
                      (const char*)event_info_ptr->auth_event_info.auth_response_header,
                      sizeof(gba_http_header_value_type));
      }

      /* copy ds cookie data */
      gba_http_cmd_ptr->data.block_rsp_data.auth_data.cookie_ptr =
         gba_http_copy_ds_cookie_data_list(event_info_ptr->auth_event_info.cookie_info,
                                           event_info_ptr->auth_event_info.num_cookies,
                                           &gba_http_cmd_ptr->data.block_rsp_data.auth_data.num_cookies);
      break;
  }

  (void)gba_http_queue_rsp_cmd(gba_http_cmd_ptr);
} /* gba_http_block_cb_fcn */


/*===========================================================================
FUNCTION GBA_HTTP_OPEN_SESSION

DESCRIPTION
  This function opens an HTTP session

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_http_open_session(
  void
)
{
  if(DS_HTTP_ERROR != gba_http_session_id.session_id &&
     DS_HTTP_READY != gba_http_session_id.session_status)
  {
    /* reach here if open session is called for 2nd time from ds callback.
       quit waiting for the call back from DS open session */
    GBA_MSG_HIGH_2("pending session open : session id 0x%x; http_status 0x%x",
                   gba_http_session_id.session_id,
                   gba_http_session_id.session_status);
    return GBA_GENERIC_ERROR;
  }

  if(DS_HTTP_ERROR == gba_http_session_id.session_id)
  {
    ds_http_iface_info_s_type iface_info;
    memset(&iface_info, 0, sizeof(ds_http_iface_info_s_type));
    iface_info.iface_type = DS_HTTP_IFACE_MAX;
    iface_info.profile_id = GBA_HTTP_DEFAULT_PROFILE;

    /* 1) If APN is available get the profile number from request.
       2) Otherwise get the APN from NV and get the profile number.
       3) Otherwise use the default profile */
    if (!gba_http_global_data_ptr->apn_info.apn_available)
    {
      (void)gba_nv_get_apn(&gba_http_global_data_ptr->apn_info);
    }

    if(gba_http_global_data_ptr->apn_info.apn_available)
    {
      if(GBA_SUCCESS != gba_http_get_profile_num(&iface_info.profile_id))
      {
        iface_info.profile_id = GBA_HTTP_DEFAULT_PROFILE;
        gba_http_global_data_ptr->apn_info.apn_available = FALSE;
      }
    }

    /* NW service check on 3GPP/iWLAN/LTE for available network.
       DS_HTTP_IFACE_ANY covers nw service check on 3GPP/iWLAN/LTE */
    if (ds_http_is_nw_srv_avail(DS_HTTP_IFACE_ANY))
    {
      iface_info.iface_type = DS_HTTP_IFACE_ANY;
    }
    /* if client didn't specifiy an APN for connection, then
       do NW service check on WLAN as well */
    else if (!gba_http_global_data_ptr->apn_info.apn_available &&
             ds_http_is_nw_srv_avail(DS_HTTP_IFACE_WLAN_LB))
    {
        iface_info.iface_type = DS_HTTP_IFACE_WLAN_LB;
    }

    if (iface_info.iface_type != DS_HTTP_IFACE_MAX)
    {
      /* DS HTTP will ignore profile id for WLAN_LB connection.
	     When sending GBA_HTTP_DEFAULT_PROFILE (0) for DS_HTTP_IFACE_ANY,
		 DS HTTP will look for the default profile for the iface type that
		 is used for connection */
      gba_http_session_id.session_id = ds_http_open_session_ex(gba_http_cb_fcn,
                                                               gba_http_block_cb_fcn,
                                                               &iface_info);
    }
    GBA_MSG_MED_3("session id from ds_http open session 0x%x, iface_type 0x%x, profile id 0x%x",
                  gba_http_session_id.session_id,
                  iface_info.iface_type,
                  iface_info.profile_id);

    if(DS_HTTP_ERROR == gba_http_session_id.session_id)
    {
      return GBA_NETWORK_NOT_READY;
    }
  }
  return GBA_SUCCESS;
} /* gba_http_open_session */


/*===========================================================================
FUNCTION GBA_HTTP_PROCESS_CUSTOM_HEADER_VALUE

DESCRIPTION
  This function will put up the string buffer for custome header value
  from the input custom token table

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_http_process_custom_header_value (
  uint8                             table_size,
  gba_http_token_value_type       * token_table_ptr,
  char                            * data_ptr,
  uint16                            token_value_size
)
{
  uint16                       avail_buf_size      = GBA_HTTP_HEADER_VALUE_MAX_LEN;
  uint16                       offset              = 0;
  uint16                       str_len             = 0;
  uint8                        i                   = 0;
  uint8                        j                   = 0;
  gba_result_enum_type         gba_status          = 0;
  gba_http_token_value_type  * token_ptr           = NULL;
  char                       * str_buf_ptr         = NULL;
  char                       * token_value_buf_ptr = NULL;

  if(table_size == 0 || token_table_ptr == NULL || data_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  str_buf_ptr = (char *) GBA_MALLOC(sizeof(gba_http_header_value_type));
  if (str_buf_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  token_value_buf_ptr = (char *) GBA_MALLOC(sizeof(gba_http_header_value_type));
  if (token_value_buf_ptr == NULL)
  {
    GBA_FREE(str_buf_ptr);
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* set custom header value */
  offset = 0;

  /* loop thru the custom header value token table */
  for(i = 0; (i < table_size) && (offset <= GBA_HTTP_HEADER_VALUE_MAX_LEN); i++)
  {
    str_len = 0;
    memset(str_buf_ptr, 0x00, sizeof(gba_http_header_value_type));
    token_ptr = &token_table_ptr[i];

    for(j = 0;
        j < (sizeof(gba_http_token_index_table)/sizeof(gba_http_token_index_type));
        j++)
    {
      /* look for the corresponding token name string */
      if(token_ptr->token_id == gba_http_token_index_table[j].token_id)
      {
        strlcat(str_buf_ptr,
                gba_http_token_index_table[j].name_ptr,
                sizeof(gba_http_header_value_type));
        break;
      }
    }

    if((j >= (sizeof(gba_http_token_index_table)/sizeof(gba_http_token_index_type))) ||
       str_buf_ptr[0] == '\0')
    {
      /* unable to find the header token name */
      gba_status = GBA_INCORRECT_PARAMS;
      break;
    }

    /* if token has value, set token name = token value */
    /* TS 24.109 Annex A */
    if(token_ptr->value_ptr== NULL)
    {
      /* for scheme token, there is no value and add a space before next token */
      str_len = (uint16)strnlen(str_buf_ptr, sizeof(gba_http_header_value_type));
      gba_status = gba_http_set_header_value(&data_ptr[offset],
                                             avail_buf_size,
                                             str_buf_ptr,
                                             str_len);
      if (GBA_SUCCESS == gba_status)
      {
        avail_buf_size = avail_buf_size - str_len;
        offset = offset + str_len;
        data_ptr[offset++] = GBA_HTTP_SPACE_CHAR;
      }
    }
    else
    {
      uint16 token_value_len = (uint16)strnlen(token_ptr->value_ptr, token_value_size);

      if(token_value_len > GBA_HTTP_HEADER_VALUE_MAX_LEN)
      {
        gba_status = GBA_GENERIC_ERROR;
        break;
      }

      memset(token_value_buf_ptr, 0x00, sizeof(gba_http_header_value_type));

      if(token_ptr->token_id == GBA_HTTP_REALM_TOKEN )
      {
        /* get realm from global ptr */
        strlcpy(token_value_buf_ptr, gba_http_global_data_ptr->realm, sizeof(gba_http_global_data_ptr->realm));
      }
      else
      {
        strlcpy(token_value_buf_ptr, token_ptr->value_ptr, sizeof(gba_http_header_value_type));
      }

      str_len = (uint16)(strnlen(str_buf_ptr, sizeof(gba_http_header_value_type)) +
                strnlen(token_value_buf_ptr, sizeof(gba_http_header_value_type))+
                GBA_HTTP_EQUAL_LEN);
      if((token_ptr->has_quotes == (GBA_HTTP_TRUE)) ? TRUE : FALSE)
      {
        str_len = str_len + GBA_HTTP_DOUBLE_QUOTES_LEN;
      }
      gba_status = gba_http_construct_token(str_buf_ptr,
                                            token_value_buf_ptr,
                                            (token_ptr->has_quotes == (GBA_HTTP_TRUE)) ? TRUE : FALSE,
                                            &avail_buf_size,
                                            &data_ptr[offset],
                                            str_len);
      if (GBA_SUCCESS == gba_status)
      {
        offset = offset + str_len;
        /* do not add comma after last header token */
        if(i < (table_size - 1))
        {
          data_ptr[offset++] = GBA_HTTP_COMMA_CHAR;
        }
      }
    }
    if(gba_status != GBA_SUCCESS)
    {
      break;
    }
  }

  GBA_FREE(str_buf_ptr);
  GBA_FREE(token_value_buf_ptr);

  return gba_status;
} /* gba_http_process_custom_header_value */


/*===========================================================================
FUNCTION GBA_HTTP_VALIDATE_AUTH_RESPONSE_HEADER

DESCRIPTION
  This function validate the 2 authenticate response headers from BSF

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_http_result_enum_type gba_http_validate_auth_response_header(
  const char *                 auth_resp_ptr,
  const char *                 realm_ptr,
  gba_bootstrap_cmd_enum_type  resp_type,
  sint15                       http_error
)
{
  gba_http_result_enum_type        result          = GBA_HTTP_VALIDATE_SUCCESS;
  uint8                            i               = 0;
  uint8                            j               = 0;
  boolean                          done            = FALSE;
  gba_http_header_value_type       str_buf         = {0x00};
  char                           * str_ptr         = NULL;
  char                           * token_ptr       = NULL;
  char                           * remain_ptr      = NULL;
  gba_http_token_value_type        resp_token_list[GBA_MAX_NUM_CUSTOM_TOKEN];
  uint8                            num_token       = 0;
  gba_http_token_value_type      * token_table_ptr = NULL;
  uint8                            table_size      = 0;
  char                           * temp_ptr        = NULL;

  /* RFC 2617 S3.2.1 WWW-Authenticate Response Header */
  gba_http_token_value_type gba_http_unauthorized_cust_value_table[] =
  /* token enum,              is_optional, has_quotes,        value */
  {{GBA_HTTP_DIGEST_TOKEN,    FALSE,       GBA_HTTP_FALSE,    NULL},
   {GBA_HTTP_REALM_TOKEN,     FALSE,       GBA_HTTP_TRUE,     NULL},
   {GBA_HTTP_QOP_TOKEN,       FALSE,       GBA_HTTP_OPTIONAL, NULL},
   {GBA_HTTP_ALGOR_TOKEN,     FALSE,       GBA_HTTP_FALSE,    GBA_HTTP_ALGOR_TAG},
   {GBA_HTTP_NONCE_TOKEN,     FALSE,       GBA_HTTP_TRUE,     NULL},
   {GBA_HTTP_OPAQUE_TOKEN,    FALSE,       GBA_HTTP_TRUE,     NULL},
   {GBA_HTTP_DOMAIN_TOKEN,    TRUE,        GBA_HTTP_OPTIONAL, NULL},
   {GBA_HTTP_STALE_TOKEN,     TRUE,        GBA_HTTP_FALSE,    NULL}
  };

  /* RFC 2617 S3.2.3 Authenticate-Info Header */
  gba_http_token_value_type gba_http_resp_final_cust_value_table[] =
  /* token enum,              is_optional, has_quotes,        value */
  {{GBA_HTTP_QOP_TOKEN,       FALSE,       GBA_HTTP_OPTIONAL, NULL},
   {GBA_HTTP_NC_TOKEN,        FALSE,       GBA_HTTP_FALSE,    NULL},
   {GBA_HTTP_NEXTNONCE_TOKEN, TRUE,        GBA_HTTP_OPTIONAL, NULL},
   {GBA_HTTP_CNONCE_TOKEN,    FALSE,       GBA_HTTP_TRUE,     NULL},
   {GBA_HTTP_RSPAUTH_TOKEN,   FALSE,       GBA_HTTP_TRUE,     NULL}
  };

  if (gba_http_global_data_ptr == NULL)
  {
    return GBA_HTTP_VALIDATE_FAIL;
  }

  if(auth_resp_ptr == NULL ||
     (http_error != DS_HTTP_ERROR_RSPAUTH_MISMATCH &&
      http_error != DS_HTTP_STATUS_OK))
  {
    return GBA_HTTP_VALIDATE_FAIL;
  }

  switch(resp_type)
  {
    case GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD:
      token_table_ptr = gba_http_unauthorized_cust_value_table;
      table_size = sizeof(gba_http_unauthorized_cust_value_table) /
                     sizeof(gba_http_token_value_type);
      break;

    case GBA_HTTP_RESPONSE_FINAL_CMD:
      token_table_ptr = gba_http_resp_final_cust_value_table;
      table_size = sizeof(gba_http_resp_final_cust_value_table) /
                     sizeof(gba_http_token_value_type);
      break;

    default:
      GBA_MSG_ERR_0("invalid response type");
      return GBA_HTTP_VALIDATE_FAIL;
  }

  /* validate realm */
  if(realm_ptr != NULL)
  {
    if(strncasecmp(realm_ptr,
                   gba_http_global_data_ptr->realm,
                   sizeof(gba_http_global_data_ptr->realm)) != 0)
    {
      GBA_MSG_SPRINTF_2("realm doesn't match: reqest realm : %s; respone realm : %s",
                         gba_http_global_data_ptr->realm,
                         realm_ptr);
      return GBA_HTTP_VALIDATE_FAIL;
    }
  }

  /* give realm value to gba_http_unauthorized_cust_value_table */
  if (GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD == resp_type)
  {
    for (i = 0; i < table_size; i++)
    {
      if (GBA_HTTP_REALM_TOKEN == token_table_ptr[i].token_id)
      {
        token_table_ptr[i].value_ptr = gba_http_global_data_ptr->realm;
        break;
      }
    }
  }

  /* validate resp_data */
  strlcpy(str_buf, auth_resp_ptr, sizeof(gba_http_header_value_type));

  str_ptr = gba_util_trim_spaces(str_buf);
  if(str_ptr == NULL)
  {
    return GBA_HTTP_VALIDATE_FAIL;
  }

  /* check if there's 'Digest' token is in place, and only
     Digest token has no value */
  i = 0;
  if(strncasecmp(str_ptr, GBA_HTTP_DIGEST_TAG, strlen(GBA_HTTP_DIGEST_TAG)) == 0)
  {
    resp_token_list[0].token_id = GBA_HTTP_DIGEST_TOKEN;
    resp_token_list[0].value_ptr = NULL;
    resp_token_list[0].has_quotes = GBA_HTTP_FALSE;
    str_ptr = str_ptr + strlen(GBA_HTTP_DIGEST_TAG);
    str_ptr = gba_util_trim_spaces(str_ptr);
    i++;
  }

  /* parsing rest of the token pairs */
  while(str_ptr != NULL && !done && i < GBA_MAX_NUM_CUSTOM_TOKEN)
  {
    remain_ptr = strchr((const char*)str_ptr, GBA_HTTP_EQUAL_CHAR);
    /* if remain_ptr == NULL, ie. no delimiter is found
       also, expect token value after '=' */
    if(remain_ptr == NULL || remain_ptr[1] == '\0')
    {
      GBA_MSG_ERR_0("invalid format for resp header");
      result = GBA_HTTP_VALIDATE_FAIL;
      break;
    }
    temp_ptr = remain_ptr;
    remain_ptr++;

    temp_ptr[0] = GBA_HTTP_NULL_CHAR;
    token_ptr = str_ptr;
    token_ptr = gba_util_trim_spaces(token_ptr);

    if(token_ptr == NULL)
    {
      GBA_MSG_ERR_0("invalid format for resp header");
      result = GBA_HTTP_VALIDATE_FAIL;
      break;
    }

    /* lookup a token name */
    for(j = 0;
        j < (sizeof(gba_http_token_index_table)/sizeof(gba_http_token_index_type));
        j++)
    {
      /* look for the corresponding token name string */
      if(strncasecmp(token_ptr,
                     gba_http_token_index_table[j].name_ptr,
                     strlen(gba_http_token_index_table[j].name_ptr)) == 0)
      {
        resp_token_list[i].token_id = gba_http_token_index_table[j].token_id;
        break;
      }
    }
    if(j >= (sizeof(gba_http_token_index_table)/sizeof(gba_http_token_index_type)))
    {
      if(token_ptr != NULL)
      {
        GBA_MSG_SPRINTF_1("unknown token name %s", token_ptr);
      }
      result = GBA_HTTP_VALIDATE_FAIL;
      break;
    }

    /* point str_ptr to the remaining string */
    str_ptr = remain_ptr;

    /* remove any spaces after '=' */
    str_ptr = gba_util_trim_spaces(str_ptr);
    if(str_ptr == NULL)
    {
      GBA_MSG_ERR_0("token without value");
      result = GBA_HTTP_VALIDATE_FAIL;
      break;
    }

    /* for token value with quotes */
    if(str_ptr[0] == GBA_HTTP_DOUBLE_QUOTE_CHAR)
    {
      str_ptr++;
      if(str_ptr == NULL)
      {
        GBA_MSG_ERR_0("invalid format");
      result = GBA_HTTP_VALIDATE_FAIL;
        break;
      }
      /* do a strchr to check if double quote is in place, then take the token */
      remain_ptr = strchr((const char*)str_ptr, GBA_HTTP_DOUBLE_QUOTE_CHAR);
      /* if remain_ptr == NULL, ie. no delimiter is found */
      if(remain_ptr == NULL)
      {
        GBA_MSG_ERR_0("invalid format for resp header");
        result = GBA_HTTP_VALIDATE_FAIL;
        break;
      }
      temp_ptr = remain_ptr;
      remain_ptr++;

      temp_ptr[0] = GBA_HTTP_NULL_CHAR;
      token_ptr = str_ptr;

      resp_token_list[i].has_quotes = GBA_HTTP_TRUE;
      resp_token_list[i].value_ptr = GBA_MALLOC(sizeof(gba_http_header_value_type));
      if(resp_token_list[i].value_ptr == NULL)
      {
        result = GBA_HTTP_VALIDATE_FAIL;
        break;
      }

      strlcpy(resp_token_list[i].value_ptr, (const char*)token_ptr, sizeof(gba_http_header_value_type));

      /* remove any spaces after double quotes */
      /* do trimming instead to cover the invalid format
         e.g. xxx="token value"xyz, */
      /* point str_ptr to the remaining string, since there's no trimming,
         strlen(token) reflects the correct token len */
      str_ptr = remain_ptr;

      /* if reach end of str_buf, last token pair */
      if(str_ptr == NULL)
      {
        done = TRUE;
        break;
      }

      /* in case there are trailing spaces before '\0' */
      str_ptr = gba_util_trim_spaces(str_ptr);
      if(str_ptr == NULL)
      {
        done = TRUE;
        break;
      }
      else if(str_ptr[0] != GBA_HTTP_COMMA_CHAR)
      {
        /* having a token pair without ',' separator or
           end of string after ',' */
        GBA_MSG_ERR_0("invalid format");
        result = GBA_HTTP_VALIDATE_FAIL;
        break;
      }
      /* skip ',' */
      str_ptr++;
    }
    /* for token value without quotes */
    else
    {
      resp_token_list[i].has_quotes = GBA_HTTP_FALSE;
      /* use strchr to search for ',' first to ensure proper format */
      remain_ptr = strchr((const char*)str_ptr, GBA_HTTP_COMMA_CHAR);
      if(remain_ptr != NULL)
      {
        temp_ptr = remain_ptr;
        remain_ptr++;
      }

      /* since remain_ptr != NULL, ie ',' is found in the string.
         checks if there's next token after ','*/

      temp_ptr[0] = GBA_HTTP_NULL_CHAR;
      token_ptr = str_ptr;
      if (token_ptr == NULL)
      {
        GBA_MSG_ERR_0("invalid format");
        result = GBA_HTTP_VALIDATE_FAIL;
        break;
      }
      token_ptr = gba_util_trim_spaces(token_ptr);
      if(token_ptr == NULL)
      {
        /* token has all spaces */
        GBA_MSG_ERR_0("invalid format");
        result = GBA_HTTP_VALIDATE_FAIL;
        break;
      }

      resp_token_list[i].value_ptr = GBA_MALLOC(sizeof(gba_http_header_value_type));

      if(resp_token_list[i].value_ptr == NULL)
      {
        result = GBA_HTTP_VALIDATE_FAIL;
        break;
      }

      strlcpy(resp_token_list[i].value_ptr, (const char*)token_ptr, sizeof(gba_http_header_value_type));

      if(remain_ptr == NULL)
      {
        done = TRUE;
        break;
      }
      /* move str_ptr to char after token string*/
      str_ptr = remain_ptr;

      /* in case there are trailing spaces before '\0' */
      str_ptr = gba_util_trim_spaces(str_ptr);
      if(str_ptr == NULL)
      {
        done = TRUE;
        break;
      }
    }

    /* trim after comma */
    str_ptr = gba_util_trim_spaces(str_ptr);
    if(str_ptr == NULL)
    {
      GBA_MSG_ERR_0("invalid format");
      result = GBA_HTTP_VALIDATE_FAIL;
      break;
    }
    i++;
  }

  /* increment counter for the last token pair */
  i++;
  num_token = i;
  if(num_token >= GBA_MAX_NUM_CUSTOM_TOKEN)
  {
    GBA_MSG_ERR_0("Too many tokens");
    result = GBA_HTTP_VALIDATE_FAIL;
  }

  /* free the resp_token list in case there's any error in
     parsing */
  if(result != GBA_HTTP_VALIDATE_SUCCESS)
  {
    /* free resp_token_list[].value */
    for(i = 0; i < num_token ; i++)
    {
      if(resp_token_list[i].value_ptr != NULL)
      {
        GBA_FREE(resp_token_list[i].value_ptr);
      }
    }
    return result;
  }

  /* validate token value */
  for(i = 0; i < table_size; i++)
  {
    for(j = 0; j < num_token; j++)
    {
      if(token_table_ptr[i].token_id == resp_token_list[j].token_id)
      {
        /* found the matching token id */
        break;
      }
    }
    if(j >= num_token)
    {
      GBA_MSG_ERR_2("missing token id, 0x%x : is_optional, 0x%x",
                    token_table_ptr[i].token_id,
                    token_table_ptr[i].is_optional);
      /* skip optional token in auth response header */
      if (token_table_ptr[i].is_optional)
      {
        continue;
      }

      /* return error for required token */
      result = GBA_HTTP_VALIDATE_FAIL;
      break;
    }

    /* compare has_quotes and values*/
    if((token_table_ptr[i].has_quotes != GBA_HTTP_OPTIONAL) &&
       (token_table_ptr[i].has_quotes != resp_token_list[j].has_quotes))
    {
      GBA_MSG_ERR_2("has_quotes check failed, index 0x%x, 0x%x",
                    j, resp_token_list[j].has_quotes);
      result = GBA_HTTP_VALIDATE_FAIL;
      break;
    }

    /* special check for QOP value */
    if(token_table_ptr[i].token_id == GBA_HTTP_QOP_TOKEN)
    {
      if((strncasecmp(resp_token_list[j].value_ptr,
                      GBA_HTTP_QOP_AUTH_INT_TAG,
                      strlen(GBA_HTTP_QOP_AUTH_INT_TAG)) != 0) &&
         (strncasecmp(resp_token_list[j].value_ptr,
                      GBA_HTTP_QOP_AUTH_TAG,
                      strlen(GBA_HTTP_QOP_AUTH_INT_TAG)) != 0))
      {
        GBA_MSG_ERR_2("value check failed, index 0x%x, 0x%x",
                      j, resp_token_list[j].value_ptr);
        result = GBA_HTTP_VALIDATE_FAIL;
        break;
      }

      if(resp_type == GBA_HTTP_RESPONSE_FINAL_CMD &&
         http_error == DS_HTTP_ERROR_RSPAUTH_MISMATCH &&
         (strncasecmp(resp_token_list[j].value_ptr,
                      GBA_HTTP_QOP_AUTH_INT_TAG,
                      strlen(GBA_HTTP_QOP_AUTH_INT_TAG)) == 0))
      {
        GBA_MSG_ERR_2("value check failed, re-try needed, index 0x%x, 0x%x",
                      j, resp_token_list[j].value_ptr);
        result = GBA_HTTP_VALIDATE_RETRY;
        break;
      }

      /* sucessfully validate QOP for the following:
         GBA_HTTP_RESPONSE_FINAL_CMD with "auth" & resp mis-match,
         GBA_HTTP_RESPONSE_FINAL_CMD with "auth"/"auth-int" but no resp mis-match
         GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD with QOP = "auth"/"auth-int" */
      continue;
    }

    /* skip checking value if it's not define in token table */
    if(token_table_ptr[i].value_ptr == NULL)
    {
      continue;
    }

    if(strncasecmp(token_table_ptr[i].value_ptr,
                   resp_token_list[j].value_ptr,
                   strlen(token_table_ptr[i].value_ptr)) != 0)
    {
      GBA_MSG_ERR_2("value check failed, index 0x%x, 0x%x",
                    j, resp_token_list[j].value_ptr);
      result = GBA_HTTP_VALIDATE_FAIL;
      break;
    }
  }

  /* done with comparing all tokens with table */
  /* free resp_token_list[].value */
  for(i = 0; i < num_token ; i++)
  {
    if(resp_token_list[i].value_ptr != NULL)
    {
      GBA_FREE(resp_token_list[i].value_ptr);
    }
  }
  return result;
} /* gba_http_validate_auth_response_header */


/*===========================================================================
FUNCTION GBA_HTTP_RETRIEVE_RAND_AND_AUTN

DESCRIPTION
  This function retrieve the AUTN and RAND from NONCE from BSF

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_http_retrieve_rand_and_autn (
  const char                      * nonce_ptr,
  gba_http_authenticate_data_type * auth_ptr
)
{
  uint8                     * binary_nonce_ptr   = NULL;
  uint16                      nonce_len          = 0;
  uint16                      offset             = 0;

  /* parse the nonce for RAND and AUTN
     RFC 3310 S3.2, 31.102 7.1.2 */
  binary_nonce_ptr = gba_util_alloc_base64string_to_bin(nonce_ptr, &nonce_len);
  if(binary_nonce_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  memscpy(auth_ptr->rand, GBA_RAND_LEN,
          &binary_nonce_ptr[offset], GBA_RAND_LEN);

  offset = 0;
  auth_ptr->auth_challenge.data_len = GBA_RAND_LEN + GBA_AUTN_LEN + 2;
  auth_ptr->auth_challenge.data[offset++] = GBA_RAND_LEN;
  memscpy(&auth_ptr->auth_challenge.data[offset], GBA_RAND_LEN,
          &binary_nonce_ptr[0], GBA_RAND_LEN);
  offset = offset + GBA_RAND_LEN;
  auth_ptr->auth_challenge.data[offset++] = GBA_AUTN_LEN;
  memscpy(&auth_ptr->auth_challenge.data[offset], GBA_AUTN_LEN,
          &binary_nonce_ptr[GBA_RAND_LEN], GBA_AUTN_LEN);

  GBA_FREE(binary_nonce_ptr);

  return GBA_SUCCESS;
} /* gba_http_retrieve_rand_and_autn */


/*===========================================================================
FUNCTION GBA_HTTP_GET_TMPI_SUPPORT

DESCRIPTION
  This function determine if BSF support the TMPI

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_http_get_tmpi_support (
  const char                 * server_value_ptr,
  boolean                    * support_tmpi_ptr
)
{
  char                             str_buf[GBA_HTTP_HEADER_VALUE_MAX_LEN + 1] = {0};
  char                           * str_ptr     = NULL;
  char                           * remain_ptr  = NULL;
  char                           * temp_ptr    = NULL;

  if(server_value_ptr == NULL ||
     support_tmpi_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  *support_tmpi_ptr = FALSE;
  strlcpy(str_buf, server_value_ptr, GBA_HTTP_HEADER_VALUE_MAX_LEN + 1);

  str_ptr = gba_util_trim_spaces(str_buf);
  if(str_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  while(str_ptr != NULL)
  {
    /* do a strchr to check if semi colon is in place, then take the token */
    remain_ptr = strchr((const char*)str_ptr, GBA_HTTP_SEMI_COLON_CHAR);

    /* if remain_ptr == NULL, ie. no delimiter is found */
    if(remain_ptr != NULL)
    {
      temp_ptr = remain_ptr;
      remain_ptr++;
      temp_ptr[0] = GBA_HTTP_NULL_CHAR;
    }

    str_ptr = gba_util_trim_spaces(str_ptr);

    if(str_ptr != NULL &&
       ((!gba_nv_get_tmpi_user_agent() &&
         strncasecmp(str_ptr,
                     GBA_HTTP_SERVER_TMPI_STR,
                     strlen(GBA_HTTP_SERVER_TMPI_STR)) == 0) ||
        (gba_nv_get_tmpi_user_agent() &&
         strncasecmp(str_ptr,
                     GBA_HTTP_SERVER_CUSTOM_TMPI_STR,
                     strlen(GBA_HTTP_SERVER_CUSTOM_TMPI_STR)) == 0)))
    {
      *support_tmpi_ptr = TRUE;
      return GBA_SUCCESS;
    }
    str_ptr = remain_ptr;
  }

  return GBA_SUCCESS;
} /* gba_http_get_tmpi_support */


/*===========================================================================
FUNCTION GBA_HTTP_PARSE_CONTENT_XML

DESCRIPTION
  This function parse the content xml and retrieve btid and lifetime
  data from xml.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_http_parse_content_xml (
  const char                * xml_ptr,
  gba_btid_type             * btid_ptr,
  gba_lifetime_type         * lifetime_ptr
)
{
  mre_status_t              status;
  mre_xml_root_t          * xmlroot_ptr  = NULL;
  mre_xml_element_t const * elem_ptr;
  char const              * str_ptr;
  char const              * ptemp_ptr;

  status = mre_xml_parse_from_text(xml_ptr, (const mre_xml_root_t **) &xmlroot_ptr);
  if(status != MRE_STATUS_SUCCESS || xmlroot_ptr == NULL)
  {
    GBA_MSG_ERR_1("error parsing content XML, 0x%x", status);
    return GBA_GENERIC_ERROR;
  }

  /*  The root element should be "p1:BootstrappingInfo" */
  elem_ptr = mre_xml_get_root_element(xmlroot_ptr);
  if(elem_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  str_ptr = mre_xml_get_tag(elem_ptr);

  /* skip the tag before ':' */
  ptemp_ptr = strchr(str_ptr, GBA_HTTP_COLON_CHAR);
  if(ptemp_ptr == NULL)
  {
    ptemp_ptr = str_ptr;
  }
  else
  {
    ptemp_ptr = &ptemp_ptr[1];
  }
  if(strcmp(GBA_HTTP_BS_CONTENT_TAG, ptemp_ptr) != 0)
  {
    return GBA_GENERIC_ERROR;
  }

  /*  Ensure that the text associated with the 'btid' and 'lifetime' tags is as in the XML. */
  str_ptr = mre_xml_get_text_from_child(elem_ptr, GBA_HTTP_BTID_CONTENT_TAG);
  if(str_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  memset(btid_ptr, 0x00, sizeof(gba_btid_type));
  btid_ptr->data_len = (uint16)strnlen(str_ptr, GBA_MAX_BTID_LEN + 1);
  if(btid_ptr->data_len > GBA_MAX_BTID_LEN)
  {
    btid_ptr->data_len = 0;
    return GBA_GENERIC_ERROR;
  }
  memscpy(btid_ptr->data, btid_ptr->data_len, str_ptr, btid_ptr->data_len);

  str_ptr = mre_xml_get_text_from_child(elem_ptr, GBA_HTTP_LIFETIME_CONTENT_TAG);
  if(str_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  memset(lifetime_ptr, 0x00, sizeof(gba_lifetime_type));
  lifetime_ptr->data_len = (uint16)strnlen(str_ptr, GBA_MAX_LIFETIME_LEN + 1);
  if(lifetime_ptr->data_len > GBA_MAX_LIFETIME_LEN)
  {
    lifetime_ptr->data_len = 0;
    return GBA_GENERIC_ERROR;
  }
  memscpy(lifetime_ptr->data, lifetime_ptr->data_len, str_ptr, lifetime_ptr->data_len);

  /* release object after content parsing */
  REF_CNT_OBJ_RELEASE_IF(xmlroot_ptr);
  return GBA_SUCCESS;
} /* gba_http_parse_content_xml */


/*===========================================================================
FUNCTION GBA_HTTP_PROCESS_USER_IDENTITY_REQUEST

DESCRIPTION
  This function will put up the user identity http header and sends it
  to given BSF address

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_http_process_user_identity_request (
  char                           * bsf_ptr,
  uint32                         * request_id_ptr
)
{
  ds_http_header_s_type      * req_header_ptr       = NULL;
  gba_nv_imei_data_type      * imei_header_ptr      = NULL;
  gba_result_enum_type         gba_status           = GBA_SUCCESS;
  sint15                       request_errno        = 0;
  uint16                       data_len             = 0;
  uint16                       avail_buf_size       = GBA_HTTP_HEADER_VALUE_MAX_LEN;
  uint8                        i                    = 0;
  gba_http_header_value_type   str_buf              = {0x00};
  gba_nv_bsf_http_enum_type    bsf_protocol         = GBA_NV_BSF_HTTP;
  uint16                       token_value_max_size = GBA_HTTP_HEADER_VALUE_MAX_LEN + 1;

  gba_http_token_value_type gba_http_authorization_value_table[] =
  /* token enum,              is_optional, has_quotes,        value */
  {{GBA_HTTP_DIGEST_TOKEN,    FALSE,       GBA_HTTP_FALSE,    NULL},
   {GBA_HTTP_USERNAME_TOKEN,  FALSE,       GBA_HTTP_TRUE,     NULL},
   {GBA_HTTP_REALM_TOKEN,     FALSE,       GBA_HTTP_TRUE,     bsf_ptr},
   {GBA_HTTP_URI_TOKEN,       FALSE,       GBA_HTTP_TRUE,     GBA_HTTP_FRWD_SLASH_STR},
   {GBA_HTTP_NONCE_TOKEN,     FALSE,       GBA_HTTP_TRUE,     GBA_HTTP_EMPTY_STR},
   {GBA_HTTP_RESPONSE_TOKEN,  FALSE,       GBA_HTTP_TRUE,     GBA_HTTP_EMPTY_STR}
  };

  gba_http_req_header_type gba_http_request_header_table[] =
  {
    {GBA_HTTP_USER_AGENT_HEADER, /* header id enum */
     GBA_HTTP_USER_AGENT_STR,    /* header value/cust header name */
     NULL,                       /* auth/cust_value_table pointer */
     0},                         /* auth/cust_value_table size */

    {GBA_HTTP_AUTHORIZATION_HEADER,
     NULL,
     gba_http_authorization_value_table,
     sizeof(gba_http_authorization_value_table) / sizeof(gba_http_token_value_type)}
  };

  if (gba_http_global_data_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  if(bsf_ptr == NULL || request_id_ptr == NULL)
  {
    GBA_MSG_ERR_0("Invalid input parameters");
    return GBA_INCORRECT_PARAMS;
  }

  /* give value to user name token in gba_http_authorization_value_table */
  for (i = 0;
       i < sizeof(gba_http_authorization_value_table) / sizeof(gba_http_token_value_type);
       i++)
  {
    if (GBA_HTTP_USERNAME_TOKEN == gba_http_authorization_value_table[i].token_id)
    {
      gba_http_authorization_value_table[i].value_ptr = gba_http_global_data_ptr->username;
      break;
    }
  }

  gba_status = gba_http_open_session();

  if(gba_status != GBA_SUCCESS ||
     gba_http_session_id.session_id == DS_HTTP_ERROR)
  {
    GBA_MSG_ERR_0("failed to open HTTP session");
    return GBA_GENERIC_ERROR;
  }
  else if(gba_http_session_id.session_id != DS_HTTP_ERROR &&
          gba_http_session_id.session_status != DS_HTTP_READY)
  {
    /* wait for DS open_session callback before sending get request */
    gba_http_global_data_ptr->pending_cmd = GBA_HTTP_PENDING_GET_REQUEST;
    return GBA_SUCCESS;
  }

  req_header_ptr = GBA_MALLOC(sizeof(ds_http_header_s_type));
  if(req_header_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  memset(req_header_ptr, 0x00, sizeof(ds_http_header_s_type));

  for(i = 0;
      i < (sizeof(gba_http_request_header_table)/sizeof(gba_http_request_header_table[0]));
      i++)
  {
    data_len = 0;
    avail_buf_size = GBA_HTTP_HEADER_VALUE_MAX_LEN;

    switch(gba_http_request_header_table[i].header_id)
    {
       case GBA_HTTP_USER_AGENT_HEADER:
         /* set user agent header value */
         data_len = (uint16)strnlen(gba_http_request_header_table[i].str_data_ptr, GBA_HTTP_HEADER_VALUE_MAX_LEN + 1);

         /* check gba nv for custom user agent header */
         if (gba_nv_get_tmpi_user_agent())
         {
           gba_status = gba_http_set_header_value(req_header_ptr->user_agent,
                                                  avail_buf_size,
                                                  GBA_HTTP_CUSTOM_USER_AGENT_STR,
                                                  data_len);
         }
         else
         {
           gba_status = gba_http_set_header_value(req_header_ptr->user_agent,
                                                  avail_buf_size,
                                                  gba_http_request_header_table[i].str_data_ptr,
                                                  data_len);
         }
         break;

       case GBA_HTTP_AUTHORIZATION_HEADER:
         /* set auth header value */
         gba_status = gba_http_process_custom_header_value(
                          gba_http_request_header_table[i].table_size,
                          gba_http_request_header_table[i].token_table_ptr,
                          req_header_ptr->authorization,
                          token_value_max_size);
         break;

       default:
         gba_status = GBA_INCORRECT_PARAMS;
         break;
    }
    if(gba_status != GBA_SUCCESS)
    {
      break;
    }
  }

  if(gba_status == GBA_SUCCESS)
  {
    /* construct URI with BSF and check BSF NV for which protocol
       to use, http or https */
    bsf_protocol = gba_nv_get_bsf_protocol();
    if (GBA_NV_BSF_HTTPS == bsf_protocol)
    {
      strlcat(str_buf, GBA_HTTPS_URL_TAG, sizeof(gba_http_header_value_type));
    }
    else
    {
      strlcat(str_buf, GBA_HTTP_URL_TAG, sizeof(gba_http_header_value_type));
    }
    strlcat(str_buf, bsf_ptr, sizeof(gba_http_header_value_type));

    /* check GBA IMEI header NV for adding IMEI header */
    imei_header_ptr = (gba_nv_imei_data_type *)GBA_MALLOC(sizeof(gba_nv_imei_data_type));

    if (NULL == imei_header_ptr)
    {
      GBA_FREE(req_header_ptr);
      return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    if(GBA_SUCCESS == gba_nv_get_imei_http_header(imei_header_ptr) &&
       (imei_header_ptr->imei_header_name[0] != 0x0))
    {
      strlcpy(req_header_ptr->cust_header_list[req_header_ptr->num_cust_headers].name,
              imei_header_ptr->imei_header_name,
              sizeof(req_header_ptr->cust_header_list[req_header_ptr->num_cust_headers].name));
      strlcpy(req_header_ptr->cust_header_list[req_header_ptr->num_cust_headers].value,
              imei_header_ptr->imei_digits,
              sizeof(req_header_ptr->cust_header_list[req_header_ptr->num_cust_headers].value));
      req_header_ptr->num_cust_headers++;
    }

    GBA_FREE(imei_header_ptr);

    /* contruct HTTP Cookies */
    gba_http_cookie_get_cookie_header(gba_http_global_data_ptr->impi,
                                      gba_http_global_data_ptr->session_type,
                                      sizeof(req_header_ptr->cookie)/sizeof(ds_http_cookie_name_s_type),
                                      &req_header_ptr->num_cookies,
                                      req_header_ptr->cookie);

    *request_id_ptr = ds_http_create_get_request((const uint32)gba_http_session_id.session_id,
                                                 (const char*)str_buf,
                                                 req_header_ptr,
                                                 0,
                                                 &request_errno);

    gba_http_global_data_ptr->pending_cmd = GBA_HTTP_PENDING_401_UNAUTH;
    GBA_MSG_HIGH_3("http create get request: session id 0x%x; request id 0x%x; request errno 0x%x",
                   gba_http_session_id.session_id, *request_id_ptr, request_errno);

    if(*request_id_ptr == DS_HTTP_ERROR)
    {
      gba_http_global_data_ptr->pending_cmd = GBA_HTTP_PENDING_CMD_NONE;
      gba_status = GBA_GENERIC_ERROR;
    }
  }

  GBA_FREE(req_header_ptr);
  return gba_status;
} /* gba_http_process_user_identity_request */


/*===========================================================================
FUNCTION GBA_HTTP_PROCESS_RSP

DESCRIPTION
  This function process HTTP response from DS

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static void gba_http_process_rsp (
  gba_http_response_data_type  rsp_data
)
{
  gba_bootstrap_callback_data_type    callback_data;
  gba_result_enum_type                gba_status    = GBA_SUCCESS;
  boolean                             auth_header   = FALSE;
  boolean                             server_header = FALSE;
  uint16                              i             = 0;
  gba_http_result_enum_type           result        = GBA_HTTP_VALIDATE_SUCCESS;
  gba_http_response_error_data_type  *resp_err_ptr  = NULL;

  if (gba_http_global_data_ptr == NULL)
  {
    return;
  }

  memset(&callback_data, 0x00, sizeof(gba_bootstrap_callback_data_type));

  if(rsp_data.session_id != gba_http_session_id.session_id ||
     (gba_http_session_id.session_status == DS_HTTP_READY &&
      rsp_data.request_id != gba_http_global_data_ptr->request_id) ||
      gba_http_global_data_ptr->callback == NULL)
  {
    GBA_MSG_ERR_3("session id 0x%x, request id 0x%x, global callback 0x%x",
                  rsp_data.session_id,
                  rsp_data.request_id,
                  gba_http_global_data_ptr->callback);
    return;
  }

  /* if HTTP status = HTTP ready, ie. session open successful.
     process pending user identity request */
  if(rsp_data.error == DS_HTTP_READY &&
     gba_http_global_data_ptr->pending_cmd == GBA_HTTP_PENDING_GET_REQUEST)
  {
    gba_http_global_data_ptr->pending_cmd = GBA_HTTP_PENDING_CMD_NONE;
    gba_http_session_id.session_status = DS_HTTP_READY;
    gba_status = gba_http_process_user_identity_request(gba_http_global_data_ptr->bsf,
                                                        &gba_http_global_data_ptr->request_id);
    if(gba_status != GBA_SUCCESS)
    {
      /* call back with error  */
      callback_data.cmd_type = GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD;
      (gba_http_global_data_ptr->callback)(GBA_GENERIC_ERROR,
                                           &callback_data,
                                           gba_http_global_data_ptr->reference_id);
    }
    return;
  }

  switch(gba_http_global_data_ptr->pending_cmd)
  {
     case GBA_HTTP_PENDING_401_UNAUTH:
     case GBA_HTTP_PENDING_GET_REQUEST:
     case GBA_HTTP_PENDING_RESOLVE_AUTH:
      callback_data.cmd_type = GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD;
      resp_err_ptr = &callback_data.data.auth.error_data;
      break;
    case GBA_HTTP_PENDING_200_OK:
      callback_data.cmd_type = GBA_HTTP_RESPONSE_FINAL_CMD;
      resp_err_ptr = &callback_data.data.http_200_ok.error_data;
      break;
    default:
      break;
  }

  if(resp_err_ptr == NULL)
  {
    GBA_MSG_ERR_1("Unexpected callback for pending cmd 0x%x",
                  gba_http_global_data_ptr->pending_cmd);
    return;
  }

  /* update http cookie regardless of HTTP status */
  for (i = 0; i < rsp_data.num_cookies; i++)
  {
    (void)gba_http_cookie_add_cookie(gba_http_global_data_ptr->impi,
                                     gba_http_global_data_ptr->session_type,
                                     &rsp_data.cookie_ptr[i]);
  }

  if((rsp_data.error != DS_HTTP_STATUS_OK &&
      rsp_data.error != DS_HTTP_ERROR_RSPAUTH_MISMATCH) ||
     rsp_data.http_status != GBA_HTTP_STATUS_200OK ||
     rsp_data.content_size == 0 || rsp_data.content_ptr == NULL ||
     gba_http_global_data_ptr->pending_cmd != GBA_HTTP_PENDING_200_OK)
  {
    /* callback with error  */
    GBA_MSG_ERR_3("error 0x%x, http_status 0x%x, content_size 0x%x",
                  rsp_data.error, rsp_data.http_status, rsp_data.content_size);

    if(gba_http_global_data_ptr->pending_cmd == GBA_HTTP_PENDING_401_UNAUTH &&
       rsp_data.error == DS_HTTP_ERROR_HTTP_STATUS &&
       rsp_data.http_status != GBA_HTTP_STATUS_403FORBIDDEN)
    {
      /* check if TMPI is being used and re-try */
      if((strlen(gba_http_global_data_ptr->impi) != strlen(gba_http_global_data_ptr->username)) ||
         (strncasecmp(gba_http_global_data_ptr->impi, gba_http_global_data_ptr->username,
                      strlen(gba_http_global_data_ptr->impi)) != 0))
      {
        memscpy(gba_http_global_data_ptr->username, GBA_HTTP_HEADER_VALUE_MAX_LEN,
                gba_http_global_data_ptr->impi, GBA_HTTP_HEADER_VALUE_MAX_LEN);
        gba_status = gba_http_process_user_identity_request(gba_http_global_data_ptr->bsf,
                                                            &gba_http_global_data_ptr->request_id);
        if(gba_status == GBA_SUCCESS)
        {
          return;
        }
      }
    }

    switch(rsp_data.error)
    {
      case DS_HTTP_ERROR_NETWORK_DOWN:
      case DS_HTTP_ERROR_DSNET:
      case DS_HTTP_ERROR_SOCKET:
      case DS_HTTP_ERROR_DNS:
        gba_status = GBA_NETWORK_ERROR;
        resp_err_ptr->data.network_error.netdown_reason_present = FALSE;

        /* get DSS reason enum if it is a network related error */
        if(rsp_data.error == DS_HTTP_ERROR_NETWORK_DOWN ||
           rsp_data.error == DS_HTTP_ERROR_DSNET)
        {
          dss_net_down_reason_type  dss_reason    = DSS_NET_DOWN_REASON_NOT_SPECIFIED;

          (void)ds_http_last_netdownreason(rsp_data.session_id, &dss_reason);
          resp_err_ptr->data.network_error.netdown_reason_present = TRUE;
          resp_err_ptr->data.network_error.netdown_reason = (uint32)dss_reason;
        }
        break;

      case DS_HTTP_ERROR_HTTP_STATUS:
      case DS_HTTP_ERROR_RSPAUTH_MISMATCH:
        gba_status = GBA_SERVER_ERROR;
        resp_err_ptr->data.server_error.http_status_present = TRUE;
        resp_err_ptr->data.server_error.http_status = rsp_data.http_status;
        break;

      case DS_HTTP_ERROR_HTTP_HEADER:
        gba_status = GBA_GENERIC_ERROR;
        break;

      case DS_HTTP_ERROR_SSL:
      default:
        gba_status = GBA_SERVER_ERROR;
        resp_err_ptr->data.server_error.http_status_present = FALSE;
        break;
    }

    /* return FAILURE for all other errors, includes 403 error */
    (gba_http_global_data_ptr->callback)(gba_status,
                                         &callback_data,
                                         gba_http_global_data_ptr->reference_id);

    gba_http_global_data_ptr->pending_cmd = GBA_HTTP_PENDING_CMD_NONE;
    return;
  }

  /* expects 200 OK response with XML content */
  for(i = 0;
      ((!auth_header || !server_header) && (i < rsp_data.num_headers));
       i++)
  {
    GBA_MSG_SPRINTF_2("GBA rsp data: %s : %s",
                      rsp_data.header_info_ptr[i].name,
                      rsp_data.header_info_ptr[i].value);

    /* Auth header is required in response-header for GBA */
    if(strncasecmp(rsp_data.header_info_ptr[i].name,
                   GBA_HTTP_AUTH_INFO_STR,
                   strlen(GBA_HTTP_AUTH_INFO_STR)) == 0)
    {
      /* to validate Auth header */
      result = gba_http_validate_auth_response_header(
                            (const char*)rsp_data.header_info_ptr[i].value,
                            NULL,
                            GBA_HTTP_RESPONSE_FINAL_CMD,
                            rsp_data.error);

      if(result == GBA_HTTP_VALIDATE_FAIL)
      {
        GBA_MSG_ERR_0("Invalid auth response header");
        gba_status = GBA_GENERIC_ERROR;
        break;
      }
      auth_header = TRUE;
    }
    /* RCF 2616 S6.2: server header field is optional in response-header.
       checking server header for TMPI support, default to FALSE if header
       is not present */
    else if(strncasecmp(rsp_data.header_info_ptr[i].name,
                        GBA_HTTP_SERVER_STR,
                        strlen(GBA_HTTP_SERVER_STR)) == 0)
    {
      /* TS 33.200 S4.5.2: BSF includes string "3gpp-gba-tmpi" in Server
         header to indicate TMPI is supported by BSF */
      gba_status = gba_http_get_tmpi_support(
                       (const char*)rsp_data.header_info_ptr[i].value,
                       &callback_data.data.http_200_ok.support_tmpi);

      if(gba_status != GBA_SUCCESS)
      {
        GBA_MSG_MED_1("failed to get TMPI support from server header",
                      gba_status);
        break;
      }

      server_header = TRUE;
    }
  }

  if(!auth_header || gba_status != GBA_SUCCESS)
  {
    GBA_MSG_ERR_3("missing or invalid AUTH INFO header, "
                  "auth header 0x%x, gba_status 0x%x, num headers 0x%x",
                  auth_header, gba_status, rsp_data.num_headers);
    /* call back with error  */
    (gba_http_global_data_ptr->callback)(GBA_AUTH_FAILURE,
                                         &callback_data,
                                         gba_http_global_data_ptr->reference_id);
    return;
  }

  if(result == GBA_HTTP_VALIDATE_RETRY)
  {
    if(gba_http_global_data_ptr->qop_retry_count < GBA_MAX_QOP_RETRY)
    {
      gba_status = gba_http_process_user_identity_request(gba_http_global_data_ptr->bsf,
                                                          &gba_http_global_data_ptr->request_id);
    }

    if(gba_status != GBA_SUCCESS ||
       gba_http_global_data_ptr->qop_retry_count >= GBA_MAX_QOP_RETRY)
    {
      GBA_MSG_ERR_1("Failed to retry QOP mismatch, retry count 0x%x",
                    gba_http_global_data_ptr->qop_retry_count);
      (gba_http_global_data_ptr->callback)(GBA_AUTH_FAILURE,
                                           &callback_data,
                                           gba_http_global_data_ptr->reference_id);
    }
    gba_http_global_data_ptr->qop_retry_count++;
    return;
  }

  gba_status = gba_http_parse_content_xml((const char*) rsp_data.content_ptr,
                                          &callback_data.data.http_200_ok.btid,
                                          &callback_data.data.http_200_ok.lifetime);

  if(gba_status != GBA_SUCCESS)
  {
    GBA_MSG_ERR_1("failed to get btid / lifetime from content xml",
                  gba_status);
  }

  /* call back to GBA module for authentication */
  callback_data.cmd_type = GBA_HTTP_RESPONSE_FINAL_CMD;

  (gba_http_global_data_ptr->callback)(gba_status,
                                       &callback_data,
                                       gba_http_global_data_ptr->reference_id);
} /* gba_http_process_rsp */


/*===========================================================================
FUNCTION GBA_HTTP_PROCESS_BLOCK_RSP

DESCRIPTION
  This function process HTTP block response from DS

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_http_process_block_rsp (
  gba_http_block_response_data_type rsp_data
)
{
  gba_result_enum_type                gba_status      = GBA_SUCCESS;
  gba_http_result_enum_type           result          = GBA_HTTP_VALIDATE_FAIL;
  gba_bootstrap_callback_data_type    callback_data   = {0};
  uint16                              i               = 0;

  if (gba_http_global_data_ptr == NULL)
  {
    return;
  }

  /* close the DS socket by sending NULL credential */
  if(rsp_data.session_id != gba_http_session_id.session_id ||
     rsp_data.request_id != gba_http_global_data_ptr->request_id ||
     rsp_data.event_type != GBA_HTTP_BLOCK_AUTHENTICATION ||
     gba_http_global_data_ptr->callback == NULL)
  {
    gba_bootstrap_callback_data_type callback_data;

    GBA_MSG_ERR_0("gba_http_process_block_rsp - Invalid cb message received.");

    memset(&callback_data, 0x00, sizeof(gba_bootstrap_callback_data_type));
    switch(rsp_data.event_type)
    {
       case GBA_HTTP_BLOCK_SSL_CERTIFICATE_ALERT:
         ds_http_resolve_ssl_cert_block(rsp_data.session_id,
                                        rsp_data.request_id,
                                        DS_HTTP_SSL_ABORT);

         /* return auth failure to gba bootstrap. */
         if (gba_http_global_data_ptr->callback != NULL)
         {
           callback_data.cmd_type = GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD;
           callback_data.data.auth.error_data.data.server_error.http_status_present = FALSE;
           (gba_http_global_data_ptr->callback)(GBA_SERVER_ERROR,
                                                &callback_data,
                                                gba_http_global_data_ptr->reference_id);
         }
         break;

       case GBA_HTTP_BLOCK_AUTHENTICATION:
         ds_http_resolve_auth_block(rsp_data.session_id,
                                    rsp_data.request_id,
                                    NULL);
         break;

       default:
         GBA_MSG_ERR_1("unknown event type 0x%x", rsp_data.event_type);
         break;
    }
    return;
  }

  result = gba_http_validate_auth_response_header(
                        (const char*)rsp_data.auth_data.auth_response_header_ptr,
                        (const char*)rsp_data.auth_data.realm_ptr,
                        GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD,
                        DS_HTTP_STATUS_OK);

  if(result != GBA_HTTP_VALIDATE_SUCCESS)
  {
    GBA_MSG_ERR_0("Invalid auth response header");
    ds_http_resolve_auth_block(rsp_data.session_id, rsp_data.request_id, NULL);
    /* call back to GBA module to notify error. */
    (gba_http_global_data_ptr->callback)(GBA_GENERIC_ERROR,
                                         NULL,
                                         gba_http_global_data_ptr->reference_id);
    return;
  }

  gba_status = gba_http_retrieve_rand_and_autn(
                   (const char*) rsp_data.auth_data.nonce_ptr,
                   &callback_data.data.auth);

  if(gba_status != GBA_SUCCESS)
  {
    memset(&callback_data, 0, sizeof(gba_bootstrap_callback_data_type));
  }

  /* update http cookies */
  for (i = 0; i < rsp_data.auth_data.num_cookies; i++)
  {
    (void)gba_http_cookie_add_cookie(gba_http_global_data_ptr->impi,
                                     gba_http_global_data_ptr->session_type,
                                     &rsp_data.auth_data.cookie_ptr[i]);
  }

  gba_http_global_data_ptr->pending_cmd = GBA_HTTP_PENDING_RESOLVE_AUTH;
  callback_data.cmd_type = GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD;

  /* call back to GBA module for authentication */
  (gba_http_global_data_ptr->callback)(gba_status,
                                       &callback_data,
                                       gba_http_global_data_ptr->reference_id);
} /* gba_http_process_block_rsp */


/*===========================================================================
FUNCTION GBA_HTTP_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in gba_main, is called to process HTTP event

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_http_handle_cmd_sig (
  void
)
{
  gba_http_cmd_type          * task_cmd_ptr  = NULL;

  task_cmd_ptr = (gba_http_cmd_type*)(q_get(&gba_http_cmd_q));

  /* If there is a command on either of the queues */
  if(task_cmd_ptr != NULL)
  {
    GBA_MSG_HIGH_0("GBA_HTTP_CMD_SIG received");

    switch (task_cmd_ptr->cmd_type)
    {
      case GBA_HTTP_RSP:
        gba_http_process_rsp(task_cmd_ptr->data.rsp_data);
        break;
      case GBA_HTTP_BLOCK_RSP:
        gba_http_process_block_rsp(task_cmd_ptr->data.block_rsp_data);
        break;
      default:
        break;
    }
  }

  (void) rex_clr_sigs(rex_self(), GBA_HTTP_CMD_SIG);

  /* Set the signal if there are one or more commands to look at */
  if (q_cnt(&gba_http_cmd_q) > 0)
  {
    /*  Items on the queue. Set the signal */
    (void) rex_set_sigs(rex_self(), GBA_HTTP_CMD_SIG);
  }

  gba_http_free_task_cmd_ptr(task_cmd_ptr);
} /* gba_http_handle_cmd_sig */


/*===========================================================================
FUNCTION:      GBA_HTTP_INIT

DESCRIPTION:
  GBA HTTP init function called during task init

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void gba_http_init (
  void
)
{
  (void)q_init(&gba_http_cmd_q);
  (void)gba_http_cleanup();
} /* gba_http_init */


/*===========================================================================
FUNCTION:      GBA_HTTP_CLEANUP

DESCRIPTION:
  GBA HTTP cleanup function called to clean up refrerence data

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  gba_result_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
gba_result_enum_type gba_http_cleanup (
  void
)
{
  /* close DS socket if DS is pending for 401 un-auth response */
  if (gba_http_global_data_ptr != NULL &&
      gba_http_global_data_ptr->pending_cmd == GBA_HTTP_PENDING_RESOLVE_AUTH)
  {
    ds_http_resolve_auth_block(gba_http_session_id.session_id,
                               gba_http_global_data_ptr->request_id, NULL);
  }
  GBA_FREE(gba_http_global_data_ptr);

  /* Close the opened session and initialize the session id and status */
  if(gba_http_session_id.session_id != DS_HTTP_ERROR &&
     gba_http_session_id.session_status == DS_HTTP_READY)
  {
    ds_http_close_session(gba_http_session_id.session_id);
  }


  gba_http_session_id.session_id = DS_HTTP_ERROR;
  gba_http_session_id.session_status = DS_HTTP_ERROR_HTTP_STATUS;

  /* write cookies to storage and clean up */
  gba_http_cookie_cleanup();

  GBA_MSG_HIGH_0("gba_http_cleanup");
  return GBA_SUCCESS;
} /* gba_http_cleanup */


/*===========================================================================
FUNCTION GBA_HTTP_SEND_USER_IDENTITY_REQUEST

DESCRIPTION
  This function sends the user identity to given BSF address

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_http_send_user_identity_request
(
  const gba_impi_type                  * impi_ptr,
  const gba_impi_type                  * tmpi_ptr,
  const gba_bsf_address_type           * bsf_ptr,
  const gba_bsf_address_type           * realm_ptr,
  const gba_apn_type                   * apn_ptr,
  gba_session_type                       session_type,
  uint32                                 reference_id,
  gba_bootstrap_callback_type            user_identity_cb
)
{
  gba_http_header_value_type   bsf_str_buf = {0};

  if(user_identity_cb == NULL ||
     impi_ptr == NULL ||
     bsf_ptr == NULL ||
     realm_ptr == NULL ||
     impi_ptr->data_len == 0 ||
     bsf_ptr->data_len == 0)
  {
    return GBA_INCORRECT_PARAMS;
  }

  gba_http_global_data_ptr =
    (gba_http_ref_data_type *)GBA_MALLOC(sizeof(gba_http_ref_data_type));

  if (gba_http_global_data_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  GBA_MSG_MED_0("gba_http_send_user_identity_request");

  memset(gba_http_global_data_ptr->impi, 0x00, sizeof(gba_http_header_value_type));
  memset(gba_http_global_data_ptr->bsf, 0x00, sizeof(gba_http_header_value_type));
  memset(gba_http_global_data_ptr->realm, 0x00, sizeof(gba_http_header_value_type));

  memscpy(gba_http_global_data_ptr->impi, GBA_HTTP_HEADER_VALUE_MAX_LEN,
          impi_ptr->data, impi_ptr->data_len);
  if(tmpi_ptr != NULL &&
     tmpi_ptr->data_len > 0)
  {
    memscpy(gba_http_global_data_ptr->username, GBA_HTTP_HEADER_VALUE_MAX_LEN,
            tmpi_ptr->data, tmpi_ptr->data_len);
  }
  else
  {
    memscpy(gba_http_global_data_ptr->username, GBA_HTTP_HEADER_VALUE_MAX_LEN,
            impi_ptr->data, impi_ptr->data_len);
  }
  memscpy(bsf_str_buf, GBA_HTTP_HEADER_VALUE_MAX_LEN, bsf_ptr->data, bsf_ptr->data_len);
  memscpy(gba_http_global_data_ptr->bsf, GBA_HTTP_HEADER_VALUE_MAX_LEN, bsf_ptr->data, bsf_ptr->data_len);
  memscpy(gba_http_global_data_ptr->realm, GBA_HTTP_HEADER_VALUE_MAX_LEN, realm_ptr->data, realm_ptr->data_len);

  if(apn_ptr != NULL && apn_ptr->apn_available)
  {
    gba_http_global_data_ptr->apn_info = *apn_ptr;
    GBA_MSG_SPRINTF_1("apn : %s", gba_http_global_data_ptr->apn_info.apn);
  }

  GBA_MSG_SPRINTF_1("username : %s", gba_http_global_data_ptr->username);
  GBA_MSG_SPRINTF_1("bsf : %s", gba_http_global_data_ptr->bsf);
  GBA_MSG_SPRINTF_1("realm : %s", gba_http_global_data_ptr->realm);

  gba_http_global_data_ptr->session_type = session_type;
  gba_http_global_data_ptr->reference_id = reference_id;
  gba_http_global_data_ptr->callback     = user_identity_cb;

  return gba_http_process_user_identity_request(bsf_str_buf,
                                                &gba_http_global_data_ptr->request_id);
} /* gba_http_send_user_identity_request */


/*===========================================================================
FUNCTION GBA_HTTP_SEND_AUTHENTICATION_SUCCESS

DESCRIPTION
  This function sends the HTTP GET to BSF with success authentication result

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_http_send_authentication_success (
   const gba_res_type                   * res_ptr,
   uint32                                 reference_id,
   gba_bootstrap_callback_type            auth_result_cb
)
{
  ds_http_auth_info_s_type        * auth_info_ptr = NULL;

  if (gba_http_global_data_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  GBA_MSG_HIGH_2("gba_http_send_authentication_success for session_id 0x%x; request id 0x%x",
                 gba_http_session_id.session_id, gba_http_global_data_ptr->request_id);

  if(gba_http_global_data_ptr->reference_id != reference_id ||
     (strnlen(gba_http_global_data_ptr->username, sizeof(gba_http_global_data_ptr->username)) >=
      sizeof(auth_info_ptr->credential.username)) ||
     res_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  auth_info_ptr = GBA_MALLOC(sizeof(ds_http_auth_info_s_type));

  if (auth_info_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  auth_info_ptr->credential.username_len = (uint16)strnlen(gba_http_global_data_ptr->username,
                                              sizeof(gba_http_global_data_ptr->username));

  strlcpy(auth_info_ptr->credential.username,
          gba_http_global_data_ptr->username,
          sizeof(auth_info_ptr->credential.username));

  /* copy RES as credential password RFC 3310 S3 */
  auth_info_ptr->credential.password_len = res_ptr->data_len;
  memscpy(auth_info_ptr->credential.password,
          DS_HTTP_PASSWORD_MAX_LEN,
          res_ptr->data,
          res_ptr->data_len);

  GBA_MSG_HIGH_2("session_id 0x%x; request id 0x%x",
                 gba_http_session_id.session_id, gba_http_global_data_ptr->request_id);

  /* contruct HTTP Cookies */
  gba_http_cookie_get_cookie_header(gba_http_global_data_ptr->impi,
                                    gba_http_global_data_ptr->session_type,
                                    sizeof(auth_info_ptr->cookie)/sizeof(ds_http_cookie_name_s_type),
                                    &auth_info_ptr->num_cookies,
                                    auth_info_ptr->cookie);

  ds_http_resolve_auth_block(gba_http_session_id.session_id,
                             gba_http_global_data_ptr->request_id,
                             auth_info_ptr);

  gba_http_global_data_ptr->callback = auth_result_cb;
  gba_http_global_data_ptr->pending_cmd  = GBA_HTTP_PENDING_200_OK;

  GBA_FREE(auth_info_ptr);

  return GBA_SUCCESS;
} /* gba_http_send_authentication_success */


/*===========================================================================
FUNCTION GBA_HTTP_SEND_AUTHENTICATION_FAILURE

DESCRIPTION
  This function sends the HTTP GET to BSF with authentication sync failure
  or mac failure

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_http_send_authentication_failure (
  const gba_auts_type                  * auts_ptr,
  uint32                                 reference_id,
  gba_bootstrap_callback_type            user_identity_cb
)
{
  char                             * base64_auts_ptr = NULL;
  ds_http_auth_info_s_type         * auth_info_ptr   = NULL;

  if (gba_http_global_data_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  if(gba_http_global_data_ptr->reference_id != reference_id)
  {
    return GBA_INCORRECT_PARAMS;
  }

  GBA_MSG_HIGH_2("gba_http_send_authentication_failure for session_id 0x%x; request id 0x%x",
                 gba_http_session_id.session_id, gba_http_global_data_ptr->request_id);
				 
  gba_http_global_data_ptr->pending_cmd  = GBA_HTTP_PENDING_CMD_NONE;
  /* if auts is null, ie. MAC failure or Generic AUTH failure return NULL as
     crediential */
  if(auts_ptr == NULL)
  {
    ds_http_resolve_auth_block(gba_http_session_id.session_id,
                               gba_http_global_data_ptr->request_id,
                               NULL);
    return GBA_SUCCESS;
  }

  /* for Sync failure, RFC3310 Section 3.4. Set empty string as password and base64 AUTS */
  GBA_MSG_MED_0("send authentication resp for sync failure with auts");

  if (auts_ptr->data_len > sizeof(auts_ptr->data) ||
      (strnlen(gba_http_global_data_ptr->username, sizeof(gba_http_global_data_ptr->username)) >=
       sizeof(auth_info_ptr->credential.username)))
  {
    return GBA_GENERIC_ERROR;
  }

  base64_auts_ptr = gba_util_alloc_bin_to_base64string((const uint8*)auts_ptr->data, auts_ptr->data_len);
  if(base64_auts_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  auth_info_ptr = GBA_MALLOC(sizeof(ds_http_auth_info_s_type));

  if (auth_info_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  auth_info_ptr->credential.username_len = (uint16)strnlen(gba_http_global_data_ptr->username,
                                                           sizeof(gba_http_global_data_ptr->username));
  strlcpy(auth_info_ptr->credential.username,
          gba_http_global_data_ptr->username,
          sizeof(auth_info_ptr->credential.username));

  auth_info_ptr->credential.password_len = 0;
  memscpy(auth_info_ptr->credential.password,
          DS_HTTP_PASSWORD_MAX_LEN,
          GBA_HTTP_EMPTY_STR,
          1);

  strlcpy(auth_info_ptr->cust_auth_param[0].name, GBA_HTTP_AUTS_TAG,
          sizeof(auth_info_ptr->cust_auth_param[0].name));
  strlcat(auth_info_ptr->cust_auth_param[0].value, GBA_HTTP_DOUBLE_QUOTE_STR,
          sizeof(auth_info_ptr->cust_auth_param[0].value));
  strlcat(auth_info_ptr->cust_auth_param[0].value, base64_auts_ptr,
          sizeof(auth_info_ptr->cust_auth_param[0].value));
  strlcat(auth_info_ptr->cust_auth_param[0].value, GBA_HTTP_DOUBLE_QUOTE_STR,
          sizeof(auth_info_ptr->cust_auth_param[0].value));
  auth_info_ptr->num_cust_auth_param = 1;

  /* contruct HTTP Cookies */
  gba_http_cookie_get_cookie_header(gba_http_global_data_ptr->impi,
                                    gba_http_global_data_ptr->session_type,
                                    sizeof(auth_info_ptr->cookie)/sizeof(ds_http_cookie_name_s_type),
                                    &auth_info_ptr->num_cookies,
                                    auth_info_ptr->cookie);

  ds_http_resolve_auth_block(gba_http_session_id.session_id,
                             gba_http_global_data_ptr->request_id,
                             auth_info_ptr);

  /* expect a 2nd 401 from BSF for sync failure */
  gba_http_global_data_ptr->pending_cmd  = GBA_HTTP_PENDING_401_UNAUTH;
  gba_http_global_data_ptr->callback = user_identity_cb;

  GBA_FREE(base64_auts_ptr);
  GBA_FREE(auth_info_ptr);

  return GBA_SUCCESS;
} /* gba_http_send_authentication_failure */

