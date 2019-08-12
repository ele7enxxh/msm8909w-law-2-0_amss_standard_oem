#ifndef GBA_LIB_H
#define GBA_LIB_H
/*===========================================================================


                  G B A   D E F I N I T I O N S

                      A N D   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/28/16   tkl     Added support of DSS network down reason in GBA response
05/12/15   tkl     Moved request_id up to callback function
04/30/15   vdc     Added support for APN for bootstrapping
04/15/15   tl      Added support for GBA_U
03/26/15   vdc     Added support to check whether B-TID and Ks is valid
03/27/15   tl      Add support for GBA cancel
03/14/15   vdc     Allow bootstrapping with empty NAF id
01/26/15   tl      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define GBA_KS_NAF_LEN                     32
#define GBA_MAX_BTID_LEN                   255
#define GBA_MAX_LIFETIME_LEN               25
#define GBA_MAX_NAF_FQDN_LEN               255
#define GBA_NAF_SECURITY_PROTOCOL_LEN      5
#define GBA_MAX_AID_LEN                    16
#define GBA_MIN_IMPI_LEN                   3
#define GBA_MAX_IMPI_LEN                   255

typedef uint8 gba_ks_naf_type[GBA_KS_NAF_LEN];
typedef uint8 gba_naf_security_protocol_type[GBA_NAF_SECURITY_PROTOCOL_LEN];

/* ----------------------------------------------------------------------------
   ENUM:      GBA_RESULT_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the result of GBA request message
-------------------------------------------------------------------------------*/
typedef enum {
  /* Successful GBA command processed */
  GBA_SUCCESS,
  /* Generic internal error */
  GBA_GENERIC_ERROR,
  /* Invalid input parameter(s) */
  GBA_INCORRECT_PARAMS,
  /* Not enough memory available */
  GBA_MEMORY_ERROR_HEAP_EXHAUSTED,
  /* Command/parameter not supported */
  GBA_UNSUPPORTED,
  /* Authentication failure */
  GBA_AUTH_FAILURE,
  /* Card error causing GBA failure */
  GBA_SIM_ERROR,
  /* Card is not ready. GBA unable to proceed */
  GBA_SIM_NOT_READY,
  /* Network failure causing GBA failure */
  GBA_NETWORK_ERROR,
  /* Network is not yet ready */
  GBA_NETWORK_NOT_READY,
  /* Command process timed out */
  GBA_TIMEOUT,
  /* Command was cancelled */
  GBA_CANCELLED,
  /* Server error from BSF */
  GBA_SERVER_ERROR
} gba_result_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      GBA_SESSION_TYPE

   DESCRIPTION:
     This enum specifies application session.
-------------------------------------------------------------------------------*/
typedef enum {
  GBA_3GPP_PROV_SESSION_PRI,
  GBA_3GPP_PROV_SESSION_SEC,
  GBA_NON_PROV_SESSION_SLOT_1,
  GBA_NON_PROV_SESSION_SLOT_2,
  GBA_SESSION_MAX
} gba_session_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_AID_TYPE

   DESCRIPTION:
     Structure contains the application ID of the application on the card to
     be used for the GBA procedure. This parameter is used only in case of
     non-provisioning sessions.
-------------------------------------------------------------------------------*/
typedef struct {
  uint8                            data_len;
  uint8                            data[GBA_MAX_AID_LEN];
} gba_aid_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_BTID_TYPE

   DESCRIPTION:
     Structure contains the BSF transaction ID.
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                           data_len;
  char                             data[GBA_MAX_BTID_LEN];
} gba_btid_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_LIFETIME_TYPE

   DESCRIPTION:
     Structure contains the lifetime data.
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                           data_len;
  char                             data[GBA_MAX_LIFETIME_LEN];
} gba_lifetime_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_IMPI_TYPE

   DESCRIPTION:
     Structure which stores the IMPI data.
-------------------------------------------------------------------------------*/
typedef struct {
  uint16                              data_len;
  char                                data[GBA_MAX_IMPI_LEN];
} gba_impi_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_NAF_FQDN_TYPE

   DESCRIPTION:
     Structure contains the NAF FQDN data.
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                           data_len;
  char                             data[GBA_MAX_NAF_FQDN_LEN];
} gba_naf_fqdn_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GBA_RESPONSE_SUCCESS_DATA_TYPE

   DESCRIPTION:
     Structure contains the success data for GBA response.
-------------------------------------------------------------------------------*/
typedef struct
{
  gba_ks_naf_type              ks_naf;
  gba_btid_type                btid;
  gba_lifetime_type            lifetime;
  gba_impi_type                impi;
} gba_response_success_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    GBA_RESPONSE_DATA_TYPE

   DESCRIPTION:
     Structure contains the response data returned to client.
	 Response data returned depends on the GBA status returned with callback.
	 GBA_SUCCESS includes success response data;
	 GBA_SERVER_ERROR  includes server_error with optional error data
	 GBA_NETWORK_ERROR includes network_error with optional error data
-----------------------------------------------------------------------------*/
typedef struct {
  union
  {
    gba_response_success_data_type success;
    struct
    {
      boolean                      error_code_present;
      uint16                       error_code;
    } server_error;
    struct
    {
      boolean                      netdown_reason_present;
      uint32                       netdown_reason;
    } network_error;
  }                                resp_data;
} gba_response_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    GBA_NAF_ID_TYPE

   DESCRIPTION:
     Structure contains the NAF ID request input information
-----------------------------------------------------------------------------*/
typedef struct {
  gba_naf_fqdn_type                fqdn;
  gba_naf_security_protocol_type   security_protocol;
} gba_naf_id_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      GBA_CALLBACK_TYPE
-----------------------------------------------------------------------------*/
/** @brief GBA callback function.

    The parameter to the callback function contains the results of the
    client's request.
*/
typedef void (*gba_callback_type) (gba_result_enum_type                status,
                                   uint32                              request_id,
                                   const gba_response_data_type      * resp_ptr,
                                   const void                        * user_data_ptr);

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*=============================================================================
FUNCTION        GBA_BOOTSTRAP

DESCRIPTION
  This function to begin the Generic Bootstrapping procedure. If the NAF id is
  NULL of no FQDN is provided then GBA does not calculate Ks_NAF and returns
  after bootstrapping

PARAMETERS
  naf_id_ptr:        Network Application Function ID
  session_type:      Session type indicator
  aid_ptr:           Application ID (NULL for provisioning sessions)
  force_bootstrap:   Indicates whether to force bootstrap procedure
  request_id_ptr:    Output request id of the queued command
                     (Can be NULL if request id is not required)
  apn_data_ptr:      Pointer to APN data (optional if default APN can be used)
  gba_cb:            Client response function
  user_data_ptr:     Client user data

RETURN VALUE
  gba_result_enum_type:
    GBA_SUCCESS:               The command structure was properly
                               generated and queued onto the GBA
                               Command Queue.
    GBA_INCORRECT_PARAMS:      The parameters supplied to the API are not
                               within appropriate ranges.

===============================================================================*/
gba_result_enum_type gba_bootstrap (
  gba_naf_id_type                * naf_id_ptr,
  gba_session_type                 session_type,
  const gba_aid_type             * aid_ptr,
  boolean                          force_bootstrap,
  uint32                         * request_id_ptr,
  const char                     * apn_ptr,
  gba_callback_type                gba_cb,
  const void                     * user_data_ptr
);

/*=============================================================================
FUNCTION        GBA_CANCEL

DESCRIPTION
  This synchronous command will delete a GBA bootstrap request that matches the
  request id passed in. If the matching request is currently being processed, then
  the bootstrap command will be cancelled and any stored data related to the command
  will be cleared. If the bootstrap request is still in the queue then it is simply
  removed. In both cases, the callback of the original request will be triggered
  allowing the client to clean-up any userdata that may have been allocated or any
  other extra processing that may have been required.

PARAMETERS
  request_id:                  Request ID of the command to be cancelled

RETURN VALUE
  gba_result_enum_type:
    GBA_SUCCESS:               The command was successfully cancelled
    GBA_INCORRECT_PARAMS:      The request id parameter was invalid

===============================================================================*/
gba_result_enum_type gba_cancel (
  uint32                           request_id
);

/*=============================================================================
FUNCTION        GBA_IS_KEY_VALID

DESCRIPTION
  This function checks whether the B-TID is valid and its life time validity

PARAMETERS
  session_type:   Session type indicator
  aid_ptr:        Pointer to application id. This is required if session type
                  is non-prov
  btid_ptr:       Pointer to B-TID value
  life_time_ptr:  Life time pointer. This can be set to NULL if client is not
                  interested

RETURN VALUE
  Boolean:
    TRUE:         If the B-TID matches and life time validity has not expired
    FALSE:        If the B-TID does not match or the B-TID's life time has expired

===============================================================================*/
boolean gba_is_key_valid (
  gba_session_type        session_type,
  const gba_aid_type    * aid_ptr,
  const gba_btid_type   * btid_ptr,
  gba_lifetime_type     * life_time_ptr
);

#endif /* GBA_LIB_H */

