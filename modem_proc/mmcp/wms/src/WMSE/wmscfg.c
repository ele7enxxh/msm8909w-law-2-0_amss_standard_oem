/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmscfg.c -- Configuration Group

  The WMS module which implements the User API for SMS. This source file
  implements the configuration group functions which process the client
  commands of the configuration group.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmscfg.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "wms_msgr_msg.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmscfg.h"
#include "wmscl.h"
#include "msg.h"
#include "nv.h"
#include "err.h"
#include "wmssim.h"
#include "wmsnv.h"
#include "string.h"
#include "wmsts.h"
#include "wmsmsg.h"
#include "wmsdbg.h"
#include "wmstsi.h"
#include "wmsutils.h"
#include "wmsbc.h"
#include "wmscm.h"
#include "stringl.h"

#ifdef FEATURE_CDSMS
#include "wmstscdma.h"
#include "wmsbc.h"
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

#include "wmscm_gw.h"

#endif /* FEATURE_GWSMS */


/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
#ifdef FEATURE_GWSMS
LOCAL rex_crit_sect_type                sts_rpt_crit_sect;
#endif /* FEATURE_GWSMS */

/* Define a special outbound buffer type to isolate WMS client data
 * from internal WMS data.  This is done to protect from two specific
 * scenarios:
 *    1) Data tampering by clients causing other clients to receive bad data
 *    2) Clients calling free() on WMS buffers causing WMS to crash later
 *
 * This buffer will be used to hold data passed to the client during the callbacks.
 * Solution for 1) For each client callback called, the data will be re-copied
 *                 into the buffer to ensure each client receives un-tainted data.
 * Solution for 2) The buffer structure is set up so that there is never a valid
 *                 pointer for a client to free().  This will cause any client
 *                 attempting to free to cause an exception in the client's context.
 */
typedef struct wms_cfg_outbound_event_s
{
  uint32                     unused_offset_do_not_remove;
  wms_cfg_event_info_s_type  ev_info;
} wms_cfg_outbound_event_s_type;

/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_cfg_s_ptr

DESCRIPTION
  Return the pointer to the configuration data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal configuration data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_cfg_s_type * wms_cfg_s_ptr(
  void
)
{
  static wms_cfg_s_type cfg;

  return &cfg;
} /* wms_cfg_s_ptr() */


/*===========================================================================
FUNCTION wms_cfg_filter_events

DESCRIPTION
  Decide whether this event can be sent to the client or not based on
  power management settings.

DEPENDENCIES
  none

RETURN VALUE
  TRUE:  OK to send event to client
  FALSE: Not OK to send event to client

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_filter_events(
  wms_client_id_type       client_id,
  wms_client_s_type       *client_ptr,
  wms_cfg_event_e_type     event
)
{
  boolean   ok_to_send_event = FALSE;

  MSG_HIGH_3( "CFG: client_id=%d, event=%d, notify type = %d",
              client_id,
              event,
              client_ptr->cfg_notify_type );

  if( WMS_PM_NOTIFY_ALL == client_ptr->cfg_notify_type )
  {
    ok_to_send_event = TRUE;
  }
  else if( (WMS_PM_NOTIFY_CUSTOM_FILTER == client_ptr->cfg_notify_type) &&
           ( client_ptr->cfg_event_mask & ((uint32)1 << (uint32)event) ) )
  {
    ok_to_send_event = TRUE;
  }
  else if( WMS_PM_NOTIFY_DEFAULT_FILTER == client_ptr->cfg_notify_type )
  {
    switch( event )
    {
      case WMS_CFG_EVENT_GW_READY:
      case WMS_CFG_EVENT_CDMA_READY:
      case WMS_CFG_EVENT_MS_READY:
      case WMS_CFG_EVENT_MEMORY_FULL:
      case WMS_CFG_EVENT_MS_MEMORY_FULL:
      case WMS_CFG_EVENT_CB_ERROR:
      case WMS_CFG_EVENT_MS_NOT_READY:
        ok_to_send_event = TRUE;
        break;

      default:
        break;
    }
  }
  else if ( WMS_PM_NOTIFY_NONE == client_ptr->cfg_notify_type )
  {
     ok_to_send_event = TRUE;
  }

  if( FALSE == ok_to_send_event )
  {
    MSG_HIGH_2( "Blocking cfg event %d for client %d",
                event,
                client_id );
  }

  return ok_to_send_event;
} /* wms_cfg_filter_events() */

/*===========================================================================
FUNCTION wms_cfg_prep_and_copy_ob_cfg_event

DESCRIPTION
  Make a heap-based copy of the outbound event data provided to the function
  wms_cfg_event_notify() into a buffer to be sent to clients. The copy is
  made into a specific outbound data structure which allows the data sent to
  the client via callback to be completely disregarded after returning from
  the callback. The buffer is organized in such a fashion so that client
  tampering with the buffer does not impact WMS.

  If a buffer is not allocated yet, it will be allocated before the copy.
  If the buffer is already allocated, it will be zeroed.

DEPENDENCIES
  none

RETURN VALUE
  TRUE:  OK to send event to client
  FALSE: Not OK to send event to client

SIDE EFFECTS
  An outbound buffer may be allocated and returned to the caller. This
  buffer must be freed by the caller when appropriate.
===========================================================================*/
STATIC boolean wms_cfg_prep_and_copy_ob_cfg_event(
  wms_cfg_event_e_type            event,
  wms_cfg_event_info_s_type      *cfg_event_info_ptr,
  wms_cfg_outbound_event_s_type **ob_evt
)
{
  if ( (NULL == ob_evt) || (NULL == cfg_event_info_ptr) )
  {
    return FALSE;
  }

  // Allocate a new buffer or zero the old
  if (NULL == *ob_evt)
  {
    *ob_evt = wms_mem_malloc(sizeof(wms_cfg_outbound_event_s_type));
  }

  if (NULL == *ob_evt)
  {
    MSG_HIGH_0("Malloc failed");
    return FALSE;
  }
  else {
    memset(*ob_evt, 0, sizeof(wms_cfg_outbound_event_s_type) );
  }

  // Copy event
  (void)memscpy( &((*ob_evt)->ev_info),
                 sizeof(wms_cfg_event_info_s_type),
                 cfg_event_info_ptr,
                 sizeof(wms_cfg_event_info_s_type));

  // Cfg events do not have any additional data (externally
  // referencing pointers) to be copied
  (void)event;

  return TRUE;
} /* wms_cfg_prep_and_copy_ob_cfg_event() */

/*===========================================================================
FUNCTION   wms_is_detail_info_event

DESCRIPTION
  Check if the event is for WMS status information.

DEPENDENCIES
  None

RETURNS
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean wms_is_detail_info_event(
  wms_cfg_event_e_type event
)
{
   if ((WMS_CFG_EVENT_MS_SIM_INIT_START == event) ||
       (WMS_CFG_EVENT_MS_SIM_INIT_END   == event) ||
       (WMS_CFG_EVENT_MS_REFRESH_START  == event) ||
       (WMS_CFG_EVENT_MS_REFRESH_END    == event) ||
       (WMS_CFG_EVENT_MS_SERV_AVAIL     == event) ||
       (WMS_CFG_EVENT_MS_SERV_NOT_AVAIL == event))
   {
     return TRUE;
   }
   else
   {
     return FALSE;
   }
}

/*===========================================================================
FUNCTION wms_cfg_event_notify_internal

DESCRIPTION
  Notify configuration event to the clients having client ID within the client ID range input
  to this function.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_event_notify_internal(
  wms_cfg_event_e_type         event,
  wms_cfg_event_info_s_type   *cfg_event_info_ptr,
  uint8                        client_idx_start,
  uint8                        client_idx_end
)
{
  wms_client_s_type             *client_ptr;
  uint8                          i;
  wms_cfg_outbound_event_s_type *ob_evt = NULL;   // Outbound event data
  uint8                          client_array[16]={0};//taking 16 as, it is enough to hold all client
  uint8                          client_count = 0;

  {
    sys_modem_as_id_e_type  as_id = SYS_MODEM_AS_ID_NONE;
    char                   *evt_str;

    switch (event)
    {
    case WMS_CFG_EVENT_ROUTES:
      as_id = cfg_event_info_ptr->routes.as_id;
      break;
    case WMS_CFG_EVENT_MEMORY_STATUS:
      as_id = cfg_event_info_ptr->memory_status.as_id;
      break;
    case WMS_CFG_EVENT_MESSAGE_LIST:
      as_id = cfg_event_info_ptr->message_list.as_id;
      break;
    case WMS_CFG_EVENT_PRIMARY_CLIENT_SET:
      as_id = cfg_event_info_ptr->set_primary.as_id;
      break;
    case WMS_CFG_EVENT_LINK_CONTROL:
      as_id = cfg_event_info_ptr->link_control.as_id;
      break;
    case WMS_CFG_EVENT_MS_READY:
      as_id = cfg_event_info_ptr->ms_ready.as_id;
      break;
    case WMS_CFG_EVENT_MS_MEMORY_FULL:
      as_id = cfg_event_info_ptr->ms_memory_full.as_id;
      break;
    case WMS_CFG_EVENT_MS_GW_DOMAIN_PREF:
      as_id = cfg_event_info_ptr->ms_gw_domain_pref.as_id;
      break;
    case WMS_CFG_EVENT_MS_CELL_CHANGE:
      as_id = cfg_event_info_ptr->ms_cell_change.as_id;
      break;
    case WMS_CFG_EVENT_MS_MEMORY_STATUS_SET:
      as_id = cfg_event_info_ptr->ms_memory_status_set.as_id;
      break;
    case WMS_CFG_EVENT_MSG_WAITING:
      as_id = cfg_event_info_ptr->msg_wait_info.as_id;
      break;
    case WMS_CFG_EVENT_MS_NOT_READY:
      as_id = cfg_event_info_ptr->ms_not_ready.as_id;
      break;
    case WMS_CFG_EVENT_MS_SIM_INIT_START:
      as_id = cfg_event_info_ptr->begin_sim_init.as_id;
      break;
    case WMS_CFG_EVENT_MS_SIM_INIT_END:
      as_id = cfg_event_info_ptr->end_sim_init.as_id;
      break;
    case WMS_CFG_EVENT_MS_REFRESH_START:
      as_id = cfg_event_info_ptr->begin_refresh.as_id;
      break;
    case WMS_CFG_EVENT_MS_REFRESH_END:
      as_id = cfg_event_info_ptr->end_refresh.as_id;
      break;
    case WMS_CFG_EVENT_MS_SERV_AVAIL:
      as_id = cfg_event_info_ptr->svc_avail.as_id;
      break;
    case WMS_CFG_EVENT_MS_SERV_NOT_AVAIL:
      as_id = cfg_event_info_ptr->svc_not_avail.as_id;
      break;
    case WMS_CFG_EVENT_GW_READY:
    case WMS_CFG_EVENT_CDMA_READY:
    case WMS_CFG_EVENT_MEMORY_FULL:
    case WMS_CFG_EVENT_GW_DOMAIN_PREF:
    case WMS_CFG_EVENT_CELL_CHANGE:
    case WMS_CFG_EVENT_MEMORY_STATUS_SET:
    case WMS_CFG_EVENT_CB_ERROR:
      // These are non-DSDS events
      break;
    default:
      break;
    }

    evt_str = wms_dbg_get_cfg_event_string((uint16)event);

    MSG_SPRINTF_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s (as_id %d)",
                   event,
                   evt_str,
                   as_id );
  }

  /* Notify clients
  */
  for (i = client_idx_start; i <= client_idx_end; i++ )
  {
    client_ptr = wms_client_ptr_fetch( (wms_client_id_type) i );

    if ( (NULL != client_ptr) &&
         (NULL != client_ptr->cfg_event_cb) &&
         (client_ptr->activated == TRUE) )
    {
      if (wms_is_detail_info_event(event) && !client_ptr->send_wms_info)
      {
        //Don't send this event to client.
        WMS_CLIENT_PTR_RELEASEIF(client_ptr);
        continue;
      }

      if (TRUE == wms_cfg_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        // Make copy of outbound data FOR EACH CLIENT. The event data cannot be trusted
        // after a client has touched it. This way clients cannot corrupt the downstream
        // client notification data and any data, buffers, or pointers do not have to be
        // trusted or valid after the callback notification and the copy can be simply discarded.
        if ( wms_cfg_prep_and_copy_ob_cfg_event(event, cfg_event_info_ptr, &ob_evt) && (NULL != ob_evt) )
        {
          /* Send event to this client */
          client_ptr->cfg_event_cb( event, &ob_evt->ev_info );
        }
	    // store client in array
		client_array[client_count] = i;
		client_count++;
      }
    }
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }
  
   // print client ID
  MSG_HIGH_8("WMS Clients(Part 1)(NON Zero event send) %d %d %d %d %d %d %d %d ", client_array[0],
                                                  client_array[1],client_array[2],client_array[3],
                                                  client_array[4],client_array[5],client_array[6],
                                                                                  client_array[7]);
  
  MSG_HIGH_8("WMS Clients(Part 2)(NON Zero event send) %d %d %d %d %d %d %d %d ", client_array[8],
                                                client_array[9],client_array[10],client_array[11],
                                                client_array[12],client_array[13],client_array[14],
                                                                                  client_array[15]);

  WMS_MEM_FREEIF(ob_evt);

  return;
} /* wms_cfg_event_notify_internal() */

/*===========================================================================
FUNCTION wms_cfg_event_notify()                                    (INTERNAL)

DESCRIPTION
  Notify all clients of a configuration event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_event_notify(
  wms_cfg_event_e_type         event,
  wms_cfg_event_info_s_type   *cfg_event_info_ptr
)
{
  wms_cfg_event_notify_internal(event, cfg_event_info_ptr, 0, WMS_CLIENT_TYPE_MAX);

  return;
} /* wms_cfg_event_notify() */

/*===========================================================================
FUNCTION wms_cfg_event_notify_single_client()                      (INTERNAL)

DESCRIPTION
  Notify a single client of a configuration event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void wms_cfg_event_notify_single_client(
  wms_cfg_event_e_type         event,
  wms_cfg_event_info_s_type   *cfg_event_info_ptr,
  wms_client_id_type           client_id
)
{
  wms_cfg_event_notify_internal(event, cfg_event_info_ptr, client_id, client_id);

  return;
}  /* wms_cfg_event_notify_single_client() */

/*===========================================================================
FUNCTION wms_cfg_send_ready_event_ex()                             (INTERNAL)

DESCRIPTION
  Sends the appropriate ready event to clients based on the subscription
  information provided.

  appInfo_ptr:      [IN]  Pointer to subscription info
  dest_client:      [IN]  Specifies the client to send the event to.
                          WMS_CLIENT_ALL (-1) specifies all clients.
                          To send to a single client, use a valid client_id
                          as defined in wms_client_type_e_type.
  send_ready        [IN]  Boolean to specify the type of ready event to send
                          TRUE:  WMS_CFG_[CDMA|GW|MS]_READY
                          FALSE: WMS_CFG_[CDMA|GW|MS]_NOT_READY

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_send_ready_event_ex(
  wms_prov_app_info_s_type   *appInfo_ptr,
  int                         dest_client,
  boolean                     send_ready
)
{
  wms_cfg_event_e_type         ready_event;
  wms_cfg_event_info_s_type   *cfg_event_ptr = NULL;

  do
  {
    boolean session_is_valid = TRUE;

    if (!wms_check_appinfo_is_valid(appInfo_ptr))
    {
      break;
    }

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL == cfg_event_ptr)
    {
      MSG_ERROR_0("WMS_CFG_EVENT_(CDMA|GW|MS)_READY not sent: memory allocation failed");
      break;
    }

    switch (appInfo_ptr->mmgsdi_appInfo.session_type)
    {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_GW_PROV_TER_SESSION:
      ready_event                      = WMS_CFG_EVENT_GW_READY;
      cfg_event_ptr->ms_ready.sys_type = WMS_CFG_SYS_GW;
      cfg_event_ptr->ms_ready.as_id    = appInfo_ptr->asid;
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      ready_event                      = WMS_CFG_EVENT_CDMA_READY;
      cfg_event_ptr->ms_ready.sys_type = WMS_CFG_SYS_CDMA;
      cfg_event_ptr->ms_ready.as_id    = appInfo_ptr->asid;
      break;
    default:
      session_is_valid = FALSE;
      break;
    }

    if (!session_is_valid)
    {
      MSG_ERROR_1("WMS_CFG_EVENT_(CDMA|GW|MS)_READY not sent: Invalid session %d", appInfo_ptr->mmgsdi_appInfo.session_type);
      break;
    }

    MSG_HIGH_6( "Sending READY event %d: appInfo_ptr 0x%x, as_id %d, sys_type %d, session_type %d, dest_client %d",
                ready_event,
                appInfo_ptr,
                appInfo_ptr->asid,
                cfg_event_ptr->ms_ready.sys_type,
                appInfo_ptr->mmgsdi_appInfo.session_type,
                dest_client );

    if (WMS_CLIENT_ALL == dest_client)
    { // Send to all clients

      if (send_ready)
      {
        // Send non-DS READY event to old clients that expect this event */
        wms_cfg_event_notify( ready_event, cfg_event_ptr );

        // Send DS READY event
        wms_cfg_event_notify( WMS_CFG_EVENT_MS_READY, cfg_event_ptr );
      }
      else
      {
        // Send DS NOT_READY event (non-DS event does not exist)
        wms_cfg_event_notify( WMS_CFG_EVENT_MS_NOT_READY, cfg_event_ptr );
      }
    }
    else
    { // Send to single client
      wms_client_id_type  client_id;

      // Convert dest_client to client_id (int to uint8 conversion here)
      client_id = (wms_client_id_type)dest_client;

      if (send_ready)
      {
        // Send non-DS READY event to old clients that expect this event */
        wms_cfg_event_notify_single_client( ready_event, cfg_event_ptr, client_id );

        // Send DS READY event
        wms_cfg_event_notify_single_client( WMS_CFG_EVENT_MS_READY, cfg_event_ptr, client_id );
      }
      else
      {
        // Send DS NOT_READY event (non-DS event does not exist)
        wms_cfg_event_notify_single_client( WMS_CFG_EVENT_MS_NOT_READY, cfg_event_ptr, client_id );
      }
    }
  } while(0);

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_send_ready_event_ex() */

/*===========================================================================
FUNCTION wms_cfg_send_ready_event()                                (INTERNAL)

DESCRIPTION
  Sends the appropriate ready event to clients based on the subscription
  information provided.

  appInfo_ptr:      [IN]  Pointer to subscription info
  dest_client:      [IN]  Specifies the client to send the event to.
                          WMS_CLIENT_ALL (-1) specifies all clients.
                          To send to a single client, use a valid client_id
                          as defined in wms_client_type_e_type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_send_ready_event(
  wms_prov_app_info_s_type   *appInfo_ptr,
  int                         dest_client
)
{
  wms_cfg_send_ready_event_ex(appInfo_ptr, dest_client, TRUE);

  return;
} /* wms_cfg_send_ready_event() */

/*===========================================================================
FUNCTION wms_cfg_send_not_ready_event()                            (INTERNAL)

DESCRIPTION
  Sends the appropriate not ready event to clients based on the subscription
  information provided.

  appInfo_ptr:      [IN]  Pointer to subscription info
  dest_client:      [IN]  Specifies the client to send the event to.
                          WMS_CLIENT_ALL (-1) specifies all clients.
                          To send to a single client, use a valid client_id
                          as defined in wms_client_type_e_type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_send_not_ready_event(
  wms_prov_app_info_s_type   *appInfo_ptr,
  int                         dest_client
)
{
  wms_cfg_send_ready_event_ex(appInfo_ptr, dest_client, FALSE);

  return;
} /* wms_cfg_send_not_ready_event() */

/*===========================================================================
FUNCTION wms_cfg_set_routes_proc

DESCRIPTION
  Process the request of setting message routes.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  NV is updated.
===========================================================================*/
void wms_cfg_set_routes_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err    = WMS_CMD_ERR_NONE;
  wms_routes_s_type          *routes_ptr = &cmd_ptr->cmd.cfg_set_routes.routes;
  wms_routes_s_type           new_routes;
  int                         route_idx;
  wms_cfg_s_type             *cfg_s_ptr  = wms_cfg_s_ptr();
  boolean                     routes_change = FALSE;

  MSG_HIGH_2( "wms_cfg_set_routes_proc(client %d, as_id %d)",
              cmd_ptr->hdr.client_id,
              cmd_ptr->hdr.as_id );

  if ( (SYS_MODEM_AS_ID_NONE >= cmd_ptr->hdr.as_id) || (MAX_AS_IDS <= cmd_ptr->hdr.as_id) )
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_AS_ID );
    return;
  }

  // Copy existing routes and update with the new routes
  new_routes = cfg_s_ptr->routes[cmd_ptr->hdr.as_id];

#ifdef FEATURE_GWSMS
  if (new_routes.transfer_status_report != routes_ptr->transfer_status_report)
  {
    new_routes.transfer_status_report = routes_ptr->transfer_status_report;
    routes_change = TRUE;
  }
#endif /* FEATURE_GWSMS */

  for (route_idx = 0; route_idx < (uint8)WMS_MESSAGE_CLASS_MAX; route_idx++)
  {
    if (WMS_ROUTE_MAX <= routes_ptr->pp_routes[route_idx].route)
    {
      cmd_err = WMS_CMD_ERR_CFG_ROUTE;
      break;
    }

    if (WMS_MEMORY_STORE_MAX <= routes_ptr->pp_routes[route_idx].mem_store)
    {
      cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      break;
    }

    if (WMS_ROUTE_NO_CHANGE != routes_ptr->pp_routes[route_idx].route)
    {
      if ( (new_routes.pp_routes[route_idx].route != routes_ptr->pp_routes[route_idx].route) ||
           (new_routes.pp_routes[route_idx].mem_store != routes_ptr->pp_routes[route_idx].mem_store) )
      {
        new_routes.pp_routes[route_idx] = routes_ptr->pp_routes[route_idx];
        routes_change = TRUE;
      }

      MSG_HIGH_3( "Class Type=%d, Route Type = %d, Mem Store Type=%d",
                  route_idx,
                  new_routes.pp_routes[route_idx].route,
                  new_routes.pp_routes[route_idx].mem_store );
    }
  }

  /* Write the new routes to NV if it has changed
  */
  if ((WMS_CMD_ERR_NONE == cmd_err) && routes_change)
  {
    if (FALSE == wms_nv_write_routing(& new_routes))
    {
      cmd_err = WMS_CMD_ERR_CFG_NV_WRITE;
    }
  }

  MSG_HIGH_3( "Send command status(client %d): routes_change = %d, cmd_err = %d",
              cmd_ptr->hdr.client_id, routes_change, cmd_err );

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    // Update configuration
    cfg_s_ptr->routes[cmd_ptr->hdr.as_id] = new_routes;

    // Notify
    {
      wms_cfg_event_info_s_type         *cfg_event_info_ptr = NULL;

      cfg_event_info_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

      if (NULL != cfg_event_info_ptr)
      {
        // Set up event and send
        cfg_event_info_ptr->routes.as_id           = cmd_ptr->hdr.as_id;
        cfg_event_info_ptr->routes                 = new_routes;
        cfg_event_info_ptr->routes.client_id       = WMS_CLIENT_TYPE_INTERNAL; // client_id set to Internal so that cfg event gets notified to all clients

        wms_cfg_event_notify(WMS_CFG_EVENT_ROUTES, cfg_event_info_ptr);
      }
      else
      {
        MSG_ERROR_0("WMS_CFG_EVENT_ROUTES not sent: memory allocation failed");
      }

      WMS_MEM_FREEIF(cfg_event_info_ptr);
    }
  }
  else
  {
    MSG_ERROR_1("wms_cfg_set_routes_proc failed, error=%d", cmd_err);
  }

  return;
} /* wms_cfg_set_routes_proc() */


/*===========================================================================
FUNCTION wms_cfg_get_routes_proc

DESCRIPTION
  Process the request of retrieving message routes.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_get_routes_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cfg_s_type             *cfg_s_ptr  = wms_cfg_s_ptr();

  MSG_HIGH_2( "wms_cfg_get_routes_proc(client %d, asid %d)",
              cmd_ptr->hdr.client_id,
              cmd_ptr->hdr.as_id );

  if ( (SYS_MODEM_AS_ID_NONE >= cmd_ptr->hdr.as_id) || (MAX_AS_IDS <= cmd_ptr->hdr.as_id) )
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_AS_ID );
    return;
  }

  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

  {
    wms_cfg_event_info_s_type         *cfg_event_info_ptr = NULL;

    cfg_event_info_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL != cfg_event_info_ptr)
    {
      // Set up event information
      cfg_event_info_ptr->routes           = cfg_s_ptr->routes[cmd_ptr->hdr.as_id];
      cfg_event_info_ptr->routes.as_id     = cmd_ptr->hdr.as_id;
      cfg_event_info_ptr->routes.client_id = cmd_ptr->hdr.client_id;

      wms_cfg_event_notify_single_client( WMS_CFG_EVENT_ROUTES, cfg_event_info_ptr, cfg_event_info_ptr->routes.client_id );
    }
    else
    {
      MSG_ERROR_0("WMS_CFG_EVENT_ROUTES not sent: memory allocation failed");
    }

    WMS_MEM_FREEIF(cfg_event_info_ptr);
  }

  return;
} /* wms_cfg_get_routes_proc() */


/*===========================================================================
FUNCTION wms_cfg_get_mem_status_proc

DESCRIPTION
  Process the request of retrieving memory status of a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_get_mem_status_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type         cmd_err     = WMS_CMD_ERR_NONE;
  wms_memory_store_e_type    mem_store   = cmd_ptr->cmd.cfg_get_mem_status.mem_store;
  wms_message_tag_e_type     tag         = cmd_ptr->cmd.cfg_get_mem_status.tag;
  wms_prov_app_info_s_type  *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  MSG_HIGH_3( "wms_cfg_get_mem_status_proc(client %d, as_id %d, mem_store %d)",
              cmd_ptr->hdr.client_id,
              cmd_ptr->hdr.as_id,
              mem_store );

  do
  {
    if (NULL == appInfo_ptr)
    {
       cmd_err = WMS_CMD_ERR_NULL_PTR;
       break;
    }

    switch (mem_store)
    {
#ifdef FEATURE_CDSMS
    case WMS_MEMORY_STORE_RUIM:
    case WMS_MEMORY_STORE_NV_CDMA:
      break;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_NV_GW:
      break;

    case WMS_MEMORY_STORE_RAM_GW:
      if( tag == WMS_TAG_MO_TEMPLATE )
      {
         cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      }
      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    case WMS_MEMORY_STORE_RAM_CDMA:
    case WMS_MEMORY_STORE_CSIM:
#endif /* FEATURE_CDSMS */
    case WMS_MEMORY_STORE_NONE:
    default:
      cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      break;
    }

    if (WMS_CMD_ERR_NONE != cmd_err)
    {
      break; // out of while()
    }

    if (!WMS_IS_VALID_TAG(tag))
    {
      cmd_err = WMS_CMD_ERR_MSG_TAG;
    }
  } while (0);

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if (cmd_err == WMS_CMD_ERR_NONE)
  {
    wms_cfg_do_memory_status(appInfo_ptr, mem_store, tag, cmd_ptr);
  }

  return;
} /* wms_cfg_get_mem_status_proc() */


/*===========================================================================
FUNCTION wms_cfg_do_message_list

DESCRIPTION
  Build data for message list event from a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_message_list(
  wms_prov_app_info_s_type   *appInfo_ptr,
  wms_memory_store_e_type     mem_store,
  wms_message_tag_e_type      tag,
  wms_cmd_type               *cmd_ptr
)
{
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();
  wms_cfg_event_info_s_type  *cfg_event_ptr = NULL;

  MSG_LOW_2( "wms_cfg_do_message_list(): mem store %d, tag %d",
             mem_store,
             tag );

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  if (!asid_is_valid(appInfo_ptr->asid))
  {
    return;
  }
  
  cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

  if (NULL != cfg_event_ptr)
  {
    // Set up common event information
    cfg_event_ptr->message_list.len       = 0;
    cfg_event_ptr->message_list.mem_store = mem_store;
    cfg_event_ptr->message_list.tag       = tag;
    cfg_event_ptr->message_list.as_id     = appInfo_ptr->asid;

    switch( mem_store )
    {
#ifdef FEATURE_CDSMS
    case WMS_MEMORY_STORE_RAM_CDMA:
      /*
      for (i = 0; i < WMS_MAX_RAM_MESSAGES; i++)
      {
        if ( WMS_TAG_NONE == cfg_s_ptr->ram_cdma_sms[i].status )
        {
         continue;
        }

        if ( (WMS_TAG_NONE == tag) ||
             (tag == (cfg_s_ptr->ram_cdma_sms[i].status & 0x7)) )
        {
          cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len] = i;
          cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]    = cfg_s_ptr->ram_cdma_sms[i].status & 0x7;
          cfg_event_ptr->message_list.len++;
        }
      }
      */
      break;

    case WMS_MEMORY_STORE_CSIM:
    case WMS_MEMORY_STORE_RUIM:
      {
        wms_message_index_type      i;

        cfg_event_ptr->message_list.voice_mail_index  = cfg_s_ptr->cdma_voice_mail_index;
        cfg_event_ptr->message_list.voice_mail_active = cfg_s_ptr->cdma_voice_mail_active;

        for (i = 0; i < (uint32)appInfo_ptr->sms_rec_count; i++)
        {
          if (WMS_TAG_NONE == appInfo_ptr->sms_rec_tags[i])
          {
            continue;
          }

          if ( (WMS_TAG_NONE == tag) ||
               (tag == appInfo_ptr->sms_rec_tags[i]) )
          {
            cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len] = i;
            cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]    = appInfo_ptr->sms_rec_tags[i];
            cfg_event_ptr->message_list.len++;
          }
        }
      }
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
      {
        wms_message_index_type      i;

        cfg_event_ptr->message_list.voice_mail_index  = cfg_s_ptr->cdma_voice_mail_index;
        cfg_event_ptr->message_list.voice_mail_active = cfg_s_ptr->cdma_voice_mail_active;

        for (i = 0; (i < WMS_MAX_NV_CDMA_MESSAGES) && (i < cfg_s_ptr->nv_cdma_max_slots); i++)
        {
          if (WMS_TAG_NONE == cfg_s_ptr->nv_cdma_tags[i])
          {
           continue;
          }

          if ( (WMS_TAG_NONE == tag) ||
               (tag == cfg_s_ptr->nv_cdma_tags[i]) )
          {
            cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len] = i;
            cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]    = cfg_s_ptr->nv_cdma_tags[i];
            cfg_event_ptr->message_list.len++;
          }
        }
      }
      break;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_RAM_GW:
      {
        wms_message_index_type      ram_idx;
        uint32                      lower_bound = 0;
        uint32                      upper_bound = WMS_MAX_RAM_MESSAGES;

        cfg_event_ptr->message_list.voice_mail_index  = appInfo_ptr->voice_mail_index;
        cfg_event_ptr->message_list.voice_mail_active = appInfo_ptr->voice_mail_active;

        if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
        {
          lower_bound = appInfo_ptr->asid * WMS_MAX_RAM_MESSAGES / MAX_AS_IDS;
          upper_bound = lower_bound + WMS_MAX_RAM_MESSAGES / MAX_AS_IDS;
        }

        for (ram_idx = lower_bound;  ram_idx < upper_bound; ram_idx++)
        {
          if (NULL != cfg_s_ptr->ram_gw_sms[ram_idx])
          {
            wms_message_tag_e_type      ram_tag;

            ram_tag = (wms_message_tag_e_type)(cfg_s_ptr->ram_gw_sms[ram_idx]->status & 0x7);

            if ( (WMS_TAG_NONE == tag) ||  // If searching for all,
                 (tag == ram_tag) )        // or search tag matches RAM tag
            {
              // Add messages to list

              // Translate the global message list index into the sub-list index
              cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len] = ram_idx - lower_bound;
              cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]    = ram_tag;
              cfg_event_ptr->message_list.len++;
            }
          }
        }
      }
      break;

    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      {
        wms_message_index_type      i;

        cfg_event_ptr->message_list.voice_mail_index  = appInfo_ptr->voice_mail_index;
        cfg_event_ptr->message_list.voice_mail_active = appInfo_ptr->voice_mail_active;

        for (i = 0;  i < (uint32)appInfo_ptr->sms_rec_count; i++)
        {
          if (WMS_TAG_NONE == appInfo_ptr->sms_rec_tags[i])
          {
           continue;
          }

          if ( (WMS_TAG_NONE == tag) ||
               (tag == appInfo_ptr->sms_rec_tags[i]) )
          {
            cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len] = i;
            cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]    = appInfo_ptr->sms_rec_tags[i];
            cfg_event_ptr->message_list.len ++;
          }
        }
      }
      break;

    case WMS_MEMORY_STORE_NV_GW:
      {
        wms_message_index_type      msg_idx;
        uint32                      lower_bound = 0;
        uint32                      upper_bound = 0;

        cfg_event_ptr->message_list.voice_mail_index  = appInfo_ptr->voice_mail_index;
        cfg_event_ptr->message_list.voice_mail_active = appInfo_ptr->voice_mail_active;

        upper_bound = cfg_s_ptr->nv_gw_max_slots;
        upper_bound = MIN(WMS_MAX_NV_GW_MESSAGES, cfg_s_ptr->nv_gw_max_slots);

        if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
        {
          lower_bound = appInfo_ptr->asid * cfg_s_ptr->nv_gw_max_slots / MAX_AS_IDS;
          upper_bound = lower_bound + cfg_s_ptr->nv_gw_max_slots / MAX_AS_IDS;
        }

        for (msg_idx = lower_bound; msg_idx < upper_bound; msg_idx++)
        {
          wms_message_tag_e_type      nv_tag;

          nv_tag = cfg_s_ptr->nv_gw_tags[msg_idx];

          // Ignore any messages that are not in use
          if (WMS_TAG_NONE == nv_tag)
          {
           continue;
          }

          if ( (WMS_TAG_NONE == tag) ||    // If searching for all,
               (tag == nv_tag) )           // or search tag matches message tag
          {
            // Add messages to list

            // Translate the global message list index into the sub-list index
            cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len]   = msg_idx - lower_bound;
            cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]      = nv_tag;
            cfg_event_ptr->message_list.len++;
          }
        } // for

      }
      break;

#endif /* FEATURE_GWSMS */

    default:
      break;
    }

    if (NULL != cmd_ptr && WMS_CMD_CFG_GET_MESSAGE_LIST == cmd_ptr->hdr.cmd_id)
    {
      /* cmd_ptr is not NULL which means that this event is generated as a result of a client requested command.
         A client requested command that doesn't do any update will be notified only to the client who requested
         for that command. */
      cfg_event_ptr->message_list.client_id = cmd_ptr->hdr.client_id;
      wms_cfg_event_notify_single_client( WMS_CFG_EVENT_MESSAGE_LIST, cfg_event_ptr, cfg_event_ptr->message_list.client_id );
    }
    else
    {
      cfg_event_ptr->message_list.client_id = WMS_CLIENT_TYPE_INTERNAL;
      wms_cfg_event_notify( WMS_CFG_EVENT_MESSAGE_LIST, cfg_event_ptr );
    }
  }
  else
  {
    MSG_ERROR_0("WMS_CFG_EVENT_MESSAGE_LIST not sent: memory allocation failed");
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_do_message_list() */



/*===========================================================================
FUNCTION wms_cfg_do_template_list

DESCRIPTION
  Build data for template list event from a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_template_list(
  wms_prov_app_info_s_type  *appInfo_ptr,
  wms_memory_store_e_type    mem_store,
  wms_cmd_type              *cmd_ptr
)
{
  wms_cfg_event_info_s_type *cfg_event_ptr = NULL;

  MSG_HIGH_1("wms_cfg_do_template_list(): mem_store = %d", mem_store);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

  if (NULL != cfg_event_ptr)
  {
    // Set up common event information
    cfg_event_ptr->message_list.len       = 0;
    cfg_event_ptr->message_list.mem_store = mem_store;
    cfg_event_ptr->message_list.tag       = WMS_TAG_MO_TEMPLATE;
    cfg_event_ptr->message_list.as_id     = appInfo_ptr->asid;

    switch (mem_store)
    {
#ifdef FEATURE_CDSMS
    case WMS_MEMORY_STORE_NV_CDMA:
      cfg_event_ptr->message_list.indices[0] = 0;
      cfg_event_ptr->message_list.tags[0]    = WMS_TAG_MO_TEMPLATE;
      cfg_event_ptr->message_list.len        = 1;
      break;

    case WMS_MEMORY_STORE_RUIM:
    case WMS_MEMORY_STORE_CSIM:
      {
        wms_message_index_type     msg_idx       = 0;

        for (msg_idx = 0; msg_idx < (uint32)appInfo_ptr->smsp_rec_count; msg_idx++)
        {
          if (TRUE == appInfo_ptr->smsp_rec_templates[msg_idx])
          {
            cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len] = msg_idx;
            cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]    = WMS_TAG_MO_TEMPLATE;
            cfg_event_ptr->message_list.len++;
          }
        }
      }
      break;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
    case WMS_MEMORY_STORE_NV_GW:
      cfg_event_ptr->message_list.indices[0] = 0;
      cfg_event_ptr->message_list.tags[0]    = WMS_TAG_MO_TEMPLATE;
      cfg_event_ptr->message_list.len        = 1;
      break;

    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_USIM:
      {
        wms_message_index_type     msg_idx       = 0;

        for (msg_idx = 0; msg_idx < (uint32)appInfo_ptr->smsp_rec_count; msg_idx++)
        {
          if (appInfo_ptr->smsp_rec_templates[msg_idx] == TRUE)
          {
            cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len] = msg_idx;
            cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]    = WMS_TAG_MO_TEMPLATE;
            cfg_event_ptr->message_list.len++;
          }
        }
      }
      break;
#endif /* FEATURE_GWSMS */

    default:
      break;
    }

    if (NULL != cmd_ptr && WMS_CMD_CFG_GET_MESSAGE_LIST == cmd_ptr->hdr.cmd_id)
    {
      /* cmd_ptr is not NULL which means that this event is generated as a result of a client requested command.
         A client requested command that doesn't do any update will be notified only to the client who requested
         for that command. */
      cfg_event_ptr->message_list.client_id = cmd_ptr->hdr.client_id;
      wms_cfg_event_notify_single_client( WMS_CFG_EVENT_MESSAGE_LIST, cfg_event_ptr, cfg_event_ptr->message_list.client_id );
    }
    else
    {
      cfg_event_ptr->message_list.client_id = WMS_CLIENT_TYPE_INTERNAL;
      wms_cfg_event_notify( WMS_CFG_EVENT_MESSAGE_LIST, cfg_event_ptr );
    }
  }
  else
  {
    MSG_ERROR_0("WMS_CFG_EVENT_MESSAGE_LIST not sent: memory allocation failed");
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_do_template_list() */

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_do_sts_report_list

DESCRIPTION
  Build data for Status Report list event from a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_sts_report_list(
  wms_prov_app_info_s_type   *appInfo_ptr,
  wms_memory_store_e_type     mem_store,
  wms_cmd_type               *cmd_ptr
)
{
  wms_cfg_event_info_s_type  *cfg_event_ptr = NULL;

  MSG_HIGH_1("wms_cfg_do_sts_report_list() mem store %d", mem_store);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return;
  }

  if (!asid_is_valid(appInfo_ptr->asid))
  {
    return;
  }

  cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

  if (NULL != cfg_event_ptr)
  {
    // Set up common event information
    cfg_event_ptr->message_list.len       = 0;
    cfg_event_ptr->message_list.mem_store = mem_store;
    cfg_event_ptr->message_list.tag       = WMS_TAG_STATUS_RPT;
    cfg_event_ptr->message_list.as_id     = appInfo_ptr->asid;

    switch (mem_store)
    {
    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_USIM:
      {
        wms_message_index_type      msg_idx       = 0;

        for (msg_idx = 0; msg_idx < (uint32)appInfo_ptr->smsr_rec_count; msg_idx++)
        {
          if (appInfo_ptr->smsr_status_rpts[msg_idx] == TRUE)
          {
            cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len] = msg_idx;
            cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]    = WMS_TAG_STATUS_RPT;
            cfg_event_ptr->message_list.len++;
          }
        }
      }
      break;

    case WMS_MEMORY_STORE_RAM_GW:
      {
        wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();
        wms_message_index_type      msg_idx;
        uint32                      lower_bound   = 0;
        uint32                      upper_bound   = WMS_MAX_RAM_STATUS_REPORTS;

        if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
        {
          lower_bound = appInfo_ptr->asid * WMS_MAX_RAM_STATUS_REPORTS / MAX_AS_IDS;
          upper_bound = lower_bound + WMS_MAX_RAM_STATUS_REPORTS / MAX_AS_IDS;
        }

        for (msg_idx = lower_bound;  msg_idx < upper_bound; msg_idx++)
        {
          if (cfg_s_ptr->ram_status_rpts[msg_idx] == TRUE)
          {
            // Translate the global message list index into the sub-list index
            cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len] = msg_idx - lower_bound;
            cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len] = WMS_TAG_STATUS_RPT;
            cfg_event_ptr->message_list.len++;
          }
        }
      }
      break;

    default:
      break;
    }

    if (NULL != cmd_ptr && WMS_CMD_CFG_GET_MESSAGE_LIST == cmd_ptr->hdr.cmd_id)
    {
      /* cmd_ptr is not NULL which means that this event is generated as a result of a client requested command.
         A client requested command that doesn't do any update will be notified only to the client who requested
         for that command. */
      cfg_event_ptr->message_list.client_id = cmd_ptr->hdr.client_id;
      wms_cfg_event_notify_single_client( WMS_CFG_EVENT_MESSAGE_LIST, cfg_event_ptr, cfg_event_ptr->message_list.client_id );
    }
    else
    {
      cfg_event_ptr->message_list.client_id = WMS_CLIENT_TYPE_INTERNAL;
      wms_cfg_event_notify( WMS_CFG_EVENT_MESSAGE_LIST, cfg_event_ptr );
    }
  }
  else
  {
    MSG_ERROR_0("WMS_CFG_EVENT_ROUTES not sent: memory allocation failed");
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_do_sts_report_list() */
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_cfg_get_msg_list_proc

DESCRIPTION
  Process the request of retrieving message list for a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_get_msg_list_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type         cmd_err     = WMS_CMD_ERR_NONE;
  wms_memory_store_e_type    mem_store   = cmd_ptr->cmd.cfg_get_msg_list.mem_store;
  wms_message_tag_e_type     tag         = cmd_ptr->cmd.cfg_get_msg_list.tag;
  wms_prov_app_info_s_type  *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("wms_cfg_get_msg_list_proc()");

  switch( mem_store )
  {
#ifdef FEATURE_CDSMS
    //case WMS_MEMORY_STORE_RAM_CDMA,
    case WMS_MEMORY_STORE_RUIM:
      if (NULL == appInfo_ptr)
      {
         cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      }
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
      break;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_USIM:
      if (NULL == appInfo_ptr)
      {
         cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      }
      break;

    case WMS_MEMORY_STORE_RAM_GW:
      if( tag == WMS_TAG_MO_TEMPLATE )
      {
         cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      }
      break;

    case WMS_MEMORY_STORE_NV_GW:
      break;
#endif /* FEATURE_GWSMS */

    default:
      cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      break;
  }

  if (( tag == WMS_TAG_STATUS_RPT )
      &&( mem_store != WMS_MEMORY_STORE_SIM
      &&  mem_store != WMS_MEMORY_STORE_USIM ))
  {
    cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
  }

  if (!WMS_IS_VALID_TAG(cmd_ptr->cmd.cfg_get_msg_list.tag))
  {
    cmd_err = WMS_CMD_ERR_MSG_TAG;
  }

  wms_client_cmd_status( cmd_ptr, cmd_err);

  if (cmd_err == WMS_CMD_ERR_NONE)
  {
    if( tag == WMS_TAG_MO_TEMPLATE )
    {
      wms_cfg_do_template_list(appInfo_ptr, mem_store, cmd_ptr);
    }
#ifdef FEATURE_GWSMS
    else if( tag == WMS_TAG_STATUS_RPT )
    {
      wms_cfg_do_sts_report_list(appInfo_ptr, mem_store, cmd_ptr);
    }
#endif /* FEATURE_GWSMS */
    else
    {
      wms_cfg_do_message_list(appInfo_ptr, mem_store, tag, cmd_ptr);
    }
  }

  /* done */
  return;

} /* wms_cfg_get_msg_list_proc() */


#ifdef FEATURE_GWSMS

/*===========================================================================
FUNCTION wms_cfg_mm_set_domain_pref

DESCRIPTION
  Sets the GW Domain Preference to MM Layer

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_cfg_mm_set_domain_pref
(
  sys_modem_as_id_e_type     as_id,
  wms_gw_domain_pref_e_type  gw_domain_pref
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return (WMS_CMD_ERR_NONE == wms_cm_gw_bearer_selection(
     as_id,
     gw_domain_pref));
} /* wms_cfg_mm_set_domain_pref() */

/*===========================================================================
FUNCTION wms_cfg_set_gw_domain_pref_proc

DESCRIPTION
  Set the GW Domain Preference Procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_set_gw_domain_pref_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err        = WMS_CMD_ERR_NONE;
  wms_gw_domain_pref_e_type   gw_domain_pref = cmd_ptr->cmd.cfg_set_gw_domain_pref.gw_domain_pref;
  wms_prov_app_info_s_type   *appInfo_ptr    = cmd_ptr->hdr.appInfo_ptr;
  wms_cfg_event_info_s_type  *cfg_event_ptr  = NULL;

  MSG_HIGH_0("wms_cfg_set_gw_domain_pref_proc()");

  do
  {
    if (NULL == appInfo_ptr)
    {
      cmd_err = WMS_CMD_ERR_AS_ID;
      break;
    }

    if (gw_domain_pref >= WMS_GW_DOMAIN_PREF_MAX)
    {
      cmd_err = WMS_CMD_ERR_CFG_DOMAIN_PREF;
      break;
    }

    appInfo_ptr->gw_domain_pref = gw_domain_pref;

    // Write new pref value to NV
    if( wms_nv_write_gw_domain_pref(appInfo_ptr->asid, gw_domain_pref) == FALSE )
    {
      cmd_err = WMS_CMD_ERR_CFG_NV_WRITE;
      break;
    }

    // Send domain pref change to MM
    if( wms_cfg_mm_set_domain_pref(appInfo_ptr->asid, gw_domain_pref) == FALSE )
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
      break;
    }

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL == cfg_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
      break;
    }

  } while(0);

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    cfg_event_ptr->gw_domain_pref                = gw_domain_pref;
    wms_cfg_event_notify( WMS_CFG_EVENT_GW_DOMAIN_PREF, cfg_event_ptr );

    cfg_event_ptr->ms_gw_domain_pref.domain_pref = gw_domain_pref;
    cfg_event_ptr->ms_gw_domain_pref.as_id       = appInfo_ptr->asid;
    cfg_event_ptr->ms_gw_domain_pref.client_id   = WMS_CLIENT_TYPE_INTERNAL;
    wms_cfg_event_notify( WMS_CFG_EVENT_MS_GW_DOMAIN_PREF, cfg_event_ptr );
  }
  else
  {
    MSG_ERROR_1("WMS_CFG_EVENT_(MS_)GW_DOMAIN_PREF not sent: error %d", cmd_err);
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_set_gw_domain_pref_proc() */



/*===========================================================================
FUNCTION wms_cfg_get_gw_domain_pref_proc

DESCRIPTION
  Get the GW Domain Preference Procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_get_gw_domain_pref_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err       = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type   *appInfo_ptr   = cmd_ptr->hdr.appInfo_ptr;
  wms_cfg_event_info_s_type  *cfg_event_ptr = NULL;

  MSG_HIGH_0("wms_cfg_get_gw_domain_pref_proc()");

  do
  {
    if (NULL == appInfo_ptr)
    {
      cmd_err = WMS_CMD_ERR_AS_ID;
      break;
    }

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL == cfg_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
      break;
    }

  } while(0);

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    if (WMS_CLIENT_TYPE_QMI != cmd_ptr->hdr.client_id)
    {
      cfg_event_ptr->gw_domain_pref                = appInfo_ptr->gw_domain_pref;
      wms_cfg_event_notify_single_client( WMS_CFG_EVENT_GW_DOMAIN_PREF, cfg_event_ptr, cmd_ptr->hdr.client_id );

      cfg_event_ptr->ms_gw_domain_pref.domain_pref = appInfo_ptr->gw_domain_pref;
      cfg_event_ptr->ms_gw_domain_pref.as_id       = appInfo_ptr->asid;
      cfg_event_ptr->ms_gw_domain_pref.client_id   = cmd_ptr->hdr.client_id;
      wms_cfg_event_notify_single_client( WMS_CFG_EVENT_MS_GW_DOMAIN_PREF, cfg_event_ptr, cfg_event_ptr->ms_gw_domain_pref.client_id );
    }

    if (WMS_CLIENT_TYPE_QMI == cmd_ptr->hdr.client_id)
    {
      wms_cfg_s_type  *cfg_s_ptr = wms_cfg_s_ptr();

      cfg_event_ptr->ms_gwl_domain_pref.gw.as_id         = appInfo_ptr->asid;
      cfg_event_ptr->ms_gwl_domain_pref.gw.client_id     = cmd_ptr->hdr.client_id;
      cfg_event_ptr->ms_gwl_domain_pref.gw.domain_pref   = appInfo_ptr->gw_domain_pref;
      cfg_event_ptr->ms_gwl_domain_pref.lte_domain_pref  = cfg_s_ptr->lte_domain_pref;
      wms_cfg_event_notify_single_client( WMS_CFG_EVENT_MS_GWL_DOMAIN_PREF, cfg_event_ptr, cfg_event_ptr->ms_gwl_domain_pref.gw.client_id );
    }
  }
  else
  {
    MSG_ERROR_1("WMS_CFG_EVENT_(MS_)GW(L)_DOMAIN_PREF not sent: error %d", cmd_err);
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_get_gw_domain_pref_proc() */

#endif /* FEATURE_GWSMS */


#ifdef FEATURE_DOMAIN_SELECTION
/*===========================================================================
FUNCTION wms_cfg_set_lte_domain_pref_proc

DESCRIPTION
  Set the GW Domain Preference Procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_set_lte_domain_pref_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type  cmd_err = WMS_CMD_ERR_NONE;

  MSG_HIGH_0("wms_cfg_set_lte_domain_pref_proc()");

  if (cmd_ptr->cmd.cfg_set_lte_domain_pref.lte_domain_pref >= SYS_SMS_DOMAIN_PREF_MAX)
  {
    cmd_err = WMS_CMD_ERR_CFG_DOMAIN_PREF;
    MSG_HIGH_1("Invalid lte_domain_pref %d", cmd_ptr->cmd.cfg_set_lte_domain_pref.lte_domain_pref );  
  }
  else
  {
    wms_cfg_s_type                           *cfg_s_ptr = wms_cfg_s_ptr();
    cm_domain_sel_config_update_req_msg_type  cm_lte_domain_pref;


    cfg_s_ptr->lte_domain_pref = cmd_ptr->cmd.cfg_set_lte_domain_pref.lte_domain_pref;
    msgr_init_hdr((msgr_hdr_struct_type *)&cm_lte_domain_pref,
                  MSGR_WMS_CM,
                  MM_DOM_SEL_CONFIG_UPDATE_LTE_SMS_DOMAIN_PREF_REQ);
  

    cm_lte_domain_pref.data.config                 = CM_DOM_SEL_CONFIG_TYPE_LTE_SMS_DOM_PREF;
    cm_lte_domain_pref.data.value.lte_sms_dom_pref = cmd_ptr->cmd.cfg_set_lte_domain_pref.lte_domain_pref;

    if (E_SUCCESS != msgr_send((msgr_hdr_struct_type*)&cm_lte_domain_pref,
                                sizeof(cm_domain_sel_config_update_req_msg_type)))
    {
      cmd_err = WMS_CMD_ERR_CFG_DOMAIN_PREF;
      MSG_HIGH_0("Error: msgr_send()");
    }
  } 

  wms_client_cmd_status( cmd_ptr, cmd_err );
  return;
} /* wms_cfg_set_lte_domain_pref_proc() */
#endif /* FEATURE_DOMAIN_SELECTION */
/*===========================================================================
FUNCTION wms_cfg_set_primary_client_proc

DESCRIPTION
  Set the Primary Client GW Domain Preference Procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_set_primary_client_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  boolean                     use_client_memory;
  boolean                     set_primary;
  wms_prov_app_info_s_type   *appInfo_ptr;
  wms_cfg_event_info_s_type  *cfg_event_ptr = NULL;
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();

  use_client_memory = cmd_ptr->cmd.cfg_set_primary_client.use_client_memory;
  set_primary       = cmd_ptr->cmd.cfg_set_primary_client.set_primary;
  appInfo_ptr       = cmd_ptr->hdr.appInfo_ptr;

  MSG_HIGH_0("wms_cfg_set_primary_client_proc()");

  do
  {
    if (NULL == appInfo_ptr)
    {
      cmd_err = WMS_CMD_ERR_AS_ID;
      break;
    }

    /* Check for Valid Client */
    if (cmd_ptr->hdr.client_id >= (uint8)WMS_CLIENT_TYPE_MAX)
    {
      cmd_err = WMS_CMD_ERR_CLIENT_ID;
      break;
    }

    if (set_primary == TRUE)
    {
      /* Register a Primary Client */
      MSG_HIGH_1("Setting Primary Client: %d", cmd_ptr->hdr.client_id);
      appInfo_ptr->primary_client     = cmd_ptr->hdr.client_id;    /* Register */
      appInfo_ptr->use_client_memory  = use_client_memory;
      appInfo_ptr->client_memory_full = FALSE;
    }
    else
    {
      /* De-Register a Primary Client */
      if (appInfo_ptr->primary_client != cmd_ptr->hdr.client_id)
      {
        /* Only Primary Client can De-Register Itself */
        cmd_err = WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT;
        break;
      }
      else
      {
        MSG_HIGH_1("Resetting Primary Client: %d", cmd_ptr->hdr.client_id);
        appInfo_ptr->primary_client     = (uint8)WMS_CLIENT_TYPE_MAX;  /*DeRegistered */
        appInfo_ptr->use_client_memory  = FALSE;
        appInfo_ptr->client_memory_full = FALSE;
      }
    }

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL == cfg_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
      break;
    }

  } while(0);

  /* Notify Command Error in Command Callback */
  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    /* Event Information */
    cfg_event_ptr->set_primary.as_id             = appInfo_ptr->asid;

    /* Handling the case of client trying to be a primary client before WMS is intialized.
    */
    if ((SYS_MODEM_AS_ID_NONE == appInfo_ptr->asid) && (WMS_DEVICE_MODE_SS == cfg_s_ptr->device_mode))
    {
      cfg_event_ptr->set_primary.as_id = SYS_MODEM_AS_ID_1;
    }

    cfg_event_ptr->set_primary.set_primary       = set_primary;
    cfg_event_ptr->set_primary.use_client_memory = use_client_memory;

    /* Notify Event */
    wms_cfg_event_notify( WMS_CFG_EVENT_PRIMARY_CLIENT_SET, cfg_event_ptr );
  }
  else
  {
    MSG_ERROR_1("WMS_CFG_EVENT_PRIMARY_CLIENT_SET not sent: error %d", cmd_err);
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_set_primary_client_proc() */

/*===========================================================================
FUNCTION wms_cfg_set_memory_full_proc

DESCRIPTION
  Set the Memory Full Status for the Primary Client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_set_memory_full_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  boolean                     memory_full;
  wms_prov_app_info_s_type   *appInfo_ptr;
  wms_cfg_event_info_s_type  *cfg_event_ptr = NULL;

  memory_full = cmd_ptr->cmd.cfg_set_memory_full.memory_full;
  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  MSG_HIGH_0("wms_cfg_set_memory_full_proc()");

  do
  {
    if (NULL == appInfo_ptr)
    {
      cmd_err = WMS_CMD_ERR_AS_ID;
      break;
    }

    if (cmd_ptr->hdr.client_id >= (uint8)WMS_CLIENT_TYPE_MAX)
    {
      cmd_err = WMS_CMD_ERR_CLIENT_ID;
      break;
    }

    /* Set / Reset Memory Full for Primary Client */
    if (appInfo_ptr->primary_client != cmd_ptr->hdr.client_id)
    {
     /* Only Primary Client can Set / Reset Memory Full */
      cmd_err = WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT;
      break;
    }

    appInfo_ptr->client_memory_full = memory_full;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL == cfg_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }

  } while(0);

  /* Notify Command Error in Command Callback */
  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    cfg_event_ptr->memory_full = memory_full;
    wms_cfg_event_notify( WMS_CFG_EVENT_MEMORY_STATUS_SET, cfg_event_ptr );

    cfg_event_ptr->ms_memory_status_set.memory_full = memory_full;
    cfg_event_ptr->ms_memory_status_set.as_id       = appInfo_ptr->asid;
    wms_cfg_event_notify( WMS_CFG_EVENT_MS_MEMORY_STATUS_SET, cfg_event_ptr );

#ifdef FEATURE_GWSMS
    /* Send SMMA to the Network */
    wms_msg_notify_SMMA( appInfo_ptr, FALSE );
#endif /* FEATURE_GWSMS */
  }
  else
  {
    MSG_ERROR_1("WMS_CFG_EVENT_(MS_)MEMORY_STATUS_SET not sent: error %d", cmd_err);
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_set_memory_full_proc() */

/*===========================================================================
FUNCTION wms_cfg_do_memory_full

DESCRIPTION
  Send the Memory Full Event for the given Memory Store

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_do_memory_full(
  sys_modem_as_id_e_type  as_id,
  wms_memory_store_e_type mem_store
)
{
  wms_cfg_event_info_s_type   *cfg_event_ptr = NULL;

  MSG_HIGH_1("wms_cfg_do_memory_full(): mem_store %d", mem_store);

  cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

  if (NULL != cfg_event_ptr)
  {
    cfg_event_ptr->mem_store = mem_store;
    wms_cfg_event_notify(WMS_CFG_EVENT_MEMORY_FULL, cfg_event_ptr);

    cfg_event_ptr->ms_memory_full.mem_store = mem_store;
    cfg_event_ptr->ms_memory_full.as_id     = as_id;
    wms_cfg_event_notify(WMS_CFG_EVENT_MS_MEMORY_FULL, cfg_event_ptr);
  }
  else
  {
    MSG_ERROR_1("WMS_CFG_EVENT_(MS_)MEMORY_FULL not sent: error %d", WMS_CMD_ERR_NO_RESOURCE);
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_do_memory_full() */


/*===========================================================================
FUNCTION wms_cfg_get_message_waiting_proc

DESCRIPTION
  Process the request of getting message waiting information.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  EF-MWIS is read.
===========================================================================*/
void wms_cfg_get_message_waiting_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type            cmd_err            = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type     *appInfo_ptr        = NULL;
  uint8 sim_data[WMS_SIM_MWI_REC_LEN_MAX];

  if (NULL == cmd_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  MSG_HIGH_2( "wms_cfg_get_message_waiting_proc()(client %d, as_id %d)",
              cmd_ptr->hdr.client_id,
              cmd_ptr->hdr.as_id );

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_AS_ID );
    return;
  }

  if (FALSE == appInfo_ptr->session_activated)
  {
    MSG_ERROR_0("Session is deactivated");
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BUSY );
    return;
  }

  if (0 == appInfo_ptr->mwis_rec_len)
  {
    MSG_HIGH_1("EFmwis rec len = %d", appInfo_ptr->mwis_rec_len);
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_CFG_MESSAGE_WAITING );
    return;
  }

  memset (sim_data, 0, sizeof(sim_data));

  if (wms_sim_mmgsdi_read_data( appInfo_ptr,
                                appInfo_ptr->mwis_file_enum,
                                0, // first record
                                appInfo_ptr->mwis_rec_len,
                                sim_data,
                                sizeof(sim_data)) == MMGSDI_SUCCESS)
  {
    MSG_HIGH_0("EFmwis file read successfully");
  }
  else
  {
    MSG_HIGH_0("EFmwis file read unsuccessfully...not trying to read MWIS information");
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_MSG_SIM_READ );
    return;
  }

  {
    wms_cfg_event_info_s_type    *cfg_event_ptr = NULL;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL == cfg_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }

    wms_client_cmd_status( cmd_ptr, cmd_err );

    /* Send event
    */
    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      cfg_event_ptr->msg_wait_info.as_id = appInfo_ptr->asid;
      cfg_event_ptr->msg_wait_info.state = WMS_MWI_STATE_INIT;

      (void)memscpy( cfg_event_ptr->msg_wait_info.sim_mwi_data,
                     sizeof(sim_data),
                     sim_data,
                     sizeof(sim_data));

      wms_cfg_event_notify( WMS_CFG_EVENT_MSG_WAITING, cfg_event_ptr );
    }
    else
    {
      MSG_ERROR_1("WMS_CFG_EVENT_MSG_WAITING not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(cfg_event_ptr);
  }

  return;
} /* wms_cfg_get_message_waiting_proc() */

/*===========================================================================
FUNCTION wms_cfg_set_message_waiting_proc

DESCRIPTION
  Process the request of setting message waiting information.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  EF-MWIS is updated.
===========================================================================*/
void wms_cfg_set_message_waiting_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type            cmd_err            = WMS_CMD_ERR_NONE;
  int                           i                  = 0;
  int                           count;
  boolean                       write_to_file      = TRUE;
  wms_prov_app_info_s_type     *appInfo_ptr        = NULL;

  /*Assuming there is only one record stored in the EFmwis.
    Each record will contain information of different MWIS*/
  uint8 sim_data[WMS_SIM_MWI_REC_LEN_MAX];

  if (NULL == cmd_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NULL_PTR );
    return;
  }

  MSG_HIGH_2( "wms_cfg_set_message_waiting_proc()(client %d, as_id %d)",
              cmd_ptr->hdr.client_id,
              cmd_ptr->hdr.as_id );

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (NULL == appInfo_ptr)
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_AS_ID );
    return;
  }

  count = cmd_ptr->cmd.cfg_set_mwi_info.num_mwi;

  if( (0 == appInfo_ptr->mwis_rec_len)|| (appInfo_ptr->mwis_rec_len < count) )
  {
    MSG_HIGH_1("EFmwis rec len = %d", appInfo_ptr->mwis_rec_len);
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_CFG_MESSAGE_WAITING );
    return;
  }

  if ((int)WMS_GW_MSG_WAITING_KIND_MAX < count)
  {
    MSG_HIGH_1("The count of MWI instances is %d", count);
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BAD_PARM );
    return;
  }

  memset (sim_data, 0, sizeof(sim_data));

  if (wms_sim_mmgsdi_read_data( appInfo_ptr,
                                appInfo_ptr->mwis_file_enum,
                                0, // first record
                                appInfo_ptr->mwis_rec_len,
                                sim_data,
                                sizeof(sim_data)) == MMGSDI_SUCCESS)
  {
    MSG_HIGH_0("EFmwis file read successfully");
  }
  else
  {
    MSG_HIGH_0("EFmwis file read unsuccessfully...not trying to write MWIS information");
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_MSG_SIM_READ );
    return;
  }

  for( i=0; i<count; i++ )
  {
    switch( cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].type )
    {
      case WMS_GW_MSG_WAITING_VOICEMAIL:
        if( cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].is_active == TRUE )
        {
          sim_data[0] |= WMS_MASK_EF_MWIS_VOICEMAIL;
        }
        else
        {
          sim_data[0] &= ~WMS_MASK_EF_MWIS_VOICEMAIL;
        }
        sim_data[WMS_EF_MWIS_VOICEMAIL_POS] = (uint8) cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].count;
        break;

      case WMS_GW_MSG_WAITING_FAX:
        if( cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].is_active == TRUE )
        {
          sim_data[0] |= WMS_MASK_EF_MWIS_FAX;
        }
        else
        {
          sim_data[0] &= ~WMS_MASK_EF_MWIS_FAX;
        }
        sim_data[WMS_EF_MWIS_FAX_POS] = (uint8) cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].count;
        break;

      case WMS_GW_MSG_WAITING_EMAIL:
        if( cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].is_active == TRUE )
        {
          sim_data[0] |= WMS_MASK_EF_MWIS_EMAIL;
        }
        else
        {
          sim_data[0] &= ~WMS_MASK_EF_MWIS_EMAIL;
        }
        sim_data[WMS_EF_MWIS_EMAIL_POS] = (uint8) cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].count;
        break;

      case WMS_GW_MSG_WAITING_OTHER:
        if( cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].is_active == TRUE )
        {
          sim_data[0] |= WMS_MASK_EF_MWIS_OTHERS;
        }
        else
        {
          sim_data[0] &= ~WMS_MASK_EF_MWIS_OTHERS;
        }
        sim_data[WMS_EF_MWIS_OTHERS_POS] = (uint8) cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].count;
        break;

      case WMS_GW_MSG_WAITING_VIDEOMAIL:
        if( appInfo_ptr->mwis_rec_len <= WMS_EF_MWIS_VIDEOMAIL_POS)
        {
          MSG_HIGH_0("EFmwis file does not support MWI Videomail");
          write_to_file = FALSE;
        }
        else
        {
          if( cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].is_active == TRUE )
          {
            sim_data[0] |= WMS_MASK_EF_MWIS_VIDEOMAIL;
          }
          else
          {
            sim_data[0] &= ~WMS_MASK_EF_MWIS_VIDEOMAIL;
          }
          sim_data[WMS_EF_MWIS_VIDEOMAIL_POS] = (uint8) cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].count;
        }
        break;

      default:
        write_to_file = FALSE;
        break;
    }
  }
  if( write_to_file == TRUE )
  {
    /* write to SIM*/
    if (MMGSDI_SUCCESS == wms_sim_mmgsdi_write_data( appInfo_ptr,
                                                     appInfo_ptr->mwis_file_enum,
                                                     0,
                                                     sim_data,
                                                     (uint32)(appInfo_ptr->mwis_rec_len) ) )
    {
      MSG_HIGH_0("EFmwis file has been updated successfully");
    }
    else
    {
      cmd_err = WMS_CMD_ERR_MSG_SIM_WRITE;
      MSG_HIGH_0("EFmwis file update failed");
    }
  }
  else
  {
    cmd_err = WMS_CMD_ERR_CFG_MESSAGE_WAITING;
    MSG_HIGH_0("Nothing to write to EFmwis file");
  }

  {
    wms_cfg_event_info_s_type    *cfg_event_ptr = NULL;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL == cfg_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }

    wms_client_cmd_status( cmd_ptr, cmd_err );

    /* Send event
    */
    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      cfg_event_ptr->msg_wait_info.as_id = appInfo_ptr->asid;
      cfg_event_ptr->msg_wait_info.state = WMS_MWI_STATE_INIT;

      (void)memscpy( cfg_event_ptr->msg_wait_info.sim_mwi_data,
                     sizeof(sim_data),
                     sim_data,
                     sizeof(sim_data));

      wms_cfg_event_notify( WMS_CFG_EVENT_MSG_WAITING, cfg_event_ptr );
    }
    else
    {
      MSG_ERROR_1("WMS_CFG_EVENT_MSG_WAITING not sent: error %d", cmd_err);
    }

    WMS_MEM_FREEIF(cfg_event_ptr);
  }

  return;
} /* wms_cfg_set_message_waiting_proc() */


/*===========================================================================
FUNCTION wms_cfg_send_sim_file_read_complete_event_to_pbm

DESCRIPTION
  Notify PBM that WMS has read all the SIM EFS

DEPENDENCIES
  PBM should send a req with Transaction_id != 0

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_send_sim_file_read_complete_event_to_pbm(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  appInfo_ptr->sim_file_read_is_complete = TRUE;

  
  if (0 != appInfo_ptr->mmgsdi_appInfo.sim_file_read_complete_param.t_id)
  {
    wms_cfg_send_efs_read_event send_evt_cb = NULL;

    send_evt_cb = (wms_cfg_send_efs_read_event)appInfo_ptr->mmgsdi_appInfo.sim_file_read_complete_param.cfg_event_cb;

    if (NULL != send_evt_cb)
    {
      MSG_HIGH_1("Sending read_complete event to pbm, t_id %d", appInfo_ptr->mmgsdi_appInfo.sim_file_read_complete_param.t_id);
      send_evt_cb (&(appInfo_ptr->mmgsdi_appInfo.sim_file_read_complete_param));
    }
    else
    {
      MSG_HIGH_0("Read_complete event not sent to PBM: send_evt_cb == NULL");
    }

    appInfo_ptr->mmgsdi_appInfo.sim_file_read_complete_param.t_id = 0;
  }
  else
  {
    MSG_HIGH_0("Read_complete event not sent to PBM: t_id == 0");
  }
} /* wms_cfg_send_sim_file_read_complete_event_to_pbm() */

/*===========================================================================
FUNCTION wms_cfg_notify_mwi_data()

DESCRIPTION
  Notifies the client with the MWI status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_notify_mwi_data(
  uint8                     *sim_data,
  wms_prov_app_info_s_type  *appInfo_ptr,
  wms_mwi_state_e_type       state
)
{
  wms_cfg_event_info_s_type    *cfg_event_ptr = NULL;

  cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

  if (NULL != cfg_event_ptr)
  {
    cfg_event_ptr->msg_wait_info.state = state;
    cfg_event_ptr->msg_wait_info.as_id = appInfo_ptr->asid;

    if (sim_data != NULL)
    {
      (void)memscpy( cfg_event_ptr->msg_wait_info.sim_mwi_data,
                     sizeof(cfg_event_ptr->msg_wait_info.sim_mwi_data),
                     sim_data,
                     sizeof(sim_data));
    }

    wms_cfg_event_notify( WMS_CFG_EVENT_MSG_WAITING, cfg_event_ptr );

    wms_mem_free(cfg_event_ptr);
  }
  else
  {
    MSG_ERROR_0("cfg_evt_ptr is null, no resource");
  }

  return;
} /* wms_cfg_notify_mwi_data() */


/*===========================================================================
FUNCTION wms_cfg_do_ready_event()                                  (INTERNAL)

DESCRIPTION
  Notifies UI when card has been initialised

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_do_ready_event(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if (NULL == appInfo_ptr)
  {
    return;
  }

  if ((MMGSDI_APP_SIM  == appInfo_ptr->mmgsdi_appInfo.app_type) ||
      (MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type))
  {
#ifdef FEATURE_GWSMS
    appInfo_ptr->sim_init_complete = TRUE;

    MSG_HIGH_1("sc_addr.number_of_digits in wms global cache is %d", appInfo_ptr->gw_template.sc_addr.number_of_digits);

    /* If there was no template found in the card records, read from NV */
    if (0 == appInfo_ptr->gw_template.sc_addr.number_of_digits)
    {
      wms_read_gw_template_from_nv(&appInfo_ptr->gw_template, appInfo_ptr->asid);
      MSG_HIGH_1( "No template found in card so default to SC address in NV. SC Address length = %d",
                   appInfo_ptr->gw_template.sc_addr.number_of_digits);
      appInfo_ptr->gw_template_index       = 0;
      appInfo_ptr->gw_template_mem_store   = WMS_MEMORY_STORE_NV_GW;
    }

    MSG_HIGH_2( "Send  WMS_CFG_EVENT_GW_READY event: appInfo_ptr 0x%x, asid %d",
                appInfo_ptr,
                appInfo_ptr->asid );

    wms_cfg_send_ready_event( appInfo_ptr, WMS_CLIENT_ALL );

    wms_msg_notify_SMMA( appInfo_ptr, FALSE );

    wms_msg_send_smsc_address_change_ind( appInfo_ptr );

#endif
  }
  else if ((MMGSDI_APP_RUIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
           (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
           (MMGSDI_APP_NONE == appInfo_ptr->mmgsdi_appInfo.app_type))
  {
#ifdef FEATURE_CDSMS
    appInfo_ptr->sim_init_complete = TRUE;

    MSG_HIGH_2( "Send  WMS_CFG_EVENT_CDMA_READY event: appInfo_ptr 0x%x, asid %d",
                appInfo_ptr,
                appInfo_ptr->asid );

    wms_cfg_send_ready_event(appInfo_ptr, WMS_CLIENT_ALL);

#ifdef FEATURE_CDSMS_BROADCAST
    wms_bc_online_proc(appInfo_ptr);
#endif /* FEATURE_CDSMS_BROADCAST */
#endif /* FEATURE_CDSMS */
  }

  return;
} /* wms_cfg_do_ready_event() */

/*===========================================================================
FUNCTION wms_cfg_report_not_ready_event

DESCRIPTION
  Notifies UI when WMS has not been initialised

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_not_ready_event(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if ( (NULL == appInfo_ptr) ||
       (SYS_MODEM_AS_ID_NONE == appInfo_ptr->asid) )
  {
    MSG_HIGH_0( "wms_cfg_report_not_ready_event: NULL appInfo or asid=NONE");
    return;
  }

  MSG_HIGH_2( "Send  WMS_CFG_EVENT_MS_NOT_READY event: appInfo_ptr 0x%x, asid %d",
              appInfo_ptr,
              appInfo_ptr->asid );

  wms_cfg_send_not_ready_event(appInfo_ptr, WMS_CLIENT_ALL);

  return;
} /* wms_cfg_report_not_ready_event() */

/*===========================================================================
FUNCTION wms_cfg_report_sim_init_start_event

DESCRIPTION
  Notifies UI when WMS starts reading SIM files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_sim_init_start_event(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if (NULL == appInfo_ptr)
  {
    return;
  }

  MSG_HIGH_4( "Send WMS_CFG_EVENT_MS_SIM_INIT_START: appInfo_ptr 0x%x, asid %d, session_type %d, app_type %d",
              appInfo_ptr,
              appInfo_ptr->asid,
              appInfo_ptr->mmgsdi_appInfo.session_type,
              appInfo_ptr->mmgsdi_appInfo.app_type );

  {
    wms_cfg_event_info_s_type         *cfg_event_ptr = NULL;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL != cfg_event_ptr)
    {
      if ((MMGSDI_APP_SIM  == appInfo_ptr->mmgsdi_appInfo.app_type) ||
          (MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->begin_sim_init.sys_type = WMS_CFG_SYS_GW;
      }
      else if ((MMGSDI_APP_RUIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_NONE == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->begin_sim_init.sys_type = WMS_CFG_SYS_CDMA;
      }

      cfg_event_ptr->begin_sim_init.as_id        = appInfo_ptr->asid;
      cfg_event_ptr->begin_sim_init.is_inited    = FALSE;
      cfg_event_ptr->begin_sim_init.session_type = appInfo_ptr->mmgsdi_appInfo.session_type;
      wms_cfg_event_notify( WMS_CFG_EVENT_MS_SIM_INIT_START, cfg_event_ptr );
    }
    else
    {
      MSG_ERROR_0("WMS_CFG_EVENT_MS_SIM_INIT_START: memory allocation failed");
    }

    WMS_MEM_FREEIF(cfg_event_ptr);
  }

  return;
} /* wms_cfg_report_sim_init_start_event() */

/*===========================================================================
FUNCTION wms_cfg_report_sim_init_complete_event

DESCRIPTION
  Notifies UI when WMS has finished reading SIM files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_sim_init_complete_event(
  wms_prov_app_info_s_type *appInfo_ptr,
  boolean                   sim_ready
)
{
  if (NULL == appInfo_ptr)
  {
    return;
  }

  MSG_HIGH_5( "Send WMS_CFG_EVENT_MS_SIM_INIT_END: appInfo_ptr 0x%x, asid %d, session_type %d, app_type %d, sim_ready %d",
              appInfo_ptr,
              appInfo_ptr->asid,
              appInfo_ptr->mmgsdi_appInfo.session_type,
              appInfo_ptr->mmgsdi_appInfo.app_type,
              sim_ready );

  {
    wms_cfg_event_info_s_type         *cfg_event_ptr = NULL;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL != cfg_event_ptr)
    {
      if ((MMGSDI_APP_SIM  == appInfo_ptr->mmgsdi_appInfo.app_type) ||
          (MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->end_sim_init.sys_type = WMS_CFG_SYS_GW;
      }
      else if ((MMGSDI_APP_RUIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_NONE == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->end_sim_init.sys_type = WMS_CFG_SYS_CDMA;
      }

      cfg_event_ptr->end_sim_init.as_id        = appInfo_ptr->asid;
      cfg_event_ptr->end_sim_init.is_inited    = sim_ready;
      cfg_event_ptr->end_sim_init.session_type = appInfo_ptr->mmgsdi_appInfo.session_type;
      wms_cfg_event_notify( WMS_CFG_EVENT_MS_SIM_INIT_END, cfg_event_ptr );
    }
    else
    {
      MSG_ERROR_0("WMS_CFG_EVENT_MS_SIM_INIT_END: memory allocation failed");
    }

    WMS_MEM_FREEIF(cfg_event_ptr);
  }

  return;
} /* wms_cfg_report_sim_init_complete_event() */

/*===========================================================================
FUNCTION wms_cfg_report_start_refresh_event

DESCRIPTION
  Notifies UI when refresh process starts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_start_refresh_event(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if (NULL == appInfo_ptr)
  {
    return;
  }

  MSG_HIGH_3( "Send WMS_CFG_EVENT_MS_REFRESH_START: appInfo_ptr 0x%x, asid %d, app_type %d",
              appInfo_ptr, appInfo_ptr->asid, appInfo_ptr->mmgsdi_appInfo.app_type );

  {
    wms_cfg_event_info_s_type         *cfg_event_ptr = NULL;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL != cfg_event_ptr)
    {
      if ((MMGSDI_APP_SIM  == appInfo_ptr->mmgsdi_appInfo.app_type) ||
          (MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->begin_refresh.sys_type = WMS_CFG_SYS_GW;
      }
      else if ((MMGSDI_APP_RUIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_NONE == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->begin_refresh.sys_type = WMS_CFG_SYS_CDMA;
      }

      cfg_event_ptr->begin_refresh.as_id        = appInfo_ptr->asid;
      //cfg_event_ptr->begin_refresh.session_type = appInfo_ptr->session_type;
      wms_cfg_event_notify( WMS_CFG_EVENT_MS_REFRESH_START, cfg_event_ptr );
    }
    else
    {
      MSG_ERROR_0("WMS_CFG_EVENT_MS_REFRESH_START: memory allocation failed");
    }

    WMS_MEM_FREEIF(cfg_event_ptr);
  }

  return;
} /* wms_cfg_report_start_refresh_event() */

/*===========================================================================
FUNCTION wms_cfg_report_end_refresh_event

DESCRIPTION
  Notifies UI when refresh process ends.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_end_refresh_event(
  wms_prov_app_info_s_type         *appInfo_ptr,
  boolean                          is_successful
)
{
  if (NULL == appInfo_ptr)
  {
    return;
  }

  MSG_HIGH_3( "Send WMS_CFG_EVENT_MS_REFRESH_END: appInfo_ptr 0x%x, asid %d, app_type %d",
              appInfo_ptr, appInfo_ptr->asid, appInfo_ptr->mmgsdi_appInfo.app_type );

  {
    wms_cfg_event_info_s_type         *cfg_event_ptr = NULL;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL != cfg_event_ptr)
    {
      if ((MMGSDI_APP_SIM  == appInfo_ptr->mmgsdi_appInfo.app_type) ||
          (MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->end_refresh.sys_type = WMS_CFG_SYS_GW;
      }
      else if ((MMGSDI_APP_RUIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_NONE == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->end_refresh.sys_type = WMS_CFG_SYS_CDMA;
      }

      cfg_event_ptr->end_refresh.as_id         = appInfo_ptr->asid;
      cfg_event_ptr->end_refresh.is_success    = is_successful;
      //cfg_event_ptr->end_refresh.session_type  = appInfo_ptr->session_type;
      wms_cfg_event_notify( WMS_CFG_EVENT_MS_REFRESH_END, cfg_event_ptr );
    }
    else
    {
      MSG_ERROR_0("WMS_CFG_EVENT_MS_REFRESH_END: memory allocation failed");
    }

    WMS_MEM_FREEIF(cfg_event_ptr);
  }

  return;
} /* wms_cfg_report_end_refresh_event() */

/*===========================================================================
FUNCTION wms_cfg_report_network_service_avail_event

DESCRIPTION
  Notifies UI when whether WMS OTA is available.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_network_service_avail_event(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if ( (NULL == appInfo_ptr) ||
       (SYS_MODEM_AS_ID_NONE == appInfo_ptr->asid) )
  {
    MSG_HIGH_0( "wms_cfg_report_service_avail_event: NULL appInfo or asid=NONE");
    return;
  }

  MSG_HIGH_3( "Send WMS_CFG_EVENT_MS_SERV_AVAIL: appInfo_ptr 0x%x, asid %d, app_type %d",
              appInfo_ptr,
              appInfo_ptr->asid,
              appInfo_ptr->mmgsdi_appInfo.app_type );

  {
    wms_cfg_event_info_s_type         *cfg_event_ptr = NULL;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL != cfg_event_ptr)
    {
      if ((MMGSDI_APP_SIM  == appInfo_ptr->mmgsdi_appInfo.app_type) ||
          (MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->svc_avail.sys_type = WMS_CFG_SYS_GW;
      }
      else if ((MMGSDI_APP_RUIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_NONE == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->svc_avail.sys_type = WMS_CFG_SYS_CDMA;
      }

      cfg_event_ptr->svc_avail.as_id          = appInfo_ptr->asid;
      cfg_event_ptr->svc_avail.is_serv_avail  = TRUE;
      wms_cfg_event_notify( WMS_CFG_EVENT_MS_SERV_AVAIL, cfg_event_ptr );
    }
    else
    {
      MSG_ERROR_0("WMS_CFG_EVENT_MS_SERV_AVAIL : memory allocation failed");
    }

    WMS_MEM_FREEIF(cfg_event_ptr);
  }

  return;
} /* wms_cfg_report_network_service_avail_event() */

/*===========================================================================
FUNCTION wms_cfg_report_network_service_not_avail_event

DESCRIPTION
  Notifies UI when whether WMS OTA is unavailable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_network_service_not_avail_event(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if ( (NULL == appInfo_ptr) ||
       (SYS_MODEM_AS_ID_NONE == appInfo_ptr->asid) )
  {
    MSG_HIGH_0( "wms_cfg_report_service_not_avail_event: NULL appInfo or asid=NONE");
    return;
  }

  MSG_HIGH_3( "Send WMS_CFG_EVENT_MS_SERV_NOT_AVAIL: appInfo_ptr 0x%x, asid %d, app_type %d",
              appInfo_ptr,
              appInfo_ptr->asid,
              appInfo_ptr->mmgsdi_appInfo.app_type );

  {
    wms_cfg_event_info_s_type         *cfg_event_ptr = NULL;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL != cfg_event_ptr)
    {
      if ((MMGSDI_APP_SIM  == appInfo_ptr->mmgsdi_appInfo.app_type) ||
          (MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->svc_not_avail.sys_type = WMS_CFG_SYS_GW;
      }
      else if ((MMGSDI_APP_RUIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
               (MMGSDI_APP_NONE == appInfo_ptr->mmgsdi_appInfo.app_type))
      {
        cfg_event_ptr->svc_not_avail.sys_type = WMS_CFG_SYS_CDMA;
      }

      cfg_event_ptr->svc_not_avail.as_id          = appInfo_ptr->asid;
      cfg_event_ptr->svc_not_avail.is_serv_avail  = FALSE;
      wms_cfg_event_notify( WMS_CFG_EVENT_MS_SERV_NOT_AVAIL, cfg_event_ptr );
    }
    else
    {
      MSG_ERROR_0("WMS_CFG_EVENT_MS_SERV_NOT_AVAIL : memory allocation failed");
    }

    WMS_MEM_FREEIF(cfg_event_ptr);
  }

  return;
} /* wms_cfg_report_network_service_not_avail_event() */

/*===========================================================================
FUNCTION wms_cfg_report_network_service

DESCRIPTION
  Notifies UI when whether WMS OTA is available or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_network_service(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  int8 stack_idx;

  if ( NULL != appInfo_ptr )
  {

    for ( stack_idx = 0; stack_idx < appInfo_ptr->number_of_stacks; stack_idx++ )
    {
      if ( (SYS_SRV_STATUS_NO_SRV  == appInfo_ptr->stack_info[stack_idx].srv_status)  ||
           (SYS_SRV_STATUS_PWR_SAVE == appInfo_ptr->stack_info[stack_idx].srv_status) ||
           (SYS_SYS_MODE_NO_SRV == appInfo_ptr->stack_info[stack_idx].sys_mode) )
      {
        appInfo_ptr->srv_available = FALSE;
      }
      else
      {
        appInfo_ptr->srv_available = TRUE;
        break;
      }
    }

    if ( appInfo_ptr->srv_available )
    {
      wms_cfg_report_network_service_avail_event( appInfo_ptr );
    }
    else
    {
      wms_cfg_report_network_service_not_avail_event( appInfo_ptr );
    }
  } /* for()*/
} /* wms_cfg_report_network_service() */

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_set_link_control_proc

DESCRIPTION
  Send a request to the lower layers to disable/enable SMS link control for
  connection establishment and timer duration

  Parameter values:
    control_option
        0: Disable - Lower layers don't need to keep up the link when a
                    MO SMS is sent
        1: Enable - If the timer (link period) expires, close the link and
                    automatically switch back to 0 (Disable)
        2: Enable - If the timer (linke period) expires, close the link and
                    retain the perference to be 2
    link_period - timer between the previous and next send command
                    (1 to 5 second interval)
DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_set_link_control_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type                  cmd_err = WMS_CMD_ERR_NONE;
  wms_cfg_link_control_mode_e_type    link_control_mode;
  wms_cfg_link_control_status_e_type  link_control_status;
  uint8                               link_control_timer;
  wms_cfg_event_info_s_type          *cfg_event_ptr = NULL;
  wms_prov_app_info_s_type           *appInfo_ptr   = cmd_ptr->hdr.appInfo_ptr;

  link_control_mode   = cmd_ptr->cmd.cfg_set_link_control.link_control.link_control_mode;
  link_control_status = cmd_ptr->cmd.cfg_set_link_control.link_control.link_control_status;
  link_control_timer  = cmd_ptr->cmd.cfg_set_link_control.link_control_timer;

  MSG_HIGH_3( "wms_cfg_set_link_control_proc() mode %d, status %d, timer %d",
              link_control_mode, link_control_status, link_control_timer );

  do
  {
    if (NULL == appInfo_ptr)
    {
      cmd_err = WMS_CMD_ERR_AS_ID;
      break;
    }

    if (link_control_mode >= WMS_LINK_CONTROL_MAX || link_control_timer > 5)
    {
      MSG_ERROR_2( "set link control with bad param value: mode %d, timer %d",
                   link_control_mode, link_control_timer );
      cmd_err = WMS_CMD_ERR_CFG_LINK_CONTROL;
      break;
    }

    appInfo_ptr->link_control_mode   = link_control_mode;
    appInfo_ptr->link_control_status = link_control_status;
    appInfo_ptr->link_control_timer  = link_control_timer;

    cmd_err = wms_cm_gw_rl_link_control(appInfo_ptr->asid, link_control_mode, link_control_timer);

    if (WMS_CMD_ERR_NONE != cmd_err)
    {
      break;
    }

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL == cfg_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }
  } while(0);

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    cfg_event_ptr->link_control.as_id               = appInfo_ptr->asid;
    cfg_event_ptr->link_control.link_control_mode   = link_control_mode;
    cfg_event_ptr->link_control.link_control_status = link_control_status;
    cfg_event_ptr->link_control.client_id           = WMS_CLIENT_TYPE_INTERNAL;

    wms_cfg_event_notify(WMS_CFG_EVENT_LINK_CONTROL, cfg_event_ptr);
  }
  else
  {
    MSG_ERROR_1("WMS_CFG_EVENT_LINK_CONTROL not sent: error", cmd_err);
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
}/* wms_cfg_set_link_control_proc */

/*===========================================================================
FUNCTION wms_cfg_get_link_control_proc

DESCRIPTION
  Retrieves the current link control setting.  WMS will send a cfg event to
  all clients to infom if the link is up or down.and the mode

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_get_link_control_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_prov_app_info_s_type   *appInfo_ptr   = cmd_ptr->hdr.appInfo_ptr;
  wms_cfg_event_info_s_type  *cfg_event_ptr = NULL;
  wms_cmd_err_e_type          cmd_err       = WMS_CMD_ERR_NONE;

  MSG_HIGH_2( "wms_cfg_get_link_control_proc(client %d, as_id %d)",
              cmd_ptr->hdr.client_id,
              cmd_ptr->hdr.as_id );

  do
  {
    if (NULL == appInfo_ptr)
    {
     cmd_err = WMS_CMD_ERR_AS_ID;
     break;
    }

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL == cfg_event_ptr)
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }

  } while(0);

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    cfg_event_ptr->link_control.as_id               = appInfo_ptr->asid;
    cfg_event_ptr->link_control.link_control_mode   = appInfo_ptr->link_control_mode;
    cfg_event_ptr->link_control.link_control_status = appInfo_ptr->link_control_status;
    cfg_event_ptr->link_control.client_id           = cmd_ptr->hdr.client_id;

    wms_cfg_event_notify_single_client(WMS_CFG_EVENT_LINK_CONTROL, cfg_event_ptr, cfg_event_ptr->link_control.client_id);
  }
  else
  {
    MSG_ERROR_1("WMS_CFG_EVENT_LINK_CONTROL not sent: error %d", cmd_err);
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_get_link_control_proc() */


/*===========================================================================
FUNCTION wms_cfg_link_control_ind_proc

DESCRIPTION
  Lower layer notifies wms link control setting change.
  WMS then sends a cfg event to all clients to inform if
  the link is up or down.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_link_control_ind_proc(
  SM_RL_LINK_CONTROL_REPORT_T *ind_ptr
)
{
  sys_modem_as_id_e_type    asid = SYS_MODEM_AS_ID_1;
  wms_prov_app_info_s_type *appInfo_ptr;

  if (WMS_DEVICE_MODE_SS != wms_cfg_s_ptr()->device_mode)
  {
    asid = ind_ptr->as_id;
  }

  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(asid, WMS_MESSAGE_MODE_GW);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return;
  }

  appInfo_ptr->link_control_mode                  = (wms_cfg_link_control_mode_e_type) ind_ptr->mode;
  appInfo_ptr->link_control_status                = (wms_cfg_link_control_status_e_type) ind_ptr->status;

  {
    wms_cfg_event_info_s_type         *cfg_event_ptr = NULL;

    cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

    if (NULL != cfg_event_ptr)
    {
      cfg_event_ptr->link_control.as_id               = asid;
      cfg_event_ptr->link_control.link_control_mode   = (wms_cfg_link_control_mode_e_type) ind_ptr->mode;
      cfg_event_ptr->link_control.link_control_status = (wms_cfg_link_control_status_e_type) ind_ptr->status;
      cfg_event_ptr->link_control.client_id           = WMS_CLIENT_TYPE_INTERNAL;

      wms_cfg_event_notify( WMS_CFG_EVENT_LINK_CONTROL, cfg_event_ptr );
    }
    else
    {
      MSG_ERROR_0("WMS_CFG_EVENT_LINK_CONTROL not sent: memory allocation failed");
    }

    WMS_MEM_FREEIF(cfg_event_ptr);
  }

  return;
} /* wms_cfg_link_control_ind_proc() */

#endif /* FEATURE_GWSMS */


void wms_cfg_send_sim_file_read_complete_evt_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_prov_app_info_s_type     *appInfo_ptr;
  wms_prov_mmgsdi_app_info_s_type      *mmgsdi_appInfo;
  uint32                        tx_id        = 0x0000;
  wms_cfg_send_efs_read_event   send_evt_cb  = NULL;



  mmgsdi_appInfo = cmd_ptr->hdr.mmgsdi_appInfo; 
  tx_id = cmd_ptr->cmd.cfg_send_sim_efs_read_complete_evt.t_id;
  send_evt_cb = (wms_cfg_send_efs_read_event)cmd_ptr->cmd.cfg_send_sim_efs_read_complete_evt.cfg_event_cb;

  if ( (0 != tx_id) && (NULL != send_evt_cb) )
  {
    appInfo_ptr = wms_get_appinfo_by_session_type(mmgsdi_appInfo->session_type);

    MSG_HIGH_4( "wms_cfg_send_sim_file_read_complete_evt_proc() session_type: %d, tx_id: %d, send_evt_cb: (0x%x) app_info %d",
               mmgsdi_appInfo->session_type, tx_id, send_evt_cb,appInfo_ptr);

    if(appInfo_ptr != NULL)
    {
      if (appInfo_ptr->sim_file_read_is_complete)
      {
        MSG_HIGH_0("Sending read_complete to PBM");
        send_evt_cb (&(cmd_ptr->cmd.cfg_send_sim_efs_read_complete_evt));
      }
      else
      {
        
        appInfo_ptr->mmgsdi_appInfo.sim_file_read_complete_param = cmd_ptr->cmd.cfg_send_sim_efs_read_complete_evt;
        MSG_HIGH_1("WMS is not ready, saving read_complete parameters t_id =  %d", appInfo_ptr->mmgsdi_appInfo.sim_file_read_complete_param.t_id);
      }
    }
    else
    {
     /* Buffer the efs read event by the time if WMS has not get the SUBSCRIPTION_READY_EVENT */
      
      mmgsdi_appInfo->sim_file_read_complete_param = cmd_ptr->cmd.cfg_send_sim_efs_read_complete_evt; 
      MSG_HIGH_1("WMS is not ready, saving read_complete parameters t_id =  %d", mmgsdi_appInfo->sim_file_read_complete_param.t_id);
    }
  }
} /* wms_cfg_send_sim_file_read_complete_evt_proc() */

/*===========================================================================
FUNCTION wms_cfg_process_cmd

DESCRIPTION
  Dispatch the configuration command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_process_cmd
(
  wms_cmd_type  *cmd_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( cmd_ptr->hdr.cmd_id )
  {
    case WMS_CMD_CFG_SET_ROUTES:
      wms_cfg_set_routes_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_GET_ROUTES:
      wms_cfg_get_routes_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_GET_MEMORY_STATUS:
      wms_cfg_get_mem_status_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_GET_MESSAGE_LIST:
      wms_cfg_get_msg_list_proc( cmd_ptr );
      break;

#ifdef FEATURE_GWSMS
    case WMS_CMD_CFG_SET_GW_DOMAIN_PREF:
      wms_cfg_set_gw_domain_pref_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_GET_GW_DOMAIN_PREF:
      wms_cfg_get_gw_domain_pref_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_SET_LINK_CONTROL:
      wms_cfg_set_link_control_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_GET_LINK_CONTROL:
      wms_cfg_get_link_control_proc( cmd_ptr );
      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_DOMAIN_SELECTION
    case WMS_CMD_CFG_SET_LTE_DOMAIN_PREF:
      wms_cfg_set_lte_domain_pref_proc( cmd_ptr );
      break;
#endif /* FEATURE_DOMAIN_SELECTION */

    case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
      wms_cfg_set_primary_client_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_SET_MEMORY_FULL:
      wms_cfg_set_memory_full_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_SET_MESSAGE_WAITING:
      wms_cfg_set_message_waiting_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_GET_MESSAGE_WAITING:
      wms_cfg_get_message_waiting_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT:
      wms_cfg_send_sim_file_read_complete_evt_proc( cmd_ptr );
      break;

#if (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA))
    case WMS_CMD_CFG_RRC_CELL_CHANGE_EVENT_CB:
      wms_cfg_rrc_cell_change_cb_proc(cmd_ptr);
      break;
#endif /* (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)) */

    default:
      break;
  }

  /* done */
  return;

} /* wms_cfg_process_cmd() */

/*===========================================================================
FUNCTION wms_cfg_cleanup

DESCRIPTION
  Clean up (release) memory held in various places

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Runtime memory is released.  WMS must be reinitialized.
===========================================================================*/
void wms_cfg_cleanup(
  void
)
{
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();
  int                         idx;

  // Release any raw SMSP records
  if (NULL != cfg_s_ptr->wms_app0_info)
  {
    for (idx = 0; idx < WMS_MAX_TEMPLATES; idx++)
    {
      WMS_MEM_FREEIF(cfg_s_ptr->wms_app0_info->smsp_raw_rec[idx].data_ptr);
      cfg_s_ptr->wms_app0_info->smsp_raw_rec[idx].data_len = 0;
    }
  }

  if (NULL != cfg_s_ptr->wms_app1_info)
  {
    for (idx = 0; idx < WMS_MAX_TEMPLATES; idx++)
    {
      WMS_MEM_FREEIF(cfg_s_ptr->wms_app1_info->smsp_raw_rec[idx].data_ptr);
      cfg_s_ptr->wms_app1_info->smsp_raw_rec[idx].data_len = 0;
    }
  }

  if (NULL != cfg_s_ptr->wms_app2_info)
  {
    for (idx = 0; idx < WMS_MAX_TEMPLATES; idx++)
    {
      WMS_MEM_FREEIF(cfg_s_ptr->wms_app2_info->smsp_raw_rec[idx].data_ptr);
      cfg_s_ptr->wms_app2_info->smsp_raw_rec[idx].data_len = 0;
    }
  }

  if (NULL != cfg_s_ptr->wms_app3_info)
  {
    for (idx = 0; idx < WMS_MAX_TEMPLATES; idx++)
    {
      WMS_MEM_FREEIF(cfg_s_ptr->wms_app3_info->smsp_raw_rec[idx].data_ptr);
      cfg_s_ptr->wms_app3_info->smsp_raw_rec[idx].data_len = 0;
    }
  }

} /* wms_cfg_cleanup() */

/*===========================================================================
FUNCTION wms_cfg_pre_init()                                        (INTERNAL)

DESCRIPTION
  Do Pre-configuration-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Pre-Configuration data initialized.
===========================================================================*/
void wms_cfg_pre_init(
  void
)
{
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();

#ifdef FEATURE_GSTK
  cfg_s_ptr->refresh_registered  = FALSE;
  cfg_s_ptr->refresh_in_progress = FALSE;
#endif /* FEATURE_GSTK */

#ifdef FEATURE_CDSMS
  cfg_s_ptr->nv_cdma_max_slots  = 0;
  cfg_s_ptr->cdma_last_message_number = 0;
  cfg_s_ptr->phone_esn = 0;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
  cfg_s_ptr->nv_gw_max_slots  = 0;
#endif /* FEATURE_GWSMS */

  cfg_s_ptr->wms_app0_info = NULL;
  cfg_s_ptr->wms_app1_info = NULL;
  cfg_s_ptr->wms_app2_info = NULL;
  cfg_s_ptr->wms_app3_info = NULL;

  memset(&cfg_s_ptr->wms_ph_info,      0, sizeof(wms_cm_ph_info_s_type));
  memset(&cfg_s_ptr->wms_cm_subs_info, 0, sizeof(wms_cm_subs_pref_info_s_type));

  memset(&cfg_s_ptr->wms_mmgsdi_app0_info, 0, sizeof(wms_prov_mmgsdi_app_info_s_type));
  memset(&cfg_s_ptr->wms_mmgsdi_app1_info, 0, sizeof(wms_prov_mmgsdi_app_info_s_type));
  memset(&cfg_s_ptr->wms_mmgsdi_app2_info, 0, sizeof(wms_prov_mmgsdi_app_info_s_type));
  memset(&cfg_s_ptr->wms_mmgsdi_app3_info, 0, sizeof(wms_prov_mmgsdi_app_info_s_type));

  cfg_s_ptr->wms_mmgsdi_app0_info.session_type       = MMGSDI_1X_PROV_PRI_SESSION;
  cfg_s_ptr->wms_mmgsdi_app1_info.session_type       = MMGSDI_GW_PROV_PRI_SESSION;
  cfg_s_ptr->wms_mmgsdi_app2_info.session_type       = MMGSDI_GW_PROV_SEC_SESSION;
  cfg_s_ptr->wms_mmgsdi_app3_info.session_type       = MMGSDI_GW_PROV_TER_SESSION;

  // Initialize the subs_pref session state
  {
    int   idx;

    for (idx = 0; idx < MAX_AS_IDS; idx++)
    {
      cfg_s_ptr->wms_cm_subs_info.sub[idx].session_type_1x = MMGSDI_MAX_SESSION_TYPE_ENUM;
      cfg_s_ptr->wms_cm_subs_info.sub[idx].is_available_1x = FALSE;
      cfg_s_ptr->wms_cm_subs_info.sub[idx].session_type_gw = MMGSDI_MAX_SESSION_TYPE_ENUM;
      cfg_s_ptr->wms_cm_subs_info.sub[idx].is_available_gw = FALSE;
      cfg_s_ptr->wms_cm_subs_info.sub[idx].is_operational  = FALSE;
    }
  }

  return;
} /* wms_cfg_pre_init() */

/*===========================================================================
FUNCTION wms_cfg_init

DESCRIPTION
  Do configuration-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Configuration data initialized.
===========================================================================*/
void wms_cfg_init(
  void
)
{
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();

  MSG_HIGH_0("wms_cfg_init()");

  wms_cm_init_routes(SYS_MODEM_AS_ID_1);

#ifdef FEATURE_GWSMS

#if defined (FEATURE_CPHS) && defined(FEATURE_ALS)
  cfg_s_ptr->sim_voice_mail_index_line2       = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
  cfg_s_ptr->sim_voice_mail_active_line2      = FALSE;
#endif /* defined (FEATURE_CPHS) && (FEATURE_ALS) */

  /* Initialization for link control with lower layers */
  cfg_s_ptr->link_control.link_control_mode   = WMS_LINK_CONTROL_DISABLED;
  cfg_s_ptr->link_control.link_control_status = WMS_LINK_CONTROL_INACTIVE;
  cfg_s_ptr->link_control_timer               = 0;

  // Init for multi-mode SIM
  if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
  {
    wms_cm_init_routes(SYS_MODEM_AS_ID_2);

    // Init for TSTS (3 SIM slots)
    if ( WMS_DEVICE_MODE_DSDA < cfg_s_ptr->device_mode )
    {
      wms_cm_init_routes(SYS_MODEM_AS_ID_3);
    }
  } // WMS_DEVICE_MODE_SS

  // Read the ENS flag required for some GCF tests
  if (!wms_nv_read_ens_flag(&cfg_s_ptr->ens_flag))
  {
    cfg_s_ptr->ens_flag = FALSE;
  }

  // Initialize the RAM message lists
  {
    int i = 0;

    // Init for RAM messages
    for (i = 0; i < WMS_MAX_RAM_MESSAGES; i++)
    {
      cfg_s_ptr->ram_gw_sms[i] = NULL; /* free space */
    }

    // Init for RAM Status Reports
    for (i = 0; i < WMS_MAX_RAM_STATUS_REPORTS; i++)
    {
      cfg_s_ptr->ram_status_rpts[i] = FALSE; /* free space */
    }
  }

  rex_init_crit_sect( &sts_rpt_crit_sect );

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
  // Read the ESN from NV
  {
    nv_item_type *nvi_ptr = (nv_item_type *) wms_mem_malloc ( sizeof(nv_item_type) );
    
    if ( NULL != nvi_ptr && NV_DONE_S == wms_nv_read_wait( NV_ESN_I, nvi_ptr ) )
    {
      cfg_s_ptr->phone_esn = nvi_ptr->esn.esn;
      MSG_HIGH_1("ESN = 0x%x", cfg_s_ptr->phone_esn);
    }

    WMS_MEM_FREEIF( nvi_ptr );
  }
#endif /* FEATURE_CDSMS */

  /* Initialize Bearer Prefs */
  {
    boolean pref_is_updated = FALSE;

    if ( FALSE == wms_nv_read_bearer_prefs( cfg_s_ptr->bearer_prefs ) )
    {
      int i = 0;

      MSG_HIGH_0("NV read for bearer prefs failed -- reset all to ALLOWED");

      /* NV read failed; make default usage to be 'ALLOWED'/ used */
      for ( i = 0; i < (int)WMS_BEARER_MAX; i++ )
      {
        cfg_s_ptr->bearer_prefs[i] = WMS_BEARER_USAGE_ALLOWED;
      }

      /* By default, prefer EVDO if SMS over IP is enabled */
      MSG_HIGH_0("Setting WMS_BEARER_CDMA_EVDO as PREFERRED bearer");
      cfg_s_ptr->bearer_prefs[WMS_BEARER_CDMA_EVDO] = WMS_BEARER_USAGE_PREFERRED;
      pref_is_updated = TRUE;
    }
    else
    {
      int i = 0;

      for ( i = 0; i < (int)WMS_BEARER_MAX; i++ )
      {
        if ( cfg_s_ptr->bearer_prefs[i] >= WMS_BEARER_USAGE_MAX )
        {
          /* bad usage value; make default usage to be 'used' */
          MSG_ERROR_3( "Setting invalid bearer_prefs[%d]=%d to WMS_BEARER_USAGE_ALLOWED(%d)",
                       i, cfg_s_ptr->bearer_prefs[i], WMS_BEARER_USAGE_ALLOWED );
          cfg_s_ptr->bearer_prefs[i] = WMS_BEARER_USAGE_ALLOWED;
          pref_is_updated = TRUE;
        }
      }
    }

    /* Write to NV only if bearer prefs were missing or had bad values */
    if ( pref_is_updated )
    {
      MSG_HIGH_0("Writing bearer prefs to NV");
      (void) wms_nv_write_bearer_prefs( cfg_s_ptr->bearer_prefs );
    }
  }

  memset( cfg_s_ptr->active_bearers, FALSE, sizeof(cfg_s_ptr->active_bearers) );

  cfg_s_ptr->last_used_bearer = WMS_BEARER_MAX;


  cfg_s_ptr->lte_sub_info = SYS_MODEM_AS_ID_NONE;

  wms_nv_efs_init();

  return;
} /* wms_cfg_init */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cfg_get_ruim_next_message_number

DESCRIPTION
  Get the next message number from RUIM to use for an MO message.

DEPENDENCIES
  none

RETURN VALUE
  The next message number to use

SIDE EFFECTS
  The current message number is incremented.
===========================================================================*/
wms_message_number_type wms_cfg_get_ruim_next_message_number(
  void
)
{
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();
  wms_message_number_type     mn;
  uint8                       data[5];
  wms_prov_app_info_s_type   *appInfo_ptr;

  appInfo_ptr = wms_get_appinfo_1x();

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return 0;
  }

  /* 3GPP2 spec, section 4.3.1.5 specifies that the mobile station shall increment the
     MESSAGE_ID field, modulo 65536, for each new message sent.
  */
  if (wms_nv_rtre_control_is_nv())
  {
    mn = ++cfg_s_ptr->cdma_last_message_number;
    if( cfg_s_ptr->cdma_last_message_number > 65535 )
    {
      cfg_s_ptr->cdma_last_message_number = 0;
      mn = 0;
    }
    wms_nv_write_sms_status();

  }
  else
  {
    mn = ++appInfo_ptr->smss_last_message_number;
    if( appInfo_ptr->smss_last_message_number > 65535 )
    {
      appInfo_ptr->smss_last_message_number = 0;
      mn = 0;
    }

    if (appInfo_ptr->sms_rec_len != 0)
    {
      data[0]   = (uint8) ( appInfo_ptr->smss_last_message_number >> 8 );
      data[1]   = (uint8) ( appInfo_ptr->smss_last_message_number & 0xFF );

      data[2]   = (uint8) ( appInfo_ptr->smss_last_wap_message_number >> 8 );
      data[3]   = (uint8) ( appInfo_ptr->smss_last_wap_message_number & 0xFF );

      data[4]   = appInfo_ptr->smss_mem_full_flag;

      if (MMGSDI_SUCCESS != wms_sim_mmgsdi_write_data( appInfo_ptr,
                                                       appInfo_ptr->smss_file_enum,
                                                       0, //not used
                                                       data,
                                                       sizeof(data) ))
      {
        MSG_ERROR_0("RUIM/CSIM write error");
      }
    }
    else
    {
       MSG_ERROR_0("RUIM/CSIM was not initialized");
    }

  }

  MSG_HIGH_1("wms_cfg_get_ruim_next_message_number() returns %d", mn);

  return mn;
} /* wms_cfg_get_ruim_next_message_number() */


/*===========================================================================
FUNCTION wms_cfg_update_cdma_memory_full_flag

DESCRIPTION
  Update memory full flag in RUIM/CSIM card

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_update_cdma_memory_full_flag(
  wms_prov_app_info_s_type     *appInfo_ptr,
  wms_sim_mem_full_flag_e_type  mem_full_flag
)
{
  uint8                       data[5];

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return;
  }

  if (appInfo_ptr->sms_rec_len != 0)
  {
    MSG_HIGH_2( "Mem Full Flag:0xFE, Mem Clear Flag:0xFF, current flag=%d mem_full %d",
                appInfo_ptr->smss_mem_full_flag,
                mem_full_flag );

    if(WMS_SIM_MEM_FULL_SET == mem_full_flag)
    {
      appInfo_ptr->smss_mem_full_flag &= (uint8)mem_full_flag;
    }
    else if(WMS_SIM_MEM_FULL_CLEAR == mem_full_flag)
    {
      appInfo_ptr->smss_mem_full_flag |= (uint8)mem_full_flag;
    }
    else
    {
      MSG_ERROR_0("Unknown Flag value");
      return;
    }

    data[0]   = (uint8) ( appInfo_ptr->smss_last_message_number >> 8 );
    data[1]   = (uint8) ( appInfo_ptr->smss_last_message_number & 0xFF );

    data[2]   = (uint8) ( appInfo_ptr->smss_last_wap_message_number >> 8 );
    data[3]   = (uint8) ( appInfo_ptr->smss_last_wap_message_number & 0xFF );

    data[4]   = appInfo_ptr->smss_mem_full_flag;

    if (MMGSDI_SUCCESS != wms_sim_mmgsdi_write_data( appInfo_ptr,
                                                     appInfo_ptr->smss_file_enum,
                                                     0, //not used
                                                     data,
                                                     sizeof(data) ))
    {
      MSG_ERROR_0("RUIM/CSIM write error");
    }

  }
  else
  {
     MSG_ERROR_0("RUIM/CSIM was not initialized");
  }

  return;
} /* wms_cfg_update_cdma_memory_full_flag() */

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_get_sim_next_message_number

DESCRIPTION
  Get the next message number from SIM to use for an MO message.

DEPENDENCIES
  none

RETURN VALUE
  The next message number to use

SIDE EFFECTS
  The current message number is incremented.
===========================================================================*/
wms_message_number_type wms_cfg_get_sim_next_message_number(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  uint8                       data[2];
  wms_message_number_type     mn;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return 0;
  }

  mn = ++ appInfo_ptr->smss_last_message_number;

  /* Spec GSM 03.40, section 9.2.3.6 specifies that the reference number (TP-MR)
     may possess values in the range 0 to 255. */
  if (appInfo_ptr->smss_last_message_number > 255)
  {
    MSG_HIGH_0("Resetting SIM Message Ref to 0");
    appInfo_ptr->smss_last_message_number = 0;
    mn = 0;
  }

  /* Typecasting only the the lower 8 bits of 32-bit message number */
  data[0]   = (uint8) mn;
  data[1]   = appInfo_ptr->smss_mem_full_flag;
  if (MMGSDI_SUCCESS != wms_sim_mmgsdi_write_data( appInfo_ptr,
                                                   appInfo_ptr->smss_file_enum,
                                                   0, //not used
                                                   data,
                                                   sizeof(data) ))
  {
    MSG_ERROR_0("SIM write error");
  }

  MSG_HIGH_1("wms_cfg_get_sim_next_message_number() returns %d", mn);

  return mn;
} /* wms_cfg_get_sim_next_message_number() */
#endif /* FEATURE_GWSMS */


/*===========================================================================
FUNCTION wms_cfg_do_memory_status

DESCRIPTION
  Build data for memory status event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_memory_status(
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_memory_store_e_type   mem_store,
  wms_message_tag_e_type    tag,
  wms_cmd_type             *cmd_ptr
)
{
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();
  wms_message_index_type      i;
  uint32                      lower_bound   = 0;
  uint32                      upper_bound   = 0;
  wms_cfg_event_info_s_type  *cfg_event_ptr = NULL;
#ifdef FEATURE_GWSMS
  wms_gw_template_s_type      tmplt;
#endif /* FEATURE_GWSMS */

  MSG_HIGH_2("In wms_cfg_do_memory_status(): tag = %d, mem store = %d", tag, mem_store);
  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  if (!asid_is_valid(appInfo_ptr->asid))
  {
    return;
  }

  cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

  if (NULL == cfg_event_ptr)
  {
    MSG_ERROR_0("WMS_CFG_EVENT_MEMORY_STATUS not sent: memory allocation failed");
    return;
  }

  cfg_event_ptr->memory_status.mem_store      = mem_store;
  cfg_event_ptr->memory_status.tag            = tag;
  cfg_event_ptr->memory_status.free_slots     = 0;
  cfg_event_ptr->memory_status.used_tag_slots = 0;

  if( tag == WMS_TAG_MO_TEMPLATE )
  {
    /********************/
    /* Handle templates */
    /********************/
    switch( mem_store )
    {
#ifdef FEATURE_CDSMS

      case WMS_MEMORY_STORE_NV_CDMA:
        // TBD: support multiple templates in NV in the future
        {
          cfg_event_ptr->memory_status.max_slots      = 1;
          cfg_event_ptr->memory_status.free_slots     = 0;
          cfg_event_ptr->memory_status.used_tag_slots = 1;
        }
        break;

      case WMS_MEMORY_STORE_CSIM:
      case WMS_MEMORY_STORE_RUIM:
        cfg_event_ptr->memory_status.max_slots = (uint32)appInfo_ptr->smsp_rec_count;

        for( i=0; i< (uint32)appInfo_ptr->smsp_rec_count; i++ )
        {
          if( appInfo_ptr->smsp_rec_templates[i] == FALSE )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        }
        break;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
      case WMS_MEMORY_STORE_NV_GW:
        tmplt.alpha_id.data = (uint8 *)wms_mem_malloc( (size_t) WMS_ALPHA_ID_MAX);
        if ((NULL == tmplt.alpha_id.data) ||
            (FALSE == wms_nv_read_gw_params(&tmplt, appInfo_ptr->asid)))
        {
          cfg_event_ptr->memory_status.max_slots  = 0;
        }
        else
        {
          cfg_event_ptr->memory_status.max_slots      = 1;
          cfg_event_ptr->memory_status.free_slots     = 0;
          cfg_event_ptr->memory_status.used_tag_slots = 1;
        }
        WMS_MEM_FREEIF(tmplt.alpha_id.data);
        break;

      case WMS_MEMORY_STORE_USIM:
      case WMS_MEMORY_STORE_SIM:
        cfg_event_ptr->memory_status.max_slots = (uint32)appInfo_ptr->smsp_rec_count;

        for( i=0; i<(uint32)appInfo_ptr->smsp_rec_count; i++ )
        {
          if( appInfo_ptr->smsp_rec_templates[i] == FALSE )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        }
        break;
#endif /* FEATURE_GWSMS */

      default:
        break;
    }
  }
#ifdef FEATURE_GWSMS
  else if( tag == WMS_TAG_STATUS_RPT )
  {

    /********************/
    /* Handle Status Reports */
    /********************/
    switch( mem_store )
    {

    case WMS_MEMORY_STORE_SIM:
        cfg_event_ptr->memory_status.max_slots = (uint32)appInfo_ptr->smsr_rec_count;

        for( i=0; i<(uint32)appInfo_ptr->smsr_rec_count; i++ )
        {
          if( appInfo_ptr->smsr_status_rpts[i] == FALSE )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        }
        break;

      case WMS_MEMORY_STORE_RAM_GW:
        upper_bound = WMS_MAX_RAM_STATUS_REPORTS;
        cfg_event_ptr->memory_status.max_slots = WMS_MAX_RAM_STATUS_REPORTS;

        if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
        {
          cfg_event_ptr->memory_status.max_slots = WMS_MAX_RAM_STATUS_REPORTS / MAX_AS_IDS;
          lower_bound = appInfo_ptr->asid * WMS_MAX_RAM_STATUS_REPORTS / MAX_AS_IDS;
          upper_bound = lower_bound + WMS_MAX_RAM_STATUS_REPORTS / MAX_AS_IDS;
        }

        for (i = lower_bound;  i < upper_bound; i++)
        {
          if( cfg_s_ptr->ram_status_rpts[i] == FALSE )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        }
        break;


      default:
        break;
    }
  }
#endif /* FEATURE_GWSMS */
  else
  {
    /***************************/
    /* Handle regular messages */
    /***************************/
    switch( mem_store )
    {
#ifdef FEATURE_CDSMS
/*
      case WMS_MEMORY_STORE_RAM_CDMA:
        cfg_event_ptr->memory_status.max_slots  = WMS_MAX_RAM_MESSAGES;

        for(i=0; i<WMS_MAX_RAM_MESSAGES; i++)
        {
          if( cfg_s_ptr->ram_cdma_sms[i].status == WMS_TAG_NONE )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else if( tag==WMS_TAG_NONE ||
                   (cfg_s_ptr->ram_cdma_sms[i].status & 0x7) == tag )
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        } // for
        break;
*/

      case WMS_MEMORY_STORE_CSIM:
      case WMS_MEMORY_STORE_RUIM:
        cfg_event_ptr->memory_status.max_slots = (uint32)appInfo_ptr->sms_rec_count;

        for( i=0; i< (uint32) appInfo_ptr->sms_rec_count; i++ )
        {
          if( appInfo_ptr->sms_rec_tags[i] == WMS_TAG_NONE )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else if( (WMS_TAG_NONE == tag) ||
                   ((wms_message_tag_e_type)((int)appInfo_ptr->sms_rec_tags[i] & 0x7) == tag) )
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        }
        break;

      case WMS_MEMORY_STORE_NV_CDMA:
        cfg_event_ptr->memory_status.max_slots = cfg_s_ptr->nv_cdma_max_slots;

        for( i=0; i<cfg_s_ptr->nv_cdma_max_slots; i++ )
        {
          if( cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else if( (uint8)(tag == WMS_TAG_NONE ) ||
                   ( (uint8)((uint8)cfg_s_ptr->nv_cdma_tags[i] & 0x7) == (uint8)tag ))
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        }
        break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS

      case WMS_MEMORY_STORE_RAM_GW:
        upper_bound = WMS_MAX_RAM_STATUS_REPORTS;
        cfg_event_ptr->memory_status.max_slots = WMS_MAX_RAM_MESSAGES;

        if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
        {
          cfg_event_ptr->memory_status.max_slots = WMS_MAX_RAM_MESSAGES / MAX_AS_IDS;
          lower_bound = appInfo_ptr->asid * cfg_event_ptr->memory_status.max_slots;
          upper_bound = lower_bound + cfg_event_ptr->memory_status.max_slots;
        }

        for( i = lower_bound; i < upper_bound; i++)
        {
          if( NULL == cfg_s_ptr->ram_gw_sms[i] )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else if ( (WMS_TAG_NONE == tag) ||
                    ((wms_message_tag_e_type)(cfg_s_ptr->ram_gw_sms[i]->status & 0x7) == tag) )
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        } // for
        break;


    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
        cfg_event_ptr->memory_status.max_slots = (uint32)appInfo_ptr->sms_rec_count;

        for( i=0; i<(uint32) appInfo_ptr->sms_rec_count; i++ )
        {
          if( appInfo_ptr->sms_rec_tags[i] == WMS_TAG_NONE )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else if( (WMS_TAG_NONE == tag) ||
                   ((wms_message_tag_e_type)((int)appInfo_ptr->sms_rec_tags[i] & 0x7) == tag ))
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        }
        break;

      case WMS_MEMORY_STORE_NV_GW:
        upper_bound = cfg_s_ptr->nv_gw_max_slots;
        cfg_event_ptr->memory_status.max_slots = cfg_s_ptr->nv_gw_max_slots;

        if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
        {
          cfg_event_ptr->memory_status.max_slots = cfg_s_ptr->nv_gw_max_slots / MAX_AS_IDS;
          lower_bound = appInfo_ptr->asid * cfg_event_ptr->memory_status.max_slots;
          upper_bound = lower_bound + cfg_event_ptr->memory_status.max_slots;
        }

        for( i = lower_bound; i < upper_bound; i++)
        {
          if( cfg_s_ptr->nv_gw_tags[i] == WMS_TAG_NONE )
          {
            cfg_event_ptr->memory_status.free_slots ++;
          }
          else if ( (WMS_TAG_NONE == tag) ||
                    ((wms_message_tag_e_type)((int)cfg_s_ptr->nv_gw_tags[i] & 0x7) == tag ))
          {
            cfg_event_ptr->memory_status.used_tag_slots ++;
          }
        }
        break;

#endif /* FEATURE_GWSMS */

      default:
        break;
    }
  }

  cfg_event_ptr->memory_status.as_id = appInfo_ptr->asid;

  if (NULL != cmd_ptr && WMS_CMD_CFG_GET_MEMORY_STATUS == cmd_ptr->hdr.cmd_id)
  {
    cfg_event_ptr->memory_status.client_id = cmd_ptr->hdr.client_id;
    wms_cfg_event_notify_single_client( WMS_CFG_EVENT_MEMORY_STATUS, cfg_event_ptr, cfg_event_ptr->memory_status.client_id );
  }
  else
  {
    cfg_event_ptr->memory_status.client_id = WMS_CLIENT_TYPE_INTERNAL;
    wms_cfg_event_notify( WMS_CFG_EVENT_MEMORY_STATUS, cfg_event_ptr);
  }

  wms_mem_free(cfg_event_ptr); // No 'if' was validated earlier

  return;
} /* wms_cfg_do_memory_status() */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cfg_cdma_update_voice_mail_index

DESCRIPTION
  Update CDMA voice mail info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_cdma_update_voice_mail_index(
  wms_format_e_type         format,
  wms_message_tag_e_type    tag,
  uint16                    data_len,
  const uint8              *data,
  wms_message_index_type    msg_index
)
{
  wms_cfg_s_type           *cfg_s_ptr   = wms_cfg_s_ptr();
  wms_OTA_message_type     *cdma_ota    = NULL;
  wms_tl_message_type      *cdma_tl     = NULL;
  int                       fail_code   = 0;
  wms_status_e_type         fail_status = WMS_OK_S;

  if ( !WMS_IS_MT(tag) )
  {
    return;
  }

  do
  {
    cdma_ota = (wms_OTA_message_type *)wms_mem_malloc(sizeof(wms_OTA_message_type));

    if (NULL == cdma_ota)
    {
      fail_code = 1;
      break;
    }

    cdma_tl = (wms_tl_message_type *)wms_mem_malloc(sizeof(wms_tl_message_type));

    if (NULL == cdma_tl)
    {
      fail_code = 2;
      break;
    }

    cdma_ota->format   = format;
    cdma_ota->data_len = MIN(data_len, WMS_MAX_LEN);

    (void)memscpy( (uint8 *)cdma_ota->data,
                   cdma_ota->data_len,
                   data,
                   cdma_ota->data_len );

    // Decode
    {
      wms_status_e_type decode_status;

      decode_status = wms_ts_decode_OTA(cdma_ota, cdma_tl);

      if (WMS_OK_S != decode_status)
      {
        MSG_ERROR_1("NV msg %d decoding error", msg_index);
        fail_code = 3;
        fail_status = decode_status;
        break;
      }
    }

    if( (cdma_tl->mask & WMS_MASK_TL_BEARER_DATA) &&
        (cdma_tl->cl_bd.mask & WMS_MASK_BD_NUM_OF_MSGS) )
    {
      // This is a voice mail msg
      cfg_s_ptr->cdma_voice_mail_index  = msg_index;
      cfg_s_ptr->cdma_voice_mail_active = (cdma_tl->cl_bd.num_messages > 0);
    }

  } while(0);

  WMS_MEM_FREEIF(cdma_ota);
  WMS_MEM_FREEIF(cdma_tl);

  if (fail_code > 0)
  {
    MSG_ERROR_3( "wms_cfg_cdma_update_voice_mail_index() failed: msg_idx %d, code %d, status %d",
                 msg_index, fail_code, fail_status );
  }

  return;
} /* wms_cfg_cdma_update_voice_mail_index() */

#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION wms_cfg_locate_voicemail_line2

DESCRIPTION
  Locate the Voicemail Line 2 slot information. Applicable only for GW
  with ALS Feature.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_locate_voicemail_line2
(
  wms_message_index_type            *sms_index   /* INPUT/OUTPUT */
)
{
  MSG_HIGH_0("wms_cfg_locate_voicemail_line2 function is called");

#if defined (FEATURE_CPHS) && defined(FEATURE_ALS)
  { 
    wms_cfg_s_type  *cfg_s_ptr  = wms_cfg_s_ptr(); 
	
    if (cfg_s_ptr->sim_voice_mail_index_line2 == WMS_DUMMY_MESSAGE_INDEX)
    {
      MSG_ERROR_0("dummy value for voicemail index line 2");
      *sms_index = WMS_DUMMY_MESSAGE_INDEX;
      return FALSE;
    }
    else
    {
      *sms_index = cfg_s_ptr->sim_voice_mail_index_line2;
      return cfg_s_ptr->sim_voice_mail_active_line2;
    }
  }
#else /* defined (FEATURE_CPHS) && defined(FEATURE_ALS) */
  return FALSE;
#endif /* defined (FEATURE_CPHS) && defined(FEATURE_ALS) */
}

/*===========================================================================
FUNCTION wms_cfg_check_cphs_msg

DESCRIPTION
  Check for CPHS Message. Applicable only for GW with CPHS Feature.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_check_cphs_msg
(
   const wms_address_s_type     *addr
)
{
  MSG_HIGH_0("wms_cfg_check_cphs_msg function is called");

#ifdef FEATURE_CPHS
  /* CPHS specific TP-OA, see CPHS Phase 2 ref #CPHS4_2.WW6 */
  /* address length: 0x04 */
  /* type of address: 0xD0 */
  /* address value: 2 characters */
  /*    char 1 - 0x10 or 0x11 */
  /*    char 2 - 0x00 or 0x01 */
  /* Comments: */
  /*    number_of_digits is 0x02 since the type is alphanumeric */

  if (addr->number_of_digits == 0x02 &&
     addr->number_type == WMS_NUMBER_ALPHANUMERIC &&
     addr->number_plan == WMS_NUMBER_PLAN_UNKNOWN )
  {
    if ( (addr->digits[0] == 0x10 || addr->digits[0] == 0x11) &&
       (addr->digits[1] == 0x00 || addr->digits[1] == 0x01) )
    {
      return TRUE;
    }
  }
#endif /* FEATURE_CPHS */

  return FALSE;

}

#ifdef FEATURE_GWSMS

/*===========================================================================
FUNCTION wms_cfg_determine_pp_route

DESCRIPTION
  Determine message route according to message class.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
wms_routing_s_type wms_cfg_determine_pp_route(
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_gw_deliver_s_type    *deliver
)
{
  wms_routing_s_type  route       = {WMS_ROUTE_DISCARD, WMS_MEMORY_STORE_NONE};
  wms_cfg_s_type     *cfg_s_ptr   = wms_cfg_s_ptr();
  boolean             msg_waiting = FALSE;
  boolean             routes_set  = FALSE;
  int i;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return route;
  }

  if ( (SYS_MODEM_AS_ID_NONE >= appInfo_ptr->asid) || (MAX_AS_IDS <= appInfo_ptr->asid) )
  {
    MSG_ERROR_0("Invalid as_id in appInfo_ptr");
    return route;
  }

  if (deliver->dcs.msg_class < WMS_MESSAGE_CLASS_MAX)
  {
    MSG_HIGH_1("MSG_CLASS of MT SMS=%d", deliver->dcs.msg_class);

    route = cfg_s_ptr->routes[appInfo_ptr->asid].pp_routes[deliver->dcs.msg_class];

    if (WMS_MEMORY_STORE_SIM  == route.mem_store ||
        WMS_MEMORY_STORE_USIM == route.mem_store)
    {
      route.mem_store = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);
    }
  }
  else
  {
    MSG_ERROR_1("Invalid MSG Class in wms_cfg_determine_pp_route(): %d", deliver->dcs.msg_class);
    route.route     = WMS_ROUTE_DISCARD;
    route.mem_store = WMS_MEMORY_STORE_NONE;
    return route;
  }

  if (deliver->dcs.msg_waiting_kind <= WMS_GW_MSG_WAITING_OTHER)
  {
    if (WMS_GW_MSG_WAITING_DISCARD == deliver->dcs.msg_waiting)
    {
      route.route        = WMS_ROUTE_TRANSFER_AND_ACK;
      route.mem_store    = WMS_MEMORY_STORE_NONE;
      routes_set = TRUE;
    }

    if (WMS_GW_MSG_WAITING_STORE == deliver->dcs.msg_waiting)
    {
      route.route        = WMS_ROUTE_STORE_AND_NOTIFY;
      route.mem_store    = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);
      routes_set         = TRUE;
    }
  }

#ifdef FEATURE_CPHS
  if ( wms_cfg_check_cphs_msg(&deliver->address) )
  {
    /*if user data consists of a single space (0100000 in gsm 7-bit alphabet) */
    if (deliver->user_data.sm_len == 1 && deliver->user_data.sm_data[0] == 0x20)
    {
      route.route        = WMS_ROUTE_TRANSFER_AND_ACK;
      route.mem_store    = WMS_MEMORY_STORE_NONE;
    }
    else
    {
      route.route        = WMS_ROUTE_STORE_AND_NOTIFY;
      route.mem_store    = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);
    }
    routes_set = TRUE;
  }
#endif

  for (i = 0; i < deliver->user_data.num_headers; i++)
  {
    if ( (deliver->user_data.headers[i].header_id == WMS_UDH_SPECIAL_SM) &&
         (deliver->user_data.headers[i].u.special_sm.msg_waiting_kind <= WMS_GW_MSG_WAITING_VIDEOMAIL) )
    {
      if (WMS_GW_MSG_WAITING_DISCARD == deliver->user_data.headers[i].u.special_sm.msg_waiting)
      {
        route.route     = WMS_ROUTE_TRANSFER_AND_ACK;
        route.mem_store = WMS_MEMORY_STORE_NONE;
        routes_set      = TRUE;
      }
      else if (WMS_GW_MSG_WAITING_STORE == deliver->user_data.headers[i].u.special_sm.msg_waiting)
      {
        msg_waiting = TRUE;
      }
    }
  }

  /*If TP-UDH indicates that the message has to be stored or */
  /* the TP-DCS suggests that the message has to be stored */
  /* Refer 23.0.40 sec 9.2.3.24.2*/

  if (!routes_set)
  {
    if ( (WMS_PID_RETURN_CALL == deliver->pid) ||
         (WMS_GW_MSG_WAITING_STORE == deliver->dcs.msg_waiting) ||
         msg_waiting )
    {
      // store voice mail notification in SIM
      route.route     = WMS_ROUTE_STORE_AND_NOTIFY;
      route.mem_store = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);
    }
    else if (WMS_GW_MSG_WAITING_DISCARD == deliver->dcs.msg_waiting)
    {
      // Don't store it, but ack it
      route.route     = WMS_ROUTE_TRANSFER_AND_ACK;
      route.mem_store = WMS_MEMORY_STORE_NONE;
    }
    else
    {
      route = cfg_s_ptr->routes[appInfo_ptr->asid].pp_routes[deliver->dcs.msg_class];

      if ( (WMS_MEMORY_STORE_SIM  == route.mem_store) ||
           (WMS_MEMORY_STORE_USIM == route.mem_store) )
      {
        route.mem_store = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);
      }
    }
  }

  return route;
} /* wms_cfg_determine_pp_route() */

#endif /* FEATURE_GWSMS */


/*===========================================================================
FUNCTION wms_cfg_ms_get_sts_rpt_index                            (PUBLIC API)

DESCRIPTION
  Get the Status Report Index for the given SMS Message having the same
  Message Reference.

DEPENDENCIES
  none

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_ms_get_sts_rpt_index(
  sys_modem_as_id_e_type  as_id,
  wms_message_index_type  sms_index, /*IN*/
  wms_message_index_type *smsr_index /*OUT*/
)
{
  boolean                   msg_found = FALSE;
#ifdef FEATURE_GWSMS
  uint32                    idx = 0;
  wms_prov_app_info_s_type *appInfo_ptr;

  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return FALSE;
  }

  rex_enter_crit_sect(&sts_rpt_crit_sect);

  while (idx < (uint32)appInfo_ptr->smsr_rec_count && !msg_found)
  {
    if ( (appInfo_ptr->smsr_sms_index[idx] == sms_index) &&
         (appInfo_ptr->smsr_status_rpts[idx] == TRUE) )
    {
      if (NULL != smsr_index)
      {
        *smsr_index = idx;
      }
      msg_found = TRUE;
    }
    idx++;
  }

  rex_leave_crit_sect(&sts_rpt_crit_sect);
#endif /* FEATURE_GWSMS */

  return msg_found;
} /* wms_cfg_ms_get_sts_rpt_index() */


/*===========================================================================
FUNCTION wms_cfg_get_sts_rpt_index                               (PUBLIC API)

DESCRIPTION
  Get the Status Report Index for the given SMS Message having the same
  Message Reference.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_get_sts_rpt_index(
  wms_message_index_type  sms_index, /*IN*/
  wms_message_index_type *smsr_index /*OUT*/
)
{
  wms_cfg_s_type            *cfg_s_ptr = wms_cfg_s_ptr();
  sys_modem_as_id_e_type     as_id     = SYS_MODEM_AS_ID_1;

  if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
  {
    as_id = wms_get_default_as_id();
  }

  return wms_cfg_ms_get_sts_rpt_index(as_id, sms_index, smsr_index);
} /* wms_cfg_get_sts_rpt_index() */

/*===========================================================================
FUNCTION wms_cfg_ms_locate_mo_msg_for_sts_report                 (PUBLIC API)

DESCRIPTION
  Get the MO SMS Index for the given Message Reference.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_ms_locate_mo_msg_for_sts_report(
  sys_modem_as_id_e_type    as_id,
  wms_message_index_type   *rec_index,
  wms_message_number_type   message_ref
)
{
  boolean msg_found = FALSE;

#ifdef FEATURE_GWSMS
  wms_prov_app_info_s_type *appInfo_ptr;
  uint8                     i = 0;

  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return FALSE;
  }

  rex_enter_crit_sect(&sts_rpt_crit_sect);

  /*Try to Locate the MO Message for this status report*/
  while (i < (uint32)appInfo_ptr->sms_rec_count && !msg_found)
  {
    if ( WMS_IS_MO(appInfo_ptr->sms_rec_tags[i]) &&
        (appInfo_ptr->sms_reference[i] == message_ref) &&
        (message_ref != WMS_DUMMY_MESSAGE_NUMBER) )
    {
      msg_found = TRUE;
      *rec_index =i;
    }
    i++;
  }

  rex_leave_crit_sect(&sts_rpt_crit_sect);
#endif /* FEATURE_GWSMS */

  return msg_found;
} /* wms_cfg_ms_locate_mo_msg_for_sts_report() */


/*===========================================================================
FUNCTION wms_cfg_locate_mo_msg_for_sts_report                    (PUBLIC API)

DESCRIPTION
  Get the MO SMS Index for the given Message Reference.

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_locate_mo_msg_for_sts_report(
  wms_message_index_type  *rec_index,
  wms_message_number_type  message_ref
)
{
  wms_cfg_s_type            *cfg_s_ptr = wms_cfg_s_ptr();
  sys_modem_as_id_e_type     as_id     = SYS_MODEM_AS_ID_1;

  if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
  {
    as_id = wms_get_default_as_id();
  }

  return wms_cfg_ms_locate_mo_msg_for_sts_report(as_id, rec_index, message_ref);
} /* wms_cfg_locate_mo_msg_for_sts_report() */

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_determine_sts_rpt_pp_route
DESCRIPTION
  Determine message route according to message class.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_determine_sts_rpt_pp_route(
  wms_prov_app_info_s_type    *appInfo_ptr,
  wms_gw_status_report_s_type *status_report,
  wms_routing_s_type          *route,
  wms_message_index_type      *rec_id,
  wms_write_mode_e_type       *write_mode,
  wms_message_index_type      *smsr_index
)
{
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  if ( (SYS_MODEM_AS_ID_NONE >= appInfo_ptr->asid) || (MAX_AS_IDS <= appInfo_ptr->asid) )
  {
    MSG_ERROR_0("Invalid as_id in appInfo_ptr");
    return;
  }

  if ( (NULL == status_report) ||
       (NULL == route) ||
       (NULL == rec_id) ||
       (NULL == write_mode) ||
       (NULL == smsr_index) )
  {
    MSG_ERROR_0("An input parameter is NULL");
    return;
  }

  *write_mode = WMS_WRITE_MODE_INSERT;
  *rec_id     = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
  *smsr_index = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;

  if (cfg_s_ptr->routes[appInfo_ptr->asid].transfer_status_report == TRUE)
  {
    route->route     = WMS_ROUTE_TRANSFER_AND_ACK;
    route->mem_store = WMS_MEMORY_STORE_NONE;
  }
  else if (wms_cfg_ms_locate_mo_msg_for_sts_report(appInfo_ptr->asid, rec_id, status_report->message_reference))
  {
    route->route     = WMS_ROUTE_STORE_AND_NOTIFY;
    route->mem_store = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);

    /* Find out if a status report was received for this message in the past */
    if (wms_cfg_ms_get_sts_rpt_index(appInfo_ptr->asid, *rec_id, smsr_index) == TRUE)
    {
      *write_mode = WMS_WRITE_MODE_REPLACE;
    }
  }
  else
  {
    // Message not Found
    route->route     = WMS_ROUTE_TRANSFER_AND_ACK;
    route->mem_store = WMS_MEMORY_STORE_NONE;
  }

  return;
} /* wms_cfg_determine_sts_rpt_pp_route() */

/*===========================================================================
FUNCTION wms_cfg_update_voice_mail_index

DESCRIPTION
  Update the Voicemail Index.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_update_voice_mail_index(
  wms_prov_app_info_s_type  *appInfo_ptr,
  uint8                     *sim_data,
  wms_message_index_type     msg_index
)
{
  wms_msg_sim_sms_s_type            *sim_sms          = NULL;
  wms_msg_sim_sms_unpacked_s_type   *sim_sms_unpacked = NULL;
  int                                fail_code        = 0;

  MSG_HIGH_0("In wms_cfg_update_voice_mail_index");

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  do
  {
    // Voicemail records must be of type MT
    if ( !WMS_IS_MT((wms_message_tag_e_type)sim_data[0]) )
    {
      fail_code = 1;
      break;
    }

    sim_sms = (wms_msg_sim_sms_s_type *)wms_mem_malloc(sizeof(wms_msg_sim_sms_s_type));

    if (NULL == sim_sms)
    {
      fail_code = 2;
      break;
    }

    sim_sms_unpacked = (wms_msg_sim_sms_unpacked_s_type *)wms_mem_malloc(sizeof(wms_msg_sim_sms_unpacked_s_type));

    if (NULL == sim_sms_unpacked)
    {
      fail_code = 3;
      break;
    }

    // Status is first byte of sim sms record
    sim_sms->status = sim_data[0];

    // Length is WMS_SIM_SMS_MAX - 1 for the status byte
    sim_sms->len = WMS_SIM_SMS_MAX - 1;

    // Copy sim sms data out of raw sim sms
    (void)memscpy( (void *)sim_sms->data,
                   sim_sms->len,
                   sim_data + 1,
                   sim_sms->len );

    // Unpack
    {
      boolean unpack_is_successful;

      unpack_is_successful = wms_ts_unpack_sim_sms(sim_sms, sim_sms_unpacked);

      if (!unpack_is_successful)
      {
        fail_code = 4;
        break;
      }
    }

    // Check if this is a voicemail record
    {
      boolean is_voicemail;

      is_voicemail = wms_cfg_check_voicemail_contents( WMS_MESSAGE_MODE_GW,
                                                       &sim_sms_unpacked->raw_ts_data,
                                                       &appInfo_ptr->voice_mail_active,
                                                       NULL /* don't need the count */ );
      if (is_voicemail)
      {
        // This message is a voicemail record
        appInfo_ptr->voice_mail_index = msg_index;
        MSG_HIGH_1("wms_cfg_update_voice_mail_index(): voice_mail_index = %d", appInfo_ptr->voice_mail_index);
      }
    }
  } while(0);

  WMS_MEM_FREEIF(sim_sms);
  WMS_MEM_FREEIF(sim_sms_unpacked);

  if (fail_code > 0)
  {
    MSG_ERROR_1("wms_cfg_update_voice_mail_index() failed: code %d", fail_code);
  }

  return;
} /* wms_cfg_update_voice_mail_index() */

#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_cfg_update_client_bearers

DESCRIPTION
  Update the client bearer information with that stored in cfg data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_update_client_bearers(
  wms_client_s_type *client_ptr
)
{
  if ((NULL != client_ptr) && client_ptr->activated)
  {
    wms_cfg_s_type *cfg_s_ptr     = wms_cfg_s_ptr();
    int             i;

    client_ptr->preferred_bearer = WMS_BEARER_MAX;
    client_ptr->allowed_bearer   = WMS_BEARER_MAX;
    client_ptr->last_used_bearer = cfg_s_ptr->last_used_bearer;

    for (i = 0; i < (int)WMS_BEARER_MAX; i++)
    {
      /* Find and update client preferred bearer */
      if ( WMS_BEARER_USAGE_PREFERRED == cfg_s_ptr->bearer_prefs[i] )
      {
        client_ptr->preferred_bearer = (wms_bearer_e_type) i;
      }

      /* Find and update client firt allowed bearer */
      if ( (WMS_BEARER_MAX == client_ptr->allowed_bearer) &&
           (WMS_BEARER_USAGE_ALLOWED == cfg_s_ptr->bearer_prefs[i]) )
      {
        client_ptr->allowed_bearer = (wms_bearer_e_type) i;
      }

      client_ptr->active_bearers[i] = cfg_s_ptr->active_bearers[i];
    }
  }
} /* wms_cfg_update_client_bearers() */

/*===========================================================================
FUNCTION  wms_cfg_update_all_client_bearers

DESCRIPTION
  Update all the active client bearer information with that stored in cfg data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_update_all_client_bearers(
  void
)
{
  wms_client_id_type client_id;

  for (client_id = 0; client_id < (wms_client_id_type)WMS_CLIENT_TYPE_MAX; client_id++)
  {
    wms_client_s_type *client_ptr = wms_client_ptr_fetch(client_id);
    wms_cfg_update_client_bearers(client_ptr);
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }
} /* wms_cfg_update_all_client_bearers() */

/*===========================================================================
FUNCTION wms_cfg_activate_client

DESCRIPTION
  Activate the WMS Client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_activate_client(
  wms_client_id_type client_id
)
{
  wms_client_s_type *client_ptr = NULL;

  MSG_HIGH_1("cfg_activate_client: %d", client_id);

  /* Notify clients
  */
  client_ptr = wms_client_ptr_fetch( client_id );

  wms_cfg_update_client_bearers(client_ptr);

  if ( ( NULL != client_ptr ) &&
       ( TRUE == client_ptr->activated ) )
  {
    wms_prov_app_info_s_type    *appInfo_ptr = NULL;

#ifdef FEATURE_CDSMS
    {
      if ( NULL != client_ptr->cfg_event_cb )
      {
        appInfo_ptr = wms_get_appinfo_1x();

        if ( ( NULL != appInfo_ptr ) && 
               appInfo_ptr->sim_init_complete &&
               appInfo_ptr->is_available )
        {
          MSG_HIGH_4( "Sending CDMA|MS READY to client %d appInfo_ptr 0x%x, as_id %d, session_type %d",
                      client_id,
                      appInfo_ptr,
                      appInfo_ptr->asid,
                      appInfo_ptr->mmgsdi_appInfo.session_type );

          wms_cfg_send_ready_event(appInfo_ptr, client_id);
        }
      }
    }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
    {
      int as_id_index;

      for (as_id_index = 0; as_id_index < MAX_AS_IDS; as_id_index++)
      {
        appInfo_ptr = NULL;

        switch (as_id_index)
        {
          case 0: // SYS_MODEM_AS_ID1
            appInfo_ptr = wms_get_appinfo_by_session_type(MMGSDI_GW_PROV_PRI_SESSION);
            break;

          case 1: // SYS_MODEM_AS_ID2
            appInfo_ptr = wms_get_appinfo_by_session_type(MMGSDI_GW_PROV_SEC_SESSION);
            break;

          case 2: // SYS_MODEM_AS_ID3
            appInfo_ptr = wms_get_appinfo_by_session_type(MMGSDI_GW_PROV_TER_SESSION);
            break;

          default:
            break;
        }

        if ( ( NULL != appInfo_ptr ) &&  appInfo_ptr->sim_init_complete)
        {
          if ( (NULL != client_ptr->cfg_event_cb) && appInfo_ptr->is_available )
          {
            MSG_HIGH_4( "Sending GW|MS READY to client %d appInfo_ptr 0x%x, as_id %d, session_type %d",
                         client_id,
                         appInfo_ptr,
                         appInfo_ptr->asid,
                         appInfo_ptr->mmgsdi_appInfo.session_type );

            wms_cfg_send_ready_event(appInfo_ptr, client_id);
          }

          if (WMS_CLIENT_TYPE_QMI == client_id)
          {
            wms_msg_send_smsc_address_change_ind(appInfo_ptr);
          }
        }
      } // for as_id_index
    }
#endif /* FEATURE_GWSMS */
  }

  if ( ( NULL != client_ptr ) &&
       ( client_ptr->activated ) &&
       ( NULL != client_ptr->msg_event_cb ) )
  {
    wms_msg_transport_s_type* transport_ptr = NULL;
    int as_id_index;

    for (as_id_index = 0; as_id_index < MAX_AS_IDS; as_id_index++)
    {
      transport_ptr = wms_msg_choose_registered_transport(as_id_index);
      if (NULL != transport_ptr)
        wms_msg_send_transport_reg_event(transport_ptr, client_ptr);
    }
  }

  WMS_CLIENT_PTR_RELEASEIF(client_ptr);

  return;
} /* wms_cfg_activate_client() */

#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cfg_delete_dup_info_cache

DESCRIPTION
Deletes an entry in the duplicate detection message info cache.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_delete_dup_info_cache(
  wms_memory_store_e_type         mem_store,
  uint32                          index
)
{
#ifdef FEATURE_CDSMS_DUP_DETECTION
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();
  uint8                       i;

  /* Find the entry to delete from the cache */
  for (i = 0; i < WMS_MESSAGE_LIST_MAX; i++)
  {
    if( ( NULL != cfg_s_ptr->dup_detect[i] ) &&
        ( mem_store == cfg_s_ptr->dup_detect[i]->mem_store ) &&
        ( index == cfg_s_ptr->dup_detect[i]->index ) )
    {
      WMS_MEM_FREEIF(cfg_s_ptr->dup_detect[i]);
      break;
    }
  }
#endif /* FEATURE_CDSMS_DUP_DETECTION */
} /* wms_cfg_delete_dup_info_cache */

/*===========================================================================
FUNCTION wms_cfg_update_dup_info_cache

DESCRIPTION
  Updates the duplicate detection message info cache.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
/* Updates the duplicate detection message info cache.
*/
void wms_cfg_update_dup_info_cache(
  wms_memory_store_e_type         mem_store,
  uint32                          index,
  const wms_tl_message_type      *new_tl_msg
)
{
#ifdef FEATURE_CDSMS_DUP_DETECTION
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();
  uint8                       i;

  if (new_tl_msg->teleservice == WMS_TELESERVICE_WAP ||
      (wms_cm_CT_features_supported(wms_get_appinfo_1x_asid()) &&
       new_tl_msg->teleservice == WMS_TELESERVICE_CT_CDMA_WAP)
      )
  {
    /* No dup detection to be done for the above teleservices
    */
    return;
  }

  /* Find an empty slot for updating */
  for (i = 0; i < WMS_MESSAGE_LIST_MAX; i++)
  {
     if (NULL == cfg_s_ptr->dup_detect[i])
     {
       cfg_s_ptr->dup_detect[i] = (wms_msg_dup_detect_info_s_type*)
                                  wms_mem_malloc(sizeof(wms_msg_dup_detect_info_s_type));

       if (NULL == cfg_s_ptr->dup_detect[i])
       {
          return;
       }

       /* found an empty slot */
       break;
     }
  }

  if (WMS_MESSAGE_LIST_MAX == i)
  {
    return;
  }
  /* Update the memory store and index */
  cfg_s_ptr->dup_detect[i]->mem_store  = mem_store;
  cfg_s_ptr->dup_detect[i]->index      = index;

  /* Fill the info cache with the contents from the message being saved. */
  cfg_s_ptr->dup_detect[i]->tl_mask = new_tl_msg->mask;

  cfg_s_ptr->dup_detect[i]->cl_mask = new_tl_msg->cl_bd.mask;

  if(new_tl_msg->mask & WMS_MASK_TL_ADDRESS)
  {
    (void)memscpy( (void *)&cfg_s_ptr->dup_detect[i]->address,
                   sizeof(wms_address_s_type),
                   (void *)&new_tl_msg->address,
                   sizeof(wms_address_s_type));
  }

  if(new_tl_msg->mask & WMS_MASK_TL_SUBADDRESS)
  {
    (void)memscpy( (void *)&cfg_s_ptr->dup_detect[i]->sub_address,
                   sizeof(wms_subaddress_s_type),
                   (void *)&new_tl_msg->subaddress,
                   sizeof(wms_subaddress_s_type));
  }

  if(new_tl_msg->mask & WMS_MASK_TL_BEARER_DATA)
  {
    if(new_tl_msg->cl_bd.mask & WMS_MASK_BD_MSG_ID)
    {
      (void)memscpy( (void *)&cfg_s_ptr->dup_detect[i]->msg_id,
                     sizeof(wms_message_id_s_type),
                     (void *)&new_tl_msg->cl_bd.message_id,
                     sizeof(wms_message_id_s_type));
    }

    if(new_tl_msg->cl_bd.mask & WMS_MASK_BD_MC_TIME)
    {
      (void)memscpy( (void *)&cfg_s_ptr->dup_detect[i]->mc_time,
                     sizeof(wms_timestamp_s_type),
                     (void *)&new_tl_msg->cl_bd.mc_time,
                     sizeof(wms_timestamp_s_type));
    }
  }
#endif /* FEATURE_CDSMS_DUP_DETECTION */
}

#endif /* FEATURE_CDSMS */

STATIC boolean wms_cfg_UDHHeaderIsMsgWaitingHeader(
  wms_udh_s_type *hdr_ptr
)
{
  if (WMS_UDH_SPECIAL_SM == hdr_ptr->header_id)
  {
    if ( (WMS_GW_MSG_WAITING_STORE == hdr_ptr->u.special_sm.msg_waiting) ||
         (WMS_GW_MSG_WAITING_DISCARD == hdr_ptr->u.special_sm.msg_waiting) )
    {
      return TRUE;
    }
  }

  return FALSE;
}

STATIC boolean wms_cfg_GwMessageIsMsgWaiting(
  wms_client_ts_data_s_type *client_ts_ptr
)
{
  if ( (WMS_TPDU_DELIVER == client_ts_ptr->u.gw_pp.tpdu_type) ||
       (WMS_TPDU_RESERVED == client_ts_ptr->u.gw_pp.tpdu_type) )
  {
    if (WMS_PID_RETURN_CALL == client_ts_ptr->u.gw_pp.u.deliver.pid)
    {
      return TRUE;
    }

    if ( (WMS_GW_MSG_WAITING_STORE == client_ts_ptr->u.gw_pp.u.deliver.dcs.msg_waiting) ||
         (WMS_GW_MSG_WAITING_DISCARD == client_ts_ptr->u.gw_pp.u.deliver.dcs.msg_waiting) )
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*===========================================================================
FUNCTION wms_cfg_check_voicemail_contents

DESCRIPTION
  Check the Message Content for Voicemail Information.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_check_voicemail_contents
(
  wms_message_mode_e_type         msg_mode,
  const wms_raw_ts_data_s_type    *raw_ts,              /* IN  */
  boolean                         *is_voicemail_active, /* OUT */
  uint8                           *count                /* OUT */
)
{
  boolean is_voicemail = FALSE;
  boolean is_vm_active    = FALSE;
  uint8   num_of_msgs  = 0;
  int     j = 0;
  wms_client_ts_data_s_type *client_ts_ptr = NULL;

  if (NULL != raw_ts)
  {
    client_ts_ptr = (wms_client_ts_data_s_type*) wms_mem_malloc(sizeof(wms_client_ts_data_s_type));

    if (NULL == client_ts_ptr)
    {
      MSG_HIGH_0("Memory allocation for decode failed");
    }
  }

  if ( (NULL != raw_ts) &&
       (NULL != client_ts_ptr) &&
       (WMS_OK_S == wms_ts_decode(raw_ts, client_ts_ptr)) )
  {
    /* Check CDMA voicemail */
    if(msg_mode == WMS_MESSAGE_MODE_CDMA)
    {
      if(client_ts_ptr->u.cdma.mask & WMS_MASK_BD_NUM_OF_MSGS)
      {
        /* This is a voicemail message */
        is_voicemail  = TRUE;

        /* Assign the number of messages */
        num_of_msgs   = client_ts_ptr->u.cdma.num_messages;

        /* Active means there is voicemail waiting on the server */
        is_vm_active  = (client_ts_ptr->u.cdma.num_messages != 0) ? TRUE : FALSE;
      }
    }
    /* Check GW voicemail */
    else if ((msg_mode == WMS_MESSAGE_MODE_GW)&&(raw_ts->format == WMS_FORMAT_GW_PP))
    {
      if ( wms_cfg_GwMessageIsMsgWaiting(client_ts_ptr) &&
           (WMS_GW_MSG_WAITING_VOICEMAIL == client_ts_ptr->u.gw_pp.u.deliver.dcs.msg_waiting_kind) )
      {
        is_voicemail = TRUE;

        if(client_ts_ptr->u.gw_pp.u.deliver.dcs.msg_waiting
           != WMS_GW_MSG_WAITING_NONE)
        {
          is_vm_active = client_ts_ptr->u.gw_pp.u.deliver.dcs.msg_waiting_active;
        }
      }

#ifdef FEATURE_CPHS
      if (wms_cfg_check_cphs_msg(&client_ts_ptr->u.gw_pp.u.deliver.address) )
      {
        is_voicemail = TRUE;

        /* check for clear or set indicator to determine if voicemail is active */
        if (client_ts_ptr->u.gw_pp.u.deliver.address.digits[0] == 0x10)
        {
          is_vm_active = FALSE;
        }
        else if (client_ts_ptr->u.gw_pp.u.deliver.address.digits[0] == 0x11)
        {
          is_vm_active = TRUE;
        }
      }
#endif /* FEATURE_CPHS */

      for (j=0;j<client_ts_ptr->u.gw_pp.u.deliver.user_data.num_headers;j++)
      {
        wms_udh_s_type *hdr_ptr = &client_ts_ptr->u.gw_pp.u.deliver.user_data.headers[j];

        if ( wms_cfg_UDHHeaderIsMsgWaitingHeader(hdr_ptr) &&
             (WMS_GW_MSG_WAITING_VOICEMAIL == hdr_ptr->u.special_sm.msg_waiting_kind) )
        {
          is_voicemail = TRUE;

          /* Get the number of messages */
          num_of_msgs = client_ts_ptr->u.gw_pp.u.deliver.user_data.headers[j].u.
                          special_sm.message_count;

          /* Active means messages waiting */
          is_vm_active = (num_of_msgs != 0) ? TRUE : FALSE;
        }
      }
    }
  }
  else
  {
    MSG_HIGH_0("Can not perform voicemail check");
  }

  if(is_voicemail == TRUE)
  {
    /* make sure passed in pointers are valid before assignment */
    if(is_voicemail_active != NULL)
    {
      *is_voicemail_active = is_vm_active;
    }

    if(count != NULL)
    {
      *count = num_of_msgs;
    }
  }

  WMS_MEM_FREEIF(client_ts_ptr);

  return is_voicemail;
} /* wms_cfg_check_voicemail_contents */

/*=========================================================================
FUNCTION
wms_cfg_check_mwi_contents

DESCRIPTION
  Check the message for message waiting indication contents. This is a
  synchronous function call. The last parameter indicates the number of
  message waiting types in the SMS message. Caller can find out if the
  array is big enough to hold all the message waiting indication information
  by comparing the size of the passed-in array and the required array size.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If the message contains MWI information
  FALSE - otherwise

SIDE EFFECTS
  Update *mwi_info_arr and *mwi_info_arr_size_req if not NULL

=========================================================================*/
boolean wms_cfg_check_mwi_contents
(
  const wms_raw_ts_data_s_type  *message,
  wms_gw_mwi_info_s_type        *mwi_info_arr,
  int                            mwi_info_arr_size,
  int                           *mwi_info_arr_size_req
)
{
  boolean                    process_status = FALSE;
  int                        item_count     = 0;
  int                        j              = 0;
  wms_client_ts_data_s_type *client_ts_ptr  = NULL;
  wms_cfg_s_type            *cfg_s_ptr      = wms_cfg_s_ptr();

  if ( (NULL != message) && (WMS_FORMAT_GW_PP == message->format) )
  {
    client_ts_ptr = (wms_client_ts_data_s_type*) wms_mem_malloc(sizeof(wms_client_ts_data_s_type));

    if (NULL == client_ts_ptr)
    {
      MSG_HIGH_0("Memory allocation for decode failed");
    }
  }

  if ( (NULL != message) &&
       (NULL != client_ts_ptr) &&
       (WMS_OK_S == wms_ts_decode(message, client_ts_ptr)) )
  {

    if( mwi_info_arr != NULL && mwi_info_arr_size > 0 )
    {
      memset (mwi_info_arr, 0, (uint32)((uint32)mwi_info_arr_size*sizeof( wms_gw_mwi_info_s_type )));

    }

    if (wms_cfg_GwMessageIsMsgWaiting(client_ts_ptr))
    {
      if( mwi_info_arr != NULL )
      {
        mwi_info_arr[item_count].type = client_ts_ptr->u.gw_pp.u.deliver.dcs.msg_waiting_kind ;
        if( client_ts_ptr->u.gw_pp.u.deliver.dcs.msg_waiting != WMS_GW_MSG_WAITING_NONE )
        {
          mwi_info_arr[item_count].is_active = client_ts_ptr->u.gw_pp.u.deliver.dcs.msg_waiting_active;
          /*The dcs method of indicating message waiting does not provide the message waiting count
            It only specifies whether there is some message waiting. WMS will set the count to 0 as
            the information is not available */
          mwi_info_arr[item_count].count = 0;
        }
      }
      item_count ++;
    }

#ifdef FEATURE_CPHS
    if( wms_cfg_check_cphs_msg(&client_ts_ptr->u.gw_pp.u.deliver.address) )
    {
      if( mwi_info_arr != NULL && item_count < mwi_info_arr_size )
      {
        mwi_info_arr[item_count].type = WMS_GW_MSG_WAITING_VOICEMAIL;
        /* check for clear or set indicator to determine if voicemail is active */
        if (client_ts_ptr->u.gw_pp.u.deliver.address.digits[0] == 0x10)
        {
          mwi_info_arr[item_count].is_active = FALSE;
        }
        else if (client_ts_ptr->u.gw_pp.u.deliver.address.digits[0] == 0x11)
        {
          mwi_info_arr[item_count].is_active = TRUE;
        }
        /*The cphs method of indicating message waiting does not provide the message waiting count
          It only specifies whether there is some message waiting. WMS will set the count to 0 as
          the information is not available */
        mwi_info_arr[item_count].count = 0;
        // AT&T spec requires count to be set to 0xFF for active CPHS voice mail.
        if (cfg_s_ptr->ens_flag && mwi_info_arr[item_count].is_active)
        {
           mwi_info_arr[item_count].count = 0xFF;
        }
      }
      item_count++;
    }
#endif /* FEATURE_CPHS */

    for (j =0;j<client_ts_ptr->u.gw_pp.u.deliver.user_data.num_headers;j++)
    {
      wms_udh_s_type *hdr_ptr = &client_ts_ptr->u.gw_pp.u.deliver.user_data.headers[j];

      if (wms_cfg_UDHHeaderIsMsgWaitingHeader(hdr_ptr))
      {
        if( mwi_info_arr != NULL &&
            item_count < mwi_info_arr_size )
        {
          mwi_info_arr[item_count].type = hdr_ptr->u.special_sm.msg_waiting_kind;
          mwi_info_arr[item_count].count = hdr_ptr->u.special_sm.message_count;
          mwi_info_arr[item_count].is_active = (mwi_info_arr[item_count].count != 0) ? TRUE : FALSE;
        }
        item_count++;
      }
    }
  }
  else
  {
    MSG_HIGH_0("Can not perform MWI check");
  }

  if(mwi_info_arr_size_req != NULL)
  {
    *mwi_info_arr_size_req = item_count;
  }

  if( item_count > 0 )
  {
    process_status = TRUE;
  }

  WMS_MEM_FREEIF(client_ts_ptr);

  return process_status;
} /* wms_cfg_check_mwi_contents */


/*=========================================================================
FUNCTION
wms_cfg_check_cdma_duplicate

DESCRIPTION
  Check if a CDMA message is a duplicate in comparison to the messages
  stored in RUIM and NV.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  duplicate
  FALSE: not a duplicate

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_cfg_check_cdma_duplicate(
  const wms_client_message_s_type     *msg_ptr
)
{
  boolean                  is_duplicate = FALSE;
#ifdef FEATURE_CDSMS
  wms_tl_message_type     *cdma_tl;
  int                      fail_code = 0;
  wms_status_e_type        fail_status = WMS_OK_S;

  do
  {
    cdma_tl = (wms_tl_message_type *)wms_mem_malloc(sizeof(wms_tl_message_type));

    if (NULL == cdma_tl)
    {
      fail_code = 1;
      break;
    }

    wms_ts_convert_cl2tl(&msg_ptr->u.cdma_message, cdma_tl);

    // Decode the raw bearer data
    {
      wms_status_e_type decode_status;

      decode_status = wms_ts_decode_CDMA_bd( &msg_ptr->u.cdma_message.raw_ts,
                                             FALSE,
                                             FALSE,
                                             &cdma_tl->cl_bd );

      if (WMS_OK_S != decode_status)
      {
        fail_code = 2;
        fail_status = decode_status;
        break;
      }
    }

    // Validate the sub parameters
    {
      wms_status_e_type validate_status;

      validate_status = wms_ts_validate_bd_subparms( &cdma_tl->cl_bd,
                                                     cdma_tl->teleservice );

      if (WMS_OK_S != validate_status)
      {
        fail_code = 3;
        fail_status = validate_status;
        break;
      }
    }

    is_duplicate = wms_msg_cdma_check_dups(cdma_tl);

  } while(0);

  if (fail_code > 0)
  {
    MSG_ERROR_2( "wms_cfg_check_cdma_duplicate() failed: code %d, status %d",
                 fail_code,
                 fail_status );
  }
#endif /* FEATURE_CDSMS */

  return is_duplicate;
} /* wms_cfg_check_cdma_duplicate() */


#ifdef FEATURE_GWSMS_BROADCAST
/*=========================================================================
FUNCTION
wms_cfg_cell_change_ind_proc

DESCRIPTION
  Cell Change Indication Procedure

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  duplicate
  FALSE: not a duplicate

SIDE EFFECTS
  None

=========================================================================*/
void wms_cfg_cell_change_ind_proc(
  sys_modem_as_id_e_type as_id,
  cell_change_type_T     cb_cell_change
)
{
  wms_status_e_type           st            = WMS_OK_S;
  wms_cfg_event_info_s_type  *cfg_event_ptr = NULL;

  cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

  if (NULL != cfg_event_ptr)
  {
    wms_gw_cb_cell_change_e_type change_type = WMS_GW_CB_SAME_CELL;

    switch (cb_cell_change)
    {
    case CB_WMS_NO_SERVICE:
      change_type = WMS_GW_CB_NO_SERVICE;
      break;

    case CB_WMS_CELL_CHANGE:
      change_type = WMS_GW_CB_CELL_CHANGE;
      break;

    case CB_WMS_LAC_CHANGE:
      change_type = WMS_GW_CB_LAC_CHANGE;
      break;

    case CB_WMS_PLMN_CHANGE:
      change_type = WMS_GW_CB_PLMN_CHANGE;
      break;

    case CB_WMS_SAME_CELL:
    default:
       MSG_ERROR_1("Dropping unwanted Cell Change event %d", cb_cell_change);
       st = WMS_GENERAL_ERROR_S;
       break;
    }

    if (WMS_OK_S == st )
    {
      cfg_event_ptr->cell_change_type = change_type;
      wms_cfg_event_notify( WMS_CFG_EVENT_CELL_CHANGE, cfg_event_ptr );

      cfg_event_ptr->ms_cell_change.as_id            = as_id;
      cfg_event_ptr->ms_cell_change.cell_change_type = change_type;
      wms_cfg_event_notify( WMS_CFG_EVENT_MS_CELL_CHANGE, cfg_event_ptr );
    }
  }
  else
  {
    MSG_ERROR_0("WMS_CFG_EVENT_(MS_)CELL_CHANGE not sent: memory allocation failed");
  }

  WMS_MEM_FREEIF(cfg_event_ptr);

  return;
} /* wms_cfg_cell_change_ind_proc() */
#endif /* FEATURE_GWSMS_BROADCAST */

/*=========================================================================
FUNCTION
wms_cfg_check_wap_push_message

DESCRIPTION
  Check if a CDMA / GSM / WCDMA message is a WAP Push Message.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Is WAP Push Message
  FALSE: Is Not a WAP Push Message

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_cfg_check_wap_push_message
(
  const wms_client_message_s_type     *msg_ptr
)
{
  boolean ret_value = FALSE;

  MSG_HIGH_0("Function wms_cfg_check_wap_push_message entered");

  if (msg_ptr == NULL)
  {
    MSG_ERROR_0("Null Parameter Passed in wms_cfg_check_wap_push_message");
    return FALSE;
  }

  if(msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    /* Check for WAP teleservice */
    if(msg_ptr->u.cdma_message.teleservice == WMS_TELESERVICE_WAP)
    {
      ret_value = TRUE;
    }
    else if(wms_cm_CT_features_supported(wms_get_appinfo_1x_asid()) &&
             msg_ptr->u.cdma_message.teleservice == WMS_TELESERVICE_CT_CDMA_WAP)
    {
      ret_value = TRUE;
    }
#endif /* FEATURE_CDSMS */
  }
  else if (msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS
    wms_client_ts_data_s_type *client_ts_data = NULL;
    if ((client_ts_data = wms_mem_malloc(sizeof(wms_client_ts_data_s_type) )) == NULL)
    {
      MSG_ERROR_0("Memory Allocation Failed");
    }
    else
    {
      if (wms_ts_decode(&msg_ptr->u.gw_message.raw_ts_data, client_ts_data) == WMS_OK_S)
      {
        if ( (client_ts_data->u.gw_pp.tpdu_type == WMS_TPDU_DELIVER) ||
             (client_ts_data->u.gw_pp.tpdu_type == WMS_TPDU_RESERVED) )
        {
          if (client_ts_data->u.gw_pp.u.deliver.user_data_header_present)
          {
            uint8 i=0;
            for(i=0; i<client_ts_data->u.gw_pp.u.deliver.user_data.num_headers; i++)
            {
              if ((client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].header_id == WMS_UDH_PORT_16)
                  &&((client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].u.wap_16.dest_port == 2948)
                    ||(client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].u.wap_16.dest_port == 2949)))
              {
                /* Is a WAP Push Message */
                ret_value = TRUE;
                break;
              }
            }
          }
        }
      }
      else
      {
        MSG_ERROR_0("GW Message Decode Failed");
      }

      wms_mem_free(client_ts_data);
    }
#endif /* FEATURE_GWSMS */
  }
  else
  {
    MSG_ERROR_0("Incorrect Message Mode");
  }

  MSG_HIGH_1("Function wms_cfg_check_wap_push_message result = %d", ret_value);
  return ret_value;

} /* wms_cfg_check_wap_push_message() */

wms_client_type_e_type wms_cfg_check_appport_reg_client
(
  const wms_client_message_s_type     *msg_ptr
)
{


  boolean                 source_port_match = FALSE;
  boolean                 dest_port_match   = FALSE;
  wms_client_type_e_type  client_id         = WMS_CLIENT_TYPE_MAX;
  
  
  MSG_HIGH_0("Function wms_cfg_check_appport_reg_client entered");

  if (msg_ptr == NULL)
  {
    MSG_ERROR_0("Null Parameter Passed in wms_cfg_check_appport_reg_client");
    return WMS_CLIENT_TYPE_MAX;
  }

  if (msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS
     wms_client_s_type              *client_ptr;
     wms_status_e_type              err_status = WMS_OK_S;
     uint32                         source_port = 0;
     uint32                         dest_port = 0;

    client_ptr = wms_client_ptr_fetch( (wms_client_id_type) WMS_CLIENT_TYPE_IMS_APP );

    if( client_ptr != NULL &&
       ((client_ptr->source_port != WMS_PORT_MATCH_IGNORED) ||
       (client_ptr->dest_port != WMS_PORT_MATCH_IGNORED)))
    {       
       source_port = client_ptr->source_port;
       dest_port = client_ptr->dest_port;
       WMS_CLIENT_PTR_RELEASEIF(client_ptr);
    }
    else
    {
       WMS_CLIENT_PTR_RELEASEIF(client_ptr);
       return WMS_CLIENT_TYPE_MAX;
    }
    

     MSG_HIGH_2("IMS client id  source_port = %d : dest_port = %d", source_port, dest_port);

     /* validate the soucre and dest ports. If any port = PORT_NOT_USED_FOR_MATCHING , then that port will not be used for filtering */
     {
          wms_client_ts_data_s_type *client_ts_data = NULL;
          uint8 i=0;

          if ((client_ts_data = wms_mem_malloc(sizeof(wms_client_ts_data_s_type) )) == NULL)
          {
            MSG_ERROR_0("Memory Allocation Failed");
          }
          else
          {
            err_status = wms_ts_decode(&msg_ptr->u.gw_message.raw_ts_data, client_ts_data);

            if (WMS_OK_S == err_status)
            {
              if ((client_ts_data->u.gw_pp.tpdu_type == WMS_TPDU_DELIVER) &&
                    (client_ts_data->u.gw_pp.u.deliver.user_data_header_present))
              {
                
                for(i=0; i<client_ts_data->u.gw_pp.u.deliver.user_data.num_headers; i++)
                {
                  if((source_port == WMS_PORT_MATCH_IGNORED) ||
                      ((((WMS_UDH_PORT_16 == client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].header_id ) &&
                       (source_port == (uint32)client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].u.wap_16.orig_port )))||
                       ((WMS_UDH_PORT_8 == client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].header_id )&&
                        (source_port == (uint32)client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].u.wap_8.orig_port ))))
                  {
                    source_port_match = TRUE;
                  }
                  if((dest_port == WMS_PORT_MATCH_IGNORED) ||
                      ((((WMS_UDH_PORT_16 == client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].header_id ) &&
                          (dest_port == (uint32)client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].u.wap_16.dest_port )))||
                          ((WMS_UDH_PORT_8 == client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].header_id )&&
                          (dest_port == (uint32)client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].u.wap_8.dest_port ))))
                  {
                    dest_port_match = TRUE;
                  }
                  if( source_port_match && dest_port_match)
                  {
                    client_id = WMS_CLIENT_TYPE_IMS_APP;
                    break;
                  }
                }
              }
            }
            wms_mem_free(client_ts_data);
          }
     }     
#endif
  }

  MSG_HIGH_1("Function wms_cfg_check_appport_reg_client returns client_id = %d", client_id);
  
  return client_id; 
}

/*
*/
void wms_cfg_print_active_bearers
(
  void
)
{
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();

  /* Print out active bearers */
  MSG_HIGH_0("==== SMS Active Bearers ====");

  MSG_HIGH_1("WMS_BEARER_CDMA_1X: %d", cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_1X]);
  MSG_HIGH_1("WMS_BEARER_CDMA_EVDO: %d", cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_EVDO]);
  MSG_HIGH_1("WMS_BEARER_CDMA_WLAN: %d", cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_WLAN]);
  MSG_HIGH_2( "WMS_BEARER_LTE: %d, S102: %d",
              cfg_s_ptr->active_bearers[WMS_BEARER_LTE],
              cfg_s_ptr->sms_over_s102 );

} /* wms_cfg_print_active_bearers() */

/*=========================================================================
FUNCTION
wms_cfg_lte_service_available

DESCRIPTION
  Check if LTE service is available on main stack for non-SVLTE device or
  GW-HYBR stack for SVLTE device

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_cfg_lte_service_available(
  sys_srv_status_e_type srv_status,
  sys_sys_mode_e_type sys_mode
)
{
  return ( ((SYS_SRV_STATUS_LIMITED == srv_status) ||
            (SYS_SRV_STATUS_SRV == srv_status)     ||
            (SYS_SRV_STATUS_LIMITED_REGIONAL == srv_status)) &&
           ((SYS_SYS_MODE_LTE == sys_mode) ||
            (SYS_SYS_MODE_GWL == sys_mode)) );
} /*  wms_cfg_lte_service_available() */

/*=========================================================================
FUNCTION
wms_cfg_ss_change_info

DESCRIPTION
  Process SS info change.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updated CFG database.

=========================================================================*/
void wms_cfg_ss_change_info(
  wms_cmd_type *cmd_ptr
)
{
  int8                            stack_idx;
  boolean                         lte_is_available;
  wms_cmd_ss_change_info_s_type  *cmd_ss_ptr;
  wms_ss_stack_info_s_type       *cmd_ss_stack_ptr;
  wms_cfg_s_type                 *cfg_s_ptr;
  wms_prov_app_info_s_type       *appInfo_ptr = NULL;

  cfg_s_ptr                                 = wms_cfg_s_ptr();
  cmd_ss_ptr                                = &cmd_ptr->cmd.ss_change_info;
  lte_is_available                          = FALSE;
  
  // If L is on index 1, then only 1x can be on index 0 (SVLTE)
  // If L is on index 0, then G can only be on index 1 (SGLTE)
  // If L is on index 0, with index 1 not operational, this is CSFB mode of operation / SGLTE single stack operation
  //
  // Q: Is there any time when G is on index 0 and L on index 1?
  // A: No
  // Q: Can it be assumed that if LTE is found on one of the stacks that it is related to the GW_[PRI|SEC|TER] session?
  // A: Yes. LTE can only be acquired using GW_xyz session. 1X session cannot acquire LTE.

  for ( stack_idx = 0; stack_idx < cmd_ss_ptr->number_of_stacks; stack_idx++ )
  {
    cmd_ss_stack_ptr       = &cmd_ss_ptr->stack_info[stack_idx];
    appInfo_ptr            = NULL;

    if ( cmd_ss_stack_ptr->is_stack_operational )
    {
      MSG_HIGH_6( "wms_cfg_ss_change_info: as_id = %d, stack[%d]: srv_status = %d, sys_mode = %d, id_type = %d, srv_domain = %d",
                   cmd_ptr->hdr.as_id,
                   stack_idx,
                   cmd_ss_stack_ptr->srv_status,
                   cmd_ss_stack_ptr->sys_mode,
                   cmd_ss_stack_ptr->sys_id.id_type,
                   cmd_ss_stack_ptr->srv_domain );

      //  on LTE mode if CS domain FB is disabled  then there should not be any retry
      if (SYS_MODEM_AS_ID_1 == cmd_ptr->hdr.as_id)
      {
         if ( NULL != cfg_s_ptr )
         {
            if (SYS_SRV_STATUS_SRV == cmd_ss_stack_ptr->srv_status)
            {
               cfg_s_ptr->sys_mode = cmd_ss_stack_ptr->sys_mode;
            }
            else
            {
               cfg_s_ptr->sys_mode = SYS_SYS_MODE_NO_SRV;
            }
         }
         else
         {
            MSG_ERROR_0("cfg_s_ptr was NULL hence not captured sys_mode");
         }
      }

      //IS95 (CDMA)
      if ( SYS_SYS_ID_TYPE_IS95 == cmd_ss_stack_ptr->sys_id.id_type )
      {
        cfg_s_ptr->hs_based_plus_dial_setting         = cmd_ss_ptr->hs_based_plus_dial_setting;
        cfg_s_ptr->home_mobile_country_code           = cmd_ss_ptr->home_mobile_country_code;
        cfg_s_ptr->current_mobile_country_code        = cmd_ss_ptr->current_mobile_country_code;
        cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_1X] = (SYS_SRV_STATUS_SRV == cmd_ss_stack_ptr->srv_status);

  #ifdef FEATURE_CDSMS
        appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode( SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA );

        if ( NULL != appInfo_ptr )
        {
          appInfo_ptr->number_of_stacks         = cmd_ss_ptr->number_of_stacks;
          appInfo_ptr->stack_info[stack_idx]    = cmd_ss_ptr->stack_info[stack_idx];

          appInfo_ptr->in_roaming = (SYS_ROAM_STATUS_ON == cmd_ss_stack_ptr->roam_status);
          MSG_HIGH_1("in_roaming global set as %d", appInfo_ptr->in_roaming);

          if (SYS_SRV_STATUS_SRV == cmd_ss_stack_ptr->srv_status)
          {
            appInfo_ptr->srv_available = TRUE;
            wms_cfg_report_network_service_avail_event(appInfo_ptr);
          }

          // on SYS_MODEM_AS_ID_1 , there are two possible appInfo (each having at max 2 stacks). 
          // However, at a time maximum 2 stacks out of these 4 can be there.
          // Other 2 stacks will be unused.
          // So, while updating the stack info at an index in one appInfo,
          // Clear the stack info at corresponding index in other appInfo since it shouldn't be used.
          
          // Clear app_info_ptr->stack_info[stack_idx] data from GWL appInfo.
          appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode( SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_GW );

          if (NULL != appInfo_ptr)
          {
            MSG_HIGH_1("Clear up GW appInfo_ptr->stack_info[%d] on SYS_MODEM_AS_ID_1", stack_idx);
            memset(&appInfo_ptr->stack_info[stack_idx], 0, sizeof(wms_ss_stack_info_s_type));
          }
        }
        else
        {
          MSG_ERROR_0("appInfo for CDMA message mode is NULL");
        }
  #endif /* FEATURE_CDSMS */
      }
      else if(SYS_SYS_ID_TYPE_IS856 == cmd_ss_stack_ptr->sys_id.id_type )
      {
#ifdef FEATURE_CDSMS
        appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode( SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA );

        if ( NULL != appInfo_ptr )
        {
          appInfo_ptr->stack_info[stack_idx]    = cmd_ss_ptr->stack_info[stack_idx];
        }
        else
        {
          MSG_ERROR_0("appInfo for CDMA message mode is NULL");
        }

        // on SYS_MODEM_AS_ID_1 , there are two possible appInfo (each having at max 2 stacks). 
        // However, at a time maximum 2 stacks out of these 4 can be there.
        // Other 2 stacks will be unused.
        // So, while updating the stack info at an index in one appInfo,
        // Clear the stack info at corresponding index in other appInfo since it shouldn't be used.
        
        // Clear app_info_ptr->stack_info[stack_idx] data from GWL appInfo.
        appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode( SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_GW );

        if (NULL != appInfo_ptr)
        {
          MSG_HIGH_1("Clear up GW appInfo_ptr->stack_info[%d] on SYS_MODEM_AS_ID_1", stack_idx);
          memset(&appInfo_ptr->stack_info[stack_idx], 0, sizeof(wms_ss_stack_info_s_type));
        }
#endif /* FEATURE_CDSMS */
      }
#ifdef FEATURE_GWSMS
      // UMTS (TDS/WCDMA/GSM/LTE)
      else if ( SYS_SYS_ID_TYPE_UMTS == cmd_ss_stack_ptr->sys_id.id_type )
      {
       if( (SYS_SRV_STATUS_SRV == cmd_ss_stack_ptr->srv_status) && 
                    (SYS_SYS_MODE_LTE == cmd_ss_stack_ptr->sys_mode) )  
       {
       
         cfg_s_ptr->active_bearers[WMS_BEARER_LTE] = ( (SYS_SRV_STATUS_SRV == cmd_ss_stack_ptr->srv_status) && 
                                                      (SYS_SYS_MODE_LTE == cmd_ss_stack_ptr->sys_mode) );
         cfg_s_ptr->lte_sub_info = cmd_ptr->hdr.as_id;
       }
       else if(cfg_s_ptr->active_bearers[WMS_BEARER_LTE] && (cfg_s_ptr->lte_sub_info == cmd_ptr->hdr.as_id) )
       {
         cfg_s_ptr->active_bearers[WMS_BEARER_LTE]= FALSE;
         cfg_s_ptr->lte_sub_info = SYS_MODEM_AS_ID_NONE;
       }

        lte_is_available                            = wms_cfg_lte_service_available( cmd_ss_stack_ptr->srv_status,
                                                                                     cmd_ss_stack_ptr->sys_mode );
        appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

        if (NULL != appInfo_ptr)
        {
          //For 1xCSFB, disable 1x bearer when UE moves back to LTE.
          if ( (SYS_SYS_ID_TYPE_IS95 == appInfo_ptr->stack_info[stack_idx].sys_id.id_type) &&
               (appInfo_ptr->stack_info[stack_idx].asubs_id == cmd_ss_ptr->stack_info[stack_idx].asubs_id) )
          {
            cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_1X] = FALSE;
            appInfo_ptr->srv_available = FALSE;
            wms_cfg_report_network_service_not_avail_event(appInfo_ptr);
          }
          appInfo_ptr->stack_info[stack_idx] = cmd_ss_ptr->stack_info[stack_idx];
        }
        appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode( cmd_ptr->hdr.as_id, WMS_MESSAGE_MODE_GW );

        if ( (NULL == appInfo_ptr) && 
             ( (SYS_SRV_STATUS_LIMITED == cmd_ss_stack_ptr->srv_status) ||
                (SYS_SRV_STATUS_LIMITED_REGIONAL == cmd_ss_stack_ptr->srv_status) ) )
        {
          MSG_ERROR_0("No valid as_id mapping yet - using default as_id"); 
          appInfo_ptr = wms_get_gw_appinfo_by_asid( cmd_ptr->hdr.as_id );

          if (NULL != appInfo_ptr)               
          {
            appInfo_ptr->asid = cmd_ptr->hdr.as_id;
          }
        }

        if ( NULL != appInfo_ptr )
        {
          appInfo_ptr->number_of_stacks      = cmd_ss_ptr->number_of_stacks;              
          appInfo_ptr->stack_info[stack_idx] = cmd_ss_ptr->stack_info[stack_idx];

          //  Revert back to original domain preference due to certain cell change
          //  events and notify NAS.
          if ( (cmd_ptr->cmd.ss_change_info.umts_plmn_changed) &&
               (appInfo_ptr->curr_gw_domain_pref != appInfo_ptr->gw_domain_pref) )
          {
            MSG_HIGH_2("PLMN has changed. Restoring domain pref from %d to %d",
                        appInfo_ptr->curr_gw_domain_pref,
                        appInfo_ptr->gw_domain_pref );

            appInfo_ptr->curr_gw_domain_pref = appInfo_ptr->gw_domain_pref;
            (void) wms_cfg_mm_set_domain_pref(cmd_ptr->hdr.as_id, appInfo_ptr->curr_gw_domain_pref);
          }

          // Allow ETWS and CMAS when in limited service without SIM initialization
          if ( (SYS_SRV_STATUS_LIMITED == cmd_ss_stack_ptr->srv_status) ||
               (SYS_SRV_STATUS_LIMITED_REGIONAL == cmd_ss_stack_ptr->srv_status) )
          {
            if (!appInfo_ptr->sim_init_complete)
            {
              // SIM initialization is not completed, send the broadcast
              // configuration to NAS if it hasn't already been sent.
              if ( !appInfo_ptr->limited_bc_config_sent &&
                   !appInfo_ptr->sim_file_init_is_done[CBMIR_FILE_INDEX] &&
                   !appInfo_ptr->sim_file_init_is_done[CBMI_FILE_INDEX] )
              {
                // Send NV-only BC config to NAS
                wms_bc_gw_cb_init(appInfo_ptr, FALSE);

                appInfo_ptr->limited_bc_config_sent = TRUE;
              }
            }    
          }

  #ifdef FEATURE_SMS_PWS_OVER_LTE
          // Need to support ETWS and CMAS on an acceptable cell while in Limited Service (eg no SIM)
          // according to TS 36.304
          if (lte_is_available)
          {
            MSG_HIGH_0("LTE service is available. Send search list to RRC");
            if ( !appInfo_ptr->sim_init_complete )
            {
              // SIM initialization is not completed, send the broadcast
              // configuration to NAS if it hasn't already been sent.
              if ( !appInfo_ptr->limited_bc_config_sent &&
                   !appInfo_ptr->sim_file_init_is_done[CBMIR_FILE_INDEX] &&
                   !appInfo_ptr->sim_file_init_is_done[CBMI_FILE_INDEX] )
              {
                MSG_HIGH_0("Calling wms_bc_gw_cb_init()");

                // Send NV-only BC config to NAS
                wms_bc_gw_cb_init(appInfo_ptr, FALSE);
                appInfo_ptr->limited_bc_config_sent = TRUE;
              }
            }
            else
            {
              MSG_HIGH_0( "Calling wms_bc_gw_cb_enable_to_nas");
              wms_bc_gw_cb_enable_to_nas(appInfo_ptr);
            }
          }
  #endif /* FEATURE_SMS_PWS_OVER_LTE */

          // Perform SMMA procedure if needed
          wms_msg_notify_SMMA( appInfo_ptr, FALSE );

          if (SYS_MODEM_AS_ID_1 == cmd_ptr->hdr.as_id)
          {
            // on SYS_MODEM_AS_ID_1 , there are two possible appInfo (each having at max 2 stacks). 
            // However, at a time maximum 2 stacks out of these 4 can be there.
            // Other 2 stacks will be unused.
            // So, while updating the stack info at an index in one appInfo,
            // Clear the stack info at corresponding index in other appInfo since it shouldn't be used.
  
            // Clear app_info_ptr->stack_info[stack_idx] data from CDMA appInfo.
            appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode( SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA );
	  
            if (NULL != appInfo_ptr)
            {
              MSG_HIGH_1("Clear up CDMA appInfo_ptr->stack_info[%d] on SYS_MODEM_AS_ID_1", stack_idx);
              memset(&appInfo_ptr->stack_info[stack_idx], 0, sizeof(wms_ss_stack_info_s_type));
            }
          }
        }
        else
        {
          MSG_ERROR_0("appInfo for GW message mode is NULL");
        }
      }
    }   //end if(is_operational)
#endif /* FEATURE_GWSMS */
  } //end for()

  // Update active bearers
  wms_cfg_update_all_client_bearers();
  wms_cfg_print_active_bearers();

  // Update the GW appinfo with the LTE availablility
  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode( cmd_ptr->hdr.as_id, WMS_MESSAGE_MODE_GW );

  if ( NULL != appInfo_ptr )
  {
    appInfo_ptr->lte_is_available = lte_is_available;

    // Notify SMS client on network service status
    wms_cfg_report_network_service( appInfo_ptr );
  }
  else
  {
    MSG_ERROR_1("Could not update LTE service status on as_id %d", cmd_ptr->hdr.as_id);
  }

  return;
} /* wms_cfg_ss_change_info() */


/*=========================================================================
FUNCTION
wms_cfg_ipapp_change_info

DESCRIPTION
  Process IPApp info change.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updated CFG database.

=========================================================================*/
void wms_cfg_ipapp_change_info(
  const cm_ipapp_info_s_type    *ipapp_info_ptr
)
{
  wms_cfg_s_type             *cfg_s_ptr     = wms_cfg_s_ptr();
  boolean                     sts;

  sts = (CM_IPAPP_REG_STATUS_REGISTERED == ipapp_info_ptr->reg_status) ? TRUE : FALSE;

  MSG_HIGH_2( "CFG: wms_cfg_ipapp_change_info with mode mask (%d), status (%d).",
            ipapp_info_ptr->sys_mode[CM_CALL_TYPE_SMS], sts);
#ifdef FEATURE_CDSMS
  if ( (int)ipapp_info_ptr->sys_mode[CM_CALL_TYPE_SMS] & (int)SYS_SYS_MODE_MASK_HDR )
  {
    cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_EVDO] = sts;
    MSG_HIGH_1("WMS_BEARER_CDMA_EVDO: %d", sts);
  }
#endif /* FEATURE_CDSMS */

  if ( (int)ipapp_info_ptr->sys_mode[CM_CALL_TYPE_SMS] & (int)SYS_SYS_MODE_MASK_WLAN )
  {
#ifdef FEATURE_CDSMS
    cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_WLAN] = sts;
    MSG_HIGH_1("WMS_BEARER_CDMA_WLAN: %d", sts);
#endif /* FEATURE_CDSMS */
  }

  // No service -- reset all IP bearers ONLY
  // Assumption: IP bearers are known to work in the following
  // systems - DO, WLAN(IS637).
  if ( SYS_SYS_MODE_MASK_NONE == ipapp_info_ptr->sys_mode[CM_CALL_TYPE_SMS] ||
       (int)ipapp_info_ptr->sys_mode[CM_CALL_TYPE_SMS] & (int)SYS_SYS_MODE_MASK_NO_SRV )
  {
    MSG_HIGH_0("No service, resetting IP bearers ONLY");
    cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_EVDO] = FALSE;
    cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_WLAN] = FALSE;
  }

  wms_cfg_update_all_client_bearers();
  wms_cfg_print_active_bearers();
  return;

} /* wms_cfg_ipapp_change_info() */


/*=========================================================================
FUNCTION wms_cfg_handle_gw_lpm

DESCRIPTION
  Process the LPM (Low Power Mode) event for GW subscriptions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The broadcast configurations are disabled in NAS.
  LTE duplicate detection lists are cleared.

=========================================================================*/
void wms_cfg_handle_gw_lpm(
  boolean                            in_lpm
)
{
#ifdef FEATURE_GWSMS_BROADCAST
  wms_prov_app_info_s_type   *appInfo_ptr = NULL;
  sys_modem_as_id_e_type      asid;

  MSG_HIGH_1("In wms_cfg_handle_gw_lpm(lpm %d)", in_lpm);

  // Since LPM is a system state, there really is no modem
  // stack associated with this command.  Basically it applies
  // to all subscriptions. Disable/enable all.

 for (asid = SYS_MODEM_AS_ID_1; asid < MAX_AS_IDS; asid++)
  {
    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(asid, WMS_MESSAGE_MODE_GW);

    if ( (NULL != appInfo_ptr) )
    {
      if (in_lpm)
      {
        (void)wms_cm_gw_cb_disable(appInfo_ptr->asid);
        wms_cfg_report_not_ready_event(appInfo_ptr);
      }
      else
      {
        wms_bc_gw_cb_enable_to_nas(appInfo_ptr);
        if (appInfo_ptr->sim_init_complete)
        {
          MSG_HIGH_2( "LPM mode = %d, WMS sim_init_complete = %d",
                      in_lpm,
                      appInfo_ptr->sim_init_complete );

          MSG_HIGH_2( "Send  WMS_CFG_EVENT_GW_READY event: appInfo_ptr 0x%x, asid %d",
                      appInfo_ptr,
                      appInfo_ptr->asid );
          
          wms_cfg_send_ready_event(appInfo_ptr, WMS_CLIENT_ALL);
        }
        else
        {
          MSG_HIGH_0("Waiting on CM event to start initialization");
        }
      }
    }
  }

#ifdef FEATURE_SMS_PWS_OVER_LTE
  if (in_lpm)
  {
    wms_bc_clear_duplists();
  }
#endif /* FEATURE_SMS_PWS_OVER_LTE */
#endif /* FEATURE_GWSMS_BROADCAST */

  return;
} /* wms_cfg_handle_gw_lpm() */

/*=========================================================================
FUNCTION wms_cfg_handle_cdma_lpm

DESCRIPTION
  Process the LPM (Low Power Mode) event for CDMA subscription.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_cfg_handle_cdma_lpm(
  boolean in_lpm
)
{
#ifdef FEATURE_CDSMS
  wms_prov_app_info_s_type   *appInfo_ptr = NULL;

  MSG_HIGH_0("In wms_cfg_handle_cdma_lpm()");

  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

  if ( (NULL != appInfo_ptr) )
  {
    if (in_lpm)
    {
      wms_cfg_report_not_ready_event(appInfo_ptr);
    }
    else
    {
      if (appInfo_ptr->sim_init_complete)
      {
        MSG_HIGH_2( "LPM mode = %d, WMS sim_init_complete = %d",
                    in_lpm,
                    appInfo_ptr->sim_init_complete );

        MSG_HIGH_2( "Send  WMS_CFG_EVENT_CDMA_READY event: appInfo_ptr 0x%x, asid %d",
                    appInfo_ptr,
                    appInfo_ptr->asid );

        wms_cfg_send_ready_event(appInfo_ptr, WMS_CLIENT_ALL);
      }
      else
      {
        MSG_HIGH_0("Waiting on CM event to start initialization");
      }
    }
  }
#endif /* FEATURE_CDSMS */

  return;
} /* wms_cfg_handle_cdma_lpm() */

/*=========================================================================
FUNCTION wms_cfg_lpm_proc

DESCRIPTION
  Process the LPM (Low Power Mode) event.

  LPM is used by many carriers during testing to indicate a power off
  without actually powering off the handset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updated CFG database.

=========================================================================*/
void wms_cfg_lpm_proc(
  wms_cmd_type    *cmd_ptr
)
{
  if (NULL != cmd_ptr)
  {
    MSG_HIGH_1("LPM: low power mode set as %d", cmd_ptr->cmd.oprt_mode.in_lpm);

    if (cmd_ptr->cmd.oprt_mode.in_lpm)
    {
      wms_msg_offline();
    }

    wms_cfg_handle_cdma_lpm(cmd_ptr->cmd.oprt_mode.in_lpm);
    wms_cfg_handle_gw_lpm(cmd_ptr->cmd.oprt_mode.in_lpm);
  }

  return;
} /* wms_cfg_lpm_proc() */

/*===========================================================================
FUNCTION wms_cfg_check_1xsrlte_or_ehrpd_oper_mode

DESCRIPTION
  This function check wether a device operates in 1XSRLTE or not. 
 
DEPENDENCIES
  none

RETURN VALUE
  FALSE:  Not in 1XSRLTE mode
  TRUE:   In 1XSRLTE mode or EHRPD Mode
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_check_1xsrlte_or_ehrpd_oper_mode( 
  sys_modem_as_id_e_type as_id ) 
{
  wms_cfg_s_type           *lcfg_s_ptr     = wms_cfg_s_ptr();
  boolean ret_val = FALSE;
  
  /* 1X and LTE are available for both SVLTE and SRLTE modes of operation.
     UE mode will determine the mode of operation. */
  if (as_id < MAX_AS_IDS && SYS_SUBS_FEATURE_MODE_SRLTE == lcfg_s_ptr->sub_feature_mode[as_id])
  {
    ret_val = (wms_check_srv_avail_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_CDMA, SYS_SYS_MODE_CDMA) &&
               wms_check_srv_avail_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW, SYS_SYS_MODE_LTE) ) ;

    if (FALSE == ret_val)
    {
      // For IMS on EHRPD, Both 1X and HDR should be simultaneously active. Also Hybr_Pref whould be set to ON
      if(CM_HYBR_PREF_OFF != lcfg_s_ptr->hybr_pref)
      {
        ret_val = (wms_check_srv_avail_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_CDMA, SYS_SYS_MODE_CDMA) &&
                   wms_check_srv_avail_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_CDMA, SYS_SYS_MODE_HDR) );
      }

    }
  }
  
  MSG_HIGH_1("In wms_cfg_check_1xsrlte_or_ehrpd_oper_mode returns %d", ret_val);
  return ret_val;
} /* wms_cfg_check_1xsrlte_or_ehrpd_oper_mode() */

/*===========================================================================
FUNCTION wms_cfg_check_hvolte_oper_mode

DESCRIPTION
  This function check wether a device operates in hVOLTE or not. 
 
DEPENDENCIES
  none

RETURN VALUE
  FALSE:  Not in hVOLTE mode
  TRUE:   In hVOLTE mode 
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_check_hvolte_oper_mode( 
  void
) 
{
  wms_cfg_s_type           *cfg_s_ptr     = wms_cfg_s_ptr();
  boolean                   ret_val       = FALSE;
  
  /* Device is in hVoLTE mode of operation if:
     - Device is in SRLTE mode. 
     - LTE is on main stack. 
  */
  if ( ( SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED == cfg_s_ptr->ue_mode ) ||
       ( SYS_UE_MODE_1XSRLTE_ONLY           == cfg_s_ptr->ue_mode ) )
       
  {
    wms_prov_app_info_s_type   *appInfo_ptr = NULL;

    appInfo_ptr = wms_get_lte_appinfo();
    
    if ( NULL != appInfo_ptr && SYS_MODEM_AS_ID_1 == appInfo_ptr->asid ) 
    {
      MSG_HIGH_2("appInfo_ptr = 0x%x, as id = %d", appInfo_ptr, appInfo_ptr->asid);

      if ( (appInfo_ptr->stack_info[0].is_stack_operational) &&
           (SYS_SYS_MODE_LTE == appInfo_ptr->stack_info[0].sys_mode) )
      {
        MSG_HIGH_0("wms_cfg_check_hvolte_oper_mode(): LTE is on main stack");
        ret_val = TRUE;
      }
    }
  }
  
  return ret_val;
} /* wms_cfg_check_hvolte_oper_mode() */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_update_msg_ref_for_mo_msg

DESCRIPTION
  This function fills in the appropriate slot of the message reference array of
  the configuration manager at startup. The message reference number is required only for
  those MO messages which are waiting on the STATUS report

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_update_msg_ref_for_mo_msg(
  wms_prov_app_info_s_type  *appInfo_ptr,
  uint8                     *sim_data,
  wms_message_index_type     msg_index
)
{
  wms_msg_sim_sms_s_type            *sim_sms          = NULL;
  wms_msg_sim_sms_unpacked_s_type   *sim_sms_unpacked = NULL;
  int                                fail_code        = 0;
  wms_status_e_type                  fail_status      = WMS_OK_S;

  MSG_HIGH_0("In wms_cfg_update_msg_ref_for_mo_msg");

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  do
  {
    sim_sms = (wms_msg_sim_sms_s_type *)wms_mem_malloc(sizeof(wms_msg_sim_sms_s_type));

    if (NULL == sim_sms)
    {
      fail_code = 1;
      break;
    }

    sim_sms_unpacked = (wms_msg_sim_sms_unpacked_s_type *)wms_mem_malloc(sizeof(wms_msg_sim_sms_unpacked_s_type));

    if (NULL == sim_sms_unpacked)
    {
      fail_code = 2;
      break;
    }
    // Status is first byte of sim sms record
    sim_sms->status = sim_data[0];
    // Length is record length - 1 for the status byte
    sim_sms->len    = (uint8)MIN(appInfo_ptr->sms_rec_len - 1, WMS_SIM_SMS_MAX-1);

    // Copy sim sms data out of raw sim sms
    (void)memscpy( (void *)sim_sms->data,
                   sim_sms->len,
                   sim_data + 1,
                   sim_sms->len );

    {
      boolean unpack_is_successful;

      unpack_is_successful = wms_ts_unpack_sim_sms(sim_sms, sim_sms_unpacked);

      if (!unpack_is_successful)
      {
        fail_code = 3;
        break;
      }
    }

    {
      wms_client_ts_data_s_type *client_ts_ptr = NULL;
      wms_status_e_type          decode_status;

      client_ts_ptr = (wms_client_ts_data_s_type *)wms_mem_malloc(sizeof(wms_client_ts_data_s_type));

      if (NULL == client_ts_ptr)
      {
        fail_code = 4;
        break;
      }

      decode_status = wms_ts_decode(&sim_sms_unpacked->raw_ts_data, client_ts_ptr);

      if (WMS_OK_S != decode_status)
      {
        fail_code = 5;
        fail_status = decode_status;
        break;
      }

      // Update the message reference
      appInfo_ptr->sms_reference[msg_index] = client_ts_ptr->u.gw_pp.u.submit.message_reference;

      wms_mem_free(client_ts_ptr);
    }

  } while(0);

  WMS_MEM_FREEIF(sim_sms);
  WMS_MEM_FREEIF(sim_sms_unpacked);

  if (fail_code > 0)
  {
    MSG_ERROR_2( "wms_cfg_update_msg_ref_for_mo_msg() failed: code %d, status %d",
                 fail_code,
                 fail_status );
  }

  return;
} /* wms_cfg_update_msg_ref_for_mo_msg() */

/*===========================================================================
FUNCTION wms_cfg_update_mwis_info

DESCRIPTION
  This function updates the EFmwis with the MWI information.
  EFmwis file layout:
       Bytes  Description                                  M/O  Length
         1    Message Waiting Indicator Status              M   1 byte
         2    Number of Voicemail Messages Waiting          M   1 byte
         3    Number of Fax Messages Waiting                M   1 byte
         4    Number of Electronic Mail Messages Waiting    M   1 byte
         5    Number of Other Messages Waiting              M   1 byte
         6    Number of Videomail Messages waiting          O   1 byte
DEPENDENCIES
  none

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_update_mwis_info(
  wms_prov_app_info_s_type      *appInfo_ptr,
  const wms_raw_ts_data_s_type  *message
)
{
  int                      i = 0;
  int                      count = 0;
  boolean                  write_to_file = TRUE;
  /*Assuming there is only one record stored in the EFmwis.
    Each record will contain information of different MWIS*/
  wms_gw_mwi_info_s_type   mwi_info_arr[WMS_SIM_MWI_REC_LEN_MAX - 1];
  uint8                    sim_data[WMS_SIM_MWI_REC_LEN_MAX];

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return;
  }

  if ( (0 == appInfo_ptr->mwis_rec_len) ||
       FALSE == wms_cfg_check_mwi_contents( message,
                                            mwi_info_arr,
                                            WMS_SIM_MWI_REC_LEN_MAX - 1,
                                            &count) )
  {
    MSG_HIGH_2( "Not a MWI message. MWI count in msg = %d, EFmwis rec len = %d",
                count,
                appInfo_ptr->mwis_rec_len );
    return;
  }

  memset( sim_data, 0, (uint32)appInfo_ptr->mwis_rec_len);

  if (wms_sim_mmgsdi_read_data( appInfo_ptr,
                                appInfo_ptr->mwis_file_enum,
                                0, // first record
                                appInfo_ptr->mwis_rec_len,
                                sim_data,
                                sizeof(sim_data)) == MMGSDI_SUCCESS)
  {
    MSG_HIGH_0("EFmwis file read successfully");
  }
  else
  {
    MSG_HIGH_0("EFmwis file read unsuccessfully...not trying to write MWIS information");
    return;
  }

  for (i = 0; i < count; i++)
  {
    switch( mwi_info_arr[i].type )
    {
      case WMS_GW_MSG_WAITING_VOICEMAIL:
        if (mwi_info_arr[i].is_active == TRUE)
        {
          sim_data[0] |= WMS_MASK_EF_MWIS_VOICEMAIL;
        }
        else
        {
          sim_data[0] &= ~WMS_MASK_EF_MWIS_VOICEMAIL;
        }
        sim_data[WMS_EF_MWIS_VOICEMAIL_POS] = (uint8) mwi_info_arr[i].count;
        break;

      case WMS_GW_MSG_WAITING_FAX:
        if (mwi_info_arr[i].is_active == TRUE)
        {
          sim_data[0] |= WMS_MASK_EF_MWIS_FAX;
        }
        else
        {
          sim_data[0] &= ~WMS_MASK_EF_MWIS_FAX;
        }
        sim_data[WMS_EF_MWIS_FAX_POS] = (uint8) mwi_info_arr[i].count;
        break;

      case WMS_GW_MSG_WAITING_EMAIL:
        if (mwi_info_arr[i].is_active == TRUE)
        {
          sim_data[0] |= WMS_MASK_EF_MWIS_EMAIL;
        }
        else
        {
          sim_data[0] &= ~WMS_MASK_EF_MWIS_EMAIL;
        }
        sim_data[WMS_EF_MWIS_EMAIL_POS] = (uint8) mwi_info_arr[i].count;
        break;

      case WMS_GW_MSG_WAITING_OTHER:
        if (mwi_info_arr[i].is_active == TRUE)
        {
          sim_data[0] |= WMS_MASK_EF_MWIS_OTHERS;
        }
        else
        {
          sim_data[0] &= ~WMS_MASK_EF_MWIS_OTHERS;
        }
        sim_data[WMS_EF_MWIS_OTHERS_POS] = (uint8) mwi_info_arr[i].count;
        break;

      case WMS_GW_MSG_WAITING_VIDEOMAIL:
        if (appInfo_ptr->mwis_rec_len <= WMS_EF_MWIS_VIDEOMAIL_POS)
        {
          MSG_HIGH_0("EFmwis file does not support MWI Videomail");
          write_to_file = FALSE;
        }
        else
        {
          if (mwi_info_arr[i].is_active == TRUE)
          {
            sim_data[0] |= WMS_MASK_EF_MWIS_VIDEOMAIL;
          }
          else
          {
            sim_data[0] &= ~WMS_MASK_EF_MWIS_VIDEOMAIL;
          }
          sim_data[WMS_EF_MWIS_VIDEOMAIL_POS] = (uint8) mwi_info_arr[i].count;
        }
        break;

      default:
        write_to_file = FALSE;
        break;
    }
  }

  if (write_to_file == TRUE)
  {
    /* write to SIM*/
    if (MMGSDI_SUCCESS == wms_sim_mmgsdi_write_data( appInfo_ptr,
                                                     appInfo_ptr->mwis_file_enum,
                                                     0,
                                                     sim_data,
                                                     (uint32)appInfo_ptr->mwis_rec_len ) )
    {
      MSG_HIGH_0("EFmwis file has been updated successfully");
    }
    else
    {
      MSG_HIGH_0("EFmwis file update failed");
    }
  }
  else
  {
    MSG_HIGH_0("Nothing to write to EFmwis file");
  }

  return;
} /* wms_cfg_update_mwis_info() */

void wms_cfg_gw_cb_error_ind_proc(
  cb_error_indication_type *cb_error_ind_ptr
)
{
  if (NULL != cb_error_ind_ptr)
  {
    boolean                 ok_to_notify = TRUE;
    wms_gw_cb_error_e_type  err_type     = WMS_GW_CB_ERROR_MEMORY_FULL;

    MSG_HIGH_1("wms_cfg_gw_cb_error_ind_proc: error = %d", cb_error_ind_ptr->error_value);

    switch(cb_error_ind_ptr->error_value)
    {
    case CB_WMS_ERROR_MEMORY_FULL:
      err_type = WMS_GW_CB_ERROR_MEMORY_FULL;
      break;

    default:
      MSG_ERROR_1( "Invalid CB error code: %d", cb_error_ind_ptr->error_value);
      ok_to_notify = FALSE;
      break;
    }

    if (ok_to_notify)
    {
      wms_cfg_event_info_s_type         *cfg_event_ptr = NULL;

      cfg_event_ptr = (wms_cfg_event_info_s_type *)wms_mem_malloc(sizeof(wms_cfg_event_info_s_type));

      if (NULL != cfg_event_ptr)
      {
        cfg_event_ptr->cb_error_info = err_type;
        wms_cfg_event_notify( WMS_CFG_EVENT_CB_ERROR, cfg_event_ptr );
      }
      else
      {
        MSG_ERROR_0("WMS_CFG_EVENT_CB_ERROR not sent: memory allocation failed");
      }

      WMS_MEM_FREEIF(cfg_event_ptr);
    }
  }

  return;
} /* wms_cfg_gw_cb_error_ind_proc() */

#if (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA))
void wms_cfg_rrc_cell_change_cb_proc(
  wms_cmd_type  *cmd_ptr
)
{
  wms_cfg_s_type           *cfg_s_ptr     = wms_cfg_s_ptr();
  wms_rrc_cell_info_s_type *cell_info_ptr = &cmd_ptr->cmd.rrc_cb.cell_info;

  MSG_HIGH_0("wms_cfg_rrc_cell_change_cb_proc()");

  memset(cfg_s_ptr->umts_loc_info.mcc_and_mnc, 0x00, 3);

  cfg_s_ptr->umts_loc_info.cell_id[1] = (uint8)(cell_info_ptr->cell_id & 0x000000FF);
  cfg_s_ptr->umts_loc_info.cell_id[0] = (uint8)((cell_info_ptr->cell_id >> 8) & 0x000000FF);
  cfg_s_ptr->umts_loc_info.cell_id[3] = (uint8)((cell_info_ptr->cell_id >> 16) & 0x000000FF);
  cfg_s_ptr->umts_loc_info.cell_id[2] = (uint8)((cell_info_ptr->cell_id >> 24) & 0x000000FF);

  if(cell_info_ptr->plmn_id.num_mnc_digits == 2)
  {
     cell_info_ptr->plmn_id.mnc[2] = 0x0F;
  }

  MSG_HIGH_3("RRC Plmn MCC: 0x%x 0x%x 0x%x ",
              cell_info_ptr->plmn_id.mcc[0],
              cell_info_ptr->plmn_id.mcc[1],
              cell_info_ptr->plmn_id.mcc[2]);

  MSG_HIGH_3("RRC Plmn MNC 0x%x 0x%x 0x%x",
              cell_info_ptr->plmn_id.mnc[0],
              cell_info_ptr->plmn_id.mnc[1],
              cell_info_ptr->plmn_id.mnc[2]);

  MSG_HIGH("RRC LAC: 0x%x 0x%x, Cell ID: 0x%x",
           cell_info_ptr->lac[0],
           cell_info_ptr->lac[1],
           cell_info_ptr->cell_id );

  /* ----------------------------------------
  ** PLMN octet 1 = MCC digit 2 | MCC digit 1
  ** ---------------------------------------- */
  cfg_s_ptr->umts_loc_info.mcc_and_mnc[0] =  (cell_info_ptr->plmn_id.mcc[1] << 4) + cell_info_ptr->plmn_id.mcc[0];

  /* ----------------------------------------
  ** PLMN octet 2 = MNC digit 3 | MCC digit 3
  ** ---------------------------------------- */
  cfg_s_ptr->umts_loc_info.mcc_and_mnc[1] = (cell_info_ptr->plmn_id.mnc[2] << 4) + cell_info_ptr->plmn_id.mcc[2];

  /* ----------------------------------------
  ** PLMN octet 3 = MNC digit 2 | MNC digit 1
  ** ---------------------------------------- */
  cfg_s_ptr->umts_loc_info.mcc_and_mnc[2] = (cell_info_ptr->plmn_id.mnc[1] << 4) + cell_info_ptr->plmn_id.mnc[0];

  cfg_s_ptr->umts_loc_info.loc_area_code[0] = cell_info_ptr->lac[0];
  cfg_s_ptr->umts_loc_info.loc_area_code[1] = cell_info_ptr->lac[1];
}
#endif /* (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)) */

#endif /* FEATURE_GWSMS */

/* END of wmscfg.c */
