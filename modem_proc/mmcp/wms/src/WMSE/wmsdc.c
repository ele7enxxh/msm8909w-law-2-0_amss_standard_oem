/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wms.c -- Dedicated channel Group

  The WMS module which implements the User API for SMS. This source file
  implements the Dedicated Channel group functions which process the client
  commands of the Dedicated Channel group.
  -------------

    Copyright (c) 2001-2013 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsdc.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmsdc.h"
#include "wmsnv.h"
#include "wmsmsg.h"
#include "wmscl.h"
#include "wmsdbg.h"
#include "msg.h"
#include "err.h"
#include "string.h"
#include "wmscm.h"
#include "wmscfg.h"
#include "wmsutils.h"


/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/

#ifdef FEATURE_CDSMS
static wms_dc_s_type              *dc_s_ptr;
static wms_cm_s_type              *cm_s_ptr;
static wms_cfg_s_type             *cfg_s_ptr;
static wms_dc_event_info_s_type   dc_event_info;
static boolean                    is_dc_call_connected = FALSE;

#endif /* FEATURE_CDSMS */


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_dc_s_ptr

DESCRIPTION
  Return the pointer to the DC data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal DC data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_dc_s_type* wms_dc_s_ptr
(
  void
)
{
  static wms_dc_s_type  dc;

  return &dc;

} /* wms_dc_s_ptr() */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_dc_filter_events

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
boolean wms_dc_filter_events
(
  wms_client_id_type       client_id,
  wms_client_s_type        * client_ptr,
  wms_dc_event_e_type      event
)
{
  boolean   ok_to_send_event = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_3( "DC: client_id=%d, event=%d, notify type = %d",
              client_id,
              event,
              client_ptr->dc_notify_type );

  if( WMS_PM_NOTIFY_ALL == client_ptr->dc_notify_type )
  {
    ok_to_send_event = TRUE;
  }
  else if( (WMS_PM_NOTIFY_CUSTOM_FILTER == client_ptr->dc_notify_type) &&
           ( client_ptr->dc_event_mask & ((uint32)1 << (uint32)event) ) )
  {
    ok_to_send_event = TRUE;
  }
  else if( WMS_PM_NOTIFY_DEFAULT_FILTER == client_ptr->dc_notify_type )
  {
    switch( event )
    {
      case WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT:
      case WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT:
        MSG_HIGH_0("PM: This event matches internal default filter");
        ok_to_send_event = TRUE;
        break;

      default:
        break;
    }
  }
  else if ( WMS_PM_NOTIFY_NONE == client_ptr->cfg_notify_type )
  {
     //Modem / Apps Client without PM
     ok_to_send_event = TRUE;
  }

  if( FALSE == ok_to_send_event )
  {
    MSG_ERROR_2( "Blocking dc event %d for client %d",
                 event,
                 client_id );
  }

  return ok_to_send_event;

} /* wms_dc_filter_events() */

/*===========================================================================
FUNCTION wms_dc_event_notify

DESCRIPTION
  Notify all clients of a DC event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_event_notify
(
  wms_dc_event_e_type        event,
  wms_dc_event_info_s_type   *dc_event_info_ptr
)
{
  wms_client_s_type           *client_ptr;
  uint8                       i;
  wms_dc_event_info_s_type    local_dc_event_info = { WMS_DC_SO_AUTO, 0 };

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s", event,
                  wms_dbg_get_dc_event_string((uint16)event));

  /* Notify clients
  */
  for( i=0; i< (uint8) WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr_fetch( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->dc_event_cb )
    {
      if( TRUE == wms_dc_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        if (NULL != dc_event_info_ptr)
        {
          // Update the outbound data for each client to avoid data
          // tampering or corruption for downstream clients
          local_dc_event_info = *dc_event_info_ptr;
          client_ptr->dc_event_cb( event, &local_dc_event_info );
        }
      }
    }
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }

  /* done */
  return;

} /* wms_dc_event_notify() */


/*===========================================================================
FUNCTION wms_dc_disconnect_cb

DESCRIPTION
  Notify clients of a DC Disconnect callback result.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_disconnect_cb
(
   wms_dc_so_e_type                 so,
   uint32                           idle_timeout,
   wms_dc_disconnect_cause_e_type   cause
)
{
  wms_client_s_type                *client_ptr;
  uint8                            i;
  wms_dc_disconnect_info_s_type    disconnect_info;

  MSG_HIGH_1("wms_dc_disconnect_cb, cause %d", cause);

  /* Notify clients
  */
  for( i=0; i< (uint8) WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr_fetch( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->dc_disconnect_cb )
    {
      // Update the outbound data for each client to avoid data
      // tampering or corruption for downstream clients
      disconnect_info.so = so;
      disconnect_info.idle_timeout = idle_timeout;
      disconnect_info.disconnect_cause =  cause;
      client_ptr->dc_disconnect_cb( &disconnect_info );
    }
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }
} /* wms_dc_disconnect_cb() */

/*===========================================================================
FUNCTION wms_dc_connect_proc

DESCRIPTION
  Process the request of connecting DC.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_connect_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  wms_dc_so_e_type            dc_connect_so = WMS_DC_SO_6;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("wms_dc_connect_proc()");

  if( TRUE == cfg_s_ptr->lpm )
  {
    cmd_err = WMS_CMD_ERR_DC_LPM;
  }
  /*
  else if(FALSE == cfg_s_ptr->srv_available)
  {
    MSG_HIGH_0("Service Not Available");
    cmd_err = WMS_CMD_ERR_NO_NETWORK;
  }
  */
  else if( CM_CALL_STATE_IDLE != cm_s_ptr->call_info.call_state )
  {
    cmd_err = WMS_CMD_ERR_DC_BAD_STATE;
  }
  else
  {
    dc_s_ptr->auto_disc_active = FALSE;

    switch (cmd_ptr->cmd.dc_connect.so)
    {
      case WMS_DC_SO_AUTO:
        /* Change it to Default Value from NV */
        dc_connect_so = dc_s_ptr->default_so_from_nv;
        break;

      case WMS_DC_SO_6:
      case WMS_DC_SO_14:
        /* Pass through to CM */
        dc_connect_so = cmd_ptr->cmd.dc_connect.so;
        break;

      default:
        MSG_ERROR_1("Bad Service Option %d for wms_dc_connect", cmd_ptr->cmd.dc_connect.so);
        cmd_err = WMS_CMD_ERR_DC_BAD_PARM;
        break;
    }
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    wms_cm_call_orig( dc_connect_so );

    /* set timer for call origination
    */
    MSG_HIGH_1("Set DC call_out_timer: 0x%lx ms", WMS_TIMER_DC_CALL_OUT);
    (void) rex_set_timer( & dc_s_ptr->call_out_timer, WMS_TIMER_DC_CALL_OUT );

    /* Send event
    */
    dc_event_info.so = dc_connect_so;
    wms_dc_event_notify( WMS_DC_EVENT_CONNECTING, &dc_event_info );
  }
  else
  {
    /* If It is WMS initiated call and it fails to call CM API.
     * Send SUBMIT REPORT for the msg.
    */
    if(TRUE == dc_s_ptr->wms_initiated_call)
    {
      /* check again if any call is already connected */
      if ( (dc_s_ptr->call_active) || (dc_s_ptr->is_voice_data_call_conn) )
      {
        wms_msg_auto_dc_send();
      }
      else
      {
        wms_msg_stop_dc_resend();
        wms_msg_auto_send_ach();
      }
      dc_s_ptr->wms_initiated_call = FALSE;
    }
  }

  /* done */
  return;

} /* wms_dc_connect_proc() */


/*===========================================================================
FUNCTION wms_dc_clear_timers

DESCRIPTION
  Clear the Dedicated Channel Timers

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_clear_timers
(
  void
)
{
  (void) rex_clr_timer( & dc_s_ptr->call_out_timer );
  (void) rex_clr_timer( & dc_s_ptr->idle_timer );
  dc_s_ptr->auto_disc_active = FALSE;

} /* wms_dc_clear_timers */


/*===========================================================================
FUNCTION wms_dc_disconnect_proc

DESCRIPTION
  Process the request of disconnecting DC.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_disconnect_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("wms_dc_disconnect_proc()");

  if(WMS_DUMMY_CALL_ID == dc_s_ptr->dc_call_id)
  {
     MSG_HIGH_0("No call is initiated, do not call wms_cm_call_end");
     return;
  }

  // If the disconnect request is from WMS,
  // if external client request, check to make sure there is no more pending MO or MT
  // messages before disconnecting the dc.
  if (dc_s_ptr->wms_initiated_call &&
     (WMS_CLIENT_TYPE_INTERNAL != cmd_ptr->hdr.client_id) &&
     (wms_msg_cdma_mo_busy(NULL) || wms_msg_cdma_mt_busy())) {
     MSG_HIGH_0("dc is not disconnected because there are pending records.");
     wms_client_cmd_status(cmd_ptr,  WMS_CMD_ERR_BUSY);
     return;
  }

  /* Clear all DC timers
  */
  wms_dc_clear_timers();

  /* End the call
  */
  wms_cm_call_end();


  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* Send event
  */
  dc_s_ptr->call_active = FALSE;
  dc_s_ptr->wms_initiated_call = FALSE;
  dc_s_ptr->dc_call_id = WMS_DUMMY_CALL_ID;
  wms_dc_event_notify( WMS_DC_EVENT_DISCONNECTED, &dc_event_info );
  wms_dc_disconnect_cb(dc_event_info.so, dc_event_info.idle_timeout, WMS_DC_DISCONNECT_CAUSE_NORMAL);

  /* done */
  return;

} /* wms_dc_disconnect_proc() */


/*===========================================================================
FUNCTION wms_dc_enable_auto_disc_proc

DESCRIPTION
  Process the request of setting auto-disconnect timeout value.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Auto-disconnect timeout value is updated.
===========================================================================*/
void wms_dc_enable_auto_disc_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("wms_dc_enable_auto_disc_proc()");

  dc_s_ptr->idle_timeout = cmd_ptr->cmd.dc_enable_auto_disc.timeout * 1000;


  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* Send event
  */

  dc_event_info.idle_timeout = cmd_ptr->cmd.dc_enable_auto_disc.timeout;
  wms_dc_event_notify( WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT, &dc_event_info );

  /* done */
  return;

} /* wms_dc_enable_auto_disc_proc() */



/*===========================================================================
FUNCTION wms_dc_disable_auto_disc_proc

DESCRIPTION
  Process the request of disabling auto-disconnect.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Auto-disconnect timeout is reset.
===========================================================================*/
void wms_dc_disable_auto_disc_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("wms_dc_disable_auto_disc_proc()");

  dc_s_ptr->idle_timeout = 0;

  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* Send event
  */
  dc_event_info.idle_timeout = 0;
  wms_dc_event_notify( WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT, &dc_event_info );

  /* done */
  return;

} /* wms_dc_disable_auto_disc_proc() */

void wms_dc_cache_call_info(
  cm_mm_call_info_s_type *call_info
)
{
  if (NULL != call_info && NULL != cm_s_ptr)
  {
    cm_s_ptr->call_info.asubs_id             = call_info->asubs_id;
    cm_s_ptr->call_info.call_state           = call_info->call_state;
    cm_s_ptr->call_info.call_type            = call_info->call_type;
    cm_s_ptr->call_info.call_id              = call_info->call_id;
    cm_s_ptr->call_info.mode_info_cdma_call  = call_info->mode_info.info.cdma_call;
  }
  else
  {
    MSG_ERROR_2("NULL_PTR call_info: 0x%x, cm_s_ptr: 0x%x", call_info, cm_s_ptr);
  }
}

/*===========================================================================
FUNCTION wms_dc_incoming_call_proc

DESCRIPTION
  Process the incoming call event from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_incoming_call_proc
(
  wms_cmd_type      *cmd_ptr
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("wms_dc_incoming_call_proc()");

  wms_dc_cache_call_info( &(cmd_ptr->cmd.call_info) );


  /* Send event
  */
#ifdef FEATURE_IS95B_ALERT_ANS_BYPASS
    /* If the bypass flag is set, CM has already answered the call, therefore
    ** don't tell CM to answer the call again.
    */
    if( !cm_s_ptr->call_info.mode_info_cdma_call.alert_ans_bypass )
#endif /* FEATURE_IS95B_ALERT_ANS_BYPASS */
    {
      /* auto answer the SMS call
      */
      MSG_HIGH_0("Answering incoming call...");

      wms_cm_call_answer();
    }

    dc_event_info.so  = (wms_dc_so_e_type) cm_s_ptr->call_info.mode_info_cdma_call.srv_opt;

    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;

    wms_dc_event_notify( WMS_DC_EVENT_INCOMING, &dc_event_info );

    /* set timer for call origination
    */
    MSG_HIGH_1("Set DC call_out_timer: 0x%lx ms", WMS_TIMER_DC_CALL_OUT);
    (void) rex_set_timer( & dc_s_ptr->call_out_timer, WMS_TIMER_DC_CALL_OUT );


  /* done */
  return;

} /* wms_dc_incoming_call_proc() */



/*===========================================================================
FUNCTION wms_dc_connected_call_proc

DESCRIPTION
  Process the connected call event from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_connected_call_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  MSG_HIGH_0("wms_dc_connected_call_proc()");

  is_dc_call_connected = TRUE;
  /* Trigger retry of large messages over DC */
  wms_msg_auto_dc_send();

  /* case: Voice/Data call is connected, Client also initiated MO SMS call
           WMS silently disconnect this call and sends WMS_DC_EVENT_DISCONNECTED
           with WMS_DC_ERR_CALL_IN_PROGRESS error code
  */

  if( (WMS_DUMMY_CALL_ID != dc_s_ptr->dc_call_id) &&
      (TRUE == wms_cm_check_call_type(cmd_ptr->cmd.call_info.call_type,
                                      cmd_ptr->cmd.call_info.sys_mode) ) )
  {
     MSG_HIGH_1("Disconnect requested SMS call call id %d", dc_s_ptr->dc_call_id);

     wms_cm_call_end();

     dc_s_ptr->call_active = FALSE;
     dc_s_ptr->wms_initiated_call = FALSE;
     dc_s_ptr->dc_call_id = WMS_DUMMY_CALL_ID;

     wms_dc_event_notify( WMS_DC_EVENT_DISCONNECTED, &dc_event_info );
     wms_dc_disconnect_cb(dc_event_info.so, dc_event_info.idle_timeout, WMS_DC_DISCONNECT_CAUSE_CALL_IN_PROGRESS);
     wms_dc_clear_timers();

     return;
  }

  /* Send event
  */
  if( ( cmd_ptr->cmd.call_info.call_id == cm_s_ptr->call_info.call_id ) ||
      ( cmd_ptr->cmd.call_info.call_id == dc_s_ptr->dc_call_id ) )
  {
    wms_dc_cache_call_info( &(cmd_ptr->cmd.call_info) );

    /* notify the client of the event
    */
    dc_event_info.so = (wms_dc_so_e_type) cmd_ptr->cmd.call_info.mode_info.info.cdma_call.srv_opt;
    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;

    dc_s_ptr->call_active = TRUE;
    wms_dc_event_notify( WMS_DC_EVENT_CONNECTED, &dc_event_info );

    wms_dc_clear_timers();

    if( dc_s_ptr->idle_timeout != 0)
    {
      MSG_HIGH_1("Set DC_idle_timer: 0x%lx ms", dc_s_ptr->idle_timeout);
      (void) rex_set_timer( & dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
      dc_s_ptr->auto_disc_active = TRUE;
    }

  }

  /* done */
  return;

} /* wms_dc_connected_call_proc() */

/*===========================================================================
FUNCTION wms_dc_call_end_proc

DESCRIPTION
  Process the ended call event from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_call_end_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("wms_dc_call_end_proc()");

  // If the DC call is ended before the call is connected,
  // stop retry sending large MO messages over DC.
  if (!is_dc_call_connected) {
    MSG_HIGH_0("wms_msg_stop_dc_resend()");
    wms_msg_stop_dc_resend();
  }

  is_dc_call_connected = FALSE;


  MSG_HIGH_1("dc_s_ptr->dc_call_id =%d", dc_s_ptr->dc_call_id);
  MSG_HIGH_1("cmd_ptr->cmd.call_info.call_id =%d", cmd_ptr->cmd.call_info.call_id);
  MSG_HIGH_1("cm_s_ptr->call_info.call_id =%d", cm_s_ptr->call_info.call_id);

  if( (cmd_ptr->cmd.call_info.call_id != cm_s_ptr->call_info.call_id) &&
      (dc_s_ptr->dc_call_id != cmd_ptr->cmd.call_info.call_id) )
  {
    MSG_ERROR_0("call_id mismatch in Call Connect!");
    cmd_err = WMS_CMD_ERR_DC_BAD_CALL_ID;
  }

  /* Send event
  */
  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    wms_dc_cache_call_info( &(cmd_ptr->cmd.call_info) );

    /* notify the client of the event
    */
    dc_event_info.so = (wms_dc_so_e_type) cmd_ptr->cmd.call_info.mode_info.info.cdma_call.srv_opt;
    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;

    dc_s_ptr->call_active = FALSE;
    dc_s_ptr->wms_initiated_call = FALSE;

    /* SMS call originated using WMS, ended by network */
    if(dc_s_ptr->dc_call_id == cmd_ptr->cmd.call_info.call_id)
    {
       MSG_HIGH_1("SMS call originated, ended by network %d ", dc_s_ptr->dc_call_id);
       dc_s_ptr->dc_call_id = WMS_DUMMY_CALL_ID;
    }

    /* case  : WMS opens traffic channel, Send SMS, Disconnect SMS
               WMS is sending WMS_DC_EVENT_DISCONNECTED twice.

               WMS does not know if disconnected is user initiated
               or network initiated
    */
    wms_dc_event_notify( WMS_DC_EVENT_DISCONNECTED, &dc_event_info );

    /* case : WMS has not received CM connect event but CM has rejected SMS request becz
              call has connected already in CM. Send wms_dc_disconnect_cb with cause
              WMS_DC_DISCONNECT_CAUSE_CALL_IN_PROGRESS
    */
    if (CM_CALL_END_ALREADY_IN_TC == cmd_ptr->cmd.call_info.end_status)
    {
       MSG_HIGH_1("SMS call rejected by CM, end status %d ", cmd_ptr->cmd.call_info.end_status);

       wms_dc_disconnect_cb(dc_event_info.so, dc_event_info.idle_timeout, WMS_DC_DISCONNECT_CAUSE_CALL_IN_PROGRESS);
       dc_s_ptr->dc_call_id = WMS_DUMMY_CALL_ID;
    }
    else
    {
       wms_dc_disconnect_cb(dc_event_info.so, dc_event_info.idle_timeout, WMS_DC_DISCONNECT_CAUSE_NORMAL);
    }

    wms_dc_clear_timers();

  }

  /* Automatically send ACH messages  */
  wms_msg_auto_send_ach();

  /* done */
  return;

} /* wms_dc_call_end_proc() */


/*===========================================================================
FUNCTION wms_dc_call_error_proc

DESCRIPTION
  Process the call cmd error event from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_call_error_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  //wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("wms_dc_call_end_proc()");

  if( cmd_ptr->cmd.call_error.call_cmd == CM_CALL_CMD_ANSWER )
  {
    /* need to abort the call request */
    MSG_HIGH_0("Ending the call due to error()...");
    wms_cm_call_end();

    /* notify the client of the event
    */
    dc_event_info.so = (wms_dc_so_e_type) cmd_ptr->cmd.call_info.mode_info.info.cdma_call.srv_opt;

    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;

    wms_dc_event_notify( WMS_DC_EVENT_ABORTED, &dc_event_info );

    wms_dc_clear_timers();
  }
  else if( cmd_ptr->cmd.call_error.call_cmd == CM_CALL_CMD_ORIG )
  {
    /* Notify clients that orig failed */
    MSG_HIGH_0("SMS call orig failed!");

    dc_s_ptr->call_active = FALSE;
    dc_s_ptr->wms_initiated_call = FALSE;
    dc_s_ptr->dc_call_id = WMS_DUMMY_CALL_ID;
    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;
    wms_dc_event_notify( WMS_DC_EVENT_ABORTED, &dc_event_info );
    wms_dc_clear_timers();
    wms_msg_stop_dc_resend();
    wms_msg_auto_send_ach();
  }

  /* done */
  return;

} /* wms_dc_call_error_proc() */


/*===========================================================================
FUNCTION wms_dc_call_out_timer_proc

DESCRIPTION
  Process the DC Call out timer

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_call_out_timer_proc
(
  void
)
{
  MSG_HIGH_0( "got WMS DC_CALL_OUT_TIMER_SIG");

  (void) rex_clr_sigs( rex_self(), WMS_DC_CALL_OUT_TIMER_SIG );

  /* the base station didn't answer the call in time; abort the call
  */
  wms_cm_call_end();

  /* notify the client of the error
  */
  MSG_HIGH_0("Sending EVENT_DC_ABORTED to client");
  wms_dc_event_notify( WMS_DC_EVENT_ABORTED, &dc_event_info );

} /* wms_dc_call_out_timer_proc() */

/*===========================================================================
FUNCTION wms_dc_idle_timer_proc

DESCRIPTION
  Process the DC Idle timer

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_idle_timer_proc
(
  void
)
{
  MSG_HIGH_0( "got UASMS_DC_IDLE_TIMER_SIG");

  (void) rex_clr_sigs( rex_self(), WMS_DC_IDLE_TIMER_SIG );

  if( wms_msg_cdma_mt_busy() == TRUE || wms_msg_cdma_mo_busy(NULL) == TRUE )
  {
    MSG_HIGH_0("MT or MO records are in use; restart DC IDLE timer");
    (void) rex_set_timer( & dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
  }
  else
  {
    wms_dc_clear_timers();

    /* end the SMS call after the idle period
    */
    wms_cm_call_end();

    /* Event will be sent when we receive call end event from CM */
  }

  return;

} /* wms_dc_idle_timer_proc() */


#endif /* FEATURE_CDSMS */



/*===========================================================================
FUNCTION wms_dc_process_cmd

DESCRIPTION
  Dispatch the DC command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_process_cmd
(
  wms_cmd_type  *cmd_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( cmd_ptr->hdr.cmd_id )
  {

#ifdef FEATURE_CDSMS

    case WMS_CMD_DC_CONNECT:
      wms_dc_connect_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_DISCONNECT:
      wms_dc_disconnect_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
      wms_dc_enable_auto_disc_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
      wms_dc_disable_auto_disc_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_CALL_INCOM_E:
      wms_dc_incoming_call_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_CALL_CONNECT_E:
      wms_dc_connected_call_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_CALL_END_E:
      wms_dc_call_end_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_CALL_ERROR_E:  /* in case of CM cmd errors */
      wms_dc_call_error_proc( cmd_ptr );
      break;

#endif /* FEATURE_CDSMS */

    default:
      MSG_ERROR_1("Invalid cmd: %d", cmd_ptr->hdr.cmd_id);
      break;
  }

  /* done */
  return;

} /* wms_dc_process_cmd() */


/*===========================================================================
FUNCTION wms_dc_init

DESCRIPTION
  Do DC-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  DC data initialized.
===========================================================================*/
void wms_dc_init
(
  void
)
{
  MSG_HIGH_0("wms_dc_init()");

#ifdef FEATURE_CDSMS
  dc_s_ptr = wms_dc_s_ptr();
  cm_s_ptr = wms_cm_s_ptr();
  cfg_s_ptr = wms_cfg_s_ptr();

  dc_s_ptr->auto_disc_active      = FALSE;
  dc_s_ptr->idle_timeout          = 0;
  dc_s_ptr->wms_initiated_call    = FALSE;
  dc_s_ptr->call_active           = FALSE;
  dc_s_ptr->is_voice_data_call_conn = FALSE;
  dc_s_ptr->dc_call_id = WMS_DUMMY_CALL_ID;

  /* ----------------------- define timers -------------------------- */

  rex_def_timer(
                 & dc_s_ptr->call_out_timer,
                 rex_self(),
                 WMS_DC_CALL_OUT_TIMER_SIG
               );

  rex_def_timer(
                 & dc_s_ptr->idle_timer,
                 rex_self(),
                 WMS_DC_IDLE_TIMER_SIG
               );

  /* Read CDMA Channel setting from NV or RUIM card */
  wms_nv_read_cdma_channel_setting();

  /* Read SERVICE OPTION setting from NV or RUIM card */
  wms_nv_read_cdma_service_option();

#endif /* FEATURE_CDSMS */

  /* done */
  return;
} /* wms_dc_init */


#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wms_dc.c */
