/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmscl.c -- Client Group

  The WMS module which implements the User API for SMS. This source file
  implements the client group functions which process the client
  requests for client management.
  -------------

    Copyright (c) 2001-2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmscl.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmscl.h"
#include "wmscfg.h"
#include "wmsdc.h"
#include "wmsutils.h"
#include "err.h"

/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
static wms_client_s_type   *clients[WMS_CLIENT_TYPE_MAX];

/* Critsect wms_client_crit_sect protects:
 *   - array "clients[]" defined above during assignments and fetches
 */
static rex_crit_sect_type           wms_client_crit_sect;


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_client_ptr_fetch                                      (INTERNAL)

DESCRIPTION
  Return the pointer to a client's data, incrementing the reference count.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to client data.

SIDE EFFECTS
  None
===========================================================================*/
wms_client_s_type * wms_client_ptr_fetch
(
  wms_client_id_type client_id
)
{
  wms_client_s_type *client_ptr = NULL;

  if (client_id < (uint8)WMS_CLIENT_TYPE_MAX)
  {
    // Client id in range
    rex_enter_crit_sect(&wms_client_crit_sect);
    client_ptr = clients[client_id];
    if (NULL != client_ptr)
    {
      ref_cnt_obj_add_ref(client_ptr);
    }
    rex_leave_crit_sect(&wms_client_crit_sect);
  }

  return client_ptr;

} /* wms_client_ptr_fetch() */

#ifdef FEATURE_CDSMS
/*=========================================================================
FUNCTION wms_get_client_default_channel_pref

DESCRIPTION
   Get channel pref based on current NV settings

DEPENDENCIES
  wms_nv_init() and wms_dc_init() must have been initialized first before
  calling this function.

RETURN VALUE
  default client channel preference

SIDE EFFECTS
  None
=========================================================================*/
wms_client_ch_pref_e_type wms_get_client_default_channel_pref(
  void
)
{
  wms_client_ch_pref_e_type ch_pref;
  wms_dc_s_type *dc_s_ptr = wms_dc_s_ptr();

  if ( (dc_s_ptr->is_access_channel_enabled) && (dc_s_ptr->is_traffic_channel_enabled) )
  {
    ch_pref = WMS_CLIENT_CH_PREF_ACH_OR_TCH;
  }
  else if ( dc_s_ptr->is_access_channel_enabled )
  {
    ch_pref = WMS_CLIENT_CH_PREF_ACH_ONLY;
  }
  else if ( dc_s_ptr->is_traffic_channel_enabled )
  {
    ch_pref = WMS_CLIENT_CH_PREF_TCH_ONLY;
  }
  else
  {
    // (FALSE == dc_s_ptr->is_access_channel_enabled) &&
    // (FALSE == dc_s_ptr->is_traffic_channel_enabled )
    // this shouldn't happen!
    ch_pref = WMS_CLIENT_CH_NONE;
  }

  return ch_pref;
} /* wms_get_client_default_channel_pref() */

/*=========================================================================
FUNCTION wms_get_client_default_service_option

DESCRIPTION
   Get service option preference based on current NV settings

DEPENDENCIES
  wms_nv_init() and wms_dc_init() must have been initialized first before
  calling this function.

RETURN VALUE
  default client service option preference

SIDE EFFECTS
  None
=========================================================================*/
wms_dc_so_e_type wms_get_client_default_service_option(
  void
)
{
  wms_dc_so_e_type dc_so;
  wms_dc_s_type *dc_s_ptr = wms_dc_s_ptr();

  dc_so = dc_s_ptr->default_so_from_nv;

  return dc_so;
} /* wms_get_client_default_service_option()*/
#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION wms_client_ptr_release                                    (INTERNAL)

DESCRIPTION
  Release the held pointer to client data, decrementing its reference count

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_client_ptr_release(
  wms_client_s_type *client_ptr
)
{
  if (NULL != client_ptr)
  {
    ref_cnt_obj_release(client_ptr);
  }

} /* wms_client_ptr_release() */

/*=========================================================================
FUNCTION wms_client_entry_dtor                                   (INTERNAL)

DESCRIPTION
  Destructor function for a client array entry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  * Deallocates the memory allocated to a client_ptr after the last
    reference has been released.  It is called as a result of the
    object's reference count becoming zero during a ref_cnt_obj_release()
    call.  This function is registered during ref_cnt_obj_init() when
    the object is initialized.

=========================================================================*/
void wms_client_entry_dtor
(
  void *pUser
)
{
  wms_client_s_type  *client_ptr = (wms_client_s_type *)pUser;

  // The final client reference has been released. Free the object.
  WMS_MEM_FREEIF(client_ptr);

} /* wms_client_entry_dtor() */

/*=========================================================================
FUNCTION wms_client_entry_set                                    (INTERNAL)

DESCRIPTION
  Assigns the client array entry for the associated client_id while under
  critical section protection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  * Internal client data for the associated client is allocated and
    inserted into the client array.

=========================================================================*/
wms_client_err_e_type wms_client_entry_set
(
  wms_client_id_type       client_id,
  wms_client_s_type      **client_ptr
)
{
  wms_client_s_type      *new_client_ptr;

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID_PTR;
  }

  if ((uint8)WMS_CLIENT_TYPE_MAX <= client_id)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  // Allocate
  new_client_ptr = (wms_client_s_type *) wms_mem_malloc(sizeof(wms_client_s_type));

  if (NULL == new_client_ptr)
  {
    return WMS_CLIENT_ERR_GENERAL;
  }

  // Allocated, now initialize ref count object.  Rest is already zeroed by malloc.
  ref_cnt_obj_init(new_client_ptr, wms_client_entry_dtor);

  {
    wms_client_s_type       *prev_client_ptr;

    rex_enter_crit_sect(&wms_client_crit_sect);

    prev_client_ptr = clients[client_id];   // Fetch current entry

    if (NULL == prev_client_ptr)            // Only assign if unused
    {
      clients[client_id] = new_client_ptr;  // Assign
      *client_ptr = new_client_ptr;
      ref_cnt_obj_add_ref(new_client_ptr);  // Addref (refs=2; *client_ptr, clients[])
    }

    rex_leave_crit_sect(&wms_client_crit_sect);

    if (NULL != prev_client_ptr)
    {
      // Issue warning if entry was already used and was not assigned
      MSG_ERROR_0("wms_client_entry_set(): attempt to set non-empty client entry");
    }
  }

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_entry_set() */

/*=========================================================================
FUNCTION wms_client_entry_unset                                  (INTERNAL)

DESCRIPTION
  Zeroes the client array entry for the associated client_id and releases
  a reference to the reference counted client object.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  * Internal client data for the associated client is no longer accessible.
  * The memory associated with this client may be deallocated (a reference
    is released).
  * The calling client can no longer interact with WMS until it has called
    wms_client_init() again.
  * Any commands from this client which are queued but not started may not
    be processed.  Commands already started should complete, however any
    notifications from those commands should not be expected to arrive.

=========================================================================*/
wms_client_err_e_type wms_client_entry_unset
(
  wms_client_id_type       client_id
)
{
  wms_client_s_type  *client_ptr;

  rex_enter_crit_sect(&wms_client_crit_sect);
  client_ptr = clients[client_id];
  clients[client_id] = NULL;
  rex_leave_crit_sect(&wms_client_crit_sect);

  if (NULL != client_ptr)
  {
    // Release the client reference obtained at initialization
    wms_client_ptr_release(client_ptr);
    return WMS_CLIENT_ERR_NONE;
  }

  return WMS_CLIENT_ERR_CLIENT_ID;
} /* wms_client_entry_unset() */

/*=========================================================================
FUNCTION wms_client_init                                   (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register itself with the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data initialized for the new client.

=========================================================================*/
wms_client_err_e_type wms_client_init
(
  wms_client_type_e_type   client_type,
  wms_client_id_type       *client_id_ptr
)
{
  wms_client_s_type     *client_ptr = NULL;
  wms_client_id_type     client_id = (wms_client_id_type)client_type;

  if (WMS_CLIENT_TYPE_MAX <= client_type)
  {
    // Client type out of range
    return WMS_CLIENT_ERR_CLIENT_TYPE;
  }

  if (NULL == client_id_ptr)
  {
    // Invalid client id ptr
    return WMS_CLIENT_ERR_CLIENT_ID_PTR;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)   // Client not allocated
  {
    wms_client_err_e_type  cl_error;

    // Create and put newly active client info into client array
    cl_error = wms_client_entry_set(client_id, &client_ptr);

    if (WMS_CLIENT_ERR_NONE != cl_error)
    {
      MSG_ERROR_1("wms_client_entry_set failed: %d", cl_error);
      return WMS_CLIENT_ERR_GENERAL;
    }

    // A ref-counted client_ptr is active here.
    // It has been zeroed from the malloc, and the ref_cnt_object initialized.
  }

  client_ptr->client_id        = client_id;

  // Re-initialize the client notification info
  client_ptr->cfg_event_mask   = 0;
  client_ptr->msg_event_mask   = 0;
  client_ptr->dc_event_mask    = 0;
  client_ptr->bc_event_mask    = 0;
  client_ptr->bc_mm_event_mask = 0;
  client_ptr->dbg_event_mask   = 0;

  client_ptr->source_port      = WMS_PORT_MATCH_IGNORED;
  client_ptr->dest_port        = WMS_PORT_MATCH_IGNORED;

  client_ptr->cfg_notify_type   = WMS_PM_NOTIFY_NONE;
  client_ptr->msg_notify_type   = WMS_PM_NOTIFY_NONE;
  client_ptr->dc_notify_type    = WMS_PM_NOTIFY_NONE;
  client_ptr->bc_notify_type    = WMS_PM_NOTIFY_NONE;
  client_ptr->bc_mm_notify_type = WMS_PM_NOTIFY_NONE;
  client_ptr->dbg_notify_type   = WMS_PM_NOTIFY_NONE;

  client_ptr->preferred_bearer = WMS_BEARER_MAX;
  client_ptr->allowed_bearer   = WMS_BEARER_MAX;
  client_ptr->last_used_bearer = WMS_BEARER_MAX;

#ifdef FEATURE_CDSMS
  {
    client_ptr->ch_pref      = wms_get_client_default_channel_pref();
    client_ptr->ch_pref_set  = FALSE;
    client_ptr->so           = wms_get_client_default_service_option();
    client_ptr->follow_on_dc = TRUE; 
    MSG_HIGH_3( "wms_client_init(): client_type = %d, default ch_pref = %d, default so = %d",
                (uint8)client_type, client_ptr->ch_pref, client_ptr->so);
  }
#else
    MSG_HIGH_1("wms_client_init(): client_type = %d", (uint8)client_type);
#endif /* FEATURE_CDSMS */

  ref_cnt_obj_release(client_ptr);

  // Return the client id
  *client_id_ptr = client_id;

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_init() */

/*=========================================================================
FUNCTION wms_client_init_for_pm                            (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register itself with the API for the purpose of
  power management.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data initialized for the new client.

=========================================================================*/
wms_client_err_e_type wms_client_init_for_pm
(
  wms_client_type_e_type       client_type,
  wms_client_id_type           *client_id_ptr,
  wms_client_processor_e_type  processor
)
{
  wms_client_s_type     *client_ptr = NULL;
  wms_client_id_type     client_id = (wms_client_id_type)client_type;


  if( client_type >= WMS_CLIENT_TYPE_MAX )
  {
    return WMS_CLIENT_ERR_CLIENT_TYPE;
  }

  if (NULL == client_id_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID_PTR;
  }

  if (WMS_CLIENT_PROCESSOR_NONE == processor ||
         WMS_CLIENT_PROCESSOR_MAX <= processor )
  {
    return WMS_CLIENT_ERR_INVALID_PROCESSOR;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)  // Client not allocated
  {
    wms_client_err_e_type  cl_error;

    // Create and put newly active client info into client array
    cl_error = wms_client_entry_set(client_id, &client_ptr);

    if (WMS_CLIENT_ERR_NONE != cl_error)
    {
      MSG_ERROR_1("wms_client_entry_set failed: %d", cl_error);
      return WMS_CLIENT_ERR_GENERAL;
    }

    // A ref-counted client_ptr is active here.
    // It has been zeroed from the malloc, and the ref_cnt_object initialized.
    client_ptr->processor = processor;
  }
  else
  {
    // Re-initialize the client notification info
    client_ptr->processor          = processor;
    client_ptr->cfg_event_mask     = 0;
    client_ptr->msg_event_mask     = 0;
    client_ptr->dc_event_mask      = 0;
    client_ptr->bc_event_mask      = 0;
    client_ptr->dbg_event_mask     = 0;
    client_ptr->bc_mm_event_mask   = 0;
    client_ptr->source_port        = WMS_PORT_MATCH_IGNORED;
    client_ptr->dest_port          = WMS_PORT_MATCH_IGNORED;
    client_ptr->cfg_notify_type    = WMS_PM_NOTIFY_NONE;
    client_ptr->msg_notify_type    = WMS_PM_NOTIFY_NONE;
    client_ptr->dc_notify_type     = WMS_PM_NOTIFY_NONE;
    client_ptr->bc_notify_type     = WMS_PM_NOTIFY_NONE;
    client_ptr->bc_mm_notify_type  = WMS_PM_NOTIFY_NONE;
    client_ptr->dbg_notify_type    = WMS_PM_NOTIFY_NONE;
  }

  ref_cnt_obj_release(client_ptr);

  // Return the client id
  *client_id_ptr = client_id;

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_init_for_pm() */


/*=========================================================================
FUNCTION wms_client_release                                (PUBLIC WMS API)

DESCRIPTION
  Indicates that this client no longer requires interaction with WMS.
  Any data associated with this client will be released and/or reset.
  The client must call wms_client_init() before making any other WMS API
  calls.

DEPENDENCIES
  None

RETURN VALUE
  WMS client error codes

SIDE EFFECTS
  * Internal client data for the associated client is dellocated/reset.
  * This client can no longer interact with WMS until a wms_client_init()
    call has been done.
  * Any commands from this client which are queued but not started may not
    be processed.  Commands already started should complete, however any
    notifications from those commands should not be expected to arrive.

=========================================================================*/
wms_client_err_e_type wms_client_release
(
  wms_client_id_type       client_id
)
{
  // Remove and release the client entry.
  // Once done, any queued WMS commands from this client will not be executed
  // because they will not be able to get a client pointer.  Any in-progress
  // WMS commands will continue to use the pointer they have. Eventually
  // all references will be released and the memory freed.
  return wms_client_entry_unset(client_id);

} /* wms_client_release() */


/*=========================================================================
FUNCTION wms_client_activate                               (PUBLIC WMS API)

DESCRIPTION
  The client tells the API that it is ready to use the services from the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
wms_client_err_e_type wms_client_activate
(
  wms_client_id_type       client_id
)
{
  wms_cmd_type          *cmd_ptr;
  wms_client_err_e_type  cl_error = WMS_CLIENT_ERR_NONE;
  wms_client_s_type     *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    // Set client active here because command processing requires it
    client_ptr->activated = TRUE;

    // Setup internal activation command.  No callbacks or user data.
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CLIENT_ACTIVATE;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_NONE;
    cmd_ptr->hdr.appInfo_ptr = NULL;

    // Enqueue the command.  The client will receive events when activation is complete
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    // Could not get a command buffer.  Client has not been activated.
    cl_error = WMS_CLIENT_ERR_GENERAL;
  }

  wms_client_ptr_release(client_ptr);

  return cl_error;

} /* wms_client_activate() */


/*=========================================================================
FUNCTION wms_client_deactivate                             (PUBLIC WMS API)

DESCRIPTION
  The client tells the API that it is not ready to use the services.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
wms_client_err_e_type wms_client_deactivate
(
  wms_client_id_type       client_id
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->activated = FALSE;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_deactivate() */


/*=========================================================================
FUNCTION wms_client_activate_bearer                        (PUBLIC WMS API)

DESCRIPTION
  Allow the client to activate a specified bearer.

DEPENDENCIES
  None

RETURN VALUE
  Client error code

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
wms_client_err_e_type wms_client_activate_bearer
(
  wms_client_id_type       client_id,
  wms_bearer_e_type        bearer
)
{
  wms_client_s_type  *client_ptr;

  if (WMS_BEARER_MAX <= bearer)
  {
    return WMS_CLIENT_ERR_UNSUPPORTED;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->active_bearers[bearer] = TRUE;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_activate_bearer() */


/*=========================================================================
FUNCTION wms_client_deactivate_bearer                      (PUBLIC WMS API)

DESCRIPTION
  Allow the client to deactivate a specified bearer.

DEPENDENCIES
  None

RETURN VALUE
  Client error code

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
wms_client_err_e_type wms_client_deactivate_bearer
(
  wms_client_id_type       client_id,
  wms_bearer_e_type        bearer
)
{
  wms_client_s_type  *client_ptr;

  if (bearer >= WMS_BEARER_MAX)
  {
    return WMS_CLIENT_ERR_UNSUPPORTED;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->active_bearers[bearer] = FALSE;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_deactivate_bearer() */

/*=========================================================================
FUNCTION wms_client_active_bearer                         (INTERNAL)

DESCRIPTION
     Deactivate the specified bearer according the the following criteria:

     1. bearer = WMS_BEARER_CDMA_EVDO
          Deactivate EVDO bearer if 1x voice call is active.
     2) bearer = WMS_BEARER_LTE
          Deactivate LTE bearer if there's no S102 tunnel enabled.

DEPENDENCIES
  Client must have been activated before calling this function.

RETURN VALUE
  TRUE - If the specified bearer is activated
  FALSE - If the specified bearer is deactived

SIDE EFFECTS
  No internal data changed if:
    1) client_poiter is NULL
    2) client has not been activated
    3) bearer value is not within range.

=========================================================================*/
boolean wms_client_active_bearer
(
  wms_client_s_type  *client_ptr,
  wms_bearer_e_type  bearer
)
{
  boolean st = FALSE;
  wms_dc_s_type *dc_s_ptr = wms_dc_s_ptr();
  wms_cfg_s_type *cfg_s_ptr = wms_cfg_s_ptr();

  if ( (NULL != client_ptr) && client_ptr->activated && (bearer < WMS_BEARER_MAX) &&
       (NULL != dc_s_ptr) && (NULL != cfg_s_ptr) )
  {
    if( (WMS_BEARER_CDMA_EVDO == bearer) && client_ptr->active_bearers[bearer] &&
             ((dc_s_ptr->call_active) || (dc_s_ptr->is_voice_data_call_conn)) )
    {
      MSG_HIGH_0("EVDO client bearer is active but not usable while 1x call is active");
      client_ptr->active_bearers[WMS_BEARER_CDMA_EVDO] = FALSE;
    }
    else if ( (WMS_BEARER_LTE == bearer) && client_ptr->active_bearers[bearer] &&
              !cfg_s_ptr->sms_over_s102 )
    {
      MSG_HIGH_0("LTE client bearer is active but not usable when S102 not enabled");
      client_ptr->active_bearers[WMS_BEARER_LTE] = FALSE;
    }

    st = client_ptr->active_bearers[bearer];
  }

  return st;
}

/*=========================================================================
FUNCTION wms_client_reg_cfg_cb                             (PUBLIC WMS API)

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
wms_client_err_e_type wms_client_reg_cfg_cb
(
  wms_client_id_type       client_id,
  wms_cfg_event_cb_type    cfg_event_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->cfg_event_cb = cfg_event_cb;
  // Set mask and type. Otherwise, if wms_client_reg_cfg_cb_for_pm() is
  // called before wms_client_reg_cfg_cb(), mask and type are left set
  // for pm.
  client_ptr->cfg_event_mask = 0;
  client_ptr->cfg_notify_type = WMS_PM_NOTIFY_NONE;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_cfg_cb() */

/*=========================================================================
FUNCTION wms_client_reg_cfg_cb_for_pm                      (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its configuration event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_cfg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         cfg_notify_type,
  uint32                       cfg_event_mask,
  wms_cfg_event_cb_type        cfg_event_cb
)
{
  wms_client_s_type  *client_ptr;

  if( WMS_PM_NOTIFY_MAX <= cfg_notify_type )
  {
    return WMS_CLIENT_ERR_INVALID_NOTIFY_TYPE;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return  WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->cfg_event_cb        = cfg_event_cb;
  client_ptr->cfg_event_mask      = cfg_event_mask;
  client_ptr->cfg_notify_type     = cfg_notify_type;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_cfg_cb_for_pm() */

/*=========================================================================
FUNCTION wms_client_reg_detail_cb                      (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register to recieve WMS information.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_detail_cb
(
  wms_client_id_type   client_id,
  boolean              req_detail
)
{
  wms_client_err_e_type st = WMS_CLIENT_ERR_NONE;
  wms_client_s_type  *client_ptr;

  MSG_HIGH_2( "wms_client_reg_detail_cb(): client_id = %d requested for WMS status info = %d",
              client_id,
              req_detail );

  if ( (uint8)WMS_CLIENT_TYPE_MAX <= client_id )
  {
    // Client id is out of range
    st = WMS_CLIENT_ERR_CLIENT_ID;    
  }
  else
  {
    client_ptr = wms_client_ptr_fetch(client_id);

    if (NULL == client_ptr)
    {
      st = WMS_CLIENT_ERR_CLIENT_ID;
    }
    else
    {
      client_ptr->send_wms_info = req_detail;
    }

    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }

  if (st != WMS_CLIENT_ERR_NONE)
  {
  	  MSG_ERROR_2( "wms_client_reg_detail_cb status of of client_id = %d is %d", client_id, st); 
  }

  return st;
} /* wms_client_reg_detail_cb() */

/*=========================================================================
FUNCTION wms_client_reg_msg_cb                             (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its message event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_msg_cb
(
  wms_client_id_type       client_id,
  wms_msg_event_cb_type    msg_event_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->msg_event_cb = msg_event_cb;
  // Set mask and type. Otherwise, if wms_client_reg_msg_cb_for_pm() is
  // called before wms_client_reg_msg_cb(), mask and type are left set
  // for pm.
  client_ptr->msg_event_mask = 0;
  client_ptr->msg_notify_type = WMS_PM_NOTIFY_NONE;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;

} /* wms_client_reg_msg_cb() */


/*=========================================================================
FUNCTION wms_client_reg_msg_cb_for_pm                      (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its Message event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_msg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         msg_notify_type,
  uint32                       msg_event_mask,
  wms_msg_event_cb_type        msg_event_cb
)
{
  wms_client_s_type  *client_ptr;

  if (WMS_PM_NOTIFY_MAX <= msg_notify_type)
  {
    return WMS_CLIENT_ERR_INVALID_NOTIFY_TYPE;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->msg_event_cb        = msg_event_cb;
  client_ptr->msg_event_mask      = msg_event_mask;
  client_ptr->msg_notify_type     = msg_notify_type;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_msg_cb_for_pm() */


/*=========================================================================
FUNCTION wms_client_reg_dc_cb                              (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its DC event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dc_cb
(
  wms_client_id_type       client_id,
  wms_dc_event_cb_type     dc_event_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->dc_event_cb = dc_event_cb;
  // Set mask and type. Otherwise, if wms_client_reg_dc_cb_for_pm() is
  // called before wms_client_reg_dc_cb(), mask and type are left set
  // for pm.
  client_ptr->dc_event_mask = 0;
  client_ptr->dc_notify_type = WMS_PM_NOTIFY_NONE;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_dc_cb() */

/*=========================================================================
FUNCTION wms_client_reg_dc_disconnect_cb                   (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its DC disconnect callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dc_disconnect_cb
(
  wms_client_id_type            client_id,
  wms_dc_disconnect_cb_type     dc_disconnect_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->dc_disconnect_cb = dc_disconnect_cb;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_dc_disconnect_cb() */

/*=========================================================================
FUNCTION wms_client_reg_transport_status_cb                (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register a callback function for transport layer status.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_transport_status_cb
(
  wms_client_id_type                 client_id,
  wms_msg_transport_status_cb_type   transport_status_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->transport_status_cb = transport_status_cb;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_transport_status_cb() */

/*=========================================================================
FUNCTION wms_client_reg_dc_cb_for_pm                       (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its DC event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dc_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         dc_notify_type,
  uint32                       dc_event_mask,
  wms_dc_event_cb_type         dc_event_cb
)
{
  wms_client_s_type  *client_ptr;

  if (WMS_PM_NOTIFY_MAX <= dc_notify_type)
  {
    return WMS_CLIENT_ERR_INVALID_NOTIFY_TYPE;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->dc_event_cb        = dc_event_cb;
  client_ptr->dc_event_mask      = dc_event_mask;
  client_ptr->dc_notify_type     = dc_notify_type;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_dc_cb_for_pm() */

/*=========================================================================
FUNCTION wms_client_reg_bc_mm_cb                           (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its Broadcast event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_bc_mm_cb
(
  wms_client_id_type       client_id,
  wms_bc_mm_event_cb_type     bc_mm_event_cb
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE

  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->bc_mm_event_cb = bc_mm_event_cb;
  // Set mask and type. Otherwise, if wms_client_reg_bc_mm_cb_for_pm() is
  // called before wms_client_reg_bc_mm_cb(), mask and type are left set
  // for pm.
  client_ptr->bc_mm_event_mask = 0;
  client_ptr->bc_mm_notify_type = WMS_PM_NOTIFY_NONE;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;

#else /* FEATURE_BROADCAST_SMS_MULTIMODE */

  return WMS_CLIENT_ERR_GENERAL;

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

} /* wms_client_reg_bc_mm_cb() */


/*=========================================================================
FUNCTION wms_client_reg_bc_mm_cb_for_pm                    (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its BC_mm event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_bc_mm_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         bc_mm_notify_type,
  uint32                       bc_mm_event_mask,
  wms_bc_mm_event_cb_type      bc_mm_event_cb
)
{
  wms_client_s_type  *client_ptr;

  if (WMS_PM_NOTIFY_MAX <= bc_mm_notify_type)
  {
    return WMS_CLIENT_ERR_INVALID_NOTIFY_TYPE;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->bc_mm_event_cb        = bc_mm_event_cb;
  client_ptr->bc_mm_event_mask      = bc_mm_event_mask;
  client_ptr->bc_mm_notify_type     = bc_mm_notify_type;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_bc_mm_cb_for_pm() */

/*=========================================================================
FUNCTION wms_client_reg_dbg_cb                             (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its Broadcast event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dbg_cb
(
  wms_client_id_type        client_id,
  wms_dbg_event_cb_type     dbg_event_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->dbg_event_cb = dbg_event_cb;
  // Set mask and type. Otherwise, if wms_client_reg_dbg_cb_for_pm() is
  // called before wms_client_reg_dbg_cb(), mask and type are left set
  // for pm.
  client_ptr->dbg_event_mask = 0;
  client_ptr->dbg_notify_type = WMS_PM_NOTIFY_NONE;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_dbg_cb() */


/*=========================================================================
FUNCTION wms_client_reg_dbg_cb_for_pm                      (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its Debug event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dbg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         dbg_notify_type,
  uint32                       dbg_event_mask,
  wms_dbg_event_cb_type        dbg_event_cb
)
{
  wms_client_s_type  *client_ptr;

  if (WMS_PM_NOTIFY_MAX <= dbg_notify_type)
  {
    return WMS_CLIENT_ERR_INVALID_NOTIFY_TYPE;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->dbg_event_cb        = dbg_event_cb;
  client_ptr->dbg_event_mask      = dbg_event_mask;
  client_ptr->dbg_notify_type     = dbg_notify_type;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_dbg_cb_for_pm() */



/*=========================================================================
FUNCTION wms_reg_cl_parse_msg_cb                           (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register its callback function for additional parsing
  of a message.

  If a null pointer is passed, the callback is de-registered. No parsing of the
  message will be administered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The message may not be shared amoung other clients, depending on the criteria
  of the parsing being administered to the message.
=========================================================================*/
wms_client_err_e_type wms_client_reg_parse_msg_cb
(
  wms_client_id_type               client_id,
  wms_cl_parse_msg_cb_type         msg_parsing_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->parse_msg_cb = msg_parsing_cb;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_reg_cl_parse_msg_cb */

/*=========================================================================
FUNCTION wms_client_reg_application_port                           (PUBLIC WMS API)

DESCRIPTION
  Allow the client to register with WMS with specific application port number 

 DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Internal data updated
=========================================================================*/
wms_client_err_e_type wms_client_reg_application_port
(
  wms_client_id_type               client_id,
  uint32                           dest_port,
  uint32                           source_port
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->dest_port = dest_port;
  client_ptr->source_port = source_port;

  wms_client_ptr_release(client_ptr);

  return WMS_CLIENT_ERR_NONE;
} /* wms_client_reg_application_port */


/*=========================================================================
FUNCTION wms_cl_init                                             (INTERNAL)

DESCRIPTION
  Initialize all client data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Internal data initialized.

=========================================================================*/
void wms_cl_init
(
  void
)
{
   int i;
   wms_client_id_type       client_id;

   // Initialize the client crit sect
   rex_init_crit_sect(&wms_client_crit_sect);

   rex_enter_crit_sect(&wms_client_crit_sect);
   for( i=0; i<(int)WMS_CLIENT_TYPE_MAX; i++ )
   {
      clients[i] = NULL;
   }
   rex_leave_crit_sect(&wms_client_crit_sect);

   if ( WMS_CLIENT_ERR_NONE == wms_client_init ( WMS_CLIENT_TYPE_INTERNAL,
                                                 &client_id) )
   {
      clients[WMS_CLIENT_TYPE_INTERNAL]->activated   = TRUE;
   }
   else
   {
      MSG_ERROR_0("Internal client initialization failed");
   }


   return;
} /* wms_cl_init */

/*=========================================================================
FUNCTION wms_client_release_all                                  (INTERNAL)

DESCRIPTION
  Release all client memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void wms_client_release_all(void)
{
  int i = 0;

  for( i=0; i<(int)WMS_CLIENT_TYPE_MAX; i++ )
  {
    (void) wms_client_entry_unset((wms_client_id_type)i);
  }
}

/*=========================================================================
FUNCTION wms_client_update_channel_pref                          (INTERNAL)

DESCRIPTION
  Updates the channel preference for clients which have not specified
  a preference with the current preference default.

DEPENDENCIES
  Only clients initialized by calling wms_client_init() and have
  not had a channel preference set via wms_client_set_channel_pref()
  are updated.

RETURN VALUE
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_UNSUPPORTED

SIDE EFFECTS
   None
=========================================================================*/
wms_client_err_e_type wms_client_update_channel_pref(
  void
)
{
  wms_client_err_e_type    st = WMS_CLIENT_ERR_NONE;
#ifdef FEATURE_CDSMS
  int                      client_idx;
  wms_client_s_type       *client_ptr;

  MSG_HIGH_0("wms_client_update_channel_pref()");

  for ( client_idx = 0; client_idx < (int)WMS_CLIENT_TYPE_MAX; client_idx++ )
  {
    client_ptr = wms_client_ptr_fetch((wms_client_id_type)client_idx);

    if ( NULL != client_ptr )
    {
      if (!client_ptr->ch_pref_set)
      {
        client_ptr->ch_pref = wms_get_client_default_channel_pref();
        MSG_HIGH_2( "wms_client_update_channel_pref(): client_id = %d, ch_pref = %d",
                    client_idx,
                    client_ptr->ch_pref );
      }
    }

    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }
#else /* FEATURE_CDSMS */
  st = WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_CDSMS */

  return st;
} /* wms_client_update_channel_pref() */


/*=========================================================================
FUNCTION wms_client_set_cdma_channel_pref

DESCRIPTION
  Allow a client to specify the channel preference for sending the SMS.

DEPENDENCIES
  Client must have been initialized by calling wms_client_init() first.

RETURN VALUE
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_CH_PREF
   - WMS_CLIENT_ERR_UNSUPPORTED

SIDE EFFECTS
   None
=========================================================================*/
wms_client_err_e_type wms_client_set_cdma_channel_pref(
  wms_client_id_type           client_id,
  wms_client_ch_pref_e_type    ch_pref
)
{
  wms_client_err_e_type st = WMS_CLIENT_ERR_NONE;

#ifdef FEATURE_CDSMS
  wms_client_s_type *client_ptr;

  MSG_HIGH_2( "wms_client_set_cdma_channel_pref(): client_id = %d, ch_pref = %d",
              client_id,
              ch_pref );

  if ( (uint8)WMS_CLIENT_TYPE_MAX <= client_id )
  {
    // Client id is out of range
    st = WMS_CLIENT_ERR_CLIENT_ID;

  }
  else if ( WMS_CLIENT_CH_PREF_MAX <= ch_pref )
  {
    // ch_pref is out of range
    st = WMS_CLIENT_ERR_CH_PREF;

  }
  else
  {
    client_ptr = wms_client_ptr_fetch(client_id);

    if ( NULL == client_ptr )
    {

      st = WMS_CLIENT_ERR_CLIENT_ID;
    }
    else
    {
      client_ptr->ch_pref = ch_pref;
    }

	 if ( st != WMS_CLIENT_ERR_NONE )
	 {
	   MSG_ERROR_3("wms_client_set_cdma_channel_pref status is %d for client_id = %d and client ptr = 0x%x", st, client_id, client_ptr );
	 }

    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }
#else
  st = WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_CDSMS */

  return st;
} /* wms_client_set_cdma_channel_pref() */

/*=========================================================================
FUNCTION wms_client_get_cdma_channel_pref

DESCRIPTION
  Allows API client to retrieves a previously registered channel preference.

DEPENDENCIES
  Client must have been initialized by calling wms_client_init() first.

RETURN VALUE
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_CH_PREF_PTR
   - WMS_CLIENT_ERR_UNSUPPORTED

SIDE EFFECTS
  None
=========================================================================*/
wms_client_err_e_type wms_client_get_cdma_channel_pref(
  wms_client_id_type           client_id,
  wms_client_ch_pref_e_type    *ch_pref
)
{
  wms_client_err_e_type st = WMS_CLIENT_ERR_NONE;

#ifdef FEATURE_CDSMS
  wms_client_s_type *client_ptr;

  MSG_HIGH_1("wms_client_get_cdma_channel_pref(): client_id = %d", client_id);

  if ( (uint8)WMS_CLIENT_TYPE_MAX <= client_id )
  {
    // Client id out of range
    st = WMS_CLIENT_ERR_CLIENT_ID;   
  }
  else if ( NULL == ch_pref )
  {     
     st = WMS_CLIENT_ERR_CH_PREF_PTR;
  }
  else
  {
    client_ptr = wms_client_ptr_fetch(client_id);
    if ( NULL == client_ptr )
    {      
      st = WMS_CLIENT_ERR_CLIENT_ID;
    }
    else
    {
      *ch_pref = client_ptr->ch_pref;
    }

	 if (st != WMS_CLIENT_ERR_NONE)
	 {
      MSG_ERROR_4("wms_client_get_cdma_channel_pref staus %d for client id = %d and ch_Prefptr @0x%x and client ptr @0x%x ",st , client_id, ch_pref, client_ptr); 
	 }

    MSG_HIGH_1("current ch_pref = %d", *ch_pref);
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }
#else
  st = WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_CDSMS */

  return st;
}  /* wms_client_get_cdma_channel_pref() */

/*=========================================================================
FUNCTION wms_client_set_cdma_service_option_pref

DESCRIPTION
  Allow a client to specify the service option preference for sending the SMS.

DEPENDENCIES
  Client must have been initialized by calling wms_client_init() first.

RETURN VALUE
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_SO_PREF
   - WMS_CLIENT_ERR_UNSUPPORTED

SIDE EFFECTS
   None
=========================================================================*/
wms_client_err_e_type wms_client_set_cdma_service_option_pref(
  wms_client_id_type           client_id,
  wms_dc_so_e_type             dc_so
)
{
  wms_client_err_e_type st = WMS_CLIENT_ERR_NONE;

#ifdef FEATURE_CDSMS
  wms_client_s_type *client_ptr = NULL;

  MSG_HIGH_2( "wms_client_set_cdma_service_option_pref : client_id = %d, so = %d",
              client_id,
              dc_so );

  if ((uint8)WMS_CLIENT_TYPE_MAX <= client_id)
  {
    // Client id is out of range
    st = WMS_CLIENT_ERR_CLIENT_ID;   
  }
  else if (WMS_DC_SO_MAX <= dc_so)
  {
    // SO is out of range
    st = WMS_CLIENT_ERR_SO_PREF;
  }
  else
  {
    client_ptr = wms_client_ptr_fetch(client_id);

    if (NULL == client_ptr)
    {
      st = WMS_CLIENT_ERR_CLIENT_ID;
    }
    else
    {
      client_ptr->so = dc_so;
    }

    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }
#else
  st = WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_CDSMS */

  MSG_HIGH_4( "wms_client_set_cdma_service_option_pref : client_id = %d, client ptr @0x%x, so = %d, status %d",
                client_id,
                client_ptr,
                dc_so,
                st );


  return st;
} /* wms_client_set_cdma_service_option_pref() */

/*=========================================================================
FUNCTION wms_client_get_cdma_service_option_pref

DESCRIPTION
  Allows API client to retrieve a previously registered service option
  preference.

DEPENDENCIES
  Client must have been initialized by calling wms_client_init() first.

RETURN VALUE
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_SO_PREF_PTR
   - WMS_CLIENT_ERR_UNSUPPORTED

SIDE EFFECTS
  None
=========================================================================*/
wms_client_err_e_type wms_client_get_cdma_service_option_pref(
  wms_client_id_type           client_id,
  wms_dc_so_e_type             *dc_so
)
{
  wms_client_err_e_type st = WMS_CLIENT_ERR_NONE;

#ifdef FEATURE_CDSMS
  wms_client_s_type *client_ptr = NULL;

  MSG_HIGH_1("wms_client_get_cdma_service_option_pref(): client_id = %d", client_id);

  if ((uint8)WMS_CLIENT_TYPE_MAX <= client_id)
  {
    // Client id out of range
    st = WMS_CLIENT_ERR_CLIENT_ID;
  }
  else if (NULL == dc_so)
  {

     st = WMS_CLIENT_ERR_SO_PREF_PTR;
  }
  else
  {
    client_ptr = wms_client_ptr_fetch(client_id);
    if (NULL == client_ptr)
    {

      st = WMS_CLIENT_ERR_CLIENT_ID;
    }
    else
    {
      *dc_so = client_ptr->so;
    }

    MSG_HIGH_1("current so = %d", *dc_so);
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }
#else
  st = WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_CDSMS */

  if (WMS_CLIENT_ERR_NONE == st)
  {
     MSG_HIGH_4("wms_client_get_cdma_service_option_pref status= %d for client id= %d and service optionp= %d and clientptr @0x%x", st, client_id, *dc_so, client_ptr); 
  }
  else
  {
    MSG_ERROR_4("wms_client_get_cdma_service_option_pref status= %d for client id= %d and service optionpte= @0x%x and clientptr @0x%x", st, client_id, dc_so, client_ptr); 
  }
  return st;
}  /* wms_client_get_cdma_service_option_pref() */

/*=========================================================================
FUNCTION wms_client_set_cdma_service_follow_on_dc_pref

DESCRIPTION
  Allow a client to specify the follow on DC preference for sending the SMS.

DEPENDENCIES
  Client must have been initialized by calling wms_client_init() first.

RETURN VALUE
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_UNSUPPORTED
 
SIDE EFFECTS
   None
=========================================================================*/
wms_client_err_e_type wms_client_set_cdma_service_follow_on_dc_pref(
  wms_client_id_type           client_id,
  boolean                      follow_on_dc
)
{
  wms_client_err_e_type st = WMS_CLIENT_ERR_NONE;

#ifdef FEATURE_CDSMS
  wms_client_s_type *client_ptr;

  MSG_HIGH_2("wms_client_set_cdma_service_follow_on_dc_pref : client_id = %d, follow_on_dc = %d", 
            client_id, follow_on_dc);

  if ((uint8)WMS_CLIENT_TYPE_MAX <= client_id)
  {
    // Client id is out of range
    st = WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr = wms_client_ptr_fetch(client_id);

  if (NULL == client_ptr)
  {
    st = WMS_CLIENT_ERR_CLIENT_ID;
  }
  else
  {
    client_ptr->follow_on_dc = follow_on_dc;
  }

  WMS_CLIENT_PTR_RELEASEIF(client_ptr);
#else
  st = WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_CDSMS */

  if (st != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR_3("wms_client_set_cdma_service_follow_on_dc_pref status= %d for client id= %d with clientptr @0x%x", st, client_id, client_ptr); 	
  }

  return st;
} /* wms_client_set_cdma_service_follow_ond_dc_pref() */


/*=========================================================================
FUNCTION wms_client_get_cdma_service_follow_on_dc_pref

DESCRIPTION
  Allows API client to retrieve a previously registered service option
  preference.
  
DEPENDENCIES
  Client must have been initialized by calling wms_client_init() first.

RETURN VALUE
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_FDC_PREF_PTR       
   - WMS_CLIENT_ERR_UNSUPPORTED
 
SIDE EFFECTS 
  None 
=========================================================================*/
wms_client_err_e_type wms_client_get_cdma_service_follow_on_dc_pref(
  wms_client_id_type           client_id,
  boolean                     *follow_on_dc
)
{
  wms_client_err_e_type st = WMS_CLIENT_ERR_NONE;

#ifdef FEATURE_CDSMS
  wms_client_s_type *client_ptr = NULL;

  MSG_HIGH_1("wms_client_get_cdma_service_follow_on_dc_pref(): client_id = %d", client_id);

  if ((uint8)WMS_CLIENT_TYPE_MAX <= client_id)
  {
    // Client id out of range
    st = WMS_CLIENT_ERR_CLIENT_ID;

  }
  else if (NULL == follow_on_dc)
  {
     st = WMS_CLIENT_ERR_FDC_PREF_PTR;
  }
  else 
  {
    client_ptr = wms_client_ptr_fetch(client_id);    
    if (NULL == client_ptr)
    {   
      st = WMS_CLIENT_ERR_CLIENT_ID;
    }
    else 
    {
      *follow_on_dc = client_ptr->follow_on_dc;
    }

    MSG_HIGH_1("current follow_on_dc = %d", *follow_on_dc);
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }
#else
  st = WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_CDSMS */

  if (st != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR_4("wms_client_get_cdma_service_follow_on_dc_pref status= %d with follow_on_dc @0x%x for client id= %d with clientptr @0x%x", st, follow_on_dc, client_id, client_ptr); 	
  }

  return st;
}  /* wms_client_get_cdma_service_follow_on_dc_pref() */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wmscl.c */
