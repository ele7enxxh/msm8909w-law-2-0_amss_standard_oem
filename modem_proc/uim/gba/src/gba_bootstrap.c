/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   B O O T S T R A P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA functions which handles bootstrap processing.

                        COPYRIGHT INFORMATION

Copyright (c) 2015-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/16   tkl     Added support of DSS network down reason in GBA response
09/23/15   vdc     Do not read EF GBABP while generating TMPI
09/23/15   vdc     Do not send failure resp while generating TMPI for GBA_U
07/20/15   tkl     Added HTTP Cookies support
07/09/15   tkl     Added support for custom fields
07/09/15   tkl     Check EF GBABP for Ks validity
06/26/15   lxu     remove ims. tag in USIM bsf address
06/10/15   tkl     Add support of IPv6 BSF address
05/30/15   tkl     Clear TMPI in storage in case of server error
05/12/15   tkl     Moved request_id up to callback function
05/12/15   tkl     Fixed lifetime expiration check
05/05/15   lxu     Fixed various issues
04/30/15   vdc     Added support for APN for bootstrapping
04/06/15   tl      Add GBA_U support
03/29/15   tkl     Add TMPI support
03/26/15   vdc     Added support to check whether B-TID and Ks is valid
03/25/15   lxu     Use gba_url and gba_port in nv if they are valid
03/20/15   tl      Add support for GBA cancel
03/14/15   vdc     Allow bootstrapping with empty NAF id
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "queue.h"
#include "fs_public.h"
#include <stringl/stringl.h>

#include "gba.h"
#include "gba_bootstrap.h"
#include "gba_cmd.h"
#include "gba_http.h"
#include "gba_key_gen.h"
#include "gba_lib.h"
#include "gba_lifetime.h"
#include "gba_mmgsdi.h"
#include "gba_nv.h"
#include "gba_platform.h"
#include "gba_response.h"
#include "gba_state.h"
#include "gba_storage.h"
#include "gba_tmpi.h"
#include "gba_util.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

#define GBA_IMPI_LABEL_DOMAIN            "3gppnetwork.org"
#define GBA_DOMAIN_NAME_MAX              255
#define GBA_BSF_TAG                      "bsf."
#define GBA_PUB_TAG                      "pub."
#define GBA_PUB_3GPPNETWORK_DOMAIN       "pub.3gppnetwork.org"
#define GBA_IMS_TAG                      "ims."

typedef char gba_domain_name_type[GBA_DOMAIN_NAME_MAX];

typedef struct {
  uint32                                  reference_id;
  gba_naf_id_type                         naf_id;
  gba_session_type                        session_type;
  boolean                                 force_bootstrap;
  gba_aid_type                            aid;
  gba_impi_type                           impi;
  gba_bsf_address_type                    bsf_address;
  gba_bsf_address_type                    realm;
  gba_rand_type                           rand;
  gba_mode_enum_type                      mode;
  boolean                                 support_tmpi;
  gba_apn_type                            apn_info;
} gba_bootstrap_info_type;

static q_type                             gba_bootstrap_cmd_q;

/* Contains information regarding the current bootstrapping procedure. */
static gba_bootstrap_info_type          * gba_bootstrap_info_ptr = NULL;

/* Identifies the next valid unique GBA request ID. */
static uint32                             gba_request_id = GBA_INVALID_REFERENCE_ID;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_BOOTSTRAP_CB

DESCRIPTION
  Function allows a re-entry point for other modules to communicate with
  the GBA task and respond.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_bootstrap_cb (
  gba_result_enum_type                      status,
  const gba_bootstrap_callback_data_type  * resp_ptr,
  uint32                                    reference_id
)
{
  gba_bootstrap_cmd_type  * task_cmd_ptr = NULL;

  GBA_MSG_LOW_1 ("Received gba_bootstrap_cb status: 0x%x", status);

  task_cmd_ptr = (gba_bootstrap_cmd_type*)GBA_MALLOC(sizeof(gba_bootstrap_cmd_type));
  if (task_cmd_ptr == NULL)
  {
    return;
  }

  task_cmd_ptr->reference_id       = reference_id;
  task_cmd_ptr->status             = status;

  if(status == GBA_SUCCESS || status == GBA_SERVER_ERROR ||
     status == GBA_NETWORK_ERROR)
  {
    if (resp_ptr != NULL)
    {
      task_cmd_ptr->cmd_data = *resp_ptr;
    }
    else
    {
      task_cmd_ptr->status = GBA_GENERIC_ERROR;
    }
  }

  if (gba_queue_bootstrap_cmd(task_cmd_ptr) != GBA_SUCCESS)
  {
    GBA_FREE(task_cmd_ptr);
  }
} /* gba_bootstrap_cb */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_VALIDATE_CACHED_KS

DESCRIPTION
  Function checks if there is a valid KS stored in the SFS.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
static boolean gba_bootstrap_validate_cached_ks (
  const gba_identity_data_type          * card_identity_ptr,
  const gba_storage_profile_data_type   * identity_profile_ptr
)
{
  if(card_identity_ptr == NULL ||
     identity_profile_ptr == NULL)
  {
    return FALSE;
  }

  if((card_identity_ptr->session_type ==  GBA_NON_PROV_SESSION_SLOT_1 ||
      card_identity_ptr->session_type ==  GBA_NON_PROV_SESSION_SLOT_2) &&
     (card_identity_ptr->aid.data_len > GBA_MAX_AID_LEN ||
      card_identity_ptr->aid.data_len != identity_profile_ptr->aid.data_len ||
      memcmp(card_identity_ptr->aid.data,
             identity_profile_ptr->aid.data,
             card_identity_ptr->aid.data_len) != 0))
  {
    return FALSE;
  }

  if(card_identity_ptr->impi.data_len > GBA_MAX_IMPI_LEN ||
     card_identity_ptr->impi.data_len != identity_profile_ptr->impi.data_len ||
     memcmp(card_identity_ptr->impi.data,
            identity_profile_ptr->impi.data,
            card_identity_ptr->impi.data_len) != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* gba_bootstrap_validate_cached_ks */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_BUILD_AND_SEND_RESPONSE

DESCRIPTION
  Function builds the bootstrap response including the KS NAF, request id,
  IMPI, B-TID, and lifetime and sends that information back to the client.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_build_and_send_response (
  const gba_ks_naf_type                * ks_naf_ptr,
  const gba_storage_profile_data_type  * profile_data_ptr
)
{
  gba_response_data_type            * bootstrap_resp_ptr   = NULL;

  if(profile_data_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  bootstrap_resp_ptr = GBA_MALLOC(sizeof(gba_response_data_type));
  if(bootstrap_resp_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  bootstrap_resp_ptr->resp_data.success.btid     = profile_data_ptr->btid;
  bootstrap_resp_ptr->resp_data.success.lifetime = profile_data_ptr->lifetime;
  bootstrap_resp_ptr->resp_data.success.impi     = profile_data_ptr->impi;

  if(ks_naf_ptr != NULL)
  {
    memscpy(bootstrap_resp_ptr->resp_data.success.ks_naf,
            sizeof(bootstrap_resp_ptr->resp_data.success.ks_naf),
            ks_naf_ptr,
            sizeof(gba_ks_naf_type));
  }

  gba_send_response(GBA_SUCCESS,
                    bootstrap_resp_ptr);

  GBA_FREE(bootstrap_resp_ptr);

  (void)gba_state_change(gba_state_current(), GBA_STATE_DONE);

  return GBA_SUCCESS;
} /* gba_bootstrap_build_and_send_response */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_BUILD_AND_SEND_ERROR_RESPONSE

DESCRIPTION
  Function builds the bootstrap error response including corresponding
  error code to the client.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_build_and_send_error_response (
  gba_result_enum_type                      response_status,
  const gba_bootstrap_callback_data_type  * cmd_data_ptr
)
{
  gba_response_data_type                  * bootstrap_resp_ptr   = NULL;
  const gba_http_response_error_data_type * resp_error_ptr       = NULL;
  gba_state_enum_type                       gba_state            = GBA_STATE_IDLE;
  gba_result_enum_type                      gba_status           = GBA_SUCCESS;

  if(cmd_data_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  /* clear TMPI in storage if available */
  (void)gba_tmpi_clear_tmpi_storage(gba_bootstrap_info_ptr->session_type);
  gba_state = gba_state_current();

  switch(gba_state)
  {
    case GBA_STATE_INITIAL_HTTP:
      resp_error_ptr = &cmd_data_ptr->data.auth.error_data;
      break;
    case GBA_STATE_FINAL_HTTP:
      resp_error_ptr = &cmd_data_ptr->data.http_200_ok.error_data;
      break;
    default:
      /* unexpected gba_state for GBA_SERVER_ERROR */
      break;
  }

  if(resp_error_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  bootstrap_resp_ptr = GBA_MALLOC(sizeof(gba_response_data_type));
  if(bootstrap_resp_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  
  switch(response_status)
  {
    case GBA_SERVER_ERROR:
      bootstrap_resp_ptr->resp_data.server_error.error_code_present = resp_error_ptr->data.server_error.http_status_present;
      bootstrap_resp_ptr->resp_data.server_error.error_code = resp_error_ptr->data.server_error.http_status;
      break;
    case GBA_NETWORK_ERROR:
      bootstrap_resp_ptr->resp_data.network_error.netdown_reason_present = resp_error_ptr->data.network_error.netdown_reason_present;
      bootstrap_resp_ptr->resp_data.network_error.netdown_reason = resp_error_ptr->data.network_error.netdown_reason;
      break;
    default:
      /* unexpected GBA error status */
      gba_status = GBA_GENERIC_ERROR;
      break;
  }

  if(gba_status == GBA_SUCCESS)
  {
    gba_send_response(response_status, bootstrap_resp_ptr);
    (void)gba_state_change(gba_state, GBA_STATE_DONE);
  }

  GBA_FREE(bootstrap_resp_ptr);

  return gba_status;
} /* gba_bootstrap_build_and_send_error_response */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_GET_DOMAIN_NAME_FROM_IMPI

DESCRIPTION
  Function takes IMPI data as input and constructs a valid domain name.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_get_domain_name_from_impi(
  const gba_impi_type               * impi_ptr,
  gba_domain_name_type              * domain_name_ptr
)
{
  int i = 0;

  if(impi_ptr == NULL ||
     domain_name_ptr == NULL ||
     impi_ptr->data_len == 0 ||
     impi_ptr->data_len > sizeof(impi_ptr->data))
  {
    return GBA_GENERIC_ERROR;
  }

  for(i = 0; i < impi_ptr->data_len; i++)
  {
    if(impi_ptr->data[i] == '@' &&
       (i+1) < impi_ptr->data_len)
    {
      (void)memscpy(*domain_name_ptr,
                    sizeof(*domain_name_ptr) - 1,
                    &impi_ptr->data[i+1],
                    impi_ptr->data_len - i);

      return GBA_SUCCESS;
    }
  }

  return GBA_GENERIC_ERROR;
} /* gba_bootstrap_get_domain_name_from_impi */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_IS_DOMAIN_3GPPNETWORK

DESCRIPTION
  Function determines if the input is a "3gppnetwork" domain name.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
static boolean gba_bootstrap_is_domain_3gppnetwork(
  const gba_domain_name_type              * domain_name_ptr
)
{
  uint32 domain_len = 0;

  if(domain_name_ptr == NULL ||
     strlen(*domain_name_ptr) < strlen(GBA_IMPI_LABEL_DOMAIN))
  {
    return FALSE;
  }

  domain_len = strnlen(*domain_name_ptr, sizeof(gba_domain_name_type));

  if(memcmp(&(*domain_name_ptr)[domain_len - strlen(GBA_IMPI_LABEL_DOMAIN)],
            GBA_IMPI_LABEL_DOMAIN,
            strlen(GBA_IMPI_LABEL_DOMAIN)) == 0)
  {
    return TRUE;
  }

  return FALSE;
} /* gba_bootstrap_is_domain_3gppnetwork */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_IS_IMS_TAG

DESCRIPTION
  Function determines if the input has ims. tag.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
static boolean gba_bootstrap_is_ims_tag(
  const gba_domain_name_type              * domain_name_ptr
)
{
  if(domain_name_ptr == NULL ||
     strnlen(*domain_name_ptr, sizeof(gba_domain_name_type)) < strlen(GBA_IMS_TAG))
  {
    return FALSE;
  }

  if(memcmp(*domain_name_ptr,
            GBA_IMS_TAG,
            strlen(GBA_IMS_TAG)) == 0)
  {
    return TRUE;
  }

  return FALSE;
} /* gba_bootstrap_is_ims_tag */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_BUILD_BSF_ADDRESS_FROM_IMPI

DESCRIPTION
  Function constructs a valid BSF address using the IMPI as input.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_build_bsf_address_from_impi (
  const gba_impi_type               * impi_ptr,
  gba_bsf_address_type              * bsf_address_ptr,
  gba_session_type                    session_type
)
{
  gba_result_enum_type    gba_status         = GBA_SUCCESS;
  boolean                 domain_3gppnetwork = FALSE;
  gba_domain_name_type  * domain_name_ptr    = NULL;

  if(impi_ptr == NULL ||
     bsf_address_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  memset(bsf_address_ptr, 0x00, sizeof(gba_bsf_address_type));

  domain_name_ptr = GBA_MALLOC(sizeof(gba_domain_name_type));
  if(domain_name_ptr == NULL){
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  memset(domain_name_ptr, 0x00, sizeof(gba_domain_name_type));

  gba_status = gba_bootstrap_get_domain_name_from_impi(impi_ptr, domain_name_ptr);
  if(gba_status != GBA_SUCCESS)
  {
    GBA_FREE(domain_name_ptr);
    return gba_status;
  }

  domain_3gppnetwork = gba_bootstrap_is_domain_3gppnetwork(domain_name_ptr);

  strlcpy(bsf_address_ptr->data, GBA_BSF_TAG, sizeof(bsf_address_ptr->data));

  /* For ISIM, The BSF address constructed from the IMPI is contingent on the last two
     labels (as described in TS 23.003):

     If the last two labels of the domain name extracted from the IMPI are
     "3gppnetwork.org":

     The first label is "bsf", the next labels are all labels of the domain name
     extracted from the IMPI apart from the last two labels and the last three
     labels are "pub.3gppnetwork.org".

     ex. If the IMPI in use is "234150999999999@ims.mnc015.mcc234.3gppnetwork.org",
         the BSF address would be "bsf.ims.mnc015.mcc234.pub.3gppnetwork.org".

     If the last two labels of the domain name extracted from the IMPI is
     NOT "3gppnetwork.org":

     The BSF address will just be the domain name prepended with "bsf.".

     ex. If the IMPI in use is "user@operator.com", the BSF address would be
         "bsf.operator.com ".
     */
  if(domain_3gppnetwork)
  {
    if((sizeof(bsf_address_ptr->data) - strnlen(bsf_address_ptr->data, sizeof(bsf_address_ptr->data)) - 1) <
       (strnlen(*domain_name_ptr, sizeof(gba_domain_name_type)) + strlen(GBA_PUB_TAG)))
    {
      GBA_FREE(domain_name_ptr);
      return GBA_GENERIC_ERROR;
    }

    /* For USIM, If IMSI in use is "234150999999999", where MCC=234, MNC=15, and MSIN=0999999999,
       the BSF address would be "bsf.mnc015.mcc234.pub.3gppnetwork.org".
    */
    if (gba_bootstrap_is_ims_tag(domain_name_ptr) &&
        (session_type == GBA_3GPP_PROV_SESSION_PRI ||
          session_type == GBA_3GPP_PROV_SESSION_SEC))
    {
      strlcat(bsf_address_ptr->data,
              *domain_name_ptr + strlen(GBA_IMS_TAG),
              (strlen(GBA_BSF_TAG) +
               strnlen(*domain_name_ptr, sizeof(gba_domain_name_type)) -
               strlen(GBA_IMPI_LABEL_DOMAIN) -
               strlen(GBA_IMS_TAG) + 1));
    }
    else
    {
      strlcat(bsf_address_ptr->data,
              *domain_name_ptr,
              (strlen(GBA_BSF_TAG) +
               strnlen(*domain_name_ptr, sizeof(gba_domain_name_type)) -
               strlen(GBA_IMPI_LABEL_DOMAIN) + 1));
    }
    strlcat(bsf_address_ptr->data,
            GBA_PUB_3GPPNETWORK_DOMAIN,
            sizeof(bsf_address_ptr->data));
  }
  else
  {
    strlcat(bsf_address_ptr->data,
            *domain_name_ptr,
            sizeof(bsf_address_ptr->data));
  }

  GBA_FREE(domain_name_ptr);

  bsf_address_ptr->data_len = (uint16)strnlen(bsf_address_ptr->data, sizeof(bsf_address_ptr->data));

  return GBA_SUCCESS;
} /* gba_bootstrap_build_bsf_address_from_impi */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_BUILD_AND_SEND_INITIAL_HTTP

DESCRIPTION
  Function constructs the initial HTTP request message and send the data
  to the GBA HTTP module for parsing further processing.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_build_and_send_initial_http (
  gba_identity_data_type          * card_identity_ptr
)
{
  gba_result_enum_type      gba_status      = GBA_SUCCESS;
  gba_impi_type           * tmpi_ptr        = NULL;
  uint16                    port            = 0;

  if(card_identity_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_bootstrap_info_ptr->impi = card_identity_ptr->impi;

  /* Construct the BSF address from NV or from data in the card. */
  if(GBA_SUCCESS != gba_nv_get_bsf_url(
                      &gba_bootstrap_info_ptr->bsf_address))
  {
    gba_status = gba_bootstrap_build_bsf_address_from_impi(
                   &card_identity_ptr->impi,
                   &gba_bootstrap_info_ptr->bsf_address,
                   gba_bootstrap_info_ptr->session_type);
    if(gba_status != GBA_SUCCESS)
    {
      return gba_status;
    }
  }

  /* keep the realm for identity and naf generation */
  switch(gba_nv_get_custom_realm())
  {
    case GBA_NV_REALM_OMIT_BSF:
    {
      uint8   offset = 0;
      if(memcmp(gba_bootstrap_info_ptr->bsf_address.data,
                GBA_BSF_TAG,
                strlen(GBA_BSF_TAG)) == 0)
      {
        /* skip copying "bsf." to realm from bsf fqdn*/
        offset = strlen(GBA_BSF_TAG);
      }

      gba_bootstrap_info_ptr->realm.data_len =
         gba_bootstrap_info_ptr->bsf_address.data_len - offset;

      (void)memscpy(gba_bootstrap_info_ptr->realm.data,
                    sizeof(gba_bootstrap_info_ptr->realm.data),
                    &gba_bootstrap_info_ptr->bsf_address.data[offset],
                    gba_bootstrap_info_ptr->bsf_address.data_len - offset);
      break;
    }

    case GBA_NV_REALM_PER_SPEC:
    default:
      (void)memscpy(&gba_bootstrap_info_ptr->realm,
                    sizeof(gba_bootstrap_info_ptr->realm.data),
                    &gba_bootstrap_info_ptr->bsf_address,
                    sizeof(gba_bootstrap_info_ptr->bsf_address));
      break;
  }

  /* Add port to BSF address, if needed. */
  if (GBA_SUCCESS == gba_nv_get_bsf_port(&port))
  {
    char  bsf_port[6] = {'\0'};

    if (GBA_SUCCESS == gba_util_itoa(port, bsf_port, sizeof(bsf_port)))
    {
      if (GBA_MAX_BSF_ADDRESS_LEN - gba_bootstrap_info_ptr->bsf_address.data_len >=
            1 + (uint8)strnlen(bsf_port, sizeof(bsf_port)))
      {
        strlcat(gba_bootstrap_info_ptr->bsf_address.data, ":",
                sizeof(gba_bootstrap_info_ptr->bsf_address.data));
        gba_bootstrap_info_ptr->bsf_address.data_len++;
        strlcat(gba_bootstrap_info_ptr->bsf_address.data, bsf_port,
                sizeof(gba_bootstrap_info_ptr->bsf_address.data));
        gba_bootstrap_info_ptr->bsf_address.data_len +=
          (uint16)strnlen(bsf_port, sizeof(bsf_port));
      }
      else
      {
        return GBA_GENERIC_ERROR;
      }
    }
    else
    {
      return GBA_GENERIC_ERROR;
    }
  }

  GBA_MSG_SPRINTF_1("gba_bootstrap_build_bsf_address_from_impi : %s",
                    gba_bootstrap_info_ptr->bsf_address.data);

  tmpi_ptr = (gba_impi_type *)GBA_MALLOC(sizeof(gba_impi_type));
  /* If TMPI allocation fails then just proceed with IMPI instead. */
  if(tmpi_ptr != NULL)
  {
    /* if pass in session type, then can get tmpi in gba_http */
    (void)gba_tmpi_get_impi_tmpi_pair(gba_bootstrap_info_ptr->session_type,
                                      &gba_bootstrap_info_ptr->impi,
                                      tmpi_ptr);
  }

  gba_status = gba_http_send_user_identity_request(&gba_bootstrap_info_ptr->impi,
                                                   tmpi_ptr,
                                                   &gba_bootstrap_info_ptr->bsf_address,
                                                   &gba_bootstrap_info_ptr->realm,
                                                   &gba_bootstrap_info_ptr->apn_info,
                                                   gba_bootstrap_info_ptr->session_type,
                                                   gba_bootstrap_info_ptr->reference_id,
                                                   gba_bootstrap_cb);

  GBA_FREE(tmpi_ptr);
  return gba_status;
} /* gba_bootstrap_build_and_send_initial_http */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_RESET

DESCRIPTION
  Function resets the global bootstrap data.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_reset (
  void
)
{
  GBA_FREE(gba_bootstrap_info_ptr);
} /* gba_bootstrap_reset */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_PROCESS_GEN_TMPI

DESCRIPTION
  This function processes the TMPI generation for GBA_U bootstrap command for
  use in subsequent bootstrap procedures.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void gba_bootstrap_process_gen_tmpi (
  const gba_ks_naf_type              * tmpi_ks_naf_ptr
)
{
  gba_result_enum_type             gba_status              = GBA_SUCCESS;
  gba_storage_profile_data_type  * identity_profile_ptr    = NULL;
  gba_impi_type                  * tmpi_ptr                = NULL;

  if(tmpi_ks_naf_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL)
  {
    goto cleanup;
  }

  identity_profile_ptr = gba_storage_get_profile(gba_bootstrap_info_ptr->session_type);
  if(identity_profile_ptr == NULL)
  {
    goto cleanup;
  }

  tmpi_ptr =
    (gba_impi_type *)GBA_MALLOC(sizeof(gba_impi_type));
  if(tmpi_ptr == NULL)
  {
    goto cleanup;
  }

  gba_status = gba_tmpi_calculate_tmpi(tmpi_ks_naf_ptr,
                                       tmpi_ptr);
  if(gba_status != GBA_SUCCESS)
  {
    goto cleanup;
  }

  GBA_MSG_SPRINTF_1("tmpi : %s", tmpi_ptr->data);

  identity_profile_ptr->tmpi = *tmpi_ptr;

  (void)gba_storage_put_profile(gba_bootstrap_info_ptr->session_type,
                                identity_profile_ptr);

cleanup:
  if(tmpi_ptr != NULL)
  {
    GBA_FREE(tmpi_ptr);
  }

  if(identity_profile_ptr != NULL)
  {
    gba_storage_free_profile(&identity_profile_ptr);
  }

  /* Return to GBA_STATE_DONE so that clean-up and state reset is done
     in the calling function. */
  (void)gba_state_change(gba_state_current(), GBA_STATE_DONE);
} /* gba_bootstrap_process_gen_tmpi */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_PROCESS_GEN_KS_EXT_NAF

DESCRIPTION
  This function processes the Ks ext naf generation message from the UICC and
  proceeds to either generate the BSF key for TMPI generation if TMPI is supported
  by the BSF or sends the status and data of the GBA_U bootstrap proceedure to the
  client.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_process_gen_ks_ext_naf (
  const gba_ks_naf_type              * ks_ext_naf_ptr
)
{
  gba_result_enum_type             gba_status              = GBA_SUCCESS;
  gba_storage_profile_data_type  * identity_profile_ptr    = NULL;

  if(ks_ext_naf_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  identity_profile_ptr = gba_storage_get_profile(gba_bootstrap_info_ptr->session_type);
  if(identity_profile_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_status = gba_bootstrap_build_and_send_response(ks_ext_naf_ptr,
                                                     identity_profile_ptr);

  /* If the BSF supports TMPI functionality, then proceed to generate TMPI for use
     in future GBA bootstrapping procedures to protect the IMPI. If there is any
     failure in generating TMPI then return success so that GBA does not return
     the response again for GBA bootstrapping request */
  if(gba_bootstrap_info_ptr->support_tmpi)
  {
    gba_naf_id_type   * bsf_id_ptr   = NULL;

    bsf_id_ptr = (gba_naf_id_type *)GBA_MALLOC(sizeof(gba_naf_id_type));
    if(bsf_id_ptr)
    {
      if(GBA_SUCCESS == gba_tmpi_get_bsf_id(&gba_bootstrap_info_ptr->realm,
                                            bsf_id_ptr))
      {
        if(GBA_SUCCESS == gba_mmgsdi_generate_ks_ext_naf(
                            gba_bootstrap_info_ptr->reference_id,
                            gba_bootstrap_cb,
                            bsf_id_ptr,
                            &identity_profile_ptr->impi,
                            &identity_profile_ptr->btid,
                            &identity_profile_ptr->lifetime,
                            TRUE))
        {
          (void)gba_state_change(gba_state_current(), GBA_STATE_GEN_TMPI);
        }
      }

      GBA_FREE(bsf_id_ptr);
    }
  }

  gba_storage_free_profile(&identity_profile_ptr);

  return gba_status;
} /* gba_bootstrap_process_gen_ks_ext_naf */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_PROCESS_200_OK

DESCRIPTION
  This function processes the 200 OK message from the BSF and proceeds
  to send the status and data to the client.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_process_200_ok (
  gba_http_200_ok_data_type          * http_200_ok_ptr
)
{
  gba_result_enum_type             gba_status              = GBA_SUCCESS;
  gba_storage_profile_data_type  * identity_profile_ptr    = NULL;

  if(http_200_ok_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  identity_profile_ptr = gba_storage_get_profile(gba_bootstrap_info_ptr->session_type);
  if(identity_profile_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  identity_profile_ptr->btid = http_200_ok_ptr->btid;
  identity_profile_ptr->lifetime = http_200_ok_ptr->lifetime;
  /* BSF has generated a new TMPI upon 200 0k, clear stale TMPI in storage */
  memset(&identity_profile_ptr->tmpi, 0x00, sizeof(gba_impi_type));

  (void)gba_storage_put_profile(gba_bootstrap_info_ptr->session_type,
                        identity_profile_ptr);

  if(gba_bootstrap_info_ptr->mode == GBA_ME)
  {
    gba_ks_naf_type ks_naf;

    memset(ks_naf, 0x00, sizeof(ks_naf));

    /* Calculation of Ks_NAF is not required when FQDN is empty */
    if(gba_bootstrap_info_ptr->naf_id.fqdn.data_len > 0)
    {
      gba_status = gba_key_gen_ksnaf(&identity_profile_ptr->ks,
                                     &identity_profile_ptr->rand,
                                     &identity_profile_ptr->impi,
                                     &gba_bootstrap_info_ptr->naf_id,
                                     &ks_naf);
      if(gba_status != GBA_SUCCESS)
      {
        goto cleanup;
      }
    }

    gba_status = gba_bootstrap_build_and_send_response(&ks_naf,
                                                       identity_profile_ptr);
  }
  else if(gba_bootstrap_info_ptr->mode == GBA_U)
  {
    /* Only generate KS ext NAF when NAF ID is present */
    if(gba_bootstrap_info_ptr->naf_id.fqdn.data_len != 0)
    {
      gba_status = gba_mmgsdi_generate_ks_ext_naf(gba_bootstrap_info_ptr->reference_id,
                                                  gba_bootstrap_cb,
                                                  &gba_bootstrap_info_ptr->naf_id,
                                                  &identity_profile_ptr->impi,
                                                  &identity_profile_ptr->btid,
                                                  &identity_profile_ptr->lifetime,
                                                  FALSE);
      if(gba_status == GBA_SUCCESS)
      {
        (void)gba_state_change(gba_state_current(), GBA_STATE_GEN_KS_EXT_NAF);
      }
    }
    else
    {
      /* If NAF ID is not present then response can now be returned to clients
         now and TMPI will not be generated. */
      gba_status = gba_bootstrap_build_and_send_response(NULL,
                                                         identity_profile_ptr);

      goto cleanup;
    }
  }

  if(gba_status != GBA_SUCCESS)
  {
    goto cleanup;
  }

  /* If the BSF supports TMPI functionality, then proceed to generate TMPI for use
     in future GBA bootstrapping procedures to protect the IMPI. For GBA_U the
     TMPI will be generated after Ks ext NAF is generated so it cannot be performed
     here. */
  if(http_200_ok_ptr->support_tmpi &&
     gba_bootstrap_info_ptr->mode == GBA_ME)
  {
    gba_naf_id_type    * bsf_id_ptr = NULL;

    bsf_id_ptr = GBA_MALLOC(sizeof(gba_naf_id_type));
    if(bsf_id_ptr == NULL)
    {
      gba_status = GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
      goto cleanup;
    }

    /* get BSF ID from BSF address */
    gba_status = gba_tmpi_get_bsf_id(&gba_bootstrap_info_ptr->realm,
                                     bsf_id_ptr);

    if(gba_status == GBA_SUCCESS)
    {
      gba_status = gba_tmpi_compute_for_gba_me(&identity_profile_ptr->ks,
                                               &identity_profile_ptr->impi,
                                               &identity_profile_ptr->rand,
                                               bsf_id_ptr,
                                               &identity_profile_ptr->tmpi);
      if(gba_status == GBA_SUCCESS)
      {
        (void)gba_storage_put_profile(gba_bootstrap_info_ptr->session_type,
                              identity_profile_ptr);
      }
    }

    GBA_FREE(bsf_id_ptr);
  }

  gba_bootstrap_info_ptr->support_tmpi = http_200_ok_ptr->support_tmpi;

cleanup:
  gba_storage_free_profile(&identity_profile_ptr);

  return gba_status;
} /* gba_bootstrap_process_200_ok */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_PROCESS_RESULT_AUTHORIZATION

DESCRIPTION
  This function processes the authentication result from MMGSDI and
  determines how to proceed depending on if the authentication is a
  success or failure.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_process_result_authorization (
  gba_auth_status_data_type          * auth_status_ptr
)
{
  gba_result_enum_type            gba_status           = GBA_SUCCESS;
  gba_storage_profile_data_type * identity_profile_ptr = NULL;
  gba_res_type                  * res_ptr              = NULL;

  if(auth_status_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  if(auth_status_ptr->result == GBA_AUTH_SYNC_FAILURE)
  {
    gba_status = gba_state_change(GBA_STATE_UICC_AUTHENTICATION, GBA_STATE_INITIAL_HTTP);
    if(gba_status != GBA_SUCCESS)
    {
      return gba_status;
    }

    return gba_http_send_authentication_failure(&auth_status_ptr->data.auth_sync_failure.auts,
                                                gba_bootstrap_info_ptr->reference_id,
                                                gba_bootstrap_cb);
  }
  else if(auth_status_ptr->result != GBA_AUTH_SUCCESS)
  {
    (void)gba_http_send_authentication_failure(NULL,
                                               gba_bootstrap_info_ptr->reference_id,
                                               gba_bootstrap_cb);

    return GBA_AUTH_FAILURE;
  }

  identity_profile_ptr =
    (gba_storage_profile_data_type *)GBA_MALLOC(sizeof(gba_storage_profile_data_type));
  if(identity_profile_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  if(auth_status_ptr->data.auth_success.auth_mode == GBA_AUTH_CONTEXT_3G_AKA)
  {
    gba_status = gba_key_gen_ks(&auth_status_ptr->data.auth_success.auth_rsp_data.auth_3g_rsp.ik,
                                &auth_status_ptr->data.auth_success.auth_rsp_data.auth_3g_rsp.ck,
                                &identity_profile_ptr->ks);
    if(gba_status != GBA_SUCCESS)
    {
      gba_storage_free_profile(&identity_profile_ptr);
      return gba_status;
    }

    res_ptr = &auth_status_ptr->data.auth_success.auth_rsp_data.auth_3g_rsp.res;
  }
  else if(auth_status_ptr->data.auth_success.auth_mode == GBA_AUTH_CONTEXT_GBA_BOOTSTRAP)
  {
    res_ptr = &auth_status_ptr->data.auth_success.auth_rsp_data.auth_gba_bootstrap_rsp;
  }
  else
  {
    GBA_FREE(identity_profile_ptr);
    return GBA_GENERIC_ERROR;
  }

  identity_profile_ptr->aid = gba_bootstrap_info_ptr->aid;
  identity_profile_ptr->impi = gba_bootstrap_info_ptr->impi;
  (void)memscpy(identity_profile_ptr->rand,
                GBA_RAND_LEN,
                gba_bootstrap_info_ptr->rand,
                GBA_RAND_LEN);

  (void)gba_storage_put_profile(auth_status_ptr->session_type,
                                identity_profile_ptr);

  gba_storage_free_profile(&identity_profile_ptr);

  gba_status = gba_state_change(GBA_STATE_UICC_AUTHENTICATION, GBA_STATE_FINAL_HTTP);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  return gba_http_send_authentication_success(res_ptr,
                                              gba_bootstrap_info_ptr->reference_id,
                                              gba_bootstrap_cb);
} /* gba_bootstrap_process_result_authorization */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_PROCESS_AUTHENTICATION

DESCRIPTION
  This function processes the authentication command by triggering the
  SIM authentication using the RAND and AUTN generated from the BSF.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_bootstrap_process_authentication (
  gba_http_authenticate_data_type    * auth_data_ptr
)
{
  gba_result_enum_type             gba_status = GBA_SUCCESS;
  gba_auth_context_enum_type       auth_mode  = GBA_AUTH_CONTEXT_3G_AKA;

  if(auth_data_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  (void)memscpy(gba_bootstrap_info_ptr->rand,
                GBA_RAND_LEN,
                auth_data_ptr->rand,
                GBA_RAND_LEN);

  gba_status = gba_state_change(GBA_STATE_INITIAL_HTTP, GBA_STATE_UICC_AUTHENTICATION);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  switch(gba_bootstrap_info_ptr->mode)
  {
    case GBA_ME:
      auth_mode = GBA_AUTH_CONTEXT_3G_AKA;
      break;
     case GBA_U:
       auth_mode = GBA_AUTH_CONTEXT_GBA_BOOTSTRAP;
       break;
     default:
       return GBA_GENERIC_ERROR;
  }

  return gba_mmgsdi_auth(&auth_data_ptr->auth_challenge,
                         gba_bootstrap_cb,
                         gba_bootstrap_info_ptr->reference_id,
                         auth_mode);
} /* gba_bootstrap_process_authentication */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_IS_IMPI_VALID

DESCRIPTION
  This fuction verifies that the IMPI retrieved from MMGSDI is the correct
  size and does not contain any invalid characters.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
static boolean gba_bootstrap_is_impi_valid (
  const gba_impi_type  * impi_ptr
)
{
  boolean  at_sign_present  = FALSE;
  uint16   i                = 0;

  if(impi_ptr == NULL)
  {
    return FALSE;
  }

  if(impi_ptr->data_len < GBA_MIN_IMPI_LEN ||
     impi_ptr->data_len > GBA_MAX_IMPI_LEN)
  {
    return FALSE;
  }

  /* The IMPI cannot start or end with the '@' character */
  if(impi_ptr->data[0] == '@' ||
     impi_ptr->data[impi_ptr->data_len - 1] == '@')
  {
    return FALSE;
  }

  for(i = 1; i < (impi_ptr->data_len - 1); i++)
  {
    if(impi_ptr->data[i] == '@')
    {
      if(at_sign_present)
      {
        /* Too many '@' characters present in IMPI */
        return FALSE;
      }

      at_sign_present = TRUE;
    }
  }

  return at_sign_present;
} /* gba_bootstrap_is_impi_valid */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_PROCESS_IDENTITY_VALIDATION

DESCRIPTION
  This function process the application identity information returned
  from MMGSDI. If the application is already stored in cache then directly
  construct and send the Ks_NAF to the client. If not, then construct and
  send the initial http request to the BSF server.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_process_identity_validation (
  gba_identity_data_type          * card_identity_ptr
)
{
  gba_result_enum_type              gba_status              = GBA_SUCCESS;

  if(card_identity_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  if(!gba_bootstrap_is_impi_valid(&card_identity_ptr->impi))
  {
    GBA_MSG_ERR_0("Invalid IMPI value");
    return GBA_GENERIC_ERROR;
  }

  /* If we force bootstrapping, do not check storage for valid Ks,
     just proceed with bootstrapping procedure. */
  if(!gba_bootstrap_info_ptr->force_bootstrap)
  {
    gba_storage_profile_data_type   * identity_profile_ptr = NULL;

    do
    {
      identity_profile_ptr = gba_storage_get_profile(gba_bootstrap_info_ptr->session_type);
      if(identity_profile_ptr == NULL)
      {
        if(card_identity_ptr->mode == GBA_U)
        {
          /* Since Ks is persistent over power cycle for GBA U, we need to read from EF GBABP
             for BTID and Lifetime to check the Ks validity, if Ks is still valid, then generate
             Ks NAF and return to client */
          gba_bootstrap_info_ptr->aid  = card_identity_ptr->aid;
          gba_bootstrap_info_ptr->impi = card_identity_ptr->impi;
          gba_bootstrap_info_ptr->mode = card_identity_ptr->mode;

          gba_status = gba_mmgsdi_read_ef_gbabp(gba_bootstrap_info_ptr->reference_id,
                                                gba_bootstrap_cb);

          /* change state and wait for GBABP data from GBA MMGSDI if read request
             is queued successfully */
          if(gba_status == GBA_SUCCESS)
          {
            (void)gba_state_change(gba_state_current(), GBA_STATE_GBABP);
          }

          return gba_status;
        }
        else
        {
          gba_status = GBA_GENERIC_ERROR;
          break;
        }
      }

      /* If cached bootstrap identity is still valid and the lifetime has not
         expired, use the cached Ks to generate Ks NAF and return to the client. */
      if(gba_bootstrap_validate_cached_ks(card_identity_ptr,
                                          identity_profile_ptr) &&
         !gba_lifetime_is_ks_expired(identity_profile_ptr->lifetime))
      {
        if(card_identity_ptr->mode == GBA_ME)
        {
          gba_ks_naf_type ks_naf;

          memset(ks_naf, 0x00, sizeof(ks_naf));

          gba_status = gba_key_gen_ksnaf(&identity_profile_ptr->ks,
                                         &identity_profile_ptr->rand,
                                         &identity_profile_ptr->impi,
                                         &gba_bootstrap_info_ptr->naf_id,
                                         &ks_naf);
          if(gba_status != GBA_SUCCESS)
          {
            gba_storage_free_profile(&identity_profile_ptr);
            break;
          }

          /* A valid Ks is present in the cache and can be used to construct the Ks NAF */
          gba_status = gba_bootstrap_build_and_send_response(&ks_naf,
                                                             identity_profile_ptr);
        }
        else if(card_identity_ptr->mode == GBA_U)
        {
          /* Only generate KS ext NAF when NAF ID is present */
          if(gba_bootstrap_info_ptr->naf_id.fqdn.data_len != 0)
          {
            gba_status = gba_mmgsdi_generate_ks_ext_naf(gba_bootstrap_info_ptr->reference_id,
                                                        gba_bootstrap_cb,
                                                        &gba_bootstrap_info_ptr->naf_id,
                                                        &identity_profile_ptr->impi,
                                                        &identity_profile_ptr->btid,
                                                        &identity_profile_ptr->lifetime,
                                                        FALSE);
            if(gba_status == GBA_SUCCESS)
            {
              (void)gba_state_change(gba_state_current(), GBA_STATE_GEN_KS_EXT_NAF);
            }
          }
          else
          {
            /* If NAF ID is not present then response can now be returned to clients
               now and TMPI will not be generated. */
            gba_status = gba_bootstrap_build_and_send_response(NULL,
                                                               identity_profile_ptr);
          }
        }
      }
      else
      {
        /* invalid SFS storage or lifetime expired.
           Setting gba_status to ERROR so that function continues with sending
           the HTTP request */
        gba_status = GBA_GENERIC_ERROR;
      }

      gba_storage_free_profile(&identity_profile_ptr);
    } while(0);

    if(gba_status == GBA_SUCCESS)
    {
      return GBA_SUCCESS;
    }
  }

  gba_bootstrap_info_ptr->aid = card_identity_ptr->aid;
  gba_bootstrap_info_ptr->impi = card_identity_ptr->impi;
  gba_bootstrap_info_ptr->mode = card_identity_ptr->mode;

  gba_status = gba_state_change(GBA_STATE_IMPI, GBA_STATE_INITIAL_HTTP);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  return gba_bootstrap_build_and_send_initial_http(card_identity_ptr);
} /* gba_bootstrap_process_identity_validation */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_PROCESS_KS_VALIDITY_DATA

DESCRIPTION
  This function process the ks validity data returned from MMGSDI.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_bootstrap_process_ks_validity_data (
  gba_ks_validity_data_type          * ks_validity_ptr
)
{
  gba_storage_profile_data_type   * identity_profile_ptr = NULL;
  gba_identity_data_type          * card_identity_ptr    = NULL;
  gba_result_enum_type              gba_status           = GBA_SUCCESS;

  if(ks_validity_ptr == NULL ||
     gba_bootstrap_info_ptr == NULL ||
     gba_bootstrap_info_ptr->mode != GBA_U)
  {
    return GBA_GENERIC_ERROR;
  }

  /* update existing SFS profile storage with BTID and Lifetime data
     from EF GBABP, create new storage profile if storage is not present */
  identity_profile_ptr = gba_storage_get_profile(gba_bootstrap_info_ptr->session_type);
  if(identity_profile_ptr == NULL)
  {
    identity_profile_ptr =
      (gba_storage_profile_data_type *)GBA_MALLOC(sizeof(gba_storage_profile_data_type));
    if(identity_profile_ptr == NULL)
    {
      return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  identity_profile_ptr->aid          = gba_bootstrap_info_ptr->aid;
  identity_profile_ptr->impi         = gba_bootstrap_info_ptr->impi;
  identity_profile_ptr->btid         = ks_validity_ptr->btid;
  identity_profile_ptr->lifetime     = ks_validity_ptr->lifetime;

  (void)gba_storage_put_profile(gba_bootstrap_info_ptr->session_type,
                                identity_profile_ptr);

  gba_storage_free_profile(&identity_profile_ptr);

  /* with SFS storage profile updated with data from EF GBABP,
     reset to GBA_STATE_IMPI state and retry process identity validation */
  gba_status = gba_state_change(GBA_STATE_GBABP, GBA_STATE_IMPI);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  card_identity_ptr = GBA_MALLOC(sizeof(gba_identity_data_type));

  if(card_identity_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  card_identity_ptr->aid           = gba_bootstrap_info_ptr->aid;
  card_identity_ptr->impi          = gba_bootstrap_info_ptr->impi;
  card_identity_ptr->mode          = gba_bootstrap_info_ptr->mode;

  gba_status = gba_bootstrap_process_identity_validation(card_identity_ptr);

  GBA_FREE(card_identity_ptr);

  return gba_status;
} /* gba_bootstrap_process_ks_validity_data */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_PROCESS_EXTERNAL_CMD

DESCRIPTION
  Entry function into the GBA bootstrap procedure/state machine. Performs
  some basic initalization before triggering the GBA MMGSDI module to
  retrieve the IMPI.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_bootstrap_process_external_cmd (
  gba_bootstrap_data_type       * gba_bootstrap_ptr,
  uint32                          request_id
)
{
  gba_result_enum_type    gba_status = GBA_SUCCESS;

  if(gba_bootstrap_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  if (gba_bootstrap_info_ptr == NULL)
  {
    gba_bootstrap_info_ptr = GBA_MALLOC(sizeof(gba_bootstrap_info_type));
  }
  if(gba_bootstrap_info_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Assign the identity globals */
  gba_bootstrap_info_ptr->reference_id = request_id;
  gba_bootstrap_info_ptr->session_type = gba_bootstrap_ptr->session_type;
  gba_bootstrap_info_ptr->naf_id = gba_bootstrap_ptr->naf_id;
  gba_bootstrap_info_ptr->force_bootstrap = gba_bootstrap_ptr->force_bootstrap;
  gba_bootstrap_info_ptr->apn_info = gba_bootstrap_ptr->apn_info;

  gba_status = gba_state_change(GBA_STATE_IDLE, GBA_STATE_IMPI);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  return gba_mmgsdi_retrieve_identity(gba_bootstrap_ptr->aid,
                                      gba_bootstrap_ptr->session_type,
                                      gba_bootstrap_cb,
                                      gba_bootstrap_info_ptr->reference_id);
} /* gba_bootstrap_process_external_cmd */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_SESSION_CLEANUP

DESCRIPTION
  This function will clean the application specific information from
  the storage module and if the application being cleared matches the
  application on which the current GBA procedure is being run, the
  cached identity information will be cleared as well.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_session_cleanup (
  uint32                                reference_id,
  gba_session_type                      session_type
)
{
  gba_storage_clear_profile(session_type);

  /* If the application being cleared matches the
     application on which the current GBA procedure
     is being run, then the currently cached identity
     information is stale and needs to be cleared. */
  if(gba_bootstrap_info_ptr &&
     reference_id == gba_bootstrap_info_ptr->reference_id)
  {
    gba_bootstrap_reset();
  }
} /* gba_bootstrap_session_cleanup */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in gba_main, handles all bootstrap command
  processing for each GBA command. This function will also maintain
  the state machine and handle invalid states appropriately.

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_handle_cmd_sig (
  void
)
{
  gba_bootstrap_cmd_type          * task_cmd_ptr  = NULL;
  gba_result_enum_type              gba_status    = GBA_SUCCESS;
  gba_state_enum_type               gba_state     = GBA_STATE_IDLE;

  task_cmd_ptr = (gba_bootstrap_cmd_type*)(q_get(&gba_bootstrap_cmd_q));

  (void) rex_clr_sigs(rex_self(), GBA_BOOTSTRAP_CMD_SIG);

  if (task_cmd_ptr == NULL)
  {
    return;
  }

  gba_status = task_cmd_ptr->status;

  if(task_cmd_ptr->reference_id != gba_bootstrap_info_ptr->reference_id)
  {
    /* If the reference ID does not match, that means the current command
       has already timed out and an error response was already sent so we
       can return so this command is dropped. */
    return;
  }

  if (gba_status == GBA_SUCCESS)
  {
    gba_state = gba_state_current();

    GBA_MSG_HIGH_2("bootstrap state: 0x%x, cmd_type: 0x%x",
                    gba_state,
                    task_cmd_ptr->cmd_data.cmd_type);

    switch(gba_state)
    {
      case GBA_STATE_IMPI:
        if(task_cmd_ptr->cmd_data.cmd_type != GBA_RETRIEVE_IDENTITY_CMD)
        {
          gba_status = GBA_GENERIC_ERROR;
          break;
        }

        gba_status = gba_bootstrap_process_identity_validation(
                       &task_cmd_ptr->cmd_data.data.identity);
        break;

       case GBA_STATE_GBABP:
        if(task_cmd_ptr->cmd_data.cmd_type != GBA_RETRIEVE_KS_VALIDITY_CMD)
        {
          gba_status = GBA_GENERIC_ERROR;
          break;
        }

        gba_status = gba_bootstrap_process_ks_validity_data(
                       &task_cmd_ptr->cmd_data.data.ks_validity);
        break;

      case GBA_STATE_INITIAL_HTTP:
        if(task_cmd_ptr->cmd_data.cmd_type != GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD)
        {
          gba_status = GBA_GENERIC_ERROR;
          break;
        }

        gba_status = gba_bootstrap_process_authentication(
                       &task_cmd_ptr->cmd_data.data.auth);
        break;

      case GBA_STATE_UICC_AUTHENTICATION:
        if(task_cmd_ptr->cmd_data.cmd_type != GBA_UICC_AUTHENTICATE_RESPONSE_CMD)
        {
          gba_status = GBA_GENERIC_ERROR;
          break;
        }

        gba_status = gba_bootstrap_process_result_authorization(
                       &task_cmd_ptr->cmd_data.data.auth_status);
        break;

     case GBA_STATE_FINAL_HTTP:
        if(task_cmd_ptr->cmd_data.cmd_type == GBA_HTTP_RESPONSE_FINAL_CMD)
        {
          gba_status = gba_bootstrap_process_200_ok(
                         &task_cmd_ptr->cmd_data.data.http_200_ok);
        }
        else if(task_cmd_ptr->cmd_data.cmd_type == GBA_HTTP_RESPONSE_UNAUTHORIZED_CMD)
        {
          gba_status = gba_state_change(gba_state_current(), GBA_STATE_INITIAL_HTTP);
          if(gba_status != GBA_SUCCESS)
          {
            break;
          }

          gba_status = gba_bootstrap_process_authentication(
                         &task_cmd_ptr->cmd_data.data.auth);
        }
        else
        {
          gba_status = GBA_GENERIC_ERROR;
        }
        break;

      case GBA_STATE_GEN_KS_EXT_NAF:
        if(task_cmd_ptr->cmd_data.cmd_type != GBA_GEN_KEY_CMD)
        {
          gba_status = GBA_GENERIC_ERROR;
          break;
        }

        gba_status = gba_bootstrap_process_gen_ks_ext_naf(
                       &task_cmd_ptr->cmd_data.data.ks_ext_naf);
        break;

      case GBA_STATE_GEN_TMPI:
        if(task_cmd_ptr->cmd_data.cmd_type != GBA_GEN_KEY_CMD)
        {
          gba_status = GBA_GENERIC_ERROR;
          break;
        }

        gba_bootstrap_process_gen_tmpi(
                       &task_cmd_ptr->cmd_data.data.tmpi_ks_naf);
        break;

      default:
        gba_status = GBA_GENERIC_ERROR;
        break;
    }
  }
  else if(gba_status == GBA_SERVER_ERROR || gba_status == GBA_NETWORK_ERROR)
  {
    gba_status = gba_bootstrap_build_and_send_error_response(gba_status,
                                                             &task_cmd_ptr->cmd_data);
  }

  /* If the status is not successful, that means an error occurred during the GBA
     bootstrap procedure and a response was not sent to the client so we must send
     a response now and notify them of the error. */
  if (gba_status != GBA_SUCCESS)
  {
    gba_send_response(gba_status,
                      NULL);

    (void)gba_state_change(gba_state_current(), GBA_STATE_DONE);
  }

  if(gba_state_current() == GBA_STATE_DONE)
  {
    gba_reset();

    /* Set the signal if there are one or more commands still in queue
       and GBA is currently in IDLE state. */
    gba_cmd_set_sig();
  }

  GBA_FREE(task_cmd_ptr);
} /* gba_bootstrap_handle_cmd_sig */


/*===========================================================================
   FUNCTION:      GBA_QUEUE_BOOTSTRAP_CMD

   DESCRIPTION:
     This function queues commands to GBA to processes the bootstrap procedure.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     gba_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type  gba_queue_bootstrap_cmd (
  gba_bootstrap_cmd_type         * task_cmd_ptr
)
{
  gba_result_enum_type    gba_status    = GBA_SUCCESS;

  if (task_cmd_ptr == NULL )
  {
    return GBA_INCORRECT_PARAMS;
  }

  GBA_ENTER_TASK_STATE_CRIT_SECT;

  if (!gba_task_stopped)
  {
    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

    /* Put the message on the queue */
    q_put(&gba_bootstrap_cmd_q, &task_cmd_ptr->link);

    /* Set the command queue signal */
    (void) rex_set_sigs(UIM_GBA_TCB, GBA_BOOTSTRAP_CMD_SIG);
  }
  else
  {
    GBA_MSG_ERR_0("GBA task stopped, cannot queue command");
    gba_status = GBA_GENERIC_ERROR;
  }

  GBA_LEAVE_TASK_STATE_CRIT_SECT;
  return gba_status;
} /* gba_queue_bootstrap_cmd */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_CLEANUP

DESCRIPTION
  This function flushes the gba bootstrap queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_cleanup (
  void
)
{
  gba_bootstrap_cmd_type  * task_cmd_ptr = NULL;

  while (q_cnt(&gba_bootstrap_cmd_q) != 0)
  {
    task_cmd_ptr = (gba_bootstrap_cmd_type*)(q_get(&gba_bootstrap_cmd_q));

    if (task_cmd_ptr != NULL)
    {
      GBA_FREE(task_cmd_ptr);
    }
  }

  GBA_FREE(gba_bootstrap_info_ptr);
} /* gba_bootstrap_cleanup */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_INIT

DESCRIPTION
  This function, called in gba_main, is called to initialize the bootstrap
  module and globals.

DEPENDENCIES
  GBA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_bootstrap_init (
  void
)
{
  (void) q_init(&gba_bootstrap_cmd_q);
} /* gba_bootstrap_init */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_CANCEL

DESCRIPTION

DEPENDENCIES
  Nonce

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_bootstrap_cancel (
  uint32                    request_id
)
{
  /* Checks if the cancel command matches the current bootstrapping command
     in progress. */
  if(gba_bootstrap_info_ptr != NULL &&
     request_id == gba_bootstrap_info_ptr->reference_id)
  {
    gba_send_response(GBA_CANCELLED,
                      NULL);

    gba_reset();

    /* Set the signal if there are one or more commands still in queue
       and GBA is currently in IDLE state. */
    gba_cmd_set_sig();

    return GBA_SUCCESS;
  }

  /* Check cmd queue for request id and cancel if present. */
  return gba_cmd_cancel(request_id);
} /* gba_bootstrap_cancel */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_GET_REQUEST_ID

DESCRIPTION
  Returns a valid command request id.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 gba_bootstrap_get_request_id (
  void
)
{
  /* to make sure GBA_INVALID_REFERENCE_ID is not returned in case of overflow */
  if(++gba_request_id == GBA_INVALID_REFERENCE_ID)
  {
    gba_request_id++;
  }
  return gba_request_id;
} /* gba_bootstrap_get_request_id */


/*===========================================================================
FUNCTION GBA_BOOTSTRAP_GET_CURRENT_REFERENCE_ID

DESCRIPTION
  Returns the reference id in progress.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  uint32
  GBA_INVALID_REFERENCE_ID in case of error

SIDE EFFECTS
  None
===========================================================================*/
uint32 gba_bootstrap_get_current_reference_id (
  void
)
{
  if(gba_bootstrap_info_ptr == NULL)
  {
    return GBA_INVALID_REFERENCE_ID;
  }

  return gba_bootstrap_info_ptr->reference_id;
}/* gba_bootstrap_get_current_reference_id */
