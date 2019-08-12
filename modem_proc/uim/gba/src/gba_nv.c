/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   N V   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA NV functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
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
08/19/15   vdc     Update IMEI header NV struct
08/12/15   ar      Added NV cleanup
07/03/15   tkl     Added support for custom fields
06/10/15   tkl     Added support of IPv6 BSF address
05/05/15   lxu     Fixed various issues
04/30/15   vdc     Added support for APN for bootstrapping
04/03/15   lxu     HTTPs support over Ub
03/16/15   lxu     Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "fs_errno.h"
#include "fs_public.h"
#include "nv.h"
#include <stringl/stringl.h>
#include "uim_common_efs.h"

#include "gba.h"
#include "gba_nv.h"
#include "gba_platform.h"
#include "gba_lib.h"
#include "gba_bootstrap.h"
#include "gba_util.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

#define GBA_MAX_BSF_URL_LEN               250
#define GBA_MAX_IMEI_LEN                  8
#define GBA_IMEI_HIGH_BYTE_MASK           0xF0
#define GBA_IMEI_LOW_BYTE_MASK            0x0F

/* ----------------------------------------------------------------------------
   STRUCTURE:     GBA_NV_BSF_ADDRESS_TYPE

   DESCRIPTION:
     Structure to store NV data for bsf_url and bsf_port.
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  char                         bsf_url[GBA_MAX_BSF_URL_LEN + 1];
  uint16                       bsf_port;
  gba_nv_bsf_http_enum_type    protocol;
} gba_nv_bsf_address_type;

typedef PACKED struct PACKED_POST {
  char                         apn[GBA_MAX_APN_LEN];
} gba_nv_apn_type;

typedef PACKED struct PACKED_POST {
  char                         imei_header_name[GBA_MAX_HEADER_NAME_LEN + 1];
} gba_nv_imei_header_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:     GBA_NV_INFO_TYPE

   DESCRIPTION:
     Structure to store GBA NV data
-------------------------------------------------------------------------------*/
typedef struct{
  boolean                          init;
  gba_nv_bsf_address_type        * bsf_address_ptr;
  gba_nv_apn_type                * apn_ptr;
  gba_nv_custom_realm_enum_type    custom_realm;
  boolean                          user_agent_tmpi;
  gba_nv_imei_data_type          * http_header_imei_ptr;
} gba_nv_info_type;

static gba_nv_info_type   gba_nv_info = {FALSE,                 /* init flag */
                                         NULL,                  /* bsf address nv data */
                                         NULL,                  /* apn name */
                                         GBA_NV_REALM_PER_SPEC, /* custom realm enum */
                                         FALSE,                 /* custom TMPI user agent */
                                         NULL};                 /* imei nv data */

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_NV_INIT

DESCRIPTION
  Function initialization of gba nv, will read all GAB related NV,
  and cache them in a static structure.

DEPENDENCIES
  This function should be called before gba_nv_get_* APIs

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_nv_init(
  void
)
{
  gba_nv_imei_header_type      nv_imei_header = {{0}};

  if (gba_nv_info.init)
  {
    return;
  }

  gba_nv_info.init = TRUE;

  /* get GBA BSF address NV */
  gba_nv_info.bsf_address_ptr =
    (gba_nv_bsf_address_type *)GBA_MALLOC(sizeof(gba_nv_bsf_address_type));

  if (gba_nv_info.bsf_address_ptr == NULL)
  {
    return;
  }

  memset(gba_nv_info.bsf_address_ptr, 0x00, sizeof(gba_nv_bsf_address_type));

  if(efs_get(UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_BSF_ADDRESS,
                gba_nv_info.bsf_address_ptr,
                sizeof(gba_nv_bsf_address_type)) != sizeof(gba_nv_bsf_address_type))
  {
    GBA_FREE(gba_nv_info.bsf_address_ptr);
  }

  /* get GBA apn NV */
  gba_nv_info.apn_ptr =
    (gba_nv_apn_type *)GBA_MALLOC(sizeof(gba_nv_apn_type));

  if (gba_nv_info.apn_ptr == NULL)
  {
    return;
  }

  if (efs_get(UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_APN,
              gba_nv_info.apn_ptr,
              sizeof(gba_nv_apn_type)) != sizeof(gba_nv_apn_type))
  {
    GBA_FREE(gba_nv_info.apn_ptr);
  }

  /* get GBA custom realm NV */
  (void)efs_get(UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_CUSTOM_REALM,
                &gba_nv_info.custom_realm,
                sizeof(gba_nv_custom_realm_enum_type));

  /* get GBA TMPI user agent NV */
  (void)efs_get(UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_CUSTOM_USER_AGENT_TMPI,
                &gba_nv_info.user_agent_tmpi,
                sizeof(boolean));

  /* get GBA IMEI header NV */
  (void)efs_get(UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_HTTP_HEADER_IMEI,
              &nv_imei_header,
              sizeof(gba_nv_imei_header_type));

  if(nv_imei_header.imei_header_name[0] != 0x0 &&
     (nv_imei_header.imei_header_name[GBA_MAX_HEADER_NAME_LEN] == 0x0))
  {
    gba_nv_info.http_header_imei_ptr =
      (gba_nv_imei_data_type *)GBA_MALLOC(sizeof(gba_nv_imei_data_type));

    if (gba_nv_info.http_header_imei_ptr == NULL)
    {
      return;
    }

    memset(gba_nv_info.http_header_imei_ptr, 0x00, sizeof(gba_nv_imei_data_type));
    memscpy(gba_nv_info.http_header_imei_ptr->imei_header_name,
            sizeof(gba_nv_info.http_header_imei_ptr->imei_header_name),
            nv_imei_header.imei_header_name,
            sizeof(nv_imei_header.imei_header_name));
  }

  /* read IMEI from NV */
  if (gba_nv_info.http_header_imei_ptr != NULL)
  {
    nv_cmd_type       * gba_nv_cmd_ptr = NULL;
    nv_item_type      * nv_data_ptr    = NULL;

    gba_nv_cmd_ptr = GBA_MALLOC(sizeof(nv_cmd_type));

    if(gba_nv_cmd_ptr == NULL)
    {
      GBA_FREE(gba_nv_info.http_header_imei_ptr);
      return;
    }

    /* Notify this task when done */
    gba_nv_cmd_ptr->tcb_ptr = rex_self();
    gba_nv_cmd_ptr->sigs = GBA_NV_READ_SIG;

    /* Command goes on no queue when done */
    gba_nv_cmd_ptr->done_q_ptr = NULL;

    /* Item to read */
    gba_nv_cmd_ptr->item = NV_UE_IMEI_I;
    gba_nv_cmd_ptr->cmd = NV_READ_F;

    nv_data_ptr = GBA_MALLOC(sizeof(nv_item_type));

    if(nv_data_ptr == NULL)
    {
      GBA_FREE(gba_nv_cmd_ptr);
      GBA_FREE(gba_nv_info.http_header_imei_ptr);
      return;
    }

    /* Set the Data Ptr for NV Item to read */
     gba_nv_cmd_ptr->data_ptr = nv_data_ptr;

    /* Clear REX Signals */
    (void) rex_clr_sigs( rex_self(), GBA_NV_READ_SIG );

    /* Send Command to NV */
    nv_cmd(gba_nv_cmd_ptr);

    /* Wait for Signal from NV that it's complete */
    (void)gba_wait(GBA_NV_READ_SIG);

    /* Clear Signal */
    (void) rex_clr_sigs( rex_self(), GBA_NV_READ_SIG );

    GBA_MSG_LOW_2("Get NV Status = 0x%x for item: 0x%x",
                   gba_nv_cmd_ptr->status, gba_nv_cmd_ptr->item);

    /* first byte of ue_imei is len of ue_imei */
    if (gba_nv_cmd_ptr->status == NV_DONE_S &&
        nv_data_ptr->ue_imei.ue_imei[0] <= GBA_MAX_IMEI_LEN)
    {
      /* convert ue_imei to imei digits */
      uint8                offset     = 0;
      uint8                i          = 0;
      gba_result_enum_type gba_status = GBA_SUCCESS;

      for (i = 1; i <= GBA_MAX_IMEI_LEN && offset < GBA_IMEI_DIGIT_LEN; i++)
      {
        if (i != 1)
        {
          gba_status = gba_util_itoa((uint16)(nv_data_ptr->ue_imei.ue_imei[i] & GBA_IMEI_LOW_BYTE_MASK),
                                     &gba_nv_info.http_header_imei_ptr->imei_digits[offset++],
                                     2);
          if (GBA_SUCCESS != gba_status)
          {
            GBA_FREE(gba_nv_info.http_header_imei_ptr);
            break;
          }
        }

        gba_status = gba_util_itoa((uint16)((nv_data_ptr->ue_imei.ue_imei[i] & GBA_IMEI_HIGH_BYTE_MASK) >> 4),
                                   &gba_nv_info.http_header_imei_ptr->imei_digits[offset++],
                                   2);
        if (GBA_SUCCESS != gba_status)
        {
          GBA_FREE(gba_nv_info.http_header_imei_ptr);
          break;
        }
      }
    }

    GBA_FREE(gba_nv_cmd_ptr);
    GBA_FREE(nv_data_ptr);
  }
} /*gba_nv_init*/


/*===========================================================================
FUNCTION GBA_NV_GET_BSF_URL

DESCRIPTION
  Function get bsf_url from nv

DEPENDENCIES
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_nv_get_bsf_url(
  gba_bsf_address_type    * bsf_ptr
)
{
  uint16 bsf_len  = 0;

  if (!gba_nv_info.init)
  {
    gba_nv_init();
  }

  if (bsf_ptr == NULL ||
      gba_nv_info.bsf_address_ptr == NULL ||
      gba_nv_info.bsf_address_ptr->bsf_url[0] == 0x0 ||
      gba_nv_info.bsf_address_ptr->bsf_url[GBA_MAX_BSF_URL_LEN] != 0x0)
  {
    return GBA_GENERIC_ERROR;
  }

  bsf_len = strnlen(gba_nv_info.bsf_address_ptr->bsf_url,
                    sizeof(gba_nv_info.bsf_address_ptr->bsf_url));

  /* check for IPv6 format */
  if(NULL != memchr(gba_nv_info.bsf_address_ptr->bsf_url,
                    ':',
                    bsf_len) &&
     gba_nv_info.bsf_address_ptr->bsf_url[0] != '[')
  {
    /* this is always TRUE as MAX value for bsf_len is 250. Keeping it
       in case GBA_MAX_BSF_ADDRESS_LEN get changed */
    if (bsf_len > GBA_MAX_BSF_ADDRESS_LEN - 2)
    {
      return GBA_GENERIC_ERROR;
    }
    /* add '[ ]' for IPv6 format URL */
    bsf_ptr->data[0] = '[';
    memscpy(&bsf_ptr->data[1],
            GBA_MAX_BSF_ADDRESS_LEN - 2,
            gba_nv_info.bsf_address_ptr->bsf_url,
            bsf_len);
    bsf_ptr->data[bsf_len + 1] = ']';
    bsf_len = bsf_len + 2;
  }
  else
  {
    memscpy(bsf_ptr->data, GBA_MAX_BSF_ADDRESS_LEN, gba_nv_info.bsf_address_ptr->bsf_url, bsf_len);
  }
  bsf_ptr->data_len = bsf_len;

  return GBA_SUCCESS;
} /* gba_nv_get_bsf_url */


/*===========================================================================
FUNCTION GBA_NV_GET_BSF_PORT

DESCRIPTION
  Function get bsf_port from nv

DEPENDENCIES
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_nv_get_bsf_port(
  uint16    * bsf_port_ptr
)
{
  if (!gba_nv_info.init)
  {
    gba_nv_init();
  }

  if (bsf_port_ptr == NULL ||
      gba_nv_info.bsf_address_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  if (gba_nv_info.bsf_address_ptr->bsf_port > 0)
  {
    *bsf_port_ptr = gba_nv_info.bsf_address_ptr->bsf_port;
    return GBA_SUCCESS;
  }

  return GBA_GENERIC_ERROR;
} /* gba_nv_get_bsf_port */


/*===========================================================================
FUNCTION GBA_NV_GET_BSF_PROTOCOL

DESCRIPTION
  This function returns the protocol to contact the BSF. If the value is
  configured in the NV item, that value is used, otherwise it fallbacks to
  HTTP, as per standard defined in TS 33.220.

DEPENDENCIES
  None

RETURN VALUE
  gba_nv_bsf_http_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_nv_bsf_http_enum_type gba_nv_get_bsf_protocol(
  void
)
{
  if (!gba_nv_info.init)
  {
    gba_nv_init();
  }

  if (gba_nv_info.bsf_address_ptr == NULL)
  {
    return GBA_NV_BSF_HTTP;
  }

  if (GBA_NV_BSF_HTTP == gba_nv_info.bsf_address_ptr->protocol ||
      GBA_NV_BSF_HTTPS == gba_nv_info.bsf_address_ptr->protocol)
  {
    return gba_nv_info.bsf_address_ptr->protocol;
  }

  return GBA_NV_BSF_HTTP;
} /* gba_nv_get_bsf_protocol */


/*===========================================================================
FUNCTION GBA_NV_GET_APN

DESCRIPTION
  Function gets the APN info from NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_nv_get_apn(
  gba_apn_type   *apn_ptr
)
{
  if(apn_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  if (!gba_nv_info.init)
  {
    gba_nv_init();
  }

  apn_ptr->apn_available = FALSE;

  if (gba_nv_info.apn_ptr != NULL &&
      gba_nv_info.apn_ptr->apn[0] != 0x0 &&
      gba_nv_info.apn_ptr->apn[GBA_MAX_APN_LEN - 1] == 0x0)
  {
    strlcpy(apn_ptr->apn, gba_nv_info.apn_ptr->apn, sizeof(apn_ptr->apn));
    apn_ptr->apn_available = TRUE;
    return GBA_SUCCESS;
  }

  return GBA_GENERIC_ERROR;
} /* gba_nv_get_apn */


/*===========================================================================
FUNCTION GBA_NV_GET_CUSTOM_REALM

DESCRIPTION
  This function returns the enum for customized realm value for GBA Ub procecure.
  If the value is configured in the NV item, that value is used, otherwise it
  fallbacks per standard defined in TS 33.220.

DEPENDENCIES
  None

RETURN VALUE
  gba_nv_custom_realm_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_nv_custom_realm_enum_type gba_nv_get_custom_realm(
  void
)
{
  if (!gba_nv_info.init)
  {
    gba_nv_init();
  }

  return gba_nv_info.custom_realm;
} /* gba_nv_get_custom_realm */


/*===========================================================================
FUNCTION GBA_NV_GET_TMPI_USER_AGENT

DESCRIPTION
  Per 3GPP 33.220, Annex M 6.3, UE shall include token "3gpp-gba-tmpi" in the
  user agen request-header field when sending HTTP messages over Ub to indicate
  TMPI is supported.

  User can configure NV to configure the use token "3gpp-gba-uicc" to indicate
  UE support TMPI functionality in place of "3gpp-gba-tmpi".

  This function returns TRUE if NV is configured to use "3gpp-gba-uicc".
  If the value is configured in the NV item, that value is used, otherwise it
  fallbacks to FALSE

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean gba_nv_get_tmpi_user_agent(
  void
)
{
  if (!gba_nv_info.init)
  {
    gba_nv_init();
  }

  return gba_nv_info.user_agent_tmpi;
} /* gba_nv_get_tmpi_user_agent */


/*===========================================================================
FUNCTION GBA_NV_GET_IMEI_HTTP_HEADER

DESCRIPTION
  Function gets the enable IMEI HTTP Header value from NV, if NV is enabled,
  IMEI header name and IMEI digits are copyed to the input imei_header_ptr.

DEPENDENCIES
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_nv_get_imei_http_header(
  gba_nv_imei_data_type * imei_header_ptr
)
{
  if (imei_header_ptr == NULL)
  {
    return GBA_INCORRECT_PARAMS;
  }

  if (!gba_nv_info.init)
  {
    gba_nv_init();
  }

  if(gba_nv_info.http_header_imei_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  memset(imei_header_ptr, 0x00, sizeof(gba_nv_imei_data_type));
  memscpy(imei_header_ptr, sizeof(gba_nv_imei_data_type),
          gba_nv_info.http_header_imei_ptr, sizeof(gba_nv_imei_data_type));

  return GBA_SUCCESS;
} /* gba_nv_get_imei_http_header */


/*===========================================================================
   FUNCTION:      GBA_NV_CLEANUP

   DESCRIPTION:
     GBA NV cleanup function called to clean up cached NV values

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void gba_nv_cleanup(
  void
)
{
 GBA_MSG_LOW_0("gba_nv_cleanup");
 
 GBA_FREE(gba_nv_info.bsf_address_ptr);
 GBA_FREE(gba_nv_info.apn_ptr);
 GBA_FREE(gba_nv_info.http_header_imei_ptr);

 memset(&gba_nv_info, 0x00, sizeof(gba_nv_info_type));
} /* gba_nv_cleanup */
