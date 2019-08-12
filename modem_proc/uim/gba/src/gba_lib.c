/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        G B A  L I B R A R Y


GENERAL DESCRIPTION

  This file contains Library function that can be used to externally
  access and configure GBA.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/20/15   tkl     Added HTTP Cookies support
04/30/15   vdc     Added support for APN for bootstrapping
04/06/15   tl      Add GBA_U support
03/30/15   lxu     Support refresh on EF GBABP
03/26/15   vdc     Added support to check whether B-TID and Ks is valid
03/27/15   tl      Add support for GBA cancel
03/14/15   vdc     Allow bootstrapping with empty NAF id
01/26/15   tl      Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "uim_msg.h"

#include "gba_cmd.h"
#include "gba_lib.h"
#include "gba_platform.h"
#include "gba_storage.h"
#include "gba.h"
#include "gba_bootstrap.h"
#include "gba_lifetime.h"

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
  apn_ptr:           Pointer to APN data
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
)
{
  gba_result_enum_type         gba_status     = GBA_SUCCESS;
  gba_cmd_type               * task_cmd_ptr   = NULL;

  if (naf_id_ptr != NULL &&
      naf_id_ptr->fqdn.data_len > GBA_MAX_NAF_FQDN_LEN)
  {
    return GBA_INCORRECT_PARAMS;
  }

  if(apn_ptr != NULL &&
     strlen(apn_ptr) >= GBA_MAX_APN_LEN)
  {
    return GBA_INCORRECT_PARAMS;
  }

  task_cmd_ptr = (gba_cmd_type*)GBA_MALLOC(sizeof(gba_cmd_type));
  if (task_cmd_ptr == NULL)
  {
    return GBA_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  memset(task_cmd_ptr, 0x00, sizeof(gba_cmd_type));

  task_cmd_ptr->gba_cb_ptr                           = gba_cb;
  task_cmd_ptr->user_data_ptr                        = user_data_ptr;
  task_cmd_ptr->gba_bootstrap.session_type           = session_type;
  task_cmd_ptr->gba_bootstrap.force_bootstrap        = force_bootstrap;

  if(naf_id_ptr != NULL)
  {
    task_cmd_ptr->gba_bootstrap.naf_id               = *naf_id_ptr;
  }

  if(apn_ptr != NULL)
  {
    task_cmd_ptr->gba_bootstrap.apn_info.apn_available = TRUE;
    strlcpy(task_cmd_ptr->gba_bootstrap.apn_info.apn,
            apn_ptr,
            sizeof(task_cmd_ptr->gba_bootstrap.apn_info.apn));
  }

  if(session_type == GBA_NON_PROV_SESSION_SLOT_1 ||
     session_type == GBA_NON_PROV_SESSION_SLOT_2)
  {
    if(aid_ptr == NULL)
    {
      GBA_FREE(task_cmd_ptr);
      return GBA_INCORRECT_PARAMS;
    }

    task_cmd_ptr->gba_bootstrap.aid = *aid_ptr;
  }

  GBA_ENTER_DATA_CRIT_SECT;
  task_cmd_ptr->request_id = gba_bootstrap_get_request_id();
  GBA_LEAVE_DATA_CRIT_SECT;

  if(request_id_ptr)
  {
    *request_id_ptr = task_cmd_ptr->request_id;
  }

  gba_status = gba_queue_cmd(task_cmd_ptr);
  if (gba_status != GBA_SUCCESS)
  {
    /* Free the task pointer and data since the task_cmd_ptr has not
       been put onto the command queue */
    GBA_FREE(task_cmd_ptr);
    return gba_status;
  }

  GBA_MSG_HIGH_1("Queue of command: GBA bootstrap status 0x%x", gba_status);

  return GBA_SUCCESS;
} /* gba_bootstrap */


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
)
{
  gba_result_enum_type   gba_status = GBA_SUCCESS;

  GBA_ENTER_DATA_CRIT_SECT;
  gba_status = gba_bootstrap_cancel(request_id);
  GBA_LEAVE_DATA_CRIT_SECT;

  return gba_status;
} /* gba_cancel */


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
)
{
  gba_storage_profile_data_type * identity_profile_ptr = NULL;
  boolean                         result               = TRUE;

  if(btid_ptr == NULL)
  {
    result = FALSE;
    goto send_result;
  }

  GBA_ENTER_DATA_CRIT_SECT;
  identity_profile_ptr = gba_storage_get_profile(session_type);
  GBA_LEAVE_DATA_CRIT_SECT;

  if(identity_profile_ptr == NULL)
  {
    result = FALSE;
    goto send_result;
  }

  /* If the session type is non-prov then AID should match */
  if(session_type == GBA_NON_PROV_SESSION_SLOT_1 ||
     session_type == GBA_NON_PROV_SESSION_SLOT_2)
  {
    if(aid_ptr == NULL                                         ||
       aid_ptr->data_len > GBA_MAX_AID_LEN                     ||
       aid_ptr->data_len != identity_profile_ptr->aid.data_len ||
       (memcmp(aid_ptr->data, identity_profile_ptr->aid.data, aid_ptr->data_len) != 0))
    {
      result = FALSE;
      goto send_result;
    }
  }

  /* If B-TID does not match then return FALSE */
  if(btid_ptr->data_len > GBA_MAX_BTID_LEN                     ||
     btid_ptr->data_len != identity_profile_ptr->btid.data_len ||
     (memcmp(btid_ptr->data, identity_profile_ptr->btid.data, btid_ptr->data_len) != 0))
  {
    result = FALSE;
    goto send_result;
  }

  /* Check whether the life time of Ks has expired  */
  if(gba_lifetime_is_ks_expired(identity_profile_ptr->lifetime))
  {
    result = FALSE;
    goto send_result;
  }

  if(life_time_ptr)
  {
    *life_time_ptr = identity_profile_ptr->lifetime;
  }

send_result:
  if(identity_profile_ptr != NULL)
  {
    gba_storage_free_profile(&identity_profile_ptr);
  }

  return result;
} /* gba_is_key_valid */

