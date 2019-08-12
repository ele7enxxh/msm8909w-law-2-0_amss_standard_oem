/*====*====*====*====*====*====*====*====*====*====*==*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsdbg.c -- Debug Group

  The WMS module which implements the User API for SMS. This source file
  implements the Debug group functions which process the client
  commands of the Debug group.
  -------------

    Copyright (c) 2002-2014 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsdbg.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"

#if (defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS))

#include "wms.h"
#include "wmsi.h"
#include "wmscl.h"
#include "wmsnv.h"
#include "wmsdbg.h"
#include "wmsmsgi.h"
#include "wmscfg.h"

#include "msg.h"
#include "err.h"
#include "wmssim.h"
#include "stringl.h"
#ifdef FEATURE_FUZZ_TEST
#include "mn_sms.h"
#include "wmstsi.h"
#endif
#include "wmsutils.h"


/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/

/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_dbg_s_ptr()                                           (INTERNAL)

DESCRIPTION
  Return the pointer to the Debug data.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the internal data.

SIDE EFFECTS
  None
===========================================================================*/
wms_dbg_s_type * wms_dbg_s_ptr(
  void
)
{
  static wms_dbg_s_type  dbg;

  return &dbg;
} /* wms_dbg_s_ptr() */

/*===========================================================================
FUNCTION wms_dbg_filter_events()                                   (INTERNAL)

DESCRIPTION
  Decide whether this event can be sent to the client or not based on
  power management settings.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  OK to send event to client
  FALSE: Not OK to send event to client

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_dbg_filter_events(
  wms_client_id_type        client_id,
  wms_client_s_type        *client_ptr,
  wms_dbg_event_e_type      event
)
{
  boolean   ok_to_send_event = FALSE;

  MSG_HIGH_3( "DBG: client_id=%d, event=%d, notify type = %d",
              client_id,
              event,
              client_ptr->dbg_notify_type );

  if ( WMS_PM_NOTIFY_ALL == client_ptr->dbg_notify_type )
  {
    ok_to_send_event = TRUE;
  }
  else if ( (WMS_PM_NOTIFY_CUSTOM_FILTER == client_ptr->dbg_notify_type) &&
           ( client_ptr->dbg_event_mask & ((uint32)1 << (uint32)event) ) )
  {
    ok_to_send_event = TRUE;
  }
  else if ( WMS_PM_NOTIFY_DEFAULT_FILTER == client_ptr->dbg_notify_type )
  {
    // No DBG event satisfies the default filter.
  }
  else if ( WMS_PM_NOTIFY_NONE == client_ptr->cfg_notify_type )
  {
     // Modem / Apps Client without PM
     ok_to_send_event = TRUE;
  }

  if ( FALSE == ok_to_send_event )
  {
    MSG_ERROR_2( "Blocking dbg event %d for client %d",
                 event,
                 client_id );
  }

  return ok_to_send_event;

} /* wms_dbg_filter_events() */

/*===========================================================================
FUNCTION wms_dbg_event_notify()                                    (INTERNAL)

DESCRIPTION
  Notify all clients of a Debug event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_dbg_event_notify(
  wms_dbg_event_e_type         event,
  wms_dbg_event_info_s_type   *dbg_event_info_ptr
)
{
  wms_client_s_type           *client_ptr;
  uint8                        i;
  wms_dbg_event_info_s_type    outbound_info;

  MSG_HIGH_1("dbg_event_notify: %d", event);


  /* Notify clients
  */
  for ( i=0; i<(uint8)WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr_fetch( (wms_client_id_type) i );
    if ( client_ptr != NULL &&
         client_ptr->activated == TRUE &&
         client_ptr->dbg_event_cb )
    {
      if ( TRUE == wms_dbg_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        // Update the outbound data for each client to avoid data
        // tampering or corruption for downstream clients
        outbound_info = *dbg_event_info_ptr;
        client_ptr->dbg_event_cb( event, &outbound_info );
      }
    }
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }

  return;
} /* wms_dbg_event_notify() */

#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION wms_dbg_reset_tl_seq_num_proc()                           (INTERNAL)

DESCRIPTION
  Process the request of resetting CDMA TL sequence number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NV is updated.
===========================================================================*/
void wms_dbg_reset_tl_seq_num_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_msg_s_type      *msg_s_ptr = wms_msg_s_ptr();
  wms_cmd_err_e_type   cmd_err   = WMS_CMD_ERR_NONE;

  MSG_HIGH_0("wms_dbg_reset_tl_seq_num_proc()");

  msg_s_ptr->tl_seq_num = 0;

  wms_client_cmd_status( cmd_ptr, cmd_err );

  {
    wms_dbg_event_info_s_type  dbg_event_info;

    memset(&dbg_event_info, 0, sizeof(dbg_event_info));

    wms_dbg_event_notify( WMS_DBG_EVENT_RESET_TL_SEQ_NUM, &dbg_event_info );
  }

  return;
} /* wms_dbg_reset_tl_seq_num_proc() */

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

/*===========================================================================
FUNCTION wms_dbg_set_sim_message_number()                          (INTERNAL)

DESCRIPTION
  Sets the msg ref# to 0

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The current message number is set to 0.
===========================================================================*/
void wms_dbg_set_sim_message_number(
  wms_cmd_type      *cmd_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr;
  wms_cmd_err_e_type        cmd_err      = WMS_CMD_ERR_NONE;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    cmd_err = WMS_CMD_ERR_NULL_PTR;
  }
  else
  {
    uint8                     data[2];
    mmgsdi_return_enum_type   mmret        = MMGSDI_SUCCESS;

    appInfo_ptr->smss_last_message_number = 0;

    data[0]   = (uint8) appInfo_ptr->smss_last_message_number ;
    data[1]   = appInfo_ptr->smss_mem_full_flag;

    mmret = wms_sim_mmgsdi_write_data( appInfo_ptr,
                                       appInfo_ptr->smss_file_enum,
                                       0, //not used
                                       data,
                                       sizeof(data) );
    if (MMGSDI_SUCCESS != mmret)
    {
      cmd_err = WMS_CMD_ERR_MSG_SIM_WRITE;
    }
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  // Send event
  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    wms_dbg_event_info_s_type  dbg_event_info;

    memset(&dbg_event_info, 0, sizeof(dbg_event_info));

    wms_dbg_event_notify( WMS_DBG_EVENT_SET_SIM_MESSAGE_NUMBER, &dbg_event_info );
  }

  return;
} /* wms_dbg_set_sim_message_number() */

/*===========================================================================
FUNCTION wms_dbg_clear_smma_flag_proc()                            (INTERNAL)

DESCRIPTION
  Resets the SMMA Flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The SMMA flag is reset
===========================================================================*/
void wms_dbg_clear_smma_flag_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr;
  wms_cmd_err_e_type        cmd_err      = WMS_CMD_ERR_NONE;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    cmd_err = WMS_CMD_ERR_NULL_PTR;
  }
  else
  {
    uint8                     data[2];
    mmgsdi_return_enum_type   mmret        = MMGSDI_SUCCESS;

    appInfo_ptr->smss_mem_full_flag |= (uint8)WMS_SIM_MEM_FULL_CLEAR;

    data[0]   = (uint8) appInfo_ptr->smss_last_message_number;
    data[1]   = appInfo_ptr->smss_mem_full_flag;

    mmret = wms_sim_mmgsdi_write_data( appInfo_ptr,
                                       appInfo_ptr->smss_file_enum,
                                       0, //not used
                                       data,
                                       sizeof(data) );
    if (MMGSDI_SUCCESS != mmret)
    {
      cmd_err = WMS_CMD_ERR_MSG_SIM_WRITE;
    }
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  // Send event
  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    wms_dbg_event_info_s_type  dbg_event_info;

    memset(&dbg_event_info, 0, sizeof(dbg_event_info));

    wms_dbg_event_notify( WMS_DBG_EVENT_CLEAR_SMMA_FLAG, &dbg_event_info );
  }

  return;
} /* wms_dbg_clear_smma_flag() */

#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_dbg_get_retry_interval_proc()                         (INTERNAL)

DESCRIPTION
  Get the Retry Interval

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_dbg_get_retry_interval_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err   = WMS_CMD_ERR_NONE;

  wms_client_cmd_status( cmd_ptr, cmd_err );

  {
    wms_msg_s_type            *msg_s_ptr      = wms_msg_s_ptr();
    wms_dbg_event_info_s_type  dbg_event_info;

    memset(&dbg_event_info, 0, sizeof(dbg_event_info));

    // Fill up Event Information
    dbg_event_info.retry_interval = msg_s_ptr->mo_retry_interval; /* In seconds */

    // Send event
    wms_dbg_event_notify( WMS_DBG_EVENT_RETRY_INTERVAL, &dbg_event_info );
  }

  return;
} /* wms_dbg_get_retry_interval_proc() */

/*===========================================================================
FUNCTION wms_dbg_set_retry_interval_proc()                         (INTERNAL)

DESCRIPTION
  Sets the Retry Interval

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_dbg_set_retry_interval_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_msg_s_type      *msg_s_ptr      = wms_msg_s_ptr();
  wms_cmd_err_e_type   cmd_err        = WMS_CMD_ERR_NONE;
  uint32               retry_interval = cmd_ptr->cmd.dbg_set_retry_interval.retry_interval;

  if (retry_interval > msg_s_ptr->mo_retry_period)
  {
    cmd_err = WMS_CMD_ERR_DBG_BAD_PARAM;
  }
  else if (msg_s_ptr->mo_retry_interval != retry_interval)
  {
    nv_item_type          nvi;
    nvi.sms_mo_retry_interval = (byte)retry_interval;

    if (NV_DONE_S != wms_nv_write_wait(NV_SMS_MO_RETRY_INTERVAL_I, &nvi))
    {
      cmd_err = WMS_CMD_ERR_CFG_NV_WRITE;
    }
    else
    {
      MSG_HIGH_1("Setting Retry Interval = %d seconds is successful.", retry_interval);
      msg_s_ptr->mo_retry_interval = retry_interval;
    }
  }
  else
  {
    MSG_HIGH_0("No change to NV_SMS_MO_RETRY_INTERVAL_I");
  }

  wms_client_cmd_status(cmd_ptr, cmd_err);

  /* Send event notification to clients
  */
  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    wms_dbg_event_info_s_type  dbg_event_info;

    memset(&dbg_event_info, 0, sizeof(dbg_event_info));

    /* Fill up Event Information */
    dbg_event_info.retry_interval = msg_s_ptr->mo_retry_interval;

    // Send event
    wms_dbg_event_notify(WMS_DBG_EVENT_RETRY_INTERVAL, &dbg_event_info);
  }
  else
  {
    MSG_ERROR_1("wms_dbg_set_retry_interval_proc, error=%d", cmd_err);
  }

  return;
} /* wms_dbg_set_retry_interval_proc() */

/*===========================================================================
FUNCTION wms_dbg_get_retry_period_proc()                           (INTERNAL)

DESCRIPTION
  Get the Retry Interval

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_dbg_get_retry_period_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err   = WMS_CMD_ERR_NONE;

  wms_client_cmd_status( cmd_ptr, cmd_err );

  {
    wms_msg_s_type            *msg_s_ptr = wms_msg_s_ptr();
    wms_dbg_event_info_s_type  dbg_event_info;

    memset(&dbg_event_info, 0, sizeof(dbg_event_info));

    // Fill up Event Information
    dbg_event_info.retry_period = msg_s_ptr->mo_retry_period; /* In seconds */

    // Send event
    wms_dbg_event_notify( WMS_DBG_EVENT_RETRY_PERIOD, &dbg_event_info );
  }

  return;
} /* wms_dbg_get_retry_period_proc() */

/*===========================================================================
FUNCTION wms_dbg_set_retry_period_proc()                           (INTERNAL)

DESCRIPTION
  Sets the Retry Interval

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_dbg_set_retry_period_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_msg_s_type      *msg_s_ptr    = wms_msg_s_ptr();
  wms_cmd_err_e_type   cmd_err      = WMS_CMD_ERR_NONE;
  uint32               retry_period = cmd_ptr->cmd.dbg_set_retry_period.retry_period;

  if (WMS_MAX_RETRY_PERIOD < retry_period)
  {
    cmd_err = WMS_CMD_ERR_DBG_BAD_PARAM;
  }
  else if (msg_s_ptr->mo_retry_period != retry_period)
  {
    nv_item_type          nvi;
    nvi.sms_mo_retry_period = (byte)retry_period;

    if (NV_DONE_S != wms_nv_write_wait(NV_SMS_MO_RETRY_PERIOD_I, &nvi))
    {
      cmd_err = WMS_CMD_ERR_CFG_NV_WRITE;
    }
    else
    {
      MSG_HIGH_1("Setting Retry Period = %d seconds is successful.", retry_period);
      msg_s_ptr->mo_retry_period = retry_period;
    }
  }
  else
  {
    MSG_HIGH_0("No change to NV_SMS_MO_RETRY_PERIOD_I");
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    wms_dbg_event_info_s_type  dbg_event_info;

    memset(&dbg_event_info, 0, sizeof(dbg_event_info));

    // Fill up Event Information
    dbg_event_info.retry_period = msg_s_ptr->mo_retry_period;

    // Send event
    wms_dbg_event_notify(WMS_DBG_EVENT_RETRY_PERIOD, &dbg_event_info);
  }
  else
  {
    MSG_ERROR_1("wms_dbg_set_retry_period_proc, error=%d", cmd_err);
  }

  return;
} /* wms_dbg_set_retry_period_proc() */

#ifdef FEATURE_FUZZ_TEST
/*===========================================================================
FUNCTION wms_dbg_send_transport_rpt_ind_proc()                     (INTERNAL)

DESCRIPTION
  Process send transport report indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_dbg_send_transport_rpt_ind_proc(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type                 cmd_err     = WMS_CMD_ERR_BAD_PARM;
  wms_msg_transport_rpt_ind_s_type  *rpt_ind_ptr = &(cmd_ptr->cmd.transport_rpt_ind);
  sys_modem_as_id_e_type             as_id;

  MSG_ERROR_0("in wms_dbg_send_transport_rpt_ind_proc()");

  if (WMS_MSG_TRANSPORT_MT_SMS_IND == rpt_ind_ptr->ind_type)
  {
#ifdef FEATURE_CDSMS
     switch (rpt_ind_ptr->mt_sms.format)
     {

     case WMS_FORMAT_CDMA:
       {
         wms_routing_s_type      routing = wms_cfg_s_ptr()->routes.pp_routes[WMS_MESSAGE_CLASS_CDMA];
         wms_cmd_mc_mt_msg_type  mt_msg;

         mt_msg.ack_pending = TRUE;
         mt_msg.bearer = WMS_BEARER_MAX;
         mt_msg.ota.format = rpt_ind_ptr->mt_sms.format;
         mt_msg.ota.data_len = MIN(rpt_ind_ptr->mt_sms.data_len, WMS_MAX_LEN);
         (void)memscpy( mt_msg.ota.data,
                        mt_msg.ota.data_len,
                        rpt_ind_ptr->mt_sms.data,
                        mt_msg.ota.data_len);
         as_id = wms_get_as_id_from_message_mode(WMS_MESSAGE_MODE_CDMA); // FIX: MESSAGE MODE MAY NOT BE SUFFICIENT
         wms_msg_mt_CDMA_proc(as_id, &mt_msg, &routing);
         cmd_err = WMS_CMD_ERR_NONE;
       }
       break;
#endif

#ifdef FEATURE_GWSMS
     case WMS_FORMAT_GW_PP:
       if ((rpt_ind_ptr->mt_sms.data_len > 5) && (rpt_ind_ptr->mt_sms.data[0] == RP_DATA_N)) //RP-DATA
       {
         int addr_len = rpt_ind_ptr->mt_sms.data[2]; // The sc addr len is at the third byte of the RP-DATA

         /* Spec TS 23.040 Section 9.1.2.5 and 9.2.3.8 specify that the maximum length of the
            full address field (Address Length, Type of Address and Address Value) is 12 octets.
            The length of the destination address should be between 3 to 12 octets (20 semi-octets). */
         if ((addr_len >= 2 && addr_len <= 11) &&
             rpt_ind_ptr->mt_sms.data_len > (5 + addr_len))
         {
           int tpdu_len = rpt_ind_ptr->mt_sms.data[4 + addr_len];
           cm_called_party_bcd_no_T  bcd_addr;

           bcd_addr.present = TRUE;
           bcd_addr.length = addr_len;
           (void)memscpy( bcd_addr.data,
                          addr_len,
                          &(rpt_ind_ptr->mt_sms.data[3]),
                          addr_len);
           tpdu_len = MIN(tpdu_len, WMS_MAX_LEN - (5 + addr_len));
           if (tpdu_len > 0) {
              as_id = wms_get_as_id_from_message_mode(WMS_MESSAGE_MODE_GW); // FIX: MESSAGE MODE MAY NOT BE SUFFICIENT
              wms_msg_sm_data_ind_proc(as_id,
                                       WMS_DBG_CONNECTION_ID,
                                       WMS_DUMMY_TRANSACTION_ID,
                                       &bcd_addr,
                                       &(rpt_ind_ptr->mt_sms.data[5 + addr_len]),
                                       tpdu_len,
                                       WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS);
              cmd_err = WMS_CMD_ERR_NONE;
           }
         }
       }
       break;
#endif

#ifdef FEATURE_GWSMS_BROADCAST
     case WMS_FORMAT_GW_CB:
        /* GSM spec 04.12, section 3.1 specifies that a cell broadcast message consists of a 88 octets
           of information. */
        if (rpt_ind_ptr->mt_sms.data_len <= WMS_GW_CB_PAGE_SIZE) {
           as_id = wms_get_as_id_from_message_mode(WMS_MESSAGE_MODE_GW); // FIX: MESSAGE MODE MAY NOT BE SUFFICIENT
           wms_msg_gw_cb_data_ind_proc(as_id, rpt_ind_ptr->mt_sms.data, rpt_ind_ptr->mt_sms.data_len, FALSE);
           cmd_err = WMS_CMD_ERR_NONE;
        }
        break;
#endif

     default:
        break;
     }
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  // Send event
  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    wms_dbg_event_info_s_type  dbg_event_info;

    memset(&dbg_event_info, 0, sizeof(dbg_event_info));

    wms_dbg_event_notify( WMS_DBG_EVENT_SEND_TRANSPORT_RPT_IND, &dbg_event_info );
  }

  return;
} /* wms_dbg_send_transport_rpt_ind_proc() */

#endif /* FEATURE_FUZZ_TEST */

/*===========================================================================
FUNCTION wms_dbg_process_cmd()                                     (INTERNAL)

DESCRIPTION
  Dispatch the Debug command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_dbg_process_cmd(
  wms_cmd_type  *cmd_ptr
)
{

  switch( cmd_ptr->hdr.cmd_id )
  {

#ifdef FEATURE_CDSMS
    case WMS_CMD_DBG_RESET_TL_SEQ_NUM:
      wms_dbg_reset_tl_seq_num_proc( cmd_ptr );
      break;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
    case WMS_CMD_DBG_SET_MSG_REF_NUM:
      wms_dbg_set_sim_message_number(cmd_ptr);
      break;

    case WMS_CMD_DBG_CLEAR_SMMA:
      wms_dbg_clear_smma_flag_proc(cmd_ptr);
      break;
#endif /* FEATURE_GWSMS */

    case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      wms_dbg_get_retry_interval_proc(cmd_ptr);
      break;

    case WMS_CMD_DBG_SET_RETRY_INTERVAL:
      wms_dbg_set_retry_interval_proc(cmd_ptr);
      break;

    case WMS_CMD_DBG_GET_RETRY_PERIOD:
      wms_dbg_get_retry_period_proc(cmd_ptr);
      break;

    case WMS_CMD_DBG_SET_RETRY_PERIOD:
      wms_dbg_set_retry_period_proc(cmd_ptr);
      break;

#ifdef FEATURE_FUZZ_TEST
    case WMS_CMD_DBG_SEND_TRANSPORT_RPT_IND:
      wms_dbg_send_transport_rpt_ind_proc(cmd_ptr);
      break;
#endif

    default:
      break;
  }

  return;
} /* wms_dbg_process_cmd() */

/*===========================================================================
FUNCTION wms_dbg_init()                                            (INTERNAL)

DESCRIPTION
  Do Debug-related initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Debug data initialized.
===========================================================================*/
void wms_dbg_init(
  void
)
{
  wms_dbg_s_type              *dbg_s_ptr = wms_dbg_s_ptr();

  MSG_HIGH_0("wms_dbg_init()");

  dbg_s_ptr->init_complete = TRUE;

  return;
} /* wms_dbg_init */


#define WMS_MAX_CMD_IDS 79
#define WMS_MAX_CMD_ERRS 66
#define WMS_MAX_CFG_EVENTS 27
#define WMS_MAX_MSG_EVENTS 26
#define WMS_MAX_DC_EVENTS 8
#define WMS_MAX_BC_EVENTS 12
#define WMS_MAX_BC_MM_EVENTS 18
#define WMS_MAX_DBG_EVENTS 6

typedef struct
{
  wms_cmd_id_e_type  cmd_id;
  char              *cmd_string;
} wms_dbg_cmd_info_s_type;

typedef struct
{
  wms_cmd_err_e_type  cmd_err_id;
  char               *cmd_err_string;
} wms_dbg_cmd_err_info_s_type;

static wms_dbg_cmd_info_s_type wms_dbg_cmd_info[] =
{
  {WMS_CMD_CFG_SET_ROUTES,                  "WMS_CMD_CFG_SET_ROUTES"},
  {WMS_CMD_CFG_GET_ROUTES,                  "WMS_CMD_CFG_GET_ROUTES"},
  {WMS_CMD_CFG_GET_MEMORY_STATUS,           "WMS_CMD_CFG_GET_MEMORY_STATUS"},
  {WMS_CMD_CFG_GET_MESSAGE_LIST,            "WMS_CMD_CFG_GET_MESSAGE_LIST"},
  {WMS_CMD_CFG_SET_GW_DOMAIN_PREF,          "WMS_CMD_CFG_SET_GW_DOMAIN_PREF"},
  {WMS_CMD_CFG_GET_GW_DOMAIN_PREF,          "WMS_CMD_CFG_GET_GW_DOMAIN_PREF"},
  {WMS_CMD_CFG_SET_PRIMARY_CLIENT,          "WMS_CMD_CFG_SET_PRIMARY_CLIENT"},
  {WMS_CMD_CFG_SET_MEMORY_FULL,             "WMS_CMD_CFG_SET_MEMORY_FULL"},
  {WMS_CMD_CFG_SET_LINK_CONTROL,            "WMS_CMD_CFG_SET_LINK_CONTROL"},
  {WMS_CMD_CFG_GET_LINK_CONTROL,            "WMS_CMD_CFG_GET_LINK_CONTROL"},
  {WMS_CMD_CFG_SET_MESSAGE_WAITING,         "WMS_CMD_CFG_SET_MESSAGE_WAITING"},
  {WMS_CMD_CFG_GET_MESSAGE_WAITING,         "WMS_CMD_CFG_GET_MESSAGE_WAITING"},
  {WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT,       "WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT"},
  {WMS_CMD_CFG_RRC_CELL_CHANGE_EVENT_CB,    "WMS_CMD_CFG_RRC_CELL_CHANGE_EVENT_CB"},
  {WMS_CMD_CFG_SET_LTE_DOMAIN_PREF,         "WMS_CMD_CFG_SET_LTE_DOMAIN_PREF"},
  {WMS_CMD_MSG_SEND,                        "WMS_CMD_MSG_SEND"},
  {WMS_CMD_MSG_ACK,                         "WMS_CMD_MSG_ACK"},
  {WMS_CMD_MSG_READ,                        "WMS_CMD_MSG_READ"},
  {WMS_CMD_MSG_WRITE,                       "WMS_CMD_MSG_WRITE"},
  {WMS_CMD_MSG_DELETE,                      "WMS_CMD_MSG_DELETE"},
  {WMS_CMD_MSG_DELETE_ALL,                  "WMS_CMD_MSG_DELETE_ALL"},
  {WMS_CMD_MSG_MODIFY_TAG,                  "WMS_CMD_MSG_MODIFY_TAG"},
  {WMS_CMD_MSG_READ_TEMPLATE,               "WMS_CMD_MSG_READ_TEMPLATE"},
  {WMS_CMD_MSG_WRITE_TEMPLATE,              "WMS_CMD_MSG_WRITE_TEMPLATE"},
  {WMS_CMD_MSG_DELETE_TEMPLATE,             "WMS_CMD_MSG_DELETE_TEMPLATE"},
  {WMS_CMD_MSG_DELETE_TEMPLATE_ALL,         "WMS_CMD_MSG_DELETE_TEMPLATE_ALL"},
  {WMS_CMD_MSG_READ_STS_REPORT,             "WMS_CMD_MSG_READ_STS_REPORT"},
  {WMS_CMD_MSG_WRITE_STS_REPORT,            "WMS_CMD_MSG_WRITE_STS_REPORT"},
  {WMS_CMD_MSG_DELETE_STS_REPORT,           "WMS_CMD_MSG_DELETE_STS_REPORT"},
  {WMS_CMD_MSG_DELETE_STS_REPORT_ALL,       "WMS_CMD_MSG_DELETE_STS_REPORT_ALL"},
  {WMS_CMD_MSG_TRANSPORT_REG,               "WMS_CMD_MSG_TRANSPORT_REG"},
  {WMS_CMD_DC_CONNECT,                      "WMS_CMD_DC_CONNECT"},
  {WMS_CMD_DC_DISCONNECT,                   "WMS_CMD_DC_DISCONNECT"},
  {WMS_CMD_DC_ENABLE_AUTO_DISCONNECT,       "WMS_CMD_DC_ENABLE_AUTO_DISCONNECT"},
  {WMS_CMD_DC_DISABLE_AUTO_DISCONNECT,      "WMS_CMD_DC_DISABLE_AUTO_DISCONNECT"},
  {WMS_CMD_BC_MM_GET_CONFIG,                "WMS_CMD_BC_MM_GET_CONFIG"},
  {WMS_CMD_BC_MM_GET_PREF,                  "WMS_CMD_BC_MM_GET_PREF"},
  {WMS_CMD_BC_MM_SET_PREF,                  "WMS_CMD_BC_MM_SET_PREF"},
  {WMS_CMD_BC_MM_GET_TABLE,                 "WMS_CMD_BC_MM_GET_TABLE"},
  {WMS_CMD_BC_MM_SELECT_SRV,                "WMS_CMD_BC_MM_SELECT_SRV"},
  {WMS_CMD_BC_MM_GET_ALL_SRV_IDS,           "WMS_CMD_BC_MM_GET_ALL_SRV_IDS"},
  {WMS_CMD_BC_MM_GET_SRV_INFO,              "WMS_CMD_BC_MM_GET_SRV_INFO"},
  {WMS_CMD_BC_MM_ADD_SRV,                   "WMS_CMD_BC_MM_ADD_SRV"},
  {WMS_CMD_BC_MM_DELETE_SRV,                "WMS_CMD_BC_MM_DELETE_SRV"},
  {WMS_CMD_BC_MM_CHANGE_SRV_INFO,           "WMS_CMD_BC_MM_CHANGE_SRV_INFO"},
  {WMS_CMD_BC_MM_DELETE_ALL_SERVICES,       "WMS_CMD_BC_MM_DELETE_ALL_SERVICES"},
  {WMS_CMD_BC_MM_SELECT_ALL_SERVICES,       "WMS_CMD_BC_MM_SELECT_ALL_SERVICES"},
  {WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES, "WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES"},
  {WMS_CMD_BC_MM_MSG_DELETE_INDICATION,     "WMS_CMD_BC_MM_MSG_DELETE_INDICATION"},
  {WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION, "WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION"},
  {WMS_CMD_BC_MM_GET_READING_PREF,          "WMS_CMD_BC_MM_GET_READING_PREF"},
  {WMS_CMD_BC_MM_SET_READING_PREF,          "WMS_CMD_BC_MM_SET_READING_PREF"},
  {WMS_CMD_DBG_RESET_TL_SEQ_NUM,            "WMS_CMD_DBG_RESET_TL_SEQ_NUM"},
  {WMS_CMD_DBG_SET_MSG_REF_NUM,             "WMS_CMD_DBG_SET_MSG_REF_NUM"},
  {WMS_CMD_DBG_CLEAR_SMMA,                  "WMS_CMD_DBG_CLEAR_SMMA"},
  {WMS_CMD_DBG_GET_RETRY_INTERVAL,          "WMS_CMD_DBG_GET_RETRY_INTERVAL"},
  {WMS_CMD_DBG_SET_RETRY_INTERVAL,          "WMS_CMD_DBG_SET_RETRY_INTERVAL"},
  {WMS_CMD_DBG_GET_RETRY_PERIOD,            "WMS_CMD_DBG_GET_RETRY_PERIOD"},
  {WMS_CMD_DBG_SET_RETRY_PERIOD,            "WMS_CMD_DBG_SET_RETRY_PERIOD"},
  {WMS_CMD_MSG_MC_MT_MSG_E,                 "WMS_CMD_MSG_MC_MT_MSG_E"},
  {WMS_CMD_MSG_MC_MO_STATUS_E,              "WMS_CMD_MSG_MC_MO_STATUS_E"},
  {WMS_CMD_MSG_MC_STATE_CHANGED_E,          "WMS_CMD_MSG_MC_STATE_CHANGED_E"},
  {WMS_CMD_MSG_MC_MWI_E,                    "WMS_CMD_MSG_MC_MWI_E"},
  {WMS_CMD_MSG_GSTK_EVT,                    "WMS_CMD_MSG_GSTK_EVT"},
  {WMS_CMD_DC_CALL_INCOM_E,                 "WMS_CMD_DC_CALL_INCOM_E"},
  {WMS_CMD_DC_CALL_CONNECT_E,               "WMS_CMD_DC_CALL_CONNECT_E"},
  {WMS_CMD_DC_CALL_END_E,                   "WMS_CMD_DC_CALL_END_E"},
  {WMS_CMD_DC_CALL_ERROR_E,                 "WMS_CMD_DC_CALL_ERROR_E"},
  {WMS_CMD_CM_BC_ENABLE_FAILURE_E,          "WMS_CMD_CM_BC_ENABLE_FAILURE_E"},
  {WMS_CMD_CM_START_FULL_SERVICE_E,         "WMS_CMD_CM_START_FULL_SERVICE_E"},
  {WMS_CMD_CM_ONLINE_E,                     "WMS_CMD_CM_ONLINE_E"},
  {WMS_CMD_CM_LPM_E,                        "WMS_CMD_CM_LPM_E"},
  {WMS_CMD_GSDI_ASYNC_CB,                   "WMS_CMD_GSDI_ASYNC_CB"},
  {WMS_CMD_GSDI_CARD_STATUS_CB,             "WMS_CMD_GSDI_CARD_STATUS_CB"},
  {WMS_CMD_GSDI_REFRESH_FCN_IND,            "WMS_CMD_GSDI_REFRESH_FCN_IND"},
  {WMS_CMD_BC_SCPT_DATA,                    "WMS_CMD_BC_SCPT_DATA"},
  {WMS_CMD_SS_CHANGE_INFO,                  "WMS_CMD_SS_CHANGE_INFO"},
  {WMS_CMD_IPAPP_CHANGE_INFO,               "WMS_CMD_IPAPP_CHANGE_INFO"},
  {WMS_CMD_DEM_APPS_POWERDOWN,              "WMS_CMD_DEM_APPS_POWERDOWN"},
  {WMS_CMD_DEM_APPS_SUSPEND,                "WMS_CMD_DEM_APPS_SUSPEND"},
  {WMS_CMD_DEM_APPS_POWER_RESTORED,         "WMS_CMD_DEM_APPS_POWER_RESTORED"},
  {WMS_CMD_DEM_APPS_RUN_QUIET,              "WMS_CMD_DEM_APPS_RUN_QUIET"},
  {WMS_CMD_DEM_APPS_RUN_NORMAL,             "WMS_CMD_DEM_APPS_RUN_NORMAL"},
  {WMS_CMD_DUMMY,                           "WMS_CMD_DUMMY"},
  {WMS_CMD_MMGSDI_EVENT_CB,                 "WMS_CMD_MMGSDI_EVENT_CB"},
  {WMS_CMD_MMGSDI_RESPONSE_CB,              "WMS_CMD_MMGSDI_RESPONSE_CB"},
  {WMS_CMD_MMGSDI_SEEK_CB,                  "WMS_CMD_MMGSDI_SEEK_CB"},
  {WMS_CMD_CM_PH_EVENT_CB,                  "WMS_CMD_CM_PH_EVENT_CB"},
  {WMS_CMD_CM_SUBS_EVENT_CB,                "WMS_CMD_CM_SUBS_EVENT_CB"},
  {WMS_CMD_CARD_EF_INFO_UPDATE,             "WMS_CMD_CARD_EF_INFO_UPDATE"},
  {WMS_CMD_CLIENT_ACTIVATE,                 "WMS_CMD_CLIENT_ACTIVATE"},
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  {WMS_CMD_NV_REFRESH_CB,                   "WMS_CMD_NV_REFRESH_CB"},  
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
#ifdef FEATURE_FUZZ_TEST
  {WMS_CMD_MAX,                             "WMS_CMD_DBG_SEND_TRANSPORT_RPT_IND"}
#else
  {WMS_CMD_MAX,                             "WMS_CMD_MAX"}
#endif
};

static wms_dbg_cmd_err_info_s_type wms_dbg_cmd_err_info[] =
{
  {WMS_CMD_ERR_NONE,                     "WMS_CMD_ERR_NONE"},
  {WMS_CMD_ERR_CLIENT_ID,                "WMS_CMD_ERR_CLIENT_ID"},
  {WMS_CMD_ERR_NO_RESOURCE,              "WMS_CMD_ERR_NO_RESOURCE"},
  {WMS_CMD_ERR_UNSUPPORTED,              "WMS_CMD_ERR_UNSUPPORTED"},
  {WMS_CMD_ERR_BUSY,                     "WMS_CMD_ERR_BUSY"},
  {WMS_CMD_ERR_NULL_PTR,                 "WMS_CMD_ERR_NULL_PTR"},
  {WMS_CMD_ERR_NO_NETWORK,               "WMS_CMD_ERR_NO_NETWORK"},
  {WMS_CMD_ERR_BAD_PARM,                 "WMS_CMD_ERR_BAD_PARM"},
  {WMS_CMD_ERR_CS_ERROR,                 "WMS_CMD_ERR_CS_ERROR"},
  {WMS_CMD_ERR_CFG_NV_WRITE,             "WMS_CMD_ERR_CFG_NV_WRITE"},
  {WMS_CMD_ERR_CFG_NV_READ,              "WMS_CMD_ERR_CFG_NV_READ"},
  {WMS_CMD_ERR_CFG_ROUTE,                "WMS_CMD_ERR_CFG_ROUTE"},
  {WMS_CMD_ERR_CFG_MEM_STORE,            "WMS_CMD_ERR_CFG_MEM_STORE"},
  {WMS_CMD_ERR_CFG_MSG_CLASS,            "WMS_CMD_ERR_CFG_MSG_CLASS"},
  {WMS_CMD_ERR_CFG_DOMAIN_PREF,          "WMS_CMD_ERR_CFG_DOMAIN_PREF"},
  {WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT,  "WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT"},
  {WMS_CMD_ERR_CFG_LINK_CONTROL,         "WMS_CMD_ERR_CFG_LINK_CONTROL"},
  {WMS_CMD_ERR_CFG_MESSAGE_WAITING,      "WMS_CMD_ERR_CFG_MESSAGE_WAITING"},
  {WMS_CMD_ERR_MSG_MEMORY_STORE,         "WMS_CMD_ERR_MSG_MEMORY_STORE"},
  {WMS_CMD_ERR_MSG_INDEX,                "WMS_CMD_ERR_MSG_INDEX"},
  {WMS_CMD_ERR_MSG_EMPTY_MESSAGE,        "WMS_CMD_ERR_MSG_EMPTY_MESSAGE"},
  {WMS_CMD_ERR_MSG_TAG,                  "WMS_CMD_ERR_MSG_TAG"},
  {WMS_CMD_ERR_MSG_ENCODE,               "WMS_CMD_ERR_MSG_ENCODE"},
  {WMS_CMD_ERR_MSG_DECODE,               "WMS_CMD_ERR_MSG_DECODE"},
  {WMS_CMD_ERR_MSG_SEND_MODE,            "WMS_CMD_ERR_MSG_SEND_MODE"},
  {WMS_CMD_ERR_MSG_WRITE_MODE,           "WMS_CMD_ERR_MSG_WRITE_MODE"},
  {WMS_CMD_ERR_MSG_MEMORY_FULL,          "WMS_CMD_ERR_MSG_MEMORY_FULL"},
  {WMS_CMD_ERR_MSG_RAM_FULL,             "WMS_CMD_ERR_MSG_RAM_FULL"},
  {WMS_CMD_ERR_MSG_SIM_FULL,             "WMS_CMD_ERR_MSG_SIM_FULL"},
  {WMS_CMD_ERR_MSG_NV_FULL,              "WMS_CMD_ERR_MSG_NV_FULL"},
  {WMS_CMD_ERR_MSG_TRANSACTION_ID,       "WMS_CMD_ERR_MSG_TRANSACTION_ID"},
  {WMS_CMD_ERR_MSG_SIM_WRITE,            "WMS_CMD_ERR_MSG_SIM_WRITE"},
  {WMS_CMD_ERR_MSG_SIM_READ,             "WMS_CMD_ERR_MSG_SIM_READ"},
  {WMS_CMD_ERR_MSG_SIM_DELETE,           "WMS_CMD_ERR_MSG_SIM_DELETE"},
  {WMS_CMD_ERR_MSG_RUIM_WRITE,           "WMS_CMD_ERR_MSG_RUIM_WRITE"},
  {WMS_CMD_ERR_MSG_RUIM_READ,            "WMS_CMD_ERR_MSG_RUIM_READ"},
  {WMS_CMD_ERR_MSG_RUIM_DELETE,          "WMS_CMD_ERR_MSG_RUIM_DELETE"},
  {WMS_CMD_ERR_MSG_NV_WRITE,             "WMS_CMD_ERR_MSG_NV_WRITE"},
  {WMS_CMD_ERR_MSG_NV_READ,              "WMS_CMD_ERR_MSG_NV_READ"},
  {WMS_CMD_ERR_MSG_NV_DELETE,            "WMS_CMD_ERR_MSG_NV_DELETE"},
  {WMS_CMD_ERR_MSG_TPDU_TYPE,            "WMS_CMD_ERR_MSG_TPDU_TYPE"},
  {WMS_CMD_ERR_MSG_FORMAT,               "WMS_CMD_ERR_MSG_FORMAT"},
  {WMS_CMD_ERR_MSG_NO_MO_MSG,            "WMS_CMD_ERR_MSG_NO_MO_MSG"},
  {WMS_CMD_ERR_MSG_NO_SC_ADDRESS,        "WMS_CMD_ERR_MSG_NO_SC_ADDRESS"},
  {WMS_CMD_ERR_MSG_LPM,                  "WMS_CMD_ERR_MSG_LPM"},
  {WMS_CMD_ERR_MSG_BLOCKED,              "WMS_CMD_ERR_MSG_BLOCKED"},
  {WMS_CMD_ERR_PREF_CHN_UNSUPPORTED_S,   "WMS_CMD_ERR_PREF_CHN_UNSUPPORTED_S"},
  {WMS_CMD_ERR_MSG_GSTK_ACCESS_RAT,      "WMS_CMD_ERR_MSG_GSTK_ACCESS_RAT"},
  {WMS_CMD_ERR_BC_BAD_PREF,              "WMS_CMD_ERR_BC_BAD_PREF"},
  {WMS_CMD_ERR_BC_CANNOT_ACTIVATE,       "WMS_CMD_ERR_BC_CANNOT_ACTIVATE"},
  {WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL,   "WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL"},
  {WMS_CMD_ERR_BC_NV_WRITE,              "WMS_CMD_ERR_BC_NV_WRITE"},
  {WMS_CMD_ERR_BC_BAD_SRV_ID,            "WMS_CMD_ERR_BC_BAD_SRV_ID"},
  {WMS_CMD_ERR_BC_TABLE_FULL,            "WMS_CMD_ERR_BC_TABLE_FULL"},
  {WMS_CMD_ERR_BC_DUPLICATE_SRV,         "WMS_CMD_ERR_BC_DUPLICATE_SRV"},
  {WMS_CMD_ERR_BC_SIM_WRITE,             "WMS_CMD_ERR_BC_SIM_WRITE"},
  {WMS_CMD_ERR_BC_SIM_READ,              "WMS_CMD_ERR_BC_SIM_READ"},
  {WMS_CMD_ERR_BC_DEACTIVATE_NOT_ALLOWED, "WMS_CMD_ERR_BC_DEACTIVATE_NOT_ALLOWED"},
  {WMS_CMD_ERR_DC_BAD_STATE,             "WMS_CMD_ERR_DC_BAD_STATE"},
  {WMS_CMD_ERR_DC_BAD_CALL_ID,           "WMS_CMD_ERR_DC_BAD_CALL_ID"},
  {WMS_CMD_ERR_DC_BAD_PARM,              "WMS_CMD_ERR_DC_BAD_PARM"},
  {WMS_CMD_ERR_DC_LPM,                   "WMS_CMD_ERR_DC_LPM"},
  {WMS_CMD_ERR_DBG_BAD_PARAM,            "WMS_CMD_ERR_DBG_BAD_PARAM"},
  {WMS_CMD_ERR_TRANSPORT_NOT_READY,      "WMS_CMD_ERR_TRANSPORT_NOT_READY"},
  {WMS_CMD_ERR_TRANSPORT_NOT_ALLOWED,    "WMS_CMD_ERR_TRANSPORT_NOT_ALLOWED"},
  {WMS_CMD_ERR_1X_AND_VOICE_NOT_ALLOWED, "WMS_CMD_ERR_1X_AND_VOICE_NOT_ALLOWED"},
  {WMS_CMD_ERR_AS_ID,                    "WMS_CMD_ERR_AS_ID"},
  {WMS_CMD_ERR_IMS_NOT_ALLOWED_IN_SRLTE, "WMS_CMD_ERR_IMS_NOT_ALLOWED_IN_SRLTE"},
  {WMS_CMD_ERR_MAX,                      "WMS_CMD_ERR_MAX"}
};

static char *wms_dbg_cfg_event_string[WMS_MAX_CFG_EVENTS] =
{
  "WMS_CFG_EVENT_GW_READY",
  "WMS_CFG_EVENT_CDMA_READY",
  "WMS_CFG_EVENT_ROUTES",
  "WMS_CFG_EVENT_MEMORY_STATUS",
  "WMS_CFG_EVENT_MESSAGE_LIST",
  "WMS_CFG_EVENT_MEMORY_FULL",
  "WMS_CFG_EVENT_GW_DOMAIN_PREF",
  "WMS_CFG_EVENT_CELL_CHANGE",
  "WMS_CFG_EVENT_PRIMARY_CLIENT_SET",
  "WMS_CFG_EVENT_MEMORY_STATUS_SET",
  "WMS_CFG_EVENT_LINK_CONTROL",
  "WMS_CFG_EVENT_CB_ERROR",
  "WMS_CFG_EVENT_MS_READY",
  "WMS_CFG_EVENT_MS_MEMORY_FULL",
  "WMS_CFG_EVENT_MS_GW_DOMAIN_PREF",
  "WMS_CFG_EVENT_MS_CELL_CHANGE",
  "WMS_CFG_EVENT_MS_MEMORY_STATUS_SET",
  "WMS_CFG_EVENT_MSG_WAITING",
  "WMS_CFG_EVENT_MS_NOT_READY",
  "WMS_CFG_EVENT_MS_SIM_INIT_START",
  "WMS_CFG_EVENT_MS_SIM_INIT_END",
  "WMS_CFG_EVENT_MS_REFRESH_START",
  "WMS_CFG_EVENT_MS_REFRESH_END",
  "WMS_CFG_EVENT_MS_SERV_AVAIL",
  "WMS_CFG_EVENT_MS_SERV_NOT_AVAIL",
  "WMS_CFG_EVENT_MS_GWL_DOMAIN_PREF",
  "WMS_CFG_EVENT_MAX"
};

static char *wms_dbg_msg_event_string[WMS_MAX_MSG_EVENTS] =
{
  /* Command related events */
  "WMS_MSG_EVENT_SEND",
  "WMS_MSG_EVENT_ACK",
  "WMS_MSG_EVENT_READ",
  "WMS_MSG_EVENT_WRITE",
  "WMS_MSG_EVENT_DELETE",
  "WMS_MSG_EVENT_DELETE_ALL",
  "WMS_MSG_EVENT_MODIFY_TAG",
  "WMS_MSG_EVENT_READ_TEMPLATE",
  "WMS_MSG_EVENT_WRITE_TEMPLATE",
  "WMS_MSG_EVENT_DELETE_TEMPLATE",
  "WMS_MSG_EVENT_DELETE_TEMPLATE_ALL",
  "WMS_MSG_EVENT_READ_STS_REPORT",
  "WMS_MSG_EVENT_WRITE_STS_REPORT",
  "WMS_MSG_EVENT_DELETE_STS_REPORT",
  "WMS_MSG_EVENT_DELETE_STS_REPORT_ALL",

  /* Events from lower layer */
  "WMS_MSG_EVENT_RECEIVED_MESSAGE",
  "WMS_MSG_EVENT_SUBMIT_REPORT",
  "WMS_MSG_EVENT_STATUS_REPORT",
  "WMS_MSG_EVENT_MT_MESSAGE_ERROR", /* for example, MT msg ack timeout */
  "WMS_MSG_EVENT_ACK_REPORT",

  "WMS_MSG_EVENT_DUPLICATE_CB_PAGE",
  "WMS_MSG_EVENT_TRANSPORT_REG",
  "WMS_MSG_EVENT_ETWS_NOTIFICATION",
  "WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION",
  "WMS_MSG_EVENT_SMSC_ADDR_CHANGE",
  "WMS_MSG_EVENT_MAX"
};

static char *wms_dbg_dc_event_string[WMS_MAX_DC_EVENTS] =
{
  "WMS_DC_EVENT_INCOMING",
  "WMS_DC_EVENT_CONNECTED",
  "WMS_DC_EVENT_ABORTED",
  "WMS_DC_EVENT_DISCONNECTED",
  "WMS_DC_EVENT_CONNECTING",
  "WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT",
  "WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT",
  "WMS_DC_EVENT_MAX"
};

static char *wms_dbg_bc_event_string[WMS_MAX_BC_EVENTS] =
{
  "WMS_BC_EVENT_CONFIG",
  "WMS_BC_EVENT_PREF",
  "WMS_BC_EVENT_TABLE",
  "WMS_BC_EVENT_SRV_IDS",
  "WMS_BC_EVENT_SRV_INFO",
  "WMS_BC_EVENT_SRV_ADDED",
  "WMS_BC_EVENT_SRV_DELETED",
  "WMS_BC_EVENT_SRV_UPDATED",
  "WMS_BC_EVENT_ALL_SRV_DELETED",
  "WMS_BC_EVENT_ENABLE_FAILURE",
  "WMS_BC_EVENT_DISABLE_FAILURE",
  "WMS_BC_EVENT_MAX"
};

static char *wms_dbg_bc_mm_event_string[WMS_MAX_BC_MM_EVENTS] =
{
  "WMS_BC_MM_EVENT_CONFIG",
  "WMS_BC_MM_EVENT_PREF",
  "WMS_BC_MM_EVENT_TABLE",
  "WMS_BC_MM_EVENT_SRV_IDS",
  "WMS_BC_MM_EVENT_SRV_INFO",
  "WMS_BC_MM_EVENT_SRV_UPDATED",
  "WMS_BC_MM_EVENT_ADD_SRVS",
  "WMS_BC_MM_EVENT_DELETE_SRVS",
  "WMS_BC_MM_EVENT_SELECT_ALL_SRVS",
  "WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS",
  "WMS_BC_MM_EVENT_READING_PREF",
  "WMS_BC_MM_EVENT_DELETE_ALL_SRVS",
  "WMS_BC_MM_EVENT_ENABLE_FAILURE",
  "WMS_BC_MM_EVENT_DISABLE_FAILURE",
  "WMS_BC_MM_EVENT_SCPT_ADD_SRVS",
  "WMS_BC_MM_EVENT_SCPT_DELETE_SRVS",
  "WMS_BC_MM_EVENT_SCPT_DELETE_ALL_SRVS",
  "WMS_BC_MM_EVENT_MAX"
};

static char *wms_dbg_dbg_event_string[WMS_MAX_DBG_EVENTS] =
{
  "WMS_DBG_EVENT_RESET_TL_SEQ_NUM",
  "WMS_DBG_EVENT_SET_SIM_MESSAGE_NUMBER",
  "WMS_DBG_EVENT_CLEAR_SMMA_FLAG",
  "WMS_DBG_EVENT_RETRY_INTERVAL",
  "WMS_DBG_EVENT_RETRY_PERIOD",
  "WMS_DBG_EVENT_MAX"
};

/*===========================================================================
FUNCTION wms_dbg_get_cmd_string()                                  (INTERNAL)

DESCRIPTION
  This function returns the Command given the Command ID

DEPENDENCIES
  Command Enumeration (wms_cmd_id_e_type)

RETURN VALUE
  Command String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cmd_string(
  wms_cmd_id_e_type cmd_id
)
{
  int i;
  int max_cmd = (sizeof(wms_dbg_cmd_info) / sizeof(wms_dbg_cmd_info[0]));

  for (i = 0; i < max_cmd; i++)
  {
    if (cmd_id == wms_dbg_cmd_info[i].cmd_id)
    {
      return (wms_dbg_cmd_info[i].cmd_string);
    }
  }

  return ("Unknown Command");
} /* wms_dbg_get_cmd_string */

/*===========================================================================
FUNCTION wms_dbg_get_cmd_err_string()                              (INTERNAL)

DESCRIPTION
  This function returns the Command Error given the Command Error ID

DEPENDENCIES
  Command Error Enumeration (wms_cmd_err_e_type)

RETURN VALUE
  Command String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cmd_err_string(
  wms_cmd_err_e_type cmd_err
)
{
  int i;
  int max_error = (sizeof(wms_dbg_cmd_err_info) / sizeof(wms_dbg_cmd_err_info[0]));

  for (i = 0; i < max_error; i++)
  {
    if (cmd_err == wms_dbg_cmd_err_info[i].cmd_err_id)
    {
      return (wms_dbg_cmd_err_info[i].cmd_err_string);
    }
  }

  return ("Unknown Command Error");
} /* wms_dbg_get_cmd_err_string */

/*===========================================================================
FUNCTION wms_dbg_get_cfg_event_string()                            (INTERNAL)

DESCRIPTION
  This function returns the Configuration event String given the cfg_event

DEPENDENCIES
  Configuration Event Enumeration (wms_cfg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cfg_event_string(
  uint16 cfg_event
)
{
  if (cfg_event < WMS_MAX_CFG_EVENTS)
  {
    return wms_dbg_cfg_event_string[cfg_event];
  }

  return "Invalid Event";
} /* wms_dbg_get_cfg_event_string */

/*===========================================================================
FUNCTION wms_dbg_get_msg_event_string()                            (INTERNAL)

DESCRIPTION
  This function returns the Message event String given the msg_event

DEPENDENCIES
  Message Event Enumeration (wms_msg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_msg_event_string(
  uint16 msg_event
)
{
  if (msg_event < WMS_MAX_MSG_EVENTS)
  {
    return wms_dbg_msg_event_string[msg_event];
  }

  return "Invalid Event";
} /* wms_dbg_get_msg_event_string() */

/*===========================================================================
FUNCTION wms_dbg_get_dc_event_string()                             (INTERNAL)

DESCRIPTION
  This function returns the DC event String given the _event

DEPENDENCIES
  DC Event Enumeration (wms_dc_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_dc_event_string(
  uint16 dc_event
)
{
  if (dc_event < WMS_MAX_DC_EVENTS)
  {
    return wms_dbg_dc_event_string[dc_event];
  }

  return "Invalid Event";
} /* wms_dbg_get_dc_event_string */

/*===========================================================================
FUNCTION wms_dbg_get_bc_event_string()                             (INTERNAL)

DESCRIPTION
  This function returns the Broadcast event String given the bc_event

DEPENDENCIES
  Broadcast Event Enumeration (wms_bc_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_bc_event_string(
  uint16 bc_event
)
{
  if (bc_event < WMS_MAX_BC_EVENTS)
  {
    return wms_dbg_bc_event_string[bc_event];
  }

  return "Invalid Event";
} /* wms_dbg_get_bc_event_string */

/*===========================================================================
FUNCTION wms_dbg_get_event_bc_mm_string()                          (INTERNAL)

DESCRIPTION
  This function returns the Multimode Broadcast event String given the bc_mm_event

DEPENDENCIES
  Multimode Broadcast Event Enumeration (wms_bc_mm_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_bc_mm_event_string(
  uint16 bc_mm_event
)
{
  if (bc_mm_event < WMS_MAX_BC_MM_EVENTS)
  {
    return wms_dbg_bc_mm_event_string[bc_mm_event];
  }

  return "Invalid Event";
} /* wms_dbg_get_bc_mm_event_string */

/*===========================================================================
FUNCTION wms_dbg_get_dbg_event_string()                            (INTERNAL)

DESCRIPTION
  This function returns the Debug event String given the dbg_event

DEPENDENCIES
  Debug Event Enumeration (wms_dbg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_dbg_event_string(
  uint16 dbg_event
)
{
  if (dbg_event < WMS_MAX_DBG_EVENTS)
  {
    return wms_dbg_dbg_event_string[dbg_event];
  }

  return "Invalid Event";
} /* wms_dbg_get_dbg_event_string */

#endif /* defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS) */

/* END of wmsdbg.c */
