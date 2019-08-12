#ifndef GBA_NV_H
#define GBA_NV_H
/*===========================================================================


            G B A   N V   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/19/15   vdc     Update IMEI header NV struct
08/12/15   ar      Added NV cleanup
07/03/15   tkl     Added support for custom fields
04/30/15   vdc     Added support for APN for bootstrapping
04/03/15   lxu     HTTPs support over Ub
03/16/15   lxu     Initial revision
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gba_bootstrap.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define GBA_IMEI_DIGIT_LEN               15
#define GBA_MAX_IMEI_LEN                 8
#define GBA_MAX_HEADER_NAME_LEN          32

typedef enum {
  GBA_NV_BSF_HTTP  = 0,
  GBA_NV_BSF_HTTPS = 1
} gba_nv_bsf_http_enum_type;

typedef enum {
  GBA_NV_REALM_PER_SPEC   = 0,
  GBA_NV_REALM_OMIT_BSF   = 1
} gba_nv_custom_realm_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_NV_IMEI_DATA_TYPE

   DESCRIPTION:
     Contains information from GBA IMEI header and IMEI digits per 3GPP 24.008
-------------------------------------------------------------------------------*/
typedef struct {
  char                     imei_header_name[GBA_MAX_HEADER_NAME_LEN + 1];
  char                     imei_digits[GBA_IMEI_DIGIT_LEN + 1];
}gba_nv_imei_data_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

#endif /* GBA_NV_H */
