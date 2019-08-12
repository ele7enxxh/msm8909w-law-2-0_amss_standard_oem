#ifndef GBA_HTTP_H
#define GBA_HTTP_H
/*===========================================================================


            G B A   H T T P   H E A D E R


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
07/20/15   tkl     Added HTTP Cookies support
06/16/15   tkl     Fixed race condition during ds_http_open_session
06/10/15   tkl     Add support of IPv6 BSF address
05/05/15   lxu     Fixed various issues
04/30/15   vdc     Added support for APN for bootstrapping
04/15/15   tkl     Add TMPI support
11/07/14   tkl     Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"
#include "time_types.h"
#include "ds_http_types.h"

#include "gba.h"
#include "gba_bootstrap.h"
#include "gba_lib.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define GBA_HTTP_HEADER_NAME_MAX_LEN     32
#define GBA_HTTP_HEADER_VALUE_MAX_LEN    256

typedef enum {
  GBA_HTTP_RSP,
  GBA_HTTP_BLOCK_RSP
}gba_http_cmd_enum_type;

/* HTTP block event types */
typedef enum
{
  GBA_HTTP_BLOCK_AUTHENTICATION,
  GBA_HTTP_BLOCK_SSL_CERTIFICATE_ALERT
} gba_http_block_event_enum_type;

/* HTTP Cookie Data type */
typedef struct
{
  /* Cookie name value pair */
  ds_http_cookie_name_s_type      name_value;
  /* Cookie attributes */
  boolean                         expiration_set;
  time_julian_type                expiration;
} gba_http_cookie_data_type;

/* Custom headers that can be specified by the client */
typedef struct
{
  char name [GBA_HTTP_HEADER_NAME_MAX_LEN+1];
  char value[GBA_HTTP_HEADER_VALUE_MAX_LEN+1];
} gba_http_response_header_type;

typedef struct {
   uint32                         session_id;
   uint32                         request_id;
   sint15                         error;
   uint16                         http_status;
   uint32                         content_size;
   uint8                        * content_ptr;
   uint32                         num_headers;
   gba_http_response_header_type* header_info_ptr;
   uint16                         num_cookies;
   gba_http_cookie_data_type    * cookie_ptr;
} gba_http_response_data_type;

typedef struct
{
  char*                           realm_ptr; /* The realm that the server
                                                requested authentication for */
  char*                           nonce_ptr; /* The nonce given in the response
                                                header for authentication */
  char*                           auth_response_header_ptr;
                                             /* WWW-Authenticate or Proxy-
                                                Authenticate response header
                                                content */
  uint16                          num_cookies;
  gba_http_cookie_data_type*      cookie_ptr;/* Set-Cookie header from server */
} gba_http_block_auth_info_type;

typedef struct {
   uint32                         session_id;
   uint32                         request_id;
   gba_http_block_event_enum_type event_type;
   gba_http_block_auth_info_type  auth_data;
} gba_http_block_response_data_type;

typedef char                      gba_http_header_value_type[GBA_HTTP_HEADER_VALUE_MAX_LEN + 1];

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_HTTP_CMD_TYPE

   DESCRIPTION:
     Contains information for handling GBA HTTP commands
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                           link;
  gba_http_cmd_enum_type                cmd_type;
  union {
    gba_http_response_data_type         rsp_data;
    gba_http_block_response_data_type   block_rsp_data;
  } data;
} gba_http_cmd_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

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
);

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
);

/*===========================================================================
   FUNCTION:      GBA_HTTP_CLEANUP

   DESCRIPTION:
     GBA HTTP cleanup function called to clean up refrerence data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
gba_result_enum_type gba_http_cleanup(
  void
);

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
gba_result_enum_type gba_http_send_user_identity_request (
  const gba_impi_type                  * impi_ptr,
  const gba_impi_type                  * tmpi_ptr,
  const gba_bsf_address_type           * bsf_ptr,
  const gba_bsf_address_type           * realm_ptr,
  const gba_apn_type                   * apn_ptr,
  gba_session_type                       session_type,
  uint32                                 reference_id,
  gba_bootstrap_callback_type            user_identity_cb
);

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
);

/*===========================================================================
FUNCTION GBA_HTTP_SEND_AUTHENTICATION_FAILURE

DESCRIPTION
  This function sends the HTTP GET to BSF with authentication sync failure
  or mac failure.

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
);

#endif /* GBA_HTTP_H */

