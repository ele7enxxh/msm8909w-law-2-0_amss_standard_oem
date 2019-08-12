/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsdiag.c -- Diagnostic Services

  This file defines the packet handlers for the WMSDIAG interface.  It also
  contains the WMSDIAG logging function definition.

  -------------

    Copyright (c) 1998-2012 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsdiag.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"
#include "bit.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)
#include "diagcmd.h"
#include "wmsdiag.h"
#include "wms.h"
#include "err.h"
#include "string.h"
#include "wmsmsg.h"
#include "stringl.h"
#include "wmsutils.h"

#ifdef FEATURE_FUZZ_TEST
#include "wmsdbg.h"
#endif

/*============================================================================
   WMS Internal Useage Defines
============================================================================*/
#define WMSDIAG_REQ_STATE_IDLE       0
#define WMSDIAG_REQ_STATE_COMMAND_CB 1
#define WMSDIAG_REQ_STATE_MESSAGE_CB 2

#define WMSDIAG_INVALID_CLIENT_ID  0xFF

/*===========================================================================
   Static variables used
===========================================================================*/
/*  Initialize our client ID to some bogus value */
static wms_client_id_type wmsdiag_client_id = WMSDIAG_INVALID_CLIENT_ID;

/*  A global pointer used to update the message list */
static  wmsdiag_get_message_list_rsp_type wmsdiag_list;

/*  Initialize a transaction ID for diag log packets */
static wms_transaction_id_type wmsdiag_transaction_ID;

/*  A flag that tells us if the get message list command failed or not */
static boolean wmsdiag_get_message_list_failed = FALSE;

/*  A flag that tells us if the set routes command failed or not */
static boolean wmsdiag_set_routes_failed = FALSE;

/*  A flag that tells us if the read command failed or not */
static boolean wmsdiag_read_failed = FALSE;

/*  A flag that tells us if the read command was initiatiated */
static boolean wmsdiag_initiated_read = FALSE;

/*  The client message header to be passed to WMS */
static wms_client_msg_hdr_s_type     msg_hdr;

/*  A flag for keeping track of the memory store requested */
static wms_memory_store_e_type wmsdiag_req_storage;

/*  A state that tells us if request is still pending or not */
static uint8 req_pending = WMSDIAG_REQ_STATE_IDLE;

/*  A state that tells us if Get Bearer Request is still pending or not */
static uint8 get_bearer_req_state = WMSDIAG_REQ_STATE_IDLE;

/* Transacation ID for Get Bearer Pref Command */
static uint32 get_bearer_tid;

/* Current active sub ID */
static uint32 current_as_id = (uint32)WMSDIAG_AS_ID_1;

/*=============================================================================
    Arrays used internally by the WMS interface to map enumerated types in the
    WMS interface to our interface.  This was done to keep changes to those
    enumerated types transparent to our users.
=============================================================================*/

static wms_memory_store_e_type WMSDIAG_MEM_STORE_VALS [WMSDIAG_MEM_STORE_MAX] =
{
  WMS_MEMORY_STORE_NONE,
  WMS_MEMORY_STORE_RAM_GW,
  WMS_MEMORY_STORE_SIM,
  WMS_MEMORY_STORE_NV_GW,
  WMS_MEMORY_STORE_RAM_CDMA,
  WMS_MEMORY_STORE_RUIM,
  WMS_MEMORY_STORE_NV_CDMA
};

static wms_message_tag_e_type WMSDIAG_MSG_TAG_VALS [WMSDIAG_TAG_MAX] =
{
  WMS_TAG_NONE,
  WMS_TAG_MT_READ,
  WMS_TAG_MT_NOT_READ,
  WMS_TAG_MO_SENT,
  WMS_TAG_MO_NOT_SENT,
  WMS_TAG_MO_SENT_ST_NOT_RECEIVED,
  WMS_TAG_MO_SENT_ST_NOT_STORED,
  WMS_TAG_MO_SENT_ST_STORED,
  WMS_TAG_MO_TEMPLATE,
  WMS_TAG_STATUS_RPT
};

static wms_send_mode_e_type WMSDIAG_SEND_MODE_VALS [WMSDIAG_SEND_MODE_MAX] =
{
  WMS_SEND_MODE_MEMORY_STORE,
  WMS_SEND_MODE_CLIENT_MESSAGE
};

static wms_write_mode_e_type WMSDIAG_WRITE_MODE_VALS [WMSDIAG_WRITE_MODE_MAX] =
{
  WMS_WRITE_MODE_INSERT,
  WMS_WRITE_MODE_REPLACE
};


static wms_message_mode_e_type WMSDIAG_MSG_MODE_VALS [(int)WMSDIAG_MSG_MODE_MAX - 1] =
{
  WMS_MESSAGE_MODE_CDMA,
  WMS_MESSAGE_MODE_GW
};

static wms_digit_mode_e_type WMSDIAG_ADDR_DIGIT_MODE_VALS [WMSDIAG_ADDR_DIGIT_MODE_MAX] =
{
  WMS_DIGIT_MODE_4_BIT,  /* DTMF digits */
  WMS_DIGIT_MODE_8_BIT
};

static wms_number_mode_e_type WMSDIAG_ADDR_NUM_MODE_VALS [WMSDIAG_ADDR_NUM_MODE_MAX] =
{
  WMS_NUMBER_MODE_NONE_DATA_NETWORK,
  WMS_NUMBER_MODE_DATA_NETWORK
};

static wms_format_e_type WMSDIAG_DATA_FORMAT_VALS [WMSDIAG_DATA_FORMAT_MAX] =
{
  WMS_FORMAT_CDMA,              /* IS-95 */
  WMS_FORMAT_ANALOG_CLI,        /* IS-91 */
  WMS_FORMAT_ANALOG_VOICE_MAIL, /* IS-91 */
  WMS_FORMAT_ANALOG_SMS,        /* IS-91 */
  WMS_FORMAT_ANALOG_AWISMS,     /* IS-95 Alert With Information SMS */
  WMS_FORMAT_MWI,               /* Message Waiting Indication as voice mail */
  WMS_FORMAT_GW_PP,             /* GW Point-to-Point SMS */
  WMS_FORMAT_GW_CB              /* GW CB SMS */
};

static wms_route_e_type WMSDIAG_ROUTE_VALS [WMSDIAG_ROUTE_MAX] =
{
  WMS_ROUTE_DISCARD,          /* Discard message */
  WMS_ROUTE_STORE_AND_NOTIFY, /* Store message and notify clients */
  WMS_ROUTE_TRANSFER_ONLY,    /* Don't store msg but send it to clients */
  WMS_ROUTE_TRANSFER_AND_ACK, /* Send the message to clients and ack it */
  WMS_ROUTE_NO_CHANGE,        /* Don't change the current setting */
};
#ifdef FEATURE_GWSMS

static wms_gw_tpdu_type_e_type WMSDIAG_GW_TPDU_VALS [WMSDIAG_TPDU_MAX] =
{
  WMS_TPDU_DELIVER,
  WMS_TPDU_DELIVER_REPORT_ACK,
  WMS_TPDU_DELIVER_REPORT_ERROR,
  WMS_TPDU_SUBMIT,
  WMS_TPDU_SUBMIT_REPORT_ACK,
  WMS_TPDU_SUBMIT_REPORT_ERROR,
  WMS_TPDU_STATUS_REPORT,
  WMS_TPDU_COMMAND,
  WMS_TPDU_NONE
};

#endif /*  FEATURE GWSMS */

/*===========================================================================
FUNCTION WMSDIAG_MAP_TAG_TYPE

DESCRIPTION
  Maps the tag given by WMS to the appropriate tag value of WMSDIAG.

DEPENDENCIES
  None.

RETURN VALUE
  WMSDIAG_MessageTagType return_tag

SIDE EFFECTS
  NONE.

===========================================================================*/
WMSDIAG_MessageTagType wmsdiag_map_tag_type(wms_message_tag_e_type tag)
{
  WMSDIAG_MessageTagType return_tag = WMSDIAG_TAG_INVALID;

  switch(tag)
  {
    case WMS_TAG_NONE:
      return_tag = WMSDIAG_TAG_NONE;
      break;

    case WMS_TAG_MT_READ:
      return_tag = WMSDIAG_TAG_MT_READ;
      break;

    case WMS_TAG_MT_NOT_READ:
      return_tag = WMSDIAG_TAG_MT_NOT_READ;
      break;

    case WMS_TAG_MO_SENT:
      return_tag = WMSDIAG_TAG_MO_SENT;
      break;

    case WMS_TAG_MO_NOT_SENT:
      return_tag = WMSDIAG_TAG_MO_NOT_SENT;
      break;

    case WMS_TAG_MO_SENT_ST_NOT_RECEIVED:
      return_tag = WMSDIAG_TAG_MO_SENT_ST_NOT_RECEIVED;
      break;

    case WMS_TAG_MO_SENT_ST_NOT_STORED:
      return_tag = WMSDIAG_TAG_MO_SENT_ST_NOT_STORED;
      break;

    case WMS_TAG_MO_SENT_ST_STORED:
      return_tag = WMSDIAG_TAG_MO_SENT_ST_STORED;
      break;

    case WMS_TAG_MO_TEMPLATE:
      return_tag = WMSDIAG_TAG_MO_TEMPLATE;
      break;

    case WMS_TAG_STATUS_RPT:
      return_tag = WMSDIAG_TAG_STATUS_RPT;
      break;

    default:
      MSG_ERROR_1("Invalid tag type = %d", tag);
      break;
  }

  return return_tag;
}

/*===========================================================================
FUNCTION WMSDIAG_LIST_UPDATE_CB

DESCRIPTION
  This function updates the message list requested and
  accepts a cfg_event_info_s_type structure and creates a
  wmsdiag_get_message_list_log_type log packet that contains all the log
  If this function is called, the list was updated correctly.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets DIAG_ASYNC_BLOCK_SIG signal.

===========================================================================*/

void wmsdiag_list_update_cb (wms_cfg_event_e_type event,
                             wms_cfg_event_info_s_type *info_ptr)
{

  /*  Loop counter */
  int i;

  /*  If this is not an event pertaining to an updated of the message list, return */
  if (event != WMS_CFG_EVENT_MESSAGE_LIST)
  {
    return;
  }

  if (req_pending != WMSDIAG_REQ_STATE_MESSAGE_CB)
  {
    return;
  }

  MSG_HIGH_2("info_ptr->message_list.mem_store = %d wmsdiag_req_storage = %d", info_ptr->message_list.mem_store, wmsdiag_req_storage);

  /* If the memory store in the info_ptr do not match the request packet, return */
  if (info_ptr->message_list.mem_store != wmsdiag_req_storage)
  {
    return;
  }

  /*  Set up the parameters */
  wmsdiag_list.num_entries = info_ptr->message_list.len;
  wmsdiag_list.storage_type = (uint32)info_ptr->message_list.mem_store;
  wmsdiag_list.voice_mail_index = info_ptr->message_list.voice_mail_index;
  wmsdiag_list.voice_mail_active = info_ptr->message_list.voice_mail_active;

  for (i = 0; i < (int)(wmsdiag_list.num_entries); i++)
  {
    wmsdiag_list.messages[i].tag_type =
      (uint32)wmsdiag_map_tag_type(info_ptr->message_list.tags[i]);
    wmsdiag_list.messages[i].index = info_ptr->message_list.indices[i];
  }

  req_pending = WMSDIAG_REQ_STATE_IDLE;

  /*  SET THE SIGNAL WHICH SHOULD UNBLOCK DIAG */
  diagpkt_async_release ();
}

/*===========================================================================

FUNCTION WMSDIAG_GET_MESSAGE_LIST_CMD_CB

DESCRIPTION
  Callback for the get_message_list command to ensure that it was sent ok.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets DIAG_ASYNC_BLOCK_SIG signal if there was an error to unblock diag. Also sets
  wmsdiag_get_message_list_failed flag to indicate that there was an error,
  if applicable.

===========================================================================*/

void wmsdiag_get_message_list_cmd_cb (wms_cmd_id_e_type cmd,
                          void* user_data,
                          wms_cmd_err_e_type cmd_err)
{
  (void)user_data; // parameter not used

  /*  If this is not pertaining to a message list being read, return */
  if (cmd != WMS_CMD_CFG_GET_MESSAGE_LIST)
  {
    return;
  }

  if (req_pending != WMSDIAG_REQ_STATE_COMMAND_CB)
  {
    return;
  }

  MSG_HIGH_1("cmd_err = %d", cmd_err);

  /*  If there was an error */
  if (cmd_err != WMS_CMD_ERR_NONE)
  {
    /*  Indicate that the command failed */
    wmsdiag_get_message_list_failed = TRUE;

    req_pending = WMSDIAG_REQ_STATE_IDLE;
    /*  SET THE SIGNAL WHICH SHOULD UNBLOCK DIAG */
    diagpkt_async_release ();
  }
  /*  If no errors */
  else
  {
    /*  Indicate that the read was ok */
    wmsdiag_get_message_list_failed = FALSE;

    /*  We don't unblock diag here because we should get a cfg event when
        our command actually completes, and this will unblock diag by
    calling wmsdiag_list_update_cb */

    req_pending = WMSDIAG_REQ_STATE_MESSAGE_CB;
  }
}

/*===========================================================================

FUNCTION WMSDIAG_GET_BEARER_PREF_CMD_CB

DESCRIPTION
  Command Callback for Get Domain Pref Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_get_bearer_pref_cmd_cb ( wms_cmd_id_e_type cmd,
                                      void* user_data,
                                      wms_cmd_err_e_type cmd_err )
{
  /* Delayed Response Packet */
  wmsdiag_get_bearer_pref_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  if (cmd != WMS_CMD_CFG_GET_GW_DOMAIN_PREF)
  {
    /* Ignore other Commands */
    return;
  }

  if (get_bearer_req_state != WMSDIAG_REQ_STATE_COMMAND_CB)
  {
    return;
  }

  /* On Success, Wait for CFG Callback for Delayed Response */
  if (cmd_err != WMS_CMD_ERR_NONE)
  {
    /*  Allocate our packet */
    delay_rsp_pkt = (wmsdiag_get_bearer_pref_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                              (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                              (diagpkt_subsys_cmd_code_type)WMSDIAG_GET_BEARER_PREF_CMD,
                              delay_rsp_id,
                              sizeof(wmsdiag_get_bearer_pref_rsp_type) );

    get_bearer_req_state = WMSDIAG_REQ_STATE_IDLE;

    if (delay_rsp_pkt == NULL)
    {
      MSG_ERROR_0("Response packet allocation Failed!");
      return;
    }
    else
    {
      /* Delayed Response */
      diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);

      /* Set Error Status */
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);

      /* Populate Fields */
      delay_rsp_pkt->transaction_ID  = (uint32)user_data;
      delay_rsp_pkt->sms_domain_pref = (uint32)WMS_GW_DOMAIN_PREF_MAX32; /* Invalid */

      /* Send the Response */
      diagpkt_delay_commit(delay_rsp_pkt);
    }
  }
  else
  {
    /* Wait for Configuration Callback */
    get_bearer_req_state = WMSDIAG_REQ_STATE_MESSAGE_CB;
    get_bearer_tid       = (uint32)user_data;
  }
}

/*===========================================================================

FUNCTION WMSDIAG_GET_BEARER_PREF_CFG_CB

DESCRIPTION
  Configuration Callback for Get Domain Pref Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_get_bearer_pref_cfg_cb ( wms_cfg_event_e_type event,
                                      wms_cfg_event_info_s_type *info_ptr)
{
  /* Delayed Response Packet */
  wmsdiag_get_bearer_pref_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /*  If this is not an event pertaining to a GW Domain Pref return */
  if (event != WMS_CFG_EVENT_MS_GW_DOMAIN_PREF)
  {
    return;
  }

  if (get_bearer_req_state != WMSDIAG_REQ_STATE_MESSAGE_CB)
  {
    return;
  }

  MSG_HIGH_1("GW Bearer Pref = %d", info_ptr->ms_gw_domain_pref.domain_pref);

  get_bearer_req_state = WMSDIAG_REQ_STATE_IDLE;

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_get_bearer_pref_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                              (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                              (diagpkt_subsys_cmd_code_type)WMSDIAG_GET_BEARER_PREF_CMD,
                              delay_rsp_id,
                              sizeof(wmsdiag_get_bearer_pref_rsp_type) );

  if (delay_rsp_pkt == NULL)
  {
    MSG_ERROR_0("Response packet allocation Failed!");
    return;
  }
  else
  {
    /* Delayed Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);

    /* Set Error Status */
    diagpkt_subsys_set_status(delay_rsp_pkt, 1);

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID  = get_bearer_tid;
    delay_rsp_pkt->sms_domain_pref = (uint32)info_ptr->ms_gw_domain_pref.domain_pref;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
}

/*===========================================================================

FUNCTION WMSDIAG_SET_BEARER_PREF_CMD_CB

DESCRIPTION
  Command Callback for Set Domain Pref Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_set_bearer_pref_cmd_cb ( wms_cmd_id_e_type cmd,
                                      void* user_data,
                                      wms_cmd_err_e_type cmd_err )
{
  /* Delayed Response Packet */
  wmsdiag_set_bearer_pref_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  if (cmd != WMS_CMD_CFG_SET_GW_DOMAIN_PREF)
  {
    /* Ignore other Commands */
    return;
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_set_bearer_pref_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                            (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                            (diagpkt_subsys_cmd_code_type)WMSDIAG_SET_BEARER_PREF_CMD,
                            delay_rsp_id,
                            sizeof(wmsdiag_set_bearer_pref_rsp_type) );

  if (delay_rsp_pkt == NULL)
  {
    MSG_ERROR_0("Response packet allocation Failed!");
    return;
  }
  else
  {
    /* Delayed Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);

    /* Set the Status */
    if (cmd_err == WMS_CMD_ERR_NONE)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID  = (uint32)user_data;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
}

/*===========================================================================

FUNCTION WMSDIAG_SET_ROUTES_RESULT_LOG

DESCRIPTION
  This function accepts a cfg_event_info_s_type structure and creates a
  wmsdiag_set_routes_log_type log packet that contains all the log
  information in the format specified in the Diag ICD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

void wmsdiag_set_routes_result_log (void)
{
  /*  Log response type */
  wmsdiag_set_routes_log_type* log_rsp;

  /*  Allocate the log packet */
  log_rsp = (wmsdiag_set_routes_log_type*)
    log_alloc (LOG_WMS_SET_ROUTES_C, sizeof (wmsdiag_set_routes_log_type));

  if (log_rsp)
  {
    /*  Set the transaction ID */
    log_rsp -> transaction_ID = (uint8) wmsdiag_transaction_ID;


    /*  Set the status */
    if ( wmsdiag_set_routes_failed )
    {
      log_rsp -> status = TRUE;
    }
    else
    {
      log_rsp -> status = FALSE;
    }

    /*  Commit our packet */
    log_commit (log_rsp);
  }
}


/*===========================================================================

FUNCTION WMSDIAG_DISPATCH_CFG_CB

DESCRIPTION
  Calls the proper cfg callback function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wmsdiag_dispatch_cfg_cb (wms_cfg_event_e_type event,
                             wms_cfg_event_info_s_type *info_ptr)
{
  /*  Callback for a message list update */
  if (event ==  WMS_CFG_EVENT_MESSAGE_LIST)
  {
    wmsdiag_list_update_cb (event, info_ptr);
  }
  /*  Callback for a message set routes */
  else if (event == WMS_CFG_EVENT_ROUTES)
  {
    wmsdiag_set_routes_result_log ();
  }
  else if (event == WMS_CFG_EVENT_MS_GW_DOMAIN_PREF)
  {
    wmsdiag_get_bearer_pref_cfg_cb(event, info_ptr);
  }
}

/*===========================================================================

FUNCTION WMS_DIAG_MAKE_MESSAGE_READ_RESULT_LOG

DESCRIPTION
  This function accepts a client_message_s_type structure and creates a
  wmsdiag_read_log_type log that contains all the log information in
  the format specified in the Diag ICD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wms_diag_make_message_read_result_log (wms_client_message_s_type wmsdiag_message);

/*===========================================================================
===========================================================================*/
uint8 wms_diag_encoding_digit_size
(
  wms_user_data_encoding_e_type  encoding
)
{
  switch( encoding )
  {
    case WMS_ENCODING_ASCII:
    case WMS_ENCODING_IA5:
    case WMS_ENCODING_GSM_7_BIT_DEFAULT:
      return 7;

    case WMS_ENCODING_UNICODE:
      return 16;

    case WMS_ENCODING_OCTET:
    case WMS_ENCODING_IS91EP:
    case WMS_ENCODING_SHIFT_JIS:
    case WMS_ENCODING_KOREAN:
    case WMS_ENCODING_LATIN_HEBREW:
    case WMS_ENCODING_LATIN:
    default:
      return 8;
  }
}

/*===========================================================================
===========================================================================*/
void  wms_diag_print_gw_cmas_message(
  wms_client_ts_data_s_type *client_ts_data_ptr
)
{
   uint16  j;
   MSG_HIGH_8("Recving CMAS with alphabet: %d language: %d geo scope: %d message code: %d page number: %d raw serial number: %d update number: %d total_page: %d",
                                               client_ts_data_ptr->u.gw_cb.cb_page_hdr.cb_dcs.alphabet, client_ts_data_ptr->u.gw_cb.cb_page_hdr.cb_dcs.language,
                                               client_ts_data_ptr->u.gw_cb.cb_page_hdr.geo_scope, client_ts_data_ptr->u.gw_cb.cb_page_hdr.message_code,
                                               client_ts_data_ptr->u.gw_cb.cb_page_hdr.page_number, client_ts_data_ptr->u.gw_cb.cb_page_hdr.raw_serial_number,
                                               client_ts_data_ptr->u.gw_cb.cb_page_hdr.update_number, client_ts_data_ptr->u.gw_cb.cb_page_hdr.total_pages);

   MSG_HIGH_1("Number of chars %d", client_ts_data_ptr->u.gw_cb.user_data.sm_len);
   for (j = 0; j < client_ts_data_ptr->u.gw_cb.user_data.sm_len; j++)
   {
     MSG_HIGH_2( "CMAS Msg[ %d ]: %c",
                 j,
                 client_ts_data_ptr->u.gw_cb.user_data.sm_data[j] );
   }

}

/*===========================================================================
===========================================================================*/
void  wms_diag_print_cmas_message(wms_cmas_msg_s_type *cmas_msg_ptr)
{
    uint8 i;

    MSG_HIGH_2("CMAS version: %d CMAS record count: %d", cmas_msg_ptr->cmae_protocol_version, cmas_msg_ptr->cmae_record_count);

    for (i = 0; i < cmas_msg_ptr->cmae_record_count; i++)
    {

      switch(cmas_msg_ptr->came_records[i].cmae_record_type)
      {
      case WMS_CMAE_RECORD_TYPE_0:
        {
            wms_cmae_record_type_0_s_type * record_type_0 = &(cmas_msg_ptr->came_records[i].u.cmae_record_type_0);
            uint8  char_size;
            uint8  j;
            uint16 bit_pos = 0;
            MSG_HIGH_2("CMAS char_set: %d number of characters: %d", record_type_0->cmae_char_set, record_type_0->number_of_chars);

            char_size = wms_diag_encoding_digit_size(record_type_0->cmae_char_set);
            for (j = 0; j < record_type_0->number_of_chars; j++)
            {
                MSG_HIGH_2( "CMAS Msg[ %d ]: %c",
                            j,
                            b_unpackb(record_type_0->data, bit_pos, char_size) );
                bit_pos += char_size;
            }
        }
        break;

      case WMS_CMAE_RECORD_TYPE_1:
        {
            wms_cmae_record_type_1_s_type * record_type_1 = &(cmas_msg_ptr->came_records[i].u.cmae_record_type_1);
            MSG_HIGH_5("CMAS category: %d certainty: %d response type: %d severity: %d urgency: %d", 
					record_type_1->cmae_category, record_type_1->cmae_certainty,
					record_type_1->cmae_response_type, record_type_1->cmae_severity,
					record_type_1->cmae_urgency );
        }
        break;

      case WMS_CMAE_RECORD_TYPE_2:
        {
            wms_cmae_record_type_2_s_type * record_type_2 = &(cmas_msg_ptr->came_records[i].u.cmae_record_type_2);

            MSG_HIGH_1("CMAS alert handling: %d", record_type_2->cmae_alert_handling);
            MSG_HIGH_8("CMAS expire year: %d expire month: %d expire day: %d expire hour: %d expire minute: %d expire second: %d response id: %d language: %d", 
					record_type_2->cmae_expires.year, record_type_2->cmae_expires.month,
					record_type_2->cmae_expires.day, record_type_2->cmae_expires.hour, 
					record_type_2->cmae_expires.minute, record_type_2->cmae_expires.second,
					record_type_2->cmae_id, record_type_2->cmae_language);

        }
        break;

      default:
        break;

      }
    }

}

/*===========================================================================
===========================================================================*/
#if defined(FEATURE_GWSMS) && defined(FEATURE_GWSMS_BROADCAST)
void wms_diag_print_etws_message(wms_etws_msg_s_type* etws_msg_ptr)
{
   int i;

   if (NULL == etws_msg_ptr)
   {
      return;
   }

   if (etws_msg_ptr->plmn_info_present)
   {
       MSG_HIGH_3( "MCC %d %d %d",
                   etws_msg_ptr->plmn_info.mcc[0],
                   etws_msg_ptr->plmn_info.mcc[1],
                   etws_msg_ptr->plmn_info.mcc[2] );
       MSG_HIGH_1("Number of MNC digits %d", etws_msg_ptr->plmn_info.num_mnc_digits);
       MSG_HIGH_3( "MNC %d %d %d",
                   etws_msg_ptr->plmn_info.mnc[0],
                   etws_msg_ptr->plmn_info.mnc[1],
                   etws_msg_ptr->plmn_info.mnc[2] );
   }

   if (WMS_ETWS_NOTIFICATION_PRIMARY == etws_msg_ptr->type)
   {
      wms_etws_primary_notification_s_type *prim_msg_ptr =
        (wms_etws_primary_notification_s_type *) wms_mem_malloc ( sizeof(wms_etws_primary_notification_s_type) );

      MSG_HIGH_0("Receiving ETWS primary notification");

      if (NULL != prim_msg_ptr)
      {
        if (WMS_OK_S != wms_ts_decode_etws_primary_notification(etws_msg_ptr->data,
                                                  etws_msg_ptr->data_len, prim_msg_ptr))
        {
           MSG_ERROR_0("Decode etws primary message failed");
        }
        else
        {
          MSG_HIGH_6("ETWS with service id: %d alphabet: %d language: %d geo scope: %d message code: %d page number: %d",
            prim_msg_ptr->cb_header_info.cb_srv_id, prim_msg_ptr->cb_header_info.cb_dcs.alphabet,
            prim_msg_ptr->cb_header_info.cb_dcs.language, prim_msg_ptr->cb_header_info.geo_scope,
            prim_msg_ptr->cb_header_info.message_code, prim_msg_ptr->cb_header_info.page_number);
			 
			 MSG_HIGH_6("ETWS with raw serial number: %d update number: %d total_page: %d emergency user alert: %d popup: %d warning type: %d", 
            prim_msg_ptr->cb_header_info.raw_serial_number, prim_msg_ptr->cb_header_info.update_number,
            prim_msg_ptr->cb_header_info.total_pages, prim_msg_ptr->activate_emergency_user_alert,
            prim_msg_ptr->activate_popup, prim_msg_ptr->warning_type);
			 

          if (prim_msg_ptr->security_info_present)
          {

            MSG_HIGH_6("ETWS with year: %d month: %d day: %d hour: %d minute: %d second: %d", 
               prim_msg_ptr->security_info.timestamp.year, prim_msg_ptr->security_info.timestamp.month,
               prim_msg_ptr->security_info.timestamp.day, prim_msg_ptr->security_info.timestamp.hour,
               prim_msg_ptr->security_info.timestamp.minute, prim_msg_ptr->security_info.timestamp.second );
				

            for (i = 0; i < WMS_ETWS_DIGITAL_SIGNATURE_LEN; i++)
            {
               MSG_HIGH_2( "ETWS message digital signatur[ %d ]: %d",
                           i,
                           prim_msg_ptr->security_info.digital_signature[i] );
            }
          }
        }

        wms_mem_free( prim_msg_ptr );
      }
   }
   else if (etws_msg_ptr->type == WMS_ETWS_NOTIFICATION_SECONDARY_UMTS_FORMAT)
   {
      wms_etws_secondary_notification_s_type *sec_msg_ptr = 
        (wms_etws_secondary_notification_s_type *) wms_mem_malloc ( sizeof(wms_etws_secondary_notification_s_type) );
      
      MSG_HIGH_0("Receiving ETWS secondary notification");

      if (NULL != sec_msg_ptr)
      {
        if (WMS_OK_S != wms_ts_decode_etws_secondary_notification(etws_msg_ptr->data, etws_msg_ptr->data_len,
                                                  WMS_ETWS_NOTIFICATION_FORMAT_UMTS, sec_msg_ptr))
        {
           MSG_ERROR_0("Decode ETWS secondary message failed");
        }
        else
        {
          MSG_HIGH_6("ETWS with service id: %d alphabet: %d language: %d geo scope: %d message code: %d page number: %d", 
            sec_msg_ptr->cb_header_info.cb_srv_id, sec_msg_ptr->cb_header_info.cb_dcs.alphabet,
            sec_msg_ptr->cb_header_info.cb_dcs.language,sec_msg_ptr->cb_header_info.geo_scope,
            sec_msg_ptr->cb_header_info.message_code, sec_msg_ptr->cb_header_info.page_number);
			 
			 MSG_HIGH_6("ETWS with raw serial number: %d update number: %d total_page: %d emergency user alert: %d popup: %d cb_data length: %d",
            sec_msg_ptr->cb_header_info.raw_serial_number, sec_msg_ptr->cb_header_info.update_number,
            sec_msg_ptr->cb_header_info.total_pages, sec_msg_ptr->activate_emergency_user_alert,
            sec_msg_ptr->activate_popup, sec_msg_ptr->cb_data_len );

          for (i = 0; i < (int) sec_msg_ptr->cb_data_len; i++)
          {
             MSG_HIGH_2( "ETWS Msg[ %d ]: %c",
                         i,
                         sec_msg_ptr->cb_data[i] );
          }
        }

        wms_mem_free( sec_msg_ptr );
      }
   }
}
#endif
/*===========================================================================

FUNCTION WMSDIAG_DISPATCH_MSG_CB

DESCRIPTION
  Calls the proper msg related callback function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wmsdiag_dispatch_msg_cb ( wms_msg_event_e_type event,
                               wms_msg_event_info_s_type *info_ptr,
                               boolean *shared )
{
  (void)shared; // parameter not used

  /*  Callback for a message read */
  if ((event ==  WMS_MSG_EVENT_READ) && (wmsdiag_initiated_read == TRUE))
  {
    /* reset the global flag */
    wmsdiag_initiated_read = FALSE;

    wms_diag_make_message_read_result_log (info_ptr -> status_info.message);
  }
  else if( event == WMS_MSG_EVENT_RECEIVED_MESSAGE )
  {
    wms_service_e_type        service;
    
    wms_client_ts_data_s_type *client_ts_data_ptr = 
      (wms_client_ts_data_s_type *) wms_mem_malloc ( sizeof(wms_client_ts_data_s_type) );

    if (NULL != client_ts_data_ptr)
    {
      service = info_ptr->mt_message_info.message.u.cdma_message.service;
      if (WMS_MESSAGE_MODE_CDMA == info_ptr->mt_message_info.message.msg_hdr.message_mode &&
          WMS_SRV_CMAS_PRESIDENTIAL_ALERT <= service && service <= WMS_SRV_CMAS_TEST_MSG)

      {
        MSG_HIGH_1("Receiving CMAS message with category: %d", service);
        if (WMS_OK_S == wms_ts_decode( &info_ptr->mt_message_info.message.u.cdma_message.raw_ts,
                                       client_ts_data_ptr ))
        {
          wms_cmas_msg_s_type *cmas_msg_ptr = 
            (wms_cmas_msg_s_type *) wms_mem_malloc ( sizeof(wms_cmas_msg_s_type) );

          if (NULL != cmas_msg_ptr)
          {
            (void)wms_ts_decode_cmas_msg( client_ts_data_ptr->u.cdma.user_data.data,
                                          client_ts_data_ptr->u.cdma.user_data.data_len, cmas_msg_ptr);
            wms_diag_print_cmas_message(cmas_msg_ptr);

            wms_mem_free( cmas_msg_ptr );
          }
        }
        else
        {
          MSG_HIGH_0("Decoding failed for CDMA MT SMS");
        }
      }
      else if (WMS_MESSAGE_MODE_GW == info_ptr->mt_message_info.message.msg_hdr.message_mode)
      {
         if ( WMS_OK_S ==
              wms_ts_decode( &info_ptr->mt_message_info.message.u.gw_message.raw_ts_data,
                             client_ts_data_ptr ) )
         {
           if(client_ts_data_ptr->format== WMS_FORMAT_GW_CB &&
              WMS_GW_CB_SRV_ID_CMAS_MSGID1 <= client_ts_data_ptr->u.gw_cb.cb_page_hdr.cb_srv_id &&
              client_ts_data_ptr->u.gw_cb.cb_page_hdr.cb_srv_id <= WMS_GW_CB_SRV_ID_CMAS_MSGID13)
           {
              MSG_HIGH_1("Receiving CMAS message with message id: %d", client_ts_data_ptr->u.gw_cb.cb_page_hdr.cb_srv_id);
              wms_diag_print_gw_cmas_message( client_ts_data_ptr );
           }
         }
         else
         {
           MSG_HIGH_0("Decoding failed for GW MT SMS");
         }
      }
      else
      {
        MSG_HIGH_1("Received SMS data of not the expected mode type %d",
                  info_ptr->mt_message_info.message.msg_hdr.message_mode );
      }
    }

    WMS_MEM_FREEIF(client_ts_data_ptr);
  }
  else if (event == WMS_MSG_EVENT_ETWS_NOTIFICATION)
  {
     MSG_HIGH_0("Receiving ETWS notification");
#if defined(FEATURE_GWSMS) && defined(FEATURE_GWSMS_BROADCAST)
     wms_diag_print_etws_message(&info_ptr->etws_msg_info);
#endif
  }
}

/*===========================================================================

FUNCTION WMSDIAG_BC_ADD_DELETE_CMD_CB

DESCRIPTION
  Command Callback for Add Delete Service ID Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_bc_add_delete_cmd_cb ( wms_cmd_id_e_type cmd,
                                void* user_data,
                                wms_cmd_err_e_type cmd_err )
{
  /* Delayed Response Packet */
  wmsdiag_broadcast_add_delete_serviceid_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  if ( (cmd != WMS_CMD_BC_MM_ADD_SRV)
       && (cmd != WMS_CMD_BC_MM_DELETE_SRV) )
  {
    /* Ignore other Commands */
    return;
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_broadcast_add_delete_serviceid_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD,
                             delay_rsp_id,
                             sizeof(wmsdiag_broadcast_add_delete_serviceid_rsp_type) );

  if (delay_rsp_pkt == NULL)
  {
    MSG_ERROR_0("Response packet allocation Failed!");
    return;
  }
  else
  {
    /* Delayed Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);

    /* Set the Status */
    if (cmd_err == WMS_CMD_ERR_NONE)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = (uint8)(((uint32)user_data)&0xFF);
    delay_rsp_pkt->reserved_byte1 = 0;
    delay_rsp_pkt->reserved_byte2 = 0;
    delay_rsp_pkt->reserved_byte3 = 0;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
}

/*===========================================================================

FUNCTION WMSDIAG_BC_DELETE_ALL_CMD_CB

DESCRIPTION
  Command Callback for Delete All Service IDs Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_bc_delete_all_cmd_cb ( wms_cmd_id_e_type cmd,
                                void* user_data,
                                wms_cmd_err_e_type cmd_err )
{
  /* Delayed Response Packet */
  wmsdiag_broadcast_delete_all_services_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  if (cmd != WMS_CMD_BC_MM_DELETE_ALL_SERVICES)
  {
    /* Ignore other Commands */
    return;
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_broadcast_delete_all_services_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD,
                             delay_rsp_id,
                             sizeof(wmsdiag_broadcast_delete_all_services_rsp_type) );

  if (delay_rsp_pkt == NULL)
  {
    MSG_ERROR_0("Response packet allocation Failed!");
    return;
  }
  else
  {
    /* Delayed Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);

    /* Set the Status */
    if (cmd_err == WMS_CMD_ERR_NONE)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = (uint8)(((uint32)user_data)&0xFF);
    delay_rsp_pkt->reserved_byte1 = 0;
    delay_rsp_pkt->reserved_byte2 = 0;
    delay_rsp_pkt->reserved_byte3 = 0;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
}

/*===========================================================================

FUNCTION WMSDIAG_DISPATCH_BC_MM_CB

DESCRIPTION
  Calls the proper multimode broadcast related callback function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wmsdiag_dispatch_bc_mm_cb ( wms_bc_mm_event_e_type event,
                                 wms_bc_mm_event_info_s_type *info_ptr )

{
  (void)event; // parameter not used
  (void)info_ptr; // parameter not used
  /* Currently, we dont use this function */
}


/*===========================================================================

FUNCTION WMSDIAG_CLIENT_INIT

DESCRIPTION
  This procedure performs the necessary API calls to establish a client ID
  for our DM WMS session and map callback functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the static variable referencing our client ID.

===========================================================================*/
void wmsdiag_client_init (void)
{
  /*  This will initialize our client ID properly */
  if (wms_client_init (WMS_CLIENT_TYPE_DIAG, &wmsdiag_client_id) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR_0("WMSDIAG: Error in wms_client_init");
  }

  /*  Activate our client ID */
  if (wms_client_activate (wmsdiag_client_id) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR_0("WMSDIAG: Error in wms_client_activate");
  }

  /*  Register a callback function for the cfg events */
  /*  The registered fcn is a dispatcher that calls the appropriate fcn given
      an event */
  if (wms_client_reg_cfg_cb (wmsdiag_client_id, wmsdiag_dispatch_cfg_cb) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR_0("WMSDIAG: Error in wms_client_reg_cfg_cb");
  }

  /*  Register a callback function for the msg events */
  /*  The registered fcn is a dispatcher that calls the appropriate fcn given
      an event.  */
  if (wms_client_reg_msg_cb (wmsdiag_client_id, wmsdiag_dispatch_msg_cb) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR_0("WMSDIAG: Error in wms_client_reg_msg_cb");
  }

  /*  Register a callback function for the Broadcast events */
  /*  The registered fcn is a dispatcher that calls the appropriate fcn given
      an event.  */
  if (wms_client_reg_bc_mm_cb (wmsdiag_client_id, wmsdiag_dispatch_bc_mm_cb) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR_0("WMSDIAG: Error in wms_client_reg_bc_mm_cb");
  }

  return;
}

/*===========================================================================

FUNCTION WMSDIAG_SEND_WRITE

DESCRIPTION
  This function  attempts to send a message with the given properties or write
  a message, depending on the parameters passed in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * wmsdiag_send_write (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_send_write_rsp_type *rsp_ptr            = NULL;
  wms_client_message_s_type   *client_message_ptr = NULL;

  /*  Cast the request pointer to proper type */
  wmsdiag_send_write_req_type *req_ptr  =
      (wmsdiag_send_write_req_type *) req_pkt_ptr;

  /*  Flag to check message mode */
  boolean is_CDSMS = TRUE;

#ifdef FEATURE_CDSMS
  static wms_cdma_message_s_type      cdma_message;
  static wms_address_s_type           cdma_message_addr;
  static wms_subaddress_s_type        cdma_message_subaddr;
  static wms_raw_ts_data_s_type       cdma_raw_data;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
  static wms_gw_message_s_type        gw_message;
  static wms_address_s_type           gw_message_addr;
  static wms_raw_ts_data_s_type       gw_raw_data;
#endif /* FEATURE_GWSMS */

  /*  Loop Counter */
  uint32 i;

  uint8 A = 0;  /*  Number of digits in the address */
  uint8 S = 0;  /*  Number of digits in the subaddress */
  uint32 D = 0;  /*  Length of the data in bytes */

#ifdef FEATURE_CDSMS
  memset(&cdma_message, 0, sizeof(cdma_message));
  memset(&cdma_message_addr, 0, sizeof(cdma_message_addr));
  memset(&cdma_message_subaddr, 0, sizeof(cdma_message_subaddr));
  memset(&cdma_raw_data, 0, sizeof(cdma_raw_data));
#endif /* FEATURE_CDSMS */

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> send_mode          >= (uint8)WMSDIAG_SEND_MODE_MAX ||
       req_ptr -> write_mode         >= (uint8)WMSDIAG_WRITE_MODE_MAX ||
       req_ptr -> msg_mode           >= ((uint8)WMSDIAG_MSG_MODE_MAX) - 1 ||
       req_ptr -> tag_type           >= (uint32)WMSDIAG_TAG_MAX ||
       req_ptr -> storage_type       >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> addr_digit_mode    >= (uint8)WMSDIAG_ADDR_DIGIT_MODE_MAX ||
       req_ptr -> addr_num_mode      >= (uint8)WMSDIAG_ADDR_NUM_MODE_MAX ||
       req_ptr -> data_format        >= (uint8)WMSDIAG_DATA_FORMAT_MAX ||
       req_ptr -> addr_num_digits    >  WMSDIAG_ADDRESS_MAX ||
       req_ptr -> subaddr_num_digits >  WMSDIAG_SUBADDRESS_MAX ||
       req_ptr -> user_data_length   >  WMSDIAG_MAX_LEN )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  Initialize the message mode flag */
  if (WMSDIAG_MSG_MODE_VALS [req_ptr -> msg_mode] == WMS_MESSAGE_MODE_CDMA)
  {
    is_CDSMS = TRUE;
  }
  else
  {
    is_CDSMS = FALSE;
  }

  #ifdef FEATURE_GWSMS
  if (req_ptr -> tpdu_type >= (uint8)WMSDIAG_TPDU_MAX && !is_CDSMS)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }
  #endif

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Initialize counters for the number of address/subaddress digits as well as the data length */
  A = (uint8)req_ptr -> addr_num_digits;
  S = (uint8)req_ptr -> subaddr_num_digits;
  D = req_ptr -> user_data_length;

  if( A+S+D > WMSDIAG_DEFAULT_DATA_BUF_MAX )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  client_message_ptr = wms_mem_malloc ( sizeof(wms_client_message_s_type) );
  if (NULL != client_message_ptr)
  {

    /*  Allocate our packet */
    rsp_ptr = (wmsdiag_send_write_rsp_type*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                                  WMSDIAG_SEND_WRITE_CMD,
                                                                  sizeof (wmsdiag_send_write_rsp_type));


    if (rsp_ptr)
    {
      /*  Set the message header variables */
      msg_hdr.message_mode = WMSDIAG_MSG_MODE_VALS [req_ptr -> msg_mode];
      msg_hdr.tag = WMSDIAG_MSG_TAG_VALS [req_ptr -> tag_type];
      msg_hdr.mem_store = WMSDIAG_MEM_STORE_VALS [req_ptr -> storage_type];
      msg_hdr.index = req_ptr -> index;

      /*  Set the CDMA message variables */
    #ifdef FEATURE_CDSMS
      if (is_CDSMS)
      {

        cdma_message.is_mo = req_ptr -> is_MO;
        cdma_message.teleservice = (wms_teleservice_e_type) req_ptr -> teleservice_type;

        /*  Set the address information */
        cdma_message_addr.digit_mode = WMSDIAG_ADDR_DIGIT_MODE_VALS [req_ptr -> addr_digit_mode];
        cdma_message_addr.number_mode = WMSDIAG_ADDR_NUM_MODE_VALS [req_ptr -> addr_num_mode];
        cdma_message_addr.number_type = (wms_number_type_e_type) req_ptr -> addr_num_type;
        cdma_message_addr.number_plan = (wms_number_plan_e_type) req_ptr -> addr_num_plan_type;
        cdma_message_addr.number_of_digits = A;
        for (i = 0; i < A ; i++)
        {
          cdma_message_addr.digits [i] = req_ptr -> data [i];
        }

        cdma_message.address = cdma_message_addr;

        /*  Set the subaddress information */
        cdma_message_subaddr.type = (wms_subaddress_type_e_type) req_ptr -> subaddr_type;
        cdma_message_subaddr.odd = req_ptr -> subaddr_odd_or_even;
        cdma_message_subaddr.number_of_digits = S;

        for (i = 0; i < S; i++)
        {
          cdma_message_subaddr.digits[i] = req_ptr->data[i + A];
        }

        cdma_message.subaddress = cdma_message_subaddr;

        cdma_message.is_tl_ack_requested = req_ptr -> tl_ack_needed;
        cdma_message.is_service_present = req_ptr -> service_present;
        cdma_message.service = (wms_service_e_type) req_ptr -> service_type;

        cdma_raw_data.format = WMSDIAG_DATA_FORMAT_VALS [req_ptr -> data_format];
        cdma_raw_data.len = D;

        for (i = 0; i < D; i++)
        {
          cdma_raw_data.data [i] = req_ptr -> data [i + A + S];
        }

        cdma_message.raw_ts = cdma_raw_data;

        client_message_ptr->msg_hdr = msg_hdr;
        client_message_ptr->u.cdma_message = cdma_message;

        /*  Send out our packet now */
        diagpkt_commit (rsp_ptr);

        /*  If they want to just send it over the air */
        if (req_ptr -> write_message == 0)
        {
          (void)wms_msg_ms_send (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                    NULL, (void *)(uint32)req_ptr -> transaction_ID,
                    WMSDIAG_SEND_MODE_VALS [req_ptr -> send_mode],
                    client_message_ptr);
        }
        /*  If they want to write it to memory */
        else
        {
          (void)wms_msg_ms_write (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                       NULL, (void *)(uint32)req_ptr -> transaction_ID,
                       WMSDIAG_WRITE_MODE_VALS [req_ptr -> write_mode],
                       client_message_ptr);
        }
      } /* if (is_CDSMS) */

    #endif /* FEATURE_CDSMS */

    #ifdef FEATURE_GWSMS
      if (!is_CDSMS)
      {

        gw_message.is_broadcast = req_ptr -> is_broadcast;

        /*  Set the address information */
        gw_message_addr.digit_mode  = WMSDIAG_ADDR_DIGIT_MODE_VALS [req_ptr -> addr_digit_mode];
        gw_message_addr.number_mode = WMSDIAG_ADDR_NUM_MODE_VALS [req_ptr -> addr_num_mode];
        gw_message_addr.number_type = (wms_number_type_e_type) req_ptr -> addr_num_type;
        gw_message_addr.number_plan = (wms_number_plan_e_type) req_ptr -> addr_num_plan_type;
        gw_message_addr.number_of_digits = A;
        for (i = 0; i < A ; i++)
        {
          gw_message_addr.digits [i] = req_ptr -> data [i];
        }

        gw_message.sc_address = gw_message_addr;

        gw_raw_data.format = WMSDIAG_DATA_FORMAT_VALS [req_ptr -> data_format];
        gw_raw_data.len = D;
        gw_raw_data.tpdu_type = WMSDIAG_GW_TPDU_VALS [req_ptr -> tpdu_type];

        for (i = 0; i < D; i++)
        {
          /* for GW, skip the subaddress in data */
          gw_raw_data.data [i] = req_ptr -> data [i + A + S];
        }

        gw_message.raw_ts_data = gw_raw_data;

        client_message_ptr->msg_hdr = msg_hdr;
        client_message_ptr->u.gw_message = gw_message;

        /*  Send out our packet now */
        diagpkt_commit (rsp_ptr);

        /*  If they want to just send it over the air */
        if (req_ptr -> write_message == 0)
        {
          (void)wms_msg_ms_send (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                    NULL, (void *)(uint32)req_ptr -> transaction_ID,
                    WMSDIAG_SEND_MODE_VALS [req_ptr -> send_mode],
                    client_message_ptr);

        }
        /*  If they want to write it to memory */
        else
        {
          (void)wms_msg_ms_write (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                    NULL, (void *)(uint32)req_ptr -> transaction_ID,
                    WMSDIAG_WRITE_MODE_VALS [req_ptr -> write_mode],
                    client_message_ptr);
        }
      } /*  if (!is_CDSMS) */

    #endif /* FEATURE_GWSMS */
    }

    wms_mem_free( client_message_ptr );
  }
  
  return NULL;
}

/*===========================================================================

FUNCTION WMSDIAG_CLEAR

DESCRIPTION
  This function  attempts to clear a message with the given index from
  the specified memory store.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_clear (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response Pointer */
  wmsdiag_clear_rsp_type *rsp_ptr  = NULL;

  /*  Cast request pointer to proper type */
  wmsdiag_clear_req_type *req_ptr  =
      (wmsdiag_clear_req_type *) req_pkt_ptr;

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= (uint32)WMSDIAG_MEM_STORE_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_clear_rsp_type*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                           WMSDIAG_CLEAR_CMD,
                                                           sizeof (wmsdiag_clear_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);


    /*  Call the WMS API function to clear the message */
    (void)wms_msg_ms_delete (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
             NULL, (void *)(uint32)req_ptr -> transaction_ID,
             WMSDIAG_MEM_STORE_VALS [req_ptr -> storage_type],
             req_ptr -> index);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMSDIAG_CLEAR_ALL

DESCRIPTION
  This function attempts to clear all messages from the specified memory
  store.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_clear_all (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response Pointer */
  wmsdiag_clear_all_rsp_type *rsp_ptr  = NULL;

  /*  Cast copy of request pointer to proper type */
  wmsdiag_clear_all_req_type *req_ptr  =
      (wmsdiag_clear_all_req_type *) req_pkt_ptr;

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= (uint32)WMSDIAG_MEM_STORE_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_clear_all_rsp_type*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                               WMSDIAG_CLEAR_ALL_CMD,
                                                               sizeof (wmsdiag_clear_all_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);

    /*  Call the WMS API function to clear the messages in the given memory store */
    (void)wms_msg_ms_delete_all (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                  NULL, (void *)(uint32)req_ptr -> transaction_ID,
                  WMSDIAG_MEM_STORE_VALS [req_ptr -> storage_type],
                  WMSDIAG_MSG_TAG_VALS [0]);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMSDIAG_GET_MESSAGE_LIST

DESCRIPTION
  This function returns all messages from the specified memory
  store.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  Will cause a diag_block until DIAG_ASYNC_BLOCK_SIG is set.  This will be done
  by either the CFG callback function or the CMD callback.

===========================================================================*/

PACKED void * wmsdiag_get_message_list (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_get_message_list_rsp_type *rsp_ptr  = NULL;

  /*  Cast a copy of the request pointer to the proper type */
  wmsdiag_get_message_list_req_type *req_ptr  =
      (wmsdiag_get_message_list_req_type *) req_pkt_ptr;

  /*  Variable to represent the final size of our packet */
  uint32 packet_size;

  /*  Loop counter */
  uint32 i;

  /* Drop re-entrant function calls if a request is still pending */
  if (req_pending != WMSDIAG_REQ_STATE_IDLE)
  {
    return rsp_ptr;
  }

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= (uint32)WMSDIAG_MEM_STORE_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  wmsdiag_req_storage = (wms_memory_store_e_type) req_ptr -> storage_type;
  MSG_HIGH_1("wmsdiag_req_storage = %d", wmsdiag_req_storage);

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  req_pending = WMSDIAG_REQ_STATE_COMMAND_CB;

  /*  Call the WMS API function to get the message list */
  (void)wms_cfg_ms_get_message_list (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                            wmsdiag_get_message_list_cmd_cb,
                            (void *)(uint32)req_ptr -> transaction_ID,
                            (wms_memory_store_e_type)req_ptr -> storage_type, WMSDIAG_MSG_TAG_VALS [0]);

  /*  ASYNC WAIT ON A SIGNAL */
  diagpkt_async_block ();

  MSG_HIGH_0("async block has been released");
  /*  Once the signal is set, we can update the response pointer parameters */
  /*  Make a flag and set it if the cmd callback showed that there was an error, if the flag is set, err pkt */

  /*  Allocate our packet */
  if (wmsdiag_get_message_list_failed)
  {
  /*  If the cmd failed, we have no messages to hold, so we don't allocate space for them */
    packet_size = FPOS (wmsdiag_get_message_list_rsp_type, messages);
  }
  else
  {
  /*  If the cmd succeeded, allocate space for the packet + messages */
    packet_size = FPOS (wmsdiag_get_message_list_rsp_type, messages) +
                sizeof (WMSDIAG_MessageListEntryType) * (wmsdiag_list.num_entries );
  }

  rsp_ptr = (wmsdiag_get_message_list_rsp_type*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                                      WMSDIAG_GET_MESSAGE_LIST_CMD,
                                                                      packet_size);

  if (rsp_ptr)
  {
    /*  Set up the parameters */
    rsp_ptr->num_entries = wmsdiag_list.num_entries;
    rsp_ptr->storage_type = wmsdiag_list.storage_type;
    rsp_ptr->voice_mail_index = wmsdiag_list.voice_mail_index;
    rsp_ptr->voice_mail_active = wmsdiag_list.voice_mail_active;

    /*  If the get message list command failed, set the status */
    if (wmsdiag_get_message_list_failed)
    {
      rsp_ptr -> status = 1;
    }
    /*  If not, set the status and the message parameters */
    else
    {
      rsp_ptr -> status = 0;


      for (i = 0; i < (wmsdiag_list.num_entries); i++)
      {
        rsp_ptr->messages[i].tag_type = wmsdiag_list.messages[i].tag_type;
        rsp_ptr->messages[i].index = wmsdiag_list.messages[i].index;
      }
    }
  }

  return (rsp_ptr);

}


/*===========================================================================

FUNCTION WMSDIAG_SET_MESSAGE_TAG

DESCRIPTION
  This function alters the tag of the selected message.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_set_message_tag (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response Pointer */
  wmsdiag_set_message_tag_rsp_type *rsp_ptr  = NULL;

  /*  Copy of request pointer cast to proper type */
  wmsdiag_set_message_tag_req_type *req_ptr  =
      (wmsdiag_set_message_tag_req_type *) req_pkt_ptr;

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> tag_type     >= (uint32)WMSDIAG_TAG_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_set_message_tag_rsp_type*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                                     WMSDIAG_SET_MESSAGE_TAG_CMD,
                                                                     sizeof (wmsdiag_set_message_tag_rsp_type));
  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);

    /*  Call the WMS API function to alter the tag */
    (void)wms_msg_ms_modify_tag (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                  NULL, (void *)(uint32)req_ptr -> transaction_ID,
                  WMSDIAG_MEM_STORE_VALS [req_ptr -> storage_type],
                  req_ptr -> index,
                  WMSDIAG_MSG_TAG_VALS [req_ptr -> tag_type]);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMSDIAG_READ

DESCRIPTION
  This function allows the user to read a message.  The response only
  indicates that a read was attempted.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_read (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_read_rsp_type *rsp_ptr  = NULL;

  /*  Copy of request pointer cast to proper type */
  wmsdiag_read_req_type *req_ptr  =
      (wmsdiag_read_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  /* WMS DIAG is initiating this read command */
  wmsdiag_initiated_read = TRUE;

  wmsdiag_transaction_ID = req_ptr->transaction_ID;

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= (uint32)WMSDIAG_MEM_STORE_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_read_rsp_type*) diagpkt_subsys_alloc ((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                           WMSDIAG_READ_CMD,
                                                           sizeof (wmsdiag_read_rsp_type));

  if (rsp_ptr)
  {
    /*  Call the WMS API function to read a message */
    st = wms_msg_ms_read (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                  NULL, (void *)(uint32)req_ptr -> transaction_ID,
                  (wms_memory_store_e_type)req_ptr -> storage_type, req_ptr -> index);

    if (st != WMS_OK_S)
    {
      rsp_ptr -> status = 1;
      wmsdiag_read_failed = TRUE;
      MSG_ERROR_0("DTC command failed");
    }
    else
    {
      rsp_ptr -> status = 0;
      wmsdiag_read_failed = FALSE;
    }

    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMS_DIAG_MAKE_MESSAGE_READ_RESULT_LOG

DESCRIPTION
  This function accepts a client_message_s_type structure and creates a
  wmsdiag_read_log_type log that contains all the log information in
  the format specified in the Diag ICD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wms_diag_make_message_read_result_log (wms_client_message_s_type wmsdiag_message )
{

  /*  Log response type */
  wmsdiag_read_log_type* log_rsp;

  uint32 A = 0;  /*  Number of digits in the address */
  uint32 S = 0;  /*  Number of digits in the subaddress */
  uint32 D = 0;  /*  Length of the data in bytes */
  uint32 i;  /*  Loop counter */
  uint32 log_size; /*  Eventually set to size of our log */
  uint32 switch_val = 0; /*  Temp value used for switch statements */
  boolean is_CDSMS = TRUE; /*  Flag for message mode type */

  /*
    Allocate the log packet, it is too large now, but we will shorten
  it later when we know exactly how large the payload is.
  */
  log_rsp = (wmsdiag_read_log_type*)
    log_alloc (LOG_WMS_READ_C, sizeof (wmsdiag_read_log_type));

  if (log_rsp)
  {
    /*  Set the transaction ID */
    log_rsp -> transaction_ID = (uint8) wmsdiag_transaction_ID;

    /*  Set the status */
    if ( wmsdiag_read_failed )
    {
      log_rsp -> status = TRUE;
    }
    else
    {
      log_rsp -> status = FALSE;
    }

    /*  Set the message mode */
    switch (wmsdiag_message.msg_hdr.message_mode)
    {
    case WMS_MESSAGE_MODE_CDMA:
      log_rsp -> msg_mode = (uint8)WMSDIAG_MSG_MODE_CDMA;
      is_CDSMS = TRUE;
      break;

    case WMS_MESSAGE_MODE_GW:
      log_rsp -> msg_mode = (uint8)WMSDIAG_MSG_MODE_GW;
      is_CDSMS = FALSE;
      break;

    default:
      log_rsp -> msg_mode = (uint8)WMSDIAG_MSG_MODE_INVALID;
      break;
    }

    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      /*  Set the counters showing the size of the address, subaddress and data */
      A = wmsdiag_message.u.cdma_message.address.number_of_digits;
      S = wmsdiag_message.u.cdma_message.subaddress.number_of_digits;
      D = wmsdiag_message.u.cdma_message.raw_ts.len;
    }
    #endif
    #ifdef FEATURE_GWSMS
    if (!is_CDSMS)
    {
      /*  Set the counters showing the size of the address, subaddress and data */
      A = wmsdiag_message.u.gw_message.sc_address.number_of_digits;
      S = 0;
      D = wmsdiag_message.u.gw_message.raw_ts_data.len;
    }
    #endif

    /*  Set the tag type */
    log_rsp->tag_type = (uint32)wmsdiag_map_tag_type(wmsdiag_message.msg_hdr.tag);

    /*  Set the memory store type */
    switch (wmsdiag_message.msg_hdr.mem_store)
    {
    case WMS_MEMORY_STORE_NONE:
      log_rsp -> storage_type = (uint32)WMSDIAG_MEM_STORE_NONE;
      break;

    case WMS_MEMORY_STORE_RAM_CDMA:
      log_rsp -> storage_type = (uint32)WMSDIAG_MEM_STORE_RAM_CDMA;
      break;

    case WMS_MEMORY_STORE_RUIM:
      log_rsp -> storage_type = (uint32)WMSDIAG_MEM_STORE_RUIM;
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
      log_rsp -> storage_type = (uint32)WMSDIAG_MEM_STORE_NV_CDMA;
      break;

    case WMS_MEMORY_STORE_RAM_GW:
      log_rsp -> storage_type = (uint32)WMSDIAG_MEM_STORE_RAM_GW;
      break;

    case WMS_MEMORY_STORE_SIM:
      log_rsp -> storage_type = (uint32)WMSDIAG_MEM_STORE_SIM;
      break;

    case WMS_MEMORY_STORE_NV_GW:
      log_rsp -> storage_type = (uint32)WMSDIAG_MEM_STORE_NV_GW;
      break;

    default:
      log_rsp -> storage_type = (uint32)WMSDIAG_MEM_STORE_INVALID;
      break;
    }

    /*  Set the index */
    log_rsp -> index = (uint32)wmsdiag_message.msg_hdr.index;

    #ifdef FEATURE_CDSMS

    if (is_CDSMS)
    {
      /*  Set whether its MO or MT */
      log_rsp -> is_MO = (uint8)wmsdiag_message.u.cdma_message.is_mo;

      /*  Set the teleservice type */
      log_rsp -> teleservice_type = (uint32)wmsdiag_message.u.cdma_message.teleservice;
    }

    #endif /* FEATURE CDSMS */

    /*  Set the address digit mode */
    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      switch_val = (uint32)(wmsdiag_message.u.cdma_message.address.digit_mode);
    }
    #endif
    #ifdef FEATURE_GWSMS
    if (!is_CDSMS)
    {
      switch_val = (uint32)(wmsdiag_message.u.gw_message.sc_address.digit_mode);
    }
    #endif

    switch (switch_val)
    {
    case WMS_DIGIT_MODE_4_BIT:
      log_rsp -> addr_digit_mode = (uint8)WMSDIAG_ADDR_DIGIT_MODE_4_BIT;
      break;

    case WMS_DIGIT_MODE_8_BIT:
      log_rsp -> addr_digit_mode = (uint8)WMSDIAG_ADDR_DIGIT_MODE_8_BIT;
      break;

    default:
      log_rsp -> addr_digit_mode = (uint8)WMSDIAG_ADDR_DIGIT_MODE_INVALID;
      break;
    }

    /*  Set the address number mode */
    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      switch_val = (uint32)(wmsdiag_message.u.cdma_message.address.number_mode);
    }
    #endif
    #ifdef FEATURE_GWSMS
    if (!is_CDSMS)
    {
      switch_val = (uint32)(wmsdiag_message.u.gw_message.sc_address.number_mode);
    }
    #endif

    switch (switch_val)
    {
    case WMS_NUMBER_MODE_NONE_DATA_NETWORK:
      log_rsp -> addr_num_mode = (uint8)WMSDIAG_ADDR_NUM_MODE_NOT_DATA_NTWK;
      break;

    case WMS_NUMBER_MODE_DATA_NETWORK:
      log_rsp -> addr_num_mode = (uint8)WMSDIAG_ADDR_NUM_MODE_DATA_NTWK;
      break;

    default:
      log_rsp -> addr_num_mode = (uint8)WMSDIAG_ADDR_NUM_MODE_INVALID;
      break;
    }

    /*  Set the address number type */
    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      log_rsp -> addr_num_type = (uint8)wmsdiag_message.u.cdma_message.address.number_type;
    }
    #endif
    #ifdef FEATURE_GWSMS
    if(!is_CDSMS)
    {
      log_rsp -> addr_num_type = (uint8)wmsdiag_message.u.gw_message.sc_address.number_type;
    }
    #endif

    /*  Set the address number plan */
    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      log_rsp -> addr_num_plan_type = (uint8)wmsdiag_message.u.cdma_message.address.number_plan;
    }
    #endif
    #ifdef FEATURE_GWSMS
    if(!is_CDSMS)
    {
      log_rsp -> addr_num_plan_type = (uint8)wmsdiag_message.u.gw_message.sc_address.number_plan;
    }
    #endif

    /*  Set the address number of digits */
    log_rsp -> addr_num_digits = (uint16)A;

    /*  Set the address digits */
    for (i = 0; i < A; i++)
    {
    #ifdef FEATURE_CDSMS
    if(is_CDSMS)
    {
      log_rsp -> data [i] = wmsdiag_message.u.cdma_message.address.digits [i];
    }
    #endif
    #ifdef FEATURE_GWSMS
    if(!is_CDSMS)
    {
      log_rsp -> data [i] = wmsdiag_message.u.gw_message.sc_address.digits [i];
    }
    #endif
    }

    #ifdef FEATURE_CDSMS

    if (is_CDSMS)
    {

      /*  Set the subaddress type */
      log_rsp -> subaddr_type = (uint8)wmsdiag_message.u.cdma_message.subaddress.type;

      /*  Set whether an odd or even number of digits in subaddr */
      log_rsp -> subaddr_odd_or_even = (uint8)wmsdiag_message.u.cdma_message.subaddress.odd;

      /*  Set the subaddress number of digits */
      log_rsp -> subaddr_num_digits = (uint16)S;

      /*  Set the subaddress digits */
      for (i = 0; i < S; i++)
      {
        log_rsp->data[i + A] = (uint8)wmsdiag_message.u.cdma_message.subaddress.digits [i];
      }

      /*  Set if ACK needed */
      log_rsp -> tl_ack_needed = (uint8)wmsdiag_message.u.cdma_message.is_tl_ack_requested;

      /*  Set if service is present or not */
      log_rsp -> service_present = (uint8)wmsdiag_message.u.cdma_message.is_service_present;

      /*  Set the service type */
      log_rsp -> service_type = (uint32)wmsdiag_message.u.cdma_message.service;

    }

    #endif /*  FEATURE_CDSMS */

    /*  Set the data format */
    #ifdef FEATURE_CDSMS
    switch_val = (uint32)(wmsdiag_message.u.cdma_message.raw_ts.format);
    #endif
    #ifdef FEATURE_GWSMS
    switch_val = (uint32)(wmsdiag_message.u.gw_message.raw_ts_data.format);
    #endif
    switch (switch_val)
    {
    case WMS_FORMAT_CDMA:
      log_rsp -> data_format = (uint8)WMSDIAG_DATA_FORMAT_CDMA;
      break;

    case WMS_FORMAT_ANALOG_CLI:
      log_rsp -> data_format = (uint8)WMSDIAG_DATA_FORMAT_ANALOG_CLI;
      break;

    case WMS_FORMAT_ANALOG_VOICE_MAIL:
      log_rsp -> data_format = (uint8)WMSDIAG_DATA_FORMAT_ANALOG_VOICE_MAIL;
      break;

    case WMS_FORMAT_ANALOG_SMS:
      log_rsp -> data_format = (uint8)WMSDIAG_DATA_FORMAT_ANALOG_SMS;
      break;

    case WMS_FORMAT_ANALOG_AWISMS:
      log_rsp -> data_format = (uint8)WMSDIAG_DATA_FORMAT_ANALOG_AWISMS;
      break;

    case WMS_FORMAT_MWI:
      log_rsp -> data_format = (uint8)WMSDIAG_DATA_FORMAT_MWI;
      break;

    case WMS_FORMAT_GW_PP:
      log_rsp -> data_format = (uint8)WMSDIAG_DATA_FORMAT_GW_PP;
      break;

    case WMS_FORMAT_GW_CB:
      log_rsp -> data_format = (uint8)WMSDIAG_DATA_FORMAT_GW_CB;
      break;

    default:
      log_rsp -> data_format = (uint8)WMSDIAG_DATA_FORMAT_INVALID;
      break;
    }

    /*  Set the data length */
    log_rsp -> user_data_length = D;

    /*  Set the raw data */
    for (i = 0; i < D; i ++)
    {
      #ifdef FEATURE_CDSMS
      if(is_CDSMS)
      {
        log_rsp->data[i + A + S] = wmsdiag_message.u.cdma_message.raw_ts.data [i];
      }
      #endif
      #ifdef FEATURE_GWSMS
      if(!is_CDSMS)
      {
        log_rsp -> data [i + A + S] = wmsdiag_message.u.gw_message.raw_ts_data.data [i];
      }
      #endif
    }

    /*  Calculate the size of our log packet */
    log_size = FPOS (wmsdiag_read_log_type, data) + A + S + D;

    /*  Adjust the size accordingly */
    log_shorten (log_rsp, log_size);

    /*  Commit our packet */
    log_commit  (log_rsp);
  }

  return;

}


/*===========================================================================

FUNCTION WMSDIAG_SET_ROUTES

DESCRIPTION
  Sets the routes for all WMS message classes.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  Uses a diag_block until the DIAG_ASYNC_BLOCK_SIG is set.  This will be done
  by either the CFG callback event being generated or the CMD callback producing
  an error.

===========================================================================*/


PACKED void * wmsdiag_set_routes (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_set_routes_rsp_type *rsp_ptr  = NULL;

  /*  Copy of request pointer cast to proper type */
  wmsdiag_set_routes_req_type *req_ptr  =
      (wmsdiag_set_routes_req_type *) req_pkt_ptr;

  /*  The routing info we will pass to the WMS API */
  wms_routes_s_type wmsdiag_routes;

  wms_status_e_type st = WMS_OK_S;

  wmsdiag_transaction_ID = req_ptr->transaction_ID;

  /*  Verify that the PP parameters passed were within the specified ranges */
  if ( req_ptr -> wmsdiag_msg_class_cdma_storage_pp >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class_cdma_route_pp   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class_none_storage_pp >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class_none_route_pp   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class0_storage_pp >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class0_route_pp   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class1_storage_pp >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class1_route_pp   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class2_storage_pp >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class2_route_pp   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class3_storage_pp >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class3_route_pp   >= (uint32)WMSDIAG_ROUTE_MAX)

  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

#ifdef FEATURE_CDSMS_BROADCAST
  /*  Verify that the BC parameters passed were within the specified ranges */
  if ( req_ptr -> wmsdiag_msg_class_cdma_storage_bc >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class_cdma_route_bc   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class_none_storage_bc >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class_none_route_bc   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class0_storage_bc >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class0_route_bc   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class1_storage_bc >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class1_route_bc   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class2_storage_bc >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class2_route_bc   >= (uint32)WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class3_storage_bc >= (uint32)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class3_route_bc   >= (uint32)WMSDIAG_ROUTE_MAX)

  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }
#endif /* FEATURE_CDSMS_BROADCAST */


  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_set_routes_rsp_type*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                                WMSDIAG_SET_ROUTES_CMD,
                                                                sizeof (wmsdiag_set_routes_rsp_type));
  if (rsp_ptr != NULL)
  {
    memset(&wmsdiag_routes, 0, sizeof(wmsdiag_routes));
    /*  Set all the PP routes */
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_0].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class0_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_0].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class0_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_1].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class1_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_1].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class1_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_2].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class2_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_2].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class2_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_3].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class3_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_3].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class3_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_NONE].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class_none_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_NONE].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class_none_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_CDMA].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class_cdma_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_CDMA].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class_cdma_storage_pp ];

#ifdef FEATURE_CDSMS_BROADCAST
    /*  Set all the BC routes */
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_0].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class0_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_0].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class0_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_1].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class1_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_1].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class1_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_2].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class2_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_2].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class2_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_3].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class3_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_3].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class3_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_NONE].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class_none_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_NONE].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class_none_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_CDMA].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class_cdma_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_CDMA].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class_cdma_storage_bc ];

#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_GWSMS
      wmsdiag_routes.transfer_status_report = req_ptr -> transfer_status_report;
#endif

    /*  Call the WMS API function to set the routes */
    st = wms_cfg_ms_set_routes (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                        NULL, (void *)(uint32)req_ptr -> transaction_ID,
                        &wmsdiag_routes);

    if(st != WMS_OK_S)
    {
      rsp_ptr -> status = 1;
      wmsdiag_set_routes_failed = TRUE;
      MSG_ERROR_0("DTC command failed");
    }
    else
    {
      rsp_ptr -> status = 0;
      wmsdiag_set_routes_failed = FALSE;
    }

    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMSDIAG_ORIG_END_SMS_CALL

DESCRIPTION
  This function attempts to originate or end a SMS call.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_orig_end_sms_call (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)
{
  /*  Response Pointer */
  wmsdiag_orig_end_sms_call_rsp_type *rsp_ptr  = NULL;

  /*  Cast copy of request pointer to proper type */
  wmsdiag_orig_end_sms_call_req_type *req_ptr  =
      (wmsdiag_orig_end_sms_call_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  if( req_ptr->is_orig == TRUE      &&
      req_ptr->so != (uint32)WMS_DC_SO_AUTO &&
      req_ptr->so != (uint32)WMS_DC_SO_6    &&
      req_ptr->so != (uint32)WMS_DC_SO_14 )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_orig_end_sms_call_rsp_type*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                                       WMSDIAG_ORIG_END_SMS_CALL_CMD,
                                                                       sizeof (wmsdiag_orig_end_sms_call_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);

    /* originate or end ? */
    if(req_ptr->is_orig)
    {
      st = wms_dc_connect(wmsdiag_client_id, NULL, NULL,
                          (wms_dc_so_e_type)req_ptr->so);
    }
    else
    {
      st = wms_dc_disconnect(wmsdiag_client_id, NULL, NULL);
    }

    if(st != WMS_OK_S)
    {
      MSG_ERROR_0("DTC command failed");
    }
  }

  return NULL;
}

/*===========================================================================

FUNCTION WMSDIAG_ENABLE_DISABLE_AUTO_DISCONNECT

DESCRIPTION
  This function attempts to enable or disable auto disconnect of a SMS call.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_enable_disable_auto_disconnect (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)
{
  /*  Response Pointer */
  wmsdiag_enable_disable_auto_disconnect_rsp_type *rsp_ptr  = NULL;

  /*  Cast copy of request pointer to proper type */
  wmsdiag_enable_disable_auto_disconnect_req_type *req_ptr  =
      (wmsdiag_enable_disable_auto_disconnect_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  if( req_ptr->enable_auto_disconnect == TRUE  &&
      req_ptr->timeout == 0 )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_enable_disable_auto_disconnect_rsp_type*)
            diagpkt_subsys_alloc ((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                  WMSDIAG_AUTO_DISCONNECT_CMD,
                                  sizeof (wmsdiag_enable_disable_auto_disconnect_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);

    /* enable or disable ? */
    if(req_ptr->enable_auto_disconnect == TRUE)
    {
      st = wms_dc_enable_auto_disconnect(wmsdiag_client_id, NULL, NULL,
                                         req_ptr->timeout);
    }
    else
    {
      st = wms_dc_disable_auto_disconnect(wmsdiag_client_id, NULL, NULL);
    }

    if(st != WMS_OK_S)
    {
      MSG_ERROR_0("DTC command failed");
    }
  }

  return NULL;
}

/*===========================================================================

FUNCTION wmsdiag_set_as_id

DESCRIPTION
  This function  attempts to set the current active sub id.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * wmsdiag_set_as_id (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_set_as_id_rsp_type *rsp_ptr = NULL;

  /*  Cast the request pointer to proper type */
  wmsdiag_set_as_id_req_type *req_ptr  =
      (wmsdiag_set_as_id_req_type *) req_pkt_ptr;


  /*  Verify that the parameters passed were within the specified ranges */
  if (req_ptr->as_id >= (uint32)WMSDIAG_AS_ID_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }


  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }


  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_set_as_id_rsp_type*) diagpkt_subsys_alloc ((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                                WMSDIAG_SET_AS_ID_CMD,
                                                                sizeof(wmsdiag_set_as_id_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit(rsp_ptr);
    current_as_id = req_ptr->as_id;
  }

  return NULL;
}

/*===========================================================================

FUNCTION wmsdiag_convert_as_id

DESCRIPTION
  This function converts diag as id to sys_modem_as_id_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  as id in sys_modem_as_id_e_type.

SIDE EFFECTS
  None.

===========================================================================*/
sys_modem_as_id_e_type wmsdiag_convert_as_id(uint32 diag_as_id)
{
   sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;

   MSG_ERROR_1("wmsdiag_convert_as_id %d ", diag_as_id);

   if ((uint32)WMSDIAG_AS_ID_2 == diag_as_id)
   {
      as_id = SYS_MODEM_AS_ID_2;
   }
   else if ((uint32)WMSDIAG_AS_ID_3 == diag_as_id)
   {
      as_id = SYS_MODEM_AS_ID_3;
   }

   return as_id;
}

/*===========================================================================
FUNCTION wmsdiag_broadcast_add_delete_serviceid

DESCRIPTION
  Function to process the WMSDIAG_BROADCAST_ADD_DELET_SERVICEID_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsdiag_broadcast_add_delete_serviceid (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
  wmsdiag_broadcast_add_delete_serviceid_rsp_type *delay_rsp_pkt  = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /* Cast copy of request pointer to proper type */
  wmsdiag_broadcast_add_delete_serviceid_req_type *req_ptr  =
        (wmsdiag_broadcast_add_delete_serviceid_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  /* set to default value */
  wms_message_mode_e_type msg_mode = WMS_MESSAGE_MODE_MAX;

  static wms_bc_mm_service_info_s_type   service_info;

  static wms_bc_mm_srv_id_type           service_id;

  int i;

  (void)pkt_len; // parameter not used

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_broadcast_add_delete_serviceid_rsp_type*) diagpkt_subsys_alloc_v2_delay (
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD,
                             delay_rsp_id,
                             sizeof(wmsdiag_broadcast_add_delete_serviceid_rsp_type) );

  if (delay_rsp_pkt != NULL)
  {
    /* Store the service id information depending on CDMA or GW mode */
    if (req_ptr->msg_mode == (uint8)WMSDIAG_MSG_MODE_CDMA)
    {
      service_info.srv_id.bc_srv_id.service = (wms_service_e_type) req_ptr->service_category;
      service_info.srv_id.bc_srv_id.language = (wms_language_e_type) req_ptr->language;

      service_id.bc_srv_id.service =  (wms_service_e_type) req_ptr->service_category;
      service_id.bc_srv_id.language = (wms_language_e_type) req_ptr->language;

      msg_mode = WMS_MESSAGE_MODE_CDMA;
    }
    else if (req_ptr->msg_mode == (uint8)WMSDIAG_MSG_MODE_GW)
    {
      service_info.srv_id.gw_cb_range.from = req_ptr->serviceid_from;
      service_info.srv_id.gw_cb_range.to = req_ptr->serviceid_to;

      service_id.gw_cb_range.from = req_ptr->serviceid_from;
      service_id.gw_cb_range.to = req_ptr->serviceid_to;

      msg_mode = WMS_MESSAGE_MODE_GW;
    }

    /* add a new service id */
    if (req_ptr->add_or_delete_serviceid == 0)
    {
      service_info.alert = (wms_bc_alert_e_type) req_ptr->alert_option;
      service_info.priority = (wms_priority_e_type) req_ptr->priority;
      service_info.selected = req_ptr->is_selected;
      service_info.max_messages = req_ptr->max_messages;
      service_info.label_encoding = (wms_user_data_encoding_e_type) req_ptr->label_encoding;

      /* label */
      for (i = 0; i < WMS_BC_MM_SRV_LABEL_SIZE; i++)
      {
        service_info.label[i] = req_ptr->label[i];
      }

      MSG_ERROR_1("IN DIAG wms_bc_ms_add_services %d ", current_as_id);
      st = wms_bc_ms_add_services(wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                                  wmsdiag_bc_add_delete_cmd_cb, (void *)(uint32)req_ptr->transaction_ID,
                                    msg_mode, 1, &service_info);
    }
    /* delete an old service id */
    else if (req_ptr->add_or_delete_serviceid == 1)
    {
      MSG_ERROR_1("IN DIAG wms_bc_ms_delete_services %d ", current_as_id);
      st = wms_bc_ms_delete_services(wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                                     wmsdiag_bc_add_delete_cmd_cb, (void *)(uint32)req_ptr->transaction_ID,
                                      msg_mode, 1, &service_id);
    }

    /* Immediate Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 0);

    /* Set the Status */
    if (st != WMS_OK_S)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
      MSG_ERROR_0("Add/Delete Broadcast Service ID failed");
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = req_ptr->transaction_ID;
    delay_rsp_pkt->reserved_byte1 = 0;
    delay_rsp_pkt->reserved_byte2 = 0;
    delay_rsp_pkt->reserved_byte3 = 0;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
  else
  {
    MSG_ERROR_0("Response Packet Allocation Failed!");
  }

  return NULL;
}

/*===========================================================================
FUNCTION wmsdiag_broadcast_delete_all_services

DESCRIPTION
  Function to process the WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsdiag_broadcast_delete_all_services (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
  wmsdiag_broadcast_delete_all_services_rsp_type *delay_rsp_pkt  = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /* Cast copy of request pointer to proper type */
  wmsdiag_broadcast_delete_all_services_req_type *req_ptr  =
        (wmsdiag_broadcast_delete_all_services_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  /* set to default value */
  wms_message_mode_e_type msg_mode = WMS_MESSAGE_MODE_MAX;

  (void)pkt_len; // parameter not used

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_broadcast_delete_all_services_rsp_type*) diagpkt_subsys_alloc_v2_delay (
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD,
                             delay_rsp_id,
                             sizeof(wmsdiag_broadcast_delete_all_services_rsp_type) );

  if (delay_rsp_pkt != NULL)
  {
    /* Store the service id information depending on CDMA or GW mode */
    if (req_ptr->msg_mode == (uint8)WMSDIAG_MSG_MODE_CDMA)
    {
      msg_mode = WMS_MESSAGE_MODE_CDMA;
    }
    else if (req_ptr->msg_mode == (uint8)WMSDIAG_MSG_MODE_GW)
    {
      msg_mode = WMS_MESSAGE_MODE_GW;
    }

    MSG_ERROR_1("IN DIAG wms_bc_ms_delete_all_services %d ", current_as_id);
    st = wms_bc_ms_delete_all_services( wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id), wmsdiag_bc_delete_all_cmd_cb,
                                        (void *)(uint32)req_ptr->transaction_ID, msg_mode );

    /* Immediate Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 0);

    /* Set the Status */
    if (st != WMS_OK_S)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
      MSG_ERROR_0("Add/Delete Broadcast Service ID failed");
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = req_ptr->transaction_ID;
    delay_rsp_pkt->reserved_byte1 = 0;
    delay_rsp_pkt->reserved_byte2 = 0;
    delay_rsp_pkt->reserved_byte3 = 0;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
  else
  {
    MSG_ERROR_0("Response Packet Allocation Failed!");
  }

  return NULL;
}

/*===========================================================================
FUNCTION wmsdiag_get_bearer_pref

DESCRIPTION
  Function to process the WMSDIAG_GET_BEARER_PREF_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsdiag_get_bearer_pref (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
  wmsdiag_get_bearer_pref_rsp_type *delay_rsp_pkt  = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /* Cast copy of request pointer to proper type */
  wmsdiag_get_bearer_pref_req_type *req_ptr  =
        (wmsdiag_get_bearer_pref_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  /* Drop re-entrant function calls if a request is still pending */
  if (get_bearer_req_state != WMSDIAG_REQ_STATE_IDLE)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_get_bearer_pref_rsp_type*) diagpkt_subsys_alloc_v2_delay (
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_GET_BEARER_PREF_CMD,
                             delay_rsp_id,
                             sizeof(wmsdiag_get_bearer_pref_rsp_type) );

  if (delay_rsp_pkt != NULL)
  {
    get_bearer_req_state = WMSDIAG_REQ_STATE_COMMAND_CB;

    st = wms_cfg_get_gw_domain_pref( wmsdiag_client_id, wmsdiag_get_bearer_pref_cmd_cb,
                                     (void *)req_ptr->transaction_ID );

    /* Immediate Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 0);

    /* Set the Status */
    if (st != WMS_OK_S)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
      MSG_ERROR_0("Get Bearer Pref failed");
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = req_ptr->transaction_ID;
    delay_rsp_pkt->sms_domain_pref = (uint32)WMS_GW_DOMAIN_PREF_MAX32; /* Invalid */

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
  else
  {
    MSG_ERROR_0("Response Packet Allocation Failed!");
  }

  return NULL;
}

/*===========================================================================
FUNCTION wmsdiag_set_bearer_pref

DESCRIPTION
  Function to process the WMSDIAG_SET_BEARER_PREF_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsdiag_set_bearer_pref (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
  wmsdiag_set_bearer_pref_rsp_type *delay_rsp_pkt  = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /* Cast copy of request pointer to proper type */
  wmsdiag_set_bearer_pref_req_type *req_ptr  =
        (wmsdiag_set_bearer_pref_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  if ( req_ptr->sms_domain_pref >= (uint32)WMS_GW_DOMAIN_PREF_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_set_bearer_pref_rsp_type*) diagpkt_subsys_alloc_v2_delay (
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_SET_BEARER_PREF_CMD,
                             delay_rsp_id,
                             sizeof(wmsdiag_set_bearer_pref_rsp_type) );

  if (delay_rsp_pkt != NULL)
  {
    st = wms_cfg_set_gw_domain_pref( wmsdiag_client_id, wmsdiag_set_bearer_pref_cmd_cb,
                                     (void *)req_ptr->transaction_ID,
                                     (wms_gw_domain_pref_e_type)req_ptr->sms_domain_pref );

    /* Immediate Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 0);

    /* Set the Status */
    if (st != WMS_OK_S)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
      MSG_ERROR_0("Set Bearer Pref failed");
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = req_ptr->transaction_ID;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
  else
  {
    MSG_ERROR_0("Response Packet Allocation Failed!");
  }

  return NULL;
}

#ifdef FEATURE_FUZZ_TEST
/*===========================================================================

FUNCTION WMSDIAG_SEND_MT_MSG

DESCRIPTION
  This function  attempts to send a message with the given properties or write
  a message, depending on the parameters passed in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * wmsdiag_send_mt_msg (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_send_mt_msg_rsp_type *rsp_ptr  = NULL;

  /*  Cast the request pointer to proper type */
  wmsdiag_send_mt_msg_req_type *req_ptr  =
      (wmsdiag_send_mt_msg_req_type *) req_pkt_ptr;


  static wms_msg_transport_rpt_ind_s_type   rpt_ind;

  wms_format_e_type format = WMS_FORMAT_CDMA;

  /*  Verify that the parameters passed were within the specified ranges */
  if (req_ptr -> format >= WMSDIAG_DATA_FORMAT_MAX || 0 == req_ptr->data_len)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  format = WMSDIAG_DATA_FORMAT_VALS [req_ptr -> format];

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_send_mt_msg_rsp_type*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                                 WMSDIAG_SEND_MT_MSG_CMD,
                                                                 sizeof (wmsdiag_send_mt_msg_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);

    rpt_ind.ind_type = WMS_MSG_TRANSPORT_MT_SMS_IND;
    rpt_ind.transport_id = (wms_msg_transport_id_type)WMS_MSG_TRANSPORT_TYPE_INTERNAL;
    rpt_ind.mt_sms.format = format;
    rpt_ind.mt_sms.data_len = MIN(req_ptr->data_len, WMSDIAG_MAX_LEN);
    (void)memscpy( rpt_ind.mt_sms.data,
                   rpt_ind.mt_sms.data_len,
                   req_ptr->data,
                   rpt_ind.mt_sms.data_len);
    (void) wms_dbg_send_transport_rpt_ind(wmsdiag_client_id, NULL, NULL, &rpt_ind);

  }

  return NULL;
}

#endif /* FEATURE_FUZZ_TEST */

/*===========================================================================

FUNCTION WMSDIAG_RESEND

DESCRIPTION
  This function  attempts to resend a message with the given properties,
  depending on the parameters passed in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * wmsdiag_resend (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_resend_rsp_type   *rsp_ptr            = NULL;
  wms_client_message_s_type *client_message_ptr = NULL;

  /*  Cast the request pointer to proper type */
  wmsdiag_resend_req_type *req_ptr  =
      (wmsdiag_resend_req_type *) req_pkt_ptr;

  /*  Flag to check message mode */
  boolean is_CDSMS = TRUE;

#ifdef FEATURE_CDSMS
  static wms_cdma_message_s_type      cdma_message;
  static wms_address_s_type           cdma_message_addr;
  static wms_subaddress_s_type        cdma_message_subaddr;
  static wms_raw_ts_data_s_type       cdma_raw_data;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
  static wms_gw_message_s_type        gw_message;
  static wms_address_s_type           gw_message_addr;
  static wms_raw_ts_data_s_type       gw_raw_data;
#endif /* FEATURE_GWSMS */

  /*  Loop Counter */
  uint32 i;

  uint8 A = 0;  /*  Number of digits in the address */
  uint8 S = 0;  /*  Number of digits in the subaddress */
  uint32 D = 0;  /*  Length of the data in bytes */

#ifdef FEATURE_CDSMS
  memset(&cdma_message, 0, sizeof(cdma_message));
  memset(&cdma_message_addr, 0, sizeof(cdma_message_addr));
  memset(&cdma_message_subaddr, 0, sizeof(cdma_message_subaddr));
  memset(&cdma_raw_data, 0, sizeof(cdma_raw_data));
#endif /* FEATURE_CDSMS */

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> send_mode          >= (uint8)WMSDIAG_SEND_MODE_MAX ||
       req_ptr -> msg_mode           >= ((uint8)WMSDIAG_MSG_MODE_MAX) - 1 ||
       req_ptr -> tag_type           >= (uint8)WMSDIAG_TAG_MAX ||
       req_ptr -> storage_type       >= (uint8)WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> addr_digit_mode    >= (uint8)WMSDIAG_ADDR_DIGIT_MODE_MAX ||
       req_ptr -> addr_num_mode      >= (uint8)WMSDIAG_ADDR_NUM_MODE_MAX ||
       req_ptr -> data_format        >= (uint8)WMSDIAG_DATA_FORMAT_MAX ||
       req_ptr -> addr_num_digits    >  WMSDIAG_ADDRESS_MAX ||
       req_ptr -> subaddr_num_digits >  WMSDIAG_SUBADDRESS_MAX ||
       req_ptr -> user_data_length   >  WMSDIAG_MAX_LEN )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  Initialize the message mode flag */
  if (WMSDIAG_MSG_MODE_VALS [req_ptr -> msg_mode] == WMS_MESSAGE_MODE_CDMA)
  {
    is_CDSMS = TRUE;
  }
  else
  {
    is_CDSMS = FALSE;
  }

  #ifdef FEATURE_GWSMS
  if (req_ptr -> tpdu_type >= (uint8)WMSDIAG_TPDU_MAX && !is_CDSMS)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }
  #endif

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Initialize counters for the number of address/subaddress digits as well as the data length */
  A = (uint8)req_ptr -> addr_num_digits;
  S = (uint8)req_ptr -> subaddr_num_digits;
  D = req_ptr -> user_data_length;

  if( A+S+D > WMSDIAG_DEFAULT_DATA_BUF_MAX )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  client_message_ptr = wms_mem_malloc ( sizeof(wms_client_message_s_type) );
  if (NULL != client_message_ptr)
  {
    /*  Allocate our packet */
    rsp_ptr = (wmsdiag_resend_rsp_type*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                                              WMSDIAG_SEND_WRITE_CMD,
                                                            sizeof (wmsdiag_resend_rsp_type));

    if (rsp_ptr)
    {
      wms_message_number_type client_msg_id = WMS_DUMMY_MESSAGE_NUMBER;

      /*  Set the message header variables */
      msg_hdr.message_mode = WMSDIAG_MSG_MODE_VALS [req_ptr -> msg_mode];
      msg_hdr.tag = WMSDIAG_MSG_TAG_VALS [req_ptr -> tag_type];
      msg_hdr.mem_store = WMSDIAG_MEM_STORE_VALS [req_ptr -> storage_type];
      msg_hdr.index = req_ptr -> index;

      /*  Set the CDMA message variables */
    #ifdef FEATURE_CDSMS
      if (is_CDSMS)
      {

        /* Set the resend message id */
        if (wms_msg_cdma_message_id_is_valid(req_ptr->message_id))
        {
          client_msg_id = req_ptr->message_id;
        }

        cdma_message.is_mo = req_ptr -> is_MO;
        cdma_message.teleservice = (wms_teleservice_e_type) req_ptr -> teleservice_type;

        /*  Set the address information */
        cdma_message_addr.digit_mode = WMSDIAG_ADDR_DIGIT_MODE_VALS [req_ptr -> addr_digit_mode];
        cdma_message_addr.number_mode = WMSDIAG_ADDR_NUM_MODE_VALS [req_ptr -> addr_num_mode];
        cdma_message_addr.number_type = (wms_number_type_e_type) req_ptr -> addr_num_type;
        cdma_message_addr.number_plan = (wms_number_plan_e_type) req_ptr -> addr_num_plan_type;
        cdma_message_addr.number_of_digits = A;
        for (i = 0; i < A ; i++)
        {
          cdma_message_addr.digits [i] = req_ptr -> data [i];
        }

        cdma_message.address = cdma_message_addr;

        /*  Set the subaddress information */
        cdma_message_subaddr.type = (wms_subaddress_type_e_type) req_ptr -> subaddr_type;
        cdma_message_subaddr.odd = req_ptr -> subaddr_odd_or_even;
        cdma_message_subaddr.number_of_digits = S;

        for (i = 0; i < S; i++)
        {
          cdma_message_subaddr.digits[i] = req_ptr->data[i + A];
        }

        cdma_message.subaddress = cdma_message_subaddr;

        cdma_message.is_tl_ack_requested = req_ptr -> tl_ack_needed;
        cdma_message.is_service_present = req_ptr -> service_present;
        cdma_message.service = (wms_service_e_type) req_ptr -> service_type;

        cdma_raw_data.format = WMSDIAG_DATA_FORMAT_VALS [req_ptr -> data_format];
        cdma_raw_data.len = D;

        for (i = 0; i < D; i++)
        {
          cdma_raw_data.data [i] = req_ptr -> data [i + A + S];
        }

        cdma_message.raw_ts = cdma_raw_data;

        client_message_ptr->msg_hdr = msg_hdr;
        client_message_ptr->u.cdma_message = cdma_message;

        /*  Send out our packet now */
        diagpkt_commit (rsp_ptr);

        /*  Send it over the air */
        (void)wms_msg_ms_resend (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                  NULL, (void *)(uint32)req_ptr -> transaction_ID,
                  WMSDIAG_SEND_MODE_VALS [req_ptr -> send_mode],
                  client_message_ptr,
                  client_msg_id );
      } /* if (is_CDSMS) */

    #endif /* FEATURE_CDSMS */

    #ifdef FEATURE_GWSMS
      if (!is_CDSMS)
      {
        /* Set the resend message id */
        if (wms_msg_gw_message_id_is_valid(req_ptr->message_id))
        {
          client_msg_id = req_ptr->message_id;
        }

        gw_message.is_broadcast = req_ptr -> is_broadcast;

        /*  Set the address information */
        gw_message_addr.digit_mode  = WMSDIAG_ADDR_DIGIT_MODE_VALS [req_ptr -> addr_digit_mode];
        gw_message_addr.number_mode = WMSDIAG_ADDR_NUM_MODE_VALS [req_ptr -> addr_num_mode];
        gw_message_addr.number_type = (wms_number_type_e_type) req_ptr -> addr_num_type;
        gw_message_addr.number_plan = (wms_number_plan_e_type) req_ptr -> addr_num_plan_type;
        gw_message_addr.number_of_digits = A;
        for (i = 0; i < A ; i++)
        {
          gw_message_addr.digits [i] = req_ptr -> data [i];
        }

        gw_message.sc_address = gw_message_addr;

        gw_raw_data.format = WMSDIAG_DATA_FORMAT_VALS [req_ptr -> data_format];
        gw_raw_data.len = D;
        gw_raw_data.tpdu_type = WMSDIAG_GW_TPDU_VALS [req_ptr -> tpdu_type];

        for (i = 0; i < D; i++)
        {
          /* for GW, skip the subaddress in data */
          gw_raw_data.data [i] = req_ptr -> data [i + A + S];
        }

        gw_message.raw_ts_data = gw_raw_data;

        client_message_ptr->msg_hdr = msg_hdr;
        client_message_ptr->u.gw_message = gw_message;

        /*  Send out our packet now */
        diagpkt_commit (rsp_ptr);

        /*  Send it over the air */
        (void)wms_msg_ms_resend (wmsdiag_client_id, wmsdiag_convert_as_id(current_as_id),
                  NULL, (void *)(uint32)req_ptr -> transaction_ID,
                  WMSDIAG_SEND_MODE_VALS [req_ptr -> send_mode],
                  client_message_ptr,
                  client_msg_id );
      } /*  if (!is_CDSMS) */

    #endif /* FEATURE_GWSMS */
    }

    wms_mem_free( client_message_ptr );
  }

  return NULL;
}

/*  Map the diag functions to the proper commands */
static const diagpkt_user_table_entry_type wmsdiag_tbl[] =
{
  {WMSDIAG_SEND_WRITE_CMD, WMSDIAG_SEND_WRITE_CMD, wmsdiag_send_write},
  {WMSDIAG_CLEAR_CMD, WMSDIAG_CLEAR_CMD, wmsdiag_clear},
  {WMSDIAG_CLEAR_ALL_CMD, WMSDIAG_CLEAR_ALL_CMD, wmsdiag_clear_all},
  {WMSDIAG_GET_MESSAGE_LIST_CMD, WMSDIAG_GET_MESSAGE_LIST_CMD, wmsdiag_get_message_list},
  {WMSDIAG_READ_CMD, WMSDIAG_READ_CMD, wmsdiag_read},
  {WMSDIAG_SET_MESSAGE_TAG_CMD, WMSDIAG_SET_MESSAGE_TAG_CMD, wmsdiag_set_message_tag},
  {WMSDIAG_SET_ROUTES_CMD, WMSDIAG_SET_ROUTES_CMD, wmsdiag_set_routes},
  {WMSDIAG_ORIG_END_SMS_CALL_CMD, WMSDIAG_ORIG_END_SMS_CALL_CMD, wmsdiag_orig_end_sms_call},
  {WMSDIAG_AUTO_DISCONNECT_CMD, WMSDIAG_AUTO_DISCONNECT_CMD, wmsdiag_enable_disable_auto_disconnect},
  {WMSDIAG_SET_AS_ID_CMD, WMSDIAG_SET_AS_ID_CMD, wmsdiag_set_as_id},
#ifdef FEATURE_FUZZ_TEST
  {WMSDIAG_SEND_MT_MSG_CMD, WMSDIAG_SEND_MT_MSG_CMD, wmsdiag_send_mt_msg},
#endif
  {WMSDIAG_RESEND_CMD, WMSDIAG_RESEND_CMD, wmsdiag_resend},

};

static const diagpkt_user_table_entry_type wmsdiag_tbl_delay_rsp[] =
{
  {WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD, WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD, wmsdiag_broadcast_add_delete_serviceid},
  {WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD, WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD, wmsdiag_broadcast_delete_all_services},
  {WMSDIAG_GET_BEARER_PREF_CMD, WMSDIAG_GET_BEARER_PREF_CMD, wmsdiag_get_bearer_pref},
  {WMSDIAG_SET_BEARER_PREF_CMD, WMSDIAG_SET_BEARER_PREF_CMD, wmsdiag_set_bearer_pref},
};

/*===========================================================================
FUNCTION wmsdiag_init

DESCRIPTION
  Initialize the WMS Diag Module

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wmsdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER ((diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, wmsdiag_tbl);
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F,
                                           (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS,
                                           wmsdiag_tbl_delay_rsp);
  wmsdiag_client_init();
}

#endif /* if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS) */
