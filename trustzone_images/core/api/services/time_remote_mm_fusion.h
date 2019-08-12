#include "time_types.h"
#include "time_remote_reg.h"

/*=============================================================================

FUNCTION TIME_REMOTE_MDM_TO_MSM_SET_JULIAN

DESCRIPTION
  Sends an update from 9K to 7x30

DEPENDENCIES
  None

RETURN VALUE
  Aqcuired System Modem

SIDE EFFECTS
  None

=============================================================================*/

extern void time_remote_mm_set_julian
(
  /* Timestamp to set time of day to */
  time_julian_type                *jul_time
);
/*~ FUNCTION time_remote_mm_set_julian */
/*~ PARAM jul_time POINTER  */

/*=========================================================================
FUNCTION
  ATS_CLIENT_INIT

DESCRIPTION
  Allow the client to register itself with the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data initialized for the new client.

=========================================================================*/
extern ats_client_err_e_type ats_mm_client_init
(
  ats_mm_client_type_e_type   client_type,
  ats_client_id_type          *client_id_ptr,
  ats_client_processor_e_type  processor
);
/*~ FUNCTION ats_mm_client_init 
    RELEASE_FUNC ats_mm_client_release(*client_id_ptr) 
    ONERROR return ATS_CLIENT_ERR_RPC */
/*~ PARAM INOUT client_id_ptr POINTER */


/*=========================================================================
FUNCTION
  ATS_CLIENT_RELEASE

DESCRIPTION
  Allow the client to release itself from the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data reset for this client.

=========================================================================*/
extern ats_client_err_e_type ats_mm_client_release
(
  ats_client_id_type       client_id
);
/*~ FUNCTION ats_mm_client_release 
    ONERROR return ATS_CLIENT_ERR_RPC */
/*=========================================================================
FUNCTION
  ATS_CLIENT_REG_CFG_CB

DESCRIPTION
  Allow the client to register its configuration event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
extern ats_client_err_e_type ats_mm_client_reg_cfg_cb
(
  ats_client_id_type           client_id,
  ats_mm_cfg_event_cb_type    cfg_event_cb
);
/*~ FUNCTION ats_mm_client_reg_cfg_cb 
    ONERROR return ATS_CLIENT_ERR_RPC */
